#pragma once
#include "Mp4FilePacketRetriever.h"
#include <thread>

typedef void(*ProgressCallbackFn)(double progress, void* param);// progress range: 0.0 ~ 1.0

struct AVStream;

class CMp4VideoCutter
{
public:
    CMp4VideoCutter(std::string srcFilename, int startTime, int endTime, std::string dstFilename, 
        ProgressCallbackFn callbackFn, void* callbackFnParam);
    virtual ~CMp4VideoCutter();

    bool IsFinished();

private:
    CMp4FilePacketRetriever m_srcPacketRetriever;
    int m_startTime = -1;// millisecond
    int m_endTime = -1;// millisecond
    std::string m_dstFilename;
    ProgressCallbackFn m_progressCallbackFn = nullptr;
    void* m_progressCallbackFnParam = nullptr;
    std::chrono::system_clock::time_point m_preCallbackTime;
    AVCodecContext* m_videoCodecContext = nullptr;
    AVCodecContext* m_audioCodecContext = nullptr;
    AVFormatContext* m_avFmtContext = nullptr;
    bool m_finished = true;
    std::thread m_threadForStopRetriever;
    double m_videoTimeUnit = 0.0;
    double m_audioTimeUnit = 0.0;
    int64_t m_videoFrameIndex = 0;
    int64_t m_baseTimestamp = 0;

private:
    static void PacketRetrieverCallback(int64_t pktTimestamp, bool audio, AVPacket* packet, void* param);
    static void StopSrcRetriever(CMp4VideoCutter* pThis);
    bool Init(std::string dstFilename);
    void Uninit();
    AVCodecContext* CreateAvCodecCtx(bool audio = false);
    void ReleaseAvCodecCtx(AVCodecContext** ppAvCodecContext);
    AVFormatContext* CreateAvFmtCtx(const AVCodecContext* videoCodecCtx, const AVCodecContext* audioCodecCtx, 
        std::string filename, double& videoTimeUnit, double& audioTimeUnit);
    void ReleaseAvFmtCtx(AVFormatContext** ppAvFmtCtx);
    AVStream* CreateNewStreamInFmtCtx(const AVCodecContext* avCodecCtx, AVFormatContext* avFmtCtx);
    void CallbackProgress(int64_t pktTimestamp);
};