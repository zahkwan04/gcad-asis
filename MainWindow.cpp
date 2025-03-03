/**
 * The main UI implementation.
 *
 * Copyright (C) Sapura Secured Technologies, 2013-2025. All Rights Reserved.
 *
 * @file
 * @version $Id: MainWindow.cpp 1905 2025-02-21 02:55:53Z rosnin $
 * @author Mazdiana Makmor
 * @author Nurfaizatul Ain Othman
 */
#include <QGuiApplication>
#include <QList>
#include <QMdiArea>
#include <QMenu>
#include <QMimeData>
#include <QNetworkInterface>
#include <QPixmap>
#include <QScreen>
#include <QStyleFactory>
#include <QTimer>
#include <iostream>
#include <string>
#include <assert.h>

#ifdef INCIDENT
#include "ActiveIncidents.h"
#endif
#include "AudioPlayer.h"
#include "CmnTypes.h"
#include "CommsRegister.h"
#include "DbInt.h"
#include "GpsMonitor.h"
#include "MessageDialog.h"
#include "Props.h"
#include "QtUtils.h"
#include "ResourceData.h"
#include "Settings.h"
#include "Style.h"
#include "SubsData.h"
#include "Updater.h"
#include "Version.h"
#include "VideoDevice.h"
#include "ui_MainWindow.h"
#include "MainWindow.h"

using namespace std;

static const int MAX_LOGIN_FAILS = 3;
static const int BROADCAST_SSI   = ServerSession::BROADCAST_SSI;

static const string LOGPREFIX("MainWindow:: ");

static const QString ICON_ONLINE  (":/Images/images/icon_stat_online.png");
static const QString ICON_OFFLINE (":/Images/images/icon_stat_offline.png");
static const QString ICON_VOIP_OFF(":/Images/images/icon_stat_voip_off.png");

//keys for mMdiSubs
enum eSubWindow
{
    SUBW_CALL,
    SUBW_CONTACTS,
    SUBW_INCIDENT,
    SUBW_REPORT,
    SUBW_RESOURCES,
    SUBW_SDS
};

MainWindow::MainWindow(QWidget *parent) :
QMainWindow(parent), ui(new Ui::MainWindow), mLoginFailCount(0), mSession(0),
mGisWindow(0), mLogin(0), mProc(0), mPoi(0)
{
    Style::init();
    mSettingsUi = new SettingsUi();
    connect(mSettingsUi, SIGNAL(isFinished(bool)),
            SLOT(onSettingsFinished(bool)));
    connect(mSettingsUi, SIGNAL(setTheme()), SLOT(onSetTheme()));
    connect(mSettingsUi, &SettingsUi::gpsMon, this,
            [this] { showGpsMon(mSettingsUi); });
    connect(mSettingsUi, &SettingsUi::branchChanged, this,
            [this]
            {
                //send branch change request to server
                if (mSession != 0 && mSession->isValid())
                {
                    string b(Settings::instance()
                             .get<string>(Props::FLD_CFG_BRANCH));
                    mSession->setBranches(&b);
                }
            });
    Settings &cfg(Settings::instance());
    mLogger = new Logger(cfg.get<string>(Props::FLD_CFG_LOGFILE));
    LOGGER_RAW(mLogger, Version::logHeader());
    Updater::setLogger(mLogger);
    ResourceData::init(mLogger);
    ServerSession::setVersion(Version::APP_VERSION.toStdString());
    mSettingsUi->setLogger(mLogger);
    string macs;
    foreach (QNetworkInterface ni, QNetworkInterface::allInterfaces())
    {
        //get all active non-loopback MAC addresses
        if (!ni.flags().testFlag(QNetworkInterface::IsLoopBack) &&
            ni.flags().testFlag(QNetworkInterface::IsUp) &&
            ni.flags().testFlag(QNetworkInterface::IsRunning))
            macs.append(" ").append(ni.hardwareAddress().toStdString());
    }
    if (macs.empty())
    {
        LOGGER_ERROR(mLogger, LOGPREFIX << "Failed to get MAC address.");
    }
    else
    {
        macs.erase(0, 1); //remove the first space
        ServerSession::setMacAddresses(macs);
    }
    ui->setupUi(this);
    mStatusIconLbl = new QLabel(this);
    statusBar()->addPermanentWidget(mStatusIconLbl);
    mStatusIconLbl->setPixmap(QPixmap(ICON_OFFLINE));
    mStatusLbl = new QLabel(this);
    mStatusLbl->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    statusBar()->insertPermanentWidget(0, mStatusLbl);
    mStatusIssiLbl = new QLabel(this);
    mStatusIssiLbl->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    statusBar()->insertPermanentWidget(0, mStatusIssiLbl);
    QString ss("background-color:");
    ss.append(Style::getStyle(Style::OBJ_BACKGROUND_PANEL));
    statusBar()->setStyleSheet(ss);
    ui->btnsScroll->setStyleSheet(ss);
    ui->upperFrame->setStyleSheet(ss);
    ui->headerFrame->setStyleSheet("background-color:" +
                                   Style::getStyle(Style::OBJ_BACKGROUND_DARK));
    ui->idLabel->setStyleSheet(Style::getStyle(Style::OBJ_LABEL_WHITE));
    setWindowState(Qt::WindowFullScreen);
    setWindowTitle(Version::APP_NAME);
    ui->titleLabel->setText(Version::NWK_NAME);
    ui->titleLabel->setStyleSheet(Style::getStyle(Style::OBJ_LABEL_TITLE));
    ui->mapButton->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->mapButton, &QToolButton::customContextMenuRequested, this,
            [this](const QPoint &pos)
            {
                if (mGisWindow == 0)
                    return;
                QMenu menu(this);
                menu.addAction(QtUtils::getActionIcon(
                                                   CmnTypes::ACTIONTYPE_DELETE),
                               tr("Delete Map Window"));
                if (menu.exec(ui->mapButton->mapToGlobal(pos)) != 0)
                {
#ifdef INCIDENT
                    mIncident->setGis(0);
#endif
                    mGisWindow->close();
                    delete mGisWindow;
                    mGisWindow = 0;
                    QtUtils::setGisOpen(false);
                }
            });
    ui->oskButton->hide();
#ifdef _WIN32
    if (QFile::exists("C:\\Windows\\System32\\osk.exe"))
    {
        ui->oskButton->setToolTip(tr("On-screen keyboard"));
        ui->oskButton->show();
        mProc = new QProcess(this);
        mProc->setProgram("explorer.exe");
        mProc->setArguments(QStringList() << "C:\\Windows\\System32\\osk.exe");
    }
#endif
    ui->monGrpList->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->monGrpList->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->monGrpList->viewport()->setAcceptDrops(true);
    ui->monGrpList->setDropIndicatorShown(true);
    ui->monGrpList->setDragDropMode(QAbstractItemView::DropOnly);
    ui->monGrpList->installEventFilter(this);
    ui->monGrpList
      ->setModel(ResourceData::createModel(ResourceData::TYPE_GROUP_OR_DGNA,
                                           ui->monGrpList));
    ui->monIndList->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->monIndList->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->monIndList->viewport()->setAcceptDrops(true);
    ui->monIndList->setDropIndicatorShown(true);
    ui->monIndList->setDragDropMode(QAbstractItemView::DropOnly);
    ui->monIndList->installEventFilter(this);
    ui->monIndList
      ->setModel(ResourceData::createModel((ResourceData::isFullMode())?
                                              ResourceData::TYPE_SUBS_OR_MOBILE:
                                              ResourceData::TYPE_MOBILE,
                                           ui->monIndList));
    qRegisterMetaType<std::string>("string");
    connect(ui->monGrpList, SIGNAL(customContextMenuRequested(QPoint)),
            SLOT(showMonContextMenu(QPoint)));
    connect(ui->monIndList, SIGNAL(customContextMenuRequested(QPoint)),
            SLOT(showMonContextMenu(QPoint)));
    int val = cfg.get<int>(Props::FLD_CFG_MSG_TMR_INTERVAL);
    if (val < 0)
    {
        //msg timer disabled - use signal/slot
        connect(this, SIGNAL(serverMsg(MsgSp*)), SLOT(onServerMsg(MsgSp*)));
    }
    else
    {
        PalLock::init(&mMsgQueueLock);
        mMsgTimer.setInterval(val);
        connect(&mMsgTimer, &QTimer::timeout, this,
                [this]
                {
                    MsgSp *m;
                    PalLock::take(&mMsgQueueLock);
                    while (!mMsgQueue.empty())
                    {
                        //must pop msg first because onServerMsg() may lead to
                        //deleteSession() which does queue clearing
                        m = mMsgQueue.front();
                        mMsgQueue.pop();
                        onServerMsg(m);
                    }
                    PalLock::release(&mMsgQueueLock);
                });
        //start mMsgTimer just before creating ServerSession
    }
    mResources = new Resources(mLogger);
    mDgna = new Dgna(mLogger, mResources);
    connect(mResources, SIGNAL(dgnaSelected(int,int,ResourceData::ListModel*)),
            mDgna, SLOT(setDgnaItems(int,int,ResourceData::ListModel*)));
    connect(mResources, SIGNAL(startAction(int,int,ResourceData::IdsT)),
            SLOT(onStartAction(int,int,ResourceData::IdsT)));

    mResourceSelector = new ResourceSelector();
    connect(mResources, SIGNAL(sendData(int)),
            mResourceSelector, SLOT(setData(int)));

    auto *callCommsReg = new CommsRegister(mLogger, CommsRegister::TYPE_CALL,
                                           this);
    auto *msgCommsReg = new CommsRegister(mLogger, CommsRegister::TYPE_MSG,
                                          this);
    connect(mSettingsUi, SIGNAL(rscDspOptChanged(int)),
            callCommsReg, SLOT(onRscDspOptChanged(int)));
    connect(mSettingsUi, SIGNAL(rscDspOptChanged(int)),
            msgCommsReg, SLOT(onRscDspOptChanged(int)));
    connect(callCommsReg, &CommsRegister::missedCall, this,
            [this]
            {
                //highlight Call button text
                ui->callButton->setStyleSheet(
                              Style::getStyle(Style::OBJ_TOOLBUTTON_HIGHLIGHT));
            });
    connect(msgCommsReg, &CommsRegister::unreadMessages, this,
            [this](int count)
            {
                //with unread messages, show the count and turn on button text
                //highlighting, otherwise turn off highlighting
                ui->sdsButton->setText(tr("Message") +
                               ((count > 0)? " " + QString::number(count): ""));
                ui->sdsButton->setStyleSheet((count > 0)?
                               Style::getStyle(Style::OBJ_TOOLBUTTON_HIGHLIGHT):
                               Style::getStyle(Style::OBJ_TOOLBUTTON));
            });
    mCall = new Call(mLogger, mResourceSelector, callCommsReg, this);
    connect(mCall, &Call::newCall, this,
            [this](int calledType, int ssi) { onNewCall(calledType, ssi); });
    connect(mCall, SIGNAL(newBroadcast()), SLOT(onNewBroadcastCall()));
    connect(mResourceSelector, SIGNAL(selectionChanged(bool,bool)),
            mCall, SLOT(onSelectionChanged(bool,bool)));
    mSds = new Sds(mLogger, mResourceSelector, msgCommsReg,
                   new MmsClient(mLogger, this, serverCallback), this);
    connect(mSettingsUi, SIGNAL(mmsDownloadDirChanged()),
            mSds, SLOT(onMmsDownloadDirChanged()));
    connect(msgCommsReg, SIGNAL(mmsDownload(MsgSp*)),
            mSds, SLOT(onMmsDownload(MsgSp*)));
    connect(mResourceSelector, SIGNAL(selectionChanged(bool,bool)),
            mSds, SLOT(onSelectionChanged(bool,bool)));
    connect(mSettingsUi, &SettingsUi::rscDspOptChanged,
            [](int type) { Contact::onRscDspOptChanged(type); });

    auto *audioPlayer = new AudioPlayer(mLogger, this);
    //Report takes ownership of audioPlayer
    mReport = new Report(audioPlayer, mLogger);
#ifdef INCIDENT
    auto *activeInc = new ActiveIncidents(true, this);
    connect(activeInc, &ActiveIncidents::showData, this,
            [this](IncidentButton *) { ui->incidentButton->click(); });
    ui->vSplitter->insertWidget(0, activeInc);
    //Incident takes ownership of activeInc
    mIncident = new Incident(audioPlayer, mLogger, activeInc, this);
    connect(callCommsReg, SIGNAL(newComm(int,QString,QString)),
            mIncident, SLOT(onNewComm(int,QString,QString)));
    connect(msgCommsReg, SIGNAL(newComm(int,QString,QString)),
            mIncident, SLOT(onNewComm(int,QString,QString)));
    connect(mIncident,
            SIGNAL(startAction(int,int,ResourceData::IdsT)),
            SLOT(onStartAction(int,int,ResourceData::IdsT)));
    connect(mIncident, &Incident::assignedResources, this,
            [this](int idType, const ResourceData::IdsT &ids, int id)
            {
                //id==0 if incident not yet created
                LOGGER_VERBOSE(mLogger, LOGPREFIX << "Adding Incident-" << id
                               << " assigned resources to monitoring: "
                               << ResourceData::toString(ids));
                ResourceData::IdsT validIds(ids);
                if (validate(idType, validIds))
                    addContacts(idType, true, validIds);
            });
    connect(mIncident, &Incident::sendSds, this,
            [this](int                       idType,
                   const ResourceData::IdsT &ids,
                   const QStringList        &msgs,
                   const QString            &txt)
            {
                if (msgs.isEmpty())
                    ui->sdsButton->click(); //user to type message
                mSds->sendMsg(idType, ids, msgs, txt);
            });
    connect(mReport, &Report::showIncidentById, this,
            [this](int id)
            {
                if (mIncident->showData(id))
                    ui->incidentButton->click();
            });
    connect(mReport, &Report::showIncidentByData, this,
            [this](IncidentData *data)
            {
                ui->incidentButton->click();
                mIncident->showData(data);
            });
    connect(mReport,
            SIGNAL(plotIncidents(std::set<IncidentData*>,std::set<int>)),
            mIncident, SLOT(onPlotData(std::set<IncidentData*>,std::set<int>)));
#else
    delete ui->incidentButton;
#endif //INCIDENT

    connect(ui->mdiArea, &QMdiArea::subWindowActivated, this,
            [this](QMdiSubWindow *sub)
            {
                //activate btn for this sub, and deactivate others
                for (const auto &it : mMdiSubs)
                {
                    if (it.second != sub)
                    {
                        getMdiBtn(it.first)->setChecked(false);
                        continue;
                    }
                    getMdiBtn(it.first)->setChecked(true);
                    //certain subwindows need to handle this event to take over
                    //mResourceSelector
                    switch (it.first)
                    {
                        case SUBW_CALL:
                            mCall->activate();
                            //clear button highlight
                            if (Style::isHighlighted(
                                                  ui->callButton->styleSheet()))
                                ui->callButton->setStyleSheet(
                                        Style::getStyle(Style::OBJ_TOOLBUTTON));
                            break;
                        case SUBW_SDS:
                            mSds->activate();
                            break;
                        default:
                            break; //do nothing
                    }
                } //for
            });
    ui->mdiArea->setBackground(QBrush(QColorConstants::Black));
    mdiAdd(SUBW_CONTACTS, Contact::init(mLogger, mResources, this));
    mdiAdd(SUBW_RESOURCES, mResources);
    mdiAdd(SUBW_CALL, mCall);
    mdiAdd(SUBW_SDS, mSds);
