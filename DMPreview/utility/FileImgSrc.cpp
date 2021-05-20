#include "stdafx.h"
#include "FileImgSrc.h"
#include "FfmpegUtility.h"
#include <chrono>

extern "C"
{
#include "libavcodec/avcodec.h"  
#include "libavformat/avformat.h" 
#include "libavutil/imgutils.h"
#include "libavutil/avutil.h"
#include "libswscale/swscale.h"
#include "libswresample/swresample.h"
#include "libavutil/opt.h"
}

#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "avdevice.lib")
#pragma comment(lib, "avfilter.lib")
#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "avutil.lib")
#pragma comment(lib, "swscale.lib")
#pragma comment(lib, "swresample.lib")

#define AUDIO_PACKET_DROP_THRESHOLD 3000 // ms
#define VIDEO_PACKET_DROP_THRESHOLD 3000 // ms


CFileImgSrc::CFileImgSrc(const std::string& filename)
    : m_imgOutFormat(AV_PIX_FMT_BGR24), m_isReady(false), m_status(CImgSrc::Status_Stop),
      m_callbackFn(nullptr), m_callbackParam(nullptr), m_pCallbackThread(nullptr), m_startCallbackThread(false),
      m_fps(30), m_speed(0), m_maxSpeed(0), m_minSpeed(0), m_onlyISpeedBoundary(99), 
      m_timeDuration(0), m_pDecodeThread(nullptr), m_startDecodeThread(false),
      m_fmt_ctx(nullptr), m_video_stream(-1), m_videoCtx(nullptr), m_swsCtx(nullptr), 
      m_audio_stream(-1), m_timestampToSeek(-1), m_outAudioFmt(AV_SAMPLE_FMT_S16), 
      m_imgRes(-1, -1), m_audioTimestampUnit(1.0), m_videoTimestampUnit(1.0)
{
    SetFile(filename);
}

CFileImgSrc::~CFileImgSrc()
{
    Stop();
}

bool CFileImgSrc::IsReady()
{
    return m_isReady;
}

void CFileImgSrc::SetFile(const std::string& filename)
{
    Stop();

    m_imgFilename = filename;
    
    // for getting audio/video info
    InitFfmpeg();
    GetImageResolution(m_imgRes);
    GetAudioInfo(m_audioInfo);
    UninitFfmpeg();

    m_isReady = !m_imgFilename.empty();
}

void CFileImgSrc::Init()
{
    if (!InitFfmpeg())
    {
        return;
    }

    m_timeInfo.SetVideoDuration(m_fmt_ctx->duration / 1000);
    m_timeInfo.ResetBaseTime();
    m_timeInfo.SetCurrentTime(0);

    if (m_pDecodeThread == nullptr)
    {
        m_startDecodeThread = true;
        m_pDecodeThread = new std::thread(CFileImgSrc::DecodeThreadFn, this);
    }

    if (m_pCallbackThread == nullptr)
    {
        m_startCallbackThread = true;
        m_pCallbackThread = new std::thread(CFileImgSrc::CallbackThreadFn, this);
    }
}

void CFileImgSrc::Uninit()
{
    m_startCallbackThread = false;
    if (m_pCallbackThread != nullptr)
    {
        m_pCallbackThread->join();
        delete m_pCallbackThread;
        m_pCallbackThread = nullptr;
    }

    m_startDecodeThread = false;
    if (m_pDecodeThread != nullptr)
    {
        m_pDecodeThread->join();
        delete m_pDecodeThread;
        m_pDecodeThread = nullptr;
    }

    m_timeInfo.ResetBaseTime();
    m_timeInfo.SetCurrentTime(0);

    UninitFfmpeg();
}

bool CFileImgSrc::InitFfmpeg()
{
    av_register_all();

    int result = avformat_open_input(&m_fmt_ctx, m_imgFilename.c_str(), nullptr, nullptr);
    if (result < 0)
    {
        char temp[1024];
        int i = av_strerror(result, temp, 1024);
        av_log(nullptr, AV_LOG_ERROR, "Can't open file\n");
        return false;
    }

    if (avformat_find_stream_info(m_fmt_ctx, nullptr) < 0)
    {
        av_log(nullptr, AV_LOG_ERROR, "Can't get stream info\n");
        return false;
    }

    if (!InitFFmpegVideoStream())
    {
        return false;
    }

    m_timeDuration = (int64_t)(m_fmt_ctx->streams[m_video_stream]->duration * m_videoTimestampUnit);
    if (m_fmt_ctx->streams[m_video_stream]->nb_frames > 1)
    {
        m_fps = 1000 /
            (int)(m_fmt_ctx->streams[m_video_stream]->duration / (m_fmt_ctx->streams[m_video_stream]->nb_frames - 1));
    }

    InitFFmpegAudioStreamIfExist();

    return true;
}

