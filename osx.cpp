#include "system.h"
#include "settings.h"
#include <QDir>
#include <QFile>

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
}
