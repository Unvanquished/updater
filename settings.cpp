#include "settings.h"

QString Settings::installPath() const {
    return settings_.value(Settings::INSTALL_PATH).toString();
}

QString Settings::commandLine() const {
    return settings_.value(Settings::COMMAND_LINE).toString();
}

QString Settings::currentVersion() const {
    return settings_.value(Settings::CURRENT_VERSION).toString();
}

bool Settings::installFinished() const {
    return settings_.value(Settings::INSTALL_FINISHED, false).toBool();
}

void Settings::setInstallPath(const QString& installPath) {
    // If we changed the download path, make sure we don't start the game.
    if (installPath != this->installPath()) {
        setInstallFinished(false);
    }
    settings_.setValue(Settings::INSTALL_PATH, installPath);
    emit installPathChanged(installPath);
}

void Settings::setCommandLine(const QString& commandLine) {
    settings_.setValue(Settings::COMMAND_LINE, commandLine);
    emit commandLineChanged(commandLine);
}

void Settings::setCurrentVersion(const QString& currentVersion) {
    settings_.setValue(Settings::CURRENT_VERSION, currentVersion);
    emit currentVersionChanged(currentVersion);
}

void Settings::setInstallFinished(bool installFinished) {
    settings_.setValue(Settings::INSTALL_FINISHED, installFinished);
    emit installFinishedChanged(installFinished);
}


