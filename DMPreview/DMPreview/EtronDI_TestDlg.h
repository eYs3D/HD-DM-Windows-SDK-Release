
// EtronDI_TestDlg.h : 標頭檔
//

#pragma once

#ifdef ESPDI_EG
#include "eSPDI_EG.h"
#else
#include "eSPDI_DM.h"
#endif
#include "afxwin.h"

class CVideoDeviceDlg;

// CEtronDI_TestDlg 對話方塊
class CEtronDI_TestDlg : public CDialog
{
// 建構
public:
	CEtronDI_TestDlg(CWnd* pParent = NULL);	// 標準建構函式
    virtual ~CEtronDI_TestDlg();

// 對話方塊資料
	enum { IDD = IDD_ETRONDI_TEST_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支援
    virtual void OnOK();
    virtual void OnCancel();


// 程式碼實作
protected:
	HICON m_hIcon;

	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
    afx_msg void OnBnClickedOpenDevice();
    LRESULT OnCloseVideoDeviceDlg(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnShapShotAll(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
  
private:
    CVideoDeviceDlg *m_pVideoDevDlg[200];
    std::vector<DEVINFORMATIONEX> m_devInfo;
    CComboBox m_Device_Ctrl;

public:
	CString m_strPath_Application;
	CString m_profile_file;

public:
	void OpenDevice(int devIndex);

};
