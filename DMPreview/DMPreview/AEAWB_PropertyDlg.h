#pragma once

#ifdef ESPDI_EG
#include "eSPDI_EG.h"
#else
#include "eSPDI_DM.h"
#endif

#include "PreviewImageDlg.h"

class AEAWB_PropertyDlg : public CDialog
{
	DECLARE_DYNAMIC( AEAWB_PropertyDlg )
    DECLARE_MESSAGE_MAP()

public:

    enum { IDD = IDD_AEAWB_PROPERTY_DIALOG };

	AEAWB_PropertyDlg( void*& hEtronDI, DEVSELINFO& devSelInfo, const DEVINFORMATIONEX& devinfoEx, CWnd* pParent );
	virtual ~AEAWB_PropertyDlg();

    void ReadProperty();

	bool IsLowLight();
	void SetLowLight(const BOOL IsLowLight);
    void LowLightEnable( const BOOL Enable );

	CPreviewImageDlg* m_pPreviewDlg;
private:

	void InitUI();

	void UpdateUI_ExposureTimeValue(long value);
	void UpdateUI_ExposureTimeRange(long min, long max);
	void UpdateUI_AE(BOOL enable);
	void EnableUI_AE(BOOL enable);

	void UpdateUI_WBTemperatureValue(long min);
	void UpdateUI_WBTemperatureRange(long min, long max);
	void UpdateUI_AWB(BOOL enable);
	void EnableUI_WB(BOOL enable);

	void UpdateUI_LightSource(long source);
	void EnableUI_LightSource(BOOL enable);

	void EnableUI_LLC(BOOL enable);
	void UpdateUI_LLC(long value);

	void*&            m_hEtronDI;
	DEVSELINFO&       m_DevSelInfo;
    const DEVINFORMATIONEX& m_xDevinfoEx;
	CComboBox*        m_pComboBox;
	CButton*          m_CButtonAE;
	CButton*          m_CButtonAWB;
	CButton*          m_CButtonLightSource50Hz;
	CButton*          m_CButtonLightSource60Hz;
	CButton*          m_CButtonLLC_On;
	CButton*          m_CButtonLLC_Off;
	CSliderCtrl*      m_ExposureTimeSliderCtrl;
	CSliderCtrl*      m_WBTemperatureSliderCtrl;
	BOOL              m_radio_btn_group1;
	BOOL              m_radio_btn_group2;

    virtual BOOL OnInitDialog();
    virtual void DoDataExchange( CDataExchange* pDX );
    virtual void OnOK() {}
    virtual void OnCancel() {}

    afx_msg void OnCbnSelchangeComboDeviceType();
    afx_msg void OnBnClickedCheckAutoExposure();
    afx_msg void OnBnClickedCheckAWB();
    afx_msg void OnBnClickedRadio50hz();
    afx_msg void OnBnClickedRadio60hz();
    afx_msg void OnBnClickedRadioLlcOn();
    afx_msg void OnBnClickedRadioLlcOff();
    afx_msg void OnHScroll( UINT nSBCode, UINT nPos, CScrollBar * pScrollBar );
    afx_msg void OnBnClickedBtPropertyReset();
    afx_msg void OnBnClickedButtonExptimeRead();
    afx_msg void OnBnClickedButtonExptimeWrite();
    afx_msg void OnBnClickedButtonGlogalGainRead();
    afx_msg void OnBnClickedButtonGlogalGainWrite();
};
