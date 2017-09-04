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
#include "statusnotifieritem.h"

// Local
#include <debug.h>
#include <iconcache.h>
#include <settings.h>
#include <statusnotifieritemadaptor.h>

// dbusmenu-qt
#include <dbusmenuexporter.h>

// Qt
#include <QApplication>
#include <QCoreApplication>
#include <QCursor>
#include <QDBusConnection>
#include <QDBusMetaType>
#include <QDebug>
#include <QMenu>
#include <QTranslator>
#include <QWheelEvent>

#if defined(Q_WS_X11)
#include <QX11Info>
#endif

static const char* SNI_CATEGORY_PROPERTY = "_sni_qt_category";
static const char* DEFAULT_CATEGORY = "ApplicationStatus";

static const char* NOTIFICATION_INTERFACE = "org.freedesktop.Notifications";
static const char* NOTIFICATION_SERVICE = "org.freedesktop.Notifications";
static const char* NOTIFICATION_PATH = "/org/freedesktop/Notifications";

void registerMetaTypes()
{
    static bool registered = false;
    if (registered) {
        return;
    }
    qDBusRegisterMetaType<DBusImage>();
    qDBusRegisterMetaType<DBusImageList>();
    qDBusRegisterMetaType<DBusToolTip>();
}

StatusNotifierItem::StatusNotifierItem(QSystemTrayIcon* icon, IconCache* iconCache)
: QAbstractSystemTrayIconSys(icon)
, m_iconCache(iconCache)
, m_activateAction(0)
, m_placeholderMenu(new QMenu)
{
    SNI_VAR(this);
    registerMetaTypes();

    static int id = 1;
    m_serviceName = QString("org.kde.StatusNotifierItem-%1-%2").arg(QCoreApplication::applicationPid()).arg(id++);
    SNI_VAR(m_serviceName);

    m_objectPath = QString("/StatusNotifierItem");
    SNI_VAR(m_objectPath);

    new StatusNotifierItemAdaptor(this);
    QDBusConnection bus = QDBusConnection::sessionBus();
    bus.registerService(m_serviceName);
    bus.registerObject(m_objectPath, this, QDBusConnection::ExportAdaptors);

    updateMenu();
}

StatusNotifierItem::~StatusNotifierItem()
{
    SNI_VAR(this);

    QDBusConnection bus = QDBusConnection::sessionBus();
    bus.unregisterObject(m_objectPath, QDBusConnection::UnregisterTree);
    bus.unregisterService(m_serviceName);

    // m_dbusMenuExporter is a child of the menu it exports, so we need to use
    // a QWeakPointer to track it:
    //
    // - If StatusNotifierItem is deleted before menu, we must delete the
    // exporter (otherwise we can't associate another exporter to it)
    //
    // - If the menu is deleted before StatusNotifierItem, then the exporter
    // will already be gone when we reach this point.
    delete m_dbusMenuExporter.data();
    delete m_placeholderMenu;
}

QRect StatusNotifierItem::geometry() const
{
    return QRect();
}

void StatusNotifierItem::updateVisibility()
{
    SNI_DEBUG << "NewStatus(" << status() << ")";
    NewStatus(status());
}

void StatusNotifierItem::updateIcon()
{
    SNI_DEBUG;
    NewIcon();
    // ToolTip contains the icon
    NewToolTip();
}

void StatusNotifierItem::updateToolTip()
{
    SNI_DEBUG;
    NewToolTip();
}

void StatusNotifierItem::updateMenu()
{
    // Delete any exporter which could be attached to a previous menu
    delete m_dbusMenuExporter.data();
    QMenu* menu = trayIcon->contextMenu();
    SNI_VAR(menu);
    if (!menu) {
        // A SNI *must* expose a valid menu.  If there is none yet, expose a
        // placeholder.
        // See: https://bugs.launchpad.net/ubuntu/+source/hplip/+bug/860395
        menu = m_placeholderMenu;
    }
    connect(menu, SIGNAL(aboutToShow()), SLOT(slotAboutToShow()));
    m_dbusMenuExporter = new DBusMenuExporter(menuObjectPath(), menu);
}

void StatusNotifierItem::showMessage(const QString &title, const QString &message,
    QSystemTrayIcon::MessageIcon icon, int msecs)
{
    SNI_DEBUG << title << message;
    QString iconString;
    switch (icon) {
    case QSystemTrayIcon::NoIcon:
        break;
    case QSystemTrayIcon::Information:
        iconString = "dialog-information";
        break;
    case QSystemTrayIcon::Warning:
        iconString = "dialog-warning";
        break;
    case QSystemTrayIcon::Critical:
        iconString = "dialog-error";
        break;
    }

    QDBusInterface iface(NOTIFICATION_SERVICE, NOTIFICATION_PATH, NOTIFICATION_INTERFACE);
    iface.asyncCall("Notify",
        id(),
        quint32(0),    // replaces_id
        iconString,
        title,
        message,
        QStringList(), // actions
        QVariantMap(), // hints
        msecs
        );
}

