
#include "stdafx.h"
#include "APC_Test.h"
#include "AEAWB_PropertyDlg.h"
#include "afxdialogex.h"
#include <ks.h>
#include <KsMedia.h>	//for PropertyPage item R/W

IMPLEMENT_DYNAMIC( AEAWB_PropertyDlg, CDialog )
BEGIN_MESSAGE_MAP( AEAWB_PropertyDlg, CDialog )
    ON_WM_HSCROLL()
    ON_CBN_SELCHANGE( IDC_COMBO_DEVICE_TYPE_CTRL, &AEAWB_PropertyDlg::OnCbnSelchangeComboDeviceType )
    ON_BN_CLICKED( IDC_CHECK_AE,      &AEAWB_PropertyDlg::OnBnClickedCheckAutoExposure )
    ON_BN_CLICKED( IDC_CHECK_AWB,     &AEAWB_PropertyDlg::OnBnClickedCheckAWB )
    ON_BN_CLICKED( IDC_RADIO_50HZ,    &AEAWB_PropertyDlg::OnBnClickedRadio50hz )
    ON_BN_CLICKED( IDC_RADIO_60HZ,    &AEAWB_PropertyDlg::OnBnClickedRadio60hz )
    ON_BN_CLICKED( IDC_RADIO_LLC_ON,  &AEAWB_PropertyDlg::OnBnClickedRadioLlcOn )
    ON_BN_CLICKED( IDC_RADIO_LLC_OFF, &AEAWB_PropertyDlg::OnBnClickedRadioLlcOff )
    ON_BN_CLICKED( IDC_BT_PROPERTY_RESET,        &AEAWB_PropertyDlg::OnBnClickedBtPropertyReset       )
    ON_BN_CLICKED( IDC_BUTTON_EXPTIME_READ,      &AEAWB_PropertyDlg::OnBnClickedButtonExptimeRead     )
    ON_BN_CLICKED( IDC_BUTTON_EXPTIME_WRITE,     &AEAWB_PropertyDlg::OnBnClickedButtonExptimeWrite    )
    ON_BN_CLICKED( IDC_BUTTON_GLOGAL_GAIN_READ,  &AEAWB_PropertyDlg::OnBnClickedButtonGlogalGainRead  )
    ON_BN_CLICKED( IDC_BUTTON_GLOGAL_GAIN_WRITE, &AEAWB_PropertyDlg::OnBnClickedButtonGlogalGainWrite )
    ON_BN_CLICKED( IDC_BUTTON_ANALOG_GAIN_READ,  &AEAWB_PropertyDlg::OnBnClickedButtonAnalogGainRead  )
    ON_BN_CLICKED( IDC_BUTTON_ANALOG_GAIN_WRITE, &AEAWB_PropertyDlg::OnBnClickedButtonAnalogGainWrite )
    ON_BN_CLICKED( IDC_BUTTON_DIGITAL_GAIN_READ, &AEAWB_PropertyDlg::OnBnClickedButtonDigitalGainRead )
    ON_BN_CLICKED( IDC_BUTTON_DIGITAL_GAIN_WRITE,&AEAWB_PropertyDlg::OnBnClickedButtonDigitalGainWrite)
    ON_EN_CHANGE(  IDC_EDIT_ANALOG_GAIN,         &AEAWB_PropertyDlg::OnEnChangeEditAnalogGain         )
    ON_EN_CHANGE(  IDC_EDIT_DIGITAL_GAIN,        &AEAWB_PropertyDlg::OnEnChangeEditDigitalGain        )
    ON_NOTIFY(UDN_DELTAPOS,IDC_SPIN_ANALOG_GAIN, &AEAWB_PropertyDlg::OnDeltaposSpinAnalogGain         )
    ON_NOTIFY(UDN_DELTAPOS,IDC_SPIN_DIGITAL_GAIN,&AEAWB_PropertyDlg::OnDeltaposSpinDigitalGain        )
END_MESSAGE_MAP()

AEAWB_PropertyDlg::AEAWB_PropertyDlg( void*&                  hApcDI, 
                                      DEVSELINFO&             devSelInfo,
                                      const DEVINFORMATIONEX& devinfoEx,
                                      CWnd*                   pParent )
	: CDialog(IDD_AEAWB_PROPERTY_DIALOG, pParent), m_hApcDI(hApcDI), m_DevSelInfo(devSelInfo), m_xDevinfoEx(devinfoEx)
	, m_radio_btn_group1(FALSE)
	, m_radio_btn_group2(FALSE)
{
}

