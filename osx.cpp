#include "system.h"
#include <QDir>

namespace Sys {
QString archiveName(void)
{
    return "mac.zip";
}

QString defaultInstallPath(void)
{
    return QDir::homePath() +  "/Application Support/Library/Unvanquished";
}

QString executableName(void)
{
    return "Unvanquished.app";
}
}
