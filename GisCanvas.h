/**
 * Qt map drawing canvas.
 * Draw map entities using Qt painter object. The origin is at the top left
 * corner, with Y increasing downwards.
 *
 * Copyright (C) Sapura Secured Technologies, 2013-2025. All Rights Reserved.
 *
 * @file
 * @version $Id: GisCanvas.h 1898 2025-01-17 00:50:10Z rosnin $
 * @author Muhd Hashim Wahab
 */
#ifndef GISCANVAS_H
#define GISCANVAS_H

#include <QGraphicsView>
#include <QObject>
#include <QQuickWidget>
#include <QWidget>
#include <set>

#include "DbInt.h"
#include "GisQmlInt.h"
#ifdef INCIDENT
#include "IncidentData.h"
#endif
#include "Logger.h"
#include "Props.h"

class GisCanvas : public QGraphicsView
{
    Q_OBJECT

public:
    //layer model types - key = layer name, value = eTypeId
    typedef std::map<std::string, int> LayerModelsT;

    /**
     * Constructor.
     * Creates a main GIS object and retrieves all info for display.
     *
     * @param[in] logger   App logger.
     * @param[in] userName The username. Used only if this instance contains POI.
     * @param[in] overview True for map overview.
     * @param[in] qw       QQuickWidget for QML map.
     * @param[in] parent   Parent widget, if any.
     */
    explicit GisCanvas(Logger        *logger,
                       const QString &userName,
                       bool           overview,
                       QQuickWidget  *qw,
                       QWidget       *parent = 0);

    ~GisCanvas();

    bool isValid() { return mValid; }

    /**
     * Updates map items.
     */
    void refresh();

    /**
     * Clears map data and recreates QML map component.
     */
    void refreshMap();

    /**
     * Sets geometry center on map.
     *
     * @param[in] center  The geometry center.
     * @param[in] zoomLvl The zoom level. Omit to use default.
     */
    void setGeomCenter(QPointF center, int zoomLvl = -1);

    /**
     * Sets zoom level.
     *
     * @param[in] zoomLvl The zoom level.
     */
    void setZoomLevel(double zoomLvl);

    /**
     * Sets operation mode and type ID.
     *
     * @param[in] mode   The mode.
     * @param[in] typeId The type ID.
     */
    void setMode(int mode, int typeId = GisQmlInt::TYPEID_NONE);

    /**
     * Gets the operation mode.
     *
     * @return The operation mode - GisMapAdapter/GisQmlInt::eMode.
     */
    int getMode();

    /**
     * Gets the geometry center.
     *
     * @return The geometry center.
     */
    QPointF getGeomCenter() const;

    /**
     * Gets the zoom level.
     *
     * @return The zoom level.
     */
    double getZoomLevel() const;

    const LayerModelsT &getModelTypes() { return mLayerModels; }

    /**
     * Gets a layer model type.
     *
     * @param[in] layer The layer name.
     * @return The type - eTypeId, or 0 if layer name not found.
     */
    int getModelType(const std::string &layer);

    /**
     * Updates (refreshes) all labels on a label layer.
     *
     * @param[in] typeId eTypeId layer identifier.
     */
    void updateLabelLayer(int typeId);

    /**
     * Gets the map scale.
     *
     * @return The scale.
     */
    double getMapScale() const;

    double getMinZoomLevel() const { return GisQmlInt::ZOOM_MIN; }

    double getMaxZoomLevel() const { return GisQmlInt::ZOOM_MAX; }

    /**
     * Clears the map operation data.
     */
    void clearData();

#ifdef INCIDENT
    /**
     * Sets or clears the locked incident ID.
     *
     * @param[in] id The incident ID being locked, or 0 to unlock.
     */
    void setIncidentLock(int id);

    /**
     * Adds/updates incident point.
     *
     * @param[in] data   The incident data.
     * @param[in] typeId TYPEID_INCIDENT or TYPEID_INCIDENT_REPORT.
     */
    void incidentUpdate(IncidentData *data, int typeId);

    /**
     * Deletes incident point.
     *
     * @param[in] id The ID.
     */
    void incidentClose(int id);

    /**
     * Plots incident position on map canvas.
     *
     * @param[in] lat The latitude.
     * @param[in] lon The longitude.
     */
    void incidentPlot(double lat, double lon);

    /**
     * Removes Report incidents from map canvas.
     */
    void incidentsReportClear();
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
     * Finds a terminal on the map, and centers on it if found.
     *
     * @param[in] issi The ISSI.
     */
    void terminalLocate(int issi);

    /**
     * Adds/updates terminal point.
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
     * Shows or hides terminals of a type.
     *
     * @param[in] show true to show.
     * @param[in] type The type - SubsData::eTerminalType.
     */
    void terminalsShow(bool show, int type);

