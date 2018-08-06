#include "system.h"
#include "settings.h"
#include "quazip/quazip/JlCompress.h"
#include <QDir>
#include <QDebug>
#include <QCoreApplication>
#include <QProcess>

namespace Sys {
QString archiveName(void)
{
    return "linux64.zip";
}

QString defaultInstallPath(void)
{
    return QDir::homePath() + "/.local/share/Unvanquished";
}

QString executableName(void)
{
    return "daemon";
}

Ret install(void)
{
    // Set up menu and protocol handler
    Settings settings;
    QFile desktopFile(":resources/unvanquished.desktop");
    if (!desktopFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return Ret(false) << "Error openning the desktop file for reading: " << desktopFile.fileName();
    }
    QString desktopStr = QString(desktopFile.readAll().data())
        .arg(settings.installPath());
    QFile outputFile(QDir::homePath() + "/.local/share/applications/unvanquished.desktop");
    if (!outputFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        desktopFile.close();
        return Ret(false) << "Error openning output file for writing: " << outputFile.fileName();
    }
    qint64 written = outputFile.write(desktopStr.toUtf8().constData(), desktopStr.size());
    if (written != desktopStr.size()) {
        return Ret(false) << "Mismatched write. Expected to write "
                          << std::to_string(desktopStr.size()) << " but only wrote "
                          << std::to_string(written);
    }
    outputFile.close();

    // install icon
    QString iconDir = QDir::homePath() + "/.local/share/icons/hicolor/128x128/apps/";
    QDir dir(iconDir);
    if (!dir.exists()) {
        if (!dir.mkpath(dir.path())) {
            return Ret(false) << "Error creating path: " << dir.path();
        }
    }

    // Don't error out on icon installation failure.
    if (!QFile::copy(":resources/unvanquished.png",
                iconDir + "unvanquished.png")) {
        qDebug() << "WARNING: Error installing icon to " << iconDir;
    }
    return true;
}

Ret updateUpdater(const QString& updaterArchive)
{
    QString current = QCoreApplication::applicationFilePath();
    QString backup = current + ".bak";
    QFile backupUpdater(backup);
    if (backupUpdater.exists()) {
        if (!backupUpdater.remove()) {
            return Ret(false) << "Could not remove backup updater. Aboring autoupdate.";
        }
    }
    if (!QFile::rename(current, backup)) {
        return Ret(false) << "Could not move " << current << " to " << backup;
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

    if (!QProcess::startDetached(current)) {
        qDebug() << "Error starting " << current;
        return false;
    }
    QCoreApplication::quit();
    return true;
}

QString updaterArchiveName(void)
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

}  // namespace Sys
