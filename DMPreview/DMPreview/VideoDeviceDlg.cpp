
#include "stdafx.h"
#include "EtronDI_Test.h"
#include "VideoDeviceDlg.h"
#include "EtronDI_TestDlg.h"
#include "AEAWB_PropertyDlg.h"
#include "AudioDlg.h"
#include "PreviewImageDlg.h"
#include "RegisterAccessDlg.h"
#include "WmMsgDef.h"
#include "mmsystem.h"
#include "utility/WindowsMfcUtility.h"
#include "IMUTestDlg.h"
#include "DistanceAccuracyDlg.h"
#include "DepthFilterDlg.h"
#include "ModeConfig.h"

void onDeviceEventFn(UINT pid, UINT vid, BOOL bAttached, void* pUserData)
{
	CVideoDeviceDlg* theDlg = (CVideoDeviceDlg*)pUserData;
	CString szMessage;

	if(bAttached)
		szMessage.Format(_T("Device PID=0x%X, VID=0x%X attached.\nYou can call EtronDI_Init2 to initialize the SDK and call EtronDI_OpenEx2 to open the device."), pid, vid, bAttached);
	else
		szMessage.Format(_T("Device PID=0x%X, VID=0x%X dettached\nYou can call EtronDI_Release to release resource allocated by the SDK."), pid, vid, bAttached);

	::OutputDebugString( szMessage );//AfxMessageBox(szMessage);
}

IMPLEMENT_DYNAMIC(CVideoDeviceDlg, CDialog)
BEGIN_MESSAGE_MAP(CVideoDeviceDlg, CDialog)
    ON_REGISTERED_MESSAGE( AFX_WM_CHANGE_ACTIVE_TAB, &CVideoDeviceDlg::OnChangeActiveTab ) 
    //ON_BN_CLICKED(IDC_FLASHDATA_READ, &CVideoDeviceDlg::OnBnClickedFlashdataRead)
    //ON_BN_CLICKED(IDC_FLASHDATA_WRITE, &CVideoDeviceDlg::OnBnClickedFlashdataWrite)
    ON_BN_CLICKED(IDC_RECTIFYLOG_GET, &CVideoDeviceDlg::OnBnClickedRectifylogGet)
    ON_MESSAGE(WM_MSG_AUTO_PREVIEW, OnAutoPreview)
    ON_WM_CLOSE()
    ON_WM_PAINT()
END_MESSAGE_MAP()

CVideoDeviceDlg::CVideoDeviceDlg(CWnd* pParent, bool enableSDKLog)
	: CDialog(CVideoDeviceDlg::IDD, pParent), m_hEtronDI(nullptr), m_pDevInfo( NULL ), m_bIMU_Device_Sync(false)
{
  /*EtronDI_Init2 can enable the auto-restart function and register USB device events */
  EtronDI_Init2(&m_hEtronDI, enableSDKLog, false); 
  if (m_hEtronDI)
	  EtronDI_RegisterDeviceEvents(m_hEtronDI, onDeviceEventFn, this);
} 

CVideoDeviceDlg::~CVideoDeviceDlg()
{
    if (m_hEtronDI != nullptr)
    {
        EtronDI_Release(&m_hEtronDI);
        m_hEtronDI = nullptr;
    }
}

void CVideoDeviceDlg::DoDataExchange(CDataExchange* pDX) {

	CDialog::DoDataExchange(pDX);
}

void CVideoDeviceDlg::DoSnapShot()
{
	for (size_t i = 0; i < m_childDlg.size(); i++)
	{
		CPreviewImageDlg* pDlg = dynamic_cast<CPreviewImageDlg*>(m_childDlg[i]);
		if (pDlg != NULL)
		{
			pDlg->OnSnapshot((WPARAM)pDlg, NULL);
		}
	}
}

void CVideoDeviceDlg::SetParams(int devIndex, const DEVINFORMATIONEX& devInfo)
{
    m_DevSelInfo.index = devIndex;
    m_pDevInfo = &devInfo;
}

BOOL CVideoDeviceDlg::OnInitDialog() {
  
  CDialog::OnInitDialog();

  CRect rtBase;
  GetClientRect( rtBase );

  CRect rt;
  GetDlgItem( IDC_STATIC_MODULE_INFO )->GetClientRect( rt );
  GetDlgItem( IDC_STATIC_MODULE_INFO )->ClientToScreen( rt );
  ScreenToClient( rt );

  rt.MoveToX( rt.right + 10 );
  rt.right = rtBase.right - 10;
  rt.bottom = rtBase.bottom - 10;

  m_oTabPage.Create( CMFCTabCtrl::STYLE_3D_ROUNDED, rt, this, IDC_TAB_PAGE, CMFCTabCtrl::LOCATION_TOP );
  m_oTabPage.EnableTabSwap( FALSE );

  InitDefaultUI();

  return TRUE;
}

