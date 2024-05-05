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

#include "splashcontroller.h"

#include <QCoreApplication>
#include <QDebug>

#include "system.h"

SplashController::SplashController(
    RelaunchCommand command, const QString& updateUpdaterVersion,
    const QString& connectUrl, const Settings& settings) :
        relaunchCommand_(command), updateUpdaterVersion_(updateUpdaterVersion),
        connectUrl_(connectUrl), settings_(settings) {}

// Possibly initiates an asynchronous request for the latest available versions.
void SplashController::checkForUpdate()
{
    // Don't need versions.json if we already know what to do next
    if (relaunchCommand_ != RelaunchCommand::NONE) {
        return;
    }

    connect(&fetcher_, SIGNAL(onCurrentVersions(QString, QString)),
            this, SLOT(onCurrentVersions(QString, QString)));
    fetcher_.fetchCurrentVersion("https://dl.unvanquished.net/versions.json");
}

// Receives the results of the checkForUpdate request.
void SplashController::onCurrentVersions(QString updater, QString game)
{
    latestUpdaterVersion_ = updater;
    latestGameVersion_ = game;
}

// Return value is whether the program should exit
bool SplashController::relaunchForSettings()
{
    qDebug() << "Possibly relaunching to open settings window";
    return Sys::RelaunchElevated("--splashms 1 --internalcommand updategame")
        != Sys::ElevationResult::UNNEEDED;
}

void SplashController::launchGameIfInstalled()
{
    if (settings_.installedVersion().isEmpty()) {
        qDebug() << "No game installed, exiting";
        QCoreApplication::quit();
    } else {
        qDebug() << "Fall back to launching installed game";
        emit updateNeeded(false);
    }
}

// This runs after the splash screen has been displayed for the programmed amount of time (and the
// user did not click the settings button). If the CurrentVersionFetcher didn't emit anything yet,
// proceed as if the request for versions.json failed.
void SplashController::autoLaunchOrUpdate()
{
    qDebug() << "Previously-installed game version:" << settings_.installedVersion();

    switch (relaunchCommand_) {
        case RelaunchCommand::UPDATE_GAME:
            qDebug() << "Game update menu requested as relaunch action";
            // It is assumed the process is already elevated
            emit updateNeeded(true);
            return;

        case RelaunchCommand::UPDATE_UPDATER:
            qDebug() << "Updater update to" << updateUpdaterVersion_ << "requested as relaunch action";
            // It is assumed the process is already elevated
            emit updaterUpdate(updateUpdaterVersion_);
            return;

        case RelaunchCommand::PLAY_NOW:
            // Should never happen

        case RelaunchCommand::NONE:
            break;
    }

    // If no relaunch action, detect update needed based on versions.json
    if (!latestUpdaterVersion_.isEmpty() && latestUpdaterVersion_ != QString(GIT_VERSION)) {
        qDebug() << "Updater update to version" << latestUpdaterVersion_ << "required";
        // Remember the URL if we are doing updater update
        QString updaterArgs = "--splashms 1 --internalcommand updateupdater:" + latestUpdaterVersion_;
        if (!connectUrl_.isEmpty()) {
            updaterArgs += " -- " + connectUrl_;
        }
        switch (Sys::RelaunchElevated(updaterArgs)) {
            case Sys::ElevationResult::UNNEEDED:
                emit updaterUpdate(latestUpdaterVersion_);
                return;
            case Sys::ElevationResult::RELAUNCHED:
                QCoreApplication::quit();
                return;
            case Sys::ElevationResult::FAILED:
                launchGameIfInstalled();
                return;
        }
    } else if (settings_.installedVersion().isEmpty() ||
               (!latestGameVersion_.isEmpty() && settings_.installedVersion() != latestGameVersion_)) {
        qDebug() << "Game update required.";
        switch (Sys::RelaunchElevated("--splashms 1 --internalcommand updategame")) {
            case Sys::ElevationResult::UNNEEDED:
                emit updateNeeded(true);
                return;
            case Sys::ElevationResult::RELAUNCHED:
                QCoreApplication::quit();
                return;
            case Sys::ElevationResult::FAILED:
                launchGameIfInstalled();
                return;
        }
    } else {
        emit updateNeeded(false);
    }
}

