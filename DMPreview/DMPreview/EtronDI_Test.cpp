
// EtronDI_Test.cpp : 定義應用程式的類別行為。
//

#include "stdafx.h"
#include "math.h"
#include "EtronDI_Test.h"
#include "EtronDI_TestDlg.h"
#include "AutoModuelSyncDlg.h"

#include <dbghelp.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

typedef  BOOL(*MINIDUMPWRITEDUMP)( HANDLE                            hProcess,
                                   DWORD                             ProcessId,
                                   HANDLE                            hFile,
                                   MINIDUMP_TYPE                     DumpType,
                                   PMINIDUMP_EXCEPTION_INFORMATION   ExceptionParam,
                                   PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
                                   PMINIDUMP_CALLBACK_INFORMATION    CallbackParam );

LONG CEtronDI_TestApp::TopLevelFilter( struct _EXCEPTION_POINTERS *pExceptionInfo )
{
	LONG    retval  = EXCEPTION_CONTINUE_SEARCH;
	HWND    hParent = NULL;	
	HMODULE hDll    = ::LoadLibrary( _T( "dbghelp.DLL" ) );
 
	if ( !hDll ) return NULL;
 
	MINIDUMPWRITEDUMP pDump = ( MINIDUMPWRITEDUMP )::GetProcAddress( hDll, "MiniDumpWriteDump" );
 
	if ( pDump )
	{
        wchar_t szPath[ MAX_PATH ] = { NULL };

        ::GetModuleFileName( NULL, szPath, MAX_PATH );

        wcscpy( wcsrchr( szPath, '\\' ), L"\\EtronDI_App.dmp" );

		HANDLE hFile = ::CreateFile( szPath, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
 
		if ( hFile != INVALID_HANDLE_VALUE )
		{
			_MINIDUMP_EXCEPTION_INFORMATION ExInfo;
 
			ExInfo.ThreadId          = ::GetCurrentThreadId();
			ExInfo.ExceptionPointers = pExceptionInfo;
			ExInfo.ClientPointers    = NULL;
 
			pDump( GetCurrentProcess(), GetCurrentProcessId(), hFile, MiniDumpNormal, &ExInfo, NULL, NULL );
 
			::CloseHandle(hFile);
        }
	}
	return ::FreeLibrary( hDll );
}

CEtronDI_TestApp theApp;

BEGIN_MESSAGE_MAP(CEtronDI_TestApp, CWinAppEx)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()

CEtronDI_TestApp::CEtronDI_TestApp()
{
}

BOOL CEtronDI_TestApp::InitInstance()
{
	::SetUnhandledExceptionFilter( TopLevelFilter );

	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 設定要包含所有您想要用於應用程式中的
	// 通用控制項類別。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();

	AfxEnableControlContainer();

	// 標準初始設定
	// 如果您不使用這些功能並且想減少
	// 最後完成的可執行檔大小，您可以
	// 從下列程式碼移除不需要的初始化常式，
	// 變更儲存設定值的登錄機碼
	// TODO: 您應該適度修改此字串
	// (例如，公司名稱或組織名稱)
	SetRegistryKey(_T("本機 AppWizard 所產生的應用程式"));

	AutoModuelSyncDlg autoModuleSyncDlg;	
	autoModuleSyncDlg.DoModal();

	CEtronDI_TestDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: 在此放置於使用 [確定] 來停止使用對話方塊時
		// 處理的程式碼
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 在此放置於使用 [取消] 來停止使用對話方塊時
		// 處理的程式碼
	}
	_CrtSetReportMode( _CRT_ERROR, _CRTDBG_MODE_DEBUG );

	return FALSE;
}

