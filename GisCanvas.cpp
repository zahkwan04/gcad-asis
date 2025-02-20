/**
 * Qt map canvas implementation.
 *
 * Copyright (C) Sapura Secured Technologies, 2013-2025. All Rights Reserved.
 *
 * @file
 * @version $Id: GisCanvas.cpp 1898 2025-01-17 00:50:10Z rosnin $
 * @author Muhd Hashim Wahab
 */
#include <QApplication>
#include <QLabel>
#include <QMessageBox>
#include <QtQuick>
#include <QWidgetAction>

#include "GisMenuTerminal.h"
#include "GisMenuUserPoi.h"
#include "GisPoint.h"
#ifdef INCIDENT
#include "IncidentButton.h"
#endif
#include "InputDialog.h"
#include "QtUtils.h"
#include "Settings.h"
#include "Style.h"
#include "SubsData.h"
#include "GisCanvas.h"

using namespace std;

//ActionData type values
enum
{
    //values initialized to CmnTypes values must come first to avoid overlap
    ACTION_RSC_CALL     = CmnTypes::ACTIONTYPE_CALL,
    ACTION_RSC_MON      = CmnTypes::ACTIONTYPE_MON,
    ACTION_RSC_SDS      = CmnTypes::ACTIONTYPE_SDS,
    ACTION_RSC_STAT     = CmnTypes::ACTIONTYPE_STATUS,
    ACTION_COORDS,
    ACTION_COORDS_DMS,
    ACTION_NEW_INC,
    ACTION_NEW_POI,
    ACTION_SRCH,
    ACTION_RSC,
    ACTION_RSC_TRL_ON,
    ACTION_RSC_TRL_OFF,
    ACTION_VIEW_INC,
    ACTION_VIEW_POI,
    ACTION_VIEW_RSC,
    ACTION_ZOOM
};

static const double COORD_INVALID = 999.0;
static const int    TIMER_TERMINAL_MIN_MS =
                                   Settings::TERMINAL_TIMER_MIN_MINUTES * 60000;

static const QString QML_FILE  ("qrc:/Qml/qml/Canvas.qml");
static const QString ICON_POI  (":/Images/images/icon_dropPin.png");
static const QString ICON_SRCH (":/Images/images/icon_search_nearby.png");
static const QString ICON_TRAIL(":/Images/images/icon_trail.png");
static const QString ICON_ZOOM (":/Images/images/icon_zoomIn.png");

//context menu action data
struct ActionData
{
    ActionData(int t) : type(t), lat(COORD_INVALID) {}
    ActionData(int t, int v) : type(t), val(v) {}
    ActionData(int t, QPointF &c) : type(t), lat(c.y()) , lon(c.x()) {}
    ActionData() : type(-1) {}

    int    type;
    int    val = 0; //type-dependent value
    double lat = 0;
    double lon = 0;
};
Q_DECLARE_METATYPE(ActionData) //to use in QVariant

GisCanvas::GisCanvas(Logger        *logger,
                     const QString &userName,
                     bool           overview,
                     QQuickWidget  *qw,
                     QWidget       *parent) :
QGraphicsView(parent), mOverview(overview), mValid(false),
#ifdef INCIDENT
mLockedIncidentId(0),
#endif
mTypeId(GisQmlInt::TYPEID_NONE), mSrchRadius(0), mUserName(userName),
mLogger(logger)
{
    if (logger == 0 || qw == 0)
    {
        assert("Bad param in GisCanvas::GisCanvas" == 0);
        return;
    }
    qw->setResizeMode(QQuickWidget::SizeRootObjectToView);
    //register GisQmlInt with QML
    qmlRegisterType<GisQmlInt>("gisInt", 1, 0, "GisQmlInt");
    qw->setSource(QUrl(QML_FILE)); //load QML file to widget
    mMap = qw->rootObject();
#ifdef DEBUG
    mMap->setProperty("mDebug", bool(true));
#endif
#ifdef GIS_SEAMAP
    mMap->setProperty("mSeaMap", bool(true));
#endif
    mMap->setProperty("mZoomLvlMax", getMaxZoomLevel());
    mMap->setProperty("mZoomLvlMin", getMinZoomLevel());
    mMap->setProperty("mOverview", mOverview);
    if (!mOverview)
    {
        setCtrRscInCall(Settings::instance()
                        .get<bool>(Props::FLD_CFG_MAP_CTR_RSC_CALL));
        //must cover all SubsData::eTerminalType
        mTerminalTypes[SubsData::TERMINALTYPE_HANDHELD]  = "Hh";
        mTerminalTypes[SubsData::TERMINALTYPE_VEHICULAR] = "Vh";
        mTerminalTypes[SubsData::TERMINALTYPE_BRIEFCASE] = "Bc";
        mTerminalTypes[SubsData::TERMINALTYPE_COVERT]    = "Cv";
        mTerminalTypes[SubsData::TERMINALTYPE_DESKTOP]   = "Dt";
        mTerminalTypes[SubsData::TERMINALTYPE_MARINEKIT] = "Mk";
        mTerminalTypes[SubsData::TERMINALTYPE_RADIODISP] = "Rd";
        mTerminalTypes[SubsData::TERMINALTYPE_RIM]       = "Rm";
        mTerminalTypes[SubsData::TERMINALTYPE_RUCKSACK]  = "Rs";
        mTerminalTypes[SubsData::TERMINALTYPE_PHONE]     = "Ph";
        mTerminalTypes[SubsData::TERMINALTYPE_BWC]       = "Bw";

        connect(mMap, SIGNAL(mapLayerLoaded()), SLOT(onMapLayerLoaded()));
        connect(mMap, SIGNAL(mapZoomChanged(double)),
                SIGNAL(zoomChanged(double)));
        connect(mMap, SIGNAL(overviewRectChanged(double,double,double,double)),
                SIGNAL(mapRectChanged(double,double,double,double)));
        connect(mMap, SIGNAL(mapViewChanged(QPointF,double)),
                SIGNAL(viewChanged(QPointF,double)));
        connect(mMap, SIGNAL(customContextMenuRequested(QPointF,QVariant)),
                SLOT(onContextMenu(QPointF,QVariant)));
        connect(mMap, SIGNAL(rscSelect(QVariant)), SLOT(onRscSelect(QVariant)));
        connect(mMap, SIGNAL(pointCoordinates(int,double,double)),
                SIGNAL(pointCoordinates(int,double,double)));
        connect(mMap, SIGNAL(searchDone(QVariant,QVariant)),
                SLOT(onSearchDone(QVariant,QVariant)));
        connect(mMap, SIGNAL(searchCtrDone(QVariant,QVariant,QPointF,QString)),
                SLOT(onSearchCtrDone(QVariant,QVariant,QPointF,QString)));
#ifdef DEBUG
        connect(mMap, SIGNAL(mouseMovedDbg(double,double,double,double)),
                SIGNAL(mapCoordinates(double,double,double,double)));
#else
        connect(mMap, SIGNAL(mouseMoved(double,double)),
                SIGNAL(mapCoordinates(double,double)));
#endif
    } //if (!mOverview)
}

GisCanvas::~GisCanvas()
{
    delete mMap;
}

void GisCanvas::refresh()
{
    QMetaObject::invokeMethod(mMap, "refresh");
}

void GisCanvas::refreshMap()
{
    QMetaObject::invokeMethod(mMap, "refreshMap");
}

