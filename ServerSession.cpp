/**
 * Implementation of communication session with the server.
 *
 * Copyright (C) Sapura Secured Technologies, 2013-2024. All Rights Reserved.
 *
 * @file
 * @version $Id: ServerSession.cpp 1885 2024-11-28 08:32:01Z hazim.rujhan $
 * @author Zahari Hadzir
 * @author Mohd Rozaimi
 */
#include <sstream>
#include <assert.h>
#include <string.h> //memset

#ifndef NO_DB
#include "DbInt.h"
#endif
#include "Md5Digest.h"
#include "StatusCodes.h"
#include "SubsData.h"
#include "Utils.h"
#include "ServerSession.h"

using namespace std;

enum
{
    SUBSSTATE_NOT_STARTED,
    SUBSSTATE_INPROGRESS,
    SUBSSTATE_COMPLETED
};

//buffer size for socket recv
static const int BUFFER_SIZE_BYTES = 2048;
//multiplication factor to get the watchdog period from the KeepAlive period
static const int WATCHDOG_KEEPALIVE_PERIOD_FACTOR = 2;

//common static initializers
int             ServerSession::sServerIdx(SERVER_IDX_MAIN);
string          ServerSession::sMacAddresses;
string          ServerSession::sVersion;
vector<int>     ServerSession::sServerPorts;
vector<string>  ServerSession::sServerIps;
Logger         *ServerSession::sLogger(0);

//singleton static initializers
string          ServerSession::sUsername;
string          ServerSession::sPassword;
#ifdef _WIN32
PalLock::LockT  ServerSession::sSingletonLock; //no init needed
#elif defined QT_CORE_LIB
PalLock::LockT  ServerSession::sSingletonLock(QMutex::Recursive);
#else
PalLock::LockT  ServerSession::sSingletonLock = PTHREAD_MUTEX_INITIALIZER;
#endif
ServerSession  *ServerSession::sInstance(0);
void           *ServerSession::sCbObj(0);
ServerSession::RecvCallbackFn ServerSession::sCbFn(0);

static void *startRecvThread(void *arg)
{
    static_cast<ServerSession *>(arg)->recvThread();
    return 0;
}

ServerSession::ServerSession(const string   &username,
                             const string   &password,
                             const string   &branches,
                             void           *callbackObj,
                             RecvCallbackFn  callbackFn
#ifdef TESTCLIENT
                             , bool          doSubsData
#endif
                            ) :
mState(STATE_INVALID), mMessageId(MsgSp::Value::MSG_ID_MIN - 1),
#ifdef TESTCLIENT
mDoSubsData(doSubsData),
#endif
mRecvTime(0), mSentTime(0), mUsername(username), mPassword(password),
mBranches(branches), mRecvThread(0), mVoipSession(0), mSocket(0),
mCbObj(callbackObj), mCbFn(callbackFn)
{
    start();
}

ServerSession::~ServerSession()
{
    if (isLoggedIn())
    {
        MsgSp m(MsgSp::Type::LOGOUT);
        m.addField(MsgSp::Field::USERNAME, mUsername);
        sendMsg(&m, false);
    }
    mState = STATE_STOPPED;
    delete mVoipSession;
    delete mSocket;
    if (mRecvThread != 0)
        PalThread::stop(mRecvThread);
    PalLock::destroy(&mSendMsgLock);
#ifndef NO_DB
    DbInt::destroy();
#endif
    LOGGER_INFO(sLogger, mLogPrefix << "Destroyed.");
}

ostream &operator<<(ostream &os, const ServerSession &obj)
{
    return os << obj.mName;
}

void ServerSession::setBranches(const string *branches)
{
    if (branches != 0)
    {
        if (*branches == mBranches)
            return; //no change
        mBranches = *branches;
    }
    if (isLoggedIn() && (branches != 0 || !mBranches.empty()))
    {
        MsgSp m(MsgSp::Type::BRANCH_DATA);
        if (!mBranches.empty() && mBranches[0] != '-')
            m.addField(MsgSp::Field::GRP_LIST, mBranches);
        sendMsg(&m, false);
        if (branches != 0)
            requestSubsData();
    }
}

bool ServerSession::monitorStart(int ssi, bool isGroup)
{
    if (!isLoggedIn())
        return false;
    if (ssi <= 0)
    {
        LOGGER_ERROR(sLogger, mLogPrefix << "monitorStart: Invalid SSI "
                     << ssi);
        return false;
    }
    sendMon(MsgSp::Type::MON_START, isGroup, 0, ssi);
    return true;
}

bool ServerSession::monitorStart(const SsiSetT &ssiSet, bool isGroup)
{
    if (!isLoggedIn())
        return false;
    if (ssiSet.empty())
    {
        LOGGER_ERROR(sLogger, mLogPrefix << "monitorStart: Empty SSI list.");
        return false;
    }
    sendMon(MsgSp::Type::MON_START, isGroup, &ssiSet);
    return true;
}

bool ServerSession::monitorStop(int ssi, bool isGroup)
{
    if (!isLoggedIn())
        return false;
    if (ssi <= 0)
    {
        LOGGER_ERROR(sLogger, mLogPrefix << "monitorStop: Invalid SSI "
                     << ssi);
        return false;
    }
    sendMon(MsgSp::Type::MON_STOP, isGroup, 0, ssi);
    return true;
}

bool ServerSession::monitorStop(const SsiSetT &ssiSet, bool isGroup)
{
    if (!isLoggedIn())
        return false;
    if (ssiSet.empty())
    {
        LOGGER_ERROR(sLogger, mLogPrefix << "monitorStop: Empty SSI list.");
        return false;
    }
    sendMon(MsgSp::Type::MON_STOP, isGroup, &ssiSet);
    return true;
}

bool ServerSession::monitorStop()
{
    if (!isLoggedIn())
        return false;
    sendMsg(new MsgSp(MsgSp::Type::MON_STOP));
    return true;
}

bool ServerSession::gpsMonitorStart(const SsiSetT &ssiSet)
{
    MsgSp m(MsgSp::Type::GPS_MON_START);
    if (!ssiSet.empty())
        m.addField(MsgSp::Field::ISSI_LIST, Utils::toStringWithRange(ssiSet));
    return (sendMsg(&m, false) > 0);
}

bool ServerSession::gpsMonitorStop(const SsiSetT &ssiSet)
{
    MsgSp m(MsgSp::Type::GPS_MON_STOP);
    if (!ssiSet.empty())
        m.addField(MsgSp::Field::ISSI_LIST, Utils::toStringWithRange(ssiSet));
    return (sendMsg(&m, false) > 0);
}

bool ServerSession::requestStatusData()
{
    if (!isLoggedIn())
        return false;
    if (StatusCodes::setStateDownloading(true))
    {
        MsgSp m(MsgSp::Type::STATUS_DATA);
        m.addField(MsgSp::Field::TIMESTAMP, StatusCodes::getTimestamp());
        sendMsg(&m, false);
    }
    return true;
}

