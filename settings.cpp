#include "settings.h"
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
