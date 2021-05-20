#include "stdafx.h"
#include "Fisheye360Helper.h"
#include "eys_fisheye360/fisheye360_api_cl.h"
#include "eys_imgproc/img_alter.h"
#include <experimental/filesystem>
#ifdef ESPDI_EG
#pragma comment(lib, "eys_world.lib")
#endif

COpenCLPlatformInitializer::COpenCLPlatformInitializer()
    : m_ready(false), m_refCount(0)
{

}

COpenCLPlatformInitializer::~COpenCLPlatformInitializer()
{
    UninitPlatform();
}

bool COpenCLPlatformInitializer::AddRef()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    
    ++m_refCount;
    if (!m_ready)
    {
        m_ready = InitPlatform();
    }

    return m_ready;
}

void COpenCLPlatformInitializer::RemoveRef()
{
    std::lock_guard<std::mutex> lock(m_mutex);

    if (m_refCount == 0)
    {
        return;
    }

    --m_refCount;
    if (m_refCount == 0 && m_ready)
    {
        UninitPlatform();
        m_ready = false;
    }
}

bool COpenCLPlatformInitializer::InitPlatform()
{
#ifdef ESPDI_EG
    int platform_total = 0;
    char platform_name[256] = { '\0' };
    char device_name[256] = { '\0' };
    if (eys::fisheye360::GPU_Get_Platform_Number_CL(platform_total) != 1 ||
        eys::fisheye360::GPU_Set_Platform_CL(0, platform_name, device_name) != 1)
    {
        OutputDebugString(_T("== OpenCL set platform failed.\n"));
        return false;
    }
    else
    {
        return true;
    }
#endif
    return false;
}

void COpenCLPlatformInitializer::UninitPlatform()
{
#ifdef ESPDI_EG
    if (eys::fisheye360::GPU_Close_Platform_CL() != 1)
    {
        OutputDebugString(_T("== OpenCL close platform failed.\n"));
    }
#endif
}


COpenCLPlatformInitializer CFisheye360Helper::m_openCLPlatformInitializer;

CFisheye360Helper::CFisheye360Helper(const std::string& lutFileName, OutputType outType)
    : m_outType(outType), m_ready(false), m_pLut(NULL), m_pColorImgBuf_Dewarping(NULL)
{
    if (!std::experimental::filesystem::exists(lutFileName))
    {
        m_errMsg = "Lut file doesn't exist.";
    }
    else if (LoadLutFromFile(lutFileName))
    {
        if (m_openCLPlatformInitializer.AddRef())
        {
            if (InitFisheye360())
            {
                m_pColorImgBuf_Dewarping = new BYTE[(size_t)m_Lut_Para.out_lut_cols * (size_t)m_Lut_Para.out_lut_rows * 3];
                m_ready = true;
            }
            else
            {
                m_openCLPlatformInitializer.RemoveRef();
                m_errMsg = "Initialize fisheye360 failed.";
            }
        }
        else
        {
            m_errMsg = "Initialize openCL platform failed.";
        }
    }
    else
    {
        m_errMsg = "Load lut file failed.";
    }
}

CFisheye360Helper::~CFisheye360Helper()
{
    if (m_pLut != NULL)
    {
        delete [] m_pLut;
    }

    if (m_pColorImgBuf_Dewarping != NULL)
    {
        delete[] m_pColorImgBuf_Dewarping;
    }

    UninitFisheye360();

    if (m_ready)
    {
        m_openCLPlatformInitializer.RemoveRef();
    }
}

bool CFisheye360Helper::LoadLutFromFile(const std::string& lutFileName)
{
#ifdef ESPDI_EG
    FILE* file = NULL;
    if (fopen_s(&file, lutFileName.c_str(), "rb") == 0)
    {
        const size_t lutParamSize = 1024;
        BYTE buf[lutParamSize] = {0};
        fread(buf, sizeof(BYTE), lutParamSize, file);
        memcpy(&m_Lut_Para, buf, lutParamSize);
        fclose(file);

        m_pLut = new BYTE[(size_t)m_Lut_Para.out_lut_cols * 2 * (size_t)m_Lut_Para.out_lut_rows * 4];
        if (eys::fisheye360::Load_LUT_CC((const int)m_Lut_Para.out_lut_cols, (const int)m_Lut_Para.out_lut_rows,
                                         lutFileName.c_str(), m_pLut, lutParamSize, true) == 1)
        {
            return true;
        }
    }
#endif
    return false;
}

