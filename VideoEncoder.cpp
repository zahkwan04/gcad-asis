/**
 * Video H264 encoding implementation.
 *
 * Copyright (C) Sapura Secured Technologies, 2021-2023. All Rights Reserved.
 *
 * @file
 * @version $Id: VideoEncoder.cpp 1856 2024-05-28 05:57:52Z zulzaidi $
 * @author Zulzaidi Atan
 */
#include <assert.h>

#include "Locker.h"
#include "VideoEncoder.h"

using namespace std;

//thread state
enum eState
{
    STATE_RUN,
    STATE_STOP,
    STATE_END
};

//based on MTU 1300++ bytes, reserve 100++ bytes for overheads
static const int MAX_BUFFER_LEN = 1200;
#ifndef MOBILE
static const string LOGPREFIX("VideoEncoder:: ");

Logger *VideoEncoder::sLogger(0);
#endif

static void *startEncodeThread(void *obj)
{
    static_cast<VideoEncoder *>(obj)->encodeThread();
    return 0;
}

VideoEncoder::VideoEncoder(int width, int height) :
mIsValid(false), mState(STATE_END), mPts(0), mEncodeThread(0), mCodecCtx(0),
mPacket(0), mCbObj(0), mCbFn(0)
{
    PalLock::init(&mQueueLock);
    PalSem::init(&mQueueAddSem);
#ifndef MOBILE
    if (sLogger == 0)
    {
        assert("Missing logger in VideoEncoder" == 0);
        return;
    }
#endif
    if (!initContext(width, height))
        return;
    mPacket = av_packet_alloc();
    if (mPacket == 0)
    {
        LOGGER_ERROR(sLogger, LOGPREFIX <<
                     "VideoEncoder: av_packet_alloc failure.");
        return;
    }
    mIsValid = true;
    PalThread::start(&mEncodeThread, startEncodeThread, this);
}

VideoEncoder::~VideoEncoder()
{
    mState = STATE_STOP;
    PalLock::take(&mQueueLock);
    while (!mQueue.empty())
    {
        av_frame_free(&mQueue.front());
        mQueue.pop();
    }
    PalSem::post(&mQueueAddSem);
    PalLock::release(&mQueueLock);
    if (mEncodeThread != 0)
    {
        PalThread::stop(mEncodeThread);
        while (mState != STATE_END) //wait for thread to end
        {
            PalThread::msleep(10);
        }
    }
    PalLock::destroy(&mQueueLock);
    PalSem::destroy(&mQueueAddSem);
    avcodec_free_context(&mCodecCtx);
    av_packet_free(&mPacket);
}

void VideoEncoder::setResolution(int width, int height)
{
    if (!mIsValid || (mCodecCtx->width == width && mCodecCtx->height == height))
        return;
    Locker lock(&mQueueLock);
    avcodec_free_context(&mCodecCtx);
    mIsValid = initContext(width, height);
}

void VideoEncoder::setCallback(void *obj, CallbackFn fn)
{
    Locker lock(&mQueueLock);
    mCbObj = obj;
    mCbFn = fn;
}

void VideoEncoder::removeCallback(void *obj)
{
    Locker lock(&mQueueLock);
    if (mCbObj == obj)
    {
        mCbObj = 0;
        mCbFn = 0;
    }
}

void VideoEncoder::encode(uchar *data,
#ifdef MOBILE
                          uchar *dataU,
                          uchar *dataV,
#endif
                          int    width,
                          int    height,
                          int    bytesPerLine)
{
    if (!mIsValid || data == 0)
    {
         assert("Bad param in VideoEncoder::encode" == 0);
         return;
    }
    if (mCbObj == 0)
        return; //output stream has stopped
    AVFrame *yuvFrame = av_frame_alloc();
    if (yuvFrame == 0)
    {
        LOGGER_ERROR(sLogger, LOGPREFIX << "encode: av_frame_alloc failure.");
        return;
    }
    yuvFrame->format = mCodecCtx->pix_fmt;
    yuvFrame->width  = mCodecCtx->width;
    yuvFrame->height = mCodecCtx->height;
    if (av_frame_get_buffer(yuvFrame, 0) < 0)
    {
        LOGGER_ERROR(sLogger, LOGPREFIX <<
                     "encode: av_frame_get_buffer failure.");
        return;
    }
#ifdef MOBILE
    AVPixelFormat format = (dataV == 0)? AV_PIX_FMT_NV12: AV_PIX_FMT_YUV420P;
    uint8_t *srcPlanes[3] = {(uint8_t *) data, (uint8_t *) dataU,
                             (uint8_t *) dataV};
    int srcStride[3] = {bytesPerLine,
                        (dataV == 0)? bytesPerLine: bytesPerLine/2,
                        (dataV == 0)? 0: bytesPerLine/2};
#else
    AVPixelFormat format = AV_PIX_FMT_BGRA;
    uint8_t *srcPlanes[3] = {(uint8_t *) data, 0, 0};
    int srcStride[3] = {bytesPerLine, 0, 0};
#endif //MOBILE
    SwsContext *swsCtx = sws_getContext(width, height,
                                        format, mCodecCtx->width,
                                        mCodecCtx->height, AV_PIX_FMT_YUV420P,
                                        SWS_BICUBIC, NULL, NULL, NULL);
    sws_scale(swsCtx, srcPlanes, srcStride, 0, height, yuvFrame->data,
              yuvFrame->linesize);
    sws_freeContext(swsCtx);
    yuvFrame->pts = mPts++;
    PalLock::take(&mQueueLock);
    mQueue.push(yuvFrame);
    PalSem::post(&mQueueAddSem);
    PalLock::release(&mQueueLock);
}