void CVideoDeviceDlg::OnPaint()
{
    CPaintDC dc( this );
    CRect rt;
    CPen cp( PS_SOLID, 1, RGB( 200, 200, 200 ) );
    CString csText;

    dc.SelectStockObject( NULL_BRUSH );
    dc.SelectObject( &cp );
    dc.SelectObject( GetFont() );
    dc.SetBkColor( GetSysColor( COLOR_BTNFACE ) );

    auto DrawF = [ & ] ( const int ids )
    {
        GetDlgItemText( ids, csText );
        GetDlgItem( ids )->GetClientRect( rt );
        GetDlgItem( ids )->ClientToScreen( rt );
        ScreenToClient( rt );

        rt.top += 10;

        dc.Rectangle( rt );
        dc.TextOut( rt.left + 10, rt.top - 10, csText );
    };
    DrawF( IDC_STATIC_MODULE_INFO );
    DrawF( IDC_STATIC_FLASH_RW );
}

LRESULT CVideoDeviceDlg::OnAutoPreview(WPARAM wParam, LPARAM lParam)
{
    ( ( CPreviewImageDlg* )m_childDlg[ NULL ] )->OnBnClickedPreviewBtn();

    return NULL;
}

void CVideoDeviceDlg::InitChildDlg()
{
    DEVINFORMATIONEX xdevinfoEx;
    USB_PORT_TYPE eUSB_Port_Type = USB_PORT_TYPE_UNKNOW;

    EtronDI_GetDevicePortType( m_hEtronDI, &m_DevSelInfo, &eUSB_Port_Type );
    EtronDI_GetDeviceInfoEx( m_hEtronDI, &m_DevSelInfo , &xdevinfoEx );

	CPreviewImageDlg* previewDlg = new CPreviewImageDlg(m_hEtronDI, m_DevSelInfo, m_pDevInfo->nDevType, eUSB_Port_Type);
	previewDlg->m_pdlgVideoDeviceDlg = this;
    DepthFilterDlg* depthfilterDlg = new DepthFilterDlg(xdevinfoEx.wPID, eUSB_Port_Type, previewDlg);
    const char* version = EtronDI_GetDepthFilterVersion(m_hEtronDI, &m_DevSelInfo);
    depthfilterDlg->setVersion(version);

    AEAWB_PropertyDlg* aeAwbCtrlDlg = new AEAWB_PropertyDlg(m_hEtronDI, m_DevSelInfo, *m_pDevInfo, &m_oTabPage);

	previewDlg->SetDepthFilterDlg( depthfilterDlg );
	previewDlg->SetPropertyDlg( aeAwbCtrlDlg );

    depthfilterDlg->Create(depthfilterDlg->IDD, &m_oTabPage);
	previewDlg->Create(previewDlg->IDD, &m_oTabPage);
    aeAwbCtrlDlg->Create(aeAwbCtrlDlg->IDD, &m_oTabPage);

	m_childDlg.push_back(previewDlg);
    m_childDlg.push_back(depthfilterDlg);
    m_childDlg.push_back(aeAwbCtrlDlg);

    m_oTabPage.AddTab( previewDlg, _T( "Preview" ) );
    m_oTabPage.AddTab( depthfilterDlg, _T( "Depth Filter" ) );
    m_oTabPage.AddTab( aeAwbCtrlDlg, _T( "Property" ) );

    DistanceAccuracyDlg* DAlg = new DistanceAccuracyDlg(m_hEtronDI, m_DevSelInfo, previewDlg );
	DAlg->Create(DAlg->IDD, &m_oTabPage);
    DAlg->EnableDepthList( xdevinfoEx.wPID == ETronDI_PID_8038 );
	m_childDlg.push_back(DAlg);
    m_oTabPage.AddTab( DAlg, _T( "Accuracy" ) );

	previewDlg->SetAccuracyDlg( DAlg );

    if ( ModeConfig::IMU_NONE != g_ModeConfig.GetIMU_Type( xdevinfoEx.wPID ) )
    {
        wchar_t szBuf[ MAX_PATH ] = { NULL };
        GetDlgItemText( IDC_EDIT_SN, szBuf, MAX_PATH );
		//IMUTestDlg* pIMUTestDlg = new IMUTestDlg( szBuf, ModeConfig::IMU_9Axis == g_ModeConfig.GetIMU_Type( xdevinfoEx.wPID ), &m_oTabPage );
		IMUTestDlg* pIMUTestDlg = new IMUTestDlg(szBuf, m_hEtronDI, m_DevSelInfo, g_ModeConfig.GetIMU_Type(xdevinfoEx.wPID), &m_oTabPage);
		pIMUTestDlg->Create(pIMUTestDlg->IDD, &m_oTabPage);
		pIMUTestDlg->startGetImuData();
		pIMUTestDlg->PauseGetImuData( true );

        m_childDlg.push_back(pIMUTestDlg);
        m_oTabPage.AddTab(pIMUTestDlg, _T( "IMU" ) );
    }
    if ( xdevinfoEx.wPID == ETronDI_PID_8060 )
    {
	    AudioDlg* audioDlg = new AudioDlg(&m_oTabPage);
	    audioDlg->Create(audioDlg->IDD, &m_oTabPage);
	    m_childDlg.push_back(audioDlg);
        m_oTabPage.AddTab( audioDlg, _T( "Audio" ) );
    }

	CRegisterAccessDlg* regAccessDlg = new CRegisterAccessDlg(m_hEtronDI, m_DevSelInfo, &m_oTabPage);
	regAccessDlg->Create(regAccessDlg->IDD, &m_oTabPage);
	m_childDlg.push_back(regAccessDlg);
    m_oTabPage.AddTab( regAccessDlg, _T( "Register" ) );

}

