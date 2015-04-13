#ifndef DOWNLOADWORKER_H
#define DOWNLOADWORKER_H

#include "ariadownloader.h"
#include <atomic>
#include <QObject>
#include <QRegularExpression>

class DownloadWorker : public QObject, public AriaDownloader::DownloadCallback
{
    Q_OBJECT
public:
    explicit DownloadWorker(QObject *parent = 0);
    ~DownloadWorker();
    void onDownloadCallback(aria2::Session* session, aria2::DownloadEvent event,
                            aria2::A2Gid gid, void* userDataevent);
    void addUri(const std::string& uri);
    void setDownloadDirectory(const std::string& dir);
    void toggle(void);
    void stop(void);

public slots:
    void download(void);

signals:
    void downloadSpeedChanged(int speed);
    void uploadSpeedChanged(int speed);
    void totalSizeChanged(int size);
    void completedSizeChanged(int size);
    void onDownloadEvent(int event);

private:
    void setDownloadPathAndFiles(aria2::Session* session, aria2::A2Gid gid);
    void extractUpdate(void);
    std::string getAriaIndexOut(size_t index, std::string path);

    enum State {
        IDLE,
        DOWNLOADING_TORRENT,
        DOWNLOADING_UNVANQUISHED
    };
    QRegularExpression regex;
    AriaDownloader downloader;
    QString downloadDir;
    State state;
    bool paused;
    std::atomic_bool running;
    int downloadSpeed;
    int uploadSpeed;
    int totalSize;
    int completedSize;
};

#endif // DOWNLOADWORKER_H
