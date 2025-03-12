/**
 * The CallWindow UI module.
 *
 * Copyright (C) Sapura Secured Technologies, 2014-2025. All Rights Reserved.
 *
 * @file
 * @version $Id: CallWindow.h 1900 2025-01-23 08:38:48Z zulzaidi $
 * @author Mazdiana Makmor
 */
#ifndef CALLWINDOW_H
#define CALLWINDOW_H

#include <time.h>       //time()
#include <QDialog>
#include <QKeyEvent>
#include <QList>
#include <QSound>
#include <QTime>
#include <QTimer>

#include "AudioManager.h"
#include "MessageDialog.h"
#include "ServerSession.h"
#include "VideoStream.h"

namespace Ui {
class CallWindow;
}

class CallWindow : public QDialog
{
    Q_OBJECT

public:
    static const QString STYLE_BGCOLOR_PENDING;
    static const QString STYLE_BGCOLOR_ACTIVE;
    static const QString STYLE_BGCOLOR_ACTIVE_PREEMPTIVE;
    static const QString STYLE_BGCOLOR_ACTIVE_EMERGENCY;
    static const QString STYLE_CALL_DETAILS_FRAME;

    /**
     * Constructor.
     *
     * @param[in] type         Call type - CmnTypes::eCallType.
     * @param[in] callId       Call ID.
     * @param[in] callingParty Calling party ID. 0 for outgoing call.
     * @param[in] callingType  ResourceData::TYPE_SUBSCRIBER or TYPE_DISPATCHER.
     *                         Used only if callingParty != 0.
     * @param[in] calledParty  Called party ID.
     * @param[in] calledType   Called party type.
     * @param[in] isHook       true for hook call. For monitored calls only.
     * @param[in] isE2ee       true for E2EE call.
     * @param[in] calledDomain Called party VOIP domain. Empty string to use the
     *                         server IP.
     * @param[in] parent       Parent widget, if any.
     */
    explicit CallWindow(int                type,
                        int                callId,
                        int                callingParty,
                        int                callingType,
                        int                calledParty,
                        int                calledType,
                        bool               isHook,
                        bool               isE2ee,
                        const std::string &calledDomain,
                        QWidget           *parent = 0);

#ifdef NOSIPSETUP
    /**
     * Constructor for incoming individual call from network.
     *
     * @param[in] callId       Call ID.
     * @param[in] callingParty Calling party ID.
     * @param[in] priority     Call priority.
     * @param[in] isHook       true for hook call.
     * @param[in] isDuplex     true for duplex call.
     * @param[in] isTxGranted  true if have Tx-Granted.
     * @param[in] parent       Parent widget, if any.
     */
    explicit CallWindow(int      callId,
                        int      callingParty,
                        int      priority,
                        bool     isHook,
                        bool     isDuplex,
                        bool     isTxGranted,
                        QWidget *parent = 0);

    /**
     * Constructor for incoming individual call from VOIP server.
     *
     * @param[in] callingParty     Calling party ID.
     * @param[in] callingPartyType Calling party type - CmnTypes::eIdType.
     * @param[in] rtpPort          The remote RTP port.
     * @param[in] parent           Parent widget, if any.
     */
    explicit CallWindow(int      callingParty,
                        int      callingPartyType,
                        int      rtpPort,
                        QWidget *parent = 0);

#else
    /**
     * Constructor for incoming individual call.
     * Must be followed by a call to callSetup().
     *
     * @param[in] callingParty     Calling party ID.
     * @param[in] callingPartyType Calling party type - CmnTypes::eIdType.
     * @param[in] isVideo          true if a video call.
     * @param[in] parent           Parent widget, if any.
     */
    explicit CallWindow(int      callingParty,
                        int      callingPartyType,
                        bool     isVideo,
                        QWidget *parent = 0);
#endif //NOSIPSETUP

    ~CallWindow();

    /**
     * If this window is already visible, just brings it up.
     * Otherwise shows it at a random position around the default, which is at
     * the center of the parent. This is to try to avoid stacking windows which
     * would hide those beneath the top.
     */
    void doShow();