void CVideoDeviceDlg::InitDefaultUI() 
{
    InitModuleInformation();
    GetFWVersion();

    ( ( CComboBox* )GetDlgItem( IDC_FLASHTYPESELECTCOMBO ) )->SetCurSel( NULL );

    CComboBox* pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_USERDATA_INDEX);

    for (int i = 0; i < USERDATA_SECTION_NUM; ++i)
    {
        pComboBox->AddString(CString(std::to_string(i).c_str()));
    }
    pComboBox->SetCurSel(0);

    // Get product ID and Vendor ID
    unsigned short nProductID[3] = {0};
    unsigned short nVendorID = 0;
    CString csText;
  
    if( EtronDI_GetPidVid( m_hEtronDI, &m_DevSelInfo, nProductID, &nVendorID ) == ETronDI_OK ) 
    {
        for ( int i = 0; i < 3; i++ )
        {
            if ( nProductID[ i ] )
            {
                csText.AppendFormat( i ? L", 0x%04X" : L"0x%04X", nProductID[ i ] );
            }
        }
        SetDlgItemText( IDC_EDIT_PID, csText );
        csText.Format( L"0x%04X", nVendorID  ); SetDlgItemText( IDC_EDIT_VID, csText );
    }
    else AfxMessageBox(_T("EtronDI_GetPidVid failed !!"));

    int nActualSNLenByByte = 0;
  
    wchar_t szBuf[ MAX_PATH ] = { NULL };
    if( EtronDI_GetSerialNumber( m_hEtronDI, &m_DevSelInfo, ( BYTE* )szBuf, MAX_PATH, &nActualSNLenByByte) == ETronDI_OK )
    {
        //char* pStrSerialNumber = ( char* )malloc( nActualSNLenByByte / 2 + 1 );  
    
        //for( int i = 0 ; i < nActualSNLenByByte ; i += 2 )
        //{
        //    pStrSerialNumber[ i / 2 ] = szBuf[ i ];
        //}  
        //pStrSerialNumber[ nActualSNLenByByte / 2 ] = '\0';
    
        SetDlgItemText( IDC_EDIT_SN, szBuf/*CString( pStrSerialNumber )*/ ); 
    
        //if( pStrSerialNumber )
        //{
        //    delete pStrSerialNumber;
        //    pStrSerialNumber = NULL;
        //}
    }
    else AfxMessageBox( _T( "EtronDI_GetSerialNumber Failed !!" ) );


    GetDlgItem( IDC_SLAVECHK )->EnableWindow( nProductID[ 0 ] == ETronDI_PID_8060  ||
                                              nProductID[ 0 ] == ETronDI_PID_8054  ||
                                              nProductID[ 0 ] == ETronDI_PID_8040S );
    InitChildDlg();
}

void CVideoDeviceDlg::GetFWVersion()
{
    std::vector<char> szBuf(256, 0);

    int nActualLength = 0;  
  
    EtronDI_GetFwVersion(m_hEtronDI, &m_DevSelInfo, &szBuf[ 0 ], 256, &nActualLength);

    char* pHasSecondFW = strchr( &szBuf[ 0 ], '\n' );

    if ( !pHasSecondFW ) { strcat( &szBuf[ 0 ], "\n\n" ); } // 1-chip
    else if ( !strchr( pHasSecondFW + 1, '\n' ) ) { strcat( &szBuf[ 0 ], "\n" ); } // 2-chip

    SetDlgItemText( IDC_STATIC_FW_VER, CA2W( &szBuf[0] ) );
}

LRESULT CVideoDeviceDlg::OnChangeActiveTab( WPARAM wparam,LPARAM lparam )
{
    IMUTestDlg* pIMUTestDlg  = dynamic_cast< IMUTestDlg* >( m_childDlg[ wparam ] );

    if ( pIMUTestDlg )
    {
        pIMUTestDlg->PauseGetImuData( false );
    }
    else
    {
        /*for ( auto& i : m_childDlg )
        {
            pIMUTestDlg = dynamic_cast< IMUTestDlg* >( i );

            if ( pIMUTestDlg )
            {
                pIMUTestDlg->PauseGetImuData( true );
                break;
            }
        }*/
    }
    ( ( CPreviewImageDlg* )m_childDlg[ NULL ] )->EnableAccuracy( NULL != dynamic_cast< DistanceAccuracyDlg* >( m_childDlg[ wparam ] ) );

    return NULL;
}

