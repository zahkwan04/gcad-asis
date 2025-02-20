/**
 * Implementation of RTP communications, managing audio, video and control
 * packets to/from a VOIP server.
 *
 * Copyright (C) Sapura Secured Technologies, 2014-2025. All Rights Reserved.
 *
 * @file
 * @version $Id: RtpSession.cpp 1900 2025-01-23 08:38:48Z zulzaidi $
 * @author Ahmad Syukri
 * @author Zulzaidi Atan
 */
#include <assert.h>

#include "VoipSessionBase.h"
#include "RtpSession.h"

using namespace std;

//receive thread state
enum eState
{
    STATE_RUN,
    STATE_STOP,
    STATE_END
};

uint32_t RtpSession::sSsrc(0);
string   RtpSession::sRemoteIp;

static const uint16_t BYTES_PER_SAMPLE = 1U;
static const int      TIME_TO_LIVE     = 16;    //in milliseconds

void *rtpSessionStartRtpRecvThread(void *arg)
{
    static_cast<RtpSession *>(arg)->recvThread();
    return 0;
}

RtpSession::RtpSession(int             type,
                       uint16_t        localPort,
                       uint16_t        remotePort,
                       Logger         *logger,
                       void           *cbObj,
                       RecvCallbackFn  recvCbFn,
                       StatCallbackFn  statCbFn) :
mTs(0), mTsInc(0), mRxSeq(0), mState(STATE_END), mBytesRcvd(0),
mLocalPort(localPort), mRemotePort(remotePort), mRecvThread(0), mLogger(logger),
mRtp(0), mCbObj(cbObj), mRxCbFn(recvCbFn), mStatCbFn(statCbFn)
{
    PalLock::init(&mRtpLock);
    if (logger == 0 || cbObj == 0)
    {
        assert("Bad param in RtpSession::RtpSession" == 0);
        return;
    }
    if (++sSsrc == 0)
        sSsrc = 1;
    ostringstream oss;
    oss << "RtpSession[SSRC=0x" << std::hex << sSsrc << std::dec << " Rx:"
        << mLocalPort << " Tx:" << mRemotePort << " TxIp=" << sRemoteIp << "]::";
    mLogPrefix = oss.str();
    if (localPort % 2 != 0)
    {
        LOGGER_ERROR(mLogger, mLogPrefix << " Local port must be even.");
        return;
    }
    if (remotePort % 2 != 0)
    {
        LOGGER_ERROR(mLogger, mLogPrefix << " Remote port must be even.");
        return;
    }
    mSample = (type == TYPE_VIDEO)? VoipSessionBase::SAMPLE_VIDEO:
                                    VoipSessionBase::SAMPLE_AUDIO;
    switch (type)
    {
        case TYPE_AUDIO_ACELP:
            mPayload = VoipSessionBase::PAYLOAD_AUDIO_ACELP;
            mPacketTime = VoipSessionBase::PTIME_ACELP;
            mRxDataMax = 120/VoipSessionBase::PTIME_ACELP; //size for 120ms data
            break;
        case TYPE_AUDIO_PCMA:
            mPayload = VoipSessionBase::PAYLOAD_AUDIO_PCMA;
            mPacketTime = VoipSessionBase::PTIME_PCMA;
            mRxDataMax = 120/VoipSessionBase::PTIME_PCMA; //size for 120ms data
            break;
        case TYPE_VIDEO:
        default:
            mPayload = VoipSessionBase::PAYLOAD_VIDEO_H264;
            mPacketTime = 33; //1000ms/30fps
            mRxDataMax = 10;  //can be multiple packets per frame
            break;
    }
    mTsInc = mPacketTime * mSample/1000;
    LOGGER_DEBUG(mLogger, mLogPrefix << " Start.");
    //RTCP bandwidth is 5% of the session rate in bytes/sec
    mRtp = rtp_init(sRemoteIp.c_str(), mLocalPort, mRemotePort, TIME_TO_LIVE,
                    0.05 * BYTES_PER_SAMPLE * mSample, rtpSessionCallback,
                    reinterpret_cast<uint8_t *>(this));
    if (mRtp == NULL)
    {
        LOGGER_ERROR(mLogger, mLogPrefix << "RtpSession: rtp_init failure.");
        mRtp = 0;
        return;
    }
    rtp_set_my_ssrc(mRtp, sSsrc);
    rtp_set_option(mRtp, RTP_OPT_FILTER_MY_PACKETS, 1);
    rtp_set_option(mRtp, RTP_OPT_WEAK_VALIDATION, 1);
    rtp_set_option(mRtp, RTP_OPT_REUSE_PACKET_BUFS, 1);
    rtp_set_option(mRtp, RTP_OPT_PROMISC, 1);
    PalThread::start(&mRecvThread, rtpSessionStartRtpRecvThread, this);
}