    /**
     * Handles incoming call setup from VOIP, which is expected to come after
     * the first call setup from server.
     * Also handles abnormal case where a CallWindow is already opened for
     * outgoing call and then an incoming call setup from VOIP comes first.
     *
     * @param[in] audPort  Audio remote RTP port.
     * @param[in] vidPort  Video remote RTP port. Non-positive value if not a
     *                     video call.
     * @param[in] priority Call priority. Used only if positive.
     * @param[in] audKey   Audio remote RTP crypto key.
     * @param[in] vidKey   Video remote RTP crypto key.
     * @param[in] callId   VOIP call ID.
     * @param[in] show     true to show this.
     */
    void callSetup(int                audPort,
                   int                vidPort,
                   int                priority,
                   const std::string &audKey,
                   const std::string &vidKey,
                   const std::string &callId,
                   bool               show = true);

#ifdef NOSIPSETUP
    /**
     * Handles incoming call setup from server in the following cases:
     *   -In the unexpected case where the VOIP call setup has come first,
     *   -CallWindow is already opened for outgoing call.
     *
     * @param[in] callId      Call ID.
     * @param[in] priority    Call priority.
     * @param[in] isHook      true for hook call.
     * @param[in] isDuplex    true for duplex call.
     * @param[in] isTxGranted true if have Tx-Granted.
     * @param[in] show        true to show this.
     */
    void callSetup(int  callId,
                   int  priority,
                   bool isHook,
                   bool isDuplex,
                   bool isTxGranted,
                   bool show = true);

#else
    /**
     * Handles incoming VOIP call setup.
     *
     * @param[in] callId      Call ID.
     * @param[in] priority    Call priority.
     * @param[in] audPort     Remote RTP port.
     * @param[in] audKey      Remote RTP crypto key.
     * @param[in] isHook      true for hook call.
     * @param[in] isDuplex    true for duplex call.
     * @param[in] isTxGranted true if have Tx-Granted.
     * @param[in] show        true to show this.
     */
    void callSetup(int                callId,
                   int                priority,
                   int                audPort,
                   const std::string &audKey,
                   bool               isHook,
                   bool               isDuplex,
                   bool               isTxGranted,
                   bool               show = true);
#endif //NOSIPSETUP

    /**
     * Plays ringback tone.
     */
    void callAlert();

    /**
     * Sets this instance for an incoming individual call.
     * It may have been previously created for an outgoing call.
     *
     * @param[in] isHook      true for hook call.
     * @param[in] isDuplex    true for duplex call.
     * @param[in] isTxGranted true if have Tx-Granted.
     * @param[in] callId      Call ID. Omit when called from constructor.
     */
    void setIncoming(bool isHook,
                     bool isDuplex,
                     bool isTxGranted,
                     int  callId = 0);

    /**
     * Sets this instance for an incoming group call.
     * It may have been previously created for an outgoing group or individual
     * call (the target ISSI is now making a group call).
     *
     * @param[in] isHook       true for hook call.
     * @param[in] isE2ee       true for E2EE call.
     * @param[in] callingParty Calling party ID.
     * @param[in] callingType  ResourceData::TYPE_SUBSCRIBER or TYPE_DISPATCHER.
     * @param[in] calledParty  Called group ID.
     * @param[in] callId       Call ID. Omit when called from constructor.
     */
    void setIncomingGrp(bool isHook,
                        bool isE2ee,
                        int  callingParty,
                        int  callingType,
                        int  calledParty,
                        int  callId = 0);

    /**
     * Sets call status to connected. Sets UI buttons to allow call listening if
     * applicable.
     *
     * @param[in] txParty  The talking party ID, if any.
     * @param[in] priority Call priority, if any.
     * @return true if the called party has been changed (for incoming call).
     */
    bool setConnected(const QString &txParty,
                      int            priority = MsgSp::Value::UNDEFINED);