bool CFisheye360Helper::IsReady(std::string* pErrMsg) const
{
    if (pErrMsg != nullptr)
    {
        *pErrMsg = m_errMsg;
    }

    return m_ready;
}

const eys::fisheye360::ParaLUT& CFisheye360Helper::LutParam() const
{
    return m_Lut_Para;
}

bool CFisheye360Helper::InitFisheye360()
{
#ifdef ESPDI_EG
    if (eys::fisheye360::GPU_Create_Buffer_LUT_CL((int)m_Lut_Para.out_lut_cols, (int)m_Lut_Para.out_lut_rows) != 1) 
    {
        OutputDebugString(_T("GPU_Create_Buffer_LUT_CL failed...\n"));
        return false;
    }    	

    if (eys::fisheye360::GPU_Create_Buffer_Img_Source_RGB_CL((int)m_Lut_Para.img_src_cols * 2, (int)m_Lut_Para.img_src_rows) != 1) 
    {
        OutputDebugString(_T("GPU_Create_Buffer_Img_Source_RGB_CL failed...\n"));
        return false;
    }	

    auto dewarpFun = (m_outType == eOutput_Rgba) ? eys::fisheye360::GPU_Create_Buffer_Img_Dewarp_RGBA_CL
        : eys::fisheye360::GPU_Create_Buffer_Img_Dewarp_RGB_CL;
    if (dewarpFun((int)m_Lut_Para.out_lut_cols, (int)m_Lut_Para.out_lut_rows) != 1)
    {
        OutputDebugString(_T("GPU_Create_Buffer_Img_Dewarp_RGB_CL failed...\n"));
        return false;
    }		

    auto blendFun = (m_outType == eOutput_Rgba) ? eys::fisheye360::GPU_Create_Buffer_Img_Blends_RGBA_CL
        : eys::fisheye360::GPU_Create_Buffer_Img_Blends_RGB_CL;
    if (blendFun((int)m_Lut_Para.out_img_cols, (int)m_Lut_Para.out_img_rows) != 1)
    {
        OutputDebugString(_T("GPU_Create_Buffer_Img_Blends_RGB_CL failed...\n"));
        return false;
    }

    if (m_outType == eOutput_Yuv422)
    {
        int ret = eys::fisheye360::GPU_Create_Buffer_Img_Blends_YUV_CL((int)m_Lut_Para.out_img_cols, (int)m_Lut_Para.out_img_rows);
        if (ret != 1)
        {
            OutputDebugString(_T("GPU_Create_Buffer_Img_Blends_YUV_CL failed...\n"));
            return false;
        }
    }

    if (eys::fisheye360::GPU_Load_Programs_CL() != 1) 
    {
        OutputDebugString(_T("GPU_Load_Programs_CL failed...\n"));
        return false;
    }	

    if (eys::fisheye360::GPU_Writes_Buffer_LUT_CL((int)m_Lut_Para.out_lut_cols, (int)m_Lut_Para.out_lut_rows, m_pLut) != 1) 
    {
        OutputDebugString(_T("GPU_Writes_Buffer_LUT_CL failed...\n"));
        return false;
    }
#endif
    return true;
}

bool CFisheye360Helper::UninitFisheye360()
{
#ifdef ESPDI_EG
    if (eys::fisheye360::GPU_Close_Buffer_CL() != 1)
    {
        OutputDebugString(_T("GPU_Close_Buffer_CL failed...\n"));
        return false;
    }
#endif
    return true;
}

