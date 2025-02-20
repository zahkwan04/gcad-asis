/**
 * The Map UI implementation.
 *
 * Copyright (C) Sapura Secured Technologies, 2014-2025. All Rights Reserved.
 *
 * @file
 * @version $Id: GisWindow.cpp 1898 2025-01-17 00:50:10Z rosnin $
 * @author Muhd Hashim Wahab
 */
#include <QAction>
#include <QActionGroup>
#include <QCheckBox>
#include <QDesktopServices>
#include <QFileDialog>
#include <QGroupBox>
#include <QIcon>
#include <QImageWriter>
#include <QMenu>
#include <QMessageBox>
#include <QSpinBox>
#include <QTimer>
#include <QWidgetAction>
#include <assert.h>

#include "DbInt.h"
#include "GisMenuUserPoi.h"
#include "GisPoint.h"
#include "GisQmlInt.h"
#include "Props.h"
#include "QtUtils.h"
#include "RscCollector.h"
#include "Settings.h"
#include "Style.h"
#include "SubsData.h"
#include "ui_GisWindow.h"
#include "GisWindow.h"

using namespace std;

static const double  ZOOM_FACTOR = 2.0;
static const char    PROP_SEARCH_LABEL[]("SrchLbl");
static const QString ICON_SEARCH(":/Images/images/icon_search.png");
static const QString ICON_SEARCH_NEARBY(":/Images/images/"
                                        "icon_search_nearby.png");
static const QString WIDGET_NAME("qwMap");

int                 GisWindow::sMeasureUnit = GisQmlInt::MEASURE_UNIT_KM;
int                 GisWindow::sTheme = -1;
QString             GisWindow::sMapVersion;
GisWindow::UnitMapT GisWindow::sUnitMap;

//add floatable widget to side panel, and handle docking by re-inserting at its
//original position and destroying the floating container dialog
#define ADDWIDGET(wdg, className) \
    do \
    { \
        mWdgIdxMap[wdg] = idx++; \
        ui->splitter1->addWidget(wdg); \
        connect(wdg, &className::dock, this, \
                [this](QDialog *dlg) \
                { \
                    /* find widget with higher idx, and take its place */ \
                    int idx = mWdgIdxMap[wdg]; \
                    int n = ui->splitter1->count(); \
                    int i = 1; \
                    for (; i<n; ++i) \
                    { \
                        if (mWdgIdxMap[ui->splitter1->widget(i)] > idx) \
                            break;\
                    } \
                    ui->splitter1->insertWidget(i, wdg); \
                    delete dlg; \
                }); \
    } \
    while (0)

GisWindow::GisWindow(Poi           *poi,
                     Resources     *resources,
                     Dgna          *dgna,
                     Logger        *logger,
                     const QString &userName,
                     QWidget       *parent) :
