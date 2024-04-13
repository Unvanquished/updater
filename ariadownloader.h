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

#ifndef ARIADOWNLOADER_H
#define ARIADOWNLOADER_H

#include "aria2/src/includes/aria2/aria2.h"
#include <list>

class AriaDownloader
{
public:
    AriaDownloader(const std::string& ariaLogFilename);
    ~AriaDownloader();

    class DownloadCallback {
    public:
        virtual void onDownloadCallback(aria2::Session* session, aria2::DownloadEvent event,
                                        aria2::A2Gid gid, void* userData) = 0;
    };

    bool addUri(const std::string& uri);
    bool run();
    void toggleDownloads();
    void setDownloadDirectory(const std::string& dir);
    void registerCallback(DownloadCallback* callback_);
    void unregisterCallback(DownloadCallback* callback_);
    DownloadCallback* callback();
    void updateStats();
    int downloadSpeed();
    int uploadSpeed();
    int completedSize();
    int totalSize();

private:
    aria2::Session* session_;
    DownloadCallback* callback_;
    std::list<aria2::A2Gid> pausedGids_;
    int downloadSpeed_ = 0;
    int uploadSpeed_ = 0;
    int completedSize_ = 0;
    int totalSize_ = 0;
};

#endif // ARIADOWNLOADER_H