void VideoEncoder::encodeThread()
{
    if (!mIsValid)
        return;
    mState = STATE_RUN;
    AVFrame *frame;
    int ret;
    while (mState == STATE_RUN)
    {
        if (!PalSem::wait(&mQueueAddSem))
            continue;
        PalLock::take(&mQueueLock);
        if (mQueue.empty())
        {
            //semaphore posted with empty queue only at shutdown
            PalLock::release(&mQueueLock);
            break;
        }
        frame = mQueue.front();
        mQueue.pop();
        if (mCbObj == 0)
        {
            //no output stream - discard
            av_frame_free(&frame);
            PalLock::release(&mQueueLock);
            continue;
        }
        ret = avcodec_send_frame(mCodecCtx, frame);
        while (ret >= 0)
        {
            ret = avcodec_receive_packet(mCodecCtx, mPacket);
            if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
                break;
            if (ret < 0)
            {
                LOGGER_ERROR(sLogger, LOGPREFIX
                             << "encodeThread: avcodec_receive_packet failure "
                             << ret);
                break;
            }
            packetize(mPacket->data, mPacket->size);
            av_packet_unref(mPacket);
        }
        av_frame_free(&frame);
        PalLock::release(&mQueueLock);
    }
    mState = STATE_END;
}

bool VideoEncoder::initContext(int width, int height)
{
    static const AVCodec *codec = avcodec_find_encoder(AV_CODEC_ID_H264);
    if (codec == 0)
    {
        LOGGER_ERROR(sLogger, LOGPREFIX <<
                     "initContext: avcodec_find_encoder failure.");
        return false;
    }
    mCodecCtx = avcodec_alloc_context3(codec);
    if (mCodecCtx == 0)
    {
        LOGGER_ERROR(sLogger, LOGPREFIX <<
                     "initContext: avcodec_alloc_context3 failure.");
        return false;
    }
    mCodecCtx->profile = FF_PROFILE_H264_BASELINE;
    mCodecCtx->bit_rate = 800000;  //800kbps
    mCodecCtx->width = width;
    mCodecCtx->height = height;
    mCodecCtx->time_base.den = 30; //30 frames per second
    mCodecCtx->time_base.num = 1;
    mCodecCtx->gop_size = 5;       //group of pictures size
    mCodecCtx->max_b_frames = 1;   //1 intra frame every 5 frames
    mCodecCtx->pix_fmt = AV_PIX_FMT_YUV420P;
    av_opt_set_int(mCodecCtx->priv_data, "slice-max-size", MAX_BUFFER_LEN, 0);
    av_opt_set(mCodecCtx->priv_data, "tune", "zerolatency", 0);
    int ret = avcodec_open2(mCodecCtx, codec, NULL);
    if (ret < 0)
    {
        LOGGER_ERROR(sLogger, LOGPREFIX <<
                     "initContext: avcodec_open2 failure: " << ret);
        return false;
    }
    return true;
}

void VideoEncoder::packetize(uchar *data, int len)
{
    assert(data != 0);
    uchar *start = data;
    uchar *end = data + len - 1;
    int l;
    //data may contain multiple NAL units, iterate each unit
    while (findFirstNalUnit(&start, &end))
    {
        l = end - start + 1;
        if (l > MAX_BUFFER_LEN)
        {
            //fragmentation not supported
            LOGGER_WARNING(sLogger, LOGPREFIX
                           << "packetize: Input length exceeds max length.");
            break;
        }
        if (end + 1 > data + len - 1)
        {
            mCbFn(mCbObj, start, l, true); //last packet - mark end of frame
            break;
        }
        mCbFn(mCbObj, start, l, false);
        start = end + 1;
        end = data + len - 1;
    }
}

bool VideoEncoder::findFirstNalUnit(uchar **start, uchar **end)
{
    assert(start != 0 && end != 0);
    uchar *p = *start;
    //lookup NAL start code "0x00 0x00 0x01" pattern
    while (p <= *end - 3 && (*p != 0 || *(p + 1) != 0 || *(p + 2) != 1))
    {
        ++p;
    }
    if (p > *end - 3)
        return false; //not found
    p += 3;
    *start = p;
    while (p <= *end - 3 && (*p != 0 || *(p + 1) != 0 || *(p + 2) != 1))
    {
        ++p;
    }
    if (p <= *end - 3)
    {
        //found next NAL unit start code
        *end = p - 1;
        //start code can also be "0x00 0x00 0x00 0x01"
        if (**end == 0)
            *end -= 1;
    }
    return true;
}
