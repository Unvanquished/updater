#ifndef SYSTEM_H_
#define SYSTEM_H_

#include <string>

#include <QSettings>
#include <QString>

namespace Sys {
QString archiveName();
QString defaultInstallPath();
bool validateInstallPath(const QString& installPath); // Checks installing as root in homepath on Linux
bool installShortcuts(); // Install launch menu entries and protocol handlers
bool installUpdater(const QString& installPath); // Copies current application to <install path>/updater[.exe|.app]
bool updateUpdater(const QString& updaterArchive);
QString updaterArchiveName();
std::string getCertStore();
QSettings* makePersistentSettings(QObject* parent);
QString getGameCommand(const QString& installPath); // Substitution for %command%
bool startGame(const QString& commandLine);

// Windows: relaunch with UAC elevation if necessary
// Other platforms always return UNNEEDED
enum class ElevationResult {
    UNNEEDED,
    RELAUNCHED,
    FAILED,
};
ElevationResult RelaunchElevated(const QString& flags);

inline QString QuoteQProcessCommandArgument(QString arg)
{
    arg.replace('"', "\"\"\"");
    return '"' + arg + '"';
}

}

#endif // SYSTEM_H_
