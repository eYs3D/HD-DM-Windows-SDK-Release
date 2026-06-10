#include "stdafx.h"
#include "ImgEncoder.h"
#include "FfmpegUtility.h"
#include <sstream>
#include <fstream>
#include <chrono>

extern "C"
{
#include "libavutil/opt.h"
#include "libavutil/imgutils.h"
#include "libswscale/swscale.h"
}

#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "avdevice.lib")
#pragma comment(lib, "avfilter.lib")
#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "avutil.lib")
#pragma comment(lib, "swscale.lib")

#define MAX_FRAMEBUFQUEUE_SIZE 30

int64_t GetProperBitrate(int width, int height)
{
    return (int64_t)150000 * (width * height) / (320 * 240);
}

std::string GetFFMpegErrorMsg(std::string failedFunction, int errCode)
{
    char msg[512] = { '\0' };
    av_make_error_string(msg, 512, errCode);
    std::ostringstream errMsg;
    errMsg << failedFunction.c_str() << " failed. (" << errCode << " " << msg << ")" << std::endl;

    return errMsg.str();
}

void ReleaseAVFrame(AVFrame** ppAvFrame)
{
    if (*ppAvFrame != nullptr)
    {
        av_frame_free(ppAvFrame);
        *ppAvFrame = nullptr;
    }
}

AVFrame* CreateAVFrame(const AVCodecContext* avCodecCtx)
{
    AVFrame* avFrame = av_frame_alloc();
    if (avFrame == nullptr)
    {
        OutputDebugString("Can't allocate AVFrame.");
        return nullptr;
    }
    avFrame->format = avCodecCtx->pix_fmt;
    avFrame->width = avCodecCtx->width;
    avFrame->height = avCodecCtx->height;

    int ret = av_frame_get_buffer(avFrame, 32);
    if (ret < 0)
    {
        OutputDebugString(GetFFMpegErrorMsg("av_frame_get_buffer", ret).c_str());
        ReleaseAVFrame(&avFrame);
        return nullptr;
    }

    return avFrame;
}

CEncodingFrameWrapper::CEncodingFrameWrapper(AVCodecContext* avCodecContext)
{
    m_avFrame = CreateAVFrame(avCodecContext);
    if (m_avFrame == nullptr)
    {
        OutputDebugString("CreateAVFrame failed.");
    }
}

CEncodingFrameWrapper::~CEncodingFrameWrapper()
{
    ReleaseAVFrame(&m_avFrame);
}


CImgEncoder::CImgEncoder()
    : m_avCodecContext(nullptr), m_avFmtContext(nullptr), m_avStream(nullptr),
    m_avFrame(nullptr), m_avPacket(nullptr), m_swsCtx(nullptr),
    m_inImgFmt(AV_PIX_FMT_YUYV422), m_ready(false), m_firstFrameTimestamp(0),
    m_timeUnit(0.0), m_frameIndex(0), m_fixedFrameRate(false),
    m_encodeThread(nullptr), m_threadStart(false)
{
}

CImgEncoder::~CImgEncoder()
{
    Uninit();
}

bool IsSupportedFormat(AVPixelFormat imgFmt)
{
    switch (imgFmt)
    {
    case AV_PIX_FMT_YUV420P:
    case AV_PIX_FMT_YUYV422:
    case AV_PIX_FMT_BGR24:
        return true;
    default:
        return false;
    }

    return false;
}

AVCodec* FindAVCodec(AVCodecID codecId, bool forceSwEncode)
{
	AVCodec* avCodec = nullptr;

	if (!forceSwEncode)
	{
		avCodec = avcodec_find_encoder_by_name("h264_nvenc");
		if (avCodec == nullptr)
		{
			OutputDebugString("Can't find \"nvenc\" encoder.\n");
		}
	}
	
	if (avCodec == nullptr)
	{
		OutputDebugString("Use ffmpeg default encoder.\n");
		avCodec = avcodec_find_encoder(codecId);
	}
    
	return avCodec;
}

