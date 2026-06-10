#pragma once

class DepthFilterDlg : public CDialog
{
	DECLARE_DYNAMIC( DepthFilterDlg )
    DECLARE_MESSAGE_MAP()

public:

	enum { IDD = IDD_DLG_DEPTHFILTER };

	DepthFilterDlg( const int PID, const USB_PORT_TYPE eUSB_Port_Type, CPreviewImageDlg* pPreviewDlg );
	virtual ~DepthFilterDlg();
	void setVersion(const char* version);

private:

    CPreviewImageDlg*   m_pPreviewDlg;
    const int           m_PID;
    const USB_PORT_TYPE m_eUSB_Port_Type;
    CSliderCtrl         m_oEPSLevel;
    CSliderCtrl         m_oTFAlpha;
    CSliderCtrl         m_oHFLevel;
    CComboBox           m_oSampleMode;
    CComboBox           m_oSampleFactor;
    string              m_version;

    void UpdateFilterParam();
    void EnableControl( const BOOL bEnable );
    void FullMinConfig( const BOOL bEnable, const BOOL bEnableRemoveCurve );
    void DefaultSetting();

	virtual void DoDataExchange( CDataExchange* pDX );
    virtual BOOL OnInitDialog();
    virtual void OnOK() {}
    virtual void OnCancel() {}

    afx_msg void OnHScroll( UINT nSBCode, UINT nPos, CScrollBar* pScrollBar );
    afx_msg void OnCbnSelchangeCbSubsampleMode();
    afx_msg void OnBnClickedChkFilterEnable();
    afx_msg void OnBnClickedChkSubsampleEnable();
    afx_msg void OnBnClickedChkEpsEnable();
    afx_msg void OnBnClickedChkHfEnable();
    afx_msg void OnBnClickedChkTfEnable();
    afx_msg void OnBnClickedChkHfHrzEnable();
    afx_msg void OnCbnSelchangeCbSubsampleFactor();
    afx_msg void OnBnClickedChkRemovecurve();
    afx_msg void OnBnClickedChkMin();
    afx_msg void OnBnClickedChkFull();
};
