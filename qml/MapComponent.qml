/**
 * QML map component implementation.
 *
 * Copyright (C) Sapura Secured Technologies, 2024. All Rights Reserved.
 *
 * @file
 * @version $Id: MapComponent.qml 1894 2024-12-20 07:15:28Z rosnin $
 * @author Rosnin
 */
import QtGraphicalEffects 1.0
import QtLocation 5.12
import QtPositioning 5.12
import QtQuick 2.9

import gisInt 1.0

Map
{
    id               : map;
    activeMapType    : supportedMapTypes[supportedMapTypes.length - 1];
    anchors.fill     : parent;
    center           : mMainCtr;
    copyrightsVisible: false;
    gesture
    {
        enabled         : !mOvr && mGesture;
        acceptedGestures: MapGestureArea.FlickGesture |
                          MapGestureArea.PanGesture |
                          MapGestureArea.PinchGesture;
    }
    plugin : Plugin
    {
        name: "osm"; //use OSM plugin
        PluginParameter
        {
            name : "osm.mapping.custom.host";
            value: mHost; //tile server
        }
        //disable retrieval of the providers info from remote repo
        PluginParameter
        {
            name : "osm.mapping.providersrepository.disabled";
            value: true;
        }
    }
    zoomLevel        : mZoom;

    readonly property int    cARROW_HEIGHT  : 15;
    readonly property int    cARROW_WIDTH   : 22;
    readonly property int    cBORDER_WIDTH  : 1;
    readonly property int    cICON_HEIGHT   : 32;
    readonly property int    cICON_WIDTH    : 32;
    //line width for overview rectangle border, tracking, trailing and
    //measure lines
    readonly property int    cLINE_WIDTH1   : 3;
    //line width for routing line
    readonly property int    cLINE_WIDTH2   : 4;
    readonly property int    cMARKER_HEIGHT : 20;
    readonly property int    cMARKER_WIDTH  : 20;
    readonly property int    cTILE_WIDTH    : 256; //square tile
    readonly property double cCIRCLE_OPACITY: 0.3;
    readonly property string cICON_MARKER:
                                         "qrc:///Images/images/icon_attach.png";
    readonly property string cICON_PIN: "qrc:///Qml/qml/search/pin.png";
    readonly property string cICON_ROUTE_END:
                                           "qrc:///Qml/qml/route/route_end.png";
    readonly property string cICON_ROUTE_START:
                                         "qrc:///Qml/qml/route/route_start.png";
    readonly property string cICON_USER_POI: "qrc:///Qml/qml/userPoi/user.png";
    readonly property string cIMG_ARROW: "qrc:///Images/images/arrow.png";
    //bounding box tileMatrixMinY values for zoom level 0 to 20 using default
    //gridsets based on EPSG:900913
    readonly property variant cGeoMinY: [      0,    0,     0,     1,     2,
                                               4,    8,    16,    33,    67,
                                             134,  269,   538,  1076,  2152,
                                            4304, 8608, 17217, 34434, 68868,
                                          137736 ];

    property int    mMeaCount : -1;        //measurement lines
    property bool   mGesture  : true;      //mouse action enabled
    property bool   mIsMeasure: false;
    property bool   mIsReady  : false;
    property bool   mMeaCircle: true;      //measurement circle
    property bool   mOvr      : false;
    property double mFactor   : 1.0;       //tile-size multiplication factor
    property double mZoom     : cZOOM_DEF; //zoom level
    property double mZoomStep : 0.5;
    property string mHost     : "";
    property string mTileName : "{z}_{x}_{y}.png"
    property string mTmsUrl   : "seamark/{z}/{x}/{y}.png";
    property string mWmsUrl   : ":8080/geoserver/gwc/service/tms/1.0.0/" +
                                "gebco2021%3Agebco_2021@EPSG%3A900913@png/" +
                                "{z}/{x}/{y}.png"
    GisQmlInt { id: mGisInt; }

    onMOvrChanged:
    {
        if (mOvr)
        {
            mZoom  = cZOOM_OV;
            center = cOvCtr;
        }
    }

    onMapReadyChanged:
    {
        if (mapReady)
        {
            mIsReady = true;
            mapLayerLoaded();
            updateFocus();
        }
    }

    onZoomLevelChanged:
    {
        if (zoomLevel !== mZoom)
            mapZoomChanged(zoomLevel);
    }

    onCenterChanged:
    {
        if (mIsReady)
            updateFocus();
    }

    onHeightChanged:
    {
        if (mIsReady)
            updateFocus();
    }

    onWidthChanged:
    {
        if (mIsReady)
            updateFocus();
    }

    MapRectangle //overview: map focus rectangle
    {
        id: rectView;
        border { color: "red"; width: cLINE_WIDTH1; }

        //minimum diagonal distance in meter
        readonly property real minDiagDist: 21000;
    }

    //layers are stacked from bottom up in order of model (MapItemView)
    //declarations here
    MapItemView //sea depth tiles
    {
        model   : mSeaDepthModel;
        delegate: MapQuickItem
        {
            coordinate: toGeoCoordinate(lat, lon);
            sourceItem: Image
            {
                height: cTILE_WIDTH * mFactor;
                width : cTILE_WIDTH * mFactor;
                source: src;
            }
            visible   : mLyrSeaDepth;
        }
    }

    MapItemView //sea marker tiles
    {
        model   : mSeaMarkerModel;
        delegate: MapQuickItem
        {
            coordinate: toGeoCoordinate(lat, lon);
            sourceItem: Image
            {
                height: cTILE_WIDTH * mFactor;
                width : cTILE_WIDTH * mFactor;
                source: src;
            }
            visible   : (mZoom > 8) && mLyrSeaMarker;
        }
    }

    MapItemView //user POI icon
    {
        model   : mPoiModel;
        delegate: MapQuickItem
        {
            anchorPoint: toPoint(imgPoi.width/2, imgPoi.height/2);
            coordinate : toGeoCoordinate(lat, lon);
            sourceItem : Image
            {
                id    : imgPoi;
                cache : false;
                height: cICON_HEIGHT;
                width : cICON_WIDTH;
                source: imgSrc;
            }
            visible    : mImgPoiView;
        }
    }

    MapItemView //user POI label
    {
        model   : mPoiModel;
        delegate: MapQuickItem
        {
            anchorPoint: toPoint(lblPoi.width/2, cICON_HEIGHT);
            coordinate : toGeoCoordinate(lat, lon);
            sourceItem : Text
            {
                id        : lblPoi;
                color     : "blue";
                font.bold : true;
                style     : Text.Outline;
                styleColor: "white";
                text      : shName;
            }
            visible    : mLblPoiView;
        }
    }

    MapItemView //incident icon
    {
        model   : mIncModel;
        delegate: MapQuickItem
        {
            anchorPoint: toPoint(imgIncident.width/2, imgIncident.height/2);
            coordinate : toGeoCoordinate(lat, lon);
            sourceItem : Image
            {
                id    : imgIncident;
                cache : false;
                height: cICON_HEIGHT;
                width : cICON_WIDTH;
                source: imgSrc;
            }
            visible    : mImgIncView;
        }
    }

    MapItemView //incident label
    {
        model   : mIncModel;
        delegate: MapQuickItem
        {
            anchorPoint: toPoint(lblIncident.width/2, cICON_HEIGHT);
            coordinate : toGeoCoordinate(lat, lon);
            sourceItem : Text
            {
                id        : lblIncident;
                color     : "red";
                font.bold : true;
                style     : Text.Outline;
                styleColor: "white";
                text      : id;
            }
            visible    : mLblIncView;
        }
    }

    MapItemView //incident report icon
    {
        model   : mIncRptModel;
        delegate: MapQuickItem
        {
            anchorPoint: toPoint(imgIncRpt.width/2, imgIncRpt.height/2);
            coordinate : toGeoCoordinate(lat, lon);
            sourceItem : Image
            {
                id    : imgIncRpt;
                cache : false;
                height: cICON_HEIGHT;
                width : cICON_WIDTH;
                source: imgSrc;
            }
            visible    : mImgIncRptView;
        }
    }

    MapItemView //incident report label
    {
        model   : mIncRptModel;
        delegate: MapQuickItem
        {
            anchorPoint: toPoint(lblIncRpt.width/2, cICON_HEIGHT);
            coordinate : toGeoCoordinate(lat, lon);
            sourceItem : Text
            {
                id        : lblIncRpt;
                color     : "pink";
                font.bold : true;
                style     : Text.Outline;
                styleColor: "black";
                text      : id;
            }
            visible    : mLblIncRptView;
        }
    }

    MapItemView //resource icon
    {
        model   : mResModel;
        delegate: MapQuickItem
        {
            anchorPoint: toPoint(imgTerminal.width/2, imgTerminal.height/2);
            coordinate : toGeoCoordinate(lat, lon);
            sourceItem : Image
            {
                id    : imgTerminal;
                cache : false;
                height: cICON_HEIGHT;
                width : cICON_WIDTH;
                source: imgSrc;
                transformOrigin: Item.Center
                transform: Scale
                {
                    property real scaleValue: 1;

                    id    : imgScaled;
                    origin { x: imgTerminal.width/2; y: imgTerminal.height/2; }
                    xScale: (blink)? scaleValue: 1;
                    yScale: (blink)? scaleValue: 1;
                }
                layer
                {
                    enabled: blink;
                    effect : Glow
                    {
                        radius : 10;
                        samples: 21; //twice radius value plus one
                        color  : "#2FF923";
                    }
                }
                SequentialAnimation
                {
                    loops  : Animation.Infinite;
                    running: blink;
                    PropertyAnimation
                    {
                        duration  : 250;
                        from      : 1;
                        to        : 1.5;
                        properties: "scaleValue";
                        target    : imgScaled;
                    }
                    PropertyAnimation
                    {
                        duration  : 250;
                        from      : 1.5;
                        to        : 1;
                        properties: "scaleValue";
                        target    : imgScaled;
                    }
                }
            }
            visible    : mImgRscView && mRscShow[iType] &&
                         (mRscFilter.length === 0 ||
                          mRscFilter.indexOf(id) >= 0);
        }
    }

    MapItemView //resource label
    {
        model   : mResModel;
        delegate: MapQuickItem
        {
            anchorPoint: toPoint(lblTerminal.width/2, cICON_HEIGHT);
            coordinate : toGeoCoordinate(lat, lon);
            sourceItem : Text
            {
                id        : lblTerminal;
                font.bold : true;
                color     : (mGisInt.isDarkMode())? "white": "black";
                style     : Text.Outline;
                styleColor: (mGisInt.isDarkMode())? "royalblue": "white";
                text      : lblTxt;
            }
            visible    : mLblRscView && mRscShow[iType] &&
                         (mRscFilter.length === 0 ||
                          mRscFilter.indexOf(id) >= 0);
        }
    }

    MapItemView //tracking line
    {
        model   : mTrackModel;
        delegate: MapPolyline
        {
            line { color: "aqua"; width: cLINE_WIDTH1; }
            path: [ toGeoCoordinate(from.latitude, from.longitude),
                    toGeoCoordinate(to.latitude, to.longitude) ]
            //only visible if position has changed
            visible: from !== to && mImgTrcView;
        }
    }

    MapItemView //tracking initial marker
    {
        model   : mTrackModel;
        delegate: MapQuickItem
        {
            anchorPoint: toPoint(cMARKER_WIDTH/2, cMARKER_HEIGHT/2);
            coordinate: toGeoCoordinate(from.latitude, from.longitude);
            sourceItem: Image
            {
                source: cICON_MARKER;
                height: cMARKER_HEIGHT;
                width : cMARKER_WIDTH;
            }
            //only visible for first tracking data
            visible: isFirst && mImgTrcView;
        }
    }

    MapItemView //tracking arrow head marker
    {
        model   : mTrackModel;
        delegate: MapQuickItem
        {
            coordinate: toGeoCoordinate(to.latitude, to.longitude);
            sourceItem: Image
            {
                source: cIMG_ARROW;
                height: cARROW_HEIGHT;
                width : cARROW_WIDTH;
                transform: [
                    Translate { x: -cARROW_WIDTH; y: -cARROW_HEIGHT/2; },
                    Rotation { angle: (rot - 90); } ]
            }
            //only visible if position has changed and not first position
            visible: !isFirst && from !== to && mImgTrcView;
        }
    }

    MapItemView //tracking label
    {
        model   : mTrackModel;
        delegate: MapQuickItem
        {
            anchorPoint: toPoint(lblTrack.width/2, cICON_HEIGHT);
            coordinate : toGeoCoordinate(to.latitude, to.longitude);
            sourceItem : Text
            {
                id  : lblTrack;
                text: lblTxt;
            }
            visible    : mLblTrcView;
        }
    }

    MapItemView //trailing line
    {
        model   : mTrailModel;
        delegate: MapPolyline
        {
            line { color: "orange"; width: cLINE_WIDTH1; }
            path: [ toGeoCoordinate(from.latitude, from.longitude),
                    toGeoCoordinate(to.latitude, to.longitude) ]
            visible: mImgTrlView;
        }
    }

    MapItemView //trailing initial marker
    {
        model   : mTrailModel;
        delegate: MapQuickItem
        {
            anchorPoint: toPoint(cMARKER_WIDTH/2, cMARKER_HEIGHT/2);
            coordinate: toGeoCoordinate(from.latitude, from.longitude);
            sourceItem: Image
            {
                source: cICON_MARKER;
                height: cMARKER_HEIGHT;
                width : cMARKER_WIDTH;
            }
            //only visible for first trailing data
            visible: isFirst && mImgTrlView;
        }
    }

    MapItemView //trailing arrow head marker
    {
        model   : mTrailModel;
        delegate: MapQuickItem
        {
            coordinate: toGeoCoordinate(to.latitude, to.longitude);
            sourceItem: Image
            {
                source: cIMG_ARROW;
                height: cARROW_HEIGHT;
                width : cARROW_WIDTH;
                transform: [
                    Translate { x: -cARROW_WIDTH; y: -cARROW_HEIGHT/2; },
                    Rotation { angle: (rot - 90); } ]
            }
            //not visible for first and last position
            visible: !isFirst && !isLast && mImgTrlView;
        }
    }

    MapItemView //measure line
    {
        model   : mMeaModel;
        delegate: MapPolyline
        {
            line { color: "green"; width: cLINE_WIDTH1; }
            path: [ toGeoCoordinate(from.latitude, from.longitude),
                    toGeoCoordinate(to.latitude, to.longitude) ]
            visible: mImgMeaView;
        }
    }

    MapItemView //measure initial marker
    {
        model   : mMeaModel;
        delegate: MapQuickItem
        {
            anchorPoint: toPoint(cMARKER_WIDTH/2, cMARKER_HEIGHT/2);
            coordinate : toGeoCoordinate(from.latitude, from.longitude);
            sourceItem : Image
            {
                source: cICON_MARKER;
                height: cMARKER_HEIGHT;
                width : cMARKER_WIDTH;
            }
            //only visible for first measurement data
            visible    : totDist === 0 && mImgMeaView;
        }
    }

    MapItemView //measure arrow head marker
    {
        model   : mMeaModel;
        delegate: MapQuickItem
        {
            coordinate: toGeoCoordinate(to.latitude, to.longitude);
            sourceItem: Image
            {
                source: cIMG_ARROW;
                height: cARROW_HEIGHT;
                width : cARROW_WIDTH;
                transform: [
                    Translate { x: -cARROW_WIDTH; y: -cARROW_HEIGHT/2; },
                    Rotation { angle: (rot - 90); } ]
            }
            visible   : mImgMeaView;
        }
    }

    MapItemView //measure circle
    {
        model: mMeaModel;
        delegate: MapCircle
        {
            border { color: "green"; width: cBORDER_WIDTH; }
            center : toGeoCoordinate(from.latitude, from.longitude);
            radius : getRadius();
            color  : "#90EE90"; //light green
            opacity: cCIRCLE_OPACITY;
            visible: isEnd && mMeaCircle && mImgMeaView;

            function getRadius()
            {
                //radius in meter
                return getDistance(from.latitude, from.longitude,
                                   to.latitude, to.longitude) * 1000;
            }
        }
    }

    MapItemView //measure distance label
    {
        model   : mMeaModel;
        delegate: MapQuickItem
        {
            anchorPoint: (rot <= 160)?
                         toPoint(0, cICON_HEIGHT):
                         toPoint(lblMeasure.width,
                                 (rot <= 270)? 0: cICON_HEIGHT/2);
            coordinate : toGeoCoordinate(to.latitude, to.longitude);
            sourceItem : Text
            {
                id        : lblMeasure;
                color     : (mGisInt.isDarkMode())? "white": "black";
                text      : lblTxt;
            }
            visible    : mLblMeaView &&
                         (getDistance(from.latitude, from.longitude,
                                      to.latitude, to.longitude) > 0);
        }
    }

    MapItemView //routing line
    {
        model   : mRouteModel;
        delegate: MapRoute
        {
            line { color: "aqua"; width: cLINE_WIDTH2; }
            route : routeData;
            smooth: true;
        }
        visible : mImgRteView;
    }

    MapItemView //routing icon model
    {
        model   : mRouteIcnModel;
        delegate: MapQuickItem
        {
            anchorPoint: toPoint(imgRouteIcon.width/2, imgRouteIcon.height);
            coordinate : toGeoCoordinate(lat, lon);
            sourceItem : Image
            {
                id    : imgRouteIcon;
                source: imgSrc;
                height: cICON_HEIGHT;
                width : cICON_WIDTH/2;
            }
        }
        visible: mImgRteView;
    }

    MapItemView //routing label
    {
        model   : mRouteIcnModel;
        delegate: MapQuickItem
        {
            anchorPoint: toPoint(-cICON_WIDTH/4, lblRouteTxt.height/2);
            coordinate : toGeoCoordinate(lat, lon);
            sourceItem : Text
            {
                id        : lblRouteTxt;
                color     : "white";
                font.bold : true;
                style     : Text.Outline;
                styleColor: "black";
                text      : txt;
            }
            visible    : mLblRteView;
        }
    }

    MapItemView //pin icon
    {
        model   : mSearchModel;
        delegate: MapQuickItem
        {
            anchorPoint: toPoint(0, imgPinIcon.height);
            coordinate : toGeoCoordinate(lat, lon);
            sourceItem : Image
            {
                id    : imgPinIcon;
                source: cICON_PIN;
                height: cICON_HEIGHT;
                width : cICON_WIDTH;
            }
        }
        visible: mImgPinView;
    }

    MapItemView //pin label
    {
        model   : mSearchModel;
        delegate: MapQuickItem
        {
            anchorPoint: toPoint(-cICON_WIDTH/4, lblPinTxt.height/2);
            coordinate : toGeoCoordinate(lat, lon);
            sourceItem : Text
            {
                id        : lblPinTxt;
                color     : "white";
                font.bold : true;
                style     : Text.Outline;
                styleColor: "black";
                text      : txt;
            }
            visible    : mLblPinView;
        }
    }

    function getMouseGeoCoordinate() { return mMouseArea.coor; }

    MouseArea
    {
        id                  : mMouseArea;
        acceptedButtons     : Qt.LeftButton | Qt.RightButton;
        anchors.fill        : parent;
        enabled             : !mOvr;
        hoverEnabled        : true;
        scrollGestureEnabled: false;

        property double startX;
        property double startY;
        property var coor: toCoordinate(toPoint(mouseX, mouseY));
        property Rectangle zoomItem: null;

        Component //zoom rectangle
        {
            id: rectangleZoom;
            Rectangle
            {
                border { color: "red"; width: cBORDER_WIDTH; }
                color: "transparent";
            }
        }

        onEntered: mGisInt.setMouseCursor(mMode);

        onExited: mGisInt.setMouseCursor();

        onPressed: (mouse) =>
        {
            if (mouse.button !== Qt.LeftButton)
                return;
            if (mMode === GisQmlInt.MODE_SELECT)
            {
                let c = center;
                mapViewChanged(toPoint(c.longitude, c.latitude), mZoom);
            }
            else if (mMode === GisQmlInt.MODE_ZOOM ||
                     mMode === GisQmlInt.MODE_RESOURCES)
            {
                if (zoomItem != null)
                    zoomItem.destroy(); //delete existing zoom rectangle
                //create zoom rectangle
                zoomItem = rectangleZoom.createObject(mMouseArea,
                                                      { x: mouseX, y: mouseY });
                startX = mouseX;
                startY = mouseY;
                if (mMode === GisQmlInt.MODE_RESOURCES)
                    zoomItem.border.color = "magenta";
            }
        }

        onClicked:
        {
            if (mouse.button === Qt.RightButton) //right click
            {
                mResLst = [];
                getItems("", true, resolution() * cICON_HEIGHT/2, coor.latitude,
                         coor.longitude);
                customContextMenuRequested(
                               toPoint(coor.longitude, coor.latitude), mResLst);
            }
            //left click
            else if (mMode === GisQmlInt.MODE_MEASURE)
            {
                mMeaCount++;
                let totDist = 0;
                if (mMeaCount > 0 && mIsMeasure)
                {
                    let prevPos = mMeaCount - 1;
                    mMeaModel.set(prevPos, { isEnd: false });
                    let prevData = mMeaModel.get(prevPos);
                    totDist = prevData.totDist +
                              getDistance(prevData.from.latitude,
                                          prevData.from.longitude,
                                          prevData.to.latitude,
                                          prevData.to.longitude);
                }
                else
                {
                    mIsMeasure = true;
                }
                mMeaModel.append({ from   : coor,
                                   to     : coor,
                                   rot    : 0,
                                   totDist: totDist,
                                   lblTxt : "",
                                   isEnd  : true });
                measureUpdate(mMeasureUnit, false);
            }
            else if (mMode === GisQmlInt.MODE_ROUTE)
            {
                routeUpdate(mRouteModel.srcPt, coor.latitude, coor.longitude);
                pointCoordinates((mRouteModel.srcPt)?
                                     GisQmlInt.TYPEID_ROUTE_START:
                                     GisQmlInt.TYPEID_ROUTE_END,
                                 coor.longitude, coor.latitude);
            }
        }

        onDoubleClicked:
        {
            if (mouse.button !== Qt.LeftButton)
                return;
            if (mMode === GisQmlInt.MODE_MEASURE)
            {
                mMeaModel.remove(mMeaCount); //remove last data
                mMeaCount--;
                let d = mMeaModel.get(mMeaCount);
                if (getDistance(d.from.latitude, d.from.longitude,
                                d.to.latitude, d.to.longitude) === 0)
                {
                    mMeaModel.remove(mMeaCount);
                    mMeaCount--;
                }
                if (mMeaCount >= 0)
                    mMeaModel.set(mMeaCount, { isEnd : true });
                mIsMeasure = false;
            }
            else if (mMode === GisQmlInt.MODE_SELECT)
            {
                if (mZoom === mZoomMax)
                    return;
                let geoPos = toCoordinate(toPoint(mouseX, mouseY));
                let preZoomPt = fromCoordinate(geoPos, false);
                let newZoom = mZoom + mZoomStep;
                mZoom = (newZoom <= mZoomMax)? newZoom: mZoomMax;
                mapZoomChanged(mZoom);
                let postZoomPt = fromCoordinate(geoPos, false);
                center = toCoordinate(toPoint(
                                        width/2 + postZoomPt.x - preZoomPt.x,
                                        height/2 + postZoomPt.y - preZoomPt.y));
            }
        }

        onPositionChanged: (mouse) =>
        {
            if (isNaN(coor.latitude) || isNaN(coor.longitude))
                return;
            if (mMode === GisQmlInt.MODE_MEASURE && mIsMeasure &&
                mMeaCount >= 0)
            {
                let fr = mMeaModel.get(mMeaCount).from;
                mMeaModel.set(mMeaCount,
                              { to : coor,
                                rot: toGeoCoordinate(fr.latitude, fr.longitude)
                                     .azimuthTo(coor) });
                measureUpdate(mMeasureUnit, false);
            }
            else if (zoomItem != null &&
                     (mMode === GisQmlInt.MODE_ZOOM ||
                      mMode === GisQmlInt.MODE_RESOURCES))
            {
                //update zoom rectangle
                if (mouseX - startX < 0 )
                    zoomItem.x = mouseX;
                if (mouseY - startY < 0)
                    zoomItem.y = mouseY;
                zoomItem.width = Math.abs(mouseX - startX);
                zoomItem.height = Math.abs(mouseY - startY);
            }
            if (mDebug)
                mouseMovedDbg(coor.longitude, coor.latitude, mouseX, mouseY);
            else
                mouseMoved(coor.longitude, coor.latitude);
        }

        onReleased:
        {
            if (mouse.button !== Qt.LeftButton ||
                (mMode !== GisQmlInt.MODE_ZOOM &&
                 mMode !== GisQmlInt.MODE_RESOURCES))
                return;
            let x = zoomItem.x;
            let y = zoomItem.y;
            let w = zoomItem.width;
            let h = zoomItem.height;
            let tl = toCoordinate(toPoint(x, y)); //top-left
            let br = toCoordinate(toPoint(x + w, y + h)); //btm-right
            if (mMode === GisQmlInt.MODE_ZOOM)
            {
                let c = toCoordinate(toPoint(x + w/2, y + h/2)); //center
                //calculate zoom level based on lat/lon difference
                let latDiff = Math.abs(tl.latitude - br.latitude);
                let lonDiff = Math.abs(tl.longitude - br.longitude);
                let zl = 0;
                if (latDiff > lonDiff)
                    zl = Math.log(height/latDiff)/Math.log(2);
                else
                    zl = Math.log(width/lonDiff)/Math.log(2);
                zl = Math.ceil(zl * 2)/2;
                if (zl >= mZoom && zl < mZoomMax)
                    mZoom = zl;
                else if (zl === mZoomMax)
                    mZoom = mZoomMax;
                if (zl <= mZoomMax)
                    mapZoomChanged(zl);
                center = c;
            }
            else if (mResModel.count > 0)
            {
                x = 1;
                mResLst = [];
                let d;
                let i = mResModel.count - 1;
                for (; i>=0; --i)
                {
                    d = mResModel.get(i);
                    if (d.lat >= br.latitude && d.lat <= tl.latitude &&
                        d.lon >= tl.longitude && d.lon <= br.longitude)
                    {
                        mResLst.push(d.id);
                        x = 0; //just to indicate non-empty
                    }
                }
                if (x === 0)
                    rscSelect(mResLst);
            }
            zoomItem.destroy();
        }

        onWheel: (wheel)=>
        {
            if ((mZoom === mZoomMax && wheel.angleDelta.y > 0) ||
                (mZoom === mZoomMin && wheel.angleDelta.y < 0))
                return;
            let geoPos = toCoordinate(toPoint(mouseX, mouseY));
            let preZoomPt = fromCoordinate(geoPos, false);
            if (wheel.angleDelta.y > 0)
            {
                let newZoom = mZoom + mZoomStep;
                mZoom = (newZoom <= mZoomMax)? newZoom: mZoomMax;
            }
            else if (wheel.angleDelta.y < 0)
            {
                let newZoom = mZoom - mZoomStep;
                mZoom = (newZoom >= mZoomMin)? newZoom: mZoomMin;
            }
            mapZoomChanged(mZoom);
            let postZoomPt = fromCoordinate(geoPos, false);
            center = toCoordinate(toPoint(width/2 + postZoomPt.x - preZoomPt.x,
                                        height/2 + postZoomPt.y - preZoomPt.y));
        }
    }

    //temporary function to get online server address simulating
    //mGisInt.getMapPath() for seamarker (!isWms)
    function getSvrAddress(isWms: Boolean)
    {
        if (isWms)
            return mGisInt.getMapPath();
        return "https://tiles.openseamap.org/";
    }

    function updateSeaTiles(model: ListModel, isWms: bool)
    {
        mFactor = (((mZoom * 2) % 2) !== 0)? 1.415: 1.0;
        const z = Math.floor(mZoom);
        //tile dimensions and central coordinates
        const widthInTiles = Math.ceil(width/cTILE_WIDTH);
        const heightInTiles = Math.ceil(height/cTILE_WIDTH);
        const startX = getTileX(center.longitude, z) -
                       Math.floor(widthInTiles/2) - 1;  //add extra left column
        const startY = getTileY(center.latitude, z) -
                       Math.floor(heightInTiles/2) - 1; //add extra top row
        const maxTiles = Math.pow(2, z); //total number of tiles at zoom level
        let tiles = [];
        let newTiles = {};
        //calculate required tiles
        let x = startX + widthInTiles + 1; //add extra right column
        for (; x>=startX; --x)
        {
            let y = startY + heightInTiles + 1; //add extra bottom row
            for (; y>=startY; --y)
            {
                let tileX = (x + maxTiles) % maxTiles;;
                if (y < cGeoMinY[z] || y >= maxTiles)
                    continue; //skip invalid tiles
                let tileY = y;
                let coor = getCtrTileCoor(tileX, tileY, z);
                tileY = (isWms)? maxTiles - 1 - tileY: tileY;
                let tileKey = `${tileX}_${tileY}`;
                tiles.push({ x  : tileX,
                             y  : tileY,
                             lat: coor.latitude,
                             lon: coor.longitude });
                newTiles[tileKey] = true;
            }
        }
        //remove outdated tiles from model
        let seaTile = {};
        let i = model.count - 1;
        for (; i>=0; --i)
        {
            seaTile = model.get(i);
            const modelKey = `${seaTile.x}_${seaTile.y}`;
            if (newTiles[modelKey])
                delete newTiles[modelKey]; //remove tile already in the model
            else
                model.remove(i); //remove tile not in the updated list
        }
        //add new tiles to model
        for (let tile of tiles)
        {
            let tileKey = `${tile.x}_${tile.y}`;
            if (!newTiles[tileKey])
                continue;
            let tileUrl = (isWms)? mWmsUrl.replace("{z}", z)
                                          .replace("{x}", tile.x)
                                          .replace("{y}", tile.y):
                                   mTmsUrl.replace("{z}", z)
                                          .replace("{x}", tile.x)
                                          .replace("{y}", tile.y);
            const tileFileName = ((isWms)? "sd_": "sm_") +
                                 mTileName.replace("{z}", z)
                                          .replace("{x}", tile.x)
                                          .replace("{y}", tile.y);
            let filePath = mGisInt.getCachePath() + tileFileName;
            if (mGisInt.fileExists(filePath))
            {
                filePath = `file:///${filePath}`;
            }
            else
            {
                tileUrl = getSvrAddress(isWms) + tileUrl;
                mGisInt.downloadTile(tileUrl, filePath);
                filePath = tileUrl; //use online tile while downloading
            }
            model.append({ x  : tile.x,
                           y  : tile.y,
                           lat: tile.lat,
                           lon: tile.lon,
                           src: filePath });
        }
    }

    function getTileX(lon, zoom: Number)
    {
        return Math.floor(((lon + 180) * Math.pow(2, zoom))/360);
    }

    function getTileY(lat, zoom: Number)
    {
        let latRad = (lat * Math.PI)/180;
        return Math.floor(Math.pow(2, zoom) *
           (1 - (Math.log(Math.tan(latRad) + (1/Math.cos(latRad))))/Math.PI)/2);
    }

    function getCtrTileCoor(x: Number, y: int, z: Number)
    {
        let n = Math.pow(2, z);
        //convert x, y to longitude, latitude
        return QtPositioning.coordinate((Math.atan(Math.sinh(Math.PI *
                                             (1 - ((2 * y)/n)))) * 180)/Math.PI,
                                        ((x * 360)/n) - 180);
    }

    function updateFocus()
    {
        if (mIsReady)
        {
            let topLeft = toCoordinate(toPoint(0, 0), false);
            let btmRight = toCoordinate(toPoint(width, height), false);
            overviewRectChanged(topLeft.latitude, topLeft.longitude,
                                btmRight.latitude, btmRight.longitude);
            if (mSeaMap && !mOvr)
            {
                if (mLyrSeaDepth)
                    updateSeaTiles(mSeaDepthModel, true);
                if (mZoom > 8 && mLyrSeaMarker)
                    updateSeaTiles(mSeaMarkerModel, false);
            }
        }
    }

    function updateRectView(tl: QtPositioning, br: QtPositioning)
    {
        rectView.topLeft = tl;
        rectView.bottomRight = (tl.distanceTo(br) < rectView.minDiagDist)?
                               tl.atDistanceAndAzimuth(rectView.minDiagDist,
                                                       135):
                               br;
    }
}
