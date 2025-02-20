/**
 * A class that provides communication session with the server.
 * Can be used as either singleton (mainly for test client) or normal instance.
 * Either usage must start with init().
 * For singleton:
 *   - must continue with setParams(),
 *   - get the object with instance(),
 *   - call destroy() at application exit.
 *
 * Copyright (C) Sapura Secured Technologies, 2013-2024. All Rights Reserved.
 *
 * @file
 * @version $Id: ServerSession.h 1878 2024-09-11 03:28:44Z zulzaidi $
 * @author Zahari Hadzir
 * @author Mohd Rozaimi
 */
#ifndef SERVERSESSION_H
#define SERVERSESSION_H

#include <set>
#include <string>
#include <vector>
#include <time.h>   //time_t, time()

#include "Logger.h"
#include "MsgSp.h"
#include "PalLock.h"
#include "PalThread.h"
#include "TcpSocket.h"
#include "VoipSessionClient.h"

class ServerSession
{
public:
    //callback signature to receive a server message
    typedef void (*RecvCallbackFn)(void *obj, MsgSp *msg);

    typedef std::vector<int> IntListT;
    typedef std::set<int>    SsiSetT;

    static const int BROADCAST_SSI = 16777215;
    //VOIPSESSION_ERROR should have any negative value below -12000 so as not
    //to overlap with socket error codes
    static const int VOIPSESSION_ERROR = -50000;

    enum eState
    {
        STATE_INVALID,
        STATE_DISCONNECTED,
        STATE_CONNECTED,
        STATE_LOGIN,
        STATE_STOPPED
    };

    enum eServerIdx
    {
        SERVER_IDX_NONE = -1,
        SERVER_IDX_MAIN,
        SERVER_IDX_REDUNDANT
    };

    /**
     * Constructor.
     *
     * @param[in] username    Username for server login.
     * @param[in] password    Password for server login.
     * @param[in] branches    Selected branches. See mBranches for format.
     * @param[in] callbackObj The callback object.
     * @param[in] callbackFn  The callback function to pass a received
     *                        message.
     */
    ServerSession(const std::string &username,
                  const std::string &password,
                  const std::string &branches,
                  void              *callbackObj,
                  RecvCallbackFn     callbackFn
#ifdef TESTCLIENT
                  , bool             doSubsData
#endif
                  );

    /**
     * Deregisters from server, terminates the message thread and destroys
     * the socket object.
     */
    ~ServerSession();

    friend std::ostream &operator<<(std::ostream        &os,
                                    const ServerSession &obj);

    /**
     * Sets the user-selected fleet branches if specified.
     * If branches specified or selection exists (non-empty), informs server,
     * and if branches specified, requests subscriber data update.
     * This is to be called with parameter when user makes a selection, and
     * without parameter upon login before requesting data.
     *
     * @param[in] branches The branches, if any. See mBranches for format.
     */
    void setBranches(const std::string *branches = 0);

    /**
     * Starts monitoring an SSI.
     *
     * @param[in] ssi     The SSI to monitor.
     * @param[in] isGroup true for GSSI.
     * @return true if successful.
     */
    bool monitorStart(int ssi, bool isGroup);

    /**
     * Starts monitoring some SSIs.
     *
     * @param[in] ssiSet  The SSIs (ISSIs or GSSIs) to monitor.
     * @param[in] isGroup true for GSSI.
     * @return true if successful.
     */
    bool monitorStart(const SsiSetT &ssiSet, bool isGroup);

    /**
     * Stops monitoring an SSI.
     *
     * @param[in] ssi     The SSI to stop monitoring.
     * @param[in] isGroup true for GSSI.
     * @return true if successful.
     */
    bool monitorStop(int ssi, bool isGroup);

    /**
     * Stops monitoring some SSIs.
     *
     * @param[in] ssiSet  The SSIs to stop monitoring.
     * @param[in] isGroup true for GSSI.
     * @return true if successful.
     */
    bool monitorStop(const SsiSetT &ssiSet, bool isGroup);

    /**
     * Stops monitoring all SSIs.
     *
     * @return true if successful.
     */
    bool monitorStop();

    /**
     * Starts GPS monitoring of some or all ISSIs.
     *
     * @param[in] ssiSet The ISSIs, or empty set to monitor all.
     * @return true if successful.
     */
    bool gpsMonitorStart(const SsiSetT &ssiSet);

