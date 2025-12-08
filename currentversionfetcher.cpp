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

static const QString versionMirrors[] = {
    "https://cdn.unvanquished.net/",
    "https://cdn.illwieckz.net/unvanquished/",
    "https://webseed.unv.kangz.net/",
    nullptr,
};

static const QString *versionMirror = &versionMirrors[0];

void CurrentVersionFetcher::fetchCurrentVersion()
{
    static const QString versionFile = "current.json";

    if (versionMirror) {
        QString versionURL = versionMirror + versionFile;
        qDebug() << "Fetching" << versionURL;
        QNetworkRequest request = QNetworkRequest(QUrl(versionURL));
        manager_->get(request);
        versionMirror++;
    }
}

void ComponentVersionFetcher(QJsonObject components, QString name, QString system, QString *version, QStringList *urls)
{
    QString path;
    QJsonArray mirrors;

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

        mirrors = component["mirrors"].toArray();
        if (!mirrors.count()) {
            qDebug() << "ComponentVersionFetcher: undefined “mirrors” key for " << name;
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

    qDebug() << "ComponentVersionFetcher: fetched component =" << name;
    qDebug() << "ComponentVersionFetcher: fetched system =" << system;
    qDebug() << "ComponentVersionFetcher: fetched version =" << *version;
    qDebug() << "ComponentVersionFetcher: fetched path =" << path;

    for (auto m : mirrors)
    {
        QString mirror = m.toString();
        qDebug() << "ComponentVersionFetcher: fetched mirror =" << mirror;
        QString url = mirror + path;
        qDebug() << "ComponentVersionFetcher: fetched url =" << url;
        urls->append(url);
    }
}

void CurrentVersionFetcher::reply(QNetworkReply* reply)
{
    QString formatVersion;
    QString updaterVersion;
    QStringList updaterUrls;
    QString gameVersion;
    QStringList gameUrls;
    QString newsVersion;
    QStringList newsUrls;

    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << "CurrentVersionFetcher: network error";

        if (versionMirror) {
            fetchCurrentVersion();
        } else {
            emit onCurrentVersions(updaterVersion, updaterUrls, gameVersion, gameUrls, "");
        }
        return;
    }

    QJsonParseError error;
    QJsonDocument json = QJsonDocument::fromJson(reply->readAll(), &error);
    if (error.error != QJsonParseError::NoError) {
        qDebug() << "CurrentVersionFetcher: JSON parsing error";
        emit onCurrentVersions(updaterVersion, updaterUrls, gameVersion, gameUrls, "");
        return;
    }

    QJsonObject jsonObject = json.object();

    QJsonValue formatVersionValue = jsonObject.value("format");

    if (formatVersionValue == QJsonValue::Undefined) {
        qDebug() << "ComponentVersionFetcher: missing “version” value in current.json";
        emit onCurrentVersions(updaterVersion, updaterUrls, gameVersion, gameUrls, "");
        return;
    } else {
        formatVersion = formatVersionValue.toString();
    }

    QJsonValue componentsValue = jsonObject.value("components");

    if (componentsValue == QJsonValue::Undefined) {
        qDebug() << "ComponentVersionFetcher: missing “components” array in current.json";
        emit onCurrentVersions(updaterVersion, updaterUrls, gameVersion, gameUrls, "");
        return;
    } else {
        qDebug() << "ComponentVersionFetcher: fetched format = " << formatVersion;

        QJsonObject components = componentsValue.toObject();

        ComponentVersionFetcher(components, "updater", Sys::updaterSystem(), &updaterVersion, &updaterUrls);

        ComponentVersionFetcher(components, "game", "all-all", &gameVersion, &gameUrls);

        ComponentVersionFetcher(components, "news", "all-all", &newsVersion, &newsUrls);
    }

    emit onCurrentVersions(updaterVersion, updaterUrls, gameVersion, gameUrls, newsUrls.at(0));
}

