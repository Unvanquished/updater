#include "system.h"
#include <QDir>

namespace Sys {
QString archiveName(void)
{
    return "linux64.zip";
}

QString defaultInstallPath(void)
{
    return QDir::homePath() + "/.local/share/unvanquished";
}

QString executableName(void)
{
    return "daemon";
}
}
