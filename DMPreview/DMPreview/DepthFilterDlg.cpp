
#include "stdafx.h"
#include "EtronDI_Test.h"
#include "PreviewImageDlg.h"
#include "DepthFilterDlg.h"
#include "DepthDlg.h"
#include "AutoModuleSyncManager.h"

IMPLEMENT_DYNAMIC(DepthFilterDlg, CDialog)
BEGIN_MESSAGE_MAP(DepthFilterDlg, CDialog)
    ON_WM_HSCROLL()
    ON_CBN_SELCHANGE(IDC_CB_SUBSAMPLE_MODE, &DepthFilterDlg::OnCbnSelchangeCbSubsampleMode)
    ON_BN_CLICKED(IDC_CHK_FILTER_ENABLE, &DepthFilterDlg::OnBnClickedChkFilterEnable)
    ON_BN_CLICKED(IDC_CHK_SUBSAMPLE_ENABLE, &DepthFilterDlg::OnBnClickedChkSubsampleEnable)
    ON_BN_CLICKED(IDC_CHK_EPS_ENABLE, &DepthFilterDlg::OnBnClickedChkEpsEnable)
    ON_BN_CLICKED(IDC_CHK_HF_ENABLE, &DepthFilterDlg::OnBnClickedChkHfEnable)
    ON_BN_CLICKED(IDC_CHK_TF_ENABLE, &DepthFilterDlg::OnBnClickedChkTfEnable)
    ON_BN_CLICKED(IDC_CHK_HF_HRZ_ENABLE, &DepthFilterDlg::OnBnClickedChkHfHrzEnable)
    ON_CBN_SELCHANGE(IDC_CB_SUBSAMPLE_FACTOR, &DepthFilterDlg::OnCbnSelchangeCbSubsampleFactor)
    ON_BN_CLICKED(IDC_CHK_REMOVECURVE, &DepthFilterDlg::OnBnClickedChkRemovecurve)
    ON_BN_CLICKED(IDC_CHK_MIN, &DepthFilterDlg::OnBnClickedChkMin)
    ON_BN_CLICKED(IDC_CHK_FULL, &DepthFilterDlg::OnBnClickedChkFull)
END_MESSAGE_MAP()

DepthFilterDlg::DepthFilterDlg( const int PID, const USB_PORT_TYPE eUSB_Port_Type, CPreviewImageDlg* pPreviewDlg ) : CDialog( IDD_DLG_DEPTHFILTER ),
                                                                                                                     m_pPreviewDlg( pPreviewDlg ),
                                                                                                                     m_PID( PID ),
                                                                                                                     m_eUSB_Port_Type( eUSB_Port_Type )
{
}

DepthFilterDlg::~DepthFilterDlg()
{
}

void DepthFilterDlg::DoDataExchange( CDataExchange* pDX )
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_SLD_EPS_LEVEL, m_oEPSLevel);
    DDX_Control(pDX, IDC_SLD_TF_ALPHA, m_oTFAlpha);
    DDX_Control(pDX, IDC_SLD_HF_LEVEL, m_oHFLevel);
    DDX_Control(pDX, IDC_CB_SUBSAMPLE_MODE, m_oSampleMode);
    DDX_Control(pDX, IDC_CB_SUBSAMPLE_FACTOR, m_oSampleFactor);
}

BOOL DepthFilterDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    m_oEPSLevel.SetRange( 1, 10 );
    m_oTFAlpha.SetRange( 1, 9 );
    m_oHFLevel.SetRange( 1, 3 );

    m_oSampleMode.AddString( L"0" );
    m_oSampleMode.AddString( L"1" );

    ( ( CButton* )GetDlgItem( IDC_CHK_FILTER_ENABLE ) )->SetCheck( BST_CHECKED );
    ( ( CButton* )GetDlgItem( IDC_CHK_MIN ) )->SetCheck( BST_CHECKED );

	if (AutoModuleSyncManager::GetInstance()->IsAutoModuleSyncEnabled()) {
		((CButton*)GetDlgItem(IDC_CHK_FILTER_ENABLE))->SetCheck(BST_UNCHECKED);
	}

    DefaultSetting();
    OnBnClickedChkFilterEnable();

    CString csText(m_version.c_str());
    SetDlgItemText(IDC_EDIT_DF_VERSION, csText);

    return TRUE;
}