void GisCanvas::setGeomCenter(QPointF center, int zoomLvl)
{
    if (zoomLvl < 0)
        zoomLvl = GisQmlInt::ZOOM_SEARCH;
    QMetaObject::invokeMethod(mMap, "setGeomCenter", Q_ARG(double, center.y()),
                              Q_ARG(double, center.x()), Q_ARG(int, zoomLvl));
}

void GisCanvas::setZoomLevel(double zoomLvl)
{
    if (mValid)
        QMetaObject::invokeMethod(mMap, "setZoomLevel", Q_ARG(double, zoomLvl));
}

void GisCanvas::setMode(int mode, int typeId)
{
    if (mValid && (mode != getMode() || typeId != mTypeId))
    {
        switch (mode)
        {
            case GisQmlInt::MODE_MEASURE:
                QMetaObject::invokeMethod(mMap, "setMeasureTl");
                break;
            case GisQmlInt::MODE_RESOURCES:
                QMetaObject::invokeMethod(mMap, "setRscMode");
                break;
            case GisQmlInt::MODE_ROUTE:
                QMetaObject::invokeMethod(mMap, "setRoute",
                        Q_ARG(bool, (typeId == GisQmlInt::TYPEID_ROUTE_START)));
                break;
            case GisQmlInt::MODE_ZOOM:
                QMetaObject::invokeMethod(mMap, "setZoomTl");
                break;
            case GisQmlInt::MODE_SELECT:
            default:
                QMetaObject::invokeMethod(mMap, "setSelectTl");
                break;
        }
        emit modeSet();
    }
    mTypeId = typeId;
}

int GisCanvas::getMode()
{
    QVariant ret;
    QMetaObject::invokeMethod(mMap, "getMode", Q_RETURN_ARG(QVariant, ret));
    return ret.toInt();
}

QPointF GisCanvas::getGeomCenter() const
{
    QVariant ret;
    QMetaObject::invokeMethod(mMap, "getGeomCenter",
                              Q_RETURN_ARG(QVariant, ret));
    return QPointF(ret.toPointF().x(), ret.toPointF().y());
}

double GisCanvas::getZoomLevel() const
{
    QVariant ret;
    QMetaObject::invokeMethod(mMap, "getZoomLevel", Q_RETURN_ARG(QVariant, ret));
    return ret.toDouble();
}

int GisCanvas::getModelType(const string &layer)
{
    if (mLayerModels.count(layer) == 0)
        return 0;
    return mLayerModels[layer];
}

void GisCanvas::updateLabelLayer(int typeId)
{
    //for now this is called only for TYPEID_TERMINAL
    if (typeId != GisQmlInt::TYPEID_TERMINAL)
    {
        LOGGER_ERROR(mLogger, "GisCanvas::updateLabelLayer: "
                              "Invalid typeId: " << typeId);
        return;
    }
    QMetaObject::invokeMethod(mMap, "updateResourceLabels");
}

double GisCanvas::getMapScale() const
{
    QVariant ret;
    QMetaObject::invokeMethod(mMap, "mapScale", Q_RETURN_ARG(QVariant, ret));
    return ret.toDouble();
}

void GisCanvas::clearData()
{
    QMetaObject::invokeMethod(mMap, "deleteItems",
                              Q_ARG(int, GisQmlInt::TYPEID_INCIDENT));
    QMetaObject::invokeMethod(mMap, "deleteItems",
                              Q_ARG(int, GisQmlInt::TYPEID_POI));
}

#ifdef INCIDENT
void GisCanvas::setIncidentLock(int id)
{
    mLockedIncidentId = id;
    if (id != 0)
        incidentClose(0); //remove temporary incident point
}

void GisCanvas::incidentUpdate(IncidentData *data, int typeId)
{
    if (data == 0 ||
        (typeId != GisQmlInt::TYPEID_INCIDENT &&
         typeId != GisQmlInt::TYPEID_INCIDENT_REPORT))
    {
        assert("Bad param in GisCanvas::incidentUpdate" == 0);
        return;
    }
    if (mValid && data->hasLocation())
        QMetaObject::invokeMethod(mMap, "incidentPt", Q_ARG(int, typeId),
                                  Q_ARG(int, data->getId()),
                                  Q_ARG(QString,
                                        (data->getCategory() == "Others")?
                                            "default": data->getCategory()),
                                  Q_ARG(int, data->getState()),
                                  Q_ARG(double, data->getLat()),
                                  Q_ARG(double, data->getLon()));
}

void GisCanvas::incidentClose(int id)
{
    if (mValid)
        QMetaObject::invokeMethod(mMap, "deleteItem",
                                  Q_ARG(int, GisQmlInt::TYPEID_INCIDENT),
                                  Q_ARG(int, id));
}

void GisCanvas::incidentPlot(double lat, double lon)
{
    //coordinates must have been validated
    assert(GisPoint::isInBounds(lat, lon));
    if (checkLayerVisible(GisQmlInt::TYPEID_INCIDENT))
        setGeomCenter(QPointF(lon, lat), GisQmlInt::ZOOM_SEARCH);
}

void GisCanvas::incidentsReportClear()
{
    if (mValid)
        QMetaObject::invokeMethod(mMap, "deleteItems",
                                  Q_ARG(int, GisQmlInt::TYPEID_INCIDENT_REPORT));
}
#endif //INCIDENT

void GisCanvas::setRscInCall(int issi, bool start)
{
    QMetaObject::invokeMethod(mMap, "setRscInCall", Q_ARG(int, issi),
                              Q_ARG(bool, start));
}

void GisCanvas::setCtrRscInCall(bool enable)
{
    mMap->setProperty("mCtrRscInCall", enable);
}

void GisCanvas::terminalLocate(int issi)
{
    Props::ValueMapT prs;
    onShowItem(prs, Utils::toString(issi),
             GisQmlInt::getModelName(GisQmlInt::TYPEID_TERMINAL).toStdString());
}

void GisCanvas::terminalUpdate(bool          isValid,
                               int           issi,
                               double        lon,
                               double        lat,
                               const string &timestamp)
{
    if (!mValid)
        return;
    QVariant ret;
    QMetaObject::invokeMethod(mMap, "locateItem", Q_RETURN_ARG(QVariant, ret),
                              Q_ARG(int, GisQmlInt::TYPEID_TERMINAL),
                              Q_ARG(int, issi), Q_ARG(bool, false));
    if (!isValid && ret.toInt() < 0)
        return;
    int type = SubsData::getIssiType(issi);
    if (mTerminalTypes.count(type) == 0)
        type = SubsData::TERMINALTYPE_DEFAULT; //default for unknown type
    string s;
    if (isValid)
    {
        //timestamp format is "YYYY-MM-dd hh:mm:ss"
        s = Utils::getTimeStr(timestamp, "%d-%d-%d %d:%d:%d");
        if (s.empty())
            LOGGER_ERROR(mLogger, "GisCanvas::terminalUpdate: "
                         "Invalid timestamp: " << timestamp);
    }
    QMetaObject::invokeMethod(mMap, "locationUpdate", Q_ARG(int, issi),
                              Q_ARG(int, type),
                              Q_ARG(double, lat), Q_ARG(double, lon),
                              Q_ARG(QString, getTerminalIconPfx(type)),
                              Q_ARG(QString, QString::fromStdString(timestamp)),
                              Q_ARG(QString, QString::fromStdString(s)));

}

