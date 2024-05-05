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

// Registry/configuration file keys
const QString INSTALL_PATH = "settings/installPath";
const QString COMMAND_LINE = "settings/commandLineParameters";
const QString INSTALLED_VERSION = "settings/currentVersion";
// const QString INSTALL_FINISHED = "settings/installFinished";  // No longer used
const QString WRITE_PROBE = "writeProbe";

Settings::Settings() : QObject(nullptr), settings_(Sys::makePersistentSettings(this)) {
    if (!settings_->contains(INSTALL_PATH)) {
        setInstallPath(Sys::defaultInstallPath());
    }
    if (!settings_->contains(COMMAND_LINE)) {
        setCommandLine(kDefaultCommand);
    }
}

QString Settings::installPath() const {
    return settings_->value(INSTALL_PATH).toString();
}

QString Settings::commandLine() const {
    return settings_->value(COMMAND_LINE).toString();
}

QString Settings::installedVersion() const {
    return settings_->value(INSTALLED_VERSION).toString();
}

void Settings::setInstallPath(const QString& installPath) {
    settings_->setValue(INSTALL_PATH, installPath);
    emit installPathChanged(installPath);
}

void Settings::setCommandLine(const QString& commandLine) {
    settings_->setValue(COMMAND_LINE, commandLine);
    emit commandLineChanged(commandLine);
}

void Settings::setInstalledVersion(const QString& installedVersion) {
    settings_->setValue(INSTALLED_VERSION, installedVersion);
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
