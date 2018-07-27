#ifndef ARIADOWNLOADER_H
#define ARIADOWNLOADER_H

#include "aria2/src/includes/aria2/aria2.h"
#include <list>

class AriaDownloader
{
public:
    AriaDownloader();
    ~AriaDownloader();

    class DownloadCallback {
    public:
        virtual void onDownloadCallback(aria2::Session* session, aria2::DownloadEvent event,
                                        aria2::A2Gid gid, void* userData) = 0;
    };

    bool addUri(const std::string& uri);
    bool run();
    void toggleDownloads();
    void setDownloadDirectory(const std::string& dir);
    void registerCallback(DownloadCallback* callback_);
    void unregisterCallback(DownloadCallback* callback_);
    DownloadCallback* callback();
    void updateStats();
    int downloadSpeed();
    int uploadSpeed();
    int completedSize();
    int totalSize();

private:
    aria2::Session* session_;
    DownloadCallback* callback_;
    std::list<aria2::A2Gid> pausedGids_;
    int downloadSpeed_;
    int uploadSpeed_;
    int completedSize_;
    int totalSize_;
};

#endif // ARIADOWNLOADER_H
