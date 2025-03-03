/**
 * VOIP server session base class implementation.
 *
 * Copyright (C) Sapura Secured Technologies, 2013-2025. All Rights Reserved.
 *
 * @file
 * @version $Id: VoipSessionBase.cpp 1904 2025-02-19 06:59:58Z zulzaidi $
 * @author Ahmad Syukri
 */
#include <assert.h>
#include <string.h> //memset

#include "Md5Digest.h"
#if defined(SNMP) && defined(SERVERAPP)
#include "SnmpAgent.h"
#endif
#include "TcpSocket.h"
#include "Utils.h"
#include "VoipSessionBase.h"

using namespace std;

#ifdef SIPTCP
static const int    DEFAULT_SERVER_PORT = 6001;
#else
static const int    DEFAULT_SERVER_PORT = 6000;
#endif
//multiples of server ping interval to set as timeout period
static const int    TIMEOUT_FACTOR      = 3;
static const string LOGPREFIX("VoipSessionBase:: ");

const string VoipSessionBase::HUPALL("hupall");
const string VoipSessionBase::PARAM_BRANCH_RFC3261_COOKIE(";branch=z9hG4bK");
const string VoipSessionBase::PARAM_EXPIRES(";expires=315360000"); //10 years
const string VoipSessionBase::PARAM_EXPIRES_0(";expires=0"); //unregister
const string VoipSessionBase::PARAM_RPORT(";rport");
const string VoipSessionBase::PARAM_TAG(";tag=");
const string VoipSessionBase::PARAM_TRANSPORT(";transport=tls");
const string VoipSessionBase::QOP_AUTH("auth");
const string VoipSessionBase::QOP_AUTH_INT("auth-int");
const string VoipSessionBase::URI_PREFIX("sip:");
const string VoipSessionBase::VALUE_ALLOW("INVITE,ACK,BYE,CANCEL,OPTIONS,"
                                          "PRACK,REFER,NOTIFY,SUBSCRIBE,"
                                          "INFO,MESSAGE");
const string VoipSessionBase::VALUE_CONTENT_TYPE_DTMF("application/"
                                                      "dtmf-relay");
const string VoipSessionBase::VALUE_CONTENT_TYPE_SDP("application/sdp");
const string VoipSessionBase::VALUE_SUPPORTED("replaces,norefersub,100rel");
#ifdef SIPTCP
const string VoipSessionBase::VIA_PROTOCOL("SIP/2.0/TLS ");
#else
const string VoipSessionBase::VIA_PROTOCOL("SIP/2.0/UDP ");
#endif //SIPTCP

#ifdef MOBILE
#define MSGCB(arg) mCbFn(arg)
static ostringstream sOss;
#define LOGX(lvl, arg)                               \
        do                                           \
        {                                            \
            LOGGER_##lvl(mLogger, LOGPREFIX << arg); \
            sOss << LOGPREFIX << arg;                \
            mStrCbFn(sOss.str());                    \
            sOss.str("");                            \
        }                                            \
        while (0)

#else //MOBILE
#define MSGCB(arg) mCbFn(mCbObj, arg)
#define LOGX(lvl, arg) LOGGER_##lvl(mLogger, LOGPREFIX << arg)
#endif //MOBILE

#ifndef NO_VOIP
static void *voipSessionStartRecvThread(void *arg)
{
    static_cast<VoipSessionBase *>(arg)->recvThread();
    return 0;
}
#endif

static void *voipSessionStartSendThread(void *arg)
{
    static_cast<VoipSessionBase *>(arg)->sendMsgThread();
    return 0;
}

#ifdef SERVERAPP
VoipSessionBase::VoipSessionBase(const string   &userId,
                                 const string   &password,
                                 Logger         *logger,
                                 void           *callbackObj,
                                 RecvCallbackFn  callbackFn) :
mState(STATE_INVALID), mServerPort(DEFAULT_SERVER_PORT), mMsgId(0), mNc(0),
mOnStandby(false), mUserId(userId), mPassword(password), mSocket(0),
mLogger(logger), mSendMsgThread(0), mRecvThread(0), mCbObj(callbackObj),
mCbFn(callbackFn)
{
    PalSem::init(&mSendMsgCountSem);
    PalLock::init(&mSendMsgQueueLock);
    PalLock::init(&mCallMapLock);
}

#else //SERVERAPP
VoipSessionBase::VoipSessionBase(int             userId,
                                 Logger         *logger,
#ifdef MOBILE
                                 StrCallbackFn   strCbFn,
#else
                                 void           *callbackObj,
#endif
                                 RecvCallbackFn  callbackFn) :
mState(STATE_INVALID), mServerPort(DEFAULT_SERVER_PORT), mMsgId(0), mNc(0),
mOnStandby(false), mSocket(0), mLogger(logger), mSendMsgThread(0),
mRecvThread(0),
#ifdef MOBILE
mStrCbFn(strCbFn),
#else
mCbObj(callbackObj),
#endif
mCbFn(callbackFn)
{
    mUserId = Utils::toString(userId);
    PalSem::init(&mSendMsgCountSem);
    PalLock::init(&mSendMsgQueueLock);
    PalLock::init(&mCallMapLock);
}
#endif //SERVERAPP

VoipSessionBase::~VoipSessionBase()
{
    if (isRegistered())
    {
        sendMsg(createUnregisterMsg(Utils::fromString<int>(mUserId), 0));
        //wait for completion with a time limit
        int i = 150;
        do
        {
            PalThread::msleep(10);
        }
        while (mState != STATE_STOPPED && mState != STATE_INVALID && --i > 0);
    }
#ifdef MOBILE
    if (mState != STATE_INVALID)
#endif
        mState = STATE_STOPPED;
    if (mSendMsgThread != 0)
    {
        PalSem::post(&mSendMsgCountSem); //to break the thread loop
        PalThread::stop(mSendMsgThread);
    }
    delete mSocket;
    if (mRecvThread != 0)
    {
        PalThread::stop(mRecvThread);
#ifdef MOBILE
        //wait for thread to finish
        while (mState != STATE_INVALID)
        {
            PalThread::msleep(10);
        }
#endif
    }
    PalLock::destroy(&mCallMapLock);
    PalLock::destroy(&mSendMsgQueueLock);
    PalSem::destroy(&mSendMsgCountSem);
    while (!mSendMsgQueue.empty())
    {
        delete mSendMsgQueue.front();
        mSendMsgQueue.pop();
    }
    LOGGER_INFO(mLogger, LOGPREFIX << "Destroyed.");
}

