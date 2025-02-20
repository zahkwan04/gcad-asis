/**
 * Video device implementation.
 *
 * Copyright (C) Sapura Secured Technologies, 2021. All Rights Reserved.
 *
 * @file
 * @version $Id: VideoDevice.cpp 1552 2021-09-22 04:51:57Z zulzaidi $
 * @author Zulzaidi Atan
 */
#include <QCameraInfo>
#include <QPixmap>

#include "Settings.h"
#include "VideoDevice.h"

using namespace std;

//video preview resolution - height is automatically calculated to preserve the
//aspect ratio
static const int     PREVIEW_WIDTH = 200;
static const int     MAX_WIDTH     = 640; //maximum resolution width
static const QString DEFAULT_RES("640x480");

bool         VideoDevice::sIsCreated(false);
void        *VideoDevice::sCamOwner(0);
VideoDevice *VideoDevice::sInstance(0);

#ifdef _WIN32
PalLock::LockT VideoDevice::sSingletonLock; //no init needed
#elif defined QT_CORE_LIB
PalLock::LockT VideoDevice::sSingletonLock(QMutex::Recursive);
#else
PalLock::LockT VideoDevice::sSingletonLock = PTHREAD_MUTEX_INITIALIZER;
#endif

VideoDevice &VideoDevice::instance()
{
    //a modified Double Checked Locking Pattern for thread-safe Singleton,
    //lock obtained only before/during creation
    if (!sIsCreated)
    {
        PalLock::take(&sSingletonLock);
        if (!sIsCreated) //thread-safety double check
        {
            sInstance = new VideoDevice();
            //ensure the compiler cannot reorder the statements and that the
            //flag is set true only after creation
            if (sInstance != 0)
                sIsCreated = true;
        }
        PalLock::release(&sSingletonLock);
    }
    return *sInstance;
}

void VideoDevice::destroy()
{
    PalLock::take(&sSingletonLock);
    if (sIsCreated)
    {
        sIsCreated = false;
        sInstance->deleteLater();
        sInstance = 0;
    }
    PalLock::release(&sSingletonLock);
}

bool VideoDevice::isDevSignalConnected()
{
    return isSignalConnected(QMetaMethod::fromSignal(&VideoDevice::newFrame));
}

QStringList VideoDevice::getDeviceNames()
{
    QStringList lst;
    foreach (const QCameraInfo &info, QCameraInfo::availableCameras())
    {
        lst.append(info.description());
    }
    return lst;
}

void VideoDevice::setDevice(const QString &devName)
{
    if (!mIsValid)
        return;
    delete mCamera;
    mCamera = new QCamera(getDeviceInfo(devName));
    mCamera->setViewfinder(this);
    mCamera->start();
}

QStringList VideoDevice::getResolutionList()
{
    QStringList lst;
    if (mCamera == 0)
         return lst;
    foreach (const QSize &s, mCamera->supportedViewfinderResolutions())
    {
        if (s.width() <= MAX_WIDTH)
            lst.append(QString("%1x%2").arg(s.width()).arg(s.height()));
    }
    return lst;
}

QSize VideoDevice::getResolution()
{
    if (!mIsValid)
        return QSize(0, 0);
    return mCamera->viewfinderSettings().resolution();
}

void VideoDevice::setResolution(const QString &res)
{
    if (!mIsValid)
        return;
    int state = mCamera->state();
    if (state == QCamera::ActiveState)
        mCamera->stop();
    QCameraViewfinderSettings v(mCamera->viewfinderSettings());
    //if convertResolution() returns empty, the backend makes an optimal choice
    //based on the supported resolutions
    v.setResolution(convertResolution(res));
    mCamera->setViewfinderSettings(v);
    if (state == QCamera::ActiveState)
        mCamera->start();
}

void VideoDevice::setCamera(void *obj, bool doStart)
{
    if (!mIsValid)
        return;
    if (doStart)
    {
        if (sCamOwner == 0)
            mCamera->start();
        sCamOwner = obj;
    }
    else if (sCamOwner == obj)
    {
        sCamOwner = 0;
        mCamera->stop();
    }
}

void VideoDevice::setCallback(void *obj, CallbackFn fn)
{
    mCbObj = obj;
    mCbFn = fn;
}

bool VideoDevice::removeCallback(void *obj)
{
    if (mCbObj != obj)
        return false;
    mCbObj = 0;
    mCbFn = 0;
    return true;
}

VideoDevice::VideoDevice(QObject *parent) :
QAbstractVideoSurface(parent), mIsValid(false), mCamera(0), mCbObj(0), mCbFn(0)
{
    if (QCameraInfo::availableCameras().size() == 0)
        return;
    mCamera = new QCamera(getDeviceInfo(QString::fromStdString(
                     Settings::instance().get<string>(Props::FLD_CFG_CAMERA))));
    mCamera->setViewfinder(this);
    mCamera->load();
    QStringList resList(getResolutionList());
    if (resList.isEmpty())
        return;
    QString s(QString::fromStdString(
                  Settings::instance().get<string>(Props::FLD_CFG_CAMERA_RES)));
    if (!resList.contains(s))
    {
        s = (resList.contains(DEFAULT_RES))? DEFAULT_RES: resList.at(0);
        Settings::instance().set(Props::FLD_CFG_CAMERA_RES, s.toStdString());
    }
    QCameraViewfinderSettings v(mCamera->viewfinderSettings());
    v.setResolution(convertResolution(s));
    mCamera->setViewfinderSettings(v);
    mIsValid = true;
}

VideoDevice::~VideoDevice()
{
    delete mCamera;
}

QList<QVideoFrame::PixelFormat> VideoDevice::supportedPixelFormats(
    QAbstractVideoBuffer::HandleType handleType) const
{
    return QList<QVideoFrame::PixelFormat>({QVideoFrame::Format_RGB32});
}

bool VideoDevice::present(const QVideoFrame &frame)
{
    if (!frame.isValid())
        return false;
    QVideoFrame f(frame);
    f.map(QAbstractVideoBuffer::ReadOnly);
    QImage img(QImage(f.bits(), f.width(), f.height(),
                      QVideoFrame::imageFormatFromPixelFormat(f.pixelFormat()))
                   .mirrored(false, true));
    emit newFrame(img.bits(), img.width(), img.height(), img.bytesPerLine());
    if (mCbObj != 0)
        mCbFn(mCbObj,
              QPixmap::fromImage(img.mirrored(true, false))
                  .scaledToWidth(PREVIEW_WIDTH, Qt::SmoothTransformation));
    f.unmap();
    return true;
}

QCameraInfo VideoDevice::getDeviceInfo(const QString &desc)
{
    foreach (const QCameraInfo &info, QCameraInfo::availableCameras())
    {
        if (desc == info.description())
            return info;
    }
    return (QCameraInfo::defaultCamera());
}

QSize VideoDevice::convertResolution(const QString &res)
{
    QStringList l(res.split('x'));
    if (l.length() != 2)
        return QSize(0, 0);
    return QSize(l.at(0).toInt(), l.at(1).toInt());
}
