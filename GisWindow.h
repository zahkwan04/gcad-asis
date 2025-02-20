/**
 * The Map UI module.
 *
 * Copyright (C) Sapura Secured Technologies, 2014-2025. All Rights Reserved.
 *
 * @file
 * @version $Id: GisWindow.h 1898 2025-01-17 00:50:10Z rosnin $
 * @author Muhd Hashim Wahab
 */
#ifndef GISWINDOW_H
#define GISWINDOW_H

#include <map>
#ifdef DEBUG
#include <sstream>
#endif
#include <qquickwidget.h>
#include <QLineEdit>
#include <QMainWindow>
#include <QPointF>
#include <QQuickItem>
#include <QQuickView>

#include "Dgna.h"
#include "GisBookmarks.h"
#include "GisCanvas.h"
#include "GisDgnaList.h"
#include "GisHistory.h"
#include "GisLayerList.h"
#include "GisLegend.h"
#include "GisMapOverview.h"
#include "GisRouting.h"
#include "GisTracking.h"
#ifdef INCIDENT
#include "IncidentData.h"
#endif
#include "Logger.h"
#include "Poi.h"
#include "ResourceData.h"
#include "Resources.h"

namespace Ui {
class GisWindow;
}

class GisWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * Constructor.
     *
     * @param[in] poi       Poi object.
     * @param[in] resources Resources object.
     * @param[in] dgna      Dgna object.
     * @param[in] logger    App logger.
     * @param[in] userName  The username.
     * @param[in] parent    Parent widget, if any.
     */
    explicit GisWindow(Poi           *poi,
                       Resources     *resources,
                       Dgna          *dgna,
                       Logger        *logger,
                       const QString &userName,
                       QWidget       *parent = 0);

    ~GisWindow();

    /**
     * Applies color theme to UI components.
     */
    void setTheme();

    /**
     * Updates displayed map version and clears map data.
     */
    void refreshMap();

    QString getVersion() { return sMapVersion; }

    /**
     * Refreshes the map operation data.
     */
    void refreshData();

    /**
     * Clears the map operation data.
     */
    void clearData();

    /**
     * Resets to selection mode.
     */
    void modeReset();

#ifdef INCIDENT
    /**
     * Changes mode to select incident resources.
     */
    void modeResources();

    /**
     * Finds resources near a point and emits resourceAssign() with the result.
     *
     * @param[in] pt     The point.
     * @param[in] radius The search radius in km.
     */
    void selectResources(const QPointF &pt, const QString &radius);

    /**
     * Sets or clears the locked incident ID.
     *
     * @param[in] id The incident ID being locked, or 0 to unlock.
     */
    void incidentLock(int id);

    /**
     * Adds/updates an incident point.
     *
     * @param[in] data       The incident data.
     * @param[in] fromReport true for a Report incident.
     */
    void incidentUpdate(IncidentData *data, bool fromReport = false);

    /**
     * Deletes an incident point.
     *
     * @param[in] id The incident ID.
     */
    void incidentClose(int id);

    /**
     * Triggers incident plot on the map canvas.
     *
     * @param[in] lat The latitude.
     * @param[in] lon The longitude.
     */
    void incidentPlot(double lat, double lon);
#endif //INCIDENT

    /**
     * Starts/stops resource icon highlight on map
     *
     * @param[in] issi  The ISSI.
     * @param[in] start true to start.
     */
    void setRscInCall(int issi, bool start);

    /**
     * Enables/disables automatic map pan to center on the last highlighted
     * resource in call.
     *
     * @param[in] enable true to enable.
     */
    void setCtrRscInCall(bool enable);

    /**
     * Enables/disables seamap layers (SEADEPTH and SEAMARKS).
     *
     * @param[in] on true to enable.
     */
    void enableSeaMap(bool on);

    /**
     * Adds/updates a terminal point.
     *
     * @param[in] isValid   The validity.
     * @param[in] issi      The ISSI.
     * @param[in] lon       The longitude.
     * @param[in] lat       The latitude.
     * @param[in] timestamp Location update timestamp.
     */
    void terminalUpdate(bool               isValid,
                        int                issi,
                        double             lon,
                        double             lat,
                        const std::string &timestamp);

    /**
     * Updates a terminal type.
     *
     * @param[in] issi The ISSI.
     * @param[in] type The type - SubsData::TERMINALTYPE_*.
     */
    void terminalUpdate(int issi, int type);

    /**
     * Removes one or all terminal points.
     *
     * @param[in] issi The ISSI, or 0 to remove all.
     */
    void terminalRemove(int issi = 0);

    /**
     * Removes terminal points.
     *
     * @param[in] rmList true to remove the listed ISSIs, false to remove all
     *                   except the ISSIs (i.e. exclude list).
     * @param[in] issis  The ISSIs.
     */
    void terminalRemove(bool rmList, const std::set<int> &issis);

    /**
     * If the group terminals filter is enabled, reruns it because of a change
     * in group attachments, but only if at least one of the affected groups is
     * in the filter.
     *
     * @param[in] gssis The affected GSSIs.
     */
    void terminalsFilterUpdate(const std::set<int> &gssis);

    /**
     * Updates a POI.
     *
     * @param[in] id The POI ID.
     */
    void poiLoad(int id);

    /**
     * Removes a POI.
     *
     * @param[in] id The POI ID.
     */
    void poiRemove(int id);

    const QString &getUserName() const { return mUserName; }

    /**
     * Sets map version and map plugin URL path.
     *
     * @param[in] ver  Map version.
     * @param[in] path Tile server URL address.
     * @return true if version or URL differs.
     */
    static bool setMapInfo(const QString &ver, const QString &path);

    /**
     * Sets a new sTheme value based on current theme from Style, with a special
     * indicator if this theme change is from dark to light or vice versa.
     * For use only before GisWindow is created.
     */
    static void setNewTheme();

