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

#include "gamelauncher.h"

#include <QDebug>
#include <QMessageBox>
#include <QRegularExpression>

#include "system.h"

static const QRegularExpression COMMAND_REGEX("%command%");

GameLauncher::GameLauncher(const QString& connectUrl, const Settings& settings)
    : connectUrl_(connectUrl), settings_(settings) {}

// Does our version of daemon have -connect-trusted?
static bool haveConnectTrusted(const QString& gameVersion)
{
    // Updater version up to v0.2.0 may set "unknown" as game version if current.json request fails
    if (gameVersion == "unknown")
        return false;
    // Hacky string comparison, assume we won't go down to 0.9 or up to 0.100 :)
    return gameVersion > "0.54.1";
}

// Using the connect URL is optional because the updater intentionally "forgets about" the URL
// if you go to the install screen.
void GameLauncher::startGame(bool useConnectUrl, bool failIfWindowsAdmin)
{
    useConnectUrl = useConnectUrl && !connectUrl_.isEmpty();
    QString gameCommand = Sys::getGameCommand(settings_.installPath());
    QString commandLine;
    if (useConnectUrl && !haveConnectTrusted(settings_.installedVersion())) {
        // Behave for now as the old protocol handler which ignores the custom command
        commandLine = gameCommand + " -connect " + connectUrl_;
    } else {
        commandLine = settings_.commandLine().trimmed();
        if (!commandLine.contains(COMMAND_REGEX)) {
            commandLine = "%command% " + commandLine;
        }
        if (useConnectUrl) {
            gameCommand = gameCommand + " -connect-trusted " + connectUrl_;
        }
        commandLine.replace(COMMAND_REGEX, gameCommand);
    }
    qDebug() << "Starting game with command line:" << commandLine;
    QString error = Sys::startGame(commandLine, failIfWindowsAdmin, connectUrl_);
    if (error.isEmpty()) {
        qDebug() << "Game started successfully";
    } else {
        qDebug() << "Failed to start Unvanquished process:" << error;
        QMessageBox errorMessageBox;
        errorMessageBox.setText("Failed to start Unvanquished process: " + error);
        errorMessageBox.exec();
    }
}
