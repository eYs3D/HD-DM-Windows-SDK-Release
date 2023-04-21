
#include "stdafx.h"
#include "APC_Test.h"
#include "SparseMode_PropertyDlg.h"
#include "afxdialogex.h"
#include <ks.h>
#include <KsMedia.h>	//for PropertyPage item R/W

IMPLEMENT_DYNAMIC( SparseMode_PropertyDlg, CDialog )
BEGIN_MESSAGE_MAP( SparseMode_PropertyDlg, CDialog )
    ON_WM_HSCROLL()
    ON_BN_CLICKED(IDC_SPARSE_MODE_RESET, &SparseMode_PropertyDlg::OnBnClickedSparseModeReset)
    ON_BN_CLICKED(IDC_CHK_ENABLE_SPARSE_MODE, &SparseMode_PropertyDlg::OnBnClickedEnableSparseMode)
    ON_BN_CLICKED(IDC_SPARSE_MODE_SAVE_CONFIG, &SparseMode_PropertyDlg::OnBnClickedSparseModeSaveConfig)
END_MESSAGE_MAP()

SparseMode_PropertyDlg::SparseMode_PropertyDlg( void*&                  hApcDI,
                                      DEVSELINFO&             devSelInfo,
                                      const DEVINFORMATIONEX& devinfoEx,
                                      CWnd*                   pParent )
	: CDialog(IDD_SPARSE_MODE, pParent), m_hApcDI(hApcDI), m_DevSelInfo(devSelInfo), m_xDevinfoEx(devinfoEx)
{
}

SparseMode_PropertyDlg::~SparseMode_PropertyDlg()
{
}

BOOL SparseMode_PropertyDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
    InitUI();
	return TRUE;
}

void SparseMode_PropertyDlg::DoDataExchange( CDataExchange* pDX )
{
	CDialog::DoDataExchange( pDX );
}

