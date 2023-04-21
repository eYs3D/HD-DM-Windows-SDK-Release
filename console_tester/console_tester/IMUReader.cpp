#include "IMUReader.h"

IMUReader::IMUReader(void*& hApcDI, DEVSELINFO& devSelInfo, IMU_TYPE imuType) : m_hApcDI(hApcDI), m_DevSelInfo(devSelInfo)
{
    m_bIs9axis = (imuType == IMU_TYPE::IMU_9Axis);
    m_nIMU_ID_INDEX = -1;
    m_nIndex_s_vecIMU = -1;
    m_bIsDeviceMappingIMU = false;
    for (int i = 0; i < MAX_DEVICE_IMU; i++)
        m_IMUThread[i] = nullptr;

    InitIMU();

    if (1 == s_vecIMU.size())
    {	// Single Module;
        m_nIndex_s_vecIMU = 0;
        m_bIsDeviceMappingIMU = true;
        handle = s_vecIMU[NULL].hIMU;
        current_imu_pid = s_vecIMU[NULL].PID; // Rem 0910 keep current PID to check setup

        int nActualSNLenByByte = 0;
        wchar_t szBuf[MAX_PATH] = { NULL };
        if (APC_GetSerialNumber(hApcDI, &devSelInfo, (BYTE*)szBuf, MAX_PATH, &nActualSNLenByByte) == APC_OK)
        {
            WriteSerialNumber((char*)szBuf);
            //wchar_t szBuf[MAX_PATH] = { NULL };
            memset(szBuf, NULL, MAX_PATH);
            ReadSerialNumber((char*)szBuf);
            s_vecIMU[NULL].SN = szBuf;
        }
        return;
    }

    // Multi Module;
    switch (m_IMU_TYPE)
    {
    case IMU_TYPE::IMU_6Axis:	// 8040;
    {
        int nActualSNLenByByte = 0;
        wchar_t SN[MAX_PATH] = { NULL };
        if (APC_GetSerialNumber(hApcDI, &devSelInfo, (BYTE*)SN, MAX_PATH, &nActualSNLenByByte) != APC_OK)
            break;
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

    case IMU_TYPE::IMU_9Axis:	// 8062 & 8060
    {
    }
    break;

    case IMU_TYPE::IMU_NONE:
        break;

    default:
        break;
    }

}

IMUReader::~IMUReader()
{
    UninitIMU();
}

void IMUReader::InitIMU()
{
    UninitIMU();

    std::set< int > setIMU_VID = { IMU_APC_VID_0x0483, IMU_APC_VID_0x1E4E, IMU_APC_VID_0x3438 };
    std::set< int > setIMU_PID = { IMU_6AXIS_8040, IMU_6AXIS_8040_NEW, IMU_9AXIS_8060, IMU_9AXIS_8062, IMU_6AXIS_8063 };

    if (0 == hid_init())
    {
        hid_device_info* devs = hid_enumerate(NULL, NULL);
        hid_device_info* cur_dev = devs;
        IMU_INFO         xIMUInfo;

        while (cur_dev)
        {
            if (setIMU_VID.find(cur_dev->vendor_id) != setIMU_VID.end() && setIMU_PID.find(cur_dev->product_id) != setIMU_PID.end())
            {
                s_vecIMU.push_back(IMU_INFO());

                s_vecIMU.rbegin()->hIMU = hid_open_path(cur_dev->path);
                s_vecIMU.rbegin()->VID = cur_dev->vendor_id;
                s_vecIMU.rbegin()->PID = cur_dev->product_id;
                s_vecIMU.rbegin()->SN = cur_dev->serial_number;
            }
            cur_dev = cur_dev->next;
        }
        hid_free_enumeration(devs);
    }
}

void IMUReader::UninitIMU()
{
    for (auto& i : s_vecIMU) hid_close(i.hIMU);

    hid_exit();

    s_vecIMU.clear();
}

int IMUReader::IMU_SetHWRegister()
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

bool IMUReader::Update_IMU_Device_Mapping(int nIMU_ID_INDEX)
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
    //startGetImuData();
    return true;
}

