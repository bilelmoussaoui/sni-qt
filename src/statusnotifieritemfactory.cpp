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
#include <statusnotifieritemfactory.h>

// Local
#include <debug.h>
#include <iconcache.h>
#include <fsutils.h>
#include <statusnotifieritem.h>

// Qt
#include <QCoreApplication>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusReply>
#include <QDBusServiceWatcher>
#include <QDebug>
#include <QtPlugin>
#include <fstream>
using namespace std;

static const char *SNW_SERVICE = "org.kde.StatusNotifierWatcher";
static const char *SNW_IFACE   = "org.kde.StatusNotifierWatcher";
static const char *SNW_PATH    = "/StatusNotifierWatcher";

StatusNotifierItemFactory::StatusNotifierItemFactory()
: m_iconCache(0)
, m_isAvailable(false)
{
    QString tempSubDir = QString("sni-qt_%1_%2")
        .arg(QCoreApplication::applicationFilePath().section('/', -1))
        .arg(QCoreApplication::applicationPid());
    m_iconCacheDir = FsUtils::getTempDirName(tempSubDir);
    m_iconCache = new IconCache(m_iconCacheDir, this);
    QDBusServiceWatcher* snwWatcher = new QDBusServiceWatcher(this);
    snwWatcher->addWatchedService(SNW_SERVICE);
    connect(snwWatcher, SIGNAL(serviceOwnerChanged(const QString&, const QString&, const QString&)),
        SLOT(slotSnwOwnerChanged(const QString&, const QString&, const QString&)));
    snwWatcher->setConnection(QDBusConnection::sessionBus());

    connectToSnw();
}

StatusNotifierItemFactory::~StatusNotifierItemFactory()
{
    SNI_DEBUG;
    if (!m_iconCacheDir.isEmpty()) {
        FsUtils::recursiveRm(m_iconCacheDir);
    }
}

void StatusNotifierItemFactory::connectToSnw()
{
    SNI_DEBUG;
    m_isAvailable = false;
    QDBusInterface snw(SNW_SERVICE, SNW_PATH, SNW_IFACE);
    if (!snw.isValid()) {
        SNI_WARNING << "Invalid interface to SNW_SERVICE";
        return;
    }

    QDBusConnection bus = QDBusConnection::sessionBus();
    bus.connect(SNW_SERVICE, SNW_PATH, SNW_IFACE, "StatusNotifierHostRegistered", "",
        this, SLOT(slotHostRegisteredWithSnw()));

    // FIXME: Make this async?
    QVariant value = snw.property("IsStatusNotifierHostRegistered");
    if (!value.canConvert<bool>()) {
        SNI_WARNING << "IsStatusNotifierHostRegistered returned something which is not a bool:" << value;
        return;
    }
    m_isAvailable = value.toBool();
    SNI_VAR(m_isAvailable);
    Q_FOREACH(StatusNotifierItem* item, m_items) {
        registerItem(item);
    }
}

QAbstractSystemTrayIconSys *StatusNotifierItemFactory::create(QSystemTrayIcon *trayIcon)
{
    SNI_DEBUG;
            qCritical("sni-qt also something");
    StatusNotifierItem* item = new StatusNotifierItem(trayIcon, m_iconCache);
    connect(item, SIGNAL(destroyed(QObject*)), SLOT(slotItemDestroyed(QObject*)));
    m_items.insert(item);
    registerItem(item);
    return item;
}

bool StatusNotifierItemFactory::isAvailable() const
{
    return m_isAvailable;
}

void StatusNotifierItemFactory::slotSnwOwnerChanged(const QString&, const QString& oldOwner, const QString& newOwner)
{
    SNI_DEBUG << "oldOwner" << oldOwner << "newOwner" << newOwner;
    bool oldAvailable = m_isAvailable;
    if (newOwner.isEmpty()) {
        m_isAvailable = false;
    } else {
        connectToSnw();
    }

    if (oldAvailable != m_isAvailable) {
        SNI_DEBUG << "Emitting availableChanged(" << m_isAvailable << ")";
        availableChanged(m_isAvailable);
    }
}

void StatusNotifierItemFactory::slotHostRegisteredWithSnw()
{
    SNI_DEBUG;
    if (!m_isAvailable) {
        m_isAvailable = true;
        SNI_DEBUG << "Emitting availableChanged(true)";
        availableChanged(m_isAvailable);
    }
}

void StatusNotifierItemFactory::slotItemDestroyed(QObject* obj)
{
    SNI_DEBUG;
    m_items.remove(static_cast<StatusNotifierItem*>(obj));
}

void StatusNotifierItemFactory::registerItem(StatusNotifierItem* item)
{
    SNI_DEBUG;
    QDBusInterface snw(SNW_SERVICE, SNW_PATH, SNW_IFACE);
    snw.asyncCall("RegisterStatusNotifierItem", item->serviceName());
}


Q_EXPORT_PLUGIN2(statusnotifieritem, StatusNotifierItemFactory)

#include <statusnotifieritemfactory.moc>
