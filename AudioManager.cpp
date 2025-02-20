/**
 * Implementation of input and output audio device management.
 *
 * Copyright (C) Sapura Secured Technologies, 2014-2025. All Rights Reserved.
 *
 * @file
 * @version $Id: AudioManager.cpp 1900 2025-01-23 08:38:48Z zulzaidi $
 * @author Ahmad Syukri
 */
#include <assert.h>

#include "AudioManager.h"

using namespace std;

static const int    AMI_MASK = 0x55;
static const string LOGPREFIX("AudioManager:: ");

set<AudioDevice *> AudioManager::sIdleOutDevs;

AudioManager::AudioManager(Logger *logger) :
mLogger(logger), mInDevice(0), mActiveRtp(0)
{
    if (logger == 0)
    {
        assert("Bad param in AudioManager::AudioManager" == 0);
        return;
    }
    connect(this, &AudioManager::deleteRtp, this,
            [](RtpSession *rtp) { delete rtp; });
    mInDevice = new AudioDevice(AudioDevice::TYPE_INPUT, mLogger);
    connect(mInDevice, &AudioDevice::received, this,
            [this](const char *data, qint64 len)
            {
                //receive audio data
                if (mRtpSessionMap.empty())
                    return;
                int n = len/2; //a-law output length is half from original
                char *b = new char[n];
                if (mActiveRtp != 0)
                {
                    char *p = b;
                    const short *q = (const short *) data;
                    int i = n;
                    for (; i>0; --i,++p,++q)
                    {
                        *p = linearToAlaw(*q);
                    }
                    mActiveRtp->send(b, n);
                }
                //send silence packet which contains 0xD5 for A-law signed data
                //to other sessions, otherwise no incoming data for those
                //sessions
                memset(b, 0xD5, n);
                for (auto &it : mRtpSessionMap)
                {
                    if (it.second != mActiveRtp)
                        it.second->send(b, n);
                }
                delete [] b;
            });
    //create and start pool of devices ready for immediate use, to avoid long
    //delay at each call startup due to AudioDevice::start() execution, which
    //may cause missing initial audio
    AudioDevice *dev;
    int i = 3;
    for (; i>0; --i)
    {
        dev = new AudioDevice(AudioDevice::TYPE_OUTPUT, mLogger);
        sIdleOutDevs.insert(dev);
        dev->start();
        dev->activateOut(false);
    }
    LOGGER_DEBUG(mLogger, LOGPREFIX << "Started outDevs idle="
                 << sIdleOutDevs.size());
}

AudioManager::~AudioManager()
{
    delete mInDevice;
    for (auto &it : mRtpSessionMap)
    {
        delete it.second;
    }
    for (auto &it : mSessionDataMap)
    {
        delete it.second.device;
    }
    if (!sIdleOutDevs.empty())
    {
        LOGGER_DEBUG(mLogger, LOGPREFIX << "Deleting outDevs idle="
                     << sIdleOutDevs.size());
        for (auto d : sIdleOutDevs)
        {
            delete d;
        }
    }
}

void AudioManager::startRtp(int           id,
                            int           lclPort,
                            int           rmtPort,
                            void         *cbObj,
                            StatCbFn      cbFn,
                            const string &lclKey,
                            const string &rmtKey)
{
    LOGGER_DEBUG(mLogger, LOGPREFIX << "startRtp: " << id << " " << lclPort
                 << " " << rmtPort);
    if (mRtpSessionMap.count(id) != 0)
    {
        LOGGER_ERROR(mLogger, LOGPREFIX << "startRtp: Existing ID " << id);
        return;
    }
    auto *rtp = new RtpSession(RtpSession::TYPE_AUDIO_PCMA, lclPort, rmtPort,
                               mLogger, this, rtpRcvCb, rtpStatCb);
    rtp->setCryptoKey(lclKey, rmtKey);
    mRtpSessionMap[id] = rtp;
    AudioDevice *dev;
    if (sIdleOutDevs.empty())
    {
        //insufficient pool devices - add
        dev = new AudioDevice(AudioDevice::TYPE_OUTPUT, mLogger);
        dev->start();
        LOGGER_DEBUG(mLogger, LOGPREFIX << "startRtp: New outDev " << dev << ":"
                     << dev->getState());
    }
    else
    {
        dev = *(sIdleOutDevs.begin());
        dev->activateOut(true);
        //state 0=active, 1=suspended, 2=stopped, 3=idle, 4=interrupted
        LOGGER_DEBUG(mLogger, LOGPREFIX << "startRtp: outDevs idle="
                     << sIdleOutDevs.size() << ", resumed " << dev << ":"
                     << dev->getState());
        sIdleOutDevs.erase(dev);
    }
    mSessionDataMap[rtp] = {dev, true, cbObj, cbFn};
    if (mInDevice->getState() != QAudio::ActiveState)
        mInDevice->start();
}

