/**
 * The CallWindow UI implementation.
 *
 * Copyright (C) Sapura Secured Technologies, 2014-2025. All Rights Reserved.
 *
 * @file
 * @version $Id: CallWindow.cpp 1903 2025-02-12 00:33:57Z zulzaidi $
 * @author Mazdiana Makmor
 */
#include <assert.h>
#include <QGraphicsPixmapItem>
#include <QMessageBox>
#include <QPixmap>
#include <QtConcurrent/QtConcurrent>

#include "CmnTypes.h"
#include "QtTableUtils.h"
#include "QtUtils.h"
#include "ResourceData.h"
#include "Settings.h"
#include "VideoDevice.h"
#include "ui_CallWindow.h"
#include "CallWindow.h"

using namespace std;

//radiobutton IDs
enum
{
    RBID_NO_HOOK,
    RBID_HOOK,
    RBID_AMBIENCE
};

static const int MAX_COUNT           = 3;
static const int PRIORITY_DEFAULT    = 3; //range 1-11
static const int PRIORITY_PREEMPTIVE = MsgSp::Value::CALL_PRIORITY_PREEMPTIVE_3;
static const int TIMEOUT_SETUP       = 6000; //milliseconds
static const int TIMEOUT_SETUP_HOOK  = 40000;
static const int TIMEOUT_ACTIVE      = 60000 * 30;
static const QString SOUND_FILE_BEEP(":/Sounds/sounds/sound_beep.wav");
static const QString SOUND_FILE_RBTONE(":/Sounds/sounds/sound_ringback.wav");
static const QString SOUND_FILE_RTONE(":/Sounds/sounds/sound_ringing.wav");

const QString CallWindow::STYLE_BGCOLOR_PENDING
                          ("background-color:rgb(255,210,0);padding-left:4px;"
                           "padding-right:4px;border-radius:5px");
const QString CallWindow::STYLE_BGCOLOR_ACTIVE
                          ("background-color:rgb(57,181,74);padding-left:4px;"
                           "padding-right:4px;border-radius:5px");
const QString CallWindow::STYLE_BGCOLOR_ACTIVE_PREEMPTIVE
                          ("background-color:rgb(247,160,0);padding-left:4px;"
                           "padding-right:4px;border-radius:5px");
const QString CallWindow::STYLE_BGCOLOR_ACTIVE_EMERGENCY
                          ("background-color: rgb(247,60,0)");
const QString CallWindow::STYLE_CALL_DETAILS_FRAME
                          ("background-color:rgb(65,65,66);color:white");

int CallWindow::sCount           = 0;
int CallWindow::sPriorityDefault = PRIORITY_DEFAULT;
int CallWindow::sPriorityPreempt = PRIORITY_PREEMPTIVE;
QString        CallWindow::sUserName;
ServerSession *CallWindow::sSession(0);
AudioManager  *CallWindow::sAudioMgr(0);
CallWindow    *CallWindow::sMicOwner(0);

CallWindow::CallWindow(int           type,
                       int           callId,
                       int           callingParty,
                       int           callingType,
                       int           calledParty,
                       int           calledType,
                       bool          isHook,
                       bool          isE2ee,
                       const string &calledDomain,
                       QWidget      *parent) :
QDialog(parent, Qt::WindowTitleHint), ui(new Ui::CallWindow), mType(type),
mCallId(callId), mCalledParty(calledParty), mCallingParty(callingParty),
mDuration(-1), mPriority(sPriorityDefault), mLocalAudRtpPort(0),
mRemoteAudRtpPort(0), mRemoteVidRtpPort(0),mOwner(false), mDuplex(false),
mE2ee(isE2ee), mFirstPtt(true), mTxGranted(false), mCalledDomain(calledDomain),
mCalledPartyName(ResourceData::getDspTxt(calledParty, calledType)), mRingTone(0),
mVideoStream(0)
{
    init();
    QPixmap pixmap(QtUtils::getRscIconSrc(CmnTypes::IDTYPE_GROUP));
    ui->txPartyLabel->hide();
    switch (type)
    {
        case CmnTypes::CALLTYPE_BROADCAST_IN:
        case CmnTypes::CALLTYPE_MON_AMBIENCE:
        case CmnTypes::CALLTYPE_MON_IND_DUPLEX:
        case CmnTypes::CALLTYPE_MON_IND_PTT:
            ui->startTimeLabel->setText(QtUtils::getTimestamp());
            ui->preemptiveCheck->setEnabled(false);
            ui->keepOpenCheck->hide();
            break;
        case CmnTypes::CALLTYPE_BROADCAST_OUT:
        case CmnTypes::CALLTYPE_DISPATCHER:
            ui->preemptiveCheck->hide();
            break;
        case CmnTypes::CALLTYPE_GROUP_IN:
            ui->startTimeLabel->setText(QtUtils::getTimestamp());
            ui->preemptiveCheck->setEnabled(false);
            break;
        case CmnTypes::CALLTYPE_GROUP_OUT:
        default:
            break; //do nothing
    }
    setCallingParty(callingType, callingParty);
    int timeout = 0;
    switch (type)
    {
        case CmnTypes::CALLTYPE_BROADCAST_IN:
            //do not show broadcast SSI
            mCalledPartyName = QString(tr("Broadcast"));
            //fallthrough
        case CmnTypes::CALLTYPE_GROUP_IN:
            setIncomingGrp(isHook, isE2ee, callingParty, callingType,
                           calledParty);
            timeout = TIMEOUT_SETUP;
            break;
        case CmnTypes::CALLTYPE_BROADCAST_OUT:
            mOwner = true;
            setWindowTitle(tr("Broadcast Call"));
            //do not show broadcast SSI
            mCalledPartyName = QString(tr("Broadcast"));
            ui->listenButton->hide();
            ui->callButton->hide();
            setPttIcon(CmnTypes::ACTIONTYPE_PTT);
            ui->txPartyLabel->setStyleSheet(STYLE_BGCOLOR_PENDING);
            setTxParty(mCallingPartyName);
            ui->noHook->setChecked(true);
            break;
        case CmnTypes::CALLTYPE_DISPATCHER:
        case CmnTypes::CALLTYPE_MOBILE:
            mDuplex = true;
            setWindowTitle(tr("Outgoing Call ") + mCallingPartyName + "->" +
                           mCalledPartyName);
            if (type == CmnTypes::CALLTYPE_MOBILE &&
                SubsData::getIssiType(mCalledParty) ==
                    SubsData::TERMINALTYPE_BWC)
            {
                ui->callButton->hide();
                ui->callButton->setCheckable(true); //just to mark for bwc
            }
            else
            {
                ui->callButton->setToolTip(tr("Call", "Button"));
            }
#ifndef NO_VIDEO
            ui->videoButton->show();
#endif
            ui->listenButton->hide();
            ui->pttButton->hide();
            pixmap.load(QtUtils::getRscIconSrc(
                         (type == CmnTypes::CALLTYPE_DISPATCHER)?
                         CmnTypes::IDTYPE_DISPATCHER: CmnTypes::IDTYPE_MOBILE));
            ui->txPartyIconLabel->setPixmap(pixmap);
            ui->hook->setChecked(true);
            break;
        case CmnTypes::CALLTYPE_GROUP_OUT:
            mOwner = true;
            setWindowTitle(tr("Outgoing Group Call ") + mCalledPartyName);
            ui->listenButton->hide();
            ui->callButton->hide();
            setPttIcon(CmnTypes::ACTIONTYPE_PTT);
            ui->noHook->setChecked(true);
            break;
        case CmnTypes::CALLTYPE_IND_OUT:
            setWindowTitle(tr("Outgoing Call ") + mCallingPartyName + "->" +
                           mCalledPartyName);
            ui->listenButton->hide();
            ui->hook->setChecked(true);
            if (calledType == ResourceData::TYPE_MOBILE)
            {
                //mobile via nwk
                pixmap.load(QtUtils::getRscIconSrc(CmnTypes::IDTYPE_MOBILE));
                ui->txPartyIconLabel->setPixmap(pixmap);
                if (SubsData::getIssiType(mCalledParty) ==
                    SubsData::TERMINALTYPE_BWC)
                {
                    ui->callButton->hide();
                    ui->callButton->setCheckable(true); //just to mark for bwc
                }
                ui->pttButton->hide();
#ifndef NO_VIDEO
                ui->videoButton->show();
#endif
            }
            else
            {
                pixmap.load(QtUtils::getRscIconSrc(CmnTypes::IDTYPE_SUBSCRIBER));
                ui->hook->setEnabled(true);
                ui->noHook->setEnabled(true);
                ui->ambience->setEnabled(true);
            }
            if (!ui->callButton->isHidden())
                ui->callButton->setToolTip(tr("Call", "Button"));
            if (!ui->pttButton->isHidden())
                setPttIcon(CmnTypes::ACTIONTYPE_PTT);
            break;
        case CmnTypes::CALLTYPE_MON_IND_DUPLEX:
            ui->txPartyLabel->setText(tr("Duplex"));
            ui->txPartyLabel->setStyleSheet(STYLE_BGCOLOR_PENDING);
            ui->txPartyLabel->show();
            //fallthrough
        case CmnTypes::CALLTYPE_MON_AMBIENCE:
            if (type == CmnTypes::CALLTYPE_MON_AMBIENCE)
                ui->ambience->setChecked(true);
            //fallthrough
        case CmnTypes::CALLTYPE_MON_IND_PTT:
            ui->callButton->hide();
            ui->pttButton->hide();
            if (isHook)
            {
                ui->hook->setChecked(true);
                timeout = TIMEOUT_SETUP_HOOK;
            }
            else
            {
                ui->noHook->setChecked(true);
                timeout = TIMEOUT_SETUP;
            }
            pixmap.load(QtUtils::getRscIconSrc(CmnTypes::IDTYPE_SUBSCRIBER));
            break;
        default:
            break; //do nothing, should not occur
    }
    ui->calledPartyLabel->setText(mCalledPartyName);
    ui->calledIcon->setPixmap(pixmap);
    if (timeout != 0)
    {
        connect(&mCallTimer, &QTimer::timeout, this,
                [=] { emit callTimeout(mCallId, 0); });
        mCallTimer.start(timeout);
    }
}

#ifdef NOSIPSETUP
//incoming individual call direct from network
CallWindow::CallWindow(int      callId,
                       int      callingParty,
                       int      priority,
                       bool     isHook,
                       bool     isDuplex,
                       bool     isTxGranted,
                       QWidget *parent) :
QDialog(parent, Qt::WindowTitleHint), ui(new Ui::CallWindow),
mType(CmnTypes::CALLTYPE_IND_IN), mCallId(callId), mCalledParty(0),
mCallingParty(callingParty), mDuration(-1),
mPriority((priority == MsgSp::Value::UNDEFINED)?
          MsgSp::Value::CALL_PRIORITY_LOWEST: priority),
mLocalAudRtpPort(0), mRemoteAudRtpPort(0), mRemoteVidRtpPort(0), mOwner(false),
mDuplex(isDuplex), mE2ee(false), mFirstPtt(true), mTxGranted(isTxGranted),
mCalledPartyName(sUserName),
mCallingPartyName(ResourceData::getDspTxt(callingParty,
                                          ResourceData::TYPE_SUBSCRIBER)),
