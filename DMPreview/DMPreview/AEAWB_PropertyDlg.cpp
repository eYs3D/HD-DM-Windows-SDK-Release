
#include "stdafx.h"
#include "EtronDI_Test.h"
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
END_MESSAGE_MAP()

AEAWB_PropertyDlg::AEAWB_PropertyDlg( void*&                  hEtronDI, 
                                      DEVSELINFO&             devSelInfo,
                                      const DEVINFORMATIONEX& devinfoEx,
                                      CWnd*                   pParent )
	: CDialog(IDD_AEAWB_PROPERTY_DIALOG, pParent), m_hEtronDI(hEtronDI), m_DevSelInfo(devSelInfo), m_xDevinfoEx(devinfoEx)
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
	case ETronDI_PID_8060://0x0152: //8060
        {
        m_pComboBox->SetItemData( m_pComboBox->AddString(L"Kolor"), ETronDI_PID_8060_K );//0x0150 );
        m_pComboBox->SetItemData( m_pComboBox->AddString(L"Track"), ETronDI_PID_8060_T );//0x0151 );
        m_pComboBox->SetItemData( m_pComboBox->AddString(L"Depth"), ETronDI_PID_8060 );//0x0152 );
        }
        break;
    case ETronDI_PID_8040S://0x0131: //8040
        {
        m_pComboBox->SetItemData( m_pComboBox->AddString(L"Color"), ETronDI_PID_8040S );//0x0131 ); // esp777
        m_pComboBox->SetItemData( m_pComboBox->AddString(L"Kolor"), ETronDI_PID_8040S_K );//0x0149 ); // AR1335
        }
        break;
    case ETronDI_PID_8054://0x0139: //8054
        {
        m_pComboBox->SetItemData( m_pComboBox->AddString(L"Color"), ETronDI_PID_8054 );//0x0139 ); // esp777
        m_pComboBox->SetItemData( m_pComboBox->AddString(L"Kolor"), ETronDI_PID_8054_K );//0x0143 ); // AR1335
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
}

