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
#include <settings.h>

// Qt
#include <QCoreApplication>
#include <QSettings>

namespace Settings {

static bool s_initialized = false;
static bool s_needsActivateAction;
static bool s_debug;

static void init()
{
    if (s_initialized) {
        return;
    }
    QSettings settings("sni-qt");

    QString binaryName = QCoreApplication::applicationFilePath().section("/", -1);
    QString key = QString("need-activate-action/%1").arg(binaryName);
    s_needsActivateAction = settings.value(key).toBool();

    // Keys without a group name are picked from the "general" group
    s_debug = settings.value("debug").toBool();

    s_initialized = true;
}

bool needsActivateAction()
{
    init();
    return s_needsActivateAction;
}

bool debug()
{
    init();
    return s_debug;
}

} // namespace