void GisCanvas::terminalsShow(bool show, int type)
{
    QMetaObject::invokeMethod(mMap, "terminalsShow", Q_ARG(bool, show),
                              Q_ARG(int, type));
}

void GisCanvas::terminalsFilter(const set<int> *issis)
{
    QVariantList l;
    if (issis != 0)
    {
        for (auto i : *issis)
        {
            l << i;
        }
    }
    mMap->setProperty("mRscFilter", l);
}

void GisCanvas::terminalUpdateType(int issi, int type)
{
    if (mTerminalTypes.count(type) == 0)
        type = SubsData::TERMINALTYPE_DEFAULT; //default for unknown type
    QMetaObject::invokeMethod(mMap, "terminalSetType", Q_ARG(int, issi),
                              Q_ARG(int, type),
                              Q_ARG(QString, getTerminalIconPfx(type)));
}

void GisCanvas::terminalRemove(int issi)
{
    if (issi == 0)
        QMetaObject::invokeMethod(mMap, "deleteItems",
                                  Q_ARG(int, GisQmlInt::TYPEID_TERMINAL));
    else
        QMetaObject::invokeMethod(mMap, "deleteItem",
                                  Q_ARG(int, GisQmlInt::TYPEID_TERMINAL),
                                  Q_ARG(int, issi));
}

void GisCanvas::terminalRemove(bool rmList, const set<int> &issis)
{
    QVariantList l;
    if (rmList)
    {
        for (auto i : issis)
        {
            l << i;
        }
    }
    else
    {
        QVariant ret;
        QMetaObject::invokeMethod(mMap, "getTerminals",
                                  Q_RETURN_ARG(QVariant, ret));
        int i;
        for (auto &s : QtUtils::tokenize(ret.toString()))
        {
            i = s.toInt(); //collect those not in issis
            if (i > 0 && issis.count(i) == 0)
                l << i;
        }
    }
    if (!l.isEmpty())
        QMetaObject::invokeMethod(mMap, "terminalsErase",
                                  Q_ARG(QVariant, QVariant::fromValue(l)));
}

void GisCanvas::terminalCheckTimeChanged()
{
    const Settings &cfg(Settings::instance());
    time_t stale1 = cfg.get<time_t>(Props::FLD_CFG_MAP_TERM_STALE1);
    time_t staleLast = cfg.get<time_t>(Props::FLD_CFG_MAP_TERM_STALELAST);
    if (stale1 == 0 && staleLast == 0)
        QMetaObject::invokeMethod(mMap, "timerStop");
    else
        QMetaObject::invokeMethod(mMap, "setTerminalCheckTimes",
                                  Q_ARG(int, stale1), Q_ARG(int, staleLast));
}

void GisCanvas::terminalTrailing(int id, bool enabled)
{
    if (mValid)
        QMetaObject::invokeMethod(mMap, "terminalTrailing", Q_ARG(int, id),
                                  Q_ARG(bool, enabled));
}

bool GisCanvas::hasTerminals()
{
    QVariant ret;
    QMetaObject::invokeMethod(mMap, "hasItem", Q_RETURN_ARG(QVariant, ret),
                              Q_ARG(int, GisQmlInt::TYPEID_TERMINAL));
    return ret.toBool();
}

bool GisCanvas::getTerminalTrailing(set<int> &terms, set<int> &trail)
{
    QVariant ret;
    QMetaObject::invokeMethod(mMap, "getTerminals", Q_RETURN_ARG(QVariant, ret));
    if (ret.toString().isEmpty())
        return false;
    foreach (QString str, QtUtils::tokenize(ret.toString()))
    {
        terms.insert(str.toInt());
    }
    QMetaObject::invokeMethod(mMap, "getTrailings", Q_RETURN_ARG(QVariant, ret));
    foreach (QString str, QtUtils::tokenize(ret.toString()))
    {
        trail.insert(str.toInt());
    }
    return true;
}

QString GisCanvas::getTerminalIconPfx(int type)
{
    switch (type)
    {
        case SubsData::TERMINALTYPE_BRIEFCASE:
            type = SubsData::TERMINALTYPE_RUCKSACK;
            break;
        case SubsData::TERMINALTYPE_BWC:
        case SubsData::TERMINALTYPE_DESKTOP:
        case SubsData::TERMINALTYPE_HANDHELD:
        case SubsData::TERMINALTYPE_MARINEKIT:
        case SubsData::TERMINALTYPE_PHONE:
        case SubsData::TERMINALTYPE_RUCKSACK:
        case SubsData::TERMINALTYPE_VEHICULAR:
            break; //do nothing
        case SubsData::TERMINALTYPE_RADIODISP:
            type = SubsData::TERMINALTYPE_DESKTOP;
            break;
        case SubsData::TERMINALTYPE_COVERT:
        case SubsData::TERMINALTYPE_RIM:
        default:
            type = SubsData::TERMINALTYPE_HANDHELD;
            break;
    }
    return QString::fromStdString(mTerminalTypes[type]).toLower();
}

void GisCanvas::typeCancelled(int typeId)
{
    if (typeId != GisQmlInt::TYPEID_NONE)
        QMetaObject::invokeMethod(mMap, "deleteItems", Q_ARG(int, typeId));
}

void GisCanvas::search(const QString &key,
                       const QPointF *ctrPt,
                       const QString &label)
{
    QPointF point;
    int val = GisPoint::checkCoords(this, tr("Search Error"), key, false,
                                    point);
    if (val != GisPoint::COORD_NONE)
    {
        //coordinates search
        if (val == GisPoint::COORD_VALID)
        {
            Props::ValueMapT prs;
            Props::set(prs, Props::FLD_LBL,
                       Utils::toString(point.y()) + ',' +
                           Utils::toString(point.x()));
            Props::set(prs, Props::FLD_LAT, point.y());
            Props::set(prs, Props::FLD_LON, point.x());
            onShowItem(prs, "0",
               GisQmlInt::getModelName(GisQmlInt::TYPEID_SEARCH).toStdString());
        }
        return;
    }
    //non-coordinates search
    if (ctrPt == 0)
        QMetaObject::invokeMethod(mMap, "search", Q_ARG(QString, key));
    else
        QMetaObject::invokeMethod(mMap, "searchNearby", Q_ARG(QString, key),
                                  Q_ARG(QString, label),
                                  Q_ARG(double, mSrchRadius),
                                  Q_ARG(double, ctrPt->y()),
                                  Q_ARG(double, ctrPt->x()));
}

void GisCanvas::getTerminalsNearby(const QPointF &pt, const QString &radius)
{
    mTypeId = GisQmlInt::TYPEID_INCIDENT;
    QMetaObject::invokeMethod(mMap, "searchNearby", Q_ARG(QString, ""),
                              Q_ARG(QString, ""),
                              Q_ARG(double, radius.toDouble()),
                              Q_ARG(double, pt.y()),
                              Q_ARG(double, pt.x()));
}

void GisCanvas::addTrackingLine(int issi, const string &coord, bool isFirst)
{
    if (!mValid)
        return;
    size_t pos = coord.find(",");
    QString lat(coord.substr(0, pos).c_str());
    QString lon(coord.substr(pos + 1).c_str());
    QMetaObject::invokeMethod(mMap, "addTrackingLine", Q_ARG(int, issi),
                              Q_ARG(double, lat.toDouble()),
                              Q_ARG(double, lon.toDouble()));
    if (isFirst) //center view for first point only
        setGeomCenter(QPointF(lon.toDouble(), lat.toDouble()),
                      GisQmlInt::ZOOM_TRACK);
}

