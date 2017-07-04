#include "system.h"
#include "settings.h"
#include <QDir>

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

bool install(void)
{
    // Set up menu and protocol handler
    Settings settings;
    QFile desktopFile(":resources/unvanquished.desktop");
    if (!desktopFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }
    QString desktopStr = QString(desktopFile.readAll().data())
        .arg(settings.installPath());
    QFile outputFile(QDir::homePath() + "/.local/share/applications/unvanquished.desktop");
    if (!outputFile.open(QIODevice::WriteOnly | QIODevice::Text  QIODevice::Truncate)) {
        desktopFile.close();
        return false;
    }
    outputFile.write(desktopStr.toUtf8().constData(), desktopStr.size());
    outputFile.close();

    // install icon
    QString iconDir = QDir::homePath() + "/.local/share/icons/hicolor/128x128/apps/";
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

}  // namespace Sys
