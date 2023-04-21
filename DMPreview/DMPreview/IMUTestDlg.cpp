// IMUTest.cpp: 實作檔案
//

#include "stdafx.h"
#include "APC_Test.h"
#include "IMUTestDlg.h"
#include "afxdialogex.h"
#include "utility\IMUData.h"
#include "utility\WindowsMfcUtility.h"
#include "PlyWriter.h"
#include "FrameSyncManager.h"
#include "PreviewImageDlg.h"

// Shifan: Another Quaternion to Euler
extern "C" {
	// add your #include statements here
#include "Quaternion1.h"
}

std::vector< IMUTestDlg::IMU_INFO > IMUTestDlg::s_vecIMU;
USHORT current_imu_pid;	// Rem 0910

#define WM_UPDATE_TEXT_MSG WM_USER + 100
#define WM_IMU_REENABLE WM_USER + 101
#define UPDATE_TEXT     0
#define UPDATE_CMD_TEXT 1

/* IMU Etron PID */
#define IMU_6AXIS_8040     0x5710
#define IMU_9AXIS_8060     0x5711
#define IMU_6AXIS_8040_NEW 0x0154
#define IMU_9AXIS_8062     0x0163
#define IMU_6AXIS_8063     0x0166

/* IMU Etron VID */
#define IMU_APC_VID_0x1E4E 0x1E4E	//9 Axis;//8062 / 8060;
#define IMU_APC_VID_0x0483 0x0483	//6 Axis;//8040;
#define IMU_APC_VID_0x3438 0x3438	//6 Axis;//8063;

IMPLEMENT_DYNAMIC(IMUTestDlg, CDialog)
BEGIN_MESSAGE_MAP(IMUTestDlg, CDialog)
    ON_WM_DESTROY()
    ON_MESSAGE( WM_UPDATE_TEXT_MSG, OnUpdateText )
    ON_MESSAGE( WM_IMU_REENABLE, OnIMUReEnable )
	ON_BN_CLICKED(IDOK, &IMUTestDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &IMUTestDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_SAVE_IMU_RAW_DATA, &IMUTestDlg::OnBnClickedButtonSaveImuRawData)
	ON_BN_CLICKED(IDC_BUTTON_GET_MODULE_NAME, &IMUTestDlg::OnBnClickedButtonGetFeatureReport)
	ON_BN_CLICKED(IDC_BUTTON_GET_FW_VERSION, &IMUTestDlg::OnBnClickedButtonGetFwVersion)
	ON_BN_CLICKED(IDC_BUTTON_GET_INPUT_REPORT, &IMUTestDlg::OnBnClickedButtonGetInputReport)
	ON_BN_CLICKED(IDC_BUTTON_SET_OUTPUT_REPORT, &IMUTestDlg::OnBnClickedButtonSetOutputReport)
	ON_BN_CLICKED(IDC_BUTTON_GET_IMU_STATUS, &IMUTestDlg::OnBnClickedButtonGetImuStatus)
	ON_BN_CLICKED(IDC_BUTTON_ENABLE_IMU, &IMUTestDlg::OnBnClickedButtonEnableImu)
	ON_BN_CLICKED(IDC_BUTTON_DISABLE_IMU, &IMUTestDlg::OnBnClickedButtonDisableImu)
	ON_BN_CLICKED(IDC_BUTTON_START_CALIBRATION, &IMUTestDlg::OnBnClickedButtonStartCalibration)
	ON_CBN_SELCHANGE(IDC_COMBO_SET_IMU_DATA_FORMAT, &IMUTestDlg::OnCbnSelchangeComboSetImuDataFormat)
	ON_BN_CLICKED(IDC_IMU_3D_RESET, &IMUTestDlg::OnBnClickedImu3dReset)
	ON_BN_CLICKED(IDC_CHECK_FRAME_SYNC, &IMUTestDlg::OnBnClickedCheckFrameSync)
	ON_BN_CLICKED(IDC_BUTTON_RTC_READ, &IMUTestDlg::OnBnClickedButtonRtcRead)
	ON_BN_CLICKED(IDC_BUTTON_RTC_WRITE, &IMUTestDlg::OnBnClickedButtonRtcWrite)
	ON_BN_CLICKED(IDC_BUTTON_ACC_FS_READ, &IMUTestDlg::OnBnClickedButtonAccFsRead)
	ON_BN_CLICKED(IDC_BUTTON_ACC_FS_WRITE, &IMUTestDlg::OnBnClickedButtonAccFsWrite)
	ON_BN_CLICKED(IDC_BUTTON_GYR_FS_READ, &IMUTestDlg::OnBnClickedButtonGyrFsRead)
	ON_BN_CLICKED(IDC_BUTTON_GYR_FS_WRITE, &IMUTestDlg::OnBnClickedButtonGyrFsWrite)
END_MESSAGE_MAP()

//IMUTestDlg::IMUTestDlg( const wchar_t* SN, const BOOL bIs9Axis, CWnd* pParent )
IMUTestDlg::IMUTestDlg(const wchar_t* SN, void*& hApcDI, DEVSELINFO& devSelInfo, ModeConfig::IMU_TYPE imu_type, CWnd* pParent, CPreviewImageDlg* pPreviewDlg)
	: CDialog(IDD_DIALOG_IMU, pParent), m_hApcDI(hApcDI), m_DevSelInfo(devSelInfo), m_IMU_TYPE(imu_type), m_bIsSyncIMU(FALSE), m_pPreviewDlg(pPreviewDlg)
{
	m_bIs9axis = (imu_type == ModeConfig::IMU_TYPE::IMU_9Axis);
	m_nIMU_ID_INDEX = -1;
	m_nIndex_s_vecIMU = -1;
	m_bIsDeviceMappingIMU = false;
	for (int i = 0; i < MAX_DEVICE_IMU; i++)
		m_IMUThread[i] = nullptr;

	if (1 == s_vecIMU.size())
	{	// Single Module;
		m_nIndex_s_vecIMU = 0;
		//m_nIMU_ID_INDEX = IMU_ID_INDEX_START;
		m_bIsDeviceMappingIMU = true;
		handle = s_vecIMU[NULL].hIMU;
		current_imu_pid = s_vecIMU[NULL].PID; // Rem 0910 keep current PID to check setup
		WriteSerialNumber((char*)SN);
		wchar_t szBuf[MAX_PATH] = { NULL };
		ReadSerialNumber((char*)szBuf);
		s_vecIMU[NULL].SN = szBuf;
		return;
	}

	// Multi Module;
	switch (m_IMU_TYPE)
	{
		case ModeConfig::IMU_TYPE::IMU_6Axis:	// 8040;
		{
			for (auto& i : s_vecIMU)
			{
				if (i.SN == SN)
				{
					handle = i.hIMU;
					current_imu_pid = s_vecIMU[NULL].PID; // Rem 0910 keep current PID to check setup
					break;
				}
			}
		}
		break;

		case ModeConfig::IMU_TYPE::IMU_9Axis:	// 8062 & 8060
		{
		}
		break;

	case ModeConfig::IMU_TYPE::IMU_NONE:
		break;

	default:
		break;
	}

}