bool ServerSession::requestSubsData()
{
    if (!isLoggedIn())
        return false;
#ifdef TESTCLIENT
    if (!mDoSubsData || SubsData::setStateDownloading())
#else
    if (SubsData::setStateDownloading())
#endif
    {
        MsgSp m(MsgSp::Type::SUBS_DATA);
        m.addField(MsgSp::Field::TIMESTAMP, SubsData::getTimestamp());
        sendMsg(&m, false);
    }
    return true;
}

bool ServerSession::changePasswd(const string &oldPasswd,
                                 const string &newPasswd)
{
    if (!isLoggedIn())
        return false;
    mOldPassword = oldPasswd;
    mNewPassword = newPasswd;
    sendMsg(new MsgSp(MsgSp::Type::CHANGE_PASSWORD));
    return true;
}

int ServerSession::sds(int ssiType, int ssi, const string &msgStr, int coding)
{
    if (!isLoggedIn())
        return 0;
    MsgSp m(MsgSp::Type::SDS_TRANSFER);
    m.addField(MsgSp::Field::CALLED_PARTY_TYPE, ssiType);
    m.addField(MsgSp::Field::CALLED_PARTY, ssi);
    m.addField(MsgSp::Field::SHORT_DATA_TYPE, MsgSp::Value::SDS_TYPE_4);
    m.addField(MsgSp::Field::PROTOCOL_ID,
               MsgSp::Value::PROTOCOL_TEXT_MSG_SDS_TL);
    m.addField(MsgSp::Field::DELIVERY_RPT_REQ, MsgSp::Value::DEL_RPT_REQ_NONE);
    m.addField(MsgSp::Field::STORAGE, MsgSp::Value::STORAGE_ALLOWED_NO);
    m.setSdsText(msgStr, coding);
    return sendMsg(&m, false);
}

int ServerSession::sds(IntListT     &ssiTypes,
                       IntListT     &ssis,
                       const string &msgStr,
                       int           coding)
{
    if (!isLoggedIn())
        return 0;
    MsgSp m(MsgSp::Type::SDS_TRANSFER);
    m.addField(MsgSp::Field::CALLED_PARTY_TYPE,
               Utils::toString(ssiTypes, MsgSp::Value::LIST_DELIMITER));
    m.addField(MsgSp::Field::CALLED_PARTY,
               Utils::toString(ssis, MsgSp::Value::LIST_DELIMITER));
    m.addField(MsgSp::Field::SHORT_DATA_TYPE, MsgSp::Value::SDS_TYPE_4);
    m.addField(MsgSp::Field::PROTOCOL_ID,
               MsgSp::Value::PROTOCOL_TEXT_MSG_SDS_TL);
    m.addField(MsgSp::Field::DELIVERY_RPT_REQ, MsgSp::Value::DEL_RPT_REQ_NONE);
    m.addField(MsgSp::Field::STORAGE, MsgSp::Value::STORAGE_ALLOWED_NO);
    m.setSdsText(msgStr, coding);
    return sendMsg(&m, false);
}

#ifdef TESTCLIENT
int ServerSession::sdsGps(int issi, const string &msgStr)
{
    if (!isLoggedIn())
        return 0;
    MsgSp m(MsgSp::Type::SDS_TRANSFER);
    m.addField(MsgSp::Field::CALLED_PARTY_TYPE,
               MsgSp::Value::IDENTITY_TYPE_ISSI);
    m.addField(MsgSp::Field::CALLED_PARTY, issi);
    m.addField(MsgSp::Field::SHORT_DATA_TYPE, MsgSp::Value::SDS_TYPE_4);
    m.addField(MsgSp::Field::PROTOCOL_ID, MsgSp::Value::PROTOCOL_SIMPLE_GPS);
    m.setSdsText(msgStr, MsgSp::Value::TEXT_CODING_7BIT);
    return sendMsg(&m, false);
}
#endif //TESTCLIENT

int ServerSession::status(int ssiType, int ssi, int statusVal)
{
    if (!isLoggedIn())
        return 0;
    MsgSp m(MsgSp::Type::STATUS);
    m.addField(MsgSp::Field::CALLED_PARTY_TYPE, ssiType);
    m.addField(MsgSp::Field::CALLED_PARTY, ssi);
    m.addField(MsgSp::Field::STATUS_CODE, statusVal);
    return sendMsg(&m, false);
}

int ServerSession::status(IntListT &ssiTypes, IntListT &ssis, int statusVal)
{
    if (!isLoggedIn())
        return 0;
    MsgSp m(MsgSp::Type::STATUS);
    m.addField(MsgSp::Field::CALLED_PARTY_TYPE,
               Utils::toString(ssiTypes, MsgSp::Value::LIST_DELIMITER));
    m.addField(MsgSp::Field::CALLED_PARTY,
               Utils::toString(ssis, MsgSp::Value::LIST_DELIMITER));
    m.addField(MsgSp::Field::STATUS_CODE, statusVal);
    return sendMsg(&m, false);
}

int ServerSession::callSetupGrp(int ssi, string &voipId, int priority)
{
    int retVal = checkVoipSession();
    if (retVal != 0)
        return retVal;
    MsgSp m(MsgSp::Type::CALL_SETUP);
    m.addField(MsgSp::Field::COMM_TYPE,
               MsgSp::Value::COMM_TYPE_POINT_TO_MULTIPOINT);
    m.addField(MsgSp::Field::SIMPLEX_DUPLEX,
               MsgSp::Value::SIMPLEX_DUPLEX_SIMPLEX);
    m.addField(MsgSp::Field::HOOK_METHOD, MsgSp::Value::HOOK_NO);
    m.addField(MsgSp::Field::CALLING_PARTY, mUsername);
    m.addField(MsgSp::Field::CIRCUIT_MODE_TYPE, MsgSp::Value::CM_TYPE_SPEECH);
    m.addField(MsgSp::Field::ENCR_FLAG, MsgSp::Value::ENCR_FLAG_CLEAR);
    m.addField(MsgSp::Field::REQ_TO_TX, MsgSp::Value::REQUEST_TX_SELF);
    m.addField(MsgSp::Field::CALL_PRIORITY, priority);
    m.addField(MsgSp::Field::CLIR_CTRL, MsgSp::Value::CLIR_NOT_IMPLEMENTED);
    m.addField(MsgSp::Field::CALLED_PARTY_TYPE,
               MsgSp::Value::IDENTITY_TYPE_GSSI);
    m.addField(MsgSp::Field::CALLED_PARTY, ssi);
    //even in !NOSIPSETUP build, still need to send direct msg in case ssi is
    //unmonitored and already in a call involving other clients
    retVal = sendMsg(&m, false);
    if (retVal > 0)
        voipId = mVoipSession->callSetupGrp(ssi, priority);
    return retVal;
}

