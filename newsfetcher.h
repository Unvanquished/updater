#ifndef NEWSFETCHER_H
#define NEWSFETCHER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QString>

#include <memory>

class QNetworkRequest;
class QStandardItemModel;

class NewsFetcher : public QObject
{
    Q_OBJECT
public:
    explicit NewsFetcher(QObject *parent = 0);
    void get(QString url);

public slots:
    void parseJson(QNetworkReply* reply);

signals:
    void newsItemsLoaded(QStandardItemModel* models);

private:
    std::unique_ptr<QNetworkAccessManager> manager;

};

#endif
