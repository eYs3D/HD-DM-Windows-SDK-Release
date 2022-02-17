// PreviewImageDlg.cpp : implementation file
// PreviewImageDlg.cpp : implementation file
//

#include "stdafx.h"
#include "APC_Test.h"
#include "PreviewImageDlg.h"
#include "ColorDlg.h"
#include "DepthDlg.h"
#include "MeasureDistanceDlg.h"
#include "WmMsgDef.h"
#include "utility/WindowsMfcUtility.h"
#include "PlyWriter.h"
#include "ModeConfig.h"
#include "WaitDlg.h"
#include "AEAWB_PropertyDlg.h"
#include "DistanceAccuracyDlg.h"
#include "DepthFilterDlg.h"
#include "AutoModuleSyncManager.h"
#include "FrameSyncManager.h"

#ifndef ESPDI_EG
#include "utility/DepthFusionHelper.h"
#endif

IMPLEMENT_DYNAMIC(CPreviewImageDlg, CDialog)

#define CHECK_RESTART_TIMER 1
#define CHECK_REGISTER_TIMER 2
#define CHECK_SYNC_TIMER 3
#define CHECK_805359_MASTER_RESET_TIMER 4
#define CHECK_REOPEN_TIMER 5
#define CHECK_REGISTER_TIME_INTERVAL 100
#define CHECK_IMAGE_CALLBACK_TIME_INTERVAL 1500
#define CHECK_IMAGE_CALLBACK_FIRSTTIME_INTERVAL 10000
#define CHECK_805359_MASTER_RESET_TIME_INTERVAL 1000
//#define CHECK_SERIALNUMBER_CALLBACK_TIME_INTERVAL 10000//8000

#define EX8038_MIN_DEPTH_RANGE 94
#define MAX_DEPTH_DISTANCE     16383 // (1 << 14)

#define MAX_IR_DEFAULT 6
#define MAX_IR_MAXIMUM 15
#define MAX_IR_HYPATIA 96
#define MAX_IR_HYPATIA_DEFAULT 60

typedef struct plyThreadData
{
	CPreviewImageDlg* pDlg;
	int irValue;
	float zFar;
    float zNear;
    BOOL bUseFilter;
} THREAD_PARAM, *LPTHREAD_PARAM;

CPreviewImageDlg::CPreviewImageDlg(void*& hApcDI, DEVSELINFO& devSelInfo, const USHORT& devType, const USB_PORT_TYPE eUSB_Port_Type)
	: CDialog(CPreviewImageDlg::IDD, NULL), 
    m_hApcDI(hApcDI), m_DevSelInfo(devSelInfo), m_DevType(devType), m_eUSB_Port_Type( eUSB_Port_Type ),
    m_colorStreamOptionCount(0), m_depthStreamOptionCount(0), m_kcolorStreamOptionCount( 0 ), m_tcolorStreamOptionCount( 0 ), m_iInterLeaveModeFPS( EOF ),
    m_pWaitDlg( new WaitDlg() ),
    m_isPreviewed( FALSE ),
    m_bPCV_NoColorStream( FALSE ),
    m_bPCV_SingleColor( FALSE ),
    m_bIsInterLeaveMode( FALSE ),
    m_pPropertyDlg( NULL ),
    m_pAccuracyDlg( NULL ),
    m_pDepthFilterDlg( NULL ),
    m_i8038DepthIndex( NULL ),
    m_DfParam( new DepthfilterParam() )
{
    APC_GetIRMaxValue(m_hApcDI, &m_DevSelInfo, &m_maxIR);
	m_pdlgVideoDeviceDlg = NULL;
#ifndef ESPDI_EG
    m_depthFusionHelper = nullptr;

	m_frameGrabber = nullptr;
    memset(m_pStreamColorInfo, 0, sizeof(APC_STREAM_INFO) * APC_MAX_STREAM_COUNT);
    memset(m_pStreamDepthInfo, 0, sizeof(APC_STREAM_INFO) * APC_MAX_STREAM_COUNT);
	m_previewParams.m_pointCloudViewer = nullptr;
#endif
    APC_GetDeviceInfoEx( m_hApcDI, &m_DevSelInfo , &m_devinfoEx );
}

CPreviewImageDlg::~CPreviewImageDlg()
{
    if ( m_previewParams.m_rectifyLogDataSlave ) free( m_previewParams.m_rectifyLogDataSlave );
}

//bool CPreviewImageDlg::usePlyFilter(APCImageType::Value depthImageType)
//{
//	return (/*depthImageType == APCImageType::DEPTH_11BITS) && (*/ ( ( CButton* )GetDlgItem( IDC_CHK_PLY_FILTER ) )->GetCheck() == BST_CHECKED );
//}

void CPreviewImageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

int CPreviewImageDlg::CurColorOption()
{
    CAutoLock lock(m_previewParams.m_mutex);
    return m_previewParams.m_colorOption;
}

bool CPreviewImageDlg::PointCloudViewer()
{
	CAutoLock lock(m_previewParams.m_mutex);
	return (((CButton*)GetDlgItem(IDC_CHECK_POINTCLOUD_VIEWER))->GetCheck() == BST_CHECKED) ? TRUE : FALSE;
}

int CPreviewImageDlg::getRectifyLogData(eSPCtrl_RectLogData* rectifyData,int index)
{	
	int nRet;

#ifndef ESPDI_EG
	//Set index by resolution	
	nRet = APC_GetRectifyMatLogData(m_hApcDI, &m_DevSelInfo, rectifyData, index);

	if (nRet == APC_OK)
	{		
		/*Modify Reprojection matrix according to rotation and resizing*/
        if ( IsDevicePid( APC_PID_8040S ) || IsDevicePid( APC_PID_8054 ) )
		{
            if ( rectifyData->OutImgWidth == 2176 && rectifyData->OutImgHeight == 1920 ) // APC_PID_8040S L+D special-case
			{
				//rectifyData->OutImgHeight and rectLogData->OutImgHeight are error, and current EX8040S only support mode 4
				rectifyData->OutImgWidth  = 1080;
				rectifyData->OutImgHeight = 1920;
			}
			float centerX = -1.0f*rectifyData->ReProjectMat[3];
			float centerY = -1.0f*rectifyData->ReProjectMat[7];
			int width = rectifyData->OutImgWidth;
			int height = rectifyData->OutImgHeight;
			rotate2Dpoint(width / 2, height / 2, height / 2, width / 2, 90.0f, centerX, centerY);
			rectifyData->ReProjectMat[3] = -1.0f*centerX;
			rectifyData->ReProjectMat[7] = -1.0f*centerY;
			rectifyData->OutImgWidth = height;
			rectifyData->OutImgHeight = width;
		}
	}
#else
	nRet = APC_DEVICE_NOT_SUPPORT;
#endif

	return nRet;
}

int CPreviewImageDlg::getRectifyLogDataSlave(eSPCtrl_RectLogData* rectifyData, int index)
{
	int nRet;

#ifndef ESPDI_EG
	//Set index by resolution	
	nRet = APC_GetRectifyMatLogDataSlave(m_hApcDI, &m_DevSelInfo, rectifyData, index);

	if ( IsDevicePid( APC_PID_8054 ) && nRet == APC_OK)
	{
		/*Modify Reprojection matrix according to rotation and resizing*/
		{
			float centerX = -1.0f*rectifyData->ReProjectMat[3];
			float centerY = -1.0f*rectifyData->ReProjectMat[7];
			int width = rectifyData->OutImgWidth;
			int height = rectifyData->OutImgHeight;
			rotate2Dpoint(width / 2, height / 2, height / 2, width / 2, 90.0f, centerX, centerY);
			rectifyData->ReProjectMat[3] = -1.0f*centerX;
			rectifyData->ReProjectMat[7] = -1.0f*centerY;
			rectifyData->OutImgWidth = height;
			rectifyData->OutImgHeight = width;
		}
	}
#else
	nRet = APC_DEVICE_NOT_SUPPORT;
#endif

	return nRet;
}

CPoint CPreviewImageDlg::CurTrackImgRes()
{
	CAutoLock lock(m_previewParams.m_mutex);
	if (m_previewParams.m_tcolorOption >= 0)
	{
		const APC_STREAM_INFO& streamTColorInfo = m_pStreamTColorInfo[m_previewParams.m_tcolorOption];
		return CPoint(streamTColorInfo.nWidth, streamTColorInfo.nHeight);
	}

	return CPoint(0, 0);
}

CPoint CPreviewImageDlg::CurKColorImgRes()
{
	CAutoLock lock(m_previewParams.m_mutex);
	if (m_previewParams.m_kcolorOption >= 0)
	{
		const APC_STREAM_INFO& streamKColorInfo = m_pStreamKColorInfo[m_previewParams.m_kcolorOption];
		return CPoint(streamKColorInfo.nWidth, streamKColorInfo.nHeight);
	}

	return CPoint(0, 0);
}

CPoint CPreviewImageDlg::CurColorImgRes()
{
    CAutoLock lock(m_previewParams.m_mutex);
    if (m_previewParams.m_colorOption >= 0)
    {
        const APC_STREAM_INFO& streamColorInfo = m_pStreamColorInfo[m_previewParams.m_colorOption];
        return CPoint(streamColorInfo.nWidth, streamColorInfo.nHeight);
    }

    return CPoint(0, 0);
}

CPoint CPreviewImageDlg::CurDepthImgRes()
{
    CAutoLock lock(m_previewParams.m_mutex);

    CPoint res( 0, 0 );

    if ( m_previewParams.m_depthOption >= 0 )
    {
        const APC_STREAM_INFO& streamDepthInfo = m_pStreamDepthInfo[ m_previewParams.m_depthOption ];

        res.SetPoint( streamDepthInfo.nWidth, streamDepthInfo.nHeight );

		if ( APCImageType::DEPTH_8BITS == APCImageType::DepthDataTypeToDepthImageType( m_previewParams.m_depthType ) )
		{
            res.SetPoint( streamDepthInfo.nWidth * 2, streamDepthInfo.nHeight );
		}
    }
    return res;
}

int CPreviewImageDlg::GetDepthStreamIndex(int depthIndex) const
{
    if (depthIndex == 0)//multi-baseline depth0
    {
        return 0;
    }

    return ((CComboBox*)GetDlgItem(IDC_COMBO_DEPTH_STREAM))->GetCurSel();
}

int CPreviewImageDlg::GetDepthStreamIndex(CPoint depthRes) const
{
    const BOOL bIs8bits = APCImageType::DEPTH_8BITS == APCImageType::DepthDataTypeToDepthImageType( m_previewParams.m_depthType );

    int iWidth = NULL;

    for ( int i = 0; i < m_depthStreamOptionCount; ++i )
    {
        iWidth = bIs8bits ? ( m_pStreamDepthInfo[ i ].nWidth * 2 ) : m_pStreamDepthInfo[ i ].nWidth;

        if ( iWidth == depthRes.x && m_pStreamDepthInfo[ i ].nHeight == depthRes.y )
        {
            return i;
        }
    }
    return EOF;
}

int CPreviewImageDlg::GetColorStreamIndex(CPoint colorRes, bool IsMJPG) const
{
	for (int i = 0; i < m_colorStreamOptionCount; ++i)
	{
		const APC_STREAM_INFO& streamColorInfo = m_pStreamColorInfo[i];
		if (streamColorInfo.nWidth == colorRes.x && streamColorInfo.nHeight == colorRes.y &&
			streamColorInfo.bFormatMJPG == IsMJPG)
		{
			return i;
		}
	}

	return -1;
}

/*
	Note: we transfer mpeg to rgb on receive frame on some case, this method is not unreliable, it should be deprecated.
 */
BOOL CPreviewImageDlg::IsColorStreamMJPEG()
{
    CAutoLock lock(m_previewParams.m_mutex);
    if (m_previewParams.m_colorOption >= 0)
    {
        const APC_STREAM_INFO& streamColorInfo = m_pStreamColorInfo[m_previewParams.m_colorOption];
        return streamColorInfo.bFormatMJPG;
    }

    return FALSE;
}

BOOL CPreviewImageDlg::IsStream0ColorPlusDepth()
{
#ifdef ESPDI_EG
    return false;
#else
    if ( !IsDevicePid( APC_PID_8040S ) && !IsDevicePid( APC_PID_8054 ) && !IsDevicePid( APC_PID_8038 ) ) return FALSE;

    const int colorStreamIndex = ((CComboBox*)GetDlgItem(IDC_COMBO_COLOR_STREAM))->GetCurSel();

    if ( colorStreamIndex < 0 )
    {
        return FALSE;
    }
    return ( ( m_pStreamColorInfo[ colorStreamIndex ].nWidth == 2160 && m_pStreamColorInfo[ colorStreamIndex ].nHeight == 1920  ) ||
		     ( m_pStreamColorInfo[ colorStreamIndex ].nWidth == 2560 && m_pStreamColorInfo[ colorStreamIndex ].nHeight == 1280  ) ||
             ( m_pStreamColorInfo[ colorStreamIndex ].nWidth == 2560 && m_pStreamColorInfo[ colorStreamIndex ].nHeight == 720   ) );
#endif
}

void CPreviewImageDlg::UpdateUIForDemo()
{
    ((CButton*)GetDlgItem(IDC_CHECK_COLOR_STREAM))->SetCheck(BST_CHECKED);
    OnBnClickedCheckColorStream();

    if(GetDlgItem(IDC_CHECK_DEPTH_FUSION)->IsWindowEnabled())
    {
        ((CButton*)GetDlgItem(IDC_CHECK_DEPTH_FUSION))->SetCheck(BST_CHECKED);
        OnBnClickedCheckDepthFusion();

        ((CButton*)GetDlgItem(IDC_RADIO_DEPTH_MAPPING_ZDTABLE))->SetCheck(BST_UNCHECKED);
        ((CButton*)GetDlgItem(IDC_RADIO_DEPTH_MAPPING_MANUAL))->SetCheck(BST_CHECKED);
        OnBnClickedRadioDepthMappingManual();

        ( ( CComboBox* )GetDlgItem( IDC_COMBO_FRAME_RATE ) )->SetCurSel( 5 ); //fps 15
        ( ( CComboBox* )GetDlgItem( IDC_DEPTH_FRAME_RATE ) )->SetCurSel( 5 ); //fps 15
        GetDlgItem( IDC_COMBO_FRAME_RATE )->EnableWindow( FALSE );
        GetDlgItem( IDC_DEPTH_FRAME_RATE )->EnableWindow( FALSE );
    }
	GetDlgItem(IDC_CHECK_POINTCLOUD_VIEWER)->EnableWindow(TRUE);
	GetDlgItem(IDC_SNAPSHOT_BTN)->EnableWindow(FALSE);
	GetDlgItem(IDC_FRAME_SYNC)->EnableWindow(FALSE);
}

void CPreviewImageDlg::UpdateKcolorStreamUI(int selectIndex)
{
	//K color combo box
	CComboBox* pKcolorComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_K_COLOR_STREAM);
	pKcolorComboBox->ResetContent();
	for (int i = 0; i < m_kcolorStreamOptionCount; ++i)
	{
		const APC_STREAM_INFO& streamKcolorInfo = m_pStreamKColorInfo[i];
		std::ostringstream kcolorOption;
		kcolorOption << "[ " << std::setfill(' ') << std::setw(4) << streamKcolorInfo.nWidth << " x "
			<< std::setfill(' ') << std::setw(4) << streamKcolorInfo.nHeight << " ] "
			<< (streamKcolorInfo.bFormatMJPG ? "MJPG" : "YUYV");
		pKcolorComboBox->AddString(CString(kcolorOption.str().c_str()));
	}

	if (pKcolorComboBox->GetCount() > 0)
	{
		if (selectIndex > 0 && selectIndex < pKcolorComboBox->GetCount())
		{
			pKcolorComboBox->SetCurSel(selectIndex);
		}
		else
		{
			pKcolorComboBox->SetCurSel(0);
		}
	}
}

void CPreviewImageDlg::UpdateTrackStreamUI(int selectIndex)
{
	//Track color combo box
	CComboBox* pTrackComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_T_COLOR_STREAM);
	pTrackComboBox->ResetContent();
	for (int i = 0; i < m_tcolorStreamOptionCount; ++i)
	{
		const APC_STREAM_INFO& streamTrackInfo = m_pStreamTColorInfo[i];
		std::ostringstream trackOption;
		trackOption << "[ " << std::setfill(' ') << std::setw(4) << streamTrackInfo.nWidth << " x "
			<< std::setfill(' ') << std::setw(4) << streamTrackInfo.nHeight << " ] "
			<< (streamTrackInfo.bFormatMJPG ? "MJPG" : "YUYV");
		pTrackComboBox->AddString(CString(trackOption.str().c_str()));
	}

	if (pTrackComboBox->GetCount() > 0)
	{
		if (selectIndex > 0 && selectIndex < pTrackComboBox->GetCount())
		{
			pTrackComboBox->SetCurSel(selectIndex);
		}
		else
		{
			pTrackComboBox->SetCurSel(0);
		}
	}
}

void CPreviewImageDlg::UpdateColorStreamUI(int selectIndex)
{
	//color combo box
	CComboBox* pColorComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_COLOR_STREAM);
	pColorComboBox->ResetContent();
	for (int i = 0; i < m_colorStreamOptionCount; ++i)
	{
		const APC_STREAM_INFO& streamColorInfo = m_pStreamColorInfo[i];
		std::ostringstream colorOption;
		colorOption << "[ " << std::setfill(' ') << std::setw(4) << streamColorInfo.nWidth << " x "
			<< std::setfill(' ') << std::setw(4) << streamColorInfo.nHeight << " ] "
			<< (streamColorInfo.bFormatMJPG ? "MJPG" : "YUYV");
		pColorComboBox->AddString(CString(colorOption.str().c_str()));
	}

	if (pColorComboBox->GetCount() > 0)
	{
		if (selectIndex > 0 && selectIndex < pColorComboBox->GetCount())
		{
			pColorComboBox->SetCurSel(selectIndex);
		}
		else
		{
			pColorComboBox->SetCurSel(0);
		}
	}
}

void CPreviewImageDlg::UpdateDepthStreamUI(int selectIndex)
{
	//depth combo box
	CComboBox* pDepthComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_DEPTH_STREAM);
	pDepthComboBox->ResetContent();
	for (int i = 0; i < m_depthStreamOptionCount; ++i)
	{
		const APC_STREAM_INFO& streamDepthInfo = m_pStreamDepthInfo[i];
		std::ostringstream depthOption;
		depthOption << "[ " << std::setfill(' ') << std::setw(4) << streamDepthInfo.nWidth << " x "
			<< std::setfill(' ') << std::setw(4) << streamDepthInfo.nHeight << " ] "
			<< (streamDepthInfo.bFormatMJPG ? "MJPG" : "YUYV");
		pDepthComboBox->AddString(CString(depthOption.str().c_str()));
	}

	if (pDepthComboBox->GetCount() > 0)
	{
		if (selectIndex > 0 && selectIndex < pDepthComboBox->GetCount())
		{
			pDepthComboBox->SetCurSel(selectIndex);
		}
		else
		{
			pDepthComboBox->SetCurSel(0);
		}
	}
}

void CPreviewImageDlg::UpdateUI()
{
	UpdateColorStreamUI(0);
	UpdateDepthStreamUI(0);	
	UpdateKcolorStreamUI(1);
	UpdateTrackStreamUI(2);
	GetDlgItem(IDC_COMBO_K_COLOR_STREAM)->EnableWindow( IsDevicePid( APC_PID_8054 ) || IsDevicePid( APC_PID_8040S ) || IsDevicePid(APC_PID_ORANGE));
	GetDlgItem(IDC_COMBO_T_COLOR_STREAM)->EnableWindow(FALSE);

    GetDlgItem(IDC_CHECK_DEPTH0)->EnableWindow(FALSE);

    bool support360 = APC_Is360Device(m_hApcDI, &m_DevSelInfo);
    CButton* button360 = (CButton*)GetDlgItem(IDC_CHECK_360MODE);
    button360->EnableWindow(support360);
    button360->SetCheck((support360 ? BST_CHECKED : BST_UNCHECKED));

#ifdef ESPDI_EG
    GetDlgItem(IDC_CHECK_DEPTH2)->EnableWindow(FALSE);
#else
    //depth2
    if ( IsDevicePid( APC_PID_8038 ) )
    {
        GetDlgItem(IDC_CHECK_DEPTH2)->EnableWindow(TRUE);
    }
    else
    {
        GetDlgItem(IDC_CHECK_DEPTH2)->EnableWindow(FALSE);
    }
#endif

    //depth fusion
    GetDlgItem(IDC_CHECK_DEPTH_FUSION)->EnableWindow(FALSE);
    GetDlgItem(IDC_CHECK_FUSION_SELECT_DLG)->EnableWindow(FALSE);
    GetDlgItem(IDC_CHECK_FUSION_SWPP)->EnableWindow(FALSE);

#ifndef ESPDI_EG
    //HW post process
    // status of hw post process is only valid after starting preview, and default is on
    ((CButton*)GetDlgItem(IDC_CHECK_HARDWARE_POSTPROC))->SetCheck(IsSupportHwPostProc() ? BST_CHECKED : BST_UNCHECKED);
    GetDlgItem(IDC_CHECK_HARDWARE_POSTPROC)->EnableWindow(FALSE);
#endif

    //depth mapping
    ((CButton*)GetDlgItem(IDC_RADIO_DEPTH_MAPPING_ZDTABLE))->SetCheck(BST_CHECKED);
    OnBnClickedRadioDepthMappingZdtable();
#ifndef ESPDI_EG
    if ( IsDevicePid( APC_PID_8038 ) )
    {
        SetDlgItemText(IDC_EDIT_CAM_FOCUS, L"800");
        SetDlgItemText(IDC_EDIT_BLDIST0, L"30");
        SetDlgItemText(IDC_EDIT_BLDIST1, L"60");
        SetDlgItemText(IDC_EDIT_BLDIST2, L"150");
    }
#endif

    //color output
    ((CComboBox*)GetDlgItem(IDC_COMBO_COLOR_OUTPUT_CTRL))->SetCurSel((support360 ? 1: 0));
    if (support360)
    {
        ((CComboBox*)GetDlgItem(IDC_COMBO_COLOR_OUTPUT_CTRL))->EnableWindow(FALSE);
    }

    //depth output
    ((CComboBox*)GetDlgItem(IDC_COMBO_DEPTH_OUTPUT_CTRL))->SetCurSel(0);

#ifdef ESPDI_EG
    GetDlgItem(IDC_COMBO_DEPTH_BIT_SEL_CTRL)->EnableWindow(FALSE);
#else
    //force to disable RGB image output
    ((CComboBox*)GetDlgItem(IDC_COMBO_COLOR_OUTPUT_CTRL))->EnableWindow(FALSE);

	//Store the status of IDC_COMBO_DEPTH_BIT_SEL_CTRL.
    //depth bits option
    CComboBox* pDepthBitCbx = (CComboBox*)GetDlgItem(IDC_COMBO_DEPTH_BIT_SEL_CTRL);

    switch (m_DevType)
    {
    case PUMA:
        {
            GetDlgItem(IDC_STATIC_DEPTH_BIT_SEL)->ShowWindow(SW_SHOW);
            pDepthBitCbx->ShowWindow(SW_SHOW);

            pDepthBitCbx->SetItemData( pDepthBitCbx->AddString( L"8 bits" ), APC_DEPTH_DATA_8_BITS );
            pDepthBitCbx->SetItemData( pDepthBitCbx->AddString(L"8 bits + 0x80"), APC_DEPTH_DATA_8_BITS_x80 );
            pDepthBitCbx->SetItemData( pDepthBitCbx->AddString(L"11 bits"), APC_DEPTH_DATA_11_BITS );
            pDepthBitCbx->SetItemData( pDepthBitCbx->AddString(L"14 bits"), APC_DEPTH_DATA_14_BITS );

            WORD wType = 0;
            int depthComboIndex = NULL;

            if ( APC_GetDepthDataType(m_hApcDI, &m_DevSelInfo, &wType) != APC_OK )
            {
                AfxMessageBox(_T("APC_GetDepthDataType failed !!"));
            }
            switch ( wType )
            {
            case APC_DEPTH_DATA_8_BITS:     pDepthBitCbx->SetCurSel( 0 ); break;
            case APC_DEPTH_DATA_8_BITS_x80: pDepthBitCbx->SetCurSel( 1 ); break;
            case APC_DEPTH_DATA_11_BITS:    pDepthBitCbx->SetCurSel( 2 ); break;
            case APC_DEPTH_DATA_14_BITS:    pDepthBitCbx->SetCurSel( 3 ); break;
            default:                            pDepthBitCbx->SetCurSel( 0 ); break;
            }
            // data mode
            ( ( CButton* )GetDlgItem( IDC_RADIO_RECTIFY_DATA ) )->SetCheck( BST_CHECKED );
        }
        break;
    case AXES1:
    default:
        {
            GetDlgItem(IDC_STATIC_DEPTH_BIT_SEL)->ShowWindow(SW_HIDE);
            pDepthBitCbx->SetItemData( pDepthBitCbx->AddString( L"8 bits" ), APC_DEPTH_DATA_8_BITS );//AXES1 only 1 opion
            pDepthBitCbx->SetCurSel(0);
            pDepthBitCbx->ShowWindow(SW_HIDE);

            // data mode
            GetDlgItem(IDC_RADIO_RECTIFY_DATA)->EnableWindow(FALSE);
            GetDlgItem(IDC_RADIO_RAW_DATA)->EnableWindow(FALSE);
        }
        break;
    }
#endif
    //IR
	//EnableIR(FALSE);

    //frame rate
    CComboBox* pCbxC = ( ( CComboBox* )GetDlgItem( IDC_COMBO_FRAME_RATE ) );
    CComboBox* pCbxD = ( ( CComboBox* )GetDlgItem( IDC_DEPTH_FRAME_RATE ) );
    pCbxC->AddString( L"4" );
    pCbxD->AddString( L"4" );
    pCbxC->AddString( L"5" );
    pCbxD->AddString( L"5" );
    pCbxC->AddString( L"8" );
    pCbxD->AddString( L"8" );
    pCbxC->AddString( L"10" );
    pCbxD->AddString( L"10" );
    pCbxC->AddString( L"12" );
    pCbxD->AddString( L"12" );
    pCbxC->AddString( L"15" );
    pCbxD->AddString( L"15" );
    pCbxC->AddString( L"24" );
    pCbxD->AddString( L"24" );
    pCbxC->AddString( L"30" );
    pCbxD->AddString( L"30" );
    pCbxC->AddString( L"45" );
    pCbxD->AddString( L"45" );
    pCbxC->AddString( L"60" );
    pCbxD->AddString( L"60" );
    pCbxC->AddString( L"90" );
    pCbxD->AddString( L"90" );
    pCbxC->SetCurSel( 7 );
    pCbxD->SetCurSel( 7 );

#ifndef ESPDI_EG
	if ( IsDevicePid( APC_PID_8054 ) || IsDevicePid( APC_PID_8040S ) )
	{
		GetDlgItem(IDC_CHECK_ROTATE_IMG)->EnableWindow(TRUE);
	}
	else
#endif
	{
		GetDlgItem(IDC_CHECK_ROTATE_IMG)->EnableWindow(FALSE);
	}

    GetDlgItem(IDC_CHECK_INTERLEAVE_MODE)->EnableWindow( FALSE );

	//Z Far
#ifndef ESPDI_EG
    SetDlgItemInt( IDC_EDIT_ZFAR, IsDevicePid( APC_PID_8038 ) ? 2000 : 1000 );
    SetDlgItemInt( IDC_EDIT_ZNEAR, 0 );
#endif

	//update DM color mape mode
	OnCbnSelchangeComboDepthOutputCtrl();

	GetDlgItem(IDC_CHK_MULTI_SYNC)->EnableWindow(IsDevicePid(APC_PID_8053) || IsDevicePid(APC_PID_8059) || IsDevicePid(APC_PID_8062));

	GetDlgItem(IDC_CHECK_POINTCLOUD_VIEWER)->EnableWindow(TRUE);
	GetDlgItem(IDC_SNAPSHOT_BTN)->EnableWindow(FALSE);
	GetDlgItem(IDC_FRAME_SYNC)->EnableWindow(FALSE);
}