QMainWindow(parent), ui(new Ui::GisWindow), mLogger(logger), mPoi(poi),
mMapCanvas(0), mBookmarks(0), mDgnaList(0), mLayers(0), mLegend(0),
mOverviewMap(0), mRoute(0), mTracking(0), mSearchPt(0), mUserName(userName)
{
    if (logger == 0)
    {
        assert("Bad param in GisWindow::GisWindow" == 0);
        return;
    }
    ui->setupUi(this);
    //initialize sUnitMap for mapping of measurement units to names,
    //to be done at runtime due to text content that is subject to translation
    sUnitMap[GisQmlInt::MEASURE_UNIT_KM]            = "Km";
    sUnitMap[GisQmlInt::MEASURE_UNIT_MILE]          = QObject::tr("Mile");
    sUnitMap[GisQmlInt::MEASURE_UNIT_NAUTICAL_MILE] = QObject::tr(
                                                               "Nautical Mile");
    sUnitMap[GisQmlInt::MEASURE_UNIT_LAST]          = "";
    const QString &ss(Style::getStyle(Style::OBJ_LABEL_WHITE));
    ui->versionLbl->setStyleSheet(ss);
    ui->scrollArea->setStyleSheet("QScrollBar:vertical{max-width: 20px;}");
    ui->searchCb->setMaxCount(20); //search history limit
    ui->backTlBtn->setEnabled(false);
    ui->fwdTlBtn->setEnabled(false);
    ui->searchCb->lineEdit()->setPlaceholderText(ui->searchTlBtn->toolTip());
    Props::clearStrs();
    auto *qw = new QQuickWidget(ui->centralwidget);
    mMapCanvas = new GisCanvas(mLogger, mUserName, false, qw, this);
    mMapCanvas->hide();
    qw->setObjectName(WIDGET_NAME); //set for saveTlBtn clicked()
    ui->mapCanvLo->addWidget(qw);
    //shift the zoom range to start from 0
    ui->zoomSlider->setMinimum(0);
    ui->zoomSlider->setMaximum((GisQmlInt::ZOOM_MAX - GisQmlInt::ZOOM_MIN) *
                               ZOOM_FACTOR);
    mHistory = new GisHistory(mMapCanvas);
    mOverviewMap = new GisMapOverview(mLogger, this);
    ui->splitter1->addWidget(mOverviewMap);
    int idx = 1;
    mLayers = new GisLayerList(mLogger, mMapCanvas, this);
    ADDWIDGET(mLayers, GisLayerList);
    mLegend = new GisLegend(mLogger, mMapCanvas, this);
    ADDWIDGET(mLegend, GisLegend);
    mBookmarks = new GisBookmarks(mMapCanvas, mUserName, this);
    ADDWIDGET(mBookmarks, GisBookmarks);
    mDgnaList = new GisDgnaList(resources, dgna, mMapCanvas, this);
    ADDWIDGET(mDgnaList, GisDgnaList);
    mTracking = new GisTracking(mMapCanvas, this);
    ADDWIDGET(mTracking, GisTracking);
#ifdef GIS_ROUTING
    mRoute = new GisRouting(mMapCanvas, this);
    ADDWIDGET(mRoute, GisRouting);
    connect(this, SIGNAL(pointCoordinates(int,double,double)),
            mRoute, SLOT(onCoordReceived(int,double,double)));
    connect(mRoute, SIGNAL(getRouting(double,double,double,double)),
            mMapCanvas, SLOT(onRouting(double,double,double,double)));
    connect(mRoute, SIGNAL(clearResult()), mMapCanvas, SLOT(onRoutingClear()));
#endif //GIS_ROUTING
    connect(ui->reloadTlBtn, &QToolButton::clicked, this,
            [this] { reloadMap(); });
    ui->reloadTlBtn->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->reloadTlBtn, &QToolButton::customContextMenuRequested, this,
            [this](const QPoint &pos)
            {
                if (QApplication::keyboardModifiers() != Qt::ControlModifier)
                    return; //accept only Ctrl-right-click
                QMenu menu(this);
                menu.addAction(QtUtils::getActionIcon(
                                                   CmnTypes::ACTIONTYPE_LOCATE),
                               tr("Open map cache folder"));
                if (menu.exec(ui->reloadTlBtn->mapToGlobal(pos)) == 0)
                    return;
                //folder name is like: <cache>\QtLocation\5.8\tiles\osm
                QString d(QStandardPaths::writableLocation(
                                         QStandardPaths::GenericCacheLocation) +
                          "/QtLocation");
                if (!QFileInfo(d).isDir())
                    return;
                QDirIterator it(d, QDirIterator::Subdirectories);
                while (it.hasNext())
                {
                    d = it.next() + "/tiles/osm";
                    if (QFileInfo(d).isDir())
                    {
                        auto url(QUrl::fromLocalFile(d));
                        if (url.isValid())
                            QDesktopServices::openUrl(url);
                        break;
                    }
                }
            });
    connect(ui->sideTlBtn, &QToolButton::clicked, this,
            [this]
            {
                if (ui->sideTlBtn->isChecked())
                {
                    ui->sideFrame->hide();
                    ui->sideTlBtn->setToolTip(tr("Show side panel"));
                }
                else
                {
                    ui->sideFrame->show();
                    ui->sideTlBtn->setToolTip(tr("Hide side panel"));
                }
            });
    connect(ui->selectTlBtn, &QToolButton::clicked, this,
            [this] { resetToMode(GisQmlInt::MODE_SELECT); });
    connect(ui->zoomTlBtn, &QToolButton::clicked, this,
            [this] { resetToMode(GisQmlInt::MODE_ZOOM); });
    connect(ui->measureTlBtn, &QToolButton::clicked, this,
            [this]
            {
                resetToMode(GisQmlInt::MODE_MEASURE, GisQmlInt::TYPEID_MEASURE);
            });
    connect(ui->zoomSlider, &QSlider::valueChanged, this,
            [this](int val)
            {
#ifdef DEBUG
                ui->zoomSlider
                  ->setToolTip(tr("Zoom Level %1").arg(val).append("  :")
                                   .append(QString::number(
                                     (val/ZOOM_FACTOR) + GisQmlInt::ZOOM_MIN)));
#else
                ui->zoomSlider->setToolTip(tr("Zoom Level %1").arg(val));
#endif
                mMapCanvas->setZoomLevel((val/ZOOM_FACTOR) +
                                         GisQmlInt::ZOOM_MIN);
                ui->zoomOutTlBtn->setEnabled(val != ui->zoomSlider->minimum());
                ui->zoomInTlBtn->setEnabled(val != ui->zoomSlider->maximum());
                setScale();
            });
    connect(ui->zoomOutTlBtn, &QToolButton::clicked, this,
            [this] { ui->zoomSlider->setValue(ui->zoomSlider->value() - 1); });
    connect(ui->zoomInTlBtn, &QToolButton::clicked, this,
            [this] { ui->zoomSlider->setValue(ui->zoomSlider->value() + 1); });
    connect(ui->backTlBtn, &QToolButton::clicked, this,
            [this]
            {
                ui->backTlBtn->setEnabled(mHistory->prev());
                ui->fwdTlBtn->setEnabled(true);
            });
    connect(ui->fwdTlBtn, &QToolButton::clicked, this,
            [this]
            {
                ui->fwdTlBtn->setEnabled(mHistory->next());
                ui->backTlBtn->setEnabled(true);
            });
    connect(ui->saveTlBtn, &QToolButton::clicked, this,
            [this]
            {
                int i = ui->mapCanvLo->count() - 1;
                for (; i>=0; --i)
                {
                    auto *w = ui->mapCanvLo->itemAt(i)->widget();
                    if (w != 0 && w->objectName().compare(WIDGET_NAME) == 0)
                    {
                        QList<QByteArray> list(
                                         QImageWriter::supportedImageFormats());
                        QString filter;
                        if (list.contains("jpg"))
                            filter.append("JPEG (*.jpg *.jpeg)");
                        if (list.contains("png"))
                            filter.append(";;PNG (*.png)");
                        if (list.contains("bmp"))
                            filter.append(";;Bitmap (*.bmp)");
                        if (list.contains("tif"))
                            filter.append(";;TIFF (*.tif *.tiff)");
                        QString fn(QFileDialog::getSaveFileName(this,
                                                 tr("Save Image"), "", filter));
                        if (!fn.isEmpty() && !w->grab().save(fn))
                            QMessageBox::critical(this, tr("Image Save Error"),
                                       tr("Failed to save image to %1").arg(fn),
                                       QMessageBox::Ok);
                        break;
                    }
                }
            });
    connect(ui->searchTlBtn, &QToolButton::clicked, this,
            [this]
            {
                if (!mMapCanvas->isValid())
                    return;
                QString key(ui->searchCb->currentText().trimmed());
                if (key.size() <= 1)
                {
                    if (!key.isEmpty())
                        QMessageBox::critical(this, tr("Search Error"),
                                              tr("Search text too short."));
                    return;
                }
                //disable temporarily to prevent multiple clicks
                ui->searchTlBtn->setEnabled(false);
                int i = ui->searchCb->findText(key);
                if (i != 0)
                {
                    //key not at the top - either move or add it there
                    ui->searchCb->insertItem(0, key);
                    ui->searchCb->setCurrentIndex(0);
                    if (i > 0)
                        ui->searchCb->removeItem(i + 1);
                }
                mMapCanvas->search(key, mSearchPt,
                                   ui->searchTlBtn
                                     ->property(PROP_SEARCH_LABEL).toString());
                //allow time for results to come out
                QTimer::singleShot(1000, this,
                                 [this] { ui->searchTlBtn->setEnabled(true); });
            });
    connect(ui->searchCb->lineEdit(), &QLineEdit::returnPressed, this,
            [this] { ui->searchTlBtn->click(); });
    connect(ui->clearTlBtn, &QToolButton::clicked, this,
            [this]
            {
                if (mMapCanvas->isValid())
                {
                    static const QIcon icon(ICON_SEARCH);
                    ui->searchCb->clearEditText();
                    mMapCanvas->typeCancelled(GisQmlInt::TYPEID_SEARCH);
                    ui->searchTlBtn->setIcon(icon);
                    ui->searchTlBtn->setProperty(PROP_SEARCH_LABEL, "");
                    ui->searchTlBtn->setToolTip(tr("Search"));
                    ui->searchCb->lineEdit()
                      ->setPlaceholderText(ui->searchTlBtn->toolTip());
                    delete mSearchPt;
                    mSearchPt = 0;
                }
            });
    //gps monitoring
    auto *menu = new QMenu();
    ui->filterBtn->setMenu(menu);
    connect(ui->filterBtn, &QToolButton::clicked, this,
            [this] { ui->filterBtn->showMenu(); });
    connect(QtUtils::addMenuAction(*menu, CmnTypes::ACTIONTYPE_MON,
                                   tr("GPS Monitoring")),
            &QAction::triggered, this, [this] { emit gpsMon(); });
    QCheckBox *chk;
    QWidgetAction *wAct;
    DbInt::Int2StringMapT tt;
    if (DbInt::instance().getTerminalTypes(tt) && tt.size() > 1)
    {
        //terminal types filter - submenu of checkboxes
        auto *subMenu = menu->addMenu(
                                   QIcon(":/Images/images/icon_select_all.png"),
                                   tr("Terminal Types"));
        auto *chkAll = new QCheckBox(tr("All"), subMenu);
        chkAll->setChecked(true); //start with all checked
        auto *vbox = new QVBoxLayout();
        vbox->addWidget(chkAll);
        for (auto &it : tt)
        {
            chk = new QCheckBox(QString::fromStdString(it.second), subMenu);
            chk->setProperty("v", it.first); //value SubsData::eTerminalType
            chk->setChecked(true);
            connect(chk, &QCheckBox::toggled, this,
                    [chk, this](bool checked)
                    {
                        mMapCanvas->terminalsShow(checked,
                                                  chk->property("v").toInt());
                    });
            //on chk click, check chkAll if all boxes are checked, and uncheck
            //it if any is unchecked
            connect(chk, &QCheckBox::clicked, this,
                    [chkAll, subMenu](bool checked)
                    {
                        if (checked)
                        {
                            for (auto *&c : subMenu->findChildren<QCheckBox *>())
                            {
                                if (c != chkAll && !c->isChecked())
                                {
                                    checked = false;
                                    break;
                                }
                            }
                        }
                        chkAll->setChecked(checked);
                    });
            //clicking chkAll goes to all checkboxes
            connect(chkAll, SIGNAL(clicked(bool)), chk, SLOT(setChecked(bool)));
            vbox->addWidget(chk);
        }
        auto *gbox = new QGroupBox();
        gbox->setLayout(vbox);
        wAct = new QWidgetAction(subMenu);
        wAct->setDefaultWidget(gbox);
        subMenu->addAction(wAct);
    }
    RscCollector::init(mLogger);
    connect(QtUtils::addMenuAction(*menu, CmnTypes::ACTIONTYPE_GRP_ATT,
                                   tr("Select Groups")),
            &QAction::triggered, this, [this]
            {
                connect(new RscCollector(ResourceData::TYPE_GROUP_OR_DGNA,
                                         &mFilterGssis, this),
                        &RscCollector::selChanged, this,
                        [this]
                        {
                            if (ui->filterBtn->isCheckable())
                                terminalsFilter(!mFilterGssis.empty());
                        });
            });
    chk = new QCheckBox(tr("Filter By Groups"), menu);
    wAct = new QWidgetAction(menu);
    wAct->setDefaultWidget(chk);
    menu->addAction(wAct);
    connect(chk, &QCheckBox::toggled, this,
            [this](bool checked)
            {
                ui->filterBtn->setCheckable(checked);
                if (!mFilterGssis.empty())
                    terminalsFilter(checked);
            });