void DepthFilterDlg::OnHScroll( UINT nSBCode, UINT nPos, CScrollBar* pScrollBar )
{
    CDialog::OnHScroll( nSBCode, nPos, pScrollBar );

    CString csText;

    switch ( pScrollBar->GetDlgCtrlID() )
    {
    case IDC_SLD_EPS_LEVEL:
        {
            csText.Format( L"%d", m_oEPSLevel.GetPos() );

            SetDlgItemText( IDC_ST_EPS_LEVEL, csText );
        }
        break;
    case IDC_SLD_TF_ALPHA:
        {
            csText.Format( L"0.%d", m_oTFAlpha.GetPos() );

            SetDlgItemText( IDC_ST_TF_ALPHA, csText );
        }
        break;
    case IDC_SLD_HF_LEVEL:
        {
            csText.Format( L"%d", m_oHFLevel.GetPos() );

            SetDlgItemText( IDC_ST_HF_LEVEL, csText );
        }
        break;
    }
    UpdateFilterParam();
}

void DepthFilterDlg::OnCbnSelchangeCbSubsampleMode()
{
    m_oSampleFactor.ResetContent();

    if ( 1 == m_oSampleMode.GetCurSel() )
    {
        m_oSampleFactor.SetItemData( m_oSampleFactor.AddString( L"4" ), 4 );
        m_oSampleFactor.SetItemData( m_oSampleFactor.AddString( L"5" ), 5 );
    }
    else
    {
        m_oSampleFactor.SetItemData( m_oSampleFactor.AddString( L"3" ), 3 );
        m_oSampleFactor.SetItemData( m_oSampleFactor.AddString( L"2" ), 2 );
    }
    m_oSampleFactor.SetCurSel( NULL );

    UpdateFilterParam();
}

void DepthFilterDlg::OnCbnSelchangeCbSubsampleFactor()
{
    UpdateFilterParam();
}

void DepthFilterDlg::OnBnClickedChkFilterEnable()
{
    const BOOL bEnable = ( ( CButton* )GetDlgItem( IDC_CHK_FILTER_ENABLE ) )->GetCheck();

    GetDlgItem( IDC_CHK_MIN )->EnableWindow( bEnable );
    GetDlgItem( IDC_CHK_FULL )->EnableWindow( bEnable );

    if ( ( ( CButton* )GetDlgItem( IDC_CHK_MIN ) )->GetCheck() )
    {
        OnBnClickedChkMin();
    }
    else if ( ( ( CButton* )GetDlgItem( IDC_CHK_FULL ) )->GetCheck() )
    {
        OnBnClickedChkFull();
    }
    else
    {
        EnableControl( bEnable );

        if ( ETronDI_PID_8029 == m_PID )
        {
            ( ( CButton* )GetDlgItem( IDC_CHK_FILTER_ENABLE ) )->SetCheck( BST_CHECKED );
            ( ( CButton* )GetDlgItem( IDC_CHK_REMOVECURVE ) )->SetCheck( BST_CHECKED );
            GetDlgItem( IDC_CHK_FILTER_ENABLE )->EnableWindow( FALSE );
            GetDlgItem( IDC_CHK_REMOVECURVE )->EnableWindow( FALSE );
        }
        UpdateFilterParam();
    }
}

void DepthFilterDlg::OnBnClickedChkSubsampleEnable()
{
    UpdateFilterParam();
}

void DepthFilterDlg::OnBnClickedChkEpsEnable()
{
    UpdateFilterParam();
}

void DepthFilterDlg::OnBnClickedChkHfEnable()
{
    UpdateFilterParam();
}

void DepthFilterDlg::OnBnClickedChkTfEnable()
{
    UpdateFilterParam();
}

void DepthFilterDlg::OnBnClickedChkHfHrzEnable()
{
    UpdateFilterParam();
}

void DepthFilterDlg::OnBnClickedChkRemovecurve()
{
    UpdateFilterParam();
}

void DepthFilterDlg::OnBnClickedChkMin()
{
    ( ( CButton* )GetDlgItem( IDC_CHK_FULL ) )->SetCheck( BST_UNCHECKED );

    FullMinConfig( ( ( CButton* )GetDlgItem( IDC_CHK_MIN ) )->GetCheck(), FALSE );
}

void DepthFilterDlg::OnBnClickedChkFull()
{
    ( ( CButton* )GetDlgItem( IDC_CHK_MIN ) )->SetCheck( BST_UNCHECKED );

    FullMinConfig( ( ( CButton* )GetDlgItem( IDC_CHK_FULL ) )->GetCheck(), TRUE );
}

void DepthFilterDlg::DefaultSetting()
{
    m_oEPSLevel.SetPos( 1 );
    m_oTFAlpha.SetPos( 4 );
    m_oHFLevel.SetPos( 1 );

    m_oSampleMode.SetCurSel( NULL );

    SetDlgItemText( IDC_ED_TF_HISTORY, L"3" );
    SetDlgItemText( IDC_ED_EPS_LUMDA, L"0.1" );
    SetDlgItemText( IDC_ED_EPS_SIGMA, L"0.015" );
    SetDlgItemText( IDC_ST_EPS_LEVEL, L"1" );
    SetDlgItemText( IDC_ST_TF_ALPHA, L"0.4" );
    SetDlgItemText( IDC_ST_HF_LEVEL, L"1" );

    OnCbnSelchangeCbSubsampleMode();
}