IMUTestDlg::~IMUTestDlg()
{
	CAutoLock lock(m_mutex);

	if (m_isRecording) {
		m_isRecording = false;
		saveImuDataToFile(true);
	}

	//if (handle) 
	//{
	//	hid_close(handle);

	//	handle = nullptr;
	//}

	/* Free static HIDAPI objects. */
	//hid_exit();
}

void IMUTestDlg::InitIMU()
{
    UninitIMU();

    std::set< int > setIMU_VID = { IMU_APC_VID_0x0483, IMU_APC_VID_0x1E4E, IMU_APC_VID_0x3438 };
    std::set< int > setIMU_PID = { IMU_6AXIS_8040, IMU_6AXIS_8040_NEW, IMU_9AXIS_8060, IMU_9AXIS_8062, IMU_6AXIS_8063 };

    if ( 0 == hid_init() )
    {
        hid_device_info* devs    = hid_enumerate( NULL, NULL );
        hid_device_info* cur_dev = devs;
	    IMU_INFO         xIMUInfo;

		while ( cur_dev )
        {
            if ( setIMU_VID.find( cur_dev->vendor_id ) != setIMU_VID.end() && setIMU_PID.find( cur_dev->product_id ) != setIMU_PID.end() )
            {
                s_vecIMU.push_back( IMU_INFO() );

                s_vecIMU.rbegin()->hIMU = hid_open_path( cur_dev->path );
                s_vecIMU.rbegin()->VID  = cur_dev->vendor_id;
                s_vecIMU.rbegin()->PID  = cur_dev->product_id;
                s_vecIMU.rbegin()->SN   = cur_dev->serial_number;
		    }
		    cur_dev = cur_dev->next;
        }
        hid_free_enumeration( devs );
    }
}

void IMUTestDlg::UninitIMU()
{
    for ( auto& i : s_vecIMU ) hid_close( i.hIMU );

    hid_exit();

    s_vecIMU.clear();
}

void IMUTestDlg::SetImuDataFormatList(char format) 
{
	CComboBox* pImuDataFormatComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_SET_IMU_DATA_FORMAT);

	//pImuDataFormatComboBox->ResetContent();
	if (pImuDataFormatComboBox->GetCount() == 0) {
		pImuDataFormatComboBox->AddString(L"1. Raw data without offset");
		pImuDataFormatComboBox->AddString(L"2. Raw data with offset");
		pImuDataFormatComboBox->AddString(L"3. Offset data");
        if ( m_bIs9axis )
        {
		    pImuDataFormatComboBox->AddString(L"4. Dmp data without offset");
		    pImuDataFormatComboBox->AddString(L"5. Dmp data with offset");
        }
	}

	WORD index = format - 1;
	if (index < pImuDataFormatComboBox->GetCount()) {
		pImuDataFormatComboBox->SetCurSel(index);
	}
}

//int IMUTestDlg::initIMU()
//{
//
//	if (hid_init())
//		return -1;
//    m_bIs9axis = FALSE;
//	// Open the device using the VID, PID,
//	// and optionally the Serial number. 0483 ,5710
//	////handle = hid_open(0x4d8, 0x3f, L"12345");
//
//	if (!handle) {
//		handle = hid_open(0x0483, 0x5710, NULL); // 8040s IMU VID/PID
//		if (!handle) {
//            handle = hid_open(0x1E4E, 0x0154, NULL); // new 8040s IMU VID/PID
//		    if (!handle) {
//			    handle = hid_open(0x0483, 0x5711, NULL); // 8060 IMU VID/PID
//			    if (!handle) {
//                    m_text = "unable to open device\n";
//                    PostMessage( WM_UPDATE_TEXT_MSG, UPDATE_TEXT );
//				    return 1;
//			    }
//                else m_bIs9axis = TRUE;
//            }
//		}
//	}
//	
//	return 0;
//}
 
int IMUTestDlg::startGetImuData()
{
	CAutoLock lock(m_mutex);
	if (m_nIndex_s_vecIMU == -1)
		return 0;

	if (m_IMUThread[m_nIndex_s_vecIMU] && !m_isRunning) {
		m_IMUThread[m_nIndex_s_vecIMU]->join();
		m_IMUThread[m_nIndex_s_vecIMU] = nullptr;
	}

	EnableDataOutout(true);
	m_isRunning = true;
	m_isPause = false;

	m_imuOutputFormat = { 0 };
	GetImuDataOutoutFormat(&m_imuOutputFormat);
	SetImuDataFormatList(m_imuOutputFormat);
	m_outputDataSize = GetImuDataOutoutByte(m_imuOutputFormat);

	handle = s_vecIMU[m_nIndex_s_vecIMU].hIMU;
	if (handle && !m_IMUThread[m_nIndex_s_vecIMU]) {
		m_IMUThread[m_nIndex_s_vecIMU] = new std::thread(IMUTestDlg::IMUfunc, this);
	}

	return 0;
}

int IMUTestDlg::stopGetImuData()
{
	CAutoLock lock(m_mutex);
	if (m_nIndex_s_vecIMU == -1)
		return 0;

    m_isRunning = false;
	m_isPause = true;

    if (m_IMUThread[m_nIndex_s_vecIMU]) {
		m_IMUThread[m_nIndex_s_vecIMU]->join();
		delete m_IMUThread[m_nIndex_s_vecIMU];

		m_IMUThread[m_nIndex_s_vecIMU] = nullptr;
	}

	EnableDataOutout(false);

	return 0;
}

void IMUTestDlg::saveImuRawData(unsigned char* buf, WORD size)
{
	std::string textStrRawData;
	for (int i = 0; i < size; i++) {
		char textTmpBuff[10];
		sprintf(textTmpBuff, "%02x ", buf[i]);
		textStrRawData = textStrRawData.append(textTmpBuff, 0, 10);
	}

	textStrRawData += "\n";
	addImuDataToBuf(textStrRawData.c_str());
}