#ifdef DEBUG
    connect(mMapCanvas, &GisCanvas::mapCoordinates, this,
            [this](double x, double y, double scX, double scY)
            {
                ui->coorLbl->setText(GisPoint::getCoords(y, x) + " / " +
                                     GisPoint::getCoordsDms(y, x) + " / " +
                                     GisPoint::getCoords(scY, scX, 0));
            });
#else
    connect(mMapCanvas, &GisCanvas::mapCoordinates, this,
            [this](double x, double y)
            {
                ui->coorLbl->setText(GisPoint::getCoords(y, x) + " / " +
                                     GisPoint::getCoordsDms(y, x));
            });
#endif
    connect(mMapCanvas, SIGNAL(pointCoordinates(int,double,double)),
            SLOT(onCoordReceived(int,double,double)));
    connect(mMapCanvas, SIGNAL(zoomChanged(double)),
            SLOT(updateZoomLevel(double)));
    connect(mMapCanvas, &GisCanvas::viewChanged, this,
            [this](QPointF ctr, double zoom)
            {
                if (!mHistory->checkNavigation())
                {
                    ui->fwdTlBtn->setEnabled(false);
                    ui->backTlBtn->setEnabled(mHistory->save(ctr, zoom));
                }
            });
    connect(mMapCanvas, SIGNAL(mapLoadComplete()), SLOT(onMainMapLoaded()));
    connect(mMapCanvas, &GisCanvas::modeSet, this,
            [this] { setButtonStyles(mMapCanvas->getMode()); });
    connect(mMapCanvas, SIGNAL(searchNearby(double,double,QString)),
            SLOT(onSearchNearby(double,double,QString)));
    connect(mMapCanvas, &GisCanvas::poiSave, this,
            [this](Props::ValueMapT *pr)
            {
                assert(pr != 0);
                if (mPoi->saveData(*pr))
                    mMapCanvas->poiLoad(*pr);
                else
                    QMessageBox::critical(this, tr("POI Error"),
                                          tr("Failed to save '%1' to database "
                                             "because of link error.")
                                              .arg(QString::fromStdString(
                                                   Props::get(*pr,
                                                    Props::FLD_USERPOI_NAME))));
                delete pr; //this is the end consumer
            });
    connect(mMapCanvas, &GisCanvas::poiDelete, this,
            [this](int id)
            {
                if (mPoi->deleteData(id))
                    mMapCanvas->poiRemove(id);
                else
                    QMessageBox::critical(this, tr("POI Error"),
                                          tr("Failed to delete POI from "
                                             "database because of link error."));
            });
