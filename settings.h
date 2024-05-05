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

#ifndef SETTINGS_H
#define SETTINGS_H
#include <QObject>
#include <QSettings>
#include <QString>

class Settings : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString installPath READ installPath WRITE setInstallPath NOTIFY installPathChanged)
    Q_PROPERTY(QString commandLine READ commandLine WRITE setCommandLine NOTIFY commandLineChanged)
public:
    Settings();

    QString installPath() const;
    QString commandLine() const;
    // installedVersion should be a non-empty iff there is a usable installation at installPath
    QString installedVersion() const;

    void setInstallPath(const QString& installPath);
    void setCommandLine(const QString& commandLine);
    void setInstalledVersion(const QString& installedVersion);
    void sync();
    QSettings::Status testWrite();

signals:
    // QML property binding signals (configured with NOTIFY)
    void installPathChanged(QString installPath);
    void commandLineChanged(QString commandLine);

private:
    QSettings* settings_;
};


#endif // SETTINGS_H