int ServerSession::callSetupInd(int     ssi,
                                bool    isDuplex,
                                bool    isHook,
                                string &voipId,
                                int     priority)
{
    int retVal = checkVoipSession();
    if (retVal != 0)
        return retVal;
    MsgSp m(MsgSp::Type::CALL_SETUP);
    m.addField(MsgSp::Field::COMM_TYPE,
               MsgSp::Value::COMM_TYPE_POINT_TO_POINT);
    m.addField(MsgSp::Field::SIMPLEX_DUPLEX,
               (isDuplex)? MsgSp::Value::SIMPLEX_DUPLEX_DUPLEX:
                           MsgSp::Value::SIMPLEX_DUPLEX_SIMPLEX);
    m.addField(MsgSp::Field::HOOK_METHOD,
               (isHook)? MsgSp::Value::HOOK_YES: MsgSp::Value::HOOK_NO);
    m.addField(MsgSp::Field::CALLING_PARTY, mUsername);
    m.addField(MsgSp::Field::CIRCUIT_MODE_TYPE, MsgSp::Value::CM_TYPE_SPEECH);
    m.addField(MsgSp::Field::ENCR_FLAG, MsgSp::Value::ENCR_FLAG_CLEAR);
    m.addField(MsgSp::Field::REQ_TO_TX, MsgSp::Value::REQUEST_TX_SELF);
    m.addField(MsgSp::Field::CALL_PRIORITY, priority);
    m.addField(MsgSp::Field::CLIR_CTRL, MsgSp::Value::CLIR_NOT_IMPLEMENTED);
    m.addField(MsgSp::Field::CALLED_PARTY_TYPE,
               MsgSp::Value::IDENTITY_TYPE_ISSI);
    m.addField(MsgSp::Field::CALLED_PARTY, ssi);
#ifdef NOSIPSETUP
    retVal = sendMsg(&m, false);
    if (retVal > 0)
    {
        if (isDuplex)
            voipId = mVoipSession->callSetupIndDuplex(ssi);
        else
            voipId = mVoipSession->callSetupIndPtt(ssi);
    }
    return retVal;
#else
    m.setMsgId();
    LOGGER_VERBOSE(sLogger, mLogPrefix << "VOIP Tx\n" << m);
    if (isDuplex)
        voipId = mVoipSession->callSetupIndDuplex(ssi, m.sipSerialize());
    else
        voipId = mVoipSession->callSetupIndPtt(ssi, m.sipSerialize());
    return 1;
#endif
}

int ServerSession::callSetupInd(int           id,
                                string       &voipId,
                                const string &calledDomain)
{
    int retVal = checkVoipSession();
    if (retVal != 0)
        return retVal;
    voipId = mVoipSession->callSetupIndDisp(id, calledDomain);
    return 1;  //any positive value
}

int ServerSession::callSetupVideo(int           id,
                                  string       &voipId,
                                  const string &calledDomain)
{
    int retVal = checkVoipSession();
    if (retVal != 0)
        return retVal;
    voipId = mVoipSession->callSetupVideo(id, calledDomain);
    return 1;  //any positive value
}

int ServerSession::callSetupAmbience(int ssi, string &voipId, int priority)
{
    int retVal = checkVoipSession();
    if (retVal != 0)
        return retVal;
    MsgSp m(MsgSp::Type::CALL_SETUP);
    m.addField(MsgSp::Field::HOOK_METHOD, MsgSp::Value::HOOK_NO);
    m.addField(MsgSp::Field::SIMPLEX_DUPLEX,
               MsgSp::Value::SIMPLEX_DUPLEX_SIMPLEX);
    m.addField(MsgSp::Field::COMM_TYPE,
               MsgSp::Value::COMM_TYPE_AMBIENCE_LISTENING);
    m.addField(MsgSp::Field::CALLING_PARTY, mUsername);
    m.addField(MsgSp::Field::CIRCUIT_MODE_TYPE, MsgSp::Value::CM_TYPE_SPEECH);
    m.addField(MsgSp::Field::ENCR_FLAG, MsgSp::Value::ENCR_FLAG_CLEAR);
    m.addField(MsgSp::Field::REQ_TO_TX, MsgSp::Value::REQUEST_TX_OTHER);
    m.addField(MsgSp::Field::CALL_PRIORITY, priority);
    m.addField(MsgSp::Field::CLIR_CTRL, MsgSp::Value::CLIR_NOT_IMPLEMENTED);
    m.addField(MsgSp::Field::CALLED_PARTY_TYPE,
               MsgSp::Value::IDENTITY_TYPE_ISSI);
    m.addField(MsgSp::Field::CALLED_PARTY, ssi);
#ifdef NOSIPSETUP
    retVal = sendMsg(&m, false);
    if (retVal > 0)
        voipId = mVoipSession->callSetupAmbience(ssi);
    return retVal;
#else
    m.setMsgId();
    LOGGER_VERBOSE(sLogger, mLogPrefix << "VOIP Tx\n" << m);
    voipId = mVoipSession->callSetupAmbience(ssi, m.sipSerialize());
    return 1;
#endif
}

int ServerSession::callSetupBroadcast(string &voipId)
{
    int retVal = checkVoipSession();
    if (retVal != 0)
        return retVal;
#ifdef NOSIPSETUP
    MsgSp m(MsgSp::Type::CALL_SETUP);
    m.addField(MsgSp::Field::COMM_TYPE, MsgSp::Value::COMM_TYPE_BROADCAST);
    m.addField(MsgSp::Field::CALLED_PARTY, BROADCAST_SSI);
    m.addField(MsgSp::Field::ENCR_FLAG, MsgSp::Value::ENCR_CTRL_CLEAR);
    retVal = sendMsg(&m, false);
    if (retVal > 0)
        voipId = mVoipSession->callSetupBroadcast(BROADCAST_SSI);
    return retVal;
#else
    voipId = mVoipSession->callSetupBroadcast(BROADCAST_SSI);
    return 1;
#endif
}

int ServerSession::callConnect(int callId, int ssi, string *audKey)
{
    if (mVoipSession == 0)
        return 0;
    MsgSp m(MsgSp::Type::CALL_CONNECT);
    m.addField(MsgSp::Field::CALL_ID, callId);
    if (sendMsg(&m, false) == 0)
        return 0;
    return mVoipSession->callConnect(ssi, audKey);
}

int ServerSession::callConnect(int     id,
                               string *audKey,
                               int    *vidPort,
                               string *vidKey)
{
    if (mVoipSession == 0)
        return 0;
    return mVoipSession->callConnect(id, audKey, vidPort, vidKey);
}

int ServerSession::callDisconnect(int callId, int ssi)
{
    int retVal = 0;
    if (isLoggedIn())
    {
        MsgSp m(MsgSp::Type::CALL_DISCONNECT);
        m.addField(MsgSp::Field::CALL_ID, callId);
        if (callId == 0) //outgoing only
            m.addField(MsgSp::Field::CALLED_PARTY, ssi);
        m.addField(MsgSp::Field::DISCONNECT_CAUSE,
                   MsgSp::Value::DC_USER_REQUESTED);
        retVal = sendMsg(&m, false);
    }
    if (mVoipSession != 0)
        mVoipSession->callRelease(ssi);
    return retVal;
}

int ServerSession::callDisconnect(int id)
{
    if (mVoipSession == 0 || !mVoipSession->callRelease(id))
        return 0;
    return 1;  //any positive value
}