    /**
     * Stops GPS monitoring of some or all ISSIs.
     *
     * @param[in] ssiSet The ISSIs, or empty set to stop all.
     * @return true if successful.
     */
    bool gpsMonitorStop(const SsiSetT &ssiSet);

    /**
     * Requests Status data from Server.
     *
     * @return true if successful.
     */
    bool requestStatusData();

    /**
     * Requests Subscriber data from Server.
     *
     * @return true if successful.
     */
    bool requestSubsData();

    /**
     * Requests password change.
     *
     * @param[in] oldPasswd The old password.
     * @param[in] newPasswd The new password.
     * @return true if request sent (but not necessarily successful yet).
     */
    bool changePasswd(const std::string &oldPasswd,
                      const std::string &newPasswd);

    /**
     * Sends an SDS message.
     *
     * @param[in] ssiType  The destination SSI type.
     *                     See MsgSp::Value::IDENTITY_TYPE_*.
     * @param[in] ssi      The destination SSI.
     * @param[in] msgStr   The message string.
     * @param[in] coding   The text coding scheme.
     *                     See MsgSp::Value::TEXT_CODING_*.
     * @return The message ID. See sendMsg().
     */
    int sds(int                ssiType,
            int                ssi,
            const std::string &msgStr,
            int                coding = MsgSp::Value::TEXT_CODING_8859_1_LATIN1);

    /**
     * Sends an SDS message to multiple recipients.
     *
     * @param[in] ssiTypes The destination SSI types.
     *                     See MsgSp::Value::IDENTITY_TYPE_*.
     * @param[in] ssis     The destination SSIs.
     * @param[in] msgStr   The message string.
     * @param[in] coding   The text coding scheme.
     *                     See MsgSp::Value::TEXT_CODING_*.
     * @return The message ID. See sendMsg().
     */
    int sds(IntListT          &ssiTypes,
            IntListT          &ssis,
            const std::string &msgStr,
            int                coding = MsgSp::Value::TEXT_CODING_8859_1_LATIN1);

#ifdef TESTCLIENT
    /**
     * Sends an SDS message with NMEA-coded Location data to an ISSI.
     *
     * @param[in] issi     The destination ISSI.
     * @param[in] msgStr   The NMEA-coded Location data string.
     * @return The message ID. See sendMsg().
     */
    int sdsGps(int issi, const std::string &msgStr);
#endif //TESTCLIENT

    /**
     * Sends a pre-coded status message.
     *
     * @param[in] ssiType   The destination SSI type.
     *                      See MsgSp::Value::IDENTITY_TYPE_*.
     * @param[in] ssi       The destination SSI.
     * @param[in] statusVal The pre-coded status value.
     * @return The message ID. See sendMsg().
     */
    int status(int ssiType, int ssi, int statusVal);

    /**
     * Sends a pre-coded status message to multiple recipients.
     *
     * @param[in] ssiTypes  The destination SSI types.
     *                      See MsgSp::Value::IDENTITY_TYPE_*.
     * @param[in] ssis      The destination SSIs.
     * @param[in] statusVal The pre-coded status value.
     * @return The message ID. See sendMsg().
     */
    int status(IntListT &ssiTypes, IntListT &ssis, int statusVal);

    std::string getMobDomain() { return mMobIp; };

    /**
     * Makes a group call.
     *
     * @param[in]  ssi      The called SSI.
     * @param[out] voipId   The VOIP call ID.
     * @param[in]  priority The call priority.
     *                      See MsgSp::Value::CALL_PRIORITY_*.
     * @return The message ID. See sendMsg().
     */
    int callSetupGrp(int          ssi,
                     std::string &voipId,
                     int          priority =
                                         MsgSp::Value::CALL_PRIORITY_LOWEST);

    /**
     * Makes an individual call.
     *
     * @param[in]  ssi      The called SSI.
     * @param[in]  isDuplex true for full duplex.
     * @param[in]  isHook   true for hook signalling.
     * @param[out] voipId   The VOIP call ID.
     * @param[in]  priority The call priority.
     *                      See MsgSp::Value::CALL_PRIORITY_*.
     * @return The message ID. See sendMsg().
     */
    int callSetupInd(int          ssi,
                     bool         isDuplex,
                     bool         isHook,
                     std::string &voipId,
                     int          priority =
                                         MsgSp::Value::CALL_PRIORITY_LOWEST);

