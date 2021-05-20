#pragma once
#ifdef ESPDI_EG
#include "eSPDI_EG.h"
#else
#include "eSPDI_DM.h"
#endif
#include "utility/ResizableDlg.h"
#include "PreviewImageDlg.h"

class CColorDlg : public CResizableDlg
{
	DECLARE_DYNAMIC(CColorDlg)
    DECLARE_MESSAGE_MAP()

public:
	CColorDlg(CWnd* pParent = NULL);
	virtual ~CColorDlg();

	enum { IDD = IDD_COLOR_DLG };
  
    virtual BOOL OnInitDialog();

    void SetDlgName(std::string name);
    void SetColorParams( void* hEtronDI, const DEVSELINFO& devSelInfo, const int imgWidth, const int imgHeight, const BOOL isLRD_Mode, const CPoint& cpDepth );
    void SetHandle( void* hEtronDI, const DEVSELINFO& devSelInfo );
    bool GetImage(std::vector<unsigned char>& imgBuf, int& width, int& height, int& serialNumber);
    void EnableRotate( const BOOL bRotate );
    void ApplyImage(unsigned char *pColorBuf, int *dataSize, BOOL bIsOutputRGB, BOOL bIsMJPEG, int nColorSerialNum);

private:
	virtual void DoDataExchange(CDataExchange* pDX);
    virtual void OnOK();
    virtual void OnCancel();

    void ShowImage( CDC& dc );
    void *m_hEtronDI;
    DEVSELINFO m_DevSelInfo;
    BOOL m_mouseTracking;
    LARGE_INTEGER m_liPerfFreq;
    LARGE_INTEGER m_liPerfTime;
    std::list<LONGLONG> m_recentFrameTimestamp;
    CString m_csDialogTitle;
    CString m_dlgName;
    std::vector< BYTE > m_vecRGBImageBuf;
    std::vector< BYTE > m_vecRawImageBuf;  
    std::thread* m_pShowImage;
    std::condition_variable m_event;

    int m_imgSerialNumber;
    int m_serialNumberShowImage;
    std::mutex m_imgBufMutex;
    int m_nColorResWidth;
    int m_nColorResHeight; 

    volatile BOOL m_bRun;
    volatile BOOL m_bImageArrival;
    BOOL m_IsLRD_Mode;
    HANDLE hloc; 
    LPBITMAPINFO m_BmpInfo;  
    CPoint m_cpDepth;
    BOOL m_bRotate;
    EtronDIImageType::Value m_eImageType;

    float UpdateAndGetFramerate();
    void Thread_ShowImage();

    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnNcLButtonDblClk(UINT nHitTest, CPoint point);
    afx_msg void OnMouseLeave();
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg BOOL OnEraseBkgnd( CDC* pDC ) { return TRUE; }
    afx_msg void OnPaint();
    afx_msg void OnClose();
    afx_msg LRESULT OnUpdateDlgTitle(WPARAM wParam, LPARAM lParam);
};