#ifdef INCIDENT
    connect(mMapCanvas, SIGNAL(showIncident(int)), SIGNAL(showIncident(int)));
    connect(mMapCanvas, SIGNAL(resourceAssign(ResourceData::ListModel*)),
            SIGNAL(resourceAssign(ResourceData::ListModel*)));
#endif
    connect(mMapCanvas, SIGNAL(startAction(int,int,ResourceData::IdsT)),
            SIGNAL(startAction(int,int,ResourceData::IdsT)));
    connect(mMapCanvas, SIGNAL(mapRectChanged(double,double,double,double)),
            mOverviewMap->getCanvas(),
            SLOT(onMapRectChanged(double,double,double,double)));
    connect(mMapCanvas, SIGNAL(dgnaAssign(ResourceData::ListModel*)),
            mDgnaList, SLOT(onDgnaAssign(ResourceData::ListModel*)));
    //add menu to measurement button
    ui->measureTlBtn->setPopupMode(QToolButton::MenuButtonPopup);
    auto *menu1 = new QMenu(this);
    auto *act = new QAction(QtUtils::getActionIcon(CmnTypes::ACTIONTYPE_CLEAR),
                            tr("Clear"), this);
    menu1->addAction(act);
    connect(act, SIGNAL(triggered()), mMapCanvas, SLOT(onMeasurementClear()));
    act = new QAction(tr("Show circle"), this);
    menu1->addAction(act);
    act->setCheckable(true);
    act->setChecked(true);
    connect(act, SIGNAL(triggered(bool)),
            mMapCanvas, SLOT(onMeasurementCircle(bool)));
    menu1->addSeparator()->setText(tr("Unit"));
    auto *actGrp = new QActionGroup(this);
    int i = 0;
    for (; i<GisQmlInt::MEASURE_UNIT_LAST; ++i)
    {
        act = new QAction(getMeasurementUnit(i), this);
        act->setCheckable(true);
        act->setData(i);
        if (i == 0)
            act->setChecked(true); //default
        actGrp->addAction(act);
        connect(act, &QAction::triggered, this,
                [this](bool checked)
                {
                    if (checked)
                        mMapCanvas->updateMeasurements(
                             qobject_cast<QAction *>(sender())->data().toInt());
                });
    }
    menu1->addActions(actGrp->actions());
    ui->measureTlBtn->setMenu(menu1);
    //add menu to search button
    ui->searchTlBtn->setPopupMode(QToolButton::MenuButtonPopup);
    menu1 = new QMenu(this);
    ui->searchTlBtn->setMenu(menu1);
    QMenu *menu2 = menu1->addMenu(tr("Nearby Search Radius") + " (km): ");
    auto *sb = new QSpinBox(this);
    sb->setRange(1, 100);
    sb->setWrapping(true);
    connect(sb, SIGNAL(valueChanged(QString)),
            mMapCanvas, SLOT(onSrchRadiusChanged(QString)));
    mMapCanvas->onSrchRadiusChanged(sb->text());
    auto *wact = new QWidgetAction(this);
    wact->setDefaultWidget(sb);
    menu2->addAction(wact);
    setTheme();
}

