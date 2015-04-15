#include "newsfetcher.h"

#include <QUrl>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QStandardItemModel>
#include <QJsonValue>
#include <QJsonDocument>
#include <QJsonObject>
#include <QVariantMap>
#include <QJsonArray>

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
    qDebug() << "Parsing json";
    if (reply->error() == QNetworkReply::NoError) {
        QString json = reply->readAll();
        QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
        QJsonObject obj = document.object();
        QJsonArray posts = obj["posts"].toArray();
        QStandardItemModel model;

        for (auto postValue : posts) {
            QJsonObject post = postValue.toObject();
            QString title = post["title"].toString();
            QString author = post["author"].toObject()["name"].toString();
            QString excerpt = post["excerpt"].toString();
            qDebug() << title;

            QStandardItem item;
            QStandardItem header(title + " by " + author);
            QStandardItem body(excerpt);
            item.appendRow(&header);
            item.appendRow(&body);
            model.appendRow(&item);
        }

        emit newsItemsLoaded(&model);
    }
}

