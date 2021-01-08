#include "currentversionfetcher.h"

#include <QDebug>
#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>

CurrentVersionFetcher::CurrentVersionFetcher(QObject* parent) : QObject(parent), manager_(new QNetworkAccessManager(this))
{
    connect(manager_.get(), SIGNAL(finished(QNetworkReply*)), this, SLOT(reply(QNetworkReply*)));
}

void CurrentVersionFetcher::fetchCurrentVersion(QString url)
{
    QNetworkRequest request = QNetworkRequest(QUrl(url));
    manager_->get(request);
}

void CurrentVersionFetcher::reply(QNetworkReply* reply)
{
    QString game;
    QString updater;
    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << "CurrentVersionFetcher: network error";
        emit onCurrentVersions(updater, game);
        return;
    }
    QJsonParseError error;
    QJsonDocument json = QJsonDocument::fromJson(reply->readAll(), &error);
    if (error.error != QJsonParseError::NoError) {
        qDebug() << "CurrentVersionFetcher: JSON parsing error";
        emit onCurrentVersions(updater, game);
        return;
    }
    QJsonValue value = json.object().value("updater");
    if (value != QJsonValue::Undefined) {
        updater = value.toString();
    } else {
        qDebug() << "CurrentVersionFetcher: undefined “updater” value";
    }
    value = json.object().value("unvanquished");
    if (value != QJsonValue::Undefined) {
        game = value.toString();
    } else {
        qDebug() << "CurrentVersionFetcher: undefined “unvanquished” value";
    }
    qDebug() << "CurrentVersionFetcher: fetched versions: updater =" << updater << "game =" << game;
    emit onCurrentVersions(updater, game);
}

