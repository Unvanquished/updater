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
    bool run(void);
    void toggleDownloads(void);
    void setDownloadDirectory(const std::string& dir);
    void registerCallback(DownloadCallback* callback_);
    void unregisterCallback(DownloadCallback* callback_);
    DownloadCallback* callback(void);
    void updateStats(void);
    int downloadSpeed(void);
    int uploadSpeed(void);
    int completedSize(void);
    int totalSize(void);

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
