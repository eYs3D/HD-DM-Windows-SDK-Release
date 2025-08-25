
#include "stdafx.h"
#include "APC_Test.h"
#include "VideoDeviceDlg.h"
#include "APC_TestDlg.h"
#include "AEAWB_PropertyDlg.h"
#include "SparseMode_PropertyDlg.h"
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
#include "SelfCalibration2Dlg.h"

class CopyG1ToG2Helper {
	friend class CVideoDeviceDlg;
	void* mInitializedDeviceHandle;
	DEVSELINFO mDeviceSelInfo;

	CopyG1ToG2Helper(void *initializedDeviceHandle, DEVSELINFO deviceSelInfo) {
		mInitializedDeviceHandle = initializedDeviceHandle;
		mDeviceSelInfo = deviceSelInfo;
	}
	~CopyG1ToG2Helper() {
		mInitializedDeviceHandle = nullptr;
	}

public:

	static constexpr int APC_USER_SETTING_OFFSET = 5;
	static constexpr int APC_ZD_TABLE_FILE_SIZE_11_BITS = 4096;
	static constexpr int COPY_RESULT_NONE = 0x0000;
	static constexpr int COPY_RESULT_YOFFSET = 0x0001;
	static constexpr int COPY_RESULT_RECTIFY = 0x0010;
	static constexpr int COPY_RESULT_ZD = 0x0100;
	static constexpr int COPY_RESULT_LOG = 0x1000;
	static constexpr int COPY_RESULT_ALL = 0x1111;

std::string CopyG1ToG2Helper::resultCodeToAlertString(int index, int copyResultCode) {
	std::string resultString = std::string("Index ");
	resultString.append(std::to_string(index));

	if (copyResultCode == COPY_RESULT_ALL)
		return std::move(resultString.append(" override ALL finished. \n"));
	else if (copyResultCode == COPY_RESULT_NONE)
		return std::move(resultString.append(" override ALL verify fail. \n"));

	resultString.append(" override partially equal ");
	if (copyResultCode & COPY_RESULT_YOFFSET)
		resultString.append(" YOFFSET ");
	if (copyResultCode & COPY_RESULT_RECTIFY)
		resultString.append(" RECTIFY ");
	if (copyResultCode & COPY_RESULT_ZD)
		resultString.append(" ZD ");
	if (copyResultCode & COPY_RESULT_LOG)
		resultString.append(" LOG ");
	resultString.append(" finish. \n");

	return std::move(resultString);
}

std::string CopyG1ToG2Helper::CopyAllFileToG2() {
	std::string resultString;
	for (int i = 0, resultCode = 0x0; i < APC_USER_SETTING_OFFSET; i++) {
		resultCode = CopyG1FileToG2(i);
		resultString.append(resultCodeToAlertString(i, resultCode));
	}
	return std::move(resultString);
}

int CopyG1ToG2Helper::CopyG1FileToG2(int fileIndex) {
	int resultCode = COPY_RESULT_NONE;
	if (fileIndex > APC_USER_SETTING_OFFSET) return false;

	auto bufferYOffset = new BYTE[APC_Y_OFFSET_FILE_SIZE];
	auto bufferYOffsetBackup = new BYTE[APC_Y_OFFSET_FILE_SIZE];
	void* EYSD = mInitializedDeviceHandle;
	auto devSelInfo = &mDeviceSelInfo;
	int actualYOffsetBufLen = 0;

	int ret = APC_GetYOffset(EYSD, devSelInfo, bufferYOffset, APC_Y_OFFSET_FILE_SIZE, &actualYOffsetBufLen, fileIndex);

	if (APC_OK != ret || actualYOffsetBufLen != APC_Y_OFFSET_FILE_SIZE) {
		fprintf(stderr, "### [Stage-YOffset] Read error \n");
	}
	else {
		fprintf(stderr, "### [Stage-YOffset] Read actualYOffsetBufLen %d file 3%d ret=%d\n", actualYOffsetBufLen, fileIndex, ret);

		memcpy(bufferYOffsetBackup, bufferYOffset, actualYOffsetBufLen);
#ifdef COPY_CLEAN
		memset(bufferYOffset, 0x0, APC_Y_OFFSET_FILE_SIZE);
#endif
		ret = APC_SetYOffset(EYSD, devSelInfo, bufferYOffset, APC_Y_OFFSET_FILE_SIZE, &actualYOffsetBufLen,
			fileIndex + APC_USER_SETTING_OFFSET);

		if (ret != APC_OK || actualYOffsetBufLen != APC_Y_OFFSET_FILE_SIZE) {
			fprintf(stderr, "### [Stage-YOffset] Write error \n");
		}
		else {
			fprintf(stderr, "### [Stage-YOffset] Write actualYOffsetBufLen %d file %d ret=%d\n", actualYOffsetBufLen,
				APC_Y_OFFSET_FILE_ID_0 + fileIndex + APC_USER_SETTING_OFFSET,
				ret);

			memset(bufferYOffset, 0xff, APC_Y_OFFSET_FILE_SIZE);
			ret = APC_GetYOffset(EYSD, devSelInfo, bufferYOffset, APC_Y_OFFSET_FILE_SIZE, &actualYOffsetBufLen,
				fileIndex + APC_USER_SETTING_OFFSET);

			if (ret != APC_OK || actualYOffsetBufLen != APC_Y_OFFSET_FILE_SIZE ||
				memcmp(bufferYOffset, bufferYOffsetBackup, actualYOffsetBufLen)) {
				fprintf(stderr, "### [Stage-YOffset] Verify error. Please check. \n");
			}
			else {
				fprintf(stderr, "### [Stage-YOffset] Verify successfully. \n");
				resultCode |= COPY_RESULT_YOFFSET;
			}
		}
	}

	delete[] bufferYOffset;
	delete[] bufferYOffsetBackup;

	auto bufferRectifyTable = new BYTE[APC_RECTIFY_FILE_SIZE];
	auto bufferRectifyTableBackup = new BYTE[APC_RECTIFY_FILE_SIZE];
	int actualRectifyBufLen = 0;
	ret = APC_GetRectifyTable(EYSD, devSelInfo, bufferRectifyTable, APC_RECTIFY_FILE_SIZE,
		&actualRectifyBufLen, fileIndex);
	if (ret != APC_OK || actualRectifyBufLen != APC_RECTIFY_FILE_SIZE) {
		fprintf(stderr, "### [Stage-Rectify] Read error \n");
	}
	else {
		fprintf(stderr, "### [Stage-Rectify] Read actualRectifyBufLen %d file 4%d ret=%d\n", actualRectifyBufLen,
			fileIndex, ret);

		memcpy(bufferRectifyTableBackup, bufferRectifyTable, actualRectifyBufLen);
#ifdef COPY_CLEAN
		memset(bufferRectifyTable, 0x0, APC_RECTIFY_FILE_SIZE);
#endif
		ret = APC_SetRectifyTable(EYSD, devSelInfo, bufferRectifyTable, APC_RECTIFY_FILE_SIZE,
			&actualRectifyBufLen, fileIndex + APC_USER_SETTING_OFFSET);

		if (ret != APC_OK || actualRectifyBufLen != APC_RECTIFY_FILE_SIZE) {
			fprintf(stderr, "### [Stage-Rectify] Write error \n");
		}
		else {
			fprintf(stderr, "### [Stage-Rectify] Write actualRectifyBufLen %d file %d ret=%d\n", actualRectifyBufLen,
				APC_RECTIFY_FILE_ID_0 + fileIndex + APC_USER_SETTING_OFFSET, ret);

			memset(bufferRectifyTable, 0xff, APC_RECTIFY_FILE_SIZE);

			ret = APC_GetRectifyTable(EYSD, devSelInfo, bufferRectifyTable, APC_RECTIFY_FILE_SIZE,
				&actualRectifyBufLen, fileIndex + APC_USER_SETTING_OFFSET);

			if (ret != APC_OK || actualRectifyBufLen != APC_RECTIFY_FILE_SIZE ||
				memcmp(bufferRectifyTable, bufferRectifyTableBackup, actualRectifyBufLen)) {
				fprintf(stderr, "### [Stage-Rectify] Verify error. Please check. \n");
			}
			else {
				fprintf(stderr, "### [Stage-Rectify] Verify successfully. \n");
				resultCode |= COPY_RESULT_RECTIFY;
			}
		}
	}

	delete[] bufferRectifyTable;
	delete[] bufferRectifyTableBackup;

	auto bufferZDTable = new BYTE[APC_ZD_TABLE_FILE_SIZE_11_BITS];
	auto bufferZDTableBackup = new BYTE[APC_ZD_TABLE_FILE_SIZE_11_BITS];
	int actualZDBufLen = 0;

	ZDTABLEINFO tableInfo; 
    tableInfo.nIndex = fileIndex;
	tableInfo.nDataType = APC_DEPTH_DATA_11_BITS;

	ret = APC_GetZDTable(EYSD, devSelInfo, bufferZDTable, APC_ZD_TABLE_FILE_SIZE_11_BITS,
		&actualZDBufLen, &tableInfo);

	if (ret != APC_OK || actualZDBufLen != APC_ZD_TABLE_FILE_SIZE_11_BITS) {
		fprintf(stderr, "### [Stage-ZD] Read error \n");
	}
	else {
		fprintf(stderr, "### [Stage-ZD] Read actualZDBufLen %d file 5%d ret=%d\n", actualZDBufLen,
			fileIndex, ret);

		memcpy(bufferZDTableBackup, bufferZDTable, APC_ZD_TABLE_FILE_SIZE_11_BITS);
#ifdef COPY_CLEAN
		memset(bufferZDTable, 0x0, APC_ZD_TABLE_FILE_SIZE_11_BITS);
#endif
		tableInfo.nIndex = fileIndex + APC_USER_SETTING_OFFSET;
		ret = APC_SetZDTable(EYSD, devSelInfo, bufferZDTable, APC_ZD_TABLE_FILE_SIZE_11_BITS,
			&actualZDBufLen, &tableInfo);

		if (ret != APC_OK || actualZDBufLen != APC_ZD_TABLE_FILE_SIZE_11_BITS) {
			fprintf(stderr, "### [Stage-ZD] Write error \n");
		}
		else {
			fprintf(stderr, "### [Stage-ZD] Write actualZDBufLen %d file %d ret=%d\n", actualZDBufLen,
				APC_ZD_TABLE_FILE_ID_0 + fileIndex + APC_USER_SETTING_OFFSET, ret);

			memset(bufferZDTable, 0xff, APC_ZD_TABLE_FILE_SIZE_11_BITS);
			tableInfo.nIndex = fileIndex + APC_USER_SETTING_OFFSET;
			ret = APC_GetZDTable(EYSD, devSelInfo, bufferZDTable, APC_ZD_TABLE_FILE_SIZE_11_BITS,
				&actualZDBufLen, &tableInfo);

			if (ret != APC_OK || actualZDBufLen != APC_ZD_TABLE_FILE_SIZE_11_BITS ||
				memcmp(bufferZDTable, bufferZDTableBackup, APC_ZD_TABLE_FILE_SIZE_11_BITS)) {
				fprintf(stderr, "### [Stage-ZD] Verify error. Please check. \n");
			}
			else {
				fprintf(stderr, "### [Stage-ZD] Verify successfully. \n");
				resultCode |= COPY_RESULT_ZD;
			}
		}
	}

	delete[] bufferZDTable;
	delete[] bufferZDTableBackup;

	auto bufferCalibrationLogData = new BYTE[APC_CALIB_LOG_FILE_SIZE];
	auto bufferCalibrationLogDataBackup = new BYTE[APC_CALIB_LOG_FILE_SIZE];
	int actualCalibrationLogDataBufLen = 0;

	ret = APC_GetLogData(EYSD, devSelInfo, bufferCalibrationLogData, APC_CALIB_LOG_FILE_SIZE,
		&actualCalibrationLogDataBufLen, fileIndex);

	if (APC_OK != ret || actualCalibrationLogDataBufLen != APC_CALIB_LOG_FILE_SIZE) {
		fprintf(stderr, "### [Stage-LOG] Read error \n");
	}
	else {
		fprintf(stderr, "### [Stage-LOG] Read actualCalibrationLogDataBufLen %d file 24%d ret=%d\n",
			actualCalibrationLogDataBufLen, fileIndex, ret);

		memcpy(bufferCalibrationLogDataBackup, bufferCalibrationLogData, APC_CALIB_LOG_FILE_SIZE);
#ifdef COPY_CLEAN
		memset(bufferCalibrationLogData, 0x0, APC_CALIB_LOG_FILE_SIZE);
#endif
		ret = APC_SetLogData(EYSD, devSelInfo, bufferCalibrationLogData, APC_CALIB_LOG_FILE_SIZE,
			&actualCalibrationLogDataBufLen, fileIndex + APC_USER_SETTING_OFFSET);

		if (actualCalibrationLogDataBufLen != APC_CALIB_LOG_FILE_SIZE || ret != APC_OK) {
			fprintf(stderr, "### [Stage-LOG] Write error \n");
		}
		else {
			fprintf(stderr, "### [Stage-LOG] Write actualCalibrationLogDataBufLen %d file %d ret=%d\n",
				actualCalibrationLogDataBufLen, APC_CALIB_LOG_FILE_ID_0 + fileIndex + APC_USER_SETTING_OFFSET, ret);

			memset(bufferCalibrationLogData, 0xff, APC_CALIB_LOG_FILE_SIZE);
			ret = APC_GetLogData(EYSD, devSelInfo, bufferCalibrationLogData, APC_CALIB_LOG_FILE_SIZE,
				&actualCalibrationLogDataBufLen, fileIndex + APC_USER_SETTING_OFFSET);

			if (ret != APC_OK || actualCalibrationLogDataBufLen != APC_CALIB_LOG_FILE_SIZE ||
				memcmp(bufferCalibrationLogData, bufferCalibrationLogDataBackup, APC_CALIB_LOG_FILE_SIZE)) {
				fprintf(stderr, "### [Stage-LOG] Verify error. Please check. \n");
			}
			else {
				fprintf(stderr, "### [Stage-LOG] Verify successfully. \n");
				resultCode |= COPY_RESULT_LOG;
			}
		}
	}

	delete[] bufferCalibrationLogData;
	delete[] bufferCalibrationLogDataBackup;

	return resultCode;
}
};

