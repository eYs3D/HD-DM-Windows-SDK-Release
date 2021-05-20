#pragma once

CString GetProductVersion();
CRect GetMaxRect(double rectRatio, int maxX, int maxY);//rectRatio: x / y
CRect GetMaxRect(double innerRectRatio, CRect border, int maxX, int maxY);//innerRectRatio: x / y
std::string GetCurrentModuleFolder();
void SaveImage(const std::vector<unsigned char>& rgbBuf, int width, int height, int bits = 24, const char* filename = nullptr, bool flipImg = false, const GUID& imgType = Gdiplus::ImageFormatJPEG);
void SaveImage2(unsigned char* pRgbBuf,BITMAPINFO *bmpInfo, const char* filename,   const GUID& imgType);
void SaveYuv(const std::vector<unsigned char>& rgbBuf, unsigned char Bits, int width, int height, const char* filename);
bool CopyFileSilently(std::string srcFilename, std::string dstFolder, std::string newFilename);
//bool CopyFilesFolderToFolder(std::string srcFolder, std::string dstFolder, bool move = false);// newer API but single thread
//bool DeleteFolder(std::string folder);
//bool CopyFilesFolderToFolder_MT(std::string srcFolder, std::string dstFolder, bool move = false);// older API but multi thread
//bool DeleteFolder_MT(std::string folder);
void EnumChildWnd(CWnd* wnd, std::vector<CWnd*>& childWnd);
//bool DownloadFile(const WCHAR WEBSITE[], const WCHAR URL[], const char DownloadFileName[]);
//bool HardDiskManager(char  *_pszDrive);	//for exampe : _pszDrive = "c:\\";
int ViewWithMeshLab(std::string filename);
void rotate2Dpoint(float cx, float cy, float cxNew, float cyNew, float angle, float &x, float &y);

class CLocaleCover
{
public:
    CLocaleCover(int category, const char* locale);
    ~CLocaleCover();

private:
    int m_category;
    std::string m_originLocale;
};

class CAutoLock
{
public:
    CAutoLock(CMutex& mutex);
    ~CAutoLock();

private:
    CMutex& m_mutex;
};

class CTimelineCtrl : public CSliderCtrl
{
    DECLARE_DYNAMIC(CTimelineCtrl)

public:
    CTimelineCtrl();
    virtual ~CTimelineCtrl();

    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

protected:
    DECLARE_MESSAGE_MAP()
};

class CRgb24CircularProgressImageDrawer
{
public:
    CRgb24CircularProgressImageDrawer(double ratioOfProgressToShortEdge, size_t progressDotCount,
        unsigned char red, unsigned char green, unsigned char blue);

    void DrawProgressImage(CDC& dc, size_t width, size_t height);
    void ResetProgress();
    void UpdateProgress();

private:
    double ratio;
    size_t dotCount;
    size_t currentProgress;
    CBrush backDotBrush;
    std::vector<std::shared_ptr<CBrush>> frontDotBrush;
    const size_t frontDotCount;
};