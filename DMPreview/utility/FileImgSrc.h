#pragma once
#include "ImgSrc.h"
#include <string>
#include <thread>
#include <vector>
#include <deque>
#include <mutex>
#include "libavutil/pixfmt.h"


struct AVFormatContext;
struct AVCodecContext;
struct AVFrame;
struct SwsContext;
enum AVSampleFormat;
class AVPacket;

class CFileImgSrc : public CImgSrc
{
public:
    CFileImgSrc(const std::string& filename);
    virtual ~CFileImgSrc();

    virtual void SetFile(const std::string& filename);

    virtual bool IsReady();
    virtual bool GetImageResolution(int& width, int& height) const;
    virtual bool GetAudioInfo(unsigned short& channelNumber, unsigned short& bitsPerSample, unsigned int& samplesPerSec) const;
    virtual CImgSrc::Status GetStatus() const;
    virtual bool RegisterCallbackFn(imgCallbackFn fn, void* pParam);
    virtual int GetImgLength() const;
    virtual bool SeekTo(int timestamp);
    virtual bool SetSpeed(int speed);
    virtual bool SetSpeedRange(int max, int min);
    virtual bool SetOnlyIBoundary(int boundarySpeed);
    virtual int GetSpeed() const;
    virtual bool Play();
    virtual bool Stop();
    virtual bool Pause();

private:
    class CRawData
    {
    public:
        CRawData();
        CRawData(const std::vector<unsigned char>& data, int64_t timestamp, bool isAudio);
        void Clear();

        std::vector<unsigned char> m_data;
        int64_t m_timestamp;
        bool m_isAudio;
    };

    class CRawDataQueue
    {
    public:
        CRawDataQueue();
        bool PutData(const CRawData& inData);
        bool GetData(CRawData& outImg);
        void EmptyQueue();
        bool IsQueueFull();
        bool IsBufferingReady();

    private:
        int64_t GetQueueTimeDuration();

    private:
        std::deque<CRawData> m_queue;
        std::mutex m_mutex;// std::mutex can't be lock twice in the same thread, if it's necessary using std::recursive_mutex
        const int64_t m_frameQueueMaxTimeDuration;// ms
        bool m_readyToGet;
    };

    class CVideoTimeInfo
    {
    public:
        CVideoTimeInfo() : m_videoDuration(0), m_curTimestamp(0), m_baseTime(0) {}
        void SetVideoDuration(int64_t duration);
        void UpdateTime(int speed);
        bool IsOutOfDate(int64_t timestamp);
        int64_t CurrentTime();
        void SetCurrentTime(int64_t timestamp);
        void ResetBaseTime();

    private:
        int64_t m_videoDuration;
        int64_t m_curTimestamp;
        int64_t m_baseTime;
        std::mutex m_mutex;
    };

    class CAudioFormatTranslator
    {
    public:
        CAudioFormatTranslator();
        ~CAudioFormatTranslator();
        void Init(int64_t srcChLayout, unsigned int srcSamplesPerSec, AVSampleFormat srcFmt,
            int64_t dstChLayout, unsigned int dstSamplesPerSec, AVSampleFormat dstFmt);
        bool IsReady() const;
        bool Translate(const std::vector<unsigned char>& src, int channels, std::vector<unsigned char>& dst);

    private:
        void Uninit();

    private:
        struct SwrContext* swr_ctx;
        AVSampleFormat srcAudioFmt;
        unsigned short srcBytePerSample;
        AVSampleFormat dstAudioFmt;
        unsigned short dstBytePerSample;
    };

    class CAudioInfo
    {
    public:
        CAudioInfo() : channelNumber(0), bitsPerSample(0), samplesPerSec(0) {}
        bool IsValid() const { return (channelNumber != 0 && bitsPerSample != 0 && samplesPerSec != 0); }

        unsigned short channelNumber;
        unsigned short bitsPerSample;
        unsigned short samplesPerSec;
    };

private:
    std::string m_imgFilename;
    AVPixelFormat m_imgOutFormat;
    bool m_isReady;
    imgCallbackFn m_callbackFn;
    void* m_callbackParam;
    std::thread* m_pCallbackThread;
    bool m_startCallbackThread;
    int m_fps;
    int m_speed;
    int m_maxSpeed;
    int m_minSpeed;
    int m_onlyISpeedBoundary;
    int64_t m_timeDuration;
    std::thread* m_pDecodeThread;
    bool m_startDecodeThread;
    CImgSrc::Status m_status;
    CRawDataQueue m_rawDataQueue;
    AVFormatContext* m_fmt_ctx;
    int m_video_stream;
    AVCodecContext* m_videoCtx;
    SwsContext* m_swsCtx;
    int m_audio_stream;
    CVideoTimeInfo m_timeInfo;
    int m_timestampToSeek;
    std::mutex m_seekMutex;
    AVSampleFormat m_outAudioFmt;
    CAudioFormatTranslator m_audioFmtTranslator;
    CRawData m_seekFrame;
    CSize m_imgRes;
    CAudioInfo m_audioInfo;
    double m_audioTimestampUnit;
    double m_videoTimestampUnit;

private:
    void Init();
    void Uninit();
    bool InitFfmpeg();
    bool InitFFmpegVideoStream();
    bool InitFFmpegAudioStreamIfExist();
    void UninitFfmpeg();
    AVCodecContext* GetAudioCtx() const;
    bool CheckAndDoSeek();
    static void DecodeThreadFn(CFileImgSrc* pThis);
    static void CallbackThreadFn(CFileImgSrc* pThis);
    bool NeedToHandleAudio() const;
    bool NeedToDropPacket(int64_t currentTime, const AVPacket* pkt) const;
    bool IsVideoPacket(const AVPacket* pkt) const;
    bool IsAudioPacket(const AVPacket* pkt) const;
    bool DecodeVideoPacket(const AVPacket* pkt, CRawData& rgbBuf);
    int DecodeAudioPacket(AVPacket* pkt, CRawData& data);
    bool GetAudioInfo(CAudioInfo& audioInfo) const;
    bool GetImageResolution(CSize& imgRes) const;
    int64_t GetPacketTimestamp(const AVPacket* pkt) const;
};