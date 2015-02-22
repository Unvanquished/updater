#include <chrono>
#include <sstream>
#include "downloadworker.h"
#include "quazip/JlCompress.h"

DownloadWorker::DownloadWorker(QObject *parent) : QObject(parent), downloadSpeed(0), uploadSpeed(0),
    totalSize(0), completedSize(0), paused(true), state(IDLE), running(false)
{
    downloader.registerCallback(this);
}

DownloadWorker::~DownloadWorker()
{
    downloader.unregisterCallback(this);
}

void DownloadWorker::addUri(const std::string& uri)
{
    downloader.addUri(uri);
}

void DownloadWorker::onDownloadCallback(aria2::Session* session, aria2::DownloadEvent event,
                                        aria2::A2Gid gid, void* userData)
{
    switch (event) {
        case aria2::EVENT_ON_BT_DOWNLOAD_COMPLETE:
            extractUpdate();
            break;

        case aria2::EVENT_ON_DOWNLOAD_COMPLETE:
            if (state == DOWNLOADING_TORRENT) {
                state = DOWNLOADING_UNVANQUISHED;
                aria2::DownloadHandle* handle = aria2::getDownloadHandle(session, gid);
                aria2::A2Gid torrentGid = handle->getFollowedBy()[0];
                setDownloadPathAndFiles(session, torrentGid);
                aria2::deleteDownloadHandle(handle);
            } else {
                event = aria2::EVENT_ON_BT_DOWNLOAD_COMPLETE;
                extractUpdate();
            }
            break;

        case aria2::EVENT_ON_DOWNLOAD_ERROR:
            break;

        case aria2::EVENT_ON_DOWNLOAD_PAUSE:
            break;

        case aria2::EVENT_ON_DOWNLOAD_START:
            if (state == IDLE) {
                state = DOWNLOADING_TORRENT;
            }
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
    // TODO: Unhack this function so that it doesn't break when people set the
    // install dir to a directory which contains unvanquished_
    const std::string prefix = "unvanquished_";
    auto pos = path.find(prefix);
    std::string relativePath = path.substr(pos + prefix.size() + 7);
    return std::to_string(index) + "=" + "Unvanquished/" + relativePath;
}



void DownloadWorker::download(void)
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
            if (totalSize != downloader.totalSize()) {
                totalSize = downloader.totalSize();
                emit totalSizeChanged(totalSize);
            }
        }
    }
}

void DownloadWorker::toggle(void)
{
    downloader.toggleDownloads();
}

void DownloadWorker::setDownloadDirectory(const std::string& dir)
{
    downloadDir = dir.c_str();
    downloader.setDownloadDirectory(dir);
}

void DownloadWorker::stop(void)
{
    running = false;
}

void DownloadWorker::extractUpdate(void)
{
    QString filename;
#ifdef _WIN32
    filename = "win32.zip";
#elif defined(__linux__)
    filename = "linux64.zip";
#elif defined(__APPLE__)
    filename = "mac.zip";
#endif
    JlCompress::extractDir(downloadDir + "/Unvanquished/win32.zip", downloadDir + "Unvanquished/");
}
