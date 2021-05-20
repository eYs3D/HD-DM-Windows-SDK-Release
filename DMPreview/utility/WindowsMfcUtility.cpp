#include "stdafx.h"
#include "WindowsMfcUtility.h"
#include <experimental/filesystem>
#include <winhttp.h>
#pragma comment(lib,"winhttp.lib")

#pragma comment(lib, "version.lib")
#pragma warning(disable:4996)

CString GetProductVersion()
{
    wchar_t filename[MAX_PATH + 1];
    const wchar_t* subBlock = L"\\";
    const char* verSeparator = ".";
    
    GetModuleFileName(NULL, filename, MAX_PATH);
    DWORD verBufSize = GetFileVersionInfoSize(filename, NULL);
    if (verBufSize > 0)
    {
        std::vector<BYTE> verBuf(verBufSize * 2);
        if (GetFileVersionInfo(filename, NULL, verBufSize, &verBuf[0]))
        {
            UINT verLen = 0;
            VS_FIXEDFILEINFO* verInfo = NULL;
            if (VerQueryValue(&verBuf[0], subBlock, reinterpret_cast<LPVOID*>(&verInfo), &verLen))
            {
                CString csVersion;

                csVersion.Format( _T( "v%d.%d.%d" ), HIWORD( verInfo->dwProductVersionMS ),
                                                     LOWORD( verInfo->dwProductVersionMS ),
                                                     HIWORD( verInfo->dwProductVersionLS ) );
                if ( LOWORD( verInfo->dwProductVersionLS ) ) csVersion.AppendFormat( _T( ".%d" ), LOWORD( verInfo->dwProductVersionLS ) );

                return csVersion;
            }
        }
    }
    return NULL;
}

CRect GetMaxRect(double rectRatio, int maxX, int maxY)
{
    CRect rect;
    rect.top = rect.left = 0;

    if ((double)maxX / maxY > rectRatio)
    {
        rect.bottom = maxY;
        rect.right = (int)(rectRatio * maxY);
    }
    else
    {
        rect.right = maxX;
        rect.bottom = (int)((double)maxX / rectRatio);
    }

    return rect;
}

CRect GetMaxRect(double innerRectRatio, CRect border, int maxX, int maxY)
{
    CRect rect = GetMaxRect(innerRectRatio, 
        maxX - (border.left + border.right), maxY - (border.top + border.bottom));
    rect.right += (border.left + border.right);
    rect.bottom += (border.top + border.bottom);

    return rect;
}

std::string GetCurrentModuleFolder()
{
    char filename[MAX_PATH] = { '\0' };
    GetModuleFileNameA(NULL, filename, MAX_PATH);
    std::string sFilename(filename);
    return sFilename.substr(0, sFilename.find_last_of("\\/"));
}

std::string GetImageFileExtension(const GUID& imgType)
{
    if (imgType == Gdiplus::ImageFormatJPEG)
    {
        return "jpg";
    }

    if (imgType == Gdiplus::ImageFormatBMP)
    {
        return "bmp";
    }

    if (imgType == Gdiplus::ImageFormatPNG)
    {
        return "png";
    }

    if (imgType == Gdiplus::ImageFormatGIF)
    {
        return "gif";
    }

    return "";
}