void SparseMode_PropertyDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{

    const long irValue = ((CSliderCtrl*)pScrollBar)->GetPos();

    if (m_SliderCtrl_DEPTH_LRCHECK_DIFF == (CSliderCtrl*)pScrollBar)
    {
        UpdateUI_SliderCtrl_DEPTH_LRCHECK_DIFF(irValue);
        if (nSBCode == SB_ENDSCROLL)
        {
            TRACE("SB_ENDSCROLL\n");
            UpdateSparseModeReg(CONFIG_DEPTH_LRCHECK_DIFF, (int)irValue);
        }
    }
    else if (m_SliderCtrl_SGBM_SAD_THD == (CSliderCtrl*)pScrollBar)
    {
        UpdateUI_SliderCtrl_SGBM_SAD_THD(irValue);
        if (nSBCode == SB_ENDSCROLL)
        {
            TRACE("SB_ENDSCROLL\n");
            UpdateSparseModeReg(CONFIG_SGBM_SAD_THD, (int)irValue);
        }
    }
    else if (m_SliderCtrl_SGBM_SAD_RATIO == (CSliderCtrl*)pScrollBar)
    {
        UpdateUI_SliderCtrl_SGBM_SAD_RATIO(irValue);
        if (nSBCode == SB_ENDSCROLL)
        {
            TRACE("SB_ENDSCROLL\n");
            UpdateSparseModeReg(CONFIG_SGBM_SAD_RATIO, (int)irValue);
        }
    }
    else if (m_SliderCtrl_TEXT_LMT == (CSliderCtrl*)pScrollBar)
    {
        UpdateUI_SliderCtrl_TEXT_LMT(irValue);
        if (nSBCode == SB_ENDSCROLL)
        {
            TRACE("SB_ENDSCROLL\n");
            UpdateSparseModeReg(CONFIG_TEXT_LMT, (int)irValue);
        }
    }
    else if (m_SliderCtrl_TEXT_PGAIN == (CSliderCtrl*)pScrollBar)
    {
        UpdateUI_SliderCtrl_TEXT_PGAIN(irValue);
        if (nSBCode == SB_ENDSCROLL)
        {
            TRACE("SB_ENDSCROLL\n");
            UpdateSparseModeReg(CONFIG_TEXT_PGAIN, (int)irValue);
        }
    }
    else if (m_SliderCtrl_TEXT_NGAIN == (CSliderCtrl*)pScrollBar)
    {
        UpdateUI_SliderCtrl_TEXT_NGAIN(irValue);
        if (nSBCode == SB_ENDSCROLL)
        {
            TRACE("SB_ENDSCROLL\n");
            UpdateSparseModeReg(CONFIG_TEXT_NGAIN, (int)irValue);
        }
    }
    else if (m_SliderCtrl_SPARSE_THD == (CSliderCtrl*)pScrollBar)
    {
        UpdateUI_SliderCtrl_SPARSE_THD(irValue);
        if (nSBCode == SB_ENDSCROLL)
        {
            TRACE("SB_ENDSCROLL\n");
            UpdateSparseModeReg(CONFIG_SPARSE_THD, (int)irValue);
        }
    }
    CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void SparseMode_PropertyDlg::InitUI()
{
    TRACE("SparseMode_PropertyDlg InitUI\n");
    ReadConfig();

    m_SliderCtrl_DEPTH_LRCHECK_DIFF = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_DEPTH_LRCHECK_DIFF);
    m_SliderCtrl_SGBM_SAD_THD = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_SGBM_SAD_THD);
    m_SliderCtrl_SGBM_SAD_RATIO = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_SGBM_SAD_RATIO2);
    m_SliderCtrl_TEXT_LMT = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_TEXT_LMT);
    m_SliderCtrl_TEXT_PGAIN = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_TEXT_PGAIN);
    m_SliderCtrl_TEXT_NGAIN = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_TEXT_NGAIN);
    m_SliderCtrl_SPARSE_THD = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_SPARSE_THD);

    UpdateUI_SliderCtrl_DEPTH_LRCHECK_DIFF_Range(0, 7);
    UpdateUI_SliderCtrl_DEPTH_LRCHECK_DIFF(Default_DEPTH_LRCHECK_DIFF_Value);

    UpdateUI_SliderCtrl_SGBM_SAD_THD_Range(0, 15);
    UpdateUI_SliderCtrl_SGBM_SAD_THD(Default_SGBM_SAD_THD_Value);

    UpdateUI_SliderCtrl_SGBM_SAD_RATIO_Range(0, 15);
    UpdateUI_SliderCtrl_SGBM_SAD_RATIO(Default_SGBM_SAD_RATIO_Value);

    UpdateUI_SliderCtrl_TEXT_LMT_Range(0, 127);
    UpdateUI_SliderCtrl_TEXT_LMT(Default_TEXT_LMT_Value);

    UpdateUI_SliderCtrl_TEXT_PGAIN_Range(0, 7);
    UpdateUI_SliderCtrl_TEXT_PGAIN(Default_TEXT_PGAIN_Value);

    UpdateUI_SliderCtrl_TEXT_NGAIN_Range(0, 7);
    UpdateUI_SliderCtrl_TEXT_NGAIN(Default_TEXT_NGAIN_Value);

    UpdateUI_SliderCtrl_SPARSE_THD_Range(0, 31);
    UpdateUI_SliderCtrl_SPARSE_THD(Default_SPARSE_THD_Value);

    EnableSliderCtrl(false);
}

void SparseMode_PropertyDlg::UpdateUI_SliderCtrl_DEPTH_LRCHECK_DIFF(long value)
{
    m_SliderCtrl_DEPTH_LRCHECK_DIFF->SetPos(value);
    SetDlgItemInt(IDC_STATIC_DEPTH_LRCHECK_DIFF_RANGE, value);
}

void SparseMode_PropertyDlg::UpdateUI_SliderCtrl_DEPTH_LRCHECK_DIFF_Range(long min, long max)
{
    m_SliderCtrl_DEPTH_LRCHECK_DIFF->SetRange(min, max);
    SetDlgItemInt(IDC_ST_DEPTH_LRCHECK_DIFF_MIN, min);
    SetDlgItemInt(IDC_ST_DEPTH_LRCHECK_DIFF_MAX, max);
}

void SparseMode_PropertyDlg::UpdateUI_SliderCtrl_SGBM_SAD_THD(long value)
{
    m_SliderCtrl_SGBM_SAD_THD->SetPos(value);
    SetDlgItemInt(IDC_STATIC_SGBM_SAD_THD_RANGE, value);
}

void SparseMode_PropertyDlg::UpdateUI_SliderCtrl_SGBM_SAD_THD_Range(long min, long max)
{
    m_SliderCtrl_SGBM_SAD_THD->SetRange(min, max);
    SetDlgItemInt(IDC_ST_SGBM_SAD_THD_MIN, min);
    SetDlgItemInt(IDC_ST_SGBM_SAD_THD_MAX, max);
}

