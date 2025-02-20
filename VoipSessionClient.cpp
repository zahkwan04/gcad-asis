/**
 * VOIP session implementation for a client application.
 *
 * Copyright (C) Sapura Secured Technologies, 2014-2024. All Rights Reserved.
 *
 * @file
 * @version $Id: VoipSessionClient.cpp 1814 2024-02-15 07:33:12Z zulzaidi $
 * @author Ahmad Syukri
 */
#include <assert.h>
#include <openssl/rand.h> //RAND_bytes

#include "Locker.h"
#include "RtpSession.h"
#ifndef MOBILE
#include "SubsData.h"
#endif
#include "VoipSessionClient.h"

using namespace std;

static const int    RTP_PORT_MIN     = 20000; //must be even
static const int    RTP_PORT_MAX     = 20100;
static const int    BASE64_CHUNK     = 3; //source data processing size (bytes)
static const int    BASE64_ENC_CHUNK = 4; //encoded data group size (bytes)
static const string BASE64_CHARS("ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                 "abcdefghijklmnopqrstuvwxyz"
                                 "0123456789+/");
static const string DEFAULT_KEY("NPF3Q+humdVMhvWwhzcpJTjUEQxaJ+PMEqicP+FknNPyOi"
                                "HwCer9yjhdGX4=");
static const string PASSWORD_STR("1234");

bool VoipSessionClient::sAcelp(false);

#ifdef MOBILE
#ifdef NATIVE_LOG
static ostringstream sOss;
#define STRCB(arg)                  \
        do                          \
        {                           \
            sOss << "VOIP " << arg; \
            mStrCbFn(sOss.str());   \
            sOss.str("");           \
        }                           \
        while (0)
#else
#define STRCB(arg)
#endif //NATIVE_LOG

#else //MOBILE
static const string LOGPREFIX("VoipSessionClient:: ");
#define STRCB(arg)
#endif //MOBILE

VoipSessionClient::VoipSessionClient(int             userId,
                                     const string   &version,
                                     const string   &localIp,
                                     const string   &serverIp,
#ifdef MOBILE
                                     StrCallbackFn   strCbFn,
#else
                                     Logger         *logger,
                                     void           *callbackObj,
#endif
                                     RecvCallbackFn  callbackFn) :
VoipSessionBase(userId,
#ifdef MOBILE
                strCbFn,
#else
                logger, callbackObj,
#endif
                callbackFn),
mSessionId(0), mLocalRtpPort(RTP_PORT_MIN)
{
    ostringstream os;
    os << PASSWORD_STR << userId << PASSWORD_STR;
    mPassword  = os.str();
    mLocalIp   = localIp;
    mServerIp  = serverIp;
#ifdef MOBILE
    mUserAgent = "STM/" + version;
#else
    mUserAgent = "SCAD/" + version;
#endif
    RtpSession::setRemoteIp(serverIp);
    LOGGER_INFO(mLogger, LOGPREFIX << "Local IP: " << mLocalIp);
    init();
}

void VoipSessionClient::setLocalIp(const string &ip)
{
    mLocalIp = ip;
    setSipFields(); //update fields that use mLocalIp
}

bool VoipSessionClient::setServer(const string &ip, int port)
{
    if (!isValid())
        return false;
    LOGGER_DEBUG(mLogger, LOGPREFIX << "setServer: " << ip << " port " << port
                 << ", current server " << mServerIp << ':' << mServerPort);
    bool serverChanged = (ip != mServerIp);
    if (serverChanged)
    {
        if (!Socket::validateIp(ip))
        {
            LOGGER_ERROR(mLogger, LOGPREFIX << "setServer: Invalid IP " << ip);
            return false;
        }
        mServerIp = ip;
        RtpSession::setRemoteIp(ip);
    }
    if (port > 0)
        mServerPort = port;
#ifdef NO_VOIP
    if (mState == STATE_STOPPED)
        start();
    return true;
#endif
    mSocket->setRemoteAddr(ip, port);
    if (mState == STATE_STOPPED)
        return start();
    if (serverChanged)
    {
        mUserUri.assign("<").append(URI_PREFIX).append(mUserId).append("@")
                .append(mServerIp).append(">");
        mTagIdentity.assign(mUserId).append(Utils::randomString(TAG_LEN, TAG_LEN));
        //in SIPTCP, this state change forces recv thread to disconnect and
        //connect to new server
        mState = STATE_STARTED;
#ifndef SIPTCP
        sendMsg(createRegisterMsg(0));
#endif
    }
    return true;
}

