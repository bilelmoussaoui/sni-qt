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
#include <dbusimage.h>

// Qt
#include <QDBusArgument>
#include <QDebug>
#include <QIcon>
#include <QImage>
#include <QPixmap>

// libc
#include <arpa/inet.h>

QDBusArgument& operator<<(QDBusArgument& argument, const DBusImage& image)
{
    argument.beginStructure();
    argument << image.width << image.height << image.pixels;
    argument.endStructure();
    return argument;
}

const QDBusArgument& operator>>(const QDBusArgument& argument, DBusImage& image)
{
    argument.beginStructure();
    argument >> image.width >> image.height >> image.pixels;
    argument.endStructure();
    return argument;
}

DBusImageList DBusImage::createListFromIcon(const QIcon& icon)
{
    if (icon.isNull()) {
        qWarning("qt-sni: DBusImage::createListFromIcon() icon is null");
        return DBusImageList();
    }

    DBusImageList list;
    QList<QSize> sizes = icon.availableSizes();
    if (sizes.isEmpty()) {
        // sizes can be empty if icon is an SVG. In this case generate images for a few sizes
        #define SIZE(x) QSize(x, x)
        sizes
            << SIZE(16)
            << SIZE(22)
            << SIZE(24)
            << SIZE(32)
            << SIZE(48)
            ;
        #undef SIZE
    }

    Q_FOREACH(const QSize& size, sizes) {
        list << createFromPixmap(icon.pixmap(size));
    }
    return list;
}

DBusImage DBusImage::createFromPixmap(const QPixmap& pixmap)
{
    QImage image = pixmap.toImage().convertToFormat(QImage::Format_ARGB32);
    DBusImage dbusImage;
    dbusImage.width = pixmap.width();
    dbusImage.height = pixmap.height();

    dbusImage.pixels.resize(dbusImage.width * dbusImage.height * 4);
    quint32* dstPtr = reinterpret_cast<quint32*>(dbusImage.pixels.data());
    for (int y = 0; y < dbusImage.height; ++y) {
        const quint32* srcPtr = reinterpret_cast<const quint32*>(image.constScanLine(y));
        const quint32* srcEnd = srcPtr + dbusImage.width;
        for (; srcPtr != srcEnd; ++srcPtr, ++dstPtr) {
            *dstPtr = htonl(*srcPtr);
        }
    }

    return dbusImage;
}