void IMUReader::IMUDeviceMapping()
{
    int nIndex = m_DevSelInfo.index;
    unsigned short value = 0;
    int nRet = APC_GetHWRegister(m_hApcDI, &m_DevSelInfo, 0xf306, &value, FG_Address_2Byte | FG_Value_1Byte);
    if (nRet != APC_OK)
        return;

    if (value == nIndex + IMU_ID_INDEX_START)
    {
        Update_IMU_Device_Mapping(value);
        return;
    }

    value = nIndex + IMU_ID_INDEX_START;
    nRet = APC_SetHWRegister(m_hApcDI, &m_DevSelInfo, 0xf306, value, FG_Address_2Byte | FG_Value_1Byte);;
    if (nRet != APC_OK)
        return;

    Update_IMU_Device_Mapping(value);
}

void IMUReader::ReadSerialNumber(char* pSerialNumber)
{
    char status[8] = { 0 };

    int data_length = 0;

    for (int i = 0; i < 5; i++)
    {
        SendFeatureReport(READ_SERIAL_NUMBER[i], sizeof(READ_SERIAL_NUMBER[i]));
        GetFeatureReport(status, sizeof(READ_SERIAL_NUMBER[i]));

        data_length = i ? 8 : 6; // first package header is 0x24,0x03

        memcpy(pSerialNumber, &status[8 - data_length], data_length);

        pSerialNumber += data_length;
    }
}

void IMUReader::WriteSerialNumber(const char* pSerialNumber)
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
    for (int i = 0; i < 6; i++)
    {
        data_length = i ? 6 : 4; // first package header is 0x24,0x03

        memcpy(&WRITE_SERIAL_NUMBER[i][8 - data_length], pSerialNumber, data_length);

        pSerialNumber += data_length;

        SendFeatureReport(WRITE_SERIAL_NUMBER[i], sizeof(WRITE_SERIAL_NUMBER[i]));
    }
    SendFeatureReport(WRITE_SERIAL_NUMBER_TO_FLASH, sizeof(WRITE_SERIAL_NUMBER_TO_FLASH));
}

void IMUReader::GetFeatureReport(char* pData, size_t data_lenght)
{
    int res = 0;

    if (handle && handle->feature_report_length) {
        unsigned char *pBuf = nullptr;
        pBuf = (unsigned char *)calloc(handle->feature_report_length, sizeof(unsigned char));
        pBuf[0] = { 0x42 };
        res = hid_get_feature_report(handle, pBuf, handle->feature_report_length);
        memcpy(pData, pBuf + 1, data_lenght);
        free(pBuf);
    }
}

void IMUReader::SendFeatureReport(const char* pData, size_t data_lenght)
{
    int res = 0;

    if (handle && handle->feature_report_length) {
        unsigned char *pBuf = nullptr;
        pBuf = (unsigned char *)calloc(handle->feature_report_length, sizeof(unsigned char));
        pBuf[0] = { 0x42 };
        memcpy(pBuf + 1, pData, data_lenght);;
        res = hid_send_feature_report(handle, pBuf, handle->feature_report_length);
        free(pBuf);
    }
}

void IMUReader::EnableDataOutout(BOOL isEnbale)
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

void IMUReader::GetImuDataOutoutFormat(char *pImuDataOutoutFormat)
{
    SetFeatureDATA_Item setFeatureData = { &READ_OUTPUT_FORMAT[0], (sizeof(READ_OUTPUT_FORMAT) / sizeof(READ_OUTPUT_FORMAT[0])) };

    char status[8] = { 0 };

    SendFeatureReport(setFeatureData.pData, setFeatureData.nDataLength);
    GetFeatureReport(&status[0], setFeatureData.nDataLength);

    //memcpy(pImuDataOutoutFormat, &status[0], setFeatureData.nDataLength);
    *pImuDataOutoutFormat = status[0];
}

WORD IMUReader::GetImuDataOutoutByte(char ImuDataOutoutFormat)
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

