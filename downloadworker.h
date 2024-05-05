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

#ifndef DOWNLOADWORKER_H
#define DOWNLOADWORKER_H

#include "ariadownloader.h"
#include <atomic>
#include <QObject>
#include <QRegularExpression>
#include <QStandardItemModel>

class DownloadWorker : public QObject, public AriaDownloader::DownloadCallback
{
    Q_OBJECT
public:
    enum Events {
        ERROR_EXTRACTING = 100
    };

    explicit DownloadWorker(QString ariaLogFilename, QObject *parent = 0);
    ~DownloadWorker();
    void onDownloadCallback(aria2::Session* session, aria2::DownloadEvent event,
                            aria2::A2Gid gid, void* userDataevent);
    void addUpdaterUri(const std::string& uri);
    void addTorrent(const std::string& uri);
    void setDownloadDirectory(const std::string& dir);
    void setConnectUrl(const QString& url);
    void toggle();
    void stop();

public slots:
    void download();

signals:
    void downloadSpeedChanged(int speed);
    void uploadSpeedChanged(int speed);
    void totalSizeChanged(int size);
    void completedSizeChanged(int size);
    void onDownloadEvent(int event);

private:
    void setDownloadPathAndFiles(aria2::Session* session, aria2::A2Gid gid);
    bool extractUpdate();
    std::string getAriaIndexOut(size_t index, std::string path);

    enum State {
        IDLE,
        DOWNLOADING_TORRENT,
        DOWNLOADING_UPDATER,
        DOWNLOADING_UNVANQUISHED
    };

    int downloadSpeed_;
    int uploadSpeed_;
    int totalSize_;
    int completedSize_;
    State state_;
    std::atomic_bool running_;
    QRegularExpression renameRegex_;
    AriaDownloader downloader_;
    QString downloadDir_;
    QString unvanquishedVersion_; // determined from path names in the download
    QString connectUrl_;
};

#endif // DOWNLOADWORKER_H
