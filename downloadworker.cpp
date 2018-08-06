#include "downloadworker.h"

#include <chrono>
#include <sstream>

#include <QDebug>

#include "quazip/quazip/JlCompress.h"
#include "system.h"
#include "ret.h"
#include <QDebug>

DownloadWorker::DownloadWorker(QObject *parent) : QObject(parent), downloadSpeed(0), uploadSpeed(0),
totalSize(0), completedSize(0), paused(true), state(IDLE), running(false), renameRegex(".*unvanquished_([0-9\\.]+/)")
{
    downloader.registerCallback(this);
}

DownloadWorker::~DownloadWorker()
{
    downloader.unregisterCallback(this);
}

Ret DownloadWorker::addUpdaterUri(const std::string& uri)
{
    Ret ret = downloader.addUri(uri);
    if (!ret.ok()) return ret;
    state = DOWNLOADING_UPDATER;
    return true;
}

Ret DownloadWorker::addTorrent(const std::string& uri)
{
    Ret ret = downloader.addUri(uri);
    if (!ret.ok()) return ret;
    state = DOWNLOADING_TORRENT;
    return true;
}

void DownloadWorker::onDownloadCallback(aria2::Session* session, aria2::DownloadEvent event,
                                        aria2::A2Gid gid, void* /* userData */)
{
    Ret ret;
    switch (event) {
        case aria2::EVENT_ON_BT_DOWNLOAD_COMPLETE:
            if (!extractUpdate()) return;
            break;

        case aria2::EVENT_ON_DOWNLOAD_COMPLETE:
            if (state == DOWNLOADING_TORRENT) {
                state = DOWNLOADING_UNVANQUISHED;
                aria2::DownloadHandle* handle = aria2::getDownloadHandle(session, gid);
                aria2::A2Gid torrentGid = handle->getFollowedBy()[0];
                setDownloadPathAndFiles(session, torrentGid);
                aria2::deleteDownloadHandle(handle);
            } else if (state == DOWNLOADING_UPDATER) {
                aria2::DownloadHandle* handle = aria2::getDownloadHandle(session, gid);
                qDebug() << handle->getNumFiles();
                if (handle->getNumFiles() > 1) {
                    return;
                }
                auto files = handle->getFiles();
                qDebug() << files[0].path.c_str();
                Sys::updateUpdater(QString(files[0].path.c_str()));
                return;
            } else {
                event = aria2::EVENT_ON_BT_DOWNLOAD_COMPLETE;
                if (!extractUpdate()) return;
            }
            break;

        case aria2::EVENT_ON_DOWNLOAD_ERROR:
            break;

        case aria2::EVENT_ON_DOWNLOAD_PAUSE:
            break;

        case aria2::EVENT_ON_DOWNLOAD_START:
            break;

        case aria2::EVENT_ON_DOWNLOAD_STOP:
            break;
    }
    emit onDownloadEvent(event);
}

void DownloadWorker::setDownloadPathAndFiles(aria2::Session* session, aria2::A2Gid gid)
{
    aria2::KeyVals options;
    aria2::DownloadHandle* handle = aria2::getDownloadHandle(session, gid);
    std::vector<aria2::FileData> files = handle->getFiles();
    for (size_t i = 0; i < files.size(); ++i) {
        options.push_back({ "index-out", getAriaIndexOut(i + 1, files[i].path) });
    }
    aria2::changeOption(session, gid, options);
    aria2::deleteDownloadHandle(handle);
}

std::string DownloadWorker::getAriaIndexOut(size_t index, std::string path)
{
    QString oldPath(path.c_str());
    oldPath.replace(renameRegex, "");
    return std::to_string(index) + "=" + oldPath.toStdString();
}



void DownloadWorker::download()
{
    auto start = std::chrono::steady_clock::now();
    paused = false;
    running = true;
    while (running) {
        Ret ret = downloader.run();
        if (!ret.ok()) {
            qDebug() << "Download loop failed: " << ret.msg();
            break;
        }
        auto now = std::chrono::steady_clock::now();

        // Print progress information once per 500ms
        if(now - start > std::chrono::milliseconds(500)) {
            start = now;
            downloader.updateStats();

            if (totalSize != downloader.totalSize()) {
                totalSize = downloader.totalSize();
                emit totalSizeChanged(totalSize);
            }
            if (downloadSpeed != downloader.downloadSpeed()) {
                downloadSpeed = downloader.downloadSpeed();
                emit downloadSpeedChanged(downloadSpeed);
            }
            if (uploadSpeed != downloader.uploadSpeed()) {
                uploadSpeed = downloader.uploadSpeed();
                emit uploadSpeedChanged(uploadSpeed);
            }
            if (completedSize != downloader.completedSize()) {
                completedSize = downloader.completedSize();
                emit completedSizeChanged(completedSize);
            }
        }
    }
}

void DownloadWorker::toggle()
{
    return downloader.toggleDownloads();
}

Ret DownloadWorker::setDownloadDirectory(const std::string& dir)
{
    Ret ret = downloader.setDownloadDirectory(dir);
    if (!ret.ok()) return ret;
    downloadDir = dir.c_str();
}

void DownloadWorker::stop()
{
    running = false;
}

Ret DownloadWorker::extractUpdate()
{
    QString filename = Sys::archiveName();
    auto out = JlCompress::extractDir(downloadDir + "/" + filename, downloadDir);
    if (out.size() < 1) {
        return Ret(false) << "Error extracting " << filename << " to " << downloadDir;
    }
    return true;
}
