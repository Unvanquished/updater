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

#include "settings.h"
#include <QRandomGenerator>
#include "system.h"

const char kDefaultCommand[] = "%command%";

Settings::Settings() : QObject(nullptr), settings_(Sys::makePersistentSettings(this)) {
    if (!settings_->contains(Settings::INSTALL_PATH)) {
        setInstallPath(Sys::defaultInstallPath());
    }
    if (!settings_->contains(Settings::COMMAND_LINE)) {
        setCommandLine(kDefaultCommand);
    }
}

QString Settings::installPath() const {
    return settings_->value(Settings::INSTALL_PATH).toString();
}

QString Settings::commandLine() const {
    return settings_->value(Settings::COMMAND_LINE).toString();
}

QString Settings::currentVersion() const {
    return settings_->value(Settings::CURRENT_VERSION).toString();
}

void Settings::setInstallPath(const QString& installPath) {
    settings_->setValue(Settings::INSTALL_PATH, installPath);
    emit installPathChanged(installPath);
}

void Settings::setCommandLine(const QString& commandLine) {
    settings_->setValue(Settings::COMMAND_LINE, commandLine);
    emit commandLineChanged(commandLine);
}

void Settings::setCurrentVersion(const QString& currentVersion) {
    settings_->setValue(Settings::CURRENT_VERSION, currentVersion);
    emit currentVersionChanged(currentVersion);
}

void Settings::sync()
{
    settings_->sync();
}

// This is a hack to detect whether the program is running as administrator on Windows,
// by testing whether it can write in HKEY_LOCAL_MACHINE in the registry
QSettings::Status Settings::testWrite()
{
    QString randomData = QString::number(QRandomGenerator::global()->generate64());
    settings_->setValue(WRITE_PROBE, randomData);
    sync();
    return settings_->status();
}