#ifdef INCIDENT
    mdiAdd(SUBW_INCIDENT, mIncident);
#endif
    mdiAdd(SUBW_REPORT, mReport);

    ui->callsOuterFrame->layout()->setAlignment(Qt::AlignTop);
    ui->monOuterFrame->layout()->setAlignment(Qt::AlignBottom);
    mIndFlowLayout = new FlowLayout(ui->indCallScrollWidget);
    mGrpFlowLayout = new FlowLayout(ui->grpCallScrollWidget);

    mAudioMgr = new AudioManager(mLogger);
    CallWindow::setAudioManager(mAudioMgr);
    connect(mSettingsUi, SIGNAL(audioInChanged(QString)),
            mAudioMgr, SLOT(onAudioInChanged(QString)));
    connect(mSettingsUi, SIGNAL(audioOutChanged(QString)),
            mAudioMgr, SLOT(onAudioOutChanged(QString)));
    onSetTheme();
    RtpSession::init();
    connect(ui->contactsButton, &QToolButton::clicked, this,
            [this] { mdiActivate(SUBW_CONTACTS); });
    connect(ui->resourcesButton, &QToolButton::clicked, this,
            [this] { mdiActivate(SUBW_RESOURCES); });
    connect(ui->callButton, &QToolButton::clicked, this,
            [this]
            {
                //clear highlight
                if (Style::isHighlighted(ui->callButton->styleSheet()))
                    ui->callButton
                      ->setStyleSheet(Style::getStyle(Style::OBJ_TOOLBUTTON));
                mdiActivate(SUBW_CALL);
            });
    connect(ui->sdsButton, &QToolButton::clicked, this,
            [this] { mdiActivate(SUBW_SDS); });
#ifdef INCIDENT
    connect(ui->incidentButton, &QToolButton::clicked, this,
            [this] { mdiActivate(SUBW_INCIDENT); });
#endif
    connect(ui->reportButton, &QToolButton::clicked, this,
            [this] { mdiActivate(SUBW_REPORT); });
    connect(ui->settingsButton, &QToolButton::clicked, this,
            [this]
            {
                mSettingsUi->raise();
                mSettingsUi->show();
            });
    connect(ui->oskButton, &QToolButton::clicked, this,
            [this] { mProc->start(); }); //show on-screen keyboard
    connect(ui->callsButton, &QToolButton::clicked, this,
            [this]
            {
                bool h = ui->callsButton->isChecked();
                ui->indCallTitleFrame->setHidden(h);
                ui->indCallScrollArea->setHidden(h);
                ui->grpCallTitleFrame->setHidden(h);
                ui->grpCallScrollArea->setHidden(h);
            });
    connect(ui->indCallButton, &QToolButton::clicked, this,
            [this]
            {
                //toggle call scroll area visibility
                ui->indCallScrollArea->setHidden(ui->indCallButton->isChecked());
            });
    connect(ui->grpCallButton, &QToolButton::clicked, this,
            [this]
            {
                //toggle call scroll area visibility
                ui->grpCallScrollArea->setHidden(ui->grpCallButton->isChecked());
            });
    connect(ui->monButton, &QToolButton::clicked, this,
            [this]
            {
                //toggle monitoring frame visibility
                ui->monFrame->setHidden(!ui->monFrame->isHidden());
            });
    connect(ui->logoutButton, &QToolButton::clicked, this,
            [this]
            {
                mSettingsUi->setSession(0);
                logout(false);
            });
    connect(ui->mapButton, &QToolButton::clicked, this,
            [this]
            {
                if (mGisWindow == 0 || mGisWindow->getUserName() != mUserName)
                {
                    if (mPoi == 0)
                        mPoi = new Poi(mSession);
                    delete mGisWindow;
                    mGisWindow = new GisWindow(mPoi, mResources, mDgna, mLogger,
                                               mUserName, this);
                    //add window properties to QDialog
                    mGisWindow->setWindowFlags(Qt::Window);
#ifdef INCIDENT
                    mIncident->setGis(mGisWindow);
                    //Incident also gets these signals
                    connect(mGisWindow, &GisWindow::showIncident, this,
                            [this](int) { ui->incidentButton->click(); });
                    connect(mGisWindow, &GisWindow::incCoordinates, this,
                            [this]() { ui->incidentButton->click(); });
                    connect(mReport, SIGNAL(clearIncidents()),
                            mGisWindow, SLOT(onClearReportIncidents()));
#endif //INCIDENT
                    connect(mGisWindow,
                            SIGNAL(startAction(int,int,ResourceData::IdsT)),
                            SLOT(onStartAction(int,int,ResourceData::IdsT)));
                    connect(mGisWindow, &GisWindow::gpsMon, this,
                            [this] { showGpsMon(mGisWindow); });
                    connect(mResources, SIGNAL(locateResource(int)),
                            mGisWindow, SLOT(onTerminalLocate(int)));
                    connect(mDgna, SIGNAL(locateResource(int)),
                            mGisWindow, SLOT(onTerminalLocate(int)));
                    connect(mSettingsUi, SIGNAL(terminalCheckTimeChanged()),
                            mGisWindow, SLOT(onTerminalCheckTimeChanged()));
                    connect(mSettingsUi, SIGNAL(terminalLblOptChanged()),
                            mGisWindow, SLOT(onTerminalLblOptChanged()));
                    mGisWindow->show();
                    QtUtils::setGisOpen(true);
                    auto screens = QGuiApplication::screens();
                    if (screens.size() > 1)
                    {
                        mGisWindow->move(QPoint(screens[1]->geometry().x(),
                                                screens[1]->geometry().y()));
                        mGisWindow->showMaximized();
                    }
                }
                else
                {
                    mGisWindow->raise();
                    mGisWindow->show();
                    //restore from minimized
                    mGisWindow->setWindowState(
                              mGisWindow->windowState() & ~Qt::WindowMinimized |
                              Qt::WindowActive);
                }
            });
    LOGGER_INFO(mLogger, LOGPREFIX << cfg);
    if (cfg.isValid())
        onSettingsFinished(true);
    else
        mSettingsUi->show();
}

MainWindow::~MainWindow()
{
    deleteSession(mSession);
    delete mSettingsUi;
    delete mPoi;
    delete mGisWindow;
#ifdef INCIDENT
    delete mIncident;
#endif
    delete mReport;
    delete mIndFlowLayout;
    delete mGrpFlowLayout;
    delete mDgna;
    delete mSds;
    delete mCall;
    delete mAudioMgr;
    //mResourceSelector is deleted by the class that currently owns it -
    //mSds or mCall
    delete mResources;
    delete ui;
    delete mLogin;
    CallWindow::finalize();
    Settings::destroy();
    Updater::destroy();
    VideoDevice::destroy();
    delete mLogger;
    PalSocket::finalize();
    if (mMsgTimer.interval() > 0)
        PalLock::destroy(&mMsgQueueLock);
}

void MainWindow::serverCallback(void *obj, MsgSp *msg)
{
    if (obj == 0 || msg == 0)
    {
        assert("Bad param in MainWindow::serverCallback" == 0);
        return;
    }
    auto *w = static_cast<MainWindow *>(obj);
    if (w->mMsgTimer.isActive())
    {
        PalLock::take(&w->mMsgQueueLock);
        w->mMsgQueue.push(msg);
        PalLock::release(&w->mMsgQueueLock);
    }
    else if (w->mMsgTimer.interval() <= 0)
    {
        emit w->serverMsg(msg);
    }
    else
    {
        delete msg; //discard because will not be processed
    }
}

void MainWindow::onNewCall(int calledType, int ssi, bool doStart)
{
    //first check for a CallWindow that has no CallButton yet - if found, just
    //show it
    auto *call = getCallWindow(ssi, false);
    if (call == 0)
    {
        //now find a CallWindow that has a CallButton
        if (calledType == ResourceData::TYPE_DISPATCHER ||
            calledType == ResourceData::TYPE_MOBILE ||
            calledType == ResourceData::TYPE_MOBILE_ONLINE)
        {
            //CallButton for dispatcher/mobile call is tracked by negative ssi
            call = getCall(-ssi);
            //even if no CallWindow, the dispatcher may be in a network call,
            //but a call to it is allowed
        }
        else
        {
            call = getCallWindow(ssi, true);
            if (call != 0 && calledType == ResourceData::TYPE_SUBSCRIBER)
            {
                LOGGER_DEBUG(mLogger, LOGPREFIX << "onNewCall: "
                             "Individual call attempt to a busy party " << ssi);
                //must show window before QMessageBox
                call->doShow();
                QMessageBox::information(this, tr("Call Party Busy"),
                                         QString::number(ssi) +
                                         tr(" is already in another call."));
                return;
            }
        }
    }
    if (call == 0)
    {
        int callType;
        string domain;
        switch (calledType)
        {
            case ResourceData::TYPE_DISPATCHER:
                callType = CmnTypes::CALLTYPE_DISPATCHER;
                domain = SubsData::getClientDomain(ssi, true);
                break;
            case ResourceData::TYPE_MOBILE:
            case ResourceData::TYPE_MOBILE_ONLINE:
                if (ResourceData::hasMobileStat())
                {
                    domain = SubsData::getClientDomain(ssi, false);
                    if (!domain.empty())
                    {
                        callType = CmnTypes::CALLTYPE_MOBILE;
                        break;
                    }
                }
                domain = mSession->getMobDomain();
                //fallthrough
            case ResourceData::TYPE_SUBSCRIBER:
                callType = CmnTypes::CALLTYPE_IND_OUT;
                break;
            default:
                callType = CmnTypes::CALLTYPE_GROUP_OUT;
                break;
        }
        call = new CallWindow(callType, 0, 0, ResourceData::TYPE_DISPATCHER,
                              ssi, calledType, true, false, domain, this);
        connectCommonSignals(call, true, true);
        connect(call, SIGNAL(callCancel(int)), SLOT(onCallCancel(int)));
        if (call->isInternalCall() || !domain.empty())
            connect(call, SIGNAL(callRelease(int)), SLOT(onCallRelease(int)));
        mCallWindowMap[ssi] = call;
    }
    if (doStart)
    {
        call->startCall();
        if (call->isInternalCall())
            Contact::callEvent(MsgSp::Type::CALL_PROCEEDING, call);
    }
    else
    {
        call->doShow();
    }
}

void MainWindow::onNewBroadcastCall()
{
    CallWindow *call = getCallWindow(BROADCAST_SSI, false);
    if (call == 0)
    {
        //calledType is meaningless here
        call = new CallWindow(CmnTypes::CALLTYPE_BROADCAST_OUT, 0, 0,
                              CmnTypes::IDTYPE_DISPATCHER, BROADCAST_SSI,
                              ResourceData::TYPE_GROUP, false, false, "", this);
        mCallWindowMap[BROADCAST_SSI] = call;
        connectCommonSignals(call, true, false);
        connect(call, SIGNAL(callCancel(int)), SLOT(onCallCancel(int)));
    }
    call->doShow();
}

void MainWindow::onCallIncomingConnected(int ssi, int callId)
{
    CallWindow *call = getCallWindow(ssi, false);
    if (call != 0)
    {
        CallButton *btn = new CallButton(call);
        btn->setConnected("");
        removeCallWindow(ssi, false);
        mIndFlowLayout->addWidget(btn);
        if (callId != 0)
            mCallMap[callId] = btn;
        else
            mCallMap[-ssi] = btn; //dispatcher call
        Contact::callEvent(MsgSp::Type::CALL_CONNECT, call);
    }
}

void MainWindow::onActiveOutCall(CallWindow *cw)
{
    if (cw != 0)
        cw->disableOutAudio();
}

void MainWindow::onActiveInCall(int callId, int callParty)
{
    LOGGER_DEBUG(mLogger, LOGPREFIX << "onActiveInCall: callId=" << callId
                 << " callParty=" << callParty);
    for (auto &it : mCallMap)
    {
        if (it.first != callId && it.first != -callParty)
            it.second->getCallWindow()->disableInAudio();
    }
}

void MainWindow::onCallCancel(int ssi)
{
    removeCallWindow(ssi, true);
}

void MainWindow::onCallRelease(int callParty)
{
    //CallButton for dispatcher call is tracked by negative call party
    auto *btn = getCallButton(-callParty, true);
    if (btn == 0)
        onCallCancel(callParty);
    else
        deleteCall(btn, true, 0, callParty);
}

void MainWindow::onCallTimeout(int callId, int callingParty)
{
    if (callId != 0)
    {
        LOGGER_WARNING(mLogger, LOGPREFIX << "Dismissing monitored call "
                       << callId << " due to timeout");
        deleteCall(getCallButton(callId, true), true);
    }
    else
    {
        removeCallWindow(callingParty, true);
    }
}

void MainWindow::onSettingsFinished(bool ok)
{
    if (ok)
    {
        Settings &cfg(Settings::instance());
        vector<string> ips;
        vector<int> ports;
        cfg.getList(Props::FLD_CFG_SERVERIP, ips);
        cfg.getList(Props::FLD_CFG_SERVERPORT, ports);
        ServerSession::init(mLogger, ips, ports);
        if (mGisWindow != 0)
        {
            mGisWindow->setCtrRscInCall(
                                cfg.get<bool>(Props::FLD_CFG_MAP_CTR_RSC_CALL));
            mGisWindow->enableSeaMap(cfg.get<bool>(Props::FLD_CFG_MAP_SEA));
        }
        if (mLogin == 0 && mSession == 0)
            ui->logoutButton->click();
    }
    else if (mLogin == 0 && mSession == 0)
    {
        if (QMessageBox::question(this, tr("Warning"),
                                  tr("Settings are incomplete. "
                                     "The application cannot proceed.\n"
                                     "Are you sure to exit?")) ==
            QMessageBox::Yes)
            exitApp();
        else
            mSettingsUi->show();
    }
}

