#include "ariadownloader.h"
#include "system.h"

#include <QDebug>

int downloadEventCallback(aria2::Session* session, aria2::DownloadEvent event,
                          aria2::A2Gid gid, void* userData)
{
    AriaDownloader* downloader = static_cast<AriaDownloader*>(userData);
    if (downloader->callback()) {
        downloader->callback()->onDownloadCallback(session, event, gid, userData);
    }
    return 1;
}

AriaDownloader::AriaDownloader() : callback_(nullptr)
{
    aria2::libraryInit();
    aria2::SessionConfig config;
    config.keepRunning = true;
    config.useSignalHandler = false;
    config.userData = this;
    config.downloadEventCallback = downloadEventCallback;
    aria2::KeyVals options;
    options.push_back({ "check-integrity", "true" });
    options.push_back({ "seed-time", "0" });
    options.push_back({ "file-allocation", "none" });
    options.push_back({ "follow-torrent", "mem" });
    options.push_back({ "quiet", "false" });

    std::string certsPath = Sys::getCertStore();
    if (!certsPath.empty()) {
        options.push_back({ "ca-certificates", certsPath });
    }
    session = aria2::sessionNew(options, config);
}

AriaDownloader::~AriaDownloader()
{
    aria2::sessionFinal(session);
    aria2::libraryDeinit();
}

Ret AriaDownloader::addUri(const std::string& uri)
{
    int ret = aria2::addUri(session, nullptr, { uri }, aria2::KeyVals());
    if (ret < 0) {
        return Ret(QString("Error adding uri: %1").arg(QString(uri.c_str())));
    }
    return true;
}

Ret AriaDownloader::run(void)
{
    int ret = aria2::run(session, aria2::RUN_ONCE);
    if (ret < 0) {
        return Ret("Error running aria loop.");
    }
    return true;
}

Ret AriaDownloader::toggleDownloads(void)
{
    if (pausedGids_.empty()) {
        auto gids = aria2::getActiveDownload(session);
        for (aria2::A2Gid gid : gids) {
            int ret = aria2::pauseDownload(session, gid, true);
            if (ret < 0) {
                qDebug() << "WARNING: Error pausing gid: " << gid;
            } else {
                pausedGids_.push_back(gid);
            }
        }
    } else {
        while (!pausedGids_.empty()) {
            int ret = aria2::unpauseDownload(session, pausedGids_.front());
            if (ret < 0) {
                qDebug() << "WARNING: Error pausing gid: " << pausedGids_.front();
            } else {
                pausedGids_.pop_front();
            }
        }
    }
    return true;
}

Ret AriaDownloader::registerCallback(DownloadCallback* callback)
{
    callback_ = callback;
    return true;
}

Ret AriaDownloader::unregisterCallback(DownloadCallback* callback)
{
    if (callback_ == callback) {
        callback_ = nullptr;
        return true;
    }
    return Ret("Tried to unregister invalid callback.");
}

AriaDownloader::DownloadCallback* AriaDownloader::callback(void)
{
    return callback_;
}

Ret AriaDownloader::updateStats(void)
{
    std::vector<aria2::A2Gid> gids = aria2::getActiveDownload(session);
    for(const auto& gid : gids) {
        aria2::DownloadHandle* dh = aria2::getDownloadHandle(session, gid);
        if(!dh) {
            return Ret(QString("Invalid download handle for gid: %1").arg(gid));
        }
        downloadSpeed_ = dh->getDownloadSpeed();
        uploadSpeed_ = dh->getUploadSpeed();
        completedSize_ = dh->getCompletedLength();
        totalSize_ = dh->getTotalLength();
        aria2::deleteDownloadHandle(dh);
    }
    return true;
}

int AriaDownloader::downloadSpeed(void)
{
    return downloadSpeed_;
}

int AriaDownloader::uploadSpeed(void)
{
    return uploadSpeed_;
}

int AriaDownloader::completedSize(void)
{
    return completedSize_;
}

int AriaDownloader::totalSize(void)
{
    return totalSize_;
}

Ret AriaDownloader::setDownloadDirectory(const std::string& dir)
{
    int ret = aria2::changeGlobalOption(session, {{ "dir", dir }});
    if (ret < 0) {
        return Ret(QString("Error setting download directory to %1").arg(dir.c_str()));
    }
    return true;
}
