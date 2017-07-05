#include "system.h"
#include "settings.h"
#include "quazip/quazip/JlCompress.h"

#include <QDir>
#include <QFile>
#include <QDebug>
#include <QCoreApplication>

namespace Sys {
QString archiveName(void)
{
    return "mac.zip";
}

QString defaultInstallPath(void)
{
    return QDir::homePath() +  "/Games/Unvanquished";
}

QString executableName(void)
{
    return "Unvanquished.app";
}

bool install(void)
{
    Settings settings;
    QFile::link(settings.installPath() + "/Unvanquished.app",
	            QDir::homePath() + "/Applications/Unvanquished.app");
    return true;
}

bool updateUpdater(const QString& updaterArchive)
{
    QDir dir(QCoreApplication::applicationFilePath());
    if (!(dir.cdUp() && dir.cdUp() && dir.cdUp())) {
        qDebug() << "Error executing cdUp()!";
        return false;
    }
    QString name = dir.dirName();
    if (!name.endsWith(".app")) {
        qDebug() << "Expected app to be started from an OSX .app";
        return false;
    }
    if (!dir.cdUp()) {
        qDebug() << "Unexpected failure...";
        return false;
    }
    QDir oldUpdate(dir.absolutePath() + "/" + name + ".bak");
    if (oldUpdate.exists()) {
        if (!oldUpdate.removeRecursively()) {
            qDebug() << "Error deleting old update. pls update manually";
            return false;
        }
    }
    if (!dir.rename(name, oldUpdate.dirName())) {
        qDebug() << "Could not rename update. pls manually update.";
        return false;
    }
    qDebug() << "current=" << dir.absolutePath() << " update=" << update;
    auto out = JlCompress::extractDir(updaterArchive, dir.absolutePath());
    if (out.size() < 1) {
        return false;
    }
    return true;
}

QString updaterArchiveName(void)
{
    return "UnvUpdaterOSX.zip";
}

}  // namespace Sys
