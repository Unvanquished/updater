#include "system.h"
#include "settings.h"
#include <QDir>
#include <QFile>
#include <QSettings>

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
	QSettings settings;
	QFile::link(settings.value(Settings::INSTALL_PATH).toString() + "/Unvanquished.app",
	            QDir::homePath() + "/Applications/Unvanquished.app");
    return true;
}
}