void VoipSessionClient::reregister(const string &localIp, const string &svrIp)
{
    if (!localIp.empty() && localIp != mLocalIp)
        setLocalIp(localIp);
    if (!svrIp.empty() && svrIp != mServerIp)
    {
        setServer(svrIp);
    }
    else
    {
        //in SIPTCP, this state change forces recv thread to reconnect
        mState = STATE_STARTED;
#ifndef SIPTCP
        sendMsg(createRegisterMsg(0));
#endif
    }
}

string VoipSessionClient::callSetupIndDuplex(int           calledParty,
                                             const string &params)
{
    return callSetup(calledParty, 0, "", false, 0, params);
}

string VoipSessionClient::callSetupIndDisp(int           calledParty,
                                           const string &calledDomain,
                                           const string &params)
{
    return callSetup(calledParty, 0, calledDomain, false, NUMPREFIX_DISPATCHER,
                     params);
}

string VoipSessionClient::callSetupVideo(int           calledParty,
                                         const string &calledDomain,
                                         const string &params)
{
    return callSetup(calledParty, 0, calledDomain, true, NUMPREFIX_DISPATCHER,
                     params);
}

#ifdef MOBILE
string VoipSessionClient::callSetupDisp(int calledParty, const string &params)
{
    mDispIssi = calledParty;
    return callSetup(calledParty, 0, "", false, 0, params);
}

string VoipSessionClient::callSetupVideoDisp(int           calledParty,
                                             const string &params)
{
    mDispIssi = calledParty;
    return callSetup(calledParty, 0, "", true, 0, params);
}
#endif

string VoipSessionClient::callSetupIndPtt(int           calledParty,
                                          const string &params)
{
    return callSetup(calledParty, 0, "", false, NUMPREFIX_IND_PTT, params);
}

string VoipSessionClient::callSetupGrp(int calledParty, int priority)
{
    if (priority >= MsgSp::Value::CALL_PRIORITY_PREEMPTIVE_1)
        return callSetup(calledParty, 0, "", false, NUMPREFIX_GROUP_PREEMPTIVE);
    return callSetup(calledParty, 0, "", false, NUMPREFIX_GROUP);
}

string VoipSessionClient::callSetupBroadcast(int calledParty)
{
    return callSetup(calledParty, 0, "", false, NUMPREFIX_BROADCAST);
}

string VoipSessionClient::callSetupSsic(int  calledParty,
                                        int  callingParty,
                                        bool isDisp)
{
    return callSetup(calledParty, callingParty, "", false,
                     (isDisp)? NUMPREFIX_SSIC_DISP: NUMPREFIX_SSIC);
}

string VoipSessionClient::callSetupAmbience(int           calledParty,
                                            const string &params)
{
    return callSetup(calledParty, 0, "", false, NUMPREFIX_AMBIENCE, params);
}

string VoipSessionClient::callSetupListen(int ssi)
{
    return callSetup(ssi, 0, "", false, NUMPREFIX_LISTEN);
}

int VoipSessionClient::callConnect(int     ssi,
                                   string *audKey,
                                   int    *vidPort,
                                   string *vidKey)
{
#ifdef NO_VOIP
    return 1; //dummy
#endif
    LOGGER_DEBUG(mLogger, LOGPREFIX << "callConnect: ssi=" << ssi);
    Locker lock(&mCallMapLock);
    if (mSsiCallMap.count(ssi) == 0)
    {
        LOGGER_ERROR(mLogger, LOGPREFIX << "callConnect: Invalid SSI "
                     << ssi);
        return 0;
    }
    auto *data = &mSsiCallMap[ssi];
    data->setupPhase = false;
    auto *m = createRespMsg(MsgSip::Type::INVITE, MsgSip::Value::RESP_OK, 0,
                            data);
    m->addField(MsgSip::Field::CALL_ID, data->voipCallId)
      .addField(MsgSip::Field::CONTACT, mContact)
      .addField(MsgSip::Field::ALLOW, VALUE_ALLOW)
      .addField(MsgSip::Field::SUPPORTED, VALUE_SUPPORTED);
    int audPort = getLocalRtpPort();
    if (audKey != 0)
        *audKey = getLocalCryptoKey();
    if (vidPort != 0)
    {
        *vidPort = getLocalRtpPort();
        if (vidKey != 0)
            *vidKey = getLocalCryptoKey();
    }
    string sdp(generateSdp(getNewSessionId(), mLocalIp, sAcelp, audPort,
                           (vidPort != 0)? *vidPort: 0,
                           (audKey != 0)? *audKey: "",
                           (vidKey != 0)? *vidKey: ""));
    m->addField(MsgSip::Field::CONTENT_TYPE, VALUE_CONTENT_TYPE_SDP)
      .addField(MsgSip::Field::CONTENT_LENGTH, sdp.length())
      .setContentBody(sdp);
    sendMsg(m);
    return audPort;
}