void MainWindow::onSetTheme()
{
    setStyleSheet(Style::getStyle(Style::OBJ_COMMON));
    QString ss(Style::getStyle(Style::OBJ_FRAME_TITLE));
    ui->monOuterFrame->setStyleSheet(ss);
    ui->callsOuterFrame->setStyleSheet(ss);
    ss = Style::getStyle(Style::OBJ_FRAME_TITLE2);
    ui->indCallTitleFrame->setStyleSheet(ss);
    ui->grpCallTitleFrame->setStyleSheet(ss);
    ui->monGrpLabelFrame->setStyleSheet(ss);
    ui->monIndLabelFrame->setStyleSheet(ss);
    ss = "border-radius:0px;background:" +
         Style::getStyle(Style::OBJ_BACKGROUND);
    ui->monGrpList->setStyleSheet(ss);
    ui->monIndList->setStyleSheet(ss);
    ss = Style::getStyle(Style::OBJ_TOOLBUTTON);
    ui->contactsButton->setStyleSheet(ss);
    ui->mapButton->setStyleSheet(ss);
    ui->logoutButton->setStyleSheet(ss);
    ui->resourcesButton->setStyleSheet(ss);
    ui->reportButton->setStyleSheet(ss);
    ui->settingsButton->setStyleSheet(ss);
    ui->oskButton->setStyleSheet(ss);
    const QString &ssh(Style::getStyle(Style::OBJ_TOOLBUTTON_HIGHLIGHT));
    if (Style::isHighlighted(ui->callButton->styleSheet()))
        ui->callButton->setStyleSheet(ssh);
    else
        ui->callButton->setStyleSheet(ss);
    if (Style::isHighlighted(ui->sdsButton->styleSheet()))
        ui->sdsButton->setStyleSheet(ssh);
    else
        ui->sdsButton->setStyleSheet(ss);
#ifdef INCIDENT
    ui->incidentButton->setStyleSheet(ss);
    mIncident->setTheme();
#endif
    //set mStatusLbl style only in normal situation
    ss = mStatusLbl->styleSheet();
    if (ss != Style::getStyle(Style::OBJ_LABEL_STATUS_OFFLINE) &&
        ss != Style::getStyle(Style::OBJ_LABEL_STATUS_ONLINE_NODATA))
        mStatusLbl->setStyleSheet(Style::getStyle(Style::OBJ_LABEL_WHITE));
    Contact::setTheme();
    mResources->setTheme();
    mResourceSelector->setTheme();
    mDgna->setTheme();
    mCall->setTheme();
    mSds->setTheme();
    mReport->setTheme();
    if (mGisWindow != 0)
        mGisWindow->setTheme();
    else
        GisWindow::setNewTheme();
    //change MDI subwindows title bar color
    //getThemeColor() returns "rgb(rval,gval,bval)" - capture the values
    static const QRegularExpression RE("\\D+");
    QList<int> l;
    for (auto &x:
         Style::getThemeColor(Style::THEME_COLOR_IDX1)
             .split(RE, QString::SkipEmptyParts))
    {
        l << x.toInt();
    }
    QColor c(QColor::fromRgb(l.at(0), l.at(1), l.at(2)));
    QPalette cp;
    cp.setColor(QPalette::Window, c);
    cp.setColor(QPalette::Highlight, c);
    cp.setColor(QPalette::Light, c);
    l.clear();
    for (auto &x:
         Style::getThemeColor(Style::THEME_COLOR_IDX2)
             .split(RE, QString::SkipEmptyParts))
    {
        l << x.toInt();
    }
    cp.setColor(QPalette::Inactive, QPalette::Window,
                QColor::fromRgb(l.at(0), l.at(1), l.at(2)));
    for (auto &it : mMdiSubs)
    {
        it.second->setPalette(cp);
        //this plus processEvents() below ensure faster change on screen
        it.second->repaint();
    }
    qApp->processEvents();
}

void MainWindow::onLoginCancel()
{
    deleteSession(mSession);
    mLogin->enable();
}