GisWindow::~GisWindow()
{
    delete mMapCanvas;
    delete mDgnaList;
    delete mHistory;
    delete mLayers;
    delete mLegend;
    delete mOverviewMap;
    delete mRoute;
    delete mTracking;
    delete mBookmarks;
    delete mSearchPt;
    delete ui;
}

void GisWindow::setTheme()
{
    QString ss(Style::getStyle(Style::OBJ_FRAME_TITLE));
    ui->mapBtnFrame->setStyleSheet(ss);
    ui->mapInfoFrame->setStyleSheet(ss);
    ui->reloadTlBtn->setStyleSheet(Style::getStyle(Style::OBJ_TOOLBUTTON));
    int theme = Style::getTheme(); //new theme sTheme >= Style::THEME_MAX
    if (sTheme >= Style::THEME_MAX ||
        (theme == Style::THEME_DARK && sTheme != Style::THEME_DARK) ||
        (theme != Style::THEME_DARK && sTheme == Style::THEME_DARK))
        reloadMap();
    sTheme = theme;
    ui->searchCb->setStyleSheet(Style::getStyle(Style::OBJ_COMBOBOX));
    ui->filterBtn->menu()->setStyleSheet(Style::getStyle(Style::OBJ_MENU));
    ui->zoomSlider->setStyleSheet(Style::getStyle(Style::OBJ_SLIDER_ZOOM));
    setButtonStyles(mMapCanvas->getMode());
    mBookmarks->setTheme();
    mDgnaList->setTheme();
    mLayers->setTheme();
    mLegend->setTheme();
    mOverviewMap->onSetTheme();
#ifdef GIS_ROUTING
    mRoute->setTheme();
#endif
    mTracking->setTheme();
}