bool VoipSessionClient::callRelease(int ssi)
{
#ifdef NO_VOIP
    return true;
#endif
    LOGGER_DEBUG(mLogger, LOGPREFIX << "callRelease: ssi=" << ssi);
    Locker lock(&mCallMapLock);
    if (mSsiCallMap.count(ssi) == 0)
    {
        LOGGER_ERROR(mLogger, LOGPREFIX << "callRelease: Invalid SSI " << ssi);
        return false;
    }

    bool doErase = true;
    MsgSip *m;
    auto *data = &mSsiCallMap[ssi];
    if (!data->setupPhase)
    {
        if (data->incoming)
        {
            string reqUri(data->from);
            getReqUri(reqUri);
            m = createReqMsg(MsgSip::Type::BYE, reqUri);
            data->to.append(PARAM_TAG + mTagIdentity);
            m->addField(MsgSip::Field::FROM, data->to)
              .addField(MsgSip::Field::TO, data->from);
        }
        else
        {
            m = createReqMsg(MsgSip::Type::BYE, data->uri);
            m->addField(MsgSip::Field::FROM, data->from)
              .addField(MsgSip::Field::TO, data->to);
        }
        m->setSeqId(getNewMsgId());
    }
    else if (data->incoming)
    {
        //reject incoming call
        m = createRespMsg(MsgSip::Type::INVITE,
                          MsgSip::Value::RESP_RFAIL_BUSY_HERE, 0, data);
    }
    else
    {
        //caller cancels outgoing call
        m = createReqMsg(MsgSip::Type::CANCEL, data->uri, 0, data);
        doErase = false;
    }
    m->addField(MsgSip::Field::CALL_ID, data->voipCallId);
    if (doErase)
        mSsiCallMap.erase(ssi);
    sendMsg(m);
    return true;
}

string VoipSessionClient::callSetup(int           calledParty,
                                    int           callingParty,
                                    const string &calledDomain,
                                    bool          isVideo,
                                    char          numPrefix,
                                    const string &params)
{
#ifdef NO_VOIP
    return "";
#endif
    ostringstream os;
    os << URI_PREFIX;
    if (numPrefix != 0)
        os << numPrefix;
#ifdef MOBILE
    os << ((calledParty == mDispIssi)? CALLED_PARTY_DISP: calledParty);
#else
    os << calledParty;
#endif
    if (callingParty != 0)
        os << '_' << callingParty;
    os << '@';
    if (calledDomain.empty())
        os << mServerIp;
    else
        os << calledDomain;
    string calledUri(os.str());
    MsgSip *m = createReqMsg(MsgSip::Type::INVITE, calledUri);
    os.clear();
    os.str("");
    os << '\"' << mUserId << "\" " << mUserUri << PARAM_TAG << mTagIdentity;
    int id = getNewMsgId();
    m->addField(MsgSip::Field::FROM, os.str())
      .addField(MsgSip::Field::TO, "<" + calledUri + ">")
      .setSeqId(id);
    os.clear();
    os.str("");
    //unique call ID with user ID, called party, message ID and epoch seconds
    os << "0-" << mUserId << '-' << calledParty << '-' << id << '-' << time(0);
    m->addField(MsgSip::Field::CALL_ID, os.str())
      .addField(MsgSip::Field::CONTACT, mContact)
      .addField(MsgSip::Field::ALLOW, VALUE_ALLOW)
      .addField(MsgSip::Field::SUPPORTED, VALUE_SUPPORTED);
    PalLock::take(&mCallMapLock);
    mSsiCallMap[calledParty] = CallData(false,
                                      m->getFieldString(MsgSip::Field::CALL_ID),
                                      calledUri,
                                      m->getFieldString(MsgSip::Field::VIA),
                                      m->getFieldString(MsgSip::Field::FROM),
                                      m->getFieldString(MsgSip::Field::TO));
    mSsiCallMap[calledParty].seqId = id;
    if (!params.empty())
    {
        //user-to-user string may have leading/trailing spaces - quote it
        string s("\"");
        s.append(MsgSp::scramble(params, true)).append("\"");
        m->addField(MsgSip::Field::USER_TO_USER, s);
        mSsiCallMap[calledParty].user2User = s;
    }
    int audPort = getLocalRtpPort();
    int vidPort = (isVideo)? getLocalRtpPort(): 0;
    string audKey(getLocalCryptoKey());
    string vidKey((isVideo)? getLocalCryptoKey(): "");
    mSsiCallMap[calledParty].audRtpPort = audPort,
    mSsiCallMap[calledParty].vidRtpPort = vidPort;
    mSsiCallMap[calledParty].audRtpKey  = audKey;
    mSsiCallMap[calledParty].vidRtpKey  = vidKey;
    string sdp(generateSdp(getNewSessionId(), mLocalIp, sAcelp, audPort,
                           vidPort, audKey, vidKey));
    m->addField(MsgSip::Field::CONTENT_TYPE, VALUE_CONTENT_TYPE_SDP)
      .addField(MsgSip::Field::CONTENT_LENGTH, sdp.length())
      .setContentBody(sdp);
    PalLock::release(&mCallMapLock);
    sendMsg(m);
    return os.str();
}