int IMUTestDlg::IMUCalibrationfunc(IMUTestDlg * pThis) {

	pThis->GetDlgItem(IDC_BUTTON_START_CALIBRATION)->EnableWindow(FALSE);

	char calibratingStatus = 0;
	pThis->CheckCalibratingStatus(&calibratingStatus);
	TRACE("CheckCalibratingStatus\n");

	int count = 0;
	if (calibratingStatus == 0x00) {

		BOOL isNeedRetartImuThread = false;

		if (pThis->m_IMUThread[pThis->m_nIndex_s_vecIMU] && pThis->m_isRunning && !pThis->m_isPause)
		{
			isNeedRetartImuThread = true;
			pThis->stopGetImuData();
		}

		pThis->StartCalibration();

		int sleepSec = 5;
		for (int i = 0; i < sleepSec; i++) {

			std::ostringstream stringStream;
			stringStream << "FW Calibration: " << 100 / sleepSec * i << "%";

            pThis->m_cmd_text = stringStream.str();

            pThis->PostMessage( WM_UPDATE_TEXT_MSG, UPDATE_CMD_TEXT );
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}

		pThis->m_cmd_text = "FW Calibration: 99%";
        pThis->PostMessage( WM_UPDATE_TEXT_MSG, UPDATE_CMD_TEXT );

		do {
			pThis->CheckCalibratingStatus(&calibratingStatus);
			if (calibratingStatus == 1) {

				std::this_thread::sleep_for(std::chrono::milliseconds(500));
				continue;
			}
			else if (calibratingStatus == 0) {
				break;
			}
		} while (count++ < 5);

		char calibrated = 0;
		pThis->ReadCalibrated(&calibrated);

		if (calibrated == 1) {
			pThis->m_cmd_text = "FW Calibration: succeed";
		}
		else
		{
			pThis->m_cmd_text = "FW Calibration: failed";
		}
        pThis->PostMessage( WM_UPDATE_TEXT_MSG, UPDATE_CMD_TEXT );

		if (isNeedRetartImuThread) {
			pThis->startGetImuData();
		}
	}

	pThis->GetDlgItem(IDC_BUTTON_START_CALIBRATION)->EnableWindow(TRUE);
	pThis->m_IMUCalibrationThread = nullptr;

	return 0;
}

void IMUTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

int IMUTestDlg::addImuDataToBuf(std::string text) {
	
	CAutoLock lock(m_mutex);
	oss << text;
	imuSaveDataCount++;
	if (imuSaveDataCount % 1000 == 0)
	{
		saveImuDataToFile(false);
	}

	return 0;
}

void IMUTestDlg::saveImuDataToFile(bool isCloseFile) {

	CAutoLock lock(m_mutex);
	if (fs)
	{
		fs << oss.str();
		fs.flush();
		if (isCloseFile) {
			fs.close();
			fs.clear();
		}
	}

	oss.str("");
	oss.clear();
}

WORD IMUTestDlg::GetImuDataOutoutByte(char ImuDataOutoutFormat)
{
	if (m_bIs9axis)
	{
		// 9axis YX8062 => IMU => quaternion
		return 58;
	}

	WORD outputDataSize = 27;
	switch (ImuDataOutoutFormat) {//
		case 4:
		case 5:
			outputDataSize = 58;
		break;

		default:
			outputDataSize = 27;
	}

	return outputDataSize;
}


void IMUTestDlg::GetFeatureReport(char* pData, size_t data_lenght)
{
	int res = 0;

	if (handle && handle->feature_report_length) {
		unsigned char *pBuf = nullptr;
		pBuf = (unsigned char *)calloc(handle->feature_report_length, sizeof(unsigned char));
		pBuf[0] = { 0x42 };
		res = hid_get_feature_report(handle, pBuf, handle->feature_report_length);
		memcpy(pData, pBuf + 1, data_lenght);
        free( pBuf );
	}

}


void IMUTestDlg::SendFeatureReport(const char* pData, size_t data_lenght)
{
	int res = 0;

	if (handle && handle->feature_report_length) {
		unsigned char *pBuf = nullptr;
		pBuf = (unsigned char *)calloc(handle->feature_report_length, sizeof(unsigned char));
		pBuf[0] = { 0x42 };
		memcpy(pBuf + 1, pData, data_lenght);;
		res = hid_send_feature_report(handle, pBuf, handle->feature_report_length);
        free( pBuf );
	}
}


void IMUTestDlg::GetInputReport(char* pData, size_t data_lenght)
{
	int res = 0;

	if (handle && handle->input_report_length) {
		unsigned char *pBuf = nullptr;
		pBuf = (unsigned char *)calloc(handle->input_report_length, sizeof(unsigned char));
		pBuf[0] = { 0x30 };
		res = hid_get_input_report(handle, pBuf, handle->input_report_length);
		memcpy(pData, pBuf, data_lenght);
        free( pBuf );
	}
}


void IMUTestDlg::SetOutputReport(const char* pData, size_t data_lenght)
{
	int res = 0;

	if (handle && handle->output_report_length) {
		unsigned char *pBuf = nullptr;
		pBuf = (unsigned char *)calloc(handle->output_report_length, sizeof(unsigned char));
		pBuf[0] = { 0x8D };
		pBuf++;
		memcpy(pBuf, pData, data_lenght);;
		res = hid_set_output_report(handle, pBuf, handle->output_report_length);
        free( pBuf );
	}
}


void IMUTestDlg::GetModuleName(char *pModuleName, WORD* pSize)
{
	SetFeatureDATA_Item setFeatureData[4] = {
		{ &GET_MODULE_NAME_0[0], (sizeof(GET_MODULE_NAME_0) / sizeof(GET_MODULE_NAME_0[0])) },
		{ &GET_MODULE_NAME_1[0], (sizeof(GET_MODULE_NAME_1) / sizeof(GET_MODULE_NAME_1[0])) },
		{ &GET_MODULE_NAME_2[0], (sizeof(GET_MODULE_NAME_2) / sizeof(GET_MODULE_NAME_2[0])) },
		{ &GET_MODULE_NAME_3[0], (sizeof(GET_MODULE_NAME_3) / sizeof(GET_MODULE_NAME_3[0])) }
	};

	char moduleName[256] = { 0 };
	//char* pBuf = &moduleName[0];
	WORD count = 0;

	for (int i = 0; i < 4; i++) {
		SendFeatureReport(setFeatureData[i].pData, setFeatureData[i].nDataLength);
		GetFeatureReport(&moduleName[count], 8);
		//pBuf += 8;
		count += 8;
	}

	memcpy(pModuleName, &moduleName[0], count);
	*pSize = count;
}

void IMUTestDlg::GetFwVersion(char *pFwVersion, WORD* pSize)
{
	SetFeatureDATA_Item setFeatureData[8] = {
		{ &GET_FW_VERSION_0[0], (sizeof(GET_FW_VERSION_0) / sizeof(GET_FW_VERSION_0[0])) },
		{ &GET_FW_VERSION_1[0], (sizeof(GET_FW_VERSION_1) / sizeof(GET_FW_VERSION_1[0])) },
		{ &GET_FW_VERSION_2[0], (sizeof(GET_FW_VERSION_2) / sizeof(GET_FW_VERSION_2[0])) },
		{ &GET_FW_VERSION_3[0], (sizeof(GET_FW_VERSION_3) / sizeof(GET_FW_VERSION_3[0])) },
		{ &GET_FW_VERSION_4[0], (sizeof(GET_FW_VERSION_4) / sizeof(GET_FW_VERSION_4[0])) },
		{ &GET_FW_VERSION_5[0], (sizeof(GET_FW_VERSION_5) / sizeof(GET_FW_VERSION_5[0])) },
		{ &GET_FW_VERSION_6[0], (sizeof(GET_FW_VERSION_6) / sizeof(GET_FW_VERSION_6[0])) },
		{ &GET_FW_VERSION_7[0], (sizeof(GET_FW_VERSION_7) / sizeof(GET_FW_VERSION_7[0])) }
	};

	char fwVersion[256] = { 0 };
	char* pBuf = &fwVersion[0];
	WORD count = 0;

	for (int i = 0; i < 8; i++) {
		SendFeatureReport(setFeatureData[i].pData, setFeatureData[i].nDataLength);
		GetFeatureReport(pBuf, 8);
		pBuf += 8;
		count += 8;
	}

	memcpy(pFwVersion, &fwVersion[0], count);
	*pSize = count;
}

