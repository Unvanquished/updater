#include "system.h"
#include "settings.h"
#include "quazip/quazip/JlCompress.h"
#include <QDir>
#include <QDebug>
#include <QCoreApplication>
#include <QProcess>
#include <QStandardPaths>
#include <QString>

namespace Sys {
QString archiveName()
{
    return "linux64.zip";
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

QString executableName()
{
    return "daemon";
}

bool install()
{
    // Set up menu and protocol handler
    Settings settings;
    QFile desktopFile(":resources/unvanquished.desktop");
    if (!desktopFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }
    QString desktopStr = QString(desktopFile.readAll().data())
        .arg(settings.installPath());
    QFile outputFile(QStandardPaths::writableLocation(QStandardPaths::ApplicationsLocation) + "/unvanquished.desktop");
    if (!outputFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        desktopFile.close();
        return false;
    }
    outputFile.write(desktopStr.toUtf8().constData(), desktopStr.size());
    outputFile.close();

    // install icon
    QString iconDir = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + "/icons/hicolor/128x128/apps/";
    QDir dir(iconDir);
    if (!dir.exists()) {
        if (!dir.mkpath(dir.path())) {
            return false;
        }
    }
    QFile::copy(":resources/unvanquished.png",
                iconDir + "unvanquished.png");
    return true;
}

bool updateUpdater(const QString& updaterArchive)
{
    QString current = QCoreApplication::applicationFilePath();
    QString backup = current + ".bak";
    QFile backupUpdater(backup);
    if (backupUpdater.exists()) {
        if (!backupUpdater.remove()) {
            qDebug() << "Could not remove backup updater. Aboring autoupdate.";
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

    if (!QProcess::startDetached(current, QStringList())) {
        qDebug() << "Error starting " << current;
        return false;
    }
    QCoreApplication::quit();
    return true;
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

// Settings are stored in ~/.config/unvanquished/updater.conf
QSettings* makePersistentSettings(QObject* parent)
{
    return new QSettings("unvanquished", "updater", parent);
}

}  // namespace Sys