void SparseMode_PropertyDlg::UpdateUI_SliderCtrl_SGBM_SAD_RATIO(long value)
{
    m_SliderCtrl_SGBM_SAD_RATIO->SetPos(value);
    SetDlgItemInt(IDC_STATIC_SGBM_SAD_RATIO_RANGE, value);
}

void SparseMode_PropertyDlg::UpdateUI_SliderCtrl_SGBM_SAD_RATIO_Range(long min, long max)
{
    m_SliderCtrl_SGBM_SAD_RATIO->SetRange(min, max);
    SetDlgItemInt(IDC_ST_SGBM_SGBM_SAD_RATIO_MIN, min);
    SetDlgItemInt(IDC_ST_SGBM_SAD_RATIO_MAX, max);
}

void SparseMode_PropertyDlg::UpdateUI_SliderCtrl_TEXT_LMT(long value)
{
    m_SliderCtrl_TEXT_LMT->SetPos(value);
    SetDlgItemInt(IDC_STATIC_TEXT_LMT_RANGE, value);
}

void SparseMode_PropertyDlg::UpdateUI_SliderCtrl_TEXT_LMT_Range(long min, long max)
{
    m_SliderCtrl_TEXT_LMT->SetRange(min, max);
    SetDlgItemInt(IDC_ST_TEXT_LMT_MIN, min);
    SetDlgItemInt(IDC_ST_TEXT_LMT_MAX, max);
}

void SparseMode_PropertyDlg::UpdateUI_SliderCtrl_TEXT_PGAIN(long value)
{
    m_SliderCtrl_TEXT_PGAIN->SetPos(value);
    SetDlgItemInt(IDC_STATIC_TEXT_PGAIN_RANGE, value);
}

void SparseMode_PropertyDlg::UpdateUI_SliderCtrl_TEXT_PGAIN_Range(long min, long max)
{
    m_SliderCtrl_TEXT_PGAIN->SetRange(min, max);
    SetDlgItemInt(IDC_ST_TEXT_PGAIN_MIN, min);
    SetDlgItemInt(IDC_ST_TEXT_PGAIN_MAX, max);
}

void SparseMode_PropertyDlg::UpdateUI_SliderCtrl_TEXT_NGAIN(long value)
{
    m_SliderCtrl_TEXT_NGAIN->SetPos(value);
    SetDlgItemInt(IDC_STATIC_TEXT_NGAIN_RANGE, value);
}

void SparseMode_PropertyDlg::UpdateUI_SliderCtrl_TEXT_NGAIN_Range(long min, long max)
{
    m_SliderCtrl_TEXT_NGAIN->SetRange(min, max);
    SetDlgItemInt(IDC_ST_TEXT_NGAIN_MIN, min);
    SetDlgItemInt(IDC_ST_TEXT_NGAIN_MAX, max);
}

void SparseMode_PropertyDlg::UpdateUI_SliderCtrl_SPARSE_THD(long value)
{
    m_SliderCtrl_SPARSE_THD->SetPos(value);
    SetDlgItemInt(IDC_STATIC_SPARSE_THD_RANGE, value);
}

void SparseMode_PropertyDlg::UpdateUI_SliderCtrl_SPARSE_THD_Range(long min, long max)
{
    m_SliderCtrl_SPARSE_THD->SetRange(min, max);
    SetDlgItemInt(IDC_ST_SPARSE_THD_MIN, min);
    SetDlgItemInt(IDC_ST_SPARSE_THD_MAX, max);
}