void GisCanvas::deleteTrackingLineSegments(int issi, int num)
{
    if (num < 0)
        QMetaObject::invokeMethod(mMap, "deleteItems",
                                  Q_ARG(int, GisQmlInt::TYPEID_TRACKING));
    else if (num != 0)
        QMetaObject::invokeMethod(mMap, "deleteItemLastNodes", Q_ARG(int, issi),
                                  Q_ARG(int, num));
}

void GisCanvas::updateMeasurements(int unit)
{
    QMetaObject::invokeMethod(mMap, "measureUpdate", Q_ARG(int, unit),
                              Q_ARG(bool, true));
}

void GisCanvas::updateView(bool label, bool show, const QString &layer)
{
    QMetaObject::invokeMethod(mMap,
                              (label)? "updateLabelView": "updateIconView",
                              Q_ARG(int, getModelType(layer.toStdString())),
                              Q_ARG(bool, show));
}

void GisCanvas::poiLoad(Props::ValueMapsT &prs)
{
    for (auto &it : prs)
    {
        poiLoad(it);
    }
}

void GisCanvas::poiLoad(Props::ValueMapT &pr)
{
    if (pr.count(Props::FLD_KEY_NEW) != 0)
    {
        Props::set(pr, Props::FLD_KEY, Props::get(pr, Props::FLD_KEY_NEW));
        pr.erase(Props::FLD_KEY_NEW);
    }
    QMetaObject::invokeMethod(mMap, "poiUpdate",
         Q_ARG(int, Props::get<int>(pr, Props::FLD_KEY)),
         Q_ARG(bool, Props::get<bool>(pr, Props::FLD_IS_PUBLIC)),
         Q_ARG(QString,
               QString::fromStdString(Props::get(pr, Props::FLD_USERPOI_NAME))),
         Q_ARG(QString,
               QString::fromStdString(
                                 Props::get(pr, Props::FLD_USERPOI_SHORTNAME))),
         Q_ARG(QString,
               QString::fromStdString(
                                  Props::get(pr, Props::FLD_USERPOI_CATEGORY))),
         Q_ARG(QString,
               QString::fromStdString(Props::get(pr, Props::FLD_USERPOI_ADDR))),
         Q_ARG(QString,
               QString::fromStdString(Props::get(pr, Props::FLD_USERPOI_DESC))),
         Q_ARG(QString,
               QString::fromStdString(Props::get(pr, Props::FLD_OWNER))),
         Q_ARG(double, Props::get<double>(pr, Props::FLD_LAT)),
         Q_ARG(double, Props::get<double>(pr, Props::FLD_LON)));
}

void GisCanvas::poiRemove(int id)
{
    QMetaObject::invokeMethod(mMap, "deleteItem",
                              Q_ARG(int, GisQmlInt::TYPEID_POI),
                              Q_ARG(int, id));
}

#ifdef GIS_ROUTING
void GisCanvas::onRouting(double srcLat,
                          double srcLon,
                          double dstLat,
                          double dstLon)
{
    if (checkLayerVisible(GisQmlInt::TYPEID_ROUTE_RESULT))
        QMetaObject::invokeMethod(mMap, "onRouting", Q_ARG(double, srcLat),
                                  Q_ARG(double, srcLon), Q_ARG(double, dstLat),
                                  Q_ARG(double, dstLon));
}

void GisCanvas::onRoutingClear()
{
    if (mValid)
        QMetaObject::invokeMethod(mMap, "deleteItems",
                                  Q_ARG(int, GisQmlInt::TYPEID_ROUTE_RESULT));
}
#endif //GIS_ROUTING

void GisCanvas::onDialogFinished(Props::ValueMapT *pr)
{
    if (pr != 0)
    {
        emit poiSave(pr);
        mTypeId = GisQmlInt::TYPEID_NONE;
    }
    QMetaObject::invokeMethod(mMap, "dismissNewPoi");
}

void GisCanvas::onMapRectChanged(double left,
                                 double top,
                                 double right,
                                 double bottom)
{
    QMetaObject::invokeMethod(mMap, "updateOvBox", Q_ARG(double, left),
                              Q_ARG(double, top), Q_ARG(double, right),
                              Q_ARG(double, bottom));
}

void GisCanvas::onMeasurementCircle(bool enabled)
{
    QMetaObject::invokeMethod(mMap, "measureCircleEnable", Q_ARG(bool, enabled));
}

void GisCanvas::onMeasurementClear()
{
    QMetaObject::invokeMethod(mMap, "deleteItems",
                              Q_ARG(int, GisQmlInt::TYPEID_MEASURE));
}

void GisCanvas::onSrchRadiusChanged(const QString &radius)
{
    mSrchRadius = radius.toInt();
}

void GisCanvas::onMapLayerLoaded()
{
    mValid = true;
    if (mLayerModels.empty())
    {
#ifdef INCIDENT
        mLayerModels[GisQmlInt::KEY_INCIDENT] = GisQmlInt::TYPEID_INCIDENT;
        mLayerModels[GisQmlInt::KEY_INCIDENT_REPORT] =
                                              GisQmlInt::TYPEID_INCIDENT_REPORT;
#endif
        mLayerModels[GisQmlInt::KEY_MEASURE]  = GisQmlInt::TYPEID_MEASURE;
#ifdef GIS_ROUTING
        mLayerModels[GisQmlInt::KEY_ROUTE]    = GisQmlInt::TYPEID_ROUTE_RESULT;
#endif
#ifdef GIS_SEAMAP
        mLayerModels[GisQmlInt::KEY_SEADEPTH] = GisQmlInt::TYPEID_SEADEPTH;
        mLayerModels[GisQmlInt::KEY_SEAMARKS] = GisQmlInt::TYPEID_SEAMARKS;
#endif
        mLayerModels[GisQmlInt::KEY_SEARCH]   = GisQmlInt::TYPEID_SEARCH;
        mLayerModels[GisQmlInt::KEY_TERMINAL] = GisQmlInt::TYPEID_TERMINAL;
        mLayerModels[GisQmlInt::KEY_TRACKING] = GisQmlInt::TYPEID_TRACKING;
        mLayerModels[GisQmlInt::KEY_TRAILING] = GisQmlInt::TYPEID_TRAILING;
        mLayerModels[GisQmlInt::KEY_USERPOI]  = GisQmlInt::TYPEID_POI;
    }
    QMetaObject::invokeMethod(mMap, "timerStart",
                              Q_ARG(int, TIMER_TERMINAL_MIN_MS));
    terminalCheckTimeChanged();
    emit mapLoadComplete();
}