void MainWindow::onServerMsg(MsgSp *msg)
{
    int         callId;
    int         val;
    int         callerId;
    QString     strVal;
    CallButton *btn = 0;
    CallWindow *call = 0;

    switch (msg->getType())
    {
        case MsgSp::Type::MON_GRP_ATTACH_DETACH:
            //ServerSession may have added field or changed type before callback
            if (msg->hasField(MsgSp::Field::GRP_LIST))
            {
                LOGGER_VERBOSE(mLogger, LOGPREFIX << "onServerMsg:\n" << *msg);
                break;
            }
            //fallthrough
        default:
            LOGGER_DEBUG(mLogger, LOGPREFIX << "onServerMsg:" << msg->getName());
            break;
    }
    switch (msg->getType())
    {
        case MsgSp::Type::BRANCH_DATA:
        {
            if (msg->getFieldInt(MsgSp::Field::RESULT) ==
                MsgSp::Value::RESULT_REJECTED)
                Settings::instance().set(Props::FLD_CFG_BRANCH_ALLOWED, "")
                                    .set(Props::FLD_CFG_BRANCH, "");
            break;
        }
        case MsgSp::Type::CALL_ALERT:
        {
            val = msg->getFieldInt(MsgSp::Field::CALLED_PARTY);
            call = getCallWindow(val, false);
            if (call == 0)
            {
                call = getCallWindow(val, true,
                                     msg->getFieldString(MsgSp::Field::CALL_ID));
            }
            else if (call->isInternalCall())
            {
                //outgoing call to dispatcher/mobile
                btn = new CallButton(call);
                removeCallWindow(val, false);
                mIndFlowLayout->addWidget(btn);
                //CallButton for disp/mobile is tracked by negative call party
                mCallMap[-val] = btn;
            }
            if (call != 0)
            {
                call->callAlert();
                Contact::callEvent(msg->getType(), call);
            }
            break;
        }
        case MsgSp::Type::CALL_CONNECT:
        {
            int remoteRtp = msg->getFieldInt(MsgSp::Field::VOIP_AUDIO_RTP_PORT);
            if (remoteRtp != MsgSp::Value::UNDEFINED)
            {
                //message is from VOIP session
                val = msg->getFieldInt(MsgSp::Field::CALLED_PARTY);
                btn = getCallButton(-val);
                if (btn != 0)
                {
                    //dispatcher call
                    btn->setConnected(strVal);
                    call = btn->getCallWindow();
                    call->setConnected(true, false,
                                       MsgSp::Value::TRANSMISSION_GRANTED);
                }
                else
                {
                    call = getCallWindow(val, true,
                                    msg->getFieldString(MsgSp::Field::CALL_ID));
                }
                if (call != 0)
                {
                    if (btn == 0)
                        call->setConnected(strVal);
                    call->setConnected(val,
                         msg->getFieldInt(MsgSp::Field::VOIP_AUDIO_RTP_LPORT),
                         remoteRtp,
                         msg->getFieldInt(MsgSp::Field::VOIP_VIDEO_RTP_LPORT),
                         msg->getFieldInt(MsgSp::Field::VOIP_VIDEO_RTP_PORT),
                         msg->getFieldString(MsgSp::Field::VOIP_AUDIO_RTP_LKEY),
                         msg->getFieldString(MsgSp::Field::VOIP_AUDIO_RTP_KEY),
                         msg->getFieldString(MsgSp::Field::VOIP_VIDEO_RTP_LKEY),
                         msg->getFieldString(MsgSp::Field::VOIP_VIDEO_RTP_KEY));
                    Contact::callEvent(msg->getType(), call);
                }
                break;
            }
            if (msg->hasField(MsgSp::Field::TX_PARTY))
                strVal = ResourceData::getDspTxt(
                             msg->getFieldInt(MsgSp::Field::TX_PARTY),
                             CmnTypes::fromMsgSpIdentityType(
                                msg->getFieldInt(MsgSp::Field::TX_PARTY_TYPE)));
            if (msg->getFieldInt(MsgSp::Field::COMM_TYPE) ==
                MsgSp::Value::COMM_TYPE_BROADCAST)
            {
                call = getCallWindow(BROADCAST_SSI, false);
                if (call != 0)
                {
                    call->setConnected(strVal);
                    call->setConnected(isCallOwner(*msg),
                                       isCallPttAllowed(*msg),
                                       msg->getFieldInt(MsgSp::Field::TX_GRANT));
                }
                break;
            }
            btn = getCallButton(msg->getFieldInt(MsgSp::Field::CALL_ID));
            if (btn != 0)
            {
                btn->setConnected(strVal,
                                 msg->getFieldInt(MsgSp::Field::CALL_PRIORITY));
                btn->getCallWindow()->setConnected(isCallOwner(*msg),
                                      isCallPttAllowed(*msg),
                                      msg->getFieldInt(MsgSp::Field::TX_GRANT));
                Contact::callEvent(msg->getType(), btn->getCallWindow());
            }
            break;
        }
        case MsgSp::Type::CALL_INFO:
        {
            callId = msg->getFieldInt(MsgSp::Field::CALL_ID);
            val = msg->getFieldInt(MsgSp::Field::NEW_CALL_ID);
            if (val != MsgSp::Value::UNDEFINED)
            {
                btn = getCallButton(callId, true); //remove from map if found
                if (btn != 0)
                {
                    //monitored call or connected outgoing call
                    LOGGER_VERBOSE(mLogger, LOGPREFIX << msg->getName()
                                   << ": Changing Call-ID from " << callId
                                   << " to " << val);
                    call = btn->getCallWindow();
                    call->setCallId(val);
                    if (mCallMap.count(val) == 0)
                    {
                        mCallMap[val] = btn; //track with new call ID
                    }
                    else
                    {
                        //CallButton already exists with new call ID -
                        //overlapping group call MON-SETUP had arrived during an
                        //outgoing group call; delete call button and window for
                        //the monitored call and keep the outgoing call
                        CallButton *btn2 = mCallMap[val];
                        if (btn2->getCallWindow()->isOutgoingCall())
                            btn2 = btn;
                        else
                            mCallMap[val] = btn;
                        strVal = btn2->getCallWindow()->getCallingPartyName();
                        LOGGER_DEBUG(mLogger, LOGPREFIX << "onServerMsg:"
                                     << msg->getName()
                                     << ": Removing CallWindow with original"
                                        " call ID " << callId
                                     << ", calling party "
                                     << strVal.toStdString());
                        deleteCall(btn2, false);
                    }
                }
                callId = val;
            } //if (val != MsgSp::Value::UNDEFINED)
            if (mCallMap.count(callId) != 0 &&
                (msg->getFieldInt(MsgSp::Field::CALL_OWNERSHIP) ==
                     MsgSp::Value::CALL_OWNER_NO ||
                 msg->hasField(MsgSp::Field::CALLING_PARTY)))
            {
                //ownership change may occur mainly for group call
                mCallMap[callId]->changeOwnership(
                            QString::fromStdString(
                              msg->getFieldString(MsgSp::Field::CALLING_PARTY)),
                            msg->getFieldInt(MsgSp::Field::CALL_PRIORITY));
            }
            break;
        }
        case MsgSp::Type::CALL_PROCEEDING:
        {
            callId = msg->getFieldInt(MsgSp::Field::CALL_ID);
            if (msg->getFieldInt(MsgSp::Field::COMM_TYPE) ==
                MsgSp::Value::COMM_TYPE_BROADCAST)
            {
                call = getCallWindow(BROADCAST_SSI, false);
                if (call != 0)
                    call->setCallId(callId);
                break;
            }
            val = msg->getFieldInt(MsgSp::Field::CALLED_PARTY);
            call = getCallWindow(val, false);
            if (call != 0) //outgoing call
            {
                call->setCallId(callId);
                btn = new CallButton(call);
                removeCallWindow(val, false);
                if (call->isGrpCall())
                    mGrpFlowLayout->addWidget(btn);
                else
                    mIndFlowLayout->addWidget(btn);
                //if CallButton already exists with callId, these are
                //overlapping outgoing and monitored group calls - delete the
                //monitored
                if (mCallMap.count(callId) != 0)
                    deleteCall(mCallMap[callId], false);
                monHighlight(0, val, true);
                mCallMap[callId] = btn;
                Contact::callEvent(msg->getType(), call);
            }
            break;
        }
        case MsgSp::Type::CALL_RELEASE:
        case MsgSp::Type::MON_DISCONNECT:
        {
            callId = msg->getFieldInt(MsgSp::Field::CALL_ID);
            callerId = msg->getFieldInt(MsgSp::Field::CALLING_PARTY);
            if (callerId == MsgSp::Value::UNDEFINED)
            {
                callerId = 0;
            }
            else if (msg->getFieldInt(MsgSp::Field::DISCONNECT_CAUSE) ==
                     MsgSp::Value::DC_CALL_COMPLETED_ELSEWHERE)
            {
                //call answered by another dispatcher
                if (callId == 0 && mCallWindowMap.count(callerId) != 0)
                {
                    //mobile call - dismiss, call details to be captured upon
                    //internal MON_DISCONNECT from answering dispatcher
                    mCallWindowMap[callerId]->setNoRecord();
                    removeCallWindow(callerId, true);
                }
                //otherwise network call - do nothing, CallWindow handled with
                //MON_CONNECT
                break;
            }
            else if (msg->getType() == MsgSp::Type::MON_DISCONNECT &&
                     msg->getFieldInt(MsgSp::Field::CALLING_PARTY_TYPE) ==
                         MsgSp::Value::IDENTITY_TYPE_MOBILE &&
                     msg->getFieldInt(MsgSp::Field::CALLED_PARTY_TYPE) ==
                         MsgSp::Value::IDENTITY_TYPE_DISPATCHER)
            {
                //internal msg for incoming mobile call, from the answering disp
                mCall->getCommsRegister()->addCall(msg);
                break;
            }
            else if (removeCallWindow(callerId, true))
            {
                //call canceled unanswered
                LOGGER_DEBUG(mLogger, LOGPREFIX << "onServerMsg:"
                             << msg->getName() << ": Destroyed CallWindow for "
                             << callerId);
                break;
            }
            val = msg->getFieldInt(MsgSp::Field::CALLED_PARTY);
            if (val == MsgSp::Value::UNDEFINED)
                val = 0;
            if (callId == MsgSp::Value::UNDEFINED)
            {
                //failed outgoing call before getting call ID
                if (val != 0)
                {
                    //expect 2 CALL-RELEASE (direct and VOIP),
                    //must remove CallWindow from map first before handling
                    //failure to avoid processing the second message while error
                    //dialog is shown
                    call = getCallWindow(val, false);
                    if (call != 0)
                    {
                        removeCallWindow(val, false);
                        handleCallFailure(call, msg);
                        deleteCall(0, true, call);
                    }
                }
                break;
            }
            //CallButton for dispatcher call is tracked by negative call party
            if (callerId != 0)
                btn = getCallButton(-callerId, true);  //incoming dispatcher
            if (btn == 0)
            {
                if (val != 0)
                    btn = getCallButton(-val, true);   //outgoing dispatcher
                if (btn == 0 && callId != 0)
                    btn = getCallButton(callId, true); //other calls
            }
            if (btn != 0)
            {
                call = btn->getCallWindow();
                if (btn->isGrpCall())
                {
                    //complete last PTT duration, if applicable
                    call->clearTxParty();
                    if (call->checkPttOnCallRelease(
                               msg->getFieldInt(MsgSp::Field::TIME_IN_TRAFFIC)))
                    {
                        btn->releaseCallWindow(); //prevent deletion
                        delete btn;
                        mCallWindowMap[call->getCallParty()] = call;
                        Contact::callEvent(MsgSp::Type::CALL_RELEASE, call);
                        break;
                    }
                }
                if (val != 0)
                    handleCallFailure(call, msg);
                else if (msg->getType() == MsgSp::Type::MON_DISCONNECT)
                    call->setDuration(
                               msg->getFieldInt(MsgSp::Field::TIME_IN_TRAFFIC));
                deleteCall(btn, true, call);
            }
            else
            {
                call = getCallWindow(val, false);
                if (call != 0)
                {
                    //failed outgoing call with no call button because release:
                    //-from voip only, or
                    //-with call id but without earlier call-proceeding
                    removeCallWindow(val, false);
                    handleCallFailure(call, msg);
                    deleteCall(0, true, call);
                }
                else if (callId != 0)
                {
                    call = getCallWindow(BROADCAST_SSI, false);
                    if (call != 0 && call->getCallId() == callId)
                    {
                        //normal broadcast call release
                        removeCallWindow(BROADCAST_SSI, true);
                    }
                    else
                    {
                        //non-specific clean-up
                        removeCallWindow(callId);
                    }
                }
            }
            break;
        }
        case MsgSp::Type::CALL_SETUP:
        {
            callId = msg->getFieldInt(MsgSp::Field::CALL_ID);
            callerId = msg->getFieldInt(MsgSp::Field::CALLING_PARTY);
            val = CmnTypes::fromMsgSpIdentityType(
                            msg->getFieldInt(MsgSp::Field::CALLING_PARTY_TYPE));
            if (msg->hasField(MsgSp::Field::MSG_ID)) //network call
            {
#ifdef NOSIPSETUP
                if (val == CmnTypes::IDTYPE_SUBSCRIBER &&
                    !SubsData::validIssi(callerId))
                    break; //for another branch - ignore
                if (mCallWindowMap.count(callerId) != 0)
                {
                    //CallWindow already created for either:
                    //-outgoing call
                    //-VOIP call setup (abnormal scenario)
                    call = mCallWindowMap[callerId];
                    call->callSetup(callId,
                                  msg->getFieldInt(MsgSp::Field::CALL_PRIORITY),
                                  isCallHook(*msg), isCallDuplex(*msg),
                                  isCallTxGranted(*msg));
                }
                else
                {
                    call = new CallWindow(callId, callerId,
                                  msg->getFieldInt(MsgSp::Field::CALL_PRIORITY),
                                  isCallHook(*msg), isCallDuplex(*msg),
                                  isCallTxGranted(*msg), this);
                    connect(call, SIGNAL(callTimeout(int, int)),
                            SLOT(onCallTimeout(int, int)));
                    mCallWindowMap[callerId] = call;
                }
#else
                break; //ignore
#endif //NOSIPSETUP
            } //if (msg->hasField(MsgSp::Field::MSG_ID))
            //VOIP call setup
#ifdef NOSIPSETUP
            else if (mCallWindowMap.count(callerId) != 0)
            {
                //CallWindow already created for either:
                //-outgoing call
                //-server call setup
                call = mCallWindowMap[callerId];
                call->callSetup(msg->getFieldInt(MsgSp::Field::VOIP_RTP_PORT));
            }
            else
            {
                call = new CallWindow(callerId, val,
                           msg->getFieldInt(MsgSp::Field::VOIP_RTP_PORT), this);
                if (call->isInternalCall())
                    connect(call, SIGNAL(callRelease(int)),
                            SLOT(onCallRelease(int)));
                else
                    connect(call, SIGNAL(callTimeout(int, int)),
                            SLOT(onCallTimeout(int, int)));
                mCallWindowMap[callerId] = call;
            }
#else
            else
            {
                call = getCallWindow(callerId, true);
                if (call != 0)
                {
                    //either:
                    //-CallWindow already created for outgoing call which can be
                    // reused
                    //-connected dispatcher call has abnormally ended but
                    // undetected, delete so that can start a new one
                    btn = getCallButton(-callerId, true);
                    if (btn != 0)
                    {
                        deleteCall(btn, false, call);
                        call = 0;
                    }
                }
                if (call == 0)
                {
                    call = new CallWindow(callerId, val,
                                          msg->hasField(
                                             MsgSp::Field::VOIP_VIDEO_RTP_PORT),
                                          this);
                    if (call->isInternalCall())
                        connect(call, SIGNAL(callRelease(int)),
                                SLOT(onCallRelease(int)));
                    else
                        connect(call, SIGNAL(callTimeout(int,int)),
                                SLOT(onCallTimeout(int,int)));
                    mCallWindowMap[callerId] = call;
                }
                //nwk MsgSp contains Hook-Method
                if (msg->hasField(MsgSp::Field::HOOK_METHOD))
                    call->callSetup(callId,
                          msg->getFieldInt(MsgSp::Field::CALL_PRIORITY),
                          msg->getFieldInt(MsgSp::Field::VOIP_AUDIO_RTP_PORT),
                          msg->getFieldString(MsgSp::Field::VOIP_AUDIO_RTP_KEY),
                          isCallHook(*msg), isCallDuplex(*msg),
                          isCallTxGranted(*msg));
                else
                    call->callSetup(msg->getFieldInt(
                                             MsgSp::Field::VOIP_AUDIO_RTP_PORT),
                                    msg->getFieldInt(
                                             MsgSp::Field::VOIP_VIDEO_RTP_PORT),
                                    msg->getFieldInt(
                                                   MsgSp::Field::CALL_PRIORITY),
                                    msg->getFieldString(
                                              MsgSp::Field::VOIP_AUDIO_RTP_KEY),
                                    msg->getFieldString(
                                              MsgSp::Field::VOIP_VIDEO_RTP_KEY),
                                    msg->getFieldString(MsgSp::Field::CALL_ID));
                Contact::callEvent(msg->getType(), call);
            }
#endif //NOSIPSETUP
            connectCommonSignals(call, true, true);
            connect(call, SIGNAL(incomingConnected(int,int)),
                    SLOT(onCallIncomingConnected(int,int)),
                    Qt::UniqueConnection);
            if (!call->isDispatcherCall())
            {
                monHighlight(callerId, 0, true);
                if (mGisWindow != 0)
                    mGisWindow->setRscInCall(callerId, true);
            }
            break;
        }
        case MsgSp::Type::CALL_TX_CEASED:
        {
            call = getCall(msg->getFieldInt(MsgSp::Field::CALL_ID));
            if (call != 0)
            {
                if (mGisWindow != 0 && call->isGrpCall())
                    mGisWindow->setRscInCall(call->getTxParty(), false);
                call->txCeased(msg->hasField(MsgSp::Field::MSG_ACK),
                               isCallPttAllowed(*msg));
                Contact::callEvent(msg->getType(), call);
            }
            break;
        }
        case MsgSp::Type::CALL_TX_GRANTED:
        {
            call = getCall(msg->getFieldInt(MsgSp::Field::CALL_ID));
            if (call != 0)
            {
                callerId = msg->getFieldInt(MsgSp::Field::TX_PARTY);
                val = CmnTypes::fromMsgSpIdentityType(
                                 msg->getFieldInt(MsgSp::Field::TX_PARTY_TYPE));
                call->txGrant(msg->getFieldInt(MsgSp::Field::TX_GRANT),
                              isCallPttAllowed(*msg),
                              ResourceData::getDspTxt(callerId, val));
                Contact::callEvent(msg->getType(), call);
                if (call->isGrpCall())
                {
                    if (mGisWindow != 0 &&
                        val != ResourceData::TYPE_DISPATCHER &&
                        (msg->getFieldInt(MsgSp::Field::TX_GRANT) ==
                         MsgSp::Value::TRANSMISSION_GRANTED_TO_ANOTHER))
                        mGisWindow->setRscInCall(callerId, true);
                    if (ResourceData::isFullMode() &&
                        val == CmnTypes::IDTYPE_SUBSCRIBER)
                        grpUncAttach(callerId, call->getCalledParty());
                }
            }
            break;
        }
        case MsgSp::Type::CHANGE_PASSWORD:
        {
            QWidget *parent = mSettingsUi;
            if (!mSettingsUi->isVisible())
                parent = this;
            if (msg->isResultSuccessful())
            {
                LOGGER_INFO(mLogger, LOGPREFIX << msg->getName() << " for "
                            << mUserName.toStdString() << " successful.");
                QMessageBox::information(parent, tr("Password Change"),
                                       tr("Successful for %1.").arg(mUserName));
            }
            else
            {
                LOGGER_ERROR(mLogger, LOGPREFIX << msg->getName() << " for "
                             << mUserName.toStdString() << " failed, "
                             << msg->getFieldValueString(MsgSp::Field::RESULT));
                QMessageBox::critical(parent, tr("Password Change Error"),
                                      tr("Failed for %1,\n%2.")
                                          .arg(mUserName,
                                               QString::fromStdString(
                                                   msg->getFieldValueString(
                                                       MsgSp::Field::RESULT))));
            }
            break;
        }
        case MsgSp::Type::GPS_LOC:
        {
            if (mGisWindow != 0)
            {
                double lat = 0;
                double lon = 0;
                msg->getFieldVal(MsgSp::Field::LOCATION_LAT, lat);
                msg->getFieldVal(MsgSp::Field::LOCATION_LONG, lon);
                mGisWindow->terminalUpdate(
                              msg->getFieldInt(MsgSp::Field::LOCATION_VALID) !=
                                  MsgSp::Value::LOCATION_VALID_NO,
                              msg->getFieldInt(MsgSp::Field::CALLING_PARTY),
                              lon, lat,
                              msg->getFieldString(MsgSp::Field::LOCATION_TIME));
            }
            break;
        }
        case MsgSp::Type::GPS_MON_START:
        {
            if (GpsMonitor::result(*msg))
            {
                mSettingsUi->gpsMonChanged(GpsMonitor::getList());
                //MSG_ACK removed as special change indicator from mon all to
                //mon selection - remove unselected issis (issi list definitely
                //exists - use as exclude list)
                if (mGisWindow != 0 && !msg->hasField(MsgSp::Field::MSG_ACK))
                    rmvMapTerminals(false,
                                  msg->getFieldString(MsgSp::Field::ISSI_LIST));
            }
            break;
        }
        case MsgSp::Type::GPS_MON_STOP:
        {
            if (GpsMonitor::result(*msg))
            {
                mSettingsUi->gpsMonChanged(GpsMonitor::getList());
                if (GpsMonitor::isMonGrps() &&
                    !msg->hasField(MsgSp::Field::ISSI_LIST))
                {
                    //changed to mon grps - select attached members
                    auto *mdl = ResourceData::model(ui->monGrpList);
                    if (!mdl->empty())
                    {
                        ResourceData::IdsT gssis;
                        if (mdl->getIds(gssis))
                        {
                            strVal = GpsMonitor::monGrpsStart(gssis);
                            if (!strVal.isEmpty())
                                QMessageBox::critical(this,
                                            tr("GPS Monitoring Error"), strVal);
                        }
                    }
                }
                if (mGisWindow != 0)
                {
                    //if no issi list (stop all), remove all from map, else
                    //remove the stopped issis
                    if (msg->hasField(MsgSp::Field::MSG_ACK))
                        rmvMapTerminals(true,
                                  msg->getFieldString(MsgSp::Field::ISSI_LIST));
                    //MSG_ACK removed as special change indicator to mon grps -
                    //remove issis not attached to mon grps
                    else
                        rmvMapTerminals(false);
                }
            }
            break;
        }
#ifdef INCIDENT
        case MsgSp::Type::INCIDENT_LOCK:
        {
            val = msg->getFieldInt(MsgSp::Field::INCIDENT_ID);
            if (!msg->hasField(MsgSp::Field::MSG_ACK))
            {
                mIncident->setLockHolder(val,
                                (msg->hasField(MsgSp::Field::USERNAME))?
                                    msg->getFieldInt(MsgSp::Field::USERNAME): 0,
                                (msg->getFieldInt(MsgSp::Field::LOCK_ACTION) ==
                                 MsgSp::Value::LOCK_ACTION_UNLOCK));
            }
            else if (msg->isResultSuccessful())
            {
                if (!isHidden()) //hidden means logging out - nothing to do
                {
                    mIncident->editLock(val,
                                        (msg->getFieldInt(
                                                   MsgSp::Field::LOCK_ACTION) ==
                                         MsgSp::Value::LOCK_ACTION_LOCK));
                }
            }
            else if (msg->getFieldInt(MsgSp::Field::RESULT) ==
                     MsgSp::Value::RESULT_REJECTED)
            {
                QMessageBox::critical(this, tr("Incident Edit Error"),
                                      tr("Incident %1 is locked for editing by "
                                         "Dispatcher %2.")
                                          .arg(val)
                                          .arg(msg->getFieldInt(
                                                      MsgSp::Field::USERNAME)));
                mIncident->editLockRevert(false);
            }
            else if (msg->getFieldInt(MsgSp::Field::RESULT) ==
                     MsgSp::Value::RESULT_DB_LINK_ERROR)
            {
                QMessageBox::critical(this, tr("Incident Edit Error"),
                                      tr("Server unable to process the request "
                                         "because of database link error."));
                mIncident->editLockRevert(true);
            }
            break;
        }
        case MsgSp::Type::INCIDENT_UPDATE:
        {
            if (!msg->hasField(MsgSp::Field::MSG_ACK))
                mIncident->loadData(msg->getFieldInt(MsgSp::Field::INCIDENT_ID));
            break;
        }
#endif //INCIDENT
        case MsgSp::Type::LISTEN_CONNECT:
        {
            call = getCall(msg->getFieldInt(MsgSp::Field::CALL_ID));
            if (call != 0)
            {
                if (msg->isResultSuccessful())
                    call->setConnected(false, false,
                                       MsgSp::Value::TRANSMISSION_NOT_GRANTED);
                else
                    call->release(msg->getFieldInt(MsgSp::Field::RESULT));
            }
            break;
        }
        case MsgSp::Type::LISTEN_RELEASE:
        {
            call = getCall(msg->getFieldInt(MsgSp::Field::CALL_ID));
            if (call != 0)
                call->release();
            break;
        }
        case MsgSp::Type::LOGOUT:
        {
            QString title;
            switch (msg->getFieldInt(MsgSp::Field::RESULT))
            {
                case MsgSp::Value::RESULT_LIC_EXPIRED:
                    title = tr("License Expired");
                    strVal = tr("has expired");
                    break;
                case MsgSp::Value::RESULT_LIC_INVALID_MAC:
                    title = tr("License Updated");
                    strVal = tr("has been updated and this device is no longer "
                                "authorized");
                    break;
                default:
                    break; //do nothing
            }
            if (!title.isEmpty())
            {
                strVal.prepend(tr("WARNING: System license "))
                      .append(".\n").append(tr("Logging out in:"));
                connect(MessageDialog::showTimer(title, strVal,
                                                 tr("Logout Now"), windowIcon(),
                                                 10, this),
                        &QDialog::finished, this,
                        [this] {
                                   if (isVisible())
                                       ui->logoutButton->click();
                               });
            }
            break;
        }
        case MsgSp::Type::MMS_RPT:
        case MsgSp::Type::MMS_TRANSFER:
        {
            mSds->mmsRcv(msg);
            break;
        }
        case MsgSp::Type::MON_CONNECT:
        {
            if (msg->hasField(MsgSp::Field::TX_PARTY))
                strVal = ResourceData::getDspTxt(
                            msg->getFieldInt(MsgSp::Field::TX_PARTY),
                            CmnTypes::fromMsgSpIdentityType(
                                msg->getFieldInt(MsgSp::Field::TX_PARTY_TYPE)));
            callId = msg->getFieldInt(MsgSp::Field::CALL_ID);
            btn = getCallButton(callId);
            if (btn != 0)
            {
                //msg has CALLED_PARTY only for a monitored individual call to
                //dispatcher on another server
                int called = msg->getFieldInt(MsgSp::Field::CALLED_PARTY);
                if (called != MsgSp::Value::UNDEFINED)
                {
                    btn->setCalledParty(ResourceData::getClientDspTxt(called));
                }
                else
                {
                    btn->setConnected(strVal,
                                 msg->getFieldInt(MsgSp::Field::CALL_PRIORITY));
                    //for group call, setConnected() may be blocking because of
                    //dialog on max calls upon auto-join - at this point, call
                    //may have ended which means btn is destroyed, so check
                    //again
                    btn = getCallButton(callId);
                    if (btn == 0)
                        break;
                    //for grp call, add btn to layout now
                    if (btn->isGrpCall())
                        mGrpFlowLayout->addWidget(btn);
                }
                Contact::callEvent(msg->getType(), btn->getCallWindow());
                break;
            }
            val = msg->getFieldInt(MsgSp::Field::CALLING_PARTY);
            if (val != MsgSp::Value::UNDEFINED)
            {
                //incoming call answered by another client
                call = getCallWindow(val, false);
                if (call != 0)
                {
                    btn = new CallButton(call);
                    if (mGisWindow != 0)
                        mGisWindow->setRscInCall(val, false);
                    removeCallWindow(val, false);
                    mCallMap[callId] = btn;
                    mIndFlowLayout->addWidget(btn);
                    btn->setConnected(
                              ResourceData::getClientDspTxt(
                                  msg->getFieldInt(MsgSp::Field::CALLED_PARTY)),
                              msg->getFieldInt(MsgSp::Field::CALL_PRIORITY));
                    if (!strVal.isEmpty())
                        call->setTxParty(strVal);
                    Contact::callEvent(msg->getType(), call);
                }
            }
            break;
        }
        case MsgSp::Type::MON_GRP_ATTACH_DETACH:
        {
            //msg has list of groups affected by the change
            set<int> gssis;
            if (Utils::fromStringWithRange(
                        msg->getFieldString(MsgSp::Field::GRP_LIST), gssis) > 0)
            {
                for (auto i : gssis)
                {
                    mResources->setGrpActive(i);
                }
                if (mGisWindow != 0)
                    mGisWindow->terminalsFilterUpdate(gssis);
                Contact::setGrpActive(gssis);
                callerId = msg->getFieldInt(MsgSp::Field::ISSI);
                mResources->addGrpAttachData(false, callerId, gssis);
                if (GpsMonitor::isMonGrps())
                {
                    gssis.clear(); //get monitored grps
                    ResourceData::model(ui->monGrpList)->getIds(gssis);
                    GpsMonitor::monGrpsAttDet(callerId, gssis);
                }
            }
            break;
        }
        case MsgSp::Type::MON_INFO:
        {
            val = msg->getFieldInt(MsgSp::Field::NEW_CALL_ID);
            if (val != MsgSp::Value::UNDEFINED)
            {
                callId = msg->getFieldInt(MsgSp::Field::CALL_ID);
                btn = getCallButton(callId, true); //remove from map if found
                if (btn != 0)
                {
                    LOGGER_VERBOSE(mLogger, LOGPREFIX << msg->getName()
                                   << ": Changing monitored Call-ID from "
                                   << callId << " to " << val);
                    btn->getCallWindow()->setCallId(val);
                    if (mCallMap.count(val) == 0)
                    {
                        mCallMap[val] = btn;
                    }
                    else
                    {
                        //CallButton already exists with new call ID -
                        //overlapping group call MON-SETUP had arrived during an
                        //outgoing group call or a monitored call; keep only the
                        //outgoing call or the first monitored call
                        CallButton *btn2 = mCallMap[val];
                        if (!btn->getCallWindow()->isOutgoingCall())
                        {
                            btn2 = btn;
                            btn = mCallMap[val];
                        }
                        else
                        {
                            mCallMap[val] = btn;
                        }
                        strVal = btn2->getCallWindow()->getCallingPartyName();
                        btn->changeOwnership(strVal);
                        LOGGER_DEBUG(mLogger, LOGPREFIX << "onServerMsg:"
                                     << msg->getName()
                                     << ": Removing CallWindow with original"
                                        " call ID " << callId
                                     << ", calling party "
                                     << strVal.toStdString());
                        deleteCall(btn2, false);
                    }
                }
            }
            break;
        }
        case MsgSp::Type::MON_SDS:
        case MsgSp::Type::MON_STATUS:
        {
            if (ResourceData::isFullMode() &&
                msg->getFieldInt(MsgSp::Field::CALLING_PARTY_TYPE) ==
                    MsgSp::Value::IDENTITY_TYPE_ISSI &&
                msg->getFieldInt(MsgSp::Field::CALLED_PARTY_TYPE) ==
                    MsgSp::Value::IDENTITY_TYPE_GSSI)
                grpUncAttach(msg->getFieldInt(MsgSp::Field::CALLING_PARTY),
                             msg->getFieldInt(MsgSp::Field::CALLED_PARTY));
            //fallthrough
        }
        case MsgSp::Type::SDS_RPT:
        case MsgSp::Type::SDS_TRANSFER:
        case MsgSp::Type::STATUS:
        case MsgSp::Type::STATUS_RPT:
        {
            mSds->getCommsRegister()->onNewMsg(msg);
            break;
        }
        case MsgSp::Type::MON_SETUP:
        {
            bool doConnectOut = false;
            int callerType = CmnTypes::fromMsgSpIdentityType(
                                 msg->getFieldInt(
                                             MsgSp::Field::CALLING_PARTY_TYPE));
            callerId = msg->getFieldInt(MsgSp::Field::CALLING_PARTY);
            callId = msg->getFieldInt(MsgSp::Field::CALL_ID);
            btn = getCallButton(callId);
            if (btn != 0)
            {
                //this is a duplicate message for a network call from a client
                //on a peer server - use it only to change the calling party to
                //that client
                if (callerType == CmnTypes::IDTYPE_DISPATCHER)
                    btn->setCallingParty(callerType, callerId);
                break;
            }
            //find CallWindow that was opened before, if any
            val = msg->getFieldInt(MsgSp::Field::CALLED_PARTY);
            int callParty = val;
            call = getCallWindow(callParty, false);
            if (call == 0)
            {
                callParty = callerId;
                call = getCallWindow(callParty, false);
            }
            if (msg->getFieldInt(MsgSp::Field::CALLED_PARTY_TYPE) ==
                MsgSp::Value::IDENTITY_TYPE_GSSI)
            {
                if (call != 0 && call->isNotStarted())
                {
                    call->setIncomingGrp(isCallHook(*msg), isCallE2ee(*msg),
                                         callerId, callerType, val, callId);
                    btn = new CallButton(call);
                    removeCallWindow(callParty, false);
                    //if CallWindow also exists for the caller, destroy it
                    if (callParty != callerId)
                        removeCallWindow(callerId, true, true);
                }
                else
                {
                    //if CallWindow exists with an active call, it started as an
                    //outgoing group call but is now to be changed to monitored
                    //call - destroy it first
                    if (call != 0)
                    {
                        if (callerId == mUserName.toInt())
                            break; //own outgoing call - should not occur
                        removeCallWindow(callParty, true, true);
                    }
                    doConnectOut = (msg->getFieldInt(MsgSp::Field::COMM_TYPE) !=
                                    MsgSp::Value::COMM_TYPE_BROADCAST);
                    call = new CallWindow((doConnectOut)?
                                              CmnTypes::CALLTYPE_GROUP_IN:
                                              CmnTypes::CALLTYPE_BROADCAST_IN,
                                          callId, callerId, callerType, val,
                                          ResourceData::TYPE_GROUP,
                                          isCallHook(*msg), isCallE2ee(*msg),
                                          "", this);
                    btn = new CallButton(call);
                }
                //do not add btn to layout yet - wait for MON-CONNECT
                if (ResourceData::isFullMode() &&
                    callerType == CmnTypes::IDTYPE_SUBSCRIBER)
                    grpUncAttach(callerId, val);
            }
            else
            {
                if (call != 0)
                {
                    LOGGER_DEBUG(mLogger, LOGPREFIX << "onServerMsg:"
                                 << msg->getName()
                                 << ": Removing CallWindow for " << callParty
                                 << " because it is now in a new call.");
                    removeCallWindow(callParty, true, true);
                    MessageDialog::showStdMessage(tr("Call Party Busy"),
                                         tr("%1\n%2 is now in another call.\n"
                                            "Previous Call Window discarded.")
                                             .arg(QtUtils::getTimestamp())
                                             .arg(callParty),
                                         QMessageBox::Information, false, this);
                    if (callParty != callerId &&
                        getCallWindow(callerId, false) != 0)
                    {
                        LOGGER_DEBUG(mLogger, LOGPREFIX << "onServerMsg:"
                                     << msg->getName()
                                     << ": Removing CallWindow for " << callerId
                                     << " because it is now in a new call.");
                        removeCallWindow(callerId, true, true);
                        MessageDialog::showStdMessage(tr("Call Party Busy"),
                                         tr("%1\n%2 is now in another call.\n"
                                            "Previous Call Window discarded.")
                                             .arg(QtUtils::getTimestamp())
                                             .arg(callerId),
                                         QMessageBox::Information, false, this);
                    }
                }
                if (isCallDuplex(*msg))
                    val = CmnTypes::CALLTYPE_MON_IND_DUPLEX;
                else if (msg->getFieldInt(MsgSp::Field::COMM_TYPE) ==
                         MsgSp::Value::COMM_TYPE_AMBIENCE_LISTENING)
                    val = CmnTypes::CALLTYPE_MON_AMBIENCE;
                else
                    val = CmnTypes::CALLTYPE_MON_IND_PTT;
                call = new CallWindow(val, callId, callerId, callerType,
                                      msg->getFieldInt(
                                                    MsgSp::Field::CALLED_PARTY),
                                      ResourceData::TYPE_SUBSCRIBER,
                                      isCallHook(*msg), isCallE2ee(*msg), "",
                                      this);
                btn = new CallButton(call);
                mIndFlowLayout->addWidget(btn);
            }
            mCallMap[callId] = btn;
            monHighlight((callerType == CmnTypes::IDTYPE_DISPATCHER)?
                             0: callerId,
                         call->getCalledParty(), true);
            connectCommonSignals(call, doConnectOut, true);
            connect(call, SIGNAL(callTimeout(int,int)),
                    SLOT(onCallTimeout(int,int)), Qt::UniqueConnection);
            Contact::callEvent(msg->getType(), call);
            break;
        }
        case MsgSp::Type::MON_SSDGNA_DEASSIGN:
        case MsgSp::Type::SSDGNA_DEASSIGN:
        {
            if (msg->getFieldInt(MsgSp::Field::ASG_DEASSIGN_STATUS) ==
                MsgSp::Value::ASGD_PENDING_DEASSIGN)
                break;
            //fallthrough
        }
        case MsgSp::Type::MON_SSDGNA_ASSIGN:
        case MsgSp::Type::SSDGNA_ASSIGN:
        {
            mDgna->dgnaResult(msg);
            val = msg->getFieldInt(MsgSp::Field::GSSI);
            mResources->setGrpActive(val);
            Contact::setGrpActive(ResourceData::IdsT({val}));
            break;
        }
        case MsgSp::Type::MON_SSDGNA_DEFINE:
        case MsgSp::Type::MON_SUBS_DEFINE:
        {
            callerId = msg->getFieldInt(MsgSp::Field::SUBS_CONTENT_ID);
            val = msg->getFieldInt(MsgSp::Field::SUBS_CONTENT_TYPE);
            //nwk msg uses ROOT for mobile
            if (val == MsgSp::Value::SUBS_TYPE_ROOT &&
                msg->getType() == MsgSp::Type::MON_SUBS_DEFINE &&
                msg->getFieldInt(MsgSp::Field::SUBS_CONTAINER_TYPE) ==
                    MsgSp::Value::SUBS_TYPE_FLEET)
                val = CmnTypes::IDTYPE_MOBILE;
            else
                val = CmnTypes::fromMsgSpSubsType(val);
            mResources->addRemoveId(true, callerId, val);
            if (!msg->hasField(MsgSp::Field::SUBS_CONTAINER_TYPE) &&
                msg->hasField(MsgSp::Field::VOIP_GW) &&
                val == ResourceData::TYPE_MOBILE &&
                ResourceData::hasMobileStat())
            {
                //mobile login - also add to online list
                val = ResourceData::TYPE_MOBILE_ONLINE;
                mResources->addRemoveId(true, callerId, val);
            }
            //activate contact except for non-login mobile addition
            if (val != ResourceData::TYPE_MOBILE)
                Contact::activate(callerId, true);
            break;
        }
        case MsgSp::Type::MON_SSDGNA_DELETE:
        {
            callerId = msg->getFieldInt(MsgSp::Field::GSSI);
            mResources->addRemoveId(false, callerId,
                                    CmnTypes::fromMsgSpSubsType(
                                        msg->getFieldInt(
                                             MsgSp::Field::SUBS_CONTENT_TYPE)));
            Contact::activate(callerId, false);
            break;
        }
        case MsgSp::Type::MON_START:
        {
            if (mStatusIssiLbl->text().isEmpty())
            {
                //issi was not available during login - update now
                val = msg->getFieldInt(MsgSp::Field::ISSI);
                if (val > 0)
                {
                    mStatusIssiLbl->setText(QString::number(val));
#ifdef INCIDENT
                    mIncident->setServerIssi(val);
#endif
                }
            }
            bool ok = msg->isResultSuccessful();
            if (!msg->hasField(MsgSp::Field::SSI_LIST))
            {
                //server notif to restart monitoring that failed earlier
                if (ok)
                    Contact::monitorRestore();
                break;
            }
            set<int> ssis;
            Utils::fromStringWithRange(msg->getFieldString(
                                                        MsgSp::Field::SSI_LIST),
                                       ssis);
            for (auto i : ssis)
            {
                auto *ctc = Contact::get(i);
                if (ctc != 0)
                    ctc->setMonitored(ok);
            }
            val = (msg->getFieldInt(MsgSp::Field::AFFECTED_USER_TYPE) ==
                   MsgSp::Value::IDENTITY_TYPE_GSSI)? 0: 1;
            if (ok)
            {
                ResourceData::addIds(
                                ResourceData::model((val == 0)? ui->monGrpList:
                                                                ui->monIndList),
                                                    ssis);
                if (val == 0)
                {
                    if (mGisWindow != 0 &&
                        msg->hasField(MsgSp::Field::CALLING_PARTY))
                        mGisWindow->terminalsFilterUpdate(ssis);
                    strVal = GpsMonitor::monGrpsStart(ssis);
                    if (!strVal.isEmpty())
                        QMessageBox::critical(this,
                                            tr("GPS Monitoring Error"), strVal);
                }
            }
            else
            {
                LOGGER_ERROR(mLogger, LOGPREFIX << "onServerMsg:"
                             << msg->getName() << " failed, "
                             << msg->getFieldValueString(MsgSp::Field::RESULT));
                //if this is after disconnection and reconnection, the SSIs are
                //now in the monitoring lists and must be removed
                ResourceData::model((val == 0)? ui->monGrpList: ui->monIndList)
                    ->removeIds(ssis);
                QMessageBox::critical(this, tr("Monitoring Error"),
                          tr("Failed to start monitoring ") +
                              QString::fromStdString(
                                  msg->getFieldString(MsgSp::Field::SSI_LIST)));
            }
            break;
        }
        case MsgSp::Type::MON_STOP:
        {
            set<int> ssis;
            Utils::fromStringWithRange(
                             msg->getFieldString(MsgSp::Field::SSI_LIST), ssis);
            val = (msg->getFieldInt(MsgSp::Field::AFFECTED_USER_TYPE) ==
                   MsgSp::Value::IDENTITY_TYPE_GSSI)? 0: 1;
            ResourceData::model((val == 0)? ui->monGrpList: ui->monIndList)
                ->removeIds(ssis);
            if (val == 0)
            {
                if (mGisWindow != 0 && !ssis.empty() &&
                    msg->hasField(MsgSp::Field::CALLING_PARTY))
                    mGisWindow->terminalsFilterUpdate(ssis);
                if (GpsMonitor::isMonGrps())
                {
                    set<int> gssis; //monitored grps
                    ResourceData::model(ui->monGrpList)->getIds(gssis);
                    strVal = GpsMonitor::monGrpsStop(gssis, ssis);
                    if (!strVal.isEmpty())
                    {
                        QMessageBox::critical(this, tr("GPS Monitoring Error"),
                                              strVal);
                    }
                    //ssis are now issis not in monitored grps - remove from map
                    else if (mGisWindow != 0 && !ssis.empty())
                    {
                        LOGGER_DEBUG(mLogger, LOGPREFIX << "onServerMsg:"
                                     << msg->getName() << " GIS terminalRemove "
                                     << Utils::toStringWithRange(ssis));
                        mGisWindow->terminalRemove(true, ssis);
                    }
                }
            }
            break;
        }
        case MsgSp::Type::MON_SUBS_DESC:
        {
            //reload updated data into the list
            val = CmnTypes::fromMsgSpSubsType(msg->getFieldInt(
                                              MsgSp::Field::SUBS_CONTENT_TYPE));
            callerId = msg->getFieldInt(MsgSp::Field::SUBS_CONTENT_ID);
            mResources->reloadData(val, callerId);
            Contact::refresh(callerId);
            //terminal type may have changed - requires update on map
            if (mGisWindow != 0 &&
                (val == CmnTypes::IDTYPE_SUBSCRIBER ||
                 val == CmnTypes::IDTYPE_MOBILE))
                mGisWindow->terminalUpdate(callerId,
                                 msg->getFieldInt(MsgSp::Field::TERMINAL_TYPE));
            break;
        }
        case MsgSp::Type::MON_SUBS_PERMISSION:
        {
            if (msg->getFieldInt(MsgSp::Field::SUBS_PERMISSION) ==
                MsgSp::Value::SUBS_PERMISSION_GRANTED)
            {
                callerId = msg->getFieldInt(MsgSp::Field::SUBS_CONTENT_ID);
                mResources->addRemoveId(true, callerId,
                                        CmnTypes::fromMsgSpSubsType(
                                           msg->getFieldInt(
                                             MsgSp::Field::SUBS_CONTENT_TYPE)));
                Contact::activate(callerId, true);
                break;
            }
            //fallthrough
        }
        case MsgSp::Type::MON_SUBS_DELETE:
        {
            callerId = msg->getFieldInt(MsgSp::Field::SUBS_CONTENT_ID);
            val = msg->getFieldInt(MsgSp::Field::SUBS_CONTENT_TYPE);
            //nwk msg uses ROOT for mobile
            if (val == MsgSp::Value::SUBS_TYPE_ROOT &&
                msg->getType() == MsgSp::Type::MON_SUBS_DELETE &&
                msg->getFieldInt(MsgSp::Field::SUBS_CONTAINER_TYPE) ==
                    MsgSp::Value::SUBS_TYPE_FLEET)
                val = CmnTypes::IDTYPE_MOBILE;
            else
                val = CmnTypes::fromMsgSpSubsType(val);
            if (!msg->hasField(MsgSp::Field::SUBS_CONTAINER_TYPE) &&
                !msg->hasField(MsgSp::Field::FLEET) &&
                val == ResourceData::TYPE_MOBILE &&
                ResourceData::hasMobileStat())
            {
                //mobile logout - only remove from online list
                val = ResourceData::TYPE_MOBILE_ONLINE;
            }
            //skip next step for mobile unless msg is from nwk
            if (val != ResourceData::TYPE_MOBILE ||
                msg->hasField(MsgSp::Field::SUBS_CONTAINER_TYPE) ||
                msg->hasField(MsgSp::Field::FLEET))
            {
                mResources->addRemoveId(false, callerId, val);
                //deactivate contact except for non-logout mobile deletion
                if (val != ResourceData::TYPE_MOBILE)
                    Contact::activate(callerId, false);
            }
            if (msg->hasField(MsgSp::Field::GRP_LIST))
            {
                //GRP_LIST added by SubsData to show grp detachments
                set<int> gssis;
                if (Utils::fromStringWithRange(
                        msg->getFieldString(MsgSp::Field::GRP_LIST), gssis) > 0)
                    mResources->addGrpAttachData(true, callerId, gssis);
            }
            break;
        }
        case MsgSp::Type::MON_TX_CEASED:
        {
            call = getCall(msg->getFieldInt(MsgSp::Field::CALL_ID));
            if (call != 0)
            {
                call->clearTxParty();
                Contact::callEvent(msg->getType(), call);
            }
            break;
        }
        case MsgSp::Type::MON_TX_GRANTED:
        {
            call = getCall(msg->getFieldInt(MsgSp::Field::CALL_ID));
            if (call != 0)
            {
                callerId = msg->getFieldInt(MsgSp::Field::TX_PARTY);
                val = CmnTypes::fromMsgSpIdentityType(
                                 msg->getFieldInt(MsgSp::Field::TX_PARTY_TYPE));
                call->setTxParty(ResourceData::getDspTxt(callerId, val));
                Contact::callEvent(msg->getType(), call);
                if (ResourceData::isFullMode() &&
                    val == CmnTypes::IDTYPE_SUBSCRIBER && call->isGrpCall())
                    grpUncAttach(callerId, call->getCalledParty());
            }
            break;
        }
        case MsgSp::Type::PASSWORD:
        {
            if (msg->isResultSuccessful())
            {
                CallWindow::setPriorities(
                             msg->getFieldInt(MsgSp::Field::CALL_PRIORITY),
                             msg->getFieldInt(MsgSp::Field::MAX_CALL_PRIORITY));
                val = msg->getFieldInt(MsgSp::Field::MAX_MONITORING_GSSI);
                //reuse var
                callerId = msg->getFieldInt(MsgSp::Field::MAX_MONITORING_ISSI);
                Contact::setMonMax(val, callerId);
                ui->monGrpLabel->setToolTip(tr("Maximum %1").arg(val));
                ui->monIndLabel->setToolTip(tr("Maximum %1").arg(callerId));
                mSds->setMmsSvrPort(msg->getFieldInt(MsgSp::Field::PORT));
                //NETWORK_TYPE presence indicates STM-network, value is
                //irrelevant
                val = msg->getFieldInt(MsgSp::Field::NETWORK_TYPE);
                //if not in STM-network, VOIP_GW presence means svr does not
                //have mobile clients, which means not possible to see mobile
                //online status, so show them as active
                mResourceSelector->setMobileType(val >= 0 ||
                                         !msg->hasField(MsgSp::Field::VOIP_GW));
                //non-empty VOIP_SSRC is for failure only
                onLoginSuccess((val >= 0),
                          !msg->getFieldString(MsgSp::Field::VOIP_SSRC).empty(),
                          msg->getFieldString(MsgSp::Field::ISSI),
                          msg->getFieldString(MsgSp::Field::SUPPORTNUM),
                          msg->getFieldString(MsgSp::Field::ORGANIZATION));
            }
            else
            {
                onLoginFailure(msg);
            }
            break;
        }
        case MsgSp::Type::POI_DELETE:
        {
            if (mGisWindow != 0)
                mGisWindow->poiRemove(msg->getFieldInt(MsgSp::Field::ID));
            break;
        }
        case MsgSp::Type::POI_UPDATE:
        {
            if (mGisWindow != 0)
                mGisWindow->poiLoad(msg->getFieldInt(MsgSp::Field::ID));
            break;
        }
        case MsgSp::Type::REMOTE_SERVER_DISCONNECTED:
        case MsgSp::Type::REMOTE_SERVER_TIMEOUT:
        {
            LOGGER_ERROR(mLogger, LOGPREFIX << "onServerMsg:" << msg->getName());
            mSettingsUi->setSession(0);
            mStatusIconLbl->setPixmap(QPixmap(ICON_OFFLINE));
            mStatusLbl->setText(tr("Offline since %1 [trying to reconnect...]")
                                .arg(QtUtils::getTimestamp(true)));
            mStatusLbl->setStyleSheet(Style::getStyle(
                                              Style::OBJ_LABEL_STATUS_OFFLINE));
            mStatusIssiLbl->clear();
            removeCallWindows();
            //save monitoring only on timeout and if subs data already complete
            Contact::setSession(0,
                                (msg->getType() ==
                                     MsgSp::Type::REMOTE_SERVER_TIMEOUT &&
                                 SubsData::isReady()));
            break;
        }
        case MsgSp::Type::SSIC_INCL:
        {
            call = getCall(msg->getFieldInt(MsgSp::Field::CALL_ID));
            if (call != 0)
            {
                call->callInclude(true, isCallPttAllowed(*msg));
                Contact::callEvent(msg->getType(), call);
                if (mGisWindow != 0 &&
                    (msg->getFieldInt(MsgSp::Field::TX_PARTY_TYPE) !=
                     MsgSp::Value::IDENTITY_TYPE_DISPATCHER))
                    mGisWindow->setRscInCall(
                                msg->getFieldInt(MsgSp::Field::TX_PARTY), true);
            }
            break;
        }
        case MsgSp::Type::SSIC_INVOCATION_FAILURE:
        {
            call = getCall(msg->getFieldInt(MsgSp::Field::CALL_ID));
            if (call != 0)
                call->callInclude(false, false);
            break;
        }
        case MsgSp::Type::SSIC_RELEASE:
        {
            call = getCall(msg->getFieldInt(MsgSp::Field::CALL_ID));
            if (call != 0)
            {
                if (mGisWindow != 0)
                    mGisWindow->setRscInCall(call->getTxParty(), false);
                call->release();
                Contact::callEvent(msg->getType(), call);
            }
            break;
        }
        case MsgSp::Type::STATUS_DATA:
        {
            mSds->updateStsData(msg);
            break;
        }
        case MsgSp::Type::SUBS_DATA:
        {
            strVal = mStatusLbl->text();
            val = strVal.indexOf(" [");
            if (msg->isResultSuccessful())
            {
                LOGGER_INFO(mLogger, LOGPREFIX
                            << "Subscriber data download successful");
                if (val > 0)
                {
                    strVal.truncate(val);
                    mStatusLbl->setText(strVal);
                    mStatusLbl->setStyleSheet(
                                       Style::getStyle(Style::OBJ_LABEL_WHITE));
                }
                onReady(true);
                SubsData::BranchMapT branches;
                if (SubsData::getBranchData(branches))
                    mSettingsUi->setAllowedBranches(branches);
                break;
            }
            callId = msg->getFieldInt(MsgSp::Field::RESULT); //reuse var
            if (callId == MsgSp::Value::RESULT_NOT_AUTHORIZED)
            {
                LOGGER_ERROR(mLogger, LOGPREFIX << "onServerMsg:"
                             << msg->getName()
                             << ": Subscriber data download failed, "
                             << msg->getFieldValueString(MsgSp::Field::RESULT));
                QMessageBox::critical(this, tr("Subscriber Data Error"),
                                      tr("User %1 not authorized.\n"
                                         "Please contact Help Desk.")
                                          .arg(mUserName));
                ui->logoutButton->click();
            }
            else
            {
                if (val < 0)
                {
                    strVal.append(tr(" [getting subscriber data...]"));
                    mStatusLbl->setText(strVal);
                    mStatusLbl->setStyleSheet(Style::getStyle(
                                        Style::OBJ_LABEL_STATUS_ONLINE_NODATA));
                }
                if (callId != MsgSp::Value::RESULT_RETRY)
                    onReady(false);
            }
            break;
        }
        case MsgSp::Type::VERSION_CLIENT:
        {
            doUpdate(msg);
            break;
        }
        case MsgSp::Type::VERSION_MAP:
        {
            if (GisWindow::setMapInfo(
                           QString::fromStdString(
                                    msg->getFieldString(MsgSp::Field::VERSION)),
                           QString::fromStdString(
                               msg->getFieldString(MsgSp::Field::FILE_PATH))) &&
                mGisWindow != 0)
                mGisWindow->refreshMap();
            break;
        }
        case MsgSp::Type::VOIP_SERVER_REGISTERED:
        {
            mStatusIconLbl->setPixmap(
                  QPixmap((mSession->isLoggedIn())? ICON_ONLINE: ICON_OFFLINE));
            break;
        }
        case MsgSp::Type::VOIP_SERVER_UNREGISTERED:
        {
            if (mSession->isLoggedIn())
                mStatusIconLbl->setPixmap(QPixmap(ICON_VOIP_OFF));
            break;
        }
        default:
        {
            LOGGER_WARNING(mLogger, LOGPREFIX << "onServerMsg:"
                           << msg->getName() << ": Unhandled");
            break;
        }
    }
    delete msg;
}

