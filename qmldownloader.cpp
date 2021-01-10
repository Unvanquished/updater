#include <QDir>
#include <QProcess>
#include <QApplication>
#include <QDebug>
#include <QMessageBox>

#include "qmldownloader.h"
#include "system.h"

namespace {
static const QRegularExpression COMMAND_REGEX("%command%");
static QString UPDATER_BASE_URL("https://github.com/Unvanquished/updater/releases/download");

QString QuoteQProcessCommandArgument(QString arg) {
    arg.replace('"', "\"\"\"");
    return '"' + arg + '"';
}
}  // namespace

QmlDownloader::QmlDownloader() : downloadSpeed_(0),
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
                qDebug() << "Calling Sys::install";
                Sys::install();
                // FIXME: latestGameVersion_ could be empty if CurrentVersionFetcher didn't succeed
                settings_.setCurrentVersion(latestGameVersion_);
                settings_.setInstallFinished(true);
                setState(COMPLETED);
                setDownloadSpeed(0);
                setUploadSpeed(0);
                setCompletedSize(totalSize_);
                emit statusMessage("Up to date");
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

void QmlDownloader::startUpdate()
{
    settings_.setInstallFinished(false);
    setState(DOWNLOADING);
    QString installDir = settings_.installPath();
    QDir dir(installDir);
    if (!dir.exists()) {
        if (!dir.mkpath(dir.path())) {
            emit statusMessage(dir.path() + " does not exist and could not be created");
            return;
        }
    }
    if (!QFileInfo(installDir).isWritable()) {
        emit statusMessage("Install dir not writable. Please select another");
        return;
    }
    emit statusMessage("Installing to " + dir.canonicalPath());

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

void QmlDownloader::startGame()
{
    QString cmd = settings_.installPath() + QDir::separator() + Sys::executableName();
    QString commandLine = settings_.commandLine();
    commandLine.replace(COMMAND_REGEX, QuoteQProcessCommandArgument(cmd));

    QProcess *process = new QProcess;
    connect(process, SIGNAL(finished(int,QProcess::ExitStatus)), process, SLOT(deleteLater()));
    connect(process, SIGNAL(finished(int,QProcess::ExitStatus)), QApplication::instance(), SLOT(quit()));
    qDebug() << "Starting game with command line:" << commandLine;
    process->start(commandLine);
    if (!process->waitForStarted(-1)) {
        qDebug() << "Failed to start Unvanquished process.";
        QMessageBox errorMessageBox;
        errorMessageBox.setText("Failed to start Unvanquished process.");
        errorMessageBox.exec();
        // If the process fails to start, it does not emit the 'finished' signal.
        QApplication::instance()->quit();
    }
}

void QmlDownloader::toggleDownload()
{
    qDebug() << "QmlDownloader::toggleDownload called";
    if (state() == COMPLETED) return;
    if (!worker_) {
        startUpdate();
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

// Initiates an asynchronous request for the latest available versions.
void QmlDownloader::checkForUpdate()
{
    connect(&fetcher_, SIGNAL(onCurrentVersions(QString, QString)), this, SLOT(onCurrentVersions(QString, QString)));
    fetcher_.fetchCurrentVersion("https://dl.unvanquished.net/versions.json");
}

// Receives the results of the checkForUpdate request.
void QmlDownloader::onCurrentVersions(QString updater, QString game)
{
    latestUpdaterVersion_ = updater;
    latestGameVersion_ = game;
}

// This runs after the splash screen has been displayed for the programmed amount of time (and the
// user did not click the settings button). If the CurrentVersionFetcher didn't emit anything yet,
// proceed as if the request for versions.json failed.
void QmlDownloader::autoLaunchOrUpdate()
{
    qDebug() << "Previously-installed game version:" << settings_.currentVersion();
    if (!latestUpdaterVersion_.isEmpty() && latestUpdaterVersion_ != QString(GIT_VERSION)) {
        qDebug() << "Updater update to version" << latestUpdaterVersion_ << "required";
        QString url = UPDATER_BASE_URL + "/" + latestUpdaterVersion_ + "/" + Sys::updaterArchiveName();
        temp_dir_.reset(new QTemporaryDir());
        worker_ = new DownloadWorker(ariaLogFilename_);
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
    } else if (settings_.currentVersion().isEmpty() ||
               (!latestGameVersion_.isEmpty() && settings_.currentVersion() != latestGameVersion_)) {
        qDebug() << "Game update required.";
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
