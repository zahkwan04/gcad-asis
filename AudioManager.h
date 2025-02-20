/**
 * A class that provides management of input and output audio devices.
 *
 * Copyright (C) Sapura Secured Technologies, 2014-2025. All Rights Reserved.
 *
 * @file
 * @version $Id: AudioManager.h 1900 2025-01-23 08:38:48Z zulzaidi $
 * @author Ahmad Syukri
 */
#ifndef AUDIOMANAGER_H
#define AUDIOMANAGER_H

#include <set>

#include "AudioDevice.h"
#include "Logger.h"
#include "RtpSession.h"

class AudioManager : public QObject
{
    Q_OBJECT

public:
    //callback signature to receive stream statistics
    typedef void (*StatCbFn)(void *obj, int kbps);

    /**
     * Constructor.
     *
     * @param[in] logger App logger.
     */
    AudioManager(Logger *logger);

    ~AudioManager();

    /**
     * Creates a new RTP session.
     *
     * @param[in] id      Session ID.
     * @param[in] lclPort Local port.
     * @param[in] rmtPort Remote port.
     * @param[in] cbObj   Callback function owner.
     * @param[in] cbFn    Callback function for passing stream statistics.
     * @param[in] lclKey  Local crypto key.
     * @param[in] rmtKey  Remote crypto key.
     */
    void startRtp(int                id,
                  int                lclPort,
                  int                rmtPort,
                  void              *cbObj,
                  StatCbFn           cbFn,
                  const std::string &lclKey = "",
                  const std::string &rmtKey = "");

    /**
     * Ends and destroys an RTP session.
     *
     * @param[in] id The session ID.
     */
    void stopRtp(int id);

    /**
     * Activates or deactivates an RTP session for outgoing audio.
     * Only zero or one session may be active at any one time.
     *
     * @param[in] id       The RTP session ID.
     * @param[in] activate true to activate.
     */
    bool setActiveOutRtp(int id, bool activate);

    /**
     * Activates or deactivates an RTP session for incoming audio.
     *
     * @param[in] id       The RTP session ID.
     * @param[in] activate true to activate.
     */
    bool setActiveInRtp(int id, bool activate);

    /**
     * Checks whether there is any active incoming or outgoing audio.
     *
     * @return true if audio is active.
     */
    bool hasActiveAudio();

    /**
     * Callback function for received RTP payload.
     *
     * @param[in] obj  AudioManager object, callback function owner.
     * @param[in] rtp  The RTP session.
     * @param[in] data The RTP payload.
     * @param[in] len  The payload length in bytes.
     */
    static void rtpRcvCb(void *obj, RtpSession *rtp, char *data, int len);

    /**
     * Callback function for stream statistics.
     *
     * @param[in] obj  AudioManager object, callback function owner.
     * @param[in] rtp  RTP session.
     * @param[in] kbps Stream receive rate in kbps.
     */
    static void rtpStatCb(void *obj, RtpSession *rtp, int kbps);

signals:
    void deleteRtp(RtpSession *rtp);

public slots:
    /**
     * Sets the input device.
     *
     * @param[in] devName The device name.
     */
    void onAudioInChanged(const QString &devName);

    /**
     * Sets the output device.
     *
     * @param[in] devName The device name.
     */
    void onAudioOutChanged(const QString &devName);

private:
    //data associated with an RtpSession
    struct SessionData
    {
        AudioDevice *device;  //output device
        bool         enabled; //output device status
        void        *cbObj;   //callback function owner
        StatCbFn     cbFn;    //stream statistics callback function
    };

    typedef std::map<int, RtpSession *>         RtpSessionMapT;
    typedef std::map<RtpSession *, SessionData> SessionDataMapT;

    Logger          *mLogger;
    AudioDevice     *mInDevice;       //input device
    RtpSession      *mActiveRtp;
    RtpSessionMapT   mRtpSessionMap;  //indexed by called/calling SSI
    SessionDataMapT  mSessionDataMap;

    static std::set<AudioDevice *> sIdleOutDevs;

    /**
     * Gets an RTP session based on the given ID.
     *
     * @param[in] id      The RTP session ID.
     * @param[in] doErase true to erase the RTP session from the map.
     * @return The RTP session, or 0 if not found.
     */
    RtpSession *getRtpSession(int id, bool doErase);

    /**
     * Processes received RTP payload.
     *
     * @param[in] rtp  The RTP session.
     * @param[in] data The RTP payload.
     * @param[in] len  The payload length in bytes.
     */
    void rtpReceived(RtpSession *rtp, char *data, int len);

    /**
     * Processes RTP statistics.
     *
     * @param[in] rtp  RTP session.
     * @param[in] kbps Stream receive rate in kbps.
     */
    void rtpStat(RtpSession *rtp, int kbps);

    /**
     * Converts a-law audio data to linear PCM.
     *
     * @param[in] alaw The audio data.
     */
    static short alawToLinear(unsigned char alaw);

    /**
     * Converts linear PCM audio data to a-law.
     *
     * @param[in] linear The audio data.
     */
    static unsigned char linearToAlaw(short linear);

};
#endif //AUDIOMANAGER_H
