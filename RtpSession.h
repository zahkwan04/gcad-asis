/**
 * A class that provides an RTP session between VOIP client and VOIP server.
 *
 * Copyright (C) Sapura Secured Technologies, 2014-2025. All Rights Reserved.
 *
 * @file
 * @version $Id: RtpSession.h 1900 2025-01-23 08:38:48Z zulzaidi $
 * @author Ahmad Syukri
 * @author Zulzaidi Atan
 */
#ifndef RTPSESSION_H
#define RTPSESSION_H

#include <map>
#include <string>

#include "Logger.h"
#include "PalThread.h"

extern "C"
{
#if defined(_WIN32) || defined(WIN32)
#include "config_win32.h"
#else
#include "config_unix.h"
#endif
#include "rtp.h"
}

class RtpSession
{
public:
    enum eType
    {
        TYPE_AUDIO_ACELP,
        TYPE_AUDIO_PCMA,
        TYPE_VIDEO
    };

    //callback function signature for received RTP data
    typedef void (*RecvCallbackFn)(void       *obj,
                                   RtpSession *rtp,
                                   char       *msg,
                                   int         len);
    //callback function signature for RTP receive statistics
    typedef void (*StatCallbackFn)(void *obj, RtpSession *rtp, int kbps);

#ifdef NO_VOIP
    RtpSession(int, uint16_t, uint16_t, Logger *, void *, RecvCallbackFn,
               StatCallbackFn) {}

    ~RtpSession() {}

    void stop() {}

    int send(char *, int) { return 0; }

    static void setRemoteIp(const std::string &) {}
#else
    /**
     * Establishes RTP connection to the VOIP server.
     * Starts the RTP data receive thread.
     *
     * @param[in] type       Session type - eType.
     * @param[in] localPort  Even number port to which to bind the UDP socket.
     * @param[in] remotePort Even number port to which to send UDP packets.
     * @param[in] logger     Logger object.
     * @param[in] cbObj      Callback function owner.
     * @param[in] recvCbFn   Callback function for received RTP data.
     * @param[in] statCbFn   Callback function for RTP statistics.
     */
    RtpSession(int             type,
               uint16_t        localPort,
               uint16_t        remotePort,
               Logger         *logger,
               void           *cbObj,
               RecvCallbackFn  recvCbFn,
               StatCallbackFn  statCbFn = 0);

    ~RtpSession();

    /**
     * Sets SRTP AES 256 GCM with 8-byte authentication tag crypto key.
     *
     * @param[in] lclKey Local crypto key.
     * @param[in] rmtKey Remote crypto key.
     */
    void setCryptoKey(const std::string &lclKey, const std::string &rmtKey);

    /**
     * Sends an RTP BYE and signals the receive thread to stop.
     */
    void stop();

    /**
     * Checks for running state.
     *
     * @return true if running.
     */
    bool isRunning();

    /**
     * Sends RTP data to the VOIP server.
     *
     * @param[in] data   The data to send.
     * @param[in] len    The data length in bytes.
     * @param[in] marker RTP marker.
     * @return Number of bytes sent, or negative value on error.
     */
    int send(char *data, int len, bool marker = false);

    /**
     * Gets the object identifier for logging.
     *
     * @return The object identifier.
     */
    std::string &getLogPrefix();

    /**
     * Receives RTP and RTCP data. Also sends RTCP data before receiving.
     */
    void recvThread();

    //packet time * sample in kHz
    int getFrameCount() { return mPacketTime * mSample/1000; }

    int getSampleRate() { return mSample; }

    /**
     * Initializes resources.
     * Must be called once at the start before using this module.
     *
     * @return true if successful.
     */
    static bool init();

    /**
     * Handles RTP events in an application-specific way.
     *
     * @param[in] s The RTP session.
     * @param[in] e The RTP event.
     */
    static void rtpSessionCallback(struct rtp *s, rtp_event *e);

    /**
     * Sets the VOIP server IP.
     *
     * @param[in] ip The IP.
     */
    static void setRemoteIp(const std::string &ip) { sRemoteIp = ip; }
#endif //NO_VOIP

private:
    struct RxData
    {
        RxData() : data(0), len(0) {}
        RxData(char *d, int l) : data(d), len(l)
        {
            data = new char[len];
            memcpy(data, d, len);
        }
        char *data;
        int   len;
    };
    typedef std::map<int, RxData> RxDataMapT; //key is RTP packet sequence

    uint32_t            mTs;              //RTP timestamp
    uint32_t            mTsInc;           //RTP timestamp increment
    uint16_t            mRxSeq;           //current RTP packet sequence
    int                 mPayload;         //payload type
    int                 mState;           //receive thread state
    int                 mSample;          //sample rate in Hz
    int                 mPacketTime;      //in milliseconds
    int                 mRxDataMax;       //Rx data buffer max size
    int                 mBytesRcvd;
    uint16_t            mLocalPort;
    uint16_t            mRemotePort;
    std::string         mLogPrefix;       //object identifier for logging
    RxDataMapT          mRxData;          //Rx data buffer
    PalThread::ThreadT  mRecvThread;
    PalLock::LockT      mRtpLock;         //guards non-thread-safe RTP lib fns
    Logger             *mLogger;
    struct rtp         *mRtp;
    void               *mCbObj;           //callback function owner object
    RecvCallbackFn      mRxCbFn;          //recv callback function
    StatCallbackFn      mStatCbFn;        //statistic callback function

    static uint32_t     sSsrc;            //SSRC running number
    static std::string  sRemoteIp;

    /**
     * Processes RTP packet and performs callback.
     *
     * @param[in] p Packet.
     */
    void recv(rtp_packet *p);
};
#endif //RTPSESSION_H