bool CPreviewImageDlg::IsSupportHwPostProc() const
{
    return (m_DevType == PUMA);
}

bool CPreviewImageDlg::UpdateStreamInfo()
{
    auto GetResolution = [ & ]( APC_STREAM_INFO* pStreamRes0, int* pStreamResCount0, APC_STREAM_INFO* pStreamRes1, int* pStreamResCount1, int PID )
    {
        memset( pStreamRes0, NULL, sizeof( APC_STREAM_INFO ) * APC_MAX_STREAM_COUNT );
        memset( pStreamRes1, NULL, sizeof( APC_STREAM_INFO ) * APC_MAX_STREAM_COUNT );

        int ret = ( EOF == PID ) ? APC_GetDeviceResolutionList( m_hApcDI, &m_DevSelInfo,
                                                                    APC_MAX_STREAM_COUNT, pStreamRes0,
                                                                    APC_MAX_STREAM_COUNT, pStreamRes1 ) :
                                   APC_GetDeviceResolutionListEx( m_hApcDI, &m_DevSelInfo,
                                                                      APC_MAX_STREAM_COUNT, pStreamRes0,
                                                                      APC_MAX_STREAM_COUNT, pStreamRes1, PID );
        *pStreamResCount0 = (int)((BYTE*)(&ret))[1];
        *pStreamResCount1 = (int)((BYTE*)(&ret))[0];
    };
    GetResolution( m_pStreamColorInfo, &m_colorStreamOptionCount, m_pStreamDepthInfo, &m_depthStreamOptionCount, EOF );

    if ( IsDevicePid( APC_PID_8054 ) )
    {
        GetResolution( m_pStreamKColorInfo, &m_kcolorStreamOptionCount, m_pStreamKDepthInfo, &m_kdepthStreamOptionCount, APC_PID_8054_K );
    }
    if (IsDevicePid(APC_PID_ORANGE))
    {
        GetResolution(m_pStreamKColorInfo, &m_kcolorStreamOptionCount, m_pStreamKDepthInfo, &m_kdepthStreamOptionCount, APC_PID_ORANGE_K);
    }
    if (IsDevicePid(APC_PID_8063))
    {
        GetResolution(m_pStreamKColorInfo, &m_kcolorStreamOptionCount, m_pStreamKDepthInfo, &m_kdepthStreamOptionCount, APC_PID_8063_K);
    }
    if ( IsDevicePid( APC_PID_8040S ) )
    {
        GetResolution( m_pStreamKColorInfo, &m_kcolorStreamOptionCount, m_pStreamKDepthInfo, &m_kdepthStreamOptionCount, APC_PID_8040S_K );
    }
	else if ( IsDevicePid( APC_PID_8060 ) )
	{
		GetResolution( m_pStreamKColorInfo, &m_kcolorStreamOptionCount, m_pStreamKDepthInfo, &m_kdepthStreamOptionCount, APC_PID_8060_K );
        GetResolution( m_pStreamTColorInfo, &m_tcolorStreamOptionCount, m_pStreamTDepthInfo, &m_tdepthStreamOptionCount, APC_PID_8060_T );
	}
	
    return false;
}

void CPreviewImageDlg::UpdateIRConfig()
{
    APC_GetIRMinValue(m_hApcDI, &m_DevSelInfo, &m_irRange.first);
    APC_GetIRMaxValue(m_hApcDI, &m_DevSelInfo, &m_irRange.second);

	if ( IsDevicePid( APC_PID_HYPATIA) )  m_irRange.second = MAX_IR_HYPATIA;
    if ( IsDevicePid( APC_PID_8060  ) ) m_irRange.second = 5;
    if ( IsDevicePid( APC_PID_8040S ) ) m_irRange.second = 4;
}

BOOL CPreviewImageDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    UpdateStreamInfo();
    UpdateUI();
    UpdateUIForDemo();

	InitIR();
    InitDepthROI();
    InitModeConfig();

	InitAutoModuleSync();

    ( ( CSliderCtrl* )GetDlgItem( IDC_SLD_PCL_PSIZE ) )->SetRange( 1, 5 );

    return TRUE;
}

BOOL CPreviewImageDlg::CheckRestart()
{
    CAutoLock lock( m_previewParams.m_mutex );

	static int restartTimeSec = 1;

	time_t now = time(NULL);

    for ( auto& i :m_mapColorStreamTimeStamp )
    {
		if ( difftime( now, i.second) > restartTimeSec )
        {
			return TRUE;
		}
	}
	for ( auto& i :m_mapDepthStreamTimeStamp )
    {
		if ( difftime( now, i.second) > restartTimeSec )
        {
			return TRUE;
		}
	}
    if ( m_mapColorStreamTimeStamp.size() && m_mapDepthStreamTimeStamp.size() )
    {
        SetTimer(CHECK_RESTART_TIMER, CHECK_IMAGE_CALLBACK_TIME_INTERVAL, nullptr);
    }
	return ( m_mapColorStreamTimeStamp.empty() && m_previewParams.m_colorOption > EOF ) ||
           ( m_mapDepthStreamTimeStamp.empty() && m_previewParams.m_depthOption > EOF );
}

BOOL CPreviewImageDlg::CheckDepthImageReady()
{
    CAutoLock lock( m_previewParams.m_mutex );

    UINT iDepthCount = NULL;

    if ( m_previewParams.m_depthSwitch & ApcDIDepthSwitch::Depth0 ) iDepthCount++;
    if ( m_previewParams.m_depthSwitch & ApcDIDepthSwitch::Depth1 ) iDepthCount++;
    if ( m_previewParams.m_depthSwitch & ApcDIDepthSwitch::Depth2 ) iDepthCount++;

    return ( m_mapDepthStreamTimeStamp.size() == iDepthCount );
}

BOOL CPreviewImageDlg::CheckColorImageReady()
{
    CAutoLock lock( m_previewParams.m_mutex );

    return !m_mapColorStreamTimeStamp.empty();
}

void CPreviewImageDlg::OnTimer(UINT_PTR nIDEvent)
{
    switch (nIDEvent)
    {
    case CHECK_RESTART_TIMER:
		if ( CheckRestart() )
		{
            KillTimer( CHECK_RESTART_TIMER );
            KillTimer( CHECK_REOPEN_TIMER );
            KillTimer( CHECK_REGISTER_TIMER );
            KillTimer( CHECK_SYNC_TIMER );
            KillTimer( CHECK_805359_MASTER_RESET_TIMER );

            int iReopenTime = 800;

            if ( IsDevicePid( APC_PID_8054 ) || IsDevicePid( APC_PID_8040S ) ) iReopenTime = CHECK_IMAGE_CALLBACK_FIRSTTIME_INTERVAL;
            else if ( IsDevicePid( APC_PID_8038 ) )                                iReopenTime = 5000;

            SetTimer(CHECK_REOPEN_TIMER, iReopenTime, nullptr);
        }
        break;
    case CHECK_REOPEN_TIMER:
        {
            KillTimer( CHECK_REOPEN_TIMER );

            APC_CloseDevice(m_hApcDI, &m_DevSelInfo);
			APC_Release(&m_hApcDI);
			APC_Init(&m_hApcDI, false);
			//m_pdlgVideoDeviceDlg->IMU_Device_Reopen(m_hApcDI, m_DevSelInfo);

            if ( m_previewParams.m_colorPreview.m_previewDlg  ) ( ( CColorDlg* )m_previewParams.m_colorPreview.m_previewDlg  )->SetHandle( m_hApcDI, m_DevSelInfo );
            if ( m_previewParams.m_kcolorPreview.m_previewDlg ) ( ( CColorDlg* )m_previewParams.m_kcolorPreview.m_previewDlg )->SetHandle( m_hApcDI, m_DevSelInfo );
            if ( m_previewParams.m_trackPreview.m_previewDlg  ) ( ( CColorDlg* )m_previewParams.m_trackPreview.m_previewDlg  )->SetHandle( m_hApcDI, m_DevSelInfo );

            for (int i = 0; i < APC_MAX_DEPTH_STREAM_COUNT; ++i)
			{
				if (m_previewParams.m_depthPreview[i].m_previewDlg != nullptr)
				{
					((CDepthDlg*)m_previewParams.m_depthPreview[i].m_previewDlg)->SetHandle(m_hApcDI, m_DevSelInfo);
				}
			}
            if (m_previewParams.m_depthFusionPreview.m_previewDlg != nullptr)
			{
				((CDepthDlg*)m_previewParams.m_depthFusionPreview.m_previewDlg)->SetHandle(m_hApcDI, m_DevSelInfo);
			}
			if (m_depthFusionHelper != nullptr)
			{
				m_depthFusionHelper->SetSDKHandle(m_hApcDI, &m_DevSelInfo);
			}
            ::CreateThread( NULL, NULL, Thread_Preview, this, NULL, NULL );
			ReOpen_MappingIMU();
        }
        break;
    case CHECK_REGISTER_TIMER:
        {
            if ( CheckDepthImageReady() )
            {
                KillTimer( CHECK_REGISTER_TIMER );
                AdjustRegister();
                OnBnClickedCheckPumaPostproc();
                RegisterSettings::DM_Quality_Register_Setting(m_hApcDI, &m_DevSelInfo);
            }
        }
        break;
    case CHECK_SYNC_TIMER:
        {
            if ( CheckColorImageReady() )
            {
                KillTimer( CHECK_SYNC_TIMER );
                DoMultiModuleSynCommand();
            }
        }
        break;
    case CHECK_805359_MASTER_RESET_TIMER:
        {
            if ( m_imgSerialNumber > GetDlgItemInt( IDC_COMBO_FRAME_RATE ) * 30 ) // master reset all slave every 30s
            {
                DoMultiModuleSynReset();
            }
        }
        break;
    default:
        break;
    }
    CDialog::OnTimer(nIDEvent);
}

BEGIN_MESSAGE_MAP(CPreviewImageDlg, CDialog)
    ON_MESSAGE(WM_MSG_CLOSE_PREVIEW_DLG, &CPreviewImageDlg::OnClosePreviewDlg)
    ON_MESSAGE(WM_MSG_UPDATE_MOUSE_POS_FROM_DIALOG, &CPreviewImageDlg::OnUpdateMousePos)
    ON_MESSAGE(WM_MSG_SNAPSHOT, &CPreviewImageDlg::OnSnapshot)
	ON_MESSAGE(WM_MSG_SNAPSHOT_ALL, &CPreviewImageDlg::OnSnapshotAll)
    ON_MESSAGE(WM_MSG_SNAPSHOT_COMPLETE, &CPreviewImageDlg::OnSnapshotComplete)
    ON_BN_CLICKED(IDC_CHECK_COLOR_STREAM, &CPreviewImageDlg::OnBnClickedCheckColorStream)
    ON_BN_CLICKED(IDC_CHECK_360MODE, &CPreviewImageDlg::OnBnClickedCheck360mode)
    ON_BN_CLICKED(IDC_PREVIEW_BTN, &CPreviewImageDlg::OnBnClickedPreviewBtn)
    ON_WM_DESTROY()
    ON_CBN_SELCHANGE(IDC_COMBO_COLOR_STREAM, &CPreviewImageDlg::OnCbnSelchangeComboColorStream)
    ON_WM_HSCROLL()
    ON_BN_CLICKED(IDC_CHECK_DEPTH_FUSION, &CPreviewImageDlg::OnBnClickedCheckDepthFusion)
    ON_BN_CLICKED(IDC_RADIO_DEPTH_MAPPING_ZDTABLE, &CPreviewImageDlg::OnBnClickedRadioDepthMappingZdtable)
    ON_BN_CLICKED(IDC_RADIO_DEPTH_MAPPING_MANUAL, &CPreviewImageDlg::OnBnClickedRadioDepthMappingManual)
	ON_CBN_SELCHANGE(IDC_COMBO_DEPTH_OUTPUT_CTRL, &CPreviewImageDlg::OnCbnSelchangeComboDepthOutputCtrl)
    ON_WM_TIMER()
	ON_BN_CLICKED(IDC_CHECK_HARDWARE_POSTPROC, &CPreviewImageDlg::OnBnClickedCheckPumaPostproc)
    ON_BN_CLICKED(IDC_CHECK_PP, &CPreviewImageDlg::OnBnClickedCheckPp)
    ON_BN_CLICKED(IDC_CHECK_FUSION_SWPP, &CPreviewImageDlg::OnBnClickedCheckFusionSwpp)
    ON_BN_CLICKED(IDC_CHECK_INTERLEAVE_MODE, &CPreviewImageDlg::OnBnClickedCheckInterleaveMode)
	ON_BN_CLICKED(IDC_SNAPSHOT_BTN, &CPreviewImageDlg::OnBnClickedSnapshotBtn)
	ON_BN_CLICKED(IDC_CHECK_ROTATE_IMG, &CPreviewImageDlg::OnBnClickedCheckRotateImg)
	ON_BN_CLICKED(IDC_CHECK_POINTCLOUD_VIEWER, &CPreviewImageDlg::OnBnClickedCheckPointcloudViewer)
	ON_BN_CLICKED(IDC_FRAME_SYNC, &CPreviewImageDlg::OnBnClickedFrameSync)
	ON_BN_CLICKED(IDC_CHECK_T_COLOR_STREAM, &CPreviewImageDlg::OnBnClickedCheckTColorStream)
	ON_CBN_SELCHANGE(IDC_COMBO_T_COLOR_STREAM, &CPreviewImageDlg::OnCbnSelchangeComboTColorStream)
    ON_CBN_SELCHANGE(IDC_COMBO_MODE_CONFIG, &CPreviewImageDlg::OnCbnSelchangeComboModeConfig)
    ON_BN_CLICKED(IDC_RADIO_RAW_DATA, &CPreviewImageDlg::OnBnClickedRadioRectifyAndRawData)
    ON_BN_CLICKED(IDC_RADIO_RECTIFY_DATA, &CPreviewImageDlg::OnBnClickedRadioRectifyAndRawData)
    ON_CBN_SELCHANGE(IDC_COMBO_FRAME_RATE, &CPreviewImageDlg::OnCbnSelchangeComboFrameRate)
    ON_BN_CLICKED(IDC_BT_Z_SET, &CPreviewImageDlg::OnBnClickedBtZSet)
    ON_BN_CLICKED(IDC_BT_Z_RESET, &CPreviewImageDlg::OnBnClickedBtZReset)
    ON_BN_CLICKED(IDC_CHK_MASTER, &CPreviewImageDlg::OnBnClickedChkMaster)
    ON_BN_CLICKED(IDC_CHK_IRMAX_EXT, &CPreviewImageDlg::OnBnClickedChkIrmaxExt)
    ON_BN_CLICKED(IDC_CHK_PCV_NOCOLOR, &CPreviewImageDlg::OnBnClickedChkPcvNocolor)
    ON_BN_CLICKED(IDC_CHK_PCV_SINGLE, &CPreviewImageDlg::OnBnClickedChkPcvSingle)
	ON_BN_CLICKED(IDC_CHK_MULTI_SYNC, &CPreviewImageDlg::OnBnClickedChkMultiSync)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()

LRESULT CPreviewImageDlg::OnClosePreviewDlg(WPARAM wParam, LPARAM lParam)
{
	if (m_isPreviewed) {

        EnableIR(FALSE);
        CloseDeviceAndStopPreview((CDialog*)wParam);

		m_isPreviewed = false;
	}
    
    return 0;
}

LRESULT CPreviewImageDlg::OnUpdateMousePos(WPARAM wParam, LPARAM lParam)
{
    const CPoint cpDepth( LOWORD( wParam ), HIWORD( wParam ) );

    for (int i = 0; i < APC_MAX_DEPTH_STREAM_COUNT; ++i)
    {
        if ( m_previewParams.m_depthPreview[i].m_previewDlg != nullptr )
        {
            ( ( CDepthDlg* )m_previewParams.m_depthPreview[ i ].m_previewDlg )->SetDepthPos( cpDepth );
        }
    }
    if ( m_previewParams.m_depthFusionPreview.m_previewDlg != nullptr )
    {
        ( ( CDepthDlg* )m_previewParams.m_depthFusionPreview.m_previewDlg )->SetDepthPos( cpDepth );
    }
    return 0;
}

void CPreviewImageDlg::SaveDepthYuv(std::vector<unsigned char> bufDepth, APCImageType::Value depthImageType,int widthDepth, int heightDepth, const char* pFileName)
{
	unsigned char DepthmapBits = (depthImageType == APCImageType::DEPTH_8BITS) ? 1 : 2;
	SaveYuv(bufDepth, DepthmapBits, widthDepth, heightDepth, pFileName);
}

void CPreviewImageDlg::SaveDepthGrayBmp(int DepthNum, const char* pFileName)
{
	std::vector<BYTE> bufferDepthToColor;
	((CDepthDlg*)m_previewParams.m_depthPreview[DepthNum].m_previewDlg)->GetDepthGrayImageFromColorPalette(bufferDepthToColor);
	BITMAPINFO* bmiDepth = ((CDepthDlg*)m_previewParams.m_depthPreview[DepthNum].m_previewDlg)->GetbmiDept();
	SaveImage2(&bufferDepthToColor[0], bmiDepth, pFileName, Gdiplus::ImageFormatBMP);
}

void CPreviewImageDlg::SaveDepthColorBmp(int DepthNum, const char* pFileName)
{
	//unsigned char* pBufferDepthToColor = ((CDepthDlg*)m_previewParams.m_depthPreview[DepthNum].m_previewDlg)->GetDepthColorMapImage();
	std::vector<BYTE> bufferDepthToColor; 
	((CDepthDlg*)m_previewParams.m_depthPreview[DepthNum].m_previewDlg)->GetDepthColorfulImageFromColorPalette(bufferDepthToColor);
	BITMAPINFO* bmiDepth = ((CDepthDlg*)m_previewParams.m_depthPreview[DepthNum].m_previewDlg)->GetbmiDept();
	SaveImage2(&bufferDepthToColor[0], bmiDepth, pFileName, Gdiplus::ImageFormatBMP);
}

void CPreviewImageDlg::DepthFusionBmp(APCImageType::Value depthImageType)
{
	std::vector<unsigned char> bufDepth;
	bufDepth.clear();
	int widthDepth = 0;
	int heightDepth = 0;
	int snDepth = -1;

	if (m_previewParams.m_depthFusionPreview.m_previewDlg != nullptr &&
		((CDepthDlg*)m_previewParams.m_depthFusionPreview.m_previewDlg)->GetDepthData(
			bufDepth, depthImageType, widthDepth, heightDepth, snDepth))
	{
		int imgWidth = widthDepth;
		if (depthImageType == APCImageType::DEPTH_8BITS)
		{// divide 16bits data into 8bits x 2
			imgWidth = widthDepth * 2;
		}

		DWORD timestamp = GetTickCount();
		CStringA Filename_DepthFusionbmp;

        Filename_DepthFusionbmp.Format( "%s\\DepthFusion_sn%d_%d.yuv", GetCurrentModuleFolder().c_str(), snDepth, timestamp );
        SaveDepthYuv( bufDepth, depthImageType, imgWidth, heightDepth, Filename_DepthFusionbmp );

        unsigned char* pBufferDepthToColor = ((CDepthDlg*)m_previewParams.m_depthFusionPreview.m_previewDlg)->GetDepthColorMapImage();
        BITMAPINFO* bmiDepth = ((CDepthDlg*)m_previewParams.m_depthFusionPreview.m_previewDlg)->GetbmiDept();

        Filename_DepthFusionbmp.Format( "%s\\DepthFusion_sn%d_%d.bmp", GetCurrentModuleFolder().c_str(), snDepth, timestamp );
		SaveImage2(pBufferDepthToColor, bmiDepth, Filename_DepthFusionbmp, Gdiplus::ImageFormatBMP);
	}
}

void CPreviewImageDlg::InitIR()
{
	UpdateIRConfig();
	if (IsDevicePid(APC_PID_HYPATIA)) {
		APC_SetIRMaxValue(m_hApcDI, &m_DevSelInfo, MAX_IR_HYPATIA);
		GetDlgItem(IDC_CHK_IRMAX_EXT)->EnableWindow(false);
	}
    else if (IsDevicePid(APC_PID_NORA))
    {
        WORD wTestMaxIR = NULL;
        APC_GetIRMaxValue(m_hApcDI, &m_DevSelInfo, &wTestMaxIR);
        APC_SetIRMaxValue(m_hApcDI, &m_DevSelInfo, wTestMaxIR);
        GetDlgItem(IDC_CHK_IRMAX_EXT)->EnableWindow(true);
    }
    else if ( !IsDevicePid( APC_PID_SALLY ) &&
			  !IsDevicePid( APC_PID_8040S ) )
    {
        WORD wTestMaxIR = NULL;

        APC_SetIRMaxValue( m_hApcDI, &m_DevSelInfo, 7 );
        APC_GetIRMaxValue(m_hApcDI, &m_DevSelInfo, &wTestMaxIR);

        GetDlgItem( IDC_CHK_IRMAX_EXT )->EnableWindow( 7 == wTestMaxIR && m_irRange.second > 5 );
        ( ( CButton* )GetDlgItem( IDC_CHK_IRMAX_EXT ) )->SetCheck( m_irRange.second == MAX_IR_MAXIMUM );

        APC_SetIRMaxValue( m_hApcDI, &m_DevSelInfo, m_irRange.second );
    }
	CSliderCtrl* irSliderCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_IR);
	irSliderCtrl->SetRange(m_irRange.first, m_irRange.second);

    switch (m_devinfoEx.wPID)
    {
    case APC_PID_HYPATIA:
        m_irValue = MAX_IR_HYPATIA_DEFAULT;
        break;

    case APC_PID_NORA:
        m_irValue = 0;
        break;

    default:
        m_irValue = m_irRange.first + 3;
        break;
    }
	m_previewParams.m_IrPreState = m_irValue; // set initial value as IrState, and this value only change on OnHScroll();

	irSliderCtrl->SetPos(m_irValue);

	std::ostringstream irValue;
	irValue << m_irValue << " / " << m_irRange.second;
	GetDlgItem(IDC_STATIC_IR_VALUE)->SetWindowText(CString(irValue.str().c_str()));
}

void CPreviewImageDlg::InitDepthROI()
{
    ( ( CSliderCtrl* )GetDlgItem( IDC_SLD_DEPTH_ROI ) )->SetRange( 0, 4 );
    ( ( CSliderCtrl* )GetDlgItem( IDC_SLD_DEPTH_ROI ) )->SetPos( 2 );

    SetDlgItemInt( IDC_ST_DEPTH_ROI, 20 );
}

