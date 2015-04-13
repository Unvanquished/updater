#include "system.h"
#include <QDir>

namespace Sys {
QString getArchiveName(void)
{
    return "mac.zip";
}

QString getDefaultInstallPath(void)
{
    return QDir::homePath() +  "/Application Support/Library/Unvanquished";
}
}
