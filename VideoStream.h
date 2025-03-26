/**
 * Video streaming module.
 *
 * Copyright (C) Sapura Secured Technologies, 2021-2025. All Rights Reserved.
 *
 * @file
 * @version $Id: VideoStream.h 1900 2025-01-23 08:38:48Z zulzaidi $
 * @author Zulzaidi Atan
 */
#ifndef VIDEOSTREAM_H
#define VIDEOSTREAM_H

#include "Logger.h"
#include "RtpSession.h"
#ifdef FFMPEG
#include "VideoDecoder.h"
#include "VideoEncoder.h"
#include "streamer.h"
#endif

#ifndef uchar
typedef unsigned char uchar;
#endif

class VideoStream
{
public:
#ifdef MOBILE
    //callback signature for decoded frame
    typedef void (*DecCbFn)(uchar *data, int w, int h, int bpl);
    //callback signature for stream statistics
    typedef void (*StatCbFn)(int kbps);

    /**
     * Constructor. Starts RTP session and streams.
     *
     * @param[in] lclPort  Local RTP port.
     * @param[in] rmtPort  Remote RTP port.
     * @param[in] lclKey   Local crypto key.
     * @param[in] rmtKey   Remote crypto key.
     * @param[in] decCbFn  Callback function for passing decoded frame.
     * @param[in] statCbFn Callback function for RTP statistics.
     */
    VideoStream(int                lclPort,
                int                rmtPort,
                const std::string &lclKey,
                const std::string &rmtKey,
                DecCbFn            decCbFn,
                StatCbFn           statCbFn);

#else
    //callback signature for decoded frame
    typedef void (*DecCbFn)(void *obj, uchar *data, int w, int h, int bpl);
    //callback signature for stream statistics
    typedef void (*StatCbFn)(void *obj, int kbps);

    /**
     * Constructor. Starts RTP session and streams.
     *
     * @param[in] lclPort  Local RTP port.
     * @param[in] rmtPort  Remote RTP port.
     * @param[in] lclKey   Local crypto key.
     * @param[in] rmtKey   Remote crypto key.
     * @param[in] cbObj    Callback function owner.
     * @param[in] decCbFn  Callback function for passing decoded frame.
     * @param[in] statCbFn Callback function for RTP statistics.
     */
    VideoStream(int                lclPort,
                int                rmtPort,
                const std::string &lclKey,
                const std::string &rmtKey,
                void              *cbObj,
                DecCbFn            decCbFn,
                StatCbFn           statCbFn);
#endif //MOBILE

    ~VideoStream();

    /**
     * Starts or stops outgoing video stream.
     * Sets the output frame resolution when starting.
     *
     * @param[in] doStart true to start.
     * @param[in] width   The width. Omit when stopping.
     * @param[in] height  The height. Omit when stopping.
     */
    void setOutgoing(bool doStart, int width = 0, int height = 0);

    /**
     * Sets the logger. Must be done before instantiating a class object.
     *
     * @param[in] logger The logger object.
     */
    static void setLogger(Logger *logger);

    /**
     * Encodes YUV 4:2:0 semi/fully planar (for MOBILE) or RGBA image and sends
     * to recipient.
     *
     * @param[in] data   The image data. For MOBILE, Y plane data.
     * @param[in] dataU  The U plane data or interleaved U and V for semi-planar
     *                   format. Only for MOBILE.
     * @param[in] dataV  The V plane data or 0 for semi planar format. Only for
     *                   MOBILE.
     * @param[in] width  The width.
     * @param[in] height The height.
     * @param[in] bpl    The number of bytes per line (stride).
     */
    static void send(uchar *data,
#ifdef MOBILE
                     uchar *dataU,
                     uchar *dataV,
#endif
                     int    width,
                     int    height,
                     int    bpl);

    /**
     * Stops and releases outgoing video stream resources.
     */
    static void stop();

    /**
     * Callback function for received RTP payload.
     *
     * @param[in] obj  VideoStream object, owner of the callback function.
     * @param[in] rtp  The RTP session.
     * @param[in] data The RTP payload.
     * @param[in] len  The payload length in bytes.
     */
    static void rtpRcvCb(void *obj, RtpSession *rtp, char *data, int len);

    /**
     * Callback function for RTP statistics.
     *
     * @param[in] obj  VideoStream object, owner of the callback function.
     * @param[in] Unused callback parameter.
     * @param[in] kbps RTP receive rate in kbps.
     */
    static void rtpStatCb(void *obj, RtpSession *, int kbps);

    /**
     * Callback function for encoded frame.
     *
     * @param[in] obj    VideoStream object, owner of the callback function.
     * @param[in] data   The encoded frame.
     * @param[in] len    The length in bytes.
     * @param[in] marker true for last packet of the frame.
     */
    static void encodeCb(void *obj, uchar *data, int len, bool marker);

private:
    RtpSession *mRtp;
    void       *mObj;       //callback function owner
    StatCbFn    mStatCbFn;  //stream statistics callback function
#ifdef FFMPEG
    VideoDecoder *mDec;
    static VideoEncoder *sEnc;
    Streamer      streamer;
#endif
    static Logger *sLogger;

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
     * @param[in] kbps Receive rate in kbps.
     */
    void rtpStat(int kbps);

    /**
     * Sends encoded frame data.
     *
     * @param[in] data   The data.
     * @param[in] len    The length in bytes.
     * @param[in] marker true for last packet of the frame.
     */
    void sendPacket(uchar *data, int len, bool marker);
};
#endif //VIDEOSTREAM_H
