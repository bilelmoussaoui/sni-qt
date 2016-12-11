# Summary

This plugin makes Qt applications use the StatusNotifierItem (SNI) protocol for
their system tray icons. It does so by turning system tray icons created with
the QSystemTrayIcon class into StatusNotifierItems.

This plugin only works when used together with the
"qsystemtrayicon-plugin-system-4.7" Qt branch. This branch is available from
this repository: http://gitorious.org/~agateau/qt/agateau-qt.

For convenience, the patch for Qt 4.7.4 is available in the patches/ dir.

# SNI category support

QSystemTrayIcon has no concept of categories, so sni-qt will create SNI in the
ApplicationStatus category by default. Application developers can override this
by setting the "_sni_qt_category" property on their QSystemTrayIcon instance.

Valid values for the properties are:
- "ApplicationStatus" (the default)
- "Communications"
- "SystemServices"
- "Hardware"

So for example to create a "Hardware" SNI, one would write:

    QSystemTrayIcon* icon = new QSystemTrayIcon();
    ...
    icon->setProperty("_sni_qt_category", "Hardware");
    ...

# QSystemTrayIcon::Trigger support
When sni-qt receives the StatusNotifierItem::Activate() call, it simulates a
left-click on the QSystemTrayIcon and emits
QSystemTrayIcon::activated(QSystemTrayIcon::Trigger)

Some implementations of the SNI protocol call StatusNotifierItem::Activate() on
left click and show the context menu on right click. KDE behaves like this.

Other implementations (Unity for example), only supports the context menu. This
means the action which is normally triggered by left-clicking the icon must also
be available in the context menu, otherwise it is not possible for the user to
trigger it.

In case an application does not provide an entry in its context menu to trigger
the same action as left-clicking on the icon, you can tell sni-qt to add an
"Activate" action at the top of the menu. This is configured in a file named
'sni-qt.conf'. sni-qt will look for it in '$HOME/.config' and '/etc/xdg'.

This file should look like this:

    [need-activate-action]
    foo=1
    bar=1

Where 'foo' and 'bar' are the binary names of the applications which needs an
"Activate" action. Setting the value to '0' will disable the action again (so
you can override the system-wide file)

# Debug

In the configuration file, add:

    [general]
    debug=1

To activate debug output to stderr.

# Author

Canonical DX Team
Maintainer: Renato Filho <renato.filho@canonical.com>
Former maintainer: Aurélien Gâteau
