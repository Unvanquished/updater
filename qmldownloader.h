#ifndef QMLDOWNLOADER_H
#define QMLDOWNLOADER_H

#include <QObject>
#include <QRegularExpression>
#include <QSettings>
#include <QNetworkConfigurationManager>
#include <QThread>

#include <memory>
#include <chrono>

#include "currentversionfetcher.h"
#include "downloadworker.h"
#include "downloadtimecalculator.h"
#include "settings.h"

class QmlDownloader : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int downloadSpeed READ downloadSpeed NOTIFY downloadSpeedChanged)
    Q_PROPERTY(int uploadSpeed READ uploadSpeed NOTIFY uploadSpeedChanged)
    Q_PROPERTY(int eta READ eta NOTIFY etaChanged)
    Q_PROPERTY(int totalSize READ totalSize NOTIFY totalSizeChanged)
    Q_PROPERTY(int completedSize READ completedSize NOTIFY completedSizeChanged)

public:
    int downloadSpeed() const;
    int uploadSpeed() const;
    int eta() const;
    int totalSize() const;
    int completedSize() const;

signals:
    void downloadSpeedChanged(int downloadSpeed);
    void uploadSpeedChanged(int uploadSpeed);
    void etaChanged(int eta);
    void totalSizeChanged(int totalSize);
    void completedSizeChanged(int completedSize);
    void statusMessage(QString message);
    void updateNeeded(bool updateNeeded);
    void updateComplete();

public slots:
    void setDownloadSpeed(int speed);
    void setUploadSpeed(int speed);
    void setTotalSize(int size);
    void setCompletedSize(int size);
    void onDownloadEvent(int event);
    void onCurrentVersion(QString version);

    Q_INVOKABLE void startUpdate();
    Q_INVOKABLE void toggleDownload();
    Q_INVOKABLE void startGame();
    Q_INVOKABLE void checkForUpdate();

private:
    void stopAria();

    QThread thread_;
    int downloadSpeed_;
    int uploadSpeed_;
    std::chrono::seconds eta_;
    int totalSize_;
    int completedSize_;
    bool paused_;

    CurrentVersionFetcher fetcher_;
    DownloadWorker* worker_;
    DownloadTimeCalculator downloadTime_;
    Settings settings_;
    QNetworkConfigurationManager networkManager_;
    QString currentVersion_;

};

#endif // QMLDOWNLOADER_H
