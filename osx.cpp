#include "system.h"
#include "settings.h"
#include "quazip/quazip/JlCompress.h"

#include <QDir>
#include <QFile>
#include <QDebug>
#include <QCoreApplication>
#include <QFileInfo>
#include <QProcess>

namespace {
// Enter path of file within an app, and get the
// path of the app. Returns empty string if not within
// an app.
QString extractAppPath(const QString& path) {
    QFileInfo info(path);
    if (info.canonicalFilePath().contains(".app")) {
        QDir path = info.absoluteDir();
        while (!path.isRoot() && !path.absolutePath().endsWith(".app")) {
            path.cdUp();
        }
        if (path.isRoot()) {
            qDebug() << "Failed to find .app name.";
            return "";
        }
        return path.absolutePath();
    }
    return "";
}
}

namespace Sys {
QString archiveName()
{
    return "macos-amd64.zip";
}

QString defaultInstallPath()
{
    return QDir::homePath() +  "/Games/Unvanquished";
}

QString executableName()
{
    return "Unvanquished.app";
}

bool install()
{
    Settings settings;
    QFile::link(settings.installPath() + "/Unvanquished.app",
	            QDir::homePath() + "/Applications/Unvanquished.app");
    return true;
}

bool updateUpdater(const QString& updaterArchive)
{
    QString currentAppPath = extractAppPath(QCoreApplication::applicationFilePath());
    if (currentAppPath.isEmpty()) return false;
    QDir currentApp(currentAppPath);
    QDir backupUpdater(currentAppPath +  ".bak");
    if (backupUpdater.exists()) {
        if (!backupUpdater.removeRecursively()) {
            qDebug() << "Error deleting old update. Please update manually";
            return false;
        }
    }
    QDir extractDir = backupUpdater;
    extractDir.cdUp();
    if (!extractDir.rename(currentApp.dirName(), backupUpdater.dirName())) {
        qDebug() << "Could not rename backup. Please manually update.";
        return false;
    }
    QStringList extractedFiles = JlCompress::extractDir(updaterArchive, extractDir.absolutePath());
    if (extractedFiles.size() < 1) {
        return false;
    }
    // Extract the .app name.
    // Since it's an .app file, it must contain an Info.plist. Use
    // this file to determine the .app name in the zip.
    // NOTE: We could have hard coded this, but there is no guarantee
    // that the person running this app is running the app that has
    // the same name as the one in the zip.
    constexpr char kFileInApp[] = "Info.plist";
    QStringList infos = extractedFiles.filter(kFileInApp);
    if (infos.size() < 1) {
        qDebug() << "Error finding " << kFileInApp;
        return false;
    }

    QString extractedAppPath = "";
    for (int i = 0; i < infos.size() && extractedAppPath.isEmpty(); ++i) {
        extractedAppPath = extractAppPath(infos[i]);
    }
    if (extractedAppPath.isEmpty()) return false;
    QDir extractedApp(extractedAppPath);
    if (extractedApp.dirName() != currentApp.dirName()) {
        if (!extractDir.rename(extractedApp.dirName(), currentApp.dirName())) {
            qDebug() << "Could not rename update. Please manually update.";
            return false;
        }
    }
    // TODO: Maybe don't use system? startDetached didn't work for me...
    int i = system((QString("/usr/bin/open -F -n \"%1\"").arg(currentAppPath)).toStdString().c_str());
    if (i != 0) {
        qDebug() << "Error launching new updater. Please try manually.";
        return false;
    }
    QCoreApplication::quit();
    return true;
}

QString updaterArchiveName()
{
    return "UnvUpdaterOSX.zip";
}

std::string getCertStore()
{
    return "";  // Not used on OSX.
}

// Settings are stored in "~/Library/Preferences/net.unvanquished.Unvanquished Updater.plist"
// After deleting/changing the file, you must run `sudo killall cfprefsd` for it to take effect
QSettings* makePersistentSettings(QObject* parent)
{
    return new QSettings(parent);
}

}  // namespace Sys