void GisWindow::refreshMap()
{
    ui->versionLbl->setText(sMapVersion);
    reloadMap();
}

void GisWindow::refreshData()
{
    GisMenuUserPoi::setCategories();
    Props::ValueMapsT prs;
    if (mPoi->loadData(prs, mUserName.toStdString()))
        mMapCanvas->poiLoad(prs);
}

void GisWindow::clearData()
{
    if (mMapCanvas->isValid())
        mMapCanvas->clearData();
}

void GisWindow::modeReset()
{
    resetToMode(GisQmlInt::MODE_SELECT);
}

#ifdef INCIDENT
void GisWindow::modeResources()
{
    resetToMode(GisQmlInt::MODE_RESOURCES, GisQmlInt::TYPEID_RSC_INCIDENT);
}

void GisWindow::selectResources(const QPointF &pt, const QString &radius)
{
    mMapCanvas->getTerminalsNearby(pt, radius);
}

void GisWindow::incidentLock(int id)
{
    mMapCanvas->setIncidentLock(id);
}

void GisWindow::incidentUpdate(IncidentData *data, bool fromReport)
{
    if (data == 0)
    {
        assert("Bad param in GisWindow::incidentUpdate" == 0);
        return;
    }
    if (fromReport)
        mMapCanvas->incidentUpdate(data, GisQmlInt::TYPEID_INCIDENT_REPORT);
    else if (data->isClosed())
        mMapCanvas->incidentClose(data->getId());
    else
        mMapCanvas->incidentUpdate(data, GisQmlInt::TYPEID_INCIDENT);
}

void GisWindow::incidentClose(int id)
{
    mMapCanvas->incidentClose((id == 0)? GisQmlInt::TYPEID_INCIDENT: id);
}

void GisWindow::incidentPlot(double lat, double lon)
{
    if (mMapCanvas->isValid())
        mMapCanvas->incidentPlot(lat, lon);
}
#endif //INCIDENT

