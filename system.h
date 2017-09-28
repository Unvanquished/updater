#include <string>

#include <QString>

namespace Sys {
QString archiveName(void);
QString defaultInstallPath(void);
QString executableName(void);
bool install(void);
bool updateUpdater(const QString& updaterArchive);
QString updaterArchiveName(void);
std::string getCertStore(void);
}
