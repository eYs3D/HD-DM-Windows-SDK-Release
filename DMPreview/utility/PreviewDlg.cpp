// PreviewDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LyfieSee.h"
#include "PreviewDlg.h"


// CPreviewDlg dialog

IMPLEMENT_DYNAMIC(CPreviewDlg, CResizableDlg)

CPreviewDlg::CPreviewDlg(CWnd* pParent /*=NULL*/)
	: CResizableDlg(IDD_DIALOG_PREVIEW, pParent), m_pParent(pParent)
    , m_imgWidth(0), m_imgHeight(0), m_drawThread(nullptr), m_startDrawThread(false)
    , m_flipImg(false), m_mouseTracking(false), m_curMousePos(-1, -1)
{
}

CPreviewDlg::~CPreviewDlg()
{
}

void CPreviewDlg::SetParams(int imgWidth, int imgHeight)
{
    m_imgWidth = imgWidth;
    m_imgHeight = imgHeight;
}

void CPreviewDlg::UpdateImage(const std::vector<unsigned char>& imgBuf)
{
    std::lock_guard<std::mutex> lock(m_imgBufMutex);
    m_imgBuf = imgBuf;
}

void CPreviewDlg::FlipImage(bool flip)
{
    m_flipImg = flip;
}

bool CPreviewDlg::GetCurrentImage(std::vector<unsigned char>& imgBuf, int& imgWidth, int& imgHeight, bool& flip)
{
    {
        std::lock_guard<std::mutex> lock(m_imgBufMutex);
        imgBuf = m_imgBuf;
    }

    imgWidth = m_imgWidth;
    imgHeight = m_imgHeight;
    flip = m_flipImg;
    return (!imgBuf.empty() && imgWidth > 0 && imgHeight > 0);
}

void CPreviewDlg::SetCurMousePos(const CPoint pos)
{
    std::lock_guard<std::mutex> lock(m_curMousePosMutex);
    m_curMousePos = pos;
}

CPoint CPreviewDlg::GetCurMousePos()
{
    std::lock_guard<std::mutex> lock(m_curMousePosMutex);
    return m_curMousePos;
}

void CPreviewDlg::GetCurMousePos(double& xRatio, double& yRatio)
{
    CPoint pos = GetCurMousePos();
    xRatio = yRatio = -1.0;
    if (pos.x >= 0 && pos.y >= 0)
    {
        CRect clientRect;
        GetClientRect(&clientRect);
        if (clientRect.Width() != 0)
        {
            xRatio = (double)pos.x / clientRect.Width();
        }

        if (clientRect.Height() != 0)
        {
            yRatio = (double)pos.y / clientRect.Height();
        }
    }
}

void CPreviewDlg::SetCurMousePos(const double xRatio, const double yRatio)
{
    if (xRatio >= 0 && yRatio >= 0)
    {
        CRect clientRect;
        GetClientRect(&clientRect);

        SetCurMousePos(CPoint((int)(clientRect.Width() * xRatio), (int)(clientRect.Height() * yRatio)));
    }
    else
    {
        SetCurMousePos(CPoint(-1, -1));
    }
}

void CPreviewDlg::DoDataExchange(CDataExchange* pDX)
{
    CResizableDlg::DoDataExchange(pDX);
}

BOOL CPreviewDlg::OnInitDialog()
{
    CResizableDlg::OnInitDialog();

    if (!MaximizeWindow())
    {
        ShowWindow(SW_SHOW);
    }

    StartDrawThread();

    return TRUE;
}