AEAWB_PropertyDlg::~AEAWB_PropertyDlg()
{
}

BOOL AEAWB_PropertyDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	InitUI();

	return TRUE;
}

void AEAWB_PropertyDlg::DoDataExchange( CDataExchange* pDX )
{
	CDialog::DoDataExchange( pDX );

	DDX_Radio( pDX, IDC_RADIO_50HZ,   m_radio_btn_group1 );
	DDX_Radio( pDX, IDC_RADIO_LLC_ON, m_radio_btn_group2 );
}

void AEAWB_PropertyDlg::InitUI()
{
	m_pComboBox = ( CComboBox* )GetDlgItem( IDC_COMBO_DEVICE_TYPE_CTRL );

    switch ( m_xDevinfoEx.wPID )
    {
	case APC_PID_8060://0x0152: //8060
        {
        m_pComboBox->SetItemData( m_pComboBox->AddString(L"Kolor"), APC_PID_8060_K );//0x0150 );
        m_pComboBox->SetItemData( m_pComboBox->AddString(L"Track"), APC_PID_8060_T );//0x0151 );
        m_pComboBox->SetItemData( m_pComboBox->AddString(L"Depth"), APC_PID_8060 );//0x0152 );
        }
        break;
    case APC_PID_8040S://0x0131: //8040
        {
        m_pComboBox->SetItemData( m_pComboBox->AddString(L"Color"), APC_PID_8040S );//0x0131 ); // esp777
        m_pComboBox->SetItemData( m_pComboBox->AddString(L"Kolor"), APC_PID_8040S_K );//0x0149 ); // AR1335
        }
        break;
    case APC_PID_8054://0x0139: //8054
        {
        m_pComboBox->SetItemData( m_pComboBox->AddString(L"Color"), APC_PID_8054 );//0x0139 ); // esp777
        m_pComboBox->SetItemData( m_pComboBox->AddString(L"Kolor"), APC_PID_8054_K );//0x0143 ); // AR1335
        }
        break;
    case APC_PID_8063://0x0164: //8063
        {
            m_pComboBox->SetItemData(m_pComboBox->AddString(L"Color"), APC_PID_8063);
            m_pComboBox->SetItemData(m_pComboBox->AddString(L"Kolor"), APC_PID_8063_K);
        }
    break;
    default: m_pComboBox->SetItemData( m_pComboBox->AddString(L"Color"), m_xDevinfoEx.wPID ); break;
    }
    m_pComboBox->SetCurSel(0);

	m_CButtonAE               = ( CButton*     )GetDlgItem( IDC_CHECK_AE              );
	m_CButtonAWB              = ( CButton*     )GetDlgItem( IDC_CHECK_AWB             );
	m_CButtonLightSource50Hz  = ( CButton*     )GetDlgItem( IDC_RADIO_50HZ            );
	m_CButtonLightSource60Hz  = ( CButton*     )GetDlgItem( IDC_RADIO_60HZ            );
	m_CButtonLLC_On           = ( CButton*     )GetDlgItem( IDC_RADIO_LLC_ON          );
	m_CButtonLLC_Off          = ( CButton*     )GetDlgItem( IDC_RADIO_LLC_OFF         );
	m_ExposureTimeSliderCtrl  = ( CSliderCtrl* )GetDlgItem( IDC_SLIDER_EXPOSURE_TIME  );
	m_WBTemperatureSliderCtrl = ( CSliderCtrl* )GetDlgItem( IDC_SLIDER_WB_TEMPERATURE );

	ReadProperty();

    CSpinButtonCtrl *spinAnalogGain = (CSpinButtonCtrl *)GetDlgItem(IDC_SPIN_ANALOG_GAIN);
    spinAnalogGain->SetBuddy(GetDlgItem(IDC_EDIT_ANALOG_GAIN));
    CSpinButtonCtrl *spinDigitalGain = (CSpinButtonCtrl *)GetDlgItem(IDC_SPIN_DIGITAL_GAIN);
    spinDigitalGain->SetBuddy(GetDlgItem(IDC_EDIT_DIGITAL_GAIN));
}

