#include "stdafx.h"
#include "Mp4VideoCutter.h"
#include "FfmpegUtility.h"


extern "C"
{
#include "libavcodec/avcodec.h"  
#include "libavformat/avformat.h" 
#include "libavutil/avutil.h"
}

#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "avutil.lib")

const auto progressCallbackPeriod = std::chrono::milliseconds(200);

CMp4VideoCutter::CMp4VideoCutter(std::string srcFilename, int startTime, int endTime, std::string dstFilename,
    ProgressCallbackFn callbackFn, void* callbackFnParam)
    : m_srcPacketRetriever(srcFilename, CMp4VideoCutter::PacketRetrieverCallback, this), 
      m_startTime(startTime), m_endTime(endTime), m_dstFilename(dstFilename),
      m_progressCallbackFn(callbackFn), m_progressCallbackFnParam(callbackFnParam)
{
    Init(dstFilename);
}

CMp4VideoCutter::~CMp4VideoCutter()
{
    if (m_threadForStopRetriever.joinable())
    {
        m_threadForStopRetriever.join();
    }
    Uninit();
}

AVCodec* FindAVCodec(AVCodecID codecId)
{
    AVCodec* avCodec = nullptr;

    if (codecId == AV_CODEC_ID_H264)
    {
        avCodec = avcodec_find_encoder_by_name("h264_nvenc");
        if (!avCodec)
        {
            OutputDebugString("Can't find \"nvenc\" encoder.\n");
        }
    }

    if (!avCodec)
    {
        OutputDebugString("Use ffmpeg default encoder.\n");
        avCodec = avcodec_find_encoder(codecId);
        if (!avCodec)
        {
            OutputDebugString("avcodec_find_encoder failed.");
        }
    }

    return avCodec;
}

AVCodecContext* CMp4VideoCutter::CreateAvCodecCtx(bool audio)
{
    AVCodecContext* avCodecContext = avcodec_alloc_context3(nullptr);
    if (!avCodecContext)
    {
        OutputDebugString("Can't allocate AVCodecContext");
        return nullptr;
    }
    
    if (!m_srcPacketRetriever.CopyAVCodecContext(avCodecContext, audio))
    {
        OutputDebugString("Can't copy AVCodecContext data of srcRetriever.");
        ReleaseAvCodecCtx(&avCodecContext);
        return nullptr;
    }

    return avCodecContext;
}

void CMp4VideoCutter::ReleaseAvCodecCtx(AVCodecContext** ppAvCodecContext)
{
    if (*ppAvCodecContext)
    {
        avcodec_free_context(ppAvCodecContext);
        *ppAvCodecContext = nullptr;
    }
}

AVStream* CMp4VideoCutter::CreateNewStreamInFmtCtx(const AVCodecContext* avCodecCtx, AVFormatContext* avFmtCtx)
{
    AVStream* avStream = avformat_new_stream(avFmtCtx, nullptr);
    if (!avStream)
    {
        return nullptr;
    }

    int ret = avcodec_parameters_from_context(avStream->codecpar, avCodecCtx);
    if (ret < 0)
    {
        OutputDebugString(GetFFMpegErrorMsg("avcodec_parameters_from_context", ret).c_str());
        return nullptr;
    }

    // if avCodecCtx is for video, this value will be translated to the new (real?) value after calling avformat_write_header.
    // packet->pts need to be recalculated by this new time_base
    avStream->time_base = avCodecCtx->time_base;

    return avStream;
}

AVFormatContext* CMp4VideoCutter::CreateAvFmtCtx(const AVCodecContext* videoCodecCtx, const AVCodecContext* audioCodecCtx,
    std::string filename, double& videoTimeUnit, double& audioTimeUnit)
{
    AVFormatContext* avFmtCtx = nullptr;
    int ret = avformat_alloc_output_context2(&avFmtCtx, nullptr, nullptr, filename.c_str());
    if (ret < 0 || !avFmtCtx)
    {
        OutputDebugString(GetFFMpegErrorMsg("avformat_alloc_output_context2", ret).c_str());
        return nullptr;
    }

    // video
    AVStream* videoStream = CreateNewStreamInFmtCtx(videoCodecCtx, avFmtCtx);
    if (!videoStream)
    {
        ReleaseAvFmtCtx(&avFmtCtx);
        return nullptr;
    }
    videoStream->time_base = { 1 , videoCodecCtx->framerate.num / videoCodecCtx->framerate.den };

    // audio
    if (audioCodecCtx)
    {
        if (!CreateNewStreamInFmtCtx(audioCodecCtx, avFmtCtx))
        {
            ReleaseAvFmtCtx(&avFmtCtx);
            return nullptr;
        }
    }

    av_dump_format(avFmtCtx, 0, filename.c_str(), 1);

    ret = avio_open(&avFmtCtx->pb, filename.c_str(), AVIO_FLAG_WRITE);
    if (ret < 0)
    {
        OutputDebugString(GetFFMpegErrorMsg("avio_open", ret).c_str());
        ReleaseAvFmtCtx(&avFmtCtx);
        return nullptr;
    }

    ret = avformat_write_header(avFmtCtx, nullptr);
    if (ret < 0)
    {
        OutputDebugString(GetFFMpegErrorMsg("avformat_write_header", ret).c_str());
        ReleaseAvFmtCtx(&avFmtCtx);
        return nullptr;
    }

    // video time_base will be translated to the new (real ? ) value after calling avformat_write_header.
    // thus calculate time unit after calling avformat_write_header.
    videoTimeUnit = ((double)videoStream->time_base.den / videoStream->time_base.num) / ((double)videoCodecCtx->framerate.num / videoCodecCtx->framerate.den);
    if (audioCodecCtx)
    {
        audioTimeUnit = 1.0 / (av_q2d(audioCodecCtx->time_base) * 1000);
    }

    return avFmtCtx;
}