    /**
     * Makes a call to another dispatcher.
     *
     * @param[in]  id           The dispatcher ID.
     * @param[out] voipId       The VOIP call ID.
     * @param[in]  calledDomain The VOIP called party domain. Required if
     *                          different from the server IP.
     * @return Positive value if successful.
     */
    int callSetupInd(int                id,
                     std::string       &voipId,
                     const std::string &calledDomain = "");

    /**
     * Makes a video call to another dispatcher.
     *
     * @param[in]  id           The dispatcher ID.
     * @param[out] voipId       The VOIP call ID.
     * @param[in]  calledDomain The VOIP called party domain. Required if
     *                          different from the server IP.
     * @return Positive value if successful.
     */
    int callSetupVideo(int                id,
                       std::string       &voipId,
                       const std::string &calledDomain = "");

    /**
     * Makes an ambience listening call.
     *
     * @param[in]  ssi      The called SSI.
     * @param[out] voipId   The VOIP call ID.
     * @param[in]  priority The call priority.
     *                      See MsgSp::Value::CALL_PRIORITY_*.
     * @return The message ID. See sendMsg().
     */
    int callSetupAmbience(int          ssi,
                          std::string &voipId,
                          int          priority =
                                         MsgSp::Value::CALL_PRIORITY_LOWEST);

    /**
     * Makes a broadcast call.
     *
     * @param[out] voipId The VOIP call ID.
     * @return The message ID. See sendMsg().
     */
    int callSetupBroadcast(std::string &voipId);

    /**
     * Connects a call.
     *
     * @param[in]  callId The call ID.
     * @param[in]  ssi    The calling SSI.
     * @param[out] audKey Crypto key for audio stream.
     * @return The audio RTP local port number, or 0 on failure.
     */
    int callConnect(int callId, int ssi, std::string *audKey);

    /**
     * Connects a call from another dispatcher.
     *
     * @param[in]  id      The dispatcher ID.
     * @param[out] audKey  Crypto key for audio stream.
     * @param[out] vidPort Video RTP local port number.
     * @param[out] vidKey  Crypto key for video stream.
     * @return The audio RTP local port number, or 0 on failure.
     */
    int callConnect(int          id,
                    std::string *audKey = 0,
                    int         *vidPort = 0,
                    std::string *vidKey = 0);

    /**
     * Disconnects a call.
     *
     * @param[in] callId The call ID.
     * @param[in] ssi    The called/calling SSI.
     * @return The message ID. See sendMsg().
     */
    int callDisconnect(int callId, int ssi);

    /**
     * Disconnects a call to/from another dispatcher.
     *
     * @param[in] id The dispatcher ID.
     * @return Positive value if successful.
     */
    int callDisconnect(int id);

    /**
     * Disconnects a broadcast call.
     *
     * @param[in] callId The call ID.
     * @return The message ID. See sendMsg().
     */
    int callDisconnectBroadcast(int callId);

    /**
     * Demands PTT transmission.
     *
     * @param[in] callId   The call ID.
     * @param[in] priority The call priority.
     *                     See MsgSp::Value::TX_DEMAND_PRIORITY_*.
     * @return The message ID. See sendMsg().
     */
    int callTxDemand(int callId,
                     int priority = MsgSp::Value::TX_DEMAND_PRIORITY_LOW);

    /**
     * Ceases PTT transmission.
     *
     * @param[in] callId The call ID.
     * @return The message ID. See sendMsg().
     */
    int callTxCeased(int callId);

    /**
     * Terminates a call.
     *
     * @param[in] callId The call ID.
     * @return The message ID. See sendMsg().
     */
    int callTerminate(int callId);

    /**
     * Requests a call inclusion.
     *
     * @param[in]  callId       The call ID.
     * @param[in]  gssi         The called GSSI.
     * @param[in]  callingParty The calling party.
     * @param[in]  isDisp       true if calling party is a dispatcher.
     * @param[out] voipId       The VOIP call ID.
     * @return The message ID. See sendMsg().
     */
    int ssicInvoke(int          callId,
                   int          gssi,
                   int          callingParty,
                   bool         isDisp,
                   std::string &voipId);