void AEAWB_PropertyDlg::ReadProperty()
{
    int pid = ( int )m_pComboBox->GetItemData( m_pComboBox->GetCurSel() );
    if (pid == APC_PID_IVY2) pid = APC_PID_IVY2_S;
	long max, min, step, default, capsflag, cur1, cur2;

    int hr = APC_DEVICE_NOT_SUPPORT;

    { // WB
        do
        {
            hr = APC_PropertyPU_GetRange( m_hApcDI, &m_DevSelInfo, PU_PROPERTY_ID_WHITEBALANCE, &min, &max, &step, &default, &capsflag, pid );

            if ( SUCCEEDED( hr ) )
            {
		        UpdateUI_WBTemperatureRange( min, max );

		        hr = APC_PropertyPU_GetCurrent( m_hApcDI, &m_DevSelInfo, PU_PROPERTY_ID_WHITEBALANCE, &cur1, &cur2, &capsflag, pid );

		        if ( SUCCEEDED( hr ) &&
                     SUCCEEDED( APC_PropertyPU_SetCurrent( m_hApcDI, &m_DevSelInfo, PU_PROPERTY_ID_WHITEBALANCE, cur1, cur2, capsflag, pid ) ) )
                {
                    EnableUI_WB( TRUE );
			        UpdateUI_WBTemperatureValue( cur1 );
			        UpdateUI_AWB( capsflag == 1 );

                    break;
		        }
	        }
            EnableUI_WB( FALSE );
        }
        while( FALSE );

        if ( 0x0151 == pid ) EnableUI_WB( FALSE ); // 8060 track WB not support yet
    }
    { // AE
	    hr = APC_PropertyCT_GetRange( m_hApcDI, &m_DevSelInfo, CT_PROPERTY_ID_EXPOSURE, &min, &max, &step, &default, &capsflag, pid );

        do
        {
            hr = APC_PropertyCT_GetRange( m_hApcDI, &m_DevSelInfo, CT_PROPERTY_ID_EXPOSURE, &min, &max, &step, &default, &capsflag, pid );

            if ( SUCCEEDED( hr ) )
            {
		        UpdateUI_ExposureTimeRange(min, max);

		        hr = APC_PropertyCT_GetCurrent( m_hApcDI, &m_DevSelInfo, CT_PROPERTY_ID_EXPOSURE, &cur1, &cur2, &capsflag, pid );

		        if ( SUCCEEDED( hr ) &&
                     SUCCEEDED( APC_PropertyCT_SetCurrent( m_hApcDI, &m_DevSelInfo, CT_PROPERTY_ID_EXPOSURE, cur1, cur2, capsflag, pid ) ) )
                {
                    EnableUI_AE( TRUE );
			        UpdateUI_ExposureTimeValue( cur1 );
			        UpdateUI_AE( capsflag == 1 );

                    break;
		        }
	        }
            EnableUI_AE( FALSE );
        }
        while( FALSE );
    }
    { // LightSource
	    hr = APC_PropertyPU_GetCurrent( m_hApcDI, &m_DevSelInfo, PU_PROPERTY_ID_POWERLINE_FREQUENCY, &cur1, &cur2, &capsflag, pid );

	    if ( SUCCEEDED( hr ) )
        {
		    UpdateUI_LightSource(cur1);
	    }
        EnableUI_LightSource( SUCCEEDED( hr ) );
    }
    { // LLC
	    hr = APC_PropertyItem_Read( m_hApcDI, &m_DevSelInfo, PROPSETID_VIDCAP_CAMERACONTROL, KSPROPERTY_CAMERACONTROL_AUTO_EXPOSURE_PRIORITY, &cur1, pid );

	    if ( SUCCEEDED( hr ) &&
             SUCCEEDED( APC_PropertyItem_Write( m_hApcDI, &m_DevSelInfo, PROPSETID_VIDCAP_CAMERACONTROL, KSPROPERTY_CAMERACONTROL_AUTO_EXPOSURE_PRIORITY, cur1, pid ) ) )
        {
		    UpdateUI_LLC(cur1);
	    }
        EnableUI_LLC( SUCCEEDED( hr ) );
    }
	TRACE("ReadProperty");
}

void AEAWB_PropertyDlg::UpdateUI_ExposureTimeValue(long value)
{
	m_ExposureTimeSliderCtrl->SetPos(value);

    SetDlgItemInt( IDC_STATIC_EXPOSURE_TIME_RANGE, value );
}

void AEAWB_PropertyDlg::UpdateUI_ExposureTimeRange(long min, long max)
{
	m_ExposureTimeSliderCtrl->SetRange(min, max);

    SetDlgItemInt( IDC_ST_AE_MIN, min );
    SetDlgItemInt( IDC_ST_AE_MAX, max );
}

