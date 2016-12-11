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
// Local
#include <fsutils.h>
#include <iconcache.h>

// Qt
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QIcon>
#include <QImage>
#include <QPainter>
#include <QtTestGui>

static QImage createTestImage(int size, const QColor& color)
{
    QImage img(size, size, QImage::Format_ARGB32);
    QPainter painter(&img);
    painter.setPen(color.darker());
    painter.setBrush(color);
    painter.drawRect(img.rect().adjusted(0, 0, -1, -1));
    return img;
}

static QIcon createTestIcon(const QList<int>& sizes, const QColor& color)
{
    QIcon icon;
    Q_FOREACH(int size, sizes) {
        QImage image = createTestImage(size, color);
        icon.addPixmap(QPixmap::fromImage(image));
    }
    return icon;
}

class IconCacheTest : public QObject
{
Q_OBJECT
private Q_SLOTS:
    void init()
    {
        QDir dir = QDir::current();
        m_sandBoxDirName = dir.path() + "/sandbox";
        dir.mkdir("sandbox");
    }

    void cleanup()
    {
        FsUtils::recursiveRm(m_sandBoxDirName);
    }

    void testThemePath()
    {
        IconCache cache(m_sandBoxDirName);
        QString themePath = cache.themePath();
        QCOMPARE(themePath, m_sandBoxDirName + "/icons");

        QFileInfo info(themePath);
        QVERIFY(info.isDir());
    }

    void testThemePathForIcon()
    {
        QList<int> sizes = QList<int>() << 16 << 22 << 32;
        QIcon icon = createTestIcon(sizes, Qt::red);

        IconCache cache(m_sandBoxDirName);
        QString themePath = cache.themePath(icon);
        QCOMPARE(themePath, m_sandBoxDirName + "/icons");

        QFileInfo info(themePath);
        QVERIFY(info.isDir());
    }

    void testThemePathForThemedIcons()
    {
        IconCache cache(m_sandBoxDirName);
        QIcon icon = QIcon::fromTheme("indicator-messages");
        QString themePath = cache.themePath(icon);

        if (icon.name().isEmpty()) {
            QSKIP("Icon has not been found in theme, so there's nothing to test here", SkipSingle);
        }

        QCOMPARE(themePath, QDir::homePath() + "/.local/share/icons");
    }

    void testPixmapIcon()
    {
        IconCache cache(m_sandBoxDirName);

        QList<int> sizes = QList<int>() << 16 << 22 << 32;
        QIcon icon = createTestIcon(sizes, Qt::red);

        QString name = cache.nameForIcon(icon);
        Q_FOREACH(int size, sizes) {
            QString dirName = cache.themePath(icon) + QString("/hicolor/%1x%1/apps").arg(size);
            QVERIFY(QFile::exists(dirName));
            QImage image;
            QVERIFY(image.load(dirName + "/" + name + ".png"));
        }
    }

    void testTrimCache()
    {
        IconCache cache(m_sandBoxDirName);

        // Create more icons than cache can hold
        QList<int> sizes = QList<int>() << 16 << 22 << 32;
        const int extraCount = 4;
        const int count = IconCache::MaxIconCount + extraCount;
        QStringList createdIconNames;
        for (int idx = 0; idx < count; ++idx) {
            QColor color = QColor::fromHsvF(double(idx) / count, 1., 1.);
            QIcon icon = createTestIcon(sizes, color);
            createdIconNames << cache.nameForIcon(icon);
        }

        // Create a QSet of the names which should still be there
        QSet<QString> expectedRemainingNames;
        auto it = createdIconNames.begin() + extraCount;
        for (; it != createdIconNames.end(); ++it) {
            expectedRemainingNames << *it + ".png";
        }

        // For each size, check the remaining icons are expectedRemainingNames
        Q_FOREACH(int size, sizes) {
            QString dirName = cache.themePath() + QString("/hicolor/%1x%1/apps").arg(size);
            QVERIFY(QFile::exists(dirName));
            QDir dir(dirName);
            dir.setFilter(QDir::Files);
            QSet<QString> remainingNames = dir.entryList().toSet();
            QCOMPARE(remainingNames.count(), IconCache::MaxIconCount);
            QCOMPARE(remainingNames, expectedRemainingNames);
        }
    }

    void testCacheOrder()
    {
        IconCache cache(m_sandBoxDirName);
        QList<int> sizes = QList<int>() << 16 << 22 << 32;
        QIcon icon1 = createTestIcon(sizes, Qt::red);
        QIcon icon2 = createTestIcon(sizes, Qt::green);
        QIcon icon3 = createTestIcon(sizes, Qt::blue);
        cache.nameForIcon(icon1);
        cache.nameForIcon(icon2);
        cache.nameForIcon(icon3);

        // cacheKeysBefore should contains keys for icon1, icon2, icon3
        QStringList cacheKeysBefore = cache.cacheKeys();

        // Access icon1, this should place icon1 at the end of cacheKeys, ie:
        // icon2, icon3, icon1
        cache.nameForIcon(icon1);

        QStringList expectedCacheKeys = cacheKeysBefore;
        expectedCacheKeys << expectedCacheKeys.takeFirst();

        // Check the result
        QCOMPARE(cache.cacheKeys(), expectedCacheKeys);
    }

    void testDetectDuplicatedImages()
    {
        IconCache cache(m_sandBoxDirName);
        QList<int> sizes = QList<int>() << 16 << 22 << 32;
        QIcon icon1 = createTestIcon(sizes, Qt::red);
        QIcon icon2 = createTestIcon(sizes, Qt::red);
        QString icon1Name = cache.nameForIcon(icon1);
        QString icon2Name = cache.nameForIcon(icon2);
        QCOMPARE(icon1Name, icon2Name);
    }

private:
    QString m_sandBoxDirName;
};

QTEST_MAIN(IconCacheTest)

#include <iconcachetest.moc>
