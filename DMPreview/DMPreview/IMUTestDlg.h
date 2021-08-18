#pragma once

#include "thread"
#include "utility\hidapi\hidapi.h"
#include "ModeConfig.h"		//IMU_TYPE
#include <eSPDI_Common.h>	//DEVSELINFO
#include "utility\CIMUViewer.h"
// IMUTest 對話方塊

const char GET_MODULE_NAME_0[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
const char GET_MODULE_NAME_1[8] = { 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00 };
const char GET_MODULE_NAME_2[8] = { 0x00, 0x02, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00 };
const char GET_MODULE_NAME_3[8] = { 0x00, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00 };

const char GET_FW_VERSION_0[8] = { 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
const char GET_FW_VERSION_1[8] = { 0x00, 0x05, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00 };
const char GET_FW_VERSION_2[8] = { 0x00, 0x06, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00 };
const char GET_FW_VERSION_3[8] = { 0x00, 0x07, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00 };
const char GET_FW_VERSION_4[8] = { 0x00, 0x08, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00 };
const char GET_FW_VERSION_5[8] = { 0x00, 0x09, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00 };
const char GET_FW_VERSION_6[8] = { 0x00, 0x0A, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00 };
const char GET_FW_VERSION_7[8] = { 0x00, 0x0B, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00 };

const char READ_OUTPUT_STATUS[8] = { 0x00, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
const char DISABLE_OUTPUT[8] = { 0x00, 0x11, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00 };
const char ENABLE_OUTPUT[8] = { 0x00, 0x11, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00 };

const char READ_OUTPUT_FORMAT[8] = { 0x00, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

const char SET_OUTPUT_FORMAT_1[8] = { 0x00, 0x12, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00 };
const char SET_OUTPUT_FORMAT_2[8] = { 0x00, 0x12, 0x01, 0x02, 0x00, 0x00, 0x00, 0x00 };
const char SET_OUTPUT_FORMAT_3[8] = { 0x00, 0x12, 0x01, 0x03, 0x00, 0x00, 0x00, 0x00 };
const char SET_OUTPUT_FORMAT_4[8] = { 0x00, 0x12, 0x02, 0x04, 0x00, 0x00, 0x00, 0x00 };
const char SET_OUTPUT_FORMAT_5[8] = { 0x00, 0x12, 0x02, 0x05, 0x00, 0x00, 0x00, 0x00 };

const char CHECK_CALIBRATING_STATUS[8] = { 0x00, 0x13, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
const char START_CALIBRATION[8] = { 0x00, 0x13, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00 };
const char READ_CALIBRATED[8] = { 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

const char READ_SERIAL_NUMBER[5][8] = { { 0x00, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
                                        { 0x00, 0x14, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00 },
                                        { 0x00, 0x14, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00 },
                                        { 0x00, 0x14, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00 },
                                        { 0x00, 0x14, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00 } };
const char WRITE_SERIAL_NUMBER_TO_FLASH[8] = { 0x00, 0x1B, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00 };

struct SetFeatureDATA_Item {
	const char* pData;
	int nDataLength;
};

class CPreviewImageDlg;
class IMUTestDlg : public CDialog
{
	DECLARE_DYNAMIC(IMUTestDlg)
    DECLARE_MESSAGE_MAP()

public:
	//IMUTestDlg( const wchar_t* SN, const BOOL bIs9Axis, CWnd* pParent );
	IMUTestDlg(const wchar_t* SN, void*& hApcDI, DEVSELINFO& devSelInfo, ModeConfig::IMU_TYPE imu_type, CWnd* pParent, CPreviewImageDlg* pPreviewDlg);
    virtual ~IMUTestDlg();

	//int initIMU();
	int startGetImuData();
    inline void PauseGetImuData( const bool bPause ) { m_isPause = bPause; }

    static void InitIMU();
    static void UninitIMU();

	enum { IDD = IDD_DIALOG_IMU};

	char m_chText_FrameCount[32];

	bool m_enableModuleSync;
	void ChangeIMU_UI_FrameCount_SerialCount(bool enableModuleSync);
	bool Update_IMU_Device_Mapping(int nIMU_ID_INDEX);
	int m_nIMU_ID_INDEX;
	int m_nIndex_s_vecIMU;
	bool m_bIsDeviceMappingIMU;
	bool Is9axis();
	int IMU_SetHWRegister();
	void*& m_hApcDI;
	DEVSELINFO& m_DevSelInfo;
	void IMU_Device_Reopen(void*& hApcDI, DEVSELINFO& devSelInfo);
	void measureBeginQuaternion();

	void IMUDataCallback(IMUData imu);

private:
	//const BOOL m_bIs9axis;
	BOOL m_bIs9axis;
	ModeConfig::IMU_TYPE m_IMU_TYPE;
	BOOL m_bIsSyncIMU;

    struct IMU_INFO
    {
        hid_device* hIMU;
        USHORT      PID;
        USHORT      VID;
        CString     SN;
    };
	static int IMUfunc(IMUTestDlg * pThis);
	static int IMUCalibrationfunc(IMUTestDlg * pThis);

    static std::vector< IMU_INFO > s_vecIMU;

	void SetImuDataFormatList(char format);
    void saveImuRawData(unsigned char* buf, WORD size);
    int stopGetImuData();

	CMutex m_mutex;
	hid_device *handle = nullptr;
	//hid_device *m_handle[MAX_DEVICE_IMU];

	CPreviewImageDlg *m_pPreviewDlg;

	//std::thread *m_IMUThread = nullptr;
	std::thread *m_IMUThread[MAX_DEVICE_IMU];
	std::thread *m_IMUCalibrationThread = nullptr;
	volatile bool m_isRunning = false;
	volatile bool m_isPause = false;
	volatile bool m_isRecording = false;

	CIMUViewer m_imuViewer;

	char m_imuOutputFormat = { 0 };
	WORD m_outputDataSize;

	std::ofstream fs;
	std::stringstream oss;
    std::string m_text;
    std::string m_cmd_text;

	int imuSaveDataCount = 0;

	virtual void DoDataExchange(CDataExchange* pDX);
    virtual BOOL OnInitDialog();

	int addImuDataToBuf(std::string text);
	void saveImuDataToFile(bool isCloseFile);
	WORD GetImuDataOutoutByte(char ImuDataOutoutFormat);

    afx_msg void    OnBnClickedOk(){};
    afx_msg void    OnBnClickedCancel(){};
	afx_msg void    OnDestroy();
	afx_msg void    OnBnClickedButtonSaveImuRawData();
	afx_msg void    OnBnClickedButtonGetFeatureReport();
	afx_msg void    OnBnClickedButtonGetFwVersion();
	afx_msg void    OnBnClickedButtonGetInputReport();
	afx_msg void    OnBnClickedButtonSetOutputReport();
	afx_msg void    OnBnClickedButtonGetImuStatus();
	afx_msg void    OnBnClickedButtonEnableImu();
	afx_msg void    OnBnClickedButtonDisableImu();
	afx_msg void    OnBnClickedButtonStartCalibration();
	afx_msg void    OnCbnSelchangeComboSetImuDataFormat();
    afx_msg LRESULT OnUpdateText( WPARAM wparam,LPARAM lparam );
	afx_msg LRESULT OnIMUReEnable(WPARAM wparam, LPARAM lparam);

	void GetFeatureReport(char* pData, size_t data_lenght);
	void SendFeatureReport(const char* pData, size_t data_lenght);
	void GetInputReport(char* pData, size_t data_lenght);
	void SetOutputReport(const char* pData, size_t data_lenght);

	void GetModuleName(char* pModuleName, WORD* pSize);
	void GetFwVersion(char* pFwVersion, WORD* pSize);
	void GetImuDataOutoutStatus(char* pImuDataOutoutStatus);
	void GetImuDataOutoutFormat(char * pImuDataOutoutFormat);
	void EnableDataOutout(BOOL isEnbale);
	void SetImuDataOutoutFormat(WORD format);
	void CheckCalibratingStatus(char * pCalibratingStatus);
	void StartCalibration();
	void ReadCalibrated(char * pCalibrated);

    void ReadSerialNumber( char* pSerialNumber );
    void WriteSerialNumber( const char* pSerialNumber );

	double m_QuaternionBeginInverse[4] = { 0 };
	bool m_bNeedInitQuaternion = true;
public:
	afx_msg void OnBnClickedImu3dReset();
	afx_msg void OnBnClickedCheckFrameSync();
};