void AEAWB_PropertyDlg::UpdateUI_AE(BOOL enable)
{
	if (enable) {
		m_CButtonAE->SetCheck(BST_CHECKED);
		m_ExposureTimeSliderCtrl->EnableWindow(false);
	}
	else {
		m_CButtonAE->SetCheck(BST_UNCHECKED);
		m_ExposureTimeSliderCtrl->EnableWindow(true);
	}
    GetDlgItem( IDC_BUTTON_EXPTIME_WRITE     )->EnableWindow( !m_CButtonAE->GetCheck() );
    GetDlgItem( IDC_EDIT_EXPTIME             )->EnableWindow( !m_CButtonAE->GetCheck() );
    GetDlgItem( IDC_BUTTON_GLOGAL_GAIN_WRITE )->EnableWindow( !m_CButtonAE->GetCheck() );
    GetDlgItem( IDC_EDIT_GLOBAL_GAIN         )->EnableWindow( !m_CButtonAE->GetCheck() );


    const int pid = ( int )m_pComboBox->GetItemData( m_pComboBox->GetCurSel() );
    if (pid == APC_PID_IVY || pid == APC_PID_IVY2 || pid == APC_PID_IVY3)
    {
        GetDlgItem(IDC_GAIN_CONTROL)->ShowWindow(SW_SHOW);
        GetDlgItem(IDC_TEXT_ANALOG_GAIN)->ShowWindow(SW_SHOW);
        GetDlgItem(IDC_TEXT_DIGITAL_GAIN)->ShowWindow(SW_SHOW);
        GetDlgItem(IDC_EDIT_ANALOG_GAIN)->ShowWindow(SW_SHOW);
        GetDlgItem(IDC_EDIT_DIGITAL_GAIN)->ShowWindow(SW_SHOW);
        GetDlgItem(IDC_SPIN_ANALOG_GAIN)->ShowWindow(SW_SHOW);
        GetDlgItem(IDC_SPIN_DIGITAL_GAIN)->ShowWindow(SW_SHOW);
        GetDlgItem(IDC_BUTTON_ANALOG_GAIN_READ)->ShowWindow(SW_SHOW);
        GetDlgItem(IDC_BUTTON_DIGITAL_GAIN_READ)->ShowWindow(SW_SHOW);
        GetDlgItem(IDC_BUTTON_ANALOG_GAIN_WRITE)->ShowWindow(SW_SHOW);
        GetDlgItem(IDC_BUTTON_DIGITAL_GAIN_WRITE)->ShowWindow(SW_SHOW);
        GetDlgItem(IDC_LIST_GAIN_REGISTER)->ShowWindow(SW_SHOW);

        GetDlgItem(IDC_BUTTON_ANALOG_GAIN_WRITE)->EnableWindow(!m_CButtonAE->GetCheck());
        GetDlgItem(IDC_EDIT_ANALOG_GAIN)->EnableWindow(!m_CButtonAE->GetCheck());
        GetDlgItem(IDC_BUTTON_DIGITAL_GAIN_WRITE)->EnableWindow(!m_CButtonAE->GetCheck());
        GetDlgItem(IDC_EDIT_DIGITAL_GAIN)->EnableWindow(!m_CButtonAE->GetCheck());

        GetDlgItem(IDC_BUTTON_GLOGAL_GAIN_WRITE)->EnableWindow(false);
    }
}

void AEAWB_PropertyDlg::EnableUI_AE(BOOL enable)
{
	m_CButtonAE->EnableWindow(enable);
	m_ExposureTimeSliderCtrl->EnableWindow(enable);
}

void AEAWB_PropertyDlg::UpdateUI_WBTemperatureValue(long value)
{
	m_WBTemperatureSliderCtrl->SetPos(value);

    SetDlgItemInt( IDC_STATIC_AB_TEMPERATURE_RANGE, value );
}

void AEAWB_PropertyDlg::UpdateUI_WBTemperatureRange(long min, long max)
{
	m_WBTemperatureSliderCtrl->SetRange(min, max);

    SetDlgItemInt( IDC_ST_AWB_MIN, min );
    SetDlgItemInt( IDC_ST_AWB_MAX, max );
}

void AEAWB_PropertyDlg::UpdateUI_AWB(BOOL enable)
{
	if (enable) {
		m_CButtonAWB->SetCheck(BST_CHECKED);
		m_WBTemperatureSliderCtrl->EnableWindow(false);
	}
	else {
		m_CButtonAWB->SetCheck(BST_UNCHECKED);
		m_WBTemperatureSliderCtrl->EnableWindow(true);
	}
}

