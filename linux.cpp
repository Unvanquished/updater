/* Unvanquished Updater
 * Copyright (C) Unvanquished Developers
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "system.h"
#include <unistd.h>
#include "settings.h"
#include "quazip/quazip/JlCompress.h"
#include <QDir>
#include <QDebug>
#include <QCoreApplication>
#include <QProcess>
#include <QRegularExpression>
#include <QStandardPaths>
#include <QString>

namespace Sys {

namespace {

// Use QProcess::splitCommand in Qt 5.15+
QStringList splitArgs(const QString& command) {
    QRegularExpression argPart(R"regex((")""|"([^\"]*)"?|([^ ]))regex");
    QRegularExpression arg("(" + argPart.pattern() + ")+");
    QStringList list;
    for (QRegularExpressionMatchIterator i = arg.globalMatch(command); i.hasNext(); ) {
        QString str;
        for (QRegularExpressionMatchIterator j = argPart.globalMatch(i.next().captured()); j.hasNext(); ) {
            QRegularExpressionMatch match = j.next();
            str += match.captured(match.lastCapturedIndex());
        }
        list.append(str);
    }
    return list;
}

// 'p' because of custom command lines
QString DoExecvp(const QStringList& args)
{
    if (args.empty()) return "missing command line";

    Settings settings;
    settings.sync(); // since normal shutdown will be skipped

    std::vector<std::string> argsUtf8;
    for (const QString& arg : args) {
        argsUtf8.push_back(arg.toStdString());
    }
    std::vector<const char*> argv;
    for (const std::string& arg : argsUtf8) {
        argv.push_back(arg.c_str());
    }
    argv.push_back(nullptr);

    qDebug() << "execvp with command line:" << args;
    execvp(argv[0], const_cast<char* const*>(argv.data()));

    QString msg = QString("error %1 (%2)").arg(errno).arg(strerror(errno));
    return msg;
}

} // namespace

QString archiveName()
{
    return "linux-amd64.zip";
}

void migrateHomePath()
{
    QString legacyHomePath = QDir::homePath() + "/.unvanquished";
    QString xdgDataHome = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);
    QString xdgHomePath = xdgDataHome + "/unvanquished";

    if (QDir(legacyHomePath).exists()) {
        if (QDir(xdgHomePath).exists()) {
            qWarning("Legacy home path %s exists but XDG home path %s already exists, doing nothing", qPrintable(legacyHomePath), qPrintable(xdgHomePath));
            return;
        }

        if (!QDir(xdgDataHome).exists() && !QDir().mkdir(xdgDataHome)) {
            qFatal("Could not create XDG data directory %s", qPrintable(xdgDataHome));
        }

        QFileInfo fileinfo(legacyHomePath);
        if (fileinfo.isSymLink()) {
            qInfo("Creating legacy home path symlink %s to XDG home path %s", qPrintable(legacyHomePath), qPrintable(xdgHomePath));
            QFile symlink(legacyHomePath);
            if (!symlink.link(xdgHomePath)) {
                qFatal("Could not create symlink %s", qPrintable(xdgHomePath));
            }
        } else {
            qInfo("Renaming legacy home path %s to XDG home path %s", qPrintable(legacyHomePath), qPrintable(xdgHomePath));
            QDir directory;
            if (!directory.rename(legacyHomePath, xdgHomePath)) {
                qFatal("Could not rename legacy home path to %s", qPrintable(xdgHomePath));
            }
        }
    }
}

QString defaultInstallPath()
{
    // if needed, migrate legacy homepath to prevent the updater
    // to create the directory before the engine tries to migrate
    // it itself
    migrateHomePath();

    // Does not use QStandardPaths::AppDataLocation because
    // it returns "~/.local/share/unvanquished/updater"
    // and we want "~/.local/share/unvanquished/base"
    // game itself puts stuff in "~/.local/share/unvanquished"
    return QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + "/unvanquished/base";
}

bool validateInstallPath(const QString& installPath)
{
    // The default install location is inside the homepath and ends with /base (any user-selected
    // path must end with /Unvanquished). Running as root with this location may result in an
    // unusable install, if the homepath ends up not writable by the regular user.
    return !(installPath.endsWith("/base") && getuid() == 0);
}

bool installShortcuts()
{
    // Set up menu and protocol handler
    Settings settings;
    QString desktopDirString = QStandardPaths::writableLocation(QStandardPaths::ApplicationsLocation);
    if (desktopDirString.isEmpty()) {
        qDebug() << "ApplicationsLocation for desktop files not provided";
        return false;
    }
    QDir desktopDir(desktopDirString);
    if (desktopDir.exists()) {
        desktopDir.remove("unvanquished.desktop"); // updater v0.0.5 and before
        desktopDir.remove("net.unvanquished.UnvanquishedProtocolHandler.desktop"); // up to v0.2.0
    } else {
        if (!desktopDir.mkpath(".")) {
            qDebug() << "Could not create directory for desktop files" << desktopDirString;
            return false;
        }
        qDebug() << "Created directory for desktop files" << desktopDirString;
    }

    QString desktopFileName = "net.unvanquished.Unvanquished.desktop";
    QFile desktopFile(":resources/" + desktopFileName);
    if (!desktopFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "missing resource" << desktopFileName;
        return false;
    }
    QString desktopStr = QString(desktopFile.readAll().data()).arg(settings.installPath());
    {
        QFile outputFile(desktopDir.filePath(desktopFileName));
        if (!outputFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
            qDebug() << "error opening" << outputFile.fileName();
            return false;
        }
        if (outputFile.write(desktopStr.toUtf8().constData(), desktopStr.size()) != desktopStr.size()) {
            qDebug() << "error writing" << outputFile.fileName();
            return false;
        }
        qDebug() << "Created desktop file" << outputFile.fileName();
    }

    int ret = QProcess::execute("xdg-mime",
                                {QString("default"),
                                 desktopDir.filePath(desktopFileName),
                                 QString("x-scheme-handler/unv")});
    qDebug() << "xdg-mime returned" << ret;
    ret = QProcess::execute("update-desktop-database", {desktopDirString});
    qDebug() << "update-desktop-database returned" << ret;

    // install icon
    QString iconDir = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + "/icons/hicolor/128x128/apps/";
    QDir dir(iconDir);
    if (!dir.exists()) {
        if (!dir.mkpath(dir.path())) {
            return false;
        }
    }
    QFile::remove(iconDir + "unvanquished.png");
    return QFile::copy(":resources/unvanquished.png", iconDir + "unvanquished.png");
}

bool installUpdater(const QString& installPath) {
    QFileInfo src(QCoreApplication::applicationFilePath());
    QFileInfo dest(installPath + QDir::separator() + "updater");
    if (src == dest) {
        qDebug() << "Updater already in install location";
        return true;
    }
    if (dest.exists()) {
        qDebug() << "Deleting updater in install path";
        if (!QFile::remove(dest.filePath())) {
            return false;
        }
    }
    qDebug() << "Copying updater from" << src.absoluteFilePath();
    return QFile::copy(src.absoluteFilePath(), dest.filePath()) &&
           QFile::setPermissions(dest.filePath(), static_cast<QFileDevice::Permissions>(0x775));
           // Yes it is really supposed to be 0x775 not 0775
}

bool updateUpdater(const QString& updaterArchive, const QString& connectUrl)
{
    QString current = QCoreApplication::applicationFilePath();
    QString backup = current + ".bak";
    QFile backupUpdater(backup);
    if (backupUpdater.exists()) {
        if (!backupUpdater.remove()) {
            qDebug() << "Could not remove backup updater. Aborting autoupdate.";
            return false;
        }
    }
    if (!QFile::rename(current, backup)) {
        qDebug() << "Could not move " << current << " to " << backup;
        return false;
    }
    QDir destination(current);
    if (!destination.cdUp()) {
        qDebug() << "Unexpected destination";
        return false;
    }
    // Only expect a single executable.
    auto out = JlCompress::extractDir(updaterArchive, destination.absolutePath());
    if (out.size() < 1) {
        qDebug() << "Error extracting update.";
        return false;
    }
    if (out.size() != 1) {
        qDebug() << "Invalid update archive.";
        return false;
    }

    if (out[0] != current) {
        if (!QFile::rename(out[0], current)) {
            qDebug() << "Error renaming new updater to previous file name.";
            return false;
        }
    }

    QStringList args;
    args << current;
    if (!connectUrl.isEmpty()) {
        args << "--" << connectUrl;
    }
    QString error = DoExecvp(args);
    qDebug() << "updater update exec failed:" << error;
    return false;
}

QString updaterArchiveName()
{
    return "UnvUpdaterLinux.zip";
}

std::string getCertStore()
{
    // From Go: https://golang.org/src/crypto/x509/root_linux.go
    static QStringList CERT_LOCATIONS = {
        "/etc/ssl/certs/ca-certificates.crt",                // Debian/Ubuntu/Gentoo etc.
        "/etc/pki/tls/certs/ca-bundle.crt",                  // Fedora/RHEL 6
        "/etc/ssl/ca-bundle.pem",                            // OpenSUSE
        "/etc/pki/tls/cacert.pem",                           // OpenELEC
        "/etc/pki/ca-trust/extracted/pem/tls-ca-bundle.pem", // CentOS/RHEL 7
    };

    for (const QString& path : CERT_LOCATIONS) {
        QFile file(path);
        if (file.exists()) return path.toStdString();
    }
    return "";
}

void initApplicationName()
{
    QCoreApplication::setOrganizationName("unvanquished");
    QCoreApplication::setApplicationName("updater");
}

// Settings are stored in ~/.config/unvanquished/updater.conf
QSettings* makePersistentSettings(QObject* parent)
{
    return new QSettings(parent);
}

QString getGameCommand(const QString& installPath)
{
    return QuoteQProcessCommandArgument(installPath + QDir::separator() + "daemon");
}

QString startGame(const QString& commandLine, bool, const QString&)
{
    QString error = DoExecvp(splitArgs(commandLine));
    QString msg = QString("error %1 (%2)").arg(errno).arg(strerror(errno));
    qDebug() << "game exec failed:" << msg;
    return msg;
}

ElevationResult RelaunchElevated(const QString&)
{
    return ElevationResult::UNNEEDED;
}

}  // namespace Sys
