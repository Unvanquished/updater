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

#include <QDebug>

#include "ariadownloader.h"
#include "system.h"

// libaria2 thread safety note:
// "Please keep in mind that only one Session object can be allowed per process due to the heavy use
// of static objects in aria2 code base. Session object is not safe for concurrent accesses from
// multiple threads. It must be used from one thread at a time. In general, libaria2 is not entirely
// thread-safe."

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
