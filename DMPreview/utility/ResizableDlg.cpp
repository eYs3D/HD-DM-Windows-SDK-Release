// ResizingDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ResizableDlg.h"

IMPLEMENT_DYNAMIC(CResizableDlg, CDialogEx)
BEGIN_MESSAGE_MAP(CResizableDlg, CDialogEx)
    ON_WM_SIZING()
END_MESSAGE_MAP()

CResizableDlg::CResizableDlg(UINT nIDTemplate, CWnd* pParent)
	: CDialogEx(nIDTemplate, pParent),
      m_minVariableWidth(320), m_variableRatio(16.0 / 9.0), 
      m_widthOffset(0), m_heightOffset(0)
{
    m_maxVariableSize.cx = 16384;
    m_maxVariableSize.cy = (LONG)(m_maxVariableSize.cx / m_variableRatio);
    m_fixedSize.cx = m_fixedSize.cy = 0;
}

CResizableDlg::~CResizableDlg()
{
}

void CResizableDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
}

BOOL CResizableDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    CRect clientRect, windowRect;
    GetClientRect(&clientRect);
    GetWindowRect(&windowRect);
    m_widthOffset = windowRect.Width() - clientRect.Width();
    m_heightOffset = windowRect.Height() - clientRect.Height();

    return FALSE;// return FALSE due to manually set the focus to IDC_BUTTON_SELECT_FILE
}

void CResizableDlg::SetResizableInfo(int maxVariableWidth, int maxVariableHeight, int minVariableWidth, 
    int fixedWidth, int fixedHeight)
{
    m_maxVariableSize.cx = maxVariableWidth;
    m_maxVariableSize.cy = maxVariableHeight;
    m_minVariableWidth = minVariableWidth;
    m_variableRatio = (double)maxVariableWidth / maxVariableHeight;
    m_fixedSize.cx = fixedWidth;
    m_fixedSize.cy = fixedHeight;
}

void CResizableDlg::OnSizing(UINT fwSide, LPRECT pRect)
{
    CDialogEx::OnSizing( fwSide, pRect );

    CRect rt( pRect );

    switch ( fwSide )
    {
    case WMSZ_TOPRIGHT:
    case WMSZ_TOP:
        {
            rt.top   = rt.bottom - (pRect->bottom - pRect->top); if ( rt.top < NULL ) rt.top = NULL;
            rt.right = rt.left + (LONG)(rt.Height() * m_variableRatio);
        }
        break;
    case WMSZ_BOTTOMRIGHT:
    case WMSZ_BOTTOM:
        {
            rt.bottom = rt.top + (pRect->bottom - pRect->top);
            rt.right  = rt.left + (LONG)(rt.Height() * m_variableRatio);
        }
        break;
    case WMSZ_LEFT:
        {
            rt.left   = rt.right - (pRect->right - pRect->left);
            rt.bottom = rt.top + (LONG)(rt.Width() / m_variableRatio);
        }
        break;
    case WMSZ_RIGHT:
        {
            rt.right  = rt.left + (pRect->right - pRect->left);
            rt.bottom = rt.top + (LONG)(rt.Width() / m_variableRatio);
        }
        break;
    case WMSZ_TOPLEFT:
        {
            rt.top  = rt.bottom - (pRect->bottom - pRect->top);
            rt.left = rt.right - (LONG)(rt.Height() * m_variableRatio);
        }
        break;
    case WMSZ_BOTTOMLEFT:
        {
            rt.bottom = rt.top + (pRect->bottom - pRect->top);
            rt.left   = rt.right - (LONG)(rt.Height() * m_variableRatio);
        }
        break;
    }
    if ( rt.Width() < 320 )
    {
        GetWindowRect( rt );
    }
    else
    {
        HMONITOR hMonitor = MonitorFromWindow(this->GetSafeHwnd(), MONITOR_DEFAULTTONEAREST);
        MONITORINFO mInfo;
        mInfo.cbSize = sizeof(MONITORINFO);
        GetMonitorInfo(hMonitor, &mInfo);

        if ( rt.Width()  > ( mInfo.rcWork.right  - mInfo.rcWork.left ) ||
             rt.Height() > ( mInfo.rcWork.bottom - mInfo.rcWork.top  ) )
        {
            GetMaxWindowRect( rt );

            rt.bottom = rt.top + (LONG)(rt.Width() / m_variableRatio);

            SetWindowPos( &CWnd::wndTop, NULL, NULL, rt.Width(), rt.Height(), SWP_NOMOVE );

            return;
        }
    }
    *pRect = rt;
}

int CResizableDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CDialogEx::OnCreate(lpCreateStruct) == -1)
        return -1;

    ModifyStyle(0, DS_SETFONT | DS_FIXEDSYS | WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME);

    return 0;
}

bool CResizableDlg::GetMaxWindowRect(CRect& wndRect)
{
    HMONITOR hMonitor = MonitorFromWindow(this->GetSafeHwnd(), MONITOR_DEFAULTTONEAREST);
    MONITORINFO mInfo;
    mInfo.cbSize = sizeof(MONITORINFO);
    if (GetMonitorInfo(hMonitor, &mInfo))
    {
        if (m_maxVariableSize.cx + m_fixedSize.cx + m_widthOffset > mInfo.rcWork.right - mInfo.rcWork.left)
        {
            double xRatio = (double)(mInfo.rcWork.right - mInfo.rcWork.left - m_fixedSize.cx - m_widthOffset) / m_maxVariableSize.cx;
            if ((int)(m_maxVariableSize.cy * xRatio) + m_fixedSize.cy + m_heightOffset > mInfo.rcWork.bottom - mInfo.rcWork.top)
            {
                double yRatio = (double)(mInfo.rcWork.bottom - mInfo.rcWork.top - m_fixedSize.cy - m_heightOffset) / m_maxVariableSize.cy;
                wndRect.top = mInfo.rcWork.top;
                wndRect.bottom = mInfo.rcWork.bottom;
                wndRect.left = ((mInfo.rcWork.left + mInfo.rcWork.right) - ((int)(m_maxVariableSize.cx * yRatio) + m_fixedSize.cx + m_widthOffset)) / 2;
                wndRect.right = wndRect.left + (int)(m_maxVariableSize.cx * yRatio) + m_fixedSize.cx + m_widthOffset;
            }
            else
            {
                wndRect.left = mInfo.rcWork.left;
                wndRect.right = mInfo.rcWork.right;
                wndRect.top = mInfo.rcWork.top;
                wndRect.bottom = wndRect.top + (int)(m_maxVariableSize.cy * xRatio) + m_fixedSize.cy + m_heightOffset;
            }
        }
        else if (m_maxVariableSize.cy + m_fixedSize.cy + m_heightOffset > mInfo.rcWork.bottom - mInfo.rcWork.top)
        {
            double yRatio = (double)(mInfo.rcWork.bottom - mInfo.rcWork.top - m_fixedSize.cy - m_heightOffset) / m_maxVariableSize.cy;
            wndRect.top = mInfo.rcWork.top;
            wndRect.bottom = mInfo.rcWork.bottom;
            wndRect.left = ((mInfo.rcWork.left + mInfo.rcWork.right) - ((int)(m_maxVariableSize.cx * yRatio) + m_fixedSize.cx + m_widthOffset)) / 2;
            wndRect.right = wndRect.left + (int)(m_maxVariableSize.cx * yRatio) + m_fixedSize.cx + m_widthOffset;
        }
        else
        {
            wndRect.top = mInfo.rcWork.top;
            wndRect.bottom = wndRect.top + m_maxVariableSize.cy + m_fixedSize.cy + m_heightOffset;
            wndRect.left = ((mInfo.rcWork.left + mInfo.rcWork.right) - (m_maxVariableSize.cx + m_fixedSize.cx + m_widthOffset)) / 2;
            wndRect.right = wndRect.left + m_maxVariableSize.cx + m_fixedSize.cx + m_widthOffset;
        }

        return true;
    }

    return false;
}

void CResizableDlg::MaximizeWindow()
{
    CRect wndRect;
    if (GetMaxWindowRect(wndRect))
    {
        SetWindowPos(&CWnd::wndTop, wndRect.left, wndRect.top, wndRect.Width(), wndRect.Height(), SWP_SHOWWINDOW);
    }
}