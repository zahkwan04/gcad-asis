/**
 * Base class for a VOIP session.
 *
 * Copyright (C) Sapura Secured Technologies, 2013-2024. All Rights Reserved.
 *
 * @file
 * @version $Id: VoipSessionBase.h 1875 2024-09-05 03:44:40Z hazim.rujhan $
 * @author Ahmad Syukri
 */
#ifndef VOIPSESSIONBASE_H
#define VOIPSESSIONBASE_H

#include <queue>
#include <string>

#include "Logger.h"
#include "MsgSip.h"
#include "MsgSp.h"
#include "PalLock.h"
#include "PalSem.h"
#include "PalThread.h"
#ifdef SIPTCP
#include "TcpSocket.h"
#else
#include "UdpSocket.h"
#endif

class VoipSessionBase
{
public:
#ifdef MOBILE
    //callback signature for event notification
    typedef void (*RecvCallbackFn)(MsgSp *msg);
    typedef void (*StrCallbackFn)(const std::string &str);
#else
    typedef void (*RecvCallbackFn)(void *obj, MsgSp *msg);
#endif

    enum eState
    {
        STATE_INVALID,
        STATE_STARTED,
#ifdef SIPTCP
        STATE_DISCONNECTED,
        STATE_CONNECTED,
#endif
        STATE_REGISTERED,
        STATE_STOPPED
    };

    //media payload type
    static const int PAYLOAD_AUDIO_ACELP = 125;
    static const int PAYLOAD_AUDIO_PCMA  = 8;
    static const int PAYLOAD_VIDEO_H264  = 96;
    //packet time in milliseconds
    static const int PTIME_ACELP         = 60;
#ifdef SERVERAPP
    static const int PTIME_PCMA          = 20;
#else
    static const int PTIME_PCMA          = 40;
#endif
    //sample rate in Hz
    static const int SAMPLE_AUDIO        = 8000;
    static const int SAMPLE_VIDEO        = 90000;

#ifdef SERVERAPP
    /**
     * Constructor for server.
     *
     * @param[in] userId      User ID for registration.
     * @param[in] password    Password for registration.
     * @param[in] logger      A logger object. Caller retains ownership.
     * @param[in] callbackObj Owner of the callback function.
     * @param[in] callbackFn  The callback function to pass a received
     *                        message.
     */
    VoipSessionBase(const std::string &userId,
                    const std::string &password,
                    Logger            *logger,
                    void              *callbackObj,
                    RecvCallbackFn     callbackFn);

#else //SERVERAPP
    /**
     * Constructor for client.
     *
     * @param[in] userId      User ID for registration.
     * @param[in] logger      A logger object. Caller retains ownership.
     * @param[in] strCbFn     The callback function to pass a log string.
     * @param[in] callbackObj Owner of the callback function.
     * @param[in] callbackFn  The callback function to pass a received
     *                        message.
     */
    VoipSessionBase(int             userId,
                    Logger         *logger,
#ifdef MOBILE
                    StrCallbackFn   strCbFn,
#else
                    void           *callbackObj,
#endif
                    RecvCallbackFn  callbackFn);
#endif //SERVERAPP

    virtual ~VoipSessionBase();

    /**
     * Sets mode to standby or active.
     *
     * @param[in] onStandby true to set to standby.
     */
    void setOnStandby(bool onStandby) { mOnStandby = onStandby; }

    bool isValid() const { return (mState != STATE_INVALID); }

    bool isRegistered() const { return (mState == STATE_REGISTERED); }

#ifndef SERVERAPP
    /**
     * Changes the state from registered to stopped, to prevent deregistration
     * on destruction. This is for the special case of being bumped off by
     * server due to duplicate login - must not deregister because current
     * registration is done by the new login.
     */
    void setStateStopped()
    {
        if (isRegistered())
            mState = STATE_STOPPED;
    }
#endif //!SERVERAPP

    /**
     * Continuously receives and processes server messages.
     */
    void recvThread();

