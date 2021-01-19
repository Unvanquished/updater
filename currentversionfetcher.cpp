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

void ComponentVersionFetcher(QJsonObject components, QString name, QString system, QString *version, QString *url)
{
    QString mirror;
    QString path;

    QJsonObject component = components[name].toObject();

    if (component.isEmpty()) {
        qDebug() << "ComponentVersionFetcher: undefined “" << name << "” key";
    } else {
        QJsonValue versionValue = component.value("version");
        if (versionValue == QJsonValue::Undefined) {
            qDebug() << "ComponentVersionFetcher: undefined “version” value for" << name;
        } else {
            *version = versionValue.toString();
        }

        QJsonArray mirrors = component["mirrors"].toArray();
        if (!mirrors.count()) {
            qDebug() << "ComponentVersionFetcher: undefined “mirrors” key for " << name;
        } else {
            mirror = mirrors.first().toString();
        }

        QJsonObject parcels = component["parcels"].toObject();
        if (parcels.isEmpty()) {
            qDebug() << "ComponentVersionFetcher: undefined “parcels” key for" << name;
        } else {
            QJsonObject systemObject = parcels[system].toObject();
            if (systemObject.isEmpty()) {
                qDebug() << "ComponentVersionFetcher: undefined “" << system << "” key for " << name;
            } else {
                QJsonValue pathValue = systemObject.value("path");
                if (pathValue == QJsonValue::Undefined) {
                    qDebug() << "ComponentVersionFetcher: undefined “path” value for" << name;
                } else {
                    path = pathValue.toString();
                }
            }
        }
    }

    *url = mirror + path;

    qDebug() << "ComponentVersionFetcher: fetched component =" << name;
    qDebug() << "ComponentVersionFetcher: fetched system =" << system;
    qDebug() << "ComponentVersionFetcher: fetched version =" << *version;
    qDebug() << "ComponentVersionFetcher: fetched mirror =" << mirror;
    qDebug() << "ComponentVersionFetcher: fetched path =" << path;
    qDebug() << "ComponentVersionFetcher: fetched url =" << *url;
}

void CurrentVersionFetcher::reply(QNetworkReply* reply)
{
    QString formatVersion;
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

    QJsonValue formatVersionValue = jsonObject.value("format");

    if (formatVersionValue == QJsonValue::Undefined) {
        qDebug() << "ComponentVersionFetcher: missing “version” value in current.json";
    } else {
        formatVersion = formatVersionValue.toString();
    }

    QJsonValue componentsValue = jsonObject.value("components");

    if (componentsValue == QJsonValue::Undefined) {
        qDebug() << "ComponentVersionFetcher: missing “components” array in current.json";
    } else {
        qDebug() << "ComponentVersionFetcher: fetched format = " << formatVersion;

        QJsonObject components = componentsValue.toObject();

        ComponentVersionFetcher(components, "updater", Sys::updaterSystem(), &updaterVersion, &updaterUrl);

        ComponentVersionFetcher(components, "game", "all-all", &gameVersion, &gameUrl);

        ComponentVersionFetcher(components, "news", "all-all", &newsVersion, &newsUrl);
    }

    emit onCurrentVersions(updaterVersion, updaterUrl, gameVersion, gameUrl, newsUrl);
}