BOOL SparseMode_PropertyDlg::ReadConfig()
{
    BOOL bRet = TRUE;
    TCHAR buff[MAX_PATH];
    memset(buff, 0, MAX_PATH);
    ::GetModuleFileName(NULL, buff, sizeof(buff));
    CString strFolder = buff;
    CString strPath_Application = strFolder.Left(strFolder.ReverseFind(_T('\\')) + 1);

    CString profile_file;
    if (PathFileExists(strPath_Application + _T(SPARSE_MODE_CONFIG_OVERRIDE_FILENAME)))
    {
        profile_file = strPath_Application + _T(SPARSE_MODE_CONFIG_OVERRIDE_FILENAME);
    }
    else
    {
        profile_file = strPath_Application + _T(SPARSE_MODE_CONFIG_DEFAULT_FILENAME);
    }

    CString strAppName, strKeyName;
    CString strData;
    strAppName = _T(CFG_SECTION_NAME);

    strData.Empty();
    strKeyName = _T(CFG_DEPTH_LRCHECK_DIFF);
    GetPrivateProfileString(strAppName, strKeyName, _T(""), strData.GetBuffer(MAX_PATH), MAX_PATH, profile_file);
    Default_DEPTH_LRCHECK_DIFF_Value = _ttoi(strData);
    strData.ReleaseBuffer();

    strData.Empty();
    strKeyName = _T(CFG_SGBM_SAD_THD);
    GetPrivateProfileString(strAppName, strKeyName, _T(""), strData.GetBuffer(MAX_PATH), MAX_PATH, profile_file);
    Default_SGBM_SAD_THD_Value = _ttoi(strData);
    strData.ReleaseBuffer();

    strData.Empty();
    strKeyName = _T(CFG_SGBM_SAD_RATIO);
    GetPrivateProfileString(strAppName, strKeyName, _T(""), strData.GetBuffer(MAX_PATH), MAX_PATH, profile_file);
    Default_SGBM_SAD_RATIO_Value = _ttoi(strData);
    strData.ReleaseBuffer();

    strData.Empty();
    strKeyName = _T(CFG_TEXT_LMT);
    GetPrivateProfileString(strAppName, strKeyName, _T(""), strData.GetBuffer(MAX_PATH), MAX_PATH, profile_file);
    Default_TEXT_LMT_Value = _ttoi(strData);
    strData.ReleaseBuffer();

    strData.Empty();
    strKeyName = _T(CFG_TEXT_PGAIN);
    GetPrivateProfileString(strAppName, strKeyName, _T(""), strData.GetBuffer(MAX_PATH), MAX_PATH, profile_file);
    Default_TEXT_PGAIN_Value = _ttoi(strData);
    strData.ReleaseBuffer();

    strData.Empty();
    strKeyName = _T(CFG_TEXT_NGAIN);
    GetPrivateProfileString(strAppName, strKeyName, _T(""), strData.GetBuffer(MAX_PATH), MAX_PATH, profile_file);
    Default_TEXT_NGAIN_Value = _ttoi(strData);
    strData.ReleaseBuffer();

    strData.Empty();
    strKeyName = _T(CFG_SPARSE_THD);
    GetPrivateProfileString(strAppName, strKeyName, _T(""), strData.GetBuffer(MAX_PATH), MAX_PATH, profile_file);
    Default_SPARSE_THD_Value = _ttoi(strData);
    strData.ReleaseBuffer();

    return TRUE;
}

void SparseMode_PropertyDlg::UpdateSparseModeDefault()
{
    UpdateSparseModeReg(CONFIG_DEPTH_LRCHECK_DIFF, Default_DEPTH_LRCHECK_DIFF_Value);
    UpdateSparseModeReg(CONFIG_SGBM_SAD_THD, Default_SGBM_SAD_THD_Value);
    UpdateSparseModeReg(CONFIG_SGBM_SAD_RATIO, Default_SGBM_SAD_RATIO_Value);
    UpdateSparseModeReg(CONFIG_TEXT_LMT, Default_TEXT_LMT_Value);
    UpdateSparseModeReg(CONFIG_TEXT_PGAIN, Default_TEXT_PGAIN_Value);
    UpdateSparseModeReg(CONFIG_TEXT_NGAIN, Default_TEXT_NGAIN_Value);
    UpdateSparseModeReg(CONFIG_SPARSE_THD, Default_SPARSE_THD_Value);
}

