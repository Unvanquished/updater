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
}  // namespace

QmlDownloader::QmlDownloader() : downloadSpeed_(0),
        uploadSpeed_(0),
        eta_(0),
        totalSize_(0),
        completedSize_(0),
        worker_(nullptr),
        forceUpdaterUpdate_(false),
        forceGameUpdate_(false),
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
                // The game should be playable at this point - set the installed version
                if (latestGameVersion_.isEmpty()) {
                    latestGameVersion_ = "unknown";
                }
                qDebug() << "Setting installed version to" << latestGameVersion_;
                settings_.setCurrentVersion(latestGameVersion_);

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
        settings_.setCurrentVersion("");
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

void QmlDownloader::startGame()
{
    QString commandLine = settings_.commandLine().trimmed();
    if (!commandLine.contains(COMMAND_REGEX)) {
        commandLine = "%command% " + commandLine;
    }
    commandLine.replace(COMMAND_REGEX, Sys::getGameCommand(settings_.installPath()));
    qDebug() << "Starting game with command line:" << commandLine;
    if (Sys::startGame(commandLine)) {
        qDebug() << "Game started successfully";
    } else {
        qDebug() << "Failed to start Unvanquished process.";
        QMessageBox errorMessageBox;
        errorMessageBox.setText("Failed to start Unvanquished process.");
        errorMessageBox.exec();
    }
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

// Initiates an asynchronous request for the latest available versions.
void QmlDownloader::checkForUpdate()
{
    connect(&fetcher_, SIGNAL(onCurrentVersions(QString, QString)), this, SLOT(onCurrentVersions(QString, QString)));
    fetcher_.fetchCurrentVersion("https://dl.unvanquished.net/versions.json");
}

// Initiate updater update to specified version
void QmlDownloader::forceUpdaterUpdate(const QString& version)
{
    forceUpdaterUpdate_ = true;
    latestUpdaterVersion_ = version;
}

// Launch the update window later even if the installed and current game versions match
void QmlDownloader::forceGameUpdate()
{
    forceGameUpdate_ = true;
}

// Receives the results of the checkForUpdate request.
void QmlDownloader::onCurrentVersions(QString updater, QString game)
{
    latestUpdaterVersion_ = updater;
    latestGameVersion_ = game;
}

void QmlDownloader::launchGameIfInstalled()
{
    if (settings_.currentVersion().isEmpty()) {
        qDebug() << "No game installed, exiting";
        QCoreApplication::quit();
    } else {
        qDebug() << "Fall back to launching installed game";
        emit updateNeeded(false);
    }
}

// This runs after the splash screen has been displayed for the programmed amount of time (and the
// user did not click the settings button). If the CurrentVersionFetcher didn't emit anything yet,
// proceed as if the request for versions.json failed.
void QmlDownloader::autoLaunchOrUpdate()
{
    qDebug() << "Previously-installed game version:" << settings_.currentVersion();
    if (forceGameUpdate_) {
        qDebug() << "Game update menu requested";
        emit updateNeeded(true);
    } else if (forceUpdaterUpdate_ ||
               (!latestUpdaterVersion_.isEmpty() && latestUpdaterVersion_ != QString(GIT_VERSION))) {
        qDebug() << "Updater update to version" << latestUpdaterVersion_ << "required";
        if (!forceUpdaterUpdate_) {
            switch (Sys::RelaunchElevated("--splashms 1 --update-updater-to " + latestUpdaterVersion_)) {
                case Sys::ElevationResult::UNNEEDED:
                    break;
                case Sys::ElevationResult::RELAUNCHED:
                    QCoreApplication::quit();
                    return;
                case Sys::ElevationResult::FAILED:
                    launchGameIfInstalled();
                    return;
            }
        }
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
        switch (Sys::RelaunchElevated("--splashms 1 --update-game")) {
            case Sys::ElevationResult::UNNEEDED:
                break;
            case Sys::ElevationResult::RELAUNCHED:
                QCoreApplication::quit();
                return;
            case Sys::ElevationResult::FAILED:
                launchGameIfInstalled();
                return;
        }
        emit updateNeeded(true);
    } else {
        emit updateNeeded(false);
    }
}

// Return value is whether the program should exit
bool QmlDownloader::relaunchForSettings()
{
    qDebug() << "Possibly relaunching to open settings window";
    return Sys::RelaunchElevated("--splashms 1 --update-game") != Sys::ElevationResult::UNNEEDED;
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
