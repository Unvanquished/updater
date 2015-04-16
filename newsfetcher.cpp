#include "newsfetcher.h"

#include <QUrl>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QJsonValue>
#include <QJsonDocument>
#include <QJsonObject>
#include <QVariantMap>
#include <QJsonArray>
#include <QLabel>

#include <QDebug>

NewsFetcher::NewsFetcher(QObject *parent) : manager(new QNetworkAccessManager(this))
{
    connect(manager.get(), SIGNAL(finished(QNetworkReply*)), this, SLOT(parseJson(QNetworkReply*)));
}

void NewsFetcher::get(QString url)
{
    QNetworkRequest request = QNetworkRequest(QUrl(url));
    manager->get(request);

}

void NewsFetcher::parseJson(QNetworkReply* reply)
{
    QStringList news;
    if (reply->error() == QNetworkReply::NoError) {
        QString json = reply->readAll();
        QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
        QJsonObject obj = document.object();
        QJsonArray posts = obj["posts"].toArray();

        for (auto postValue : posts) {
            QJsonObject post = postValue.toObject();
            QString title = post["title"].toString();
            QString author = post["author"].toObject()["name"].toString();
            QString excerpt = post["excerpt"].toString();
            news.push_back("<b>" + title + "</b> by " + author + "\n" + excerpt);
        }

        emit newsItemsLoaded(news);
    }
}