RtpSession::~RtpSession()
{
    if (mState == STATE_RUN)
    {
        //destroyed without stop() called - only during app exit cleanup
        //prevent recvThread() from making callback on exit
        mLocalPort = 0;
        stop();
    }
    if (mRecvThread != 0)
    {
        PalThread::stop(mRecvThread);
        while (mState != STATE_END) //wait for thread to end
        {
            PalThread::msleep(10);
        }
    }
    for (auto &it : mRxData)
    {
        delete it.second.data;
    }
    if (mRtp != 0)
        rtp_done(mRtp);
    PalLock::destroy(&mRtpLock);
    LOGGER_VERBOSE(mLogger, mLogPrefix << " End.");
}

void RtpSession::setCryptoKey(const string &lclKey, const string &rmtKey)
{
    if (!lclKey.empty() && !rmtKey.empty())
        srtp_set_aes_gcm_256_8_auth_key(mRtp, lclKey.c_str(), rmtKey.c_str());
}

void RtpSession::stop()
{
    LOGGER_DEBUG(mLogger, mLogPrefix << "stop");
    if (mRtp != 0)
        rtp_send_bye(mRtp);
    mState = STATE_STOP;
}

bool RtpSession::isRunning()
{
    return (mState == STATE_RUN);
}

int RtpSession::send(char *data, int len, bool marker)
{
    LOGGER_DEBUG3(mLogger, mLogPrefix << "send: " << len);
    if (data == 0 || len == 0)
    {
        LOGGER_ERROR(mLogger, mLogPrefix << "send: Missing data. data=" << data
                     << " len=" << len);
        return -1;
    }
    if (mRtp == 0)
        return -2;
    //send control packets
    PalLock::take(&mRtpLock);
    rtp_send_ctrl(mRtp, mTs, NULL);
    PalLock::release(&mRtpLock);
    //send data packets
    int sentBytes = rtp_send_data(mRtp, mTs, mPayload, (marker)? 1: 0, 0, 0,
                                  data, len, 0, 0, 0);
    mTs += mTsInc; //update timestamp
    return sentBytes;
}

std::string &RtpSession::getLogPrefix()
{
    return mLogPrefix;
}

void RtpSession::recvThread()
{
    LOGGER_DEBUG(mLogger, mLogPrefix << "recvThread: Started.");
    mState = STATE_RUN;
    time_t startTime = time(0);
    time_t curTime;
    struct timeval timeout;
    while (mState == STATE_RUN)
    {
        //send control packets
        PalLock::take(&mRtpLock);
        rtp_send_ctrl(mRtp, mTs, NULL);
        PalLock::release(&mRtpLock);
        //receive control and data packets
        timeout.tv_sec  = 0;
        timeout.tv_usec = mPacketTime * 1000;

        if (mState != STATE_RUN)
            break;
        rtp_recv(mRtp, &timeout, mTs);
        if (mState != STATE_RUN)
            break;

        //state maintenance
        rtp_update(mRtp);
        if (mStatCbFn != 0)
        {
            curTime = time(0);
            if (curTime - startTime >= 3) //update receive rate every 3 sec
            {
                mStatCbFn(mCbObj, this,
                          ((mBytesRcvd * 8)/(curTime - startTime))/1000);
                mBytesRcvd = 0;
                startTime = curTime;
            }
        }
    }
    if (mLocalPort != 0)
        mRxCbFn(mCbObj, this, 0, 0);
    LOGGER_DEBUG(mLogger, mLogPrefix << "recvThread: Stopped.");
    mState = STATE_END;
}