AVCodecContext* CreateAVCodecContext(AVCodec* avCodec, int imgWidth, int imgHeight, int frameRate)
{
    AVCodecContext* avCodecContext = avcodec_alloc_context3(avCodec);
    if (avCodecContext == nullptr)
    {
        OutputDebugString("Can't allocate AVCodecContext");
        return nullptr;
    }

    avCodecContext->bit_rate = GetProperBitrate(imgWidth, imgHeight);
    avCodecContext->width = imgWidth;
    avCodecContext->height = imgHeight;
    avCodecContext->time_base = { 1, frameRate };
    avCodecContext->framerate = { frameRate, 1 };
    avCodecContext->gop_size = frameRate / 2;
    avCodecContext->max_b_frames = 0;
    avCodecContext->pix_fmt = AV_PIX_FMT_YUV420P;

    av_opt_set(avCodecContext->priv_data, "preset", "slow", 0);// H264 only

    return avCodecContext;
}

AVCodecContext* CImgEncoder::OpenAvCodecCtx(int imgWidth, int imgHeight, int frameRate, bool forceSw)
{
    const AVCodecID dstCodecId = AV_CODEC_ID_H264;
    AVCodec* avCodec = FindAVCodec(dstCodecId, forceSw);
    if (avCodec == nullptr)
    {
        OutputDebugString("Can't find encoder");
        return nullptr;
    }

    AVCodecContext* avCodecContext = CreateAVCodecContext(avCodec, imgWidth, imgHeight, frameRate);
    if (avCodecContext == nullptr)
    {
        OutputDebugString("Create AVCodecContext failed.");
        ReleaseAvCodecCtx(&avCodecContext);
        return nullptr;
    }

    int ret = avcodec_open2(avCodecContext, avCodec, nullptr);
    if (ret < 0)
    {
        OutputDebugString(GetFFMpegErrorMsg("avcodec_open2", ret).c_str());
        ReleaseAvCodecCtx(&avCodecContext);
        return nullptr;
    }

    return avCodecContext;
}

void CImgEncoder::ReleaseAvCodecCtx(AVCodecContext** ppAvCodecContext)
{
    if (*ppAvCodecContext != nullptr)
    {
        avcodec_free_context(ppAvCodecContext);
        *ppAvCodecContext = nullptr;
    }
}

AVFormatContext* CImgEncoder::CreateAVFmtContext(const AVCodecContext* avCodecCtx, std::string filename)
{
    AVFormatContext* avFmtContext = nullptr;
    avformat_alloc_output_context2(&avFmtContext, nullptr, nullptr, filename.c_str());
    if (avFmtContext == nullptr)
    {
        return nullptr;
    }

    m_avStream = avformat_new_stream(avFmtContext, nullptr);
    if (m_avStream == nullptr)
    {
        ReleaseAVFmtContext(&avFmtContext);
        return nullptr;
    }
    //avcodec_copy_context(m_avStream->codec, avCodecCtx);

    int ret = avcodec_parameters_from_context(m_avStream->codecpar, avCodecCtx);
    if (ret < 0)
    {
        OutputDebugString(GetFFMpegErrorMsg("avcodec_parameters_from_context", ret).c_str());
        ReleaseAVFmtContext(&avFmtContext);
        return nullptr;
    }
    m_avStream->time_base = avCodecCtx->time_base;// this value will be translate to the real value after calling avformat_write_header
    av_dump_format(avFmtContext, 0, filename.c_str(), 1);
    ret = avio_open(&avFmtContext->pb, filename.c_str(), AVIO_FLAG_WRITE);
    if (ret < 0)
    {
        OutputDebugString(GetFFMpegErrorMsg("avio_open", ret).c_str());
        ReleaseAVFmtContext(&avFmtContext);
        return nullptr;
    }
    ret = avformat_write_header(avFmtContext, nullptr);
    if (ret < 0)
    {
        OutputDebugString(GetFFMpegErrorMsg("avformat_write_header", ret).c_str());
        ReleaseAVFmtContext(&avFmtContext);
        return nullptr;
    }

    return avFmtContext;
}

void CImgEncoder::ReleaseAVFmtContext(AVFormatContext** ppavFmtContext)
{
    if (*ppavFmtContext != nullptr)
    {
        av_write_trailer(*ppavFmtContext);
        avio_closep(&(*ppavFmtContext)->pb);
        avformat_free_context(*ppavFmtContext);
        *ppavFmtContext = nullptr;
        m_avStream = nullptr;
    }
}

