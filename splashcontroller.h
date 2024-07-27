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

#ifndef SPLASHCONTROLLER_H_
#define SPLASHCONTROLLER_H_

#include <QObject>
#include <QString>

#include "currentversionfetcher.h"
#include "settings.h"

// These are used only on Windows where relaunching is needed for admin (de)elevation.
enum class RelaunchCommand
{
    NONE,
    PLAY_NOW,
    UPDATE_GAME,
    UPDATE_UPDATER,
};

// The SplashController decides what to do after the splash screen has been displayed for the
// configured duration: update updater, go to game update, or start the game.
// (TODO better name?)
class SplashController : public QObject
{
    Q_OBJECT

private:
    // Actions from command line args
    RelaunchCommand relaunchCommand_;
    QString updateUpdaterUrl_; // If command is UPDATE_UPDATER
    QString connectUrl_; // for pre-updater-update elevation

    const Settings& settings_;

    // Latest versions fetching
    CurrentVersionFetcher fetcher_;
    QString latestUpdaterVersion_;
    QString latestUpdaterUrl_;
    QString latestGameVersion_;
    QString latestGameUrl_;
    QString latestNewsUrl_;

public:
    SplashController(
        RelaunchCommand command, const QString& updateUpdaterUrl,
        const QString& connectUrl, const Settings& settings);
    void checkForUpdate();
    QString gameUrl() const;
    QString newsUrl() const;
    Q_INVOKABLE bool relaunchForSettings();
    Q_INVOKABLE void autoLaunchOrUpdate();

signals:
    void updateNeeded(bool updateNeeded);
    void updaterUpdateNeeded();
    void updaterUpdate(QString updaterUrl);
    void newsUrlFetched(QString newsUrl);

private slots:
    void onCurrentVersions(QString updaterVersion, QString updaterUrl, QString gameVersion, QString gameUrl, QString newsUrl);

private:
    void launchGameIfInstalled();
};

#endif // SPLASHCONTROLLER_H_