void SaveImage(const std::vector<unsigned char>& rgbBuf, int width, int height, int bits, const char* filename, bool flipImg, const GUID& imgType)
{
    unsigned char* pRgbBuf = nullptr;

    // workaround: save 8 bits bmp will fail, SetDIBitsToDevice return OK but values of all pixel saved are 0.
    std::vector<unsigned char> buf24bits;
    if (bits == 8)
    {
        bits = 24;
        buf24bits.resize(rgbBuf.size() * 3);
        pRgbBuf = &buf24bits[0];
        const unsigned char* pBuf = &rgbBuf[0];
        for (size_t i = 0, size = rgbBuf.size(); i < size; ++i)
        {
            pRgbBuf[i * 3] = pRgbBuf[i * 3 + 1] = pRgbBuf[i * 3 + 2] = pBuf[i];
        }
    }
    else
    {
        pRgbBuf = (unsigned char*)&rgbBuf[0];
    }

    BITMAPINFO bmpInfo;
    memset(&bmpInfo.bmiHeader, 0, sizeof(BITMAPINFOHEADER));
    bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmpInfo.bmiHeader.biPlanes = 1;
    bmpInfo.bmiHeader.biBitCount = bits;
    bmpInfo.bmiHeader.biCompression = BI_RGB;
    bmpInfo.bmiHeader.biWidth = width;
    bmpInfo.bmiHeader.biHeight = (flipImg ? height : -height);

    CImage img;
    BOOL bRet = img.Create(width, height, bits);
    int ret = SetDIBitsToDevice(img.GetDC(), 0, 0, width, height, 0, 0, 0, height, pRgbBuf, &bmpInfo, DIB_RGB_COLORS);
    img.ReleaseDC();
    
    std::string imgFilename;
    if (filename == nullptr)
    {
        std::ostringstream filename;
        filename << GetCurrentModuleFolder().c_str() << "\\Image_" << GetTickCount() 
            << GetImageFileExtension(imgType).c_str();
    }
    else
    {
        imgFilename = filename;
    }

    img.Save(CString(imgFilename.c_str()), imgType);
}

void SaveImage2(unsigned char* pRgbBuf,BITMAPINFO *bmpInfo , const char* filename,  const GUID& imgType)
{
	CImage img;
	int width  = bmpInfo->bmiHeader.biWidth;
	int height = bmpInfo->bmiHeader.biHeight;
	img.Create(width, height, 24);
	int ret = SetDIBitsToDevice(img.GetDC(), 0, 0, width, height, 0, 0, 0, height, pRgbBuf, bmpInfo, DIB_RGB_COLORS);
	img.ReleaseDC();

	std::string imgFilename;
	if (filename == nullptr)
	{
		std::ostringstream filename;
		filename << GetCurrentModuleFolder().c_str() << "\\Image_" << GetTickCount()
			<< GetImageFileExtension(imgType).c_str();
	}
	else
	{
		imgFilename = filename;
	}

	img.Save(CString(imgFilename.c_str()), imgType);
}

void SaveYuv(const std::vector<unsigned char>& rgbBuf, unsigned char Bits, int width, int height, const char* pFileName)
{
	FILE *fp;
	errno_t et;
	unsigned char* pYUVBuf = nullptr;

	pYUVBuf = (unsigned char*)&rgbBuf[0];

	et = fopen_s(&fp, pFileName, "wb");

	if (et == 0) {
		fseek(fp, 0, SEEK_SET);
		fwrite(pYUVBuf, sizeof(BYTE), width * height * Bits, fp);
		fclose(fp);
	}
}

bool CopyFileSilently(std::string srcFilename, std::string dstFolder, std::string newFilename)
{
    auto replaceSlash = [](unsigned char c) -> unsigned char { return (c == '/' ? '\\' : c); };
    std::transform(srcFilename.begin(), srcFilename.end(), srcFilename.begin(), replaceSlash);
    std::transform(dstFolder.begin(), dstFolder.end(), dstFolder.begin(), replaceSlash);

    if (dstFolder.at(dstFolder.size() - 1) != '\\')
    {
        dstFolder += "\\";
    }

    CLocaleCover localeCover(LC_CTYPE, "");

    CComPtr<IFileOperation> fileOp;
    fileOp.CoCreateInstance(CLSID_FileOperation);
    fileOp->SetOperationFlags(FOF_SILENT);
    CComPtr<IShellItem> srcFile, dstDir;
    wchar_t wsSrcFile[MAX_PATH], wsDstFolder[MAX_PATH], wsNewFile[MAX_PATH];
    std::mbstowcs(wsSrcFile, srcFilename.c_str(), MAX_PATH);
    std::mbstowcs(wsDstFolder, dstFolder.c_str(), MAX_PATH);
    std::mbstowcs(wsNewFile, newFilename.c_str(), MAX_PATH);
    if (SHCreateItemFromParsingName(wsSrcFile, NULL, IID_PPV_ARGS(&srcFile)) == S_OK &&
        SHCreateItemFromParsingName(wsDstFolder, NULL, IID_PPV_ARGS(&dstDir)) == S_OK)
    {
        fileOp->CopyItem(srcFile, dstDir, wsNewFile, NULL);
        fileOp->PerformOperations();
        return true;
    }

    return false;
}