void IMUTestDlg::GetImuDataOutoutStatus(char *pImuDataOutoutStatus)
{
	SetFeatureDATA_Item setFeatureData = { &READ_OUTPUT_STATUS[0], (sizeof(READ_OUTPUT_STATUS) / sizeof(READ_OUTPUT_STATUS[0])) };

	char status[8] = { 0 };

	SendFeatureReport(setFeatureData.pData, setFeatureData.nDataLength);
	GetFeatureReport(&status[0], setFeatureData.nDataLength);

	//memcpy(pImuDataOutoutStatus, &status[0], setFeatureData.nDataLength);
	*pImuDataOutoutStatus = status[0];
}

void IMUTestDlg::EnableDataOutout(BOOL isEnbale)
{
	SetFeatureDATA_Item setFeatureData;

	if (isEnbale) {
		setFeatureData = { &ENABLE_OUTPUT[0], (sizeof(ENABLE_OUTPUT) / sizeof(ENABLE_OUTPUT[0])) };
	}
	else {
		setFeatureData = { &DISABLE_OUTPUT[0], (sizeof(DISABLE_OUTPUT) / sizeof(DISABLE_OUTPUT[0])) };
	}

	SendFeatureReport(setFeatureData.pData, setFeatureData.nDataLength);
}

void IMUTestDlg::GetImuDataOutoutFormat(char *pImuDataOutoutFormat)
{
	SetFeatureDATA_Item setFeatureData = { &READ_OUTPUT_FORMAT[0], (sizeof(READ_OUTPUT_FORMAT) / sizeof(READ_OUTPUT_FORMAT[0])) };

	char status[8] = { 0 };

	SendFeatureReport(setFeatureData.pData, setFeatureData.nDataLength);
	GetFeatureReport(&status[0], setFeatureData.nDataLength);

	//memcpy(pImuDataOutoutFormat, &status[0], setFeatureData.nDataLength);
	*pImuDataOutoutFormat = status[0];
}

void IMUTestDlg::SetImuDataOutoutFormat(WORD format)
{

	SetFeatureDATA_Item setFeatureData;

	switch (format)
	{
	case 1:
		setFeatureData = { &SET_OUTPUT_FORMAT_1[0], (sizeof(SET_OUTPUT_FORMAT_1) / sizeof(SET_OUTPUT_FORMAT_1[0])) };
		break;

	case 2:
		setFeatureData = { &SET_OUTPUT_FORMAT_2[0], (sizeof(SET_OUTPUT_FORMAT_2) / sizeof(SET_OUTPUT_FORMAT_2[0])) };
		break;

	case 3:
		setFeatureData = { &SET_OUTPUT_FORMAT_3[0], (sizeof(SET_OUTPUT_FORMAT_3) / sizeof(SET_OUTPUT_FORMAT_3[0])) };
		break;

	case 4:
		setFeatureData = { &SET_OUTPUT_FORMAT_4[0], (sizeof(SET_OUTPUT_FORMAT_4) / sizeof(SET_OUTPUT_FORMAT_4[0])) };
		break;

	case 5:
		setFeatureData = { &SET_OUTPUT_FORMAT_5[0], (sizeof(SET_OUTPUT_FORMAT_5) / sizeof(SET_OUTPUT_FORMAT_5[0])) };
		break;

	default:
		return;
	}

	TRACE("SetImuDataOutoutFormat(): format = %d", format);
	SendFeatureReport(setFeatureData.pData, setFeatureData.nDataLength);

}

void IMUTestDlg::CheckCalibratingStatus(char *pCalibratingStatus)
{
	SetFeatureDATA_Item setFeatureData = { &CHECK_CALIBRATING_STATUS[0], (sizeof(CHECK_CALIBRATING_STATUS) / sizeof(CHECK_CALIBRATING_STATUS[0])) };

	char status[8] = { 0 };

	SendFeatureReport(setFeatureData.pData, setFeatureData.nDataLength);
	GetFeatureReport(&status[0], setFeatureData.nDataLength);

	//memcpy(pCalibratingStatus, &status[0], setFeatureData.nDataLength);
	*pCalibratingStatus = status[0];
}

void IMUTestDlg::StartCalibration()
{
	SetFeatureDATA_Item setFeatureData = { &START_CALIBRATION[0], (sizeof(START_CALIBRATION) / sizeof(START_CALIBRATION[0])) };
	SendFeatureReport(setFeatureData.pData, setFeatureData.nDataLength);
}

void IMUTestDlg::ReadCalibrated(char *pCalibrated)
{
	SetFeatureDATA_Item setFeatureData = { &READ_CALIBRATED[0], (sizeof(READ_CALIBRATED) / sizeof(READ_CALIBRATED[0])) };

	char status[8] = { 0 };

	SendFeatureReport(setFeatureData.pData, setFeatureData.nDataLength);
	GetFeatureReport(&status[0], setFeatureData.nDataLength);

	//memcpy(pCalibrated, &status[0], setFeatureData.nDataLength);
	*pCalibrated = status[0];
}

void IMUTestDlg::ReadSerialNumber( char* pSerialNumber )
{
    char status[ 8 ] = { 0 };

    int data_length = 0;

    for ( int i = 0; i < 5; i++ )
    {
	    SendFeatureReport( READ_SERIAL_NUMBER[ i ], sizeof( READ_SERIAL_NUMBER[ i ] ) );
	    GetFeatureReport( status, sizeof( READ_SERIAL_NUMBER[ i ] ) );

        data_length = i ? 8 : 6; // first package header is 0x24,0x03

        memcpy( pSerialNumber, &status[ 8 - data_length ], data_length );

        pSerialNumber += data_length;
    }
}

