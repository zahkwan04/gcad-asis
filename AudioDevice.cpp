/**
 * Implementation of audio input and output devices.
 * As an output device, uses an internal IO device created by QAudioOutput to
 * write audio data to the output.
 * As an input device, creates a new IO device for QAudioInput to write audio
 * data received.
 *
 * Copyright (C) Sapura Secured Technologies, 2014-2024. All Rights Reserved.
 *
 * @file
 * @version $Id: AudioDevice.cpp 1889 2024-12-03 01:03:32Z zulzaidi $
 * @author Ahmad Syukri
 */
#include <assert.h>
#include <qendian.h>
#include <qmath.h>

#include "Settings.h"
#include "AudioDevice.h"

using namespace std;

static const int     SAMPLE_RATE      = 8000;
static const int     TONE_SAMPLE_RATE = 500;
static const int     CHANNEL_COUNT    = 1;
static const int     SAMPLE_SIZE      = 16;
static const int     PEAK_MAX         = 0x7FFF;
static const QString CODEC    ("audio/pcm");
static const string  LOGPREFIX("AudioDevice:: ");

AudioDevice::AudioDevice(int type, Logger *logger) :
mType(type), mLogger(logger), mAudioIn(0), mAudioOut(0), mDevice(0), mTimer(0)
{
    if (logger == 0 || (type != TYPE_INPUT && type != TYPE_OUTPUT))
    {
        assert("Bad param in AudioDevice::AudioDevice" == 0);
        return;
    }
    mFormat.setSampleRate(SAMPLE_RATE);
    mFormat.setChannelCount(CHANNEL_COUNT);
    mFormat.setSampleSize(SAMPLE_SIZE);
    mFormat.setSampleType(QAudioFormat::SignedInt);
    mFormat.setByteOrder(QAudioFormat::LittleEndian);
    mFormat.setCodec(CODEC);
    //get device from saved settings
    QAudioDeviceInfo info(getDeviceInfo(QString::fromStdString(
                                            Settings::instance().get<string>(
                                                   (mType == TYPE_INPUT)?
                                                   Props::FLD_CFG_AUDIO_IN:
                                                   Props::FLD_CFG_AUDIO_OUT))));
    if (!info.isFormatSupported(mFormat))
        mFormat = info.nearestFormat(mFormat);
    if (mType == TYPE_INPUT)
        mAudioIn = new QAudioInput(info, mFormat, this);
    else
        mAudioOut = new QAudioOutput(info, mFormat, this);
}

AudioDevice::~AudioDevice()
{
    if (mType == TYPE_INPUT)
    {
        delete mDevice;
        delete mAudioIn;
    }
    else
    {
        //output device not deleted here because owned by QAudioOutput
        delete mAudioOut;
    }
}

bool AudioDevice::start()
{
    if (mType == TYPE_INPUT)
    {
        if (mDevice == 0)
            mDevice = new IoDeviceIn(this);
        if (mDevice->open(QIODevice::WriteOnly))
        {
            mAudioIn->start(mDevice);
        }
        else
        {
            LOGGER_ERROR(mLogger, LOGPREFIX << "start: "
                         "Failed to open input device.");
            return false;
        }
    }
    else
    {
        mDevice = mAudioOut->start();
    }
    return true;
}

void AudioDevice::stop()
{
    if (mDevice != 0 && mDevice->isOpen())
    {
        mDevice->close();
        if (mType == TYPE_INPUT)
        {
            mAudioIn->stop();
        }
        else
        {
            mDevice = 0;
            mAudioOut->stop();
        }
    }
}

QStringList AudioDevice::getDeviceNames()
{
    QStringList l;
    foreach (const QAudioDeviceInfo &info,
             QAudioDeviceInfo::availableDevices(
                                    (mType == TYPE_INPUT)? QAudio::AudioInput:
                                                           QAudio::AudioOutput))
    {
        if (!l.contains(info.deviceName()) && info.isFormatSupported(mFormat))
            l.append(info.deviceName());
    }
    return l;
}

