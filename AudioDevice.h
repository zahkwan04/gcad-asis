/**
 * A class that provides an interface of audio input and output devices.
 * As an output device, writes audio data to an audio output device.
 * As an input device, accepts data from QAudioInput and emits via a signal
 * without further processing.
 *
 * Copyright (C) Sapura Secured Technologies, 2014-2024. All Rights Reserved.
 *
 * @file
 * @version $Id: AudioDevice.h 1895 2024-12-31 07:38:52Z zulzaidi $
 * @author Ahmad Syukri
 */
#ifndef AUDIODEVICE_H
#define AUDIODEVICE_H

#include <QAudioDeviceInfo>
#include <QAudioFormat>
#include <QAudioInput>
#include <QAudioOutput>
#include <QIODevice>
#include <QTimer>

#include "Logger.h"

class AudioDevice : public QObject
{
    Q_OBJECT

public:
    enum eType
    {
        TYPE_INPUT,
        TYPE_OUTPUT
    };

    /**
     * Sets the input or output device from the saved settings.
     *
     * @param[in] type   The device type - eType.
     * @param[in] logger App logger.
     */
    AudioDevice(int type, Logger *logger);

    ~AudioDevice();

    /**
     * Starts device.
     *
     * @return true if successful.
     */
    bool start();

    /**
     * Stops device.
     */
    void stop();

    void activateOut(bool active)
    {
        if (mAudioOut != 0)
        {
            if (active)
                mAudioOut->resume();
            else
                mAudioOut->suspend();
        }
    }

    /**
     * Gets a list of audio device names on the system.
     *
     * @return The list.
     */
    QStringList getDeviceNames();

    /**
     * Sets the device for audio processing.
     *
     * @param[in] devName The device name.
     */
    void setDevice(const QString &devName);

    /**
     * Gets the current device state.
     *
     * @return The state.
     */
    QAudio::State getState();

    /**
     * Receives audio data from the input IO device and emits the data in a
     * signal. Only applicable to TYPE_INPUT.
     *
     * @param[in] data The audio data.
     * @param[in] len  The data length in bytes.
     */
    void receive(const char *data, qint64 len);

    /**
     * Writes audio data to the output. Only applicable to TYPE_OUTPUT.
     *
     * @param[in] data The audio data.
     * @param[in] len  The data length in bytes.
     */
    void writeOutput(const char *data, qint64 len);

    /**
     * Tests the audio output by writing sinusoidal waves audio data to it.
     * Only applicable to TYPE_OUTPUT.
     *
     * @param[in] start true to start test, false to stop.
     */
    void testOutput(bool start);

    /**
     * Gets the peak level of audio data.
     *
     * @param[in] data The audio data.
     * @param[in] len  The data length in bytes.
     * @return The level.
     */
    static int getPeak(const char *data, qint64 len);

    /**
     * Gets the maximum peak level.
     *
     * @return The level.
     */
    static int getMaxPeak();

signals:
    void received(const char *data, qint64 len);

private:
    class IoDeviceIn : public QIODevice
    {
    public:
        /**
         * Input IO Device.
         *
         * @param[in] dev The AudioDevice for writing received data.
         */
        IoDeviceIn(AudioDevice *dev) : mAudioDevice(dev) {}

    protected:
        /**
         * Function called by QAudioInput for writing audio data received.
         *
         * @param[in] data The audio data.
         * @param[in] len  The data length in bytes.
         * @return The number of bytes, always equal to len.
         */
        qint64 writeData(const char *data, qint64 len);

        /**
         * Not used for input device.
         */
        qint64 readData(char *, qint64) { return 0; }

    private:
        AudioDevice *mAudioDevice;
    }; //class IoDeviceIn

    int           mType;
    QByteArray    mTestData;
    QAudioFormat  mFormat;
    Logger       *mLogger;
    QAudioInput  *mAudioIn;
    QAudioOutput *mAudioOut;
    QIODevice    *mDevice;
    QTimer       *mTimer;

    /**
     * Gets information on a device.
     *
     * @param[in] devName The device name.
     * @return The device info, or default device info if device not found.
     */
    QAudioDeviceInfo getDeviceInfo(const QString &devName);
};
#endif //AUDIODEVICE_H