bool CFileImgSrc::InitFFmpegVideoStream()
{
    m_video_stream = av_find_best_stream(m_fmt_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
    if (m_video_stream < 0)
    {
        av_log(nullptr, AV_LOG_ERROR, "Can't find video stream in input file\n");
        return false;
    }
    m_videoTimestampUnit = av_q2d(m_fmt_ctx->streams[m_video_stream]->time_base) * 1000;

    AVCodecParameters* origin_par = m_fmt_ctx->streams[m_video_stream]->codecpar;
    AVCodec* videoCodec = avcodec_find_decoder(origin_par->codec_id);

    m_videoCtx = avcodec_alloc_context3(videoCodec);
    if (m_videoCtx == nullptr)
    {
        av_log(nullptr, AV_LOG_ERROR, "Can't allocate video decoder context\n");
        return false;
    }

    if (avcodec_parameters_to_context(m_videoCtx, origin_par) != 0)
    {
        av_log(nullptr, AV_LOG_ERROR, "Can't copy video decoder context\n");
        return false;
    }

    if (avcodec_open2(m_videoCtx, videoCodec, nullptr) < 0)
    {
        av_log(m_videoCtx, AV_LOG_ERROR, "Can't open video decoder\n");
        return false;
    }

    m_swsCtx = sws_getContext(m_videoCtx->width, m_videoCtx->height, m_videoCtx->pix_fmt,
        m_videoCtx->width, m_videoCtx->height, m_imgOutFormat,
        SWS_BILINEAR, nullptr, nullptr, nullptr);
    if (m_swsCtx == nullptr)
    {
        av_log(nullptr, AV_LOG_ERROR, "Can't get swsContext\n");
        return false;
    }

    return true;
}

bool CFileImgSrc::InitFFmpegAudioStreamIfExist()
{
    AVCodec* audioCodec = nullptr;
    m_audio_stream = av_find_best_stream(m_fmt_ctx, AVMEDIA_TYPE_AUDIO, -1, -1, &audioCodec, 0);
    if (m_audio_stream < 0)
    {
        av_log(nullptr, AV_LOG_ERROR, "No audio stream\n");
        return true;
    }
    m_audioTimestampUnit = av_q2d(GetAudioCtx()->time_base) * 1000;

    if (avcodec_open2(m_fmt_ctx->streams[m_audio_stream]->codec, audioCodec, nullptr) < 0)
    {
        av_log(m_fmt_ctx->streams[m_audio_stream]->codec, AV_LOG_ERROR, "Can't open audio decoder\n");
        return false;
    }

    return true;
}

void CFileImgSrc::UninitFfmpeg()
{
    m_audio_stream = -1;
    m_audioTimestampUnit = 1.0;

    if (m_swsCtx != nullptr)
    {
        sws_freeContext(m_swsCtx);
        m_swsCtx = nullptr;
    }
    
    if (m_videoCtx != nullptr)
    {
        avcodec_close(m_videoCtx);
        avcodec_free_context(&m_videoCtx);
        m_videoCtx = nullptr;
    }
    
    m_video_stream = -1;
    m_videoTimestampUnit = 1.0;

    if (m_fmt_ctx != nullptr)
    {
        avformat_close_input(&m_fmt_ctx);
        m_fmt_ctx = nullptr;
    }
}

AVCodecContext* CFileImgSrc::GetAudioCtx() const
{
    if (m_audio_stream < 0)
    {
        return nullptr;
    }

    return m_fmt_ctx->streams[m_audio_stream]->codec;
}

bool CFileImgSrc::CheckAndDoSeek()
{
    int timestamp = -1;
    {
        std::lock_guard<std::mutex> lock(m_seekMutex);
        timestamp = m_timestampToSeek;
        m_timestampToSeek = -1;
    }

    if (timestamp != -1)
    {
        int ret = av_seek_frame(m_fmt_ctx, -1, (int64_t)timestamp * 1000, AVSEEK_FLAG_BACKWARD);
        
        m_rawDataQueue.EmptyQueue();
        m_timeInfo.ResetBaseTime();
        m_timeInfo.SetCurrentTime(timestamp);
        
        // find the exact frame to show
        const int rgbBufSize = av_image_get_buffer_size(m_imgOutFormat, m_videoCtx->width, m_videoCtx->height, 16);
        CRawData candidateFrame(std::vector<uint8_t>(rgbBufSize), 0, false);
        bool gotFrame = false;
        while (true)
        {
            CAVPacketWrapper pktWrapper;
            AVPacket& pkt = pktWrapper.pkt;
            if (av_read_frame(m_fmt_ctx, &pkt) < 0)
            {
                break;
            }

            int64_t pktTimestamp = GetPacketTimestamp(&pkt);
            if (pktTimestamp > (int64_t)timestamp)
            {
                CRawData rgbBuf(std::vector<uint8_t>(rgbBufSize), 0, false);
                if (IsVideoPacket(&pkt) && DecodeVideoPacket(&pkt, rgbBuf))
                {
                    m_rawDataQueue.PutData(rgbBuf);
                }
                break;
            }

            if (IsVideoPacket(&pkt))
            {
                if (!gotFrame ||
                    pktTimestamp > candidateFrame.m_timestamp)
                {
                    DecodeVideoPacket(&pkt, candidateFrame);
                    gotFrame = true;
                }
            }
        }

        if (gotFrame)
        {
            m_seekFrame = candidateFrame;
        }

        return true;
    }

    return false;
}

bool IsIPacket(const AVPacket& pkt)
{
    return (pkt.flags & AV_PKT_FLAG_KEY) != 0;
}

bool CFileImgSrc::IsVideoPacket(const AVPacket* pkt) const
{
    return pkt->stream_index == m_video_stream;
}

bool CFileImgSrc::IsAudioPacket(const AVPacket* pkt) const
{
    return pkt->stream_index == m_audio_stream;
}

bool CFileImgSrc::NeedToDropPacket(int64_t currentTime, const AVPacket* pkt) const
{
    int64_t threshold = IsVideoPacket(pkt) ? VIDEO_PACKET_DROP_THRESHOLD : AUDIO_PACKET_DROP_THRESHOLD;
    return (currentTime >= GetPacketTimestamp(pkt) + threshold);
}

int64_t CFileImgSrc::GetPacketTimestamp(const AVPacket* pkt) const
{
    return (int64_t)(pkt->dts * (IsVideoPacket(pkt) ? m_videoTimestampUnit : m_audioTimestampUnit));
}

bool CFileImgSrc::DecodeVideoPacket(const AVPacket* pkt, CRawData& rgbBuf)

{
    CAVFrameWrapper avFrameWrapper;
    int got_frame = 0;
    if (avcodec_decode_video2(m_videoCtx, avFrameWrapper.m_avFrame, &got_frame, pkt) < 0)
    {
        av_log(nullptr, AV_LOG_ERROR, "Error decoding video frame\n");
        return false;
    }

    if (got_frame)
    {
        int rgbLineSize[1] = { 3 * m_videoCtx->width };
        uint8_t* pRgbBuf = &rgbBuf.m_data[0];
        sws_scale(m_swsCtx, avFrameWrapper.m_avFrame->data, avFrameWrapper.m_avFrame->linesize, 
            0, m_videoCtx->height, &pRgbBuf, rgbLineSize);
        rgbBuf.m_timestamp = GetPacketTimestamp(pkt);
        
        return true;
    }

    return false;
}

int CFileImgSrc::DecodeAudioPacket(AVPacket* pkt, CRawData& data)
{
    CAVFrameWrapper avFrameWrapper;
    int got_frame = 0;
    int ret = avcodec_decode_audio4(GetAudioCtx(), avFrameWrapper.m_avFrame, &got_frame, pkt);
    if (ret < 0)
    {
        av_log(nullptr, AV_LOG_ERROR, "Error decoding audio frame\n");
        return ret;
    }

    if (got_frame)
    {
        int64_t timestamp = GetPacketTimestamp(pkt);
        size_t bytesPerSample = av_get_bytes_per_sample((AVSampleFormat)avFrameWrapper.m_avFrame->format);
        size_t bytesPerBuffer = bytesPerSample * avFrameWrapper.m_avFrame->nb_samples;
        size_t rawSize = bytesPerBuffer * GetAudioCtx()->channels;
        data = CRawData(std::vector<unsigned char>(rawSize), timestamp, true);
        for (int i = 0; i < GetAudioCtx()->channels; ++i)
        {
            memcpy((&data.m_data[0]) + bytesPerBuffer * i, avFrameWrapper.m_avFrame->data[i], bytesPerBuffer);
        }

        m_outAudioFmt = AV_SAMPLE_FMT_S16;// output is fixed to 16 bits and non-planar
        if (m_outAudioFmt != (AVSampleFormat)avFrameWrapper.m_avFrame->format)// do audio format translate
        {
            if (!m_audioFmtTranslator.IsReady())
            {
                m_audioFmtTranslator.Init(
                    GetAudioCtx()->channel_layout, GetAudioCtx()->sample_rate, (AVSampleFormat)avFrameWrapper.m_avFrame->format,
                    GetAudioCtx()->channel_layout, GetAudioCtx()->sample_rate, m_outAudioFmt);
            }

            std::vector<unsigned char> dstData;
            m_audioFmtTranslator.Translate(data.m_data, GetAudioCtx()->channels, dstData);
            data.m_data = dstData;
        }

        return ret;
    }

    return -1;
}

void CFileImgSrc::DecodeThreadFn(CFileImgSrc* pThis)
{
    const int rgbBufSize = av_image_get_buffer_size(pThis->m_imgOutFormat, pThis->m_videoCtx->width, pThis->m_videoCtx->height, 16);
    CRawData rgbBuf(std::vector<uint8_t>(rgbBufSize), 0, false);
    const int outBufSize = pThis->m_videoCtx->width * pThis->m_videoCtx->height * 3;
    bool eof = false;
    bool waitForI = true;

    while (pThis->m_startDecodeThread)
    {
        if (eof || !waitForI)// if waiting for I, no sleep to catch up with current time
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

        if (pThis->CheckAndDoSeek())
        {
            eof = false;
        }

        if (eof || 
            !(pThis->m_status == CImgSrc::Status_Play || pThis->m_status == CImgSrc::Status_Pause) || 
            pThis->m_rawDataQueue.IsQueueFull())
        {
            continue;
        }

        CAVPacketWrapper pktWrapper;
        AVPacket& pkt = pktWrapper.pkt;
        if (av_read_frame(pThis->m_fmt_ctx, &pkt) < 0)
        {
            rgbBuf.m_isAudio = false;
            rgbBuf.m_timestamp = -1;
            pThis->m_rawDataQueue.PutData(rgbBuf);// to notify eof or error
            eof = true;
            continue;
        }

        if (pThis->NeedToDropPacket(pThis->m_timeInfo.CurrentTime(), &pkt))
        {
            if (pkt.stream_index == pThis->m_video_stream)
            {
                waitForI = true;
            }
            continue;
        }

        if (pThis->IsVideoPacket(&pkt))
        {
            if (IsIPacket(pkt) || pThis->m_speed < pThis->m_onlyISpeedBoundary)// speed > m_onlyISpeedBoundary, only key frame
            {
                if (pThis->DecodeVideoPacket(&pkt, rgbBuf))
                {
                    waitForI = false;
                    pThis->m_rawDataQueue.PutData(rgbBuf);
                }
            }
        }
        else if (pThis->IsAudioPacket(&pkt))
        {
            if (!waitForI && pThis->NeedToHandleAudio())
            {
                while (pkt.size > 0)
                {
                    CRawData data;
                    int ret = pThis->DecodeAudioPacket(&pkt, data);
                    if (ret < 0)
                    {
                        break;
                    }
                    
                    pThis->m_rawDataQueue.PutData(data);
                    // some audio codec may decode a packet in several times.
                    pkt.size -= ret;
                    pkt.data += ret;
                }
            }
        }
    }
}

bool CFileImgSrc::NeedToHandleAudio() const
{
    return (m_status == CImgSrc::Status_Play && m_speed == 0);
}

bool IsPlanarAudioFmt(AVSampleFormat fmt)
{
    switch (fmt)
    {
    case AV_SAMPLE_FMT_U8P:
    case AV_SAMPLE_FMT_S16P:
    case AV_SAMPLE_FMT_S32P:
    case AV_SAMPLE_FMT_FLTP:
    case AV_SAMPLE_FMT_DBLP:
        return true;
    default:
        return false;
    }
}

void CFileImgSrc::CVideoTimeInfo::SetVideoDuration(int64_t duration)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_videoDuration = duration;
}

