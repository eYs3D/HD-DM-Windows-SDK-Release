#include "stdafx.h"
#include "Mp4FilePacketRetriever.h"
#include "FfmpegUtility.h"
#include <chrono>

extern "C"
{
#include "libavcodec/avcodec.h"  
#include "libavformat/avformat.h"
}

#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "avformat.lib")

CMp4FilePacketRetriever::CMp4FilePacketRetriever(std::string filename, 
    PacketCallbackFn callbackFn, void* callbackFnParam)
    : m_filename(filename), m_callbackFn(callbackFn), m_callbackFnParam(callbackFnParam)
{
    Init();
}

CMp4FilePacketRetriever::~CMp4FilePacketRetriever()
{
    Stop();
    Uninit();
}

bool CMp4FilePacketRetriever::Init()
{
    av_register_all();

    int ret = avformat_open_input(&m_fmt_ctx, m_filename.c_str(), nullptr, nullptr);
    if (ret < 0)
    {
        OutputDebugString(GetFFMpegErrorMsg("avformat_open_input", ret).c_str());
        return false;
    }

    ret = avformat_find_stream_info(m_fmt_ctx, nullptr);
    if (ret < 0)
    {
        OutputDebugString(GetFFMpegErrorMsg("avformat_find_stream_info", ret).c_str());
        return false;
    }

    m_video_stream = av_find_best_stream(m_fmt_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
    if (m_video_stream < 0)
    {
        OutputDebugString("Can't find video stream in input file");
        return false;
    }

    AVCodec* audioCodec = nullptr;
    m_audio_stream = av_find_best_stream(m_fmt_ctx, AVMEDIA_TYPE_AUDIO, -1, -1, &audioCodec, 0);
    if (m_audio_stream < 0)
    {
        OutputDebugString("No audio stream");
    }

    return true;
}

void CMp4FilePacketRetriever::Uninit()
{
    m_video_stream = -1;
    m_audio_stream = -1;

    if (m_fmt_ctx != nullptr)
    {
        avformat_close_input(&m_fmt_ctx);
        m_fmt_ctx = nullptr;
    }
}

void CMp4FilePacketRetriever::Start(int startTime)
{
    Stop();
    Seek(startTime);

    m_threadStart = true;
    m_thread = std::make_unique<std::thread>(CMp4FilePacketRetriever::ThreadFn, this);
}

void CMp4FilePacketRetriever::Stop()
{
    if (m_thread)
    {
        m_threadStart = false;
        m_thread->join();
        m_thread.reset();
    }
}

void CMp4FilePacketRetriever::Seek(int timestamp)
{
    std::lock_guard<std::mutex> lock(m_seekTimeMutex);
    m_seekTime = timestamp;
}

bool CMp4FilePacketRetriever::IsVideoPacket(const AVPacket* pkt) const
{
    return pkt->stream_index == m_video_stream;
}

bool CMp4FilePacketRetriever::IsAudioPacket(const AVPacket* pkt) const
{
    return pkt->stream_index == m_audio_stream;
}

AVCodecContext* CMp4FilePacketRetriever::GetVideoCtx() const
{
    if (m_video_stream < 0)
    {
        return nullptr;
    }

    return m_fmt_ctx->streams[m_video_stream]->codec;
}

AVCodecContext* CMp4FilePacketRetriever::GetAudioCtx() const
{
    if (m_audio_stream < 0)
    {
        return nullptr;
    }

    return m_fmt_ctx->streams[m_audio_stream]->codec;
}

int64_t CMp4FilePacketRetriever::GetPacketTimestamp(const AVPacket* pkt) const
{
    return IsVideoPacket(pkt) ? pkt->dts
                              : (int64_t)(pkt->dts * av_q2d(GetAudioCtx()->time_base) * 1000);
}

bool CMp4FilePacketRetriever::CheckAndDoSeek()
{
    int timestamp = -1;
    {
        std::lock_guard<std::mutex> lock(m_seekTimeMutex);
        timestamp = m_seekTime;
        m_seekTime = -1;
    }

    if (timestamp == -1)
    {
        return false;
    }

    int ret = av_seek_frame(m_fmt_ctx, -1, (int64_t)timestamp * 1000, AVSEEK_FLAG_BACKWARD);

    std::unique_ptr<CAVPacketWrapper> candidatePacket;
    while (true)
    {
        auto pktWrapper = std::make_unique<CAVPacketWrapper>();
        AVPacket& pkt = pktWrapper->pkt;
        if (av_read_frame(m_fmt_ctx, &pkt) < 0)
        {
            break;
        }

        int64_t pktTimestamp = IsVideoPacket(&pkt) ? pkt.dts
            : (double)pkt.dts * av_q2d(GetAudioCtx()->time_base) * 1000;
        if (pktTimestamp > (int64_t)timestamp)
        {
            if (candidatePacket)
            {
                m_callbackFn(GetPacketTimestamp(&candidatePacket->pkt), false, &candidatePacket->pkt, m_callbackFnParam);
                candidatePacket.reset();
            }

            // the nearest a/v packet after seek time
            m_callbackFn(GetPacketTimestamp(&pkt), IsAudioPacket(&pkt), &pkt, m_callbackFnParam);
            return true;
        }

        if (IsVideoPacket(&pkt))
        {
            // the nearest video packet before seek time
            candidatePacket.swap(pktWrapper);
        }
    }

    if (candidatePacket)
    {
        m_callbackFn(GetPacketTimestamp(&candidatePacket->pkt), false, &candidatePacket->pkt, m_callbackFnParam);
    }

    return true;
}

void CMp4FilePacketRetriever::ThreadFn(CMp4FilePacketRetriever* pThis)
{
    bool eof = false;

    while (pThis->m_threadStart)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        if (pThis->CheckAndDoSeek())
        {
            eof = false;
        }

        if (eof)
        {
            continue;
        }

        CAVPacketWrapper pktWrapper;
        AVPacket& pkt = pktWrapper.pkt;
        int ret = av_read_frame(pThis->m_fmt_ctx, &pkt);
        if (ret < 0)
        {
            OutputDebugString(GetFFMpegErrorMsg("av_read_frame", ret).c_str());
            eof = true;
            // notify an error occurs or read frame finished
            pThis->m_callbackFn(0, false, nullptr, pThis->m_callbackFnParam);
            continue;
        }

        pThis->m_callbackFn(pThis->GetPacketTimestamp(&pkt), pThis->IsAudioPacket(&pkt), &pkt, pThis->m_callbackFnParam);
    }
}

