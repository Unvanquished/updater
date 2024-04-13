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

#ifndef CURRENTVERSIONFETCHER_h
#define CURRENTVERSIONFETCHER_h

#include <QObject>
#include <QNetworkAccessManager>
#include <QString>
#include <memory>

class CurrentVersionFetcher : public QObject
{
    Q_OBJECT
public:
    explicit CurrentVersionFetcher(QObject *parent = nullptr);
    void fetchCurrentVersion(QString url);

signals:
    void onCurrentVersions(QString updater, QString game);

private slots:
    void reply(QNetworkReply* reply);

private:
    std::unique_ptr<QNetworkAccessManager> manager_;


};
#endif
