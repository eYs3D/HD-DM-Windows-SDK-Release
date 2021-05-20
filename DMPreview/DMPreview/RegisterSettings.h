#pragma once

#include <eSPDI_Common.h>

class RegisterSettings {
	RegisterSettings();
    static int FramesyncFor8054(void* hEtronDI, PDEVSELINFO pDevSelInfo, int DepthWidth, int DepthHeight, int ColorWidth, int ColorHeight, bool bFormatMJPG, int Fps);
	static int FramesyncFor8040S(void* hEtronDI, PDEVSELINFO pDevSelInfo, int DepthWidth, int DepthHeight, int ColorWidth, int ColorHeight, bool bFormatMJPG, int Fps);
public:
	//static int FramesyncD0(void* hEtronDI, PDEVSELINFO pDevSelInfo, int DepthWidth, int DepthHeight, int ColorWidth, int ColorHeight, bool bFormatMJPG, int Fps);
	static int Framesync( void* hEtronDI, PDEVSELINFO pDevSelInfo, int DepthWidth, int DepthHeight, int ColorWidth, int ColorHeight, bool bFormatMJPG, int Fps, const int iPid );
    static int FrameSync8053_8059_Clock( void* hEtronDI, PDEVSELINFO pDevSelInfo );
    static int FrameSync8053_8059_Reset( void* hEtronDI, PDEVSELINFO pDevSelInfo );
	static int ForEx8053Mode9(void* hEtronDI, PDEVSELINFO pDevSelInfo);
	static int DM_Quality_Register_Setting(void* hEtronDI, PDEVSELINFO pDevSelInfo);
	static int DM_Quality_Register_Setting_For6cm(void* hEtronDI, PDEVSELINFO pDevSelInfo);
	static int DM_Quality_Register_Setting_Slave(void* hEtronDI, PDEVSELINFO pDevSelInfo);
};