void IMUTestDlg::WriteSerialNumber( const char* pSerialNumber )
{
    int data_length = 0;
                                                  //[SS00, SS01, SS02, SS03, SS04, SS05 ]
    char WRITE_SERIAL_NUMBER[6][8] = { { 0x00, 0x15, 0x24, 0x03, 0x00, 0x00, 0x00, 0x00 },
                                                  //[SS06, SS07, SS08, SS09, SS10, SS11 ]
                                       { 0x00, 0x16, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
                                                  //[SS12, SS13, SS14, SS15, SS16, SS17 ]
                                       { 0x00, 0x17, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
                                                  //[SS18, SS19, SS20, SS21, SS22, SS23 ]
                                       { 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
                                                  //[SS24, SS25, SS26, SS27, SS28, SS29 ]
                                       { 0x00, 0x19, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
                                                  //[SS30, SS31, SS32, SS33, SS34, SS35 ]
                                       { 0x00, 0x1A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } };
    for ( int i = 0; i < 6; i++ )
    {
        data_length = i ? 6 : 4; // first package header is 0x24,0x03

        memcpy(&WRITE_SERIAL_NUMBER[ i ][ 8 - data_length ], pSerialNumber, data_length );

        pSerialNumber += data_length;

	    SendFeatureReport(WRITE_SERIAL_NUMBER[i], sizeof(WRITE_SERIAL_NUMBER[i]));
    }
    SendFeatureReport(WRITE_SERIAL_NUMBER_TO_FLASH, sizeof(WRITE_SERIAL_NUMBER_TO_FLASH));
}

void IMUTestDlg::OnDestroy()
{
    stopGetImuData();

    if ( m_IMUCalibrationThread )
    {
		m_IMUCalibrationThread->join();

		delete m_IMUCalibrationThread;

		m_IMUCalibrationThread = nullptr;
	}
	CDialog::OnDestroy();
}

LRESULT IMUTestDlg::OnUpdateText( WPARAM wparam,LPARAM lparam )
{
    CWnd* pWnd = GetDlgItem( wparam ? IDC_STATIC_IMU_OUPUT2 : IDC_STATIC_IMU_OUPUT );

	if ( pWnd )
    {
		pWnd->SetWindowText( CString( wparam ? m_cmd_text.c_str() : m_text.c_str() ) );
		pWnd->Invalidate();
	}
    return NULL;
}

LRESULT IMUTestDlg::OnIMUReEnable(WPARAM wparam, LPARAM lparam)
{
	TRACE("EnableDataOutout\n");
	startGetImuData();
	return NULL;
}

void IMUTestDlg::OnBnClickedButtonSaveImuRawData()
{
	CAutoLock lock(m_mutex);

	if (!m_isRecording) {
		DWORD timestamp = GetTickCount();
		std::ostringstream filename;
		filename << GetCurrentModuleFolder().c_str() << "\\imu_log" << "_" << timestamp << ".txt";
		fs.open(filename.str().c_str());
		if (fs)
		{
			oss.str("");
			oss.clear();
			m_isRecording = !m_isRecording;
			imuSaveDataCount = 0;
		}
	}
	else
	{
		m_isRecording = false;
		imuSaveDataCount = 0;
		saveImuDataToFile(true);
	}

	if (m_isRecording)
	{
		GetDlgItem(IDC_BUTTON_SAVE_IMU_RAW_DATA)->SetWindowText(L"Stop save");
	}
	else
	{
		GetDlgItem(IDC_BUTTON_SAVE_IMU_RAW_DATA)->SetWindowText(L"Save IMU RAW data");
	}
}


void IMUTestDlg::OnBnClickedButtonGetFeatureReport()
{
	WORD length = 0;
	char moduleName[256] = { 0 };
	GetModuleName(&moduleName[0], &length);
	TRACE("GetModuleName: %s\n", moduleName);

	std::string textStr = "GetModuleName: ";
	textStr.append(moduleName);

    m_cmd_text = textStr;
    PostMessage( WM_UPDATE_TEXT_MSG, UPDATE_CMD_TEXT );
}


void IMUTestDlg::OnBnClickedButtonGetFwVersion()
{
	WORD length = 0;
	char fwVersion[256] = { 0 };
	GetFwVersion(&fwVersion[0], &length);

	TRACE("GetFwVersion: %s\n", fwVersion);

	std::string textStr = "GetFwVersion: ";
	textStr.append(fwVersion);

    m_cmd_text = textStr;
    PostMessage( WM_UPDATE_TEXT_MSG, UPDATE_CMD_TEXT );
}


void IMUTestDlg::OnBnClickedButtonGetInputReport()
{
	int res = 0;

	if (handle && handle->input_report_length) {
		unsigned char *pBuf = nullptr;
		pBuf = (unsigned char *)calloc(handle->input_report_length, sizeof(unsigned char));
		pBuf[0] = { 0x30 };
		res = hid_get_input_report(handle, pBuf, handle->input_report_length);
        free( pBuf );
	}
}


void IMUTestDlg::OnBnClickedButtonSetOutputReport()
{
	int res = 0;

	if (handle && handle->output_report_length) {
		unsigned char *pBuf = nullptr;
		pBuf = (unsigned char *)calloc(handle->output_report_length, sizeof(unsigned char));
		pBuf[0] = { 0x8D };
		pBuf[1] = { 0x11 };
		pBuf[2] = { 0x22 };
		pBuf[3] = { 0x33 };
		pBuf[4] = { 0x44 };
		res = hid_set_output_report(handle, pBuf, handle->output_report_length);
        free( pBuf );
	}
}

void IMUTestDlg::OnBnClickedButtonGetImuStatus()
{
	char status = 0;
	GetImuDataOutoutStatus(&status);

	TRACE("OnBnClickedButtonGetImuStatus is %c\n", status);

	std::string textStr = "GetImuDataOutoutStatus: ";
	textStr.append(status == 0 ? "disable" : "enable");

    m_cmd_text = textStr;
    PostMessage( WM_UPDATE_TEXT_MSG, UPDATE_CMD_TEXT );
}

void IMUTestDlg::OnBnClickedButtonEnableImu()
{
	
	TRACE("EnableDataOutout\n");
	startGetImuData();

    m_cmd_text = "EnableDataOutout: done";
    PostMessage( WM_UPDATE_TEXT_MSG, UPDATE_CMD_TEXT );
}

void IMUTestDlg::OnBnClickedButtonDisableImu()
{

	TRACE("DisableDataOutout\n");
	stopGetImuData();

    m_cmd_text = "DisableDataOutout: done";
    PostMessage( WM_UPDATE_TEXT_MSG, UPDATE_CMD_TEXT );
}

void IMUTestDlg::OnBnClickedButtonStartCalibration()
{

	if (!m_IMUCalibrationThread) {
		m_IMUCalibrationThread = new std::thread(IMUTestDlg::IMUCalibrationfunc, this);
	}
}

void IMUTestDlg::OnCbnSelchangeComboSetImuDataFormat()
{
	CComboBox* pImuDataFormatComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_SET_IMU_DATA_FORMAT);
	int index = pImuDataFormatComboBox->GetCurSel();

    if ( EOF == index ) return;

	BOOL isNeedRetartImuThread = false;
	if (m_IMUThread[m_nIndex_s_vecIMU] && m_isRunning && !m_isPause)
	{
		isNeedRetartImuThread = true;
		stopGetImuData();
	}

	SetImuDataOutoutFormat(index + 1);

	if (isNeedRetartImuThread) {
		startGetImuData();
	}
}

BOOL IMUTestDlg::OnInitDialog()
{
    CDialog::OnInitDialog();
    //initIMU();
	memset(&m_chText_FrameCount, ' ', 32);
	m_chText_FrameCount[0] = '\0';
	snprintf(m_chText_FrameCount, sizeof(m_chText_FrameCount), "Frame Count");

	CRect rect;

	// Get size and position of the picture control  
	GetDlgItem(IDC_IMU_3D)->GetWindowRect(rect);

	// Convert screen coordinates to client coordinates  
	ScreenToClient(rect);

	// Create OpenGL Control window  
	m_imuViewer.glCreate(rect, this);

	if (m_bIs9axis)
	{
		// Setup the OpenGL Window's timer to render
		m_imuViewer.m_unpTimer = m_imuViewer.SetTimer(1, 1, 0);
	}
	else
	{
		GetDlgItem(IDC_IMU_3D_RESET)->ShowWindow(SW_HIDE);
	}

    if (m_pPreviewDlg->IsDevicePid(APC_PID_IVY2))
    {
        OnBnClickedButtonRtcRead();
        OnBnClickedButtonAccFsRead();
        OnBnClickedButtonGyrFsRead();
    }
    else
    {
        GetDlgItem(IDC_STATIC_RTC)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_STATIC_ACC_FS)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_STATIC_GYR_FS)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_EDIT_RTC_HOUR)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_EDIT_RTC_MINUTE)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_EDIT_RTC_SECOND)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_BUTTON_RTC_READ)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_BUTTON_RTC_WRITE)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_COMBO_ACC_FS)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_BUTTON_ACC_FS_READ)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_BUTTON_ACC_FS_WRITE)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_COMBO_GYR_FS)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_BUTTON_GYR_FS_READ)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_BUTTON_GYR_FS_WRITE)->ShowWindow(SW_HIDE);
    }

    return TRUE;
}