void GisWindow::setRscInCall(int issi, bool start)
{
    if (issi > 0)
        mMapCanvas->setRscInCall(issi, start);
}

void GisWindow::setCtrRscInCall(bool enable)
{
    mMapCanvas->setCtrRscInCall(enable);
}

void GisWindow::enableSeaMap(bool on)
{
    mLayers->enableSeaMap(on);
}

void GisWindow::terminalUpdate(bool          isValid,
                               int           issi,
                               double        lon,
                               double        lat,
                               const string &timestamp)
{
    if (mMapCanvas->isValid())
        mMapCanvas->terminalUpdate(isValid, issi, lon, lat, timestamp);
}

void GisWindow::terminalUpdate(int issi, int type)
{
    if (mMapCanvas->isValid())
        mMapCanvas->terminalUpdateType(issi, type);
}

void GisWindow::terminalRemove(int issi)
{
    if (mMapCanvas->isValid())
        mMapCanvas->terminalRemove(issi);
}

void GisWindow::terminalRemove(bool rmList, const std::set<int> &issis)
{
    if (mMapCanvas->isValid())
        mMapCanvas->terminalRemove(rmList, issis);
}

void GisWindow::terminalsFilterUpdate(const set<int> &gssis)
{
    if (ui->filterBtn->isCheckable())
    {
        for (auto i : gssis)
        {
            if (mFilterGssis.count(i) != 0)
            {
                terminalsFilter(true);
                break;
            }
        }
    }
}

void GisWindow::poiLoad(int id)
{
    Props::ValueMapT pr;
    Props::set(pr, Props::FLD_KEY, id);
    if (mPoi->loadData(pr))
        mMapCanvas->poiLoad(pr);
}

void GisWindow::poiRemove(int id)
{
    mMapCanvas->poiRemove(id);
}

bool GisWindow::setMapInfo(const QString &ver, const QString &path)
{
    bool ret = (sMapVersion != ver);
    sMapVersion = ver;
    return (GisQmlInt::setMapPath(path) || ret);
}

void GisWindow::setNewTheme()
{
    int theme = Style::getTheme(); //new theme
    //if changing from dark to light or vice versa, save sTheme with THEME_MAX
    //offset to indicate forced refresh for setTheme()
    if (sTheme < 0 ||
        (theme == Style::THEME_DARK &&
         (sTheme == Style::THEME_DARK || sTheme >= Style::THEME_MAX)) ||
        (theme != Style::THEME_DARK && sTheme != Style::THEME_DARK))
        sTheme = theme;
    else
        sTheme = theme + Style::THEME_MAX;
}

void GisWindow::onMainMapLoaded()
{
    if (mMapCanvas->isValid())
    {
        refreshData();
        mLayers->populateLayerList();
        updateZoomLevel(mMapCanvas->getZoomLevel());
        modeReset();
        if (sMapVersion.isEmpty())
            ui->versionLbl->setText(tr("<Missing version information>"));
        else
            ui->versionLbl->setText(sMapVersion);
        mLayers->enableSeaMap(
                        Settings::instance().get<bool>(Props::FLD_CFG_MAP_SEA));
        mLegend->showLegend();
        emit mainMapLoaded();
    }
    ui->reloadTlBtn->setEnabled(true);
}

void GisWindow::onTerminalLblOptChanged()
{
    if (mMapCanvas->isValid())
        mMapCanvas->updateLabelLayer(GisQmlInt::TYPEID_TERMINAL);
}

void GisWindow::onTerminalLocate(int issi)
{
    if (mMapCanvas->isValid())
       mMapCanvas->terminalLocate(issi);
}

void GisWindow::onTerminalCheckTimeChanged()
{
    mMapCanvas->terminalCheckTimeChanged();
}

void GisWindow::onSearchNearby(double lat, double lon, const QString &label)
{
    static const QIcon icon(ICON_SEARCH_NEARBY);
    delete mSearchPt;
    mSearchPt = new QPointF(lon, lat);
    ui->searchTlBtn->setIcon(icon);
    ui->searchTlBtn->setProperty(PROP_SEARCH_LABEL, label);
    if (label.isEmpty())
        ui->searchTlBtn->setToolTip(tr("Search nearby %1")
                                    .arg(GisPoint::getCoords(lat, lon)));
    else
        ui->searchTlBtn->setToolTip(tr("Search nearby %1 (%2)")
                                    .arg(label, GisPoint::getCoords(lat, lon)));
    ui->searchCb->lineEdit()->setPlaceholderText(ui->searchTlBtn->toolTip());
    ui->searchCb->lineEdit()->selectAll();
    ui->searchCb->setFocus();
}

