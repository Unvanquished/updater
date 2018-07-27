#include <string>

#include <QString>

namespace Sys {
QString archiveName();
QString defaultInstallPath();
QString executableName();
bool install();
bool updateUpdater(const QString& updaterArchive);
QString updaterArchiveName();
std::string getCertStore();
}
