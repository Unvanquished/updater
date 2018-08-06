#include <string>

#include <QString>

#include "ret.h"

namespace Sys {
QString archiveName(void);
QString defaultInstallPath(void);
QString executableName(void);
Ret install(void);
Ret updateUpdater(const QString& updaterArchive);
QString updaterArchiveName(void);
std::string getCertStore(void);
}
