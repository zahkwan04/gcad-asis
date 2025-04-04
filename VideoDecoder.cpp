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

void VideoDecoder::getDecodedFrame()
{
    SwsContext *imgCtx;
    int w;
    int h;
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

            // Frame available - get dimensions
            w = mCodecCtx->width;
            h = mCodecCtx->height;

            // Extract YUV data and create QByteArray
            QByteArray yuvData = createYuvQByteArray(mFrameYuv, w, h);

            // You can now use yuvData for streaming or other purposes
            // For example: streamer.sendFrameData(yuvData);

            // Continue with RGB conversion if needed
            imgCtx = sws_getContext(w, h, mCodecCtx->pix_fmt, w, h, OUTPUT_FORMAT,
                                    SWS_BICUBIC, NULL, NULL, NULL);
            if (imgCtx == 0)
            {
                av_frame_unref(mFrameYuv);
                LOGGER_ERROR(sLogger, LOGPREFIX
                                          << "getDecodedFrame: sws_getContext failure.");
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

// Helper function to create QByteArray from YUV frame
QByteArray VideoDecoder::createYuvQByteArray(const AVFrame* frame, int width, int height)
{
    // Calculate the size of each plane based on the pixel format
    AVPixelFormat format = static_cast<AVPixelFormat>(frame->format);
    int yStride = frame->linesize[0];
    int uStride = frame->linesize[1];
    int vStride = frame->linesize[2];

    int ySize = 0;
    int uSize = 0;
    int vSize = 0;

    // Handle different YUV formats
    if (format == AV_PIX_FMT_YUV420P || format == AV_PIX_FMT_YUVJ420P) {
            // For 4:2:0 formats
            ySize = yStride * height;
            uSize = uStride * (height / 2);
            vSize = vStride * (height / 2);
    } else if (format == AV_PIX_FMT_YUV422P) {
            // For 4:2:2 formats
            ySize = yStride * height;
            uSize = uStride * height;
            vSize = vStride * height;
    } else if (format == AV_PIX_FMT_YUV444P) {
            // For 4:4:4 formats
            ySize = yStride * height;
            uSize = uStride * height;
            vSize = vStride * height;
    } else {
            // For other formats, you might need different calculations
            // This is a fallback that might not be correct for all formats
            ySize = yStride * height;
            uSize = uStride * height / 2;
            vSize = vStride * height / 2;
    }

    // Total size of the YUV data
    int totalSize = ySize + uSize + vSize;

    // Create QByteArray and resize it to fit all data
    QByteArray yuvData;
    yuvData.resize(totalSize);
    char* buffer = yuvData.data();

    // Copy Y plane data (considering stride/padding)
    for (int i = 0; i < height; i++) {
            memcpy(buffer + i * width, frame->data[0] + i * yStride, width);
    }

    // Copy U plane data
    int uHeight = (format == AV_PIX_FMT_YUV420P || format == AV_PIX_FMT_YUVJ420P) ? height / 2 : height;
    int uWidth = (format == AV_PIX_FMT_YUV420P || format == AV_PIX_FMT_YUVJ420P || format == AV_PIX_FMT_YUV422P) ? width / 2 : width;

    for (int i = 0; i < uHeight; i++) {
            memcpy(buffer + ySize + i * uWidth, frame->data[1] + i * uStride, uWidth);
    }

    // Copy V plane data
    int vHeight = uHeight;  // Usually the same as U
    int vWidth = uWidth;    // Usually the same as U

    for (int i = 0; i < vHeight; i++) {
            memcpy(buffer + ySize + uSize + i * vWidth, frame->data[2] + i * vStride, vWidth);
    }

    return yuvData;
}