signals:
    void gpsMon();
    void locationUpdate(bool        isValid,
                        int         issi,
                        double      lon,
                        double      lat,
                        std::string timestamp);
    void mainMapLoaded();
    void pointCoordinates(int typeId, double x, double y);
#ifdef INCIDENT
    void incCoordinates(double x, double y);
    void resourceAssign(ResourceData::ListModel *mdl);
    void showIncident(int id);
#endif
    void startAction(int actType, int idType, const ResourceData::IdsT &ids);

public slots:
    /**
     * Handles main map loaded event.
     */
    void onMainMapLoaded();

    /**
     * Gets GisCanvas to handle a change in terminal label option.
     */
    void onTerminalLblOptChanged();

    /**
     * Gets GisCanvas to locate a terminal on the map.
     *
     * @param[in] issi The ISSI.
     */
    void onTerminalLocate(int issi);

    /**
     * Gets GisCanvas to handle a change in terminal check timer settings.
     */
    void onTerminalCheckTimeChanged();

    /**
     * Switches to search nearby mode.
     *
     * @param[in] lat   The latitude.
     * @param[in] lon   The longitude.
     * @param[in] label The search center label, if any.
     */
    void onSearchNearby(double lat, double lon, const QString &label);

#ifdef INCIDENT
    /**
     * Removes Report incidents from map.
     */
    void onClearReportIncidents();
#endif

    /**
     * Shows point coordinates.
     *
     * @param[in] typeId The type ID.
     * @param[in] x      X coordinate.
     * @param[in] y      Y coordinate.
     */
    void onCoordReceived(int typeId, double x, double y);

    /**
     * Zooms to a location.
     *
     * @param[in] lat The latitude.
     * @param[in] lon The longitude.
     */
    void onZoomTo(double lat, double lon);

    /**
     * Updates zoom level.
     *
     * @param[in] zoomLvl The zoom level.
     */
    void updateZoomLevel(double zoomLvl);

protected:
    //override
    void hideEvent(QHideEvent *event);

    //override
    void showEvent(QShowEvent *event);

    //override
    void resizeEvent(QResizeEvent *event);

private:
    typedef std::map<int, QString> UnitMapT;

    Ui::GisWindow            *ui;
    Logger                   *mLogger;
    Poi                      *mPoi;
    GisCanvas                *mMapCanvas;
    GisBookmarks             *mBookmarks;
    GisDgnaList              *mDgnaList;
    GisHistory               *mHistory;
    GisLayerList             *mLayers;
    GisLegend                *mLegend;
    GisMapOverview           *mOverviewMap;
    GisRouting               *mRoute;
    GisTracking              *mTracking;
    QPointF                  *mSearchPt; //'search nearby' center point
    QString                   mUserName;
    ResourceData::IdsT        mFilterGssis; //selected grps for terminals filter
    std::map<QWidget *, int>  mWdgIdxMap; //index in splitter1 for each widget

    static int      sMeasureUnit;            //selected measurement unit
    static int      sTheme;                  //current theme
    static QString  sMapVersion;
    static UnitMapT sUnitMap;                //measurement units to names map

    /**
     * Sets the displayed map scale value.
     */
    void setScale();

    /**
     * Stops previous mode and sets to a new one.
     *
     * @param[in] mode   The new mode.
     * @param[in] typeId The new mode type ID.
     */
    void resetToMode(int mode, int typeId = GisQmlInt::TYPEID_NONE);

    /**
     * Sets style to tool buttons.
     *
     * @param[in] mode The current mode - GisMapAdapter::eMode.
     */
    void setButtonStyles(int mode);

    /**
     * Sets the group terminals filter.
     *
     * @param[in] enable true to enable filter.
     */
    void terminalsFilter(bool enable);

    /**
     * Reloads map with new plugin.
     */
    void reloadMap();

    /**
     * Gets the measurement unit name.
     *
     * @param[in] unit A value from eMeasureUnit.
     * @return The unit name, or empty string for invalid unit.
     */
    static const QString &getMeasurementUnit(int unit);
};
#endif //GISWINDOW_H
