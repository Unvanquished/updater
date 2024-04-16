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

#ifndef QMLDOWNLOADER_H
#define QMLDOWNLOADER_H

#include <QObject>
#include <QRegularExpression>
#include <QSettings>
#include <QThread>
#include <QUrl>
#include <QTemporaryDir>

#include <memory>
#include <chrono>

#include "currentversionfetcher.h"
#include "downloadworker.h"
#include "downloadtimecalculator.h"
#include "settings.h"

void StartGame(const Settings& settings, bool failIfWindowsAdmin);

class QmlDownloader : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int downloadSpeed READ downloadSpeed NOTIFY downloadSpeedChanged)
    Q_PROPERTY(int uploadSpeed READ uploadSpeed NOTIFY uploadSpeedChanged)
    Q_PROPERTY(int eta READ eta NOTIFY etaChanged)
    Q_PROPERTY(int totalSize READ totalSize NOTIFY totalSizeChanged)
    Q_PROPERTY(int completedSize READ completedSize NOTIFY completedSizeChanged)
    Q_PROPERTY(DownloadState state READ state NOTIFY stateChanged)

public:
    enum DownloadState {
        IDLE,
        DOWNLOADING,
        PAUSED,
        COMPLETED
    };
    Q_ENUM(DownloadState)

    QmlDownloader();
    ~QmlDownloader();
    int downloadSpeed() const;
    int uploadSpeed() const;
    int eta() const;
    int totalSize() const;
    int completedSize() const;
    DownloadState state() const;
    void checkForUpdate();
    void forceUpdaterUpdate(const QString& version);
    void forceGameUpdate();

    QString ariaLogFilename_;

signals:
    void downloadSpeedChanged(int downloadSpeed);
    void uploadSpeedChanged(int uploadSpeed);
    void etaChanged(int eta);
    void totalSizeChanged(int totalSize);
    void completedSizeChanged(int completedSize);
    void statusMessage(QString message);
    void fatalMessage(QString message);
    void updateNeeded(bool updateNeeded);
    void stateChanged(DownloadState state);

public slots:
    void setDownloadSpeed(int speed);
    void setUploadSpeed(int speed);
    void setTotalSize(int size);
    void setCompletedSize(int size);
    void onDownloadEvent(int event);
    void onCurrentVersions(QString updater, QString game);

    Q_INVOKABLE void toggleDownload(QString installPath);
    Q_INVOKABLE void startGame();
    Q_INVOKABLE void autoLaunchOrUpdate();
    Q_INVOKABLE bool relaunchForSettings();

private:
    void stopAria();
    void setState(DownloadState state);
    void startDownload(const QUrl& url, const QDir& destination);
    void startUpdate(const QString& selectedInstallPath);
    void launchGameIfInstalled();

    QThread thread_;
    int downloadSpeed_;
    int uploadSpeed_;
    std::chrono::seconds eta_;
    int totalSize_;
    int completedSize_;

    CurrentVersionFetcher fetcher_;
    DownloadWorker* worker_;
    DownloadTimeCalculator downloadTime_;
    Settings settings_;
    bool forceUpdaterUpdate_;
    bool forceGameUpdate_;
    QString latestGameVersion_;
    QString latestUpdaterVersion_;
    DownloadState state_;
    std::unique_ptr<QTemporaryDir> temp_dir_;

};

#endif // QMLDOWNLOADER_H