bool CPreviewDlg::GetMaxWindowRect(CRect& wndRect)
{
    HMONITOR hMonitor = MonitorFromWindow(this->GetSafeHwnd(), MONITOR_DEFAULTTONEAREST);
    MONITORINFO mInfo;
    mInfo.cbSize = sizeof(MONITORINFO);
    if (GetMonitorInfo(hMonitor, &mInfo))
    {
        int xBorderWidth = GetSystemMetrics(SM_CXFIXEDFRAME);
        int yBorderHeight = GetSystemMetrics(SM_CYFIXEDFRAME);
        int captionHeight = GetSystemMetrics(SM_CYCAPTION);

        if (m_imgWidth + xBorderWidth * 2 > mInfo.rcWork.right - mInfo.rcWork.left)
        {
            double xRatio = (double)(mInfo.rcWork.right - mInfo.rcWork.left - xBorderWidth * 2) / m_imgWidth;
            if ((int)(m_imgHeight * xRatio) + yBorderHeight * 2 + captionHeight > mInfo.rcWork.bottom - mInfo.rcWork.top)
            {
                double yRatio = (double)(mInfo.rcWork.bottom - mInfo.rcWork.top - yBorderHeight * 2 - captionHeight) / m_imgHeight;
                wndRect.top = mInfo.rcWork.top;
                wndRect.bottom = mInfo.rcWork.bottom;
                wndRect.left = ((mInfo.rcWork.left + mInfo.rcWork.right) - ((int)(m_imgWidth * yRatio) + xBorderWidth * 2)) / 2;
                wndRect.right = wndRect.left + (int)(m_imgWidth * yRatio) + xBorderWidth * 2;
            }
            else
            {
                wndRect.left = mInfo.rcWork.left;
                wndRect.right = mInfo.rcWork.right;
                wndRect.top = mInfo.rcWork.top;
                wndRect.bottom = wndRect.top + (int)(m_imgHeight * xRatio) + yBorderHeight * 2 + captionHeight;
            }
        }
        else if (m_imgHeight + yBorderHeight * 2 + captionHeight > mInfo.rcWork.bottom - mInfo.rcWork.top)
        {
            double yRatio = (double)(mInfo.rcWork.bottom - mInfo.rcWork.top - yBorderHeight * 2 - captionHeight) / m_imgHeight;
            wndRect.top = mInfo.rcWork.top;
            wndRect.bottom = mInfo.rcWork.bottom;
            wndRect.left = ((mInfo.rcWork.left + mInfo.rcWork.right) - ((int)(m_imgWidth * yRatio) + xBorderWidth * 2)) / 2;
            wndRect.right = wndRect.left + (int)(m_imgWidth * yRatio) + xBorderWidth * 2;
        }
        else
        {
            wndRect.top = mInfo.rcWork.top;
            wndRect.bottom = wndRect.top + m_imgHeight + yBorderHeight * 2 + captionHeight;
            wndRect.left = ((mInfo.rcWork.left + mInfo.rcWork.right) - (m_imgWidth + xBorderWidth * 2)) / 2;
            wndRect.right = wndRect.left + m_imgWidth + xBorderWidth * 2;
        }

        return true;
    }

    return false;
}

bool CPreviewDlg::MaximizeWindow()
{
    CRect wndRect;
    if (GetMaxWindowRect(wndRect))
    {
        SetWindowPos(&CWnd::wndTop, wndRect.left, wndRect.top, wndRect.right - wndRect.left, wndRect.bottom - wndRect.top, SWP_SHOWWINDOW);
        return true;
    }

    return false;
}

void CPreviewDlg::StartDrawThread()
{
    m_startDrawThread = true;
    m_drawThread = new std::thread(CPreviewDlg::DrawThreadFn, this);
}

void CPreviewDlg::StopDrawThread()
{
    m_startDrawThread = false;
    m_drawThread->join();
    delete m_drawThread;
    m_drawThread = nullptr;
}