void AEAWB_PropertyDlg::EnableUI_WB(BOOL enable)
{
	m_CButtonAWB->EnableWindow(enable);
	m_WBTemperatureSliderCtrl->EnableWindow(enable);
}

void AEAWB_PropertyDlg::EnableUI_LightSource(BOOL enable)
{
	m_CButtonLightSource50Hz->EnableWindow(enable);
	m_CButtonLightSource60Hz->EnableWindow(enable);
}

void AEAWB_PropertyDlg::EnableUI_LLC(BOOL enable)
{
	m_CButtonLLC_On->EnableWindow(enable);
	m_CButtonLLC_Off->EnableWindow(enable);
}

void AEAWB_PropertyDlg::UpdateUI_LightSource(long source)
{
	m_CButtonLightSource50Hz->SetCheck(source == 1 ? BST_CHECKED : BST_UNCHECKED);
	m_CButtonLightSource60Hz->SetCheck(source == 2 ? BST_CHECKED : BST_UNCHECKED);
}

void AEAWB_PropertyDlg::UpdateUI_LLC(long value)
{
	m_CButtonLLC_On->SetCheck(value == 1 ? BST_CHECKED : BST_UNCHECKED);
	m_CButtonLLC_Off->SetCheck(value == 0 ? BST_CHECKED : BST_UNCHECKED);
}

void AEAWB_PropertyDlg::OnCbnSelchangeComboDeviceType()
{
	ReadProperty();
}

void AEAWB_PropertyDlg::OnBnClickedCheckAutoExposure()
{
    int  pid       = ( int )m_pComboBox->GetItemData( m_pComboBox->GetCurSel() );
    if (pid == APC_PID_IVY2) pid = APC_PID_IVY2_S;
    const BOOL isChecked = m_CButtonAE->GetCheck();
    const long value     = m_ExposureTimeSliderCtrl->GetPos();
    const long capsflag  = isChecked ? 1 : 2;
    const int  hr        = APC_PropertyCT_SetCurrent(m_hApcDI, &m_DevSelInfo, CT_PROPERTY_ID_EXPOSURE, value, 0, capsflag, pid);

	if ( SUCCEEDED( hr ) )
    {
		UpdateUI_AE( isChecked );
	}
}

void AEAWB_PropertyDlg::OnBnClickedCheckAWB()
{
    int  pid       = ( int )m_pComboBox->GetItemData( m_pComboBox->GetCurSel() );
    if (pid == APC_PID_IVY2) pid = APC_PID_IVY2_S;
    const BOOL isChecked = m_CButtonAWB->GetCheck();
    const long value     = m_WBTemperatureSliderCtrl->GetPos();
    const long capsflag  = isChecked ? 1 : 2;
    const int  hr        = APC_PropertyPU_SetCurrent(m_hApcDI, &m_DevSelInfo, PU_PROPERTY_ID_WHITEBALANCE, value, 0, capsflag, pid );

	if ( SUCCEEDED( hr ) )
    {
		UpdateUI_AWB( isChecked );
	}
}

void AEAWB_PropertyDlg::OnBnClickedRadio50hz()
{
    int pid = (int)m_pComboBox->GetItemData(m_pComboBox->GetCurSel());
    if (pid == APC_PID_IVY2) pid = APC_PID_IVY2_S;
    APC_PropertyPU_SetCurrent( m_hApcDI,
                                   &m_DevSelInfo,
                                   PU_PROPERTY_ID_POWERLINE_FREQUENCY,
                                   1,
                                   0,
                                   0,
                                   pid);
}

void AEAWB_PropertyDlg::OnBnClickedRadio60hz()
{
    int pid = (int)m_pComboBox->GetItemData(m_pComboBox->GetCurSel());
    if (pid == APC_PID_IVY2) pid = APC_PID_IVY2_S;
    APC_PropertyPU_SetCurrent( m_hApcDI,
                                   &m_DevSelInfo,
                                   PU_PROPERTY_ID_POWERLINE_FREQUENCY,
                                   2,
                                   0,
                                   0,
                                   pid);
}

void AEAWB_PropertyDlg::OnBnClickedRadioLlcOn()
{
    int pid = (int)m_pComboBox->GetItemData(m_pComboBox->GetCurSel());
    if (pid == APC_PID_IVY2) pid = APC_PID_IVY2_S;
    APC_PropertyItem_Write( m_hApcDI,
                                &m_DevSelInfo,
                                PROPSETID_VIDCAP_CAMERACONTROL,
                                KSPROPERTY_CAMERACONTROL_AUTO_EXPOSURE_PRIORITY,
                                1,
                                pid);
}