void GisCanvas::onRscSelect(QVariant rscList)
{
    int id;
    int tp;
    QStandardItem *itm;
    auto *mdl = ResourceData::createModel(
                                        (mTypeId == GisQmlInt::TYPEID_RSC_DGNA)?
                                        ResourceData::TYPE_SUBSCRIBER:
                                        ResourceData::TYPE_SUBS_OR_MOBILE);
    for (auto &i : rscList.toList())
    {
        id = i.toInt();
        tp = ResourceData::getType(id);
        if ((tp == ResourceData::TYPE_SUBSCRIBER) ||
            (mTypeId != GisQmlInt::TYPEID_RSC_DGNA &&
             tp == ResourceData::TYPE_MOBILE))
        {
            itm = ResourceData::addItem(mdl, id);
            if (mTypeId == GisQmlInt::TYPEID_RSC_DGNA)
            {
                itm->setCheckable(true);
                itm->setSelectable(false);
                itm->setData(Qt::Checked, Qt::CheckStateRole);
            }
        }
    }
    if (mdl->rowCount() > 0)
    {
        mdl->sort(0);
        if (mTypeId == GisQmlInt::TYPEID_RSC_DGNA)
            emit dgnaAssign(mdl);
#ifdef INCIDENT
        else
            emit resourceAssign(mdl);
#endif
    }
    else
    {
        QMessageBox::critical(this,
                              (mTypeId == GisQmlInt::TYPEID_RSC_DGNA)?
                                  tr("DGNA Assignment Error"):
                                  tr("Resource Assignment Error"),
                              tr("No terminal selected."));
        delete mdl;
    }
}

void GisCanvas::onSearchDone(QVariant key, QVariant resList)
{
    showSearchRes(key.toString(), resList.toStringList());
}

void GisCanvas::onSearchCtrDone(QVariant       key,
                                QVariant       resList,
                                QPointF        ctr,
                                const QString &lbl)
{
#ifdef INCIDENT
    if (mTypeId == GisQmlInt::TYPEID_INCIDENT)
    {
        ResourceData::IdsT ids;
        QStringList res(resList.toStringList());
        int n = res.count();
        if (n > 0)
        {
            QStringList l;
            int id;
            int tp;
            int i = 0;
            for (; i<n; ++i)
            {
                l = res[i].split(';'); //definitely has at least 5 items
                //format: layer;id;lat;lon;name/address[;distance]
                //need only the ISSIs from the results
                id = Utils::fromString<int>(l[1].toStdString());
                tp = ResourceData::getType(id);
                if (tp == ResourceData::TYPE_SUBSCRIBER ||
                    tp == ResourceData::TYPE_MOBILE)
                    ids.insert(id);
            }
        }
        if (ids.empty())
        {
            emit resourceAssign(0);
        }
        else
        {
            auto *mdl = ResourceData::createModel(
                                             ResourceData::TYPE_SUBS_OR_MOBILE);
            ResourceData::addIds(mdl, ids);
            emit resourceAssign(mdl);
        }
        mTypeId = GisQmlInt::TYPEID_NONE;
    }
    else
#endif //INCIDENT
    {
        showSearchRes(key.toString(), resList.toStringList(), &ctr, &lbl);
    }
}

void GisCanvas::onShowItem(Props::ValueMapT  prs,
                           const string     &id,
                           const string     &layer)
{
    int typeId = getModelType(layer);
    QVariant ret;
    QMetaObject::invokeMethod(mMap, "checkModelVisible",
                              Q_RETURN_ARG(QVariant, ret), Q_ARG(int, typeId));
    if (!ret.toBool())
    {
        QString s(QString::fromStdString(id));
        if (typeId == GisQmlInt::TYPEID_POI ||
            typeId == GisQmlInt::TYPEID_SEARCH)
            s = QString::fromStdString(Props::get(prs, Props::FLD_LBL));
        QMessageBox::critical(this, tr("Search Error"),
                              tr("Layer '%1' is not loaded. Unable to show "
                                 "item '%2'.")
                                  .arg(QString::fromStdString(layer), s),
                              QMessageBox::Ok);
        return;
    }
    switch (typeId)
    {
        case GisQmlInt::TYPEID_INCIDENT:
        case GisQmlInt::TYPEID_POI:
        case GisQmlInt::TYPEID_TERMINAL:
            QMetaObject::invokeMethod(mMap, "locateItem",
                                      Q_RETURN_ARG(QVariant, ret),
                                      Q_ARG(int, typeId),
                                      Q_ARG(int, Utils::fromString<int>(id)),
                                      Q_ARG(bool, true));
            if (ret.toInt() < 0)
            {
                QMessageBox::critical(this, tr("Search Error"),
                                  tr("Item '%1' not on map.")
                                      .arg((typeId == GisQmlInt::TYPEID_POI)?
                                           QString::fromStdString(
                                               Props::get(prs, Props::FLD_LBL)):
                                           QString::fromStdString(id)),
                                  QMessageBox::Ok);
                return;
            }
            break;
        case GisQmlInt::TYPEID_SEARCH:
            QMetaObject::invokeMethod(mMap, "placePin",
                Q_ARG(double,
                    Utils::fromString<double>(Props::get(prs, Props::FLD_LAT))),
                Q_ARG(double,
                    Utils::fromString<double>(Props::get(prs, Props::FLD_LON))),
                Q_ARG(QString, Props::get(prs, Props::FLD_LBL).c_str()));
        default:
            break; //do nothing - should not occur
    }
}

