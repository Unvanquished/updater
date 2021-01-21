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
    explicit CurrentVersionFetcher(QObject *parent = nullptr);
    void fetchCurrentVersion(QString url);

signals:
    void onCurrentVersions(QString updaterVersion, QString updaterUrl, QString gameVersion, QString gameUrl, QString newsUrl);

private slots:
    void reply(QNetworkReply* reply);

private:
    std::unique_ptr<QNetworkAccessManager> manager_;


};
#endif