    /**
     * Starts new RTP sessions for audio and video (if applicable), enables
     * incoming audio and, if allowed to talk, enables outgoing audio. Stops
     * playing ringback tone if applicable.
     *
     * @param[in] calledParty Called party ID.
     * @param[in] lclAudPort  Local audio RTP port.
     * @param[in] rmtAudPort  Remote audio RTP port.
     * @param[in] lclVidPort  Local video RTP port. Non-positive value if not a
     *                        video call.
     * @param[in] rmtVidPort  Remote video RTP port. Non-positive value if not a
     *                        video call.
     * @param[in] lclAudKey   Local audio RTP crypto key.
     * @param[in] rmtAudKey   Remote audio RTP crypto key.
     * @param[in] lclVidKey   Local video RTP crypto key.
     * @param[in] rmtVidKey   Remote video RTP crypto key.
     */
    void setConnected(int                calledParty,
                      int                lclAudPort,
                      int                rmtAudPort,
                      int                lclVidPort,
                      int                rmtVidPort,
                      const std::string &lclAudKey,
                      const std::string &rmtAudKey,
                      const std::string &lclVidKey,
                      const std::string &rmtVidKey);

    /**
     * Enables outgoing audio if allowed to talk and RTP session has been
     * started. Updates UI appearance to indicate connected call.
     *
     * @param[in] owner     true if have call ownership.
     * @param[in] enablePtt true to enable PTT.
     * @param[in] txGrant   Tx-Grant value - MsgSp::Value::TRANSMISSION_*.
     */
    void setConnected(bool owner, bool enablePtt, int txGrant);

    /**
     * Changes the call ownership.
     *
     * @param[in] callingPartyName The calling party name.
     * @param[in] priority         New call priority, if any. Ignored if <= 0.
     */
    void changeOwnership(const QString &callingPartyName, int priority = 0);

    void setCallId(int callId) { mCallId = callId; }

    int getCallId() const { return mCallId; }

    /**
     * Sets the talking party ID.
     *
     * @param[in] txParty The talking party ID.
     */
    void setTxParty(const QString &txParty);

    /**
     * Gets the talking party ID.
     *
     * @return] The talking party ID, or 0 if none.
     */
    int getTxParty();

    /**
     * Clears the talking party ID.
     */
    void clearTxParty();

    void setDuration(int d) { mDuration = d; }

    /**
     * Indicates call inclusion result.
     *
     * @param[in] success   true if successful.
     * @param[in] enablePtt true to enable PTT.
     */
    void callInclude(bool success, bool enablePtt);

    /**
     * Indicates Tx-Demand result.
     *
     * @param[in] txGrantVal The Tx-Grant value.
     * @param[in] enablePtt  true to enable PTT.
     * @param[in] txParty    The talking party ID.
     */
    void txGrant(int txGrantVal, bool enablePtt, const QString &txParty);

    /**
     * Indicates Tx-Ceased result.
     *
     * @param[in] isResponse true if received Tx-Ceased is a response.
     * @param[in] enablePtt  true to enable PTT.
     */
    void txCeased(bool isResponse, bool enablePtt);

    /**
     * Prevents call record when dismissed - for a mobile call answered by
     * another dispatcher.
     */
    void setNoRecord();

    /**
     * Ends the call if necessary. For use only to end the call from another
     * module, but not after a proper release.
     *
     * @param[in] isByUser true if initiated by user.
     */
    void end(bool isByUser = false);

    /**
     * Upon call release, checks whether PTT is pressed now in an active call.
     * If so, clears the current call and starts a new one.
     *
     * @param[in] duration The call duration, if any (for monitored call only).
     * @return Whether the checked scenario is true.
     */
    bool checkPttOnCallRelease(int duration);

    /**
     * Releases a call (for Call Listening and Call Inclusion).
     *
     * @param[in] result Call Listening result - MsgSp::Value::RESULT_*.
     */
    void release(int result = 0);

    /**
     * Releases an outgoing call that is still in setup phase, due to network
     * call release on failure. Stops playing ringback tone if applicable.
     *
     * @param[in] failedCause The failure reason.
     */
    void releaseOutgoing(const QString &failedCause);