void MainWindow::onStartAction(int                       actType,
                               int                       idType,
                               const ResourceData::IdsT &ids)
{
    if (actType == CmnTypes::ACTIONTYPE_CONTACT_ADD)
    {
        addContacts(idType, false, ids); //no need to validate
        return;
    }
    ResourceData::IdsT validIds(ids);
    if (!validate(idType, validIds))
        return;
    switch (actType)
    {
        case CmnTypes::ACTIONTYPE_CALL:
            mResourceSelector->setSelectedId(idType, validIds, false);
            mCall->openCallWindow();
            break;
        case CmnTypes::ACTIONTYPE_LOCATE:
            mGisWindow->onTerminalLocate(*validIds.begin());
            break;
        case CmnTypes::ACTIONTYPE_MMS:
        case CmnTypes::ACTIONTYPE_SDS:
        case CmnTypes::ACTIONTYPE_STATUS:
            mSds->startAction(actType);
            mSds->sendMsg(idType, ids, QStringList());
            ui->sdsButton->click();
            break;
        case CmnTypes::ACTIONTYPE_MON:
            addContacts(idType, true, validIds);
            break;
        case CmnTypes::ACTIONTYPE_MONSTART:
            Contact::monitor(ResourceData::isGrp(idType), true, validIds);
            break;
        case CmnTypes::ACTIONTYPE_MONSTOP:
            Contact::monitor(ResourceData::isGrp(idType), false, validIds);
            break;
        default:
            break; //do nothing
    }
}