void CPreviewImageDlg::InitModeConfig()
{
    const std::vector< ModeConfig::MODE_CONFIG >& vecModeConfig = g_ModeConfig.GetModeConfigList( m_devinfoEx.wPID );

    CComboBox* pCbx = ( ( CComboBox* )GetDlgItem( IDC_COMBO_MODE_CONFIG ) );
    CString csModeName;
    USB_PORT_TYPE eUSB_Port_Type = m_eUSB_Port_Type;

    if ( IsDevicePid( APC_PID_8029 ) ) // APC_GetDevicePortType not support in 8029
    {
        eUSB_Port_Type = USB_PORT_TYPE_3_0;

        for ( int i = NULL; i < m_colorStreamOptionCount; i++ )
        {
            if ( m_pStreamColorInfo[ i ].bFormatMJPG )
            {
                eUSB_Port_Type = USB_PORT_TYPE_2_0;
                break;
            }
        }
    }
    for ( auto& Mode : vecModeConfig )
    {
        if ( eUSB_Port_Type != USB_PORT_TYPE_UNKNOW && Mode.iUSB_Type != eUSB_Port_Type ) continue;

        csModeName.Format( L"Mode %d", Mode.iMode );

        pCbx->SetItemData( pCbx->AddString( csModeName ), Mode.iMode );
    }
    if ( pCbx->GetCount() )
    {
        GetDlgItem( IDC_CHECK_COLOR_STREAM   )->EnableWindow( FALSE );
        GetDlgItem( IDC_CHECK_DEPTH1         )->EnableWindow( FALSE );
        GetDlgItem( IDC_CHECK_K_COLOR_STREAM )->EnableWindow( FALSE );
        GetDlgItem( IDC_CHECK_T_COLOR_STREAM )->EnableWindow( FALSE );
        GetDlgItem( IDC_COMBO_COLOR_STREAM   )->EnableWindow( FALSE );
        GetDlgItem( IDC_COMBO_DEPTH_STREAM   )->EnableWindow( FALSE );
        GetDlgItem( IDC_COMBO_K_COLOR_STREAM )->EnableWindow( FALSE );
        GetDlgItem( IDC_COMBO_T_COLOR_STREAM )->EnableWindow( FALSE );
        GetDlgItem( IDC_RADIO_RECTIFY_DATA   )->EnableWindow( FALSE );
        GetDlgItem( IDC_RADIO_RAW_DATA       )->EnableWindow( FALSE );

        // Special Path for Sandra.
        // Set the default video mode to 5 when using USB3.0
        if (IsDevicePid(APC_PID_SANDRA) && (eUSB_Port_Type == USB_PORT_TYPE_3_0))
        {
            int specifyDefaultMode = 5;
            int modeNumber  = pCbx->GetCount();
            for (int loop = 0 ; loop < modeNumber; loop ++)
            {
                if (pCbx->GetItemData(loop) == specifyDefaultMode)
                {
                    pCbx->SetCurSel(loop);
                    break;
                }
                else
                {
                    continue;
                }
            }
        }
        else // Normal Path
        {
            pCbx->SetCurSel(0);
        }
        pCbx->EnableWindow( TRUE );

        OnCbnSelchangeComboModeConfig();
    }
    if ( USB_PORT_TYPE_3_0 == eUSB_Port_Type ) csModeName = _T( "USB3.1 Gen 1 Mode:" );
    else                                       csModeName = _T( "USB2.0 Mode:" );

    SetDlgItemText( IDC_ST_USBMODE, csModeName );
    GetDlgItem( IDC_COMBO_MODE_CONFIG )->EnableWindow( ( ( CComboBox* )GetDlgItem( IDC_COMBO_MODE_CONFIG ) )->GetCount() > 1 );
}

void CPreviewImageDlg::EnableIR(bool enable)
{
    OnBnClickedChkIrmaxExt();

	WORD IrState = m_previewParams.m_IrPreState;

	if (!enable)
	{
		IrState = m_irRange.first;
	}
	ChangeIRValue(IrState);
}

LRESULT CPreviewImageDlg::OnSnapshotAll(WPARAM wParam, LPARAM lParam)
{
	EnumWindows(EnumWindowsProc, (LPARAM)this);
	return 0;
}

LRESULT CPreviewImageDlg::OnSnapshotComplete(WPARAM wParam, LPARAM lParam)
{
    m_pWaitDlg->EndDialog( IDCANCEL );
    return 0;
}

BOOL CPreviewImageDlg::isDepthOnly(int depthWidth, int depthHeight, BOOL isColorBufEmpty)
{
    if (depthWidth > 0
        && depthHeight > 0
        && isColorBufEmpty == TRUE)
    {
        return true;
    }
    return false;
}

UINT CPreviewImageDlg::DoSnapshot(LPVOID pParam)
{
	LPTHREAD_PARAM lpParam = (LPTHREAD_PARAM)pParam;
	
	CPreviewImageDlg* pThis = lpParam->pDlg;
/* //debug log for serial-count-interval
Sleep( 3000 );
static int index = 1;
CString csFile;
CString csData;
csFile.Format( "d:\\%d.log", index++ );
FILE* fp = fopen( csFile, "wb" );
if ( fp )
{
CAutoLock lock(pThis->m_previewParams.m_mutex);
for ( auto& i : pThis->m_log )
{
    csData.Format( "%s,%d", CTime( i.first ).Format( "%Y/%m/%d_%H:%M:%S" ), i.second );
    fwrite( csData, csData.GetLength(), 1, fp);
    fwrite( "\n", 1, 1, fp);
}
fclose( fp );
}
AfxMessageBox( csFile );
return 0;
*/
	WORD irValue = lpParam->irValue;
	float zFar = lpParam->zFar;
    float zNear = lpParam->zNear;//mm
    BOOL bUseFilter = lpParam->bUseFilter;
	delete lpParam;

	
	DWORD timestamp = GetTickCount();
	DWORD treadID = ::GetCurrentThreadId();
	TRACE("timestamp = %d\n", timestamp);
	TRACE("treadID = %d\n", treadID);
	CString stringResult = _T( "Create ply fail" );

	std::vector<float> imgFloatBufOut;
	std::vector<unsigned char> bufColor;

	int widthColor = 0;
	int heightColor = 0;
	int snColor = -1;

	struct depth_data {
		int depthIndex = 0;
		std::vector<unsigned char> bufDepth;
		int widthDepth = 0;
		int heightDepth = 0;
		int snDepth = -1;
	};
	std::vector<depth_data> depthDatas;

	APCImageType::Value depthImageType = APCImageType::DEPTH_11BITS;

	{
		CAutoLock lock(pThis->m_previewParams.m_mutex);
		for (int i = 0; i < APC_MAX_DEPTH_STREAM_COUNT; ++i)
		{
			std::vector<unsigned char> bufDepth;
			bufDepth.clear();
			int widthDepth = 0;
			int heightDepth = 0;
			int snDepth = -1;

			if (pThis->m_previewParams.m_depthPreview[i].m_previewDlg != nullptr && (((CDepthDlg*)pThis->m_previewParams.m_depthPreview[i].m_previewDlg)->GetDepthData)(
				bufDepth, depthImageType, widthDepth, heightDepth, snDepth) == true)
			{
				if (bUseFilter)
				{
					float ratio = (float)pThis->m_previewParams.m_rectifyLogData[i]->OutImgHeight / heightDepth;
					if (ratio != 1.0f) {
						int resampleWidthDepth = widthDepth * ratio;
						int resampleHeightDepth = heightDepth * ratio;

						int bufSize = resampleWidthDepth * resampleHeightDepth * 2;
						std::vector<unsigned char> dArrayResized(bufSize);
                        if ( depthImageType == APCImageType::DEPTH_8BITS ) PlyWriter::MonoBilinearFineScaler( &bufDepth[0], &dArrayResized[0], widthDepth, heightDepth, resampleWidthDepth, resampleHeightDepth, 1);
                        else PlyWriter::MonoBilinearFineScaler_short( (USHORT*)&bufDepth[0], (USHORT*)&dArrayResized[0], widthDepth, heightDepth, resampleWidthDepth, resampleHeightDepth, 1 );
						bufDepth.resize(bufSize);
						bufDepth.assign(dArrayResized.begin(), dArrayResized.end());

						widthDepth = resampleWidthDepth;
						heightDepth = resampleHeightDepth;
					}
				}

				struct depth_data dapthData = { i, bufDepth, widthDepth , heightDepth , snDepth };
				depthDatas.push_back(dapthData);
			}
		}	//for
	}
    if ( !pThis->m_bIsInterLeaveMode && pThis->m_previewParams.m_kcolorOption == EOF && pThis->m_devinfoEx.wPID != APC_PID_HYPATIA)
    {
	    pThis->ChangeIRValue(pThis->m_irRange.first);

	    Sleep(1000);
    }
	{
		CAutoLock lock(pThis->m_previewParams.m_mutex);

        if ( pThis->m_previewParams.m_kcolorPreview.m_previewDlg )      ( ( CColorDlg* )pThis->m_previewParams.m_kcolorPreview.m_previewDlg )->GetImage(bufColor, widthColor, heightColor, snColor );
        else if ( pThis->m_previewParams.m_colorPreview.m_previewDlg  ) ( ( CColorDlg* )pThis->m_previewParams.m_colorPreview.m_previewDlg  )->GetImage(bufColor, widthColor, heightColor, snColor );
	}
    if ( !pThis->m_bIsInterLeaveMode && pThis->m_previewParams.m_kcolorOption == EOF && pThis->m_devinfoEx.wPID != APC_PID_HYPATIA)
    {
	    pThis->ChangeIRValue(irValue);

        Sleep(1000);
    }
#ifndef ESPDI_EG

	for (int i = 0; i < depthDatas.size(); i++)
	{
		int depthIndex = depthDatas.at(i).depthIndex;
		std::vector<unsigned char> bufDepth = depthDatas.at(i).bufDepth;
		int widthDepth = depthDatas.at(i).widthDepth;
		int heightDepth = depthDatas.at(i).heightDepth;
		int snDepth = depthDatas.at(i).snDepth;

		std::vector<unsigned char> bufColorRGB( widthColor * heightColor * 3 );

		//bgr to rgb
		if ( bufColor.size() )
		{
            int index = 0;
			for (int h = 0; h < heightColor; h++)
			{
				for (int w = 0; w < widthColor; w++)
				{
					bufColorRGB[ index++ ] = bufColor[ ( h * widthColor + w ) * 3 + 2 ];
					bufColorRGB[ index++ ] = bufColor[ ( h * widthColor + w ) * 3 + 1 ];
					bufColorRGB[ index++ ] = bufColor[ ( h * widthColor + w ) * 3     ];
				}
			}
		}
        //if ( pThis->IsDevicePid( APC_PID_8029 ) )
        //{
        //    APC_FlyingDepthCancellation_D8( pThis->m_hApcDI, &pThis->m_DevSelInfo, bufDepth.data(), widthDepth, heightDepth );
        //}
		if (bUseFilter && bufColorRGB.empty() == false)
		{
			//PlyFilter::CF_FILTER(bufDepth, bufColorRGB, widthDepth, heightDepth, widthColor, heightColor, imgFloatBufOut, pThis->m_previewParams.m_rectifyLogData[depthIndex]);
            if (depthImageType == APCImageType::DEPTH_8BITS)
			{
				//D8 TO D11 IMAGE +
				std::vector< BYTE > bufDepthTmpout;
				bufDepthTmpout.resize( bufDepth.size() * 2 );
				
                WORD* pDepthOut = ( WORD* )bufDepthTmpout.data();

				for ( size_t i = 0; i != bufDepth.size(); i++ )
	            {
		            pDepthOut[ i ] = ( ( WORD )bufDepth[ i ] ) << 3;
	            }
				//D8 TO D11 IMAGE -
				PlyFilter::CF_FILTER(bufDepthTmpout, bufColorRGB, widthDepth, heightDepth, widthColor, heightColor, imgFloatBufOut, pThis->m_previewParams.m_rectifyLogData[depthIndex]);
			}
			else if (depthImageType == APCImageType::DEPTH_11BITS)
			{
				PlyFilter::UnavailableDisparityCancellation(bufDepth, widthDepth, heightDepth, 16383);

				//if (pThis->IsDevicePid(APC_PID_8054))
				//{
				//	PlyFilter::CF_FILTER_8M(bufDepth, bufColorRGB, widthDepth, heightDepth, widthColor, heightColor, imgFloatBufOut, pThis->m_previewParams.m_rectifyLogData[depthIndex]);
				//}
				//else
				{
					PlyFilter::CF_FILTER(bufDepth, bufColorRGB, widthDepth, heightDepth, widthColor, heightColor, imgFloatBufOut, pThis->m_previewParams.m_rectifyLogData[depthIndex]);
				}
			}
			else if (depthImageType == APCImageType::DEPTH_14BITS)
			{
				PlyFilter::CF_FILTER_Z14(bufDepth, bufColorRGB, widthDepth, heightDepth, widthColor, heightColor, imgFloatBufOut, pThis->m_previewParams.m_rectifyLogData[depthIndex]);
			}
			if (imgFloatBufOut.empty())
			{
				AfxMessageBox(L"CF_FILTER does not support this module.");
				return 0;
			}
		}
		/*save depth yuv +*/
		std::ostringstream FileName_Depth;
		FileName_Depth << GetCurrentModuleFolder().c_str() << "\\Depth" << depthIndex << "_" << timestamp << "_" << treadID << ".yuv";
		if (bufDepth.empty() == false && widthDepth > 0 && heightDepth > 0)
		{
			pThis->SaveDepthYuv(bufDepth, depthImageType, widthDepth, heightDepth, FileName_Depth.str().c_str());
		}
		/*save depth yuv -*/

		/*save Depth gray bmp +*/
		//#6486//
		std::ostringstream Filename_Depth8BitGray;
		Filename_Depth8BitGray << GetCurrentModuleFolder().c_str() << "\\Depth" << depthIndex << "_gray_" << timestamp << "_" << treadID << "D" << depthIndex << ".bmp";
		pThis->SaveDepthGrayBmp(depthIndex, Filename_Depth8BitGray.str().c_str());

		/*save Depth gray bmp -*/

		/*save Depth color map bmp +*/
		std::ostringstream Depthcolormap_filename;
		Depthcolormap_filename << GetCurrentModuleFolder().c_str() << "\\Depthcolormap" << depthIndex << "_" << timestamp << "_" << treadID << "D" << depthIndex << ".bmp";
		pThis->SaveDepthColorBmp(depthIndex, Depthcolormap_filename.str().c_str());
		/*save Depth color map bmp -*/

		/*save ply +*/
		TRACE("save ply\n");

        if ((pThis->m_previewParams.m_rectifyLogData[depthIndex] != nullptr
            && bufDepth.empty() == false
            && widthDepth > 0
            && heightDepth > 0
            && bufColorRGB.empty() == false
            && widthColor > 0
            && heightColor > 0)
            || isDepthOnly(widthDepth, heightDepth, bufColorRGB.empty()))
        {
			/* EX8038: get baseline distance from UI instead of rectify log */
			//if (m_previewParams.m_camFocus > 0 && m_previewParams.m_baselineDist[i] > 0)
			//{
			//    m_previewParams.m_rectifyLogData[depthIndex]->ReProjectMat[14] = 1.0f / m_previewParams.m_baselineDist[i];
			//}

			//CAutoLock lock(pThis->m_previewParams.m_mutex);
			
			std::vector<CloudPoint> pointCloud; //dst for PlyWriter::etronFrameTo3D                   

            if ( pThis->IsDevicePid( APC_PID_8054 ) && pThis->m_previewParams.m_kcolorOption > EOF ) {
		        //int degreeOfRectifyLogK = 90; // default rotate once
		        //if (pThis->RotateImage()) {
		        //	degreeOfRectifyLogK = 180;
		        //}
		        if (bUseFilter) {
					PlyWriter::etronFrameTo3DMultiSensorPlyFilterFloat(widthDepth, heightDepth, imgFloatBufOut, widthColor, heightColor, bufColorRGB, pThis->m_previewParams.m_rectifyLogData[depthIndex], pThis->m_previewParams.m_rectifyLogDataSlave, depthImageType, pointCloud, true, zNear, zFar, false, false, 1.0f, 90);
				}
				else {
					PlyWriter::etronFrameTo3DMultiSensor(widthDepth, heightDepth, bufDepth, widthColor, heightColor, bufColorRGB, pThis->m_previewParams.m_rectifyLogData[depthIndex], pThis->m_previewParams.m_rectifyLogDataSlave, depthImageType, pointCloud, true, zNear, zFar, true, false, 1.0f, 90);
				}
	        }
            else if ( pThis->IsDevicePid( APC_PID_8040S ) )
            { // not support CF_FILTER current
                if ( pThis->m_previewParams.m_kcolorOption > EOF )
                    PlyWriter::etronFrameTo3DCylinder(widthDepth, heightDepth, bufDepth, widthColor, heightColor, bufColorRGB, pThis->m_previewParams.m_rectifyLogData[depthIndex], pThis->m_previewParams.m_rectifyLogDataSlave, depthImageType, pointCloud, true, zNear, zFar, false, 1.0f);
				else PlyWriter::etronFrameTo3DCylinder(widthDepth, heightDepth, bufDepth, widthColor, heightColor, bufColorRGB, pThis->m_previewParams.m_rectifyLogData[depthIndex], depthImageType, pointCloud, true, zNear, zFar, false, 1.0f);
			}
			else if (bUseFilter) {
				PlyWriter::etronFrameTo3D_PlyFilterFloat(widthDepth, heightDepth, imgFloatBufOut, widthColor, heightColor, bufColorRGB, pThis->m_previewParams.m_rectifyLogData[depthIndex], depthImageType, pointCloud, true, zNear, zFar, true, false, 1.0f);
			}
            else if ( pThis->IsDevicePid( APC_PID_8029 ) )
            {
                PlyWriter::etronFrameTo3D_8029(widthDepth, heightDepth, bufDepth, widthColor, heightColor, bufColorRGB, pThis->m_previewParams.m_rectifyLogData[depthIndex], depthImageType, pointCloud, true, zNear, zFar, true, false, 1.0f);
            }
			else {
                if (isDepthOnly(widthDepth, heightDepth, bufColorRGB.empty())) // For the depth only
                {
                    std::vector<unsigned char> bufOneColor(widthDepth * heightDepth * 3);
                    for (int i = 0; i < (widthDepth * heightDepth * 3); i += 3)
                    {
                        bufOneColor[i] = 0x0;
                        bufOneColor[i + 1] = 0xFF;
                        bufOneColor[i + 2] = 0x0;
                    }
                    PlyWriter::etronFrameTo3D(widthDepth, heightDepth, bufDepth, widthDepth, heightDepth, bufOneColor, pThis->m_previewParams.m_rectifyLogData[depthIndex], depthImageType, pointCloud, true, zNear, zFar, true, true, 1.0f);
                }
                else  // For other mode.
                {
                    PlyWriter::etronFrameTo3D(widthDepth, heightDepth, bufDepth, widthColor, heightColor, bufColorRGB, pThis->m_previewParams.m_rectifyLogData[depthIndex], depthImageType, pointCloud, true, zNear, zFar, true, false, 1.0f);
                }
			}

			std::ostringstream filename;
			filename << GetCurrentModuleFolder().c_str() << "\\cloud" << "_" << timestamp << "_" << treadID << "D" << depthIndex << ".ply";

			if (PlyWriter::writePly(pointCloud, filename.str().c_str()) == 0)
			{
				if (ViewWithMeshLab(filename.str()) == 0)
                {
                    stringResult.Empty();
                }
				//	stringResult.Format(_T("Saving the snapshot to the file path '%s'. Opening with meshlab..."), filename.str().c_str());
				//else
				//	stringResult.Format(_T("Saving the snapshot to the file path '%s'.Please open ply file with meshlab"), filename.str().c_str());
			}
		}
		else
		{
			if (pThis->m_DevType == PUMA || pThis->m_DevType == PLUM)
				stringResult = "Get the rectify log data from flash for eSP876 failed !";
			else if (pThis->m_DevType == AXES1)
				stringResult = "Get the rectify log data from flash for eSP870 failed !";
			else
				stringResult = "Get the rectify log data from flash for unknown IC failed !";
		}
		TRACE("save ply end\n");
		/*save ply end -*/
			
	}
	/*save DepthFUsion bmp +*/
	pThis->DepthFusionBmp(depthImageType);
	/*save DepthFUsion bmp -*/
#endif
	/*save color bmp +*/
	std::ostringstream FileName_Colorbmp;
	FileName_Colorbmp << GetCurrentModuleFolder().c_str() << "\\Color" << "_" << timestamp << "_" << treadID << ".bmp";
	if (bufColor.empty() == false && widthColor > 0 && heightColor)
	{
        SaveImage(bufColor, widthColor, heightColor, 24, FileName_Colorbmp.str().c_str(), false, Gdiplus::ImageFormatBMP);
	}
    
	/*save color bmp -*/
	if ( !stringResult.IsEmpty() ) {
		AfxMessageBox(stringResult);
    }
    pThis->PostMessage( WM_MSG_SNAPSHOT_COMPLETE );

	return 0;
}

LRESULT CPreviewImageDlg::OnSnapshot(WPARAM wParam, LPARAM lParam)
{
	LPTHREAD_PARAM lpParam = new THREAD_PARAM;
	lpParam->pDlg = this;
	lpParam->irValue = m_previewParams.m_IrPreState;
	lpParam->zFar  = m_ZFar  ? m_ZFar  : 1000;
    lpParam->zNear = m_ZNear ? m_ZNear : 0;
    lpParam->bUseFilter = ( ( ( CButton* )GetDlgItem( IDC_CHK_PLY_FILTER ) )->GetCheck() == BST_CHECKED );

	AfxBeginThread(CPreviewImageDlg::DoSnapshot, (LPVOID)lpParam, THREAD_PRIORITY_ABOVE_NORMAL);

    m_pWaitDlg->SetText( L"Snapshot and Ply, please wait......" );
    m_pWaitDlg->DoModal();

    return 0;
}

void CPreviewImageDlg::OnBnClickedCheckTColorStream()
{
	OnCbnSelchangeComboTColorStream();
}

void CPreviewImageDlg::OnBnClickedCheckColorStream()
{
    OnCbnSelchangeComboColorStream();
}

void CPreviewImageDlg::OnBnClickedCheck360mode()
{
    CComboBox* pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_COLOR_OUTPUT_CTRL);
    if (((CButton*)GetDlgItem(IDC_CHECK_360MODE))->GetCheck() == BST_CHECKED)
    {
        pComboBox->SetCurSel(1);
        pComboBox->EnableWindow(FALSE);
    }
    else
    {
        pComboBox->EnableWindow(TRUE);
    }
}

int CPreviewImageDlg::EnableDenoise(bool enable)
{
    unsigned short value = 0; 
    int ret = APC_GetHWRegister(m_hApcDI, &m_DevSelInfo, 0xf101, &value, FG_Address_2Byte | FG_Value_1Byte);
    if(ret != APC_OK)
    {
        return ret;
    }

    if (enable)
    {
        BIT_CLEAR(value, 2);
    }
    else
    {
        BIT_SET(value, 2);
    }

    return APC_SetHWRegister(m_hApcDI, &m_DevSelInfo, 0xf101, value, FG_Address_2Byte | FG_Value_1Byte);
}

int CPreviewImageDlg::InitEysov(LONG& outWidth, LONG& outHeight)
{
#ifdef ESPDI_EG
    PARALUT paraLut;
    int ret = APC_GetLutParam(m_hApcDI, &m_DevSelInfo, CurColorOption(), &paraLut);
    if (ret == APC_OK)
    {
        //enable dewarp stitch
        ret = APC_GenerateLut(m_hApcDI, &m_DevSelInfo, &paraLut);
        if (ret == APC_OK)
        {
            outWidth = (LONG)paraLut.out_img_cols;
            outHeight = (LONG)paraLut.out_img_rows;
        }
    }

    return ret;
#else
    return APC_DEVICE_NOT_SUPPORT;
#endif
}

CPreviewImageDlg::CPreviewItem::CPreviewItem()
    : m_previewDlg(nullptr)
{
}

CPreviewImageDlg::CPreviewItem::~CPreviewItem()
{
    Reset(nullptr);
}

void CPreviewImageDlg::CPreviewItem::Reset(CDialog* pCallerDlg)
{
    if (m_previewDlg != nullptr)
    {
        if (m_previewDlg != pCallerDlg)
        {
            m_previewDlg->SendMessage(WM_CLOSE);
        }
        delete m_previewDlg;
        m_previewDlg = nullptr;
    }
}

void CPreviewImageDlg::CPreviewItem::SetPreviewDlg(CDialog* pDlg)
{
    if (m_previewDlg != nullptr)
    {
        delete m_previewDlg;
    }

    m_previewDlg = pDlg;
}


CPreviewImageDlg::CPreviewParams::CPreviewParams()
{
    Reset(nullptr);
}

CPreviewImageDlg::CPreviewParams::~CPreviewParams()
{
    Reset(nullptr);
}

void CPreviewImageDlg::CPreviewParams::Reset(CDialog* pCallerDlg)
{
    CAutoLock lock(m_mutex);

    m_colorPreview.Reset(pCallerDlg);
    for (int i = 0; i < APC_MAX_DEPTH_STREAM_COUNT; ++i)
    {
        m_depthPreview[i].Reset(pCallerDlg);
    }
    m_depthFusionPreview.Reset(pCallerDlg);
    m_fusionSelectedTargetPreview.Reset(pCallerDlg);    
	m_depthInforPreview.Reset(pCallerDlg);
	m_trackPreview.Reset(pCallerDlg);
	m_kcolorPreview.Reset(pCallerDlg);

    m_colorOption = -1;
    m_depthOption = -1;
    m_depthSwitch = 0;
    m_IsDepthFusionOn = false;
    m_rectifyData = true;
    m_360ModeEnabled = false;
    m_depthType = APC_DEPTH_DATA_11_BITS;
    m_camFocus = 0.0;
    m_baselineDist = std::vector<float>(APC_MAX_DEPTH_STREAM_COUNT, 0.0);
    m_showFusionSelectedDlg = false;
    for (int i = 0; i < APC_MAX_DEPTH_STREAM_COUNT; ++i)
    {
        if (m_rectifyLogData[i] != nullptr)
        {
            free(m_rectifyLogData[i]);
            m_rectifyLogData[i] = nullptr;
        }
    }
}