bool CImgEncoder::Init(int imgWidth, int imgHeight, int frameRate, std::string filename, 
    AVPixelFormat imgFmt, bool fixedFrameRate)
{
    if (!IsSupportedFormat(imgFmt))
    {
        return false;
    }

    Uninit();
    m_inImgFmt = imgFmt;
    av_register_all();
    
    m_avCodecContext = OpenAvCodecCtx(imgWidth, imgHeight, frameRate);
    if (m_avCodecContext == nullptr)
    {
        // try software encoder
        m_avCodecContext = OpenAvCodecCtx(imgWidth, imgHeight, frameRate, true);
        if (m_avCodecContext == nullptr)
        {
            OutputDebugString("OpenAvCodecCtx(force sw) failed.");
            Uninit();
            return false;
        }
    }

    m_avFrame = CreateAVFrame(m_avCodecContext);
    if (m_avFrame == nullptr)
    {
        OutputDebugString("CreateAVFrame failed.");
        Uninit();
        return false;
    }

    if (m_inImgFmt != AV_PIX_FMT_YUV420P)
    {
        m_swsCtx = sws_getContext(imgWidth, imgHeight, m_inImgFmt, imgWidth, imgHeight, AV_PIX_FMT_YUV420P,
            SWS_POINT, nullptr, nullptr, nullptr);
        if (m_swsCtx == nullptr)
        {
            OutputDebugString("sws_getContext failed.");
            Uninit();
            return false;
        }
    }

    m_avPacket = av_packet_alloc();
    if (m_avPacket == nullptr)
    {
        OutputDebugString("Allocate AVPacket failed.");
        Uninit();
        return false;
    }

    m_avFmtContext = CreateAVFmtContext(m_avCodecContext, filename);
    if (m_avFmtContext == nullptr)
    {
        OutputDebugString("CreateAVFmtContext failed.");
        Uninit();
        return false;
    }

    m_fixedFrameRate = fixedFrameRate;
    m_timeUnit = fixedFrameRate ? (double)m_avStream->time_base.den / m_avStream->time_base.num
                                : (double)m_avStream->time_base.den / m_avStream->time_base.num / 1000;

    m_frameIndex = 0;
    m_firstFrameTimestamp = 0;

    CreateEncodeThread();

    m_ready = true;
    
    return true;
}

void CImgEncoder::Uninit()
{
    m_ready = false;

    ReleaseEncodeThread();

    m_firstFrameTimestamp = 0;
    m_timeUnit = 0.0;
    m_frameIndex = 0;
    m_fixedFrameRate = false;

    ReleaseAVFmtContext(&m_avFmtContext);

    if (m_avPacket != nullptr)
    {
        av_packet_free(&m_avPacket);
        m_avPacket = nullptr;
    }

    if (m_swsCtx != nullptr)
    {
        sws_freeContext(m_swsCtx);
        m_swsCtx = nullptr;
    }

    ReleaseAVFrame(&m_avFrame);
    ReleaseAvCodecCtx(&m_avCodecContext);
}

void CImgEncoder::CopyImgToAvFrame(const unsigned char* imgBuf, AVFrame* frame)
{
    switch (m_inImgFmt)
    {
    case AV_PIX_FMT_YUV420P:
    {
        memcpy(frame->data[0], imgBuf, frame->width * frame->height);
        memcpy(frame->data[1], imgBuf + frame->width * frame->height, frame->width * frame->height / 4);
        memcpy(frame->data[2], imgBuf + frame->width * frame->height * 5 / 4, frame->width * frame->height / 4);
    }
    break;
    case AV_PIX_FMT_YUYV422:
    {
        // YUV422 to YUV420P
        const int srcLineSize[] = { frame->width * 2 };
        const uint8_t* srcImg[] = { &imgBuf[0] };
        const int dstLineSize[] = { frame->width, frame->width / 2, frame->width / 2 };
        sws_scale(m_swsCtx, srcImg, srcLineSize, 0, frame->height, &frame->data[0], dstLineSize);
    }
    break;
    case AV_PIX_FMT_BGR24:
    {
        // BGR to YUV420P
        const int srcLineSize[] = { frame->width * 3 };
        const uint8_t* srcImg[] = { &imgBuf[0] };
        const int dstLineSize[] = { frame->width, frame->width / 2, frame->width / 2 };
        sws_scale(m_swsCtx, srcImg, srcLineSize, 0, frame->height, &frame->data[0], dstLineSize);
    }
    break;
    }
}

