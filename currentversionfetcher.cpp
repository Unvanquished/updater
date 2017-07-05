#include "currentversionfetcher.h"

#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>

CurrentVersionFetcher::CurrentVersionFetcher(QObject* parent) : QObject(parent), manager(new QNetworkAccessManager(this))
{
    connect(manager.get(), SIGNAL(finished(QNetworkReply*)), this, SLOT(reply(QNetworkReply*)));
}

void CurrentVersionFetcher::fetchCurrentVersion(QString url)
{
    QNetworkRequest request = QNetworkRequest(QUrl(url));
    manager->get(request);
}

void CurrentVersionFetcher::reply(QNetworkReply* reply)
{
    QString game;
    QString updater;
    if (reply->error() != QNetworkReply::NoError) {
        emit onCurrentVersions(updater, game);
        return;
    }
    QJsonParseError error;
    QJsonDocument json = QJsonDocument::fromJson(reply->readAll(), &error);
    if (error.error != QJsonParseError::NoError) {
        emit onCurrentVersions(updater, game);
        return;
    }
    QJsonValue value = json.object().value("updater");
    if (value != QJsonValue::Undefined) {
        updater = value.toString();
    }
    value = json.object().value("unvanquished");
    if (value != QJsonValue::Undefined) {
        game = value.toString();
    }
    emit onCurrentVersions(updater, game);
}