void SparseMode_PropertyDlg::UpdateSparseModeReg(int SparseMode, int value)
{
    USHORT RegValue = 0;
    USHORT ADDRESS = 0;
    USHORT NotValidDataRange = 0;
    int ret = 0;


    if (SparseMode == CONFIG_DEPTH_LRCHECK_DIFF)
    {
        ADDRESS = DEPTH_LRCHECK_DIFF_ADDRESS;
        NotValidDataRange = 0xf8; //bit[2:0]

        TRACE("ADDRESS = DEPTH_LRCHECK_DIFF_ADDRESS\n");
        ret = APC_GetHWRegister(m_hApcDI, &m_DevSelInfo, ADDRESS, &RegValue, FG_Address_2Byte | FG_Value_1Byte);
        TRACE("GetHWRegister ret:%d, ADDRESS:%04x RegValue: %04x\n", ret, ADDRESS, RegValue);
        RegValue = RegValue & NotValidDataRange;
        TRACE("NotValidDataRange RegValue: %04x, NotValidDataRange:%04x\n", RegValue, NotValidDataRange);
        RegValue |= value;
        TRACE("Data RegValue: %04x\n", RegValue);
    }
    else if ((SparseMode == CONFIG_SGBM_SAD_THD))
    {
        ADDRESS = SGBM_SAD_THD_ADDRESS;
        NotValidDataRange = 0x0f; //bit[7:4]

        TRACE("ADDRESS = SGBM_SAD_THD_ADDRESS\n");
        ret = APC_GetHWRegister(m_hApcDI, &m_DevSelInfo, ADDRESS, &RegValue, FG_Address_2Byte | FG_Value_1Byte);
        TRACE("GetHWRegister ret:%d, ADDRESS:%04x RegValue: %04x\n", ret, ADDRESS, RegValue);
        RegValue = RegValue & NotValidDataRange;
        TRACE("NotValidDataRange RegValue: %04x, NotValidDataRange:%04x\n", RegValue, NotValidDataRange);
        value = value & 0x0f;
        value = value << 4;
        RegValue |= value;
        TRACE("Data RegValue: %04x\n", RegValue);
    }
    else if ((SparseMode == CONFIG_SGBM_SAD_RATIO))
    {
        ADDRESS = SGBM_SAD_RATIO_ADDRESS;
        NotValidDataRange = 0xf0; //bit[3:0]

        TRACE("ADDRESS = SGBM_SAD_RATIO_ADDRESS\n");
        ret = APC_GetHWRegister(m_hApcDI, &m_DevSelInfo, ADDRESS, &RegValue, FG_Address_2Byte | FG_Value_1Byte);
        TRACE("GetHWRegister ret:%d, ADDRESS:%04x RegValue: %04x\n", ret, ADDRESS, RegValue);
        RegValue = RegValue & NotValidDataRange;
        TRACE("NotValidDataRange RegValue: %04x, NotValidDataRange:%04x\n", RegValue, NotValidDataRange);
        RegValue |= value;
        TRACE("Data RegValue: %04x\n", RegValue);
    }
    else if ((SparseMode == CONFIG_TEXT_LMT))
    {
        ADDRESS = TEXT_LMT_ADDRESS;
        NotValidDataRange = 0xc0; //bit[6:0]

        TRACE("ADDRESS = TEXT_LMT_ADDRESS\n");
        ret = APC_GetHWRegister(m_hApcDI, &m_DevSelInfo, ADDRESS, &RegValue, FG_Address_2Byte | FG_Value_1Byte);
        TRACE("GetHWRegister ret:%d, ADDRESS:%04x RegValue: %04x\n", ret, ADDRESS, RegValue);
        RegValue = RegValue & NotValidDataRange;
        TRACE("NotValidDataRange RegValue: %04x, NotValidDataRange:%04x\n", RegValue, NotValidDataRange);
        RegValue |= value;
        TRACE("Data RegValue: %04x\n", RegValue);
    }
    else if ((SparseMode == CONFIG_TEXT_PGAIN))
    {
        ADDRESS = TEXT_PGAIN_ADDRESS;
        NotValidDataRange = 0x8f; //bit[6:4]

        TRACE("ADDRESS = TEXT_PGAIN_ADDRESS\n");
        ret = APC_GetHWRegister(m_hApcDI, &m_DevSelInfo, ADDRESS, &RegValue, FG_Address_2Byte | FG_Value_1Byte);
        TRACE("GetHWRegister ret:%d, ADDRESS:%04x RegValue: %04x\n", ret, ADDRESS, RegValue);
        RegValue = RegValue & NotValidDataRange;
        TRACE("NotValidDataRange RegValue: %04x, NotValidDataRange:%04x\n", RegValue, NotValidDataRange);
        value = value & 0x07;
        value = value << 4;
        RegValue |= value;
        TRACE("Data RegValue: %04x\n", RegValue);
    }
    else if ((SparseMode == CONFIG_TEXT_NGAIN))
    {
        ADDRESS = TEXT_NGAIN_ADDRESS;
        NotValidDataRange = 0xf8; //bit[2:0]

        TRACE("ADDRESS = TEXT_NGAIN_ADDRESS\n");
        ret = APC_GetHWRegister(m_hApcDI, &m_DevSelInfo, ADDRESS, &RegValue, FG_Address_2Byte | FG_Value_1Byte);
        TRACE("GetHWRegister ret:%d, ADDRESS:%04x RegValue: %04x\n", ret, ADDRESS, RegValue);
        RegValue = RegValue & NotValidDataRange;
        TRACE("NotValidDataRange RegValue: %04x, NotValidDataRange:%04x\n", RegValue, NotValidDataRange);
        RegValue |= value;
        TRACE("Data RegValue: %04x\n", RegValue);
    }
    else if ((SparseMode == CONFIG_SPARSE_THD))
    {
        ADDRESS = SPARSE_THD_ADDRESS;
        NotValidDataRange = 0xe0; //bit[4:0]

        TRACE("ADDRESS = SPARSE_THD_ADDRESS\n");
        ret = APC_GetHWRegister(m_hApcDI, &m_DevSelInfo, ADDRESS, &RegValue, FG_Address_2Byte | FG_Value_1Byte);
        TRACE("GetHWRegister ret:%d, ADDRESS:%04x RegValue: %04x\n", ret, ADDRESS, RegValue);
        RegValue = RegValue & NotValidDataRange;
        TRACE("NotValidDataRange RegValue: %04x, NotValidDataRange:%04x\n", RegValue, NotValidDataRange);
        RegValue |= value;
        TRACE("Data RegValue: %04x\n", RegValue);
    }

    ret = APC_SetHWRegister(m_hApcDI, &m_DevSelInfo, ADDRESS, RegValue, FG_Address_2Byte | FG_Value_1Byte);
    TRACE("SetHWRegister ret:%d, ADDRESS:%04x RegValue: %04x\n", ret, ADDRESS, RegValue);
    USHORT RegValueCheck = 0;
    ret = APC_GetHWRegister(m_hApcDI, &m_DevSelInfo, ADDRESS, &RegValueCheck, FG_Address_2Byte | FG_Value_1Byte);
    TRACE("GetHWRegister ret:%d, ADDRESS:%04x RegValueCheck: %04x\n", ret, ADDRESS, RegValueCheck);
}

