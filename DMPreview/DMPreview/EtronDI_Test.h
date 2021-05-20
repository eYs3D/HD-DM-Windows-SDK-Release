
// EtronDI_Test.h : PROJECT_NAME 應用程式的主要標頭檔
//

#pragma once

#ifndef __AFXWIN_H__
	#error "對 PCH 包含此檔案前先包含 'stdafx.h'"
#endif

#include "resource.h"		// 主要符號

class CEtronDI_TestApp : public CWinAppEx
{
public:
	CEtronDI_TestApp();

	public:
	virtual BOOL InitInstance();

    static LONG __stdcall TopLevelFilter( struct _EXCEPTION_POINTERS *pExceptionInfo );

	DECLARE_MESSAGE_MAP()
};

extern CEtronDI_TestApp theApp;

//===============================================================

//void HSV_to_RGB(double H, double S, double V, double &R, double &G, double &B);
// set color palette for D11 and Z14
//void SetBaseColorPaletteD11(RGBQUAD *pColorPaletteD11);
//void SetBaseGrayPaletteD11(RGBQUAD *pGrayPaletteD11);
//void SetBaseColorPaletteZ14(RGBQUAD *pColorPaletteZ14);
//void SetBaseGrayPaletteZ14(RGBQUAD *pGrayPaletteZ14);
//void UpdateD11DisplayImage_DIB24(RGBQUAD *pColorPaletteD11, BYTE *pDepthD11, BYTE *pDepthDIB24, int cx, int cy);
//void UpdateD11ROIDisplayImage_DIB24(RGBQUAD *pColorPaletteD11, BYTE *pDepthD11, BYTE *pDepthDIB24, int cx, int cy, int roix, int roiy, int roiw, int roih);
//void UpdateZ14DisplayImage_DIB24(RGBQUAD *pColorPaletteZ14, BYTE *pDepthZ14, BYTE *pDepthDIB24, int cx, int cy);
//void UpdateZ14ROIDisplayImage_DIB24(RGBQUAD *pColorPaletteZ14, BYTE *pDepthZ14, BYTE *pDepthDIB24, int cx, int cy, int roix, int roiy, int roiw, int roih);