void CFileImgSrc::CVideoTimeInfo::UpdateTime(int speed)
{
    long long currentTime = std::chrono::duration_cast<std::chrono::milliseconds>
        (std::chrono::system_clock::now().time_since_epoch()).count();
    
    constexpr int64_t updateTimeThreshold = 10;
    std::lock_guard<std::mutex> lock(m_mutex);
    if (currentTime - m_baseTime > updateTimeThreshold)
    {
        int64_t diff = (int64_t)((currentTime - m_baseTime) * std::pow(2.0, speed));
        if (diff > 0)
        {
            if (m_baseTime != 0)
            {
                m_curTimestamp += diff;
            }
            m_baseTime = currentTime;
        }
    }
}

bool CFileImgSrc::CVideoTimeInfo::IsOutOfDate(int64_t timestamp)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return (m_curTimestamp >= timestamp);
}

int64_t CFileImgSrc::CVideoTimeInfo::CurrentTime()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_curTimestamp;
}

void CFileImgSrc::CVideoTimeInfo::SetCurrentTime(int64_t timestamp)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_curTimestamp = timestamp;
}

void CFileImgSrc::CVideoTimeInfo::ResetBaseTime()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_baseTime = 0;
}

void CFileImgSrc::CallbackThreadFn(CFileImgSrc* pThis)
{
    if (pThis->m_callbackFn == nullptr)
    {
        return;
    }

    int width = 0;
    int height = 0;
    pThis->GetImageResolution(width, height);
    int samplesPerSecond = 0;
    int bitsPerSample = 4;
    if (pThis->GetAudioCtx() != nullptr)
    {
        samplesPerSecond = pThis->GetAudioCtx()->sample_rate;
    }

    CRawData rawData;
    while (pThis->m_startCallbackThread)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        if (!pThis->m_seekFrame.m_data.empty())
        {
            pThis->m_callbackFn((int)pThis->m_seekFrame.m_timestamp, pThis->m_seekFrame.m_data, width, height, 
                pThis->m_seekFrame.m_isAudio, pThis->m_callbackParam);
            pThis->m_seekFrame.Clear();
            rawData.Clear();
        }

        if (pThis->m_status == CImgSrc::Status_Play && pThis->m_rawDataQueue.IsBufferingReady())
        {
            pThis->m_timeInfo.UpdateTime(pThis->m_speed);
        }

        if (rawData.m_data.empty())
        {
            pThis->m_rawDataQueue.GetData(rawData);
        }
        
        if (!rawData.m_data.empty())
        {
            if (pThis->m_timeInfo.IsOutOfDate(rawData.m_timestamp))
            {
                if (rawData.m_isAudio)
                {
                    if (pThis->NeedToHandleAudio())
                    {
                        pThis->m_callbackFn((size_t)rawData.m_timestamp, rawData.m_data, samplesPerSecond, bitsPerSample, rawData.m_isAudio, pThis->m_callbackParam);
                    }
                }
                else
                {
                    pThis->m_callbackFn((size_t)rawData.m_timestamp, rawData.m_data, width, height, rawData.m_isAudio, pThis->m_callbackParam);
                }
                
                rawData.Clear();
            }
        }
    }
}

