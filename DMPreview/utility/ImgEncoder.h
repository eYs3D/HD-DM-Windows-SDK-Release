#pragma once
#include <string>
#include <thread>
#include <mutex>
#include <queue>
#include <vector>
#include <memory>

extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
}

struct SwsContext;

class CEncodingFrameWrapper
{
public:
    CEncodingFrameWrapper(AVCodecContext* avCodecContext);
    ~CEncodingFrameWrapper();

    AVFrame* m_avFrame = nullptr;
};


class CImgEncoder
{
public:
    //encode to h264
    CImgEncoder();
    virtual ~CImgEncoder();

    // only support AV_PIX_FMT_YUV420P, AV_PIX_FMT_YUYV422, and AV_PIX_FMT_BGR24
    bool Init(int imgWidth, int imgHeight, int frameRate, std::string filename, 
        AVPixelFormat imgFmt, bool fixedFrameRate);
    void Uninit();
    bool PutFrame(const unsigned char* imgBuf);

private:
    AVCodecContext* m_avCodecContext;
    AVFormatContext* m_avFmtContext;
    AVStream* m_avStream;
    AVFrame* m_avFrame;
    AVPacket* m_avPacket;
    SwsContext* m_swsCtx;
    AVPixelFormat m_inImgFmt;
    bool m_ready;
    DWORD m_firstFrameTimestamp;
    double m_timeUnit;// m_fixedFrameRate for per second, !m_fixedFrameRate for per millisecond
    int64_t m_frameIndex;
    bool m_fixedFrameRate;
    std::thread* m_encodeThread;
    bool m_threadStart;
    std::mutex m_queueMutex;
    std::queue<std::shared_ptr<CEncodingFrameWrapper>> m_frameQueue;

private:
    AVCodecContext* OpenAvCodecCtx(int imgWidth, int imgHeight, int frameRate, bool forceSw = false);
    void ReleaseAvCodecCtx(AVCodecContext** ppAvCodecContext);
    AVFormatContext* CreateAVFmtContext(const AVCodecContext* avCodecCtx, std::string filename);
    void ReleaseAVFmtContext(AVFormatContext** ppavFmtContext);
    void CopyImgToAvFrame(const unsigned char* imgBuf, AVFrame* frame);
    bool EncodeAVFrameToFile(AVFrame* avFrame);
    void CreateEncodeThread();
    void ReleaseEncodeThread();
    static void EncodeThreadFn(CImgEncoder* pThis);
    std::shared_ptr<CEncodingFrameWrapper> CreateEncodingFrameWrapper(const unsigned char* imgBuf);
};