mRingTone(0), mVideoStream(0)
{
    init();
    ui->callButton->setToolTip(tr("Answer"));
    ui->listenButton->hide();
    setIncoming(isHook, isDuplex, isTxGranted);
#ifdef NO_VOIP
    doShow();
#else
    connect(&mCallTimer, &QTimer::timeout, this,
            [=] { emit callTimeout(0, mCallingParty); });
    mCallTimer.start(TIMEOUT_SETUP);
#endif
}

//incoming individual call from VOIP server - from either network or another
//dispatcher or mobile
CallWindow::CallWindow(int      callingParty,
                       int      callingPartyType,
                       int      rtpPort,
                       QWidget *parent) :
QDialog(parent, Qt::WindowTitleHint), ui(new Ui::CallWindow),
mCallId(-1), mCalledParty(0), mCallingParty(callingParty), mDuration(-1),
mPriority(MsgSp::Value::CALL_PRIORITY_LOWEST), mLocalAudRtpPort(0),
mRemoteAudRtpPort(rtpPort), mDuplex(false), mE2ee(false), mFirstPtt(true),
mTxGranted(true), mCalledPartyName(sUserName), mRingTone(0), mVideoStream(0)
{
    switch (callingPartyType)
    {
        case CmnTypes::IDTYPE_DISPATCHER:
            mType = CmnTypes::CALLTYPE_DISPATCHER;
            mCallingPartyName = ResourceData::getClientDspTxt(true,
                                                              callingParty);
            break;
        case CmnTypes::IDTYPE_MOBILE:
            mType = CmnTypes::CALLTYPE_MOBILE;
            mCallingPartyName = ResourceData::getClientDspTxt(false,
                                                              callingParty);
            break;
        default:
            mType = CmnTypes::CALLTYPE_IND_IN;
            mCallingPartyName = ResourceData::getDspTxt(callingParty,
                                                 ResourceData::TYPE_SUBSCRIBER);
            break;
    }
    //mCallId is a dummy non-zero value
    init();
    ui->callButton->setToolTip(tr("Answer"));
    ui->listenButton->hide();
    if (isInternalCall())
    {
        mDuplex = true;
        ui->txPartyIconLabel
          ->setPixmap(QtUtils::getRscIconSrc(callingPartyType));
        setIncoming(true, true, true);
        doShow();
    }
    else
    {
        connect(&mCallTimer, &QTimer::timeout, this,
                [=] { emit callTimeout(0, mCallingParty); });
        mCallTimer.start(TIMEOUT_SETUP);
    }
}

#else
//incoming individual call - network or dispatcher or mobile
CallWindow::CallWindow(int      callingParty,
                       int      callingPartyType,
                       bool     isVideo,
                       QWidget *parent) :
QDialog(parent, Qt::WindowTitleHint), ui(new Ui::CallWindow), mCallId(-1),
mCalledParty(0), mCallingParty(callingParty), mDuration(-1),
mPriority(MsgSp::Value::CALL_PRIORITY_LOWEST), mLocalAudRtpPort(0),
mRemoteAudRtpPort(0), mRemoteVidRtpPort(0), mOwner(false), mDuplex(true),
mE2ee(false), mFirstPtt(true), mTxGranted(true), mCalledPartyName(sUserName),
mRingTone(0), mVideoStream(0)
{
    switch (callingPartyType)
    {
        case CmnTypes::IDTYPE_DISPATCHER:
            mType = CmnTypes::CALLTYPE_DISPATCHER;
            mCallingPartyName = ResourceData::getClientDspTxt(callingParty);
            break;
        case CmnTypes::IDTYPE_MOBILE:
            mType = (ResourceData::hasMobileStat() || isVideo)?
                    CmnTypes::CALLTYPE_MOBILE: CmnTypes::CALLTYPE_IND_IN;
            mCallingPartyName = ResourceData::getDspTxt(callingParty);
            break;
        default:
            mType = CmnTypes::CALLTYPE_IND_IN;
            mCallingPartyName = ResourceData::getDspTxt(callingParty,
                                                 ResourceData::TYPE_SUBSCRIBER);
            break;
    }
    //mCallId is a dummy non-zero value
    init();
    ui->listenButton->hide();
    if (callingPartyType == CmnTypes::IDTYPE_DISPATCHER ||
        callingPartyType == CmnTypes::IDTYPE_MOBILE)
    {
        ui->txPartyIconLabel
          ->setPixmap(QtUtils::getRscIconSrc(callingPartyType));
    }
}
#endif //NOSIPSETUP

CallWindow::~CallWindow()
{
    reset(true);
    delete ui;
}

void CallWindow::doShow()
{
    if (!isVisible())
    {
        show(); //must do this first, to get initial default position
        //move by random values
        QPoint pt(pos());
        QSize sz(size());
        //range is half of the width/height in either direction
        pt.rx() += ((rand() % sz.width()) - sz.width()/2);
        pt.ry() += ((rand() % sz.height()) - sz.height()/2);
        move(pt);
    }
    raise();
    activateWindow();
}

void CallWindow::callSetup(int           audPort,
                           int           vidPort,
                           int           priority,
                           const string &audKey,
                           const string &vidKey,
                           const string &callId,
                           bool          show)
{
    mRemoteAudRtpPort = audPort;
    mRemoteVidRtpPort = vidPort;
    mRemoteAudRtpKey = audKey;
    mRemoteVidRtpKey = vidKey;
    if (priority > 0)
        mPriority = priority;
    mVoipId1 = callId;
#ifdef NOSIPSETUP
    if (mCallId == 0)
        setIncoming(true, true, true, -1); //convert outgoing to incoming
    else if (mDuplex)
        ui->callButton->show();
    else
        ui->pttButton->show();
    //call setup message complete if:
    //-call ID present (server call setup received earlier),
    //-dispatcher call,
    //otherwise pending server call setup
    if (mCallId > 0 || isInternalCall())
    {
        mCallTimer.stop();
        if (show)
            doShow();
    }
#else
    //dispatcher/mobile call
    setIncoming(true, true, true, -1);
    if (show)
        doShow();
#endif //NOSIPSETUP
}

#ifdef NOSIPSETUP
void CallWindow::callSetup(int  callId,
                           int  priority,
                           bool isHook,
                           bool isDuplex,
                           bool isTxGranted,
                           bool show)
{
    if (priority != MsgSp::Value::UNDEFINED)
        mPriority = priority;
    else
        mPriority = MsgSp::Value::CALL_PRIORITY_LOWEST;
    setIncoming(isHook, isDuplex, isTxGranted, callId);
#ifndef NO_VOIP
    if (mRemoteAudRtpPort <= 0)
    {
        //converting from outgoing to incoming - pending VOIP call setup
        ui->callButton->hide();
        ui->pttButton->hide();
    }
    else
#endif
    {
        //call setup message complete
        mCallTimer.stop();
        if (show)
            doShow();
    }
}

#else
void CallWindow::callSetup(int           callId,
                           int           priority,
                           int           audPort,
                           const string &audKey,
                           bool          isHook,
                           bool          isDuplex,
                           bool          isTxGranted,
                           bool          show)
{
    mRemoteAudRtpPort = audPort;
    mRemoteAudRtpKey = audKey;
    if (priority != MsgSp::Value::UNDEFINED)
        mPriority = priority;
    setIncoming(isHook, isDuplex, isTxGranted, callId);
    if (show)
        doShow();
}
#endif //NOSIPSETUP

void CallWindow::callAlert()
{
    playRingTone(TONETYPE_RINGBACK);
}

