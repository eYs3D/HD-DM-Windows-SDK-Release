#pragma once

#include "afxwin.h"
#ifdef ESPDI_EG
#include "eSPDI_EG.h"
#else
#include "eSPDI_DM.h"
#endif

typedef enum {
  SENSOR_OFFSET,
  RECTIFY_TABLE,
  ZD_TABLE,
  LOG_DATA,
  USER_DATA
}FLASH_SELECT_INFO;



class CVideoDeviceDlg : public CDialog
{
	DECLARE_DYNAMIC(CVideoDeviceDlg)
    DECLARE_MESSAGE_MAP()

public:                     
	CVideoDeviceDlg(CWnd* pParent = NULL, bool enableSDKLog = false);
	virtual ~CVideoDeviceDlg();

    void SetParams(int devIndex, const DEVINFORMATIONEX& devInfo);
	void DoSnapShot();

    enum { IDD = IDD_VIDEO_DEVICE_DLG };

	void ChangeIMU_UI_FrameCount_SerialCount(bool enableModuleSync);
	int IMU_Device_Mapping();
	void Update_IMU_Device_Mapping();
	void IMU_Device_Reopen(void*& hEtronDI, DEVSELINFO& devSelInfo);
	bool m_bIMU_Device_Sync;
private:

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual void OnOK(){}
    virtual void OnCancel(){}
  
    virtual BOOL OnInitDialog();
  
    void InitDefaultUI();
    void InitChildDlg();
    void InitModuleInformation();
    void GetFWVersion();

    void *m_hEtronDI;
    DEVSELINFO m_DevSelInfo; 
    const DEVINFORMATIONEX* m_pDevInfo;

    std::vector<CDialog*> m_childDlg;
    CMFCTabCtrl m_oTabPage;

    //afx_msg void    OnBnClickedFlashdataRead();
    //afx_msg void    OnBnClickedFlashdataWrite();
    afx_msg void    OnBnClickedRectifylogGet();
    afx_msg void    OnClose();
    afx_msg LRESULT OnChangeActiveTab( WPARAM wparam,LPARAM lparam );
    afx_msg void    OnPaint();
    afx_msg LRESULT OnAutoPreview(WPARAM wParam, LPARAM lParam);
};