void GisCanvas::onContextMenu(const QPointF &pos, QVariant itmList)
{
    if (!mValid)
        return;
    QMenu cmenu(this);
    QLabel *lbl;
    QAction *act;
    QWidgetAction *actTitle;
    QVariant ret;
    if (mTypeId == GisQmlInt::TYPEID_MEASURE)
    {
        //check if there is any measurement point on map
        QMetaObject::invokeMethod(mMap, "hasItem", Q_RETURN_ARG(QVariant, ret),
                                  Q_ARG(int, mTypeId));
        bool hasIdItem = ret.toBool();
        if (hasIdItem)
        {
            actTitle = new QWidgetAction(&cmenu);
            lbl = new QLabel(tr("Measurement:"), this);
            lbl->setStyleSheet(Style::getStyle(Style::OBJ_FRAME_TITLE));
            actTitle->setDefaultWidget(lbl);
            cmenu.addAction(actTitle);
            QMetaObject::invokeMethod(mMap, "measureDone",
                                      Q_RETURN_ARG(QVariant, ret));
            QAction *actRes = (ret.toBool())?
                              cmenu.addAction(tr("Resume from last point")): 0;
            QAction *actRmPoint = cmenu.addAction(tr("Remove last point"));
            QAction *actRmLine = cmenu.addAction(tr("Remove last line"));
            act = cmenu.exec(QCursor::pos());
            if (act == 0 || act == actTitle)
                return;
            if (act == actRes)
                QMetaObject::invokeMethod(mMap, "measureResume");
            else if (act == actRmPoint)
                QMetaObject::invokeMethod(mMap, "measureRmvLastPoint");
            else if (act == actRmLine)
                QMetaObject::invokeMethod(mMap, "measureRmvLastLine");
            return;
        } //else show general context menu
    } //if (mTypeId == TYPEID_MEASURE)

    QPointF ptf(pos);
    //mouse coordinates on top in 2 formats
    lbl = new QLabel(GisPoint::getCoords(ptf).append("\n")
                         .append(GisPoint::getCoords(ptf, false)),
                     this);
    lbl->setStyleSheet(Style::getStyle(Style::OBJ_FRAME_TITLE));
    actTitle = new QWidgetAction(&cmenu);
    actTitle->setDefaultWidget(lbl);
    cmenu.addAction(actTitle);

    QString lblTxt;
    int lyrType = 0;
    QMenu *cmenu2 = 0;
    double val;
    QStringList itms(itmList.toStringList());
    if (!itms.isEmpty())
    {
        int i;
        QStringList l;
        //keep only 10 closest ones
        if (itms.size() > 10)
        {
            LOGGER_DEBUG(mLogger, "GisCanvas::onContextMenu: Found "
                         << itms.size() << " items - keeping the closest 10");
            map<QString, QString> itmMap; //for easy sorting by distance
            for (auto &s : itms)
            {
                //format: layer;id;lat;lon;distance;...
                l = s.split(";");
                if (s.size() > 4)
                    itmMap[l[4]] = s;
            }
            set<QString> itmSet; //for grouping by layer
            i = 10;
            for (auto &it : itmMap)
            {
                itmSet.insert(it.second);
                if (--i == 0)
                    break;
            }
            itms.clear();
            for (auto &s : itmSet)
            {
                itms << s;
            }
        }
        QPointF ptf;
        QString nm;
        string lyr;
        string str;
        for (auto &s : itms)
        {
            l = s.split(";");
            lyr = l[0].toStdString();
            i = l[1].toInt();
            if (lyr == GisQmlInt::KEY_USERPOI)
            {
                //format: layer;id;lat;lon;distance;shortName
                lblTxt = l[5];
                if (!lblTxt.isEmpty())
                {
                    lblTxt.prepend(tr("POI "));
                    cmenu2 = cmenu.addMenu(QIcon(ICON_POI), lblTxt);
                }
            }
#ifdef INCIDENT
            else if (lyr == GisQmlInt::KEY_INCIDENT)
            {
                //format: layer;id;lat;lon;distance;category
                lblTxt = tr("Incident");
                lblTxt.append(" ").append(QString::number(i));
                str = l[5].toStdString();
                cmenu2 = cmenu.addMenu(QIcon(IncidentButton::getPixmap(str)),
                                       lblTxt);
            }
#endif
            else if (lyr == GisQmlInt::KEY_TERMINAL)
            {
                //format: layer;id;lat;lon;distance;timestamp:validTimestamp;
                //        validTime
                nm = ResourceData::getDspTxt(i, ResourceData::TYPE_SUBSCRIBER);
                lblTxt = tr("Resource");
                lblTxt.append(" ").append(nm);
                cmenu2 = cmenu.addMenu(QtUtils::getActionIcon(
                                                CmnTypes::ACTIONTYPE_RESOURCES),
                                       lblTxt);
            }
            ptf.setY(l[2].toDouble());
            ptf.setX(l[3].toDouble());
            lbl = new QLabel(GisPoint::getCoords(ptf).append("\n")
                                 .append(GisPoint::getCoords(ptf, false)),
                             this);
            lbl->setStyleSheet(Style::getStyle(Style::OBJ_FRAME_TITLE));
            actTitle = new QWidgetAction(&cmenu);
            actTitle->setDefaultWidget(lbl);
            cmenu2->addAction(actTitle);
            lyrType = getModelType(l[0].toStdString());
            switch (lyrType)
            {
#ifdef INCIDENT
                case GisQmlInt::TYPEID_INCIDENT:
                {
                    if (mLockedIncidentId == 0)
                    {
                        cmenu2->addAction(QtUtils::getActionIcon(
                                                 CmnTypes::ACTIONTYPE_INCIDENT),
                                          tr("View Incident"))
                              ->setData(QVariant::fromValue(
                                               ActionData(ACTION_VIEW_INC, i)));
                    }
                    break;
                }
#endif //INCIDENT
                case  GisQmlInt::TYPEID_POI:
                {
                    act = cmenu2->addAction(QIcon(ICON_POI), tr("POI Details"));
                    act->setObjectName(QString::fromStdString(lyr));
                    act->setStatusTip(lblTxt); //just for error msg
                    act->setData(QVariant::fromValue(
                                               ActionData(ACTION_VIEW_POI, i)));
                    break;
                }
                case  GisQmlInt::TYPEID_TERMINAL:
                {
                    string s1(l[5].toStdString());
                    string s2(l[6].toStdString());
                    QString qstr;
                    if (!s1.empty() && s1 != s2)
                    {
                        qstr = QString::fromStdString(s1);
                        qstr.append(" ").append(tr("Last Update"));
                        lbl = new QLabel(qstr, this);
                        lbl->setStyleSheet(Style::getStyle(Style::OBJ_LABEL));
                        actTitle = new QWidgetAction(&cmenu);
                        actTitle->setDefaultWidget(lbl);
                        cmenu2->addAction(actTitle);
                    }
                    if (!s2.empty())
                    {
                        qstr = QString::fromStdString(s2);
                        qstr.append(" ").append(tr("Last Valid Update"));
                        lbl = new QLabel(qstr, this);
                        lbl->setStyleSheet(Style::getStyle(Style::OBJ_LABEL));
                        actTitle = new QWidgetAction(&cmenu);
                        actTitle->setDefaultWidget(lbl);
                        cmenu2->addAction(actTitle);
                    }
                    time_t val = time(0) - l[7].toInt();
                    qstr = GisMenuTerminal::getValidityAge(
                            (val > GisQmlInt::TERMINAL_TIME_THRESHOLD)? val: 0);
                    act = cmenu2->addAction(QtUtils::getActionIcon(
                                                CmnTypes::ACTIONTYPE_RESOURCES),
                                            tr("Resource Details"));
                    //need lyr to find the item again later
                    act->setObjectName(QString::fromStdString(lyr));
                    //for error msg in case item no longer available later
                    act->setStatusTip(lblTxt);
                    act->setData(QVariant::fromValue(
                                               ActionData(ACTION_VIEW_RSC, i)));
                    auto *cmenu3 = cmenu2->addMenu(
                                            QtUtils::getActionIcon(
                                                CmnTypes::ACTIONTYPE_RESOURCES),
                                            tr("Interact"));
                    act = cmenu3->addAction(QtUtils::getActionIcon(
                                                     CmnTypes::ACTIONTYPE_CALL),
                                            tr("Call"));
                    act->setObjectName(nm);
                    act->setData(QVariant::fromValue(
                                               ActionData(ACTION_RSC_CALL, i)));
                    act = cmenu3->addAction(QtUtils::getActionIcon(
                                                      CmnTypes::ACTIONTYPE_SDS),
                                            tr("SDS"));
                    act->setObjectName(nm);
                    act->setData(QVariant::fromValue(
                                                ActionData(ACTION_RSC_SDS, i)));
                    act = cmenu3->addAction(QtUtils::getActionIcon(
                                                   CmnTypes::ACTIONTYPE_STATUS),
                                            tr("Status Message"));
                    act->setObjectName(nm);
                    act->setData(QVariant::fromValue(
                                               ActionData(ACTION_RSC_STAT, i)));
                    act = cmenu3->addAction(QtUtils::getActionIcon(
                                                      CmnTypes::ACTIONTYPE_MON),
                                            tr("Monitor"));
                    act->setObjectName(nm);
                    act->setData(QVariant::fromValue(
                                                ActionData(ACTION_RSC_MON, i)));
                    QMetaObject::invokeMethod(mMap, "isTrailing",
                                              Q_RETURN_ARG(QVariant, ret),
                                              Q_ARG(int, i));
                    if (ret.toBool())
                    {
                        cmenu3->addAction(QIcon(ICON_TRAIL),
                                          tr("Disable Trailing"))
                              ->setData(QVariant::fromValue(
                                            ActionData(ACTION_RSC_TRL_OFF, i)));
                    }
                    else
                    {
                        cmenu3->addAction(QIcon(ICON_TRAIL),
                                          tr("Enable Trailing"))
                              ->setData(QVariant::fromValue(
                                             ActionData(ACTION_RSC_TRL_ON, i)));
                    }
                    break;
                }
                default:
                {
                    break; //do nothing - should not occur
                }
            } //switch (lyrType)
            act = cmenu2->addAction(QIcon(ICON_SRCH), tr("Search Nearby"));
            act->setObjectName(lblTxt);
            act->setData(QVariant::fromValue(ActionData(ACTION_SRCH, ptf)));
            act = cmenu2->addAction(QtUtils::getActionIcon(
                                                CmnTypes::ACTIONTYPE_RESOURCES),
                                    tr("Resources Nearby"));
            act->setObjectName(lblTxt);
            act->setData(QVariant::fromValue(ActionData(ACTION_RSC, ptf)));
            cmenu2->addAction(QIcon(ICON_ZOOM), tr("Zoom to Here"))
                  ->setData(QVariant::fromValue(ActionData(ACTION_ZOOM, ptf)));
            cmenu2->addAction(QtUtils::getActionIcon(CmnTypes::ACTIONTYPE_COPY),
                              tr("Copy Coordinates"))
                  ->setData(QVariant::fromValue(ActionData(ACTION_COORDS, ptf)));
            cmenu2->addAction(QtUtils::getActionIcon(CmnTypes::ACTIONTYPE_COPY),
                              tr("Copy Coordinates DMS"))
                  ->setData(QVariant::fromValue(
                                           ActionData(ACTION_COORDS_DMS, ptf)));
        }
    } //if (!itms.isEmpty())

    //general menu
#ifdef INCIDENT
    if (mLockedIncidentId != 0)
        lblTxt = tr("Update Incident %1 Location").arg(mLockedIncidentId);
    else
        lblTxt = tr("New Incident Point");
    cmenu.addAction(QtUtils::getActionIcon(CmnTypes::ACTIONTYPE_INCIDENT),
                    lblTxt)
        ->setData(QVariant::fromValue(ActionData(ACTION_NEW_INC)));
#endif //INCIDENT
    cmenu.addAction(QIcon(ICON_POI), tr("New POI"))
        ->setData(QVariant::fromValue(ActionData(ACTION_NEW_POI)));
    cmenu.addAction(QIcon(ICON_SRCH), tr("Search Nearby"))
        ->setData(QVariant::fromValue(ActionData(ACTION_SRCH)));
    cmenu.addAction(QtUtils::getActionIcon(CmnTypes::ACTIONTYPE_RESOURCES),
                    tr("Resources Nearby"))
        ->setData(QVariant::fromValue(ActionData(ACTION_RSC)));
    cmenu.addAction(QIcon(ICON_ZOOM), tr("Zoom to Here"))
        ->setData(QVariant::fromValue(ActionData(ACTION_ZOOM)));
    cmenu.addAction(QtUtils::getActionIcon(CmnTypes::ACTIONTYPE_COPY),
                    tr("Copy Coordinates"))
        ->setData(QVariant::fromValue(ActionData(ACTION_COORDS)));
    cmenu.addAction(QtUtils::getActionIcon(CmnTypes::ACTIONTYPE_COPY),
                    tr("Copy Coordinates DMS"))
        ->setData(QVariant::fromValue(ActionData(ACTION_COORDS_DMS)));

    //show menu and perform selected action
    act = cmenu.exec(QCursor::pos());
    if (act == 0)
        return;
    ActionData d(act->data().value<ActionData>());
    switch (d.type)
    {
        case ACTION_COORDS:
        case ACTION_COORDS_DMS:
        {
            if (d.lat < COORD_INVALID)
            {
                ptf.setY(d.lat);
                ptf.setX(d.lon);
            }
            QApplication::clipboard()->setText(
                           GisPoint::getCoords(ptf, (d.type == ACTION_COORDS)));
            break;
        }
        case ACTION_NEW_INC:
        {
            createItem(GisQmlInt::TYPEID_INCIDENT, ptf.y(), ptf.x());
            break;
        }
        case ACTION_NEW_POI:
        {
            if (createItem(GisQmlInt::TYPEID_POI, ptf.y(), ptf.x()))
            {
                //for GisCanvas::onDialogFinished()
                mTypeId = GisQmlInt::TYPEID_POI;
                QMetaObject::invokeMethod(mMap, "onNewPoi",
                                          Q_ARG(double, ptf.y()),
                                          Q_ARG(double, ptf.x()));
                showDialog();
            }
            break;
        }
        case ACTION_RSC:
        {
            if (d.lat < COORD_INVALID)
            {
                ptf.setY(d.lat);
                ptf.setX(d.lon);
            }
            search("", &ptf, act->objectName());
            break;
        }
        case ACTION_SRCH:
        {
            if (d.lat < COORD_INVALID)
            {
                ptf.setY(d.lat);
                ptf.setX(d.lon);
            }
            emit searchNearby(ptf.y(), ptf.x(), act->objectName());
            break;
        }
        case ACTION_RSC_CALL:
        case ACTION_RSC_MON:
        case ACTION_RSC_SDS:
        case ACTION_RSC_STAT:
        {
            //type values equal the corresponding CmnTypes::ACTIONTYPE_*
            emit startAction(d.type, ResourceData::getType(d.val),
                             ResourceData::IdsT({d.val}));
            break;
        }
        case ACTION_RSC_TRL_OFF:
        {
            terminalTrailing(d.val, false);
            break;
        }
        case ACTION_RSC_TRL_ON:
        {
            terminalTrailing(d.val, true);
            break;
        }
#ifdef INCIDENT
        case ACTION_VIEW_INC:
        {
            emit showIncident(d.val);
            break;
        }
#endif
        case ACTION_VIEW_POI:
        {
            //item may have been deleted
            QMetaObject::invokeMethod(mMap, "getItemDetails",
                                      Q_RETURN_ARG(QVariant, ret),
                                      Q_ARG(int, GisQmlInt::TYPEID_POI),
                                      Q_ARG(int, d.val));
            auto l(ret.toString().split(';'));
            if (!l.isEmpty())
            {
                //format: lgName;shName;cat;addr;desc;;owner;isPublic;lat;lon
                mProps.clear();
                Props::set(mProps, Props::FLD_LAYERNAME, GisQmlInt::KEY_USERPOI);
                Props::set(mProps, Props::FLD_KEY, d.val);
                Props::set(mProps, Props::FLD_USERPOI_NAME, l[0].toStdString());
                Props::set(mProps, Props::FLD_USERPOI_SHORTNAME,
                           l[1].toStdString());
                Props::set(mProps, Props::FLD_USERPOI_CATEGORY,
                           l[2].toStdString());
                Props::set(mProps, Props::FLD_USERPOI_ADDR, l[3].toStdString());
                Props::set(mProps, Props::FLD_USERPOI_DESC, l[4].toStdString());
                Props::set(mProps, Props::FLD_OWNER, l[5].toStdString());
                Props::set(mProps, Props::FLD_IS_PUBLIC, !l[6].isEmpty());
                Props::set(mProps, Props::FLD_LAT, l[7].toStdString());
                Props::set(mProps, Props::FLD_LON, l[8].toStdString());
                showDialog();
            }
            else
            {
                QMessageBox::critical(this, act->text(),
                                      tr("%1 is no longer on the map.")
                                          .arg(act->statusTip()));
            }
            break;
        }
        case ACTION_VIEW_RSC:
        {
            //item may have been deleted
            QMetaObject::invokeMethod(mMap, "getItemDetails",
                                      Q_RETURN_ARG(QVariant, ret),
                                      Q_ARG(int, GisQmlInt::TYPEID_TERMINAL),
                                      Q_ARG(int, d.val));
            auto l(ret.toString().split(';'));
            if (!l.isEmpty())
            {
                //format: id;lat;lon;timestamp;validTimestamp
                mProps.clear();
                Props::set(mProps, Props::FLD_LAYERNAME,
                           GisQmlInt::KEY_TERMINAL);
                Props::set(mProps, Props::FLD_KEY, d.val);
                Props::set(mProps, Props::FLD_LAT, l[1].toStdString());
                Props::set(mProps, Props::FLD_LON, l[2].toStdString());
                Props::set(mProps, Props::FLD_LOC_TIMESTAMP,
                           l[3].toStdString());
                Props::set(mProps, Props::FLD_LOC_TIMESTAMP_VALID,
                           l[4].toStdString());
                showDialog();
            }
            else
            {
                QMessageBox::critical(this, act->text(),
                                      tr("%1 is no longer on the map.")
                                          .arg(act->statusTip()));
            }
            break;
        }
        case ACTION_ZOOM:
        {
            if (d.lat < COORD_INVALID)
            {
                ptf.setY(d.lat);
                ptf.setX(d.lon);
            }
            val = getZoomLevel();
            setGeomCenter(ptf,
                          (val < GisQmlInt::ZOOM_SEARCH)?
                              GisQmlInt::ZOOM_SEARCH: val);
            break;
        }
        default:
        {
            break; //do nothing - should not occur
        }
    } //switch (d.type)
}

