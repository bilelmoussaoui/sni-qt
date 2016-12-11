/* This file is part of the sni-qt project
   Copyright 2011 Canonical
   Author: Aurelien Gateau <aurelien.gateau@canonical.com>

   sni-qt is free software; you can redistribute it and/or modify it under the
   terms of the GNU Lesser General Public License (LGPL) as published by the
   Free Software Foundation; version 3 of the License.

   sni-qt is distributed in the hope that it will be useful, but WITHOUT ANY
   WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
   FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
   more details.

   You should have received a copy of the GNU Lesser General Public License
   along with sni-qt.  If not, see <http://www.gnu.org/licenses/>.
*/
// Qt
#include <QtTestGui>

// Local
#include <dbusimage.h>

// Qt
#include <QImage>
#include <QPainter>
#include <QPixmap>

// libc
#include <arpa/inet.h>

static QImage qImageFromDBusImage(const DBusImage& dbusImage)
{
    QImage img(dbusImage.width, dbusImage.height, QImage::Format_ARGB32);

    const quint32* srcPtr = reinterpret_cast<const quint32*>(dbusImage.pixels.constData());
    for (int y = 0; y < dbusImage.height; ++y) {
        quint32* dstPtr = reinterpret_cast<quint32*>(img.scanLine(y));
        quint32* dstEnd = dstPtr + dbusImage.width;
        for (; dstPtr != dstEnd; ++dstPtr, ++srcPtr) {
            *dstPtr = ntohl(*srcPtr);
        }
    }
    return img;
}

static QImage createTestImage(int width, int height)
{
    QImage img(width, height, QImage::Format_ARGB32);
    QPainter painter(&img);
    painter.setPen(Qt::blue);
    painter.setBrush(Qt::green);
    painter.drawRect(img.rect().adjusted(0, 0, -1, -1));
    return img;
}

class DBusImageTest : public QObject
{
Q_OBJECT
private Q_SLOTS:
    void testFromPixmap()
    {
        QImage src = createTestImage(16, 16);
        QPixmap pixmap = QPixmap::fromImage(src);
        DBusImage dbusImage = DBusImage::createFromPixmap(pixmap);
        QCOMPARE(dbusImage.width, src.width());
        QCOMPARE(dbusImage.height, src.height());

        QImage result = qImageFromDBusImage(dbusImage);
        QCOMPARE(result, src);
    }

    void testListFromIcon()
    {
        QIcon icon;
        QImage src16 = createTestImage(16, 16);
        QImage src22 = createTestImage(22, 22);
        icon.addPixmap(QPixmap::fromImage(src16));
        icon.addPixmap(QPixmap::fromImage(src22));

        DBusImageList list = DBusImage::createListFromIcon(icon);
        QCOMPARE(list.count(), 2);

        QImage result16 = qImageFromDBusImage(list[0]);
        QImage result22 = qImageFromDBusImage(list[1]);
        if (list[0].width == 22) {
            qSwap(result16, result22);
        }
        QCOMPARE(src16, result16);
        QCOMPARE(src22, result22);
    }

    void testListFromSvgIcon()
    {
        QIcon icon(":/images/heart.svg");
        DBusImageList list = DBusImage::createListFromIcon(icon);
        QCOMPARE(list.count(), 5);

        QCOMPARE(list[0].width, 16);
        QCOMPARE(list[1].width, 22);
        QCOMPARE(list[2].width, 24);
        QCOMPARE(list[3].width, 32);
        QCOMPARE(list[4].width, 48);
    }
};

QTEST_MAIN(DBusImageTest)

#include <dbusimagetest.moc>