//bool CopyFilesFolderToFolder(std::string srcFolder, std::string dstFolder, bool move)
//{
//    auto replaceSlash = [](unsigned char c) -> unsigned char { return (c == '/' ? '\\' : c); };
//    std::transform(srcFolder.begin(), srcFolder.end(), srcFolder.begin(), replaceSlash);
//    std::transform(dstFolder.begin(), dstFolder.end(), dstFolder.begin(), replaceSlash);
//    srcFolder += "\\*";
//
//    if (!std::experimental::filesystem::exists(dstFolder))
//    {
//        std::experimental::filesystem::create_directory(dstFolder);
//    }
//
//    CFileFind finder;
//    if (!finder.FindFile(srcFolder.c_str()))
//    {
//        return false;
//    }
//
//    CComPtr<IFileOperation> fileOp;
//    fileOp.CoCreateInstance(CLSID_FileOperation);
//    fileOp->SetOperationFlags(FOFX_SHOWELEVATIONPROMPT);
//
//    CLocaleCover localeCover(LC_CTYPE, "");
//
//    BOOL hasNext = true;
//    while (hasNext)
//    {
//        hasNext = finder.FindNextFile();
//        if (!finder.IsDots())
//        {
//            CComPtr<IShellItem> src, dst;
//            wchar_t wsSrcFile[MAX_PATH], wsDstFolder[MAX_PATH];
//            std::mbstowcs(wsSrcFile, finder.GetFilePath().GetBuffer(), MAX_PATH);
//            std::mbstowcs(wsDstFolder, dstFolder.c_str(), MAX_PATH);
//
//            if (SHCreateItemFromParsingName(wsSrcFile, NULL, IID_PPV_ARGS(&src)) == S_OK &&
//                SHCreateItemFromParsingName(wsDstFolder, NULL, IID_PPV_ARGS(&dst)) == S_OK)
//            {
//                if (move)
//                {
//                    fileOp->MoveItem(src, dst, 0, NULL);
//                }
//                else
//                {
//                    fileOp->CopyItem(src, dst, 0, NULL);
//                }
//            }
//        }
//    }
//
//    if (fileOp->PerformOperations() == S_OK)
//    {
//        BOOL aborted = FALSE;
//        fileOp->GetAnyOperationsAborted(&aborted);
//        return (aborted == FALSE);
//    }
//
//    return false;
//}

//bool DeleteFolder(std::string folder)
//{
//    auto replaceSlash = [](unsigned char c) -> unsigned char { return (c == '/' ? '\\' : c); };
//    std::transform(folder.begin(), folder.end(), folder.begin(), replaceSlash);
//    folder += "\\*";
//
//    CFileFind finder;
//    if (!finder.FindFile(folder.c_str()))
//    {
//        return false;
//    }
//
//    CComPtr<IFileOperation> fileOp;
//    fileOp.CoCreateInstance(CLSID_FileOperation);
//    fileOp->SetOperationFlags(FOFX_SHOWELEVATIONPROMPT);
//
//    CLocaleCover localeCover(LC_CTYPE, "");
//
//    BOOL hasNext = true;
//    while (hasNext)
//    {
//        hasNext = finder.FindNextFile();
//        if (!finder.IsDots())
//        {
//            CComPtr<IShellItem> folderToDel;
//            wchar_t wsSrcFile[MAX_PATH];
//            std::mbstowcs(wsSrcFile, finder.GetFilePath().GetBuffer(), MAX_PATH);
//
//            if (SHCreateItemFromParsingName(wsSrcFile, NULL, IID_PPV_ARGS(&folderToDel)) == S_OK)
//            {
//                fileOp->DeleteItem(folderToDel, NULL);
//            }
//        }
//    }
//
//    if (fileOp->PerformOperations() == S_OK)
//    {
//        BOOL aborted = FALSE;
//        fileOp->GetAnyOperationsAborted(&aborted);
//        return (aborted == FALSE);
//    }
//
//    return false;
//}

