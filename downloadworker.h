#ifndef DOWNLOADWORKER_H
#define DOWNLOADWORKER_H

#include <QObject>
#include "ariadownloader.h"

class DownloadWorker : public QObject, public AriaDownloader::DownloadCallback
{
    Q_OBJECT
public:
    explicit DownloadWorker(QObject *parent = 0);
    ~DownloadWorker();
    void onDownloadCallback(int event);
    void addUri(const std::string& uri);
    void toggle(void);

public slots:
    void download(void);

signals:
    void downloadSpeedChanged(int speed);
    void uploadSpeedChanged(int speed);
    void totalSizeChanged(int size);
    void completedSizeChanged(int size);
    void onDownloadEvent(int event);

private:
    AriaDownloader downloader;
    bool paused;
    int downloadSpeed;
    int uploadSpeed;
    int totalSize;
    int completedSize;
};

#endif // DOWNLOADWORKER_H