bool CFileImgSrc::GetImageResolution(int & width, int & height) const
{
    if (m_imgRes.cx == -1 && m_imgRes.cy == -1)
    {
        return false;
    }

    width = m_imgRes.cx;
    height = m_imgRes.cy;

    return true;
}

bool CFileImgSrc::GetImageResolution(CSize& imgRes) const
{
    if (m_videoCtx == nullptr)
    {
        return false;
    }

    imgRes.cx = m_videoCtx->width;
    imgRes.cy = m_videoCtx->height;

    return true;
}

bool CFileImgSrc::GetAudioInfo(unsigned short& channelNumber, unsigned short& bitsPerSample, unsigned int& samplesPerSec) const
{
    if (m_audioInfo.IsValid())
    {
        channelNumber = m_audioInfo.channelNumber;
        bitsPerSample = m_audioInfo.bitsPerSample;
        samplesPerSec = m_audioInfo.samplesPerSec;

        return true;
    }

    return false;
}

bool CFileImgSrc::GetAudioInfo(CAudioInfo& audioInfo) const
{
    if (GetAudioCtx() == nullptr)
    {
        return false;
    }

    // sample_fmt in audio ctx may not the actual format, sometimes it is fmt1 before calling avcodec_open2, and
    // becomes fmt2 after calling avcodec_open2. the actual format is in AVFrame->format after calling 
    // avcodec_decode_audio4. sample_fmt in audio ctx is only as information shown to ui before decoding.
    audioInfo.channelNumber = (unsigned short)GetAudioCtx()->channels;
    audioInfo.bitsPerSample = (unsigned short)(av_get_bytes_per_sample(m_outAudioFmt) * 8);
    audioInfo.samplesPerSec = (unsigned int)GetAudioCtx()->sample_rate;

    return true;
}

