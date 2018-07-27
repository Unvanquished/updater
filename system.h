#ifndef SYSTEM_H_
#define SYSTEM_H_

#include <string>

#include <QSettings>
#include <QString>

namespace Sys {
QString archiveName();
QString defaultInstallPath();
QString executableName();
bool install();
bool updateUpdater(const QString& updaterArchive);
QString updaterArchiveName();
std::string getCertStore();
QSettings* makePersistentSettings(QObject* parent);
}

#endif // SYSTEM_H_