bool CMp4FilePacketRetriever::GetVideoInfo(AVCodecID& format, int& width, int& height, int& frameRate, int& bitrate) const
{
    if (!GetVideoCtx())
    {
        return false;
    }

    format = GetVideoCtx()->codec_id;
    width = GetVideoCtx()->width;
    height = GetVideoCtx()->height;
    frameRate = GetVideoCtx()->framerate.num / GetVideoCtx()->framerate.den;
    bitrate = (int)GetVideoCtx()->bit_rate;

    return true;
}

bool CMp4FilePacketRetriever::GetAudioInfo(unsigned short& channelNumber, unsigned short& bitsPerSample, unsigned int& samplesPerSec) const
{
    if (!GetAudioCtx())
    {
        return false;
    }

    // sample_fmt in audio ctx may not the actual format, sometimes it is fmt1 before calling avcodec_open2, and
    // becomes fmt2 after calling avcodec_open2. the actual format is in AVFrame->format after calling 
    // avcodec_decode_audio4. sample_fmt in audio ctx is only as information shown to ui before decoding.
    channelNumber = (unsigned short)GetAudioCtx()->channels;
    bitsPerSample = (unsigned short)GetAudioCtx()->bits_per_coded_sample;
    samplesPerSec = (unsigned int)GetAudioCtx()->sample_rate;

    return true;
}

bool CMp4FilePacketRetriever::CopyAVCodecContext(AVCodecContext* avCodecContext, bool audio)
{
    AVCodecContext* dstCodecCtx = audio ? GetAudioCtx()
                                        : GetVideoCtx();
    if (!avCodecContext || !dstCodecCtx)
    {
        return false;
    }

    int ret = avcodec_copy_context(avCodecContext, dstCodecCtx);
    if (ret < 0)
    {
        OutputDebugString(GetFFMpegErrorMsg("avcodec_copy_context", ret).c_str());
        return false;
    }

    return true;
}