    /**
     * Ends a call inclusion.
     *
     * @param[in] callId The call ID.
     * @param[in] gssi   The called GSSI.
     * @return The message ID. See sendMsg().
     */
    int ssicDisconnect(int callId, int gssi);

    /**
     * Requests to listen to an individual call.
     *
     * @param[in]  callId The call ID.
     * @param[in]  ssi    The SSI to listen to.
     * @param[out] voipId The VOIP call ID.
     * @return The message ID. See sendMsg().
     */
    int listenConnect(int callId, int ssi, std::string &voipId);

    /**
     * Ends an individual call listening.
     *
     * @param[in] callId The call ID.
     * @param[in] ssi    The SSI listened to.
     * @return The message ID. See sendMsg().
     */
    int listenDisconnect(int callId, int ssi);

    /**
     * Ends an individual call listening on VOIP only.
     *
     * @param[in] ssi The SSI listened to.
     */
    void listenDisconnect(int ssi);

    /**
     * Performs DGNA assignment/deassignment.
     *
     * @param[in] gssi     The DGNA GSSI.
     * @param[in] ssi      The affected ISSI or GSSI.
     * @param[in] isIndiv  true if ssi is ISSI, false for GSSI.
     * @param[in] doAssign true to assign, false to deassign.
     * @return The message ID. See sendMsg().
     */
    int ssDgnaAssign(int gssi, int ssi, bool isIndiv, bool doAssign);

    /**
     * Notifies server of an incident update.
     *
     * @param[in] id       The incident ID.
     * @param[in] editData The edited data, or empty string for a new
     *                     incident.
     * @return The message ID. See sendMsg().
     */
    int incidentUpdate(int id, const std::string &editData = "");

    /**
     * Requests or releases lock for incident editing.
     *
     * @param[in] id     The incident ID.
     * @param[in] doLock true to request lock.
     * @return The message ID. See sendMsg().
     */
    int incidentLock(int id, bool doLock);

    /**
     * Notifies server of a POI update.
     *
     * @param[in] id       The POI ID.
     * @param[in] doDelete true to delete the POI.
     * @return The message ID. See sendMsg().
     */
    int poiUpdate(int id, bool doDelete);

    /**
     * Adds a unique message ID to a message and sends it to the server.
     *
     * @param[in] msg       The message object.
     * @param[in] deleteMsg true to delete the message after sending it.
     * @return The positive message ID if successful.
     */
    int sendMsg(MsgSp *msg, bool deleteMsg = true);

    /**
     * Gets the server address - either just the IP or as "IP:port".
     *
     * @param[in] withPort true to include the port.
     * @return The address if logged in, otherwise empty string.
     */
    std::string getServerAddress(bool withPort = true) const;

    int getState() const { return mState; }

    std::string getUsername() const { return mUsername; }

    bool isValid() const { return (mState != STATE_INVALID); }

    bool isLoggedIn() const { return (mState == STATE_LOGIN); }

    bool isVoipRegistered() const
    {
        return (mVoipSession != 0 && mVoipSession->isRegistered());
    }

    /**
     * Enables or disables message encryption.
     *
     * @param[in] enable true to enable message encryption.
     */
    void setEncryption(bool enable);

    /**
     * Continuously receives and processes server messages.
     */
    void recvThread();

    /**
     * Sets the logger and server parameters. Must be done before
     * instantiating a class object.
     *
     * @param[in] logger        The logger object.
     * @param[in] serverIp      The main server IP address.
     * @param[in] serverPort    The main server port number.
     * @param[in] serverIpRed   The redundant server IP address, if any.
     * @param[in] serverPortRed The redundant server port number, if any.
     * @return true if all parameters are valid.
     */
    static bool init(Logger            *logger,
                     const std::string &serverIp,
                     int                serverPort,
                     const std::string &serverIpRed = "",
                     int                serverPortRed = 0);

    /**
     * Sets the logger and server parameters. Must be done before
     * instantiating a class object.
     *
     * @param[in] logger     The logger object.
     * @param[in] ips        The list of server IP addresses.
     * @param[in] serverPort The list of server port numbers.
     * @return true if all parameters are valid.
     */
    static bool init(Logger                         *logger,
                     const std::vector<std::string> &ips,
                     const std::vector<int>         &ports);