int ServerSession::callDisconnectBroadcast(int callId)
{
    int retVal = 0;
    if (isLoggedIn())
    {
        MsgSp m(MsgSp::Type::CALL_DISCONNECT);
        m.addField(MsgSp::Field::CALL_ID, callId);
        m.addField(MsgSp::Field::DISCONNECT_CAUSE,
                   MsgSp::Value::DC_USER_REQUESTED);
        retVal = sendMsg(&m, false);
    }
    if (mVoipSession != 0)
        mVoipSession->callRelease(BROADCAST_SSI);
    return retVal;
}

int ServerSession::callTxDemand(int callId, int priority)
{
    MsgSp m(MsgSp::Type::CALL_TX_DEMAND);
    m.addField(MsgSp::Field::CALL_ID, callId);
    m.addField(MsgSp::Field::TX_DEMAND_PRIORITY, priority);
    m.addField(MsgSp::Field::ENCR_CTRL, MsgSp::Value::ENCR_FLAG_CLEAR);
    return sendMsg(&m, false);
}

int ServerSession::callTxCeased(int callId)
{
    MsgSp m(MsgSp::Type::CALL_TX_CEASED);
    m.addField(MsgSp::Field::CALL_ID, callId);
    return sendMsg(&m, false);
}

int ServerSession::callTerminate(int callId)
{
    MsgSp m(MsgSp::Type::CALL_FINISH);
    m.addField(MsgSp::Field::CALL_ID, callId);
    return sendMsg(&m, false);
}

int ServerSession::ssicInvoke(int     callId,
                              int     gssi,
                              int     callingParty,
                              bool    isDisp,
                              string &voipId)
{
    int retVal = checkVoipSession();
    if (retVal != 0)
        return retVal;
    MsgSp m(MsgSp::Type::SSIC_INVOKE);
    m.addField(MsgSp::Field::CALL_ID, callId);
    m.addField(MsgSp::Field::CALLED_PARTY, gssi);
    m.addField(MsgSp::Field::CALLING_PARTY, callingParty);
    m.addField(MsgSp::Field::CALLING_PARTY_TYPE,
               (isDisp)? MsgSp::Value::IDENTITY_TYPE_DISPATCHER:
                         (SubsData::isMobile(callingParty))?
                             MsgSp::Value::IDENTITY_TYPE_MOBILE:
                             MsgSp::Value::IDENTITY_TYPE_ISSI);
    retVal = sendMsg(&m, false);
    if (retVal > 0)
        voipId = mVoipSession->callSetupSsic(gssi, callingParty, isDisp);
    return retVal;
}

int ServerSession::ssicDisconnect(int callId, int gssi)
{
    int retVal = 0;
    if (isLoggedIn())
    {
        MsgSp m(MsgSp::Type::SSIC_DISCONNECT);
        m.addField(MsgSp::Field::CALL_ID, callId);
        retVal = sendMsg(&m, false);
    }
    if (mVoipSession != 0)
        mVoipSession->callRelease(gssi);
    return retVal;
}

int ServerSession::listenConnect(int callId, int issi, string &voipId)
{
    int retVal = checkVoipSession();
    if (retVal != 0)
        return retVal;
    MsgSp m(MsgSp::Type::LISTEN_CONNECT);
    m.addField(MsgSp::Field::CALL_ID, callId);
    m.addField(MsgSp::Field::CALLED_PARTY, issi);
    retVal = sendMsg(&m, false);
    if (retVal > 0)
        voipId = mVoipSession->callSetupListen(issi);
    return retVal;
}

int ServerSession::listenDisconnect(int callId, int issi)
{
    int retVal = 0;
    if (isLoggedIn())
    {
        MsgSp m(MsgSp::Type::LISTEN_DISCONNECT);
        m.addField(MsgSp::Field::CALL_ID, callId);
        m.addField(MsgSp::Field::CALLED_PARTY, issi);
        retVal = sendMsg(&m, false);
    }
    if (mVoipSession != 0)
        mVoipSession->callRelease(issi);
    return retVal;
}

void ServerSession::listenDisconnect(int issi)
{
    if (mVoipSession != 0)
        mVoipSession->callRelease(issi);
}

int ServerSession::ssDgnaAssign(int  gssi,
                                int  ssi,
                                bool isIndiv,
                                bool doAssign)
{
    if (!isLoggedIn())
        return 0;
    MsgSp m((doAssign)? MsgSp::Type::SSDGNA_ASSIGN:
                        MsgSp::Type::SSDGNA_DEASSIGN);
    m.addField(MsgSp::Field::GSSI, gssi);
    m.addField(MsgSp::Field::AFFECTED_USER, ssi);
    m.addField(MsgSp::Field::AFFECTED_USER_TYPE,
               (isIndiv)? MsgSp::Value::IDENTITY_TYPE_ISSI:
                          MsgSp::Value::IDENTITY_TYPE_GSSI);
    return sendMsg(&m, false);
}

int ServerSession::incidentUpdate(int id, const string &editData)
{
    MsgSp m(MsgSp::Type::INCIDENT_UPDATE);
    m.addField(MsgSp::Field::INCIDENT_ID, id);
    if (!editData.empty())
        m.addField(MsgSp::Field::USER_DATA, editData);
    return sendMsg(&m, false);
}

int ServerSession::incidentLock(int id, bool doLock)
{
    MsgSp m(MsgSp::Type::INCIDENT_LOCK);
    m.addField(MsgSp::Field::INCIDENT_ID, id);
    m.addField(MsgSp::Field::LOCK_ACTION,
               (doLock)? MsgSp::Value::LOCK_ACTION_LOCK:
                         MsgSp::Value::LOCK_ACTION_UNLOCK);
    return sendMsg(&m, false);
}

int ServerSession::poiUpdate(int id, bool doDelete)
{
    MsgSp m((doDelete)? MsgSp::Type::POI_DELETE: MsgSp::Type::POI_UPDATE);
    m.addField(MsgSp::Field::ID, id);
    return sendMsg(&m, false);
}

int ServerSession::sendMsg(MsgSp *msg, bool deleteMsg)
{
    if (msg == 0)
    {
        assert("Bad param in ServerSession::sendMsg" == 0);
        return 0;
    }
    PalLock::take(&mSendMsgLock);
    if (++mMessageId > MsgSp::Value::MSG_ID_MAX)
        mMessageId = MsgSp::Value::MSG_ID_MIN;
    int msgId = mMessageId;
    msg->addField(MsgSp::Field::MSG_ID, msgId);
    int res;
    if (msg->getType() == MsgSp::Type::LOGIN && !mMsgKey.empty())
    {
        msg->addField(MsgSp::Field::DESC, mIpAndPort);
        res = mSocket->send(msg->serialize(
                        MsgSp::getKey(MsgSp::getTypeName(MsgSp::Type::LOGIN))));
    }
    else
    {
        res = mSocket->send(msg->serialize(mMsgKey));
    }
    if (res > 0)
    {
        mSentTime = time(NULL);
        //need mutex only up to modifying mSentTime
        PalLock::release(&mSendMsgLock);
        switch (msg->getType())
        {
#ifndef DEBUG
            case MsgSp::Type::CHANGE_PASSWORD:
            case MsgSp::Type::LOGIN:
                //do not show message content in release build log
                LOGGER_VERBOSE(sLogger, mLogPrefix << "Tx\n"
                               << msg->getName());
                break;
            case MsgSp::Type::PASSWORD:
                //do not show at all in release build log
                break;
#endif
            case MsgSp::Type::SYS_KEEPALIVE:
                LOGGER_DEBUG2(sLogger, mLogPrefix << "Tx\n" << *msg);
                break;
            default:
                LOGGER_VERBOSE(sLogger, mLogPrefix << "Tx\n" << *msg);
                break;
        }
    }
    else
    {
        LOGGER_ERROR(sLogger, mLogPrefix << "Error " << res
                     << Socket::getErrorStr(-res) << " sending message "
                     << msg->getName());
        PalLock::release(&mSendMsgLock);
        msgId = res;
    }
    if (deleteMsg)
        delete msg;
    return msgId;
}