void CPreviewImageDlg::PrepareRectifyLogData(eSPCtrl_RectLogData*& pRectifyLog, int index)
{
    int nRet;

    pRectifyLog = (eSPCtrl_RectLogData*)malloc(sizeof(eSPCtrl_RectLogData));
    if (pRectifyLog == NULL)
    {
        TRACE("No memory allocated !");
        return;
    }		

    nRet = getRectifyLogData(pRectifyLog, index);
    if (nRet != APC_OK)
    {
        memset(pRectifyLog, 0, sizeof(eSPCtrl_RectLogData));
        TRACE("Get 'the rectify log data' from flash failed !");
    }
}

void CPreviewImageDlg::UpdatePreviewParams()
{
    CAutoLock lock(m_previewParams.m_mutex);

    m_ZFar  = GetDlgItemInt( IDC_EDIT_ZFAR  );
    m_ZNear = GetDlgItemInt( IDC_EDIT_ZNEAR );

    m_previewParams.Reset(this);

    m_previewParams.m_360ModeEnabled = ((CButton*)GetDlgItem(IDC_CHECK_360MODE))->GetCheck() == BST_CHECKED;

    if (((CButton*)GetDlgItem(IDC_CHECK_COLOR_STREAM))->GetCheck() == BST_CHECKED)
    {
        m_previewParams.m_colorOption = ((CComboBox*)GetDlgItem(IDC_COMBO_COLOR_STREAM))->GetCurSel();
    }
    else m_previewParams.m_colorOption = -1;

    if (((CButton*)GetDlgItem(IDC_CHECK_K_COLOR_STREAM))->GetCheck() == BST_CHECKED)
    {
		m_previewParams.m_kcolorOption = ((CComboBox*)GetDlgItem(IDC_COMBO_K_COLOR_STREAM))->GetCurSel();
	}
    else m_previewParams.m_kcolorOption = -1;
	
    if (((CButton*)GetDlgItem(IDC_CHECK_T_COLOR_STREAM))->GetCheck() == BST_CHECKED)
    {
        m_previewParams.m_tcolorOption = ((CComboBox*)GetDlgItem(IDC_COMBO_T_COLOR_STREAM))->GetCurSel();	
	}
	else m_previewParams.m_tcolorOption = -1;

    m_previewParams.m_IsDepthFusionOn = (((CButton*)GetDlgItem(IDC_CHECK_DEPTH_FUSION))->GetCheck() == BST_CHECKED);
	
    CPoint ptRes = m_previewParams.m_colorOption > EOF ? CurColorImgRes() : CurKColorImgRes();

	if ( IsDevicePid( APC_PID_8054 ) && ( ( CButton* )GetDlgItem( IDC_CHECK_K_COLOR_STREAM ) )->GetCheck() == BST_CHECKED ) {
		int nRet;
		int index;
        if ( m_previewParams.m_rectifyLogDataSlave ) free( m_previewParams.m_rectifyLogDataSlave );

		m_previewParams.m_rectifyLogDataSlave = (eSPCtrl_RectLogData*)malloc(sizeof(eSPCtrl_RectLogData));

		if (((CComboBox*)GetDlgItem(IDC_COMBO_DEPTH_STREAM))->GetCurSel() == 3)
			index = 1;  
		else
			index = 0;

		nRet = getRectifyLogDataSlave(m_previewParams.m_rectifyLogDataSlave, index);
		if (nRet != APC_OK)
		{
			free(m_previewParams.m_rectifyLogDataSlave);
			m_previewParams.m_rectifyLogDataSlave = nullptr;
			TRACE("Get 'the slave rectify log data' from flash failed !");
		}
		m_previewParams.m_rectifyLogDataSlave->ReProjectMat[11] = m_previewParams.m_rectifyLogDataSlave->NewCamMat2[0];
	}
    if ( IsDevicePid( APC_PID_8040S ) && ( ( CButton* )GetDlgItem( IDC_CHECK_K_COLOR_STREAM ) )->GetCheck() == BST_CHECKED )
    {
        int index = EOF;
        const int iSelectK = ( ( CComboBox* )GetDlgItem( IDC_COMBO_K_COLOR_STREAM ) )->GetCurSel();

        if ( iSelectK < m_kcolorStreamOptionCount )
        {
            if ( 2560 == m_pStreamKColorInfo[ iSelectK ].nWidth && 1216 == m_pStreamKColorInfo[ iSelectK ].nHeight ) index = 0;
            if ( 1920 == m_pStreamKColorInfo[ iSelectK ].nWidth && 912  == m_pStreamKColorInfo[ iSelectK ].nHeight ) index = 1;
            if ( 3840 == m_pStreamKColorInfo[ iSelectK ].nWidth && 1824 == m_pStreamKColorInfo[ iSelectK ].nHeight ) index = 2;
        }
        if ( index > EOF )
        {
            if ( m_previewParams.m_rectifyLogDataSlave ) free( m_previewParams.m_rectifyLogDataSlave );

		    m_previewParams.m_rectifyLogDataSlave = (eSPCtrl_RectLogData*)malloc(sizeof(eSPCtrl_RectLogData));

		    if (getRectifyLogDataSlave(m_previewParams.m_rectifyLogDataSlave, index) != APC_OK)
		    {
			    free(m_previewParams.m_rectifyLogDataSlave);
			    m_previewParams.m_rectifyLogDataSlave = nullptr;
			    TRACE("Get 'the slave rectify log data' from flash failed !");
		    }
        }
	}

    if (IsDevicePid(APC_PID_8063) && ((CButton*)GetDlgItem(IDC_CHECK_K_COLOR_STREAM))->GetCheck() == BST_CHECKED)
    {
        int nRet;
        int index = 0;
        if (m_previewParams.m_rectifyLogDataSlave) free(m_previewParams.m_rectifyLogDataSlave);

        m_previewParams.m_rectifyLogDataSlave = (eSPCtrl_RectLogData*)malloc(sizeof(eSPCtrl_RectLogData));

        nRet = getRectifyLogDataSlave(m_previewParams.m_rectifyLogDataSlave, index);
        if (nRet != APC_OK)
        {
            free(m_previewParams.m_rectifyLogDataSlave);
            m_previewParams.m_rectifyLogDataSlave = nullptr;
            TRACE("Get 'the slave rectify log data' from flash failed !");
        } else {
            memcpy(m_xPointCloudInfo.CamMat1, m_previewParams.m_rectifyLogDataSlave->CamMat1, 9 * sizeof(float));
            memcpy(m_xPointCloudInfo.RotaMat, m_previewParams.m_rectifyLogDataSlave->RotaMat, 9 * sizeof(float));
            memcpy(m_xPointCloudInfo.TranMat, m_previewParams.m_rectifyLogDataSlave->TranMat, 3 * sizeof(float));
        }
    }

    if (((CButton*)GetDlgItem(IDC_CHECK_DEPTH0))->GetCheck() == BST_CHECKED)
    {
		int index = 0;

		m_previewParams.m_depthSwitch |= ApcDIDepthSwitch::Depth0;

		if ( IsDevicePid( APC_PID_8038 ) ) index = 0; // EX8038: D0 is for 3cm baseline

        else if ( IsDevicePid( APC_PID_8040S ) )
        {
            index = ( ( ( CButton* )GetDlgItem( IDC_CHECK_K_COLOR_STREAM ) )->GetCheck() == BST_CHECKED ) ? 1 : 0;
        }
        else index = ((CComboBox*)GetDlgItem(IDC_COMBO_DEPTH_STREAM))->GetCurSel();

        PrepareRectifyLogData(m_previewParams.m_rectifyLogData[0], index);
    }

    if (((CButton*)GetDlgItem(IDC_CHECK_DEPTH1))->GetCheck() == BST_CHECKED)
    {
        int index;

        m_previewParams.m_depthSwitch |= ApcDIDepthSwitch::Depth1;
        if ( IsDevicePid( APC_PID_8038 ) )
       	    index = 1; // EX8038: D1 is for 6cm baseline
       	else if ( IsDevicePid( APC_PID_8040S ) )
       	    index = 1;
        else if ( IsDevicePid( APC_PID_8054 ) && ((CComboBox*)GetDlgItem(IDC_COMBO_DEPTH_STREAM))->GetCurSel() == 3 )
			index = 2; // EX8054: D1 640*640 is from 1280*1280 scale down
        else if ( ( IsDevicePid( APC_PID_8051 ) || IsDevicePid( APC_PID_8062 ) ) && USB_PORT_TYPE_2_0 == m_eUSB_Port_Type )
       	    index = 0; // EX8051 & YX8062: U2 is Binning mode
        else if (IsDevicePid(APC_PID_8036) && ((CComboBox*)GetDlgItem(IDC_COMBO_DEPTH_STREAM))->GetCurSel() == 1)
        {
#if 0 //REFINE_DATABASE_CODE
            index = GetTableIndex(); // EX8036: D1 640*360 is from 1280*720 scale down
#else
            index = 0; // EX8036: D1 640*360 is from 1280*720 scale down
#endif
        }
        else
            index = ((CComboBox*)GetDlgItem(IDC_COMBO_DEPTH_STREAM))->GetCurSel();

        PrepareRectifyLogData(m_previewParams.m_rectifyLogData[1], index);
    }

    if (((CButton*)GetDlgItem(IDC_CHECK_DEPTH2))->GetCheck() == BST_CHECKED)
    {
        m_previewParams.m_depthSwitch |= ApcDIDepthSwitch::Depth2; 
        PrepareRectifyLogData(m_previewParams.m_rectifyLogData[2], 2); // EX8038: D2 is for 15cm baseline
    }

    if (m_previewParams.m_depthSwitch != 0)
    {
        m_previewParams.m_depthOption = ((CComboBox*)GetDlgItem(IDC_COMBO_DEPTH_STREAM))->GetCurSel();
    }
    else m_previewParams.m_depthOption = EOF;

    CComboBox* pDepthBitCbx = (CComboBox*)GetDlgItem(IDC_COMBO_DEPTH_BIT_SEL_CTRL);

    m_previewParams.m_rectifyData = (((CButton*)GetDlgItem(IDC_RADIO_RECTIFY_DATA))->GetCheck() == BST_CHECKED);

#if 0 // REFINE_DATABASE_CODE
    m_previewParams.m_depthType = GetVideoModeFWValue(); 
#else
    m_previewParams.m_depthType = AdjDepthBitIndex(pDepthBitCbx->GetCurSel() > EOF ? pDepthBitCbx->GetItemData(pDepthBitCbx->GetCurSel()) : NULL);
#endif
    m_xPointCloudInfo.wDepthType = m_previewParams.m_depthType;
    SetFilterParam( *m_DfParam );

    if ( IsDevicePid( APC_PID_8040S ) )
    {
        if ( 2160 == ptRes.x && 1920 == ptRes.y )
        {
            ptRes.x = 1920;
            ptRes.y = 1080;
        }
        else
        {
            ptRes.x = 1920;
            ptRes.y = 912;
        }
    }
    for ( int i = 0; i < 2; i++ )
    {
        if ( m_previewParams.m_rectifyLogData[ i ] )
        {
            if ( IsDevicePid( APC_PID_8040S ) && ( ( CButton* )GetDlgItem( IDC_CHECK_K_COLOR_STREAM ) )->GetCheck() == BST_UNCHECKED )
            {
                m_previewParams.m_rectifyLogData[ i ]->ReProjectMat[11] = (m_previewParams.m_rectifyLogData[i]->OutImgHeight / 2) / tan(72/2 * 3.1415926 / 180);
            }

            if (   ptRes.x == 0 // is the depth only mode ?
                && ptRes.y == 0
                && m_pStreamDepthInfo[m_previewParams.m_depthOption].nWidth  > 0
                && m_pStreamDepthInfo[m_previewParams.m_depthOption].nHeight > 0
                )
            {
                ptRes.x = m_pStreamDepthInfo[m_previewParams.m_depthOption].nWidth;
                ptRes.y = m_pStreamDepthInfo[m_previewParams.m_depthOption].nHeight;
            }

            const float ratio_Mat = (float)ptRes.y / m_previewParams.m_rectifyLogData[i]->OutImgHeight;      
	        const float baseline  = 1.0f / m_previewParams.m_rectifyLogData[i]->ReProjectMat[14];
	        const float diff      = m_previewParams.m_rectifyLogData[i]->ReProjectMat[15] * ratio_Mat;
            m_xPointCloudInfo.centerX       = -1.0f*m_previewParams.m_rectifyLogData[i]->ReProjectMat[3] * ratio_Mat;
	        m_xPointCloudInfo.centerY       = -1.0f*m_previewParams.m_rectifyLogData[i]->ReProjectMat[7] * ratio_Mat;
	        m_xPointCloudInfo.focalLength   = m_previewParams.m_rectifyLogData[i]->ReProjectMat[11] * ratio_Mat;

            switch ( APCImageType::DepthDataTypeToDepthImageType( m_previewParams.m_depthType ) )
            {
            case APCImageType::DEPTH_14BITS: m_xPointCloudInfo.disparity_len = 0; break;
            case APCImageType::DEPTH_11BITS:
                {
                    m_xPointCloudInfo.disparity_len = 2048;

                    for (int i = 0; i < m_xPointCloudInfo.disparity_len; i++) { m_xPointCloudInfo.disparityToW[i] = ( i * ratio_Mat / 8.0f ) / baseline + diff; }
                }
                break;
        
            default:
                {
                    m_xPointCloudInfo.disparity_len = 256;

                    for (int i = 0; i < m_xPointCloudInfo.disparity_len; i++)
                    {
                        m_xPointCloudInfo.disparityToW[i] = IsDevicePid( APC_PID_8029 ) ? ( ( i * ratio_Mat ) / baseline + diff ) * 0.5f :
                                                                                              ( i * ratio_Mat ) / baseline + diff;
                    }
                }
                break;
            }
        }
    }
    if ( m_previewParams.m_rectifyLogDataSlave && !IsDevicePid(APC_PID_8063))
    {
        float ratio_Mat_K = (float)ptRes.y / m_previewParams.m_rectifyLogDataSlave->OutImgHeight;
        m_xPointCloudInfo.focalLength_K = m_previewParams.m_rectifyLogDataSlave->ReProjectMat[11] * ratio_Mat_K;
	    m_xPointCloudInfo.baseline_K = 1.0f / m_previewParams.m_rectifyLogDataSlave->ReProjectMat[14];
	    m_xPointCloudInfo.diff_K = m_previewParams.m_rectifyLogDataSlave->ReProjectMat[15] * ratio_Mat_K;
    }
    auto getFloatValue = [this](UINT UIItemId) -> float
    {
        if (GetDlgItem(UIItemId)->IsWindowEnabled())
        {
            CString focus;
            GetDlgItemText(UIItemId, focus);
            return std::stof(std::string(CW2A(focus)));
        }       
        return 0.0;
    };
    m_previewParams.m_camFocus = getFloatValue(IDC_EDIT_CAM_FOCUS);
    m_previewParams.m_baselineDist[0] = getFloatValue(IDC_EDIT_BLDIST0);
    m_previewParams.m_baselineDist[1] = getFloatValue(IDC_EDIT_BLDIST1);
    m_previewParams.m_baselineDist[2] = getFloatValue(IDC_EDIT_BLDIST2);
}

/// **************************************** ///
/// 影像 Callback Entry						 ///
/// Jacky_Tag								 ///
/// **************************************** ///
void CPreviewImageDlg::ImgCallback(APCImageType::Value imgType, int imgId, unsigned char* imgBuf, int imgSize,
    int width, int height, int serialNumber, void* pParam)
{
    CPreviewImageDlg* pThis = (CPreviewImageDlg*)pParam;

	std::vector<unsigned char> vecImgBuf(imgSize);
	memcpy(&vecImgBuf[0], imgBuf, imgSize);
	if (FrameSyncManager::GetInstance()->IsEnable())
	{
		FrameSyncManager::GetInstance()->SyncImageCallback(pThis->m_hApcDI, pThis->m_DevSelInfo,
														  imgType, imgId, serialNumber,
													      std::bind(&CPreviewImageDlg::ProcessImgCallback, pThis, imgType, imgId, std::move(vecImgBuf), imgSize, width, height, serialNumber));
	}
	else
	{
		pThis->ProcessImgCallback(imgType, imgId, std::move(vecImgBuf), imgSize, width, height, serialNumber);
	}
}

void CPreviewImageDlg::ProcessImgCallback(APCImageType::Value imgType, int imgId, std::vector<unsigned char> imgBuf, int imgSize,
	int width, int height, int serialNumber)
{
	CAutoLock lock(m_previewParams.m_mutex);
#ifndef ESPDI_EG
	auto PointCloud_ApplyImage = [&](CColorDlg* pDlg)
	{
		std::vector<unsigned char> bufColor;
		int widthColor = 0;
		int heightColor = 0;
		int snColor = -1;

		pDlg->GetImage(bufColor, widthColor, heightColor, snColor);

		m_frameGrabber->UpdateFrameData(FrameGrabber::FRAME_POOL_INDEX_COLOR, snColor, &bufColor[0], bufColor.size());
	};
#endif
	if (APCImageType::IsImageColor(imgType))
	{
		//debug log for serial-count-interval
		//m_log.push_back( { CTime::GetTickCount().GetTime(),  serialNumber } );
		//if ( m_log.size() > 500 ) m_log.pop_front();
		m_mapColorStreamTimeStamp[imgId] = time(NULL);

		if (m_bIsInterLeaveMode)
		{
			static int LastIndex = NULL;

			if (serialNumber != LastIndex + 2)
			{
				LastIndex = serialNumber;
				return;
			}
			LastIndex = serialNumber;
		}
		if (imgId == APC_Stream_Color)
		{
			m_imgSerialNumber = serialNumber;

			CColorDlg* pDlg = (CColorDlg*)m_previewParams.m_colorPreview.m_previewDlg;
			if (pDlg != nullptr)
			{
				pDlg->ApplyImage(&imgBuf[0], &imgSize, imgType == APCImageType::COLOR_RGB24, imgType == APCImageType::COLOR_MJPG, serialNumber, imgType);
#ifndef ESPDI_EG
				if (m_frameGrabber) PointCloud_ApplyImage(pDlg);

				if (m_depthFusionHelper != nullptr && imgType == APCImageType::COLOR_YUY2)
				{
					m_depthFusionHelper->UpdateColorData(serialNumber, &imgBuf[0], imgSize);
				}
#endif
			}
		}
		else if (imgId == APC_Stream_Track)
		{
			CColorDlg* pTrackDlg = (CColorDlg*)m_previewParams.m_trackPreview.m_previewDlg;
			if (pTrackDlg)
			{
				pTrackDlg->ApplyImage(&imgBuf[0], &imgSize, imgType == APCImageType::COLOR_RGB24, imgType == APCImageType::COLOR_MJPG, serialNumber, imgType);
			}
		}
		else if (imgId == APC_Stream_Kolor)
		{
			CColorDlg* pKcolorDlg = (CColorDlg*)m_previewParams.m_kcolorPreview.m_previewDlg;
			if (pKcolorDlg)
			{
				pKcolorDlg->ApplyImage(&imgBuf[0], &imgSize, imgType == APCImageType::COLOR_RGB24, imgType == APCImageType::COLOR_MJPG, serialNumber, imgType);
#ifndef ESPDI_EG
				if (m_frameGrabber) PointCloud_ApplyImage(pKcolorDlg);
#endif
			}
		}
	}
	else if (APCImageType::IsImageDepth(imgType))
	{
		if (imgId == -1)// depth fusion
		{
			static CRect rtEmpty(0, 0, 0, 0);
			CDepthDlg* pDlg = (CDepthDlg*)m_previewParams.m_depthFusionPreview.m_previewDlg;
			if (!pDlg) return;

			pDlg->UpdateFusionSelectedIndex(m_previewParams.m_depthFusionSelectedIndex);// need update selected index before apply image
			CAutoLock lock(m_previewParams.m_mutex);

			BYTE *pBuf = &imgBuf[0];
			pDlg->ApplyImage(&pBuf, imgSize, serialNumber, rtEmpty, *m_DfParam);

			//        if (m_previewParams.m_fusionSelectedTargetPreview.m_previewDlg != nullptr)
			//        {
						//unsigned char *tmpImgBuf = &m_previewParams.m_fusionTargetRgbImgBuf[0];
						//int tmpImgBufSize = m_previewParams.m_fusionTargetRgbImgBuf.size();
						//((CColorDlg*)m_previewParams.m_fusionSelectedTargetPreview.m_previewDlg)->ApplyImage( tmpImgBuf, &tmpImgBufSize, TRUE, FALSE, serialNumber );

						//m_previewParams.m_fusionTargetRgbImgBuf.resize(tmpImgBufSize);
						//m_previewParams.m_fusionTargetRgbImgBuf.assign(tmpImgBuf, tmpImgBuf + tmpImgBufSize);
			//        }
		}
		else
		{
#ifndef ESPDI_EG
			m_mapDepthStreamTimeStamp[imgId] = time(NULL);

			if (m_bIsInterLeaveMode)
			{
				static int LastIndex = NULL;

				if (serialNumber != LastIndex + 2)
				{
					LastIndex = serialNumber;
					return;
				}
				LastIndex = serialNumber;
			}
			if ((CDepthDlg*)m_previewParams.m_depthPreview[imgId].m_previewDlg)
			{
				{
					CAutoLock lock(m_previewParams.m_mutex);
					BYTE *pBuf = &imgBuf[0];
					if (IsDevicePid(APC_PID_8038))
					{
						static CRect rtEmpty(0, 0, 0, 0);
						((CDepthDlg*)m_previewParams.m_depthPreview[imgId].m_previewDlg)->ApplyImage(&pBuf, imgSize, serialNumber, m_i8038DepthIndex == imgId ? m_pAccuracyDlg->GetAccuracyRegion() : rtEmpty, *m_DfParam);
					}
					else ((CDepthDlg*)m_previewParams.m_depthPreview[imgId].m_previewDlg)->ApplyImage(&pBuf, imgSize, serialNumber, m_pAccuracyDlg->GetAccuracyRegion(), *m_DfParam);
				}
				if (m_pAccuracyDlg)
				{
					if (IsDevicePid(APC_PID_8040S))
					{
						width ^= height;
						height ^= width;
						width ^= height;
					}
					if (IsDevicePid(APC_PID_8038))
					{
						if (m_i8038DepthIndex == imgId)
						{
							m_pAccuracyDlg->UpdateDepthMap(width, height, (CDepthDlg*)m_previewParams.m_depthPreview[imgId].m_previewDlg);
						}
					}
					else m_pAccuracyDlg->UpdateDepthMap(imgType == APCImageType::DEPTH_8BITS ? width * 2 : width, height, (CDepthDlg*)m_previewParams.m_depthPreview[imgId].m_previewDlg);
				}
			}
			if (m_frameGrabber)
			{
				if (IsDevicePid(APC_PID_8038)) {
					if (imgId == 1) m_frameGrabber->UpdateFrameData(FrameGrabber::FRAME_POOL_INDEX_DEPTH, serialNumber, &imgBuf[0], imgSize);
				}
				else {
					m_frameGrabber->UpdateFrameData(FrameGrabber::FRAME_POOL_INDEX_DEPTH, serialNumber, &imgBuf[0], imgSize);
				}
			}
			if (m_previewParams.m_IsDepthFusionOn)
			{
				m_depthFusionHelper->UpdateDepthData(imgId, serialNumber, &imgBuf[0], imgSize);
			}
#endif	
		}
	}
	else
	{
		OutputDebugString(L"image callback failed. unknown image type.\n");
	}
}

void GenerateFusionTargetRgbImg(const unsigned char* selectedIndex, int width, int height, 
    std::vector<unsigned char>& rgbImgBuf)
{
    // {b, g, r}
    static const unsigned char previewColor[][3] = { { 0, 0, 255 }, { 0, 255, 0 }, { 255, 0, 0 },
        { 255, 255, 0 }, { 255, 0, 255 }, { 0, 255, 255 }, { 0, 128, 255 }, { 255, 255, 255 } };

    if (width * height * 3 != rgbImgBuf.size())
    {
        rgbImgBuf.resize(width * height * 3, 0);
    }

    //rgb buffer to dc needs bottom-up
    unsigned char* buf = &rgbImgBuf[0];
    for (int y = 0, bottom0 = width * (height - 1); y < height; ++y)
    {
        int base = y * width;
        for (int x = 0; x < width; ++x)
        {
            if (selectedIndex[base + x] != 255)
            {
                const unsigned char* color = previewColor[selectedIndex[base + x]];
                memcpy(&buf[(bottom0 - base + x) * 3], color, 3);
            }
            else
            {
                memset(&buf[(bottom0 - base + x) * 3], 0, 3);
            }
        }
    }
}

void CPreviewImageDlg::DepthFusionCallback(unsigned char* depthBuf, unsigned char* selectedIndex, int depthSize, int width, int height,
    int serialNumber, void* pParam)
{
    CPreviewImageDlg* pThis = (CPreviewImageDlg*)pParam;
    
    const int pixelCount = width * height;

    //generate fusion target preview img
    std::vector<unsigned char> fusionTargetImgBuf(pixelCount * 3, 0);
    GenerateFusionTargetRgbImg(selectedIndex, width, height, fusionTargetImgBuf);

    {
        CAutoLock lock(pThis->m_previewParams.m_mutex);
        if (pThis->m_previewParams.m_depthFusionSelectedIndex.size() != pixelCount)
        {
            pThis->m_previewParams.m_depthFusionSelectedIndex.resize(pixelCount);
        }
        memcpy(&pThis->m_previewParams.m_depthFusionSelectedIndex[0], selectedIndex, pixelCount);

        if (pThis->m_previewParams.m_fusionTargetRgbImgBuf.size() != pixelCount * 3)
        {
            pThis->m_previewParams.m_fusionTargetRgbImgBuf.resize(pixelCount * 3, 0);
        }
        memcpy(&pThis->m_previewParams.m_fusionTargetRgbImgBuf[0], &fusionTargetImgBuf[0], pixelCount * 3);
    }

    CPreviewImageDlg::ImgCallback(APCImageType::DEPTH_11BITS, -1, depthBuf, depthSize, width, height, serialNumber, pParam);
}

