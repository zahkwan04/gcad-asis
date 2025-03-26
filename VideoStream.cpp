/**
 * Video streaming implementation.
 *
 * Copyright (C) Sapura Secured Technologies, 2021-2025. All Rights Reserved.
 *
 * @file
 * @version $Id: VideoStream.cpp 1900 2025-01-23 08:38:48Z zulzaidi $
 * @author Zulzaidi Atan
 */
#include <assert.h>

#include "VideoStream.h"
#include <QtConcurrent/QtConcurrent>


using namespace std;

static const string LOGPREFIX("VideoStream:: ");

#ifdef FFMPEG
VideoEncoder *VideoStream::sEnc(0);
#endif

Logger *VideoStream::sLogger(0);
#ifdef MOBILE
VideoStream::VideoStream(int           lclPort,
                         int           rmtPort,
                         const string &lclKey,
                         const string &rmtKey,
                         DecCbFn       decCbFn,
                         StatCbFn      statCbFn) :
mStatCbFn(statCbFn)
{
    if (sLogger == 0)
    {
        assert("Bad param in VideoStream::VideoStream" == 0);
        return;
    }
#ifdef FFMPEG
    mDec = new VideoDecoder(decCbFn);
#endif
    mRtp = new RtpSession(RtpSession::TYPE_VIDEO, lclPort, rmtPort, sLogger,
                          this, rtpRcvCb, rtpStatCb);
    mRtp->setCryptoKey(lclKey, rmtKey);
    //send dummy to kick start stream
    char dummy[1] = {0};
    mRtp->send(dummy, 1);
}

#else //MOBILE
VideoStream::VideoStream(int           lclPort,
                         int           rmtPort,
                         const string &lclKey,
                         const string &rmtKey,
                         void         *cbObj,
                         DecCbFn       decCbFn,
                         StatCbFn      statCbFn) :
mObj(cbObj), mStatCbFn(statCbFn)
{
    if (sLogger == 0 || cbObj == 0)
    {
        assert("Bad param in VideoStream::VideoStream" == 0);
        return;
    }
    LOGGER_DEBUG(sLogger, LOGPREFIX << "VideoStream: " << lclPort << " "
                 << rmtPort);
#ifdef FFMPEG
    mDec = new VideoDecoder(cbObj, decCbFn);

    streamer.startStreaming();
#endif
    mRtp = new RtpSession(RtpSession::TYPE_VIDEO, lclPort, rmtPort, sLogger,
                          this, rtpRcvCb, rtpStatCb);
    mRtp->setCryptoKey(lclKey, rmtKey);
    //send dummy to kick start stream
    char dummy[1] = {0};
    mRtp->send(dummy, 1);
}
#endif //MOBILE

VideoStream::~VideoStream()
{
    if (mRtp != 0)
    {
        mRtp->stop();
        delete mRtp;
    }
#ifdef FFMPEG
    streamer.stopStreaming();
    delete mDec;
#endif
}

void VideoStream::setOutgoing(bool doStart, int width, int height)
{
#ifdef FFMPEG
    if (doStart)
    {
        if (sEnc == 0)
            sEnc = new VideoEncoder(width, height);
        else
            sEnc->setResolution(width, height);
        sEnc->setCallback(this, encodeCb);
    }
    else if (sEnc != 0)
    {
        sEnc->removeCallback(this);
    }
#endif
}

void VideoStream::setLogger(Logger *logger)
{
    sLogger = logger;
#ifdef FFMPEG
    VideoDecoder::setLogger(logger);
    VideoEncoder::setLogger(logger);
#endif
}

void VideoStream::send(uchar *data,
#ifdef MOBILE
                       uchar *dataU,
                       uchar *dataV,
#endif
                       int    width,
                       int    height,
                       int    bpl)
{
#ifdef FFMPEG
    if (sEnc != 0)
        sEnc->encode(data,
#ifdef MOBILE
                     dataU, dataV,
#endif
                     width, height, bpl);
#endif //FFMPEG
}

void VideoStream::stop()
{
#ifdef FFMPEG
    if (sEnc != 0)
    {
        delete sEnc;
        sEnc = 0;
    }
#endif
}

void VideoStream::rtpRcvCb(void *obj, RtpSession *rtp, char *data, int len)
{
    static_cast<VideoStream *>(obj)->rtpReceived(rtp, data, len);

}

void VideoStream::rtpStatCb(void *obj, RtpSession *, int kbps)
{
    static_cast<VideoStream *>(obj)->rtpStat(kbps);
}

void VideoStream::encodeCb(void *obj, uchar *data, int len, bool marker)
{
    static_cast<VideoStream *>(obj)->sendPacket(data, len, marker);
}

void VideoStream::rtpReceived(RtpSession *rtp, char *data, int len)
{
#ifdef FFMPEG
    mDec->decode(data, len);
    qDebug() << "data" << data;
    // Simulated video frame (dummy YUV420p frame)
//    QByteArray sampleFrame(1920 * 1080 * 3 / 2, 127);
//    streamer.sendFrameData(sampleFrame);
//    QThread::msleep(33);  // Simulating 30 FPS

//    while (true)
//    {
//        streamer.sendFrameData(sampleFrame);
//        QThread::msleep(33);  // Simulating 30 FPS
//    }

#endif
}

//void VideoStream::rtpReceived(RtpSession *rtp, char *data, int len)
//{
//#ifdef FFMPEG
//    // Run decoding in a separate thread
//    QtConcurrent::run([this, data, len]() {
//        mDec->decode(data, len);
//    });

//    // Run video streaming loop in another thread
//    QtConcurrent::run([this]() {
//        QByteArray sampleFrame(1920 * 1080 * 3 / 2, 127);

//        while (true)
//        {
//            streamer.sendFrameData(sampleFrame);
//            QThread::msleep(33);  // Simulating 30 FPS
//        }
//    });
//#endif
//}

void VideoStream::rtpStat(int kbps)
{
    if (mStatCbFn != 0)
        mStatCbFn(mObj, kbps);
}

void VideoStream::sendPacket(uchar *data, int len, bool marker)
{
    if (mRtp != 0 && data != 0)
        mRtp->send((char *) data, len, marker);
}