void DepthFilterDlg::FullMinConfig( const BOOL bEnable, const BOOL bEnableRemoveCurve )
{
    ( ( CButton* )GetDlgItem( IDC_CHK_SUBSAMPLE_ENABLE ) )->SetCheck( BST_CHECKED );
    ( ( CButton* )GetDlgItem( IDC_CHK_EPS_ENABLE ) )->SetCheck( BST_CHECKED );
    ( ( CButton* )GetDlgItem( IDC_CHK_HF_ENABLE ) )->SetCheck( BST_CHECKED );
    ( ( CButton* )GetDlgItem( IDC_CHK_HF_HRZ_ENABLE ) )->SetCheck( BST_CHECKED );
    ( ( CButton* )GetDlgItem( IDC_CHK_TF_ENABLE ) )->SetCheck( BST_CHECKED );
    ( ( CButton* )GetDlgItem( IDC_CHK_REMOVECURVE ) )->SetCheck( bEnableRemoveCurve ? BST_CHECKED : BST_UNCHECKED );

    EnableControl( !bEnable );

    if ( bEnable ) DefaultSetting();

    if ( ETronDI_PID_8029 == m_PID )
    {
        ( ( CButton* )GetDlgItem( IDC_CHK_FILTER_ENABLE ) )->SetCheck( BST_CHECKED );
        ( ( CButton* )GetDlgItem( IDC_CHK_REMOVECURVE ) )->SetCheck( BST_CHECKED );
        GetDlgItem( IDC_CHK_FILTER_ENABLE )->EnableWindow( FALSE );
        GetDlgItem( IDC_CHK_REMOVECURVE )->EnableWindow( FALSE );
    }
    UpdateFilterParam();
}

void DepthFilterDlg::EnableControl( const BOOL bEnable )
{
    static const std::vector< int > vecControl = { IDC_CHK_REMOVECURVE, IDC_CHK_EPS_ENABLE, IDC_CHK_HF_ENABLE, IDC_CHK_TF_ENABLE,     IDC_CHK_HF_HRZ_ENABLE, 
                                                   IDC_SLD_EPS_LEVEL,   IDC_SLD_TF_ALPHA,   IDC_SLD_HF_LEVEL,  IDC_CB_SUBSAMPLE_MODE, IDC_CB_SUBSAMPLE_FACTOR,
                                                   IDC_CHK_SUBSAMPLE_ENABLE };
    for ( auto& i : vecControl )
    {
        GetDlgItem( i )->EnableWindow( bEnable );
    }
}

void DepthFilterDlg::UpdateFilterParam()
{
    static DepthfilterParam xDFParam;

    auto GetCheck = [ = ]( const int ResIdx )
    {
        return ( ( CButton* )GetDlgItem( ResIdx ) )->GetCheck() > NULL;
    };
    xDFParam.bDoDepthFilter = GetCheck( IDC_CHK_FILTER_ENABLE );
	xDFParam.bSubSample = GetCheck( IDC_CHK_SUBSAMPLE_ENABLE );
	xDFParam.bEdgePreServingFilter = GetCheck( IDC_CHK_EPS_ENABLE );
	xDFParam.bHoleFill = GetCheck( IDC_CHK_HF_ENABLE );
	xDFParam.bTempleFilter = GetCheck( IDC_CHK_TF_ENABLE );
	xDFParam.bFlyingDepthCancellation = GetCheck( IDC_CHK_REMOVECURVE );
//SubSample
	xDFParam.SubSampleMode = m_oSampleMode.GetCurSel();
	xDFParam.SubSampleFactor = m_oSampleFactor.GetItemData( m_oSampleFactor.GetCurSel() );
//EdgePreServingFilter
    CString csValue;
	xDFParam.nEdgeLevel = m_oEPSLevel.GetPos();

    GetDlgItemText( IDC_ED_EPS_LUMDA, csValue );
	xDFParam.lumda = _wtof( csValue );

    GetDlgItemText( IDC_ED_EPS_SIGMA, csValue );
	xDFParam.sigma = _wtof( csValue );
//HoleFill	
	xDFParam.kernelSize = 1;
	xDFParam.nLevel = m_oHFLevel.GetPos();
	xDFParam.bHorizontal = GetCheck( IDC_CHK_HF_HRZ_ENABLE );
//TemporalFilter
	xDFParam.alpha = m_oTFAlpha.GetPos() / 10.0f;
	xDFParam.history = GetDlgItemInt( IDC_ED_TF_HISTORY );

    m_pPreviewDlg->SetFilterParam( xDFParam );
}

void DepthFilterDlg::setVersion(const char* version) {
	m_version.assign(version);
}