CFileImgSrc::CAudioFormatTranslator::CAudioFormatTranslator()
    : swr_ctx(nullptr), srcBytePerSample(0), dstBytePerSample(0), 
      srcAudioFmt(AV_SAMPLE_FMT_NONE), dstAudioFmt(AV_SAMPLE_FMT_NONE)
{
    
}

CFileImgSrc::CAudioFormatTranslator::~CAudioFormatTranslator()
{
    Uninit();
}

void CFileImgSrc::CAudioFormatTranslator::Uninit()
{
    if (swr_ctx != nullptr)
    {
        swr_free(&swr_ctx);
        swr_ctx = nullptr;
    }
}

bool CFileImgSrc::CAudioFormatTranslator::IsReady() const
{
    return (swr_ctx != nullptr);
}

void CFileImgSrc::CAudioFormatTranslator::Init(int64_t srcChLayout, unsigned int srcSamplesPerSec, AVSampleFormat srcFmt,
    int64_t dstChLayout, unsigned int dstSamplesPerSec, AVSampleFormat dstFmt)
{
    Uninit();

    swr_ctx = swr_alloc();

    av_opt_set_int(swr_ctx, "in_channel_layout", srcChLayout, 0);
    av_opt_set_int(swr_ctx, "in_sample_rate", srcSamplesPerSec, 0);
    av_opt_set_sample_fmt(swr_ctx, "in_sample_fmt", srcFmt, 0);
    srcAudioFmt = srcFmt;
    srcBytePerSample = (unsigned short)(av_get_bytes_per_sample(srcFmt));

    av_opt_set_int(swr_ctx, "out_channel_layout", dstChLayout, 0);
    av_opt_set_int(swr_ctx, "out_sample_rate", dstSamplesPerSec, 0);
    av_opt_set_sample_fmt(swr_ctx, "out_sample_fmt", dstFmt, 0);
    dstAudioFmt = dstFmt;
    dstBytePerSample = (unsigned short)(av_get_bytes_per_sample(dstFmt));

    if (swr_init(swr_ctx) < 0)
    {
        av_log(nullptr, AV_LOG_ERROR, "Error, init sw resample failed\n");
        Uninit();
    }
}

