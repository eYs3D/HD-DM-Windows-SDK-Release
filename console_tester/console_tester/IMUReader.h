//#pragma once
#include <iostream>
#include <vector>
#include <set>
#include <thread>
#include <atlstr.h>
#include <mutex>
#include "..\..\eSPDI\DM\include\eSPDI_DM.h"
#include "..\..\eSPDI\DM\include\eSPDI_Common.h"
#include "..\..\eSPDI\DM\include\eSPDI_ErrCode.h"
#include "IMUData.h"
#include "hidapi.h";

#define IMU_ID_INDEX_START  25
#define MAX_DEVICE_IMU		4

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

const char READ_RTC[8] = { 0x00, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
const char WRITE_RTC[8] = { 0x00, 0x1D, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00 };
const char READ_ACC_FS[8] = { 0x01, 0x22, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
const char WRITE_ACC_FS[8] = { 0x01, 0x23, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00 };
const char READ_GYR_FS[8] = { 0x01, 0x24, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
const char WRITE_GYR_FS[8] = { 0x01, 0x25, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00 };
const char RESET_BOOTLOADER[8] = { 0x00, 0x1E, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00 };

const std::vector<std::string> ACC_FS_LSM6DSR = {"2G", "4G", "8G", "16G"};
const std::vector<std::string> GYR_FS_LSM6DSR = {"125dps", "250dps", "500dps", "1000dps", "2000dps", "4000dps"};
const std::vector<std::string> ACC_FS_BMI088 = {"3G", "6G", "12G", "24G"};
const std::vector<std::string> GYR_FS_BMI088 = {"125dps", "250dps", "500dps", "1000dps", "2000dps"};

enum IMU_TYPE
{
    IMU_NONE,
    IMU_6Axis = 6,
    IMU_9Axis = 9,
};

struct SetFeatureDATA_Item {
	const char* pData;
	int nDataLength;
};

class IMUReader
{
public:
    IMUReader(void*& hApcDI, DEVSELINFO& devSelInfo, IMU_TYPE imuType);
    virtual ~IMUReader();

    typedef void(*APC_IMUCallbackFn)(IMUData imu);

    int startGetImuData(APC_IMUCallbackFn callbackFn);
    int stopGetImuData();
    void GetFwVersion(char *pFwVersion, WORD* pSize);
    void GetRTC(char* pHour, char* pMinute, char* pSecond);
    void SetRTC(char hour, char minute, char second);
    void GetAccFs(char* pData);
    void SetAccFs(char data);
    void GetGyrFs(char* pData);
    void SetGyrFs(char data);
    void ResetBootloader();

private:
    struct IMU_INFO
    {
        hid_device* hIMU;
        USHORT      PID;
        USHORT      VID;
        CString     SN;
    };

    void InitIMU();
    void UninitIMU();
    void IMUDeviceMapping();
    static int IMUfunc(IMUReader * pThis);

    void*& m_hApcDI;
	DEVSELINFO& m_DevSelInfo;
    std::vector< IMU_INFO > s_vecIMU;
    BOOL m_bIs9axis;
    IMU_TYPE m_IMU_TYPE;
    BOOL m_bIsSyncIMU;
    int m_nIMU_ID_INDEX;
    int m_nIndex_s_vecIMU;
    bool m_bIsDeviceMappingIMU;
    APC_IMUCallbackFn m_imuCallbackFn;
    std::thread *m_IMUThread[MAX_DEVICE_IMU];

    hid_device *handle = nullptr;
    USHORT current_imu_pid;	// Rem 0910

    void GetFeatureReport(char* pData, size_t data_lenght);
    void SendFeatureReport(const char* pData, size_t data_lenght);
    void ReadSerialNumber(char* pSerialNumber);
    void WriteSerialNumber(const char* pSerialNumber);

    std::mutex m_mutex;
    char m_imuOutputFormat = { 0 };
	WORD m_outputDataSize;

    void EnableDataOutout(BOOL isEnbale);
    void GetImuDataOutoutFormat(char * pImuDataOutoutFormat);
    WORD GetImuDataOutoutByte(char ImuDataOutoutFormat);
    int IMU_SetHWRegister();
    bool Update_IMU_Device_Mapping(int nIMU_ID_INDEX);

    volatile bool m_isRunning = false;
};