    /**
     * Filters shown terminals. The filter is a list of ISSIs to be shown, while
     * others are hidden. An empty list turns off the filter, which means show
     * all. A single entry of value 0 means the filter is on but has no valid
     * ISSIs, which means hide all.
     *
     * @param[in] issis ISSI list. 0 for empty list.
     */
    void terminalsFilter(const std::set<int> *issis);

    /**
     * Updates a terminal type.
     *
     * @param[in] issi The ISSI.
     * @param[in] type The type - SubsData::TERMINALTYPE_*.
     */
    void terminalUpdateType(int issi, int type);

    /**
     * Removes one or all terminal points from map.
     *
     * @param[in] issi The ISSI, or 0 to remove all.
     */
    void terminalRemove(int issi = 0);

    /**
     * Removes terminal points from map.
     *
     * @param[in] rmList true to remove the listed ISSIs, false to remove all
     *                   except the ISSIs (i.e. exclude list).
     * @param[in] issis  The ISSIs.
     */
    void terminalRemove(bool rmList, const std::set<int> &issis);

    /**
     * Stops the terminal check timer if time settings are all 0.
     * Otherwise if the timer is inactive, performs the check immediately,
     * leading to timer restart.
     */
    void terminalCheckTimeChanged();

    /**
     * Enables or disables terminal trailing.
     *
     * @param[in] id      The terminal ID.
     * @param[in] enabled true to enable.
     */
    void terminalTrailing(int id, bool enabled);

    bool hasTerminals();

    /**
     * Gets lists of terminals on the map, and terminals with trailing
     * enabled.
     *
     * @param[out] terms Terminal ISSIs currently on the map.
     * @param[out] trail Terminal ISSIs with trailing.
     * @return true if at least one terminal on the map.
     */
    bool getTerminalTrailing(std::set<int> &terms, std::set<int> &trail);

    /**
     * Gets the icon file prefix for a terminal type.
     *
     * @param[out] type SubsData::TERMINALTYPE_*.
     * @return The prefix in lower case.
     */
    QString getTerminalIconPfx(int type);

    /**
     * Handles type cancelled event.
     *
     * @param[in] typeId The type ID - eTypeId.
     */
    void typeCancelled(int typeId);

    /**
     * Finds items or locations - either in the entire map or around a center
     * point.
     *
     * @param[in] key   The search key.
     * @param[in] ctrPt The center point, if any.
     * @param[in] label The point label, if any.
     */
    void search(const QString &key,
                const QPointF *ctrPt = 0,
                const QString &label = "");

    /**
     * Finds terminals near a point.
     *
     * @param[in] pt     The point.
     * @param[in] radius The search radius in km.
     */
    void getTerminalsNearby(const QPointF &pt, const QString &radius);

    /**
     * Adds a tracking line for an ISSI to a particular position.
     * For the first line, centers view on the position and sets the
     * appropriate zoom level.
     *
     * @param[in] issi    The ISSI.
     * @param[in] coord   The position coordinates as "latitude,longitude",
     *                    e.g. "3.189073,101.734268".
     * @param[in] isFirst true for the first line.
     */
    void addTrackingLine(int issi, const std::string &coord, bool isFirst);

    /**
     * Deletes tracking line segments for an ISSI.
     *
     * @param[in] issi The ISSI.
     * @param[in] num  The number of line segments to delete. Omit to delete
     *                 all.
     */
    void deleteTrackingLineSegments(int issi, int num = -1);

    /**
     * Sets the measurement unit and updates all measurement labels.
     *
     * @param[in] unit The unit from GisLineLabel::eMeasureUnit.
     */
    void updateMeasurements(int unit);

    /**
     * Shows or hides model icons/labels on map.
     *
     * @param[in] label true for labels, otherwise icons.
     * @param[in] show  true to show.
     * @param[in] layer The layer name.
     */
    void updateView(bool label, bool show, const QString &layer);

    /**
     * Loads POI collection to map.
     *
     * @param[in] prs The POI collection.
     */
    void poiLoad(Props::ValueMapsT &prs);

    /**
     * Loads a POI to map.
     *
     * @param[in] pr The POI properties.
     */
    void poiLoad(Props::ValueMapT &pr);

