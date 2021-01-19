#include <chrono>
#include <sstream>
#include "downloadworker.h"
#include "quazip/quazip/JlCompress.h"
#include "settings.h"
#include "system.h"
#include <QDebug>

DownloadWorker::DownloadWorker(QString ariaLogFilename, QObject *parent) :
    QObject(parent), downloadSpeed(0), uploadSpeed(0),
    totalSize(0), completedSize(0), paused(true), state(IDLE), running(false),
    renameRegex(".*unvanquished_([0-9\\.]+/)"), downloader(ariaLogFilename.toStdString())
{
    downloader.registerCallback(this);
}

DownloadWorker::~DownloadWorker()
{
    downloader.unregisterCallback(this);
}

void DownloadWorker::addUpdaterUri(const std::string& uri)
{
    downloader.addUri(uri);
    state = DOWNLOADING_UPDATER;
}

void DownloadWorker::addTorrent(const std::string& uri)
{
    downloader.addUri(uri);
    state = DOWNLOADING_TORRENT;
}

void DownloadWorker::onDownloadCallback(aria2::Session* session, aria2::DownloadEvent event,
                                        aria2::A2Gid gid, void* /* userData */)
{
    switch (event) {
        case aria2::EVENT_ON_BT_DOWNLOAD_COMPLETE:
            qDebug() << "onDownloadCallback event BT_DOWNLOAD_COMPLETE";
            if (!extractUpdate()) return;
            break;

        case aria2::EVENT_ON_DOWNLOAD_COMPLETE:
            qDebug() << "onDownloadCallback event DOWNLOAD_COMPLETE";
            if (state == DOWNLOADING_TORRENT) {
                qDebug() << "DownloadWorker.state changed from DOWNLOADING_TORRENT to DOWNLOADING_UNVANQUISHED";
                state = DOWNLOADING_UNVANQUISHED;
                aria2::DownloadHandle* handle = aria2::getDownloadHandle(session, gid);
                aria2::A2Gid torrentGid = handle->getFollowedBy()[0];
                setDownloadPathAndFiles(session, torrentGid);
                aria2::deleteDownloadHandle(handle);
            } else if (state == DOWNLOADING_UPDATER) {
                qDebug() << "Updater download complete";
                aria2::DownloadHandle* handle = aria2::getDownloadHandle(session, gid);
                qDebug() << "Number of files in updater download:" << handle->getNumFiles();
                if (handle->getNumFiles() != 1) {
                    return;
                }
                auto files = handle->getFiles();
                qDebug() << "Downloaded updater at" << files[0].path.c_str();
                Sys::updateUpdater(QString(files[0].path.c_str()));
                return;
            } else {
                // For a torrent, happens when aria2 is stopped
                qDebug() << "final EVENT_ON_DOWNLOAD_COMPLETE";
                return;
            }
            break;

        case aria2::EVENT_ON_DOWNLOAD_ERROR:
            qDebug() << "onDownloadCallback event DOWNLOAD_ERROR";
            break;

        case aria2::EVENT_ON_DOWNLOAD_PAUSE:
            qDebug() << "onDownloadCallback event DOWNLOAD_PAUSE";
            break;

        case aria2::EVENT_ON_DOWNLOAD_START:
            qDebug() << "onDownloadCallback event DOWNLOAD_START";
            break;

        case aria2::EVENT_ON_DOWNLOAD_STOP:
            qDebug() << "onDownloadCallback event DOWNLOAD_STOP";
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
    bool ret = true;
    paused = false;
    running = true;
    while (ret && running) {
        ret = downloader.run();
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
    downloader.toggleDownloads();
}

void DownloadWorker::setDownloadDirectory(const std::string& dir)
{
    downloadDir = dir.c_str();
    downloader.setDownloadDirectory(dir);
}

void DownloadWorker::stop()
{
    running = false;
}

bool DownloadWorker::extractUpdate()
{
    qDebug() << "Clearing installed version prior to extraction";
    Settings().setCurrentVersion("");
    QString filename = Sys::archiveName();
    auto out = JlCompress::extractDir(downloadDir + "/" + filename, downloadDir);
    if (out.size() < 1) {
        emit onDownloadEvent(ERROR_EXTRACTING);
        return false;
    }
    return true;
}