bool CImgEncoder::EncodeAVFrameToFile(AVFrame* avFrame)
{
    int ret = avcodec_send_frame(m_avCodecContext, avFrame);
    if (ret < 0)
    {
        OutputDebugString(GetFFMpegErrorMsg("avcodec_send_frame", ret).c_str());
        return false;
    }

    while (ret >= 0)
    {
        ret = avcodec_receive_packet(m_avCodecContext, m_avPacket);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
        {
            return false;
        }
        else if (ret < 0)
        {
            OutputDebugString(GetFFMpegErrorMsg("avcodec_receive_packet", ret).c_str());
            return false;
        }

        ret = av_write_frame(m_avFmtContext, m_avPacket);
        if (ret < 0)
        {
            OutputDebugString(GetFFMpegErrorMsg("av_write_frame", ret).c_str());
            return false;
        }

        av_packet_unref(m_avPacket);
    }

    return true;
}

std::shared_ptr<CEncodingFrameWrapper> CImgEncoder::CreateEncodingFrameWrapper(const unsigned char* imgBuf)
{
    std::shared_ptr<CEncodingFrameWrapper> frameWrapper = std::make_shared<CEncodingFrameWrapper>(m_avCodecContext);
    int ret = av_frame_make_writable(frameWrapper->m_avFrame);
    if (ret < 0)
    {
        OutputDebugString(GetFFMpegErrorMsg("av_frame_make_writable", ret).c_str());
        return false;
    }

    CopyImgToAvFrame(imgBuf, frameWrapper->m_avFrame);

    if (m_fixedFrameRate)
    {
        frameWrapper->m_avFrame->pts = (m_frameIndex > 0) ? (double)m_frameIndex / m_avCodecContext->framerate.num * m_timeUnit : 0;
        ++m_frameIndex;
    }
    else
    {
        if (m_firstFrameTimestamp != 0)
        {
            frameWrapper->m_avFrame->pts = (int64_t)((double)(GetTickCount() - m_firstFrameTimestamp) * m_timeUnit);
        }
        else
        {
            m_firstFrameTimestamp = GetTickCount();
            frameWrapper->m_avFrame->pts = 0;
        }
    }

    return frameWrapper;
}

void DropFrames(std::queue<std::shared_ptr<CEncodingFrameWrapper>>& frameQueue)
{
    std::queue<std::shared_ptr<CEncodingFrameWrapper>> tempQueue;

    bool keep = false;
    while (!frameQueue.empty())
    {
        if (keep)
        {
            tempQueue.push(frameQueue.front());
        }

        tempQueue.pop();
        keep = !keep;
    }

    frameQueue = tempQueue;
}

bool CImgEncoder::PutFrame(const unsigned char* imgBuf)
{
    if (!m_ready)
    {
        return false;
    }

    std::shared_ptr<CEncodingFrameWrapper> frameWrapper = CreateEncodingFrameWrapper(imgBuf);

    std::lock_guard<std::mutex> lock(m_queueMutex);
    if (m_frameQueue.size() == MAX_FRAMEBUFQUEUE_SIZE)
    {
        DropFrames(m_frameQueue);
    }

    m_frameQueue.push(frameWrapper);

    return true;
}

void CImgEncoder::EncodeThreadFn(CImgEncoder* pThis)
{
    while (pThis->m_threadStart)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        std::shared_ptr<CEncodingFrameWrapper> frameWrapper;
        {
            std::lock_guard<std::mutex> lock(pThis->m_queueMutex);
            if (!pThis->m_frameQueue.empty())
            {
                frameWrapper = pThis->m_frameQueue.front();
                pThis->m_frameQueue.pop();
            }
        }

        if (frameWrapper)
        {
            pThis->EncodeAVFrameToFile(frameWrapper->m_avFrame);
        }
    }

    // encode remainder frames in queue
    std::lock_guard<std::mutex> lock(pThis->m_queueMutex);
    while (!pThis->m_frameQueue.empty())
    {
        pThis->EncodeAVFrameToFile(pThis->m_frameQueue.front()->m_avFrame);
        pThis->m_frameQueue.pop();
    }
}

void CImgEncoder::CreateEncodeThread()
{
    ReleaseEncodeThread();

    m_threadStart = true;
    m_encodeThread = new std::thread(CImgEncoder::EncodeThreadFn, this);
}

void CImgEncoder::ReleaseEncodeThread()
{
    if (m_encodeThread != nullptr)
    {
        m_threadStart = false;
        m_encodeThread->join();
        delete m_encodeThread;
        m_encodeThread = nullptr;
    }
}