//bool CopyFilesFolderToFolder_MT(std::string srcFolder, std::string dstFolder, bool move)
//{
//    if (!std::experimental::filesystem::exists(srcFolder))
//    {
//        return false;
//    }
//
//    auto replaceSlash = [](unsigned char c) -> unsigned char { return (c == '/' ? '\\' : c); };
//    std::transform(srcFolder.begin(), srcFolder.end(), srcFolder.begin(), replaceSlash);
//    std::transform(dstFolder.begin(), dstFolder.end(), dstFolder.begin(), replaceSlash);
//    srcFolder += "\\*";
//
//    if (!std::experimental::filesystem::exists(dstFolder))
//    {
//        std::experimental::filesystem::create_directory(dstFolder);
//    }
//
//    SHFILEOPSTRUCT fileOp = { 0 };
//    fileOp.wFunc = (move ? FO_MOVE : FO_COPY);
//    char from[MAX_PATH + 1] = {'\0'};
//    strcpy(from, srcFolder.c_str());
//    fileOp.pFrom = from;// must double null terminated
//    char to[MAX_PATH + 1] = { '\0' };
//    strcpy(to, dstFolder.c_str());
//    fileOp.pTo = to;// must double null terminated
//    fileOp.fFlags = FOF_NOCONFIRMATION | FOF_NOCONFIRMMKDIR;
//    int ret = SHFileOperation(&fileOp);
//    if (ret != 0)
//    {
//        int errCode = GetLastError();
//        std::ostringstream msg;
//        msg << "SHFileOperation failed(err: " << errCode << "). src: " << srcFolder.c_str() << " , dst: "
//            << dstFolder << std::endl;
//        OutputDebugString(msg.str().c_str());
//    }
//
//    return (ret == 0);
//}

//bool DeleteFolder_MT(std::string folder)
//{
//    if (!std::experimental::filesystem::exists(folder))
//    {
//        return false;
//    }
//
//    auto replaceSlash = [](unsigned char c) -> unsigned char { return (c == '/' ? '\\' : c); };
//    std::transform(folder.begin(), folder.end(), folder.begin(), replaceSlash);
//    folder += "\\*";
//
//    SHFILEOPSTRUCT fileOp = { 0 };
//    fileOp.wFunc = FO_DELETE;
//    char from[MAX_PATH + 1] = { '\0' };
//    strcpy(from, folder.c_str());
//    fileOp.pFrom = from;// must double null terminated
//    fileOp.fFlags = FOF_NOCONFIRMATION | FOF_NOCONFIRMMKDIR;
//    int ret = SHFileOperation(&fileOp);
//    if (ret != 0)
//    {
//        int errCode = GetLastError();
//        std::ostringstream msg;
//        msg << "SHFileOperation delete folder failed(err: " << errCode << "). folder: " << folder.c_str() << std::endl;
//        OutputDebugString(msg.str().c_str());
//    }
//
//    return (ret == 0);
//}

void EnumChildWnd(CWnd* wnd, std::vector<CWnd*>& childWnd)
{
    childWnd.clear();
    for (CWnd* child = wnd->GetWindow(GW_CHILD); child != nullptr;
        child = child->GetWindow(GW_HWNDNEXT))
    {
        childWnd.push_back(child);
    }
}


CLocaleCover::CLocaleCover(int category, const char* locale)
    : m_category(category)
{
    std::ostringstream originLocale;
    originLocale << setlocale(category, nullptr);
    m_originLocale = originLocale.str();

    setlocale(category, locale);
}

CLocaleCover::~CLocaleCover()
{
    setlocale(m_category, m_originLocale.c_str());
}


CAutoLock::CAutoLock(CMutex& mutex)
    : m_mutex(mutex)
{
    m_mutex.Lock();
}

CAutoLock::~CAutoLock()
{
    m_mutex.Unlock();
}


IMPLEMENT_DYNAMIC(CTimelineCtrl, CSliderCtrl)

CTimelineCtrl::CTimelineCtrl()
{

}

CTimelineCtrl::~CTimelineCtrl()
{

}

BEGIN_MESSAGE_MAP(CTimelineCtrl, CSliderCtrl)
    ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

void CTimelineCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
    CSliderCtrl::OnLButtonDown(nFlags, point);

    CRect thumbRect;
    GetThumbRect(&thumbRect);
    if (!PtInRect(&thumbRect, point))
    {
        CRect chRect;
        GetChannelRect(&chRect);
        int xInChRect = point.x;
        if (xInChRect < chRect.left) xInChRect = chRect.left;
        if (xInChRect > chRect.right) xInChRect = chRect.right;

        int min, max = 0;
        GetRange(min, max);
        int pos = min + (int)((double)(max - min) * ((double)(xInChRect - chRect.left) / chRect.Width()));
        SetPos(pos);
    }
}


// class CRgb24CircularProgressImageDrawer
CRgb24CircularProgressImageDrawer::CRgb24CircularProgressImageDrawer(double ratioOfProgressToImageShortEdge,
    size_t progressDotCount, unsigned char red, unsigned char green, unsigned char blue)
    : frontDotCount(progressDotCount / 4), ratio(ratioOfProgressToImageShortEdge), dotCount(progressDotCount), currentProgress(0),
    backDotBrush(RGB(red, green, blue))
{
    dotCount = (dotCount < frontDotCount) ? frontDotCount : dotCount;
    for (int i = 0; i < (int) frontDotCount; ++i)
    {
        frontDotBrush.push_back(std::make_shared<CBrush>(RGB(255 - 20 * i, 255 - 20 * i, 255 - 20 * i)));
    }
}

void CRgb24CircularProgressImageDrawer::DrawProgressImage(CDC& dc, size_t width, size_t height)
{
    size_t radius = (size_t)((double)((width > height) ? height : width) * ratio / 2);
    CPoint center((int) width / 2, (int) height / 2);
    const float pi = (float) std::acos(-1);
    size_t dotRadius = (size_t)((float)radius * 2 / (float)dotCount);
    for (size_t i = 0; i < dotCount; ++i)
    {
        float angle = (float)(i + 1) / dotCount * 2 * pi - (float) 0.5 * pi;
        CPoint drawPos(center.x + (int)((float)radius * std::cos(angle)),
            center.y + (int)((float)radius * std::sin(angle)));

        CBrush* oriBrush = nullptr;
        int j = (currentProgress >= i) ? (int)(currentProgress - i) : (int)(currentProgress + dotCount - i);
        if (j < (int) frontDotBrush.size())
        {
            oriBrush = dc.SelectObject(frontDotBrush[j].get());
        }
        else
        {
            oriBrush = dc.SelectObject(&backDotBrush);
        }
        dc.Ellipse(drawPos.x - (int) dotRadius, drawPos.y - (int) dotRadius, drawPos.x + (int)dotRadius, drawPos.y + (int) dotRadius);
        dc.SelectObject(oriBrush);
    }
}

void CRgb24CircularProgressImageDrawer::ResetProgress()
{
    currentProgress = 0;
}

void CRgb24CircularProgressImageDrawer::UpdateProgress()
{
    currentProgress = (currentProgress + 1) % dotCount;
}