void CPreviewDlg::DrawThreadFn(CPreviewDlg* pThis)
{
    CDC* pDC = pThis->GetDC();
    CDC memDC;
    memDC.CreateCompatibleDC(nullptr);
    std::vector<unsigned char> drawBuf;

    BITMAPINFO bmpInfo;
    bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmpInfo.bmiHeader.biPlanes = 1;
    bmpInfo.bmiHeader.biBitCount = 24;
    bmpInfo.bmiHeader.biCompression = BI_RGB;
    bmpInfo.bmiHeader.biWidth = pThis->m_imgWidth;
    bmpInfo.bmiHeader.biHeight = -pThis->m_imgHeight;//img buf is top-down, height should be set to negative

    while (pThis->m_startDrawThread)
    {
        {
            std::lock_guard<std::mutex> lock(pThis->m_imgBufMutex);
            drawBuf = pThis->m_imgBuf;
        }

        if (!drawBuf.empty())
        {
            CRect clientRect;
            pThis->GetClientRect(&clientRect);

            CBitmap MemBitmap;
            if (MemBitmap.CreateCompatibleBitmap(pDC, clientRect.Width(), clientRect.Height()))
            {
                memDC.SelectObject(&MemBitmap);
                MemBitmap.DeleteObject();
                memDC.SetStretchBltMode(STRETCH_DELETESCANS);

                bmpInfo.bmiHeader.biHeight = (pThis->m_flipImg ? pThis->m_imgHeight : -pThis->m_imgHeight);

                StretchDIBits(memDC.m_hDC, 
                    0, 0, clientRect.Width(), clientRect.Height(),
                    0, 0, pThis->m_imgWidth, pThis->m_imgHeight,
                    &drawBuf[0], &bmpInfo, DIB_RGB_COLORS, SRCCOPY);

                pDC->BitBlt(0, 0, clientRect.Width(), clientRect.Height(), &memDC, 0, 0, SRCCOPY);
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(33));
    }

    memDC.DeleteDC();
    pThis->ReleaseDC(pDC);
}


BEGIN_MESSAGE_MAP(CPreviewDlg, CResizableDlg)
    ON_WM_CLOSE()
    ON_WM_SYSCOMMAND()
    ON_WM_NCLBUTTONDBLCLK()
    ON_WM_MOUSELEAVE()
    ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()


void CPreviewDlg::OnClose()
{
    StopDrawThread();

    m_pParent->PostMessage(WM_MSG_CLOSE_PREVIEW_DLG, (WPARAM)this);

    CResizableDlg::OnClose();
}

void CPreviewDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
    if (nID == SC_MAXIMIZE)
    {
        if (MaximizeWindow())
        {
            return;
        }
    }

    CResizableDlg::OnSysCommand(nID, lParam);
}

void CPreviewDlg::OnNcLButtonDblClk(UINT nHitTest, CPoint point)
{
    if (nHitTest == HTCAPTION)
    {
        if (MaximizeWindow())
        {
            return;
        }
    }

    CResizableDlg::OnNcLButtonDblClk(nHitTest, point);
}

void CPreviewDlg::OnMouseLeave()
{
    m_mouseTracking = false;
    SetCurMousePos(CPoint(-1, -1));

    // if dialog's parent has WS_CHILD style, GetParent() will return parent's parent, 
    // thus the only way to get dialog's real parent is using m_pParentWnd directly
    m_pParentWnd->PostMessage(WM_MSG_UPDATE_MOUSE_POS_FROM_DIALOG, (WPARAM)this);

    CResizableDlg::OnMouseLeave();
}

void CPreviewDlg::OnMouseMove(UINT nFlags, CPoint point)
{
    if (!m_mouseTracking)
    {
        TRACKMOUSEEVENT tme;
        tme.cbSize = sizeof(TRACKMOUSEEVENT);
        tme.dwFlags = TME_LEAVE;
        tme.hwndTrack = m_hWnd;
        tme.dwHoverTime = 10;

        if (_TrackMouseEvent(&tme))
        {
            m_mouseTracking = true;
        }
    }

    SetCurMousePos(point);

    // if dialog's parent has WS_CHILD style, GetParent() will return parent's parent, 
    // thus the only way to get dialog's real parent is using m_pParentWnd directly
    m_pParentWnd->PostMessage(WM_MSG_UPDATE_MOUSE_POS_FROM_DIALOG, (WPARAM)this);

    CResizableDlg::OnMouseMove(nFlags, point);
}
