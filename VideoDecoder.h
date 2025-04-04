/**
 * Video H264 decoding module.
 *
 * Copyright (C) Sapura Secured Technologies, 2021. All Rights Reserved.
 *
 * @file
 * @version $Id: VideoDecoder.h 1562 2021-10-21 04:14:36Z zulzaidi $
 * @author Zulzaidi Atan
 */
#ifndef VIDEODECODER_H
#define VIDEODECODER_H

#include "Logger.h"
#include "RtspStreamer.h"

extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavutil/imgutils.h"
#include "libswscale/swscale.h"
}

#ifndef uchar
typedef unsigned char uchar;
#endif

class VideoDecoder
{
public:
#ifdef MOBILE
    //callback signature for decoded frame
    typedef void (*CallbackFn)(uchar *data, int w, int h, int bpl);

    /**
     * Constructor.
     *
     * @param[in] cbFn Callback function for passing decoded frame.
     */
    VideoDecoder(CallbackFn cbFn);

#else
    //callback signature for decoded frame
    typedef void (*CallbackFn)(void *obj, uchar *data, int w, int h, int bpl);

    /**
     * Constructor.
     *
     * @param[in] cbObj Callback function owner.
     * @param[in] cbFn  Callback function for passing decoded frame.
     */
    VideoDecoder(void *cbObj, CallbackFn cbFn);
#endif //MOBILE

    ~VideoDecoder();

    bool isValid() { return mIsValid; }

    /**
     * Decodes H264 RTP payload.
     *
     * @param[in] data The payload.
     * @param[in] len  The payload length in bytes.
     */
    void decode(char *data, int len);

#ifndef MOBILE
    static void setLogger(Logger *logger) { sLogger = logger; }
#endif

private:
    //based on MTU 1500 bytes, this should be enough because actual RTP payload
    //size would be way less, and the remaining could be used for Network
    //Abstraction Layer (NAL) header
    static const int MAX_BUFFER_LEN = 1500;

    bool                  mIsValid;
    uchar                 mBuffer[MAX_BUFFER_LEN];
    CallbackFn            mCbFn;      //callback function
    void                 *mCbObj;     //callback function owner
    AVCodecContext       *mCodecCtx;
    AVPacket             *mPacket;
    AVCodecParserContext *mParser;
    AVFrame              *mFrameYuv;
    AVFrame              *mFrameRgb;
    RtspStreamer          streamer;

#ifndef MOBILE
    static Logger *sLogger;
#endif

    /**
     * Unpacketizes RTP payload to mBuffer by adding H264 header.
     *
     * @param[in] data The payload.
     * @param[in] len  The payload length in bytes.
     * @return The length of written data. 0 on failure.
     */
    int unpacketize(char *data, int len);

    /**
     * Gets a decoded frame and calls callback function if frame is available.
     */
    void getDecodedFrame();

    /**
     * @brief Creates a QByteArray containing YUV data from an AVFrame
     * @param[in] frame  The source AVFrame containing YUV data.
     * @param[in] width  The width of the frame in pixels.
     * @param[in] height The height of the frame in pixels.
     * @return QByteArray containing packed YUV data with planes arranged consecutively.
     */
    QByteArray createYuvQByteArray(const AVFrame* frame, int width, int height);
};
#endif //VIDEODECODER_H
