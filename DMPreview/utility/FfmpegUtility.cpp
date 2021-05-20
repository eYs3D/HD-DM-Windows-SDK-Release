#include "stdafx.h"
#include "FfmpegUtility.h"
#include <sstream>


CAVPacketWrapper::CAVPacketWrapper()
{
    av_init_packet(&pkt);
}

CAVPacketWrapper::~CAVPacketWrapper()
{
    av_free_packet(&pkt);
}


CAVFrameWrapper::CAVFrameWrapper()
{
    m_avFrame = av_frame_alloc();
}

CAVFrameWrapper::~CAVFrameWrapper()
{
    av_frame_free(&m_avFrame);
    m_avFrame = nullptr;
}

std::string GetFFMpegErrorMsg(std::string failedFunction, int errCode)
{
    char msg[512] = { '\0' };
    av_make_error_string(msg, 512, errCode);
    std::ostringstream errMsg;
    errMsg << failedFunction.c_str() << " failed. (" << errCode << " " << msg << ")" << std::endl;

    return errMsg.str();
}