void MainWindow::exitApp()
{
    qApp->exit(0);
}

void MainWindow::showMonContextMenu(const QPoint &pos)
{
    auto *lv = static_cast<QListView *>(QObject::sender());
    if (lv == 0)
        return;
    auto idx(lv->indexAt(pos));
    if (!idx.isValid())
        return;
    QList<int> actTypes;
    actTypes << CmnTypes::ACTIONTYPE_MONSTOP;
    int id = 0; //init just to silence compiler warning
    bool single = (lv->selectionModel()->selectedIndexes().count() == 1);
    if (single)
    {
        id = ResourceData::getItemId(lv, idx.row());
        actTypes << CmnTypes::ACTIONTYPE_CONTACT_SHOW
                 << CmnTypes::ACTIONTYPE_CALL;
    }
    bool isGrp = (lv == ui->monGrpList);
    //no sds/status for multiple groups because of mixed group types
    if (single || !isGrp)
    {
        //no sds/status for single mobile bwc
        if (!single || ResourceData::getType(id) != ResourceData::TYPE_MOBILE ||
            SubsData::getIssiType(id) != SubsData::TERMINALTYPE_BWC)
            actTypes << CmnTypes::ACTIONTYPE_SDS << CmnTypes::ACTIONTYPE_STATUS;
        if (single && !isGrp)
            actTypes << CmnTypes::ACTIONTYPE_LOCATE;
    }
    actTypes << CmnTypes::ACTIONTYPE_COPY;
    QMenu menu(this);
    QtUtils::addMenuActions(menu, actTypes);
    auto *act = menu.exec(lv->mapToGlobal(pos));
    if (act == 0)
        return;
    int actType = act->data().toInt();
    switch (actType)
    {
        case CmnTypes::ACTIONTYPE_CALL:
        case CmnTypes::ACTIONTYPE_SDS:
        case CmnTypes::ACTIONTYPE_STATUS:
        {
            int rscType = ResourceData::model(lv)->type();
            ResourceData::IdsT ids;
            if (single)
            {
                ids.insert(id);
                if (isGrp || rscType == ResourceData::TYPE_SUBS_OR_MOBILE)
                    rscType = ResourceData::getType(id); //exact type
            }
            else
            {
                for (auto &idx : lv->selectionModel()->selectedIndexes())
                {
                    ids.insert(ResourceData::getItemId(lv, idx.row()));
                }
            }
            onStartAction(actType, rscType, ids);
            break;
        }
        case CmnTypes::ACTIONTYPE_CONTACT_SHOW:
        {
            ui->contactsButton->click();
            Contact::show(id);
            break;
        }
        case CmnTypes::ACTIONTYPE_COPY:
        {
            QtUtils::copyToClipboard(lv);
            break;
        }
        case CmnTypes::ACTIONTYPE_LOCATE:
        {
            mGisWindow->onTerminalLocate(id);
            break;
        }
        case CmnTypes::ACTIONTYPE_MONSTOP:
        {
            auto *mdl = ResourceData::model(lv);
            ServerSession::SsiSetT ssis;
            for (auto &idx : lv->selectionModel()->selectedIndexes())
            {
                ssis.insert(ResourceData::getItemId(mdl, idx.row()));
            }
            Contact::monitor(isGrp, false, ssis);
            break;
        }
        default:
        {
            break; //do nothing
        }
    }
}