void GisCanvas::showSearchRes(const QString &key,
                              QStringList    res,
                              QPointF       *ctr,
                              const QString *lbl)
{
    QString c; //center coordinates
    if (ctr != 0)
        c = GisPoint::getCoords(*ctr);
    int n = res.count();
    if (n == 0)
    {
        if (ctr == 0)
        {
            QMessageBox::information(this, tr("Search Results"),
                                     tr("No result for: %1").arg(key),
                                     QMessageBox::Ok);
        }
        else
        {
            if (!lbl->isEmpty())
                c.prepend(" (").prepend(*lbl).append(")");
            if (key.isEmpty())
                QMessageBox::information(this, tr("Search Results"),
                                         tr("No resource near %1").arg(c),
                                         QMessageBox::Ok);
            else
                QMessageBox::information(this, tr("Search Results"),
                                         tr("No result near %1 for: %2")
                                             .arg(c, key),
                                         QMessageBox::Ok);
        }
        return;
    }
    if (n == 1)
    {
        QStringList l(res[0].split(';')); //definitely has at least 5 items
        //format: layer;id;lat;lon;name/address[;distance]
        Props::ValueMapT prs;
        Props::set(prs, Props::FLD_LAT, l[2].toDouble());
        Props::set(prs, Props::FLD_LON, l[3].toDouble());
        //address has comma-separated parts - take first part only
        Props::set(prs, Props::FLD_LBL,
                   l[4].left(l[4].indexOf(',')).toStdString());
        onShowItem(prs, l[1].toStdString(), l[0].toStdString());
        return;
    }
    auto *mdl = new QStandardItemModel(n, InputDialog::COL_NUM);
    mdl->setHeaderData(InputDialog::COL_NAME, Qt::Horizontal, tr("Name"));
    if (ctr != 0)
        mdl->setHeaderData(InputDialog::COL_DISTANCE, Qt::Horizontal,
                           tr("Distance") + " (km)");
    mdl->setHeaderData(InputDialog::COL_LAYER, Qt::Horizontal, tr("Layer"));
    QStringList l;
    int i = 0;
    for (; i<n; ++i)
    {
        l = res[i].split(';');
        //format: layer;id;lat;lon;name/address[;distance]
        mdl->setData(mdl->index(i, InputDialog::COL_LAYER), l[0]);
        mdl->setData(mdl->index(i, InputDialog::COL_ID), l[1]);
        mdl->setData(mdl->index(i, InputDialog::COL_LAT), l[2]);
        mdl->setData(mdl->index(i, InputDialog::COL_LON), l[3]);
        mdl->setData(mdl->index(i, InputDialog::COL_NAME), l[4]);
        if (l.size() > 5)
            mdl->setData(mdl->index(i, InputDialog::COL_DISTANCE), l[5]);
    }
    auto *d = new InputDialog(key, mdl, c, (lbl == 0)? "": *lbl, "", this);
    connect(d, SIGNAL(showItem(Props::ValueMapT,string,string)),
            SLOT(onShowItem(Props::ValueMapT,string,string)));
    d->show();
}