int IMUReader::startGetImuData(APC_IMUCallbackFn callbackFn)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    IMUDeviceMapping();

    if (m_nIndex_s_vecIMU == -1)
        return 0;

    if (m_IMUThread[m_nIndex_s_vecIMU] && !m_isRunning) {
        m_IMUThread[m_nIndex_s_vecIMU]->join();
        m_IMUThread[m_nIndex_s_vecIMU] = nullptr;
    }

    EnableDataOutout(true);
    m_isRunning = true;
    m_imuCallbackFn = callbackFn;

    m_imuOutputFormat = { 0 };
    GetImuDataOutoutFormat(&m_imuOutputFormat);
    m_outputDataSize = GetImuDataOutoutByte(m_imuOutputFormat);

    handle = s_vecIMU[m_nIndex_s_vecIMU].hIMU;
    if (handle && !m_IMUThread[m_nIndex_s_vecIMU]) {
        m_IMUThread[m_nIndex_s_vecIMU] = new std::thread(IMUReader::IMUfunc, this);
    }

    return m_outputDataSize;
}

int IMUReader::stopGetImuData()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_nIndex_s_vecIMU == -1)
        return 0;

    m_isRunning = false;

    if (m_IMUThread[m_nIndex_s_vecIMU]) {
        m_IMUThread[m_nIndex_s_vecIMU]->join();
        delete m_IMUThread[m_nIndex_s_vecIMU];

        m_IMUThread[m_nIndex_s_vecIMU] = nullptr;
    }

    EnableDataOutout(false);

    return 0;
}

