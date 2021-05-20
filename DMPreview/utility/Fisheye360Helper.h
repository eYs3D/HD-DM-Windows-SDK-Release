#pragma once

#include "eys_fisheye360/fisheye360_def.h"

class COpenCLPlatformInitializer
{
public:
    COpenCLPlatformInitializer();
    ~COpenCLPlatformInitializer();

    bool AddRef();
    void RemoveRef();

private:
    bool m_ready;
    int m_refCount;
    std::mutex m_mutex;

private:
    bool InitPlatform();
    void UninitPlatform();
};


class CFisheye360Helper
{
public:
    enum OutputType
    {
        eOutput_Rgb = 0, 
        eOutput_Rgba,
        eOutput_Yuv422
    };

public:
    // input: rgb
    // output: rgb, rgba, yuv
    CFisheye360Helper(const std::string& lutFileName, OutputType outType);
    virtual ~CFisheye360Helper();

    bool IsReady(std::string* pErrMsg = nullptr) const;
    const eys::fisheye360::ParaLUT& LutParam() const;
    bool DewarpAndStitch(const BYTE* pInputImg, BYTE* pOutputImg, bool bOpenCL, bool imgFlip);

private:
    static COpenCLPlatformInitializer m_openCLPlatformInitializer;
    OutputType m_outType;
    bool m_ready;
    std::string m_errMsg;
    eys::fisheye360::ParaLUT m_Lut_Para;
    BYTE* m_pLut;
    BYTE* m_pColorImgBuf_Dewarping;//buffer for non-openCL

    bool LoadLutFromFile(const std::string& lutFileName);
    bool InitFisheye360();
    bool UninitFisheye360();
};