    /**
     * Ends the on-going call, if any, and resets this instance to initial
     * outgoing call state.
     * Main uses:
     *  - From destructor.
     *  - To keep this instance open after the call ends.
     *
     * @param[in] onDelete true if called from destructor.
     */
    void reset(bool onDelete = false);

    /**
     * Disables outgoing audio.
     */
    void disableOutAudio();

    /**
     * Disables incoming audio.
     */
    void disableInAudio();

    /**
     * Starts a call.
     */
    void startCall();

    /**
     * Handles PTT press or release from external device or another module.
     *
     * @param[in] isPressed  true if pressed.
     * @param[in] isExternal true if from external device.
     */
    void doPtt(bool isPressed, bool isExternal = false);

    /**
     * Checks whether the PTT button is pressed.
     *
     * @return true if pressed.
     */
    bool isPttPressed();

    /**
     * Sets the calling party ID, and the window title for a monitored call.
     *
     * @param[in] type ResourceData::TYPE_SUBSCRIBER or TYPE_DISPATCHER.
     * @param[in] id   The ID.
     */
    void setCallingParty(int type, int id);

    /**
     * Sets the called party name and window title for a monitored individual
     * call.
     *
     * @param[in] name The name.
     */
    void setCalledParty(const QString &name);

    int getCalledParty() const { return mCalledParty; }

    int getCallingParty() const { return mCallingParty; }

    QString getCalledPartyName() const { return mCalledPartyName; }

    QString getCallingPartyName() const { return mCallingPartyName; }

    /**
     * Gets either the calling or called party number, depending on the call
     * type.
     *
     * @return The number. 0 if call party does not exist.
     */
    int getCallParty();

    int getPriority() const { return mPriority; }

    /**
     * Checks whether this object is for a group call.
     *
     * @return true if group call.
     */
    bool isGrpCall() const;

    /**
     * Checks whether this call is to/from another dispatcher.
     *
     * @return true if dispatcher call.
     */
    bool isDispatcherCall() const;

    /**
     * Checks whether this call is to/from dispatcher or mobile.
     *
     * @return true for dispatcher or mobile call.
     */
    bool isInternalCall() const;

    /**
     * Checks whether this call is an outgoing call.
     *
     * @return true if outgoing call.
     */
    bool isOutgoingCall() const;

    /**
     * Checks whether this call is not yet started.
     *
     * @return true if not started.
     */
    bool isNotStarted() const;

    /**
     * Checks whether this call has a particular VOIP call ID.
     *
     * @param[in] id The ID.
     * @return true if it has the ID.
     */
    bool hasVoipId(const std::string &id);

    /**
     * Checks whether this call has a particular call party.
     *
     * @param[in] ssi The call party SSI.
     * @return true if it has the call party.
     */
    bool hasCallParty(int ssi) const;

    /**
     * Gets PTT-related information.
     *
     * @param[out] txParty The talking party.
     * @param[out] pttIcon The current PTT button icon.
     * @return true if PTT button is enabled.
     */
    bool getPttInfo(QString &txParty, QIcon &pttIcon);

    /**
     * Checks whether this is to be kept open after the call ends.
     *
     * @return true to keep open.
     */
    bool keepOpen() const;

    /**
     * Updates audio stream receive rate label.
     *
     * @param[in] kbps Receive rate in kbps.
     */
    void onAudioStat(int kbps);

    /**
     * Shows video.
     *
     * @param[in] data         The RGB image data.
     * @param[in] width        The width.
     * @param[in] height       The height.
     * @param[in] bytesPerLine The number of bytes per line (stride).
     */
    void onVideoReceived(uchar *data, int width, int height, int bytesPerLine);

    /**
     * Updates video stream receive rate label.
     *
     * @param[in] kbps Receive rate in kbps.
     */
    void onVideoStat(int kbps);

    /**
     * Shows video preview.
     *
     * @param[in] frame The image.
     */
    void onVideoPreviewReceived(QPixmap frame);