    /**
     * Retrieves SIP messages from the queue and sends them to VOIP server.
     */
    void sendMsgThread();

    /**
     * Converts a SIP response code to MsgSp call disconnect cause.
     *
     * @param[in] code The response code.
     * @return The disconnect cause.
     */
    static int disconnectCauseFromSipResp(int code);

    /**
     * Converts a MsgSp call disconnect cause to SIP response code.
     *
     * @param[in] dc The disconnect cause.
     * @return The response code.
     */
    static int disconnectCauseToSipResp(int dc);

protected:
    static const int  CNONCE_LEN   = 10;   //cnonce random string length
    static const int  MAX_FORWARDS = 70;   //max number of forwarded message
    static const int  TAG_LEN      = 10;   //tag ID random string length
    static const int  BUFFER_SIZE_BYTES = 4092; //for data Rx
#ifdef MOBILE
    //called party number for dispatcher (hunting line) call
    static const int  CALLED_PARTY_DISP = 997;
    static const int  LOCAL_PORT   = 6010; //port for VOIP server comm
#else
    static const int  LOCAL_PORT   = 5060; //port for VOIP server comm
#endif

    static const char NUMPREFIX_AMBIENCE         = '?';
    static const char NUMPREFIX_BROADCAST        = 'B';
    //client-to-client regardless of mobile or dispatcher
    static const char NUMPREFIX_DISPATCHER       = 'D';
    static const char NUMPREFIX_GROUP            = '*';
    static const char NUMPREFIX_GROUP_IN         = 'G';
    static const char NUMPREFIX_GROUP_PREEMPTIVE = '~';
    static const char NUMPREFIX_IND_PTT          = '#';
    static const char NUMPREFIX_LISTEN           = '&';
    //for group call originated by network
    static const char NUMPREFIX_SSIC             = 'J';
    //for group call originated by dispatcher
    static const char NUMPREFIX_SSIC_DISP        = 'K';
    static const char NUMPREFIX_TERMINATE        = '!';

    static const std::string HUPALL; //extension to terminate all calls
    static const std::string PARAM_BRANCH_RFC3261_COOKIE;
    static const std::string PARAM_EXPIRES;
    static const std::string PARAM_EXPIRES_0;
    static const std::string PARAM_RPORT;
    static const std::string PARAM_TAG;
    static const std::string PARAM_TRANSPORT;
    static const std::string QOP_AUTH;
    static const std::string QOP_AUTH_INT;
    static const std::string URI_PREFIX;
    static const std::string VALUE_ALLOW;
    static const std::string VALUE_CONTENT_TYPE_DTMF;
    static const std::string VALUE_CONTENT_TYPE_SDP;
    static const std::string VALUE_SUPPORTED;
    static const std::string VIA_PROTOCOL;

    struct CallData
    {
        CallData() {}
        CallData(bool               isIncoming,
                 const std::string &callId,
                 const std::string &calledUri,
                 const std::string &inVia,
                 const std::string &inFrom,
                 const std::string &inTo,
                 int                inNwkCallId = 0) :
            seqId(0), nwkCallId(inNwkCallId), incoming(isIncoming),
            canceled(false), setupPhase(true), voipCallId(callId),
            uri(calledUri), via(inVia), from(inFrom), to(inTo) {}

        int         invCount   = 1;     //number of INVITEs
        int         seqId      = 0;     //in last INVITE
        int         nwkCallId  = 0;     //network call ID
        int         audRtpPort = 0;
        int         vidRtpPort = 0;
        bool        incoming   = false;
        bool        canceled   = false; //outgoing canceled or incoming declined
        bool        setupPhase = true;
        std::string audRtpKey;
        std::string vidRtpKey;
        std::string voipCallId;
        std::string uri;       //called party URI in first INVITE
        std::string via;       //in first INVITE
        std::string from;      //in first INVITE
        //initially in first INVITE, overwritten by TO in server OK response
        std::string to;
        std::string auth;      //PROXY_AUTHORIZATION in last INVITE
        std::string user2User;
    };
    typedef std::map<int, CallData> IntCallMapT;