void CPreviewImageDlg::FrameGrabberCallback( BOOL isDepthOnly,
                                             std::vector<unsigned char>& bufDepth,
                                             int widthDepth,
                                             int heightDepth,
                                             std::vector<unsigned char>& bufColor,
                                             int widthColor,
                                             int heightColor,
                                             int serialNumber,
                                             void* pParam)
{
	CPreviewImageDlg* pThis = (CPreviewImageDlg*)pParam;

	if (pThis->m_previewParams.m_pointCloudViewer == nullptr)
	{
		return;
	}
    if ( pThis->m_bPCV_SingleColor )
    {
        const UINT stride = widthColor * 3;

        static std::vector< BYTE > vecGreenRGB;

        if ( stride != vecGreenRGB.size() )
        {
            vecGreenRGB.resize( stride );

            const BYTE GreenRGB[] = { 0, 255, 0 };

            for ( int i = NULL; i < widthColor; i++ ) memcpy( &vecGreenRGB[ i * 3 ], GreenRGB , 3 );
        }
        for ( int i = NULL; i < heightColor; i++ ) memcpy( &bufColor[ i * stride ], vecGreenRGB.data(), stride );
    }
    else if ( pThis->m_bPCV_NoColorStream )
    {
        for ( int i = NULL; i < APC_MAX_DEPTH_STREAM_COUNT; i++ )
        {
            if ( !pThis->m_previewParams.m_depthPreview[ i ].m_previewDlg ) continue;

            BYTE* pBufferDepthToColor = ( ( CDepthDlg* )pThis->m_previewParams.m_depthPreview[ i ].m_previewDlg )->GetDepthColorMapImage();

            if ( pBufferDepthToColor )
            {
                const UINT stride = widthDepth * 3;

                if ( widthColor != widthDepth || heightColor != heightDepth )
                {
                    const UINT DepthColorSize = stride * heightDepth;

                    static std::vector< BYTE > vecFlipBMP( DepthColorSize );

                    if ( vecFlipBMP.size() != DepthColorSize ) vecFlipBMP.resize( DepthColorSize );

                    BYTE* pSrc = &pBufferDepthToColor[ stride * ( heightDepth - 1 ) ];
                    BYTE* pDst = vecFlipBMP.data();

                    for ( int i = NULL; i < heightDepth; i++ )
                    {
                        memcpy( pDst, pSrc, stride );

                        pDst += stride;
                        pSrc -= stride;
                    }
                    PlyWriter::resampleImage( widthDepth, heightDepth, vecFlipBMP.data(), widthColor, heightColor, bufColor.data(), 3 );
                }
                else
                {
                    pBufferDepthToColor = &pBufferDepthToColor[ stride * ( heightDepth - 1 ) ];

                    for ( int i = NULL; i < heightDepth; i++ )
                    {
                        memcpy( &bufColor[ i * stride ], pBufferDepthToColor, stride );

                        pBufferDepthToColor -= stride;
                    }
                }
            }
            break;
        }
    }
	APCImageType::Value depthImageType = APCImageType::DepthDataTypeToDepthImageType(pThis->m_previewParams.m_depthType);
	/*Get RectLogData*/
	eSPCtrl_RectLogData* rectifyLogData = NULL;

	if (pThis != nullptr) {
		if ( pThis->m_previewParams.m_rectifyLogData[0] ) {
			rectifyLogData = pThis->m_previewParams.m_rectifyLogData[0];
		}
		if ( pThis->m_previewParams.m_rectifyLogData[1] ) {
			rectifyLogData = pThis->m_previewParams.m_rectifyLogData[1];
		}
	}
	else {
		return;
	}
	//const float downsampleRatio = ( heightDepth > 640 ? float( 640.0 / heightDepth ) : 1.0f );

	float zNear = 1.0f*pThis->m_ZNear ? pThis->m_ZNear : 0.1f;//mm
	float zFar = 1.0f*pThis->m_ZFar ? pThis->m_ZFar : 1000;
	//std::vector<CloudPoint> pointCloud;

    if (pThis->m_pointCloudDepth.size() != widthColor * heightColor * 3)
    {
        pThis->m_pointCloudDepth.resize(widthColor * heightColor * 3, 0.0f);
    }
    else 
    {
        std::fill(pThis->m_pointCloudDepth.begin(), pThis->m_pointCloudDepth.end(), 0.0f);
    }
    
    if (pThis->m_pointCloudRGB.size() != widthColor * heightColor * 3)
    {
        pThis->m_pointCloudRGB.resize(widthColor * heightColor * 3, 0);
    }
    else
    {
        std::fill(pThis->m_pointCloudRGB.begin(), pThis->m_pointCloudRGB.end(), 0);
    }
    
    //if ( pThis->IsDevicePid( APC_PID_8029 ) ) APC_FlyingDepthCancellation_D8( pThis->m_hApcDI, &pThis->m_DevSelInfo, bufDepth.data(), widthDepth, heightDepth );

	/*Transform to 3D */
	//if ( pThis->IsDevicePid( APC_PID_8054 ) && pThis->m_previewParams.m_kcolorOption > EOF ) {
	//	//int degreeOfRectifyLogK = 90; // default rotate once
	//	//if (pThis->RotateImage()) {
	//	//	degreeOfRectifyLogK = 180;
	//	//}
	//	PlyWriter::etronFrameTo3DMultiSensor(widthDepth, heightDepth, bufDepth, widthColor, heightColor, bufColor, rectifyLogData, pThis->m_previewParams.m_rectifyLogDataSlave, depthImageType, pointCloud, true, zNear, zFar, true, true, downsampleRatio, 90/*degreeOfRectifyLogK*/);
	//}
	//else if ( pThis->IsDevicePid( APC_PID_8040S ) ) {
 //       if ( pThis->m_previewParams.m_kcolorOption > EOF )
 //           PlyWriter::etronFrameTo3DCylinder(widthDepth, heightDepth, bufDepth, widthColor, heightColor, bufColor, rectifyLogData, pThis->m_previewParams.m_rectifyLogDataSlave, depthImageType, pointCloud, true, zNear, zFar, false, 1.0f);
	//	else PlyWriter::etronFrameTo3DCylinder(widthDepth, heightDepth, bufDepth, widthColor, heightColor, bufColor, rectifyLogData, depthImageType, pointCloud, true, zNear, zFar, false, 1.0f);
	//}
	//else if ( pThis->IsDevicePid( APC_PID_8029 ) )
 //   {
 //       PlyWriter::etronFrameTo3D_8029(widthDepth, heightDepth, bufDepth, widthColor, heightColor, bufColor, rectifyLogData, depthImageType, pointCloud, true, zNear, zFar, true, true, downsampleRatio);
 //   }
 //   else
    if ( ( pThis->IsDevicePid( APC_PID_8054 ) || pThis->IsDevicePid( APC_PID_8040S ) ) && pThis->m_previewParams.m_kcolorOption > EOF && pThis->m_bPCV_NoColorStream )
    {
        static PointCloudInfo xPointCloudInfo;

        xPointCloudInfo = pThis->m_xPointCloudInfo;

        xPointCloudInfo.focalLength_K = 0;

        APC_GetPointCloud( pThis->m_hApcDI, &pThis->m_DevSelInfo, &bufColor[0], widthColor, heightColor, &bufDepth[0], widthDepth, heightDepth, &xPointCloudInfo, &pThis->m_pointCloudRGB[0], &pThis->m_pointCloudDepth[0], zNear, zFar );
    }
    else
    {
        // For the depth only mode, set the point cloud in green.
        if (isDepthOnly)
        {
            for (int i = 0; i < (widthDepth * heightDepth * 3); i += 3)
            {
                bufColor[i]     = 0x0;
                bufColor[i + 1] = 0xFF;
                bufColor[i + 2] = 0x0;
            }
        }

		//PlyWriter::etronFrameTo3D(widthDepth, heightDepth, bufDepth, widthColor, heightColor, bufColor, rectifyLogData, depthImageType, pointCloud, true, zNear, zFar, true, true, downsampleRatio);
        APC_GetPointCloud( pThis->m_hApcDI, &pThis->m_DevSelInfo, &bufColor[0], widthColor, heightColor, &bufDepth[0], widthDepth, heightDepth, &pThis->m_xPointCloudInfo, &pThis->m_pointCloudRGB[0], &pThis->m_pointCloudDepth[0], zNear, zFar );
	}
	//PlyWriter::etronFrameTo3D(widthOutput, heightOutput, bufDepth, bufColor, rectifyLogData.ReProjectMat, depthImageType, pointCloud, clipping, zNear, zFar, removeINF);

	/*Copy to output point cloud*/
	//if (pointCloud.size() > 0) {
    if ( !pThis->m_pointCloudDepth.empty() && !pThis->m_pointCloudRGB.empty() )
    {
		//std::vector<float> outputVertexs(pointCloud.size() * 3); // 3 float per vertex
		//std::vector<unsigned char> outputColor(pointCloud.size() * 3); // r,g,b per vertex

		//for (int i = 0; i < pointCloud.size(); i++)
		//{
		//	outputVertexs[i * 3 + 0] = pointCloud[i].x;
		//	outputVertexs[i * 3 + 1] = pointCloud[i].y;
		//	outputVertexs[i * 3 + 2] = pointCloud[i].z;
		//	outputColor[i * 3 + 0] = pointCloud[i].r;
		//	outputColor[i * 3 + 1] = pointCloud[i].g;
		//	outputColor[i * 3 + 2] = pointCloud[i].b;
		//}
        if ( pThis->m_previewParams.m_pointCloudViewer->IsWindow() )
		{
			CAutoLock lock(pThis->m_previewParams.m_mutex);
			if (pThis->m_previewParams.m_pointCloudViewer != nullptr) {
				//pThis->m_previewParams.m_pointCloudViewer->updateModel(0, outputVertexs, outputColor);
                pThis->m_previewParams.m_pointCloudViewer->updateModel(0, pThis->m_pointCloudDepth, pThis->m_pointCloudRGB);
			}
		}
	}
	else {
		TRACE("Empty point cloud \n");
	}
}
void CPreviewImageDlg::PointcloudViewerMessageCallback(CPointCloudViewer::MessageType msg, int value, void* pParam) {
	CPreviewImageDlg* pThis = (CPreviewImageDlg*)pParam;

	if (msg == CPointCloudViewer::MSG_KEYBOARD) {				
		if (value == 'D' && GetKeyState(VK_CONTROL) < 0)// ctrl + D
		{			
			pThis->PostMessage(WM_MSG_SNAPSHOT, (WPARAM)pThis);
		}		
	}

	if (msg == CPointCloudViewer::MSG_CLOSE)
    {
#ifndef ESPDI_EG
        if ( pThis->m_frameGrabber )
		{
            pThis->m_frameGrabber->Close();
		}
#endif
		pThis->PostMessage(WM_MSG_CLOSE_PREVIEW_DLG, (WPARAM)pThis);
	}
}

void CPreviewImageDlg::AdjustZDTableIndex(int *pzdTblIdx, int width, int height, APCImageType::Value DImgType)
{
	if (*pzdTblIdx == -1 && m_previewParams.m_depthSwitch % 2 == 1)	// if Setup Depth0
	{
		*pzdTblIdx = 0;
	}
    else if ( IsDevicePid( APC_PID_8060 ) )
	{
        *pzdTblIdx = 0;
	}
    else if ( IsDevicePid( APC_PID_AMBER ) )
    {
        *pzdTblIdx = 0;
    }
    // some modules need to adjust zdTableIndex for USB2
    else if ( IsDevicePid( APC_PID_8037 ) )
    {	
        if (height == 720)
        {
            *pzdTblIdx = 0;
        }
		else if (height <= 480)
        {
            *pzdTblIdx = 1;
        }
    }
	else if ( IsDevicePid( APC_PID_8054 ) )
	{
		if (height == 640);
		{
			CPoint resTmp = CPoint(1280, 1280);
			*pzdTblIdx = GetDepthStreamIndex(resTmp);
		}
	}
	else if ( IsDevicePid( APC_PID_8040S ) )
	{
		if ((m_pStreamDepthInfo[m_previewParams.m_depthOption].nWidth == 912) ||
			(m_pStreamDepthInfo[m_previewParams.m_depthOption].nHeight == 912) ||
			(m_pStreamDepthInfo[m_previewParams.m_depthOption].nWidth == 456) ||
			(m_pStreamDepthInfo[m_previewParams.m_depthOption].nHeight == 456)
			)
		{
			*pzdTblIdx = 1;
		}
	}
    else if ( IsDevicePid( APC_PID_8051 ) )
    {
        if (height == 360 ) *pzdTblIdx = 0;
    }
	else if ( IsDevicePid(APC_PID_8062) )
	{
		/// **************************************** ///
		/// PIF : special for Scallar Down			 ///
		/// Jacky_Tag								 ///
		/// **************************************** ///
		if (USB_PORT_TYPE_2_0 == m_eUSB_Port_Type && height == 360)
			*pzdTblIdx = 0;
	}
    else if ( height && ( IsDevicePid( APC_PID_8052 ) || IsDevicePid( APC_PID_8036 ) ) ) // U2 mode34, 35
	{
		if (IsDevicePid(APC_PID_8036) && height == 360)
		{
			*pzdTblIdx = 0;
		}
		else if ( m_previewParams.m_colorPreview.m_imgRes.y && ( m_previewParams.m_colorPreview.m_imgRes.y % height != 0 ) )
    	{
    		*pzdTblIdx = 2;
    	}

        if (IsDevicePid(APC_PID_8052))
        {
            // For scallar down.
            if ((width == 640) && (height == 360))
            {
                *pzdTblIdx = 0;
            }
        }
    }
    else if (IsDevicePid(APC_PID_SANDRA))
    {
        int colorWidth = m_pStreamColorInfo[m_previewParams.m_colorOption].nWidth;
        int colorHeight = m_pStreamColorInfo[m_previewParams.m_colorOption].nHeight;

        // Specially for Mode 11
        if ((width == 640) && (height == 360) && (colorWidth == 1280) && (colorHeight == 360))
        {
            *pzdTblIdx = 1;
        }

        // Specially For Mode 9
        if ((width == 1280) && (height == 720) && (colorWidth == 2560) && (colorHeight == 720))
        {
            *pzdTblIdx = 0;
        }
    }
    else if (IsDevicePid(APC_PID_NORA))
    {
        // Specially for height 768
        if (height == 768)
        {
            *pzdTblIdx = 0;
        }

        // Specially for height 384
        if (height == 384)
        {
            *pzdTblIdx = 1;
        }
    }
}

void CPreviewImageDlg::AdjustColorResForDepth0(CPoint*colorRealRes)
{
#ifndef ESPDI_EG
	/*Has Depth0*/
	if ( IsDevicePid( APC_PID_8040S ) )
	{
		if ((m_pStreamColorInfo[m_previewParams.m_colorOption].nWidth == 2160) && (m_pStreamColorInfo[m_previewParams.m_colorOption].nHeight == 1920))
		{
			colorRealRes->x = 3840;
			colorRealRes->y = 1080;
		}
	}
	colorRealRes->x = colorRealRes->x / 2;
#endif
}

void CPreviewImageDlg::AdjustRegister()
{
#ifndef ESPDI_EG
        if (IsDevicePid(APC_PID_8029))
		{
			WORD wF05B = NULL;
			wF05B = 0xF0;
			APC_SetHWRegister(m_hApcDI, &m_DevSelInfo, 0xF05B, wF05B, FG_Address_2Byte | FG_Value_1Byte);
			wF05B = 0xF3;
			APC_SetHWRegister(m_hApcDI, &m_DevSelInfo, 0xF05B, wF05B, FG_Address_2Byte | FG_Value_1Byte);
		}
		if ( IsDevicePid( APC_PID_8053 ) )
		{
			APCImageType::Value depthImageType = APCImageType::DepthDataTypeToDepthImageType(m_previewParams.m_depthType);

			if (depthImageType == APCImageType::DEPTH_8BITS)
			{
				if (m_previewParams.m_colorOption >= 0 && m_previewParams.m_depthOption >=0)
				{
					CPoint Depthres = CurDepthImgRes();
					CPoint colorRealRes = CurColorImgRes();
					if (Depthres.x == 640 && Depthres.y == 360 && colorRealRes.x == 1280 && colorRealRes.y == 720)
					{
						m_registerSettings->ForEx8053Mode9(m_hApcDI, &m_DevSelInfo);
					}
				}
			}
		}
#endif

#ifndef ESPDI_EG
		if ( IsDevicePid( APC_PID_8038 ) )
		{
			Sleep(1000);
			m_registerSettings->DM_Quality_Register_Setting_For6cm(m_hApcDI, &m_DevSelInfo);
			m_registerSettings->DM_Quality_Register_Setting_Slave(m_hApcDI, &m_DevSelInfo);
		}
		m_registerSettings->DM_Quality_Register_Setting(m_hApcDI, &m_DevSelInfo);
#endif

#ifndef ESPDI_EG
		if (IsDevicePid(APC_PID_8062))
		{
			CPoint Depthres = CurDepthImgRes();
			APC_AdjustFocalLengthFromFlash(m_hApcDI, &m_DevSelInfo, Depthres.x, Depthres.y);
			m_pAccuracyDlg->UpdatePixelUnit();
			m_pAccuracyDlg->UpdateFocalLength();
		}
#endif
}

/// **************************************** ///
/// Interleave模式下的Color,理應比較鮮豔		 ///
/// 支援 Module：8052 / 8059 / 8062 / 8053	 ///
/// 鮮豔 Module 擴充判斷						 ///
/// #6412 #6493								 ///
/// Jacky_Tag: #Interleave Support Module#	 ///
/// **************************************** ///
int CPreviewImageDlg::AdjDepthBitIndex( const int depthType )
{
	if ( m_previewParams.m_depthOption == EOF )
	{
        if ( IsDevicePid( APC_PID_8060 ) ) return m_previewParams.m_rectifyData ? APC_DEPTH_DATA_11_BITS : APC_DEPTH_DATA_OFF_RAW;

        return m_previewParams.m_rectifyData ? APC_DEPTH_DATA_OFF_RECTIFY : APC_DEPTH_DATA_OFF_RAW;
	}
    else if (m_bIsInterLeaveMode)//( IsDevicePid( APC_PID_8052 ) || IsDevicePid( APC_PID_8059 ) || IsDevicePid(APC_PID_8062) )
    {
        //if ( m_bIsInterLeaveMode )	//#6493;
        {
            switch ( depthType )
            {
            case APC_DEPTH_DATA_8_BITS:      return APC_DEPTH_DATA_ILM_8_BITS;
            case APC_DEPTH_DATA_8_BITS_RAW:  return APC_DEPTH_DATA_ILM_8_BITS_RAW;
            case APC_DEPTH_DATA_11_BITS:     return APC_DEPTH_DATA_ILM_11_BITS;
            case APC_DEPTH_DATA_11_BITS_RAW: return APC_DEPTH_DATA_ILM_11_BITS_RAW;
            case APC_DEPTH_DATA_14_BITS:     return APC_DEPTH_DATA_ILM_14_BITS;
            case APC_DEPTH_DATA_14_BITS_RAW: return APC_DEPTH_DATA_ILM_14_BITS_RAW;
            }
        }
    }
    else if ( IsDevicePid( APC_PID_8054 ) || IsDevicePid( APC_PID_8040S ) || IsDevicePid( APC_PID_8038 ) )
    {
        if ( ( ( CButton* )GetDlgItem( IDC_CHECK_K_COLOR_STREAM ) )->GetCheck() != BST_CHECKED )
        {
            switch ( depthType )
            {
            case APC_DEPTH_DATA_11_BITS:
            case APC_DEPTH_DATA_11_BITS_RAW: return APC_DEPTH_DATA_11_BITS_COMBINED_RECTIFY;
            case APC_DEPTH_DATA_14_BITS:
            case APC_DEPTH_DATA_14_BITS_RAW: return APC_DEPTH_DATA_14_BITS_COMBINED_RECTIFY;
            }
        }
    }
    return depthType;
}

void CPreviewImageDlg::AdjustNearFar( int& zFar, int& zNear, CDepthDlg* pDlg )
{
    pDlg->GetDepthZValue( zFar, zNear );

    if ( IsDevicePid( APC_PID_8038 ) )
    {
        if ( zNear < EX8038_MIN_DEPTH_RANGE ) zNear = EX8038_MIN_DEPTH_RANGE;
    }
    if ( zFar > MAX_DEPTH_DISTANCE ) zFar  = MAX_DEPTH_DISTANCE;

    SetDlgItemInt( IDC_EDIT_ZFAR,  zFar  );
    SetDlgItemInt( IDC_EDIT_ZNEAR, zNear );
}

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
    wchar_t dialogClassName[1024] = { NULL };
	::GetClassName(hwnd, dialogClassName, 1024);

	CString strAppClassName;
	strAppClassName.LoadString(IDS_ERTON_DI_CLASS_NAME);

	if (wcscmp(dialogClassName, strAppClassName) == 0)
	{
		::PostMessage(hwnd, WM_MSG_SNAPSHOT_ALL, NULL, NULL);
	}

	return TRUE;
}

void CPreviewImageDlg::OpenKColorStream()
{
	if (m_previewParams.m_kcolorOption >= 0)
	{
		CPoint colorRealRes = CurKColorImgRes();
		m_previewParams.m_kcolorPreview.m_imgRes = colorRealRes;

		CColorDlg* pKcolorDlg = new CColorDlg(this);
		pKcolorDlg->SetColorParams(m_hApcDI, m_DevSelInfo, colorRealRes.x, colorRealRes.y, FALSE, CurDepthImgRes());
		pKcolorDlg->SetDlgName("K color Dialog");
		pKcolorDlg->Create(pKcolorDlg->IDD, this);

		m_previewParams.m_kcolorPreview.SetPreviewDlg(pKcolorDlg);

		InitPreviewDlgPos();
	}
	else
	{
		m_previewParams.m_kcolorPreview.m_imgRes = CPoint(0, 0);
	}
}

void CPreviewImageDlg::OpenTrackStream()
{
	if (m_previewParams.m_tcolorOption >= 0)
	{
		CPoint colorRealRes = CurTrackImgRes();
		m_previewParams.m_trackPreview.m_imgRes = colorRealRes;

		CColorDlg* pDlg = new CColorDlg(this);
		pDlg->SetColorParams(m_hApcDI, m_DevSelInfo, colorRealRes.x, colorRealRes.y, FALSE, CurDepthImgRes());
		pDlg->SetDlgName("Track Dialog");
		pDlg->Create(pDlg->IDD, this);
		m_previewParams.m_trackPreview.SetPreviewDlg(pDlg);

		InitPreviewDlgPos();
	}
	else
	{
		m_previewParams.m_trackPreview.m_imgRes = CPoint(0, 0);
	}
}

void CPreviewImageDlg::OpenMainStream()
{
    if ( m_previewParams.m_colorOption < 0 && 
         m_previewParams.m_depthOption < 0)
    {
        return;
    }
    PreparePreviewDlg();
    OnBnClickedCheckRotateImg();
#ifndef ESPDI_EG
    GetDepthIndexFromColorStream();

    if (m_previewParams.m_IsDepthFusionOn)
    {
        if (m_depthFusionHelper != nullptr)
        {
            delete m_depthFusionHelper;
        }
        CPoint res = CurDepthImgRes();

        m_depthFusionHelper = new CDepthFusionHelper(3, res.x, res.y, 
            m_previewParams.m_camFocus, m_previewParams.m_baselineDist, 1, CPreviewImageDlg::DepthFusionCallback, this);
        m_depthFusionHelper->SetSDKHandle( m_hApcDI, &m_DevSelInfo );

        if (((CButton*)GetDlgItem(IDC_CHECK_FUSION_SWPP))->GetCheck() == BST_CHECKED)
        {
            m_depthFusionHelper->EnablePostProc(true);
        }
    }
#endif
}

