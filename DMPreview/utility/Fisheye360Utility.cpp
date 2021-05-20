#include "stdafx.h"
#include "Fisheye360Utility.h"
#include "eSPDI/eSPDI_ErrCode.h"
#include "eys_fisheye360/fisheye360_api_cc.h"

const size_t ParaLUT_SIZE = 1024;
const char* Lut_Filename = "eYsGlobeK.lut";

std::string GetLutFilename()
{
    return Lut_Filename;
}

int AppendToFileFront(const char* szFilePath, BYTE* pbuffer, int nBufferLen) 
{
    if (pbuffer == NULL)
    {
        return -1;
    }

    std::fstream file(szFilePath, std::ios::in | std::ios::out | std::ios::binary | std::ios::ate);
    if (file.good())
    {
        size_t fileSize = file.tellg();
        std::vector<char> buf(nBufferLen + fileSize, 0);
        
        memcpy(&buf[0], pbuffer, nBufferLen);
        file.seekg(0, std::ios::beg);
        file.read(&buf[nBufferLen], fileSize);
        
        file.seekp(0, std::ios::beg);
        file.write(&buf[0], nBufferLen + fileSize);

        file.close();
        return 0;
    }

    return -1;
}

bool IsLutParamConsistent(const eys::fisheye360::ParaLUT& lutParam, const std::string& filename)
{
    std::ifstream file(filename.c_str(), std::ios::in | std::ios::binary);
    if (file.good())
    {
        char buf[ParaLUT_SIZE] = {'\0'};
        file.read(buf, ParaLUT_SIZE);
        file.close();

        if (memcmp(&lutParam, buf, ParaLUT_SIZE) == 0)
        {
            return true;
        }
    }

    return false;
}

int UpdateLutFile(const eys::fisheye360::ParaLUT& lutParam, const std::string& directory)
{
    if(lutParam.file_ID_header != 2230 || 
       lutParam.file_ID_version < 4)
    {
        return ETronDI_INVALID_USERDATA;
    }

    std::string filename = directory + Lut_Filename;

    if (IsLutParamConsistent(lutParam, filename))
    {
        return ETronDI_OK;
    }

    return GenerateLutFileFromParam((eys::fisheye360::ParaLUT&)lutParam, filename);
}

int GenerateLutFileFromParam(eys::fisheye360::ParaLUT& paraLut, std::string lutFilePath)
{
#ifdef ESPDI_EG
    if (eys::fisheye360::Map_LUT_CC(paraLut, lutFilePath.c_str(), nullptr, nullptr,
        eys::LUT_LXLYRXRY_16_3, true) != 1)
    {
        return ETronDI_MAP_LUT_FAIL;
    }

    if (AppendToFileFront(lutFilePath.c_str(), (BYTE*)(&paraLut), sizeof(eys::fisheye360::ParaLUT)) != 0)
    {
        return ETronDI_APPEND_TO_FILE_FRONT_FAIL;
    }
#endif
    return ETronDI_OK;
}