#include <QDir>
#include <QProcess>
#include <QApplication>
#include <QDebug>

#include "qmldownloader.h"
#include "system.h"

namespace {
static const QRegularExpression COMMAND_REGEX("%command%");
static QString UPDATER_BASE_URL("http://github.com/Unvanquished/updater2/releases/download");
}  // namespace

QmlDownloader::QmlDownloader() : downloadSpeed_(0),
        uploadSpeed_(0),
        eta_(0),
        totalSize_(0),
        completedSize_(0),
        paused_(true),
        updater_update_(false),
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
                Sys::install();
                settings_.setCurrentVersion(currentVersion_);
                settings_.setInstallFinished(true);
                setState(COMPLETED);
                setDownloadSpeed(0);
                setUploadSpeed(0);
                setCompletedSize(totalSize_);
                emit statusMessage("Up to date. Press > to play the game.");
                stopAria();
            }
            break;

        case aria2::EVENT_ON_DOWNLOAD_COMPLETE:
            emit statusMessage("Torrent downloaded");
            break;

        case aria2::EVENT_ON_DOWNLOAD_ERROR:
            emit statusMessage("Error received while downloading");
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
            emit statusMessage("Error extracting update");
            break;
    }
}

void QmlDownloader::startUpdate(void)
{
    settings_.setInstallFinished(false);
    setState(DOWNLOADING);
    QString installDir = settings_.installPath();
    QDir dir(installDir);
    if (!dir.exists()) {
        if (!dir.mkpath(dir.path())) {
            emit statusMessage(dir.canonicalPath() + " does not exist and could not be created");
            return;
        }
    }
    if (!QFileInfo(installDir).isWritable()) {
        emit statusMessage("Install dir not writable. Please select another");
        return;
    }
    emit statusMessage("Installing to " + dir.canonicalPath());

    worker_ = new DownloadWorker();
    worker_->setDownloadDirectory(dir.canonicalPath().toStdString());
    worker_->addTorrent("http://cdn.unvanquished.net/current.torrent");
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

void QmlDownloader::startGame(void)
{
    QString cmd = settings_.installPath() + QDir::separator() + Sys::executableName();
    QString commandLine = settings_.commandLine();
    commandLine.replace(COMMAND_REGEX, cmd);

    QProcess *process = new QProcess;
    connect(process, SIGNAL(finished(int,QProcess::ExitStatus)), process, SLOT(deleteLater()));
    connect(process, SIGNAL(finished(int,QProcess::ExitStatus)), QApplication::instance(), SLOT(quit()));
    process->start(commandLine);
}

void QmlDownloader::toggleDownload(void)
{
    if (state() == COMPLETED) return;
    if (!worker_) {
        startUpdate();
        return;
    }
    worker_->toggle();
    setState(state() == DOWNLOADING ? PAUSED : DOWNLOADING);
}

void QmlDownloader::stopAria(void)
{
    if (worker_) {
        worker_->stop();
        thread_.quit();
        thread_.wait();
        worker_ = nullptr;
    }
}

void QmlDownloader::checkForUpdate()
{
    if (!settings_.installFinished()) {
        emit updateNeeded(true);
        return;
    } else {
        if (networkManager_.isOnline()) {
            connect(&fetcher_, SIGNAL(onCurrentVersions(QString, QString)), this, SLOT(onCurrentVersions(QString, QString)));
            fetcher_.fetchCurrentVersion("http://dl.unvanquished.net/versions.json");
            return;
        }
    }
    emit updateNeeded(false);
}

void QmlDownloader::onCurrentVersions(QString updater, QString game)
{
    if (!updater.isEmpty() && updater != QString(GIT_VERSION)) {
        QString url = UPDATER_BASE_URL + "/" + updater + "/" + Sys::updaterArchiveName();
        temp_dir_.reset(new QTemporaryDir());
        worker_ = new DownloadWorker();
        worker_->setDownloadDirectory(QDir(temp_dir_->path()).canonicalPath().toStdString());
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
    } else if (game.isEmpty() || settings_.currentVersion() != game) {
        currentVersion_ = game;
        emit updateNeeded(true);
    } else {
        emit updateNeeded(false);
    }
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
