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
#ifndef ICONCACHE_H
#define ICONCACHE_H

// Qt
#include <QObject>
#include <QIcon>
#include <QStringList>

class QIcon;

/**
 * This class will save pixmaps from icons in a temporary dir on the disk,
 * making it possible to pass filenames for icons without names.
 */
class IconCache : public QObject
{
    Q_OBJECT
public:
    IconCache(const QString& baseDir, QObject* parent=0);

    static const int MaxIconCount;

    QString themePath(const QIcon& icon = QIcon()) const;
    QString nameForIcon(const QIcon& icon) const;

    // Internal, testing only
    QStringList cacheKeys() const { return m_cacheKeys; }

private:
    QString m_themePath;
    mutable QStringList m_cacheKeys;

    void cacheIcon(const QString& key, const QIcon&) const;
    void trimCache() const;
};

#endif /* ICONCACHE_H */