string ServerSession::getServerAddress(bool withPort) const
{
    if (isLoggedIn())
        return (withPort)? mSocket->getRemoteAddrStr(): mSocket->getRemoteIp();
    return "";
}

void ServerSession::setEncryption(bool enable)
{
    if (!enable)
        mMsgKey.clear();
    else if (mIpAndPort.empty())
        mMsgKey = "1"; //temporary until connected, just to make it non-empty
    else
        mMsgKey = MsgSp::getKey(mIpAndPort + mUsername);
}

void ServerSession::recvThread()
{
    assert(mCbObj != 0 && mCbFn != 0);
    LOGGER_DEBUG(sLogger, mLogPrefix << "recvThread started");
    if (!connectToServer())
        return;

    int     bytesRcvd;
    int     len;
    int     result;
    int     keepAlivePeriod = 0;
    int     watchdogPeriod  = 0;
    int     timeout         = 0;
    bool    doCallback;
    string  challenge;
    string  str;
    string  valStr;
    MsgSp   msgKeepAlive(MsgSp::Type::SYS_KEEPALIVE);
    MsgSp  *msg;
    MsgSp  *resp = 0;
    char    buf[BUFFER_SIZE_BYTES];

    while (mState != STATE_STOPPED)
    {
        if (keepAlivePeriod > 0)
        {
            timeout = keepAlivePeriod - (time(NULL) - mSentTime);
            if (timeout <= 0)
            {
                sendMsg(&msgKeepAlive, false);
                timeout = keepAlivePeriod;
            }
        }
        memset(buf, 0, sizeof(buf));
        bytesRcvd = mSocket->recv(buf, sizeof(buf), timeout);
        if (mState == STATE_STOPPED)
        {
            LOGGER_DEBUG(sLogger, mLogPrefix << "recvThread stopped");
            break;
        }
        if (bytesRcvd == TcpSocket::ERR_TIMEOUT)
        {
            if (time(NULL) - mRecvTime >= watchdogPeriod)
            {
                //timeout - server still connected but not sending
                LOGGER_ERROR(sLogger, mLogPrefix << "Server timeout "
                            << time(NULL) - mRecvTime << " seconds.");
                mCbFn(mCbObj, new MsgSp(MsgSp::Type::REMOTE_SERVER_TIMEOUT));
                if (isLoggedIn())
                {
                    MsgSp m(MsgSp::Type::LOGOUT);
                    m.addField(MsgSp::Field::USERNAME, mUsername);
                    sendMsg(&m, false);
                }
                mState = STATE_DISCONNECTED;
                StatusCodes::setStateDownloading(false);
                setName();
                str.clear();
                if (!connectToServer())
                    return;
            }
            else if (time(NULL) - mSentTime >= keepAlivePeriod)
            {
                sendMsg(&msgKeepAlive, false);
            }
            continue;
        }
        if (bytesRcvd <= 0)
        {
            if (Socket::isDisconnectedError(-bytesRcvd))
            {
                setState(STATE_DISCONNECTED);
                LOGGER_ERROR(sLogger, mLogPrefix << "recvThread: Error "
                             << bytesRcvd << Socket::getErrorStr(-bytesRcvd)
                             << ". Server disconnected. Reconnecting...");
                mCbFn(mCbObj,
                      new MsgSp(MsgSp::Type::REMOTE_SERVER_DISCONNECTED));
                StatusCodes::setStateDownloading(false);
                setName();
                str.clear();
                if (!connectToServer())
                    return;
            }
            else
            {
                LOGGER_DEBUG(sLogger, mLogPrefix << "recvThread: Error "
                             << bytesRcvd << Socket::getErrorStr(-bytesRcvd)
                             << " receiving message.");
            }
            continue;
        }

        mRecvTime = time(NULL);
        len = MsgSp::getMsgLen(str.append(buf, bytesRcvd));
        while ((int) str.size() >= len + MsgSp::LEN_SIZE) //cast for compiler
        {
            valStr = str.substr(MsgSp::LEN_SIZE, len); //extract msg
            str.erase(0, len + MsgSp::LEN_SIZE); //erase consumed part
            len = MsgSp::getMsgLen(str); //for next block
            msg = MsgSp::parse(valStr, mMsgKey);
            if (msg == 0)
            {
                LOGGER_ERROR(sLogger, mLogPrefix
                             << "Message parsing/decryption failed on\n"
                             << Utils::toHexString(valStr));
                continue;
            }
            doCallback = true;
            switch (msg->getType())
            {
#ifndef DEBUG
                case MsgSp::Type::CHANGE_PASSWORD:
                    //do not show message content in release build log
                    LOGGER_VERBOSE(sLogger, mLogPrefix << "Rx\n"
                                   << msg->getName());
                    break;
                case MsgSp::Type::LOGIN:
                case MsgSp::Type::PASSWORD:
                    //do not show at all in release build log
                    break;
#endif
                case MsgSp::Type::GPS_LOC:
                    LOGGER_DEBUG(sLogger, mLogPrefix << "Rx\n" << *msg);
                    break;
                case MsgSp::Type::SYS_KEEPALIVE:
                    LOGGER_DEBUG2(sLogger, mLogPrefix << "Rx\n" << *msg);
                    break;
                default:
                    LOGGER_VERBOSE(sLogger, mLogPrefix << "Rx\n" << *msg);
                    break;
            }

            switch (msg->getType())
            {
                case MsgSp::Type::BRANCH_DATA:
                {
                    if (msg->isResultSuccessful())
                        SubsData::clientInit(msg);
                    break;
                }

                case MsgSp::Type::CHANGE_PASSWORD:
                {
                    valStr = msg->getFieldString(MsgSp::Field::CHALLENGE);
                    if (!valStr.empty())
                    {
                        doCallback = false;
                        string passwd(mOldPassword);
                        md5Digest(passwd, valStr);
                        msg->reset();   //ready for reuse as response
                        resp = msg;
                        msg = 0;
                        resp->addField(MsgSp::Field::PASSWORD, passwd);
                        resp->addField(MsgSp::Field::NEW_PASSWORD,
                                       MsgSp::hexScramble(mNewPassword,
                                                          valStr));
                    }
                    else
                    {
                        if (msg->isResultSuccessful())
                            LOGGER_INFO(sLogger, mLogPrefix
                                        << "Password changed.");
                        else
#ifdef DEBUG
                            LOGGER_ERROR(sLogger, mLogPrefix
                                         << msg->getName() << " failed, "
                                         << msg->getFieldValueString(
                                                      MsgSp::Field::RESULT));
#else
                            LOGGER_ERROR(sLogger, mLogPrefix
                                         << msg->getName() << " failed.");
#endif
                    }
                    break;
                }

                case MsgSp::Type::LOGIN:
                {
                    doCallback = false;
                    string passwd(mPassword);
                    challenge.assign(
                                  msg->getFieldString(MsgSp::Field::CHALLENGE));
                    md5Digest(passwd, challenge);
                    resp = new MsgSp(MsgSp::Type::PASSWORD);
                    resp->addField(MsgSp::Field::USERNAME, mUsername);
                    resp->addField(MsgSp::Field::PASSWORD, passwd);
                    resp->addField(MsgSp::Field::VOIP_GW,
                                   sServerIps[sServerIdx]);
                    resp->addField(MsgSp::Field::MAC_ADDRESSES, sMacAddresses);
                    resp->addField(MsgSp::Field::VERSION, sVersion);
                    //this must match related code in ClientSession::recv()
                    if (!mMsgKey.empty())
                        mMsgKey = MsgSp::getKey(
                                        Utils::scramble(mSocket->getLocalPort(),
                                                        challenge, mMsgKey));
                    break;
                }

                case MsgSp::Type::LOGOUT:
                {
                    if (msg->isResultSuccessful())
                    {
                        doCallback = false;
                        setState(STATE_CONNECTED);
                        LOGGER_DEBUG(sLogger, mLogPrefix << "Logged out.");
                    }
                    break;
                }

                case MsgSp::Type::MON_GRP_ATTACH_DETACH:
                case MsgSp::Type::MON_SSDGNA_ASSIGN:
                case MsgSp::Type::MON_SSDGNA_DEASSIGN:
                case MsgSp::Type::MON_SSDGNA_DEFINE:
                case MsgSp::Type::MON_SSDGNA_DELETE:
                case MsgSp::Type::MON_SUBS_ASSIGN:
                case MsgSp::Type::MON_SUBS_DEASSIGN:
                case MsgSp::Type::MON_SUBS_DEFINE:
                case MsgSp::Type::MON_SUBS_DELETE:
                case MsgSp::Type::MON_SUBS_DESC:
                case MsgSp::Type::MON_SUBS_PERMISSION:
                {
#ifdef TESTCLIENT
                    if (!mDoSubsData || !SubsData::processMsg(msg))
#else
                    if (!SubsData::processMsg(msg))
#endif
                    {
                        //msg either copied and queued, or discarded
                        delete msg;
                        msg = 0;
                        doCallback = false;
                    }
                    break;
                }

                case MsgSp::Type::MON_LOC:
                {
#ifdef TESTCLIENT
                    if (mDoSubsData)
#endif
                        SubsData::processMsg(msg);
                    doCallback = false;
                    break;
                }

                case MsgSp::Type::MON_START:
                case MsgSp::Type::MON_STOP:
                {
                    if (msg->getFieldInt(MsgSp::Field::CALLING_PARTY_TYPE) ==
                        MsgSp::Value::IDENTITY_TYPE_MOBILE)
                    {
                        //from STM - mobile grp attach/detach
                        SubsData::processMsg(msg);
                        //change type for callback, containing only the calling
                        //party and the grp ssi list in different fields, to
                        //enable grp active state updates
                        string s(msg->getFieldString(
                                                 MsgSp::Field::CALLING_PARTY));
                        valStr = msg->getFieldString(MsgSp::Field::SSI_LIST);
                        msg->reset(MsgSp::Type::MON_GRP_ATTACH_DETACH)
                            .addField(MsgSp::Field::ISSI, s)
                            .addField(MsgSp::Field::GRP_LIST, valStr);
                    }
                    break;
                }

                case MsgSp::Type::PASSWORD:
                {
                    if (msg->isResultSuccessful())
                    {
                        msg->removeField(MsgSp::Field::RESULT);
                        mState = STATE_LOGIN;
                        LOGGER_INFO(sLogger, mLogPrefix << "Logged in to "
                                    << mSocket->getRemoteAddrStr());
                        keepAlivePeriod = msg->getFieldInt(
                                                MsgSp::Field::KEEPALIVE_PERIOD);
                        watchdogPeriod = keepAlivePeriod *
                                         WATCHDOG_KEEPALIVE_PERIOD_FACTOR;
                        result = msg->getFieldInt(MsgSp::Field::FLEET);
                        if (result == MsgSp::Value::UNDEFINED)
                            result = SubsData::FLEET_NONE;
#ifdef TESTCLIENT
                        if (mDoSubsData)
#endif
                            SubsData::clientInit(result); //set fleet ID
                        requestStatusData();
                        setBranches();
                        requestSubsData();
                        mMobIp = msg->getFieldString(MsgSp::Field::VOIP_GW);
                        //NETWORK_TYPE presence (value irrelevant) indicates
                        //STM-nwk, and VOIP svr is on STM
                        if (msg->hasField(MsgSp::Field::NETWORK_TYPE))
                            mVoipSvrIp = mMobIp;
                        else
                            mVoipSvrIp = sServerIps[sServerIdx];
                        mIpFromServer = msg->getFieldString(MsgSp::Field::DESC);
                        if (mVoipSession != 0 && mVoipSession->isValid())
                        {
                            mVoipSession->reregister(mIpFromServer, mVoipSvrIp);
                        }
                        else if (checkVoipSession() != 0)
                        {
                            //put any value in VOIP_SSRC to indicate failure
                            msg->addField(MsgSp::Field::VOIP_SSRC, "0");
                        }
#ifndef NO_DB
                        //connect to database
                        valStr = msg->getFieldString(MsgSp::Field::DB_ADDRESS);
                        DbInt::init(sLogger,
                             msg->getFieldString(MsgSp::Field::DB_USERNAME),
                             MsgSp::hexUnscramble(
                                 msg->getFieldString(MsgSp::Field::DB_PASSWORD),
                                 challenge),
                             msg->getFieldString(MsgSp::Field::DB_NAME),
                             msg->getFieldInt(MsgSp::Field::DB_PORT),
                             (valStr.empty())? sServerIps[sServerIdx]: valStr);
                        //DbInt::init() could take a while if there is
                        //connection problem - user may have logged out by now
                        if (mState == STATE_STOPPED)
                        {
                            doCallback = false;
                            break;
                        }
#endif
                    } //if (msg->isResultSuccessful())
                    else
                    {
#ifdef DEBUG
                        LOGGER_ERROR(sLogger, mLogPrefix << msg->getName()
                                     << ": Login failure, "
                                     << msg->getFieldValueString(
                                                      MsgSp::Field::RESULT));
#else
                        LOGGER_ERROR(sLogger, mLogPrefix << "Login failure.");
#endif
                    }
                    break;
                }

                case MsgSp::Type::SSDGNA_ASSIGN:
                case MsgSp::Type::SSDGNA_DEASSIGN:
                {
#ifdef TESTCLIENT
                    if (mDoSubsData)
#endif
                        SubsData::processMsg(msg);
                    break;
                }

                case MsgSp::Type::STATUS_DATA:
                {
#ifdef TESTCLIENT
                    if (!mDoSubsData)
                        doCallback = false;
                    else
#else
                        doCallback = StatusCodes::processMsg(msg);
#endif
                    break;
                }

                case MsgSp::Type::SUBS_DATA:
                {
                    if (!msg->isResultSuccessful())
                    {
#ifdef TESTCLIENT
                        if (mDoSubsData)
#endif
                            SubsData::processMsg(msg);
                        switch (msg->getFieldInt(MsgSp::Field::RESULT))
                        {
                            case MsgSp::Value::RESULT_NOT_AUTHORIZED:
                                break; //do nothing - handled in callback
                            case MsgSp::Value::RESULT_RETRY:
                                LOGGER_INFO(sLogger, mLogPrefix
                                   << "Notification to update subscriber data");
                                setBranches();
                                requestSubsData();
                                break;
                            default:
                                //wait for RESULT_RETRY
                                LOGGER_WARNING(sLogger, mLogPrefix
                                               << "Subscriber data not yet "
                                                  "available. Awaiting server "
                                                  "notification...");
                                break;
                        }
                    }
                    else if (msg->hasField(MsgSp::Field::TIMESTAMP))
                    {
                        //final message in subscriber data download
#ifdef TESTCLIENT
                        if (mDoSubsData)
#endif
                            SubsData::processMsg(msg);
                        LOGGER_INFO(sLogger, mLogPrefix
                                    << "Subscriber data downloaded");
                    }
                    else
                    {
                        //first message in subscriber data download -
                        //prepare to receive data
#ifdef TESTCLIENT
                        if (mDoSubsData)
#endif
                            SubsData::clientInit(msg);
                        doCallback = false;
                    }
                    break;
                }

                case MsgSp::Type::SUBS_DATA_DIV:
                case MsgSp::Type::SUBS_DATA_FLEET:
                case MsgSp::Type::SUBS_DATA_GROUP:
                case MsgSp::Type::SUBS_DATA_VPN:
                {
                    doCallback = false;
#ifdef TESTCLIENT
                    if (mDoSubsData)
#endif
                        SubsData::processMsg(msg);
                    break;
                }

                case MsgSp::Type::SYS_KEEPALIVE:
                {
                    doCallback = false;
                    break;
                }

                case MsgSp::Type::VOIP_SERVER_REGISTERED:
                {
                    mMobIp = msg->getFieldString(MsgSp::Field::VOIP_GW);
                    //NETWORK_TYPE presence (value irrelevant) indicates
                    //STM-nwk, and VOIP svr is on STM
                    if (msg->hasField(MsgSp::Field::NETWORK_TYPE) &&
                        mVoipSvrIp != mMobIp)
                    {
                        mVoipSvrIp = mMobIp;
                        if (mVoipSession != 0 && mVoipSession->isValid())
                            mVoipSession->reregister("", mVoipSvrIp);
                    }
                    doCallback = false;
                    break;
                }

                default:
                {
                    break; //do nothing
                }
            } //switch (msg->getType())
            if (resp != 0)
            {
                sendMsg(resp);
                resp = 0;
            }
            if (doCallback)
                mCbFn(mCbObj, msg); //msg ownership transferred
            else
                delete msg;
        } //while (pos != string::npos)
    } //while (mState != STATE_STOPPED)
}