//bool DownloadFile(const WCHAR WEBSITE[], const WCHAR URL[], const char DownloadFileName[])
//{
//	// Use WinHttpOpen to obtain a session handle
//	HINTERNET hInternet = WinHttpOpen(L"A WinHTTP Example Program/1.0",
//		WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
//		WINHTTP_NO_PROXY_NAME,
//		WINHTTP_NO_PROXY_BYPASS,
//		0);
//
//	if (hInternet == NULL)
//	{
//		TRACE("Failed to initialize http session.\n");
//		return -1;
//	}
//
//	// Specify an HTTP server
//	HINTERNET hConnected = WinHttpConnect(hInternet, WEBSITE, INTERNET_DEFAULT_HTTP_PORT, 0);
//
//	if (hConnected == NULL)
//	{
//		WinHttpCloseHandle(hInternet);
//		return -1;
//	}
//
//	// Create an HTTP Request handle
//	HINTERNET hRequest = WinHttpOpenRequest(hConnected,
//		L"GET",
//		URL,
//		NULL,
//		WINHTTP_NO_REFERER,
//		WINHTTP_DEFAULT_ACCEPT_TYPES,
//		WINHTTP_FLAG_REFRESH);
//
//	if (hRequest == NULL)
//	{
//		::WinHttpCloseHandle(hConnected);
//		::WinHttpCloseHandle(hInternet);
//		return -1;
//	}
//
//	// Send a Request
//	if (WinHttpSendRequest(hRequest,
//		WINHTTP_NO_ADDITIONAL_HEADERS, 0,
//		WINHTTP_NO_REQUEST_DATA, 0,
//		0, 0) == FALSE)
//	{
//		DWORD err = GetLastError();
//		WinHttpCloseHandle(hRequest);
//		WinHttpCloseHandle(hConnected);
//		WinHttpCloseHandle(hInternet);
//		return -1;
//	}
//
//	// Receive a Response
//	if (WinHttpReceiveResponse(hRequest, NULL) == FALSE)
//	{
//		DWORD err = GetLastError();
//		WinHttpCloseHandle(hRequest);
//		WinHttpCloseHandle(hConnected);
//		WinHttpCloseHandle(hInternet);
//		return -1;
//	}
//
//	// Create a sample binary file 
//	HANDLE hFile = CreateFile(DownloadFileName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
//	DWORD dwSize = 0;
//	bool bRetValue = false;
//	TRACE("Downloading ........\n");
//
//	do
//	{
//		// Check for available data.
//		if (!WinHttpQueryDataAvailable(hRequest, &dwSize))
//		{
//			TRACE("Data not available\n");
//		}
//
//		// Allocate space for the buffer.
//		BYTE *pchOutBuffer = new BYTE[dwSize + 1];
//		if (!pchOutBuffer)
//		{
//			TRACE("Http Request is out of memory.\n");
//			dwSize = 0;
//		}
//		else
//		{
//			// Read the Data.
//			DWORD dwDownloaded = 0;
//			ZeroMemory(pchOutBuffer, dwSize + 1);
//			if (!WinHttpReadData(hRequest, (LPVOID)pchOutBuffer, dwSize, &dwDownloaded))
//			{
//				TRACE("Http read data error.\n");
//			}
//			else
//			{
//				// Write buffer to sample binary file
//				DWORD wmWritten;
//				WriteFile(hFile, pchOutBuffer, dwSize, &wmWritten, NULL);
//			}
//
//			delete[] pchOutBuffer;
//		}
//
//	} while (dwSize>0);
//
//	TRACE("Done\n");
//
//	// Housekeeping
//	CloseHandle(hFile);
//	WinHttpCloseHandle(hRequest);
//	WinHttpCloseHandle(hConnected);
//	WinHttpCloseHandle(hInternet);
//	return 0;
//}

typedef BOOL(WINAPI *P_GDFSE)(LPCTSTR, PULARGE_INTEGER,
	PULARGE_INTEGER, PULARGE_INTEGER);