void IMUTestDlg::ChangeIMU_UI_FrameCount_SerialCount(bool enableModuleSync)
{
	m_enableModuleSync = enableModuleSync;

	memset(&m_chText_FrameCount, ' ', 32);
	m_chText_FrameCount[0] = '\0';
	if (enableModuleSync)
		snprintf(m_chText_FrameCount, sizeof(m_chText_FrameCount), "Frame Count (Serial Count)");
	else
		snprintf(m_chText_FrameCount, sizeof(m_chText_FrameCount), "Frame Count");
}

bool IMUTestDlg::Is9axis()
{
	return m_bIs9axis;
}

bool IMUTestDlg::Update_IMU_Device_Mapping(int nIMU_ID_INDEX)
{
	int nIndex = nIMU_ID_INDEX - IMU_ID_INDEX_START;
	if (s_vecIMU.size() <= nIndex)
	{
		//AfxMessageBox(_T("This could happen in camera plug-out!"));
		return false;
	}

	m_nIMU_ID_INDEX = nIMU_ID_INDEX;
	if (-1 == m_nIndex_s_vecIMU) m_nIndex_s_vecIMU = nIndex;
	
	m_nIndex_s_vecIMU = (m_nIndex_s_vecIMU + 1) % s_vecIMU.size();
	handle = s_vecIMU[m_nIndex_s_vecIMU].hIMU;
	IMU_SetHWRegister();
	startGetImuData();
	return true;
}

int IMUTestDlg::IMU_SetHWRegister()
{
	int nRet = APC_OK;
	if (!m_bIs9axis)
		return nRet;
	unsigned short value = 0;
	nRet = APC_GetHWRegister(m_hApcDI, &m_DevSelInfo, 0xf306, &value, FG_Address_2Byte | FG_Value_1Byte);
	if (nRet != APC_OK)
	{
		return nRet;
	}
	if (value == m_nIMU_ID_INDEX)
		return nRet;

	value = m_nIMU_ID_INDEX;
	nRet = APC_SetHWRegister(m_hApcDI, &m_DevSelInfo, 0xf306, value, FG_Address_2Byte | FG_Value_1Byte);;
	if (nRet != APC_OK)
		return nRet;
	return nRet;
}

void IMUTestDlg::IMU_Device_Reopen(void*& hApcDI, DEVSELINFO& devSelInfo)
{
	m_hApcDI = hApcDI;
	m_DevSelInfo = devSelInfo;
	InitIMU();
	if (m_nIMU_ID_INDEX >= IMU_ID_INDEX_START)
	{
		if (Update_IMU_Device_Mapping(m_nIMU_ID_INDEX) == true)
			startGetImuData();
	}
}

void IMUTestDlg::measureBeginQuaternion()
{
	//int retry_time = 5;//
	//while (readOneSample() <= 0 && retry_time-- > 0) {
	//	QThread::msleep(100);
	//}

	//Quaternion begin;
	//begin.w = m_quaternion[0];
	//begin.v[0] = m_quaternion[1];
	//begin.v[1] = m_quaternion[2];
	//begin.v[2] = m_quaternion[3];

	//Quaternion_inverse(&begin);
	//Quaternion_normalize(&begin, &begin);

	//m_quaternion_begin_inverse[0] = begin.w;
	//m_quaternion_begin_inverse[1] = begin.v[0];
	//m_quaternion_begin_inverse[2] = begin.v[1];
	//m_quaternion_begin_inverse[3] = begin.v[2];
	
	return;
}