void SparseMode_PropertyDlg::UpdateSparseModeConfig(bool enable)
{
    USHORT RegValue = 0;
    USHORT ADDRESS = 0;
    int ret = 0;
    USHORT RegValueCheck = 0;

    ADDRESS = DEPTH_LRCHECK_BYPASS_ADDRESS;
    TRACE("ADDRESS = DEPTH_LRCHECK_BYPASS_ADDRESS\n");
    ret = APC_GetHWRegister(m_hApcDI, &m_DevSelInfo, ADDRESS, &RegValue, FG_Address_2Byte | FG_Value_1Byte);
    TRACE("GetHWRegister ret:%d, ADDRESS:%04x RegValue: %04x\n", ret, ADDRESS, RegValue);
    if(enable)
        RegValue = RegValue & ~(1 << 7); //bit[7] off
    else
        RegValue = RegValue | (1 << 7); //bit[7] on
    TRACE("after bit ctrl RegValue: %04x, \n", RegValue);
    ret = APC_SetHWRegister(m_hApcDI, &m_DevSelInfo, ADDRESS, RegValue, FG_Address_2Byte | FG_Value_1Byte);
    TRACE("SetHWRegister ret:%d, ADDRESS:%04x RegValue: %04x\n", ret, ADDRESS, RegValue);
    ret = APC_GetHWRegister(m_hApcDI, &m_DevSelInfo, ADDRESS, &RegValueCheck, FG_Address_2Byte | FG_Value_1Byte);
    TRACE("GetHWRegister ret:%d, ADDRESS:%04x RegValueCheck: %04x\n", ret, ADDRESS, RegValueCheck);

    ADDRESS = DEPTH_SAD_ENB_ADDRESS;
    TRACE("ADDRESS = DEPTH_SAD_ENB_ADDRESS\n");
    ret = APC_GetHWRegister(m_hApcDI, &m_DevSelInfo, ADDRESS, &RegValue, FG_Address_2Byte | FG_Value_1Byte);
    TRACE("GetHWRegister ret:%d, ADDRESS:%04x RegValue: %04x\n", ret, ADDRESS, RegValue);
    if (enable)
        RegValue = RegValue | (1 << 2); //bit[2] on
    else
        RegValue = RegValue & ~(1 << 2); //bit[2] off
    TRACE("after bit ctrl RegValue: %04x, \n", RegValue);
    ret = APC_SetHWRegister(m_hApcDI, &m_DevSelInfo, ADDRESS, RegValue, FG_Address_2Byte | FG_Value_1Byte);
    TRACE("SetHWRegister ret:%d, ADDRESS:%04x RegValue: %04x\n", ret, ADDRESS, RegValue);
    ret = APC_GetHWRegister(m_hApcDI, &m_DevSelInfo, ADDRESS, &RegValueCheck, FG_Address_2Byte | FG_Value_1Byte);
    TRACE("GetHWRegister ret:%d, ADDRESS:%04x RegValueCheck: %04x\n", ret, ADDRESS, RegValueCheck);

    ADDRESS = TEXT_MODE_ADDRESS;
    TRACE("ADDRESS = TEXT_MODE_ADDRESS\n");
    ret = APC_GetHWRegister(m_hApcDI, &m_DevSelInfo, ADDRESS, &RegValue, FG_Address_2Byte | FG_Value_1Byte);
    TRACE("GetHWRegister ret:%d, ADDRESS:%04x RegValue: %04x\n", ret, ADDRESS, RegValue);
    if (enable)
        RegValue = RegValue | (1 << 7); //bit[7] on
    else
        RegValue = RegValue & ~(1 << 7); //bit[7] off
    TRACE("after bit ctrl RegValue: %04x, \n", RegValue);
    ret = APC_SetHWRegister(m_hApcDI, &m_DevSelInfo, ADDRESS, RegValue, FG_Address_2Byte | FG_Value_1Byte);
    TRACE("SetHWRegister ret:%d, ADDRESS:%04x RegValue: %04x\n", ret, ADDRESS, RegValue);
    ret = APC_GetHWRegister(m_hApcDI, &m_DevSelInfo, ADDRESS, &RegValueCheck, FG_Address_2Byte | FG_Value_1Byte);
    TRACE("GetHWRegister ret:%d, ADDRESS:%04x RegValueCheck: %04x\n", ret, ADDRESS, RegValueCheck);

    ADDRESS = SPARSE_ENB_ADDRESS;
    TRACE("ADDRESS = SPARSE_ENB_ADDRESS\n");
    ret = APC_GetHWRegister(m_hApcDI, &m_DevSelInfo, ADDRESS, &RegValue, FG_Address_2Byte | FG_Value_1Byte);
    TRACE("GetHWRegister ret:%d, ADDRESS:%04x RegValue: %04x\n", ret, ADDRESS, RegValue);
    if (enable)
        RegValue = RegValue | (1 << 7); //bit[7] on
    else
        RegValue = RegValue & ~(1 << 7); //bit[7] off
    TRACE("after bit ctrl RegValue: %04x, \n", RegValue);
    ret = APC_SetHWRegister(m_hApcDI, &m_DevSelInfo, ADDRESS, RegValue, FG_Address_2Byte | FG_Value_1Byte);
    TRACE("SetHWRegister ret:%d, ADDRESS:%04x RegValue: %04x\n", ret, ADDRESS, RegValue);
    ret = APC_GetHWRegister(m_hApcDI, &m_DevSelInfo, ADDRESS, &RegValueCheck, FG_Address_2Byte | FG_Value_1Byte);
    TRACE("GetHWRegister ret:%d, ADDRESS:%04x RegValueCheck: %04x\n", ret, ADDRESS, RegValueCheck);
}

