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
#ifndef FSUTILS_H
#define FSUTILS_H

// Qt
#include <QString>

class QDateTime;

namespace FsUtils {

QString generateTempDir(const QString& prefix);

bool recursiveRm(const QString& dirName);

bool touch(const QString& name, const QDateTime& mtime);

} // namespace

#endif /* FSUTILS_H */