bool CFileImgSrc::CAudioFormatTranslator::Translate(const std::vector<unsigned char>& src, int channels, std::vector<unsigned char>& dst)
{
    if (swr_ctx == nullptr)
    {
        return false;
    }

    const size_t samples = src.size() / srcBytePerSample;
    const size_t samplesPerChannel = samples / channels;
    dst.resize(dstBytePerSample * samples);

    std::vector<uint8_t*> srcData, dstData;
    if (IsPlanarAudioFmt(srcAudioFmt))
    {
        for (int i = 0; i < channels; ++i)
        {
            srcData.push_back((uint8_t*)&src[i * (int)src.size() / channels]);
        }
    }
    else
    {
        srcData.push_back((uint8_t*)&src[0]);
    }

    // output is non-planar
    dstData.push_back((uint8_t*)&dst[0]);
    
    if (swr_convert(swr_ctx, &dstData[0], samples, (const uint8_t**)&srcData[0], samplesPerChannel) < 0)
    {
        av_log(nullptr, AV_LOG_ERROR, "Error, translate audio format failed\n");
        return false;
    }

    return true;
}

CImgSrc::Status CFileImgSrc::GetStatus() const
{
    return m_status;
}

bool CFileImgSrc::RegisterCallbackFn(imgCallbackFn fn, void* pParam)
{
    m_callbackFn = fn;
    m_callbackParam = pParam;

    return true;
}