int IMUTestDlg::IMUfunc(IMUTestDlg * pThis)
{
	CString strMsg;
	//strMsg.Format(_T("【IMUfunc ENTER】nIndex_s_vecIMU:%d"), nIndex_s_vecIMU);
	//AfxMessageBox(strMsg);
	int res = 0;
	unsigned char buf[256] = {0};

	while (res >= 0 && pThis->m_isRunning) {		
		if (pThis->m_isPause) {
			std::this_thread::sleep_for(std::chrono::milliseconds(20));
			continue;
		}

		if (pThis->handle == nullptr)
		{
			pThis->m_text = "Unable to read(handle == nullptr)";
			pThis->PostMessage(WM_UPDATE_TEXT_MSG, UPDATE_TEXT);
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
			pThis->PostMessage(WM_IMU_REENABLE, 0);
			continue;
		}

		if (pThis->Is9axis())
		{
			if ((pThis->m_nIMU_ID_INDEX == -1) || (pThis->m_nIMU_ID_INDEX < IMU_ID_INDEX_START))
			{
				pThis->m_text = "Mapping Unmatch!\r\nFor 9 axis module, please run \"Preview\" before IMU.";
				pThis->PostMessage(WM_UPDATE_TEXT_MSG, UPDATE_TEXT);
				std::this_thread::sleep_for(std::chrono::milliseconds(20));
				continue;
			}
		}

		TRY
		{
			res = hid_read(pThis->handle, buf, sizeof(buf));
		}
		CATCH(CException, e)
		{
			continue;
		}
		END_CATCH
		if (res == 0)
            pThis->m_text = "waiting...";
		else if (res < 0)
		{
			pThis->m_text = "Unable to read()";
			pThis->PostMessage(WM_UPDATE_TEXT_MSG, UPDATE_TEXT);

			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
			if (pThis->Is9axis())
			{
				/*if (pThis->m_nIMU_ID_INDEX >= IMU_ID_INDEX_START)
				{
					pThis->InitIMU();
					pThis->Update_IMU_Device_Mapping(pThis->m_nIMU_ID_INDEX);
				}*/
				pThis->PostMessage(WM_IMU_REENABLE, 0);
			}
			break; //continue;
		}
		else {
			IMUData imu;
			/// **************************************** ///
			/// Module 8062：MAX_G = 8.0					 ///
			/// Other Module ：MAX_G = 4.0				 ///
			/// Rem 0910 Default MaxG for 8062 is 8G	 ///
			/// Jacky_Tag								 ///
			/// **************************************** ///
			// Rem 0910 Default MaxG for 8062 is 8G
			if (current_imu_pid == IMU_9AXIS_8062)	//0x0163)
			{
				imu.setMaxG(8.0f);
			}
			else { imu.setMaxG(4.0f); }
			// Rem

			if (pThis->m_bIs9axis)
			{
				imu.parsePacket_Quaternion(buf);
			}
			else if (pThis->m_outputDataSize == 27) 
			{
				imu.parsePacket(buf, (pThis->m_imuOutputFormat == 3 ? false : true) );
			} 
			else if (pThis->m_outputDataSize == 58) 
			{
				imu.parsePacket_DMP(buf);
			}


			if (FrameSyncManager::GetInstance()->IsEnable())
			{
				FrameSyncManager::GetInstance()->SyncIMUCallback(pThis->m_hApcDI, pThis->m_DevSelInfo,
															     imu._frameCount,
															     std::bind(&IMUTestDlg::IMUDataCallback, pThis, imu));
			}
			else
			{
				pThis->IMUDataCallback(imu);
			}
				

			if (pThis->m_isRecording) {
				pThis->saveImuRawData(&buf[0], pThis->m_outputDataSize);
			}
		}
        pThis->PostMessage( WM_UPDATE_TEXT_MSG, UPDATE_TEXT );

		if (!pThis->m_isRunning) {
			break;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	pThis->m_isRunning = false;

	//strMsg.Format(_T("【IMUfunc Leave】nIndex_s_vecIMU:%d"), nIndex_s_vecIMU);
	//AfxMessageBox(strMsg);
	return 0;
}

void IMUTestDlg::IMUDataCallback(IMUData imu)
{
	char textBuff[256] = { 0 };

	if (m_bIs9axis)
	{
		if (!m_bIsDeviceMappingIMU)
		{
			if (imu._module_id != m_nIMU_ID_INDEX)
			{
				m_text = "Mapping Unmatch!";
				PostMessage(WM_UPDATE_TEXT_MSG, UPDATE_TEXT);
				Update_IMU_Device_Mapping(m_nIMU_ID_INDEX);
				std::this_thread::sleep_for(std::chrono::milliseconds(20));
				return;
			}

			m_bIsDeviceMappingIMU = true;
		}

		double Roll = 0;	//Angles.X
		double Pitch = 0;	//Angles.Y
		double Yaw = 0;	//Angles.Z
						
		if (m_bNeedInitQuaternion)
		{
			m_bNeedInitQuaternion = false;

			Quaternion begin;
			begin.w = imu._quaternion[0];
			begin.v[0] = imu._quaternion[1];
			begin.v[1] = imu._quaternion[2];
			begin.v[2] = imu._quaternion[3];

			Quaternion_inverse(&begin);
			Quaternion_normalize(&begin, &begin);

			m_QuaternionBeginInverse[0] = begin.w;
			m_QuaternionBeginInverse[1] = begin.v[0];
			m_QuaternionBeginInverse[2] = begin.v[1];
			m_QuaternionBeginInverse[3] = begin.v[2];
		}

		Quaternion raw;
		Quaternion begin;
		Quaternion output;

		raw.w = imu._quaternion[0];
		raw.v[0] = imu._quaternion[1];
		raw.v[1] = imu._quaternion[2];
		raw.v[2] = imu._quaternion[3];
		Quaternion_normalize(&raw, &raw);

		begin.w = m_QuaternionBeginInverse[0];
		begin.v[0] = m_QuaternionBeginInverse[1];
		begin.v[1] = m_QuaternionBeginInverse[2];
		begin.v[2] = m_QuaternionBeginInverse[3];
		Quaternion_normalize(&begin, &begin);
		Quaternion_multiply(&begin, &raw, &output);

		imu._quaternion[0] = output.w;
		imu._quaternion[1] = output.v[0];
		imu._quaternion[2] = output.v[1];
		imu._quaternion[3] = output.v[2];

		m_imuViewer.UpdateData(&imu);

		// ----- shifan: another version of quaternion.
		double theta[3];
		Quaternion_toEulerZYX(&output, theta);

		// ----- shifan: change to degree
		const double to_deg = 180.0 / M_PI;

		Roll = theta[0] * to_deg; 
		Pitch = theta[1] * to_deg; 
		Yaw = theta[2] * to_deg; 		

		snprintf(textBuff, sizeof(textBuff), "Frame count(%s):%d\nTime:%2d:%2d:%4d\nRoll[%10.3f], Pitch[%10.3f], Yaw[%10.3f]\nQuaternion:\n0:%15.8f\n1:%15.8f\n2:%15.8f\n3:%15.8f",
			m_chText_FrameCount,
			imu._frameCount,
			imu._min, imu._sec, imu._subSecond,
			Roll, Pitch, Yaw,
			imu._quaternion[0], imu._quaternion[1], imu._quaternion[2], imu._quaternion[3]);		
	}
	else if (m_outputDataSize == 27) 
	{
		float g = sqrt((imu._accelX * imu._accelX) + (imu._accelY*imu._accelY) + (imu._accelZ*imu._accelZ));
		snprintf(textBuff, sizeof(textBuff), "%s:%d\nTime:%2d:%2d:%2d:%4d\nAccel X:%1.3f Y:%1.3f Z:%1.3f Total:%1.3f\nGyro X:%4.2f Y:%4.2f Z:%4.2f\n",
			m_chText_FrameCount,
			imu._frameCount,
			imu._module_id, imu._min, imu._sec, imu._subSecond,
			imu._accelX, imu._accelY, imu._accelZ, g,
			imu._gyroScopeX, imu._gyroScopeY, imu._gyroScopeZ);
	}
	else if (m_outputDataSize == 58) 
	{
		snprintf(textBuff, sizeof(textBuff),
			"%s:%d\nTime:%2d:%2d:%2d:%4d\nAccel X:%04.1f\t\t     Y:%04.1f\t  Z:%04.1f\nGyro X:%04.0f\t\t     Y:%04.0f  \t  Z:%04.0f \nCompass X:%.1f   Y:%.1f  \t  Z:%.1f\nCompass_TBC X:%.1f\t     Y:%.1f  \t  Z:%.1f\nAccuracy_FLAG:%d\n\n",
			m_chText_FrameCount,
			imu._frameCount,
			imu._module_id, imu._min, imu._sec, imu._subSecond,
			imu._accelX, imu._accelY, imu._accelZ,
			imu._gyroScopeX, imu._gyroScopeY, imu._gyroScopeZ,
			imu._compassX, imu._compassY, imu._compassZ,
			imu._compassX_TBC, imu._compassY_TBC, imu._compassZ_TBC,
			imu._accuracy_FLAG);
	}

	m_text = textBuff;

	PostMessage(WM_UPDATE_TEXT_MSG, UPDATE_TEXT);
}

void IMUTestDlg::OnBnClickedImu3dReset()
{
	// TODO: Add your control notification handler code here
	m_bNeedInitQuaternion = true;
}


void IMUTestDlg::OnBnClickedCheckFrameSync()
{
	// TODO: Add your control notification handler code here
	FrameSyncManager::GetInstance()->SetEnabled(((CButton*)GetDlgItem(IDC_CHECK_FRAME_SYNC))->GetCheck() == BST_CHECKED);
	if (FrameSyncManager::GetInstance()->IsEnable())
	{
		FrameSyncManager::GetInstance()->RegisterDevice(m_hApcDI, m_DevSelInfo);
		FrameSyncManager::GetInstance()->SetIsInterleave(m_hApcDI, m_DevSelInfo, m_pPreviewDlg->IsInterLeaveMode());
	}
	else
	{
		FrameSyncManager::GetInstance()->UnregisterDevice(m_hApcDI, m_DevSelInfo);
	}
}

void IMUTestDlg::GetRTC(char* pHour, char* pMinute, char* pSecond)
{
    char rtc[8] = { 0 };

    SetFeatureDATA_Item setFeatureData = { &READ_RTC[0], (sizeof(READ_RTC) / sizeof(READ_RTC[0])) };
	SendFeatureReport(setFeatureData.pData, setFeatureData.nDataLength);
	GetFeatureReport(&rtc[0], setFeatureData.nDataLength);

    char rtcStr[9] = { 0 };
    sprintf(rtcStr, "%02d:%02d:%02d", rtc[0], rtc[1], rtc[2]);
    TRACE("OnBnClickedButtonRtcRead is %c\n", rtcStr);

    *pHour = rtc[0];
    *pMinute = rtc[1];
    *pSecond = rtc[2];
}

void IMUTestDlg::SetRTC(char hour, char minute, char second)
{
    char rtc[8];
    memcpy(&rtc[0], &WRITE_RTC[0], 8);

    rtc[3] = hour;
    rtc[4] = minute;
    rtc[5] = second;

    SetFeatureDATA_Item setFeatureData = { &rtc[0], (sizeof(WRITE_RTC) / sizeof(WRITE_RTC[0])) };
	SendFeatureReport(setFeatureData.pData, setFeatureData.nDataLength);
}

void IMUTestDlg::GetAccFs(char* pData)
{
    char accfs[8] = { 0 };

    SetFeatureDATA_Item setFeatureData = { &READ_ACC_FS[0], (sizeof(READ_ACC_FS) / sizeof(READ_ACC_FS[0])) };
	SendFeatureReport(setFeatureData.pData, setFeatureData.nDataLength);
	GetFeatureReport(&accfs[0], setFeatureData.nDataLength);

    TRACE("OnBnClickedButtonAccFsRead is %d\n", accfs[0]);
    *pData = accfs[0];
}

void IMUTestDlg::SetAccFs(char data)
{
    char accfs[8];
    memcpy(&accfs[0], &WRITE_ACC_FS[0], 8);

    accfs[3] = data;

    SetFeatureDATA_Item setFeatureData = { &accfs[0], (sizeof(WRITE_ACC_FS) / sizeof(WRITE_ACC_FS[0])) };
	SendFeatureReport(setFeatureData.pData, setFeatureData.nDataLength);
}

void IMUTestDlg::GetGyrFs(char* pData)
{
    char gyrfs[8] = { 0 };

    SetFeatureDATA_Item setFeatureData = { &READ_GYR_FS[0], (sizeof(READ_GYR_FS) / sizeof(READ_GYR_FS[0])) };
	SendFeatureReport(setFeatureData.pData, setFeatureData.nDataLength);
	GetFeatureReport(&gyrfs[0], setFeatureData.nDataLength);

    TRACE("OnBnClickedButtonGyrFsRead is %d\n", gyrfs[0]);
    *pData = gyrfs[0];
}

void IMUTestDlg::SetGyrFs(char data)
{
    char gyrfs[8];
    memcpy(&gyrfs[0], &WRITE_GYR_FS[0], 8);

    gyrfs[3] = data;

    SetFeatureDATA_Item setFeatureData = { &gyrfs[0], (sizeof(WRITE_GYR_FS) / sizeof(WRITE_GYR_FS[0])) };
	SendFeatureReport(setFeatureData.pData, setFeatureData.nDataLength);
}

void IMUTestDlg::OnBnClickedButtonRtcRead()
{
    char hour, minute, second;
    GetRTC(&hour, &minute, &second);

    CString csText;
    csText.Format(L"%02d", hour);
    SetDlgItemText(IDC_EDIT_RTC_HOUR, csText);
    csText.Format(L"%02d", minute);
    SetDlgItemText(IDC_EDIT_RTC_MINUTE, csText);
    csText.Format(L"%02d", second);
    SetDlgItemText(IDC_EDIT_RTC_SECOND, csText);
}

void IMUTestDlg::OnBnClickedButtonRtcWrite()
{
    char hour, minute, second;

    CString csText;

    GetDlgItemText(IDC_EDIT_RTC_HOUR, csText);
    char value = (char)_wtoi(csText);
    hour = value;

    GetDlgItemText(IDC_EDIT_RTC_MINUTE, csText);
    value = (char)_wtoi(csText);
    minute = value;

    GetDlgItemText(IDC_EDIT_RTC_SECOND, csText);
    value = (char)_wtoi(csText);
    second = value;

    SetRTC(hour, minute, second);
}

void IMUTestDlg::OnBnClickedButtonAccFsRead()
{
    char value = 0;
    GetAccFs(&value);

    CComboBox* pCbx = (CComboBox*)GetDlgItem(IDC_COMBO_ACC_FS);
    pCbx->SetCurSel(value);
}

void IMUTestDlg::OnBnClickedButtonAccFsWrite()
{
    CComboBox* pCbx = (CComboBox*)GetDlgItem(IDC_COMBO_ACC_FS);
    char value = pCbx->GetCurSel();
    SetAccFs(value);
}

void IMUTestDlg::OnBnClickedButtonGyrFsRead()
{
    char value = 0;
    GetGyrFs(&value);

    CComboBox* pCbx = (CComboBox*)GetDlgItem(IDC_COMBO_GYR_FS);
    pCbx->SetCurSel(value);
}

void IMUTestDlg::OnBnClickedButtonGyrFsWrite()
{
    CComboBox* pCbx = (CComboBox*)GetDlgItem(IDC_COMBO_GYR_FS);
    char value = pCbx->GetCurSel();
    SetGyrFs(value);
}
