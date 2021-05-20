#pragma once

#include <vector>

typedef void(*imgCallbackFn)(size_t timestamp, const std::vector<unsigned char>& imgBuf, int width, int height, bool isAudio, void* pParam);

class CImgSrc
{
public:
    enum Status
    {
        Status_Stop = 0, 
        Status_Play = 1,
        Status_Pause = 2
    };

public:
    virtual ~CImgSrc() {};
    virtual bool IsReady() = 0;
    virtual bool GetImageResolution(int& width, int& height) const = 0;
    virtual bool GetAudioInfo(unsigned short& channelNumber, unsigned short& bitsPerSample, unsigned int& samplesPerSec) const { return false; }
    virtual CImgSrc::Status GetStatus() const = 0;
    virtual bool RegisterCallbackFn(imgCallbackFn fn, void* pParam) = 0;
    virtual int GetImgLength() const { return -1; }// in milli-seconds
    virtual bool SeekTo(int timestamp) { return false; }// in milli-seconds. will seek to the timestamp of the nearest key frame before given timestamp
    virtual bool SetSpeed(int speed) { return false; } // 2^(speed) x. ex: 2^2 = 4, 2^(-1) = 0.5
    virtual bool SetSpeedRange(int max, int min) { return false; };
    virtual bool SetOnlyIBoundary(int boundarySpeed) { return false; };// if speed >= boundarySpeed, only callback I frame
    virtual int GetSpeed() const { return 0; }
    virtual bool Play() = 0;
    virtual bool Stop() = 0;
    virtual bool Pause() { return false; }
};