    /**
     * Removes a POI from map.
     *
     * @param[in] id The POI ID.
     */
    void poiRemove(int id);

signals:
    void dgnaAssign(ResourceData::ListModel *mdl);
#ifdef DEBUG
    void mapCoordinates(double x, double y, double scX, double scY);
#else
    void mapCoordinates(double x, double y);
#endif
    void mapLoadComplete();
    void mapRectChanged(double left, double top, double right, double bottom);
    void modeSet();
    void poiDelete(int id);
    void pointCoordinates(int typeId, double x, double y);
    void poiSave(Props::ValueMapT *pr);
#ifdef INCIDENT
    void resourceAssign(ResourceData::ListModel *mdl);
    void showIncident(int id);
#endif
    void searchNearby(double lat, double lon, const QString &label);
    void startAction(int actType, int idType, const ResourceData::IdsT &ids);
    void viewChanged(QPointF center, double zoomLevel);
    void zoomChanged(double zoomLevel);

public slots:
#ifdef GIS_ROUTING
    /**
     * Gets navigation routes.
     *
     * @param[in] srcLat The source location latitude.
     * @param[in] srcLon The source location longitude.
     * @param[in] dstLat The destination latitude.
     * @param[in] dstLon The destination longitude.
     */
    void onRouting(double srcLat, double srcLon, double dstLat, double dstLon);

    /**
     * Clears routing result.
     */
    void onRoutingClear();
#endif

    /**
     * Handles dialog box close event.
     *
     * @param[in] pr The point properties, 0 if dialog canceled.
     */
    void onDialogFinished(Props::ValueMapT *pr);

    /**
     * Draws a map view rectangle.
     *
     * @param[in] left   The left coordinate.
     * @param[in] top    The top coordinate.
     * @param[in] right  The right coordinate.
     * @param[in] bottom The bottom coordinate.
     */
    void onMapRectChanged(double left,
                          double top,
                          double right,
                          double bottom);

    /**
     * Enables/disables radius circles drawing.
     *
     * @param[in] enabled true to enable.
     */
    void onMeasurementCircle(bool enabled);

    /**
     * Clears all measurement display.
     */
    void onMeasurementClear();

    /**
     * Sets the search radius.
     *
     * @param[in] radius The radius in km.
     */
    void onSrchRadiusChanged(const QString &radius);

private slots:
    /**
     * Initializes layer model types and emits signal to notify map loaded.
     */
    void onMapLayerLoaded();

    /**
     * Handles resource selection on map for DGNA or Incident assignment.
     *
     * @param[in] rscList Resource ID list.
     */
    void onRscSelect(QVariant rscList);

    /**
     * Handles search results using showSearchRes().
     *
     * @param[in] key     The search key string.
     * @param[in] resList Result list.
     */
    void onSearchDone(QVariant key, QVariant resList);

    /**
     * Handles search results around a center point using showSearchRes().
     *
     * @param[in] key     The search key string.
     * @param[in] resList Result list.
     * @param[in] ctr     Search center coordinates.
     * @param[in] lbl     Search center label.
     */
    void onSearchCtrDone(QVariant       key,
                         QVariant       resList,
                         QPointF        ctr,
                         const QString &lbl);

    /**
     * Centers the map on an item at a specific zoom level, or shows an error
     * dialog if the item does not exist.
     *
     * @param[in] prs   The item properties.
     * @param[in] id    The ID. Ignored if item is non-zero.
     * @param[in] layer The layer name.
     */
    void onShowItem(Props::ValueMapT   prs,
                    const std::string &id,
                    const std::string &layer);

    /**
     * Shows and handles context menu.
     *
     * @param[in] pos     The current position.
     * @param[in] itmList Items at current position.
     */
    void onContextMenu(const QPointF &pos, QVariant itmList);

private:
    bool                   mOverview;
    bool                   mValid;
#ifdef INCIDENT
    int                    mLockedIncidentId;
#endif
    int                    mTypeId;
    int                    mSrchRadius; //nearby search radius in km
    DbInt::Int2StringMapT  mTerminalTypes;
    QString                mUserName;
    QObject               *mMap;
    LayerModelsT           mLayerModels;
    Logger                *mLogger;
    Props::ValueMapT       mProps;

    /**
     * Displays search results.
     *
     * @param[in] key The search key.
     * @param[in] res Results.
     * @param[in] ctr Search center coordinates, if any.
     * @param[in] lbl Search center label, if any.
     */
    void showSearchRes(const QString &key,
                       QStringList    res,
                       QPointF       *ctr = 0,
                       const QString *lbl = 0);

    /**
     * Checks a layer's visibility, and shows an error dialog if not visible.
     *
     * @param[in] layer The layer.
     * @return true if visible.
     */
    bool checkLayerVisible(int layer);

    /**
     * Creates a graphics item at a given location.
     *
     * @param[in] typeId The item type ID - eTypeId.
     * @param[in] lat    The latitude.
     * @param[in] lon    The longitude.
     * @return true if successful.
     */
    bool createItem(int typeId, double lat, double lon);

    /**
     * Displays item dialog.
     */
    void showDialog();
};
#endif //GISCANVAS_H