void CPreviewImageDlg::OnBnClickedPreviewBtn()
{
	if (GetDlgItem(IDC_PREVIEW_BTN)->IsWindowEnabled() == FALSE)
		return;

    m_bIsInterLeaveMode = ( ( CButton* )GetDlgItem( IDC_CHECK_INTERLEAVE_MODE ) )->GetCheck();

    EnableAccuracy( FALSE );

    UpdatePreviewParams();
	OpenMainStream();
	OpenTrackStream();
	OpenKColorStream();

    if ( m_previewParams.m_colorOption  > EOF ||
         m_previewParams.m_depthOption  > EOF ||
         m_previewParams.m_tcolorOption > EOF ||
         m_previewParams.m_kcolorOption > EOF )
    {
        const BOOL bIs8055359Sync = ( BST_CHECKED == ( ( CButton* )GetDlgItem( IDC_CHK_MULTI_SYNC ) )->GetCheck() );
		/*if (m_pdlgVideoDeviceDlg != NULL)
		{
			m_pdlgVideoDeviceDlg->ChangeIMU_UI_FrameCount_SerialCount(bIs8055359Sync);
		}*/

        GetDlgItem( IDC_PREVIEW_BTN )->EnableWindow(FALSE);
        GetDlgItem( IDC_COMBO_FRAME_RATE )->EnableWindow( FALSE );
        GetDlgItem( IDC_DEPTH_FRAME_RATE )->EnableWindow( FALSE );
        GetDlgItem( IDC_COMBO_DEPTH_BIT_SEL_CTRL )->EnableWindow( FALSE );
        GetDlgItem( IDC_COMBO_MODE_CONFIG )->EnableWindow( FALSE );
        GetDlgItem( IDC_CHK_MULTI_SYNC )->EnableWindow( FALSE );
        GetDlgItem( IDC_SNAPSHOT_BTN )->EnableWindow( TRUE );
        GetDlgItem( IDC_BT_Z_SET  )->EnableWindow(TRUE);
        GetDlgItem( IDC_BT_Z_RESET )->EnableWindow(TRUE);
        GetDlgItem( IDC_EDIT_ZFAR  )->EnableWindow(TRUE);
        GetDlgItem( IDC_EDIT_ZNEAR )->EnableWindow(TRUE);
        GetDlgItem( IDC_CHK_MASTER )->EnableWindow( bIs8055359Sync );

		GetDlgItem(IDC_CHECK_POINTCLOUD_VIEWER)->EnableWindow(FALSE);
		if ((((CButton*)GetDlgItem(IDC_CHECK_POINTCLOUD_VIEWER))->GetCheck() == BST_CHECKED) == FALSE)
		{
			GetDlgItem(IDC_CHK_PCV_NOCOLOR)->EnableWindow(FALSE);
			GetDlgItem(IDC_CHK_PCV_SINGLE)->EnableWindow(FALSE);
		}

        if ( !bIs8055359Sync ) ( ( CButton* )GetDlgItem( IDC_CHK_MASTER ) )->SetCheck( BST_UNCHECKED );
    }
    else return;

    if ( APCImageType::DEPTH_11BITS != APCImageType::DepthDataTypeToDepthImageType( m_previewParams.m_depthType ) )
    {
        ( ( CButton* )GetDlgItem( IDC_CHK_PLY_FILTER ) )->SetCheck( BST_UNCHECKED );
    }
    if (IsSupportHwPostProc())
    {
        //GetDlgItem(IDC_CHECK_HARDWARE_POSTPROC)->EnableWindow(TRUE); // Enable UI HWPP.
    }
    m_isPreviewed = true;
#ifndef ESPDI_EG
    /*Frame grabber*/
	if (PointCloudViewer())
	{
        if (m_frameGrabber                     ) { m_frameGrabber->Close();                     delete m_frameGrabber;                     }
        if (m_previewParams.m_pointCloudViewer ) { m_previewParams.m_pointCloudViewer->close(); delete m_previewParams.m_pointCloudViewer; }

		CPoint resColor = ( ( CButton* )GetDlgItem( IDC_CHECK_K_COLOR_STREAM ) )->GetCheck() == BST_CHECKED ? m_previewParams.m_kcolorPreview.m_imgRes :
                                                                                                              m_previewParams.m_colorPreview.m_imgRes;
		CPoint resDepth;
		for (int i = 0; i < APC_MAX_DEPTH_STREAM_COUNT; ++i)//depth
		{
			if (m_previewParams.m_depthOption >= 0 && (m_previewParams.m_depthSwitch & (1 << i)) != 0)
			{
				
				resDepth = m_previewParams.m_depthPreview[i].m_imgRes;
				if (resDepth.x != 0 && resDepth.y != 0) break;
			}
		}		 

		int maxPoolSize = 1;		
		int bytesPerPixelColor = 3;
		int bytesPerPixelDepth;
		APCImageType::Value depthImageType = APCImageType::DepthDataTypeToDepthImageType(m_previewParams.m_depthType);
		if (depthImageType == APCImageType::DEPTH_8BITS)  bytesPerPixelDepth = 1;
		else bytesPerPixelDepth = 2;
		
		m_frameGrabber = new FrameGrabber(maxPoolSize, CPreviewImageDlg::FrameGrabberCallback, this);
		m_frameGrabber->SetFrameFormat(FrameGrabber::FRAME_POOL_INDEX_COLOR, resColor.x, resColor.y, bytesPerPixelColor);
		m_frameGrabber->SetFrameFormat(FrameGrabber::FRAME_POOL_INDEX_DEPTH, resDepth.x, resDepth.y, bytesPerPixelDepth);
		//if ( IsDevicePid( APC_PID_8054 ) && ((CComboBox*)GetDlgItem(IDC_COMBO_DEPTH_STREAM))->GetCurSel() > 1) {
		//	m_frameGrabber->SetDisableSerialSyn(true);
		//}
		m_previewParams.m_pointCloudViewer = new CPointCloudViewer(CPreviewImageDlg::PointcloudViewerMessageCallback, this);
        m_previewParams.m_pointCloudViewer->SetPointSize( ( ( CSliderCtrl* )GetDlgItem( IDC_SLD_PCL_PSIZE ) )->GetPos() );
		m_previewParams.m_pointCloudViewer->setWindowPos(500, 500);

        GetDlgItem( IDC_SLD_PCL_PSIZE )->EnableWindow(TRUE);
	}
#endif
    ::CreateThread( NULL, NULL, Thread_Preview, this, NULL, NULL );

	MappingIMU();
}

DWORD CPreviewImageDlg::Thread_Preview( void* pvoid )
{
    CPreviewImageDlg* pThis = ( CPreviewImageDlg* )pvoid;
	
	pThis->OnBnClickedCheckInterleaveMode();
    pThis->DoSerialNumCommand();

	if (FrameSyncManager::GetInstance()->IsEnable())
	{
		FrameSyncManager::GetInstance()->RegisterDevice(pThis->m_hApcDI, pThis->m_DevSelInfo);
		FrameSyncManager::GetInstance()->SetIsInterleave(pThis->m_hApcDI, pThis->m_DevSelInfo, pThis->m_bIsInterLeaveMode);
	}

    if ( !pThis->IsDevicePid( APC_PID_8029 ) ) pThis->EnableIR( TRUE ); // 8029 need enable IR after open-device

    auto OpenDevice = [ = ]( const int colorOption, const int depthOption, const int iFPS, const int iPid )
    {
        const int Ret = APC_OpenDevice( pThis->m_hApcDI,
                                            &pThis->m_DevSelInfo,
			                                colorOption,
                                            depthOption,
                                            pThis->m_previewParams.m_depthSwitch,
                                            iFPS,
                                            CPreviewImageDlg::ImgCallback,
                                            pThis,
                                            iPid );
        if ( APC_VIDEO_RENDER_FAIL == Ret )
        {
            AfxMessageBox( _T( "Cannot Run Graph!" ) ); 
        }
        return ( APC_OK == Ret );
    };
    do
    {
        if ( pThis->m_previewParams.m_colorOption > EOF || pThis->m_previewParams.m_depthOption > EOF )
        {
            int depthOption = pThis->m_previewParams.m_depthOption;

            int iFPS = pThis->GetDlgItemInt( pThis->m_previewParams.m_colorOption > EOF ? IDC_COMBO_FRAME_RATE : IDC_DEPTH_FRAME_RATE );

            if ( pThis->m_previewParams.m_kcolorOption > EOF )
            {
                if ( pThis->IsDevicePid( APC_PID_8054 ) || pThis->IsDevicePid( APC_PID_8040S ) ) depthOption = EOF;

                iFPS = pThis->GetDlgItemInt( IDC_DEPTH_FRAME_RATE ); // D+K use Depth-Fps
            }
#if 0 //REFINE_DATABASE_CODE
            APC_SetDepthDataType(pThis->m_hApcDI, &pThis->m_DevSelInfo, pThis->m_previewParams.m_depthType);
#else
			WORD depthDataTypeOffset = 0;
			if ((pThis->IsDevicePid(APC_PID_8036) || pThis->IsDevicePid(APC_PID_8052)) && depthOption != EOF)
			{
				if (pThis->m_pStreamDepthInfo[depthOption].nWidth == 640 && pThis->m_pStreamDepthInfo[depthOption].nHeight == 360)
				{
					depthDataTypeOffset = APC_DEPTH_DATA_SCALE_DOWN_MODE_OFFSET;
				}
			}

            APC_SetDepthDataType( pThis->m_hApcDI, &pThis->m_DevSelInfo, pThis->m_previewParams.m_depthType + depthDataTypeOffset);
#endif

            if ( !OpenDevice( pThis->m_previewParams.m_colorOption,
                              depthOption,
                              iFPS,
                              EOF ) ) break;
        }
        if ( pThis->m_previewParams.m_tcolorOption > EOF )
        {
            APC_SetDepthDataTypeEx( pThis->m_hApcDI, &pThis->m_DevSelInfo, APC_DEPTH_DATA_11_BITS, APC_PID_8060_T );

            if ( !OpenDevice( pThis->m_previewParams.m_tcolorOption,
                              EOF,
                              pThis->GetDlgItemInt( IDC_COMBO_FRAME_RATE ),
                              APC_PID_8060_T ) ) break;
        }
        if ( pThis->m_previewParams.m_kcolorOption > EOF )
        {
            int iPid = NULL;

            switch ( pThis->m_devinfoEx.wPID )
            {
            case APC_PID_8060:  iPid = APC_PID_8060_K;  break;
            case APC_PID_8054:  iPid = APC_PID_8054_K;  APC_SetDepthDataTypeEx( pThis->m_hApcDI, &pThis->m_DevSelInfo, 0x01, iPid  ); break;
            case APC_PID_ORANGE:  iPid = APC_PID_ORANGE_K;  APC_SetDepthDataTypeEx(pThis->m_hApcDI, &pThis->m_DevSelInfo, 0x01, iPid); break;
            case APC_PID_8063:  iPid = APC_PID_8063_K;  APC_SetDepthDataTypeEx(pThis->m_hApcDI, &pThis->m_DevSelInfo, 0x01, iPid); break;
            case APC_PID_8040S: iPid = APC_PID_8040S_K; APC_SetDepthDataTypeEx( pThis->m_hApcDI, &pThis->m_DevSelInfo, 0x00, iPid  ); break;
            }
            if ( !OpenDevice( pThis->m_previewParams.m_kcolorOption,
                              EOF,
                              pThis->GetDlgItemInt( IDC_COMBO_FRAME_RATE ),
                              iPid ) ) 
				break;
        }
        pThis->ResetStreamTimeStamp();   
        pThis->SetTimer(CHECK_RESTART_TIMER, CHECK_IMAGE_CALLBACK_FIRSTTIME_INTERVAL, nullptr); // wait first image

        if ( pThis->m_previewParams.m_depthOption  > EOF ) 
			pThis->SetTimer(CHECK_REGISTER_TIMER, CHECK_REGISTER_TIME_INTERVAL, nullptr);
        if ( pThis->m_previewParams.m_colorOption  > EOF ||             pThis->m_previewParams.m_kcolorOption > EOF ) 
			pThis->SetTimer(CHECK_SYNC_TIMER, CHECK_REGISTER_TIME_INTERVAL, nullptr);

        if ( pThis->IsDevicePid( APC_PID_8029 ) ) { pThis->EnableIR( FALSE ); Sleep( 50 ); pThis->EnableIR( TRUE ); }// 8029 need enable IR after open-device
    
        if (pThis->IsDevicePid( APC_PID_GRAPE ))
        {
            APC_STREAM_INFO *pStreamInfo = pThis->m_pStreamColorInfo;
            int nColorWidth = pStreamInfo[pThis->m_previewParams.m_colorOption].nWidth;
            int nColorHeight = pStreamInfo[pThis->m_previewParams.m_colorOption].nHeight;
            bool bFormatMJPG = pStreamInfo[pThis->m_previewParams.m_colorOption].bFormatMJPG;

            APC_SetHWRegister(pThis->m_hApcDI,
                &pThis->m_DevSelInfo,
                0xF100, 0x10,
                FG_Address_2Byte | FG_Value_1Byte);

            APC_SetHWRegister(pThis->m_hApcDI,
                &pThis->m_DevSelInfo,
                0xF6CE, 0x20,
                FG_Address_2Byte | FG_Value_1Byte);
        }

        return NULL;
    }while ( FALSE );

    if ( pThis->m_previewParams.m_colorOption  > EOF ) pThis->m_mapColorStreamTimeStamp[ APC_Stream_Color ] = time(NULL);
    if ( pThis->m_previewParams.m_kcolorOption > EOF ) pThis->m_mapColorStreamTimeStamp[ APC_Stream_Kolor ] = time(NULL);
    if ( pThis->m_previewParams.m_depthOption  > EOF ) pThis->m_mapDepthStreamTimeStamp[ 0 ] = time(NULL);

    pThis->SetTimer(CHECK_REOPEN_TIMER, CHECK_IMAGE_CALLBACK_TIME_INTERVAL, nullptr);

    return NULL;
}

void CPreviewImageDlg::ResetStreamTimeStamp()
{
    CAutoLock lock(m_previewParams.m_mutex);

	m_mapColorStreamTimeStamp.clear();
	m_mapDepthStreamTimeStamp.clear();
}

#if 0 //REFINE_DATABASE_CODE
int CPreviewImageDlg::GetTableIndex()
{
    int tableIndex = 0;
    const std::vector< ModeConfig::MODE_CONFIG >& vecModeConfig = g_ModeConfig.GetModeConfigList(m_devinfoEx.wPID);
    CComboBox* pCbx = (CComboBox*)GetDlgItem(IDC_COMBO_MODE_CONFIG);
    int iMode = pCbx->GetItemData(pCbx->GetCurSel());
    for (auto& ModeConfig : vecModeConfig)
    {
        if (iMode == ModeConfig.iMode)
        {
            tableIndex = ModeConfig.tableIndex;
            break;
        }
    }
    return tableIndex;
}

WORD CPreviewImageDlg::GetVideoModeFWValue()
{
    int videoModefwValue = 0;

    CComboBox* pCbx = (CComboBox*)GetDlgItem(IDC_COMBO_MODE_CONFIG);
    int iMode = pCbx->GetItemData(pCbx->GetCurSel());
    const std::vector< ModeConfig::MODE_CONFIG >& vecModeConfig = g_ModeConfig.GetModeConfigList(m_devinfoEx.wPID);
    for (auto& ModeConfig : vecModeConfig)
    {
        // Get current video mode
        if (iMode == ModeConfig.iMode)
        {
            CComboBox* pDepthBitCbx = (CComboBox*)GetDlgItem(IDC_COMBO_DEPTH_BIT_SEL_CTRL);
            int curIndex = pDepthBitCbx->GetCurSel();
            videoModefwValue = ModeConfig.vecVideoMode.at(curIndex);
            break;
        }
    }
    return videoModefwValue;
}
#endif

void CPreviewImageDlg::PreparePreviewDlg()
{
    CAutoLock lock(m_previewParams.m_mutex);

    CPoint cpDepthRes = CurDepthImgRes();
    CPoint& colorRealRes = m_previewParams.m_colorPreview.m_imgRes;
    
    colorRealRes = ( m_previewParams.m_colorOption > EOF ? CurColorImgRes() : CPoint( NULL, NULL ) );

    if ( m_previewParams.m_colorOption > EOF )//color
    {
#ifndef ESPDI_EG
        if ( IsStream0ColorPlusDepth() )
        {
			AdjustColorResForDepth0(&colorRealRes);

            cpDepthRes = colorRealRes;
        }
#endif
    }

    APCImageType::Value depthImageType = APCImageType::DepthDataTypeToDepthImageType(m_previewParams.m_depthType);
#if 0 //REFINE_DATABASE_CODE
    int zdTableIndex = GetTableIndex();
#else
    int zdTableIndex = GetDepthStreamIndex(cpDepthRes);
    AdjustZDTableIndex(&zdTableIndex, cpDepthRes.x, cpDepthRes.y, depthImageType);
#endif
    BOOL bInitialZDValue = TRUE;

    const int iDepthPeriod = ( GetDlgItemInt( IDC_DEPTH_FRAME_RATE ) ? GetDlgItemInt( IDC_DEPTH_FRAME_RATE ) : GetDlgItemInt( IDC_COMBO_FRAME_RATE ) ) / 3;
    const DEPTHMAP_TYPE depthMapType = ( ( CComboBox* )GetDlgItem(IDC_COMBO_DEPTH_OUTPUT_CTRL ) )->GetCurSel() ? TRANSFER_TO_GRAYRGB : TRANSFER_TO_COLORFULRGB;

    for (int i = 0; i < APC_MAX_DEPTH_STREAM_COUNT; ++i)//depth
    {
        if (m_previewParams.m_depthOption >= 0 && (m_previewParams.m_depthSwitch & (1 << i)) != 0)
        {
            m_previewParams.m_depthPreview[i].m_imgRes = cpDepthRes;
            const float ratio_Mat = (float)cpDepthRes.y / m_previewParams.m_rectifyLogData[ i ]->OutImgHeight;      
	        const float baseline  = 1.0f / m_previewParams.m_rectifyLogData[ i ]->ReProjectMat[ 14 ];
	        const float focalLength = m_previewParams.m_rectifyLogData[ i ]->ReProjectMat[ 11 ] * ratio_Mat;

			std::vector<float> baselineDist;
            CDepthDlg* pDlg = new CDepthDlg(this);
            pDlg->SetImageParams(m_hApcDI, m_DevSelInfo, m_DevType, depthImageType,
                i, zdTableIndex, cpDepthRes.x, cpDepthRes.y, iDepthPeriod,
				m_previewParams.m_camFocus, m_previewParams.m_baselineDist[i], baselineDist ); 
            pDlg->Create(pDlg->IDD, this);

            if ( bInitialZDValue ) // get z-value from ZD-table first time
            {
                bInitialZDValue = FALSE;

                AdjustNearFar( m_ZFar, m_ZNear, pDlg );
            }
            pDlg->SetBaselineFocalLength( baseline, focalLength );
            pDlg->UpdateColorPalette( depthMapType );
            pDlg->UpdateColorPalette( m_ZFar, m_ZNear );
            pDlg->SetDepthROI( GetDlgItemInt( IDC_ST_DEPTH_ROI ) );

            m_previewParams.m_depthPreview[i].SetPreviewDlg(pDlg);
        }
    }
    if (m_previewParams.m_IsDepthFusionOn)//depth fusion
    {
        m_previewParams.m_depthFusionPreview.m_imgRes = cpDepthRes;

        m_previewParams.m_showFusionSelectedDlg = ((CButton*)GetDlgItem(IDC_CHECK_FUSION_SELECT_DLG))->GetCheck() == BST_CHECKED;
        std::vector<float> multiBaselineDist = m_previewParams.m_showFusionSelectedDlg ? std::vector<float>() : m_previewParams.m_baselineDist;
        CDepthDlg* pDlg = new CDepthDlg(this);
        pDlg->SetImageParams(m_hApcDI, m_DevSelInfo, m_DevType, depthImageType,
            -1, zdTableIndex, cpDepthRes.x, cpDepthRes.y, iDepthPeriod, m_previewParams.m_camFocus, m_previewParams.m_baselineDist[0], 
            multiBaselineDist );// fusion using baseline0
        pDlg->Create(pDlg->IDD, this);
        pDlg->UpdateColorPalette( depthMapType );
        pDlg->UpdateColorPalette( m_ZFar, m_ZNear );
        m_previewParams.m_depthFusionPreview.SetPreviewDlg(pDlg);

        //if (m_previewParams.m_showFusionSelectedDlg)
        //{
        //    m_previewParams.m_fusionSelectedTargetPreview.m_imgRes = cpDepthRes;
        //    CColorDlg* pSelectedTargetDlg = new CColorDlg(this);
        //    pSelectedTargetDlg->SetColorParams(m_hApcDI, m_DevSelInfo, cpDepthRes.x, cpDepthRes.y, FALSE, cpDepthRes);
        //    pSelectedTargetDlg->SetDlgName("Fusion Selected Target Dialog");
        //    pSelectedTargetDlg->Create(pDlg->IDD, this);
        //    m_previewParams.m_fusionSelectedTargetPreview.SetPreviewDlg(pSelectedTargetDlg);
        //}
    }
    if (m_previewParams.m_colorOption >= 0)//color
    {
		if (m_previewParams.m_360ModeEnabled)
		{
			EnableDenoise(true);
			InitEysov(colorRealRes.x, colorRealRes.y);
		}
        const BOOL bIsLRD_Mode = ( ( IsDevicePid( APC_PID_8036 ) || IsDevicePid( APC_PID_8037 ) || IsDevicePid( APC_PID_8052 )  || IsDevicePid( APC_PID_8029 ) ) && 
                                 ( ( 2560 == colorRealRes.x && 1280 == cpDepthRes.x ) || ( 2560 == colorRealRes.x && 640 == cpDepthRes.x )  || ( 1280 == colorRealRes.x && 640 == cpDepthRes.x ) ||
                                   ( 640 == colorRealRes.x && 320 == cpDepthRes.x ) ) );
        CColorDlg* pDlg = new CColorDlg(this);
        pDlg->SetColorParams(m_hApcDI, m_DevSelInfo, colorRealRes.x, colorRealRes.y, bIsLRD_Mode, cpDepthRes);
        pDlg->Create(pDlg->IDD, this);
        m_previewParams.m_colorPreview.SetPreviewDlg(pDlg);
    }
    InitPreviewDlgPos();
}

