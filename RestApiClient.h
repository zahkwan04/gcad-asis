#ifndef RESTAPICLIENT_H
#define RESTAPICLIENT_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>

class RestApiClient : public QObject
{
    Q_OBJECT

public:
    explicit RestApiClient(QObject *parent = nullptr);
    void getRequest(const QString &url);
    void postRequest(const QString &url, const QJsonObject &data);
    void putRequest(const QString &url, const QJsonObject &data);
    void patchRequest(const QString &url, const QJsonObject &data);
    void deleteRequest(const QString &url);

signals:
    void requestFinished(QNetworkReply *reply);

private slots:
    void onReplyFinished(QNetworkReply *reply);

private:
    QNetworkAccessManager networkManager;
};

#endif // RESTAPICLIENT_H