void AudioManager::stopRtp(int id)
{
    LOGGER_DEBUG(mLogger, LOGPREFIX << "stopRtp: " << id);
    RtpSession *rtp = getRtpSession(id, true);
    if (rtp != 0)
    {
        if (mActiveRtp == rtp)
            mActiveRtp = 0; //stop sending audio data
        rtp->stop(); //stop receiving audio data
        if (mSessionDataMap.count(rtp) != 0)
        {
            auto *dev = mSessionDataMap[rtp].device;
            dev->activateOut(false);
            sIdleOutDevs.insert(dev);
            mSessionDataMap.erase(rtp);
            LOGGER_DEBUG(mLogger, LOGPREFIX << "stopRtp: Suspended outDev "
                         << dev << ":" << dev->getState() << ", idle="
                         << sIdleOutDevs.size());
        }
        if (mRtpSessionMap.empty())
            mInDevice->stop();
    }
}

bool AudioManager::setActiveOutRtp(int id, bool activate)
{
    LOGGER_DEBUG(mLogger, LOGPREFIX << "setActiveOutRtp: " << id << ' '
                 << activate);
    RtpSession *rtp = getRtpSession(id, false);
    if (rtp == 0)
    {
        if (activate)
            LOGGER_ERROR(mLogger, LOGPREFIX << "setActiveOutRtp true: "
                         "Invalid ID " << id);
        return false;
    }
    if (activate)
        mActiveRtp = rtp;
    else if (rtp == mActiveRtp)
        mActiveRtp = 0;
    return true;
}

bool AudioManager::setActiveInRtp(int id, bool activate)
{
    LOGGER_DEBUG(mLogger, LOGPREFIX << "setActiveInRtp: " << id << ' '
                 << activate);
    RtpSession *rtp = getRtpSession(id, false);
    if (rtp == 0)
    {
        if (activate)
            LOGGER_ERROR(mLogger, LOGPREFIX << "setActiveInRtp true: "
                         "Invalid ID " << id);
        return false;
    }
    mSessionDataMap[rtp].enabled = activate;
    return true;
}

bool AudioManager::hasActiveAudio()
{
     if (mActiveRtp != 0) //outgoing
         return true;
     //check for incoming
     for (auto &it : mSessionDataMap)
     {
         if (it.second.enabled)
             return true;
     }
     return false;
}

void AudioManager::rtpRcvCb(void *obj, RtpSession *rtp, char *data, int len)
{
    static_cast<AudioManager *>(obj)->rtpReceived(rtp, data, len);
}

void AudioManager::rtpStatCb(void *obj, RtpSession *rtp, int kbps)
{
    static_cast<AudioManager *>(obj)->rtpStat(rtp, kbps);
}

void AudioManager::onAudioInChanged(const QString &devName)
{
    bool isActive = (mInDevice->getState() == QAudio::ActiveState);
    mInDevice->setDevice(devName);
    if (isActive)
        mInDevice->start();
}

void AudioManager::onAudioOutChanged(const QString &devName)
{
    for (auto &it : mSessionDataMap)
    {
        it.second.device->setDevice(devName);
        it.second.device->start();
    }
    for (auto *d : sIdleOutDevs)
    {
        d->setDevice(devName);
        d->start();
        d->activateOut(false);
    }
}

inline RtpSession *AudioManager::getRtpSession(int id, bool doErase)
{
    if (mRtpSessionMap.count(id) != 0)
    {
        RtpSession *rtp = mRtpSessionMap[id];
        if (doErase)
            mRtpSessionMap.erase(id);
        return rtp;
    }
    return 0;
}

void AudioManager::rtpReceived(RtpSession *rtp, char *data, int len)
{
    if (data == 0)
    {
        //called at the end of the RtpSession receive thread, which means it is
        //exiting
        emit deleteRtp(rtp);
        return;
    }
    if (mSessionDataMap.count(rtp) != 0 && mSessionDataMap[rtp].enabled)
    {
        short *b = new short[len];
        short *p = b;
        char  *q = data;
        int    i = len;
        for (; i>0; --i,++p,++q)
        {
            *p = alawToLinear(*q);
        }
        mSessionDataMap[rtp].device->writeOutput((const char *) b,
                                                 len * sizeof(short));
        delete [] b;
    }
}

void AudioManager::rtpStat(RtpSession *rtp, int kbps)
{
    if (mSessionDataMap.count(rtp) != 0 && mSessionDataMap[rtp].cbFn != 0)
        mSessionDataMap[rtp].cbFn(mSessionDataMap[rtp].cbObj, kbps);
}

short AudioManager::alawToLinear(unsigned char alaw)
{
    alaw ^= AMI_MASK;
    int i = ((alaw & 0x0F) << 4) + 8; // rounding error
    int seg = (((int) alaw & 0x70) >> 4);
    if (seg != 0)
        i = (i + 0x100) << (seg - 1);
    return (short) (((alaw & 0x80) != 0)? i: -i);
}

unsigned char AudioManager::linearToAlaw(short linear)
{
    static int segEnd[8] =
    {
        0xFF, 0x1FF, 0x3FF, 0x7FF, 0xFFF, 0x1FFF, 0x3FFF, 0x7FFF
    };
    int mask = AMI_MASK;
    int pcmVal = linear;
    if (pcmVal >= 0)
        mask |= 0x80;
    else
        pcmVal = -pcmVal;
    int *p = segEnd;
    int  i = 0;
    for (; i<8; ++i, ++p)
    {
        if (pcmVal <= *p)
            break;
    }
    return ((i << 4) | ((pcmVal >> ((i != 0)? (i + 3): 4)) & 0x0F)) ^ mask;
}