void GetDlgPos(const std::vector<std::pair<CPoint, CDialog*>>& previewInfo, std::vector<CRect>& dlgPos)
{
    const CPoint primaryScreenRes(GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
    const int xBorderWidth = GetSystemMetrics(SM_CXFIXEDFRAME);
    const int yBorderHeight = GetSystemMetrics(SM_CYFIXEDFRAME);
    const int captionHeight = GetSystemMetrics(SM_CYCAPTION);

    dlgPos.clear();
    if (previewInfo.size() == 1)
    {
        CPoint res;
        res.x = (previewInfo[0].first.x >  (primaryScreenRes.x * 3/4)) ? primaryScreenRes.x *3/4 : previewInfo[0].first.x; //max. 0.75 screen width
        res.y = (int)(res.x * ((double)previewInfo[0].first.y / previewInfo[0].first.x));

        CRect rect;
        rect.left = (primaryScreenRes.x - res.x) / 2;
        rect.top = 0;
        rect.right = rect.left + res.x;
        rect.bottom = rect.top + res.y;
        dlgPos.push_back(rect);
        return;
    }

    double ratio4 = (double)(primaryScreenRes.x - xBorderWidth * 4) / (previewInfo[0].first.x + previewInfo[1].first.x);
    //if (ratio4 > 1.0)
    //{
    //    ratio4 = 1.0;
    //}
	
	/*
		There are three dialogs For YX8060, and their fps is 60. 
		Thus, ratio4 maximum value of dialog must be equal or less than 0.5, 
		or fps of the dialog will be reduced down.
	*/
	if (previewInfo.size() == 3)
	{
		ratio4 = (double)(primaryScreenRes.x - xBorderWidth * 4) / (previewInfo[0].first.x + previewInfo[1].first.x + previewInfo[2].first.x);
	}

    double ratio6 = 0.0; 
    if (previewInfo.size() > 4)
    {
        int base = (previewInfo.size() == 5 ? 2 : 3);
        ratio6 = (double)(primaryScreenRes.x - xBorderWidth * 6)
            / (previewInfo[base].first.x + previewInfo[base + 1].first.x + previewInfo[base + 2].first.x);
    }
    if (ratio6 > 1.0)
    {
        ratio6 = 1.0;
    }

    for (size_t i = 0; i < previewInfo.size(); ++i)
    {
        switch (i)
        {
        case 0:
        {
            dlgPos.push_back(CRect(0, 0,
                (int)(previewInfo[i].first.x * (previewInfo.size() > 5 ? ratio6 : ratio4)) + xBorderWidth * 2,
                (int)(previewInfo[i].first.y * (previewInfo.size() > 5 ? ratio6 : ratio4)) + yBorderHeight * 2 + captionHeight));
            break;
        }
        case 1:
        {
            double ratio = (previewInfo.size() < 6 ? ratio4 : ratio6);
                
            CRect rect;
            rect.left = dlgPos[i - 1].right;
            rect.top = dlgPos[i - 1].top;
            rect.right = rect.left + (int)(previewInfo[i].first.x * ratio) + xBorderWidth * 2;
            rect.bottom = rect.top + (int)(previewInfo[i].first.y * ratio) + yBorderHeight * 2 + captionHeight;
            dlgPos.push_back(rect);
            break;
        }
        case 2:
        {
            CRect rect;
            if (previewInfo.size() < 6)
            {
                rect.left = 0;
                rect.top = dlgPos[0].bottom;
                rect.right = rect.left + (int)(previewInfo[i].first.x * (previewInfo.size() == 5 ? ratio6 : ratio4)) + xBorderWidth * 2;
                rect.bottom = rect.top + (int)(previewInfo[i].first.y * (previewInfo.size() == 5 ? ratio6 : ratio4)) + yBorderHeight * 2 + captionHeight;
				
				if (previewInfo.size() == 3)	/*There are three dialogs For YX8060. Three dialogs need to be aligned.*/
				{
					rect.left = dlgPos[i - 1].right;
					rect.top = dlgPos[i - 1].top;
					rect.right = rect.left + (int)(previewInfo[i].first.x * ratio4) + xBorderWidth * 2;
					rect.bottom = rect.top + (int)(previewInfo[i].first.y * ratio4) + yBorderHeight * 2 + captionHeight;
				}
            }
            else
            {
                rect.left = dlgPos[i - 1].right;
                rect.top = dlgPos[i - 1].top;
                rect.right = rect.left + (int)(previewInfo[i].first.x * ratio6) + xBorderWidth * 2;
                rect.bottom = rect.top + (int)(previewInfo[i].first.y * ratio6) + yBorderHeight * 2 + captionHeight;
            }
            dlgPos.push_back(rect);
            break;
        }
        case 3:
        {
            CRect rect;
            if (previewInfo.size() < 6)
            {
                rect.left = dlgPos[i - 1].right;
                rect.top = dlgPos[i - 1].top;
                rect.right = rect.left + (int)(previewInfo[i].first.x * (previewInfo.size() == 5 ? ratio6 : ratio4)) + xBorderWidth * 2;
                rect.bottom = rect.top + (int)(previewInfo[i].first.y * (previewInfo.size() == 5 ? ratio6 : ratio4)) + yBorderHeight * 2 + captionHeight;
            }
            else
            {
                rect.left = 0;
                rect.top = dlgPos[0].bottom;
                rect.right = rect.left + (int)(previewInfo[i].first.x * ratio6) + xBorderWidth * 2;
                rect.bottom = rect.top + (int)(previewInfo[i].first.y * ratio6) + yBorderHeight * 2 + captionHeight;
            }
            dlgPos.push_back(rect);
            break;
        }
        case 4:
        case 5:
        {
            CRect rect;
            rect.left = dlgPos[i - 1].right;
            rect.top = dlgPos[i - 1].top;
            rect.right = rect.left + (int)(previewInfo[i].first.x * ratio6) + xBorderWidth * 2;
            rect.bottom = rect.top + (int)(previewInfo[i].first.y * ratio6) + yBorderHeight * 2 + captionHeight;
            dlgPos.push_back(rect);
            break;
        }
        default:
        {
            CRect rect;
            rect.left = (LONG) (i - 5) * 160;
            rect.top = (LONG)(i - 5) * 90;
            rect.right = rect.left + (int)(previewInfo[i].first.x * ratio6) + xBorderWidth * 2;
            rect.bottom = rect.top + (int)(previewInfo[i].first.y * ratio6) + yBorderHeight * 2 + captionHeight;
            dlgPos.push_back(rect);
            break;
        }
        }
    }
}

void CPreviewImageDlg::InitPreviewDlgPos()
{
    CAutoLock lock(m_previewParams.m_mutex);

    std::vector<std::pair<CPoint, CDialog*>> previewInfo;
    if (m_previewParams.m_colorPreview.m_previewDlg != nullptr)
    {
        previewInfo.push_back(
            std::pair<CPoint, CDialog*>(m_previewParams.m_colorPreview.m_imgRes, m_previewParams.m_colorPreview.m_previewDlg));
    }
	if (m_previewParams.m_kcolorPreview.m_previewDlg != nullptr)
	{
		previewInfo.push_back(
			std::pair<CPoint, CDialog*>(m_previewParams.m_kcolorPreview.m_imgRes, m_previewParams.m_kcolorPreview.m_previewDlg));
	}

	if (m_previewParams.m_trackPreview.m_previewDlg != nullptr)
	{
		previewInfo.push_back(
			std::pair<CPoint, CDialog*>(m_previewParams.m_trackPreview.m_imgRes, m_previewParams.m_trackPreview.m_previewDlg));
	}
    //if (m_previewParams.m_fusionSelectedTargetPreview.m_previewDlg != nullptr)
    //{
    //    previewInfo.push_back(
    //        std::pair<CPoint, CDialog*>(m_previewParams.m_fusionSelectedTargetPreview.m_imgRes, m_previewParams.m_fusionSelectedTargetPreview.m_previewDlg));
    //}

    if (m_previewParams.m_depthFusionPreview.m_previewDlg != nullptr)
    {
        previewInfo.push_back(
            std::pair<CPoint, CDialog*>(m_previewParams.m_depthFusionPreview.m_imgRes, m_previewParams.m_depthFusionPreview.m_previewDlg));
    }
    
    for (int i = 0;i < APC_MAX_DEPTH_STREAM_COUNT; ++i)
    {
        if (m_previewParams.m_depthPreview[i].m_previewDlg != nullptr)
        {
            previewInfo.push_back(
                std::pair<CPoint, CDialog*>(m_previewParams.m_depthPreview[i].m_imgRes, m_previewParams.m_depthPreview[i].m_previewDlg));
        }
    }

    if (previewInfo.empty())
    {
        return;
    }

    std::vector<CRect> dlgPos;
    GetDlgPos(previewInfo, dlgPos);
	
	if (AutoModuleSyncManager::GetInstance()->IsAutoModuleSyncEnabled() &&
		!AutoModuleSyncManager::GetInstance()->IsMasterDevice(m_DevSelInfo))
	{
		const int captionHeight = GetSystemMetrics(SM_CYCAPTION) + 20;
		int nYOffset = AutoModuleSyncManager::GetInstance()->GetIndex(m_DevSelInfo) * captionHeight ;

		for (CRect &rect : dlgPos)
		{
			rect.OffsetRect(0, nYOffset);
		}
	}

    for ( UINT i = 0; i < previewInfo.size(); ++i )
    {
        previewInfo[ i ].second->MoveWindow( dlgPos[ i ] );
        previewInfo[ i ].second->ShowWindow( PointCloudViewer() ? SW_HIDE : SW_SHOW );
    }
    if ( previewInfo.size() ) previewInfo[ NULL ].second->SetFocus();
}

void CPreviewImageDlg::OnDestroy()
{
    CloseDeviceAndStopPreview(this);

    CDialog::OnDestroy();
}


BOOL CPreviewImageDlg::IsInterLeaveMode()
{
	return m_bIsInterLeaveMode;
}

void CPreviewImageDlg::CloseDeviceAndStopPreview(CDialog* pCallerDlg)
{
    KillTimer( CHECK_RESTART_TIMER );
    KillTimer( CHECK_REGISTER_TIMER );
    KillTimer( CHECK_SYNC_TIMER );
    KillTimer( CHECK_805359_MASTER_RESET_TIMER );

	if (AutoModuleSyncManager::GetInstance()->IsAutoModuleSyncEnabled())
	{
		AutoModuleSyncManager::GetInstance()->FrameStop(m_DevSelInfo);
	}

	if (FrameSyncManager::GetInstance()->IsEnable())
	{
		FrameSyncManager::GetInstance()->UnregisterDevice(m_hApcDI, m_DevSelInfo);
	}

    ((CButton*)GetDlgItem(IDC_CHECK_HARDWARE_POSTPROC))->SetCheck(IsSupportHwPostProc() ? BST_CHECKED : BST_UNCHECKED);

    GetDlgItem(IDC_CHECK_HARDWARE_POSTPROC)->EnableWindow(FALSE);
#ifndef ESPDI_EG
    APC_EnableInterleave( m_hApcDI, &m_DevSelInfo, FALSE );
	if (m_bIsInterLeaveMode)
	{
		m_pPropertyDlg->LowLightEnable(true);
		m_pPropertyDlg->SetLowLight(m_bPrevLowLigh);	
	}
#endif
	APC_CloseDevice(m_hApcDI, &m_DevSelInfo);
#ifndef ESPDI_EG
    if (m_previewParams.m_IsDepthFusionOn)
    {
        if (m_depthFusionHelper != nullptr)
        {
            delete m_depthFusionHelper;
            m_depthFusionHelper = nullptr;
        }
    }
	if ( m_frameGrabber )
    {
        delete m_frameGrabber;
		m_frameGrabber = nullptr;
    }
    if ( m_previewParams.m_pointCloudViewer )
    {
		m_previewParams.m_pointCloudViewer->close();
        delete m_previewParams.m_pointCloudViewer;
		m_previewParams.m_pointCloudViewer = nullptr;
	}
    if ( m_previewParams.m_rectifyLogDataSlave ) { free( m_previewParams.m_rectifyLogDataSlave ); m_previewParams.m_rectifyLogDataSlave = NULL; }

    memset( &m_xPointCloudInfo, NULL, sizeof( PointCloudInfo ) );
#endif
    CAutoLock lock(m_previewParams.m_mutex);
    EnableAccuracy( FALSE );
    m_previewParams.Reset(pCallerDlg);

    GetDlgItem( IDC_SLD_PCL_PSIZE )->EnableWindow(FALSE);
    GetDlgItem( IDC_BT_Z_SET  )->EnableWindow(FALSE);
    GetDlgItem( IDC_BT_Z_RESET )->EnableWindow(FALSE);
    GetDlgItem( IDC_EDIT_ZFAR  )->EnableWindow(FALSE);
    GetDlgItem( IDC_EDIT_ZNEAR )->EnableWindow(FALSE);
    GetDlgItem(IDC_SNAPSHOT_BTN)->EnableWindow(FALSE);
	GetDlgItem(IDC_FRAME_SYNC)->EnableWindow(FALSE);
	GetDlgItem(IDC_PREVIEW_BTN)->EnableWindow(TRUE);
    GetDlgItem( IDC_CHK_MASTER )->EnableWindow(FALSE);
    GetDlgItem( IDC_COMBO_DEPTH_BIT_SEL_CTRL )->EnableWindow( ( ( CComboBox* )GetDlgItem( IDC_COMBO_DEPTH_BIT_SEL_CTRL ) )->GetCount() > 1 );
    GetDlgItem( IDC_COMBO_FRAME_RATE )->EnableWindow( ( ( CComboBox* )GetDlgItem( IDC_COMBO_FRAME_RATE ) )->GetCount() > 1 );
    GetDlgItem( IDC_DEPTH_FRAME_RATE )->EnableWindow( ( ( CComboBox* )GetDlgItem( IDC_DEPTH_FRAME_RATE ) )->GetCount() > 1 );
    GetDlgItem( IDC_COMBO_MODE_CONFIG )->EnableWindow( ( ( CComboBox* )GetDlgItem( IDC_COMBO_MODE_CONFIG ) )->GetCount() > 1 );

	GetDlgItem(IDC_CHECK_POINTCLOUD_VIEWER)->EnableWindow(TRUE);
	GetDlgItem(IDC_CHK_PCV_NOCOLOR)->EnableWindow(TRUE);
	GetDlgItem(IDC_CHK_PCV_SINGLE)->EnableWindow(TRUE);
	GetDlgItem(IDC_CHK_MULTI_SYNC)->EnableWindow(IsDevicePid(APC_PID_8053) || IsDevicePid(APC_PID_8059) || IsDevicePid(APC_PID_8062));

    OnCbnSelchangeComboFrameRate();
}

void CPreviewImageDlg::OnCbnSelchangeComboTColorStream()
{
	//
}

void CPreviewImageDlg::OnCbnSelchangeComboColorStream()
{
    if ( IsDevicePid( APC_PID_8060 ) )
    {
        GetDlgItem( IDC_COMBO_COLOR_STREAM )->EnableWindow( FALSE );
        return;
    }
    const BOOL bModeConfigLock = !GetDlgItem( IDC_COMBO_MODE_CONFIG )->IsWindowEnabled();

    if (IsStream0ColorPlusDepth() && ( ( CButton* )GetDlgItem(IDC_CHECK_COLOR_STREAM))->GetCheck() == BST_CHECKED)
    {
        GetDlgItem(IDC_CHECK_DEPTH0)->EnableWindow( bModeConfigLock );
#ifndef ESPDI_EG
        if ( IsDevicePid( APC_PID_8038 ) )
        {
            GetDlgItem(IDC_CHECK_DEPTH_FUSION)->EnableWindow(bModeConfigLock);
            GetDlgItem(IDC_CHECK_FUSION_SELECT_DLG)->EnableWindow(TRUE);
            GetDlgItem(IDC_CHECK_FUSION_SWPP)->EnableWindow(TRUE);
        }
		int CurColorSel = ((CComboBox*)GetDlgItem(IDC_COMBO_COLOR_STREAM))->GetCurSel();
		CPoint ColorRes = CPoint(m_pStreamColorInfo[CurColorSel].nWidth, m_pStreamColorInfo[CurColorSel].nHeight);
		CPoint DepthRes = CPoint(ColorRes.x / 2, ColorRes.y);

		int DepthIndex = 0;
		if (GetDepthStreamIndex(DepthRes) >= 0)
		{
			DepthIndex = GetDepthStreamIndex(DepthRes);
		}
		((CComboBox*)GetDlgItem(IDC_COMBO_DEPTH_STREAM))->SetCurSel(DepthIndex);
		((CButton*)GetDlgItem(IDC_CHECK_DEPTH0))->SetCheck(BST_CHECKED);
		((CButton*)GetDlgItem(IDC_CHECK_DEPTH1))->SetCheck(BST_UNCHECKED);
        ((CButton*)GetDlgItem(IDC_CHECK_K_COLOR_STREAM))->SetCheck(BST_UNCHECKED);
        
		GetDlgItem(IDC_CHECK_DEPTH1)->EnableWindow(FALSE);
        GetDlgItem(IDC_COMBO_DEPTH_STREAM)->EnableWindow(FALSE);
        GetDlgItem(IDC_CHECK_K_COLOR_STREAM)->EnableWindow(FALSE);
        GetDlgItem(IDC_COMBO_K_COLOR_STREAM)->EnableWindow(FALSE);
#endif
    }
    else
    {
        ((CButton*)GetDlgItem(IDC_CHECK_DEPTH0))->SetCheck(BST_UNCHECKED);
        GetDlgItem(IDC_CHECK_DEPTH0)->EnableWindow(FALSE);

        ((CButton*)GetDlgItem(IDC_CHECK_DEPTH_FUSION))->SetCheck(BST_UNCHECKED);
        GetDlgItem(IDC_CHECK_DEPTH_FUSION)->EnableWindow(FALSE);

        ((CButton*)GetDlgItem(IDC_CHECK_FUSION_SELECT_DLG))->SetCheck(BST_UNCHECKED);
        GetDlgItem(IDC_CHECK_FUSION_SELECT_DLG)->EnableWindow(FALSE);

        ((CButton*)GetDlgItem(IDC_CHECK_FUSION_SWPP))->SetCheck(BST_UNCHECKED);
        GetDlgItem(IDC_CHECK_FUSION_SWPP)->EnableWindow(FALSE);

        GetDlgItem(IDC_COMBO_DEPTH_STREAM)->EnableWindow( bModeConfigLock );
    }
    OnBnClickedCheckDepthFusion();

    if ( ( ( CButton* )GetDlgItem(IDC_CHECK_COLOR_STREAM))->GetCheck() != BST_CHECKED && bModeConfigLock )
    {
        GetDlgItem(IDC_COMBO_K_COLOR_STREAM)->EnableWindow( TRUE );      
        GetDlgItem(IDC_CHECK_K_COLOR_STREAM)->EnableWindow( TRUE );
        GetDlgItem(IDC_CHECK_DEPTH1)->EnableWindow( TRUE );
    } 
}

void CPreviewImageDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    CDialog::OnHScroll( nSBCode, nPos, pScrollBar );

    const int iValue = ( ( CSliderCtrl* )pScrollBar )->GetPos();

    switch ( pScrollBar->GetDlgCtrlID() )
    {
    case IDC_SLIDER_IR:
        {
            m_previewParams.m_IrPreState = iValue;
		    SetIRValue( iValue );
        }
        break;
    case IDC_SLD_DEPTH_ROI:
        {
            static int DEPTH_ROI_VALUE[] = { 1, 10, 20, 30, 40 };

            SetDlgItemInt( IDC_ST_DEPTH_ROI, DEPTH_ROI_VALUE[ iValue ] );

            for ( int i = 0; i < APC_MAX_DEPTH_STREAM_COUNT; ++i )
            {
                if ( m_previewParams.m_depthPreview[ i ].m_previewDlg )
                {
                    ( ( CDepthDlg* )m_previewParams.m_depthPreview[ i ].m_previewDlg )->SetDepthROI( DEPTH_ROI_VALUE[ iValue ] );
                }
            }
        }
        break;
    case IDC_SLD_PCL_PSIZE:
        {
            CString csPSize;

            csPSize.Format( L"%d/5", iValue );

            SetDlgItemText( IDC_ST_PSIZE, csPSize );

            if ( m_previewParams.m_pointCloudViewer ) m_previewParams.m_pointCloudViewer->SetPointSize( iValue );
        }
        break;
    }
}

void CPreviewImageDlg::SetIRValue(WORD value)
{
	if (value != m_irValue)
	{
        m_irValue = value;

		if (m_isPreviewed)
		{
			ChangeIRValue(value);
		}
	}
    std::ostringstream irValue;
	irValue << value << " / " << m_irRange.second;
	GetDlgItem(IDC_STATIC_IR_VALUE)->SetWindowText(CString(irValue.str().c_str()));
}

void CPreviewImageDlg::ChangeIRValue(WORD value)
{
	if (value != m_irRange.first)
	{
        APC_SetIRMode(m_hApcDI, &m_DevSelInfo, 0x3f);// 6 bits on for opening 6 ir
	}
    APC_SetCurrentIRValue(m_hApcDI, &m_DevSelInfo, value);
}

void CPreviewImageDlg::OnBnClickedCheckDepthFusion()
{
    const BOOL bModeConfigLock = !GetDlgItem( IDC_COMBO_MODE_CONFIG )->IsWindowEnabled();

    if (((CButton*)GetDlgItem(IDC_CHECK_DEPTH_FUSION))->GetCheck() == BST_CHECKED)
    {
        GetDlgItem(IDC_CHECK_FUSION_SELECT_DLG)->EnableWindow(TRUE);
        GetDlgItem(IDC_CHECK_FUSION_SWPP)->EnableWindow(TRUE);
        
        ((CButton*)GetDlgItem(IDC_CHECK_DEPTH0))->SetCheck(BST_CHECKED);
        GetDlgItem(IDC_CHECK_DEPTH0)->EnableWindow(FALSE);
        ((CButton*)GetDlgItem(IDC_CHECK_DEPTH1))->SetCheck(BST_CHECKED);
        GetDlgItem(IDC_CHECK_DEPTH1)->EnableWindow(FALSE);
        ((CButton*)GetDlgItem(IDC_CHECK_DEPTH2))->SetCheck(BST_CHECKED);
        GetDlgItem(IDC_CHECK_DEPTH2)->EnableWindow(FALSE);
    }
    else
    {
        GetDlgItem(IDC_CHECK_FUSION_SELECT_DLG)->EnableWindow(FALSE);
        GetDlgItem(IDC_CHECK_FUSION_SWPP)->EnableWindow(FALSE);

        if (IsStream0ColorPlusDepth())
        {
            GetDlgItem(IDC_CHECK_DEPTH0)->EnableWindow(bModeConfigLock);
        }

        GetDlgItem(IDC_CHECK_DEPTH1)->EnableWindow(bModeConfigLock);

#ifdef ESPDI_EG
        GetDlgItem(IDC_CHECK_DEPTH2)->EnableWindow(FALSE);
#else
        if ( IsDevicePid( APC_PID_8038 ) )
        {
            GetDlgItem(IDC_CHECK_DEPTH2)->EnableWindow( bModeConfigLock );
        }
#endif

    }
}

void CPreviewImageDlg::OnBnClickedRadioDepthMappingZdtable()
{
    GetDlgItem(IDC_STATIC_CAM_FOCUS)->EnableWindow(FALSE);
    GetDlgItem(IDC_EDIT_CAM_FOCUS)->EnableWindow(FALSE);
    GetDlgItem(IDC_STATIC_BASELINE_DIST)->EnableWindow(FALSE);
    GetDlgItem(IDC_EDIT_BLDIST0)->EnableWindow(FALSE);
    GetDlgItem(IDC_EDIT_BLDIST1)->EnableWindow(FALSE);
    GetDlgItem(IDC_EDIT_BLDIST2)->EnableWindow(FALSE);
    GetDlgItem(IDC_STATIC_BSDIST_UNIT)->EnableWindow(FALSE);
}

void CPreviewImageDlg::OnBnClickedRadioDepthMappingManual()
{
    GetDlgItem(IDC_STATIC_CAM_FOCUS)->EnableWindow(TRUE);
    GetDlgItem(IDC_EDIT_CAM_FOCUS)->EnableWindow(TRUE);
    GetDlgItem(IDC_STATIC_BASELINE_DIST)->EnableWindow(TRUE);
    GetDlgItem(IDC_EDIT_BLDIST0)->EnableWindow(TRUE);
    GetDlgItem(IDC_EDIT_BLDIST1)->EnableWindow(TRUE);
    GetDlgItem(IDC_EDIT_BLDIST2)->EnableWindow(TRUE);
    GetDlgItem(IDC_STATIC_BSDIST_UNIT)->EnableWindow(TRUE);
}

void CPreviewImageDlg::OnCbnSelchangeComboDepthOutputCtrl()
{
    const DEPTHMAP_TYPE eDepthMapType = ( ( CComboBox* )GetDlgItem(IDC_COMBO_DEPTH_OUTPUT_CTRL ) )->GetCurSel() ? TRANSFER_TO_GRAYRGB : TRANSFER_TO_COLORFULRGB;

    for ( int i = 0; i < APC_MAX_DEPTH_STREAM_COUNT; i++ )
	{
		if (m_previewParams.m_depthOption >= 0 && (m_previewParams.m_depthSwitch & (1 << i)) != 0)
		{
	        ( ( CDepthDlg* ) m_previewParams.m_depthPreview[ i ].m_previewDlg )->UpdateColorPalette( eDepthMapType );
        }
    }
    if ( m_previewParams.m_depthFusionPreview.m_previewDlg ) ( ( CDepthDlg* ) m_previewParams.m_depthFusionPreview.m_previewDlg )->UpdateColorPalette( eDepthMapType );
}

void CPreviewImageDlg::OnBnClickedBtZSet()
{
    int zFar  = GetDlgItemInt( IDC_EDIT_ZFAR  );
    int zNear = GetDlgItemInt( IDC_EDIT_ZNEAR );

    BOOL bInitialZDValue = TRUE;

    if ( zFar == 0 ) zFar = MAX_DEPTH_DISTANCE;

	for ( int i = 0; i < APC_MAX_DEPTH_STREAM_COUNT; i++ )
	{
		if (m_previewParams.m_depthOption >= 0 && (m_previewParams.m_depthSwitch & (1 << i)) != 0)
		{
            if ( bInitialZDValue )
            {
                AdjustNearFar( zFar, zNear, ( CDepthDlg* ) m_previewParams.m_depthPreview[ i ].m_previewDlg );

                bInitialZDValue = FALSE;
                m_ZFar  = zFar;
                m_ZNear = zNear;
            }
			( ( CDepthDlg* ) m_previewParams.m_depthPreview[ i ].m_previewDlg )->UpdateColorPalette( zFar, zNear );
		}
	}
    if ( m_previewParams.m_depthFusionPreview.m_previewDlg ) ( ( CDepthDlg* ) m_previewParams.m_depthFusionPreview.m_previewDlg )->UpdateColorPalette( zFar, zNear );
}

void CPreviewImageDlg::OnBnClickedBtZReset()
{
    SetDlgItemInt( IDC_EDIT_ZFAR, IsDevicePid( APC_PID_8038 ) ? 2000 : 1000 );
    SetDlgItemInt( IDC_EDIT_ZNEAR, 0 );

    OnBnClickedBtZSet();
}

void CPreviewImageDlg::OnBnClickedCheckPumaPostproc()
{
#ifndef ESPDI_EG
    APC_SetHWPostProcess(m_hApcDI, &m_DevSelInfo, ((CButton*)GetDlgItem(IDC_CHECK_HARDWARE_POSTPROC))->GetCheck() == BST_CHECKED);
#endif
}

void CPreviewImageDlg::OnBnClickedCheckPp()
{
}

void CPreviewImageDlg::OnBnClickedCheckFusionSwpp()
{
#ifndef ESPDI_EG
    bool enableFusionPostProc = ((CButton*)GetDlgItem(IDC_CHECK_FUSION_SWPP))->GetCheck() == BST_CHECKED;
    if (m_depthFusionHelper != nullptr)
    {
        m_depthFusionHelper->EnablePostProc(enableFusionPostProc);
    }
#endif
}

void CPreviewImageDlg::OnBnClickedCheckInterleaveMode()
{
#ifndef ESPDI_EG
    if ( !IsDevicePid( APC_PID_8040S ) && !IsDevicePid( APC_PID_8054 ) )
    {
		if (m_bIsInterLeaveMode) 
		{
			m_bPrevLowLigh = m_pPropertyDlg->IsLowLight();
			m_pPropertyDlg->SetLowLight(false);
			m_pPropertyDlg->LowLightEnable(false);
		}		

        APC_EnableInterleave( m_hApcDI, &m_DevSelInfo, m_bIsInterLeaveMode );
    }
#endif
}

void CPreviewImageDlg::OnCbnSelchangeComboFrameRate()
{
    { // inter-leave ui
        const BOOL bIsInterLeaveModeFPS = ( m_iInterLeaveModeFPS && GetDlgItemInt( IDC_COMBO_FRAME_RATE ) == m_iInterLeaveModeFPS );

        USB_PORT_TYPE eUSB_Port_Type = USB_PORT_TYPE_UNKNOW;

        APC_GetDevicePortType( m_hApcDI, &m_DevSelInfo, &eUSB_Port_Type );

        if ( bIsInterLeaveModeFPS )
        {
            ( ( CButton* )GetDlgItem( IDC_CHK_MULTI_SYNC ) )->SetCheck( BST_UNCHECKED );
            ( ( CButton* )GetDlgItem( IDC_CHK_MASTER ) )->SetCheck( BST_UNCHECKED );
        }

        ( ( CButton* )GetDlgItem( IDC_CHECK_INTERLEAVE_MODE ) )->SetCheck( bIsInterLeaveModeFPS ? BST_CHECKED : BST_UNCHECKED );
    }
    if ( ( ( CButton* )GetDlgItem( IDC_CHECK_COLOR_STREAM ) )->GetCheck() == BST_CHECKED &&
         ( ( CButton* )GetDlgItem( IDC_CHECK_DEPTH1       ) )->GetCheck() == BST_CHECKED &&
         !GetDlgItem( IDC_CHECK_DEPTH1 )->IsWindowEnabled() )
    { // L+D, depth-fps is same as color
        CComboBox* pCbx = ( CComboBox* )GetDlgItem( IDC_DEPTH_FRAME_RATE );
        CString csFPS;

        GetDlgItemText( IDC_COMBO_FRAME_RATE, csFPS );
        pCbx->ResetContent();
        pCbx->AddString( csFPS );
        pCbx->SetCurSel( NULL );
    }
}

