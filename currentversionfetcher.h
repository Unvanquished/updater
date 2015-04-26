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
    explicit CurrentVersionFetcher(QObject *parent = 0);
    void fetchCurrentVersion(QString url);

signals:
    void onCurrentVersion(QString version);

private slots:
    void reply(QNetworkReply* reply);

private:
    std::unique_ptr<QNetworkAccessManager> manager;


};
#endif