//void CVideoDeviceDlg::OnBnClickedFlashdataRead()
//{
//#ifdef ESPDI_EG
//    AfxMessageBox(_T("EtronDI_GetRectifyTable NOT support !!"));
//    return;
//#else
//    FILE*   fp            = NULL;
//    BYTE    buffer[4096]  = { NULL };
//    int     nActualLength = 0;
//    int     nRet          = 0;
//    int     index         = 0;
//    CString csMessage;
//    CString csFileName;
//
//    const BOOL bSlaveCheck = ( ( ( CButton* )GetDlgItem( IDC_SLAVECHK ) )->GetCheck() == BST_CHECKED );
//
//    const FLASH_SELECT_INFO iSelect = ( FLASH_SELECT_INFO )( ( CComboBox* )GetDlgItem( IDC_FLASHTYPESELECTCOMBO ) )->GetCurSel();
//
//    auto WriteFile = [ & ]()
//    {
//        if ( csFileName.GetLength() )
//        {
//            if( 0 == _wfopen_s( &fp, csFileName, L"wb" ) )
//            {
//                fwrite( buffer, nActualLength, 1, fp );
//                fclose( fp );
//            }
//            else csMessage = _T( "fopen_s Failed !!" );
//        }
//        csFileName.Empty();
//    };
//    if ( ( ( CComboBox* )GetDlgItem( IDC_COMBO_USERDATA_INDEX ) )->IsWindowVisible() )
//    {
//	    index = ( USERDATA_SECTION_INDEX )( ( CComboBox* )GetDlgItem( IDC_COMBO_USERDATA_INDEX ) )->GetCurSel();
//    }
//    if( iSelect == SENSOR_OFFSET )
//    {
//	    if ( ETronDI_OK == EtronDI_GetSlaveYOffset( m_hEtronDI, &m_DevSelInfo, buffer, ETronDI_Y_OFFSET_FILE_SIZE, &nActualLength, index ) )
//        {
//            csFileName.Format( L"FDATA_Offset_Slave_%d.bin", index );
//
//            WriteFile();
//
//            csMessage = _T( "EtronDI_GetYOffset Success !!" );
//        }
//	    /*Only EX8054 & 8040S Master need to Set write to AR0330 sensor*/   
//	    if ( ETronDI_OK == EtronDI_SetPlumAR0330( m_hEtronDI, &m_DevSelInfo, true ) )
//	    {
//		    if ( ETronDI_OK == EtronDI_GetYOffset( m_hEtronDI, &m_DevSelInfo, buffer, ETronDI_Y_OFFSET_FILE_SIZE, &nActualLength, index ) )
//            {
//                csFileName.Format( L"FDATA_Offset_ARO330_%d.bin", index );
//
//                WriteFile();
//
//                csMessage = _T( "EtronDI_GetYOffset Success !!" );
//            }
//		    EtronDI_SetPlumAR0330( m_hEtronDI, &m_DevSelInfo, false );
//	    }
//	    nRet = EtronDI_GetYOffset( m_hEtronDI, &m_DevSelInfo, buffer, ETronDI_Y_OFFSET_FILE_SIZE, &nActualLength, index );
//    
//        if ( csMessage.IsEmpty() ) csMessage = _T( "EtronDI_GetYOffset Failed !!" );
//
//        if ( ETronDI_OK == nRet )
//        {
//            csFileName.Format( L"FDATA_Offset_%d.bin", index );
//
//            csMessage = _T( "EtronDI_GetYOffset Success !!" );
//        }
//    }
//  
//    else if( iSelect == RECTIFY_TABLE )
//    {
//        if ( ETronDI_OK == EtronDI_GetSlaveRectifyTable( m_hEtronDI, &m_DevSelInfo, buffer, ETronDI_RECTIFY_FILE_SIZE, &nActualLength, index ) )
//        {
//            csFileName.Format( L"FDATA_Rectify_Slave_%d.bin", index );
//
//            WriteFile();
//
//            csMessage = _T( "EtronDI_GetRectifyTable Success !!" );
//        }
//	    /*Only EX8054 & 8040S Master need to Set write to AR0330 sensor*/   
//	    if ( ETronDI_OK == EtronDI_SetPlumAR0330( m_hEtronDI, &m_DevSelInfo, true ) )
//	    {
//		    if ( ETronDI_OK == EtronDI_GetRectifyTable( m_hEtronDI, &m_DevSelInfo, buffer, ETronDI_RECTIFY_FILE_SIZE, &nActualLength, index ) )
//            {
//                csFileName.Format( L"FDATA_Rectify_ARO330_%d.bin", index );
//
//                WriteFile();
//
//                csMessage = _T( "EtronDI_GetRectifyTable Success !!" );
//            }
//		    EtronDI_SetPlumAR0330( m_hEtronDI, &m_DevSelInfo, false );
//	    }
//	    nRet = EtronDI_GetRectifyTable( m_hEtronDI, &m_DevSelInfo, buffer, ETronDI_RECTIFY_FILE_SIZE, &nActualLength, index );
//
//        if ( csMessage.IsEmpty() ) csMessage = _T( "EtronDI_GetRectifyTable Failed !!" );
//
//        if ( ETronDI_OK == nRet )
//        {
//            csFileName.Format( L"FDATA_Rectify_%d.bin", index );
//
//            csMessage = _T( "EtronDI_GetRectifyTable Success !!" );
//        }
//    }
//    else if( iSelect == ZD_TABLE )
//    {
//        ZDTABLEINFO zdTableInfo { index, ETronDI_DEPTH_DATA_11_BITS };
//
//        csMessage = _T( "EtronDI_GetZDTable Failed !!" );
//
//        if( ETronDI_OK == EtronDI_GetZDTable( m_hEtronDI, &m_DevSelInfo, buffer, sizeof( buffer ), &nActualLength, &zdTableInfo ) )
//        {   
//            csFileName.Format( L"FDATA_ZDTable_%d.bin", index );
//
//            csMessage = _T( "EtronDI_GetZDTable Success !!" );
//        }
//    }
//    else if( iSelect == LOG_DATA )
//    {
//        if ( ETronDI_OK == EtronDI_GetSlaveLogData( m_hEtronDI, &m_DevSelInfo, buffer, ETronDI_CALIB_LOG_FILE_SIZE, &nActualLength, index ) )
//        {
//            csFileName.Format( L"FDATA_LogData_Slave_%d.bin", index );
//
//            WriteFile();
//
//            csMessage = _T( "EtronDI_GetLogData Success !!" );
//        }
//	    /*Only EX8054 & 8040S Master need to Set write to AR0330 sensor*/   
//	    if ( ETronDI_OK == EtronDI_SetPlumAR0330( m_hEtronDI, &m_DevSelInfo, true ) )
//	    {
//		    if ( ETronDI_OK == EtronDI_GetLogData( m_hEtronDI, &m_DevSelInfo, buffer, ETronDI_CALIB_LOG_FILE_SIZE, &nActualLength, index ) )
//            {
//                csFileName.Format( L"FDATA_LogData_ARO330_%d.bin", index );
//
//                WriteFile();
//
//                csMessage = _T( "EtronDI_GetLogData Success !!" );
//            }
//		    EtronDI_SetPlumAR0330( m_hEtronDI, &m_DevSelInfo, false );
//	    }
//        nRet = EtronDI_GetLogData( m_hEtronDI, &m_DevSelInfo, buffer, ETronDI_CALIB_LOG_FILE_SIZE, &nActualLength, index );
//
//        if ( csMessage.IsEmpty() ) csMessage = _T( "EtronDI_GetLogData Failed !!" );
//
//        if ( ETronDI_OK == nRet )
//        {
//            csFileName.Format( L"FDATA_LogData_%d.bin", index );
//
//            csMessage = _T( "EtronDI_GetLogData Success !!" );
//        }
//    }
//    WriteFile();
//
//    AfxMessageBox( csMessage );
//#endif
//}
//
//void CVideoDeviceDlg::OnBnClickedFlashdataWrite()
//{
//#ifdef ESPDI_EG
//    AfxMessageBox(_T("EtronDI_GetRectifyTable NOT support !!"));
//    return;
//#else
//    BYTE buffer[4096] = {0};
//    int nActualLength = 0;
//    int nBufferLength;
//    int index = 0;
//    CString csFileName;
//    CString csMessage;
//
//    const BOOL bSlaveCheck = ( ( ( CButton* )GetDlgItem( IDC_SLAVECHK ) )->GetCheck() == BST_CHECKED );
//
//    const FLASH_SELECT_INFO iSelect = ( FLASH_SELECT_INFO )( ( CComboBox* )GetDlgItem( IDC_FLASHTYPESELECTCOMBO ) )->GetCurSel();
//
//    if ( ( ( CComboBox* )GetDlgItem( IDC_COMBO_USERDATA_INDEX ) )->IsWindowVisible() )
//    {
//	    index = ( USERDATA_SECTION_INDEX )( ( CComboBox* )GetDlgItem( IDC_COMBO_USERDATA_INDEX ) )->GetCurSel();
//    }
//    auto ReadFile = [ & ]( const int iReadLen )->BOOL
//    {
//        FILE* fp;
//
//        if( 0 == _wfopen_s( &fp, csFileName, L"rb" ) )
//        {
//            fread( buffer, iReadLen, 1, fp );
//            fclose( fp );
//
//            return TRUE;
//        }
//        return FALSE;
//    };
//	if( iSelect == SENSOR_OFFSET )
//    {
//        csFileName.Format( L"FDATA_Offset_Slave_%d.bin", index );
//
//        if( ReadFile( ETronDI_Y_OFFSET_FILE_SIZE ) )
//        {
//            EtronDI_SetSlaveYOffset( m_hEtronDI, &m_DevSelInfo, buffer, ETronDI_Y_OFFSET_FILE_SIZE, &nActualLength, index );
//
//            csMessage = _T( "EtronDI_SetYOffset Success !!" );
//        }
//	    /*Only EX8054 & 8040S Master need to Set write to AR0330 sensor*/
//        if ( ETronDI_OK == EtronDI_SetPlumAR0330( m_hEtronDI, &m_DevSelInfo, true ) )
//        {
//            csFileName.Format( L"FDATA_Offset_ARO330_%d.bin", index );
//
//	        if( ReadFile( ETronDI_Y_OFFSET_FILE_SIZE ) )
//	        {
//		        EtronDI_SetYOffset( m_hEtronDI, &m_DevSelInfo, buffer, ETronDI_Y_OFFSET_FILE_SIZE, &nActualLength, index );
//
//		        EtronDI_SetPlumAR0330( m_hEtronDI, &m_DevSelInfo, false );
//
//                csMessage = _T( "EtronDI_SetYOffset Success !!" );
//	        }
//        }
//        csFileName.Format( L"FDATA_Offset_%d.bin", index );
//    
//        if ( csMessage.IsEmpty() ) csMessage = _T( "EtronDI_SetYOffset Failed !!" );
//
//        if( ReadFile( ETronDI_Y_OFFSET_FILE_SIZE ) )
//        {
//            if ( ETronDI_OK == EtronDI_SetYOffset( m_hEtronDI, &m_DevSelInfo, buffer, ETronDI_Y_OFFSET_FILE_SIZE, &nActualLength, index ) )
//                csMessage = _T( "EtronDI_SetYOffset Success !!" );
//        }
//    }
//    else if( iSelect == RECTIFY_TABLE )
//    {
//        csFileName.Format( L"FDATA_Rectify_Slave_%d.bin", index );
//
//        if( ReadFile( ETronDI_RECTIFY_FILE_SIZE ) )
//        {
//            EtronDI_SetSlaveRectifyTable( m_hEtronDI, &m_DevSelInfo, buffer, ETronDI_RECTIFY_FILE_SIZE, &nActualLength, index );
//
//            csMessage = _T( "EtronDI_SetRectifyTable Success !!" );
//        }
//	    /*Only EX8054 & 8040S Master need to Set write to AR0330 sensor*/
//        if ( ETronDI_OK == EtronDI_SetPlumAR0330( m_hEtronDI, &m_DevSelInfo, true ) )
//        {
//            csFileName.Format( L"FDATA_Rectify_ARO330_%d.bin", index );
//
//	        if( ReadFile( ETronDI_RECTIFY_FILE_SIZE ) )
//	        {
//		        EtronDI_SetRectifyTable( m_hEtronDI, &m_DevSelInfo, buffer, ETronDI_RECTIFY_FILE_SIZE, &nActualLength, index );
//
//		        EtronDI_SetPlumAR0330( m_hEtronDI, &m_DevSelInfo, false );
//
//                csMessage = _T( "EtronDI_SetRectifyTable Success !!" );
//	        }
//        }
//        csFileName.Format( L"FDATA_Rectify_%d.bin", index );
//
//        if ( csMessage.IsEmpty() ) csMessage = _T( "EtronDI_SetRectifyTable Failed !!" );
//
//        if( ReadFile( ETronDI_RECTIFY_FILE_SIZE ) )
//        {
//	        if ( ETronDI_OK == EtronDI_SetRectifyTable( m_hEtronDI, &m_DevSelInfo, buffer, ETronDI_RECTIFY_FILE_SIZE, &nActualLength, index ) )
//                csMessage = _T( "EtronDI_SetRectifyTable Success !!" );
//        }
//    }
//    else if( iSelect == ZD_TABLE )
//    {
//	    ZDTABLEINFO zdTableInfo { index, ETronDI_DEPTH_DATA_11_BITS };
//
//        csFileName.Format( L"FDATA_ZDTable_%d.bin", index );
//
//        if( ReadFile( sizeof( buffer ) ) )
//        {
//            csMessage = ( ETronDI_OK == EtronDI_SetZDTable( m_hEtronDI, &m_DevSelInfo, buffer, sizeof( buffer ), &nActualLength, &zdTableInfo ) ) ?
//                        _T("EtronDI_SetZDTable Success !!" ) :
//                        _T("EtronDI_SetZDTable Failed !!" );
//        }
//    } 
//    else if( iSelect == LOG_DATA )
//    {
//        csFileName.Format( L"FDATA_LogData_Slave_%d.bin", index );
//
//        if( ReadFile( ETronDI_RECTIFY_FILE_SIZE ) )
//        {
//            EtronDI_SetSlaveLogData( m_hEtronDI, &m_DevSelInfo, buffer, ETronDI_CALIB_LOG_FILE_SIZE, &nActualLength, index );
//
//            csMessage = _T( "EtronDI_SetLogData Success !!" );
//        }
//	    /*Only EX8054 & 8040S Master need to Set write to AR0330 sensor*/
//        if ( ETronDI_OK == EtronDI_SetPlumAR0330( m_hEtronDI, &m_DevSelInfo, true ) )
//        {
//            csFileName.Format( L"FDATA_LogData_ARO330_%d.bin", index );
//
//	        if( ReadFile( ETronDI_RECTIFY_FILE_SIZE ) )
//	        {
//		        EtronDI_SetLogData( m_hEtronDI, &m_DevSelInfo, buffer, ETronDI_CALIB_LOG_FILE_SIZE, &nActualLength, index );
//
//		        EtronDI_SetPlumAR0330( m_hEtronDI, &m_DevSelInfo, false );
//
//                csMessage = _T( "EtronDI_SetLogData Success !!" );
//	        }
//        }
//        csFileName.Format( L"FDATA_LogData_%d.bin", index );
//
//        if ( csMessage.IsEmpty() ) csMessage = _T( "EtronDI_SetLogData Failed !!" );
//
//        if( ReadFile( ETronDI_CALIB_LOG_FILE_SIZE ) )
//        {
//            if ( ETronDI_OK == EtronDI_SetLogData( m_hEtronDI, &m_DevSelInfo, buffer, ETronDI_CALIB_LOG_FILE_SIZE, &nActualLength, index ) )
//                csMessage = _T( "EtronDI_SetLogData Success !!" );
//        }
//    }
//    AfxMessageBox( csMessage );
//#endif
//}