    /**
     * Sets the MAC addresses.
     *
     * @param[in] macs The space-separated MAC addresses.
     */
    static void setMacAddresses(const std::string &macs)
    {
        sMacAddresses = macs;
    }

    static void setVersion(const std::string &version)
    {
        sVersion = version;
    }

    /**
     * Sets the singleton parameters. Must be done before calling instance().
     *
     * @param[in] username    Username for server login.
     * @param[in] password    Password for server login.
     * @param[in] callbackObj The callback object.
     * @param[in] callbackFn  The callback function to pass a received
     *                        message.
     * @return true if all parameters are valid.
     */
    static bool setParams(const std::string &username,
                          const std::string &password,
                          void              *callbackObj,
                          RecvCallbackFn     callbackFn);

    /**
     * Gets the singleton instance.
     * init() and setParams() must have been called first.
     *
     * @return The instance.
     */
    static ServerSession &instance();

    /**
     * Deletes the singleton instance.
     */
    static void destroy();

private:
    int                mState;
    int                mMessageId;    //incremented in every sent message
#ifdef TESTCLIENT
    bool               mDoSubsData;
#endif
    //time of last message received from server, for watchdog
    time_t             mRecvTime;
    //time of last message sent to server, for KeepAlive
    time_t             mSentTime;
    std::string        mName;             //for logging
    std::string        mIpAndPort;
    //depending on network setup, the IP seen by server on connection
    //may be different from the local IP captured here - this IP, to be
    //received from server, is mainly for VOIP
    std::string        mIpFromServer;
    std::string        mLogPrefix;
    std::string        mUsername;
    std::string        mPassword;
    std::string        mOldPassword;
    std::string        mNewPassword;
    std::string        mMsgKey;           //for encryption
    std::string        mVoipSvrIp;        //normally svr IP, but not in STM-nwk
    std::string        mMobIp;            //for video call to mobile on STM svr
    //user-selected fleet branches:
    //-comma-separated branch IDs: selection from allowed IDs
    //-empty:                      no selection - follow server assignment
    //-starts with '-':            select all
    std::string        mBranches;
    PalThread::ThreadT mRecvThread;       //receive thread ID
    PalLock::LockT     mSendMsgLock;      //guards sendMsg() call

    VoipSessionClient *mVoipSession;
    TcpSocket         *mSocket;
    void              *mCbObj;        //callback function owner object
    RecvCallbackFn     mCbFn;         //callback function for received messages

    static int                       sServerIdx;    //current server
    static std::string               sMacAddresses; //space-separated
    static std::string               sVersion;      //client version
    static std::vector<int>          sServerPorts;
    static std::vector<std::string>  sServerIps;
    static Logger                   *sLogger;

    //for singleton use
    static std::string     sUsername;
    static std::string     sPassword;
    static PalLock::LockT  sSingletonLock;
    static ServerSession  *sInstance;
    static void           *sCbObj;
    static RecvCallbackFn  sCbFn;

    /**
     * Constructor for singleton.
     */
    ServerSession();

    /**
     * Establishes network connection and login to server.
     * Starts the message receive thread.
     */
    void start();

    /**
     * If VOIP Session is not running, creates and starts it.
     *
     * @return 0 if successful or already running,
     *         VOIPSESSION_ERROR otherwise.
     */
    int checkVoipSession();

    /**
     * Sets the instance name for logging, showing the local address and
     * username.
     */
    void setName();

    /**
     * Sets the state, only if current state is not STATE_STOPPED.
     *
     * @param[in] state The new state. See eState.
     */
    void setState(int state);

    /**
     * Tries to create a socket connection to the server, alternating between
     * main and redundant servers, until succesful or stopped.
     * If successful, sends a login message.
     *
     * @return true if successful.
     */
    bool connectToServer();

    /**
     * Sends a monitoring start/stop message to the server, for either
     * multiple SSIs or a single SSI.
     *
     * @param[in] msgType MsgSp::Type::MON_START or MON_STOP.
     * @param[in] isGroup true for group targets.
     * @param[in] ssiSet  The target SSIs if for multiple.
     * @param[in] ssi     The target SSI if for single. Used only if ssiSet
     *                    is 0.
     */
    void sendMon(int            msgType,
                 bool           isGroup,
                 const SsiSetT *ssiSet,
                 int            ssi = 0);
};
#endif //SERVERSESSION_H
