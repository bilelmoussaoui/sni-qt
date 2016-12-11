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
#include <debug.h>

// Qt
#include <QCoreApplication>
#include <QTime>

namespace Debug {

QDebug trace(Level level, const char* function)
{
    static const char* levelStrings[] = {
        "WARN ",
        "INFO ",
        "DEBUG"
    };
    static const QByteArray prefix = QByteArray("sni-qt/") + QByteArray::number(QCoreApplication::applicationPid());
    QDebug stream = level == WarningLevel ? qWarning() : qDebug();
    stream << prefix << levelStrings[level];
    stream << QTime::currentTime().toString("HH:mm:ss.zzz").toUtf8().data();
    stream << function;
    return stream;
}

} // namespace Debug