#ifdef INCIDENT
void GisWindow::onClearReportIncidents()
{
    mMapCanvas->incidentsReportClear();
}
#endif

void GisWindow::onCoordReceived(int typeId, double x, double y)
{
    if (!mMapCanvas->isValid())
        return;
    switch (typeId)
    {
#ifdef INCIDENT
        case GisQmlInt::TYPEID_INCIDENT:
            emit incCoordinates(x, y);
            break;
#endif
        case GisQmlInt::TYPEID_ROUTE_START:
        case GisQmlInt::TYPEID_ROUTE_END:
            emit pointCoordinates(typeId, x, y);
            break;
        default:
            break; //do nothing
    }
}

void GisWindow::onZoomTo(double lat, double lon)
{
    if (mMapCanvas->isValid())
        mMapCanvas->setGeomCenter(QPointF(lon, lat));
}

void GisWindow::updateZoomLevel(double zoomLvl)
{
    ui->zoomSlider->setValue((zoomLvl - GisQmlInt::ZOOM_MIN) * ZOOM_FACTOR);
}

void GisWindow::hideEvent(QHideEvent *event)
{
    for (auto &d : findChildren<QDialog *>())
    {
        //when minimized, the dialogs already become invisible - should not
        //hide() here, because otherwise they will not reappear upon
        //restore/maximize even with show() in showEvent() below
        if (!isMinimized())
            d->hide();
    }
    QWidget::hideEvent(event);
}

void GisWindow::showEvent(QShowEvent *event)
{
    for (auto &d : findChildren<QDialog *>())
    {
        d->show();
    }
    QWidget::showEvent(event);
}

void GisWindow::resizeEvent(QResizeEvent *event)
{
    mMapCanvas->refresh();
    QWidget::resizeEvent(event);
}

inline void GisWindow::setScale()
{
    if (mMapCanvas->isValid())
        ui->scaleLbl
          ->setText("1:" + QString::number(mMapCanvas->getMapScale(), 'f', 0));
}

void GisWindow::resetToMode(int mode, int typeId)
{
    if (!mMapCanvas->isValid())
        return;
    mMapCanvas->setMode(mode, typeId);
    setButtonStyles(mode);
}

void GisWindow::setButtonStyles(int mode)
{
    ui->selectTlBtn->setChecked(mode == GisQmlInt::MODE_SELECT);
    ui->zoomTlBtn->setChecked(mode == GisQmlInt::MODE_ZOOM);
    ui->measureTlBtn->setChecked(mode == GisQmlInt::MODE_MEASURE);
}

void GisWindow::terminalsFilter(bool enable)
{
    if (enable)
    {
        ResourceData::IdsT issis;
#ifdef DGNA_OF_GRPS
        ResourceData::IdsT gssis;
#endif
        for (auto i : mFilterGssis)
        {
#ifdef DGNA_OF_GRPS
            if (ResourceData::getType(i) == ResourceData::TYPE_DGNA_GRP)
            {
                if (SubsData::getGrpMembers(i, gssis))
                {
                    for (auto j : gssis)
                    {
                        SubsData::getGrpAttachedMembers(j, issis);
                    }
                    gssis.clear();
                }
            }
            else
#endif
            {
                SubsData::getGrpAttachedMembers(i, issis);
            }
        }
        if (issis.empty())
            issis.insert(0);
        LOGGER_DEBUG(mLogger, "GisWindow:: terminalsFilter: GSSIs="
                     << Utils::toStringWithRange(mFilterGssis) << " ISSIs="
                     << Utils::toStringWithRange(issis));
        mMapCanvas->terminalsFilter(&issis);
    }
    else
    {
        LOGGER_DEBUG(mLogger, "GisWindow:: terminalsFilter: Off");
        mMapCanvas->terminalsFilter(0); //turn off filter
    }
}

void GisWindow::reloadMap()
{
    mMapCanvas->refreshMap();
    mOverviewMap->reload();
}

const QString &GisWindow::getMeasurementUnit(int unit)
{
    if (sUnitMap.count(unit) == 0)
        unit = GisQmlInt::MEASURE_UNIT_LAST;
    return sUnitMap[unit];
}
