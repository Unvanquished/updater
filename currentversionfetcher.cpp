#include "currentversionfetcher.h"

#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkReply>

CurrentVersionFetcher::CurrentVersionFetcher(QObject* parent) : manager(new QNetworkAccessManager(this))
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
    if (reply->error() == QNetworkReply::NoError) {
        QString version = reply->readAll();
        emit onCurrentVersion(version);
        return;
    }
    emit onCurrentVersion("");
}

