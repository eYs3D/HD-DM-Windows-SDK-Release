#pragma once
#include "utility/ResizableDlg.h"
#include <vector>
#include <mutex>
#include <queue>


#define WM_MSG_CLOSE_PREVIEW_DLG (WM_APP + 1)
#define WM_MSG_UPDATE_MOUSE_POS_FROM_DIALOG (WM_APP + 2)
// CPreviewDlg dialog

class CPreviewDlg : public CResizableDlg
{
	DECLARE_DYNAMIC(CPreviewDlg)

public:
	CPreviewDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPreviewDlg();
    void SetParams(int imgWidth, int imgHeight);
    void UpdateImage(const std::vector<unsigned char>& imgBuf);
    void FlipImage(bool flip);
    bool GetCurrentImage(std::vector<unsigned char>& imgBuf, int& imgWidth, int& imgHeight, bool& flip);
    void GetCurMousePos(double& xRatio, double& yRatio);
    void SetCurMousePos(const double xRatio, const double yRatio);

// Dialog Data
	enum { IDD = IDD_DIALOG_PREVIEW };

private:
    // if dialog's parent has WS_CHILD style, GetParent() will return parent's parent.
    // One way to get dialog's real parent is using m_pParentWnd.
    // here we keep the parent's pointer, and reference the pointer directly.
    CWnd* m_pParent;

    std::vector<unsigned char> m_imgBuf;
    std::mutex m_imgBufMutex;
    int m_imgWidth;
    int m_imgHeight;
    std::thread* m_drawThread;
    bool m_startDrawThread;
    bool m_flipImg;
    bool m_mouseTracking;
    CPoint m_curMousePos;
    std::mutex m_curMousePosMutex;

private:
    void StartDrawThread();
    void StopDrawThread();
    static void DrawThreadFn(CPreviewDlg* pThis);
    bool GetMaxWindowRect(CRect& wndRect);
    bool MaximizeWindow();
    void SetCurMousePos(const CPoint pos);
    CPoint GetCurMousePos();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnClose();
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnNcLButtonDblClk(UINT nHitTest, CPoint point);
    afx_msg void OnMouseLeave();
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};