//
//===============================================================
/*
void YUY2_to_RGB24_Convert(BYTE *pSrc, BYTE *pDst, int cx, int cy) 
{
	int nSrcBPS,nDstBPS,x,y,x2,x3,m;
	int ma0,mb0,m02,m11,m12,m21;
	BYTE *pS0,*pD0;
	int Y,U,V,Y2;
	BYTE R,G,B,R2,G2,B2;
	//
	nSrcBPS = cx*2;
	nDstBPS = ((cx*3+3)/4)*4;
	//
	pS0 = pSrc;
	pD0 = pDst+nDstBPS*(cy-1);
	for (y=0; y<cy; y++) {
		for (x3=0,x2=0,x=0; x<cx; x+=2,x2+=4,x3+=6) {
			Y = (int)pS0[x2+0]-16;
			Y2= (int)pS0[x2+2]-16;
			U = (int)pS0[x2+1]-128;
			V = (int)pS0[x2+3]-128;
			//
			ma0 = 298*Y;
			mb0 = 298*Y2;
			m02 =  409*V+128;
			m11 = -100*U;
			m12 = -208*V+128;
			m21 =  516*U+128;
			//
			m = (ma0    +m02)>>8;
			R = (m<0)?(0):(m>255)?(255):((BYTE)m);
			m = (ma0+m11+m12)>>8;
			G = (m<0)?(0):(m>255)?(255):((BYTE)m);
			m = (ma0+m21    )>>8;
			B = (m<0)?(0):(m>255)?(255):((BYTE)m);
			//
			m = (mb0    +m02)>>8;
			R2 = (m<0)?(0):(m>255)?(255):((BYTE)m);
			m = (mb0+m11+m12)>>8;
			G2 = (m<0)?(0):(m>255)?(255):((BYTE)m);
			m = (mb0    +m21)>>8;
			B2 = (m<0)?(0):(m>255)?(255):((BYTE)m);
			//
			pD0[x3  ] = B;
			pD0[x3+1] = G;
			pD0[x3+2] = R;
			pD0[x3+3] = B2;
			pD0[x3+4] = G2;
			pD0[x3+5] = R2;
		}
		pS0 += nSrcBPS;
		pD0 -= nDstBPS;
	}
}

void HSV_to_RGB(double H, double S, double V, double &R, double &G, double &B)
{
	double nMax,nMin;
	double fDet;
	//
	while (H<0.0) H+=360.0;
	while (H>=360.0) H-=360.0;
	H /= 60.0;
	if (V<0.0) V = 0.0;
	if (V>1.0) V = 1.0;
	V *= 255.0;
	if (S<0.0) S = 0.0;
	if (S>1.0) S = 1.0;
	//
	if (V == 0.0) {
		R = G = B = 0;
	} else {
		fDet = S*V;
		nMax = (V);
		nMin = (V-fDet);
		if (H<=1.0) { //R>=G>=B, H=(G-B)/fDet
			R = nMax;
			B = nMin;
			G = (H*fDet+B);
		} else if (H<=2.0) { //G>=R>=B, H=2+(B-R)/fDet
			G = nMax;
			B = nMin;
			R = ((2.0-H)*fDet+B);
		} else if (H<=3.0) { //G>=B>=R, H=2+(B-R)/fDet
			G = nMax;
			R = nMin;
			B = ((H-2.0)*fDet+R);
		} else if (H<=4.0) { //B>=G>=R, H=4+(R-G)/fDet
			B = nMax;
			R = nMin;
			G = ((4.0-H)*fDet+R);
		} else if (H<=5.0) { //B>=R>=G, H=4+(R-G)/fDet
			B = nMax;
			G = nMin;
			R = ((H-4.0)*fDet+G);
		} else { // if(H<6.0) //R>=B>=G, H=(G-B)/fDet+6
			R = nMax;
			G = nMin;
			B = ((6.0-H)*fDet+G);
		}
	}
}


void SetBaseColorPaletteD11(RGBQUAD *pColorPaletteD11)
{
	int i;
	double R,G,B;
	//
	for (i=0; i<2048; i++) {
		HSV_to_RGB((2047.0-i)/8,1.0,1.0,R,G,B);
		pColorPaletteD11[i].rgbBlue		= (BYTE)B;
		pColorPaletteD11[i].rgbGreen		= (BYTE)G;
		pColorPaletteD11[i].rgbRed		= (BYTE)R;
		pColorPaletteD11[i].rgbReserved	= 0;
	}
	{
		i = 0;
		pColorPaletteD11[i].rgbBlue		= (BYTE)0;
		pColorPaletteD11[i].rgbGreen		= (BYTE)0;
		pColorPaletteD11[i].rgbRed		= (BYTE)0;
		pColorPaletteD11[i].rgbReserved	= 0;
	}
	{
		i = 2047;
		pColorPaletteD11[i].rgbBlue		= (BYTE)255;
		pColorPaletteD11[i].rgbGreen		= (BYTE)255;
		pColorPaletteD11[i].rgbRed		= (BYTE)255;
		pColorPaletteD11[i].rgbReserved	= 0;
	}
}

void SetBaseGrayPaletteD11(RGBQUAD *pGrayPaletteD11)
{
	int i;
	double R,G,B;
	//
	for (i=0; i<2048; i++) {
		HSV_to_RGB((2047.0-i)/8,1.0,1.0,R,G,B);
		pGrayPaletteD11[i].rgbBlue		= (BYTE)B;
		pGrayPaletteD11[i].rgbGreen		= (BYTE)B;
		pGrayPaletteD11[i].rgbRed			= (BYTE)B;
		pGrayPaletteD11[i].rgbReserved	= 0;
	}
	{
		i = 0;
		pGrayPaletteD11[i].rgbBlue		= (BYTE)0;
		pGrayPaletteD11[i].rgbGreen		= (BYTE)0;
		pGrayPaletteD11[i].rgbRed			= (BYTE)0;
		pGrayPaletteD11[i].rgbReserved	= 0;
	}
	{
		i = 2047;
		pGrayPaletteD11[i].rgbBlue		= (BYTE)255;
		pGrayPaletteD11[i].rgbGreen		= (BYTE)255;
		pGrayPaletteD11[i].rgbRed			= (BYTE)255;
		pGrayPaletteD11[i].rgbReserved	= 0;
	}
}

void SetBaseColorPaletteZ14(RGBQUAD *pColorPaletteZ14)
{
	int i;
	double R,G,B;
	double fx,fy;
	//
	double fCV = 180;
	int nCenter=1500;
	double r1=0.35;
	double r2=0.55;
	//
	for (i=1; i<16384; i++) {
		if (i==nCenter) {
			fy = fCV;
		} else if (i<nCenter) {
			fx = (double)(nCenter-i)/nCenter;
			fy = fCV - pow(fx, r1)*fCV;
		} else {
			fx = (double)(i-nCenter)/(16384-nCenter);
			fy = fCV + pow(fx, r2)*(256-fCV);
		}
		HSV_to_RGB(fy,1.0,1.0,R,G,B);
		pColorPaletteZ14[i].rgbBlue		= (BYTE)B;
		pColorPaletteZ14[i].rgbGreen		= (BYTE)G;
		pColorPaletteZ14[i].rgbRed		= (BYTE)R;
		pColorPaletteZ14[i].rgbReserved	= 0;
	}
	{
		i = 0;
		pColorPaletteZ14[i].rgbBlue		= (BYTE)0;
		pColorPaletteZ14[i].rgbGreen		= (BYTE)0;
		pColorPaletteZ14[i].rgbRed		= (BYTE)0;
		pColorPaletteZ14[i].rgbReserved	= 0;
	}
	{
		i = 16383;
		pColorPaletteZ14[i].rgbBlue		= (BYTE)255;
		pColorPaletteZ14[i].rgbGreen		= (BYTE)255;
		pColorPaletteZ14[i].rgbRed		= (BYTE)255;
		pColorPaletteZ14[i].rgbReserved	= 0;
	}
}

void SetBaseGrayPaletteZ14(RGBQUAD *pGrayPaletteZ14)
{
	int i;
	double R,G,B;
	double fx,fy;
	//
	double fCV = 180;
	int nCenter=1500;
	double r1=0.35;
	double r2=0.55;
	//
	for (i=1; i<16384; i++) {
		if (i==nCenter) {
			fy = fCV;
		} else if (i<nCenter) {
			fx = (double)(nCenter-i)/nCenter;
			fy = fCV - pow(fx, r1)*fCV;
		} else {
			fx = (double)(i-nCenter)/(16384-nCenter);
			fy = fCV + pow(fx, r2)*(256-fCV);
		}
		HSV_to_RGB(fy,1.0,1.0,R,G,B);
		pGrayPaletteZ14[i].rgbBlue		= (BYTE)B;
		pGrayPaletteZ14[i].rgbGreen		= (BYTE)B;
		pGrayPaletteZ14[i].rgbRed			= (BYTE)B;
		pGrayPaletteZ14[i].rgbReserved	= 0;
	}
	{
		i = 0;
		pGrayPaletteZ14[i].rgbBlue		= (BYTE)0;
		pGrayPaletteZ14[i].rgbGreen		= (BYTE)0;
		pGrayPaletteZ14[i].rgbRed			= (BYTE)0;
		pGrayPaletteZ14[i].rgbReserved	= 0;
	}
	{
		i = 16383;
		pGrayPaletteZ14[i].rgbBlue		= (BYTE)255;
		pGrayPaletteZ14[i].rgbGreen		= (BYTE)255;
		pGrayPaletteZ14[i].rgbRed			= (BYTE)255;
		pGrayPaletteZ14[i].rgbReserved	= 0;
	}
}


void UpdateD11DisplayImage_DIB24(RGBQUAD *pColorPaletteD11, BYTE *pDepthD11, BYTE *pDepthDIB24, int cx, int cy)
{
	int x,y,nBPS;
	WORD *pWSL,*pWS;
	BYTE *pDL,*pD;
	RGBQUAD *pClr;
	//
	if ((cx<=0) || (cy<=0)) return;
	//
	nBPS = ((cx*3+3)/4)*4;
	pWSL = (WORD*)pDepthD11;
	pDL = pDepthDIB24 + (cy-1)*nBPS;
	for (y=0; y<cy; y++) {
		pWS = pWSL;
		pD = pDL;
		for (x=0; x<cx; x++) {
			pClr = &(pColorPaletteD11[pWS[x]]);
			pD[0] = pClr->rgbBlue; //B
			pD[1] = pClr->rgbGreen; //G
			pD[2] = pClr->rgbRed; //R
			pD += 3;
		}
		pWSL += cx;
		pDL -= nBPS;
	}
}

void UpdateZ14DisplayImage_DIB24(RGBQUAD *pColorPaletteZ14, BYTE *pDepthZ14, BYTE *pDepthDIB24, int cx, int cy)
	{
	int x,y,nBPS;
	WORD *pWSL,*pWS;
	BYTE *pDL,*pD;
	RGBQUAD *pClr;
    // 
	if ((cx<=0) || (cy<=0)) return;
	//
	nBPS = ((cx*3+3)/4)*4;
	pWSL = (WORD*)pDepthZ14;
	pDL = pDepthDIB24 + (cy-1)*nBPS;
	for (y=0; y<cy; y++) {
		pWS = pWSL;
		pD = pDL;
		for (x=0; x<cx; x++) {
			pClr = &(pColorPaletteZ14[pWS[x]]);
			pD[0] = pClr->rgbBlue; //B
			pD[1] = pClr->rgbGreen; //G
			pD[2] = pClr->rgbRed; //R
			pD += 3;
		}
		pWSL += cx;
		pDL -= nBPS;
	}
}

void UpdateD11ROIDisplayImage_DIB24(RGBQUAD *pColorPaletteD11, BYTE *pDepthD11, BYTE *pDepthDIB24, int cx, int cy, int roix, int roiy, int roiw, int roih)
{
	int x,y,nBPS;
	WORD *pWSL,*pWS;
	BYTE *pDL,*pD;
	RGBQUAD *pClr;
	CPoint ptBegin; 
    CPoint ptEnd; 
	//ptBegin.x = (cx-roiw)/2;  
	//ptBegin.y = (cy-roih)/2; 
	ptBegin.x = roix;
	ptBegin.y = roiy;
	ptEnd.x   = ptBegin.x+roiw;  
	ptEnd.y   = ptBegin.y+roih;
	if ((cx<=0) || (cy<=0)) return;
	nBPS = ((cx*3+3)/4)*4;
	pWSL = (WORD*)pDepthD11;
	pDL = pDepthDIB24 + (cy-1)*nBPS;
	for (y=0; y<cy; y++) 
	{
		pWS = pWSL;
		pD = pDL;
		for (x=0; x<cx; x++) 
		{
			if (roiw>0 && roih>0)
			{
				if (x>ptBegin.x && x<ptEnd.x && y>ptBegin.y && y<ptEnd.y) 
				{
					pClr = &(pColorPaletteD11[pWS[x]]);
					pD[0] = pClr->rgbBlue;  
					pD[1] = pClr->rgbGreen;  
					pD[2] = pClr->rgbRed;  
				}
				else
				{
					pD[0] = 64; 
					pD[1] = 64; 				
					pD[2] = 64; 
				}
			}	
			else
			{
				pClr = &(pColorPaletteD11[pWS[x]]);
				pD[0] = pClr->rgbBlue;  
				pD[1] = pClr->rgbGreen;  				
				pD[2] = pClr->rgbRed;  
			}
			pD += 3;
		}
		pWSL += cx;
		pDL -= nBPS;
	}
}

void UpdateZ14ROIDisplayImage_DIB24(RGBQUAD *pColorPaletteZ14, BYTE *pDepthZ14, BYTE *pDepthDIB24, int cx, int cy, int roix, int roiy, int roiw, int roih)
{
	int x,y,nBPS;
	WORD *pWSL,*pWS;
	BYTE *pDL,*pD;
	RGBQUAD *pClr;
	CPoint ptBegin; 
    CPoint ptEnd; 
	//ptBegin.x = (cx-roiw)/2;  
	//ptBegin.y = (cy-roih)/2; 
	ptBegin.x = roix;
	ptBegin.y = roiy;
	ptEnd.x   = ptBegin.x+roiw;  
	ptEnd.y   = ptBegin.y+roih;  
	if ((cx<=0) || (cy<=0)) return;
	nBPS = ((cx*3+3)/4)*4;
	pWSL = (WORD*)pDepthZ14;
	pDL = pDepthDIB24 + (cy-1)*nBPS;
	for (y=0; y<cy; y++) 
	{
		pWS = pWSL;
		pD = pDL;
		for (x=0; x<cx; x++) 
		{
			if (roiw>0 && roih>0)
			{
				if (x>ptBegin.x && x<ptEnd.x && y>ptBegin.y && y<ptEnd.y) 
				{
					pClr = &(pColorPaletteZ14[pWS[x]]);
					pD[0] = pClr->rgbBlue; //B
					pD[1] = pClr->rgbGreen; //G
					pD[2] = pClr->rgbRed; //R
				}
				else
				{
					pD[0] = 64; 
					pD[1] = 64; 				
					pD[2] = 64; 
				}
			}	
			else
			{
				pClr = &(pColorPaletteZ14[pWS[x]]);
				pD[0] = pClr->rgbBlue; //B
				pD[1] = pClr->rgbGreen; //G
				pD[2] = pClr->rgbRed; //R
			}
			pD += 3;
		}
		pWSL += cx;
		pDL -= nBPS;
	}
}

*/