MsgSip *VoipSessionClient::processMsg(const MsgSip *msg, MsgSp *&msgSp)
{
    if (msg == 0)
    {
        assert("Bad param in VoipSessionClient::processMsg" == 0);
        return 0;
    }

    int     val;
    string  str;
    MsgSip *resp = 0;
    switch (msg->getType())
    {
        case MsgSip::Type::BYE:
        {
            if (msg->getRespCode() == MsgSip::Value::UNDEFINED)
            {
                //server ending call
                Locker lock(&mCallMapLock);
                val = getCallPartyInName(
                                msg->getFieldString(MsgSip::Field::FROM), true);
                if (val <= 0)
                {
                    val = getCallPartyInName(
                                  msg->getFieldString(MsgSip::Field::TO), true);
                    if (val <= 0)
                    {
                        val = getCallPartyInUri(
                                msg->getFieldString(MsgSip::Field::FROM), true);
                        if (val <= 0)
                        {
                            val = getCallPartyInUri(
                                  msg->getFieldString(MsgSip::Field::TO), true);
                            if (val <= 0)
                            {
                                //terminated call
                                resp = createRespMsg(MsgSip::Type::BYE,
                                                     MsgSip::Value::RESP_OK,
                                                     msg);
                                break;
                            }
                        }
                    }
                }
                str = msg->getFieldString(MsgSip::Field::CALL_ID);
                auto *data = &mSsiCallMap[checkDisp(val)];
                if (str != data->voipCallId)
                {
                    //call ended via direct MsgSp and replaced by a new one
                    resp = createRespMsg(MsgSip::Type::BYE,
                                         MsgSip::Value::RESP_OK, msg);
                    break;
                }
                data->seqId = msg->getSeqId();
                data->via = msg->getFieldString(MsgSip::Field::VIA);
                data->from = msg->getFieldString(MsgSip::Field::FROM);
                data->to = msg->getFieldString(MsgSip::Field::TO);

                msgSp = new MsgSp(MsgSp::Type::CALL_RELEASE);
                msgSp->addField(MsgSp::Field::CALL_ID, data->nwkCallId)
                      .addField(MsgSp::Field::DISCONNECT_CAUSE,
                                MsgSp::Value::DC_USER_REQUESTED);
                if (data->incoming)
                    msgSp->addField(MsgSp::Field::CALLING_PARTY, val);
                else
                    msgSp->addField(MsgSp::Field::CALLED_PARTY, val);

                resp = createRespMsg(MsgSip::Type::BYE,
                                     MsgSip::Value::RESP_OK, 0, data);
                resp->addField(MsgSip::Field::CALL_ID, str);
                mSsiCallMap.erase(val);
            } //if (msg->getRespCode() == MsgSip::Value::UNDEFINED)
            break;
        }
        case MsgSip::Type::CANCEL:
        {
            if (msg->getRespCode() != MsgSip::Value::UNDEFINED)
            {
                //server response to a canceled outgoing call, nothing more
                //to do
                break;
            }
            str = msg->getFieldString(MsgSip::Field::FROM);
            Locker lock(&mCallMapLock);
            val = getCallPartyInName(str, true);
            if (val <= 0)
                break;
            auto &data = mSsiCallMap[checkDisp(val)];
            if (msg->getFieldString(MsgSip::Field::CALL_ID) !=
                data.voipCallId)
            {
                //this is for a duplicate invite
                resp = createRespMsg(MsgSip::Type::CANCEL,
                                     MsgSip::Value::RESP_OK, msg);
                break;
            }
            data.seqId = msg->getSeqId();
            data.via = msg->getFieldString(MsgSip::Field::VIA);
            data.from = str;
            data.to = msg->getFieldString(MsgSip::Field::TO);
            msgSp = new MsgSp(MsgSp::Type::CALL_RELEASE);
            msgSp->addField(MsgSp::Field::CALL_ID, data.nwkCallId)
                  .addField(MsgSp::Field::CALLING_PARTY, val);
            if (msg->getReasonCause() ==
                MsgSip::Value::PARAM_REASON_CAUSE_CALL_COMPLETED_ELSEWHERE)
                msgSp->addField(MsgSp::Field::DISCONNECT_CAUSE,
                                MsgSp::Value::DC_CALL_COMPLETED_ELSEWHERE);
            else
                msgSp->addField(MsgSp::Field::DISCONNECT_CAUSE,
                                MsgSp::Value::DC_USER_REQUESTED);
            resp = createRespMsg(MsgSip::Type::CANCEL,
                                 MsgSip::Value::RESP_OK, 0, &data);
            resp->addField(MsgSip::Field::CALL_ID, data.voipCallId);
            mSsiCallMap.erase(val);
            break;
        }
        case MsgSip::Type::INFO:
        {
            //application level information is not supported
            resp = createRespMsg(MsgSip::Type::INFO,
                                 MsgSip::Value::RESP_RFAIL_METHOD_NOT_ALLOWED,
                                 msg);
            break;
        }
        case MsgSip::Type::INVITE:
        {
            switch (msg->getRespCode())
            {
                case MsgSip::Value::RESP_GFAIL_DECLINE:
                case MsgSip::Value::RESP_RFAIL_BUSY_HERE:
                case MsgSip::Value::RESP_RFAIL_NOT_FOUND:
                case MsgSip::Value::RESP_RFAIL_SVC_UNAVAILABLE:
                case MsgSip::Value::RESP_RFAIL_TEMP_UNAVAILABLE:
                {
                    Locker lock(&mCallMapLock);
                    val = getCallPartyInUri(msg->getFieldString(
                                                             MsgSip::Field::TO),
                                            true);
                    if (val <= 0)
                        break;
                    msgSp = new MsgSp(MsgSp::Type::CALL_RELEASE);
                    msgSp->addField(MsgSp::Field::CALL_ID,
                                    msg->getFieldString(MsgSip::Field::CALL_ID))
                          .addField(MsgSp::Field::CALLED_PARTY, checkDisp(val))
                          .addField(MsgSp::Field::DISCONNECT_CAUSE,
                                disconnectCauseFromSipResp(msg->getRespCode()));
                    resp = createReqMsg(MsgSip::Type::ACK, mSsiCallMap[val].uri,
                                        msg);
                    mSsiCallMap.erase(val);
                    break;
                }
                case MsgSip::Value::RESP_OK:
                {
                    Locker lock(&mCallMapLock);
                    val = getCallPartyInUri(msg->getFieldString(
                                                             MsgSip::Field::TO),
                                            true);
                    if (val <= 0)
                        break;
                    auto &data = mSsiCallMap[checkDisp(val)];
                    LOGGER_DEBUG(mLogger, LOGPREFIX << "INVITE success for SSI "
                                 << val);
                    data.to = msg->getFieldString(MsgSip::Field::TO);
                    data.setupPhase = false;
                    msgSp = new MsgSp(MsgSp::Type::CALL_CONNECT);
                    msgSp->addField(MsgSp::Field::CALL_ID,
                                    msg->getFieldString(MsgSip::Field::CALL_ID))
                          .addField(MsgSp::Field::CALLED_PARTY, val)
                          .addField(MsgSp::Field::VOIP_AUDIO_RTP_PORT,
                                    getRtpPort(*msg))
                          .addField(MsgSp::Field::VOIP_AUDIO_RTP_LPORT,
                                    data.audRtpPort);
                    if (!data.audRtpKey.empty())
                        msgSp->addField(MsgSp::Field::VOIP_AUDIO_RTP_KEY,
                                        getCryptoKey(*msg))
                              .addField(MsgSp::Field::VOIP_AUDIO_RTP_LKEY,
                                        data.audRtpKey);
                    if (data.vidRtpPort > 0)
                    {
                        msgSp->addField(MsgSp::Field::VOIP_VIDEO_RTP_PORT,
                                        getRtpPort(*msg, true))
                              .addField(MsgSp::Field::VOIP_VIDEO_RTP_LPORT,
                                        data.vidRtpPort);
                        if (!data.vidRtpKey.empty())
                            msgSp->addField(MsgSp::Field::VOIP_VIDEO_RTP_KEY,
                                            getCryptoKey(*msg, true))
                                  .addField(MsgSp::Field::VOIP_VIDEO_RTP_LKEY,
                                            data.vidRtpKey);
                    }
                    resp = createReqMsg(MsgSip::Type::ACK, data.uri, 0, &data);
                    if (!data.auth.empty())
                        resp->addField(MsgSip::Field::PROXY_AUTHORIZATION,
                                       data.auth);
                    resp->addField(MsgSip::Field::CALL_ID, data.voipCallId);
                    break;
                }
                case MsgSip::Value::RESP_PROV_RINGING:
                {
                    Locker lock(&mCallMapLock);
                    val = getCallPartyInUri(msg->getFieldString(
                                                             MsgSip::Field::TO),
                                            true);
                    msgSp = new MsgSp(MsgSp::Type::CALL_ALERT);
                    msgSp->addField(MsgSp::Field::CALL_ID,
                                    msg->getFieldString(MsgSip::Field::CALL_ID))
                          .addField(MsgSp::Field::CALLED_PARTY, val);
                    break;
                }
                case MsgSip::Value::RESP_PROV_TRYING:
                {
                    break; //do nothing
                }
                case MsgSip::Value::RESP_RFAIL_PROXY_AUTH_REQD:
                {
                    Locker lock(&mCallMapLock);
                    val = getCallPartyInUri(msg->getFieldString(
                                                             MsgSip::Field::TO),
                                            true);
                    if (val <= 0)
                        break;
                    auto &data = mSsiCallMap[checkDisp(val)];
                    sendMsg(createReqMsg(MsgSip::Type::ACK, data.uri, msg));
                    resp = createReqMsg(MsgSip::Type::INVITE, data.uri, msg);
                    resp->setSeqId(getNewMsgId())
                         .addField(MsgSip::Field::CONTACT, mContact)
                         .addField(MsgSip::Field::ALLOW, VALUE_ALLOW);
                    addAuthParam(*msg, *resp, data.uri);
                    data.seqId = resp->getSeqId();
                    data.auth = resp->getFieldString(
                                            MsgSip::Field::PROXY_AUTHORIZATION);
                    resp->addField(MsgSip::Field::TO, data.to);
#ifndef NOSIPSETUP
                    if (!data.user2User.empty())
                        resp->addField(MsgSip::Field::USER_TO_USER,
                                       data.user2User);
#endif
                    break;
                }
                case MsgSip::Value::RESP_RFAIL_REQ_TERMINATED:
                {
                    Locker lock(&mCallMapLock);
                    val = getCallPartyInUri(msg->getFieldString(
                                                             MsgSip::Field::TO),
                                            true);
                    if (val <= 0)
                        break;
                    resp = createReqMsg(MsgSip::Type::ACK,
                                        mSsiCallMap[checkDisp(val)].uri, msg);
                    mSsiCallMap.erase(val);
                    break;
                }
                case MsgSip::Value::UNDEFINED:
                {
                    //incoming call
                    str = msg->getFieldString(MsgSip::Field::TO);
                    CallData data(true,
                                  msg->getFieldString(MsgSip::Field::CALL_ID),
                                  msg->getReqUri(),
                                  msg->getFieldString(MsgSip::Field::VIA),
                                  msg->getFieldString(MsgSip::Field::FROM),
                                  str);
                    data.seqId = msg->getSeqId();
                    bool dispCall = false;
#ifndef NOSIPSETUP
                    str = msg->getFieldString(MsgSip::Field::USER_TO_USER);
                    if (!str.empty())
                    {
                        //remove enclosing quotes before descrambling
                        str.erase(str.size() - 1).erase(0, 1);
                        msgSp = MsgSp::parse(MsgSp::scramble(str, false));
                        if (msgSp == 0)
                        {
                            LOGGER_ERROR(mLogger, LOGPREFIX << "processMsg: "
                                         << msg->getName()
                                         << ": Invalid MsgSp data");
                            resp = createRespMsg(MsgSip::Type::INVITE,
                                              MsgSip::Value::RESP_RFAIL_BAD_REQ,
                                              0, &data);
                            resp->addField(MsgSip::Field::CALL_ID,
                                           data.voipCallId);
                            sendMsg(resp);
                            break;
                        }
                        data.nwkCallId = msgSp->getFieldInt(
                                                         MsgSp::Field::CALL_ID);
                        if (data.nwkCallId == 0)
                            msgSp->addField(MsgSp::Field::CALL_ID,
                                            data.voipCallId);
                        val = msgSp->getFieldInt(MsgSp::Field::CALLING_PARTY);
                        if (msgSp->getFieldInt(
                                            MsgSp::Field::CALLING_PARTY_TYPE) ==
                            MsgSp::Value::IDENTITY_TYPE_DISPATCHER)
                            dispCall = true;
#ifndef MOBILE
                        //STM client adds msg ID - need to remove here
                        msgSp->removeField(MsgSp::Field::MSG_ID);
#endif
                    } //if (!str.empty())
                    else
#endif //!NOSIPSETUP
                    {
                        str = msg->getFieldString(MsgSip::Field::TO);
                        val = getCallPartyInUri(str, false);
                        if (val == Utils::fromString<int>(mUserId))
                        {
                            //From: "1234567" <sip:123@1.2.3.4>
                            str = msg->getFieldString(MsgSip::Field::FROM);
                            val = getCallPartyInName(str, false);
                            //in network call, the caller name is the terminal
                            //ISSI and the user ID is the server ID;
                            //in dispatcher call, both contain the dispatcher ID
                            if (val == getCallPartyInUri(str, false))
                                dispCall = true;
                        }
                        else
                        {
                            resp = createRespMsg(MsgSip::Type::INVITE,
                                  MsgSip::Value::RESP_RFAIL_NOT_ACCEPTABLE_HERE,
                                  0, &data);
                            resp->addField(MsgSip::Field::CALL_ID,
                                           data.voipCallId);
                            sendMsg(resp);
                            resp = createUnregisterMsg(val, 0);
                            break;
                        }
                    }
#ifndef MOBILE
                    if (!dispCall && !SubsData::validIssi(val) &&
                        !SubsData::isMobile(val))
                    {
                        //call is for another branch
                        delete msgSp;
                        msgSp = 0;
                        resp = createRespMsg(MsgSip::Type::INVITE,
                                  MsgSip::Value::RESP_RFAIL_NOT_ACCEPTABLE_HERE,
                                  0, &data);
                        resp->addField(MsgSip::Field::CALL_ID, data.voipCallId);
                        break;
                    }
#endif //!MOBILE
                    PalLock::take(&mCallMapLock);
                    if (mSsiCallMap.count(val) != 0)
                    {
                        //duplicate invite possible reasons:
                        //-call abnormally ended but undetected
                        //-stale duplicate freeswitch registration entry
                        PalLock::release(&mCallMapLock);
                        callRelease(val); //release to start a new one
                        PalLock::take(&mCallMapLock);
                    }
                    mSsiCallMap[val] = data;
                    if (msgSp == 0)
                    {
                        msgSp = new MsgSp(MsgSp::Type::CALL_SETUP);
                        if (data.nwkCallId == 0)
                            msgSp->addField(MsgSp::Field::CALL_ID,
                                            data.voipCallId);
                        else
                            msgSp->addField(MsgSp::Field::CALL_ID,
                                            data.nwkCallId);
                        msgSp->addField(MsgSp::Field::CALLING_PARTY, val);
                        if (dispCall)
                            val = MsgSp::Value::IDENTITY_TYPE_DISPATCHER;
#ifndef MOBILE
                        else if (SubsData::isMobile(val))
                            val = MsgSp::Value::IDENTITY_TYPE_MOBILE;
#endif
                        else
                            val = MsgSp::Value::IDENTITY_TYPE_ISSI;
                        msgSp->addField(MsgSp::Field::CALLING_PARTY_TYPE, val)
                              .addField(MsgSp::Field::COMM_TYPE,
                                        MsgSp::Value::COMM_TYPE_POINT_TO_POINT);
                    }
                    msgSp->addField(MsgSp::Field::VOIP_AUDIO_RTP_PORT,
                                    getRtpPort(*msg));
                    val = getRtpPort(*msg, true);
                    if (val > 0)
                        msgSp->addField(MsgSp::Field::VOIP_VIDEO_RTP_PORT, val);
                    str = getCryptoKey(*msg);
                    if (!str.empty())
                        msgSp->addField(MsgSp::Field::VOIP_AUDIO_RTP_KEY, str);
                    str = getCryptoKey(*msg, true);
                    if (!str.empty())
                        msgSp->addField(MsgSp::Field::VOIP_VIDEO_RTP_KEY, str);
                    resp = createRespMsg(MsgSip::Type::INVITE,
                                         MsgSip::Value::RESP_PROV_TRYING, 0,
                                         &data);
                    resp->addField(MsgSip::Field::CALL_ID, data.voipCallId);
                    sendMsg(resp);
                    resp = createRespMsg(MsgSip::Type::INVITE,
                                         MsgSip::Value::RESP_PROV_RINGING, 0,
                                         &data);
                    resp->addField(MsgSip::Field::CALL_ID, data.voipCallId)
                         .addField(MsgSip::Field::CONTACT, mContact);
                    PalLock::release(&mCallMapLock);
                    break;
                }
                default:
                {
                    LOGGER_ERROR(mLogger, LOGPREFIX << "processMsg: "
                                 << msg->getName()
                                 << ": Unhandled response code "
                                 << msg->getRespCode());
                    Locker lock(&mCallMapLock);
                    val = getCallPartyInUri(msg->getFieldString(
                                                             MsgSip::Field::TO),
                                            true);
                    if (val > 0)
                    {
                        resp = createReqMsg(MsgSip::Type::ACK,
                                            mSsiCallMap[checkDisp(val)].uri,
                                            msg);
                        mSsiCallMap.erase(val);
                    }
                    break;
                }
            }
            break;
        }
        default:
        {
            LOGGER_DEBUG(mLogger, LOGPREFIX << "processMsg: " << msg->getName()
                         << ": Unhandled");
            break;
        }
    } //switch (msg->getType())
    return resp;
}

