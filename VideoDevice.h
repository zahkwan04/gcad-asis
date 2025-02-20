/**
 * A singleton class for interfacing with system camera device and capturing
 * video frames.
 *
 * Copyright (C) Sapura Secured Technologies, 2021. All Rights Reserved.
 *
 * @file
 * @version $Id: VideoDevice.h 1552 2021-09-22 04:51:57Z zulzaidi $
 * @author Zulzaidi Atan
 */
#ifndef VIDEODEVICE_H
#define VIDEODEVICE_H

#include <QAbstractVideoSurface>
#include <QCamera>
#include <QObject>

#include "PalLock.h"

class VideoDevice : public QAbstractVideoSurface
{
    Q_OBJECT

public:
    //callback signature for video preview input frame
    typedef void (*CallbackFn)(void  *obj, QPixmap img);

    /**
     * Instantiates the singleton if it has not been created.
     */
    static VideoDevice &instance();

    /**
     * Deletes the single instance.
     */
    static void destroy();

    bool isValid() { return mIsValid; }

    bool isBusy() { return mCamera->state() == QCamera::ActiveState; }

    /**
     * Checks whether device signal has been connected.
     *
     * @return true if signal connected.
     */
    bool isDevSignalConnected();

    /**
     * Gets a list of camera device names on the system.
     *
     * @return The string list.
     */
    QStringList getDeviceNames();

    /**
     * Sets and starts a device.
     *
     * @param[in] devName The device name.
     */
    void setDevice(const QString &devName);

    /**
     * Gets a list of supported resolutions for the loaded device.
     *
     * @return The string list. Entry format: "<width>x<height>"
     */
    QStringList getResolutionList();

    /**
     * Gets the resolution of the loaded device.
     *
     * @return The resolution size or empty on failure.
     */
    QSize getResolution();

    /**
     * Sets the resolution of the loaded device.
     *
     * @param[in] res The resolution. Format: "<width>x<height>".
     */
    void setResolution(const QString &res);

    /**
     * Starts or stops camera.
     * Only stops if the calling object is the camera owner.
     *
     * @param[in] obj     The calling object.
     * @param[in] doStart true to start.
     */
    void setCamera(void *obj, bool doStart);

    /**
     * Sets the callback for receiving video preview frames.
     *
     * @param[in] obj The function owner.
     * @param[in] fn  The function.
     */
    void setCallback(void *obj, CallbackFn fn);

    /**
     * Removes a callback function.
     *
     * @param[in] obj The function owner.
     * @return true if removed.
     */
    bool removeCallback(void *obj);

    //overrides
    QList<QVideoFrame::PixelFormat> supportedPixelFormats(
                             QAbstractVideoBuffer::HandleType handleType =
                                 QAbstractVideoBuffer::NoHandle) const override;

    bool present(const QVideoFrame &frame) override;

signals:
    void newFrame(uchar *data, int width, int height, int bpl);

private:
    bool        mIsValid;
    QCamera    *mCamera;
    void       *mCbObj;    //callback function owner
    CallbackFn  mCbFn;     //callback function

    static bool            sIsCreated;
    static void           *sCamOwner;
    static VideoDevice    *sInstance;       //single class instance
    static PalLock::LockT  sSingletonLock;

    /**
     * Constructor is private to prevent direct instantiation.
     *
     * @param[in] parent Parent object, if any.
     */
    explicit VideoDevice(QObject *parent = 0);

    ~VideoDevice();

    /**
     * Gets information on a device.
     *
     * @param[in] devName The device name.
     * @return The device info, or default device info if specified device
     *         not found.
     */
    QCameraInfo getDeviceInfo(const QString &desc);

    /**
     * Converts a resolution string to QSize.
     *
     * @param[in] res The string. Format: "<width>x<height>".
     * @return The resolution size or empty on failure.
     */
    QSize convertResolution(const QString &res);
};
#endif //VIDEODEVICE_H
