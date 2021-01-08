#include <QDebug>

#include "ariadownloader.h"
#include "system.h"

int downloadEventCallback(aria2::Session* session, aria2::DownloadEvent event,
                          aria2::A2Gid gid, void* userData)
{
    AriaDownloader* downloader = static_cast<AriaDownloader*>(userData);
    if (downloader->callback()) {
        downloader->callback()->onDownloadCallback(session, event, gid, userData);
    }
    return 1;
}

AriaDownloader::AriaDownloader(const std::string& ariaLogFilename) : callback_(nullptr)
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

    if (!ariaLogFilename.empty()) {
        options.push_back({ "log", ariaLogFilename });
    }

    std::string certsPath = Sys::getCertStore();
    if (!certsPath.empty()) {
        options.push_back({ "ca-certificate", certsPath });
    }
    session_ = aria2::sessionNew(options, config);
}

AriaDownloader::~AriaDownloader()
{
    aria2::sessionFinal(session_);
    aria2::libraryDeinit();
}

bool AriaDownloader::addUri(const std::string& uri)
{
    int ret = aria2::addUri(session_, nullptr, { uri }, aria2::KeyVals());
    qDebug() << "aria2::addUri returned" << ret;
    return ret == 0;
}

bool AriaDownloader::run()
{
    int ret = aria2::run(session_, aria2::RUN_ONCE);
    if (ret != 1) {
        qDebug() << "aria2::run returned" << ret;
    }
    return ret == 1;
}

void AriaDownloader::toggleDownloads()
{
    if (pausedGids_.empty()) {
        qDebug() << "AriaDownloader::toggleDownloads - pausing";
        auto gids = aria2::getActiveDownload(session_);
        for (aria2::A2Gid gid : gids) {
            aria2::pauseDownload(session_, gid, true);
            pausedGids_.push_back(gid);
        }
    } else {
        qDebug() << "AriaDownloader::toggleDownloads - resuming";
        while (!pausedGids_.empty()) {
            aria2::unpauseDownload(session_, pausedGids_.front());
            pausedGids_.pop_front();
        }
    }
}

void AriaDownloader::registerCallback(DownloadCallback* callback)
{
    callback_ = callback;
}

void AriaDownloader::unregisterCallback(DownloadCallback* callback)
{
    // TODO: Add logging if this fails
    if (callback_ == callback) {
        callback_ = nullptr;
    }
}

AriaDownloader::DownloadCallback* AriaDownloader::callback()
{
    return callback_;
}

void AriaDownloader::updateStats()
{
    std::vector<aria2::A2Gid> gids = aria2::getActiveDownload(session_);
    for(const auto& gid : gids) {
        aria2::DownloadHandle* dh = aria2::getDownloadHandle(session_, gid);
        if(dh) {
            downloadSpeed_ = dh->getDownloadSpeed();
            uploadSpeed_ = dh->getUploadSpeed();
            completedSize_ = dh->getCompletedLength();
            totalSize_ = dh->getTotalLength();
            aria2::deleteDownloadHandle(dh);
        }
    }
}

int AriaDownloader::downloadSpeed()
{
    return downloadSpeed_;
}

int AriaDownloader::uploadSpeed()
{
    return uploadSpeed_;
}

int AriaDownloader::completedSize()
{
    return completedSize_;
}

int AriaDownloader::totalSize()
{
    return totalSize_;
}

void AriaDownloader::setDownloadDirectory(const std::string& dir)
{
    aria2::changeGlobalOption(session_, {{ "dir", dir }});
}
