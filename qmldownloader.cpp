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

#include <QDir>
#include <QApplication>
#include <QDebug>

#include "qmldownloader.h"
#include "system.h"

static const QString UPDATER_BASE_URL("https://github.com/Unvanquished/updater/releases/download");

QmlDownloader::QmlDownloader(QString ariaLogFilename, QString connectUrl, Settings& settings) :
        ariaLogFilename_(ariaLogFilename),
        connectUrl_(connectUrl),
        settings_(settings),
        downloadSpeed_(0),
        uploadSpeed_(0),
        eta_(0),
        totalSize_(0),
        completedSize_(0),
        worker_(nullptr),
        state_(IDLE) {}

QmlDownloader::~QmlDownloader()
{
    stopAria();
}

int QmlDownloader::downloadSpeed() const {
    return downloadSpeed_;
}

int QmlDownloader::uploadSpeed() const {
    return uploadSpeed_;
}

int QmlDownloader::eta() const {
    return eta_.count();
}

int QmlDownloader::totalSize() const {
    return totalSize_;
}

int QmlDownloader::completedSize() const {
    return completedSize_;
}

void QmlDownloader::setDownloadSpeed(int speed) {
    downloadSpeed_ = speed;
    downloadTime_.addSpeed(speed);
    emit downloadSpeedChanged(speed);
}

void QmlDownloader::setUploadSpeed(int speed) {
    uploadSpeed_ = speed;
    emit uploadSpeedChanged(speed);
}

void QmlDownloader::setTotalSize(int size) {
    totalSize_ = size;
    emit totalSizeChanged(size);
}

void QmlDownloader::setCompletedSize(int size) {
    completedSize_ = size;
    emit completedSizeChanged(size);
    eta_ = std::chrono::seconds(downloadTime_.getTime(totalSize_ - completedSize_));
    emit etaChanged(eta_.count());
}

void QmlDownloader::onDownloadEvent(int event)
{
    switch (event) {
        case aria2::EVENT_ON_BT_DOWNLOAD_COMPLETE:
            if (state() != COMPLETED) {
                qDebug() << "installUpdater in" << settings_.installPath();
                if (!Sys::installUpdater(settings_.installPath())) {
                    emit fatalMessage("Error installing launcher");
                    return;
                }
                qDebug() << "Calling Sys::install";
                if (Sys::installShortcuts()) {
                    emit statusMessage("Up to date");
                } else {
                    emit statusMessage("Error installing shortcuts");
                }
                setState(COMPLETED);
                setDownloadSpeed(0);
                setUploadSpeed(0);
                setCompletedSize(totalSize_);
                stopAria();
            }
            break;

        case aria2::EVENT_ON_DOWNLOAD_COMPLETE:
            emit statusMessage("Torrent downloaded");
            break;

        case aria2::EVENT_ON_DOWNLOAD_ERROR:
            emit fatalMessage("Error received while downloading");
            break;

        case aria2::EVENT_ON_DOWNLOAD_PAUSE:
            emit statusMessage("Download paused");
            break;

        case aria2::EVENT_ON_DOWNLOAD_START:
            emit statusMessage("Download started");
            break;

        case aria2::EVENT_ON_DOWNLOAD_STOP:
            emit statusMessage("Download stopped");
            break;

        case DownloadWorker::ERROR_EXTRACTING:
            emit fatalMessage("Error extracting update");
            break;
    }
}

void QmlDownloader::startUpdate(const QString& selectedInstallPath)
{
    qDebug() << "Selected install path:" << selectedInstallPath;
    if (!Sys::validateInstallPath(selectedInstallPath)) {
        emit fatalMessage("You are running as root, which may cause the installation to"
                          " work incorrectly. Restart the program without using 'sudo'.");
        return;
    }

    QDir dir(selectedInstallPath);
    if (!dir.exists()) {
        if (!dir.mkpath(dir.path())) {
            emit fatalMessage(dir.path() + " does not exist and could not be created");
            return;
        }
    }
    if (!QFileInfo(selectedInstallPath).isWritable()) {
        emit fatalMessage("Install dir not writable. Please select another");
        return;
    }
    // Persist the install path only now that download has been initiated and we know the path is good
    emit statusMessage("Installing to " + dir.canonicalPath());
    if (settings_.installPath() != selectedInstallPath) {
        qDebug() << "Clearing installed version because path was changed";
        settings_.setInstalledVersion("");
    }
    settings_.setInstallPath(selectedInstallPath);

    setState(DOWNLOADING);
    worker_ = new DownloadWorker(ariaLogFilename_);
    worker_->setDownloadDirectory(dir.canonicalPath().toStdString());
    worker_->addTorrent("https://cdn.unvanquished.net/current.torrent");
    worker_->moveToThread(&thread_);
    connect(&thread_, SIGNAL(finished()), worker_, SLOT(deleteLater()));
    connect(worker_, SIGNAL(onDownloadEvent(int)), this, SLOT(onDownloadEvent(int)));
    connect(worker_, SIGNAL(downloadSpeedChanged(int)), this, SLOT(setDownloadSpeed(int)));
    connect(worker_, SIGNAL(uploadSpeedChanged(int)), this, SLOT(setUploadSpeed(int)));
    connect(worker_, SIGNAL(totalSizeChanged(int)), this, SLOT(setTotalSize(int)));
    connect(worker_, SIGNAL(completedSizeChanged(int)), this, SLOT(setCompletedSize(int)));
    connect(&thread_, SIGNAL(started()), worker_, SLOT(download()));
    thread_.start();
}

void QmlDownloader::toggleDownload(QString installPath)
{
    qDebug() << "QmlDownloader::toggleDownload called";
    if (state() == COMPLETED) return;
    if (!worker_) {
        startUpdate(installPath);
        return;
    }
    worker_->toggle();
    setState(state() == DOWNLOADING ? PAUSED : DOWNLOADING);
}

void QmlDownloader::stopAria()
{
    if (worker_) {
        qDebug() << "Stopping downloader thread";
        worker_->stop();
        thread_.quit();
        thread_.wait();
        worker_ = nullptr;
    }
}

void QmlDownloader::startUpdaterUpdate(QString version)
{
    QString url = UPDATER_BASE_URL + "/" + version + "/" + Sys::updaterArchiveName();
    temp_dir_.reset(new QTemporaryDir());
    worker_ = new DownloadWorker(ariaLogFilename_);
    worker_->setDownloadDirectory(QDir(temp_dir_->path()).canonicalPath().toStdString());
    worker_->setConnectUrl(connectUrl_);
    worker_->addUpdaterUri(url.toStdString());
    worker_->moveToThread(&thread_);
    connect(&thread_, SIGNAL(finished()), worker_, SLOT(deleteLater()));
    connect(worker_, SIGNAL(onDownloadEvent(int)), this, SLOT(onDownloadEvent(int)));
    connect(worker_, SIGNAL(downloadSpeedChanged(int)), this, SLOT(setDownloadSpeed(int)));
    connect(worker_, SIGNAL(uploadSpeedChanged(int)), this, SLOT(setUploadSpeed(int)));
    connect(worker_, SIGNAL(totalSizeChanged(int)), this, SLOT(setTotalSize(int)));
    connect(worker_, SIGNAL(completedSizeChanged(int)), this, SLOT(setCompletedSize(int)));
    connect(&thread_, SIGNAL(started()), worker_, SLOT(download()));
    thread_.start();
}

QmlDownloader::DownloadState QmlDownloader::state() const
{
    return state_;
}

void QmlDownloader::setState(DownloadState state)
{
    state_ = state;
    emit stateChanged(state);
}