int CFileImgSrc::GetImgLength() const
{
    return (m_isReady ? (int)m_timeDuration : -1);
}

bool CFileImgSrc::SeekTo(int timestamp)
{
    if (!m_isReady)
    {
        return false;
    }

    if (timestamp < 0)
    {
        timestamp = 0;
    }

    if (timestamp > m_timeDuration)
    {
        timestamp = m_timeDuration;
    }

    std::lock_guard<std::mutex> lock(m_seekMutex);
    m_timestampToSeek = timestamp;

    return true;
}

CFileImgSrc::CRawData::CRawData()
{
    Clear();
}

CFileImgSrc::CRawData::CRawData(const std::vector<unsigned char>& data, int64_t timestamp, bool isAudio)
    : m_data(data), m_timestamp(timestamp), m_isAudio(isAudio)
{
}

void CFileImgSrc::CRawData::Clear()
{
    m_data.clear();
    m_timestamp = 0;
    m_isAudio = false;
}

CFileImgSrc::CRawDataQueue::CRawDataQueue()
    : m_frameQueueMaxTimeDuration(1000), m_readyToGet(false)
{
}

int64_t CFileImgSrc::CRawDataQueue::GetQueueTimeDuration()
{
    if (m_queue.size() > 1)
    {
        return m_queue.back().m_timestamp - m_queue.front().m_timestamp;
    }
    else
    {
        return 0;
    }
}

bool CFileImgSrc::CRawDataQueue::PutData(const CRawData& inData)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (GetQueueTimeDuration() < m_frameQueueMaxTimeDuration)
    {
        auto iter = m_queue.rbegin();
        for (; iter != m_queue.rend(); ++iter)
        {
            if (inData.m_timestamp > iter->m_timestamp)
            {
                break;
            }
        }

        m_queue.insert(iter.base(), inData);

        if (!m_readyToGet &&
            GetQueueTimeDuration() >= m_frameQueueMaxTimeDuration)
        {
            m_readyToGet = true;
        }

        return true;
    }
    else
    {
        return false;
    }

}

bool CFileImgSrc::CRawDataQueue::GetData(CRawData& outImg)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!m_readyToGet || m_queue.empty())
    {
        return false;
    }

    outImg = m_queue.front();
    m_queue.pop_front();
    if (m_queue.empty())
    {
        m_readyToGet = false;
    }

    return true;
}

void CFileImgSrc::CRawDataQueue::EmptyQueue()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_queue.clear();
    m_readyToGet = false;
}

bool CFileImgSrc::CRawDataQueue::IsQueueFull()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return (GetQueueTimeDuration() >= m_frameQueueMaxTimeDuration);
}

bool CFileImgSrc::CRawDataQueue::IsBufferingReady()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_readyToGet;
}

bool CFileImgSrc::SetSpeed(int speed)
{
    if ((m_status != CImgSrc::Status_Play && m_status != CImgSrc::Status_Pause) || 
        (speed > m_maxSpeed || speed < m_minSpeed))
    {
        return false;
    }

    if (m_speed == speed)
    {
        return true;
    }

    m_speed = speed;
    if (NeedToHandleAudio())// to prevent audio/video non sync at playing
    {
        m_rawDataQueue.EmptyQueue();
    }

    return true;
}

bool CFileImgSrc::SetSpeedRange(int max, int min)
{
    m_maxSpeed = max;
    m_minSpeed = min;
    return true;
}

bool CFileImgSrc::SetOnlyIBoundary(int boundarySpeed)
{
    m_onlyISpeedBoundary = boundarySpeed;
    return true;
}

int CFileImgSrc::GetSpeed() const
{
    return m_speed;
}

bool CFileImgSrc::Play()
{
    m_status = CImgSrc::Status_Play;

    if (m_pCallbackThread != nullptr)
    {
        return true;
    }

    if (m_imgFilename.empty())
    {
        return false;
    }

    Init();

    return true;
}

bool CFileImgSrc::Stop()
{
    m_status = CImgSrc::Status_Stop;

    Uninit();
    m_rawDataQueue.EmptyQueue();
    m_speed = 0;

    return true;
}

bool CFileImgSrc::Pause()
{
    m_status = CImgSrc::Status_Pause;
    m_timeInfo.ResetBaseTime();
    return true;
}