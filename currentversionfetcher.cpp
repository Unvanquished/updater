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

#include "currentversionfetcher.h"
#include "system.h"

#include <QDebug>
#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

CurrentVersionFetcher::CurrentVersionFetcher(QObject* parent) : QObject(parent), manager_(new QNetworkAccessManager(this))
{
    connect(manager_.get(), SIGNAL(finished(QNetworkReply*)), this, SLOT(reply(QNetworkReply*)));
}

void CurrentVersionFetcher::fetchCurrentVersion(QString url)
{
    QNetworkRequest request = QNetworkRequest(QUrl(url));
    manager_->get(request);
}

void ComponentVersionFetcher(QJsonObject jsonObject, QString componentSlug, QString componentSystem, QString *componentVersion, QString *componentUrl)
{
    QString componentMirror;
    QString componentPath;

    QJsonObject componentObject = jsonObject[componentSlug].toObject();
    if (componentObject.isEmpty()) {
        qDebug() << "ComponentVersionFetcher: undefined “" << componentSlug << "” key";
    } else {
        QJsonValue version = componentObject.value("version");
        if (version == QJsonValue::Undefined) {
            qDebug() << "ComponentVersionFetcher: undefined “version” value for" << componentSlug;
        } else {
            *componentVersion = version.toString();
        }

        QJsonArray mirrorsArray = componentObject["mirrors"].toArray();
        if (!mirrorsArray.count()) {
            qDebug() << "ComponentVersionFetcher: undefined “mirrors” key for " << componentSlug;
        } else {
            componentMirror = mirrorsArray.first().toString();
        }

        QJsonObject parcelsObject = componentObject["parcels"].toObject();
        if (parcelsObject.isEmpty()) {
            qDebug() << "ComponentVersionFetcher: undefined “parcels” key for" << componentSlug;
        } else {
            QJsonObject systemObject = parcelsObject[componentSystem].toObject();
            if (systemObject.isEmpty()) {
                qDebug() << "ComponentVersionFetcher: undefined “" << componentSystem << "” key for " << componentSlug;
            } else {
                QJsonValue path = systemObject.value("path");
                if (path == QJsonValue::Undefined) {
                    qDebug() << "ComponentVersionFetcher: undefined “path” value for" << componentSlug;
                } else {
                    componentPath = path.toString();
                }
            }
        }

        *componentUrl = componentMirror + "/" + componentPath;
        if (*componentUrl == "/") {
            *componentUrl = "";
        }

        qDebug() << "ComponentVersionFetcher: fetched component =" << componentSlug;
        qDebug() << "ComponentVersionFetcher: fetched system =" << componentSystem;
        qDebug() << "ComponentVersionFetcher: fetched version =" << *componentVersion;
        qDebug() << "ComponentVersionFetcher: fetched mirror =" << componentMirror;
        qDebug() << "ComponentVersionFetcher: fetched path =" << componentPath;
        qDebug() << "ComponentVersionFetcher: fetched url =" << *componentUrl;
    }
}

void CurrentVersionFetcher::reply(QNetworkReply* reply)
{
    QString updaterVersion;
    QString updaterUrl;
    QString gameVersion;
    QString gameUrl;
    QString newsVersion;
    QString newsUrl;

    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << "CurrentVersionFetcher: network error";
        emit onCurrentVersions(updaterVersion, updaterUrl, gameVersion, gameUrl, newsUrl);
        return;
    }

    QJsonParseError error;
    QJsonDocument json = QJsonDocument::fromJson(reply->readAll(), &error);
    if (error.error != QJsonParseError::NoError) {
        qDebug() << "CurrentVersionFetcher: JSON parsing error";
        emit onCurrentVersions(updaterVersion, updaterUrl, gameVersion, gameUrl, newsUrl);
        return;
    }

    QJsonObject jsonObject = json.object();

    ComponentVersionFetcher(jsonObject, "updater", Sys::updaterSystem(), &updaterVersion, &updaterUrl);

    ComponentVersionFetcher(jsonObject, "game", "all-all", &gameVersion, &gameUrl);

    ComponentVersionFetcher(jsonObject, "news", "all-all", &newsVersion, &newsUrl);

    emit onCurrentVersions(updaterVersion, updaterUrl, gameVersion, gameUrl, newsUrl);
}

