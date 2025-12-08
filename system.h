/* Unvanquished Updater
 * Copyright (C) Unvanquished Developers
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef SYSTEM_H_
#define SYSTEM_H_

#include <string>

#include <QSettings>
#include <QString>

namespace Sys {
QString archiveName();
QString updaterSystem();
QString defaultInstallPath();
void initApplicationName(); // influences default storage location for QSettings
bool validateInstallPath(const QString& installPath); // Checks installing as root in homepath on Linux
bool installShortcuts(); // Install launch menu entries and protocol handlers
bool installUpdater(const QString& installPath); // Copies current application to <install path>/updater[.exe|.app]
bool updateUpdater(const QString& updaterArchive, const QString& connectUrl);
std::string getCertStore();
QSettings* makePersistentSettings(QObject* parent);
QString getGameCommand(const QString& installPath); // Substitution for %command%
// A connect URL is already included in command line but on Windows if the updater has to restart to
// lose admin elevation, it uses connectUrlForRestart
QString startGame(const QString& commandLine, bool failIfWindowsAdmin, const QString& connectUrlForRestart);

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