void AudioDevice::setDevice(const QString &devName)
{
    if (mType == TYPE_INPUT)
    {
        mAudioIn->disconnect(this);
        delete mAudioIn;
        mAudioIn = new QAudioInput(getDeviceInfo(devName), mFormat, this);
    }
    else
    {
        mDevice = 0;
        mAudioOut->disconnect(this);
        delete mAudioOut;
        mAudioOut = new QAudioOutput(getDeviceInfo(devName), mFormat, this);
    }
}

QAudio::State AudioDevice::getState()
{
    return ((mType == TYPE_INPUT)? mAudioIn->state(): mAudioOut->state());
}

void AudioDevice::receive(const char *data, qint64 len)
{
    assert(mType == TYPE_INPUT);
    emit received(data, len);
}

void AudioDevice::writeOutput(const char *data, qint64 len)
{
    assert(mType == TYPE_OUTPUT);
    if (mDevice != 0)
        mDevice->write(data, len);
}

void AudioDevice::testOutput(bool start)
{
    assert(mType == TYPE_OUTPUT);
    if (!start)
    {
        if (mTimer != 0 && mTimer->isActive())
            mTimer->stop();
        return;
    }
    if (mTestData.size() == 0)
    {
        int n = SAMPLE_SIZE/8; //channel bytes
        qint64 len = SAMPLE_RATE * CHANNEL_COUNT * n * 10;
        mTestData.resize(len);
        auto *p = reinterpret_cast<unsigned char *>(mTestData.data());
        int i = 0; //sample index
        while (len > 0)
        {
            qToLittleEndian<qint16>(static_cast<qint16>(
                                      qSin(2 * M_PI * TONE_SAMPLE_RATE *
                                           qreal(i % SAMPLE_RATE)/SAMPLE_RATE) *
                                      PEAK_MAX),
                                    p);
            p += n;
            len -= n;
            ++i;
        }
    }
    if (mTimer == 0)
    {
        mTimer = new QTimer(this);
        connect(mTimer, &QTimer::timeout, this,
                [this]
                {
                    //write sinusoidal waves audio data
                    assert(mType == TYPE_OUTPUT);
                    if (mAudioOut->state() != QAudio::StoppedState)
                    {
                        int n = mAudioOut->periodSize();
                        int i = (mAudioOut->bytesFree()/n) - 1; //chunks
                        for (; i>=0; --i)
                        {
                            writeOutput(mTestData.data(), n);
                        }
                    }
                });
    }
    mTimer->start(20); //write test data every 20ms
}

int AudioDevice::getPeak(const char *data, qint64 len)
{
    const short *q = (const short *) data;
    qreal maxVal = 0;
    qreal val = 0;
    for (len/=2; len>0; --len,++q)
    {
        val = qAbs(qreal(*q));
        if (val > maxVal)
            maxVal = val;
    }
    return maxVal;
}

int AudioDevice::getMaxPeak()
{
    return PEAK_MAX;
}

QAudioDeviceInfo AudioDevice::getDeviceInfo(const QString &devName)
{
    foreach (const QAudioDeviceInfo &info,
             QAudioDeviceInfo::availableDevices(
                                    (mType == TYPE_INPUT)? QAudio::AudioInput:
                                                           QAudio::AudioOutput))
    {
        if (devName == info.deviceName() && info.isFormatSupported(mFormat))
            return info;
    }
    return ((mType == TYPE_INPUT)? QAudioDeviceInfo::defaultInputDevice():
                                   QAudioDeviceInfo::defaultOutputDevice());
}

qint64 AudioDevice::IoDeviceIn::writeData(const char *data, qint64 len)
{
    assert(mAudioDevice != 0);
    mAudioDevice->receive(data, len);
    return len;
}
