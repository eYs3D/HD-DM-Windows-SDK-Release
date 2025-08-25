#pragma once
#include <windows.h>
//#include "stdafx.h"
#include <stdio.h>
#include <vector>

#ifndef APC_API
#ifdef __WEYE__
#define APC_API
#else
#ifdef APC_EXPORTS
#define APC_API __declspec(dllexport)
#else
#define APC_API __declspec(dllimport)
#endif
#endif
#endif

// Include these SDK header files.
#include "..\..\eSPDI\DM\include\eSPDI_DM.h"
#include "..\..\eSPDI\DM\include\eSPDI_Common.h"
#include "..\..\eSPDI\DM\include\eSPDI_ErrCode.h"

void Read3X();
void Write3X();
void Read4X();
void Write4X();
void Read5X();
void Write5X();
void Read24X();
void Write24X();
void ResetUNPData();
void GetUserData(); // Issue 6882

void GetColorDepthImage(bool printAll);
void GetPointCloud();
void CopyFromG1ToG2();
void SetAnalogAndDigitalGainExample();
void IMUCallbackDemo();
void IMUAPIDemo();
void ResetBootloader();
void BatchReadASIC();
void BatchReadASICSensorFW();
void InitOpenCloseReleaseLoop();

int selectDeviceIndex(void* pHandleApcDI);
int selectColorDepth(void* pHandleApcDI, DEVSELINFO devSelInfo, int* color_index, int* depth_index, int* fps);
void selectDepthType(void* pHandleApcDI, DEVSELINFO devSelInfo, int* depthType);

class FramePool
{
	public:
		FramePool() : m_width(0), m_height(0), sn(-1) {}

		int m_width;
		int m_height;
		int sn;
		APCImageType::Value m_imgType;
		std::vector<unsigned char> data;
};

class PreviewParam
{
	public:
		void* handleApcDI;
		DEVSELINFO devSelInfo;
		PointCloudInfo pointCloudInfo;
};

struct CloudPoint {
	unsigned char r;
	unsigned char g;
	unsigned char b;
	float x;
	float y;
	float z;
};

struct CallbackParam {
    bool print_log;
    bool save_file;
};