void CallWindow::setIncoming(bool isHook,
                             bool isDuplex,
                             bool isTxGranted,
                             int  callId)
{
    if (isHook)
    {
        ui->hook->setChecked(true);
        if (sAudioMgr->hasActiveAudio())
            playRingTone(TONETYPE_BEEP);
        else
            playRingTone(TONETYPE_RINGING);
    }
    else
    {
        ui->noHook->setChecked(true);
    }
    mDuplex = isDuplex;
    if (mCallId == 0) //converting from outgoing to incoming
    {
        if (mType == CmnTypes::CALLTYPE_IND_OUT)
            mType = CmnTypes::CALLTYPE_IND_IN;
        mTxGranted = isTxGranted;
        mCallingPartyName = mCalledPartyName;
        mCalledPartyName = sUserName;
        int tmp = mCallingParty;
        mCallingParty = mCalledParty;
        mCalledParty = tmp;
    }
    mCallId = callId;
    ui->closeButton->setEnabled(false); //not allowed to dismiss incoming call
    ui->hook->setEnabled(false);
    ui->noHook->setEnabled(false);
    ui->ambience->setEnabled(false);
    ui->preemptiveCheck->setEnabled(false);
    ui->preemptiveCheck
      ->setChecked(mPriority >= MsgSp::Value::CALL_PRIORITY_PREEMPTIVE_1);
    ui->preemptiveCheck->show();
    ui->callingPartyLabel->setText(mCallingPartyName);
    ui->calledPartyLabel->setText(mCalledPartyName);
    if (mPriority == MsgSp::Value::CALL_PRIORITY_PREEMPTIVE_4_EMERGENCY)
        ui->callDetailsFrame->setStyleSheet(STYLE_BGCOLOR_ACTIVE_EMERGENCY);
    setWindowTitle(tr("Incoming Call ") + mCallingPartyName + "->" +
                   mCalledPartyName);
    if (isDuplex)
    {
        ui->callButton->setToolTip(tr("Answer"));
        ui->callButton->show();
        ui->pttButton->hide();
#ifndef NO_VIDEO
        if (mRemoteVidRtpPort > 0)
        {
            ui->videoButton->show();
            ui->txPartyLabel->setText(tr("Video"));
        }
        else
        {
            ui->videoButton->hide();
#else
        {
#endif
            ui->txPartyLabel->setText(tr("Duplex"));
        }
        ui->txPartyLabel->setStyleSheet(STYLE_BGCOLOR_PENDING);
    }
    else
    {
        ui->callButton->hide();
        setPttIcon(CmnTypes::ACTIONTYPE_PTT);
        ui->pttButton->show();
        ui->videoButton->hide();
        ui->txPartyLabel->hide();
    }
    ui->calledIcon
      ->setPixmap(QtUtils::getRscIconSrc(CmnTypes::IDTYPE_DISPATCHER));
    //set start time to enable a missed call record - will be overwritten on
    //call connect
    ui->startTimeLabel->setText(QtUtils::getTimestamp());
}

void CallWindow::setIncomingGrp(bool isHook,
                                bool isE2ee,
                                int  callingParty,
                                int  callingType,
                                int  calledParty,
                                int  callId)
{
    mE2ee = isE2ee;
    if (isHook)
        ui->hook->setChecked(true);
    else
        ui->noHook->setChecked(true);
    if (mCallId == 0) //converting from outgoing to incoming
    {
        if (mType != CmnTypes::CALLTYPE_GROUP_OUT)
        {
            //CALLTYPE_GROUP_IN means called from constructor
            if (mType != CmnTypes::CALLTYPE_GROUP_IN)
            {
                mCalledParty = calledParty;
                mCalledPartyName = ResourceData::getDspTxt(calledParty,
                                                      ResourceData::TYPE_GROUP);
            }
            ui->calledPartyLabel->setText(mCalledPartyName);
            ui->calledIcon
              ->setPixmap(QtUtils::getRscIconSrc(CmnTypes::IDTYPE_GROUP));
        }
        mType = CmnTypes::CALLTYPE_GROUP_IN;
        mCallId = callId;
        mCallingParty = callingParty;
        mCallingPartyName = ResourceData::getDspTxt(callingParty, callingType);
        ui->callingPartyLabel->setText(mCallingPartyName);
        ui->hook->setEnabled(false);
        ui->noHook->setEnabled(false);
        ui->ambience->setEnabled(false);
        //ensure unique connection, because Qt::UniqueConnection does not work
        //for lambda
        disconnect(&mCallTimer, &QTimer::timeout, this, 0);
        connect(&mCallTimer, &QTimer::timeout, this,
                [=] { emit callTimeout(mCallId, 0); });
        mCallTimer.start(TIMEOUT_SETUP);
    }
    else
    {
        ui->listenButton->hide();
    }
    ui->preemptiveCheck->setEnabled(false);
    ui->preemptiveCheck->setChecked(false);
    ui->preemptiveCheck->show();
    if (mType == CmnTypes::CALLTYPE_BROADCAST_IN)
        setWindowTitle(tr("Broadcast Call ") + mCallingPartyName);
    else
        setWindowTitle(tr("Group Call ") + mCallingPartyName + "->" +
                       mCalledPartyName);
    if (mE2ee)
    {
        ui->callButton->setEnabled(false);
        ui->callButton->setToolTip(tr("Not allowed for E2EE call"));
        ui->micButton->setEnabled(false);
    }
    else
    {
        ui->callButton->setToolTip(tr("Join"));
    }
    ui->callButton->show();
    ui->pttButton->hide();
    ui->videoButton->hide();
}

bool CallWindow::setConnected(const QString &txParty, int priority)
{
    stopRingTone();
    //check priority first, in case the peer server message comes first (it has
    //no priority)
    if (priority != MsgSp::Value::UNDEFINED)
    {
        mPriority = priority;
        if (priority >= MsgSp::Value::CALL_PRIORITY_PREEMPTIVE_1)
        {
            ui->preemptiveCheck->setChecked(true);
            if (priority == MsgSp::Value::CALL_PRIORITY_PREEMPTIVE_4_EMERGENCY)
                ui->callDetailsFrame->setStyleSheet(STYLE_BGCOLOR_ACTIVE_EMERGENCY);
        }
    }
    if (mTimer.isActive())
    {
        //2 cases to update Tx-Party here in outgoing client call:
        //-duplicate MON-CONNECT from peer server for dispatcher Tx-Party,
        //-server CALL-CONNECT after VOIP CALL-CONNECT, which does not have
        // Tx-Party
        if (ui->txPartyLabel->text().isEmpty() ||
            ResourceData::isClient(txParty))
            setTxParty(txParty);
        return false;
    }
    bool retVal = false;
    ui->startTimeLabel->setText(QtUtils::getTimestamp());
    ui->timeDurationLabel->setText("00:00");
    if (mType == CmnTypes::CALLTYPE_IND_IN && !txParty.isEmpty())
    {
        //incoming call answered by another client
        mCalledPartyName = txParty;
        ui->calledPartyLabel->setText(mCalledPartyName);
        setWindowTitle(tr("Incoming Call ") + mCallingPartyName + "->" +
                       mCalledPartyName);
        mCalledParty = ResourceData::getId(txParty);
        ui->callButton->hide();
        ui->pttButton->hide();
        ui->closeButton->setEnabled(true);
        hide();
        retVal = true;
    }
    else if (mType != CmnTypes::CALLTYPE_BROADCAST_OUT)
    {
        if (!txParty.isEmpty())
            setTxParty(txParty);
        if (mType == CmnTypes::CALLTYPE_MOBILE)
        {
            mStartTime = time(0);
        }
        else if (!ui->listenButton->isHidden() && !ui->endButton->isEnabled())
        {
            if (mE2ee)
                ui->listenButton->setToolTip(tr("Not allowed for E2EE call"));
            else
                ui->listenButton->setEnabled(true);
            ui->speakerButton->setEnabled(false);
            ui->speakerSingleButton->setEnabled(false);
        }
        ui->closeButton->setEnabled(true);
    }
    ui->txPartyLabel->setStyleSheet(STYLE_BGCOLOR_ACTIVE);
    mTimer.start();
    if (mCallTimer.isActive())
    {
        mCallTimer.stop();
        mCallTimer.start(TIMEOUT_ACTIVE);
    }
    //auto-join: broadcast call (always),
    //           group call (if not E2EE, and enabled or emergency)
    if (mType == CmnTypes::CALLTYPE_BROADCAST_IN ||
        (mType == CmnTypes::CALLTYPE_GROUP_IN && !mE2ee &&
         (Settings::instance().get<bool>(Props::FLD_CFG_GRPCALL_AUTOJOIN) ||
          mPriority == MsgSp::Value::CALL_PRIORITY_PREEMPTIVE_4_EMERGENCY)))
    {
        //do not show for other calls - accessed through Contact instead
        if (mType == CmnTypes::CALLTYPE_BROADCAST_IN)
            doShow();
        ui->callButton->click();
    }
    return retVal;
}

void CallWindow::setConnected(int           calledParty,
                              int           lclAudPort,
                              int           rmtAudPort,
                              int           lclVidPort,
                              int           rmtVidPort,
                              const string &lclAudKey,
                              const string &rmtAudKey,
                              const string &lclVidKey,
                              const string &rmtVidKey)
{
    stopRingTone();
    mRemoteAudRtpPort = rmtAudPort;
    mRemoteVidRtpPort = rmtVidPort;
    mRemoteAudRtpKey = rmtAudKey;
    mRemoteVidRtpKey = rmtVidKey;
    if (mCallId != 0 || isInternalCall() || mRemoteVidRtpPort > 0)
    {
        sAudioMgr->startRtp(calledParty, lclAudPort, rmtAudPort,
                            this, (isGrpCall())? 0: audStatCb, lclAudKey,
                            rmtAudKey);
        if (!isGrpCall())
            ui->statFrame->show();
        setAudioEnabled((mTxGranted)? AUDIOTYPE_BOTH: AUDIOTYPE_IN, true, false,
                        calledParty);
        if (lclVidPort > 0 && rmtVidPort > 0)
        {
            startVideo(lclVidPort, lclVidKey);
            setVideoOut(true);
            mRemoteVidRtpPort = -mRemoteVidRtpPort; //-ve indicates video out
            if (ui->callButton->isCheckable() && ui->micButton->isChecked())
                ui->micButton->click(); //force mute for bwc video call
        }
    }
    else
    {
        //save local port for starting the RTP session later
        mLocalAudRtpPort = lclAudPort;
        mLocalAudRtpKey = lclAudKey;
    }
}

void CallWindow::setConnected(bool owner, bool enablePtt, int txGrant)
{
    mOwner = owner;
    mTxGranted = (txGrant == MsgSp::Value::TRANSMISSION_GRANTED);
    if (mRemoteAudRtpPort != 0 && mLocalAudRtpPort != 0)
    {
        sAudioMgr->startRtp(mCalledParty, mLocalAudRtpPort, mRemoteAudRtpPort,
                            this, (isGrpCall())? 0: audStatCb, mLocalAudRtpKey,
                            mRemoteAudRtpKey);
        if (!isGrpCall())
            ui->statFrame->show();
        setAudioEnabled((mTxGranted)? AUDIOTYPE_BOTH: AUDIOTYPE_IN, true, false,
                        mCalledParty);
    }
    switch (mType)
    {
        case CmnTypes::CALLTYPE_BROADCAST_OUT:
            if (!isPttPressed())    //PTT already released
                onPttRelease(true); //cease now
            else if (mTxGranted)
                setPttIcon(CmnTypes::ACTIONTYPE_PTT_ACTIVE, true);
            break;
        case CmnTypes::CALLTYPE_GROUP_IN:
            if (enablePtt)
            {
                setPttIcon(CmnTypes::ACTIONTYPE_PTT);
                ui->pttButton->show();
            }
            break;
        case CmnTypes::CALLTYPE_GROUP_OUT:
            if (!owner)
                setWindowTitle(tr("Group Call ") + mCalledPartyName);
            if (!isPttPressed())
                onPttRelease(true);
            else if (mTxGranted)
                setPttIcon(CmnTypes::ACTIONTYPE_PTT_ACTIVE, true);
            else if (!mFirstPtt)
                setPttIcon(CmnTypes::ACTIONTYPE_PTT);
            //called into on-going call - release PTT if not queued
            else if (txGrant != MsgSp::Value::TRANSMISSION_QUEUED)
                onPttRelease(false);
            break;
        case CmnTypes::CALLTYPE_IND_OUT:
            if (!mDuplex)
            {
                if (ui->categoryGroup->checkedId() == RBID_NO_HOOK &&
                    !isPttPressed())
                    onPttRelease(true);
                else if (mTxGranted)
                    setPttIcon(CmnTypes::ACTIONTYPE_PTT_ACTIVE, true);
                else
                    setPttIcon(CmnTypes::ACTIONTYPE_PTT);
            }
            break;
        case CmnTypes::CALLTYPE_MON_AMBIENCE:
        case CmnTypes::CALLTYPE_MON_IND_DUPLEX:
        case CmnTypes::CALLTYPE_MON_IND_PTT:
            ui->listenButton->setEnabled(false);
            ui->closeButton->setEnabled(true);
            break;
        case CmnTypes::CALLTYPE_DISPATCHER:
        case CmnTypes::CALLTYPE_MOBILE:
        case CmnTypes::CALLTYPE_IND_IN:
        default:
            break; //do nothing
    }
}

void CallWindow::changeOwnership(const QString &callingPartyName, int priority)
{
    mOwner = false;
    if (!callingPartyName.isEmpty())
        mCallingPartyName = callingPartyName;
    else
        mCallingPartyName = tr("Unknown");
    switch (mType)
    {
        case CmnTypes::CALLTYPE_BROADCAST_OUT:
            setWindowTitle(tr("Broadcast Call ") + mCallingPartyName);
            ui->pttButton->hide();
            break;
        case CmnTypes::CALLTYPE_GROUP_IN:
        case CmnTypes::CALLTYPE_GROUP_OUT:
            setWindowTitle(tr("Group Call ") + mCallingPartyName + "->" +
                           mCalledPartyName);
            break;
        default:
            break; //do nothing
    }
    ui->callingPartyLabel->setText(mCallingPartyName);
    clearTxParty(); //in case no CALL_TX_CEASED for current tx party
    if (priority > 0 && priority != mPriority)
    {
        mPriority = priority;
        ui->preemptiveCheck
          ->setChecked(mPriority >= MsgSp::Value::CALL_PRIORITY_PREEMPTIVE_1);
        if (mPriority == MsgSp::Value::CALL_PRIORITY_PREEMPTIVE_4_EMERGENCY)
            ui->callDetailsFrame->setStyleSheet(STYLE_BGCOLOR_ACTIVE_EMERGENCY);
    }
}

void CallWindow::setTxParty(const QString &txParty)
{
    switch (mType)
    {
        case CmnTypes::CALLTYPE_GROUP_IN:
        case CmnTypes::CALLTYPE_GROUP_OUT:
            //seconds<=0 in the last PTT entry means that this is a repeated
            //invocation just to update the Tx-Party with a dispatcher ID,
            //and should not be added as a new entry
            if (mPttHistory.isEmpty() || mPttHistory.last().seconds > 0)
            {
                //mark start with negative seconds since 00:00:00
                mPttHistory.append(PttData(ui->timeDurationLabel->text(),
                                           txParty,
                                           -mTime.msecsSinceStartOfDay()/1000));
            }
            else if (ResourceData::isClient(txParty))
            {
                mPttHistory.last().txParty = txParty;
            }
            else
            {
                //unexpected case where the message with dispatcher ID came
                //before the actual network message - do nothing
                break;
            }
            //fallthrough
        case CmnTypes::CALLTYPE_BROADCAST_IN:
        case CmnTypes::CALLTYPE_BROADCAST_OUT:
        case CmnTypes::CALLTYPE_IND_IN:
        case CmnTypes::CALLTYPE_IND_OUT:
        case CmnTypes::CALLTYPE_MON_AMBIENCE:
        case CmnTypes::CALLTYPE_MON_IND_PTT:
            if (ResourceData::isClient(txParty))
                ui->txPartyIconLabel
                  ->setPixmap(QtUtils::getRscIconSrc(
                                                  CmnTypes::IDTYPE_DISPATCHER));
            else
                ui->txPartyIconLabel
                  ->setPixmap(QtUtils::getRscIconSrc(
                                               ResourceData::getType(txParty)));
            ui->txPartyLabel->setText(txParty);
            ui->txPartyLabel->show();
            break;
        default:
            break; //do nothing
    }
}

int CallWindow::getTxParty()
{
    QString s(ui->txPartyLabel->text());
    return (s.isEmpty())? 0: ResourceData::getId(s);
}

void CallWindow::clearTxParty()
{
    ui->txPartyLabel->hide();
    //PTT start marker is negative
    if (!mPttHistory.isEmpty() && mPttHistory.last().seconds <= 0)
    {
        int seconds = mTime.msecsSinceStartOfDay()/1000 +
                      mPttHistory.last().seconds;
        if (seconds == 0) //PTT less than 1 second
            seconds = 1;
        mPttHistory.last().seconds = seconds;
    }
}

void CallWindow::callInclude(bool success, bool enablePtt)
{
    if (!success)
    {
        ui->callButton->show();
    }
    else
    {
        setPttIcon(CmnTypes::ACTIONTYPE_PTT);
        ui->pttButton->show();
        ui->pttButton->setEnabled(enablePtt);
    }
    ui->closeButton->setEnabled(true);
}

void CallWindow::txGrant(int txGrantVal, bool enablePtt, const QString &txParty)
{
    mTxGranted = (txGrantVal == MsgSp::Value::TRANSMISSION_GRANTED);
    if (mTxGranted)
        setAudioEnabled(AUDIOTYPE_OUT, true);
    else if (txGrantVal == MsgSp::Value::TRANSMISSION_NOT_GRANTED)
        onPttRelease(false); //rejected tx-demand
    if (mTxGranted && isPttPressed())
        setPttIcon(CmnTypes::ACTIONTYPE_PTT_ACTIVE, true);
    else
        ui->pttButton->setEnabled(enablePtt);
    setTxParty(txParty);
}

void CallWindow::txCeased(bool isResponse, bool enablePtt)
{
    if (isResponse)
    {
        //visible txPartyLabel means TX-DMD & TX-CEASED sent while another party
        //talking, because otherwise in normal PTT release, txPartyLabel is
        //hidden by onPttReleased() - do nothing
        if (!ui->txPartyLabel->isHidden())
            return;
    }
    else if (mTxGranted)
    {
        onPttRelease(false); //server-forced TX-CEASED
    }
    //if pttButton is currently pressed, enablePtt==false triggers
    //onPttReleased() below
    ui->pttButton->setEnabled(enablePtt);
    clearTxParty();
}

void CallWindow::setNoRecord()
{
    //prevent this from being recorded as a missed call by preventing endCall()
    //from reset()
    ui->startTimeLabel->setText("");
}

void CallWindow::end(bool isByUser)
{
    if (ui->endButton->isEnabled())
        onEndButton(isByUser);
}

bool CallWindow::checkPttOnCallRelease(int duration)
{
    if (mCallId == 0 || mFirstPtt || !isPttPressed())
        return false;
    if (mPttTimer.isActive())
        mPttTimer.stop(); //prevent premature onPttPress()
    if (duration >= 0)
        mDuration = duration;
    reset(false);
    onPttPress();
    return true;
}

void CallWindow::release(int result)
{
    if (result != 0)
    {
        //call listening failed
        if (ui->endButton->isEnabled())
        {
            ui->endButton->setEnabled(false);
            //allow some time for VOIP to settle before taking action
            QTimer::singleShot(200, this, [this]
            {
                if (mTimer.isActive())
                    stopRtp();
                decrCount();
                sSession->listenDisconnect(mCallingParty);
                sSession->listenDisconnect(mCalledParty);
            });
        }
        return;
    }
    if (!ui->listenButton->isHidden())
    {
        ui->listenButton->setEnabled(true);
        setAudioEnabled(AUDIOTYPE_IN, false);
        ui->speakerButton->setEnabled(false);
        ui->speakerSingleButton->setEnabled(false);
    }
    else
    {
        ui->callButton->show();
        ui->pttButton->hide();
    }
    if (ui->endButton->isEnabled())
    {
        ui->endButton->setEnabled(false);
        if (mTimer.isActive())
            stopRtp();
        decrCount();
    }
}

void CallWindow::releaseOutgoing(const QString &failedCause)
{
    //take action only for outgoing call in setup phase
    switch (mType)
    {
        case CmnTypes::CALLTYPE_BROADCAST_OUT:
        case CmnTypes::CALLTYPE_GROUP_OUT:
        case CmnTypes::CALLTYPE_IND_AMBIENCE:
        case CmnTypes::CALLTYPE_IND_OUT:
        case CmnTypes::CALLTYPE_MOBILE:
            //in setup phase, endButton is enabled and either:
            //-timer is not running because call is not connected
            //-timer is running but no call ID (call connected on VOIP only -
            // excluding mobile call)
            if (ui->endButton->isEnabled() &&
                (!mTimer.isActive() ||
                 (mCallId == 0 && mType != CmnTypes::CALLTYPE_MOBILE)))
                break;
            //fallthrough
        default:
            return; //do nothing
    }
    stopRingTone();
    mFailedCause = failedCause;
    if (mTimer.isActive())
        stopRtp();
    endCall();
    if (mType == CmnTypes::CALLTYPE_BROADCAST_OUT)
        showFailureMsg(this, tr("Call Failure"),
                       tr("Failed broadcast call.\n") + failedCause);
    else
        showFailureMsg(this, tr("Call Failure"),
                       tr("Failed call to ") + mCalledPartyName + ".\n" +
                           failedCause);
}

void CallWindow::reset(bool onDelete)
{
    mCallTimer.stop();
    if (ui->endButton->isEnabled())
    {
        if (mTimer.isActive())
            stopRtp();
        decrCount();
        if (mType == CmnTypes::CALLTYPE_MON_IND_DUPLEX ||
            mType == CmnTypes::CALLTYPE_MON_IND_PTT)
        {
            sSession->listenDisconnect(mCallingParty);
            sSession->listenDisconnect(mCalledParty);
        }
    }
    if (!ui->startTimeLabel->text().isEmpty())
        endCall();
    delete mRingTone;
    if (onDelete)
    {
        if (sMicOwner == this)
            sMicOwner = 0;
        return;
    }
    if (ui->fullscreenButton->isChecked())
        ui->fullscreenButton->click();
    //change into outgoing
    mDuration = -1;
    mLocalAudRtpPort = 0;
    mRemoteAudRtpPort = 0;
    mRemoteVidRtpPort = 0;
    mOwner = true;
    mE2ee = false;
    mFirstPtt = true;
    mTxGranted = false;
    mRingTone = 0;
    mTime.setHMS(0, 0, 0);
    ui->callDetailsFrame->setStyleSheet(STYLE_CALL_DETAILS_FRAME);
    ui->startTimeLabel->clear();
    ui->timeDurationLabel->clear();
    ui->txPartyLabel->clear();
    ui->audStatLabel->clear();
    ui->vidStatLabel->clear();
    ui->statFrame->hide();
    ui->speakerButton->setChecked(false);
    ui->speakerButton->setEnabled(false);
    ui->speakerSingleButton->setEnabled(false);
    ui->endButton->setEnabled(false);
    ui->closeButton->setEnabled(true);
    setCallingParty(ResourceData::TYPE_DISPATCHER, 0);
    if (mPriority == MsgSp::Value::CALL_PRIORITY_PREEMPTIVE_4_EMERGENCY)
    {
        mPriority = sPriorityDefault;
        ui->preemptiveCheck->setChecked(false);
        ui->callFrame->setStyleSheet(""); //follow parent
    }
    if (mType == CmnTypes::CALLTYPE_MOBILE && !ResourceData::hasMobileStat())
    {
        //when running with svr without mobile clients, mobile call must start
        //as IND_OUT, but if this was an incoming call, must reset to IND_IN
        //first, to be changed below
        mType = (ResourceData::getType(mCallingParty) ==
                 ResourceData::TYPE_MOBILE)?
                 CmnTypes::CALLTYPE_IND_IN: CmnTypes::CALLTYPE_IND_OUT;
        if (mCalledDomain.empty())
            mCalledDomain = sSession->getMobDomain();
    }
    switch (mType)
    {
        case CmnTypes::CALLTYPE_GROUP_IN:
            mType = CmnTypes::CALLTYPE_GROUP_OUT;
            setWindowTitle(tr("Outgoing Group Call ") + mCalledPartyName);
            //fallthrough
        case CmnTypes::CALLTYPE_BROADCAST_OUT:
            ui->txPartyLabel->setStyleSheet(STYLE_BGCOLOR_PENDING);
            //fallthrough
        case CmnTypes::CALLTYPE_GROUP_OUT:
            ui->callButton->hide();
            ui->preemptiveCheck->setEnabled(true);
            onHookClicked();
            break;
        case CmnTypes::CALLTYPE_IND_IN:
            mType = CmnTypes::CALLTYPE_IND_OUT; //needed for onHookClicked()
            mCalledParty = mCallingParty;
            mCalledPartyName = ResourceData::getDspTxt(mCalledParty,
                                                 ResourceData::TYPE_SUBSCRIBER);
            setWindowTitle(tr("Outgoing Call ") + mCallingPartyName + "->" +
                           mCalledPartyName);
            ui->calledPartyLabel->setText(mCalledPartyName);
            ui->callButton->setToolTip(tr("Call", "Button"));
            ui->calledIcon
              ->setPixmap(QtUtils::getRscIconSrc(
                                          ResourceData::getType(mCalledParty)));
            //fallthrough
        case CmnTypes::CALLTYPE_IND_OUT:
            ui->txPartyIconLabel
              ->setPixmap(QtUtils::getRscIconSrc(
                                          ResourceData::getType(mCalledParty)));
            onHookClicked();
#ifndef NO_VIDEO
            if (ResourceData::getType(mCalledParty) == ResourceData::TYPE_MOBILE)
                ui->videoButton->show();
#endif
            //fallthrough
        case CmnTypes::CALLTYPE_IND_AMBIENCE:
            mType = CmnTypes::CALLTYPE_IND_OUT;
            ui->callButton->show();
            ui->callButton->setEnabled(true);
            if (ResourceData::getType(mCalledParty) ==
                ResourceData::TYPE_SUBSCRIBER)
            {
                ui->hook->setEnabled(true);
                ui->noHook->setEnabled(true);
                ui->ambience->setEnabled(true);
            }
            ui->preemptiveCheck->setEnabled(true);
            break;
        case CmnTypes::CALLTYPE_MOBILE:
            ui->preemptiveCheck->setEnabled(true);
            //fallthrough
        case CmnTypes::CALLTYPE_DISPATCHER:
            if (!ui->callButton->isCheckable()) //exclude bwc
            {
                ui->callButton->show();
                ui->callButton->setEnabled(true);
            }
#ifndef NO_VIDEO
            ui->videoButton->show();
#endif
            if (mCallId != 0) //was incoming
            {
                mCalledParty = mCallingParty;
                mCalledPartyName = ResourceData::getDspTxt(mCalledParty,
                                      (mType == CmnTypes::CALLTYPE_DISPATCHER)?
                                          ResourceData::TYPE_DISPATCHER:
                                          ResourceData::TYPE_MOBILE);
                setWindowTitle(tr("Outgoing Call ") + mCallingPartyName + "->" +
                               mCalledPartyName);
                ui->calledPartyLabel->setText(mCalledPartyName);
                ui->callButton->setToolTip(tr("Call", "Button"));
                mCalledDomain = SubsData::getClientDomain(mCalledParty, true);
            }
            break;
        default:
            break; //do nothing
    }
    mCallId = 0;
    mCallingParty = 0;
}

void CallWindow::disableOutAudio()
{
    setAudioEnabled(AUDIOTYPE_OUT, false);
    if (mVideoStream != 0 && mRemoteVidRtpPort < 0)
        setVideoOut(false);
}

void CallWindow::disableInAudio()
{
    setAudioEnabled(AUDIOTYPE_IN, false);
}

void CallWindow::startCall()
{
    if (ui->callButton->isHidden())
        doPtt(true);
    else
        ui->callButton->click();
}

void CallWindow::doPtt(bool isPressed, bool isExternal)
{
    if (!isExternal || !ui->pttButton->isHidden())
    {
        ui->pttButton->setDown(isPressed);
        if (isPressed)
        {
            raise();
            onPttPressed();
        }
        else
        {
            onPttReleased();
        }
    }
}

bool CallWindow::isPttPressed()
{
    return ui->pttButton->isDown();
}

void CallWindow::setCallingParty(int type, int id)
{
    if (id == 0)
        mCallingPartyName = sUserName;
    else
        mCallingPartyName = ResourceData::getDspTxt(id, type);
    ui->callingPartyLabel->setText(mCallingPartyName);
    switch (mType)
    {
        case CmnTypes::CALLTYPE_BROADCAST_IN:
            setWindowTitle(tr("Broadcast Call ") + mCallingPartyName);
            break;
        case CmnTypes::CALLTYPE_GROUP_IN:
            setWindowTitle(tr("Group Call ") + mCallingPartyName + "->" +
                           mCalledPartyName);
            break;
        case CmnTypes::CALLTYPE_MON_AMBIENCE:
        case CmnTypes::CALLTYPE_MON_IND_DUPLEX:
        case CmnTypes::CALLTYPE_MON_IND_PTT:
            setWindowTitle(tr("Call ") + mCallingPartyName + "->" +
                           mCalledPartyName);
            if (type == ResourceData::TYPE_DISPATCHER ||
                type == ResourceData::TYPE_MOBILE)
                ui->listenButton->hide();
            break;
        default:
            break; //do nothing
    }
}

void CallWindow::setCalledParty(const QString &name)
{
    if (mType == CmnTypes::CALLTYPE_MON_IND_DUPLEX ||
        mType == CmnTypes::CALLTYPE_MON_IND_PTT)
    {
        mCalledPartyName = name;
        ui->calledPartyLabel->setText(name);
        setWindowTitle(tr("Call ") + mCallingPartyName + "->" + name);
        if (ResourceData::isClient(name))
        {
            ui->calledIcon
              ->setPixmap(QtUtils::getRscIconSrc(CmnTypes::IDTYPE_DISPATCHER));
            ui->listenButton->hide();
        }
    }
}

int CallWindow::getCallParty()
{
    switch (mType)
    {
        case CmnTypes::CALLTYPE_BROADCAST_IN:
        case CmnTypes::CALLTYPE_BROADCAST_OUT:
        case CmnTypes::CALLTYPE_GROUP_IN:
        case CmnTypes::CALLTYPE_GROUP_OUT:
        case CmnTypes::CALLTYPE_IND_AMBIENCE:
        case CmnTypes::CALLTYPE_IND_OUT:
        case CmnTypes::CALLTYPE_MON_AMBIENCE:
        case CmnTypes::CALLTYPE_MON_IND_DUPLEX:
        case CmnTypes::CALLTYPE_MON_IND_PTT:
            return mCalledParty;
        case CmnTypes::CALLTYPE_DISPATCHER:
        case CmnTypes::CALLTYPE_MOBILE:
            if (mCallId == 0) //outgoing
                return mCalledParty;
            //fallthrough
        case CmnTypes::CALLTYPE_IND_IN:
            return mCallingParty;
        default:
            break; //do nothing
    }
    return 0;
}

bool CallWindow::isGrpCall() const
{
    return (mType == CmnTypes::CALLTYPE_GROUP_IN ||
            mType == CmnTypes::CALLTYPE_GROUP_OUT ||
            mType == CmnTypes::CALLTYPE_BROADCAST_IN);
}

bool CallWindow::isDispatcherCall() const
{
    return (mType == CmnTypes::CALLTYPE_DISPATCHER);
}

bool CallWindow::isInternalCall() const
{
    return (mType == CmnTypes::CALLTYPE_DISPATCHER ||
            mType == CmnTypes::CALLTYPE_MOBILE);
}

bool CallWindow::isOutgoingCall() const
{
    return (mType == CmnTypes::CALLTYPE_GROUP_OUT ||
            mType == CmnTypes::CALLTYPE_IND_OUT ||
            (isInternalCall() && mCallId == 0) ||
            mType == CmnTypes::CALLTYPE_BROADCAST_OUT);
}

bool CallWindow::isNotStarted() const
{
    return !ui->endButton->isEnabled();
}

bool CallWindow::hasVoipId(const string &id)
{
    return (id == mVoipId1 || id == mVoipId2);
}

bool CallWindow::hasCallParty(int ssi) const
{
    return (ssi == mCallingParty || ssi == mCalledParty);
}

bool CallWindow::getPttInfo(QString &txParty, QIcon &pttIcon)
{
    if (ui->txPartyLabel->isHidden())
        txParty.clear();
    else
        txParty = ui->txPartyLabel->text();
    pttIcon = ui->pttButton->icon();
    return ui->pttButton->isEnabled();
}

bool CallWindow::keepOpen() const
{
    return (isVisible() && ui->keepOpenCheck->isChecked());
}

void CallWindow::onAudioStat(int kbps)
{
    ui->audStatLabel->setText(QString("A: ").append(QString::number(kbps))
                                            .append(" kbps"));
}

void CallWindow::onVideoReceived(uchar *data,
                                 int    width,
                                 int    height,
                                 int    bytesPerLine)
{
    emit drawFrame(QPixmap::fromImage(QImage(data, width, height, bytesPerLine,
                                             QImage::Format_RGB888)));
}

void CallWindow::onVideoStat(int kbps)
{
    QString s("V: ");
    if (kbps < 1000)
        s.append(QString::number(kbps)).append(" kbps");
    else
        s.append(QString::number(kbps/1000.0, 'f', 2)).append(" mbps");
    ui->vidStatLabel->setText(s);
}

void CallWindow::onVideoPreviewReceived(QPixmap frame)
{
    if ((ui->previewLabel->isHidden() && !ui->hidePreviewButton->isChecked()) ||
        ui->previewLabel->size() != frame.size())
    {
        ui->previewLabel->setMaximumSize(frame.width(), frame.height());
        ui->previewLabel->show();
    }
    ui->previewLabel->setPixmap(frame);
}

void CallWindow::showFailureMsg(QWidget       *parent,
                                const QString &title,
                                const QString &msg)
{
    if (title.isEmpty())
    {
        MessageDialog::showStdMessage(tr("VOIP Error"),
                                      tr("VOIP Server not available."),
                                      QMessageBox::Critical, true, parent);
    }
    else
    {
        assert(!msg.isEmpty());
        MessageDialog::showStdMessage(title, msg, QMessageBox::Critical,
                                      true, parent);
    }
}

void CallWindow::setPriorities(int def, int preemptive)
{
    sPriorityDefault = (def > 0)? def: PRIORITY_DEFAULT;
    sPriorityPreempt = (preemptive > 0)? preemptive: PRIORITY_PREEMPTIVE;
}

void CallWindow::decodeCb(void *obj, uchar *data, int w, int h, int bpl)
{
    if (obj == 0 || data == 0)
    {
        assert("Bad param in CallWindow::decodeCb" == 0);
        return;
    }
    static_cast<CallWindow *>(obj)->onVideoReceived(data, w, h, bpl);
}

void CallWindow::audStatCb(void *obj, int kbps)
{
    if (obj == 0)
    {
        assert("Bad param in CallWindow::audStatCb" == 0);
        return;
    }
    static_cast<CallWindow *>(obj)->onAudioStat(kbps);
}

void CallWindow::vidStatCb(void *obj, int kbps)
{
    if (obj == 0)
    {
        assert("Bad param in CallWindow::vidStatCb" == 0);
        return;
    }
    static_cast<CallWindow *>(obj)->onVideoStat(kbps);
}

void CallWindow::previewCb(void *obj, QPixmap frame)
{
    if (obj == 0)
    {
        assert("Bad param in CallWindow::previewCb" == 0);
        return;
    }
    static_cast<CallWindow *>(obj)->onVideoPreviewReceived(frame);
}

void CallWindow::finalize()
{
    VideoStream::stop();
}

void CallWindow::keyPressEvent(QKeyEvent *evt)
{
    if (!evt->isAutoRepeat() && sMicOwner != 0)
    {
        const Settings &st(Settings::instance());
        string s(st.get<string>(Props::FLD_CFG_PTT_CHAR));
        if (!s.empty())
        {
            //with Ctrl, event text is always uppercase
            bool ctrl = evt->modifiers().testFlag(Qt::ControlModifier);
            if (ctrl == st.get<bool>(Props::FLD_CFG_PTT_CTRL) &&
                evt->modifiers().testFlag(Qt::AltModifier) ==
                    st.get<bool>(Props::FLD_CFG_PTT_ALT) &&
                ((ctrl &&
                  evt->text().compare(s.c_str(), Qt::CaseInsensitive) == 0) ||
                 (!ctrl && evt->text().toStdString() == s)))
            {
                sMicOwner->doPtt(true, true);
            }
        }
    }
}

void CallWindow::keyReleaseEvent(QKeyEvent *evt)
{
    if (!evt->isAutoRepeat() && sMicOwner != 0 && sMicOwner->isPttPressed() &&
        evt->text().toStdString() ==
            Settings::instance().get<string>(Props::FLD_CFG_PTT_CHAR))
    {
        sMicOwner->doPtt(false, true);
    }
}

void CallWindow::onHookClicked()
{
    setPttIcon(CmnTypes::ACTIONTYPE_PTT);
    ui->pttButton->show();
    ui->micButton->setEnabled(true);
    if (sMicOwner == 0)
        setAudioEnabled(AUDIOTYPE_OUT, true);
}

void CallWindow::onPttPressed()
{
    mPttTimer.start(150); //delay action (milliseconds) until confirmed
}

void CallWindow::onPttReleased()
{
    if (mPttTimer.isActive())
    {
        mPttTimer.stop(); //pressed too briefly - do nothing
        return;
    }
    if (mFirstPtt && !ui->endButton->isEnabled())
    {
        setPttIcon(CmnTypes::ACTIONTYPE_PTT);
        if (!ui->closeButton->isEnabled())
        {
            //special case for VOIP error in onPttPress()
            ui->closeButton->setEnabled(true);
            showFailureMsg(this);
        }
        else if (!ui->closeButton->toolTip().isEmpty())
        {
            ui->closeButton->setToolTip("");
            showFailureMsg(this, tr("Call Not Allowed"),
                           tr("You already have the maximum %1 concurrent "
                              "sessions. End one before starting another.")
                               .arg(MAX_COUNT));
        }
    }
    else if (ui->pttButton->isCheckable())
    {
        //no TX-CEASED (delay it) if outgoing call not yet connected
        onPttRelease(!isOutgoingCall() || mTimer.isActive());
    }
}

void CallWindow::init()
{
    assert(sSession != 0);
    ui->setupUi(this);
    ui->callDetailsFrame->setStyleSheet(STYLE_CALL_DETAILS_FRAME);
    ui->categoryGroup->setId(ui->noHook, RBID_NO_HOOK);
    ui->categoryGroup->setId(ui->hook, RBID_HOOK);
    ui->categoryGroup->setId(ui->ambience, RBID_AMBIENCE);
    connect(ui->listenButton, &QToolButton::clicked, this,
            [this]
            {
                //start call listening
                if (!incrCount())
                    return;
                int res = sSession->listenConnect(mCallId, mCallingParty,
                                                  mVoipId1);
                if (res == ServerSession::VOIPSESSION_ERROR)
                {
                    showFailureMsg(this);
                    decrCount();
                }
                else if (res > 0 &&
                   sSession->listenConnect(mCallId, mCalledParty, mVoipId2) > 0)
                {
                    ui->listenButton->setEnabled(false);
                    ui->endButton->setEnabled(true);
                }
                else
                {
                    decrCount();
                }
            });
    connect(ui->videoButton, &QToolButton::clicked, this,
            [this]
            {
                ApiClient apiClient;
                apiClient.sendRequest();
                //start or answer video call
                if (mType == CmnTypes::CALLTYPE_IND_OUT &&
                    !ResourceData::hasMobileStat())
                    mType = CmnTypes::CALLTYPE_MOBILE;
                if (mCallId != 0)
                {
                    //answering incoming call
                    if (mType != CmnTypes::CALLTYPE_MOBILE &&
                        ResourceData::getType(mCallingParty) ==
                            ResourceData::TYPE_MOBILE)
                        mType = CmnTypes::CALLTYPE_MOBILE;
                    ui->callButton->click();
                    setVideoOut(true);
                    mRemoteVidRtpPort = -mRemoteVidRtpPort; //-ve is video out
                    return;
                }
                //new outgoing video call
                if (!incrCount())
                    return;
                int res = sSession->callSetupVideo(mCalledParty, mVoipId1,
                                                   mCalledDomain);
                if (res > 0)
                {
                    ui->txPartyLabel->setText(tr("Video"));
                    ui->txPartyLabel->setStyleSheet(STYLE_BGCOLOR_PENDING);
                    ui->txPartyLabel->show();
                    ui->callButton->hide();
                    ui->videoButton->hide();
                    ui->pttButton->hide();
                    ui->endButton->setEnabled(true);
                    ui->startTimeLabel->setText(QtUtils::getTimestamp());
                    if (isVisible())
                        ui->closeButton->setEnabled(false);
                }
                else
                {
                    decrCount();
                    if (res == ServerSession::VOIPSESSION_ERROR)
                        showFailureMsg(this);
                }
            });
    connect(ui->callButton, &QToolButton::clicked, this,
            [this]
            {
                //start individual full duplex call, or join group call
                if (!incrCount())
                    return;
                if (mType == CmnTypes::CALLTYPE_IND_OUT &&
                    ui->categoryGroup->checkedId() == RBID_AMBIENCE)
                    mType = CmnTypes::CALLTYPE_IND_AMBIENCE;
                int res = 0;
                int lclAport;
                string lclAkey;
                bool outCallStarted = false;
                switch (mType)
                {
                    case CmnTypes::CALLTYPE_BROADCAST_IN:
                    case CmnTypes::CALLTYPE_GROUP_IN:
                        res = sSession->ssicInvoke(mCallId, mCalledParty,
                                      ResourceData::getId(mCallingPartyName),
                                      ResourceData::isClient(mCallingPartyName),
                                      mVoipId1);
                        if (res > 0)
                        {
                            ui->callButton->hide();
                            ui->endButton->setEnabled(true);
                            if (isVisible() &&
                                mType == CmnTypes::CALLTYPE_GROUP_IN)
                                ui->closeButton->setEnabled(false);
                        }
                        else
                        {
                            decrCount();
                        }
                        break;
                    case CmnTypes::CALLTYPE_DISPATCHER:
                    case CmnTypes::CALLTYPE_MOBILE:
                        if (mCallId == 0) //new outgoing call
                        {
                            res = sSession->callSetupInd(mCalledParty, mVoipId1,
                                                         mCalledDomain);
                            if (res > 0)
                            {
                                ui->txPartyLabel->setText(tr("Duplex"));
                                ui->txPartyLabel
                                  ->setStyleSheet(STYLE_BGCOLOR_PENDING);
                                ui->txPartyLabel->show();
                                ui->callButton->hide();
                                ui->videoButton->hide();
                                ui->endButton->setEnabled(true);
                                if (isVisible())
                                    ui->closeButton->setEnabled(false);
                                outCallStarted = true;
                            }
                            else
                            {
                                decrCount();
                            }
                        }
                        else
                        {
                            //answering incoming call
                            int lclVport = 0;
                            string lclVkey;
                            lclAport = sSession->callConnect(mCallingParty,
                                       (!mRemoteAudRtpKey.empty())? &lclAkey: 0,
                                       (mRemoteVidRtpPort > 0)? &lclVport: 0,
                                       (mRemoteVidRtpPort > 0)? &lclVkey: 0);
                            if (lclAport > 0)
                            {
                                ui->txPartyLabel
                                  ->setStyleSheet(STYLE_BGCOLOR_ACTIVE);
                                ui->txPartyLabel->show();
                                ui->callButton->hide();
                                ui->videoButton->hide();
                                ui->endButton->setEnabled(true);
                                ui->closeButton->setEnabled(false);
                                emit incomingConnected(mCallingParty, 0);
                                sAudioMgr->startRtp(mCallingParty, lclAport,
                                                    mRemoteAudRtpPort, this,
                                                    audStatCb, lclAkey,
                                                    mRemoteAudRtpKey);
                                ui->statFrame->show();
                                setAudioEnabled(AUDIOTYPE_BOTH, true);
#ifndef NO_VIDEO
                                if (lclVport > 0)
                                    startVideo(lclVport, lclVkey);
#endif
                            }
                            else
                            {
                                decrCount();
                            }
                        }
                        break;
                    case CmnTypes::CALLTYPE_IND_AMBIENCE:
                        if (sSession->isLoggedIn())
                            res = sSession->callSetupAmbience(mCalledParty,
                                                              mVoipId1);
                        if (res > 0)
                        {
                            ui->txPartyLabel->setText(mCalledPartyName);
                            ui->txPartyLabel
                              ->setStyleSheet(STYLE_BGCOLOR_PENDING);
                            ui->txPartyLabel->show();
                            ui->callButton->hide();
                            ui->endButton->setEnabled(true);
                            ui->closeButton->setEnabled(false);
                            ui->hook->setEnabled(false);
                            ui->noHook->setEnabled(false);
                            ui->ambience->setEnabled(false);
                            outCallStarted = true;
                        }
                        else
                        {
                            decrCount();
                        }
                        break;
                    case CmnTypes::CALLTYPE_IND_IN:
                        lclAport = sSession->callConnect(mCallId, mCallingParty,
                                                    (!mRemoteAudRtpKey.empty())?
                                                        &lclAkey: 0);
                        if (lclAport > 0)
                        {
                            //answering incoming call
                            ui->txPartyLabel
                              ->setStyleSheet(STYLE_BGCOLOR_ACTIVE);
                            ui->txPartyLabel->show();
                            ui->callButton->hide();
                            ui->endButton->setEnabled(true);
                            emit incomingConnected(mCallingParty, mCallId);
                            sAudioMgr->startRtp(mCallingParty, lclAport,
                                                mRemoteAudRtpPort, this,
                                                audStatCb, lclAkey,
                                                mRemoteAudRtpKey);
                            ui->statFrame->show();
                            setAudioEnabled(AUDIOTYPE_BOTH, true);
                        }
                        else
                        {
                            decrCount();
                            //call answered by another client, do nothing more
                            //here - will be closed upon CALL_RELEASE
                        }
                        break;
                    case CmnTypes::CALLTYPE_IND_OUT:
                        if (sSession->isLoggedIn())
                            res = sSession->callSetupInd(mCalledParty, true,
                                      (ui->categoryGroup->checkedId() ==
                                           RBID_HOOK),
                                      mVoipId1,
                                      ((ui->preemptiveCheck->isChecked())?
                                           sPriorityPreempt: sPriorityDefault));
                        if (res > 0)
                        {
                            mDuplex = true;
                            ui->txPartyLabel->setText(tr("Duplex"));
                            ui->txPartyLabel
                              ->setStyleSheet(STYLE_BGCOLOR_PENDING);
                            ui->txPartyLabel->show();
                            ui->callButton->hide();
                            ui->videoButton->hide();
                            ui->pttButton->hide();
                            ui->endButton->setEnabled(true);
                            if (isVisible())
                                ui->closeButton->setEnabled(false);
                            ui->hook->setEnabled(false);
                            ui->noHook->setEnabled(false);
                            ui->ambience->setEnabled(false);
                            outCallStarted = true;
                        }
                        else
                        {
                            decrCount();
                        }
                        break;
                    default:
                        decrCount(); //should never occur
                        return;
                }
                //set outgoing call start time to enable a failed call record -
                //will be overwritten on call connect
                if (outCallStarted)
                    ui->startTimeLabel->setText(QtUtils::getTimestamp());
                else if (res == ServerSession::VOIPSESSION_ERROR)
                    showFailureMsg(this);
            });
    connect(ui->pttButton, SIGNAL(pressed()), SLOT(onPttPressed()));
    connect(ui->pttButton, SIGNAL(released()), SLOT(onPttReleased()));
    connect(ui->endButton, &QToolButton::clicked, this,
            [this] { onEndButton(true); });
    connect(ui->hook, SIGNAL(clicked()), SLOT(onHookClicked()));
    connect(ui->noHook, SIGNAL(clicked()), SLOT(onHookClicked()));
    connect(ui->ambience, &QRadioButton::clicked, this,
            [this]
            {
                ui->pttButton->hide();
                ui->micButton->setEnabled(false);
                setAudioEnabled(AUDIOTYPE_OUT, false);
            });
    connect(ui->micButton, &QPushButton::clicked, this,
            [this]
            {
                //enable/disable outgoing audio
                if (sMicOwner != 0 && sMicOwner->isPttPressed())
                {
                    //toggle back
                    ui->micButton->setChecked(!ui->micButton->isChecked());
                }
                else
                {
                    setAudioEnabled(AUDIOTYPE_OUT, ui->micButton->isChecked());
                    if (mVideoStream != 0 && mRemoteVidRtpPort < 0)
                        setVideoOut(ui->micButton->isChecked());
                }
            });
    connect(ui->speakerButton, &QPushButton::clicked, this,
            [this]
            {
                //enable/disable incoming audio
                setAudioEnabled(AUDIOTYPE_IN, ui->speakerButton->isChecked());
            });
    connect(ui->speakerSingleButton, &QToolButton::clicked, this,
            [this]
            {
                //enable incoming audio, and emit signal to disable incoming
                //audio in all other call windows
                if (!ui->speakerButton->isChecked())
                    ui->speakerButton->click();
                emit activeInCall(mCallId, getCallParty());
            });
    connect(ui->closeButton, &QPushButton::clicked, this,
            [this]
            {
                close();
                if (!mTimer.isActive())
                    emit callCancel(mCalledParty);
            });
    connect(ui->fullscreenButton, &QToolButton::clicked, this,
            [this]
            {
                //enter/exit video fullscreen mode
                QSize sz;
                if (ui->fullscreenButton->isChecked())
                {
                    setProperty("MyPos", pos()); //store pos in dynamic property
                    ui->callFrame->hide();
                    showFullScreen();
                    ui->fullscreenButton->setToolTip(tr("Exit full screen"));
                    sz = QSize(45, 45);
                }
                else
                {
                    ui->callFrame->show();
                    showFullScreen(); //ensure title bar shown after showNormal()
                    showNormal();
                    move(property("MyPos").value<QPoint>());
                    ui->fullscreenButton->setToolTip(tr("Full screen"));
                    sz = QSize(32, 32);
                }
                ui->fullscreenButton->setIconSize(sz);
                ui->hidePreviewButton->setIconSize(sz);
            });
    connect(ui->hidePreviewButton, &QToolButton::clicked, this,
            [this]
            {
                //show/hide video preview
                if (ui->hidePreviewButton->isChecked())
                {
                    VideoDevice::instance().removeCallback(this);
                    ui->previewLabel->hide();
                    ui->hidePreviewButton->setToolTip(tr("Show preview"));
                }
                else
                {
                    VideoDevice::instance().setCallback(this, previewCb);
                    ui->hidePreviewButton->setToolTip(tr("Hide preview"));
                }
            });
    switch (mType)
    {
        case CmnTypes::CALLTYPE_BROADCAST_IN:
        case CmnTypes::CALLTYPE_MON_IND_DUPLEX:
        case CmnTypes::CALLTYPE_MON_IND_PTT:
            ui->micButton->setEnabled(false);
            break;
        default:
            if (sMicOwner == 0)
            {
                ui->micButton->setChecked(true);
                sMicOwner = this;
            }
            break;
    }
    ui->speakerButton->setEnabled(false);
    ui->speakerSingleButton->setEnabled(false);
    mTimer.setInterval(1000);
    connect(&mTimer, &QTimer::timeout, this,
            [this]
            {
                //update ticking clock
                mTime = mTime.addSecs(1);
                ui->timeDurationLabel
                  ->setText(mTime.toString((mTime.hour() > 0)?
                                           "h:mm:ss": "mm:ss"));
            });
    mTime.setHMS(0, 0, 0);
    mCallTimer.setSingleShot(true);
    mPttTimer.setSingleShot(true);
    connect(&mPttTimer, &QTimer::timeout, this, [this] { onPttPress(); });
}

void CallWindow::setAudioEnabled(CallWindow::AudioType type,
                                 bool                  enabled,
                                 bool                  resetMic,
                                 int                   callParty)
{
    int cp = (callParty != 0)? callParty: getCallParty();
    if (cp == 0)
        return;
    if (type == AUDIOTYPE_OUT || type == AUDIOTYPE_BOTH)
    {
        if (mRemoteAudRtpPort != 0)
            sAudioMgr->setActiveOutRtp(cp, enabled);
        if (enabled)
        {
            if (sMicOwner != this)
            {
                ui->micButton->setChecked(true);
                CallWindow *prevMicOwner = sMicOwner;
                sMicOwner = this;
                if (prevMicOwner != 0)
                    emit activeOutCall(prevMicOwner);
            }
        }
        else if (resetMic)
        {
            ui->micButton->setChecked(false);
            if (sMicOwner == this)
                sMicOwner = 0;
        }
    }
    if (type == AUDIOTYPE_IN || type == AUDIOTYPE_BOTH)
    {
        if (mType != CmnTypes::CALLTYPE_BROADCAST_OUT)
        {
            ui->speakerButton->setChecked(enabled);
            ui->speakerButton->setEnabled(true);
            ui->speakerSingleButton->setEnabled(true);
            sAudioMgr->setActiveInRtp(cp, enabled);
            if (callParty == 0 &&
                (mType == CmnTypes::CALLTYPE_MON_IND_DUPLEX ||
                 mType == CmnTypes::CALLTYPE_MON_IND_PTT))
                sAudioMgr->setActiveInRtp(mCallingParty, enabled);
        }
    }
}

void CallWindow::stopRtp()
{
    switch (mType)
    {
        case CmnTypes::CALLTYPE_BROADCAST_IN:
        case CmnTypes::CALLTYPE_BROADCAST_OUT:
        case CmnTypes::CALLTYPE_GROUP_IN:
        case CmnTypes::CALLTYPE_GROUP_OUT:
        case CmnTypes::CALLTYPE_IND_AMBIENCE:
        case CmnTypes::CALLTYPE_IND_OUT:
        case CmnTypes::CALLTYPE_MON_AMBIENCE:
            sAudioMgr->stopRtp(mCalledParty);
            break;
        case CmnTypes::CALLTYPE_DISPATCHER:
        case CmnTypes::CALLTYPE_MOBILE:
            if (mCallId == 0) //outgoing
                sAudioMgr->stopRtp(mCalledParty);
            else
                sAudioMgr->stopRtp(mCallingParty);
            if (mVideoStream != 0)
            {
                setVideoOut(false);
                //deletion may take some time - execute in a different thread
                VideoStream *v = mVideoStream;
                QtConcurrent::run([v] { delete v; });
                mVideoStream = 0;
                ui->videoView->hide();
                ui->videoButtonFrame->hide();
                ui->previewLabel->hide();
                adjustSize();
            }
            break;
        case CmnTypes::CALLTYPE_IND_IN:
            sAudioMgr->stopRtp(mCallingParty);
            break;
        case CmnTypes::CALLTYPE_MON_IND_DUPLEX:
        case CmnTypes::CALLTYPE_MON_IND_PTT:
            sAudioMgr->stopRtp(mCallingParty);
            sAudioMgr->stopRtp(mCalledParty);
            break;
        default:
            break; //do nothing
    }
}

void CallWindow::setPttIcon(int type, bool pressed)
{
    ui->pttButton->setIcon(QtUtils::getActionIcon(type));
    ui->pttButton->setCheckable(pressed); //to indicate PTT button state
}

void CallWindow::onPttPress()
{
    setPttIcon(CmnTypes::ACTIONTYPE_PTT_PENDING, true);
    if (mCallId == 0) //starting a call
    {
        if (!mFirstPtt)
        {
            //call started but not yet established - do nothing
            return;
        }
        if (!incrCount(true))
        {
            //mark error with non-empty tooltip for onPttReleased()
            ui->closeButton->setToolTip("-");
            return;
        }
        int ssResult = 0;
        if (sSession->isLoggedIn())
        {
            switch (mType)
            {
                case CmnTypes::CALLTYPE_BROADCAST_OUT:
                    ssResult = sSession->callSetupBroadcast(mVoipId1);
                    if (ssResult > 0)
                    {
                        ui->endButton->setEnabled(true);
                        ui->closeButton->setEnabled(false);
                    }
                    break;
                case CmnTypes::CALLTYPE_GROUP_OUT:
                    ssResult = sSession->callSetupGrp(mCalledParty, mVoipId1,
                                       ((ui->preemptiveCheck->isChecked())?
                                        sPriorityPreempt: sPriorityDefault));
                    if (ssResult > 0)
                    {
                        ui->endButton->setEnabled(true);
                        if (isVisible())
                            ui->closeButton->setEnabled(false);
                    }
                    break;
                case CmnTypes::CALLTYPE_IND_OUT:
                    ssResult = sSession->callSetupInd(mCalledParty, false,
                                  (ui->categoryGroup->checkedId() == RBID_HOOK),
                                  mVoipId1,
                                  ((ui->preemptiveCheck->isChecked())?
                                   sPriorityPreempt: sPriorityDefault));
                    if (ssResult > 0)
                    {
                        ui->callButton->setEnabled(false);
                        ui->hook->setEnabled(false);
                        ui->noHook->setEnabled(false);
                        ui->ambience->setEnabled(false);
                        ui->endButton->setEnabled(true);
                        ui->closeButton->setEnabled(false);
                        //set call start time to enable a failed call record
                        //- will be overwritten on call connect
                        ui->startTimeLabel->setText(QtUtils::getTimestamp());
                    }
                    break;
                default:
                    assert("Invalid type in CallWindow::onPttPress" == 0);
                    break;
            }
        } //if (sSession->isLoggedIn())
        if (ssResult <= 0)
        {
            decrCount();
            //on PTT release, need to differentiate between failures due to max
            //calls and disconnection - use closeButton state
            if (ssResult == ServerSession::VOIPSESSION_ERROR)
                ui->closeButton->setEnabled(false);
        }
    }
    else if (mFirstPtt && mType == CmnTypes::CALLTYPE_IND_IN)
    {
        //answering incoming call
        if (!incrCount(true))
            return;
        string localAudKey;
        int localAudRtpPort = sSession->callConnect(mCallId, mCallingParty,
                                                    &localAudKey);
        if (localAudRtpPort > 0)
        {
            setPttIcon(CmnTypes::ACTIONTYPE_PTT_ACTIVE, true);
            setTxParty(mCalledPartyName);
            emit incomingConnected(mCallingParty, mCallId);
            sAudioMgr->startRtp(mCallingParty, localAudRtpPort,
                                mRemoteAudRtpPort, this, audStatCb, localAudKey,
                                mRemoteAudRtpKey);
            ui->statFrame->show();
            setAudioEnabled(AUDIOTYPE_BOTH, true);
            ui->endButton->setEnabled(true);
            if (!mTxGranted)
            {
                ui->txPartyLabel->hide();
                sSession->callTxDemand(mCallId);
            }
        }
        else
        {
            decrCount();
            //the call has been answered by another client,
            //do nothing more here - will be closed upon MON_DISCONNECT
        }
    }
#ifdef DEBUG
    //when testing outgoing group call with third party client,
    //we get CALL_PROCEEDING before pressing PTT
    else if (mFirstPtt &&
             (mType == CmnTypes::CALLTYPE_GROUP_OUT ||
              mType == CmnTypes::CALLTYPE_BROADCAST_OUT))
    {
        return;
    }
#endif
    else if (!mTxGranted && mTimer.isActive())
    {
        sSession->callTxDemand(mCallId);
    }
    //else no Tx-Demand because either already granted (perhaps after outgoing
    //non-hook call connected), or call not yet connected
}

void CallWindow::onPttRelease(bool doTxCease)
{
    setPttIcon(CmnTypes::ACTIONTYPE_PTT);
    //do nothing else if:
    //-not currently GRANTED
    //-first PTT in outgoing individual hook call
    if (!mTxGranted && mFirstPtt && mType == CmnTypes::CALLTYPE_IND_OUT &&
        ui->categoryGroup->checkedId() == RBID_HOOK)
        return;
    if (doTxCease)
        sSession->callTxCeased(mCallId);
    if (mTxGranted)
    {
        mTxGranted = false;
        ui->txPartyLabel->hide();
        setAudioEnabled(AUDIOTYPE_OUT, false, false);
    }
    mFirstPtt = false;
}

void CallWindow::onEndButton(bool isByUser)
{
    if (mTimer.isActive())
        stopRtp();
    ui->pttButton->hide();
    ui->endButton->setEnabled(false);
    ui->closeButton->setEnabled(true);
    int dispCallParty = 0;
    bool isComplete = true;
    switch (mType)
    {
        case CmnTypes::CALLTYPE_BROADCAST_IN:
        case CmnTypes::CALLTYPE_GROUP_IN:
            sSession->ssicDisconnect(mCallId, mCalledParty);
            isComplete = false;
            break;
        case CmnTypes::CALLTYPE_BROADCAST_OUT:
            sSession->callDisconnectBroadcast(mCallId);
            break;
        case CmnTypes::CALLTYPE_DISPATCHER:
        case CmnTypes::CALLTYPE_MOBILE:
            dispCallParty = (mCallId == 0)? mCalledParty: mCallingParty;
            sSession->callDisconnect(dispCallParty);
            break;
        case CmnTypes::CALLTYPE_GROUP_OUT:
            sSession->callDisconnect(mCallId, mCalledParty);
            if (!mOwner)
            {
                isComplete = false;
                mType = CmnTypes::CALLTYPE_GROUP_IN;
                ui->callButton->show();
                ui->speakerButton->setEnabled(false);
                ui->speakerSingleButton->setEnabled(false);
                hide();
            }
            break;
        case CmnTypes::CALLTYPE_IND_AMBIENCE:
        case CmnTypes::CALLTYPE_IND_OUT:
            sSession->callDisconnect(mCallId, mCalledParty);
            break;
        case CmnTypes::CALLTYPE_IND_IN:
            sSession->callDisconnect(mCallId, mCallingParty);
            break;
        case CmnTypes::CALLTYPE_MON_AMBIENCE:
            sSession->listenDisconnect(mCallId, mCalledParty);
            isComplete = false;
            break;
        case CmnTypes::CALLTYPE_MON_IND_DUPLEX:
        case CmnTypes::CALLTYPE_MON_IND_PTT:
            sSession->listenDisconnect(mCallId, mCallingParty);
            sSession->listenDisconnect(mCallId, mCalledParty);
            isComplete = false;
            break;
        default:
            return; //do nothing - should never occur
    }
    decrCount();
    if (isComplete)
        endCall();
    if (isByUser && dispCallParty != 0)
    {
        //no network call release for dispatcher call, so need to emit
        //callRelease signal for CallWindow deletion;
        //this signal may be processed synchronously, which means the destructor
        //may be invoked immediately now
        emit callRelease(dispCallParty);
    }
}

void CallWindow::endCall()
{
    //also record unanswered individual calls
    if (!mTimer.isActive() && mCalledPartyName != sUserName &&
        mType != CmnTypes::CALLTYPE_IND_OUT &&
        mType != CmnTypes::CALLTYPE_IND_AMBIENCE && !isInternalCall())
        return;
    mTimer.stop();
    mCallTimer.stop();
    ui->txPartyLabel->hide();
    QString dur;
    if (mDuration >= 0)
    {
        //monitored call
        mTime.setHMS(0, 0, 0);
        mTime = mTime.addSecs(mDuration);
        dur = mTime.toString((mTime.hour() > 0)? "h:mm:ss": "mm:ss");
    }
    else
    {
        dur = ui->timeDurationLabel->text();
        if (mCallId == 0 && !dur.isEmpty() && !mFailedCause.isEmpty())
            dur = ""; //group call started but actually failed
        else if (dur.isEmpty() && mFailedCause.isEmpty() &&
                 (mType == CmnTypes::CALLTYPE_IND_IN ||
                  (mCallId != 0 && isInternalCall())))
            mFailedCause = tr("Missed call");
        if (mType == CmnTypes::CALLTYPE_MOBILE && !dur.isEmpty())
        {
            //send to server for call record
            MsgSp m(MsgSp::Type::MON_DISCONNECT);
            m.addField(MsgSp::Field::CALL_ID, mVoipId1);
            if (mCallId == 0) //outgoing
            {
                m.addField(MsgSp::Field::CALLING_PARTY,
                           ResourceData::getId(sUserName));
                m.addField(MsgSp::Field::CALLING_PARTY_TYPE,
                           MsgSp::Value::IDENTITY_TYPE_DISPATCHER);
                m.addField(MsgSp::Field::CALLED_PARTY, mCalledParty);
                m.addField(MsgSp::Field::CALLED_PARTY_TYPE,
                           MsgSp::Value::IDENTITY_TYPE_MOBILE);
            }
            else
            {
                m.addField(MsgSp::Field::CALLING_PARTY, mCallingParty);
                m.addField(MsgSp::Field::CALLING_PARTY_TYPE,
                           MsgSp::Value::IDENTITY_TYPE_MOBILE);
                m.addField(MsgSp::Field::CALLED_PARTY,
                           ResourceData::getId(sUserName));
                m.addField(MsgSp::Field::CALLED_PARTY_TYPE,
                           MsgSp::Value::IDENTITY_TYPE_DISPATCHER);
            }
            //put start time as: <time_t> <startTimeLabel>,
            //time_t for server, the rest for client
            string s(Utils::toString(mStartTime));
            s.append(" ").append(ui->startTimeLabel->text().toStdString());
            m.addField(MsgSp::Field::TIME, s);
            //put duration as: <seconds> <dur>, for server & client
            s.assign(Utils::toString(mTime.msecsSinceStartOfDay()/1000))
             .append(" ").append(dur.toStdString());
            m.addField(MsgSp::Field::TIME_IN_TRAFFIC, s);
            sSession->sendMsg(&m, false);
        }
    }
    MessageDialog::TableDataT data;
    if (!mPttHistory.isEmpty())
    {
        data.append(QtTableUtils::getHeaders(QtTableUtils::TBLTYPE_PTT));
        for (auto &it : mPttHistory)
        {
            data.append(QStringList() << it.startTime
                        << QString::number(it.seconds) << it.txParty);
        }
        mPttHistory.clear(); //no longer needed
    }
    if (isInternalCall() && mCallId == 0) //outgoing
        emit callData(mType, mPriority, mDuplex, ui->startTimeLabel->text(),
                      dur, "", mCalledPartyName, mFailedCause, data);
    else
        emit callData(mType, mPriority, mDuplex, ui->startTimeLabel->text(),
                      dur, mCallingPartyName, mCalledPartyName, mFailedCause,
                      data);
    ui->startTimeLabel->setText(""); //prevent another call from destructor
}

bool CallWindow::incrCount(bool isPtt)
{
    //dispatcher/mobile call is not counted
    if (isInternalCall())
        return true;
    //incoming broadcast call is always allowed
    if (sCount >= MAX_COUNT && mType != CmnTypes::CALLTYPE_BROADCAST_IN)
    {
        if (!isPtt)
            showFailureMsg(this, tr("Call Not Allowed"),
                           tr("You already have the maximum %1 concurrent "
                              "sessions. End one before starting another.")
                              .arg(MAX_COUNT));
        return false;
    }
    ++sCount;
    return true;
}

inline void CallWindow::decrCount()
{
    if (!isInternalCall())
        --sCount;
}

void CallWindow::playRingTone(ToneType type)
{
    if (mRingTone != 0)
        delete mRingTone;
    switch (type)
    {
        case TONETYPE_BEEP:
            mRingTone = new QSound(SOUND_FILE_BEEP, this);
            break;
        case TONETYPE_RINGBACK:
            mRingTone = new QSound(SOUND_FILE_RBTONE, this);
            mRingTone->setLoops(QSound::Infinite);
            break;
        case TONETYPE_RINGING:
            mRingTone = new QSound(SOUND_FILE_RTONE, this);
            mRingTone->setLoops(QSound::Infinite);
            break;
        default:
            mRingTone = 0; //should never occur
            return;
    }
    mRingTone->play();
}

void CallWindow::stopRingTone()
{
    if (mRingTone != 0)
    {
        delete mRingTone;
        mRingTone = 0;
    }
}

void CallWindow::startVideo(int lclPort, const string &lclKey)
{
    static QPixmap img(":/Images/images/icon_person.png");
    QGraphicsPixmapItem *item;
    auto *scene = ui->videoView->scene();
    if (scene == 0)
    {
        scene = new QGraphicsScene(this);
        ui->videoView->setScene(scene);
        item = new QGraphicsPixmapItem(img);
        scene->addItem(item);
        connect(this, &CallWindow::drawFrame, this,
                [this, scene, item](QPixmap p)
                {
                    item->setPixmap(p);
                    if (scene->sceneRect() != item->boundingRect())
                        scene->setSceneRect(item->boundingRect());
                    ui->videoView->fitInView(item, Qt::KeepAspectRatio);
                });
    }
    else
    {
        assert(scene->items().count() == 1);
        item = static_cast<QGraphicsPixmapItem *>(scene->items().at(0));
        item->setPixmap(img);
    }
    ui->videoView->show();
    ui->videoView->setMinimumSize(640, 480);
    ui->videoButtonFrame->show();
    if (mVideoStream == 0)
        mVideoStream = new VideoStream(lclPort, mRemoteVidRtpPort, lclKey,
                                       mRemoteVidRtpKey, this, decodeCb,
                                       vidStatCb);
}

void CallWindow::setVideoOut(bool doStart)
{
    assert(mVideoStream != 0);
    VideoDevice &dev(VideoDevice::instance());
    if (!dev.isValid())
        return;
    if (doStart)
    {
        QSize sz(dev.getResolution());
        if (sz.isEmpty())
            return;
        mVideoStream->setOutgoing(true, sz.width(), sz.height());
        dev.setCallback(this, previewCb);
        //connect only once - Qt::UniqueConnection does not work for non-member
        //functions
        if (!dev.isDevSignalConnected())
            connect(&dev, &VideoDevice::newFrame, VideoStream::send);
    }
    else
    {
        mVideoStream->setOutgoing(false);
        dev.removeCallback(this);
        ui->previewLabel->hide();
    }
    dev.setCamera(this, doStart);
}
