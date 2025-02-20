/**
 * Video H264 encoding module.
 *
 * Copyright (C) Sapura Secured Technologies, 2021-2023. All Rights Reserved.
 *
 * @file
 * @version $Id: VideoEncoder.h 1783 2023-11-20 08:54:37Z zulzaidi $
 * @author Zulzaidi Atan
 */
#ifndef VIDEOENCODER_H
#define VIDEOENCODER_H

#include <queue>

#include "Logger.h"
#include "PalLock.h"
#include "PalSem.h"
#include "PalThread.h"

extern "C"
{
#include "libavcodec/avcodec.h"
#include "libswscale/swscale.h"
#include <libavutil/opt.h>
}

#ifndef uchar
typedef unsigned char uchar;
#endif

class VideoEncoder
{
public:
    //callback signature for encoded frame
    typedef void (*CallbackFn)(void *obj, uchar *data, int len, bool marker);

    /**
     * Constructor. Initializes output frame resolution.
     *
     * @param[in] width  The width.
     * @param[in] height The height.
     */
    VideoEncoder(int width, int height);

    ~VideoEncoder();

    bool isValid() { return mIsValid; }

    /**
     * Sets the output frame resolution.
     *
     * @param[in] width  The width.
     * @param[in] height The height.
     */
    void setResolution(int width, int height);

    /**
     * Sets the callback for receiving encoded frames.
     *
     * @param[in] obj The function owner.
     * @param[in] fn  The function.
     */
    void setCallback(void *obj, CallbackFn fn);

    /**
     * Removes a callback function.
     *
     * @param[in] obj The function owner.
     */
    void removeCallback(void *obj);

    /**
     * Converts YUV 4:2:0 semi/fully planar (for MOBILE) or RGBA to YUV frame
     * and queues for encoding.
     *
     * @param[in] data         The image data. For MOBILE, Y plane data.
     * @param[in] dataU        The U plane data or interleaved U and V for
     *                         semi-planar format. Only for MOBILE.
     * @param[in] dataV        The V plane data or 0 for semi planar format.
     *                         Only for MOBILE.
     * @param[in] width        The width.
     * @param[in] height       The height.
     * @param[in] bytesPerLine The number of bytes per line (stride).
     */
    void encode(uchar *data,
#ifdef MOBILE
                uchar *dataU,
                uchar *dataV,
#endif
                int    width,
                int    height,
                int    bytesPerLine);

    /**
     * Continuously encodes frames in the queue, triggering callback if
     * successful.
     */
    void encodeThread();

#ifndef MOBILE
    static void setLogger(Logger *logger) { sLogger = logger; }
#endif

private:
    bool                   mIsValid;
    int                    mState;         //encode thread state
    int                    mPts;           //frame presentation timestamp
    std::queue<AVFrame *>  mQueue;
    PalLock::LockT         mQueueLock;     //guards queue access
    PalSem::SemT           mQueueAddSem;   //signals queue addition
    PalThread::ThreadT     mEncodeThread;
    AVCodecContext        *mCodecCtx;
    AVPacket              *mPacket;
    void                  *mCbObj;         //callback function owner
    CallbackFn             mCbFn;          //callback function

#ifndef MOBILE
    static Logger *sLogger;
#endif

    /**
     * Initializes codec context with the given output frame resolution.
     *
     * @param[in] width  The width.
     * @param[in] height The height.
     * @return true if successful.
     */
    bool initContext(int width, int height);

    /**
     * Packetizes encoded frame data and performs callback if successful.
     *
     * @param[in] data The data.
     * @param[in] len  The length in bytes.
     * @return true if successful.
     */
    void packetize(uchar *data, int len);

    /**
     * Finds the first NAL unit from the given memory address range.
     *
     * @param[in,out] start The start address.
     * @param[in,out] end   The end address.
     * @return true if found with updated start and end addresses.
     */
    bool findFirstNalUnit(uchar **start, uchar **end);
};
#endif //VIDEOENCODER_H
