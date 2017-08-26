#ifndef ARIADOWNLOADER_H
#define ARIADOWNLOADER_H

#include <list>

#include "ret.h"
#include "aria2/src/includes/aria2/aria2.h"

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

    Ret addUri(const std::string& uri);
    Ret run(void);
    Ret toggleDownloads(void);
    Ret setDownloadDirectory(const std::string& dir);
    Ret registerCallback(DownloadCallback* callback_);
    Ret unregisterCallback(DownloadCallback* callback_);
    DownloadCallback* callback(void);
    Ret updateStats(void);
    int downloadSpeed(void);
    int uploadSpeed(void);
    int completedSize(void);
    int totalSize(void);

private:
    aria2::Session* session;
    DownloadCallback* callback_;
    std::list<aria2::A2Gid> pausedGids_;
    int downloadSpeed_;
    int uploadSpeed_;
    int completedSize_;
    int totalSize_;
};

#endif // ARIADOWNLOADER_H