void CVideoDeviceDlg::OnBnClickedRectifylogGet()
{
#ifdef ESPDI_EG
    AfxMessageBox(_T("EtronDI_GetRectifyLogData NOT support !!"));
#else
  int index = ((CComboBox*)GetDlgItem(IDC_COMBO_DEPTH_INDEX))->GetCurSel();
  eSPCtrl_RectLogData data;
  int nRet;

  /*
  EX8054, PLUM Flash File ID (K) = 240, 241. Calibration Log Infor.
  EX8054, PUMA Flash File ID (D) = 242. Calibration Log Infor
  */
  if (((CButton*)GetDlgItem(IDC_SLAVECHK))->GetCheck() == BST_CHECKED)
  {
	  nRet = EtronDI_GetRectifyMatLogDataSlave(m_hEtronDI, &m_DevSelInfo, &data, index);
  }
  else
  {
	  nRet = EtronDI_GetRectifyMatLogData(m_hEtronDI, &m_DevSelInfo, &data, index);
  }

  if( nRet == ETronDI_OK) {
    
    // store to txt
    FILE *fl;
    errno_t err;
    char filename[128] = { 0 };

    sprintf_s(filename, 128, "RectifyLog%d.txt", index); 
    err = fopen_s(&fl, filename, "wt");  
    if (err != 0) AfxMessageBox(_T(" fopen_s failed !!"));
    else {
    
      int i = 0;
    
      fprintf(fl, "InImgWidth = %d\n", data.InImgWidth);
      fprintf(fl, "InImgHeight = %d\n", data.InImgHeight);
      fprintf(fl, "OutImgWidth = %d\n", data.OutImgWidth);
      fprintf(fl, "OutImgHeight = %d\n", data.OutImgHeight);
      //
      fprintf(fl, "RECT_ScaleWidth = %d\n", data.RECT_ScaleWidth);
      fprintf(fl, "RECT_ScaleHeight = %d\n", data.RECT_ScaleHeight);
      //
      fprintf(fl, "CamMat1 = ");
      for (i=0; i<9; i++) {
      fprintf(fl, "%.8f, ",  data.CamMat1[i]);
      }
      fprintf(fl, "\n");
      //
      fprintf(fl, "CamDist1 = ");
      for (i=0; i<8; i++) {
      fprintf(fl, "%.8f, ",  data.CamDist1[i]);
      }
      fprintf(fl, "\n");
      //
      fprintf(fl, "CamMat2 = ");
      for (i=0; i<9; i++) {
      fprintf(fl, "%.8f, ",  data.CamMat2[i]);
      }
      fprintf(fl, "\n");
      //
      fprintf(fl, "CamDist2 = ");
      for (i=0; i<8; i++) {
      fprintf(fl, "%.8f, ",  data.CamDist2[i]);
      }
      fprintf(fl, "\n");
      //
      fprintf(fl, "RotaMat = ");
      for (i=0; i<9; i++) {
      fprintf(fl, "%.8f, ",  data.RotaMat[i]);
      }
      fprintf(fl, "\n");
      //
      fprintf(fl, "TranMat = ");
      for (i=0; i<3; i++) {
      fprintf(fl, "%.8f, ",  data.TranMat[i]);
      }
      fprintf(fl, "\n");
      //
      fprintf(fl, "LRotaMat = ");
      for (i=0; i<9; i++) {
      fprintf(fl, "%.8f, ",  data.LRotaMat[i]);
      }
      fprintf(fl, "\n");
      //
      fprintf(fl, "RRotaMat = ");
      for (i=0; i<9; i++) {
      fprintf(fl, "%.8f, ",  data.RRotaMat[i]);
      }
      fprintf(fl, "\n");
      //
      fprintf(fl, "NewCamMat1 = ");
      for (i=0; i<12; i++) {
      fprintf(fl, "%.8f, ",  data.NewCamMat1[i]);
      }
      fprintf(fl, "\n");
      //
      fprintf(fl, "NewCamMat2 = ");
      for (i=0; i<12; i++) {
      fprintf(fl, "%.8f, ",  data.NewCamMat2[i]);
      }
      fprintf(fl, "\n");
      //
      fprintf(fl, "RECT_Crop_Row_BG = %d\n", data.RECT_Crop_Row_BG);
      fprintf(fl, "RECT_Crop_Row_ED = %d\n", data.RECT_Crop_Row_ED);
      fprintf(fl, "RECT_Crop_Col_BG_L = %d\n", data.RECT_Crop_Col_BG_L);
      fprintf(fl, "RECT_Crop_Col_ED_L = %d\n", data.RECT_Crop_Col_ED_L);
      fprintf(fl, "RECT_Scale_Col_M = %d\n", data.RECT_Scale_Col_M);
      fprintf(fl, "RECT_Scale_Col_N = %d\n", data.RECT_Scale_Col_N);
      fprintf(fl, "RECT_Scale_Row_M = %d\n", data.RECT_Scale_Row_M);
      fprintf(fl, "RECT_Scale_Row_N = %d\n", data.RECT_Scale_Row_N);
      //
      fprintf(fl, "RECT_AvgErr = %.8f\n", data.RECT_AvgErr);
      //
      fprintf(fl, "nLineBuffers = %d\n", data.nLineBuffers);

	  if (m_pDevInfo->nDevType == PUMA) {  
          fprintf(fl, "ReProjectMat = ");
          for (i=0; i<16; i++) {
            fprintf(fl, "%.8f, ", data.ReProjectMat[i]);
          }
          fprintf(fl, "\n");
	  }
	  fclose(fl);

	  if (m_pDevInfo->nDevType == PUMA) {
	    std::ostringstream stringStream;
	    stringStream << "index:"<<index << std::endl;
		stringStream << "ReProjectMat = " << std::endl;
	    for (i = 0; i<16; i++) {
	  	  stringStream << data.ReProjectMat[i] <<" ,";
	    }	
	    std::string strReprojectMat = stringStream.str();
	    AfxMessageBox(CString(strReprojectMat.c_str()));
	  }
	  else {
	  	AfxMessageBox(_T(" Complete !!"));
	  }

    }
  }
  else AfxMessageBox(_T("EtronDI_GetRectifyLogData Failed !!"));
#endif
}
void CVideoDeviceDlg::OnClose()
{
    m_oTabPage.DestroyWindow();

    for (size_t i = 0; i < m_childDlg.size(); ++i)
    {
        m_childDlg[i]->DestroyWindow();
        delete m_childDlg[i];
    }
    m_childDlg.clear();

    GetParent()->PostMessage(WM_MSG_CLOSE_VIDEO_DEVICE_DLG, (WPARAM)this);
    
    CDialog::OnClose();
}

