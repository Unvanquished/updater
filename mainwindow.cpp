#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "downloadworker.h"
#include "aria2/src/includes/aria2/aria2.h"

#include <QDir>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    totalSize(0),
    paused(false)
{
    ui->setupUi(this);
    ui->progressBar->setValue(0);
    connect(ui->updateButton, SIGNAL(clicked()), this, SLOT(startUpdate()));
    connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(close()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::startUpdate(void)
{
    ui->textBrowser->append("Starting update");
    ui->toggleButton->setEnabled(true);
    ui->updateButton->setEnabled(false);
    QFile oldTorrent(QDir::temp().filePath("current.torrent"));
    if (oldTorrent.exists()) {
        ui->textBrowser->append("Removing old torrent");
        oldTorrent.remove();
    }
    oldTorrent.close();
    worker = new DownloadWorker();
    worker->setDownloadDirectory(QDir::tempPath().toStdString());
    worker->addUri("http://cdn.unvanquished.net/current.torrent");
    worker->moveToThread(&thread);
    connect(&thread, SIGNAL(finished()), worker, SLOT(deleteLater()));
    connect(worker, SIGNAL(onDownloadEvent(int)), this, SLOT(onDownloadEvent(int)));
    connect(worker, SIGNAL(downloadSpeedChanged(int)), this, SLOT(setDownloadSpeed(int)));
    connect(worker, SIGNAL(uploadSpeedChanged(int)), this, SLOT(setUploadSpeed(int)));
    connect(worker, SIGNAL(totalSizeChanged(int)), this, SLOT(setTotalSize(int)));
    connect(worker, SIGNAL(completedSizeChanged(int)), this, SLOT(setCompletedSize(int)));
    connect(&thread, SIGNAL(started()), worker, SLOT(download()));
    connect(ui->toggleButton, SIGNAL(clicked()), this, SLOT(toggleDownload()));
    thread.start();
}

void MainWindow::toggleDownload(void)
{
    worker->toggle();
    paused = !paused;
    if (paused) {
        ui->toggleButton->setText("Resume");
    } else {
        ui->toggleButton->setText("Pause");
    }
}


void MainWindow::setDownloadSpeed(int speed)
{
    ui->downloadSpeed->setText(sizeToString(speed) + "/s");
}

void MainWindow::setUploadSpeed(int speed)
{
    ui->uploadSpeed->setText(sizeToString(speed) + "/s");
}

void MainWindow::setTotalSize(int size)
{
    ui->totalSize->setText(sizeToString(size));
    totalSize = size;
}

void MainWindow::setCompletedSize(int size)
{
    ui->completedSize->setText(sizeToString(size));
    ui->progressBar->setValue((static_cast<float>(size) / totalSize) * 100);
}

void MainWindow::onDownloadEvent(int event)
{
    switch (event) {
        case aria2::EVENT_ON_BT_DOWNLOAD_COMPLETE:
            ui->textBrowser->append("yay");
            thread.quit();
            thread.wait();
            break;

        case aria2::EVENT_ON_DOWNLOAD_COMPLETE:
            ui->textBrowser->append("Torrent downloaded");
            break;

        case aria2::EVENT_ON_DOWNLOAD_ERROR:
            ui->textBrowser->append("Error received while downloading");
            break;

        case aria2::EVENT_ON_DOWNLOAD_PAUSE:
            ui->textBrowser->append("Download paused");
            break;

        case aria2::EVENT_ON_DOWNLOAD_START:
            ui->textBrowser->append("Download started");
            break;

        case aria2::EVENT_ON_DOWNLOAD_STOP:
            ui->textBrowser->append("Download stopped");
            break;
    }
}

QString MainWindow::sizeToString(int size)
{
    static QString sizes[] = { "Bytes", "KiB", "MiB", "GiB" };
    const int num_sizes = 4;
    int i = 0;
    while (size > 1024 && i++ < 4) {
        size /= 1024;
    }

    return QString::number(size) + " " + sizes[std::min(i, num_sizes - 1)];
}