void CPreviewImageDlg::OnBnClickedSnapshotBtn()
{
#if 0 // test APC_GetColorImage & APC_GetDepthImage
	
	std::vector<BYTE> colorBuffer;
	int nColorWidth = m_pStreamDepthInfo[m_previewParams.m_colorOption].nWidth;
	int nColorHeight = m_pStreamDepthInfo[m_previewParams.m_colorOption].nHeight;
	colorBuffer.resize(nColorWidth * nColorHeight * 2);

	int nColorSerialNumber;
	unsigned long int nColorImageSize;
	if (APC_OK == APC_GetColorImage(m_hApcDI, &m_DevSelInfo, &colorBuffer[0], &nColorImageSize, &nColorSerialNumber))
	{
		
		std::vector<BYTE> m_vecRGBImageBuf(nColorWidth * nColorHeight * 3, 0);

		if (APC_OK != APC_ColorFormat_to_RGB24(m_hApcDI, &m_DevSelInfo,
			&m_vecRGBImageBuf[NULL],
			&colorBuffer[NULL],
			colorBuffer.size(),
			nColorWidth,
			nColorHeight,
			APCImageType::COLOR_YUY2))
		{
			TRACE("APC_ColorFormat_to_RGB24 fail\n");
		}
		
		std::ostringstream FileName_Colorbmp;
		FileName_Colorbmp << GetCurrentModuleFolder().c_str() << "\\Color_TEST.bmp";
		if (colorBuffer.empty() == false && nColorWidth > 0 && nColorHeight)
		{
			SaveImage(m_vecRGBImageBuf, nColorWidth, nColorHeight, 24, FileName_Colorbmp.str().c_str(), false, Gdiplus::ImageFormatBMP);
		}
	}

	std::vector<BYTE> depthBuffer;
	int nDepthWidth = m_pStreamDepthInfo[m_previewParams.m_depthOption].nWidth;
	int nDepthHeight = m_pStreamDepthInfo[m_previewParams.m_depthOption].nHeight;
	depthBuffer.resize(nDepthWidth * nDepthHeight * 2);

	int nDepthSerialNumber;
	unsigned long int nDepthImageSize;
	if (APC_OK == APC_GetDepthImage(m_hApcDI, &m_DevSelInfo, &depthBuffer[0], &nDepthImageSize, &nDepthSerialNumber))
	{
		std::ostringstream FileName_Depth;
		FileName_Depth << GetCurrentModuleFolder().c_str() << "\\Depth_TEST.yuv";
		if (depthBuffer.empty() == false && nDepthWidth > 0 && nDepthHeight > 0)
		{
			SaveDepthYuv(depthBuffer, APCImageType::DEPTH_11BITS, nDepthWidth, nDepthHeight, FileName_Depth.str().c_str());
		}
	}
#endif

#ifndef ESPDI_EG
	PostMessage(WM_MSG_SNAPSHOT, (WPARAM)this);
#endif
}

BOOL CPreviewImageDlg::IsDevicePid( const int pid )
{
	return ( m_devinfoEx.wPID == pid && (m_devinfoEx.wVID == APC_VID_0x1E4E || m_devinfoEx.wVID == APC_VID_0x3438));
}
#ifndef ESPDI_EG
void CPreviewImageDlg::GetDepthIndexFromColorStream()
{
    if ( m_previewParams.m_colorOption > EOF ) return;

    if ( m_previewParams.m_depthOption == EOF || m_previewParams.m_depthOption >= m_depthStreamOptionCount ) return;

    if ( IsDevicePid( APC_PID_8054 ) || IsDevicePid( APC_PID_8040S ) ) // For 8054 and 8040S D+K mode
	{
        for ( int i = NULL; i < m_colorStreamOptionCount; i++ )
        {
            if ( ( m_pStreamColorInfo[ i ].nWidth  == m_pStreamDepthInfo[ m_previewParams.m_depthOption ].nWidth &&
                   m_pStreamColorInfo[ i ].nHeight == m_pStreamDepthInfo[ m_previewParams.m_depthOption ].nHeight ) ||
                 ( m_pStreamColorInfo[ i ].nWidth  == m_pStreamDepthInfo[ m_previewParams.m_depthOption ].nHeight &&
                   m_pStreamColorInfo[ i ].nHeight == m_pStreamDepthInfo[ m_previewParams.m_depthOption ].nWidth && m_previewParams.m_rotate ) )
            {
                m_previewParams.m_colorOption = i;

                break;
            }
        }
	}
}
#endif

void CPreviewImageDlg::OnBnClickedCheckRotateImg()
{
    const BOOL bRotate = ( ( ( CButton* )GetDlgItem( IDC_CHECK_ROTATE_IMG ) )->GetCheck() == BST_CHECKED );

    if ( m_previewParams.m_colorPreview.m_previewDlg  ) ( ( CColorDlg* )m_previewParams.m_colorPreview.m_previewDlg )->EnableRotate( bRotate );
    if ( m_previewParams.m_kcolorPreview.m_previewDlg ) ( ( CColorDlg* )m_previewParams.m_kcolorPreview.m_previewDlg )->EnableRotate( bRotate );

    for ( int i = 0; i < APC_MAX_DEPTH_STREAM_COUNT; ++i )
	{
		if ( m_previewParams.m_depthPreview[ i ].m_previewDlg ) ( ( CDepthDlg* )m_previewParams.m_depthPreview[ i ].m_previewDlg )->EnableRotate( bRotate );
	}
}

void CPreviewImageDlg::OnBnClickedCheckPointcloudViewer()
{
#ifndef ESPDI_EG
	if ( IsDevicePid( APC_PID_8054 ) || IsDevicePid( APC_PID_8040S ) || IsDevicePid( APC_PID_8053 ) || IsDevicePid( APC_PID_8036 ) )
	{
		((CButton*)GetDlgItem(IDC_RADIO_RECTIFY_DATA))->SetCheck(BST_CHECKED);
		OnBnClickedRadioRectifyAndRawData();
	}
#endif
}

void CPreviewImageDlg::OnBnClickedFrameSync()
{
#ifndef ESPDI_EG
    if( IsDevicePid( APC_PID_8054 ) || IsDevicePid( APC_PID_8040S ) )
	{
	    int CurDepthSel = ( ( CComboBox* )GetDlgItem( IDC_COMBO_DEPTH_STREAM ) )->GetCurSel();

        const APC_STREAM_INFO& xStreamInfo = ( ( CButton* )GetDlgItem( IDC_CHECK_K_COLOR_STREAM ) )->GetCheck() == BST_CHECKED ? 
                                                    m_pStreamKColorInfo[ ( ( CComboBox* )GetDlgItem( IDC_COMBO_K_COLOR_STREAM ) )->GetCurSel() ] :
                                                    m_pStreamColorInfo[ ( ( CComboBox* )GetDlgItem( IDC_COMBO_COLOR_STREAM ) )->GetCurSel() ];
	
        m_registerSettings->Framesync( m_hApcDI, &m_DevSelInfo, m_pStreamDepthInfo[CurDepthSel].nWidth, m_pStreamDepthInfo[CurDepthSel].nHeight,
				                       xStreamInfo.nWidth, xStreamInfo.nHeight,
				                       xStreamInfo.bFormatMJPG, GetDlgItemInt( IDC_COMBO_FRAME_RATE ), m_devinfoEx.wPID );
	}
#endif
}

void CPreviewImageDlg::OnCbnSelchangeComboModeConfig()
{
    ModeConfig::MODE_CONFIG::DECODE_TYPE eNoUse;

    CComboBox* pCbx = ( CComboBox* )GetDlgItem( IDC_COMBO_MODE_CONFIG );

    auto GetCurModeConfig = [ & ] ( const int iMode )
    {
        const std::vector< ModeConfig::MODE_CONFIG >& vecModeConfig = g_ModeConfig.GetModeConfigList( m_devinfoEx.wPID );

        for ( auto& ModeConfig : vecModeConfig )
        {
            if ( iMode == ModeConfig.iMode ) return ModeConfig;
        }
        return ModeConfig::MODE_CONFIG();
    };
    const ModeConfig::MODE_CONFIG& xModeConfig = GetCurModeConfig( pCbx->GetItemData( pCbx->GetCurSel() ) );

    auto CheckSreamOption = [ & ]( const APC_STREAM_INFO* pStreamInfo,
                                   const int iStreamCount,
                                   const ModeConfig::MODE_CONFIG::RESOLUTION& xResolution,
                                   const ModeConfig::MODE_CONFIG::DECODE_TYPE& eDecodeType )
    {
        for ( int i = NULL; i < iStreamCount; i++ )
        {
            if ( ( pStreamInfo[ i ].nWidth  == xResolution.Width  && pStreamInfo[ i ].nHeight == xResolution.Height ) ||
                 ( pStreamInfo[ i ].nWidth  == xResolution.Height && pStreamInfo[ i ].nHeight == xResolution.Width && m_previewParams.m_rotate ) )
            {
                if ( pStreamInfo == m_pStreamDepthInfo ) return i;
                else
                {
                    switch ( eDecodeType )
                    {
                    case ModeConfig::MODE_CONFIG::MJPEG: if ( pStreamInfo[ i ].bFormatMJPG  ) return i; else break;
                    case ModeConfig::MODE_CONFIG::YUYV:  if ( !pStreamInfo[ i ].bFormatMJPG ) return i; else break;
                    }
                }
            }
        }
        return EOF;
    };
    auto UpdateUI = [ & ] ( const APC_STREAM_INFO* pStreamInfo,
                            const int iStreamCount,
                            const ModeConfig::MODE_CONFIG::RESOLUTION& xResolution,
                            const ModeConfig::MODE_CONFIG::DECODE_TYPE& eDecodeType,
                            const int iComboBoxResIdx,
                            const int iCheckBoxResIdx )
    {
        const int iSelect = CheckSreamOption( pStreamInfo, iStreamCount, xResolution, eDecodeType );

        if ( iSelect > EOF ) ( ( CComboBox* )GetDlgItem( iComboBoxResIdx ) )->SetCurSel( iSelect );

        ( ( CButton* )GetDlgItem( iCheckBoxResIdx ) )->SetCheck( iSelect > EOF ? BST_CHECKED : BST_UNCHECKED );
    };
    auto UpdateFPS = [] ( CComboBox* pCbxFPS, const std::vector< int >& vecFps )
    {
        pCbxFPS->ResetContent();

        if ( vecFps.size() )
        {
            CString csFPS;
            int     iFPS_idx = NULL;

            for ( UINT i = NULL; i < vecFps.size(); i++ )
            {
                if ( 30 == vecFps[ i ] ) iFPS_idx = i;

                csFPS.Format( L"%d", vecFps[ i ] );

                pCbxFPS->AddString( csFPS );
            }
            pCbxFPS->SetCurSel( iFPS_idx );
        }
        pCbxFPS->EnableWindow( pCbxFPS->GetCount() > 1 );
    };
    UpdateFPS( ( ( CComboBox* )GetDlgItem( IDC_COMBO_FRAME_RATE ) ), xModeConfig.vecColorFps );
    UpdateFPS( ( ( CComboBox* )GetDlgItem( IDC_DEPTH_FRAME_RATE ) ), xModeConfig.vecDepthFps );

    if ( xModeConfig.vecDepthType.size() )
    {
        CComboBox* pDepthBitCbx = (CComboBox*)GetDlgItem( IDC_COMBO_DEPTH_BIT_SEL_CTRL );

        pDepthBitCbx->ResetContent();

        for ( auto& DepthType : xModeConfig.vecDepthType )
        {
            switch( DepthType )
            {
            case 8:  pDepthBitCbx->SetItemData( pDepthBitCbx->AddString( L"8 bits"  ), APC_DEPTH_DATA_8_BITS  ); break;
            case 11: pDepthBitCbx->SetItemData( pDepthBitCbx->AddString( L"11 bits" ), APC_DEPTH_DATA_11_BITS ); break;
            case 14: pDepthBitCbx->SetItemData( pDepthBitCbx->AddString( L"14 bits" ), APC_DEPTH_DATA_14_BITS ); break;
            }
        } 
        pDepthBitCbx->EnableWindow( xModeConfig.vecDepthType.size() > 1 );
        pDepthBitCbx->SetCurSel( NULL );
    }
    if ( !xModeConfig.csModeDesc.IsEmpty() )
    {
        SetDlgItemText( IDC_ST_MODE_CONFIG, xModeConfig.csModeDesc );
    }
    m_iInterLeaveModeFPS = xModeConfig.iInterLeaveModeFPS;

    CheckRadioButton( IDC_RADIO_RAW_DATA, IDC_RADIO_RECTIFY_DATA, xModeConfig.bRectifyMode ? IDC_RADIO_RECTIFY_DATA : IDC_RADIO_RAW_DATA );
    OnBnClickedRadioRectifyAndRawData();

    UpdateUI( m_pStreamColorInfo,  m_colorStreamOptionCount,  xModeConfig.L_Resolution, xModeConfig.eDecodeType_L, IDC_COMBO_COLOR_STREAM,   IDC_CHECK_COLOR_STREAM   );
    UpdateUI( m_pStreamKColorInfo, m_kcolorStreamOptionCount, xModeConfig.K_Resolution, xModeConfig.eDecodeType_K, IDC_COMBO_K_COLOR_STREAM, IDC_CHECK_K_COLOR_STREAM );
    UpdateUI( m_pStreamTColorInfo, m_tcolorStreamOptionCount, xModeConfig.T_Resolution, xModeConfig.eDecodeType_T, IDC_COMBO_T_COLOR_STREAM, IDC_CHECK_T_COLOR_STREAM );
    UpdateUI( m_pStreamDepthInfo,  m_depthStreamOptionCount,  xModeConfig.D_Resolution, eNoUse,                    IDC_COMBO_DEPTH_STREAM,   IDC_CHECK_DEPTH1         );

    OnCbnSelchangeComboColorStream();
    OnCbnSelchangeComboFrameRate();
}

void CPreviewImageDlg::OnBnClickedRadioRectifyAndRawData()
{
    const BOOL bRectify = ( ( CButton* )GetDlgItem( IDC_RADIO_RECTIFY_DATA ) )->GetCheck();

    CComboBox* pDepthBitCbx = (CComboBox*)GetDlgItem( IDC_COMBO_DEPTH_BIT_SEL_CTRL );

    for ( int i = NULL; i < pDepthBitCbx->GetCount(); i++ )
    {
        switch ( pDepthBitCbx->GetItemData( i ) )
        {
        case APC_DEPTH_DATA_8_BITS:
        case APC_DEPTH_DATA_8_BITS_RAW:     pDepthBitCbx->SetItemData( i, bRectify ? APC_DEPTH_DATA_8_BITS      : APC_DEPTH_DATA_8_BITS_RAW     ); break;
        case APC_DEPTH_DATA_8_BITS_x80:
        case APC_DEPTH_DATA_8_BITS_x80_RAW: pDepthBitCbx->SetItemData( i, bRectify ? APC_DEPTH_DATA_8_BITS_x80  : APC_DEPTH_DATA_8_BITS_x80_RAW ); break;
        case APC_DEPTH_DATA_11_BITS:
        case APC_DEPTH_DATA_11_BITS_RAW:    pDepthBitCbx->SetItemData( i, bRectify ? APC_DEPTH_DATA_11_BITS     : APC_DEPTH_DATA_11_BITS_RAW    ); break;
        case APC_DEPTH_DATA_14_BITS:
        case APC_DEPTH_DATA_14_BITS_RAW:    pDepthBitCbx->SetItemData( i, bRectify ? APC_DEPTH_DATA_14_BITS     : APC_DEPTH_DATA_14_BITS_RAW    ); break;
        case APC_DEPTH_DATA_OFF_RAW:
        case APC_DEPTH_DATA_OFF_RECTIFY:    pDepthBitCbx->SetItemData( i, bRectify ? APC_DEPTH_DATA_OFF_RECTIFY : APC_DEPTH_DATA_OFF_RAW        ); break;
        }
    }
}

/// **************************************** ///
/// Module Sync								 ///
/// Sync the Frame Number					 ///
/// Jacky_Tag								 ///
/// **************************************** ///
void CPreviewImageDlg::DoSerialNumCommand()
{
#ifndef ESPDI_EG
    if ( IsDevicePid( APC_PID_8053 ) || IsDevicePid( APC_PID_8059 ) || IsDevicePid( APC_PID_8062 ) )
    {
        APC_EnableSerialCount( m_hApcDI, &m_DevSelInfo, ( BST_CHECKED == ( ( CButton* )GetDlgItem( IDC_CHK_MULTI_SYNC ) )->GetCheck() ) );
    }
    else if ( IsDevicePid( APC_PID_8038 ) ) APC_EnableSerialCount( m_hApcDI, &m_DevSelInfo, FALSE );
#endif
}

/// **************************************** ///
/// Module Sync								 ///
/// Sync the Frame Number					 ///
/// Jacky_Tag #Module Sync					 ///
/// **************************************** ///
void CPreviewImageDlg::DoMultiModuleSynCommand()
{
#ifndef ESPDI_EG
    if ( IsDevicePid( APC_PID_8053 ) || IsDevicePid( APC_PID_8059 ) || IsDevicePid( APC_PID_8062 ) )
    {
        if ( ( BST_CHECKED == ( ( CButton* )GetDlgItem( IDC_CHK_MULTI_SYNC ) )->GetCheck() ) )
        {
            m_registerSettings->FrameSync8053_8059_Clock( m_hApcDI, &m_DevSelInfo );
        }
    }

	if (AutoModuleSyncManager::GetInstance()->IsAutoModuleSyncEnabled())
	{
		AutoModuleSyncManager::GetInstance()->FrameStart(m_DevSelInfo);
	}
#endif
}

void CPreviewImageDlg::DoMultiModuleSynReset()
{
#ifndef ESPDI_EG
    m_registerSettings->FrameSync8053_8059_Reset( m_hApcDI, &m_DevSelInfo );
#endif
}

/// **************************************** ///
/// Module Sync								 ///
/// Sync the Frame Number					 ///
/// Jacky_Tag								 ///
/// **************************************** ///
void CPreviewImageDlg::OnBnClickedChkMaster()
{
    if ( IsDevicePid( APC_PID_8053 ) || IsDevicePid( APC_PID_8059 ) || IsDevicePid( APC_PID_8062 ) )
    {
        if ( BST_CHECKED == ( ( CButton* )GetDlgItem( IDC_CHK_MASTER ) )->GetCheck() )
        {
            DoMultiModuleSynReset();

            SetTimer( CHECK_805359_MASTER_RESET_TIMER, CHECK_805359_MASTER_RESET_TIME_INTERVAL, nullptr );
        }
        else KillTimer( CHECK_805359_MASTER_RESET_TIMER );
    }
}

void CPreviewImageDlg::OnBnClickedChkIrmaxExt()
{
    if ( !IsDevicePid( APC_PID_SALLY ) && !IsDevicePid( APC_PID_8040S ) && !IsDevicePid(APC_PID_HYPATIA) && !IsDevicePid(APC_PID_NORA))
    {
        APC_SetIRMaxValue( m_hApcDI, &m_DevSelInfo, BST_CHECKED == ( ( CButton* )GetDlgItem( IDC_CHK_IRMAX_EXT ) )->GetCheck() ? MAX_IR_MAXIMUM : MAX_IR_DEFAULT );
    }

    if (BST_CHECKED == ((CButton*)GetDlgItem(IDC_CHK_IRMAX_EXT))->GetCheck())
    {
        if (IsDevicePid(APC_PID_NORA))
        {
            APC_GetIRMaxValue(m_hApcDI, &m_DevSelInfo, &m_maxIR);
            APC_SetIRMaxValue(m_hApcDI, &m_DevSelInfo, 0x7F);
        }
    }
    else
    {
        APC_SetIRMaxValue(m_hApcDI, &m_DevSelInfo, m_maxIR);
    }

    UpdateIRConfig();

	( ( CSliderCtrl* )GetDlgItem( IDC_SLIDER_IR ) )->SetRange( m_irRange.first, m_irRange.second, TRUE );

    SetIRValue( ( ( CSliderCtrl* )GetDlgItem( IDC_SLIDER_IR ) )->GetPos() );
}

void CPreviewImageDlg::OnBnClickedChkPcvNocolor()
{
    m_bPCV_NoColorStream = ( BST_CHECKED == ( ( CButton* )GetDlgItem( IDC_CHK_PCV_NOCOLOR ) )->GetCheck() );

    if ( m_bPCV_NoColorStream )
    {
        CheckRadioButton( IDC_CHK_PCV_NOCOLOR, IDC_CHK_PCV_SINGLE, IDC_CHK_PCV_NOCOLOR );

        ( ( CButton* )GetDlgItem( IDC_CHK_PCV_SINGLE ) )->SetCheck( FALSE );

        m_bPCV_SingleColor = FALSE;
    }
}

void CPreviewImageDlg::OnBnClickedChkPcvSingle()
{
    m_bPCV_SingleColor = ( BST_CHECKED == ( ( CButton* )GetDlgItem( IDC_CHK_PCV_SINGLE ) )->GetCheck() );

    if ( m_bPCV_SingleColor )
    {
        CheckRadioButton( IDC_CHK_PCV_NOCOLOR, IDC_CHK_PCV_SINGLE, IDC_CHK_PCV_SINGLE );

        ( ( CButton* )GetDlgItem( IDC_CHK_PCV_NOCOLOR ) )->SetCheck( FALSE );

        m_bPCV_NoColorStream = FALSE;
    }
}

void CPreviewImageDlg::EnableAccuracy( const BOOL bEnableAccuracy )
{
    static int Far  = NULL;
    static int Near = NULL;

    if ( !m_pAccuracyDlg ) return;

    if ( bEnableAccuracy )
    {
        if ( !Far  ) Far  = GetDlgItemInt( IDC_EDIT_ZFAR  );
        if ( !Near ) Near = GetDlgItemInt( IDC_EDIT_ZNEAR );

        SetDlgItemInt( IDC_EDIT_ZFAR,  MAX_DEPTH_DISTANCE );
        SetDlgItemInt( IDC_EDIT_ZNEAR, NULL );
    }
    else
    {
        if ( Far  ) SetDlgItemInt( IDC_EDIT_ZFAR,  Far  );
        if ( Near ) SetDlgItemInt( IDC_EDIT_ZNEAR, Near );

        Far  = NULL;
        Near = NULL;
    }
    m_pAccuracyDlg->Reset( bEnableAccuracy );

    OnBnClickedBtZSet();
}

void CPreviewImageDlg::SetFilterParam( DepthfilterParam& xDfParam )
{
    xDfParam.bytesPerPixel = 2;

    switch( APCImageType::DepthDataTypeToDepthImageType( m_previewParams.m_depthType ) )
    {
    case APCImageType::DEPTH_8BITS:
        {
            xDfParam.type = 1;
            xDfParam.bytesPerPixel = 1;
        }
        break;
    case APCImageType::DEPTH_11BITS:
        {
            xDfParam.type = 2;
        }
        break;
    case APCImageType::DEPTH_14BITS:
        {
            xDfParam.type = 3;
        }
        break;
    }
    CAutoLock lock(m_previewParams.m_mutex);
    *m_DfParam = xDfParam;
}


void CPreviewImageDlg::OnBnClickedChkMultiSync()
{
	// TODO: Add your control notification handler code here
	if (BST_CHECKED == ((CButton*)GetDlgItem(IDC_CHK_MULTI_SYNC))->GetCheck())
		m_pdlgVideoDeviceDlg->ChangeIMU_UI_FrameCount_SerialCount(TRUE);
	else
		m_pdlgVideoDeviceDlg->ChangeIMU_UI_FrameCount_SerialCount(FALSE);
}


void CPreviewImageDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	// TODO: Add your message handler code here
	if (!bShow) return;
	
	if (AutoModuleSyncManager::GetInstance()->IsAutoModuleSyncEnabled())
	{
		OnBnClickedPreviewBtn();
	}	
}

int CPreviewImageDlg::ReOpen_MappingIMU()
{
	int nRet = APC_OK;
	bool reopen = true;
	nRet = MappingIMU(reopen);
	return nRet;
}

int CPreviewImageDlg::MappingIMU(bool reopen)
{
	int nRet = APC_OK;
	//return nRet;

	int nIndex = m_pdlgVideoDeviceDlg->IMU_Device_Mapping();//
	unsigned short value = 0;
	nRet = APC_GetHWRegister(m_hApcDI, &m_DevSelInfo, 0xf306, &value, FG_Address_2Byte | FG_Value_1Byte);
	if (nRet != APC_OK)
		return nRet;
	if (value == nIndex + IMU_ID_INDEX_START)
	{
		m_pdlgVideoDeviceDlg->Update_IMU_Device_Mapping();
		return nRet;
	}

	value = nIndex + IMU_ID_INDEX_START;
	nRet = APC_SetHWRegister(m_hApcDI, &m_DevSelInfo, 0xf306, value, FG_Address_2Byte | FG_Value_1Byte);;
	if (nRet != APC_OK)
		return nRet;

	if (reopen)
		m_pdlgVideoDeviceDlg->IMU_Device_Reopen(m_hApcDI, m_DevSelInfo);
	m_pdlgVideoDeviceDlg->Update_IMU_Device_Mapping();
	return nRet;
}

void CPreviewImageDlg::InitAutoModuleSync()
{
	if (!AutoModuleSyncManager::GetInstance()->IsAutoModuleSyncEnabled()) return;

	((CButton*)GetDlgItem(IDC_CHK_MULTI_SYNC))->SetCheck(BST_CHECKED);

	if (AutoModuleSyncManager::GetInstance()->IsMasterDevice(m_DevSelInfo))
	{
		AutoModuleSyncManager::GetInstance()->RegisterFrameResetCallback(
		[&]()
		{
			((CButton*)GetDlgItem(IDC_CHK_MASTER))->SetCheck(BST_CHECKED);
			OnBnClickedChkMaster();
		}
		);
	}

}
