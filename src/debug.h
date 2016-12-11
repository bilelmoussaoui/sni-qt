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
#ifndef DEBUG_H
#define DEBUG_H

// Local
#include <settings.h>

// Qt
#include <QDebug>

namespace Debug {
enum Level {
    WarningLevel,
    InfoLevel,
    DebugLevel
};

QDebug trace(Level level, const char* function);

} // namespace

// Simple macros to get KDebug like support
#define SNI_DEBUG if (Settings::debug()) Debug::trace(Debug::DebugLevel, __PRETTY_FUNCTION__)
#define SNI_INFO Debug::trace(Debug::InfoLevel, __PRETTY_FUNCTION__)
#define SNI_WARNING Debug::trace(Debug::WarningLevel, __PRETTY_FUNCTION__)

// Log a variable name and value
#define SNI_VAR(var) SNI_DEBUG << #var ":" << var

#define SNI_RETURN_IF_FAIL(cond) if (!(cond)) { \
    SNI_WARNING << "Condition failed: " #cond; \
    return; \
}

#define SNI_RETURN_VALUE_IF_FAIL(cond, value) if (!(cond)) { \
    SNI_WARNING << "Condition failed: " #cond; \
    return (value); \
}

#endif /* DEBUG_H */
