/**
 * Video H264 decoding implementation.
 *
 * Copyright (C) Sapura Secured Technologies, 2021. All Rights Reserved.
 *
 * @file
 * @version $Id: VideoDecoder.cpp 1562 2021-10-21 04:14:36Z zulzaidi $
 * @author Zulzaidi Atan
 */
#include <assert.h>

#include "VideoDecoder.h"
#include "qdebug.h"

using namespace std;

enum eNalType
{
    NAL_TYPE_SINGLE_MIN = 1,
    NAL_TYPE_SINGLE_MAX = 23,
    NAL_TYPE_STAP_A     = 24,
    NAL_TYPE_FU_A       = 28
};

static const char NAL_START_CODE[3] = {0, 0, 1};
#ifdef MOBILE
static const AVPixelFormat OUTPUT_FORMAT = AV_PIX_FMT_RGBA;

VideoDecoder::VideoDecoder(CallbackFn cbFn) :
mIsValid(false), mCbFn(cbFn), mCodecCtx(0), mPacket(0), mParser(0),
mFrameYuv(0), mFrameRgb(0)
{

#else //MOBILE
static const AVPixelFormat OUTPUT_FORMAT = AV_PIX_FMT_RGB24;
static const string        LOGPREFIX("VideoDecoder:: ");

Logger *VideoDecoder::sLogger(0);

VideoDecoder::VideoDecoder(void *cbObj, CallbackFn cbFn) :
mIsValid(false), mCbFn(cbFn), mCbObj(cbObj), mCodecCtx(0), mPacket(0),
mParser(0), mFrameYuv(0), mFrameRgb(0)
{
    streamer.startStreaming();
    if (sLogger == 0 || cbObj == 0)
    {
        assert("Bad param in VideoDecoder::VideoDecoder" == 0);
        return;
    }
#endif //MOBILE
    const AVCodec *codec = avcodec_find_decoder(AV_CODEC_ID_H264);
    if (codec == 0)
    {
        LOGGER_ERROR(sLogger, LOGPREFIX
                     << "VideoDecoder: avcodec_find_decoder failure.");
        return;
    }
    mCodecCtx = avcodec_alloc_context3(codec);
    if (mCodecCtx == 0)
    {
        LOGGER_ERROR(sLogger, LOGPREFIX
                     << "VideoDecoder: avcodec_alloc_context3 failure.");
        return;
    }
    if (avcodec_open2(mCodecCtx, codec, NULL) < 0)
    {
        LOGGER_ERROR(sLogger, LOGPREFIX
                     << "VideoDecoder: avcodec_open2 failure.");
        return;
    }
    mCodecCtx->codec_type = AVMEDIA_TYPE_VIDEO;
    mCodecCtx->pix_fmt = AV_PIX_FMT_YUV420P;
    mPacket = av_packet_alloc();
    if (mPacket == 0)
    {
        LOGGER_ERROR(sLogger, LOGPREFIX
                     << "VideoDecoder: av_packet_alloc failure.");
        return;
    }
    mParser = av_parser_init(codec->id);
    if (mParser == 0)
    {
        LOGGER_ERROR(sLogger, LOGPREFIX
                     << "VideoDecoder: av_parser_init failure.");
        return;
    }
    mFrameYuv = av_frame_alloc();
    if (mFrameYuv == 0)
    {
        LOGGER_ERROR(sLogger, LOGPREFIX
                     << "VideoDecoder: av_frame_alloc failure.");
        return;
    }
    mFrameRgb = av_frame_alloc();
    if (mFrameRgb == 0)
    {
        LOGGER_ERROR(sLogger, LOGPREFIX
                     << "VideoDecoder: av_frame_alloc failure.");
        return;
    }
    mIsValid = true;
}

VideoDecoder::~VideoDecoder()
{
    avcodec_free_context(&mCodecCtx);
    if (mParser != 0)
        av_parser_close(mParser);
    av_packet_free(&mPacket);
    av_frame_free(&mFrameRgb);
    av_frame_free(&mFrameYuv);
    streamer.stopStreaming();
}

void VideoDecoder::decode(char *data, int len)
{
    if (!mIsValid || data == 0 || len <= 0)
        return;
    int bufLen = unpacketize(data, len);
    if (bufLen == 0)
    {
        LOGGER_ERROR(sLogger, LOGPREFIX
                     << "decode: Fail to unpacketize payload data.");
        return;
    }
    uchar *p = mBuffer;
    while (bufLen > 0)
    {
        int ret = av_parser_parse2(mParser, mCodecCtx, &mPacket->data,
                                   &mPacket->size, p, bufLen, AV_NOPTS_VALUE,
                                   AV_NOPTS_VALUE, 0);
        if (ret < 0)
        {
            LOGGER_ERROR(sLogger, LOGPREFIX <<
                         "decode: av_parser_parse2 failure " << ret);
            return;
        }
        p += ret;
        bufLen -= ret;
        if (mPacket->size > 0)
        {
            getDecodedFrame();
            av_packet_unref(mPacket);
        }
    }
}

#define COPY(data, len) \
    do \
    { \
        if (bufLen + len > MAX_BUFFER_LEN) \
            return 0; \
        memcpy(mBuffer + bufLen, data, len); \
        bufLen += len; \
    } \
    while (0)

int VideoDecoder::unpacketize(char *data, int len)
{
    assert(data != 0);
    int bufLen = 0;
    int nalType = *data & 0x1F; //type stored in 5 low-order bits
    switch (nalType)
    {
        case NAL_TYPE_FU_A:
        {
            //fragmentation unit starts with 2 bytes header:
            //   FU indicator        FU header
            // +---------------+ +---------------+
            // |0|1|2|3|4|5|6|7| |0|1|2|3|4|5|6|7|
            // +-+-+-+-+-+-+-+-+ +-+-+-+-+-+-+-+-+
            // |F|NRI|  Type   | |S|E|R|  Type   |
            // +---------------+ +---------------+
            if ((*(data + 1) & 0x80) == 128)
            {
                //start bit S is set - this is the first fragment
                COPY(NAL_START_CODE, sizeof(NAL_START_CODE));
                //construct new header consisting of NRI and payload type
                char hdr = (*data & 0x60) | (*(data + 1) & 0x1f);
                COPY(&hdr, 1);
            }
            COPY(data + 2, len - 2); //skip FU indicator and header
            break;
        }
        case NAL_TYPE_STAP_A:
        {
            //format:
            // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
            // |0|1|2|3|4|5|6|7|0|1|2|3|4|5|6|7|0|1|2|3|4|5|6|7|0|1|2|3|4|5|6|7|
            // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
            // |STAP-A NAL HDR |         NALU 1 Size           |               |
            // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+               |
            // |                         NALU 1                                |
            // :                                                               :
            // +               +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
            // |               |         NALU 2 Size           |               |
            // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+               +
            // |                         NALU 2                                |
            // :                                                               :
            // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
            char *p = data + 1; //skip type
            int sz;
            while (p < data + len - 1)
            {
                sz = (*p << 8) | *(p + 1);
                if (sz <= 0)
                {
                    LOGGER_ERROR(sLogger, LOGPREFIX
                                 << "unpacketize: Invalid STAP-A size.");
                    //return current bufLen and let av_parser_parse2() decide if
                    //data is invalid
                    break;
                }
                p += 2; //skip size bytes
                COPY(NAL_START_CODE, sizeof(NAL_START_CODE));
                COPY(p, sz);
                p += sz;
            }
            break;
        }
        default:
        {
            if (nalType >= NAL_TYPE_SINGLE_MIN && nalType <= NAL_TYPE_SINGLE_MAX)
            {
                COPY(NAL_START_CODE, sizeof(NAL_START_CODE));
                COPY(data, len);
            }
            else
            {
                LOGGER_ERROR(sLogger, LOGPREFIX
                             << "unpacketize: Unsupported NAL type " << nalType);
            }
            break;
        }
    }
    return bufLen;
}

//void VideoDecoder::getDecodedFrame()
//{
//    SwsContext *imgCtx;
//    int w;
//    int h;
//    int ret = avcodec_send_packet(mCodecCtx, mPacket);
//    while (ret >= 0)
//    {
//        ret = avcodec_receive_frame(mCodecCtx, mFrameYuv);
//        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
//            break;
//        if (ret < 0)
//        {
//            LOGGER_ERROR(sLogger, LOGPREFIX
//                         << "getDecodedFrame: avcodec_receive_frame failure "
//                         << ret);
//            break;
//        }
//        //frame available - convert YUV to output format
//        w = mCodecCtx->width;
//        h = mCodecCtx->height;

//        /////////////////////////

//        // 1. Create a SwsContext to convert from YUVJ420P to YUV420P
//        SwsContext *imgCtx = sws_getContext(
//            w, h, AV_PIX_FMT_YUVJ420P,  // Input format (full-range YUV)
//            w, h, AV_PIX_FMT_YUV420P,   // Output format (limited-range YUV)
//            SWS_BICUBIC, NULL, NULL, NULL
//            );

//        if (!imgCtx) {
//            LOGGER_ERROR(sLogger, "Failed to create swsContext for YUV420P conversion.");
////            return QByteArray();
//        }

//        // 2. Allocate the output frame for YUV420P
//        AVFrame *frameYUV420P = av_frame_alloc();
//        av_image_alloc(frameYUV420P->data, frameYUV420P->linesize,
//                       w, h, AV_PIX_FMT_YUV420P, 32);

//        // 3. Convert YUVJ420P -> YUV420P
//        sws_scale(imgCtx, mFrameYuv->data, mFrameYuv->linesize, 0, h,
//                  frameYUV420P->data, frameYUV420P->linesize);

//        // 4. Extract YUV420P data into QByteArray
//        int ySize = w * h;
//        int uSize = (w / 2) * (h / 2);
//        int vSize = uSize;
//        int totalSize = ySize + uSize + vSize;

//        QByteArray yuvData;
//        yuvData.resize(totalSize);
//        uchar *buffer = reinterpret_cast<uchar *>(yuvData.data());

//        memcpy(buffer, frameYUV420P->data[0], ySize);                // Y plane
//        memcpy(buffer + ySize, frameYUV420P->data[1], uSize);        // U plane
//        memcpy(buffer + ySize + uSize, frameYUV420P->data[2], vSize);// V plane

//        // 5. Free resources
//        sws_freeContext(imgCtx);
//        av_freep(frameYUV420P->data);
//        av_frame_unref(frameYUV420P);
//        av_frame_free(&frameYUV420P);

////        return yuvData;  // Now contains YUV420P (limited range)
//        LOGGER_ERROR(sLogger, "Pix format before conversion " << mCodecCtx->pix_fmt);
//        LOGGER_ERROR(sLogger, "Pix format after conversion " << mFrameYuv->format);

//        // After conversion
////        LOGGER_INFO(sLogger, "Input format: " << av_get_pix_fmt_name((AVPixelFormat)mFrameYuv->format));
//        LOGGER_INFO(sLogger, "Output format: " << AV_PIX_FMT_YUV420P);
//        LOGGER_INFO(sLogger, "Frame dimensions: " << w << "x" << h);

//        streamer.sendFrameData(yuvData);


//        //////////////////////

//        imgCtx = sws_getContext(w, h, mCodecCtx->pix_fmt, w, h, OUTPUT_FORMAT,
//                                SWS_BICUBIC, NULL, NULL, NULL);
//        if (imgCtx == 0)
//        {
//            av_frame_unref(mFrameYuv);
//            LOGGER_ERROR(sLogger, LOGPREFIX
//                         << "getDecodedFrame: sws_getContext failure.");
//            return;
//        }
//        av_image_alloc(mFrameRgb->data, mFrameRgb->linesize, w, h,
//                       OUTPUT_FORMAT, 32);
//        sws_scale(imgCtx, mFrameYuv->data, mFrameYuv->linesize, 0, h,
//                  mFrameRgb->data, mFrameRgb->linesize);
//#ifdef MOBILE
//        mCbFn(mFrameRgb->data[0], w, h, mFrameRgb->linesize[0]);
//#else
//        mCbFn(mCbObj, mFrameRgb->data[0], w, h, mFrameRgb->linesize[0]);
//#endif
//        av_freep(mFrameRgb->data);
//        av_frame_unref(mFrameRgb);
//        av_frame_unref(mFrameYuv);
//        sws_freeContext(imgCtx);
////        qDebug() << "\nmframergb: " << mFrameRgb << "\nmframeYUV" << mFrameYuv;
//    }
//}

//void VideoDecoder::getDecodedFrame()
//{
//    SwsContext *imgCtx;
//    int w, h;
//    QByteArray yuv420pData; // Store YUV420P data for later use

//    int ret = avcodec_send_packet(mCodecCtx, mPacket);
//    while (ret >= 0)
//    {
//            ret = avcodec_receive_frame(mCodecCtx, mFrameYuv);
//            if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
//                break;
//            if (ret < 0)
//            {
//                LOGGER_ERROR(sLogger, LOGPREFIX
//                                          << "getDecodedFrame: avcodec_receive_frame failure "
//                                          << ret);
//                break;
//            }

//            // Get frame dimensions
//            w = mCodecCtx->width;
//            h = mCodecCtx->height;

//            // Convert YUVJ420P (12) to YUV420P (0)
//            SwsContext *yuvCtx = sws_getContext(w, h, AV_PIX_FMT_YUVJ420P, // Input format
//                                                w, h, AV_PIX_FMT_YUV420P,  // Output format
//                                                SWS_BICUBIC, NULL, NULL, NULL);
//            if (!yuvCtx)
//            {
//                av_frame_unref(mFrameYuv);
//                LOGGER_ERROR(sLogger, LOGPREFIX
//                                          << "getDecodedFrame: sws_getContext failure for YUV conversion.");
//                return;
//            }

//            // Allocate YUV420P frame
//            AVFrame *frameYUV420P = av_frame_alloc();
//            frameYUV420P->format = AV_PIX_FMT_YUV420P;
//            frameYUV420P->width = w;
//            frameYUV420P->height = h;
//            av_image_alloc(frameYUV420P->data, frameYUV420P->linesize, w, h,
//                           AV_PIX_FMT_YUV420P, 32);

//            // Convert YUVJ420P -> YUV420P
//            sws_scale(yuvCtx, mFrameYuv->data, mFrameYuv->linesize, 0, h,
//                      frameYUV420P->data, frameYUV420P->linesize);

//            // Store YUV420P data into QByteArray
//            int ySize = w * h;
//            int uvSize = ySize / 4;
//            yuv420pData.append(reinterpret_cast<const char *>(frameYUV420P->data[0]), ySize);  // Y plane
//            yuv420pData.append(reinterpret_cast<const char *>(frameYUV420P->data[1]), uvSize); // U plane
//            yuv420pData.append(reinterpret_cast<const char *>(frameYUV420P->data[2]), uvSize); // V plane

//            streamer.sendFrameData(yuv420pData);

//            // Free YUV conversion resources
//            av_freep(&frameYUV420P->data[0]);
//            av_frame_free(&frameYUV420P);
//            sws_freeContext(yuvCtx);

//            // Continue with the original RGB conversion
//            imgCtx = sws_getContext(w, h, mCodecCtx->pix_fmt, w, h, OUTPUT_FORMAT,
//                                    SWS_BICUBIC, NULL, NULL, NULL);
//            if (!imgCtx)
//            {
//                av_frame_unref(mFrameYuv);
//                LOGGER_ERROR(sLogger, LOGPREFIX
//                                          << "getDecodedFrame: sws_getContext failure for RGB conversion.");
//                return;
//            }

//            av_image_alloc(mFrameRgb->data, mFrameRgb->linesize, w, h,
//                           OUTPUT_FORMAT, 32);
//            sws_scale(imgCtx, mFrameYuv->data, mFrameYuv->linesize, 0, h,
//                      mFrameRgb->data, mFrameRgb->linesize);

//#ifdef MOBILE
//            mCbFn(mFrameRgb->data[0], w, h, mFrameRgb->linesize[0]);
//#else
//            mCbFn(mCbObj, mFrameRgb->data[0], w, h, mFrameRgb->linesize[0]);
//#endif

//            av_freep(mFrameRgb->data);
//            av_frame_unref(mFrameRgb);
//            av_frame_unref(mFrameYuv);
//            sws_freeContext(imgCtx);
//    }

//    // The variable `yuv420pData` now contains the YUV420P frame in QByteArray format.
//    // You can use `yuv420pData` later as needed.
//}

void VideoDecoder::getDecodedFrame()
{
    SwsContext *imgCtx;
    int w, h;
    QByteArray yuv420pData; // Store YUV420P data for later use

    int ret = avcodec_send_packet(mCodecCtx, mPacket);
    while (ret >= 0)
    {
            ret = avcodec_receive_frame(mCodecCtx, mFrameYuv);
            if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
                break;
            if (ret < 0)
            {
                LOGGER_ERROR(sLogger, LOGPREFIX
                                          << "getDecodedFrame: avcodec_receive_frame failure "
                                          << ret);
                break;
            }

            // Get frame dimensions
            w = mCodecCtx->width;
            h = mCodecCtx->height;

            // Check original format
            AVPixelFormat originalFormat = mCodecCtx->pix_fmt;
            LOGGER_INFO(sLogger, LOGPREFIX
                                     << "Original pixel format: " << av_get_pix_fmt_name(originalFormat));

            // Convert YUVJ420P to YUV420P if needed
            if (originalFormat == AV_PIX_FMT_YUVJ420P)
            {
                SwsContext *yuvCtx = sws_getContext(w, h, AV_PIX_FMT_YUVJ420P, // Input format
                                                    w, h, AV_PIX_FMT_YUV420P,  // Output format
                                                    SWS_BICUBIC, NULL, NULL, NULL);
                if (!yuvCtx)
                {
                    av_frame_unref(mFrameYuv);
                    LOGGER_ERROR(sLogger, LOGPREFIX
                                              << "getDecodedFrame: sws_getContext failure for YUV conversion.");
                    return;
                }

                // Allocate YUV420P frame
                AVFrame *frameYUV420P = av_frame_alloc();
                frameYUV420P->format = AV_PIX_FMT_YUV420P;
                frameYUV420P->width = w;
                frameYUV420P->height = h;
                av_image_alloc(frameYUV420P->data, frameYUV420P->linesize, w, h,
                               AV_PIX_FMT_YUV420P, 32);

                // Convert YUVJ420P -> YUV420P
                sws_scale(yuvCtx, mFrameYuv->data, mFrameYuv->linesize, 0, h,
                          frameYUV420P->data, frameYUV420P->linesize);

                // Verify converted format
                LOGGER_INFO(sLogger, LOGPREFIX
                                         << "Converted pixel format: " << av_get_pix_fmt_name(static_cast<AVPixelFormat>(frameYUV420P->format)));

                // Store YUV420P data into QByteArray
                int ySize = w * h;
                int uvSize = ySize / 4;
                yuv420pData.append(reinterpret_cast<const char *>(frameYUV420P->data[0]), ySize);  // Y plane
                yuv420pData.append(reinterpret_cast<const char *>(frameYUV420P->data[1]), uvSize); // U plane
                yuv420pData.append(reinterpret_cast<const char *>(frameYUV420P->data[2]), uvSize); // V plane

                streamer.sendFrameData(yuv420pData);

                // Free YUV conversion resources
                av_freep(&frameYUV420P->data[0]);
                av_frame_free(&frameYUV420P);
                sws_freeContext(yuvCtx);
            }

            // Continue with the original RGB conversion
            imgCtx = sws_getContext(w, h, mCodecCtx->pix_fmt, w, h, OUTPUT_FORMAT,
                                    SWS_BICUBIC, NULL, NULL, NULL);
            if (!imgCtx)
            {
                av_frame_unref(mFrameYuv);
                LOGGER_ERROR(sLogger, LOGPREFIX
                                          << "getDecodedFrame: sws_getContext failure for RGB conversion.");
                return;
            }

            av_image_alloc(mFrameRgb->data, mFrameRgb->linesize, w, h,
                           OUTPUT_FORMAT, 32);
            sws_scale(imgCtx, mFrameYuv->data, mFrameYuv->linesize, 0, h,
                      mFrameRgb->data, mFrameRgb->linesize);

#ifdef MOBILE
            mCbFn(mFrameRgb->data[0], w, h, mFrameRgb->linesize[0]);
#else
            mCbFn(mCbObj, mFrameRgb->data[0], w, h, mFrameRgb->linesize[0]);
#endif

            av_freep(mFrameRgb->data);
            av_frame_unref(mFrameRgb);
            av_frame_unref(mFrameYuv);
            sws_freeContext(imgCtx);
    }
}