    //data for tracking register/unregister response
    struct RegData
    {
        void setReg(bool r)
        {
            reg = r;
            t = time(0);
        }

        bool   reg; //true for register, false for unregister
        time_t t;   //creation time for timeout check
    };
    typedef std::map<int, RegData> RegMapT; //key is cseq

    int           mState;
    int           mServerPort;
    int           mMsgId;          //incrementing message ID
#ifdef MOBILE
    int           mDispIssi;       //dispatcher called party ID
#endif
    unsigned int  mNc;
    bool          mOnStandby;
    std::string   mLocalIp;
    std::string   mServerIp;
    std::string   mUserId;
    std::string   mPassword;
    std::string   mUserUri;
    std::string   mContact;
    std::string   mVia;
    std::string   mTagIdentity;    //tag identity for each session
    std::string   mUserAgent;
#ifdef SIPTCP
    TcpSocket    *mSocket;
#else
    UdpSocket    *mSocket;
#endif
    Logger       *mLogger;

    PalThread::ThreadT    mSendMsgThread;
    PalThread::ThreadT    mRecvThread;
    PalLock::LockT        mSendMsgQueueLock; //guards queue access
    PalLock::LockT        mCallMapLock;      //guards call map access
    PalSem::SemT          mSendMsgCountSem;
    std::queue<MsgSip *>  mSendMsgQueue;
    IntCallMapT           mSsiCallMap;
    RegMapT               mRegMap;
#ifdef MOBILE
    StrCallbackFn         mStrCbFn;
#else
    void                 *mCbObj;            //callback function owner object
#endif
    RecvCallbackFn        mCbFn;             //callback function

    /**
     * Initializes some members.
     *
     * @param[in] localPort The local port for incoming SIP messages.
     * @return true if successful.
     */
    bool init(int localPort = LOCAL_PORT);

    /**
     * Sets SIP field values that use mLocalIp. Must be called from init() and
     * whenever mLocalIp changes.
     */
    void setSipFields();

    /**
     * Connects and logs in to VOIP server.
     *
     * @return true if successful.
     */
    bool start();

    /**
     * Gets next message ID.
     *
     * @return New message ID.
     */
    int getNewMsgId();

    /**
     * Retrieves and sets the local IP address.
     *
     * @return true if successful.
     */
    bool setLocalIp();

    /**
     * Sets the session state, other than STATE_STOPPED.
     *
     * @param[in] state The state. See enum eState.
     */
    void setState(int state);

    /**
     * Sends a SIP message to the server.
     *
     * @param[in] msg The SIP message.
     */
    void sendToServer(MsgSip &msg);

    /**
     * Puts a message in the queue to send to the server. Takes over
     * ownership of the message object from the caller.
     *
     * @param[in] msg The message object.
     */
    void sendMsg(MsgSip *msg);

    /**
     * Creates a REGISTER message, as a request or response.
     *
     * @param[in] svrMsg Server message with authentication challenge if
     *                   creating a response. 0 if creating a request.
     * @return The message object. Caller takes ownership of the created
     *         object, and is responsible for deleting it.
     */
    MsgSip *createRegisterMsg(const MsgSip *svrMsg);

    /**
     * Creates a REGISTER message for the purpose of unregistering from the
     * VOIP server, as a request or response.
     *
     * @param[in] userId The user ID.
     * @param[in] svrMsg Server message with authentication challenge if
     *                   creating a response. 0 if creating a request.
     * @return The message object. Caller takes ownership of the created
     *         object, and is responsible for deleting it.
     */
    MsgSip *createUnregisterMsg(int userId, const MsgSip *svrMsg);

