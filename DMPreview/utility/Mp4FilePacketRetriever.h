#pragma once
#include <string>
#include <thread>
#include <memory>
#include <mutex>

extern "C"
{
#include "libavcodec/avcodec.h"  
}

struct AVFormatContext;

typedef void(*PacketCallbackFn)(int64_t pktTimestamp, bool audio, AVPacket* packet, void* param);

class CMp4FilePacketRetriever
{
public:
    CMp4FilePacketRetriever(std::string filename, PacketCallbackFn callbackFn, void* callbackFnParam);
    virtual ~CMp4FilePacketRetriever();

    void Start(int startTime = -1);// millisecond
    void Stop();
    void Seek(int timestamp);// millisecond

    bool GetVideoInfo(AVCodecID& format, int& width, int& height, int& frameRate, int& bitrate) const;
    bool GetAudioInfo(unsigned short& channelNumber, unsigned short& bitsPerSample, unsigned int& samplesPerSec) const;

    bool CopyAVCodecContext(AVCodecContext* avCodecContext, bool audio = false);

private:
    std::string m_filename;
    PacketCallbackFn m_callbackFn = nullptr;
    void* m_callbackFnParam = nullptr;
    AVFormatContext* m_fmt_ctx = nullptr;
    int m_video_stream = -1;
    int m_audio_stream = -1;
    std::unique_ptr<std::thread> m_thread;
    bool m_threadStart = false;
    std::mutex m_seekTimeMutex;
    int m_seekTime = -1;// millisecond

private:
    bool Init();
    void Uninit();
    static void ThreadFn(CMp4FilePacketRetriever* pThis);
    bool CheckAndDoSeek();
    bool IsVideoPacket(const AVPacket* pkt) const;
    bool IsAudioPacket(const AVPacket* pkt) const;
    AVCodecContext* GetVideoCtx() const;
    AVCodecContext* GetAudioCtx() const;
    int64_t GetPacketTimestamp(const AVPacket* pkt) const;
};