    /**
     * Shows a call failure error message.
     *
     * @param[in] parent The parent widget.
     * @param[in] title  The message title. Omit for VOIP error.
     * @param[in] msg    The error message. Used only if title is not empty.
     */
    static void showFailureMsg(QWidget       *parent,
                               const QString &title = "",
                               const QString &msg   = "");

    static void setSession(ServerSession *session, const QString &name)
    {
        sSession = session;
        sUserName = name;
    }

    static void setAudioManager(AudioManager *am) { sAudioMgr = am; }

    /**
     * Sets the call priorities.
     *
     * @param[in] def        The default priority, or zero or negative value to
     *                       use default.
     * @param[in] preemptive The preemptive priority, or zero or negative value
     *                       to use default.
     */
    static void setPriorities(int def, int preemptive);

    static const QString &getUserName() { return sUserName; }

    /**
     * Callback function for video decoded frame.
     *
     * @param[in] obj  CallWindow object, owner of the callback function.
     * @param[in] data The RGB image data.
     * @param[in] w    The width.
     * @param[in] h    The height.
     * @param[in] bpl  The number of bytes per line (stride).
     */
    static void decodeCb(void *obj, uchar *data, int w, int h, int bpl);

    /**
     * Callback function for audio stream statistics.
     *
     * @param[in] obj  CallWindow object, owner of the callback function.
     * @param[in] kbps Receive rate in kbps.
     */
    static void audStatCb(void *obj, int kbps);

    /**
     * Callback function for video stream statistics.
     *
     * @param[in] obj  CallWindow object, owner of the callback function.
     * @param[in] kbps Receive rate in kbps.
     */
    static void vidStatCb(void *obj, int kbps);

    /**
     * Callback function for video preview frame.
     *
     * @param[in] obj   CallWindow object, owner of the callback function.
     * @param[in] frame The image.
     */
    static void previewCb(void *obj, QPixmap frame);

    /**
     * Releases static resources.
     */
    static void finalize();

signals:
    void activeOutCall(CallWindow *cw);
    void activeInCall(int callId, int callParty);
    void callCancel(int ssi);
    void callData(int                              type,
                  int                              priority,
                  bool                             isDuplex,
                  const QString                   &startTime,
                  const QString                   &duration,
                  const QString                   &callingParty,
                  const QString                   &calledParty,
                  const QString                   &failedCause,
                  const MessageDialog::TableDataT &data);
    void callRelease(int callParty);
    void callTimeout(int callId, int callingParty);
    void drawFrame(QPixmap p);
    void incomingConnected(int ssi, int callId);

public slots:
    /**
     * Handles PTT key press event.
     *
     * @param[in] evt The event.
     */
    void keyPressEvent(QKeyEvent *evt);

    /**
     * Handles PTT key release event.
     *
     * @param[in] evt The event.
     */
    void keyReleaseEvent(QKeyEvent *evt);

private slots:
    /**
     * Enables PTT button.
     */
    void onHookClicked();

    /**
     * Handles PTT press.
     */
    void onPttPressed();

    /**
     * Handles PTT release.
     */
    void onPttReleased();

private:
    enum AudioType
    {
        AUDIOTYPE_BOTH,
        AUDIOTYPE_IN,
        AUDIOTYPE_OUT
    };

    enum ToneType
    {
        TONETYPE_BEEP,
        TONETYPE_RINGBACK,
        TONETYPE_RINGING
    };

    struct PttData
    {
        PttData() {}
        PttData(const QString &start, const QString &p, int t) :
            startTime(start), txParty(p), seconds(t) {}

        QString startTime;
        QString txParty;
        int     seconds = 0;
    };
    typedef QList<PttData> PttHistoryT;