    /**
     * Creates a request message.
     *
     * @param[in] type       The message type.
     * @param[in] requestUri The request URI.
     * @param[in] svrMsg     VOIP server message that triggers this request,
     *                       if any.
     * @param[in] data       The call data, if any.
     * @return The message object. Caller takes ownership of the created
     *         object, and is responsible for deleting it.
     */
    MsgSip *createReqMsg(int                type,
                         const std::string &requestUri,
                         const MsgSip      *svrMsg = 0,
                         const CallData    *data = 0);

    /**
     * Creates a response message.
     *
     * @param[in] type   The message type.
     * @param[in] status The response status code.
     * @param[in] svrMsg VOIP server message that triggers this response, if
     *                   any.
     * @param[in] data   The call data, if any.
     * @return The message object. Caller takes ownership of the created
     *         object, and is responsible for deleting it.
     */
    MsgSip *createRespMsg(int             type,
                          int             status,
                          const MsgSip   *svrMsg = 0,
                          const CallData *data = 0);

    /**
     * Creates an ACK message in error cases.
     *
     * @param[in] svrMsg VOIP server message that triggers this.
     * @return The message object, or 0 on failure. Caller takes ownership
     *         of the created object, and is responsible for deleting it.
     */
    MsgSip *createErrorAckMsg(const MsgSip &svrMsg);

    /**
     * Adds authentication credentials to a response message.
     *
     * @param[in]     msg    Server message with authentication challenge.
     * @param[in,out] resp   The response message.
     * @param[in]     reqUri Request URI for INVITE. Omit for REGISTER.
     */
    void addAuthParam(const MsgSip      &msg,
                      MsgSip            &resp,
                      const std::string &reqUri = "");

    /**
     * Generates SDP parameters for a call.
     *
     * @param[in] sessionId The session ID, which could be the call ID.
     * @param[in] ip        On server: The RTP remote IP.
     *                      On client: The local IP.
     * @param[in] doAcelp   true for ACELP codec, otherwise PCMA.
     * @param[in] audPort   The audio RTP port.
     * @param[in] vidPort   The video RTP port. 0 if not a video call.
     * @param[in] audKey    Audio AEAD_AES_256_GCM_8 crypto key. Empty if no
     *                      encryption.
     * @param[in] vidKey    Video AEAD_AES_256_GCM_8 crypto key. Empty if no
     *                      encryption.
     * @return The SDP message for RTP.
     */
    std::string generateSdp(int                sessionId,
                            const std::string &ip,
                            bool               doAcelp,
                            int                audPort,
                            int                vidPort = 0,
                            const std::string &audKey = "",
                            const std::string &vidKey = "");

    /**
     * Gets the call party number from the URI part of a SIP FROM/TO line.
     * For example, in:
     *     From: "name" <sip:1234@1.2.3.4>
     *     To: "name" <sip:1234@1.2.3.4>  (individual call)
     *     To: "name" <sip:*1234@1.2.3.4> (group call)
     * the call party is 1234.
     *
     * @param[in]  sipLine   The SIP line.
     * @param[in]  checkCall true to check whether a call exists for the call
     *                       party. Caller must hold mCallMapLock.
     * @param[out] type      The call party type, if needed.
     *                       MsgSp::Value::IDENTITY_TYPE_GSSI or
     *                       IDENTITY_TYPE_ISSI.
     * @param[out] numPrefix The number prefix, if needed.
     * @return The call party number. -1 on error or 0 if no call exists.
     */
    int getCallPartyInUri(const std::string &sipLine,
                          bool               checkCall,
                          int               *type      = 0,
                          char              *numPrefix = 0);

    /**
     * Gets the call party number from the Name part of a SIP FROM/TO line.
     * For example, in:
     *     From: "1234" <sip:1007@1.2.3.4>  (individual call)
     *     From: "*1234" <sip:1007@1.2.3.4> (group call)
     *     From: "!1234" <sip:1007@1.2.3.4> (call termination)
     *     To: "1234" <sip:1007@1.2.3.4>
     * the call party is 1234.
     *
     * @param[in]  sipLine   The SIP line.
     * @param[in]  checkCall true to check whether a call exists for the call
     *                       party. Caller must hold mCallMapLock.
     * @param[out] type      The call party type, if needed.
     *                       MsgSp::Value::IDENTITY_TYPE_GSSI or
     *                       IDENTITY_TYPE_ISSI.
     * @param[out] numPrefix The number prefix, if needed.
     * @return The call party number. -1 on error or 0 if no call exists.
     */
    int getCallPartyInName(const std::string &sipLine,
                           bool               checkCall,
                           int               *type      = 0,
                           char              *numPrefix = 0);

