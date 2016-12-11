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
// Local
#include <fsutils.h>

// Qt
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QProcess>
#include <QtTest>

// libc
#include <stdlib.h>

static void createEmptyFile(const QString& name)
{
    QFile file(name);
    QVERIFY(file.open(QIODevice::WriteOnly));
    file.close();
    QVERIFY(QFile::exists(name));
}

class FsUtilsTest : public QObject
{
Q_OBJECT
private Q_SLOTS:
    void initTestCase()
    {
        m_baseDirName = QDir::currentPath();
    }

    void init()
    {
        QDir::setCurrent(m_baseDirName);
        m_sandBoxDirName = m_baseDirName + "/sandbox";
        setenv("TMPDIR", m_sandBoxDirName.toLocal8Bit().constData(), 1 /*overwrite*/);
    }

    void cleanup()
    {
        QProcess::execute("/bin/rm", QStringList() << "-r" << m_sandBoxDirName);
    }

    void testGenerateTempDir()
    {
        QString prefix = "fsutilstest";
        QString dirName = FsUtils::generateTempDir(prefix);
        QFileInfo info(dirName);
        QVERIFY(info.exists());
        QVERIFY(info.isDir());
        QVERIFY(info.isAbsolute());
        QVERIFY(info.fileName().startsWith(prefix + "-"));
    }

    void testRecursiveRm()
    {
        QDir dir(m_sandBoxDirName);
        QVERIFY(dir.mkpath("dir/dir1"));
        QVERIFY(dir.mkpath("dir/dir2"));
        QVERIFY(dir.mkpath("dir/.hiddendir"));
        QString testDir = dir.path() + "/dir";
        createEmptyFile(testDir + "/dir1/f1");
        createEmptyFile(testDir + "/dir2/f2");
        createEmptyFile(testDir + "/dir2/.hidden");
        createEmptyFile(testDir + "/.hiddendir/f1");
        createEmptyFile(testDir + "/.hiddendir/.hidden");

        QVERIFY(FsUtils::recursiveRm(testDir));
        QVERIFY(!QFile::exists(testDir));
    }

    void testRecursiveRmEmptyDir()
    {
        // Calling recursiveRm() with an empty QString should not do anything
        // https://bugs.launchpad.net/sni-qt/+bug/874447
        QDir dir(m_sandBoxDirName);
        dir.mkpath("dir");
        QString testDir = dir.path() + "/dir";

        QVERIFY(QDir::setCurrent(testDir));
        QVERIFY(!FsUtils::recursiveRm(QString()));
        QVERIFY(QFile::exists(testDir));
    }

    void testTouch()
    {
        QDir dir(m_sandBoxDirName);
        dir.mkpath(".");
        QString testFile = dir.path() + "/test";
        createEmptyFile(testFile);
        QFileInfo info(testFile);
        QDateTime mtime = info.lastModified();

        QVERIFY(FsUtils::touch(testFile, mtime.addSecs(1)));

        info.refresh();
        QVERIFY(mtime.msecsTo(info.lastModified()) == 1000);
    }

private:
    QString m_baseDirName;
    QString m_sandBoxDirName;
};

QTEST_MAIN(FsUtilsTest)

#include <fsutilstest.moc>