void AEAWB_PropertyDlg::OnBnClickedRadioLlcOff()
{
    int pid = (int)m_pComboBox->GetItemData(m_pComboBox->GetCurSel());
    if (pid == APC_PID_IVY2) pid = APC_PID_IVY2_S;
    APC_PropertyItem_Write( m_hApcDI,
                                &m_DevSelInfo,
                                PROPSETID_VIDCAP_CAMERACONTROL,
                                KSPROPERTY_CAMERACONTROL_AUTO_EXPOSURE_PRIORITY,
                                0,
                                pid);
}

void AEAWB_PropertyDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    int pid = ( int )m_pComboBox->GetItemData( m_pComboBox->GetCurSel() );
    if (pid == APC_PID_IVY2) pid = APC_PID_IVY2_S;
    const long irValue = ( ( CSliderCtrl* )pScrollBar )->GetPos();

	if ( m_ExposureTimeSliderCtrl == ( CSliderCtrl* )pScrollBar )
	{
		UpdateUI_ExposureTimeValue(irValue);

        APC_PropertyCT_SetCurrent( m_hApcDI, &m_DevSelInfo, CT_PROPERTY_ID_EXPOSURE, irValue, 0, 2, pid );
	}
	else if ( m_WBTemperatureSliderCtrl == ( CSliderCtrl* )pScrollBar )
	{
		UpdateUI_WBTemperatureValue(irValue);

        APC_PropertyPU_SetCurrent( m_hApcDI, &m_DevSelInfo, PU_PROPERTY_ID_WHITEBALANCE, irValue, 0, 2, pid );
	}
	CDialog::OnHScroll( nSBCode, nPos, pScrollBar );
}

void AEAWB_PropertyDlg::OnBnClickedBtPropertyReset()
{
    int pid = ( int )m_pComboBox->GetItemData( m_pComboBox->GetCurSel() );
    if (pid == APC_PID_IVY2) pid = APC_PID_IVY2_S;
    CheckRadioButton( IDC_RADIO_50HZ,   IDC_RADIO_60HZ,    IDC_RADIO_60HZ    );
    CheckRadioButton( IDC_RADIO_LLC_ON, IDC_RADIO_LLC_OFF, IDC_RADIO_LLC_OFF );

    UpdateUI_ExposureTimeValue( m_ExposureTimeSliderCtrl->GetRangeMin() );
    UpdateUI_WBTemperatureValue( 5500 );

    APC_PropertyCT_SetCurrent( m_hApcDI, &m_DevSelInfo, CT_PROPERTY_ID_EXPOSURE, m_ExposureTimeSliderCtrl->GetRangeMin(), 0, 2, pid );
    APC_PropertyPU_SetCurrent( m_hApcDI, &m_DevSelInfo, PU_PROPERTY_ID_WHITEBALANCE, 5500, 0, 2, pid );

    m_CButtonAE->SetCheck( TRUE );
    m_CButtonAWB->SetCheck( TRUE );

    OnBnClickedCheckAutoExposure();
    OnBnClickedCheckAWB();
    OnBnClickedRadioLlcOff();
    OnBnClickedRadio60hz(); 
}

void AEAWB_PropertyDlg::OnBnClickedButtonExptimeRead()
{
    int pid = ( int )m_pComboBox->GetItemData( m_pComboBox->GetCurSel() );
    if (pid == APC_PID_IVY2) pid = APC_PID_IVY2_S;
    float value = NULL;

    APC_GetExposureTime( m_hApcDI, &m_DevSelInfo, APC_SensorMode::SensorAll, pid, &value );

    CString csText;

    csText.Format( L"%.4f", value );

    SetDlgItemText( IDC_EDIT_EXPTIME, csText );
}

void AEAWB_PropertyDlg::OnBnClickedButtonExptimeWrite()
{
    int pid = ( int )m_pComboBox->GetItemData( m_pComboBox->GetCurSel() );
    if (pid == APC_PID_IVY2) pid = APC_PID_IVY2_S;
    CString csText;

    GetDlgItemText( IDC_EDIT_EXPTIME, csText );

    const float value = ( float )_wtof( csText );

    APC_SetExposureTime( m_hApcDI, &m_DevSelInfo, APC_SensorMode::SensorAll, pid, value );
}

