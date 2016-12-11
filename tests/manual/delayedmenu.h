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
#ifndef DELAYEDMENU_H
#define DELAYEDMENU_H

#include <QObject>

class QSystemTrayIcon;

class DelayedMenuApp : public QObject
{
    Q_OBJECT
public:
    DelayedMenuApp();

private Q_SLOTS:
    void setMenu();

private:
    QSystemTrayIcon* m_sti;
};

#endif /* DELAYEDMENU_H */