bool RtpSession::init()
{
    return (srtp_initialize() == 0);
}

void RtpSession::rtpSessionCallback(struct rtp *s, rtp_event *e)
{
    if (s == NULL || e == 0)
    {
        assert("Bad param in RtpSession::rtpSessionCallback" == 0);
        return;
    }
    RtpSession *pThis = reinterpret_cast<RtpSession *>(rtp_get_userdata(s));
    switch (e->type)
    {
        case RX_RTP:
            if (pThis->isRunning())
            {
                rtp_packet *p = static_cast<rtp_packet *>(e->data);
                pThis->recv(p);
                LOGGER_DEBUG3(pThis->mLogger, pThis->mLogPrefix << "recv SSRC="
                              << std::hex << e->ssrc << ' ' << std::dec << p->ts
                              << ' ' << p->data_len);
            }
            break;
        case RX_BYE:
            LOGGER_DEBUG2(pThis->mLogger, pThis->getLogPrefix() << " RX_BYE "
                          << "SSRC=0x" << std::hex << e->ssrc << "="
                          << std::dec << e->ssrc);
            break;
        case SOURCE_CREATED:
            LOGGER_DEBUG2(pThis->mLogger, pThis->getLogPrefix()
                          << " SOURCE_CREATED " << "SSRC=0x" << std::hex
                          << e->ssrc << "=" << std::dec << e->ssrc);
            break;
        case SOURCE_DELETED:
            LOGGER_DEBUG2(pThis->mLogger, pThis->getLogPrefix()
                          << " SOURCE_DELETED " << "SSRC=0x" << std::hex
                          << e->ssrc << "=" << std::dec << e->ssrc);
            break;
        case RX_RTCP_START:
        case RX_RTCP_FINISH:
        case RX_SDES:
        case RX_SR:
        case RX_RR:
        case RX_RR_EMPTY:
        case RR_TIMEOUT:
        case RX_APP:
        default:
            break; //do nothing
    } //switch (e->type)
}

void RtpSession::recv(rtp_packet *p)
{
    assert(p != 0);
    if (p->data == NULL || p->data_len <= 0 || (p->seq > 0 && p->seq < mRxSeq))
        return; //discard invalid or old data
    mBytesRcvd += p->data_len;
    if (p->seq == 0 && !mRxData.empty())
    {
        //sequence rollover - for simplicity, process all pending data
        for (auto &it : mRxData)
        {
            mRxCbFn(mCbObj, this, it.second.data, it.second.len);
            delete it.second.data;
        }
        mRxData.clear();
    }
    if (mRxSeq == 0 || p->seq == 0 || p->seq == mRxSeq + 1)
    {
        //first or expected data
        mRxCbFn(mCbObj, this, p->data, p->data_len);
        mRxSeq = p->seq;
    }
    else if (p->seq > mRxSeq + 1)
    {
        //higher seq than expected, wait for missing data
        mRxData[p->seq] = RxData(p->data, p->data_len);
    }
    if (!mRxData.empty())
    {
        auto it = mRxData.begin();
        if (mRxData.size() >= mRxDataMax)
            mRxSeq = it->first; //do not wait anymore, just skip missing data
        while (it != mRxData.end())
        {
            if (it->first > mRxSeq + 1)
                break; //wait for missing data
            mRxCbFn(mCbObj, this, it->second.data, it->second.len);
            mRxSeq = it->first;
            delete it->second.data;
            it = mRxData.erase(it);
        }
    }
}
