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
#ifndef STATUSNOTIFIERITEMFACTORY_H
#define STATUSNOTIFIERITEMFACTORY_H

// Local
#include <private/qabstractsystemtrayiconsys_p.h>

// Qt
#include <QSet>

class IconCache;
class StatusNotifierItem;

class StatusNotifierItemFactory : public QSystemTrayIconSysFactoryInterface
{
    Q_OBJECT
    Q_INTERFACES(QSystemTrayIconSysFactoryInterface:QFactoryInterface)
public:
    StatusNotifierItemFactory();
    ~StatusNotifierItemFactory();
    QAbstractSystemTrayIconSys * create(QSystemTrayIcon *trayIcon);
    bool isAvailable() const;

private Q_SLOTS:
    void slotSnwOwnerChanged(const QString&, const QString&, const QString&);
    void slotHostRegisteredWithSnw();
    void slotItemDestroyed(QObject*);

private:
    QString m_iconCacheDir;
    IconCache* m_iconCache;
    bool m_isAvailable;
    QSet<StatusNotifierItem*> m_items;

    void connectToSnw();
    void registerItem(StatusNotifierItem*);
};

#endif /* STATUSNOTIFIERITEMFACTORY_H */