bool ServerSession::init(Logger       *logger,
                         const string &serverIp,
                         int           serverPort,
                         const string &serverIpRed,
                         int           serverPortRed)
{
    if (logger == 0)
    {
        assert("Bad param in ServerSession::init" == 0);
        return false;
    }
    sLogger = logger;
    if (serverIp.empty())
    {
        LOGGER_ERROR(sLogger, "ServerSession::init: Empty Server IP.");
        return false;
    }
    if (!Socket::validateIp(serverIp))
    {
        LOGGER_ERROR(sLogger, "ServerSession::init: Invalid Server IP: "
                    << serverIp);
        return false;
    }
    if (serverPort <= 0)
    {
        LOGGER_ERROR(sLogger, "ServerSession::init: Invalid Server Port: "
                    << serverPort);
        return false;
    }
    sServerIps.clear();
    sServerPorts.clear();
    sServerIps.push_back(serverIp);
    sServerPorts.push_back(serverPort);

    if (!serverIpRed.empty())
    {
        if (!Socket::validateIp(serverIpRed))
        {
            LOGGER_ERROR(sLogger, "ServerSession::init: "
                         << "Invalid Redundant Server IP: " << serverIpRed);
            return false;
        }
        if (serverPortRed <= 0)
        {
            LOGGER_ERROR(sLogger, "ServerSession::init: "
                         << "Invalid Redundant Server Port: "
                         << serverPortRed);
            return false;
        }
        sServerIps.push_back(serverIpRed);
        sServerPorts.push_back(serverPortRed);
    }
    return true;
}