int IMUReader::IMUfunc(IMUReader * pThis)
{
    CString strMsg;
    //strMsg.Format(_T("¡iIMUfunc ENTER¡jnIndex_s_vecIMU:%d"), nIndex_s_vecIMU);
    //AfxMessageBox(strMsg);
    int res = 0;
    unsigned char buf[256] = { 0 };

    while (res >= 0 && pThis->m_isRunning) {
        //if (pThis->m_isPause) {
        //	std::this_thread::sleep_for(std::chrono::milliseconds(20));
        //	continue;
        //}

        if (pThis->handle == nullptr)
        {
            //pThis->m_text = "Unable to read(handle == nullptr)";
            //pThis->PostMessage(WM_UPDATE_TEXT_MSG, UPDATE_TEXT);
            printf("Unable to read(handle == nullptr)\n");
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            //pThis->PostMessage(WM_IMU_REENABLE, 0);
            continue;
        }

        if (pThis->m_bIs9axis)
        {
            if ((pThis->m_nIMU_ID_INDEX == -1) || (pThis->m_nIMU_ID_INDEX < IMU_ID_INDEX_START))
            {
                //pThis->m_text = "Mapping Unmatch!\r\nFor 9 axis module, please run \"Preview\" before IMU.";
                //pThis->PostMessage(WM_UPDATE_TEXT_MSG, UPDATE_TEXT);
                printf("Mapping Unmatch!\r\nFor 9 axis module, please run \"Preview\" before IMU.\n");
                std::this_thread::sleep_for(std::chrono::milliseconds(20));
                continue;
            }
        }

        try
        {
            res = hid_read(pThis->handle, buf, sizeof(buf));
        }
        catch (const char* e)
        {
            printf("%s\n", e);
            continue;
        }

        if (res == 0)
            printf("waiting...\n");//pThis->m_text = "waiting...";
        else if (res < 0)
        {
            //pThis->m_text = "Unable to read()";
            //pThis->PostMessage(WM_UPDATE_TEXT_MSG, UPDATE_TEXT);
            printf("Unable to read()\n");
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            break; //continue;
        }
        else {
            IMUData imu;
            /// **************************************** ///
            /// Module 8062¡GMAX_G = 8.0					 ///
            /// Other Module ¡GMAX_G = 4.0				 ///
            /// Rem 0910 Default MaxG for 8062 is 8G	 ///
            /// Jacky_Tag								 ///
            /// **************************************** ///
            // Rem 0910 Default MaxG for 8062 is 8G
            if (pThis->current_imu_pid == IMU_9AXIS_8062)	//0x0163)
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
                imu.parsePacket(buf, (pThis->m_imuOutputFormat == 3 ? false : true));
            }
            else if (pThis->m_outputDataSize == 58)
            {
                imu.parsePacket_DMP(buf);
            }


            //if (FrameSyncManager::GetInstance()->IsEnable())
            //{
            //	FrameSyncManager::GetInstance()->SyncIMUCallback(pThis->m_hApcDI, pThis->m_DevSelInfo,
            //												     imu._frameCount,
            //												     std::bind(&IMUTestDlg::IMUDataCallback, pThis, imu));
            //}
            //else
            //{
            //	pThis->IMUDataCallback(imu);
            //}
            if (pThis->m_imuCallbackFn) (pThis->m_imuCallbackFn)(imu);

            //if (pThis->m_isRecording) {
            //	pThis->saveImuRawData(&buf[0], pThis->m_outputDataSize);
            //}
        }
        //pThis->PostMessage( WM_UPDATE_TEXT_MSG, UPDATE_TEXT );

        if (!pThis->m_isRunning) {
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    pThis->m_isRunning = false;

    //strMsg.Format(_T("¡iIMUfunc Leave¡jnIndex_s_vecIMU:%d"), nIndex_s_vecIMU);
    //AfxMessageBox(strMsg);
    return 0;
}

void IMUReader::GetRTC(char* pHour, char* pMinute, char* pSecond)
{
    char rtc[8] = { 0 };

    SetFeatureDATA_Item setFeatureData = { &READ_RTC[0], (sizeof(READ_RTC) / sizeof(READ_RTC[0])) };
    SendFeatureReport(setFeatureData.pData, setFeatureData.nDataLength);
    GetFeatureReport(&rtc[0], setFeatureData.nDataLength);

    char rtcStr[9] = { 0 };
    sprintf(rtcStr, "%02d:%02d:%02d", rtc[0], rtc[1], rtc[2]);

    *pHour = rtc[0];
    *pMinute = rtc[1];
    *pSecond = rtc[2];
}

void IMUReader::SetRTC(char hour, char minute, char second)
{
    char rtc[8];
    memcpy(&rtc[0], &WRITE_RTC[0], 8);

    rtc[3] = hour;
    rtc[4] = minute;
    rtc[5] = second;

    SetFeatureDATA_Item setFeatureData = { &rtc[0], (sizeof(WRITE_RTC) / sizeof(WRITE_RTC[0])) };
    SendFeatureReport(setFeatureData.pData, setFeatureData.nDataLength);
}

void IMUReader::GetAccFs(char* pData)
{
    char accfs[8] = { 0 };

    SetFeatureDATA_Item setFeatureData = { &READ_ACC_FS[0], (sizeof(READ_ACC_FS) / sizeof(READ_ACC_FS[0])) };
    SendFeatureReport(setFeatureData.pData, setFeatureData.nDataLength);
    GetFeatureReport(&accfs[0], setFeatureData.nDataLength);

    *pData = accfs[0];
}

void IMUReader::SetAccFs(char data)
{
    char accfs[8];
    memcpy(&accfs[0], &WRITE_ACC_FS[0], 8);

    accfs[3] = data;

    SetFeatureDATA_Item setFeatureData = { &accfs[0], (sizeof(WRITE_ACC_FS) / sizeof(WRITE_ACC_FS[0])) };
    SendFeatureReport(setFeatureData.pData, setFeatureData.nDataLength);
}

void IMUReader::GetGyrFs(char* pData)
{
    char gyrfs[8] = { 0 };

    SetFeatureDATA_Item setFeatureData = { &READ_GYR_FS[0], (sizeof(READ_GYR_FS) / sizeof(READ_GYR_FS[0])) };
    SendFeatureReport(setFeatureData.pData, setFeatureData.nDataLength);
    GetFeatureReport(&gyrfs[0], setFeatureData.nDataLength);

    *pData = gyrfs[0];
}

void IMUReader::SetGyrFs(char data)
{
    char gyrfs[8];
    memcpy(&gyrfs[0], &WRITE_GYR_FS[0], 8);

    gyrfs[3] = data;

    SetFeatureDATA_Item setFeatureData = { &gyrfs[0], (sizeof(WRITE_GYR_FS) / sizeof(WRITE_GYR_FS[0])) };
    SendFeatureReport(setFeatureData.pData, setFeatureData.nDataLength);
}