void CVideoDeviceDlg::ChangeIMU_UI_FrameCount_SerialCount(bool enableModuleSync)
{
	for (size_t i = 0; i < m_childDlg.size(); ++i)
	{
		IMUTestDlg* pIMUTestDlg = dynamic_cast<IMUTestDlg*>(m_childDlg[i]);

		if (pIMUTestDlg)
		{
			pIMUTestDlg->ChangeIMU_UI_FrameCount_SerialCount(enableModuleSync);
			break;
		}
	}
}

int CVideoDeviceDlg::IMU_Device_Mapping()
{
	return m_DevSelInfo.index;
}

void CVideoDeviceDlg::Update_IMU_Device_Mapping()
{
	if (m_bIMU_Device_Sync == true)
		return;

	int nIMU_ID_INDEX = m_DevSelInfo.index + IMU_ID_INDEX_START;
	for (size_t i = 0; i < m_childDlg.size(); ++i)//
	{
		IMUTestDlg* pIMUTestDlg = dynamic_cast<IMUTestDlg*>(m_childDlg[i]);

		if (pIMUTestDlg)
		{
			pIMUTestDlg->Update_IMU_Device_Mapping(nIMU_ID_INDEX);
			m_bIMU_Device_Sync = true;
			break;
		}
	}
}

