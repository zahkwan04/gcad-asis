/**
 * Qt QML map interface.
 *
 * Copyright (C) Sapura Secured Technologies, 2023-2024. All Rights Reserved.
 *
 * @file
 * @version $Id: GisQmlInt.cpp 1894 2024-12-20 07:15:28Z rosnin $
 * @author Rosnin Mustaffa
 */
#include <QGuiApplication>
#include <QNetworkRequest>
#include <QStandardPaths>
#include <QUrl>

#include "GisQmlInt.h"

using namespace std;

static const string  SYSPREFIX  ("sys_");
static const QString IMG_ROUTE  (":/Images/images/cursor_route.png");
static const QString IMG_SEL_RSC(":/Images/images/cursor_sel_resource.png");
static const QString CACHE_DIR  (QStandardPaths::writableLocation(
                                         QStandardPaths::GenericCacheLocation) +
                                 "/QtLocation/seaTiles/");

const double GisQmlInt::ZOOM_MAX    = 20.0;
const double GisQmlInt::ZOOM_MIN    =  5.0;
const double GisQmlInt::ZOOM_SEARCH = 15.0;
const double GisQmlInt::ZOOM_TRACK  = 18.0;

#ifdef INCIDENT
const string GisQmlInt::KEY_INCIDENT       (SYSPREFIX + "Incident");
const string GisQmlInt::KEY_INCIDENT_REPORT(SYSPREFIX + "ReportIncident");
#endif
const string GisQmlInt::KEY_MEASURE        (SYSPREFIX + "Measure");
#ifdef GIS_ROUTING
const string GisQmlInt::KEY_ROUTE          (SYSPREFIX + "Route");
#endif
const string GisQmlInt::KEY_SEADEPTH       (SYSPREFIX + "Seadepth");
const string GisQmlInt::KEY_SEAMARKS       (SYSPREFIX + "Seamarks");
const string GisQmlInt::KEY_SEARCH         (SYSPREFIX + "Search");
const string GisQmlInt::KEY_TERMINAL       (SYSPREFIX + "Terminal");
const string GisQmlInt::KEY_TRACKING       (SYSPREFIX + "Tracking");
const string GisQmlInt::KEY_TRAILING       (SYSPREFIX + "Trailing");
const string GisQmlInt::KEY_USERPOI        (SYSPREFIX + "UserPoi");
const string GisQmlInt::SYS_ATTR_VALUE     (SYSPREFIX + "CATEGORY");

const time_t GisQmlInt::TERMINAL_TIME_THRESHOLD = 30 * 60; //seconds

QString GisQmlInt::sMapPath;

void GisQmlInt::downloadTile(const QString &url, const QString &filepath)
{
    if (mDownloads.contains(filepath))
        return;
    mDownloads.insert(filepath);
    QNetworkReply *reply = mNwkManager->get(QNetworkRequest(QUrl(url)));
    connect(reply, &QNetworkReply::finished, this,
            [this, filepath, reply] { handleReply(reply, filepath); });
}

void GisQmlInt::clearTiles()
{
    QDir dir(CACHE_DIR);
    dir.setNameFilters(QStringList() << "*.png");
    dir.setFilter(QDir::Files);
    foreach(QString f, dir.entryList())
    {
        dir.remove(f);
    }
}

QString GisQmlInt::getCachePath()
{
    QDir dir(CACHE_DIR);
    if (!dir.exists())
        dir.mkpath(".");
    return CACHE_DIR;
}

QString GisQmlInt::getModelName(int typeId)
{
    switch (typeId)
    {
#ifdef INCIDENT
        case TYPEID_INCIDENT:
            return QString::fromStdString(KEY_INCIDENT);
#endif
        case TYPEID_POI:
            return QString::fromStdString(KEY_USERPOI);
#ifdef GIS_ROUTING
        case TYPEID_ROUTE_RESULT:
            return QString::fromStdString(KEY_ROUTE);
#endif
        case TYPEID_SEARCH:
            return QString::fromStdString(KEY_SEARCH);
        default:
            return QString::fromStdString(KEY_TERMINAL);
    }
}

void GisQmlInt::setMouseCursor(int mode)
{
    switch (mode)
    {
        case GisQmlInt::MODE_MEASURE:
            QGuiApplication::setOverrideCursor(Qt::PointingHandCursor);
            break;
        case GisQmlInt::MODE_RESOURCES:
            QGuiApplication::setOverrideCursor(
             QCursor(QPixmap(IMG_SEL_RSC).scaled(32, 32, Qt::KeepAspectRatio)));
            break;
        case GisQmlInt::MODE_ROUTE:
            QGuiApplication::setOverrideCursor(
                 QCursor(QPixmap(IMG_ROUTE).scaled(32, 32, Qt::KeepAspectRatio),
                         -1, 32));
            break;
        case GisQmlInt::MODE_ZOOM:
            QGuiApplication::setOverrideCursor(Qt::CrossCursor);
            break;
        default:
            QGuiApplication::restoreOverrideCursor();
            break;
    }
}

bool GisQmlInt::setMapPath(const QString &path)
{
    if (path.isEmpty() || path == sMapPath)
        return false;
    sMapPath = path;
    return true;
}

void GisQmlInt::handleReply(QNetworkReply *reply, const QString &filepath)
{
    if (isRedirect(reply))
        openUrl(reply, filepath);
    else if (reply->error() == QNetworkReply::NoError)
        saveTile(reply, filepath);
    else
        emit tileDownloadFailed(filepath, "Failed to save tile.");
    reply->deleteLater();
    mDownloads.remove(filepath);
}

bool GisQmlInt::isRedirect(QNetworkReply *reply)
{
    return !reply->attribute(QNetworkRequest::RedirectionTargetAttribute)
                  .isNull();
}

void GisQmlInt::openUrl(QNetworkReply *reply, const QString &filepath)
{
    QUrl url(reply->attribute(QNetworkRequest::RedirectionTargetAttribute)
                   .toUrl());
    if (url.isRelative())
        url = reply->url().resolved(url);
    QNetworkReply *urlReply = mNwkManager->get(QNetworkRequest(url));
    connect(urlReply, &QNetworkReply::finished, this,
            [this, filepath, urlReply] { handleReply(urlReply, filepath); });
}

void GisQmlInt::saveTile(QNetworkReply *reply, const QString &filepath)
{
    QByteArray data(reply->readAll());
    if (!(data.startsWith("\x89PNG") || data.startsWith("\xFF\xD8")))
    {
        emit tileDownloadFailed(filepath, "Invalid image file");
        return;
    }
    if (writeTile(filepath, data))
        emit tileDownloaded(filepath);
    else
        emit tileDownloadFailed(filepath, "Failed to write");
}

bool GisQmlInt::writeTile(const QString &filepath, const QByteArray &data)
{
    QFile file(filepath);
    if (!file.open(QIODevice::WriteOnly))
    {
        emit tileDownloadFailed(filepath, file.errorString());
        return false;
    }
    file.write(data);
    file.close();
    return true;
}