void AEAWB_PropertyDlg::OnBnClickedButtonGlogalGainRead()
{
    int pid = ( int )m_pComboBox->GetItemData( m_pComboBox->GetCurSel() );
    if (pid == APC_PID_IVY2) pid = APC_PID_IVY2_S;
    float value = NULL;

    APC_GetGlobalGain( m_hApcDI, &m_DevSelInfo, APC_SensorMode::SensorAll, pid, &value );

    CString csText;

    csText.Format( L"%.4f", value );

    SetDlgItemText( IDC_EDIT_GLOBAL_GAIN, csText );
}

void AEAWB_PropertyDlg::OnBnClickedButtonGlogalGainWrite()
{
    int pid = ( int )m_pComboBox->GetItemData( m_pComboBox->GetCurSel() );
    if (pid == APC_PID_IVY2) pid = APC_PID_IVY2_S;
    CString csText;

    GetDlgItemText( IDC_EDIT_GLOBAL_GAIN, csText );

    const float value = ( float )_wtof( csText );

    APC_SetGlobalGain( m_hApcDI, &m_DevSelInfo, APC_SensorMode::SensorAll, pid, value );
}

void AEAWB_PropertyDlg::UpdateGainRegisterList() {
    constexpr int kRegisterFlag = FG_Address_2Byte | FG_Value_1Byte;
    constexpr int kSensorSlaveAddress = 0x20;
    unsigned short gainReg;
    unsigned short gainValue;
    std::vector<std::pair<unsigned short, unsigned short>> gainRegisters;
    std::vector<std::string> resultStdStringList;

    gainRegisters.push_back(std::make_pair(0x044D, 0x0));
    gainRegisters.push_back(std::make_pair(0x0450, 0x0));
    gainRegisters.push_back(std::make_pair(0x0451, 0x0));
    gainRegisters.push_back(std::make_pair(0x0452, 0x0));
    gainRegisters.push_back(std::make_pair(0x0453, 0x0));
    gainRegisters.push_back(std::make_pair(0x0454, 0x0));
    gainRegisters.push_back(std::make_pair(0x0455, 0x0));
    gainRegisters.push_back(std::make_pair(0x0456, 0x0));
    gainRegisters.push_back(std::make_pair(0x0457, 0x0));

    CListBox* registerList = (CListBox*)GetDlgItem(IDC_LIST_GAIN_REGISTER);
    registerList->ResetContent();
    for (auto it : gainRegisters) {
        int ret = APC_GetSlaveSensorRegister(m_hApcDI, &m_DevSelInfo, kSensorSlaveAddress, it.first, &it.second, kRegisterFlag, APC_SensorMode::SensorAll);
        char buff[100];
        snprintf(buff, sizeof(buff), "Address 0x%x Value 0x%x", it.first, it.second);
        CString buffAsStdStr(buff);
        registerList->AddString(buffAsStdStr);
    }
}

void AEAWB_PropertyDlg::OnBnClickedButtonAnalogGainRead()
{
    int pid = ( int )m_pComboBox->GetItemData( m_pComboBox->GetCurSel() );
    if (pid == APC_PID_IVY2) pid = APC_PID_IVY2_S;
    float value = NULL;

    APC_GetAnalogGain( m_hApcDI, &m_DevSelInfo, APC_SensorMode::SensorAll, pid, &value );

    CString csText;

    csText.Format( L"%.4f", value );

    SetDlgItemText( IDC_EDIT_ANALOG_GAIN, csText );

    TRACE("AnalogGainRead\n");
    UpdateGainRegisterList();
}

void AEAWB_PropertyDlg::OnBnClickedButtonAnalogGainWrite()
{
    int pid = ( int )m_pComboBox->GetItemData( m_pComboBox->GetCurSel() );
    if (pid == APC_PID_IVY2) pid = APC_PID_IVY2_S;
    CString csText;

    GetDlgItemText( IDC_EDIT_ANALOG_GAIN, csText );

    const float value = ( float )_wtof( csText );

    APC_SetAnalogGain( m_hApcDI, &m_DevSelInfo, APC_SensorMode::SensorAll, pid, value );

    TRACE("AnalogGainWrite\n");
    UpdateGainRegisterList();
}

void AEAWB_PropertyDlg::OnBnClickedButtonDigitalGainRead()
{
    int pid = ( int )m_pComboBox->GetItemData( m_pComboBox->GetCurSel() );
    if (pid == APC_PID_IVY2) pid = APC_PID_IVY2_S;
    float value = NULL;

    APC_GetDigitalGain( m_hApcDI, &m_DevSelInfo, APC_SensorMode::SensorAll, pid, &value );

    CString csText;

    csText.Format( L"%.4f", value );

    SetDlgItemText( IDC_EDIT_DIGITAL_GAIN, csText );

    TRACE("DigitalGainRead\n");
    UpdateGainRegisterList();
}