void MainWindow::onUpdaterFinished(bool doExit)
{
    Updater::destroy();
    if (doExit)
    {
        logout(true);
        exitApp();
    }
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    auto *lv = dynamic_cast<QListView *>(obj);
    if (lv != 0)
    {
        if (event->type() == QEvent::DragEnter)
        {
            auto *e = static_cast<QDragEnterEvent *>(event);
            if (e->mimeData()->hasFormat(Draggable::MIMETYPE))
            {
                QDataStream dataStream(e->mimeData()->data(Draggable::MIMETYPE));
                int idType;
                dataStream >> idType;
                if (isDragEventAllowed(lv, idType))
                {
                    e->acceptProposedAction();
                    return true;
                }
            }
            return false;
        }
        if (event->type() == QEvent::Drop)
        {
            auto *e = static_cast<QDropEvent *>(event);
            if (e->mimeData()->hasFormat(Draggable::MIMETYPE))
            {
                QDataStream dataStream(e->mimeData()->data(Draggable::MIMETYPE));
                int idType;
                dataStream >> idType;
                if (isDragEventAllowed(lv, idType))
                {
                    int id;
                    ResourceData::IdsT ids;
                    while (dataStream.status() == QDataStream::Ok)
                    {
                        dataStream >> id;
                        if (id != 0)
                            ids.insert(id);
                    }
                    if (!ids.empty() && validate(idType, ids))
                        addContacts(idType, true, ids);
                    return true;
                }
            }
            return false;
        }
    }
    else
    {
        bool isDone;
        Contact::ContactsT ctcs;
        if (Contact::doEventFilter(qobject_cast<QWidget *>(obj), event, isDone,
                                   ctcs))
        {
            for (auto &ctc : ctcs)
            {
                connect(ctc, SIGNAL(newCall(int,int,bool)),
                        SLOT(onNewCall(int,int,bool)), Qt::UniqueConnection);
                connect(ctc, SIGNAL(startAction(int,int,ResourceData::IdsT)),
                        SLOT(onStartAction(int,int,ResourceData::IdsT)),
                        Qt::UniqueConnection);
            }
            return isDone;
        }
    }
    return QMainWindow::eventFilter(obj, event);
}

void MainWindow::mdiAdd(int index, QWidget *widget)
{
    //index is from eSubWindow
    assert(widget != 0);
    auto *sub = ui->mdiArea->addSubWindow(widget,
                             Qt::WindowTitleHint | Qt::WindowMinMaxButtonsHint);
    sub->setStyle(QStyleFactory::create("Fusion"));
    sub->showMaximized();
    mMdiSubs[index] = sub;
}

void MainWindow::mdiActivate(int index)
{
    auto *sub = mMdiSubs[index];
    if (sub->isMinimized())
        sub->showNormal();
    if (ui->mdiArea->activeSubWindow() == sub)
        getMdiBtn(index)->setChecked(true); //restore
    else
        ui->mdiArea->setActiveSubWindow(sub);
}

inline QToolButton *MainWindow::getMdiBtn(int index)
{
    //index is from eSubWindow
    switch (index)
    {
        case SUBW_CALL:
            return ui->callButton;
        case SUBW_CONTACTS:
            return ui->contactsButton;
        case SUBW_INCIDENT:
            return ui->incidentButton;
        case SUBW_REPORT:
            return ui->reportButton;
        case SUBW_RESOURCES:
            return ui->resourcesButton;
        case SUBW_SDS:
        default:
            return ui->sdsButton;
    }
}

void MainWindow::onLoginSuccess(bool          stmNwk,
                                bool          voipFailure,
                                const string &issi,
                                const string &helpDeskNum,
                                const string &org)
{
    if (mLogin != 0)
    {
        mLoginFailCount = 0; //reset
        mLogin->onLoginSuccess(voipFailure, helpDeskNum);
    }
    if (isHidden())
    {
        //STM-nwk mode cannot be toggled in subsequent logins
        ResourceData::setFullMode(!stmNwk);
        mResources->init(!stmNwk);
        mResourceSelector->init(!stmNwk);
    }
    mSettingsUi->setSession(mSession);
    mSettingsUi->setOrganization(QString::fromStdString(org));
    Settings &cfg(Settings::instance());
    string num(helpDeskNum);
    if (!num.empty())
        cfg.set(Props::FLD_CFG_HELPDESKNUM, num);
    else
        num = cfg.get<string>(Props::FLD_CFG_HELPDESKNUM);
    ui->helpDeskLabel->setText(QString::fromStdString(num));
    ui->helpDeskLabel->setStyleSheet(Style::getStyle(Style::OBJ_LABEL_WHITE));
    GpsMonitor::init(mLogger, mSession,
                     cfg.get<string>(Props::FLD_CFG_GPS_MON));
    QString str(ResourceData::getClientDspTxt(mUserName.toInt()));
    mCall->getCommsRegister()->setUser(str);
    mCall->setBroadcastPermission(SubsData::getFleet() == SubsData::FLEET_NONE);
    mSds->setSession(mSession, str);
    CallWindow::setSession(mSession, str);
    mDgna->setSession(mSession);
    Contact::setUsername(mUserName.toStdString());
    Contact::setSession(mSession);
    if (mPoi != 0)
        mPoi->setSession(mSession);
    mStatusIconLbl->setPixmap(QPixmap((mSession->isVoipRegistered())?
                                      ICON_ONLINE: ICON_VOIP_OFF));
    mStatusLbl->setText(tr("%1 since %2")
                        .arg(QString::fromStdString(
                                                  mSession->getServerAddress()),
                             QtUtils::getTimestamp(true)));
    mStatusLbl->setStyleSheet(Style::getStyle(Style::OBJ_LABEL_WHITE));
    mStatusIssiLbl->setText(QString::fromStdString(issi));
    mStatusIssiLbl->setStyleSheet(Style::getStyle(Style::OBJ_LABEL_WHITE));
    if (voipFailure)
        CallWindow::showFailureMsg(this);
#ifdef INCIDENT
    mIncident->init(mSession);
    mIncident->setServerIssi(Utils::fromString<int>(issi));
    mIncident->retrieveData(mUserName); //must be done before DbInt check
#endif
#ifndef NO_DB
    //retrieve here because require database connection
    if (DbInt::instance().isValid())
        mReport->retrieveData();
    else
        QMessageBox::critical(this, tr("Database Error"),
                              tr("Connection failed.\nSome functions may "
                                 "not work.") +
                              "\nPQerrorMessage " +
                              QString::fromStdString(DbInt::instance()
                                  .getConnError()));
#endif //NO_DB
    if (mGisWindow != 0 && mGisWindow->getUserName() == mUserName)
        mGisWindow->refreshData();
    Updater::setLogout(false);
}

void MainWindow::onLoginFailure(MsgSp *msg)
{
    assert(msg != 0);
    QString errMsg;
    switch (msg->getFieldInt(MsgSp::Field::RESULT))
    {
        case MsgSp::Value::RESULT_ALREADY_LOGGED_IN:
            errMsg = tr("User %1 already logged in")
                      .arg(msg->getFieldInt(MsgSp::Field::USERNAME));
            break;
#ifdef DEBUG
        case MsgSp::Value::RESULT_INVALID_PASSWORD:
            ++mLoginFailCount;
            errMsg = "Incorrect Password";
            break;
        case MsgSp::Value::RESULT_INVALID_USER:
            ++mLoginFailCount;
            errMsg = "Invalid ID";
            break;
#else
        case MsgSp::Value::RESULT_INVALID_PASSWORD:
        case MsgSp::Value::RESULT_INVALID_USER:
            ++mLoginFailCount;
            errMsg = tr("Incorrect ID or Password");
            break;
#endif
        case MsgSp::Value::RESULT_LIC_EXPIRED:
            errMsg = tr("License has expired");
            break;
        case MsgSp::Value::RESULT_LIC_INVALID_MAC:
            errMsg = tr("Unauthorized device");
            break;
        case MsgSp::Value::RESULT_LIC_MAX:
            errMsg = tr("Maximum number of users reached");
            break;
        default:
            //just show result string to avoid empty dialog
            errMsg = QString::fromStdString(
                             msg->getFieldValueString(MsgSp::Field::RESULT));
            break;
    } //switch
    //active timer indicates from onServerMsg() - lock acquired
    deleteSession(mSession, mMsgTimer.isActive());
    QMessageBox::critical(this, tr("Login Failure"), errMsg);
    if (mLoginFailCount >= MAX_LOGIN_FAILS)
    {
        logout(true);
        LOGGER_ERROR(mLogger, LOGPREFIX
                     << "Too many failed login attempts. Exiting.");
        QApplication::exit();
    }
    ui->logoutButton->click();
}

void MainWindow::onReady(bool isSubsDataReady)
{
    mSds->loadData();
    mResources->loadData(mUserName);
#ifdef INCIDENT
    mIncident->setUserName(mUserName);
#endif
    if (isSubsDataReady)
    {
        Contact::ContactsT ctcs;
        if (Contact::dataRestore(ctcs))
        {
            for (auto &ctc : ctcs)
            {
                connect(ctc, SIGNAL(newCall(int,int,bool)),
                        SLOT(onNewCall(int,int,bool)), Qt::UniqueConnection);
                connect(ctc, SIGNAL(startAction(int,int,ResourceData::IdsT)),
                        SLOT(onStartAction(int,int,ResourceData::IdsT)),
                        Qt::UniqueConnection);
            }
        }
    }
    if (isHidden())
    {
        delete mLogin;
        mLogin = 0;
        showNormal();
        if (!Contact::empty())
            ui->contactsButton->click();
        else
            ui->resourcesButton->click();
    }
}

void MainWindow::connectCommonSignals(CallWindow *cw,
                                      bool        activeOut,
                                      bool        activeIn)
{
    assert(cw != 0);
    if (activeOut)
        connect(cw, SIGNAL(activeOutCall(CallWindow*)),
                SLOT(onActiveOutCall(CallWindow*)), Qt::UniqueConnection);
    if (activeIn)
        connect(cw, SIGNAL(activeInCall(int,int)),
                SLOT(onActiveInCall(int,int)), Qt::UniqueConnection);
    connect(cw,
            SIGNAL(callData(int,int,bool,QString,QString,QString,QString,
                            QString,MessageDialog::TableDataT)),
            mCall->getCommsRegister(),
            SLOT(onNewCall(int,int,bool,QString,QString,QString,QString,QString,
                           MessageDialog::TableDataT)),
            Qt::UniqueConnection);
}

void MainWindow::showGpsMon(QWidget *parent)
{
    if (mSession == 0 || !mSession->isLoggedIn())
    {
        QMessageBox::information(this, tr("GPS Monitoring"),
                                 tr("Function disabled while offline."));
        return;
    }
    auto *mdl = ResourceData::getModel(ResourceData::TYPE_SUBSCRIBER);
    if (mdl == 0 || mdl->rowCount() == 0)
    {
        mdl = ResourceData::getModel(ResourceData::TYPE_MOBILE);
        if (mdl == 0 || mdl->rowCount() == 0)
        {
            QMessageBox::information(this, tr("GPS Monitoring"),
                                     tr("Subscriber data not yet ready."));
            return;
        }
    }
    //no risk of multiple instances from SettingsUi & GisWindow because
    //GpsMonitor is modal
    auto *d = new GpsMonitor(parent);
    connect(d, &GpsMonitor::listChanged, this,
            [this](const string &s) { mSettingsUi->gpsMonChanged(s); });
    connect(d, &GpsMonitor::locateResource, this,
            [this](int i) { mGisWindow->onTerminalLocate(i); });
}

CallButton *MainWindow::getCallButton(int callId, bool doErase)
{
    if (mCallMap.count(callId) == 0)
        return 0;
    if (!doErase)
        return mCallMap[callId];
    CallButton *btn = mCallMap[callId];
    mCallMap.erase(callId);
    return btn;
}

inline CallWindow *MainWindow::getCall(int callId)
{
    if (mCallMap.count(callId) != 0)
        return mCallMap[callId]->getCallWindow();
    return 0;
}