#if defined(SNMP) && defined(SERVERAPP)
#define SEND_SNMP_DISC()                                     \
    do                                                       \
    {                                                        \
        if (!alertSent)                                      \
            alertSent = SNMP_TRAP(SnmpAgent::TRAP_VOIP_STAT, \
                                  SnmpAgent::VAL_DISC);      \
    }                                                        \
    while (0)
#define SEND_SNMP_CONN()                                      \
    do                                                        \
    {                                                         \
        if (alertSent)                                        \
            alertSent = !SNMP_TRAP(SnmpAgent::TRAP_VOIP_STAT, \
                                   SnmpAgent::VAL_CONN);      \
    }                                                         \
    while (0)
#else
#define SEND_SNMP_DISC()
#define SEND_SNMP_CONN()
#endif

void VoipSessionBase::recvThread()
{
#ifdef MOBILE
    assert(mStrCbFn != 0 && mCbFn != 0);
#else
    assert(mCbObj != 0 && mCbFn != 0);
#endif
    LOGGER_DEBUG(mLogger, LOGPREFIX << "recvThread: started.");
    PalThread::start(&mSendMsgThread, voipSessionStartSendThread, this);
#ifdef SIPTCP
    if (!connectToServer())
    {
#ifdef MOBILE
        MSGCB(0); //notify thread exit
#endif
        mState = STATE_INVALID;
        return;
    }
#else
    sendMsg(createRegisterMsg(0));
#endif //SIPTCP

    static const int TIMEOUT_DEF = 300; //seconds
#if defined(SNMP) && defined(SERVERAPP)
    bool    alertSent = false;
#endif
    int     type;
    int     id;
    int     bytesRcvd = 0;
    int     timeoutSecs = TIMEOUT_DEF; //server timeout in seconds
    time_t  lastRcvTime = time(0);
    time_t  now;
#ifdef SIPTCP
    int     len = 0;         //content body length
    time_t  connTime = lastRcvTime;
    size_t  termLen = MsgSip::Value::TERMINATOR.length();
    size_t  pos;
    string  dataStr;
#else
    int     rmtPort;
    string  rmtIp;
#endif
    string  str;
    char    buf[BUFFER_SIZE_BYTES];
    MsgSip *msg = 0;
    MsgSip *resp = 0;
    MsgSp  *msgSp = 0;
    while (mState != STATE_STOPPED)
    {
        memset(buf, 0, BUFFER_SIZE_BYTES);
#ifdef SIPTCP
        bytesRcvd = mSocket->recv(buf, sizeof(buf), 5);
        now = time(0);
        if (bytesRcvd <= 0)
        {
            if (mState == STATE_STOPPED)
                break;
            if (mState == STATE_STARTED)
            {
                LOGX(VERBOSE, "Rx\nSwitching/reconnecting server");
            }
            else if (Socket::isDisconnectedError(-bytesRcvd))
            {
                mState = STATE_DISCONNECTED;
                LOGX(ERROR, "Rx\nDisconnected, error " << bytesRcvd
                     << Socket::getErrorStr(-bytesRcvd));
                MSGCB(new MsgSp(MsgSp::Type::VOIP_SERVER_UNREGISTERED));
                SEND_SNMP_DISC();
                //if too soon after last connect, wait a bit to avoid rapid
                //disconnect-reconnect cycles
                if (now - connTime < 3)
                    PalThread::sleep(3);
            }
            else
            {
                if (bytesRcvd != TcpSocket::ERR_TIMEOUT)
                {
                    LOGX(ERROR, "Rx\nSocket error " << bytesRcvd
                         << Socket::getErrorStr(-bytesRcvd));
                }
                if (now - lastRcvTime < timeoutSecs)
                    continue;
                mState = STATE_DISCONNECTED;
                LOGX(WARNING, "Rx\nServer timeout after " << timeoutSecs
                     << " seconds");
                MSGCB(new MsgSp(MsgSp::Type::VOIP_SERVER_UNREGISTERED));
                SEND_SNMP_DISC();
            }
            mSsiCallMap.clear();
            mRegMap.clear();
            dataStr.clear();
            if (!connectToServer())
                break;
            SEND_SNMP_CONN();
            connTime = time(0);
            len = 0;
            timeoutSecs = TIMEOUT_DEF;
            lastRcvTime = connTime;
            delete msg;
            continue;
        }
        dataStr.append(buf, bytesRcvd);
        pos = dataStr.find(MsgSip::Value::TERMINATOR);
        while (pos != string::npos || (len > 0 && dataStr.size() >= len))
        {
            id = dataStr.size(); //temp use
            if (len == 0)
            {
                //no pending content body - get next msg
                msg = MsgSip::parse(dataStr, len);
                if (msg == 0)
                    LOGX(ERROR, "recvThread: Failed to parse message: <<\n"
                         << dataStr << "\n>>");
                //erase consumed data
                dataStr.erase(0, pos + termLen); //may clear it
            }
            else if (id >= len)
            {
                msg->setContentBody(dataStr.substr(0, len));
                dataStr.erase(0, len); //may clear it
                len = 0;
            }
            pos = (dataStr.empty())? string::npos:
                                     dataStr.find(MsgSip::Value::TERMINATOR);
            if (msg == 0 || len > 0)
                continue;

#else //SIPTCP
        { //just to match SIPTCP 'while (pos != string::npos...)' brace
            bytesRcvd = mSocket->recv(buf, BUFFER_SIZE_BYTES, 10, &rmtIp,
                                      &rmtPort);
            now = time(0);
            if (bytesRcvd <= 0)
            {
                if (mState == STATE_STOPPED)
                    break;
                if (bytesRcvd != UdpSocket::ERR_TIMEOUT)
                {
                    LOGX(ERROR, "recvThread: Socket error " << bytesRcvd
                         << Socket::getErrorStr(-bytesRcvd));
                    SEND_SNMP_DISC();
                }
                else if (now - lastRcvTime >= timeoutSecs)
                {
                    LOGGER_WARNING(mLogger, LOGPREFIX
                                   << "recvThread: Server timeout after "
                                   << (now - lastRcvTime) << " seconds");
                    mState = STATE_STARTED;
                    mSsiCallMap.clear();
                    sendMsg(createRegisterMsg(0));
                    timeoutSecs = TIMEOUT_DEF;
                    lastRcvTime = now;
                    SEND_SNMP_DISC();
                }
                continue;
            }
            SEND_SNMP_CONN();
            str.assign(buf, bytesRcvd);
            msg = MsgSip::parse(str);
            if (msg == 0)
            {
                LOGX(ERROR, "recvThread: Failed to parse message: <<\n" << str
                     << "\n>>");
                continue;
            }
            if (rmtIp != mServerIp || rmtPort != mServerPort)
            {
                LOGGER_INFO(mLogger, LOGPREFIX << "Received from old server "
                            << rmtIp << ':' << rmtPort << " (discarded)\n"
                            << *msg);
                delete msg;
                continue;
            }
#endif //SIPTCP

            LOGX(VERBOSE, "Rx\n" << *msg);
            type = msg->getType();
            switch (type)
            {
                case MsgSip::Type::ACK:
                {
                    break; //server ack of our INVITE OK response - do nothing
                }
                case MsgSip::Type::OPTIONS:
                {
                    //set timeout using last ping interval with minimum
                    id = (now - lastRcvTime) * TIMEOUT_FACTOR;
                    if (id >= 60)
                        timeoutSecs = id;
                    lastRcvTime = now;
                    //fallthrough
                }
                case MsgSip::Type::NOTIFY:
                {
                    resp = createRespMsg(type, MsgSip::Value::RESP_OK, msg);
                    str = msg->getFieldString(MsgSip::Field::TO);
                    resp->addField(MsgSip::Field::TO,
                                   str + PARAM_TAG + mTagIdentity)
                         .addField(MsgSip::Field::CSEQ,
                                   msg->getFieldString(MsgSip::Field::CSEQ));
                    id = getCallPartyInUri(str, false);
                    if (id != Utils::fromString<int>(mUserId))
                    {
                        sendMsg(resp);
                        resp = createUnregisterMsg(id, 0);
                    }
                    break;
                }
                case MsgSip::Type::REGISTER:
                {
                    int cseq = msg->getFieldInt(MsgSip::Field::CSEQ);
                    if (mRegMap.count(cseq) == 0)
                    {
                        LOGX(WARNING, "recvThread: "
                             << msg->getName() << ": Unknown "
                             << MsgSip::getFieldName(MsgSip::Field::CSEQ)
                             << ' ' << cseq << " - discarded");
                        break;
                    }
                    auto &dt(mRegMap[cseq]);
                    switch (msg->getRespCode())
                    {
                        case MsgSip::Value::RESP_OK:
                        {
                            id = getCallPartyInUri(
                                         msg->getFieldString(MsgSip::Field::TO),
                                         false);
                            bool isMyId = (id ==
                                           Utils::fromString<int>(mUserId));
                            if (!dt.reg)
                            {
                                //unregister may be for id != mUserId
                                LOGGER_INFO(mLogger, LOGPREFIX
                                            << "VOIP unregistered: " << id);
                                if (isMyId && mState != STATE_INVALID)
                                    mState = STATE_STOPPED;
                            }
                            else if (isMyId && !isRegistered())
                            {
                                LOGGER_INFO(mLogger, LOGPREFIX
                                            << "VOIP registered: " << id);
                                setState(STATE_REGISTERED);
#ifdef SERVERAPP
                                //terminate any lingering active calls on server
                                LOGGER_VERBOSE(mLogger, LOGPREFIX
                                               << "Terminating all calls");
                                ostringstream os;
                                os << URI_PREFIX << HUPALL << '@' << mServerIp;
                                str = os.str(); //called party
                                resp = createReqMsg(MsgSip::Type::INVITE, str);
                                str.insert(0, "<").append(">"); //to
                                resp->addField(MsgSip::Field::TO, str);
                                os.clear();
                                os.str("");
                                os << '"' << HUPALL << "\" " << mUserUri
                                   << PARAM_TAG << mTagIdentity;
                                resp->addField(MsgSip::Field::FROM, os.str())
                                //set dummy call ID to make FreeSWITCH execute
                                //correct dialplan
                                     .addField(MsgSip::Field::CALL_ID, 0)
                                     .setSeqId(getNewMsgId())
                                     .addField(MsgSip::Field::CONTACT, mContact)
                                     .addField(MsgSip::Field::ALLOW, VALUE_ALLOW)
                                     .addField(MsgSip::Field::SUPPORTED,
                                               VALUE_SUPPORTED);
#else
                                MSGCB(new MsgSp(
                                          MsgSp::Type::VOIP_SERVER_REGISTERED));
#endif //SERVERAPP
                            }
                            break;
                        }
                        case MsgSip::Value::RESP_RFAIL_SVC_UNAVAILABLE:
                        {
                            //VOIP server not ready
                            //retry is in ms
                            id = msg->getFieldInt(MsgSip::Field::RETRY_AFTER);
                            if (id <= 0)
                                id = 300; //default
                            else
                                id += 20; //allow a bit more time
                            PalThread::msleep(id);
                            resp = createRegisterMsg(0);
                            break;
                        }
                        case MsgSip::Value::RESP_RFAIL_UNAUTHORIZED:
                        {
                            //get rport from via, if any
                            //Via: SIP/2.0/UDP <ip>:<port>;rport=51062;branch=
                            //Via: SIP/2.0/TCP <ip>;rport=51062;branch=
                            id = msg->getParamInt(MsgSip::Field::VIA,
                                                  PARAM_RPORT);
                            if (id != MsgSip::Value::UNDEFINED
#ifndef SIPTCP
                                && id != mSocket->getLocalPort()
#endif
                                )
                            {
                                //change Contact port to rport value
                                mContact.assign("<").append(URI_PREFIX)
                                        .append(mUserId).append("@")
                                        .append(mLocalIp).append(":")
                                        .append(Utils::toString(id))
#ifdef SIPTCP
                                        .append(PARAM_TRANSPORT)
#endif
                                        .append(">")
                                        .append(PARAM_EXPIRES);
                            }
                            id = getCallPartyInUri(
                                         msg->getFieldString(MsgSip::Field::TO),
                                         false);
                            //unregister may be for id != mUserId
                            if (!dt.reg)
                                resp = createUnregisterMsg(id, msg);
                            else if (id == Utils::fromString<int>(mUserId))
                                resp = createRegisterMsg(msg);
                            break;
                        }
                        default:
                        {
                            LOGGER_ERROR(mLogger, LOGPREFIX << "recvThread: "
                                         << msg->getName()
                                         << ": Unhandled response code "
                                         << msg->getRespCode());
                            break;
                        }
                    }
                    mRegMap.erase(cseq);
                    break;
                }
                default:
                {
                    if (!mOnStandby)
                        resp = processMsg(msg, msgSp);
                    break;
                }
            } //switch (type)
            if (resp != 0)
            {
                sendMsg(resp);
                resp = 0;
            }
            if (msgSp != 0)
            {
                LOGX(VERBOSE, "Rx callback\n" << *msgSp);
                MSGCB(msgSp);
                msgSp = 0;
            }
            delete msg;
            msg = 0;
            if (!mRegMap.empty())
            {
                //delete obsolete entries
                auto it = mRegMap.begin();
                while (it != mRegMap.end())
                {
                    if (now - it->second.t > 5)
                    {
                        LOGX(VERBOSE, "Erasing timed out "
                             << ((it->second.reg)? "": "un")
                             << "register data "
                             << MsgSip::getFieldName(MsgSip::Field::CSEQ)
                             << '=' << it->first << " after "
                             << now - it->second.t << " seconds");
                        it = mRegMap.erase(it);
                    }
                    else
                    {
                        ++it;
                    }
                }
            }
        } //SIPTCP: while (pos != string::npos...), otherwise standalone braces
    } //while (mState != STATE_STOPPED)
    delete msg;
#ifdef MOBILE
    MSGCB(0); //notify thread exit
#endif
    LOGGER_DEBUG(mLogger, LOGPREFIX << "recvThread: stopped");
    mState = STATE_INVALID;
}

void VoipSessionBase::sendMsgThread()
{
    LOGGER_DEBUG(mLogger, LOGPREFIX << "sendMsgThread: started");
    MsgSip *msg;
    while (mState != STATE_STOPPED && mState != STATE_INVALID)
    {
        //wait for a message in the queue
        if (!PalSem::wait(&mSendMsgCountSem))
            continue;
        if (mState == STATE_STOPPED || mState == STATE_INVALID)
            break;
        PalLock::take(&mSendMsgQueueLock);
        if (mSendMsgQueue.empty())
        {
            //semaphore posted with empty queue only at shutdown
            PalLock::release(&mSendMsgQueueLock);
            break;
        }
        msg = mSendMsgQueue.front();
        mSendMsgQueue.pop();
        PalLock::release(&mSendMsgQueueLock);
#ifdef SIPTCP
        if (mState < STATE_REGISTERED)
        {
            int target = (msg->getType() == MsgSip::Type::REGISTER)?
                         STATE_CONNECTED: STATE_REGISTERED;
            while (mState < target && mState != STATE_INVALID)
            {
                PalThread::msleep(20); //wait till connected/registered
            }
            if (mState == STATE_STOPPED || mState == STATE_INVALID)
            {
                delete msg;
                break;
            }
        }
#endif
        sendToServer(*msg);
        delete msg; //message completely consumed
    }
#ifdef MOBILE
    MSGCB(0); //notify thread exit
#endif
    LOGGER_DEBUG(mLogger, LOGPREFIX << "sendMsgThread: stopped");
}

int VoipSessionBase::disconnectCauseFromSipResp(int code)
{
    switch (code)
    {
        case MsgSip::Value::RESP_RFAIL_BUSY_HERE:
            return MsgSp::Value::DC_CALLED_PARTY_BUSY;
        case MsgSip::Value::RESP_RFAIL_NOT_ACCEPTABLE_HERE:
        case MsgSip::Value::RESP_RFAIL_NOT_FOUND:
            return MsgSp::Value::DC_INVALID_CALLED_PARTY;
        case MsgSip::Value::RESP_RFAIL_SVC_UNAVAILABLE:
            return MsgSp::Value::DC_SVC_NOT_AVAILABLE;
        case MsgSip::Value::RESP_RFAIL_TEMP_UNAVAILABLE:
            return MsgSp::Value::DC_CALLED_PARTY_NOT_REACHABLE;
        case MsgSip::Value::RESP_GFAIL_DECLINE:
        default:
            return MsgSp::Value::DC_NOT_DEFINED;
    }
}

int VoipSessionBase::disconnectCauseToSipResp(int dc)
{
    switch (dc)
    {
        case MsgSp::Value::DC_CALLED_PARTY_BUSY:
            return MsgSip::Value::RESP_RFAIL_BUSY_HERE;
        case MsgSp::Value::DC_CALLED_PARTY_NOT_REACHABLE:
            return MsgSip::Value::RESP_RFAIL_TEMP_UNAVAILABLE;
        case MsgSp::Value::DC_INVALID_CALLED_PARTY:
            return MsgSip::Value::RESP_RFAIL_NOT_FOUND;
        case MsgSp::Value::DC_SVC_NOT_AVAILABLE:
        case MsgSp::Value::DC_SVC_TEMP_NOT_AVAILABLE:
            return MsgSip::Value::RESP_RFAIL_SVC_UNAVAILABLE;
        default:
            return MsgSip::Value::RESP_GFAIL_DECLINE;
    }
}

bool VoipSessionBase::init(int localPort)
{
#ifdef NO_VOIP
    mState = STATE_STOPPED;
    return true;
#endif
    LOGGER_DEBUG(mLogger, LOGPREFIX << "init: localPort=" << localPort
                 << " server=" << mServerIp << ":" << mServerPort);
#ifdef SIPTCP
    mSocket = new TcpSocket(mServerIp, mServerPort, true);
#else
    mSocket = getUdpSocket(localPort);
#endif //SIPTCP
    mUserUri.assign("<").append(URI_PREFIX).append(mUserId).append("@")
            .append(mServerIp).append(">");
    mTagIdentity.assign(mUserId).append(Utils::randomString(TAG_LEN, TAG_LEN));
    setSipFields();
    mState = STATE_STOPPED;
    return start();
}

void VoipSessionBase::setSipFields()
{
    mContact.assign("<").append(URI_PREFIX).append(mUserId).append("@")
            .append(mLocalIp);
    mVia.assign(VIA_PROTOCOL).append(mLocalIp);
#ifdef SIPTCP
    mContact.append(PARAM_TRANSPORT);
#else
    string p(":");
    p.append(Utils::toString(mSocket->getLocalPort()));
    mContact.append(p);
    mVia.append(p);
#endif
    mContact.append(">").append(PARAM_EXPIRES);
    mVia.append(PARAM_RPORT);
    mVia.append(PARAM_BRANCH_RFC3261_COOKIE);
}

bool VoipSessionBase::start()
{
    if (!isValid())
        return false;
    if (mState != STATE_STOPPED)
    {
        LOGGER_DEBUG(mLogger, LOGPREFIX << "start: Already running.");
        return true;
    }
    LOGGER_INFO(mLogger, LOGPREFIX << "Starting...");
    mState = STATE_STARTED;
#ifndef NO_VOIP
    PalThread::start(&mRecvThread, voipSessionStartRecvThread, this);
#endif
    return true;
}

int VoipSessionBase::getNewMsgId()
{
    if (++mMsgId <= 0)
        mMsgId = 1;
    return mMsgId;
}

bool VoipSessionBase::setLocalIp()
{
    if (mServerIp == Socket::LOCALHOST)
    {
        mLocalIp = Socket::LOCALHOST;
        return true;
    }
    TcpSocket socket(mServerIp, mServerPort);
    if (socket.connect() == 0)
    {
        mLocalIp = socket.getLocalIp();
        return true;
    }
    return false;
}

inline void VoipSessionBase::setState(int state)
{
    if (mState != STATE_STOPPED)
        mState = state;
}

void VoipSessionBase::sendToServer(MsgSip &msg)
{
    int   n;
    char *data = msg.getBytes(n);
    if (data == 0)
    {
        LOGX(ERROR, "sendToServer: Error serializing message\n" << msg);
        return;
    }
#ifdef SIPTCP
    n = mSocket->send(data, n);
#else
    n = mSocket->send(data, n, mServerIp, mServerPort);
#endif
    delete [] data;
    if (n < 0)
    {
        LOGX(ERROR, "sendToServer: Error " << n << Socket::getErrorStr(-n)
             << " sending message\n" << msg);
        return;
    }
    LOGX(VERBOSE, "Tx\n" << msg);
}

void VoipSessionBase::sendMsg(MsgSip *msg)
{
    assert(msg != 0);
#ifdef SIPTCP
    if (mState == STATE_CONNECTED && msg->getType() == MsgSip::Type::REGISTER)
    {
        sendToServer(*msg); //jump queue
        delete msg;
        return;
    }
#endif
    PalLock::take(&mSendMsgQueueLock);
    mSendMsgQueue.push(msg);
    PalSem::post(&mSendMsgCountSem);
    PalLock::release(&mSendMsgQueueLock);
}

MsgSip *VoipSessionBase::createRegisterMsg(const MsgSip *svrMsg)
{
    MsgSip *m = createReqMsg(MsgSip::Type::REGISTER, URI_PREFIX + mServerIp);
    m->addField(MsgSip::Field::FROM, mUserUri + PARAM_TAG + mTagIdentity)
      .addField(MsgSip::Field::TO, mUserUri)
      .setSeqId(getNewMsgId())
      .addField(MsgSip::Field::CONTACT, mContact)
      .addField(MsgSip::Field::ALLOW, VALUE_ALLOW);
    mRegMap[m->getFieldInt(MsgSip::Field::CSEQ)].setReg(true);
    if (svrMsg != 0)
    {
        m->addField(MsgSip::Field::CALL_ID,
                    svrMsg->getFieldString(MsgSip::Field::CALL_ID));
        addAuthParam(*svrMsg, *m);
    }
    else
    {
        //prefix with user ID to ensure uniqueness among users
        m->addField(MsgSip::Field::CALL_ID,
                    mUserId + Utils::randomString(TAG_LEN, TAG_LEN));
    }
    return m;
}

MsgSip *VoipSessionBase::createUnregisterMsg(int           userId,
                                             const MsgSip *svrMsg)
{
    string userUri("<" + URI_PREFIX + Utils::toString(userId) + "@");
    MsgSip *m = createReqMsg(MsgSip::Type::REGISTER, URI_PREFIX + mServerIp);
    m->addField(MsgSip::Field::CONTACT,
                userUri + mLocalIp + ">" + PARAM_EXPIRES_0);
    userUri.append(mServerIp).append(">");
    m->addField(MsgSip::Field::FROM, userUri + PARAM_TAG + mTagIdentity)
      .addField(MsgSip::Field::TO, userUri)
      .setSeqId(getNewMsgId());
    mRegMap[m->getFieldInt(MsgSip::Field::CSEQ)].setReg(false);
    if (svrMsg != 0)
    {
        m->addField(MsgSip::Field::CALL_ID,
                    svrMsg->getFieldString(MsgSip::Field::CALL_ID));
        //authentication user ID need not be the same as the SIP user ID,
        //so the fact that addAuthParam() uses mUserId is OK
        addAuthParam(*svrMsg, *m);
    }
    else
    {
        m->addField(MsgSip::Field::CALL_ID,
                    mUserId + Utils::randomString(TAG_LEN, TAG_LEN));
    }
    return m;
}

MsgSip *VoipSessionBase::createReqMsg(int             type,
                                      const string   &requestUri,
                                      const MsgSip   *svrMsg,
                                      const CallData *data)
{
    MsgSip *m = new MsgSip(type, requestUri);
    m->addField(MsgSip::Field::MAX_FORWARDS, MAX_FORWARDS)
      .addField(MsgSip::Field::USER_AGENT, mUserAgent)
      .addField(MsgSip::Field::CONTENT_LENGTH, 0);

    if (svrMsg != 0)
    {
        m->addField(MsgSip::Field::VIA,
                    svrMsg->getFieldString(MsgSip::Field::VIA))
          .addField(MsgSip::Field::FROM,
                    svrMsg->getFieldString(MsgSip::Field::FROM))
          .addField(MsgSip::Field::TO,
                    svrMsg->getFieldString(MsgSip::Field::TO))
          .addField(MsgSip::Field::CALL_ID,
                    svrMsg->getFieldString(MsgSip::Field::CALL_ID))
          .setSeqId(svrMsg->getSeqId());
    }
    else if (data != 0)
    {
        m->addField(MsgSip::Field::VIA, data->via)
          .addField(MsgSip::Field::FROM, data->from)
          .addField(MsgSip::Field::TO, data->to)
          .setSeqId(data->seqId);
    }
    else
    {
        m->addField(MsgSip::Field::VIA,
                    mVia + mUserId + Utils::randomString(TAG_LEN, TAG_LEN));
    }
    return m;
}

MsgSip *VoipSessionBase::createRespMsg(int             type,
                                       int             status,
                                       const MsgSip   *svrMsg,
                                       const CallData *data)
{
    MsgSip *m = new MsgSip(type, status);
    m->addField(MsgSip::Field::MAX_FORWARDS, MAX_FORWARDS)
      .addField(MsgSip::Field::USER_AGENT, mUserAgent)
      .addField(MsgSip::Field::CONTENT_LENGTH, 0);

    if (svrMsg != 0)
    {
        m->addField(MsgSip::Field::VIA,
                    svrMsg->getFieldString(MsgSip::Field::VIA))
          .addField(MsgSip::Field::FROM,
                    svrMsg->getFieldString(MsgSip::Field::FROM))
          .addField(MsgSip::Field::TO,
                    svrMsg->getFieldString(MsgSip::Field::TO))
          .addField(MsgSip::Field::CALL_ID,
                    svrMsg->getFieldString(MsgSip::Field::CALL_ID))
          .setSeqId(svrMsg->getSeqId());
    }
    else if (data != 0)
    {
        m->addField(MsgSip::Field::VIA, data->via)
          .addField(MsgSip::Field::FROM, data->from)
          .addField(MsgSip::Field::TO, data->to)
          .setSeqId(data->seqId);
    }
    return m;
}

MsgSip *VoipSessionBase::createErrorAckMsg(const MsgSip &svrMsg)
{
    string reqUri(svrMsg.getFieldString(MsgSip::Field::TO));
    if (!getReqUri(reqUri))
    {
        LOGGER_ERROR(mLogger, LOGPREFIX << "createErrorAckMsg: "
                     << svrMsg.getName() << ": Invalid TO: " << reqUri);
        return 0;
    }
    return createReqMsg(MsgSip::Type::ACK, reqUri, &svrMsg);
}

void VoipSessionBase::addAuthParam(const MsgSip &msg,
                                   MsgSip       &resp,
                                   const string &reqUri)
{
    if (++mNc == 0)
        mNc = 1;

    int reqField  = MsgSip::Field::PROXY_AUTHENTICATE;  //for invite
    int respField = MsgSip::Field::PROXY_AUTHORIZATION;
    if (msg.hasSubfieldHeader(MsgSip::Field::WWW_AUTHENTICATE))
    {
        //for register
        reqField = MsgSip::Field::WWW_AUTHENTICATE;
        respField = MsgSip::Field::AUTHORIZATION;
    }

    string realm(msg.getSubfieldString(reqField, MsgSip::Subfield::REALM));
    ostringstream os;
    os << md5Digest(mUserId + ":" + realm + ":" + mPassword)
       << ":" << msg.getSubfieldString(reqField, MsgSip::Subfield::NONCE)
       << ":";

    string qop(msg.getSubfieldString(reqField, MsgSip::Subfield::QOP));
    string nc(Utils::toHexString(mNc, 8));
    string cnonce(Utils::randomString(CNONCE_LEN, CNONCE_LEN));
    if (qop == QOP_AUTH || qop == QOP_AUTH_INT)
        os << nc << ":" << cnonce << ":" << qop << ":";

    string uri;
    if (reqField == MsgSip::Field::PROXY_AUTHENTICATE)
    {
        assert(!reqUri.empty());
        uri = reqUri;
    }
    else
    {
        uri.assign(URI_PREFIX).append(realm);
    }
    if (qop == QOP_AUTH || qop.empty())
        os << md5Digest(msg.getName() + ":" + uri);
    else
        os << md5Digest("");

    resp.addSubfield(respField, MsgSip::Subfield::USERNAME, mUserId)
        .addSubfield(respField, MsgSip::Subfield::URI, uri)
        .addSubfield(respField, MsgSip::Subfield::CNONCE, cnonce)
        .addSubfield(respField, MsgSip::Subfield::NC, nc)
        .addSubfield(respField, MsgSip::Subfield::RESPONSE, md5Digest(os.str()))
        .addSubfield(respField, MsgSip::Subfield::REALM, realm)
        .addSubfield(respField, MsgSip::Subfield::NONCE,
                     msg.getSubfieldString(reqField, MsgSip::Subfield::NONCE))
        .addSubfield(respField, MsgSip::Subfield::QOP, qop)
        .addSubfield(respField, MsgSip::Subfield::ALGORITHM,
                     msg.getSubfieldString(reqField,
                                           MsgSip::Subfield::ALGORITHM));
}

string VoipSessionBase::generateSdp(int           sessionId,
                                    const string &ip,
                                    bool          doAcelp,
                                    int           audPort,
                                    int           vidPort,
                                    const string &audKey,
                                    const string &vidKey)
{
    ostringstream os;
    os << "v=0" << MsgSip::Value::ENDL
       << "o=- " << sessionId << " 1 IN IP4 " << mLocalIp
       << MsgSip::Value::ENDL
       << "s= " << MsgSip::Value::ENDL
       << "c=IN IP4 " << ip << MsgSip::Value::ENDL
       << "m=audio " << audPort
       << ((audKey.empty())? " RTP/AVP ": " RTP/SAVP ");
    if (vidPort <= 0 && doAcelp)
    {
        os << PAYLOAD_AUDIO_ACELP << MsgSip::Value::ENDL
           << "a=rtpmap:" << PAYLOAD_AUDIO_ACELP << " TETRA/"
           << SAMPLE_AUDIO << MsgSip::Value::ENDL
           << "a=ptime:" << PTIME_ACELP;
    }
    else
    {
        os << PAYLOAD_AUDIO_PCMA << MsgSip::Value::ENDL
           << "a=rtpmap:" << PAYLOAD_AUDIO_PCMA << " PCMA/"
           << SAMPLE_AUDIO << MsgSip::Value::ENDL
           << "a=ptime:" << PTIME_PCMA;
    }
    if (!audKey.empty())
        os << MsgSip::Value::ENDL
           << "a=crypto:1 AEAD_AES_256_GCM_8 inline:" << audKey;
    if (vidPort > 0)
    {
        os << MsgSip::Value::ENDL
           << "m=video " << vidPort
           << ((vidKey.empty())? " RTP/AVP ": " RTP/SAVP ")
           << PAYLOAD_VIDEO_H264 << MsgSip::Value::ENDL
           << "a=rtpmap:" << PAYLOAD_VIDEO_H264 << " H264/" << SAMPLE_VIDEO;
        if (!vidKey.empty())
            os << MsgSip::Value::ENDL
               << "a=crypto:1 AEAD_AES_256_GCM_8 inline:" << vidKey;
    }
    return os.str();
}

int VoipSessionBase::getCallPartyInUri(const string &sipLine,
                                       bool          checkCall,
                                       int          *type,
                                       char         *numPrefix)
{
    //From: "name" <sip:1234@1.2.3.4>;tag=e4F2y8D6v0r1m
    //To: "name" <sip:1234@1.2.3.4>  (individual call)
    //To: "name" <sip:*1234@1.2.3.4> (group call)
    //To: "name" <sip:G1234@1.2.3.4> (incoming group call)
    //To: "name" <sip:J1234_5678@1.2.3.4> (SSIC on call started by network)
    //To: "name" <sip:K1234_5678@1.2.3.4> (SSIC on call started by dispatcher)
    //To: "name" <sip:&1234@1.2.3.4> (call listening)
    //get the 1234 part
    size_t pos = sipLine.find(URI_PREFIX);
    if (pos == string::npos)
        return -1;
    string str(sipLine, pos + URI_PREFIX.size());
    switch (str[0])
    {
        case NUMPREFIX_AMBIENCE:
        case NUMPREFIX_IND_PTT:
        case NUMPREFIX_LISTEN:
            if (type != 0)
                *type = MsgSp::Value::IDENTITY_TYPE_ISSI;
            if (numPrefix != 0)
                *numPrefix = str[0];
            str.erase(0, 1); //remove the prefix
            break;
        case NUMPREFIX_BROADCAST:
        case NUMPREFIX_GROUP:
        case NUMPREFIX_GROUP_IN:
        case NUMPREFIX_GROUP_PREEMPTIVE:
        case NUMPREFIX_SSIC:
        case NUMPREFIX_SSIC_DISP:
            if (type != 0)
                *type = MsgSp::Value::IDENTITY_TYPE_GSSI;
            if (numPrefix != 0)
                *numPrefix = str[0];
            str.erase(0, 1);
            break;
        case NUMPREFIX_DISPATCHER:
        case NUMPREFIX_TERMINATE:
            if (numPrefix != 0)
                *numPrefix = str[0];
            str.erase(0, 1);
            break;
        default:
            if (type != 0)
                *type = MsgSp::Value::IDENTITY_TYPE_ISSI;
            if (numPrefix != 0)
                *numPrefix = 0;
            break;
    }
    int ssi = Utils::fromString<int>(str);
    checkDisp(ssi);
    if (checkCall && mSsiCallMap.count(ssi) == 0)
        return 0;
    return ssi;
}

int VoipSessionBase::getCallPartyInName(const string &sipLine,
                                        bool          checkCall,
                                        int          *type,
                                        char         *numPrefix)
{
    //From: "1234" <sip:1007@1.2.3.4>  (individual call)
    //From: "*1234" <sip:1007@1.2.3.4> (group call)
    //From: "J1234_5678" <sip:1007@1.2.3.4> (SSIC on call by network)
    //From: "K1234_5678" <sip:1007@1.2.3.4> (SSIC on call by dispatcher)
    //From: "&1234" <sip:1007@1.2.3.4> (call listening)
    //From: "!1234" <sip:1007@1.2.3.4> (call termination)
    //get the 1234 part
    size_t pos = sipLine.find('\"');
    if (pos == string::npos)
        return -1;
    string str(sipLine, pos + 1);
#ifdef SERVERAPP
    if (str.find(HUPALL) == 0 && str.find('"') == HUPALL.size())
    {
        //From: "hupall" <sip:990@1.2.3.4> (termination of all calls)
        if (numPrefix != 0)
            *numPrefix = NUMPREFIX_TERMINATE;
        return 0;
    }
#endif
    switch (str[0])
    {
        case NUMPREFIX_AMBIENCE:
        case NUMPREFIX_IND_PTT:
        case NUMPREFIX_LISTEN:
            if (type != 0)
                *type = MsgSp::Value::IDENTITY_TYPE_ISSI;
            if (numPrefix != 0)
                *numPrefix = str[0];
            str.erase(0, 1); //remove the prefix
            break;
        case NUMPREFIX_BROADCAST:
        case NUMPREFIX_GROUP:
        case NUMPREFIX_GROUP_PREEMPTIVE:
        case NUMPREFIX_SSIC:
        case NUMPREFIX_SSIC_DISP:
            if (type != 0)
                *type = MsgSp::Value::IDENTITY_TYPE_GSSI;
            if (numPrefix != 0)
                *numPrefix = str[0];
            str.erase(0, 1);
            break;
        case NUMPREFIX_DISPATCHER:
        case NUMPREFIX_TERMINATE:
            if (numPrefix != 0)
                *numPrefix = str[0];
            str.erase(0, 1);
            break;
        default:
            if (type != 0)
                *type = MsgSp::Value::IDENTITY_TYPE_ISSI;
            if (numPrefix != 0)
                *numPrefix = 0;
            break;
    }
    int ssi = Utils::fromString<int>(str);
    checkDisp(ssi);
    if (checkCall && mSsiCallMap.count(ssi) == 0)
        return 0;
    return ssi;
}

bool VoipSessionBase::getReqUri(string &sipLine)
{
    //From: "name" <sip:1234@1.2.3.4>;tag=e4F2y8D6v0r1m
    //To: "name" <sip:1234@1.2.3.4>
    //get the sip:1234@1.2.3.4 part
    size_t pos = sipLine.find(URI_PREFIX);
    if (pos != string::npos)
    {
        sipLine.erase(0, pos);
        pos = sipLine.find('>');
        if (pos != string::npos)
        {
            sipLine.erase(pos);
            return true;
        }
    }
    return false;
}

int VoipSessionBase::getRtpPort(const MsgSip &msg, bool isVideo)
{
    int val = 0;
    string str;
    istringstream is(msg.getContentBody());
    while (getline(is, str))
    {
        //m=audio 27706 RTP/AVP 8
        //m=video 31994 RTP/AVP 96
        if ((!isVideo && str.compare(0, 7, "m=audio") == 0) ||
            (isVideo && str.compare(0, 7, "m=video") == 0))
        {
            istringstream iss(str);
            iss >> str >> val;
            break;
        }
    }
    return val;
}

string VoipSessionBase::getCryptoKey(const MsgSip &msg, bool isVideo)
{
    string str;
    istringstream is(msg.getContentBody());
    bool found = false;
    while (getline(is, str))
    {
        if (!found)
        {
            if ((!isVideo && str.compare(0, 7, "m=audio") == 0) ||
                (isVideo && str.compare(0, 7, "m=video") == 0))
               found = true;
            else
                continue;
        }
        //a=crypto:3 AEAD_AES_128_GCM_8 inline:<key>
        if (str.compare(0, 8, "a=crypto") == 0)
        {
            size_t pos = str.find("inline:");
            if (pos != std::string::npos)
            {
                str.erase(0, pos + 7);
                str.erase(str.find_last_not_of(" \t\n\r\f\v") + 1);
                return str;
            }
            break;
        }
    }
    return "";
}

#ifdef SIPTCP
bool VoipSessionBase::connectToServer()
{
    const unsigned int TRYBLOCKS = 10;
    unsigned int count = 1;
    int connectRes = 0;
    string toServer(" to ");
    toServer.append(mSocket->getRemoteAddrStr());
    while (mState != STATE_STOPPED)
    {
        if (count == 1)
            LOGX(DEBUG, "Connection attempt 1" << toServer << "...");
        else if (count <= TRYBLOCKS)
            LOGX(DEBUG, "Connection attempt " << count << toServer
                 << ", last error = " << -connectRes
                 << Socket::getErrorStr(-connectRes) << "...");
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
            LOGX(ERROR, "ERROR: Server connection failed after " << count
                 << " attempts. Will keep trying...");
        PalThread::sleep(2);
        if (mState == STATE_STOPPED)
            return false;
        ++count;
    } //while (mState != STATE_STOPPED)
    if (mState != STATE_CONNECTED)
        return false;
    LOGX(VERBOSE, "Connected" << toServer << " after attempt " << count);
    sendMsg(createRegisterMsg(0));
    return true;
}

#else //SIPTCP
UdpSocket *VoipSessionBase::getUdpSocket(int &localPort)
{
    int maxPort = localPort + 999;
    auto *sock = new UdpSocket(mServerPort, mServerIp, localPort);
    while (!sock->isValid())
    {
        LOGGER_ERROR(mLogger, LOGPREFIX << "init: "
                     "Socket creation failed on port " << localPort);
        delete sock;
        if (localPort >= maxPort)
            return 0;
        //keep trying with incremental port
        sock = new UdpSocket(mServerPort, mServerIp, ++localPort);
    }
    LOGGER_INFO(mLogger, LOGPREFIX << "UDP local port " << localPort);
    return sock;
}
#endif //SIPTCP

int VoipSessionBase::checkDisp(int &id)
{
#ifdef MOBILE
    if (id == CALLED_PARTY_DISP)
        id = mDispIssi;
#endif
    return id;
}
