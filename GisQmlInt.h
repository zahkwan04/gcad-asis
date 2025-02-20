/**
 * Qt QML map interface.
 *
 * Copyright (C) Sapura Secured Technologies, 2023-2024. All Rights Reserved.
 *
 * @file
 * @version $Id: GisQmlInt.h 1894 2024-12-20 07:15:28Z rosnin $
 * @author Rosnin Mustaffa
 */
#ifndef GISQMLINT_H
#define GISQMLINT_H

#include <QFile>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QObject>
#include <QtPositioning>

#include "GisLocation.h"
#include "ResourceData.h"
#include "Style.h"

class GisQmlInt : public QObject
{
    Q_OBJECT
public:
    static const double ZOOM_MAX;
    static const double ZOOM_MIN;
    static const double ZOOM_SEARCH;
    static const double ZOOM_TRACK;

#ifdef INCIDENT
    static const std::string KEY_INCIDENT;
    static const std::string KEY_INCIDENT_REPORT;
#endif
    static const std::string KEY_MEASURE;
#ifdef GIS_ROUTING
    static const std::string KEY_ROUTE;
#endif
    static const std::string KEY_SEADEPTH;
    static const std::string KEY_SEAMARKS;
    static const std::string KEY_SEARCH;
    static const std::string KEY_TERMINAL;
    static const std::string KEY_TRACKING;
    static const std::string KEY_TRAILING;
    static const std::string KEY_USERPOI;
    static const std::string SYS_ATTR_VALUE;

    static const time_t TERMINAL_TIME_THRESHOLD;

    enum eMode
    {
        MODE_MEASURE,
        MODE_RESOURCES,
        MODE_ROUTE,
        MODE_SELECT,
        MODE_ZOOM
    };
    Q_ENUM(eMode)

    //keys for operation item
    enum eTypeId
    {
        TYPEID_SEARCH          = -1,
        TYPEID_MEASURE         = -2,
        TYPEID_TRACKING        = -3,
        TYPEID_NONE            = -4,
        TYPEID_INCIDENT        = -5,
        TYPEID_INCIDENT_REPORT = -6,
        TYPEID_POI             = -7,
        TYPEID_ROUTE_END       = -8,
        TYPEID_ROUTE_RESULT    = -9,
        TYPEID_ROUTE_START     = -10,
        TYPEID_RSC_DGNA        = -11,
        TYPEID_RSC_INCIDENT    = -12,
        TYPEID_TERMINAL        = -13,
        TYPEID_TRAILING        = -14,
        //these 2 must be last with the lowest values, because the layers may be
        //added/removed by user
        TYPEID_SEAMARKS        = -15,
        TYPEID_SEADEPTH        = -16
    };
    Q_ENUM(eTypeId)

    enum eMeasureUnit
    {
        //each must have a corresponding entry in GisWindow::init()
        MEASURE_UNIT_KM,            //default must be first
        MEASURE_UNIT_MILE,
        MEASURE_UNIT_NAUTICAL_MILE,
        MEASURE_UNIT_LAST           //marker only
    };
    Q_ENUM(eMeasureUnit)

    /**
     * Constructor.
     * Creates GIS interface to QML.
     *
     * @param[in] parent Parent widget, if any.
     */
    explicit GisQmlInt(QObject *parent = 0) :
    QObject(parent), mNwkManager(new QNetworkAccessManager(this)) {}

    /**
     * Gets the resource map label.
     *
     * @param[in] issi The ISSI.
     * @return The label text.
     */
    Q_INVOKABLE QString rscLbl(int issi)
    {
        return QString::fromStdString(ResourceData::getMapSubsLbl(issi));
    }

    /**
     * Checks whether map resources option is set to display name.
     *
     * @return true if set to display name.
     */
    Q_INVOKABLE bool rscShowName()
    {
        return (ResourceData::getMapSubsDspOpt() != ResourceData::DSP_OPT_SSI);
    }

    /**
     * Gets the predefined rectangular geographic area.
     *
     * @return The area.
     */
    Q_INVOKABLE QGeoRectangle getGeoBounds()
    {
        return QGeoRectangle(QGeoCoordinate(GisLocation::BOUND_TOP,
                                            GisLocation::BOUND_LEFT),
                             QGeoCoordinate(GisLocation::BOUND_BTM,
                                            GisLocation::BOUND_RIGHT));
    }

    /**
     * Gets a circular geographic area.
     *
     * @param[in] lat Center latitude.
     * @param[in] lon Center longitude.
     * @param[in] rad Radius in meters.
     * @return The area.
     */
    Q_INVOKABLE QGeoCircle getGeoBoundsCircle(double lat, double lon, double rad)
    {
        return QGeoCircle(QGeoCoordinate(lat, lon), rad);
    }

    Q_INVOKABLE double getZoomMax() { return ZOOM_MAX; }

    Q_INVOKABLE double getZoomMin() { return ZOOM_MIN; }

    Q_INVOKABLE bool fileExists(const QString &fileName)
    {
        return QFile::exists(fileName);
    }

    /**
     * Starts tile downloading process and saves to file if successful.
     *
     * @param[in] url      Tile server URL.
     * @param[in] filepath The filepath.
     */
    Q_INVOKABLE void downloadTile(const QString &url, const QString &filepath);

    /**
     * Clears seadepth and seamark tiles.
     */
    Q_INVOKABLE void clearTiles();

    Q_INVOKABLE bool isDarkMode()
    {
        return (Style::getTheme() == Style::THEME_DARK);
    }

    /**
     * Gets cached data folder path.
     *
     * @return The path.
     */
    Q_INVOKABLE static QString getCachePath();

    /**
     * Gets a layer model name.
     *
     * @param[in] typeId The type - eTypeId.
     * @return The layer model name.
     */
    Q_INVOKABLE static QString getModelName(int typeId);

    /**
     * Sets the cursor.
     *
     * @param[in] mode The mode - eMode.
     */
    Q_INVOKABLE static void setMouseCursor(int mode = MODE_SELECT);

    Q_INVOKABLE QString getMapPath() { return sMapPath; }

    static bool setMapPath(const QString &path);

signals:
    void tileDownloaded(const QString &filePath);
    void tileDownloadFailed(const QString &url, const QString &err);

private:
    QNetworkAccessManager *mNwkManager;
    QSet<QString>          mDownloads; //download tiles filepath list

    static QString sMapPath;

    /**
     * Handles network reply for tile download request. If available, saves
     * tile to file.
     *
     * @param[in] reply    The reply from network.
     * @param[in] filepath The filepath.
     */
    void handleReply(QNetworkReply *reply, const QString &filepath);

    /**
     * Checks whether network reply contains redirect URL.
     *
     * @param[in] reply The reply from network.
     * @return true if reply contains redirect URL.
     */
    bool isRedirect(QNetworkReply *reply);

    /**
     * Handles URL redirection.
     *
     * @param[in] reply    The reply from network.
     * @param[in] filepath The filepath.
     */
    void openUrl(QNetworkReply *reply, const QString &filepath);

    /**
     * Saves downloaded tile to file. Accepts only "*.png", discards others.
     *
     * @param[in] reply    The reply from network.
     * @param[in] filepath The filepath.
     */
    void saveTile(QNetworkReply *reply, const QString &filepath);

    /**
     * Writes tile data to file.
     *
     * @param[in] filepath The filepath.
     * @param[in] data     The binary data.
     * @return true if file is successfully created.
     */
    bool writeTile(const QString &filepath, const QByteArray &data);
};
#endif // GISQMLINT_H