    /**
     * Extracts the Request URI from a SIP FROM/TO line.
     * For example, in:
     *     From: "name" <sip:1234@1.2.3.4>;tag=e4F2y8D6v0r1m
     *     To: "name" <sip:1234@1.2.3.4>
     * the Request URI is sip:1234@1.2.3.4.
     *
     * @param[in,out] sipLine The SIP line on input, Request URI on output.
     *                        May be modified even on failure.
     * @return true if successful.
     */
    bool getReqUri(std::string &sipLine);

    /**
     * Gets the RTP port from a SIP SDP content.
     * For example, in:
     *     v=0
     *     o=FreeSWITCH 1386271556 1386271557 IN IP4 172.20.200.100
     *     s=FreeSWITCH
     *     c=IN IP4 172.20.200.100
     *     t=0 0
     *     m=audio 21232 RTP/AVP 8
     *     a=rtpmap:8 PCMA/8000
     *     a=ptime:20
     *     m=video 20004 RTP/AVP 96
     *     a=rtpmap:96 H264/90000
     * the audio/video RTP port is 21232/20004 respectively.
     *
     * @param[in] msg     The SIP message.
     * @param[in] isVideo true to get video port. Otherwise audio port.
     * @return The RTP port. 0 if not found.
     */
    int getRtpPort(const MsgSip &msg, bool isVideo = false);

    /**
     * Gets the SRTP crypto key from a SIP SDP content.
     * For example, in:
     *     m=audio 20014 RTP/SAVP 8
     *     a=rtpmap:8 PCMA/8000
     *     a=ptime:40
     *     a=crypto:1 AEAD_AES_256_GCM_8 inline:xxxx
     *     m=video 20016 RTP/SAVP 96
     *     a=rtpmap:96 H264/90000
     *     a=crypto:1 AEAD_AES_256_GCM_8 inline:yyyy
     * the audio/video crypto key is xxxx/yyyy respectively.
     *
     * @param[in] msg     The SIP message.
     * @param[in] isVideo true to get video key. Otherwise audio key.
     * @return The crypto key. Empty string if not found.
     */
    std::string getCryptoKey(const MsgSip &msg, bool isVideo = false);

#ifdef SIPTCP
    /**
     * Tries to create a socket connection to the server until succesful or
     * stopped. If successful, sends a registration message.
     *
     * @return true if successful.
     */
    bool connectToServer();

#else
    /**
     * Creates a UDP socket on a local port. If the port is unavailable, keeps
     * incrementing it until successful, with up to 999 retries.
     *
     * @param[in,out] localPort Input:  The first port to try.
     *                          Output: The actual used port.
     * @return The socket, or 0 if unsuccessful (not likely to occur).
     */
    UdpSocket *getUdpSocket(int &localPort);
#endif

    /**
     * Convenience function to check whether an ID is equal to the special
     * dispatcher ID for outgoing call.
     * If so, changes it to the dispatcher ISSI.
     *
     * @param[in,out] id The ID.
     * @return The ID itself.
     */
    int checkDisp(int &id);

    /**
     * Checks and validates the configuration settings.
     *
     * @return true if all required configuration settings are valid.
     */
    virtual bool checkConfig() { return false; }

    /**
     * Processes a VOIP server message.
     *
     * @return The response message, or 0 if none. Caller takes ownership
     *         and is responsible for deleting it.
     */
    virtual MsgSip *processMsg(const MsgSip *, MsgSp *&) { return 0; }
};
#endif //VOIPSESSIONBASE_H