void SparseMode_PropertyDlg::OnBnClickedSparseModeReset()
{
    ReadConfig();
    UpdateUI_SliderCtrl_DEPTH_LRCHECK_DIFF(Default_DEPTH_LRCHECK_DIFF_Value);
    UpdateUI_SliderCtrl_SGBM_SAD_THD(Default_SGBM_SAD_THD_Value);
    UpdateUI_SliderCtrl_SGBM_SAD_RATIO(Default_SGBM_SAD_RATIO_Value);
    UpdateUI_SliderCtrl_TEXT_LMT(Default_TEXT_LMT_Value);
    UpdateUI_SliderCtrl_TEXT_PGAIN(Default_TEXT_PGAIN_Value);
    UpdateUI_SliderCtrl_TEXT_NGAIN(Default_TEXT_NGAIN_Value);
    UpdateUI_SliderCtrl_SPARSE_THD(Default_SPARSE_THD_Value);
    UpdateSparseModeDefault();
}

void SparseMode_PropertyDlg::EnableSliderCtrl(bool enable)
{
    GetDlgItem(IDC_SLIDER_DEPTH_LRCHECK_DIFF)->EnableWindow(enable);
    GetDlgItem(IDC_SLIDER_SGBM_SAD_THD)->EnableWindow(enable);
    GetDlgItem(IDC_SLIDER_SGBM_SAD_RATIO2)->EnableWindow(enable);
    GetDlgItem(IDC_SLIDER_TEXT_LMT)->EnableWindow(enable);
    GetDlgItem(IDC_SLIDER_TEXT_PGAIN)->EnableWindow(enable);
    GetDlgItem(IDC_SLIDER_TEXT_NGAIN)->EnableWindow(enable);
    GetDlgItem(IDC_SLIDER_SPARSE_THD)->EnableWindow(enable);
    GetDlgItem(IDC_SPARSE_MODE_RESET)->EnableWindow(enable);
    GetDlgItem(IDC_SPARSE_MODE_SAVE_CONFIG)->EnableWindow(enable);
}

