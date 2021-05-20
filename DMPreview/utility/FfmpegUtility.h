#pragma once
#include <string>

extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavutil/avutil.h"
}


class CAVPacketWrapper
{
public:
    CAVPacketWrapper();
    ~CAVPacketWrapper();

    AVPacket pkt;
};


class CAVFrameWrapper
{
public:
    CAVFrameWrapper();
    ~CAVFrameWrapper();

    AVFrame* m_avFrame = nullptr;
};

std::string GetFFMpegErrorMsg(std::string failedFunction, int errCode);