void StatusNotifierItem::Activate(int, int)
{
    SNI_DEBUG;
    sendActivatedByTrigger();
}

void StatusNotifierItem::ContextMenu(int, int)
{
    SNI_DEBUG;
}

void StatusNotifierItem::Scroll(int delta, const QString& orientationString)
{
    SNI_DEBUG << "delta=" << delta << "orientationString=" << orientationString;
    QPoint globalPos = QCursor::pos();
    QPoint pos;
    Qt::Orientation orientation = orientationString == "horizontal" ? Qt::Horizontal : Qt::Vertical;
    QWheelEvent event(pos, globalPos, delta, Qt::NoButton, Qt::NoModifier, orientation);
    QApplication::sendEvent(trayIcon, &event);
}

void StatusNotifierItem::SecondaryActivate(int, int)
{
    SNI_DEBUG;
    sendActivated(QSystemTrayIcon::MiddleClick);
}

QString StatusNotifierItem::iconThemePath() const
{
    return m_iconCache->themePath(trayIcon->icon());
}

QString StatusNotifierItem::iconName() const
{
    QIcon icon = trayIcon->icon();
    if (icon.isNull()) {
        return QString();
    }
    QString name = icon.name();
    if (!name.isEmpty()) {
        return name;
    }
    return m_iconCache->nameForIcon(icon);
}

QString StatusNotifierItem::serviceName() const
{
    return m_serviceName;
}

QString StatusNotifierItem::objectPath() const
{
    return m_objectPath;
}

QString StatusNotifierItem::id() const
{
    return QCoreApplication::applicationFilePath().section('/', -1);
}

QString StatusNotifierItem::title() const
{
    QString title = QCoreApplication::applicationName();
    return title.isEmpty() ? id() : title;
}

QDBusObjectPath StatusNotifierItem::menu() const
{
    return m_dbusMenuExporter
        ? QDBusObjectPath(menuObjectPath())
        : QDBusObjectPath("/invalid");
}

QString StatusNotifierItem::menuObjectPath() const
{
    return objectPath() + "/menu";
}

QString StatusNotifierItem::category() const
{
    static QStringList validCategories = QStringList()
        << "ApplicationStatus"
        << "Communications"
        << "SystemServices"
        << "Hardware"
        ;

    QVariant value = trayIcon->property(SNI_CATEGORY_PROPERTY);
    if (!value.canConvert<QString>()) {
        return DEFAULT_CATEGORY;
    }

    QString category = value.toString();
    if (!validCategories.contains(category)) {
        SNI_WARNING << category << "is not a valid value for the" << SNI_CATEGORY_PROPERTY << "property. Valid values are:"
            << validCategories.join(", ")
            ;
    }
    return category;
}

QString StatusNotifierItem::status() const
{
    return trayIcon->isVisible() ? "Active" : "Passive";
}

DBusToolTip StatusNotifierItem::toolTip() const
{
    DBusToolTip tip;
    tip.iconName = iconName();
    tip.title = trayIcon->toolTip();
    return tip;
}

void StatusNotifierItem::sendActivatedByTrigger()
{
#if defined(Q_WS_X11)
    // Workarounds LP: #627195
    if (QString::fromUtf8(getenv("XDG_CURRENT_DESKTOP")).split(':').contains("Unity")) {
        QX11Info::setAppUserTime(0);
    }
#endif
    sendActivated(QSystemTrayIcon::Trigger);
}

void StatusNotifierItem::slotAboutToShow()
{
    SNI_DEBUG;
    if (!m_activateAction) {
        if (Settings::needsActivateAction()) {
            SNI_INFO << "Adding an \"Activate\" entry to the StatusNotifierItem context menu";
            // Hack: reuse an existing Qt translation so we don't have to add
            // translations ourself. Note that we use QTranslator without
            // installing it to ensure we do not trigger any unwanted side-effect
            // in the application.
            QTranslator translator;
            translator.load("qt_" + QLocale::system().name(), QLibraryInfo::location(QLibraryInfo::TranslationsPath));
            QString text = translator.translate("QApplication", "Activate");
            if (text.isEmpty()) {
                // QTranslator::translate() does not fallback, do it ourself
                text = "Activate";
            }

            m_activateAction = new QAction(this);
            m_activateAction->setText(text);
            connect(m_activateAction, SIGNAL(triggered(bool)), SLOT(sendActivatedByTrigger()));
        }
    }
    if (!m_activateAction) {
        return;
    }
    QMenu* menu = qobject_cast<QMenu*>(sender());
    SNI_RETURN_IF_FAIL(menu);
    if (menu->actions().first() != m_activateAction) {
        menu->insertAction(menu->actions().first(), m_activateAction);
    }
}

#include <statusnotifieritem.moc>