void onDeviceEventFn(UINT pid, UINT vid, BOOL bAttached, void* pUserData)
{
	CVideoDeviceDlg* theDlg = (CVideoDeviceDlg*)pUserData;
	CString szMessage;

	if(bAttached)
		szMessage.Format(_T("Device PID=0x%X, VID=0x%X attached.\nYou can call APC_Init2 to initialize the SDK and call APC_OpenEx2 to open the device."), pid, vid, bAttached);
	else
		szMessage.Format(_T("Device PID=0x%X, VID=0x%X dettached\nYou can call APC_Release to release resource allocated by the SDK."), pid, vid, bAttached);

	::OutputDebugString( szMessage );//AfxMessageBox(szMessage);
}

IMPLEMENT_DYNAMIC(CVideoDeviceDlg, CDialog)
BEGIN_MESSAGE_MAP(CVideoDeviceDlg, CDialog)
    ON_REGISTERED_MESSAGE( AFX_WM_CHANGE_ACTIVE_TAB, &CVideoDeviceDlg::OnChangeActiveTab ) 
    //ON_BN_CLICKED(IDC_FLASHDATA_READ, &CVideoDeviceDlg::OnBnClickedFlashdataRead)
    //ON_BN_CLICKED(IDC_FLASHDATA_WRITE, &CVideoDeviceDlg::OnBnClickedFlashdataWrite)
    ON_BN_CLICKED(IDC_RECTIFYLOG_GET, &CVideoDeviceDlg::OnBnClickedRectifylogGet)
    ON_BN_CLICKED(IDC_TEMPERATURE_GET, &CVideoDeviceDlg::OnBnClickedTemperatureGet)
    ON_MESSAGE(WM_MSG_AUTO_PREVIEW, OnAutoPreview)
    ON_WM_CLOSE()
    ON_WM_PAINT()
	ON_BN_CLICKED(IDC_EDIT_FORCE_OVERRIDE, &CVideoDeviceDlg::OnBnClickedEditForceOverride)
