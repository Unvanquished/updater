#include <chrono>
#include "downloadworker.h"

DownloadWorker::DownloadWorker(QObject *parent) : QObject(parent), downloadSpeed(0), uploadSpeed(0),
    totalSize(0), completedSize(0), paused(true)
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

void DownloadWorker::onDownloadCallback(int event)
{
    emit onDownloadEvent(event);
}

void DownloadWorker::download(void)
{
    auto start = std::chrono::steady_clock::now();
    paused = false;
    while (downloader.run()) {
        auto now = std::chrono::steady_clock::now();
        auto count = std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count();
        // Print progress information once per 500ms
        if(count >= 500) {
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
