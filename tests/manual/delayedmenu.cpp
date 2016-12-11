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
#include <delayedmenu.h>

// Qt
#include <QApplication>
#include <QDebug>
#include <QIcon>
#include <QMenu>
#include <QSystemTrayIcon>
#include <QTimer>

DelayedMenuApp::DelayedMenuApp()
: m_sti(new QSystemTrayIcon(this))
{
    m_sti->setIcon(QIcon(":/images/bad.svg"));
	m_sti->show();
    QTimer::singleShot(500, this, SLOT(setMenu()));
}

void DelayedMenuApp::setMenu()
{
    qDebug() << "Setting menu";
    QMenu* menu = new QMenu;
    menu->addAction("Quit", qApp, SLOT(quit()));
    m_sti->setContextMenu(menu);
}

int main(int argc, char** argv)
{
	QApplication app(argc, argv);
    DelayedMenuApp dmApp;
	return app.exec();
}

#include <delayedmenu.moc>
