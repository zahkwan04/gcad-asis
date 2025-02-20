/**
 * QML main map implementation.
 *
 * Copyright (C) Sapura Secured Technologies, 2023-2025. All Rights Reserved.
 *
 * @file
 * @version $Id: Canvas.qml 1898 2025-01-17 00:50:10Z rosnin $
 * @author Rosnin
 */
import QtLocation 5.12
import QtPositioning 5.12
import QtQuick 2.9
import QtQuick.Window 2.3

import gisInt 1.0

Item
{
    signal customContextMenuRequested(point coor, variant itmList);
    signal mapLayerLoaded();
    signal mapViewChanged(point ctr, double zl);
    signal mapZoomChanged(double value);
    signal mouseMoved(double lon, double lat);
    signal mouseMovedDbg(double lon, double lat, double x, double y);
    signal overviewRectChanged(double top,
                               double left,
                               double btm,
                               double right);
    signal pointCoordinates(int type, double lon, double lat);
    signal rscSelect(variant rscList);
    signal searchCtrDone(variant key, variant resList, point ctr, string lbl);
    signal searchDone(variant key, variant resList);

    readonly property int     cPRECISION  : 3;
    readonly property double  cZOOM_DEF   : 6.0;
    readonly property double  cZOOM_OV    : 5.2;
    readonly property double  cZOOM_SEARCH: 15.0;
    readonly property string  cLIGHT      : "/light/";
    readonly property string  cNIGHT      : "/night/";
    readonly property variant cMainCtr    : QtPositioning.coordinate(
                                                          4.052387, 108.013738);
    readonly property variant cOvCtr      : QtPositioning.coordinate(
                                                            4.13085, 109.45045);

    property bool mDebug        : false;
    property bool mOverview     : false;
    property bool mSeaMap       : false;
    property bool mCtrRscInCall : true;
    //layer filter
    property bool mImgIncRptView: true;
    property bool mImgIncView   : true;
    property bool mImgMeaView   : true;
    property bool mImgPinView   : true;
    property bool mImgPoiView   : true;
    property bool mImgRscView   : true;
    property bool mImgRteView   : true;
    property bool mImgTrcView   : true;
    property bool mImgTrlView   : true;
    property bool mLblIncRptView: true;
    property bool mLblIncView   : true;
    property bool mLblMeaView   : true;
    property bool mLblPinView   : true;
    property bool mLblPoiView   : true;
    property bool mLblRscView   : true;
    property bool mLblRteView   : true;
    property bool mLyrSeaDepth  : true;
    property bool mLyrSeaMarker : true;
    property bool mLblTrcView   : true;

    property int  mMeasureUnit  : 0;
    property int  mMode         : GisQmlInt.MODE_SELECT;
    property int  mStale1       : 0;
    property int  mStaleLast    : 0;

    property double mZoomMax    : mGisInt.getZoomMax();
    property double mZoomMin    : mGisInt.getZoomMin();

    property variant mResLst;
    property variant mRscFilter : [];
    //terminal type visibility filter - indexed by SubsData::eTerminalType
    property variant mRscShow   : [ true, true, true, true, true, true,
                                    true, true, true, true, true ];

    property Map mMap           : null;

    onMOverviewChanged:
    {
        if (mMap != null)
            mMap.mOvr = mOverview;
    }

    Component.onCompleted: createMap();

    GisQmlInt
    {
        id: mGisInt;

        onTileDownloaded:
            console.log("Tile download finished, saved to: " + filePath);

        onTileDownloadFailed:
            console.log("Tile download failed: " + url + "\nError: " + error);
    }

    Loader { id: mLoader; anchors.fill: parent; }

    ListModel //sea depth tile model
    {
        id: mSeaDepthModel; onDataChanged: { mMap.update(); }
    }

    ListModel //sea marker tile model
    {
        id: mSeaMarkerModel; onDataChanged: { mMap.update(); }
    }

    ListModel //user POI list model
    {
        id: mPoiModel; onDataChanged: { mMap.update(); }
    }

    ListModel //incident list model
    {
        id: mIncModel; onDataChanged: { mMap.update(); }
    }

    ListModel //incident report list model
    {
        id: mIncRptModel; onDataChanged: { mMap.update(); }
    }

    ListModel //resource list model
    {
        id: mResModel; onDataChanged: { mMap.update(); }
    }

    ListModel //tracking model data
    {
        id: mTrackModel; onDataChanged: { mMap.update(); }
    }

    ListModel //trailing list model
    {
        id: mTrailModel; onDataChanged: { mMap.update(); }
    }

    ListModel //measure list model
    {
        id: mMeaModel; onDataChanged: { mMap.update(); }
    }

    ListModel //routing icon list
    {
        id: mRouteIcnModel; onDataChanged: { mMap.update(); }
    }

    ListModel //pin model
    {
        id: mSearchModel; onDataChanged: { mMap.update(); }
    }

    RouteModel
    {
        id    : mRouteModel;
        plugin: mMap.plugin;
        query : RouteQuery { id: mRouteQuery }

        property bool srcPt: false; //true for route source point
    }

    GeocodeModel //Geocode search model
    {
        id    : mGeocodeModel;
        plugin: mMap.plugin;
        //should be unlimited with default -1, but that causes limit 10 instead
        limit : 100;

        property bool   ctrPt: false; //true for search around center point
        property double ctrLat;       //center point
        property double ctrLon;

        onStatusChanged:
        {
            if (status !== GeocodeModel.Ready)
                return;
            if (count > 0)
            {
                let d;
                let s;
                let i = count - 1;
                for (; i>=0; --i)
                {
                    d = get(i);
                    s = mGisInt.getModelName(GisQmlInt.TYPEID_SEARCH) + ";0;" +
                        d.coordinate.latitude + ";" + d.coordinate.longitude +
                        ";" + d.address.text;
                    if (ctrPt)
                        s += ";" +
                             getDistance(ctrLat, ctrLon, d.coordinate.latitude,
                                         d.coordinate.longitude)
                                 .toFixed(cPRECISION);
                    mResLst.push(s);
                }
            }
            if (ctrPt)
                searchCtrDone(query, mResLst, toPoint(ctrLon, ctrLat),
                              display.text);
            else
                searchDone(query, mResLst);
        }
    }

    /**
     * Converts latitude and longitude values to a QGeoCoordinate representation
     * of a geographic position.
     *
     * @param[in] lat The latitude.
     * @param[in] lon The longitude.
     * @return The QGeoCoordinate.
     */
    function toGeoCoordinate(lat, lon)
    {
        return QtPositioning.coordinate(lat, lon);
    }

    /**
     * Converts to point coordinates.
     *
     * @param[in] x The X value.
     * @param[in] y The Y value.
     * @return The point.
     */
    function toPoint(x, y)
    {
        return Qt.point(x, y);
    }

    /**
     * Gets the distance between two coordinates.
     *
     * @param[in] fromLat The start latitude.
     * @param[in] fromLon The start longitude.
     * @param[in] toLat   The end latitude.
     * @param[in] toLon   The end longitude.
     * @return The distance in kilometers.
     */
    function getDistance(fromLat, fromLon, toLat, toLon)
    {
        return toGeoCoordinate(fromLat, fromLon)
               .distanceTo(toGeoCoordinate(toLat, toLon))/1000.0;
    }

    function getMode() { return mMode; }

    /**
     * Sets map to Select mode.
     */
    function setSelectTl()
    {
        mMode = GisQmlInt.MODE_SELECT;
        mMap.mGesture = true;
    }

    /**
     * Sets map to Zoom mode.
     */
    function setZoomTl()
    {
        mMode = GisQmlInt.MODE_ZOOM;
        mMap.mGesture = false;
    }

    /**
     * Sets map to Measure mode.
     */
    function setMeasureTl()
    {
        mMode = GisQmlInt.MODE_MEASURE;
        mMap.mGesture = true;
    }

    /**
     * Adds/updates route waypoint.
     *
     * @param[in] isSrc true for source point, otherwise destination.
     * @param[in] lat   The latitude.
     * @param[in] lon   The longitude.
     */
    function routeUpdate(isSrc: bool, lat: double, lon: double)
    {
        let idx = findItem(mRouteIcnModel, (isSrc)? 0: 1);
        if (idx < 0)
        {
            mRouteIcnModel.append({ id    : (isSrc)? 0: 1,
                                    lat   : lat,
                                    lon   : lon,
                                    imgSrc: (isSrc)? mMap.cICON_ROUTE_START:
                                                     mMap.cICON_ROUTE_END,
                                    txt   : lat.toFixed(cPRECISION) + "," +
                                            lon.toFixed(cPRECISION) });
        }
        else
        {
            mRouteIcnModel.set(idx,
                               { lat: lat,
                                 lon: lon,
                                 txt: lat.toFixed(cPRECISION) + "," +
                                      lon.toFixed(cPRECISION) });
        }
    }

    /**
     * Sets map to Route mode.
     */
    function setRoute(start: bool)
    {
        mMode = GisQmlInt.MODE_ROUTE;
        mRouteModel.srcPt = start;
    }

    /**
     * Sets route waypoints and updates the routing.
     *
     * @param[in] srcLat Source latitude.
     * @param[in] srcLon Source longitude.
     * @param[in] dstLat Destination latitude.
     * @param[in] dstLon Destination longitude.
     */
    function onRouting(srcLat: double,
                       srcLon: double,
                       dstLat: double,
                       dstLon: double)
    {
        mRouteQuery.clearWaypoints(); //clear previous routing
        mRouteQuery.addWaypoint(toGeoCoordinate(srcLat, srcLon));
        routeUpdate(true, srcLat, srcLon);
        mRouteQuery.addWaypoint(toGeoCoordinate(dstLat, dstLon));
        routeUpdate(false, dstLat, dstLon);
        mRouteQuery.travelModes = RouteQuery.CarTravel;
        mRouteQuery.routeOptimizations = RouteQuery.FastestRoute;
        mRouteModel.update();
    }

    /**
     * Sets map to resource selection mode.
     */
    function setRscMode()
    {
        mMode = GisQmlInt.MODE_RESOURCES;
        mMap.mGesture = false;
    }

    /**
     * Sets geometry center on map.
     *
     * @param[in] lat The latitude.
     * @param[in] lon The longitude.
     * @param[in] zl  The zoom level.
     */
    function setGeomCenter(lat: double, lon: double, zl: int)
    {
        if (zl >= mZoomMin && zl <= mZoomMax)
            mMap.mZoom = zl;
        else
            mMap.mZoom = cZOOM_DEF;
        mMap.center = toGeoCoordinate(lat, lon);
        mapZoomChanged(mMap.mZoom);
    }

    /**
     * Gets the geometry center.
     *
     * @return The geometry center.
     */
    function getGeomCenter()
    {
        return (toPoint(mMap.center.longitude, mMap.center.latitude));
    }

    /**
     * Gets the geometry center latitude.
     *
     * @return The geometry center latitude.
     */
    function getGeomCenterLat()
    {
        return mMap.center.latitude;
    }

    /**
     * Sets the zoom level.
     *
     * @param[in] zl The zoom level.
     */
    function setZoomLevel(zl: double)
    {
        if (zl >= mZoomMin && zl <= mZoomMax)
            mMap.mZoom = zl;
        else
            mMap.mZoom = cZOOM_DEF;
        mMap.updateFocus();
        let c = mMap.center;
        mapViewChanged(toPoint(c.longitude, c.latitude), mMap.mZoom);
    }

    /**
     * Gets the zoom level.
     *
     * @return The zoom level.
     */
    function getZoomLevel()
    {
        return mMap.zoomLevel;
    }

    /**
     * Shows or hides model label.
     *
     * @param[in] type Model type - GisQmlInt::eTypeId.
     * @param[in] show true to show.
     */
    function updateLabelView(type: int, show: bool)
    {
        switch (type)
        {
            case GisQmlInt.TYPEID_INCIDENT:
                mLblIncView = show;
                break;
            case GisQmlInt.TYPEID_INCIDENT_REPORT:
                mLblIncRptView = show;
                break;
            case GisQmlInt.TYPEID_MEASURE:
                mLblMeaView = show;
                break;
            case GisQmlInt.TYPEID_POI:
                mLblPoiView = show;
                break;
            case GisQmlInt.TYPEID_ROUTE_RESULT:
                mLblRteView = show;
                break;
            case GisQmlInt.TYPEID_SEARCH:
                mLblPinView = show;
                break;
            case GisQmlInt.TYPEID_TERMINAL:
                mLblRscView = show;
                break;
            case GisQmlInt.TYPEID_TRACKING:
                mLblTrcView = show;
                break;
            default:
                return; //do nothing
        }
        mMap.update();
    }

    /**
     * Shows or hides model images.
     *
     * @param[in] type Model type - GisQmlInt::eTypeId.
     * @param[in] show true to show.
     */
    function updateIconView(type: int, show: bool)
    {
        switch (type)
        {
            case GisQmlInt.TYPEID_INCIDENT:
                mImgIncView = show;
                break;
            case GisQmlInt.TYPEID_INCIDENT_REPORT:
                mImgIncRptView = show;
                break;
            case GisQmlInt.TYPEID_MEASURE:
                mImgMeaView = show;
                break;
            case GisQmlInt.TYPEID_POI:
                mImgPoiView = show;
                break;
            case GisQmlInt.TYPEID_ROUTE_RESULT:
                mImgRteView = show;
                break;
            case GisQmlInt.TYPEID_SEADEPTH:
                mLyrSeaDepth = show;
                mMap.updateSeaTiles(mSeaDepthModel, true);
                break;
            case GisQmlInt.TYPEID_SEAMARKS:
                mLyrSeaMarker = show;
                mMap.updateSeaTiles(mSeaMarkerModel, false);
                break;
            case GisQmlInt.TYPEID_SEARCH:
                mImgPinView = show;
                break;
            case GisQmlInt.TYPEID_TERMINAL:
                mImgRscView = show;
                break;
            case GisQmlInt.TYPEID_TRACKING:
                mImgTrcView = show;
                break;
            case GisQmlInt.TYPEID_TRAILING:
                mImgTrlView = show;
                break;
            default:
                return; //do nothing
        }
        mMap.update();
    }

    /**
     * Checks a model's visibility.
     *
     * @param[in] type Model type - GisQmlInt::eTypeId.
     * @return true if visible.
     */
    function checkModelVisible(type: int)
    {
        switch (type)
        {
            case GisQmlInt.TYPEID_INCIDENT:
                return mImgIncView;
            case GisQmlInt.TYPEID_MEASURE:
                return mImgMeaView;
            case GisQmlInt.TYPEID_POI:
                return mImgPoiView;
            case GisQmlInt.TYPEID_ROUTE_RESULT:
                return mImgRteView;
            case GisQmlInt.TYPEID_SEARCH:
                return mImgPinView;
            case GisQmlInt.TYPEID_TERMINAL:
                return mImgRscView;
            case GisQmlInt.TYPEID_TRACKING:
                return mImgTrcView;
            case GisQmlInt.TYPEID_TRAILING:
                return mImgTrlView;
            default:
                return false;
        }
    }

    /**
     * Checks for model item on the map.
     *
     * @return true if at least one item present.
     */
    function hasItem(type: int)
    {
        switch (type)
        {
            case GisQmlInt.TYPEID_INCIDENT:
                return (mIncModel.count > 0);
            case GisQmlInt.TYPEID_INCIDENT_REPORT:
                return (mIncRptModel.count > 0);
            case GisQmlInt.TYPEID_MEASURE:
                return (mMeaModel.count > 0);
            case GisQmlInt.TYPEID_POI:
                return (mPoiModel.count > 0);
            case GisQmlInt.TYPEID_ROUTE_RESULT:
                return (mRouteIcnModel.count > 0);
            case GisQmlInt.TYPEID_SEARCH:
                return (mSearchModel.count > 0);
            case GisQmlInt.TYPEID_TERMINAL:
                return (mResModel.count > 0);
            case GisQmlInt.TYPEID_TRACKING:
                return (mTrackModel.count > 0);
            case GisQmlInt.TYPEID_TRAILING:
                return (mTrailModel.count > 0);
            default:
                return false;
        }
    }

    /**
     * Finds an item in a model.
     *
     * @param[in] model The search model.
     * @param[in] id    The ID.
     * @return The item index, or -1 if not found.
     */
    function findItem(model: ListModel, id: int)
    {
        let i = model.count - 1;
        for (; i>=0; --i)
        {
            if (model.get(i).id === id)
                return i;
        }
        return -1;
    }

    /**
     * Deletes an item in a model.
     *
     * @param[in] model The model.
     * @param[in] id    The ID.
     */
    function deleteItem(type: int, id: int)
    {
        let model;
        switch (type)
        {
            case GisQmlInt.TYPEID_INCIDENT:
                model = mIncModel;
                break;
            case GisQmlInt.TYPEID_POI:
                model = mPoiModel;
                break;
            case GisQmlInt.TYPEID_TERMINAL:
                model = mResModel;
                break;
            default:
                return; //do nothing
        }
        let idx = findItem(model, id);
        if (idx >= 0)
            model.remove(idx);
    }

    /**
     * Deletes all items in a model.
     *
     * @param[in] type Model type - GisQmlInt::eTypeId.
     */
    function deleteItems(type: int)
    {
        switch (type)
        {
            case GisQmlInt.TYPEID_INCIDENT:
                mIncModel.clear();
                break;
            case GisQmlInt.TYPEID_INCIDENT_REPORT:
                mIncRptModel.clear();
                break;
            case GisQmlInt.TYPEID_MEASURE:
                mMeaModel.clear();
                mMap.mMeaCount = -1;
                break;
            case GisQmlInt.TYPEID_POI:
                mPoiModel.clear();
                break;
            case GisQmlInt.TYPEID_ROUTE_RESULT:
                mRouteQuery.clearWaypoints();
                mRouteModel.reset();
                mRouteIcnModel.clear();
                break
            case GisQmlInt.TYPEID_SEARCH:
                mSearchModel.clear();
                break;
            case GisQmlInt.TYPEID_TERMINAL:
                mResModel.clear();
                break;
            case GisQmlInt.TYPEID_TRACKING:
                mTrackModel.clear();
                break;
            case GisQmlInt.TYPEID_TRAILING:
                mTrailModel.clear();
                break;
            default:
                break; //do nothing
        }
    }

    /**
     * Finds an item on the map, and centers on it if found.
     *
     * @param[in] type Model type - GisQmlInt::eTypeId.
     * @param[in] id   The ID.
     * @param[in] ctr  true to center on item.
     * @return The item index in the model, or -1 if not found.
     */
    function locateItem(type:int, id: int, ctr: bool)
    {
        let model;
        switch (type)
        {
            case GisQmlInt.TYPEID_INCIDENT:
                model = mIncModel;
                break;
            case GisQmlInt.TYPEID_POI:
                model = mPoiModel;
                break;
            default:
                model = mResModel;
                break
        }
        let idx = findItem(model, id);
        if (ctr && idx >= 0)
        {
            let d = model.get(idx);
            setGeomCenter(d.lat, d.lon,
                        (mMap.mZoom >= cZOOM_SEARCH)? mMap.mZoom: cZOOM_SEARCH);
        }
        return idx;
    }

    /**
     * Finds items and locations.
     *
     * @param[in] key The search key.
     */
    function search(key: string)
    {
        mResLst = [];
        let i;
        let d = 0;
        if (+key) //check for numeric key
        {
            i = findItem(mResModel, +key); //check for resource id
            if (i >= 0)
            {
                d = mResModel.get(i);
                mResLst.push(mGisInt.getModelName(GisQmlInt.TYPEID_TERMINAL) +
                             ";" + d.id + ";" + d.lat + ";" + d.lon + ";" +
                             d.id);
            }
            i = findItem(mIncModel, +key); //check for incident id
            if (i >= 0)
            {
                d = mIncModel.get(i);
                mResLst.push(mGisInt.getModelName(GisQmlInt.TYPEID_INCIDENT) +
                             ";" + d.id + ";" + d.lat + ";" + d.lon + ";" +
                             d.id);
            }
        }
        else if (mGisInt.rscShowName())
        {
            //check for resource label
            for (i=mResModel.count-1; i>=0; --i)
            {
                d = mResModel.get(i);
                if (d.lblTxt.toLowerCase().includes(key.toLowerCase()))
                {
                    mResLst.push(mGisInt.getModelName(
                                                    GisQmlInt.TYPEID_TERMINAL) +
                                 ";" + d.id + ";" + d.lat + ";" + d.lon + ";" +
                                 d.lblTxt);
                }
            }
        }
        //check for poi name
        for (i=mPoiModel.count-1; i>=0; --i)
        {
            d = mPoiModel.get(i);
            if ((d.lgName.toLowerCase().includes(key.toLowerCase())) ||
                (d.shName.toLowerCase().includes(key.toLowerCase())))
            {
                mResLst.push(mGisInt.getModelName(GisQmlInt.TYPEID_POI) + ";" +
                             d.id + ";" + d.lat + ";" + d.lon + ";" + d.lgName +
                             " (" + d.shName + ")");
            }
        }
        mGeocodeModel.bounds = mGisInt.getGeoBounds();
        mGeocodeModel.ctrPt = false;
        mGeocodeModel.query = key;
        mGeocodeModel.update();
    }

    /**
     * Finds items and locations around a center point.
     *
     * @param[in] key The search key. Empty to find resources only.
     * @param[in] lbl Center point label, if any.
     * @param[in] rad Search radius in kilometers.
     * @param[in] lat Center latitude.
     * @param[in] lon Center longitude.
     */
    function searchNearby(key: string,
                          lbl: string,
                          rad: double,
                          lat: double,
                          lon: double)
    {
        mResLst = [];
        getItems(key, false, rad, lat, lon);
        if (key.length === 0)
        {
            searchCtrDone(key, mResLst, toPoint(lon, lat), lbl);
            return;
        }
        display.text = lbl;
        //GeoCodeModel search adds about 20% to the area, so reduce radius a bit
        mGeocodeModel.bounds = mGisInt.getGeoBoundsCircle(lat, lon, rad * 800);
        mGeocodeModel.ctrPt = true;
        mGeocodeModel.ctrLat = lat;
        mGeocodeModel.ctrLon = lon;
        mGeocodeModel.query = key;
        mGeocodeModel.update();
    }

    /**
     * Gets terminals, POIs and incidents around a center point.
     *
     * @param[in] key The search key, or empty string if not applicable.
     * @param[in] all true to get all types - for context menu items, and only
     *                with empty key. Otherwise for search nearby.
     * @param[in] rad Search radius in kilometers.
     * @param[in] lat Center latitude.
     * @param[in] lon Center longitude.
     */
    function getItems(key: string,
                      all: bool,
                      rad: double,
                      lat: double,
                      lon: double)
    {
        let i;
        let d;
        let x;
        if (key.length === 0)
        {
            for (i=mResModel.count-1; i>=0; --i)
            {
                d = mResModel.get(i);
                x = getDistance(lat, lon, d.lat, d.lon);
                if (x <= rad)
                {
                    if (all)
                        mResLst.push(mGisInt.getModelName(
                                                    GisQmlInt.TYPEID_TERMINAL) +
                                     ";" + d.id + ";" + d.lat + ";" + d.lon +
                                     ";" + x.toFixed(cPRECISION) + ";" + d.ts +
                                     ";" + d.tsVld + ";" + d.tm);
                    else
                        mResLst.push(mGisInt.getModelName(
                                                    GisQmlInt.TYPEID_TERMINAL) +
                                     ";" + d.id + ";" + d.lat + ";" + d.lon +
                                     ";" + d.id + ";" + x.toFixed(cPRECISION));
                }
            }
            if (all)
            {
                for (i=mPoiModel.count-1; i>=0; --i)
                {
                    d = mPoiModel.get(i);
                    x = getDistance(lat, lon, d.lat, d.lon);
                    if (x <= rad)
                        mResLst.push(mGisInt.getModelName(
                                                         GisQmlInt.TYPEID_POI) +
                                     ";" + d.id + ";" + d.lat + ";" + d.lon +
                                     ";" + x.toFixed(cPRECISION) + ";" +
                                     d.shName);
                }
                for (i=mIncModel.count-1; i>=0; --i)
                {
                    d = mIncModel.get(i);
                    x = getDistance(lat, lon, d.lat, d.lon);
                    if (x <= rad)
                        mResLst.push(mGisInt.getModelName(
                                                    GisQmlInt.TYPEID_INCIDENT) +
                                     ";" + d.id + ";" + d.lat + ";" + d.lon +
                                     ";" + x.toFixed(cPRECISION) + ";" + d.cat);
                }
            }
            return;
        }
        if (mGisInt.rscShowName())
        {
            for (i=mResModel.count-1; i>=0; --i) //match resource labels
            {
                d = mResModel.get(i);
                if (d.lblTxt.toLowerCase().includes(key.toLowerCase()))
                {
                    x = getDistance(lat, lon, d.lat, d.lon);
                    if (x <= rad)
                        mResLst.push(mGisInt.getModelName(
                                                    GisQmlInt.TYPEID_TERMINAL) +
                                     ";" + d.id + ";" + d.lat + ";" + d.lon +
                                     ";" + d.lblTxt + ";" +
                                     x.toFixed(cPRECISION));
                }
            }
        }
        for (i=mPoiModel.count-1; i>=0; --i) //match poi names
        {
            d = mPoiModel.get(i);
            if ((d.lgName.toLowerCase().includes(key.toLowerCase())) ||
                (d.shName.toLowerCase().includes(key.toLowerCase())))
            {
                x = getDistance(lat, lon, d.lat, d.lon);
                if (x <= rad)
                    mResLst.push(mGisInt.getModelName(GisQmlInt.TYPEID_POI) +
                                 ";" + d.id + ";" + d.lat + ";" + d.lon + ";" +
                                 d.lgName + " (" + d.shName + ");" +
                                 x.toFixed(cPRECISION));
            }
        }
    }

    /**
     * Gets item details.
     *
     * @return Details delimited by ';'.
     */
    function getItemDetails(type:int, id: int)
    {
        let idx = locateItem(type, id, false);
        if (idx < 0)
            return "";
        let res = "";
        let d;
        switch (type)
        {
            case GisQmlInt.TYPEID_INCIDENT:
                break; //do nothing - should not occur
            case GisQmlInt.TYPEID_POI:
                d = mPoiModel.get(idx);
                res = d.lgName + ";" + d.shName + ";" + d.cat + ";" + d.addr +
                      ";" + d.desc + ";" + d.owner + ";" +
                      ((d.isPublic)? "1": "") + ";" + d.lat + ";" + d.lon;
                break;
            default:
                d = mResModel.get(idx);
                res = d.issi + ";" + d.lat + ";" + d.lon + ";" + d.ts + ";" +
                      d.tsVld;
                break;
        }
        return res;
    }

    /**
     * Shows selected search item on map, and centers on it.
     *
     * @param[in] lat  The latitude.
     * @param[in] lon  The longitude.
     * @param[in] name The text to be displayed.
     */
    function placePin(lat: double, lon: double, name: string)
    {
        mSearchModel.set(0,
                         { lat: lat,
                           lon: lon,
                           txt: (name.length === 0)? lat + "," + lon: name });
        setGeomCenter(lat, lon, cZOOM_SEARCH);
    }

    /**
     * Gets resources on the map.
     *
     * @return Space-separated ISSIs.
     */
    function getTerminals()
    {
        let n = mResModel.count;
        if (n == 0)
            return "";
        display.text = mResModel.get(0).id;
        for (var i=1; i<n; ++i)
        {
            display.text += ' ' + mResModel.get(i).id;
        }
        return display.text;
    }

    /**
     * Gets resources with trailing enabled.
     *
     * @return Space-separated ISSIs.
     */
    function getTrailings()
    {
        display.text = "";
        let n = mResModel.count;
        if (n == 0)
            return "";
        for (var i=0; i<n; ++i)
        {
            if (mResModel.get(i).trail)
                display.text += ' ' + mResModel.get(i).id;
        }
        return (display.text.length === 0)? "": display.text.substring(1);
    }

    /**
     * Enables/disables resource trailing.
     *
     * @param[in] issi    The resource ISSI.
     * @param[in] enabled true to enable.
     */
    function terminalTrailing(issi: int, enabled: bool)
    {
        let idx = findItem(mResModel, issi);
        if (idx >= 0)
            mResModel.set(idx, { trail: enabled });
        if (enabled)
        {
            //get resource current coordinates
            let currCoor = toGeoCoordinate(mResModel.get(idx).lat,
                                           mResModel.get(idx).lon);
            mTrailModel.append({ id     : issi,
                                 from   : currCoor,
                                 to     : currCoor,
                                 rot    : 0,
                                 isFirst: true,
                                 isLast : true });
        }
        else
        {
            let i = mTrailModel.count - 1;
            for (; i>=0; --i)
            {
                if (mTrailModel.get(i).id === issi)
                    mTrailModel.remove(i);
            }
        }
    }

    /**
     * Checks whether a resource trailing property is enabled.
     *
     * @param[in] issi The resource ISSI.
     * @return true if enabled.
     */
    function isTrailing(issi: int)
    {
        return (findItem(mTrailModel, issi) >= 0);
    }

    /**
     * Sets a resource state or type.
     *
     * @param[in] i    For state, the resource model index, else its ISSI.
     * @param[in] type Negative for state, else the new resource type -
     *                 SubsData::eTerminalType.
     * @param[in] val  The new resource state or type. Must be in lowercase.
     */
    function terminalSetParam(i: int, type: int, val: string)
    {
        if (type < 0)
        {
            mResModel.set(i,
                          { state : val,
                            imgSrc: "qrc:///Qml/qml/terminal/" +
                                    mResModel.get(i).sType + val + ".png" });
        }
        else
        {
            i = findItem(mResModel, i);
            if (i >= 0)
                mResModel.set(i,
                              { iType : type,
                                sType : val,
                                imgSrc: "qrc:///Qml/qml/terminal/" + val +
                                        mResModel.get(i).state + ".png" });
        }
    }

    /**
     * Sets a resource type.
     *
     * @param[in] issi  The ISSI.
     * @param[in] tpVal The type - SubsData::eTerminalType.
     * @param[in] tpStr The lower case type from GisCanvas::getTerminalIconPfx().
     */
    function terminalSetType(issi: int, tpVal: int, tpStr: string)
    {
        terminalSetParam(issi, tpVal, tpStr);
    }

    /**
     * Removes listed resources from map.
     *
     * @param[in] issis The ISSIs.
     */
    function terminalsErase(issis: list)
    {
        let idx;
        for (let i=issis.length-1; i>=0; --i)
        {
            idx = findItem(mResModel, issis[i]);
            if (idx >= 0)
                mResModel.remove(idx);
        }
    }

    /**
     * Shows or hides terminals of a type.
     *
     * @param[in] show true to show.
     * @param[in] type The type - SubsData::eTerminalType.
     */
    function terminalsShow(show: bool, type: int)
    {
        mRscShow[type] = show;
        //changing list property content does not generate a property change
        //notification - need to trigger manually by making a call to
        //<propertyname>Changed()
        mRscShowChanged();
    }

    /**
     * Adds/updates a resource point.
     *
     * @param[in] issi  The ISSI.
     * @param[in] tpVal The type - SubsData::eTerminalType.
     * @param[in] lat   The latitude.
     * @param[in] lon   The longitude.
     * @param[in] tpStr The lower case type from GisCanvas::getTerminalIconPfx().
     * @param[in] ts    Location update timestamp.
     * @param[in] tm    Timestamp value in seconds from epoch. Empty for invalid
     *                  location.
     */
    function locationUpdate(issi : int,
                            tpVal: int,
                            lat  : double,
                            lon  : double,
                            tpStr: string,
                            ts   : string,
                            tm   : string)
    {
        let idx = findItem(mResModel, issi);
        if (idx < 0) //new resource
        {
            mResModel.append({ id    : issi,
                               lat   : lat,
                               lon   : lon,
                               state : "",
                               iType : tpVal,
                               sType : tpStr,
                               lblTxt: mGisInt.rscLbl(issi),
                               ts    : ts,
                               tsVld : ts,
                               tm    : tm,
                               imgSrc: "qrc:///Qml/qml/terminal/" + tpStr +
                                       ".png",
                               blink : false,
                               trail : false });
        }
        else
        {
            let d = mResModel.get(idx);
            if (d.trail)
            {
                let prev = toGeoCoordinate(d.lat, d.lon); //last location
                mTrailModel.set(findItem(mTrailModel, issi), { isLast: false });
                let curr = toGeoCoordinate(lat, lon);
                mTrailModel.append({ id     : issi,
                                     from   : prev,
                                     to     : curr,
                                     rot    : prev.azimuthTo(curr),
                                     isFirst: false,
                                     isLast : true });
            }
            if (idx < mResModel.count - 1)
            {
                //move item to end just to ensure it appears on top
                mResModel.append(d);
                mResModel.remove(idx);
                idx = mResModel.count - 1;
                d = mResModel.get(idx);
            }
            d.lat = lat;
            d.lon = lon;
            d.ts = ts;
            if (tm.length > 0)
            {
                d.tsVld = ts;
                d.tm = tm;
            }
            terminalSetParam(idx, -1, (tm.length > 0)? "": "_invalid");
        }
    }

    /**
     * Checks/updates terminal states.
     */
    function terminalsCheck()
    {
        if (mStale1 <= 0 && mStaleLast <= 0)
            return;
        let now = Date.now()/1000; //seconds from epoch
        let t; //elapsed seconds for terminal
        let i = mResModel.count - 1;
        for (; i>=0; --i)
        {
            if (mResModel.get(i).ts.length === 0)
                continue; //skip because no timestamp
            t = Date.parse(mResModel.get(i).ts.split('[')[0]);
            if (t === NaN)
                continue;
            t = now - t/1000;
            //change resource state or remove resource
            if (mStaleLast > 0 && t >= mStaleLast)
                mResModel.remove(i);
            else if (mStale1 > 0 && t >= mStale1)
                terminalSetParam(i, -1, "_stale1");
        }
    }

    /**
     * Updates the resource labels.
     */
    function updateResourceLabels()
    {
        let i = mResModel.count - 1;
        for (; i>=0; --i)
        {
            mResModel.set(i, { lblTxt: mGisInt.rscLbl(mResModel.get(i).id) });
        }
    }

    /**
     * Starts/stops resource icon highlight on map
     *
     * @param[in] issi  The ISSI.
     * @param[in] start true to start.
     */
    function setRscInCall(issi: int, start: bool)
    {
        let i = findItem(mResModel, issi);
        if (i >= 0)
        {
            let d = mResModel.get(i);
            d.blink = start;
            if (start)
            {
                if (i < mResModel.count - 1)
                {
                    //move item to end just to ensure it appears on top
                    mResModel.append(d);
                    mResModel.remove(i);
                    d = mResModel.get(mResModel.count - 1);
                }
                if (mCtrRscInCall)
                {
                    //if resource not inside map view, center on it
                    let tl = mMap.toCoordinate(toPoint(0, 0), false);
                    let br = mMap.toCoordinate(toPoint(mMap.width, mMap.height),
                                               false);
                    if (d.lat > tl.latitude || d.lat < br.latitude ||
                        d.lon > br.longitude || d.lon < tl.longitude)
                        mMap.center = toGeoCoordinate(d.lat, d.lon);
                }
            }
        }
    }

    /**
     * Adds/updates an incident or incident report point.
     *
     * @param[in] type  Model type - GisQmlInt.TYPEID_INCIDENT/INCIDENT_REPORT.
     * @param[in] id    The incident ID.
     * @param[in] cat   The category.
     * @param[in] state The state.
     * @param[in] lat   The latitude.
     * @param[in] lon   The longitude.
     */
    function incidentPt(type : int,
                        id   : int,
                        cat  : string,
                        state: int,
                        lat  : double,
                        lon  : double)
    {
        cat = cat.toLowerCase();
        let dir = "/Qml/qml/incident" +
                  ((type === GisQmlInt.TYPEID_INCIDENT_REPORT)? "_report/": "/");
        let f = dir + cat + state + ".png";
        if (!mGisInt.fileExists(":" + f))
        {
            cat = "default";
            f = dir + cat + state + ".png";
        }
        if (type === GisQmlInt.TYPEID_INCIDENT_REPORT)
        {
            mIncRptModel.append({ id    : id,
                                  lat   : lat,
                                  lon   : lon,
                                  cat   : cat,
                                  imgSrc: "qrc://" + f });
        }
        else
        {
            let idx = findItem(mIncModel, id);
            if (idx < 0) //new incident
            {
                mIncModel.append({ id    : id,
                                   lat   : lat,
                                   lon   : lon,
                                   cat   : cat,
                                   imgSrc: "qrc://" + f });
            }
            else
            {
                mIncModel.set(idx,
                              { lat   : lat,
                                lon   : lon,
                                cat   : cat,
                                imgSrc: "qrc://" + f });
            }
        }
    }

    /**
     * Adds/updates a POI point.
     *
     * @param[in] key      The POI ID.
     * @param[in] isPublic Public setting.
     * @param[in] longName The long name.
     * @param[in] shrtName The short name.
     * @param[in] cat      The category.
     * @param[in] addr     The address.
     * @param[in] desc     The description.
     * @param[in] owner    The owner.
     * @param[in] lat      The latitude.
     * @param[in] lon      The longitude.
     */
    function poiUpdate(key     : int,
                       isPublic: bool,
                       longName: string,
                       shrtName: string,
                       cat     : string,
                       addr    : string,
                       desc    : string,
                       owner   : string,
                       lat     : double,
                       lon     : double)
    {
        cat = cat.toLowerCase();
        let idx = findItem(mPoiModel, key);
        let f = "/Qml/qml/userPoi/" + cat + ".png";
        if (!mGisInt.fileExists(":" + f))
        {
            cat = "others";
            f = "/Qml/qml/userPoi/others.png";
        }
        if (idx < 0) //new POI
        {
            mPoiModel.append({ id      : key,
                               lgName  : longName,
                               shName  : shrtName,
                               cat     : cat,
                               addr    : addr,
                               desc    : desc,
                               owner   : owner,
                               isPublic: isPublic,
                               lat     : lat,
                               lon     : lon,
                               imgSrc  : "qrc://" + f });
        }
        else
        {
            mPoiModel.set(idx,
                          { lgName  : longName,
                            shName  : shrtName,
                            cat     : cat,
                            addr    : addr,
                            desc    : desc,
                            owner   : owner,
                            isPublic: isPublic,
                            lat     : lat,
                            lon     : lon,
                            imgSrc  : "qrc://" + f });
        }
    }

    /**
     * Adds a temporary POI point.
     *
     * @param[in] lat The latitude.
     * @param[in] lon The longitude.
     */
    function onNewPoi(lat: double, lon: double)
    {
        mPoiModel.append({ id    : -1,
                           lgName: "",
                           shName: "",
                           lat   : lat,
                           lon   : lon,
                           imgSrc: mMap.cICON_USER_POI });
    }

    /**
     * Deletes temporary POI point.
     */
    function dismissNewPoi()
    {
        let idx = findItem(mPoiModel, -1);
        if (idx >= 0)
            mPoiModel.remove(idx);
    }

    /**
     * Enables/disables measurement circle drawing.
     *
     * @param[in] enabled true to enable.
     */
    function measureCircleEnable(enabled: bool)
    {
        mMap.mMeaCircle = enabled;
    }

    /**
     * Sets the measurement unit and updates measurement labels.
     *
     * @param[in] unit   The unit from GisLineLabel::eMeasureUnit.
     * @param[in] fixAll true to change all distance measurements in model.
     */
    function measureUpdate(unit: int, fixAll: bool)
    {
        mMeasureUnit = unit;
        let conv = 1; //conversion factor from default unit
        switch (mMeasureUnit)
        {
            case GisQmlInt.MEASURE_UNIT_MILE:
                conv = 0.621371;
                break;
            case GisQmlInt.MEASURE_UNIT_NAUTICAL_MILE:
                conv = 0.539957;
                break;
            default:
                break; //do nothing
        }
        if (fixAll)
        {
            //update all text in measurement model
            let i = mMeaModel.count - 1;
            for (; i>=0; --i)
            {
                mMeaModel.set(i, { lblTxt: measureLbl(i, conv) });
            }
        }
        else
        {
            mMeaModel.set(mMap.mMeaCount,
                          { lblTxt: measureLbl(mMap.mMeaCount, conv) });
        }
    }

    /**
     * Gets the measurement label showing distance, total distance and azimuth.
     *
     * @param[in] idx  The item index in measurement model.
     * @param[in] conv The conversion factor.
     * @return String in format: "<current_distance> [<total_distance>]
     *                            <azimuth>".
     */
    function measureLbl(idx: int, conv: double)
    {
        let itm = mMeaModel.get(idx);
        let dist = getDistance(itm.from.latitude, itm.from.longitude,
                               itm.to.latitude, itm.to.longitude);
        let aDist = itm.totDist;
        let tot = "";
        if (aDist !== 0)
            tot = " [" + ((aDist * conv) + dist).toFixed(cPRECISION) + ']';
        return ((dist * conv).toFixed(cPRECISION) + tot + '\n' +
                (itm.rot).toFixed(cPRECISION) + '\xB0');
    }

    /**
     * Checks if a measurement line has been completed.
     *
     * @return true if completed.
     */
    function measureDone()
    {
        return (!mMap.mIsMeasure && mMap.mMeaCount >= 0);
    }

    /**
     * Resumes measurement from last point
     */
    function measureResume()
    {
        if (!measureDone())
            return;
        mMap.mIsMeasure = true;
        let pos = mMap.mMeaCount;
        mMap.mMeaCount++;
        let lastData = mMeaModel.get(pos);
        mMeaModel.set(pos, { isEnd: false });
        mMeaModel.append({ from   : lastData.to,
                           to     : mMap.getMouseGeoCoordinate(),
                           rot    : 0,
                           totDist: lastData.totDist +
                                    getDistance(lastData.from.latitude,
                                                lastData.from.longitude,
                                                lastData.to.latitude,
                                                lastData.to.longitude),
                           lblTxt : "",
                           isEnd  : true });
    }

    /**
     * Deletes last measurement point.
     */
    function measureRmvLastPoint()
    {
        if (mMap.mMeaCount < 0)
            return;
        if (mMap.mMeaCount > 0)
        {
            //at least 1 line present
            let prevPos = mMap.mMeaCount - 1;
            if (mMap.mIsMeasure) //ongoing measurement
            {
                if (mMeaModel.get(prevPos).isEnd) //hit a different line
                {
                    mMap.mIsMeasure = false;
                }
                else
                {
                    let c = mMap.getMouseGeoCoordinate();
                    let fr = mMeaModel.get(prevPos).from;
                    mMeaModel.set(prevPos,
                                  { to : c,
                                    rot: toGeoCoordinate(fr.latitude,
                                                         fr.longitude)
                                         .azimuthTo(c) });
                }
            }
            mMeaModel.set(prevPos, { isEnd: true }); //show measure circle
        }
        mMeaModel.remove(mMap.mMeaCount--);
    }

    /**
     * Deletes last measurement line.
     */
    function measureRmvLastLine()
    {
        while (mMap.mMeaCount >= 0)
        {
            mMeaModel.remove(mMap.mMeaCount--);
            if (mMap.mMeaCount >= 0 && mMeaModel.get(mMap.mMeaCount).isEnd)
            {
                mMap.mIsMeasure = false; //hit a different line
                break;
            }
        }
    }

    /**
     * Adds a tracking line.
     *
     * @param[in] issi The ISSI.
     * @param[in] lat  The coordinates latitude.
     * @param[in] lon  The coordinates longitude.
     */
    function addTrackingLine(issi: int, lat: double, lon: double)
    {
        let coor = toGeoCoordinate(lat, lon);
        let idx = findItem(mTrackModel, issi);
        if (idx < 0) //new tracking
        {
            mTrackModel.append({ id     : issi,
                                 from   : coor,
                                 to     : coor,
                                 rot    : 0,
                                 lblTxt : "",
                                 isFirst: true });
        }
        else
        {
            //get resource last locations
            let prevCoor = toGeoCoordinate(mTrackModel.get(idx).to.latitude,
                                           mTrackModel.get(idx).to.longitude);
            mTrackModel.set(idx, { lblTxt: "" });
            mTrackModel.append({ id     : issi,
                                 from   : prevCoor,
                                 to     : coor,
                                 rot    : prevCoor.azimuthTo(coor),
                                 lblTxt : String(issi),
                                 isFirst: false });
        }
    }

    /**
     * Deletes tracking line segments for an ISSI.
     *
     * @param[in] issi The ISSI.
     * @param[in] num  The number of line segments to delete.
     */
    function deleteItemLastNodes(issi: int, num: int)
    {
        let idx = -1;
        for (; num>0; --num)
        {
            idx = findItem(mTrackModel, issi);
            mTrackModel.remove(idx);
        }
        idx = findItem(mTrackModel, issi);
        mTrackModel.set(idx, { lblTxt: String(issi) });
    }

    /**
     * Creates map component.
     */
    function createMap()
    {
        let zl = (mOverview)? cZOOM_OV: cZOOM_DEF;
        let ctr = (mOverview)? cOvCtr: cMainCtr;
        if (mMap != null)
        {
            zl = mMap.mZoom;
            ctr = mMap.center;
        }
        let host = mGisInt.getMapPath();
        host += (mGisInt.isDarkMode())? cNIGHT: cLIGHT;
        mLoader.setSource("qrc:/Qml/qml/MapComponent.qml",
                          {
                              "mHost" : host,
                              "mOvr"  : mOverview,
                              "center": ctr,
                              "mZoom" : zl
                          });
        mMap = mLoader.item;
    }

    /**
     * Refreshes map display - mainly after window state/size changes.
     */
    function refresh()
    {
        if (mMap.mapReady)
        {
            mMap.update();
            mMap.pan(0, 1);
            mMap.pan(0, -1);
        }
    }

    /**
     * Clears map data and recreates map component.
     */
    function refreshMap()
    {
        //sample location:
        //  C:\Users\<username>\AppData\Local\cache\QtLocation\5.8\tiles\osm
        mMap.clearData();
        if (mSeaMap)
            mGisInt.clearTiles();
        createMap();
    }

    /**
     * Gets the map resolution at the current center latitude and zoom level.
     *
     * @return kilometers per pixel.
     */
    function resolution()
    {
        //resolution = (156.543034km/pixel * cos(latitude<rad>))/(2^zoom)
        //rad = degree * Pi/180 = 0.01745329251994329576923690768489
        return (156.543034 * Math.cos(getGeomCenterLat() * 0.017453)/
                Math.pow(2.0, mMap.zoomLevel));
    }

    /**
     * Gets the map scale.
     *
     * @return the scale.
     */
    function mapScale()
    {
        //logicalPixelDensity = pixels per millimeter
        return Screen.logicalPixelDensity * 1000000 * resolution();
    }

    /**
     * Sets the focus rectangle on overview map.
     *
     * @param[in] left  Left longitude.
     * @param[in] top   Top latitude.
     * @param[in] right Right longitude.
     * @param[in] btm   Bottom latitude.
     */
    function updateOvBox(left: double, top: double, right: double, btm: double)
    {
        if (mMap != null)
            mMap.updateRectView(toGeoCoordinate(left, top),
                                toGeoCoordinate(right, btm));
    }

    /**
     * Sets the timer interval and starts the timer.
     *
     * @param[in] intvl Interval in seconds.
     */
    function timerStart(intvl: int)
    {
        termTimer.interval = intvl;
        termTimer.start();
    }

    /**
     * Stops the timer.
     */
    function timerStop()
    {
        termTimer.stop();
    }

    /**
     * Sets the terminal check threshold times, and starts the timer if not
     * running.
     *
     * @param[in] stale1    Threshold in minutes to change appearance.
     * @param[in] staleLast Threshold in minutes to remove from map.
     */
    function setTerminalCheckTimes(stale1: int, staleLast: int)
    {
        mStale1 = stale1 * 60;
        mStaleLast = staleLast * 60;
        if (!termTimer.running)
            termTimer.start();
    }

    Timer
    {
        id         : termTimer;
        running    : false;
        repeat     : true;
        onTriggered: { terminalsCheck(); }
    }

    //temporary elements to process text
    Text
    {
        id     : txtIssi;
        visible: false;
    }
    Text
    {
        id     : display;
        visible: false;
    }
}