void AEAWB_PropertyDlg::ReadProperty()
{
    const int pid = ( int )m_pComboBox->GetItemData( m_pComboBox->GetCurSel() );

	long max, min, step, default, capsflag, cur1, cur2;

    int hr = ETronDI_DEVICE_NOT_SUPPORT;

    { // WB
        do
        {
            hr = EtronDI_PropertyPU_GetRange( m_hEtronDI, &m_DevSelInfo, PU_PROPERTY_ID_WHITEBALANCE, &min, &max, &step, &default, &capsflag, pid );

            if ( SUCCEEDED( hr ) )
            {
		        UpdateUI_WBTemperatureRange( min, max );

		        hr = EtronDI_PropertyPU_GetCurrent( m_hEtronDI, &m_DevSelInfo, PU_PROPERTY_ID_WHITEBALANCE, &cur1, &cur2, &capsflag, pid );

		        if ( SUCCEEDED( hr ) &&
                     SUCCEEDED( EtronDI_PropertyPU_SetCurrent( m_hEtronDI, &m_DevSelInfo, PU_PROPERTY_ID_WHITEBALANCE, cur1, cur2, capsflag, pid ) ) )
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
	    hr = EtronDI_PropertyCT_GetRange( m_hEtronDI, &m_DevSelInfo, CT_PROPERTY_ID_EXPOSURE, &min, &max, &step, &default, &capsflag, pid );

        do
        {
            hr = EtronDI_PropertyCT_GetRange( m_hEtronDI, &m_DevSelInfo, CT_PROPERTY_ID_EXPOSURE, &min, &max, &step, &default, &capsflag, pid );

            if ( SUCCEEDED( hr ) )
            {
		        UpdateUI_ExposureTimeRange(min, max);

		        hr = EtronDI_PropertyCT_GetCurrent( m_hEtronDI, &m_DevSelInfo, CT_PROPERTY_ID_EXPOSURE, &cur1, &cur2, &capsflag, pid );

		        if ( SUCCEEDED( hr ) &&
                     SUCCEEDED( EtronDI_PropertyCT_SetCurrent( m_hEtronDI, &m_DevSelInfo, CT_PROPERTY_ID_EXPOSURE, cur1, cur2, capsflag, pid ) ) )
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
	    hr = EtronDI_PropertyPU_GetCurrent( m_hEtronDI, &m_DevSelInfo, PU_PROPERTY_ID_POWERLINE_FREQUENCY, &cur1, &cur2, &capsflag, pid );

	    if ( SUCCEEDED( hr ) )
        {
		    UpdateUI_LightSource(cur1);
	    }
        EnableUI_LightSource( SUCCEEDED( hr ) );
    }
    { // LLC
	    hr = EtronDI_PropertyItem_Read( m_hEtronDI, &m_DevSelInfo, PROPSETID_VIDCAP_CAMERACONTROL, KSPROPERTY_CAMERACONTROL_AUTO_EXPOSURE_PRIORITY, &cur1, pid );

	    if ( SUCCEEDED( hr ) &&
             SUCCEEDED( EtronDI_PropertyItem_Write( m_hEtronDI, &m_DevSelInfo, PROPSETID_VIDCAP_CAMERACONTROL, KSPROPERTY_CAMERACONTROL_AUTO_EXPOSURE_PRIORITY, cur1, pid ) ) )
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
    const int  pid       = ( int )m_pComboBox->GetItemData( m_pComboBox->GetCurSel() );
    const BOOL isChecked = m_CButtonAE->GetCheck();
    const long value     = m_ExposureTimeSliderCtrl->GetPos();
    const long capsflag  = isChecked ? 1 : 2;
    const int  hr        = EtronDI_PropertyCT_SetCurrent(m_hEtronDI, &m_DevSelInfo, CT_PROPERTY_ID_EXPOSURE, value, 0, capsflag, pid);

	if ( SUCCEEDED( hr ) )
    {
		UpdateUI_AE( isChecked );
	}
}

void AEAWB_PropertyDlg::OnBnClickedCheckAWB()
{
    const int  pid       = ( int )m_pComboBox->GetItemData( m_pComboBox->GetCurSel() );
    const BOOL isChecked = m_CButtonAWB->GetCheck();
    const long value     = m_WBTemperatureSliderCtrl->GetPos();
    const long capsflag  = isChecked ? 1 : 2;
    const int  hr        = EtronDI_PropertyPU_SetCurrent(m_hEtronDI, &m_DevSelInfo, PU_PROPERTY_ID_WHITEBALANCE, value, 0, capsflag, pid );

	if ( SUCCEEDED( hr ) )
    {
		UpdateUI_AWB( isChecked );
	}
}

void AEAWB_PropertyDlg::OnBnClickedRadio50hz()
{
    EtronDI_PropertyPU_SetCurrent( m_hEtronDI,
                                   &m_DevSelInfo,
                                   PU_PROPERTY_ID_POWERLINE_FREQUENCY,
                                   1,
                                   0,
                                   0,
                                   ( int )m_pComboBox->GetItemData( m_pComboBox->GetCurSel() ) );
}

void AEAWB_PropertyDlg::OnBnClickedRadio60hz()
{
    EtronDI_PropertyPU_SetCurrent( m_hEtronDI,
                                   &m_DevSelInfo,
                                   PU_PROPERTY_ID_POWERLINE_FREQUENCY,
                                   2,
                                   0,
                                   0,
                                   ( int )m_pComboBox->GetItemData( m_pComboBox->GetCurSel() ) );
}

void AEAWB_PropertyDlg::OnBnClickedRadioLlcOn()
{
    EtronDI_PropertyItem_Write( m_hEtronDI,
                                &m_DevSelInfo,
                                PROPSETID_VIDCAP_CAMERACONTROL,
                                KSPROPERTY_CAMERACONTROL_AUTO_EXPOSURE_PRIORITY,
                                1,
                                ( int )m_pComboBox->GetItemData( m_pComboBox->GetCurSel() ) );
}

void AEAWB_PropertyDlg::OnBnClickedRadioLlcOff()
{
    EtronDI_PropertyItem_Write( m_hEtronDI,
                                &m_DevSelInfo,
                                PROPSETID_VIDCAP_CAMERACONTROL,
                                KSPROPERTY_CAMERACONTROL_AUTO_EXPOSURE_PRIORITY,
                                0,
                                ( int )m_pComboBox->GetItemData( m_pComboBox->GetCurSel() ) );
}

void AEAWB_PropertyDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    const int pid = ( int )m_pComboBox->GetItemData( m_pComboBox->GetCurSel() );

    const long irValue = ( ( CSliderCtrl* )pScrollBar )->GetPos();

	if ( m_ExposureTimeSliderCtrl == ( CSliderCtrl* )pScrollBar )
	{
		UpdateUI_ExposureTimeValue(irValue);

        EtronDI_PropertyCT_SetCurrent( m_hEtronDI, &m_DevSelInfo, CT_PROPERTY_ID_EXPOSURE, irValue, 0, 2, pid );
	}
	else if ( m_WBTemperatureSliderCtrl == ( CSliderCtrl* )pScrollBar )
	{
		UpdateUI_WBTemperatureValue(irValue);

        EtronDI_PropertyPU_SetCurrent( m_hEtronDI, &m_DevSelInfo, PU_PROPERTY_ID_WHITEBALANCE, irValue, 0, 2, pid );
	}
	CDialog::OnHScroll( nSBCode, nPos, pScrollBar );
}

void AEAWB_PropertyDlg::OnBnClickedBtPropertyReset()
{
    const int pid = ( int )m_pComboBox->GetItemData( m_pComboBox->GetCurSel() );

    CheckRadioButton( IDC_RADIO_50HZ,   IDC_RADIO_60HZ,    IDC_RADIO_60HZ    );
    CheckRadioButton( IDC_RADIO_LLC_ON, IDC_RADIO_LLC_OFF, IDC_RADIO_LLC_OFF );

    UpdateUI_ExposureTimeValue( m_ExposureTimeSliderCtrl->GetRangeMin() );
    UpdateUI_WBTemperatureValue( 5500 );

    EtronDI_PropertyCT_SetCurrent( m_hEtronDI, &m_DevSelInfo, CT_PROPERTY_ID_EXPOSURE, m_ExposureTimeSliderCtrl->GetRangeMin(), 0, 2, pid );
    EtronDI_PropertyPU_SetCurrent( m_hEtronDI, &m_DevSelInfo, PU_PROPERTY_ID_WHITEBALANCE, 5500, 0, 2, pid );

    m_CButtonAE->SetCheck( TRUE );
    m_CButtonAWB->SetCheck( TRUE );

    OnBnClickedCheckAutoExposure();
    OnBnClickedCheckAWB();
    OnBnClickedRadioLlcOff();
    OnBnClickedRadio60hz(); 
}

void AEAWB_PropertyDlg::OnBnClickedButtonExptimeRead()
{
    const int pid = ( int )m_pComboBox->GetItemData( m_pComboBox->GetCurSel() );

    float value = NULL;

    EtronDI_GetExposureTime( m_hEtronDI, &m_DevSelInfo, EtronDI_SensorMode::SensorAll, pid, &value );

    CString csText;

    csText.Format( L"%.4f", value );

    SetDlgItemText( IDC_EDIT_EXPTIME, csText );
}

void AEAWB_PropertyDlg::OnBnClickedButtonExptimeWrite()
{
    const int pid = ( int )m_pComboBox->GetItemData( m_pComboBox->GetCurSel() );

    CString csText;

    GetDlgItemText( IDC_EDIT_EXPTIME, csText );

    const float value = ( float )_wtof( csText );

    EtronDI_SetExposureTime( m_hEtronDI, &m_DevSelInfo, EtronDI_SensorMode::SensorAll, pid, value );
}

void AEAWB_PropertyDlg::OnBnClickedButtonGlogalGainRead()
{
    const int pid = ( int )m_pComboBox->GetItemData( m_pComboBox->GetCurSel() );

    float value = NULL;

    EtronDI_GetGlobalGain( m_hEtronDI, &m_DevSelInfo, EtronDI_SensorMode::SensorAll, pid, &value );

    CString csText;

    csText.Format( L"%.4f", value );

    SetDlgItemText( IDC_EDIT_GLOBAL_GAIN, csText );
}

void AEAWB_PropertyDlg::OnBnClickedButtonGlogalGainWrite()
{
    const int pid = ( int )m_pComboBox->GetItemData( m_pComboBox->GetCurSel() );

    CString csText;

    GetDlgItemText( IDC_EDIT_GLOBAL_GAIN, csText );

    const float value = ( float )_wtof( csText );

    EtronDI_SetGlobalGain( m_hEtronDI, &m_DevSelInfo, EtronDI_SensorMode::SensorAll, pid, value );
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