void CVideoDeviceDlg::IMU_Device_Reopen(void*& hEtronDI, DEVSELINFO& devSelInfo)
{
	if (m_bIMU_Device_Sync == false)
		return;

	m_bIMU_Device_Sync = false;
	int nIMU_ID_INDEX = m_DevSelInfo.index + IMU_ID_INDEX_START;
	for (size_t i = 0; i < m_childDlg.size(); ++i)//
	{
		IMUTestDlg* pIMUTestDlg = dynamic_cast<IMUTestDlg*>(m_childDlg[i]);

		if (pIMUTestDlg)
		{
			pIMUTestDlg->IMU_Device_Reopen(hEtronDI, devSelInfo);
			break;
		}
	}
}

void CVideoDeviceDlg::InitModuleInformation()
{
	CString edit_na("N/A");
	CString edit_nas("N / A\nN / A\nN / A");
	CString cs_vid("Vender ID:");
	CString cs_pid("Product ID:");
	CString cs_sn("Serial Number:");
	CString cs_fw_ver("FW Ver:");
	SetDlgItemText(IDC_EDIT_STATIC_VID, cs_vid);
	SetDlgItemText(IDC_EDIT_VID, edit_na);
	SetDlgItemText(IDC_EDIT_STATIC_PID, cs_pid);
	SetDlgItemText(IDC_EDIT_PID, edit_na);
	SetDlgItemText(IDC_EDIT_STATIC_SN, cs_sn);
	SetDlgItemText(IDC_EDIT_SN, edit_na);
	SetDlgItemText(IDC_EDIT_STATIC_FW_VER_TITLE, cs_fw_ver);
	SetDlgItemText(IDC_STATIC_FW_VER, edit_nas);
}