void CMp4VideoCutter::ReleaseAvFmtCtx(AVFormatContext** ppAvFmtCtx)
{
    if (*ppAvFmtCtx)
    {
        av_write_trailer(*ppAvFmtCtx);
        avio_closep(&(*ppAvFmtCtx)->pb);
        avformat_free_context(*ppAvFmtCtx);
        *ppAvFmtCtx = nullptr;
    }
}

bool CMp4VideoCutter::Init(std::string dstFilename)
{
    m_videoCodecContext = CreateAvCodecCtx(false);
    if (!m_videoCodecContext)
    {
        return false;
    }

    m_audioCodecContext = CreateAvCodecCtx(true);

    m_avFmtContext = CreateAvFmtCtx(m_videoCodecContext, m_audioCodecContext, m_dstFilename, 
        m_videoTimeUnit, m_audioTimeUnit);
    if (!m_avFmtContext)
    {
        ReleaseAvCodecCtx(&m_audioCodecContext);
        ReleaseAvCodecCtx(&m_videoCodecContext);
        return false;
    }

    m_videoFrameIndex = 0;
    m_baseTimestamp = 0;
    m_finished = false;
    m_srcPacketRetriever.Start(m_startTime);

    return true;
}

void CMp4VideoCutter::Uninit()
{
    m_srcPacketRetriever.Stop();
    ReleaseAvFmtCtx(&m_avFmtContext);
    ReleaseAvCodecCtx(&m_videoCodecContext);
}

void CMp4VideoCutter::PacketRetrieverCallback(int64_t pktTimestamp, bool audio, AVPacket* packet, void* param)
{
    CMp4VideoCutter* pThis = (CMp4VideoCutter*)param;

    if (pThis->m_finished)
    {
        return;
    }

    if (!packet || // packet == nullptr means src file has finished or an error occured
        pktTimestamp >= (int64_t)pThis->m_endTime)
    {
        pThis->m_finished = true;
    }

    if (packet)
    {
        // update timestamp
        if (audio)
        {
            if (pThis->m_baseTimestamp == 0 && pktTimestamp < pThis->m_baseTimestamp)
            {
                return;
            }

            packet->pts = packet->dts = (int64_t)(pThis->m_audioTimeUnit * (pktTimestamp - pThis->m_baseTimestamp));
        }
        else
        {
            if (pThis->m_baseTimestamp == 0)
            {
                pThis->m_baseTimestamp = pktTimestamp;
            }

            packet->pts = packet->dts = (int64_t)(pThis->m_videoTimeUnit * pThis->m_videoFrameIndex);
            ++pThis->m_videoFrameIndex;
        }

        int ret = av_write_frame(pThis->m_avFmtContext, packet);
        if (ret < 0)
        {
            OutputDebugString(GetFFMpegErrorMsg("av_write_frame", ret).c_str());
        }

        auto curTime = std::chrono::system_clock::now();
        if (curTime - pThis->m_preCallbackTime > progressCallbackPeriod)
        {
            pThis->CallbackProgress(pktTimestamp);
            pThis->m_preCallbackTime = curTime;
        }
    }

    if (pThis->m_finished)
    {
        pThis->m_threadForStopRetriever = std::thread(CMp4VideoCutter::StopSrcRetriever, pThis);
        pThis->CallbackProgress(pThis->m_endTime);
    }
}

void CMp4VideoCutter::StopSrcRetriever(CMp4VideoCutter* pThis)
{
    pThis->Uninit();
}

bool CMp4VideoCutter::IsFinished()
{
    return m_finished && m_avFmtContext == nullptr;
}

void CMp4VideoCutter::CallbackProgress(int64_t pktTimestamp)
{
    if (m_progressCallbackFn)
    {
        double progress = (pktTimestamp < m_endTime) ? (double)(pktTimestamp - m_startTime) / (m_endTime - m_startTime)
                                                     : 1.0;
        m_progressCallbackFn(progress, m_progressCallbackFnParam);
    }
}