bool ServerSession::init(Logger               *logger,
                         const vector<string> &ips,
                         const vector<int>    &ports)
{
    if (logger == 0)
    {
        assert("Bad param in ServerSession::init" == 0);
        return false;
    }
    sLogger = logger;
    if (ips.empty() || ports.empty())
    {
        LOGGER_ERROR(sLogger, "ServerSession::init: No Server IP or port.");
        return false;
    }
    if (!Socket::validateIp(ips[0]))
    {
        LOGGER_ERROR(sLogger, "ServerSession::init: Invalid Server IP: "
                     << ips[0]);
        return false;
    }
    if (ports[0] <= 0)
    {
        LOGGER_ERROR(sLogger, "ServerSession::init: Invalid Server Port: "
                     << ports[0]);
        return false;
    }
    sServerIps.clear();
    sServerPorts.clear();
    sServerIps.push_back(ips[0]);
    sServerPorts.push_back(ports[0]);
#ifdef DEBUG
    //main and redundant servers may be on the same machine
    if (ips.size() > 1)
#else
    //main and redundant servers must be on different machines
    if (ips.size() > 1 && ips[1] != ips[0])
#endif
    {
        if (!Socket::validateIp(ips[1]))
        {
            LOGGER_ERROR(sLogger, "ServerSession::init: "
                         << "Invalid Redundant Server IP: " << ips[1]);
            return false;
        }
        if (ports.size() > 1)
        {
            if (ports[1] <= 0)
            {
                LOGGER_ERROR(sLogger, "ServerSession::init: "
                             << "Invalid Redundant Server Port: "
                             << ports[1]);
                return false;
            }
            sServerPorts.push_back(ports[1]);
        }
        else
        {
            sServerPorts.push_back(ports[0]); //same ports for both
        }
#ifdef DEBUG
        if (ips[1] == ips[0] && sServerPorts[1] == sServerPorts[0])
            sServerPorts.pop_back();
        else
#endif
            sServerIps.push_back(ips[1]);
    } //if (ips.size() > 1 ...)
    return true;
}

