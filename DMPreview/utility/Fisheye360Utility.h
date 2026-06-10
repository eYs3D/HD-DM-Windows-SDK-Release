#pragma once

#include "eys_fisheye360/fisheye360_def.h"


int AppendToFileFront(const char* szFilePath, BYTE* pbuffer, int nBufferLen);
std::string GetLutFilename();
int UpdateLutFile(const eys::fisheye360::ParaLUT& lutParam, const std::string& directory);
int GenerateLutFileFromParam(eys::fisheye360::ParaLUT& paraLut, std::string lutFilePath);