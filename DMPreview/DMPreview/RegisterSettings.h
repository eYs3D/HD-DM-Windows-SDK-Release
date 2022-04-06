#pragma once

#include <eSPDI_Common.h>

class RegisterSettings {
	RegisterSettings();
    static int FramesyncFor8054(void* hApcDI, PDEVSELINFO pDevSelInfo, int DepthWidth, int DepthHeight, int ColorWidth, int ColorHeight, bool bFormatMJPG, int Fps);
	static int FramesyncFor8040S(void* hApcDI, PDEVSELINFO pDevSelInfo, int DepthWidth, int DepthHeight, int ColorWidth, int ColorHeight, bool bFormatMJPG, int Fps);
public:
	//static int FramesyncD0(void* hApcDI, PDEVSELINFO pDevSelInfo, int DepthWidth, int DepthHeight, int ColorWidth, int ColorHeight, bool bFormatMJPG, int Fps);
	static int Framesync( void* hApcDI, PDEVSELINFO pDevSelInfo, int DepthWidth, int DepthHeight, int ColorWidth, int ColorHeight, bool bFormatMJPG, int Fps, const int iPid );
    static int FrameSync8053_8059_Clock( void* hApcDI, PDEVSELINFO pDevSelInfo );
    static int FrameSync8053_8059_Reset( void* hApcDI, PDEVSELINFO pDevSelInfo );
	static int ForEx8053Mode9(void* hApcDI, PDEVSELINFO pDevSelInfo);
	static int For8063TriggerMode(void* hApcDI, PDEVSELINFO pDevSelInfo, bool off);
	static int DM_Quality_Register_Setting(void* hApcDI, PDEVSELINFO pDevSelInfo);
	static int DM_Quality_Register_Setting_For6cm(void* hApcDI, PDEVSELINFO pDevSelInfo);
	static int DM_Quality_Register_Setting_Slave(void* hApcDI, PDEVSELINFO pDevSelInfo);
};