    Ui::CallWindow *ui;
    int             mType;
    int             mCallId;
    int             mCalledParty;
    int             mCallingParty;
    //TIME_IN_TRAFFIC value (in seconds) from MON_DISCONNECT
    int             mDuration;
    int             mPriority;
    int             mLocalAudRtpPort;
    int             mRemoteAudRtpPort;
    int             mRemoteVidRtpPort; //negative after video out enabled
    std::string     mLocalAudRtpKey;
    std::string     mRemoteAudRtpKey;
    std::string     mLocalVidRtpKey;
    std::string     mRemoteVidRtpKey;
    time_t          mStartTime;        //only for mobile call
    bool            mOwner;
    bool            mDuplex;
    bool            mE2ee;
    bool            mFirstPtt;         //true until after first PTT press
    bool            mTxGranted;
    std::string     mVoipId1;
    std::string     mVoipId2;          //call listening has 2 IDs
    std::string     mCalledDomain;
    QString         mCalledPartyName;
    QString         mCallingPartyName;
    QString         mFailedCause;
    QTime           mTime;
    QTimer          mTimer;
    //to protect against missing MON-DISCONNECT for a monitored call
    QTimer          mCallTimer;
    //to protect against PTT click as opposed to press
    QTimer          mPttTimer;
    PttHistoryT     mPttHistory;
    QSound         *mRingTone;
    VideoStream    *mVideoStream;

    static int            sCount;
    static int            sPriorityDefault;
    static int            sPriorityPreempt;
    static QString        sUserName;
    static ServerSession *sSession;
    static AudioManager  *sAudioMgr;
    static CallWindow    *sMicOwner;

    /**
     * Initializes the CallWindow interface.
     */
    void init();

    /**
     * Enables or disables incoming/outgoing audio. If outgoing is enabled,
     * activates the associated RTP session.
     *
     * @param[in] type      The audio type.
     * @param[in] enabled   true to enable.
     * @param[in] resetMic  true to reset microphone ownership and UI.
     *                      Applicable only for AUDIOTYPE_OUT and if enabled is
     *                      false.
     * @param[in] callParty The call party whose audio is to be enabled or
     *                      disabled. If not provided, uses the stored value,
     *                      and for call listening, additionally
     *                      enables/disables incoming audio for the other call
     *                      party.
     */
    void setAudioEnabled(AudioType type,
                         bool      enabled,
                         bool      resetMic = true,
                         int       callParty = 0);

    /**
     * Stops the RTP session.
     */
    void stopRtp();

    /**
     * Sets the PTT button icon.
     *
     * @param[in] type    The action type - CmnTypes::eActionType.
     * @param[in] pressed true if button is pressed.
     */
    void setPttIcon(int type, bool pressed = false);

    /**
     * Performs PTT press action.
     */
    void onPttPress();

    /**
     * Handles PTT button release and server-forced TX-CEASED.
     *
     * @param[in] doTxCease true to send TX-CEASED to server.
     */
    void onPttRelease(bool doTxCease);

    /**
     * Ends or leaves the call.
     *
     * @param[in] isByUser true if initiated by user.
     */
    void onEndButton(bool isByUser);

    /**
     * Performs UI changes and emits a call event data notification upon call
     * release.
     */
    void endCall();

    /**
     * Checks whether a new network call session (outgoing call, answering
     * incoming call, joining group call or call listening) is allowed based on
     * the concurrent network call limit.
     * If allowed, increments count of concurrent network calls.
     * Answering an incoming Broadcast call is allowed unconditionally.
     *
     * @param[in] isPtt true for PTT call.
     * @return true if call is allowed.
     */
    bool incrCount(bool isPtt = false);

    /**
     * Decrements count of concurrent network calls.
     */
    void decrCount();

    /**
     * Creates and plays a ring tone.
     * Only one tone can be played at a time.
     *
     * @param type The tone type
     */
    void playRingTone(ToneType type);

    /**
     * Stops and destroys current playing ring tone, if any.
     */
    void stopRingTone();

    /**
     * Starts video streaming.
     *
     * @param[in] lclPort Local port.
     * @param[in] lclKey  Local crypto key.
     */
    void startVideo(int lclPort, const std::string &lclKey);

    /**
     * Starts or stops outgoing video stream.
     *
     * @param[in] doStart true to start.
     */
    void setVideoOut(bool doStart);

    /**
     * Starts or stops outgoing video stream.
     *
     * @param[in] doStart true to start.
     */
    void setWebRTCOut(bool doStart);
};
#endif //CALLWINDOW_H