void SparseMode_PropertyDlg::OnBnClickedEnableSparseMode()
{
    const BOOL checked = ((CButton*)GetDlgItem(IDC_CHK_ENABLE_SPARSE_MODE))->GetCheck();
    if (!checked)
        return;
    ((CButton*)GetDlgItem(IDC_CHK_ENABLE_SPARSE_MODE))->EnableWindow(false);
    UpdateSparseModeConfig(checked);
    OnBnClickedSparseModeReset();
    EnableSliderCtrl(checked);
}

void SparseMode_PropertyDlg::OnBnClickedSparseModeSaveConfig()
{
    BOOL bRet = TRUE;
    TCHAR buff[MAX_PATH];
    memset(buff, 0, MAX_PATH);
    ::GetModuleFileName(NULL, buff, sizeof(buff));
    CString strFolder = buff;
    CString strPath_Application = strFolder.Left(strFolder.ReverseFind(_T('\\')) + 1);

    CString profile_file = strPath_Application + _T(SPARSE_MODE_CONFIG_OVERRIDE_FILENAME);

    CString strAppName, strKeyName;
    CString strData;
    strAppName = _T(CFG_SECTION_NAME);
    int value;

    strKeyName = _T(CFG_DEPTH_LRCHECK_DIFF);
    value = ((CSliderCtrl*)GetDlgItem(IDC_SLIDER_DEPTH_LRCHECK_DIFF))->GetPos();
    strData.Format(_T("%d"), value);
    WritePrivateProfileString(strAppName, strKeyName, strData, profile_file);

    strKeyName = _T(CFG_SGBM_SAD_THD);
    value = ((CSliderCtrl*)GetDlgItem(IDC_SLIDER_SGBM_SAD_THD))->GetPos();
    strData.Format(_T("%d"), value);
    WritePrivateProfileString(strAppName, strKeyName, strData, profile_file);

    strKeyName = _T(CFG_SGBM_SAD_RATIO);
    value = ((CSliderCtrl*)GetDlgItem(IDC_SLIDER_SGBM_SAD_RATIO2))->GetPos();
    strData.Format(_T("%d"), value);
    WritePrivateProfileString(strAppName, strKeyName, strData, profile_file);

    strKeyName = _T(CFG_TEXT_LMT);
    value = ((CSliderCtrl*)GetDlgItem(IDC_SLIDER_TEXT_LMT))->GetPos();
    strData.Format(_T("%d"), value);
    WritePrivateProfileString(strAppName, strKeyName, strData, profile_file);

    strKeyName = _T(CFG_TEXT_PGAIN);
    value = ((CSliderCtrl*)GetDlgItem(IDC_SLIDER_TEXT_PGAIN))->GetPos();
    strData.Format(_T("%d"), value);
    WritePrivateProfileString(strAppName, strKeyName, strData, profile_file);

    strKeyName = _T(CFG_TEXT_NGAIN);
    value = ((CSliderCtrl*)GetDlgItem(IDC_SLIDER_TEXT_NGAIN))->GetPos();
    strData.Format(_T("%d"), value);
    WritePrivateProfileString(strAppName, strKeyName, strData, profile_file);

    strKeyName = _T(CFG_SPARSE_THD);
    value = ((CSliderCtrl*)GetDlgItem(IDC_SLIDER_SPARSE_THD))->GetPos();
    strData.Format(_T("%d"), value);
    WritePrivateProfileString(strAppName, strKeyName, strData, profile_file);
}