bool CFisheye360Helper::DewarpAndStitch(const BYTE* pInputImg, BYTE* pOutputImg, bool bOpenCL, bool imgFlip)
{
    if (!IsReady())
    {
        return false;
    }

    //if (imgFlip)
    //{
    //    if (eys::img_alter::Img_Flip_CV((int)(m_Lut_Para.img_src_cols*2), (int)(m_Lut_Para.img_src_rows), 
    //          (BYTE*)pInputImg, 0, (BYTE*)pInputImg) != 1)
    //    {
    //        OutputDebugString(_T("Img_Flip_CV 1 failed...\n"));
    //        return false;
    //    }
    //}
#ifdef ESPDI_EG
    if (bOpenCL)
    {
        int ret = eys::fisheye360::GPU_Writes_Buffer_Img_Source_RGB_CL((int)m_Lut_Para.img_src_cols * 2,
            (int)m_Lut_Para.img_src_rows, (BYTE*)pInputImg);
        if (ret != 1)
        {
            OutputDebugString(_T("GPU_Writes_Buffer_Img_Source_RGB_CL failed...\n"));

            UninitFisheye360();
            InitFisheye360();
            if (eys::fisheye360::GPU_Writes_Buffer_Img_Source_RGB_CL((int)m_Lut_Para.img_src_cols * 2,
                (int)m_Lut_Para.img_src_rows, (BYTE*)pInputImg) != 1)
            {
                OutputDebugString(_T("GPU_Writes_Buffer_Img_Source_RGB_CL failed again...\n"));
                return false;
            }
        }  
          
        auto remapFun = (m_outType == eOutput_Rgba) ? eys::fisheye360::GPU_Run_Remap_Bilinear_RGB2RGBA_CL
            : eys::fisheye360::GPU_Run_Remap_Bilinear_RGB2RGB_CL;
        if (remapFun((int)m_Lut_Para.img_src_cols * 2, (int)m_Lut_Para.img_src_rows, (int)m_Lut_Para.out_lut_cols, 
              (int)m_Lut_Para.out_lut_rows, NULL) != 1) 
        {                                                                                                                      
            OutputDebugString(_T("GPU_Run_Remap_Bilinear_RGB2RGB_CL failed...\n"));
            return false;                                                            
        }

        auto alphaBlendingFun = (m_outType == eOutput_Rgba) ? eys::fisheye360::GPU_Run_Alpha_Blending_RGBA_CL
            : eys::fisheye360::GPU_Run_Alpha_Blending_RGB_CL;
        BYTE* outBuf = (m_outType == eOutput_Yuv422 ? m_pColorImgBuf_Dewarping : pOutputImg);
        if (alphaBlendingFun((int)m_Lut_Para.out_lut_cols, (int)m_Lut_Para.out_lut_rows, (int)m_Lut_Para.out_overlay_LR,
            (int)m_Lut_Para.out_overlay_RL, outBuf, (int)m_Lut_Para.out_lut_cols_eff) != 1)
        {
            OutputDebugString(_T("GPU_Run_Alpha_Blending_RGB_CL failed...\n"));
            return false;
        }
          
        if (m_outType == eOutput_Yuv422)
        {
            int ret = eys::fisheye360::GPU_Run_RGB_To_YUV422_CL((int)m_Lut_Para.out_img_cols,
                (int)m_Lut_Para.out_img_rows, outBuf, pOutputImg);
            if (ret != 1)
            {
                OutputDebugString(_T("GPU_Run_RGB_To_YUV422_CL failed...\n"));
                return false;
            }
        }
    }
    else
    {
        if (eys::fisheye360::Remap_Bilinear_RGB2RGB_CC((int)(m_Lut_Para.img_src_cols * 2),
              (int)(m_Lut_Para.img_src_rows), (int)(m_Lut_Para.out_lut_cols), (int)(m_Lut_Para.out_lut_rows),
              m_pLut, (BYTE*)pInputImg, m_pColorImgBuf_Dewarping) != 1)
        {
            OutputDebugString(_T("Remap_Bilinear_RGB2RGB_CC failed...\n"));
            return false;
        }

        if (eys::fisheye360::Alpha_Blending_RGB_CC((int)m_Lut_Para.out_lut_cols, (int)m_Lut_Para.out_lut_rows,
              (int)m_Lut_Para.out_overlay_LR, (int)m_Lut_Para.out_overlay_RL, m_pColorImgBuf_Dewarping,
              pOutputImg, (int)m_Lut_Para.out_lut_cols_eff) != 1)
        {
            OutputDebugString(_T("Alpha_Blending_RGB_CC failed...\n"));
            return false;
        }
    }

    if (imgFlip)
    {
        if (eys::img_alter::Img_Flip_CV((int)(m_Lut_Para.out_img_cols), (int)(m_Lut_Para.out_img_rows), 
              pOutputImg, 0, pOutputImg) != 1)
        {
            OutputDebugString(_T("Img_Flip_CV 2 failed...\n"));
            return false;
        }
    }
#endif
    return true;
}