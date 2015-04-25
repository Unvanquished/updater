#include "system.h"
#include <QDir>

namespace Sys {
QString getArchiveName(void)
{
    return "linux64.zip";
}

QString getDefaultInstallPath(void)
{
    return QDir::homePath() + "/.local/share/unvanquished";
}

QString getExecutableName(void)
{
    return "daemon";
}
}
