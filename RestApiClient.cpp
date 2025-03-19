#include "RestApiClient.h"
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QDebug>

RestApiClient::RestApiClient(QObject *parent) : QObject(parent)
{
    connect(&networkManager, &QNetworkAccessManager::finished, this, &RestApiClient::onReplyFinished);
}

void RestApiClient::getRequest(const QString &url)
{
    QNetworkRequest request{ QUrl(url) };
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkReply *reply = networkManager.get(request);
    connect(reply, &QNetworkReply::finished, this, [reply]() {
        qDebug() << "GET Response:" << reply->readAll();
        reply->deleteLater();
    });
}

void RestApiClient::postRequest(const QString &url, const QJsonObject &data)
{
    QNetworkRequest request{ QUrl(url) };
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkReply *reply = networkManager.post(request, QJsonDocument(data).toJson());
    connect(reply, &QNetworkReply::finished, this, [reply]() {
        qDebug() << "POST Response:" << reply->readAll();
        reply->deleteLater();
    });
}

void RestApiClient::putRequest(const QString &url, const QJsonObject &data)
{
    QNetworkRequest request{ QUrl(url) };
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkReply *reply = networkManager.put(request, QJsonDocument(data).toJson());
    connect(reply, &QNetworkReply::finished, this, [reply]() {
        qDebug() << "PUT Response:" << reply->readAll();
        reply->deleteLater();
    });
}

void RestApiClient::patchRequest(const QString &url, const QJsonObject &data)
{
    QNetworkRequest request{ QUrl(url) };
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkReply *reply = networkManager.sendCustomRequest(request, QByteArray("PATCH"), QJsonDocument(data).toJson());
    connect(reply, &QNetworkReply::finished, this, [reply]() {
        qDebug() << "PATCH Response:" << reply->readAll();
        reply->deleteLater();
    });
}

void RestApiClient::deleteRequest(const QString &url)
{
    QNetworkRequest request{ QUrl(url) };
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkReply *reply = networkManager.deleteResource(request);
    connect(reply, &QNetworkReply::finished, this, [reply]() {
        qDebug() << "DELETE Response:" << reply->readAll();
        reply->deleteLater();
    });
}

void RestApiClient::onReplyFinished(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << "Error:" << reply->errorString();
    } else {
        qDebug() << "Response:" << reply->readAll();
    }
    reply->deleteLater();
}