bool ServerSession::setParams(const string   &username,
                              const string   &password,
                              void           *callbackObj,
                              RecvCallbackFn  callbackFn)
{
    assert(sLogger != 0);
    if (sInstance != 0)
    {
        assert("ServerSession::setParams: Instance already created" == 0);
        return false;
    }
    if (username.empty() || password.empty() ||
        callbackObj == 0 || callbackFn == 0)
    {
        assert("Bad param in ServerSession::setParams" == 0);
        return false;
    }
    sUsername.assign(username);
    sPassword.assign(password);
    sCbObj   = callbackObj;
    sCbFn    = callbackFn;
    return true;
}

ServerSession &ServerSession::instance()
{
    static bool isCreated = false;

    //a modified Double Checked Locking Pattern for thread-safe Singleton,
    //lock obtained only before/during creation
    if (!isCreated)
    {
        PalLock::take(&sSingletonLock);
        if (!isCreated) //thread-safety double check
        {
            assert(!sUsername.empty() && !sPassword.empty() &&
                   sCbObj != 0 && sCbFn != 0);
            sInstance = new ServerSession();
            //ensure the compiler cannot reorder the statements and that the
            //flag is set true only after creation
            if (sInstance != 0)
                isCreated = true;
        }
        PalLock::release(&sSingletonLock);
    }
    return *sInstance;
}

void ServerSession::destroy()
{
    delete sInstance;
    sInstance = 0;
    SubsData::final();
}

ServerSession::ServerSession() :
mState(STATE_INVALID), mMessageId(MsgSp::Value::MSG_ID_MIN - 1),
mRecvTime(0), mSentTime(0), mUsername(sUsername), mPassword(sPassword),
mRecvThread(0), mVoipSession(0), mSocket(0), mCbObj(sCbObj), mCbFn(sCbFn)
{
    start();
}

void ServerSession::start()
{
    PalLock::init(&mSendMsgLock);
    assert(sLogger != 0);
    if (mCbObj == 0 || mCbFn == 0)
    {
        assert("Bad param in ServerSession::start" == 0);
        return;
    }
    setEncryption(true);
    setName();
    if (mUsername.empty())
    {
        LOGGER_ERROR(sLogger, mLogPrefix << "Empty Username.");
        return;
    }
    if (mPassword.empty())
    {
        LOGGER_ERROR(sLogger, mLogPrefix << "Empty Password.");
        return;
    }
    sServerIdx = SERVER_IDX_MAIN;
    LOGGER_INFO(sLogger, mLogPrefix << "Starting...");
    mState = STATE_DISCONNECTED;
    mSocket = new TcpSocket(sServerIps[sServerIdx], sServerPorts[sServerIdx]);
    PalThread::start(&mRecvThread, startRecvThread, this);
}

int ServerSession::checkVoipSession()
{
    if (mVoipSession == 0 || !mVoipSession->isValid())
    {
        delete mVoipSession;
        mVoipSession = new VoipSessionClient(Utils::fromString<int>(mUsername),
                                             sVersion, mIpFromServer,
                                             mVoipSvrIp, sLogger, mCbObj, mCbFn);
        if (!mVoipSession->isValid())
        {
            LOGGER_ERROR(sLogger, mLogPrefix
                         << "Failed to create VoIP Session.");
            delete mVoipSession;
            mVoipSession = 0;
            return VOIPSESSION_ERROR;
        }
    }
    return 0;
}

void ServerSession::setName()
{
    if (mState == STATE_CONNECTED || mState == STATE_LOGIN)
    {
        mIpAndPort = mSocket->getLocalAddrStr();
        if (!mMsgKey.empty())
            setEncryption(true);
    }
    else
    {
        mIpAndPort.clear();
    }
    mName.assign("ServerSession[").append(mIpAndPort).append(":")
         .append(mUsername).append("]");
    mLogPrefix.assign(mName).append(":: ");
}

inline void ServerSession::setState(int state)
{
    if (mState != STATE_STOPPED)
        mState = state;
}

bool ServerSession::connectToServer()
{
    static const string TO_MAIN(" to main server ");
    static const string TO_RED(" to redundant server ");
    const unsigned int TRYBLOCKS = 10;
    unsigned int count = 1;
    int connectRes = 0;
    //start with the last connected server, if any
    int svrIdx = sServerIdx;
    string toServer((svrIdx == SERVER_IDX_MAIN)? TO_MAIN: TO_RED);
    toServer.append(mSocket->getRemoteAddrStr());

    while (mState != STATE_STOPPED)
    {
        if (count == 1)
            LOGGER_INFO(sLogger, mLogPrefix << "Connection attempt 1"
                        << toServer << " ...");
        else if (count <= TRYBLOCKS)
            LOGGER_INFO(sLogger, mLogPrefix << "Connection attempt " << count
                        << toServer << ", last error = " << -connectRes
                        << Socket::getErrorStr(-connectRes) << " ...");
        connectRes = mSocket->connect();
        if (connectRes == 0)
        {
            setState(STATE_CONNECTED);
            break;
        }
        if (mState == STATE_STOPPED)
            return false;
        //log after every block of attempts
        if (count % TRYBLOCKS == 0)
            LOGGER_ERROR(sLogger, mLogPrefix
                         << "Server connection failed after " << count
                         << " attempts. Will keep trying...");
        PalThread::sleep(2);
        if (mState == STATE_STOPPED)
            return false;
        ++count;
        if (sServerPorts.size() > 1)
        {
            //toggle servers
            if (svrIdx == SERVER_IDX_MAIN)
            {
                svrIdx = SERVER_IDX_REDUNDANT;
                toServer = TO_RED;
            }
            else
            {
                svrIdx = SERVER_IDX_MAIN;
                toServer = TO_MAIN;
            }
            mSocket->setRemoteAddr(sServerIps[svrIdx], sServerPorts[svrIdx]);
            toServer.append(mSocket->getRemoteAddrStr());
        }
    } //while (mState != STATE_STOPPED)

    if (mState != STATE_CONNECTED)
        return false;
    setName();
    LOGGER_INFO(sLogger, mLogPrefix << "Connected" << toServer << " attempt "
                << count);
    sServerIdx = svrIdx;
    MsgSp m(MsgSp::Type::LOGIN);
    m.addField(MsgSp::Field::USERNAME, mUsername);
    sendMsg(&m, false);
    mRecvTime = time(NULL); //starting time for watchdog
    return true;
}

void ServerSession::sendMon(int            msgType,
                            bool           isGroup,
                            const SsiSetT *ssiSet,
                            int            ssi)
{
    assert((msgType == MsgSp::Type::MON_START ||
            msgType == MsgSp::Type::MON_STOP) &&
           (ssiSet != 0 || ssi != 0));
    MsgSp m(msgType);
    m.addField(MsgSp::Field::AFFECTED_USER_TYPE,
               (isGroup)? MsgSp::Value::IDENTITY_TYPE_GSSI :
                          MsgSp::Value::IDENTITY_TYPE_ISSI);
    if (ssiSet != 0)
        m.addField(MsgSp::Field::SSI_LIST, Utils::toStringWithRange(*ssiSet));
    else if (ssi > 0)
        m.addField(MsgSp::Field::SSI_LIST, ssi);
    else
        return;
    sendMsg(&m, false);
}