int VoipSessionClient::getNewSessionId()
{
    if (++mSessionId <= 0)
        mSessionId = 1;
    return mSessionId;
}

inline int VoipSessionClient::getLocalRtpPort()
{
    mLocalRtpPort += 2; //must be an even number
    if (mLocalRtpPort > RTP_PORT_MAX)
        mLocalRtpPort = RTP_PORT_MIN;
    return mLocalRtpPort;
}

string VoipSessionClient::getLocalCryptoKey()
{
    unsigned char key[44]; //key (32 bytes) + IV (12 bytes)
    if (RAND_bytes(key, sizeof(key)) != 1) //generate a random key
        return DEFAULT_KEY;
    return base64Encode(key, sizeof(key));
}

string VoipSessionClient::base64Encode(const unsigned char *buf, int len)
{
    int i = 0;
    int j = 0;
    unsigned char src[BASE64_CHUNK];
    unsigned char enc[BASE64_ENC_CHUNK];
    ostringstream os;
    while (len-- > 0)
    {
        src[i++] = *(buf++);
        if (i == BASE64_CHUNK)
        {
            enc[0] = (src[0] & 0xFC) >> 2;
            enc[1] = ((src[0] & 0x03) << 4) + ((src[1] & 0xF0) >> 4);
            enc[2] = ((src[1] & 0x0F) << 2) + ((src[2] & 0xC0) >> 6);
            enc[3] = src[2] & 0x3F;
            for (i=0; i<BASE64_ENC_CHUNK; ++i)
            {
                os << BASE64_CHARS[enc[i]];
            }
            i = 0;
        }
    }
    if (i > 0)
    {
        for (j=i; j<BASE64_CHUNK; ++j)
        {
            src[j] = '\0';
        }
        enc[0] = (src[0] & 0xFC) >> 2;
        enc[1] = ((src[0] & 0x03) << 4) + ((src[1] & 0xF0) >> 4);
        enc[2] = ((src[1] & 0x0F) << 2) + ((src[2] & 0xC0) >> 6);
        enc[3] = src[2] & 0x3F;
        for (j=0; j<i + 1; ++j)
        {
            os << BASE64_CHARS[enc[j]];
        }
        while (i++ < BASE64_CHUNK)
        {
            os << '=';
        }
    }
    return os.str();
}