END_MESSAGE_MAP()

CVideoDeviceDlg::CVideoDeviceDlg(CWnd* pParent, bool enableSDKLog)
                                :CDialog(CVideoDeviceDlg::IDD, pParent),
                                m_hApcDI(nullptr),
                                m_pDevInfo( NULL ),
                                m_bIMU_Device_Sync(false),
                                m_TabPage4DepthFilter(TRUE),
                                m_TabPage4Property(TRUE),
                                m_TabPage4SparseMode(TRUE),
                                m_TabPage4Accuracy(TRUE),
                                m_TabPage4RegisterSetting(TRUE),
                                m_TabPage4IMU(TRUE),
                                m_TabPage4SelfCalibration2(TRUE),
                                m_TabPage4Audio(TRUE)
{
  /*APC_Init2 can enable the auto-restart function and register USB device events */
  APC_Init2(&m_hApcDI, enableSDKLog, false); 
  if (m_hApcDI)
	  APC_RegisterDeviceEvents(m_hApcDI, onDeviceEventFn, this);
} 

CVideoDeviceDlg::~CVideoDeviceDlg()
{
    if (m_hApcDI != nullptr)
    {
        APC_Release(&m_hApcDI);
        m_hApcDI = nullptr;
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

BOOL CVideoDeviceDlg::ReadUIConfig()
{
    BOOL bRet = TRUE;
    TCHAR buff[MAX_PATH];
    memset(buff, 0, MAX_PATH);
    ::GetModuleFileName(NULL, buff, sizeof(buff));
    CString strFolder = buff;
    CString strPath_Application = strFolder.Left(strFolder.ReverseFind(_T('\\')) + 1);

    CString profile_file = strPath_Application + _T("APC_UIConfig.ini");

    CString strAppName, strKeyName;
    CString strData;
    strAppName = _T("Setting");

    // Enable_TabPage_DepthFilter =
    strData.Empty();
    strKeyName = _T("Enable_TabPage_DepthFilter");
    GetPrivateProfileString(strAppName, strKeyName, _T(""), strData.GetBuffer(MAX_PATH), MAX_PATH, profile_file);
    m_TabPage4DepthFilter = (strData == _T("1"));
    strData.ReleaseBuffer();

    // Enable_TabPage_Property =
    strData.Empty();
    strKeyName = _T("Enable_TabPage_Property");
    GetPrivateProfileString(strAppName, strKeyName, _T(""), strData.GetBuffer(MAX_PATH), MAX_PATH, profile_file);
    m_TabPage4Property = (strData == _T("1"));
    strData.ReleaseBuffer();

    // Enable_TabPage_SparseMode =
    strData.Empty();
    strKeyName = _T("Enable_TabPage_SparseMode");
    GetPrivateProfileString(strAppName, strKeyName, _T(""), strData.GetBuffer(MAX_PATH), MAX_PATH, profile_file);
    m_TabPage4SparseMode = (strData == _T("1") || m_pDevInfo->wPID == APC_PID_IVY || m_pDevInfo->wPID == APC_PID_IVY2);
    strData.ReleaseBuffer();

    // Enable_TabPage_Accuracy =
    strData.Empty();
    strKeyName = _T("Enable_TabPage_Accuracy");
    GetPrivateProfileString(strAppName, strKeyName, _T(""), strData.GetBuffer(MAX_PATH), MAX_PATH, profile_file);
    m_TabPage4Accuracy = (strData == _T("1"));
    strData.ReleaseBuffer();

    // Enable_TabPage_RegisterSetting =
    strData.Empty();
    strKeyName = _T("Enable_TabPage_RegisterSetting");
    GetPrivateProfileString(strAppName, strKeyName, _T(""), strData.GetBuffer(MAX_PATH), MAX_PATH, profile_file);
    m_TabPage4RegisterSetting = (strData == _T("1"));
    strData.ReleaseBuffer();

    // Enable_TabPage_IMU =
    strData.Empty();
    strKeyName = _T("Enable_TabPage_IMU");
    GetPrivateProfileString(strAppName, strKeyName, _T(""), strData.GetBuffer(MAX_PATH), MAX_PATH, profile_file);
    m_TabPage4IMU = (strData == _T("1"));
    strData.ReleaseBuffer();

    // Enable_TabPage_Audio =
    strData.Empty();
    strKeyName = _T("Enable_TabPage_Audio");
    GetPrivateProfileString(strAppName, strKeyName, _T(""), strData.GetBuffer(MAX_PATH), MAX_PATH, profile_file);
    m_TabPage4Audio = (strData == _T("1"));
    strData.ReleaseBuffer();

    // Enable_TabPage_SelfCalibration2 =
    strData.Empty();
    strKeyName = _T("Enable_TabPage_SelfCalibration2");
    GetPrivateProfileString(strAppName, strKeyName, _T(""), strData.GetBuffer(MAX_PATH), MAX_PATH, profile_file);
    m_TabPage4SelfCalibration2 = (strData == _T("1"));
    strData.ReleaseBuffer();

    // Enable_SelfCalibration2_Runtime =
    strData.Empty();
    strKeyName = _T("Enable_SelfCalibration2_Runtime");
    GetPrivateProfileString(strAppName, strKeyName, _T(""), strData.GetBuffer(MAX_PATH), MAX_PATH, profile_file);
    m_bEnable_SK2_Runtime = (strData == _T("1"));
    strData.ReleaseBuffer();

    return TRUE;
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

  BOOL readOK = ReadUIConfig();
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
    DrawF( IDC_STATIC_FLASH_RW2 );
    if (m_pDevInfo->wPID == APC_PID_8063 || m_pDevInfo->wPID == APC_PID_80362 || m_pDevInfo->wPID == APC_PID_TARYN || m_pDevInfo->wPID == APC_PID_8077)
    {
        DrawF(IDC_STATIC_TEMPERATURE);
    }
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

    APC_GetDevicePortType(m_hApcDI, &m_DevSelInfo, &eUSB_Port_Type);
    APC_GetDeviceInfoEx(m_hApcDI, &m_DevSelInfo, &xdevinfoEx);

    CPreviewImageDlg* previewDlg = new CPreviewImageDlg(m_hApcDI, m_DevSelInfo, m_pDevInfo->nDevType, eUSB_Port_Type);
    previewDlg->m_pdlgVideoDeviceDlg = this;
    DepthFilterDlg* depthfilterDlg = new DepthFilterDlg(xdevinfoEx.wPID, eUSB_Port_Type, previewDlg);
    const char* version = APC_GetDepthFilterVersion(m_hApcDI, &m_DevSelInfo);
    depthfilterDlg->setVersion(version);

    AEAWB_PropertyDlg* aeAwbCtrlDlg = new AEAWB_PropertyDlg(m_hApcDI, m_DevSelInfo, *m_pDevInfo, &m_oTabPage);
    SparseMode_PropertyDlg* pSparseModeDlg = new SparseMode_PropertyDlg(m_hApcDI, m_DevSelInfo, *m_pDevInfo, &m_oTabPage);
    SelfCalibration2Dlg* pSelfCalibration2Dlg = new SelfCalibration2Dlg(m_hApcDI, m_DevSelInfo, *m_pDevInfo, &m_oTabPage, previewDlg);

    previewDlg->SetDepthFilterDlg(depthfilterDlg);
    previewDlg->SetPropertyDlg(aeAwbCtrlDlg);
    if (m_TabPage4SparseMode)
    {
        previewDlg->SetSparseModeDlg(pSparseModeDlg);
    }
    if (m_TabPage4SelfCalibration2)
    {
        previewDlg->SetSelfCalibration2Dlg(pSelfCalibration2Dlg);
    }

    depthfilterDlg->Create(depthfilterDlg->IDD, &m_oTabPage);
    previewDlg->Create(previewDlg->IDD, &m_oTabPage);
    aeAwbCtrlDlg->Create(aeAwbCtrlDlg->IDD, &m_oTabPage);
    pSparseModeDlg->Create(pSparseModeDlg->IDD, &m_oTabPage);
    pSelfCalibration2Dlg->Create(pSelfCalibration2Dlg->IDD, &m_oTabPage);

    m_childDlg.push_back(previewDlg);
    m_childDlg.push_back(depthfilterDlg);
    m_childDlg.push_back(aeAwbCtrlDlg);
    if (m_TabPage4SparseMode) m_childDlg.push_back(pSparseModeDlg);
    if (m_TabPage4SelfCalibration2) m_childDlg.push_back(pSelfCalibration2Dlg);


    m_oTabPage.AddTab(previewDlg, _T("Preview"));
    if (m_TabPage4DepthFilter)  m_oTabPage.AddTab(depthfilterDlg, _T("Depth Filter"));
    if (m_TabPage4Property)     m_oTabPage.AddTab(aeAwbCtrlDlg, _T("Property"));
    if (m_TabPage4SparseMode)     m_oTabPage.AddTab(pSparseModeDlg, _T("Sparse Mode"));
    if (m_TabPage4SelfCalibration2)    m_oTabPage.AddTab(pSelfCalibration2Dlg, _T("SelfCalibration2"));

    DistanceAccuracyDlg* DAlg = new DistanceAccuracyDlg(m_hApcDI, m_DevSelInfo, previewDlg);
    DAlg->Create(DAlg->IDD, &m_oTabPage);
    DAlg->EnableDepthList(xdevinfoEx.wPID == APC_PID_8038);
    m_childDlg.push_back(DAlg);
    if (m_TabPage4Accuracy) m_oTabPage.AddTab(DAlg, _T("Accuracy"));

    previewDlg->SetAccuracyDlg(DAlg);

    if (ModeConfig::IMU_NONE != g_ModeConfig.GetIMU_Type(xdevinfoEx.wPID))
    {
        wchar_t szBuf[MAX_PATH] = { NULL };
        GetDlgItemText(IDC_EDIT_SN, szBuf, MAX_PATH);
        //IMUTestDlg* pIMUTestDlg = new IMUTestDlg( szBuf, ModeConfig::IMU_9Axis == g_ModeConfig.GetIMU_Type( xdevinfoEx.wPID ), &m_oTabPage );
        IMUTestDlg* pIMUTestDlg = new IMUTestDlg(szBuf, m_hApcDI, m_DevSelInfo, g_ModeConfig.GetIMU_Type(xdevinfoEx.wPID), &m_oTabPage, previewDlg);
        pIMUTestDlg->Create(pIMUTestDlg->IDD, &m_oTabPage);
        pIMUTestDlg->startGetImuData();
        pIMUTestDlg->PauseGetImuData(true);

        m_childDlg.push_back(pIMUTestDlg);
        if (m_TabPage4IMU) m_oTabPage.AddTab(pIMUTestDlg, _T("IMU"));
    }
    if (xdevinfoEx.wPID == APC_PID_8060)
    {
        AudioDlg* audioDlg = new AudioDlg(&m_oTabPage);
        audioDlg->Create(audioDlg->IDD, &m_oTabPage);
        m_childDlg.push_back(audioDlg);
        if (m_TabPage4Audio) m_oTabPage.AddTab(audioDlg, _T("Audio"));
    }

    CRegisterAccessDlg* regAccessDlg = new CRegisterAccessDlg(m_hApcDI, m_DevSelInfo, &m_oTabPage);
    regAccessDlg->Create(regAccessDlg->IDD, &m_oTabPage);
    m_childDlg.push_back(regAccessDlg);
    if (m_TabPage4RegisterSetting) m_oTabPage.AddTab(regAccessDlg, _T("Register"));
    if (m_pDevInfo->wPID == APC_PID_8063 || m_pDevInfo->wPID == APC_PID_80362 || m_pDevInfo->wPID == APC_PID_TARYN || m_pDevInfo->wPID == APC_PID_8077)
    {
        GetDlgItem(IDC_TEMPERATURE_GET)->ShowWindow(SW_SHOW);
        GetDlgItem(IDC_EDIT_TEMPERATURE)->ShowWindow(SW_SHOW);
    }
    else
    {
        GetDlgItem(IDC_TEMPERATURE_GET)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_EDIT_TEMPERATURE)->ShowWindow(SW_HIDE);
    }
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
  
    if( APC_GetPidVid( m_hApcDI, &m_DevSelInfo, nProductID, &nVendorID ) == APC_OK ) 
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
    else AfxMessageBox(_T("APC_GetPidVid failed !!"));

    int nActualSNLenByByte = 0;
  
    wchar_t szBuf[ MAX_PATH ] = { NULL };
    if( APC_GetSerialNumber( m_hApcDI, &m_DevSelInfo, ( BYTE* )szBuf, MAX_PATH, &nActualSNLenByByte) == APC_OK )
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
    else AfxMessageBox( _T( "APC_GetSerialNumber Failed !!" ) );


    GetDlgItem( IDC_SLAVECHK )->EnableWindow( nProductID[ 0 ] == APC_PID_8060  ||
                                              nProductID[ 0 ] == APC_PID_8054  ||
                                              nProductID[ 0 ] == APC_PID_8040S );
    InitChildDlg();
}

void CVideoDeviceDlg::GetFWVersion()
{
    std::vector<char> szBuf(256, 0);

    int nActualLength = 0;  
  
    APC_GetFwVersion(m_hApcDI, &m_DevSelInfo, &szBuf[ 0 ], 256, &nActualLength);

    char* pHasSecondFW = strchr( &szBuf[ 0 ], '\n' );

    if ( !pHasSecondFW ) // Only one chip, 1-chip
    {
        strcat( &szBuf[ 0 ], "\n\n" );
    }
    else if ( !strchr( pHasSecondFW + 1, '\n' ) ) // The module has two chips, 2-chip
    {
        std::vector<char> firstChipStr(256, 0);
        std::vector<char> secondChipStr(256, 0);

        int chip2StrLen = strlen(pHasSecondFW);
        strncpy(&firstChipStr[0], &szBuf[0], (nActualLength - chip2StrLen));
        strncpy(&secondChipStr[0], (pHasSecondFW + 1), chip2StrLen);

        strcat(&firstChipStr[0], "\r\n");
        strcat(&secondChipStr[0], "\r\n");

        std::fill(szBuf.begin(), szBuf.end(), 0);
        strcat(&szBuf[0], &firstChipStr[0]);
        strcat(&szBuf[0], &secondChipStr[0]);
    }

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
//    AfxMessageBox(_T("APC_GetRectifyTable NOT support !!"));
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
//	    if ( APC_OK == APC_GetSlaveYOffset( m_hApcDI, &m_DevSelInfo, buffer, APC_Y_OFFSET_FILE_SIZE, &nActualLength, index ) )
//        {
//            csFileName.Format( L"FDATA_Offset_Slave_%d.bin", index );
//
//            WriteFile();
//
//            csMessage = _T( "APC_GetYOffset Success !!" );
//        }
//	    /*Only EX8054 & 8040S Master need to Set write to AR0330 sensor*/   
//	    if ( APC_OK == APC_SetPlumAR0330( m_hApcDI, &m_DevSelInfo, true ) )
//	    {
//		    if ( APC_OK == APC_GetYOffset( m_hApcDI, &m_DevSelInfo, buffer, APC_Y_OFFSET_FILE_SIZE, &nActualLength, index ) )
//            {
//                csFileName.Format( L"FDATA_Offset_ARO330_%d.bin", index );
//
//                WriteFile();
//
//                csMessage = _T( "APC_GetYOffset Success !!" );
//            }
//		    APC_SetPlumAR0330( m_hApcDI, &m_DevSelInfo, false );
//	    }
//	    nRet = APC_GetYOffset( m_hApcDI, &m_DevSelInfo, buffer, APC_Y_OFFSET_FILE_SIZE, &nActualLength, index );
//    
//        if ( csMessage.IsEmpty() ) csMessage = _T( "APC_GetYOffset Failed !!" );
//
//        if ( APC_OK == nRet )
//        {
//            csFileName.Format( L"FDATA_Offset_%d.bin", index );
//
//            csMessage = _T( "APC_GetYOffset Success !!" );
//        }
//    }
//  
//    else if( iSelect == RECTIFY_TABLE )
//    {
//        if ( APC_OK == APC_GetSlaveRectifyTable( m_hApcDI, &m_DevSelInfo, buffer, APC_RECTIFY_FILE_SIZE, &nActualLength, index ) )
//        {
//            csFileName.Format( L"FDATA_Rectify_Slave_%d.bin", index );
//
//            WriteFile();
//
//            csMessage = _T( "APC_GetRectifyTable Success !!" );
//        }
//	    /*Only EX8054 & 8040S Master need to Set write to AR0330 sensor*/   
//	    if ( APC_OK == APC_SetPlumAR0330( m_hApcDI, &m_DevSelInfo, true ) )
//	    {
//		    if ( APC_OK == APC_GetRectifyTable( m_hApcDI, &m_DevSelInfo, buffer, APC_RECTIFY_FILE_SIZE, &nActualLength, index ) )
//            {
//                csFileName.Format( L"FDATA_Rectify_ARO330_%d.bin", index );
//
//                WriteFile();
//
//                csMessage = _T( "APC_GetRectifyTable Success !!" );
//            }
//		    APC_SetPlumAR0330( m_hApcDI, &m_DevSelInfo, false );
//	    }
//	    nRet = APC_GetRectifyTable( m_hApcDI, &m_DevSelInfo, buffer, APC_RECTIFY_FILE_SIZE, &nActualLength, index );
//
//        if ( csMessage.IsEmpty() ) csMessage = _T( "APC_GetRectifyTable Failed !!" );
//
//        if ( APC_OK == nRet )
//        {
//            csFileName.Format( L"FDATA_Rectify_%d.bin", index );
//
//            csMessage = _T( "APC_GetRectifyTable Success !!" );
//        }
//    }
//    else if( iSelect == ZD_TABLE )
//    {
//        ZDTABLEINFO zdTableInfo { index, APC_DEPTH_DATA_11_BITS };
//
//        csMessage = _T( "APC_GetZDTable Failed !!" );
//
//        if( APC_OK == APC_GetZDTable( m_hApcDI, &m_DevSelInfo, buffer, sizeof( buffer ), &nActualLength, &zdTableInfo ) )
//        {   
//            csFileName.Format( L"FDATA_ZDTable_%d.bin", index );
//
//            csMessage = _T( "APC_GetZDTable Success !!" );
//        }
//    }
//    else if( iSelect == LOG_DATA )
//    {
//        if ( APC_OK == APC_GetSlaveLogData( m_hApcDI, &m_DevSelInfo, buffer, APC_CALIB_LOG_FILE_SIZE, &nActualLength, index ) )
//        {
//            csFileName.Format( L"FDATA_LogData_Slave_%d.bin", index );
//
//            WriteFile();
//
//            csMessage = _T( "APC_GetLogData Success !!" );
//        }
//	    /*Only EX8054 & 8040S Master need to Set write to AR0330 sensor*/   
//	    if ( APC_OK == APC_SetPlumAR0330( m_hApcDI, &m_DevSelInfo, true ) )
//	    {
//		    if ( APC_OK == APC_GetLogData( m_hApcDI, &m_DevSelInfo, buffer, APC_CALIB_LOG_FILE_SIZE, &nActualLength, index ) )
//            {
//                csFileName.Format( L"FDATA_LogData_ARO330_%d.bin", index );
//
//                WriteFile();
//
//                csMessage = _T( "APC_GetLogData Success !!" );
//            }
//		    APC_SetPlumAR0330( m_hApcDI, &m_DevSelInfo, false );
//	    }
//        nRet = APC_GetLogData( m_hApcDI, &m_DevSelInfo, buffer, APC_CALIB_LOG_FILE_SIZE, &nActualLength, index );
//
//        if ( csMessage.IsEmpty() ) csMessage = _T( "APC_GetLogData Failed !!" );
//
//        if ( APC_OK == nRet )
//        {
//            csFileName.Format( L"FDATA_LogData_%d.bin", index );
//
//            csMessage = _T( "APC_GetLogData Success !!" );
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
//    AfxMessageBox(_T("APC_GetRectifyTable NOT support !!"));
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
//        if( ReadFile( APC_Y_OFFSET_FILE_SIZE ) )
//        {
//            APC_SetSlaveYOffset( m_hApcDI, &m_DevSelInfo, buffer, APC_Y_OFFSET_FILE_SIZE, &nActualLength, index );
//
//            csMessage = _T( "APC_SetYOffset Success !!" );
//        }
//	    /*Only EX8054 & 8040S Master need to Set write to AR0330 sensor*/
//        if ( APC_OK == APC_SetPlumAR0330( m_hApcDI, &m_DevSelInfo, true ) )
//        {
//            csFileName.Format( L"FDATA_Offset_ARO330_%d.bin", index );
//
//	        if( ReadFile( APC_Y_OFFSET_FILE_SIZE ) )
//	        {
//		        APC_SetYOffset( m_hApcDI, &m_DevSelInfo, buffer, APC_Y_OFFSET_FILE_SIZE, &nActualLength, index );
//
//		        APC_SetPlumAR0330( m_hApcDI, &m_DevSelInfo, false );
//
//                csMessage = _T( "APC_SetYOffset Success !!" );
//	        }
//        }
//        csFileName.Format( L"FDATA_Offset_%d.bin", index );
//    
//        if ( csMessage.IsEmpty() ) csMessage = _T( "APC_SetYOffset Failed !!" );
//
//        if( ReadFile( APC_Y_OFFSET_FILE_SIZE ) )
//        {
//            if ( APC_OK == APC_SetYOffset( m_hApcDI, &m_DevSelInfo, buffer, APC_Y_OFFSET_FILE_SIZE, &nActualLength, index ) )
//                csMessage = _T( "APC_SetYOffset Success !!" );
//        }
//    }
//    else if( iSelect == RECTIFY_TABLE )
//    {
//        csFileName.Format( L"FDATA_Rectify_Slave_%d.bin", index );
//
//        if( ReadFile( APC_RECTIFY_FILE_SIZE ) )
//        {
//            APC_SetSlaveRectifyTable( m_hApcDI, &m_DevSelInfo, buffer, APC_RECTIFY_FILE_SIZE, &nActualLength, index );
//
//            csMessage = _T( "APC_SetRectifyTable Success !!" );
//        }
//	    /*Only EX8054 & 8040S Master need to Set write to AR0330 sensor*/
//        if ( APC_OK == APC_SetPlumAR0330( m_hApcDI, &m_DevSelInfo, true ) )
//        {
//            csFileName.Format( L"FDATA_Rectify_ARO330_%d.bin", index );
//
//	        if( ReadFile( APC_RECTIFY_FILE_SIZE ) )
//	        {
//		        APC_SetRectifyTable( m_hApcDI, &m_DevSelInfo, buffer, APC_RECTIFY_FILE_SIZE, &nActualLength, index );
//
//		        APC_SetPlumAR0330( m_hApcDI, &m_DevSelInfo, false );
//
//                csMessage = _T( "APC_SetRectifyTable Success !!" );
//	        }
//        }
//        csFileName.Format( L"FDATA_Rectify_%d.bin", index );
//
//        if ( csMessage.IsEmpty() ) csMessage = _T( "APC_SetRectifyTable Failed !!" );
//
//        if( ReadFile( APC_RECTIFY_FILE_SIZE ) )
//        {
//	        if ( APC_OK == APC_SetRectifyTable( m_hApcDI, &m_DevSelInfo, buffer, APC_RECTIFY_FILE_SIZE, &nActualLength, index ) )
//                csMessage = _T( "APC_SetRectifyTable Success !!" );
//        }
//    }
//    else if( iSelect == ZD_TABLE )
//    {
//	    ZDTABLEINFO zdTableInfo { index, APC_DEPTH_DATA_11_BITS };
//
//        csFileName.Format( L"FDATA_ZDTable_%d.bin", index );
//
//        if( ReadFile( sizeof( buffer ) ) )
//        {
//            csMessage = ( APC_OK == APC_SetZDTable( m_hApcDI, &m_DevSelInfo, buffer, sizeof( buffer ), &nActualLength, &zdTableInfo ) ) ?
//                        _T("APC_SetZDTable Success !!" ) :
//                        _T("APC_SetZDTable Failed !!" );
//        }
//    } 
//    else if( iSelect == LOG_DATA )
//    {
//        csFileName.Format( L"FDATA_LogData_Slave_%d.bin", index );
//
//        if( ReadFile( APC_RECTIFY_FILE_SIZE ) )
//        {
//            APC_SetSlaveLogData( m_hApcDI, &m_DevSelInfo, buffer, APC_CALIB_LOG_FILE_SIZE, &nActualLength, index );
//
//            csMessage = _T( "APC_SetLogData Success !!" );
//        }
//	    /*Only EX8054 & 8040S Master need to Set write to AR0330 sensor*/
//        if ( APC_OK == APC_SetPlumAR0330( m_hApcDI, &m_DevSelInfo, true ) )
//        {
//            csFileName.Format( L"FDATA_LogData_ARO330_%d.bin", index );
//
//	        if( ReadFile( APC_RECTIFY_FILE_SIZE ) )
//	        {
//		        APC_SetLogData( m_hApcDI, &m_DevSelInfo, buffer, APC_CALIB_LOG_FILE_SIZE, &nActualLength, index );
//
//		        APC_SetPlumAR0330( m_hApcDI, &m_DevSelInfo, false );
//
//                csMessage = _T( "APC_SetLogData Success !!" );
//	        }
//        }
//        csFileName.Format( L"FDATA_LogData_%d.bin", index );
//
//        if ( csMessage.IsEmpty() ) csMessage = _T( "APC_SetLogData Failed !!" );
//
//        if( ReadFile( APC_CALIB_LOG_FILE_SIZE ) )
//        {
//            if ( APC_OK == APC_SetLogData( m_hApcDI, &m_DevSelInfo, buffer, APC_CALIB_LOG_FILE_SIZE, &nActualLength, index ) )
//                csMessage = _T( "APC_SetLogData Success !!" );
//        }
//    }
//    AfxMessageBox( csMessage );
//#endif
//}

void CVideoDeviceDlg::OnBnClickedRectifylogGet()
{
#ifdef ESPDI_EG
    AfxMessageBox(_T("APC_GetRectifyLogData NOT support !!"));
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
	  nRet = APC_GetRectifyMatLogDataSlave(m_hApcDI, &m_DevSelInfo, &data, index);
  }
  else
  {
	  nRet = APC_GetRectifyMatLogData(m_hApcDI, &m_DevSelInfo, &data, index);
  }

  if( nRet == APC_OK) {
    
    // store to txt
    FILE *fl;
    errno_t err;
    char filename[128] = { 0 };

    sprintf_s(filename, 128, "RectifyLog%d.txt", index); 
    err = fopen_s(&fl, filename, "wt");  
    if (err != 0) AfxMessageBox(_T(" fopen_s failed !!"));
    else {
    
      int i = 0;
      if (data.type == 1){
          fprintf(fl, "Calibration Type : Manual Calibration\n");
      }else if (data.type == 2)
      {
          fprintf(fl, "Calibration Type : OEM Calibration\n");
      }else{
          fprintf(fl, "Calibration Type : User Calibration\n");
      }
      fprintf(fl, "Calibration Version : v%d,%d,%d,%d\n", data.version[0], data.version[1], data.version[2], data.version[3]);
      fprintf(fl, "Calibration Date : %d\n", data.Date);
    
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
      fprintf(fl,"ParameterRatio1 = %.8f\n",data.ParameterRatio[0]);
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
      fprintf(fl,"ParameterRatio2 = %.8f\n",data.ParameterRatio[1]);
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
  else AfxMessageBox(_T("APC_GetRectifyLogData Failed !!"));
#endif
}

void CVideoDeviceDlg::OnBnClickedTemperatureGet()
{
	CString csText;

	if (m_pDevInfo->wPID == APC_PID_8063)
	{
		float temp = 0.0f;

		unsigned char device_id_reg = 0x0F;
		unsigned short device_id_reg_val = 0x00;
		unsigned char temperature_reg = 0x00;
		unsigned short temperature_reg_val = 0x00;
		unsigned char nID = 0x90;
		int nSensorMode = 2;
		unsigned short temperature_reg_val_tmp = 0;
		bool is_negtive = false;
		int ret = APC_GetSensorRegister(m_hApcDI, &m_DevSelInfo, nID, (unsigned short)device_id_reg, &device_id_reg_val, FG_Address_1Byte | FG_Value_2Byte, nSensorMode);
		if (ret != APC_OK) {
			csText.Format(L"did error!");
			SetDlgItemText(IDC_EDIT_TEMPERATURE, csText);
			return;
		}
		unsigned short device_id_reg_val_reverse = ((((unsigned char*)&device_id_reg_val)[0]) << 8) + ((unsigned char*)&device_id_reg_val)[1];
		if (device_id_reg_val_reverse != 0x7500) {
			csText.Format(L"wrong did %04x", device_id_reg_val_reverse);
			SetDlgItemText(IDC_EDIT_TEMPERATURE, csText);
			return;
		}

		ret = APC_GetSensorRegister(m_hApcDI, &m_DevSelInfo, nID, (unsigned short)temperature_reg, &temperature_reg_val, FG_Address_1Byte | FG_Value_2Byte, nSensorMode);
		if (ret != APC_OK) {
			csText.Format(L"temp error!");
			SetDlgItemText(IDC_EDIT_TEMPERATURE, csText);
			return;
		}

		unsigned short temperature_reg_val_reverse = ((((unsigned char*)&temperature_reg_val)[0]) << 8) + ((unsigned char*)&temperature_reg_val)[1];
		temperature_reg_val_tmp = temperature_reg_val_reverse;
		if (temperature_reg_val_reverse & 0x8000) {
			temperature_reg_val_tmp = ~temperature_reg_val_tmp + 0x0010;
			is_negtive = true;
		}

		temp = (temperature_reg_val_tmp >> 8);
		if ((temperature_reg_val_tmp & 0x00f0) & 0x80) {
			temp += 0.5;
		}

		if ((temperature_reg_val_tmp & 0x00f0) & 0x40) {
			temp += 0.25;
		}

		if ((temperature_reg_val_tmp & 0x00f0) & 0x20) {
			temp += 0.125;
		}

		if ((temperature_reg_val_tmp & 0x00f0) & 0x10) {
			temp += 0.0625;
		}

		if (is_negtive) {
			temp = 0.0 - temp;
		}
		csText.Format(L"%.2f", temp);
		csText = csText + _T("°C");
		SetDlgItemText(IDC_EDIT_TEMPERATURE, csText);
	}
    else if (m_pDevInfo->wPID == APC_PID_80362 || m_pDevInfo->wPID == APC_PID_TARYN || m_pDevInfo->wPID == APC_PID_8077)
    {
        int ret = APC_OK;
        unsigned short TempeRegAddr = 0x00;
        unsigned short TempeRegVal = 0x00;
        bool is_negtive = false;
        constexpr unsigned short THERMAL_SENSOR_ID = 0x90;
        int SENSOR_BOTH = 2;
        float fTemperature = 0.0f;

        ret = APC_GetSensorRegister(m_hApcDI, &m_DevSelInfo, THERMAL_SENSOR_ID,
            TempeRegAddr, &TempeRegVal, FG_Address_1Byte | FG_Value_2Byte, SENSOR_BOTH);

        if (ret == APC_OK)
        {
            unsigned short temperature_reg_val_reverse = ((((unsigned char*)&TempeRegVal)[0]) << 8) + ((unsigned char*)&TempeRegVal)[1];
            TempeRegVal = temperature_reg_val_reverse;
            TempeRegVal >>= 5;
            fTemperature = (float)TempeRegVal * 0.125;
            csText.Format(L"%.2f", fTemperature);
            csText = csText + _T("°C");
            SetDlgItemText(IDC_EDIT_TEMPERATURE, csText);
        }
        else
        {
            csText.Format(L"temp error!");
            SetDlgItemText(IDC_EDIT_TEMPERATURE, csText);
            return;
        }
    }
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

void CVideoDeviceDlg::IMU_Device_Reopen(void*& hApcDI, DEVSELINFO& devSelInfo)
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
			pIMUTestDlg->IMU_Device_Reopen(hApcDI, devSelInfo);
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

void CVideoDeviceDlg::OnBnClickedEditForceOverride()
{
	int result = AfxMessageBox(_T("Dangerous action !! Pressed Yes, all your calibration files in G2 section will be override by the value in G1."), 
                               MB_OKCANCEL);
	if (result == IDOK) {
		CopyG1ToG2Helper helper(m_hApcDI, m_DevSelInfo);
		std::string resultString;

		CProgressCtrl progressIndicator;
		progressIndicator.Create(WS_CHILD | WS_VISIBLE, CRect(17, 187, 230, 195), this, IDC_EDIT_FORCE_OVERRIDE);
		progressIndicator.SetRange(0, CopyG1ToG2Helper::APC_USER_SETTING_OFFSET);
		progressIndicator.SetStep(1);

	    for (int i = 0, resultCode = 0x0; i < CopyG1ToG2Helper::APC_USER_SETTING_OFFSET; i++) {
            resultCode = helper.CopyG1FileToG2(i);
		    resultString.append(helper.resultCodeToAlertString(i, resultCode));
			Sleep(500);
			progressIndicator.StepIt();
        }
		progressIndicator.SetPos(0);

		std::wstring wsResult(resultString.begin(), resultString.end());
		result = AfxMessageBox(wsResult.c_str(), MB_OK);
	}
}