bool GisCanvas::checkLayerVisible(int layer)
{
    assert(layer != 0);
    QVariant ret;
    QMetaObject::invokeMethod(mMap, "checkModelVisible",
                              Q_RETURN_ARG(QVariant, ret), Q_ARG(int, layer));
    if (!ret.toBool())
    {
        QString layerName(GisQmlInt::getModelName(layer));
        QMessageBox::critical(this, tr("Hidden Layer"),
                              tr("Please make %1 layer visible before doing "
                                 "this operation.").arg(layerName),
                              QMessageBox::Ok);
        return false;
    }
    return true;
}

bool GisCanvas::createItem(int typeId, double lat, double lon)
{
    if (!checkLayerVisible(typeId))
        return false;
    mProps.clear();
    Props::set(mProps, Props::FLD_KEY, typeId);
    Props::set(mProps, Props::FLD_LAT, GisPoint::getCoord(lat));
    Props::set(mProps, Props::FLD_LON, GisPoint::getCoord(lon));
    Props::set(mProps, Props::FLD_OWNER, mUserName.toStdString());
    switch (typeId)
    {
#ifdef INCIDENT
        case GisQmlInt::TYPEID_INCIDENT:
            Props::set(mProps, Props::FLD_LAYERNAME, GisQmlInt::KEY_INCIDENT);
            emit pointCoordinates(typeId, lon, lat);
            break;
#endif
        case GisQmlInt::TYPEID_POI:
            Props::set(mProps, Props::FLD_LAYERNAME, GisQmlInt::KEY_USERPOI);
            break;
        default:
            break; //do nothing
    }
    return true;
}

void GisCanvas::showDialog()
{
    string layerName(Props::get(mProps, Props::FLD_LAYERNAME));
    if (layerName == GisQmlInt::KEY_USERPOI)
    {
        auto *poi = new GisMenuUserPoi(mProps, mUserName, parentWidget());
        /**
         * @todo Create frameless dialog.
         *       Rosnin 21/11/2023.
         */
        poi->show();
        if (poi->isOwner())
            connect(poi, SIGNAL(poiDelete(int)), SIGNAL(poiDelete(int)));
        connect(poi, SIGNAL(dismissed(Props::ValueMapT*)),
                SLOT(onDialogFinished(Props::ValueMapT*)));
    }
    else if (layerName == GisQmlInt::KEY_TERMINAL)
    {
        auto *terminal = new GisMenuTerminal(mProps, parentWidget());
        terminal->show();
    }
}