//bool HardDiskManager(char  *_pszDrive)
//{
//	BOOL  fResult;
//
//	char  *pszDrive = NULL,
//		szDrive[4];
//
//	DWORD dwSectPerClust,
//		dwBytesPerSect,
//		dwFreeClusters,
//		dwTotalClusters;
//
//	P_GDFSE pGetDiskFreeSpaceEx = NULL;
//
//	unsigned __int64 i64FreeBytesToCaller,
//		i64TotalBytes,
//		i64FreeBytes;
//
//	pszDrive = _pszDrive;
//
//	if (pszDrive[1] == ':')
//	{
//		szDrive[0] = pszDrive[0];
//		szDrive[1] = ':';
//		szDrive[2] = '\\';
//		szDrive[3] = '\0';
//
//		pszDrive = szDrive;
//	}
//
//	/*
//		Use GetDiskFreeSpaceEx if available; otherwise, use
//		GetDiskFreeSpace.
//
//		Note: Since GetDiskFreeSpaceEx is not in Windows 95 Retail, we
//		dynamically link to it and only call it if it is present.  We 
//		don't need to call LoadLibrary on KERNEL32.DLL because it is 
//		already loaded into every Win32 process's address space.
//	*/ 
//	pGetDiskFreeSpaceEx = (P_GDFSE)GetProcAddress (
//						GetModuleHandle ("kernel32.dll"),
//										"GetDiskFreeSpaceExA");
//	if (pGetDiskFreeSpaceEx)
//	{
//		fResult = pGetDiskFreeSpaceEx (pszDrive,
//								(PULARGE_INTEGER)&i64FreeBytesToCaller,
//								(PULARGE_INTEGER)&i64TotalBytes,
//								(PULARGE_INTEGER)&i64FreeBytes);
//		if (fResult)
//		{
//			TRACE("\n\nGetDiskFreeSpaceEx reports\n\n");
//			TRACE("Available space to caller = %I64u MB\n",
//				i64FreeBytesToCaller / (1024 * 1024));
//			TRACE("Total space               = %I64u MB\n",
//				i64TotalBytes / (1024 * 1024));
//			TRACE("Free space on drive       = %I64u MB\n",
//				i64FreeBytes / (1024 * 1024));
//			
//			CString str;
//			str.Format(
//						"Drive %s\n"
//						"Available space to caller= %I64u MB\n"
//						"Total space= %I64u MB\n"
//						"Free space on drive= %I64u MB\n"
//						, _pszDrive,
//						i64FreeBytesToCaller / (1024 * 1024),
//						i64TotalBytes / (1024 * 1024),
//						i64FreeBytes / (1024 * 1024)
//			);
//
//			MessageBox(NULL,_T(str), "HardDiskManager", MB_OK);
//		}
//	}
//	else
//	{
//		fResult = GetDiskFreeSpace (pszDrive, 
//									&dwSectPerClust,
//									&dwBytesPerSect, 
//									&dwFreeClusters,
//									&dwTotalClusters);
//		if (fResult)
//		{
//			/* force 64-bit math */ 
//			i64TotalBytes = (__int64)dwTotalClusters * dwSectPerClust *
//								dwBytesPerSect;
//			i64FreeBytes = (__int64)dwFreeClusters * dwSectPerClust *
//								dwBytesPerSect;
//
//			TRACE("GetDiskFreeSpace reports\n\n");
//			TRACE("Free space  = %I64u MB\n",
//					i64FreeBytes / (1024*1024));
//			TRACE("Total space = %I64u MB\n",
//					i64TotalBytes / (1024*1024));
//		}
//	}
//
//	if (!fResult) {
//		TRACE("error: %lu:  could not get free space for \"%s\"\n",
//			GetLastError(), _pszDrive);
//		return false;
//	}
//	return true;
//}


int ViewWithMeshLab(std::string filename) {
	HKEY hKey = 0;
	DWORD keyType = REG_SZ;
	TCHAR buf[255] = { 0 };
	DWORD bufSize = sizeof(buf);
	LONG returnStatus;
	returnStatus = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\meshlab.exe", 0, KEY_QUERY_VALUE, &hKey);
	if (returnStatus == ERROR_SUCCESS)
	{
		TRACE("RegOpenKeyEx() is OK! SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\meshlab.exe\n");
		returnStatus = RegQueryValueEx(hKey, L"", NULL, &keyType, (LPBYTE)buf, &bufSize);
		if (returnStatus == ERROR_SUCCESS)
		{			
			TRACE("Meshlab path: %s\n", buf);
            CString csFile;
            csFile.Format( L"\"%s\"", CString(filename.c_str()) );
			ShellExecute(NULL, NULL, buf, csFile, NULL, SW_HIDE);
			if (RegCloseKey(hKey) == ERROR_SUCCESS)
				TRACE("Closing the hKey handle...\n");
			return 0;
		}	
	}
	else
		TRACE("RegOpenKeyEx() failed, error %u\n", GetLastError());
	if (RegCloseKey(hKey) == ERROR_SUCCESS)
		TRACE("Closing the hKey handle...\n");	

	
	return -1;
}

void rotate2Dpoint(float cx, float cy, float cxNew, float cyNew, float angle, float &x, float &y)
{

	const float PI = 3.14159265;
	const float val = PI / 180.0f;
	float s = sin(angle*val);
	float c = cos(angle*val);

	// translate point back to origin:
	x -= cx;
	y -= cy;

	// rotate point
	float xnew = x * c - y * s;
	float ynew = x * s + y * c;

	// translate point back:
	x = xnew + cxNew;
	y = ynew + cyNew;
}