inline CallWindow *MainWindow::getCallWindow(int           ssi,
                                             bool          checkCallButtons,
                                             const string &voipId)
{
    if (mCallWindowMap.count(ssi) != 0)
        return mCallWindowMap[ssi];
    if (checkCallButtons)
    {
        CallWindow *cw;
        for (auto &it : mCallMap)
        {
            cw = it.second->getCallWindow();
            if (cw->hasCallParty(ssi) &&
                (voipId.empty() || cw->hasVoipId(voipId)))
                return cw;
        }
    }
    return 0;
}

void MainWindow::removeCallWindows()
{
    for (auto &it : mCallMap)
    {
        it.second->getCallWindow()->end();
        deleteCall(it.second, false);
    }
    mCallMap.clear();
    for (auto &it : mCallWindowMap)
    {
        it.second->end();
        deleteCall(0, false, it.second);
    }
    mCallWindowMap.clear();
}

inline bool MainWindow::removeCallWindow(int ssi, bool doDelete, bool force)
{
    if (mCallWindowMap.count(ssi) == 0)
        return false;
    auto *cw = mCallWindowMap[ssi];
    mCallWindowMap.erase(ssi);
    if (doDelete)
        deleteCall(0, !force, cw, ssi);
    return true;
}

void MainWindow::removeCallWindow(int callId)
{
    for (auto &it : mCallWindowMap)
    {
        if (it.second->getCallId() == callId)
        {
            LOGGER_DEBUG(mLogger, LOGPREFIX << "removeCallWindow: "
                         << "Calling-party " << it.second->getCallingParty()
                         << " Call-ID " << callId);
            callId = it.first; //reuse for call party
            auto *cw = it.second;
            mCallWindowMap.erase(it.first);
            deleteCall(0, true, cw, callId);
            break;
        }
    }
}

void MainWindow::deleteCall(CallButton *btn,
                            bool        checkKeepOpen,
                            CallWindow *cw,
                            int         callParty)
{
    if (btn == 0 && cw == 0)
        return;
    if (cw == 0)
        cw = btn->getCallWindow();
    if (!cw->isDispatcherCall())
    {
        monHighlight(cw->getCallingParty(), cw->getCalledParty(), false);
        if (mGisWindow != 0)
            mGisWindow->setRscInCall((cw->isGrpCall())? cw->getTxParty():
                                                        cw->getCallingParty(),
                                     false);
    }
    Contact::callEvent(MsgSp::Type::CALL_RELEASE, cw);
    if (checkKeepOpen && cw->keepOpen())
    {
        if (btn != 0)
            btn->resetCallWindow();
        else
            cw->reset();
        mCallWindowMap[(callParty != 0)? callParty: cw->getCallParty()] = cw;
        cw = 0; //prevent delete
    }
    if (btn != 0)
        delete btn;
    else
        delete cw;
}

inline bool MainWindow::isCallDuplex(const MsgSp &msg)
{
    return (msg.getFieldInt(MsgSp::Field::SIMPLEX_DUPLEX) ==
            MsgSp::Value::SIMPLEX_DUPLEX_DUPLEX);
}

inline bool MainWindow::isCallE2ee(const MsgSp &msg)
{
    return (msg.getFieldInt(MsgSp::Field::ENCR_FLAG) ==
            MsgSp::Value::ENCR_FLAG_E2EE);
}

inline bool MainWindow::isCallHook(const MsgSp &msg)
{
    return (msg.getFieldInt(MsgSp::Field::HOOK_METHOD) ==
            MsgSp::Value::HOOK_YES);
}

inline bool MainWindow::isCallOwner(const MsgSp &msg)
{
    return (msg.getFieldInt(MsgSp::Field::CALL_OWNERSHIP) ==
            MsgSp::Value::CALL_OWNER_YES);
}

inline bool MainWindow::isCallPttAllowed(const MsgSp &msg)
{
    return (msg.getFieldInt(MsgSp::Field::TX_REQ_PERMISSION) ==
            MsgSp::Value::TX_REQ_PERM_ALLOWED_YES);
}

inline bool MainWindow::isCallTxGranted(const MsgSp &msg)
{
    return (msg.getFieldInt(MsgSp::Field::TX_GRANT) ==
            MsgSp::Value::TRANSMISSION_GRANTED);
}

inline void MainWindow::addContacts(int                       type,
                                    bool                      doMonitor,
                                    const ResourceData::IdsT &ids,
                                    const QString            &tabName)
{
    LOGGER_DEBUG(mLogger, LOGPREFIX << "addContacts: (type=" << type
                 << " monitor=" << doMonitor << " tab=" << tabName.toStdString()
                 << ") " << ResourceData::toString(ids));
    Contact::MonMapT idMap;
    for (auto i : ids)
    {
        idMap[i] = doMonitor;
    }
    addContacts(type, true, idMap, tabName);
}

void MainWindow::addContacts(int                     type,
                             bool                    userDefined,
                             const Contact::MonMapT &idMap,
                             const QString          &tabName)
{
    Contact::ContactsT ctcs;
    if (Contact::add(ctcs, type, userDefined, idMap, tabName))
    {
        for (auto &ctc : ctcs)
        {
            connect(ctc, SIGNAL(newCall(int,int,bool)),
                    SLOT(onNewCall(int,int,bool)), Qt::UniqueConnection);
            connect(ctc, SIGNAL(startAction(int,int,ResourceData::IdsT)),
                    SLOT(onStartAction(int,int,ResourceData::IdsT)),
                    Qt::UniqueConnection);
        }
    }
}

void MainWindow::monHighlight(int caller, int called, bool on)
{
    //highlight is green - taken from CallWindow::STYLE_BGCOLOR_ACTIVE
    QStandardItem *itm;
    if (caller > 0)
    {
        itm = ResourceData::getItem(ui->monIndList, caller);
        if (itm != 0)
            itm->setBackground((on)? QColor(57, 181, 74): Qt::transparent);
    }
    if (called > 0)
    {
        itm = ResourceData::getItem(ui->monGrpList, called);
        if (itm == 0)
            itm = ResourceData::getItem(ui->monIndList, called);
        if (itm != 0)
            itm->setBackground((on)? QColor(57, 181, 74): Qt::transparent);
    }
}

bool MainWindow::validate(int type, ResourceData::IdsT &ids)
{
    ResourceData::IdsT ids2(ids);
    QStringList l;
    bool retVal = ResourceData::validate(type, ids2, l);
    if (!l.isEmpty())
    {
        QString s(tr("ID no longer available: "));
        s.append(l.join(",")).append("\n")
         .append(tr("Please refresh the Resources list."));
        QMessageBox::critical(this, tr("Resource Error"), s);
    }
    return retVal;
}

void MainWindow::grpUncAttach(int issi, int gssi)
{
    if (SubsData::grpUncAttach(issi, gssi))
    {
        set<int> gssis({gssi});
        mResources->addGrpAttachData(false, issi, gssis);
        if (GpsMonitor::isMonGrps())
        {
            gssis.clear(); //now get monitored grps
            ResourceData::model(ui->monGrpList)->getIds(gssis);
            GpsMonitor::monGrpsAttDet(issi, gssis);
        }
    }
}

void MainWindow::rmvMapTerminals(bool rmList, const string &ssiList)
{
    assert(mGisWindow != 0);
    set<int> issis;
    if (!ssiList.empty())
    {
        LOGGER_DEBUG(mLogger, LOGPREFIX << "rmvMapTerminals: GIS terminalRemove "
                     << ((rmList)? "": "all excluding ") << ssiList);
        if (Utils::fromStringWithRange(ssiList, issis) > 0)
            mGisWindow->terminalRemove(rmList, issis);
        return;
    }
    if (rmList)
    {
        LOGGER_DEBUG(mLogger, LOGPREFIX
                     << "rmvMapTerminals: GIS terminalRemove all");
        mGisWindow->terminalRemove();
        return;
    }
    //remove those not attached to monitored grps
    auto *mdl = ResourceData::model(ui->monGrpList);
    if (!mdl->empty())
    {
        set<int> gssis;
        mdl->getIds(gssis);
        for (auto i : gssis)
        {
            SubsData::getGrpAttachedMembers(i, issis);
        }
    }
    if (issis.empty())
    {
        LOGGER_DEBUG(mLogger, LOGPREFIX
                     << "rmvMapTerminals: GIS terminalRemove all");
        mGisWindow->terminalRemove(); //no monitored grp - remove all
    }
    else
    {
        LOGGER_DEBUG(mLogger, LOGPREFIX
                     << "rmvMapTerminals: GIS terminalRemove all excluding "
                     << Utils::toStringWithRange(issis));
        mGisWindow->terminalRemove(false, issis);
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (QMessageBox::question(this, Version::APP_TITLE,
                              tr("Are you sure to exit?")) ==
        QMessageBox::Yes)
    {
        event->accept();
        logout(true);
    }
    else
    {
       event->ignore();
    }
}

bool MainWindow::isDragEventAllowed(QListView *lv, int idType)
{
    switch (idType)
    {
        case CmnTypes::IDTYPE_GROUP:
        case CmnTypes::IDTYPE_DGNA_IND:
        case CmnTypes::IDTYPE_DGNA_GRP:
            return (lv == ui->monGrpList);
        case CmnTypes::IDTYPE_MOBILE:
        case CmnTypes::IDTYPE_MOBILE_ONLINE:
        case CmnTypes::IDTYPE_SUBSCRIBER:
            return (lv == ui->monIndList);
        default:
            break; //do nothing
    }
    return false;
}

inline void MainWindow::handleCallFailure(CallWindow *cw, const MsgSp *msg)
{
    assert(msg != 0);
    if (cw != 0 && msg->getType() == MsgSp::Type::CALL_RELEASE)
        cw->releaseOutgoing(QString::fromStdString(
                     msg->getFieldValueString(MsgSp::Field::DISCONNECT_CAUSE)));
}

void MainWindow::logout(bool doExit)
{
    Updater::setLogout(true);
    if (mGisWindow != 0)
        mGisWindow->hide();
    hide();
    foreach(QDialog *dlg, findChildren<QDialog *>())
    {
        if (qobject_cast<MessageDialog *>(dlg) != 0)
            dlg->close();
    }
    mResources->deactivate(!doExit);
    removeCallWindows();
    //save contacts only if subs data already complete
    Contact::setSession(0, SubsData::isReady(), !doExit);
    if (mSession != 0)
    {
        mReport->handleLogout();
        Settings &cfg(Settings::instance());
        cfg.set(Props::FLD_CFG_SDSTEMPLATE,
                mSds->getTemplate().toStdString());
#ifdef INCIDENT
        mIncident->handleLogout();
        cfg.set(Props::FLD_CFG_INCFILTER_ADDRSTATE,
                IncidentButton::getFilter(IncidentButton::FIELD_ADDRSTATE))
           .set(Props::FLD_CFG_INCFILTER_CATEGORY,
                IncidentButton::getFilter(IncidentButton::FIELD_CATEGORY))
           .set(Props::FLD_CFG_INCFILTER_PRIORITY,
                IncidentButton::getFilter(IncidentButton::FIELD_PRIORITY))
           .set(Props::FLD_CFG_INCFILTER_STATE,
                IncidentButton::getFilter(IncidentButton::FIELD_STATE));
#endif
        mSettingsUi->save();
        deleteSession(mSession);
    }

    if (doExit)
    {
        if (mLogin != 0)
            mLogin->close();
        return;
    }

    ResourceData::model(ui->monIndList)->clear();
    ResourceData::model(ui->monGrpList)->clear();
    CallWindow::setSession(0, "");
    mDgna->setSession(0);
    mDgna->hide();
    mSds->setSession(0);
    mCall->getCommsRegister()->setUser("");
#ifdef INCIDENT
    mIncident->reset();
#endif
    if (mPoi != 0)
        mPoi->setSession(0);
    if (mGisWindow != 0)
        mGisWindow->clearData();

    if (mLogin == 0)
    {
        mLogin = new Login(mUserName, mSettingsUi, (mProc != 0));
        connect(mLogin, &Login::doLogin, this,
                [this](const QString &nm, const QString &pswd)
                {
                    //start server session
                    mUserName = nm;
                    ui->idLabel->setText(tr("User ID: ") + nm);
                    ui->idLabel
                      ->setStyleSheet(Style::getStyle(Style::OBJ_LABEL_TITLE));
                    if (mMsgTimer.interval() > 0)
                        mMsgTimer.start();
                    mSession = new ServerSession(nm.toStdString(),
                                             pswd.toStdString(),
                                             Settings::instance().get<string>(
                                                         Props::FLD_CFG_BRANCH),
                                             this, serverCallback);
                    if (!mSession->isValid())
                    {
                        LOGGER_ERROR(mLogger, LOGPREFIX
                                  << "doLogin handler: Server Session failed.");
                        QMessageBox::critical(this, tr("Server settings"),
                                              tr("Missing input."));
                        onLoginCancel();
                    }
                });
        connect(mLogin, SIGNAL(cancelLogin()), SLOT(onLoginCancel()));
        connect(mLogin, SIGNAL(lastWindowClosed()), SLOT(exitApp()));
        if (mProc != 0)
            connect(mLogin, &Login::showOsk, this,
                    [this] { ui->oskButton->click(); });
    }
    else
    {
        mLogin->enable();
    }
    mLogin->show();
}

void MainWindow::doUpdate(MsgSp *msg)
{
    assert(msg != 0 && msg->getType() == MsgSp::Type::VERSION_CLIENT);
    bool forced = (msg->getFieldInt(MsgSp::Field::RESULT) ==
                   MsgSp::Value::RESULT_INVALID_CLIENT);
    if (forced)
    {
        if (mLogin == 0)
        {
            logout(false);
        }
        else
        {
            //active timer indicates from onServerMsg() - lock acquired
            deleteSession(mSession, mMsgTimer.isActive());
        }
        mLogin->hide();
        if (QMessageBox::Yes !=
            QMessageBox::question(this, tr("Invalid Client"),
                                  tr("This client version %1 is no longer "
                                     "compatible with  the server.\n"
                                     "You must upgrade to version %2 to "
                                     "continue.\nUpgrade now?")
                                   .arg(Version::APP_VERSION,
                                        QString::fromStdString(
                                            msg->getFieldString(
                                                      MsgSp::Field::VERSION)))))
        {
            exitApp();
            return;  //still required
        }
    }
    Updater &up(Updater::instance(this));
    connect(&up, SIGNAL(finished(bool)), SLOT(onUpdaterFinished(bool)),
            Qt::UniqueConnection);
    up.updateApp(forced, msg);
}

void MainWindow::deleteSession(ServerSession *session, bool haveLock)
{
    QTimer::singleShot(0, this, [=] { delete session; });
    mSession = 0;
    if (mMsgTimer.isActive())
    {
        mMsgTimer.stop();
        if (!haveLock)
            PalLock::take(&mMsgQueueLock);
        while (!mMsgQueue.empty())
        {
            delete mMsgQueue.front();
            mMsgQueue.pop();
        }
        if (!haveLock)
            PalLock::release(&mMsgQueueLock);
    }
}