void AEAWB_PropertyDlg::OnBnClickedButtonDigitalGainWrite()
{
    int pid = ( int )m_pComboBox->GetItemData( m_pComboBox->GetCurSel() );
    if (pid == APC_PID_IVY2) pid = APC_PID_IVY2_S;
    CString csText;

    GetDlgItemText( IDC_EDIT_DIGITAL_GAIN, csText );

    const float value = ( float )_wtof( csText );

    APC_SetDigitalGain( m_hApcDI, &m_DevSelInfo, APC_SensorMode::SensorAll, pid, value );

    TRACE("DigitalGainWrite\n");
    UpdateGainRegisterList();
}

void AEAWB_PropertyDlg::OnEnChangeEditAnalogGain()
{
    TRACE("OnEnChangeEditAnalogGain\n");
}

void AEAWB_PropertyDlg::OnEnChangeEditDigitalGain()
{
    TRACE("OnEnChangeEditDigitalGain\n");
}

void AEAWB_PropertyDlg::OnDeltaposSpinAnalogGain(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

    CString csText;
    GetDlgItemText(IDC_EDIT_ANALOG_GAIN, csText);
    float analog_gain = (float)_wtof(csText);

    float gap = 0.5f;
    float min_value = 1.0f;
    float max_value = 8.0f;

    if (pNMUpDown->iDelta < 0)
    {
        if (analog_gain - gap < min_value)
        {
            csText.Format(L"%.4f", min_value);
            SetDlgItemText(IDC_EDIT_ANALOG_GAIN, csText);
        }
        else
        {
            csText.Format(L"%.4f", (analog_gain - gap));
            SetDlgItemText(IDC_EDIT_ANALOG_GAIN, csText);
        }
    }
    else
    {
        if (analog_gain + gap > max_value)
        {
            csText.Format(L"%.4f", max_value);
            SetDlgItemText(IDC_EDIT_ANALOG_GAIN, csText);
        }
        else
        {
            csText.Format(L"%.4f", (analog_gain + gap));
            SetDlgItemText(IDC_EDIT_ANALOG_GAIN, csText);
        }
    }

    *pResult = 0;
}

void AEAWB_PropertyDlg::OnDeltaposSpinDigitalGain(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

    CString csText;
    GetDlgItemText(IDC_EDIT_DIGITAL_GAIN, csText);
    float digital_gain = (float)_wtof(csText);

    float gap = 0.003906f;
    float min_value = 1.0f;
    float max_value = 8.0f;

    if (pNMUpDown->iDelta < 0)
    {
        if (digital_gain - gap < min_value)
        {
            csText.Format(L"%.4f", min_value);
            SetDlgItemText(IDC_EDIT_DIGITAL_GAIN, csText);
        }
        else
        {
            csText.Format(L"%.4f", (digital_gain - gap));
            SetDlgItemText(IDC_EDIT_DIGITAL_GAIN, csText);
        }
    }
    else
    {
        if (digital_gain + gap > max_value)
        {
            csText.Format(L"%.4f", max_value);
            SetDlgItemText(IDC_EDIT_DIGITAL_GAIN, csText);
        }
        else
        {
            csText.Format(L"%.4f", (digital_gain + gap));
            SetDlgItemText(IDC_EDIT_DIGITAL_GAIN, csText);
        }
    }

    *pResult = 0;
}

bool AEAWB_PropertyDlg::IsLowLight()
{
	return IsDlgButtonChecked(IDC_RADIO_LLC_ON);
}

void AEAWB_PropertyDlg::SetLowLight(const BOOL IsLowLight)
{
	if (IsLowLight)
	{
		CheckRadioButton(IDC_RADIO_LLC_ON, IDC_RADIO_LLC_OFF, IDC_RADIO_LLC_ON);
		OnBnClickedRadioLlcOn();
	}
	else
	{
		CheckRadioButton(IDC_RADIO_LLC_ON, IDC_RADIO_LLC_OFF, IDC_RADIO_LLC_OFF);
		OnBnClickedRadioLlcOff();
	}
}

void AEAWB_PropertyDlg::LowLightEnable( const BOOL Enable )
{
    m_CButtonLLC_On->EnableWindow( Enable );
    m_CButtonLLC_Off->EnableWindow( Enable );
}
