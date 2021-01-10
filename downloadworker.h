#ifndef DOWNLOADWORKER_H
#define DOWNLOADWORKER_H

#include "ariadownloader.h"
#include <atomic>
#include <QObject>
#include <QRegularExpression>
#include <QStandardItemModel>

class DownloadWorker : public QObject, public AriaDownloader::DownloadCallback
{
    Q_OBJECT
public:
    enum Events {
        ERROR_EXTRACTING = 100
    };

    explicit DownloadWorker(QString ariaLogFilename, QObject *parent = 0);
    ~DownloadWorker();
    void onDownloadCallback(aria2::Session* session, aria2::DownloadEvent event,
                            aria2::A2Gid gid, void* userDataevent);
    void addUpdaterUri(const std::string& uri);
    void addTorrent(const std::string& uri);
    void setDownloadDirectory(const std::string& dir);
    void toggle();
    void stop();

public slots:
    void download();

signals:
    void downloadSpeedChanged(int speed);
    void uploadSpeedChanged(int speed);
    void totalSizeChanged(int size);
    void completedSizeChanged(int size);
    void onDownloadEvent(int event);

private:
    void setDownloadPathAndFiles(aria2::Session* session, aria2::A2Gid gid);
    bool extractUpdate();
    std::string getAriaIndexOut(size_t index, std::string path);

    enum State {
        IDLE,
        DOWNLOADING_TORRENT,
        DOWNLOADING_UPDATER,
        DOWNLOADING_UNVANQUISHED
    };

    int downloadSpeed;
    int uploadSpeed;
    int totalSize;
    int completedSize;
    bool paused;
    State state;
    std::atomic_bool running;
    QRegularExpression renameRegex;
    AriaDownloader downloader;
    QString downloadDir;
};

#endif // DOWNLOADWORKER_H
