#pragma once

#ifdef ESPDI_EG
#include "eSPDI_EG.h"
#else
#include "eSPDI_DM.h"
#endif

#include "PreviewImageDlg.h"

#define SPARSE_MODE_CONFIG_DEFAULT_FILENAME "APC_SparseModeConfig_Default.ini"
#define SPARSE_MODE_CONFIG_OVERRIDE_FILENAME "APC_SparseModeConfig_Override.ini"

#define CFG_SECTION_NAME "SPARSE_MODE"

#define CFG_DEPTH_LRCHECK_DIFF "DEPTH_LRCHECK_DIFF"
#define CFG_SGBM_SAD_THD "SGBM_SAD_THD"
#define CFG_SGBM_SAD_RATIO "SGBM_SAD_RATIO"
#define CFG_TEXT_LMT "TEXT_LMT"
#define CFG_TEXT_PGAIN "TEXT_PGAIN"
#define CFG_TEXT_NGAIN "TEXT_NGAIN"
#define CFG_SPARSE_THD "SPARSE_THD"

class SparseMode_PropertyDlg : public CDialog
{
	DECLARE_DYNAMIC( SparseMode_PropertyDlg )
    DECLARE_MESSAGE_MAP()

public:

    enum { IDD = IDD_SPARSE_MODE };

	SparseMode_PropertyDlg( void*& hApcDI, DEVSELINFO& devSelInfo, const DEVINFORMATIONEX& devinfoEx, CWnd* pParent );
	virtual ~SparseMode_PropertyDlg();

	CPreviewImageDlg* m_pPreviewDlg;

    static const int DEPTH_LRCHECK_DIFF_ADDRESS = 0xf410;
    static const int SGBM_SAD_THD_ADDRESS = 0xf409;
    static const int SGBM_SAD_RATIO_ADDRESS = 0xf409;
    static const int TEXT_LMT_ADDRESS = 0xf429;
    static const int TEXT_PGAIN_ADDRESS = 0xf42A;
    static const int TEXT_NGAIN_ADDRESS = 0xf42A;
    static const int SPARSE_THD_ADDRESS = 0xf430;

    static const int DEPTH_LRCHECK_BYPASS_ADDRESS = 0xf410;
    static const int DEPTH_SAD_ENB_ADDRESS = 0xf402;
    static const int TEXT_MODE_ADDRESS = 0xf429;
    static const int SPARSE_ENB_ADDRESS = 0xf430;

    enum
    {
        CONFIG_DEPTH_LRCHECK_DIFF,
        CONFIG_SGBM_SAD_THD,
        CONFIG_SGBM_SAD_RATIO,
        CONFIG_TEXT_LMT,
        CONFIG_TEXT_PGAIN,
        CONFIG_TEXT_NGAIN,
        CONFIG_SPARSE_THD
    };

    int Default_DEPTH_LRCHECK_DIFF_Value = 7;
    int Default_SGBM_SAD_THD_Value = 4;
    int Default_SGBM_SAD_RATIO_Value = 5;
    int Default_TEXT_LMT_Value = 127;
    int Default_TEXT_PGAIN_Value = 7;
    int Default_TEXT_NGAIN_Value = 7;
    int Default_SPARSE_THD_Value = 8;

    void UpdateSparseModeConfig(bool enable);
    void UpdateSparseModeDefault();

    afx_msg void OnBnClickedEnableSparseMode();

private:
	void*&            m_hApcDI;
	DEVSELINFO&       m_DevSelInfo;
    const DEVINFORMATIONEX& m_xDevinfoEx;
    CSliderCtrl*      m_SliderCtrl_DEPTH_LRCHECK_DIFF;
    CSliderCtrl*      m_SliderCtrl_SGBM_SAD_THD;
    CSliderCtrl*      m_SliderCtrl_SGBM_SAD_RATIO;
    CSliderCtrl*      m_SliderCtrl_TEXT_LMT;
    CSliderCtrl*      m_SliderCtrl_TEXT_PGAIN;
    CSliderCtrl*      m_SliderCtrl_TEXT_NGAIN;
    CSliderCtrl*      m_SliderCtrl_SPARSE_THD;

    virtual BOOL OnInitDialog();
    virtual void DoDataExchange( CDataExchange* pDX );
    virtual void OnOK() {}
    virtual void OnCancel() {}

    afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar * pScrollBar);
    afx_msg void OnBnClickedSparseModeReset();
    afx_msg void OnBnClickedSparseModeSaveConfig();

    void InitUI();
    void UpdateUI_SliderCtrl_DEPTH_LRCHECK_DIFF(long value);
    void UpdateUI_SliderCtrl_DEPTH_LRCHECK_DIFF_Range(long min, long max);
    void UpdateUI_SliderCtrl_SGBM_SAD_THD(long value);
    void UpdateUI_SliderCtrl_SGBM_SAD_THD_Range(long min, long max);
    void UpdateUI_SliderCtrl_SGBM_SAD_RATIO(long value);
    void UpdateUI_SliderCtrl_SGBM_SAD_RATIO_Range(long min, long max);
    void UpdateUI_SliderCtrl_TEXT_LMT(long value);
    void UpdateUI_SliderCtrl_TEXT_LMT_Range(long min, long max);
    void UpdateUI_SliderCtrl_TEXT_PGAIN(long value);
    void UpdateUI_SliderCtrl_TEXT_PGAIN_Range(long min, long max);
    void UpdateUI_SliderCtrl_TEXT_NGAIN(long value);
    void UpdateUI_SliderCtrl_TEXT_NGAIN_Range(long min, long max);
    void UpdateUI_SliderCtrl_SPARSE_THD(long value);
    void UpdateUI_SliderCtrl_SPARSE_THD_Range(long min, long max);
    void UpdateSparseModeReg(int SparseMode, int value);
    void EnableSliderCtrl(bool enable);
    BOOL ReadConfig();
};
