#include "stdafx.h"
#include "APC_Test.h"
#include "SelfCalibration2Dlg.h"
#include <bitset>

#define CHECK_PERIOD_IN_MS 100
/* RECTIFY  */
#define N_RECTIFY_PAYLOAD_STEP_BYTES (3)
#define N_RECTIFY_BUFFER_PREFIX_BYTES (2)
#define REG_ADDR_RECTIFY (0xf5)
#define _COUT_FUNCTION_CALL_LOG_ false
#define _CALIB_HW_API_DEBUG_ false
#define HWAPI_RECTIFICATION_N_DATA_STRUCTURE_ELEMENTS (76)

/* RECTIFICATION DATA STRUCTURE INFORMATION */
// ******************************************************************************
static const short Rectification_Data_Structure_Bytes_Offset[HWAPI_RECTIFICATION_N_DATA_STRUCTURE_ELEMENTS] = {
    0, 1,  2, 4, 6, 8, 10, 13, 15, 17, 20, 22, 24, 27, 29, 31, 33, 35, 37, 39, 42, 44, 46, 49, 51, 53, 56, 58, 60, 62, 64, 65, 66, 67, 68, 70, 72, 74, 76, 78, 80, 82, 84, 87, 90, 91, 92, 94, 96, 98, 100, 102, 104, 106, 108, 111, 114, 115, 117, 119, 121, 123, 125, 127, 128, 144, 160, 175, 176, 186, 188, 190, 191, 193, 194, 196 };
static const short Rectification_Data_Structure_Size_in_Bytes[HWAPI_RECTIFICATION_N_DATA_STRUCTURE_ELEMENTS] = {
    1, 1, 2, 2, 2, 2, 3, 2, 2, 3, 2, 2, 3, 2, 2, 2, 2, 2, 2, 3, 2, 2, 3, 2, 2, 3, 2, 2, 2, 2, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 1, 2, 2, 2, 2, 2, 2, 1, 16, 16, 15, 1, 10, 2, 2, 2, 2, 2, 2, 2 };
static const short Rectification_Data_Structure_Size_in_Bits[HWAPI_RECTIFICATION_N_DATA_STRUCTURE_ELEMENTS] = {
    8, 8, 12, 12, 16, 11, 19, 11, 16, 19, 12, 10, 19, 10, 11, 10, 11, 16, 11, 19, 11, 16, 19, 12, 10, 19, 10, 11, 10, 11, 8, 8, 7, 8, 16, 16, 16, 16, 16, 16, 16, 16, 18, 18, 8, 7, 16, 16, 16, 16, 16, 16, 16, 16, 18, 18, 8, 13, 13, 11, 11, 11, 11, 8, 128, 128, 120, 8, 80, 13, 13, 11, 11, 11, 11, 16 };
//static const short Rectification_Data_Structure_Local_Bits_Offset[HWAPI_RECTIFICATION_N_DATA_STRUCTURE_ELEMENTS] = { 0, 0, 1, 1, 2, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
static const short Rectification_Data_Structure_Fractional_Bits[HWAPI_RECTIFICATION_N_DATA_STRUCTURE_ELEMENTS] = {
    0, 0, 0, 0, 21, 21, 17, 21, 21, 17, 21, 21, 17, 0, -1, 0, -1, 21, 21, 17, 21, 21, 17, 21, 21, 17, 0, -1, 0, -1, 13, 0, 13, 0, 10, 10, 10, 10, 10, 10, 2, 2, 3, 3, 13, 13, 10, 10, 10, 10, 10, 10, 2, 2, 3, 3, 0, 9, 9, -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, 9, 9, -1, -1, -1, -1, 0 };
static const short Rectification_Data_Structure_Little_Endian[HWAPI_RECTIFICATION_N_DATA_STRUCTURE_ELEMENTS] = {
1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1 };
static const short Rectification_Data_Structure_DontCare[HWAPI_RECTIFICATION_N_DATA_STRUCTURE_ELEMENTS] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1 };
static const short Rectification_Data_Structure_HighByte_MSB_Index[HWAPI_RECTIFICATION_N_DATA_STRUCTURE_ELEMENTS] = {
    7, 7, 3, 3, 7, 2, 2, 2, 7, 2, 3, 1, 2, 1, 2, 1, 2, 7, 2, 2, 2, 7, 2, 3, 1, 2, 1, 2, 1, 2, 7, 7, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 1, 1, 7, 6, 7, 7, 7, 7, 7, 7, 7, 7, 1, 1, 7, 4, 4, 2, 2, 2, 2, 7, 127, 127, 119, 7, 79, 4, 4, 2, 6, 2, 6, 15 };
static const short Rectification_Data_Structure_HighByte_LSB_Index[HWAPI_RECTIFICATION_N_DATA_STRUCTURE_ELEMENTS] = {
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 4, 0 };
// ******************************************************************************

int get_temperature_param(SelfCalibration2Dlg* selfCalibration2Dlg, E_THERMAL_SENSOR_MODEL& e_sensor_model, int& sensor_slave_addr, int& nSensorMode)
{
    switch (selfCalibration2Dlg->m_xDevinfoEx.wPID)
    {
    case APC_PID_80362:
    case APC_PID_IRIS:
        e_sensor_model = E_THERMAL_SENSOR_MODEL::ONSEMI_AR0144;
        sensor_slave_addr = _THERMAL_SENSOR_SLAVE_ADDR_80362;
        nSensorMode = 0;
        selfCalibration2Dlg->m_isSensorSlave = false;
        break;
    case APC_PID_IVY2:
        e_sensor_model = E_THERMAL_SENSOR_MODEL::STM_VD5X;
        sensor_slave_addr = _THERMAL_SENSOR_SLAVE_ADDR_8083;
        nSensorMode = 0;
        selfCalibration2Dlg->m_isSensorSlave = true;
        break;
    }
    return 0;
}

int get_temperature_of_img_sensor_ar0144(SelfCalibration2Dlg* selfCalibration2Dlg, E_THERMAL_SENSOR_MODEL e_sensor_model, int sensor_slave_addr, int nSensorMode, float& fTemperature) {
    // change for linux version by Brook at 20230913

    int ret = 0;

    USHORT RegValue;
    USHORT QRegValue;
    //APC_SensorMode::SensorAll
    if (!selfCalibration2Dlg->m_isSensorSlave)
        ret = APC_SetSensorRegister(selfCalibration2Dlg->m_hApcDI, &selfCalibration2Dlg->m_DevSelInfo, sensor_slave_addr, _ONSEMI_AR0144_PRESET_ADDR, _ONSEMI_AR0144_PRESET_VALUE, FG_Address_2Byte | FG_Value_2Byte, nSensorMode);
    else
        ret = APC_SetSlaveSensorRegister(selfCalibration2Dlg->m_hApcDI, &selfCalibration2Dlg->m_DevSelInfo, sensor_slave_addr, _ONSEMI_AR0144_PRESET_ADDR, _ONSEMI_AR0144_PRESET_VALUE, FG_Address_2Byte | FG_Value_2Byte, nSensorMode);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    if (ret != APC_OK) {
        cout << "APC_SetSensorRegister Failed." << endl;
        return -1;
    }

    if (!selfCalibration2Dlg->m_isSensorSlave)
        ret = APC_GetSensorRegister(selfCalibration2Dlg->m_hApcDI, &selfCalibration2Dlg->m_DevSelInfo, sensor_slave_addr, _ONSEMI_AR0144_CALIB_DATA_READ_ADDR, &QRegValue, FG_Address_2Byte | FG_Value_2Byte, nSensorMode);
    else
        ret = APC_GetSlaveSensorRegister(selfCalibration2Dlg->m_hApcDI, &selfCalibration2Dlg->m_DevSelInfo, sensor_slave_addr, _ONSEMI_AR0144_CALIB_DATA_READ_ADDR, &QRegValue, FG_Address_2Byte | FG_Value_2Byte, nSensorMode);

    if (ret != APC_OK) {
        cout << "APC_GetSensorRegister Failed." << endl;
        return -1;
    }

    // change for linux version by Brook at 20230913
    std::bitset<16>b = QRegValue;
    std::bitset<16>a;
    for (int i = 8; i < 16; i++)
    {
        a[i - 8] = b[i];
    }

    for (int i = 0; i < 8; i++)
    {
        a[i + 8] = b[i];
    }


    unsigned long Valuea = a.to_ullong();

    //unsigned long Valuea = 443.2;
    // change for linux version by Brook at 20230913
#ifdef _WIN32
    if (!selfCalibration2Dlg->m_isSensorSlave)
        ret = APC_GetSensorRegister(selfCalibration2Dlg->m_hApcDI, &selfCalibration2Dlg->m_DevSelInfo, sensor_slave_addr, _ONSEMI_AR0144_TEMP_READ_ADDR, &QRegValue, FG_Address_2Byte | FG_Value_2Byte, nSensorMode);
    else
        ret = APC_GetSlaveSensorRegister(selfCalibration2Dlg->m_hApcDI, &selfCalibration2Dlg->m_DevSelInfo, sensor_slave_addr, _ONSEMI_AR0144_TEMP_READ_ADDR, &QRegValue, FG_Address_2Byte | FG_Value_2Byte, nSensorMode);
#elif __linux__
    ret = APC_GetSensorRegister(pHandleApcDI, pDevSelInfo, sensor_slave_addr, _ONSEMI_AR0144_TEMP_READ_ADDR, &QRegValue, FG_Address_2Byte | FG_Value_2Byte, (SENSORMODE_INFO)nSensorMode);
#endif

    if (ret != APC_OK) {
        cout << "APC_GetSensorRegister Failed." << endl;
        fTemperature = NAN;
        return -1;
    }

    // change for linux version by Brook at 20230913
    b = QRegValue;

    for (int i = 8; i < 16; i++)
    {
        a[i - 8] = b[i];
    }

    for (int i = 0; i < 8; i++)
    {
        a[i + 8] = b[i];
    }

    unsigned long Valueb = a.to_ullong();
    //cout << "Valuea:" << Valuea << endl;
    //cout << "Valueb:" << Valueb << endl;

    fTemperature = (float)(_ONSEMI_AR0144_TEMP_CURVE_SLOPE * ((float)Valueb - (float)Valuea) + _ONSEMI_AR0144_CALIB_DATA_TEMP);

    return 0;
}

int get_temperature_of_img_sensor_ar0135(SelfCalibration2Dlg* selfCalibration2Dlg, E_THERMAL_SENSOR_MODEL e_sensor_model, int sensor_slave_addr, int nSensorMode, float& fTemperature) {

    int ret = 0;

    // change for linux version by Brook at 20230913
#ifdef _WIN32
    USHORT RegValue;
    USHORT QRegValue;
#elif __linux__
    unsigned short RegValue;
    unsigned short QRegValue;
#endif

    if (!selfCalibration2Dlg->m_isSensorSlave)
        ret = APC_SetSensorRegister(selfCalibration2Dlg->m_hApcDI, &selfCalibration2Dlg->m_DevSelInfo, sensor_slave_addr, _ONSEMI_AR0135_PRESET_ADDR, _ONSEMI_AR0135_PRESET_VALUE, FG_Address_2Byte | FG_Value_2Byte, nSensorMode);
    else
        ret = APC_SetSlaveSensorRegister(selfCalibration2Dlg->m_hApcDI, &selfCalibration2Dlg->m_DevSelInfo, sensor_slave_addr, _ONSEMI_AR0135_PRESET_ADDR, _ONSEMI_AR0135_PRESET_VALUE, FG_Address_2Byte | FG_Value_2Byte, nSensorMode);

    std::this_thread::sleep_for(std::chrono::milliseconds(10));


    if (ret != APC_OK) {
        cout << "APC_SetSensorRegister Failed." << endl;
        return -1;
    }

    if (!selfCalibration2Dlg->m_isSensorSlave)
        ret = APC_GetSensorRegister(selfCalibration2Dlg->m_hApcDI, &selfCalibration2Dlg->m_DevSelInfo, sensor_slave_addr, _ONSEMI_AR0135_CALIB_DATA_READ_ADDR, &QRegValue, FG_Address_2Byte | FG_Value_2Byte, nSensorMode);
    else
        ret = APC_GetSlaveSensorRegister(selfCalibration2Dlg->m_hApcDI, &selfCalibration2Dlg->m_DevSelInfo, sensor_slave_addr, _ONSEMI_AR0135_CALIB_DATA_READ_ADDR, &QRegValue, FG_Address_2Byte | FG_Value_2Byte, nSensorMode);

    if (ret != APC_OK) {
        cout << "APC_GetSensorRegister Failed." << endl;
        return -1;
    }

    // change for linux version by Brook at 20230913
    bitset<16>b = QRegValue;
    bitset<16>a;
    for (int i = 8; i < 16; i++)
    {
        a[i - 8] = b[i];
    }

    for (int i = 0; i < 8; i++)
    {
        a[i + 8] = b[i];
    }

    unsigned long Valuea = a.to_ullong();

    // change for linux version by Brook at 20230913
    if (!selfCalibration2Dlg->m_isSensorSlave)
        APC_GetSensorRegister(selfCalibration2Dlg->m_hApcDI, &selfCalibration2Dlg->m_DevSelInfo, sensor_slave_addr, _ONSEMI_AR0135_TEMP_READ_ADDR, &QRegValue, FG_Address_2Byte | FG_Value_2Byte, nSensorMode);
    else
        APC_GetSlaveSensorRegister(selfCalibration2Dlg->m_hApcDI, &selfCalibration2Dlg->m_DevSelInfo, sensor_slave_addr, _ONSEMI_AR0135_TEMP_READ_ADDR, &QRegValue, FG_Address_2Byte | FG_Value_2Byte, nSensorMode);

    if (ret != APC_OK) {
        cout << "APC_GetSensorRegister Failed." << endl;
        fTemperature = NAN;
        return -1;
    }

    // change for linux version by Brook at 20230913
    b = QRegValue;

    for (int i = 8; i < 16; i++)
    {
        a[i - 8] = b[i];
    }

    for (int i = 0; i < 8; i++)
    {
        a[i + 8] = b[i];
    }

    unsigned long Valueb = a.to_ullong();

    fTemperature = (float)(_ONSEMI_AR0135_TEMP_CURVE_SLOPE * ((float)Valueb - (float)Valuea) + _ONSEMI_AR0135_CALIB_DATA_TEMP);

    return 0;
}

int get_temperature_of_img_sensor_st_vd5x(SelfCalibration2Dlg* selfCalibration2Dlg, E_THERMAL_SENSOR_MODEL e_sensor_model, int sensor_slave_addr, int nSensorMode, float& fTemperature) {
    cout << ">> get_temperature_of_img_sensor_st_vd5x: " << endl;

    // ivy2: sensor_id = 0x20

    int ret = APC_OK;
    unsigned short TempeRegVal = 0x00;
    bool is_negtive = false;
    const unsigned short ADDR = _STM_VD5X_TEMP_READ_ADDR;
    // change for linux version by Brook at 20230913
    if (!selfCalibration2Dlg->m_isSensorSlave)
        ret = APC_GetSensorRegister(selfCalibration2Dlg->m_hApcDI, &selfCalibration2Dlg->m_DevSelInfo, sensor_slave_addr, ADDR, &TempeRegVal, FG_Address_2Byte | FG_Value_2Byte, nSensorMode);
    else
        ret = APC_GetSlaveSensorRegister(selfCalibration2Dlg->m_hApcDI, &selfCalibration2Dlg->m_DevSelInfo, sensor_slave_addr, ADDR, &TempeRegVal, FG_Address_2Byte | FG_Value_2Byte, nSensorMode);


    // change for linux version by Brook at 20230913
    // HardCode Rule
    if (ret == APC_OK)
    {
        fTemperature = (float)TempeRegVal;
    }
    else {
        cout << "APC_GetSensorRegister() failed." << endl;
        fTemperature = NAN;
        return -1;
    }

    return 0;
}

int get_temperature_of_ti_tmp_xxxx(SelfCalibration2Dlg* selfCalibration2Dlg, E_THERMAL_SENSOR_MODEL e_sensor_model, int sensor_slave_addr,  int nSensorMode, float& fTemperature){

    cout << "@camera_device::get_temperature_of_ti_tmp1075()" << endl;
    bool b_successful = false;
    fTemperature = NAN;

    int ret = APC_OK;
    unsigned short TempeRegVal = 0x00;
    bool is_negtive = false;
    const unsigned short ADDR = 0x00;
    // change for linux version by Brook at 20230913
    if (!selfCalibration2Dlg->m_isSensorSlave)
        ret = APC_GetSensorRegister(selfCalibration2Dlg->m_hApcDI, &selfCalibration2Dlg->m_DevSelInfo, sensor_slave_addr, ADDR, &TempeRegVal, FG_Address_1Byte | FG_Value_2Byte, APC_SensorMode::SensorAll);
    else
        ret = APC_GetSlaveSensorRegister(selfCalibration2Dlg->m_hApcDI, &selfCalibration2Dlg->m_DevSelInfo, sensor_slave_addr, ADDR, &TempeRegVal, FG_Address_1Byte | FG_Value_2Byte, APC_SensorMode::SensorAll);

    // change for linux version by Brook at 20230913
    // HardCode Rule
    if (ret == APC_OK)
    {
        unsigned short temperature_reg_val_reverse = ((((unsigned char*)&TempeRegVal)[0]) << 8) + ((unsigned char*)&TempeRegVal)[1];
        TempeRegVal = temperature_reg_val_reverse;
        TempeRegVal >>= 5;
        fTemperature = (float)TempeRegVal * 0.125;
        b_successful = true;
    }
    else {
        cout << "get_temperature_of_ti_tmp1075() failed." << endl;
    }


    if (b_successful) {
        cout << "get temperature successfully. " << endl;
        return 0;
    }
    else {
        cout << "WARNING:  get temperature failed(). " << endl;
        return -1;
    }

}

int get_temperature(SelfCalibration2Dlg* selfCalibration2Dlg, E_THERMAL_SENSOR_MODEL e_sensor_model, int sensor_slave_addr, int nSensorMode, float& fTemperature) {

    cout << "@camera_device::get_temperature()" << endl;
    bool b_successful = false;

    switch (e_sensor_model) {

    case E_THERMAL_SENSOR_MODEL::ONSEMI_AR0144:
    {
        int ret = get_temperature_of_img_sensor_ar0144(selfCalibration2Dlg, e_sensor_model, sensor_slave_addr,  nSensorMode, fTemperature);
        if (ret < 0)
            b_successful = false;
        else
            b_successful = true;
    }
    break;

    case E_THERMAL_SENSOR_MODEL::ONSEMI_AR0135:
    {
        int ret = get_temperature_of_img_sensor_ar0135(selfCalibration2Dlg, e_sensor_model, sensor_slave_addr,  nSensorMode, fTemperature);
        if (ret < 0)
            b_successful = false;
        else
            b_successful = true;
    }
    break;

    case E_THERMAL_SENSOR_MODEL::STM_VD5X:
    {
        int ret = get_temperature_of_img_sensor_st_vd5x(selfCalibration2Dlg, e_sensor_model, sensor_slave_addr,  nSensorMode, fTemperature);
        if (ret < 0)
            b_successful = false;
        else
            b_successful = true;
    }
    break;

    case E_THERMAL_SENSOR_MODEL::OV_OG02:
    case E_THERMAL_SENSOR_MODEL::OV_OG01:
    {
    }
    b_successful = true;
    break;
    case E_THERMAL_SENSOR_MODEL::TI_TMP1075:
    {
        int ret = get_temperature_of_ti_tmp_xxxx(selfCalibration2Dlg, e_sensor_model, sensor_slave_addr, nSensorMode, fTemperature);
        if (ret < 0)
            b_successful = false;
        else
            b_successful = true;
    }
    break;


    default:
        cout << "NO Matching Senor Model." << endl;
        break;
    }


    if (b_successful) {
        cout << "get temperature successfully. " << endl;
        return 0;
    }
    else {
        cout << "WARNING:  get temperature failed(). " << endl;
        return -1;
    }

}

void get_temperature_thread(SelfCalibration2Dlg* selfCalibration2Dlg) {
    while (!gStopGettingTemperature) {
        E_THERMAL_SENSOR_MODEL sensorModel = E_THERMAL_SENSOR_MODEL::ONSEMI_AR0135;
        int sensorAddress = 0;
        int sensorMode = 0;

        get_temperature_param(selfCalibration2Dlg, sensorModel, sensorAddress, sensorMode);

        int ret = get_temperature(selfCalibration2Dlg, sensorModel, sensorAddress, sensorMode, gTemperature);
        if (ret < 0) {
            TRACE("get_temperature failed\n");
        }
        else {
            TRACE("get_temperature : %f\n", gTemperature);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}

#define WM_UPDATE_TEXT_MSG WM_USER + 100
#define UPDATE_TEXT     0

BEGIN_MESSAGE_MAP(SelfCalibration2Dlg, CDialog)
ON_BN_CLICKED(IDC_BTN_RUN, &SelfCalibration2Dlg::OnBnClickedBtnRun)
ON_BN_CLICKED(IDC_BTN_RESET, &SelfCalibration2Dlg::OnBnClickedBtnReset)
ON_MESSAGE(WM_UPDATE_TEXT_MSG, OnUpdateText)
ON_BN_CLICKED(IDC_BTN_WRITE, &SelfCalibration2Dlg::OnBnClickedBtnWrite)
END_MESSAGE_MAP()

struct PARAM_t param_repair =  { 9, 1, true, 1.0, 999.0, 0.15, 1, 0.025, true, 5.0, SelfK2::C_Cy_Compensator::E_ESTIMATOR_TYPES::_confidence_weighted_ranking, 0.00, 0.98,  1, 30, 0.0 };
struct PARAM_t param_runtime = { 1, 1, true, 1.0,  60.0, 0.15, 1,  0.10, true, 2.5, SelfK2::C_Cy_Compensator::E_ESTIMATOR_TYPES::_confidence_weighted_ranking, 0.00, 0.997, 3, 30, 0.5 };

SelfCalibration2Dlg::SelfCalibration2Dlg(void*& hApcDI, DEVSELINFO& devSelInfo, const DEVINFORMATIONEX& devinfoEx, CWnd* pParent, CPreviewImageDlg* pPreviewDlg)
    : CDialog(IDD_SELF_K2_DIALOG, pParent), m_hApcDI(hApcDI), m_DevSelInfo(devSelInfo), m_xDevinfoEx(devinfoEx), m_pPreviewDlg(pPreviewDlg), m_Run(false), m_ThreadTerminated(true), m_Reset(false)
{
    m_RectifyData = (eSPCtrl_RectLogData*)malloc(sizeof(eSPCtrl_RectLogData));
    m_cy_compensator = nullptr;
    m_focal_compensator = nullptr;
}

SelfCalibration2Dlg::~SelfCalibration2Dlg()
{
    std::lock_guard< std::mutex > lock(m_CompensatorMutex);

    if (m_RectifyData) free(m_RectifyData);
    if (m_cy_compensator) delete m_cy_compensator;
    if (m_focal_compensator) delete m_focal_compensator;
    gStopGettingTemperature = true;
}

BOOL SelfCalibration2Dlg::OnInitDialog()
{
    CDialog::OnInitDialog();
    CheckRadioButton(IDC_RADIO_RUNTIME, IDC_RADIO_REPAIR, IDC_RADIO_RUNTIME);
    return TRUE;
}

void SelfCalibration2Dlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
}

LRESULT SelfCalibration2Dlg::OnUpdateText( WPARAM wparam,LPARAM lparam )
{
    CWnd* pWnd = GetDlgItem(IDC_EDIT_OUT_AND_INFO);

    if (pWnd) {
        pWnd->SetWindowText(CString(wparam ? m_OutAndInfo.str().c_str() : m_OutAndInfo.str().c_str()));
        pWnd->Invalidate();
    }
    return NULL;
}

void SelfCalibration2Dlg::SetImageParams(int width, int height, int FileSystemIndex, eSPCtrl_RectLogData* rectifyData)
{
    m_DepthWidth = width;
    m_DepthHeight = height;
    m_FileSystemIndex = FileSystemIndex;

    memcpy(m_RectifyData, rectifyData, sizeof(eSPCtrl_RectLogData));
}

void run_compensator_thread(SelfCalibration2Dlg* selfCalibration2Dlg)//(std::vector<unsigned char>& bufDepth)
{
    selfCalibration2Dlg->m_ThreadTerminated = false;

    bool cy_compensator_is_idle = false;

    while (selfCalibration2Dlg->m_Run) {
        {
            std::lock_guard< std::mutex > lock(selfCalibration2Dlg->m_CompensatorMutex);

            if (selfCalibration2Dlg->m_Reset) {
                if (selfCalibration2Dlg->m_cy_compensator != nullptr) {
                    selfCalibration2Dlg->m_cy_compensator->_reset();
                }

                if (selfCalibration2Dlg->m_focal_compensator != nullptr) {
                    selfCalibration2Dlg->m_focal_compensator->_reset();
                }

                selfCalibration2Dlg->m_Reset = false; // clear
            }
            else {
                if (selfCalibration2Dlg->m_cy_compensator != nullptr && selfCalibration2Dlg->m_Depth.size() > 0) {
                    //FILE* pFile = fopen("D:/depth.bin", "wb");
                    //fwrite(&selfCalibration2Dlg->m_Depth[0], 1, selfCalibration2Dlg->m_DepthWidth * selfCalibration2Dlg->m_DepthHeight * 2, pFile);
                    //fclose(pFile);
                    int ret = selfCalibration2Dlg->m_cy_compensator->run(&selfCalibration2Dlg->m_Depth[0], selfCalibration2Dlg->m_DepthWidth * selfCalibration2Dlg->m_DepthHeight, gTemperature);
                    if (ret < 0) {
                        //cout << m_cy_compensator->map_error_code[ret] << endl;
                        TRACE("[error] cy_compensator : %s\n", selfCalibration2Dlg->m_cy_compensator->map_error_code[ret].c_str());
                    }

                    cy_compensator_is_idle = (selfCalibration2Dlg->m_cy_compensator->info.e_states == SelfK2::C_Cy_Compensator::E_STATES::_idle);

                    selfCalibration2Dlg->m_OutAndInfo.str("");
                    selfCalibration2Dlg->m_OutAndInfo << "Temperature:" << gTemperature << endl;

                    selfCalibration2Dlg->m_OutAndInfo << "[Outputs]" << endl;
                    selfCalibration2Dlg->m_OutAndInfo << "comp_cy:" << selfCalibration2Dlg->m_cy_compensator->oputs.comp_cy << endl;
                    selfCalibration2Dlg->m_OutAndInfo << "default_cy:" << selfCalibration2Dlg->m_cy_compensator->oputs.default_cy << endl;
                    selfCalibration2Dlg->m_OutAndInfo << "estimaed_cy:" << selfCalibration2Dlg->m_cy_compensator->oputs.estimaed_cy << endl;
                    selfCalibration2Dlg->m_OutAndInfo << "default_cy_in_register:" << selfCalibration2Dlg->m_cy_compensator->oputs.default_cy_in_register << endl;

                    // INFO
                    selfCalibration2Dlg->m_OutAndInfo << "[Info]" << endl;
                    selfCalibration2Dlg->m_OutAndInfo << "p_f_in_acquisition:" << *selfCalibration2Dlg->m_cy_compensator->info.p_f_in_acquisition << endl;
                    selfCalibration2Dlg->m_OutAndInfo << "p_n_acq_iterations: " << *selfCalibration2Dlg->m_cy_compensator->info.p_n_acq_iterations << endl;
                    selfCalibration2Dlg->m_OutAndInfo << "p_convergency_cnt: " << *selfCalibration2Dlg->m_cy_compensator->info.p_convergency_cnt << endl;
                    selfCalibration2Dlg->m_OutAndInfo << "p_i_test: " << *selfCalibration2Dlg->m_cy_compensator->info.p_i_test << endl;
                    selfCalibration2Dlg->m_OutAndInfo << "p_i_successfully_test: " << *selfCalibration2Dlg->m_cy_compensator->info.p_i_successfully_test << endl;
                    selfCalibration2Dlg->m_OutAndInfo << "p_i_update_pars: " << *selfCalibration2Dlg->m_cy_compensator->info.p_i_update_pars << endl;


                    int n_test_cys = selfCalibration2Dlg->m_cy_compensator->info.v_test_cys.size();
                    selfCalibration2Dlg->m_OutAndInfo << "v_test_cys & fillrates : " << endl;
                    for (int i = 0; i < n_test_cys; i++) {
                        selfCalibration2Dlg->m_OutAndInfo << selfCalibration2Dlg->m_cy_compensator->info.v_test_cys[i] << ", " << selfCalibration2Dlg->m_cy_compensator->info.v_fill_rate_of_cys[i] << endl;
                    }

                    selfCalibration2Dlg->m_OutAndInfo << "p_convergency_cnt: " << *selfCalibration2Dlg->m_cy_compensator->info.p_convergency_cnt << endl;

                    selfCalibration2Dlg->PostMessage(WM_UPDATE_TEXT_MSG, UPDATE_TEXT);
                }
                if (selfCalibration2Dlg->m_focal_compensator != nullptr) {

                    if (cy_compensator_is_idle) { // prevent interference.

                        int ret = selfCalibration2Dlg->m_focal_compensator->run(gTemperature);
                        if (ret < 0) {
                            //cout << m_focal_compensator->map_error_code[ret] << endl;
                            TRACE("[error] focal_compensator : %s\n", selfCalibration2Dlg->m_focal_compensator->map_error_code[ret].c_str());
                        }
                    }

                }
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(CHECK_PERIOD_IN_MS));
    }

    selfCalibration2Dlg->m_ThreadTerminated = true;
    TRACE("\n\ncompensator thread terminated.\n\n");
}

void SelfCalibration2Dlg::ApplyInputImage(APCImageType::Value imgType, int imgId, std::vector<unsigned char>& bufDepth, int imgSize, int width, int height, int serialNumber)
{
    std::lock_guard< std::mutex > lock(m_CompensatorMutex);

    if (m_Run && APCImageType::IsImageDepth(imgType)) {
        if (m_Depth.size() != imgSize) m_Depth.resize(imgSize, 0);
        memcpy(&m_Depth[0], &bufDepth[0], imgSize);
    }
}

void SelfCalibration2Dlg::StartSelfK2()
{
    std::lock_guard< std::mutex > lock(m_CompensatorMutex);

    SetDlgItemText(IDC_BTN_RUN, L"Stop");
    GetDlgItem(IDC_RADIO_RUNTIME)->EnableWindow(FALSE);
    GetDlgItem(IDC_RADIO_REPAIR)->EnableWindow(FALSE);
    GetDlgItem(IDC_BTN_RESET)->EnableWindow(TRUE);

    float calib_temperature = m_RectifyData->LR_cam_K_temperature[_1st_];
    float LR_cam_thermal_variation_rate_of_focal = m_RectifyData->LR_cam_thermal_variation_rate_of_focal[_1st_];
    if (IsDlgButtonChecked(IDC_RADIO_RUNTIME)) {
        // Run-Time Correction Mode
        memcpy(&m_cy_compensator_param, &param_runtime, sizeof(struct PARAM_t));
    }
    else {
        // Depth-Broken Repair Mode
        memcpy(&m_cy_compensator_param, &param_repair, sizeof(struct PARAM_t));
        GetDlgItem(IDC_BTN_WRITE)->EnableWindow(TRUE);
    }

    SelfK2::set_hw_handler(
        m_hApcDI,
        &m_DevSelInfo,
        (DEVINFORMATIONEX*)&m_xDevinfoEx
    );

    m_cy_compensator = new SelfK2::C_Cy_Compensator(
        m_cy_compensator_param.mean_shift_kernel_size_acquisition,
        m_cy_compensator_param.mean_shift_kernel_size_tracking
    );
    m_cy_compensator->pars.n_cols = m_DepthWidth;
    m_cy_compensator->pars.n_rows = m_DepthHeight;
    m_cy_compensator->pars.b_auto_adjust_period = m_cy_compensator_param.b_auto_adjust_period;
    m_cy_compensator->pars.update_period_in_seconds_of_cy_in_acquisition = m_cy_compensator_param.update_period_in_seconds_of_cy_in_acquisition;
    m_cy_compensator->pars.update_period_in_seconds_of_cy_in_tracking = m_cy_compensator_param.update_period_in_seconds_of_cy_in_tracking;
    m_cy_compensator->pars.cy_sampling_period_in_seconds = m_cy_compensator_param.cy_sampling_period_in_seconds;
    m_cy_compensator->pars.temperatureThreshold = m_cy_compensator_param.temperatureThreshold;
    m_cy_compensator->pars.valid_min_fill_rate_threshold = m_cy_compensator_param.valid_min_fill_rate_threshold;
    m_cy_compensator->pars.b_dynamic_kernel_size = m_cy_compensator_param.b_dynamic_kernel_size;
    m_cy_compensator->pars.max_devication_of_cy = m_cy_compensator_param.max_devication_of_cy;
    m_cy_compensator->pars.e_estimator_type = m_cy_compensator_param.e_estimator_type;
    m_cy_compensator->pars.blind_zone_ratio = m_cy_compensator_param.blind_zone_ratio;
    m_cy_compensator->pars.fillrate_threshold_to_enter_tracking = m_cy_compensator_param.fillrate_threshold_to_enter_tracking;
    m_cy_compensator->pars.convergency_cnt_threshold = m_cy_compensator_param.convergency_cnt_threshold;
    m_cy_compensator->pars.max_acq_iterations = m_cy_compensator_param.max_acq_iterations;
    m_cy_compensator->pars.smoothing_factor = m_cy_compensator_param.smoothing_factor;

    if (LR_cam_thermal_variation_rate_of_focal != 0) {
        m_focal_compensator = new SelfK2::C_Focal_Compensator(calib_temperature, LR_cam_thermal_variation_rate_of_focal);
    }

    gStopGettingTemperature = false;
    std::thread getTemperature(get_temperature_thread, this);
    getTemperature.detach();

    m_Run = true;
    // run compensator on another thread,
    // or can't get image data after a while if running repair mode on this thread
    std::thread runCompensator(run_compensator_thread, this);
    runCompensator.detach();
}

bool SelfCalibration2Dlg::checkDepth()
{
    if (!m_pPreviewDlg->m_isPreviewed || m_pPreviewDlg->GetCurrentMode().D_Resolution.Width == 0 || m_pPreviewDlg->GetCurrentMode().D_Resolution.Height == 0) {
        CString  str;
        str.Format(_T("Must preview D"));
        AfxMessageBox(str, MB_ICONINFORMATION | MB_SYSTEMMODAL);
        return false;
    }
    return true;
}

void SelfCalibration2Dlg::StopSelfK2()
{
    {
        std::lock_guard< std::mutex > lock(m_CompensatorMutex);

        if (m_cy_compensator) {
            delete m_cy_compensator;
            m_cy_compensator = nullptr;
        }
        if (m_focal_compensator) {
            delete m_focal_compensator;
            m_focal_compensator = nullptr;
        }
        gStopGettingTemperature = true;
        m_Run = false;
    }
    SetDlgItemText(IDC_BTN_RUN, L"Run");
    ((CButton*)GetDlgItem(IDC_RADIO_RUNTIME))->SetCheck(TRUE);
    ((CButton*)GetDlgItem(IDC_RADIO_REPAIR))->SetCheck(FALSE);
    GetDlgItem(IDC_RADIO_RUNTIME)->EnableWindow(TRUE);
    GetDlgItem(IDC_RADIO_REPAIR)->EnableWindow(TRUE);
    GetDlgItem(IDC_BTN_RESET)->EnableWindow(FALSE);
    m_OutAndInfo.str("");
    PostMessage(WM_UPDATE_TEXT_MSG, UPDATE_TEXT);
    GetDlgItem(IDC_BTN_WRITE)->EnableWindow(FALSE);

    while (!m_ThreadTerminated) {
        TRACE("wait for compensator thread terminate\n");
        Sleep(10);
    }
}

void SelfCalibration2Dlg::OnBnClickedBtnRun()
{
    if (!m_Run) {
        if (checkDepth()){
            StartSelfK2();
        }
    }
    else {
        StopSelfK2();
    }
}

void SelfCalibration2Dlg::OnBnClickedBtnReset()
{
    m_Reset = true;
}

CFile_IO_Data_Structure_Rectify data_structure;
CFile_IO_Data_Structure_Rectify data_structure_scaled;
int n_read_buffer_data_items = 0;
int n_write_buffer_data_items = 0;
int n_data_structure_elements = HWAPI_RECTIFICATION_N_DATA_STRUCTURE_ELEMENTS;
uint16_t table_size = 0;

int reorder_rectify_buffer(unsigned char* p_buffer_reordered, unsigned char* p_buffer, int input_data_length, int& output_data_length) {
    if (_COUT_FUNCTION_CALL_LOG_)
        cout<<"@ reorder_rectify_buffer()"<<endl;

    // copy for elements not being reordered.
    unsigned char* Buffer_0_tmp = new unsigned char[input_data_length* N_RECTIFY_PAYLOAD_STEP_BYTES];
    unsigned char* Buffer_0 = new unsigned char[input_data_length* N_RECTIFY_PAYLOAD_STEP_BYTES];
    unsigned char* Buffer_1 = new unsigned char[input_data_length* N_RECTIFY_PAYLOAD_STEP_BYTES];
    memset(Buffer_0_tmp, 0, input_data_length* N_RECTIFY_PAYLOAD_STEP_BYTES);
    memset(Buffer_0, 0,input_data_length * N_RECTIFY_PAYLOAD_STEP_BYTES);
    memset(Buffer_1,0, input_data_length * N_RECTIFY_PAYLOAD_STEP_BYTES);

    if (_CALIB_HW_API_DEBUG_)
        printf("input_data_length: \n");
    int CNT = input_data_length;//(HWAPI_RECTIFICATION_TABLE_SIZE - 2) / 3;
    int cnt_0 = 0;
    int cnt_1 = 0;
    int max_id = 0;


    if (_CALIB_HW_API_DEBUG_) {
        cout << endl;
        printf(" Input RAW Reorder\n");
    }
    for (int i = 0; i < CNT; i++) {

        unsigned char addr = p_buffer[N_RECTIFY_BUFFER_PREFIX_BYTES + N_RECTIFY_PAYLOAD_STEP_BYTES * i + 0];
        unsigned char id = (unsigned char)p_buffer[N_RECTIFY_BUFFER_PREFIX_BYTES + N_RECTIFY_PAYLOAD_STEP_BYTES * i + 1];
        unsigned char byte_value = p_buffer[N_RECTIFY_BUFFER_PREFIX_BYTES + N_RECTIFY_PAYLOAD_STEP_BYTES * i + 2];

        // put data @ idx buffer position
        if (addr == REG_ADDR_RECTIFY) { // rectification address
            if (id > max_id)
                max_id = id;
            int idx = N_RECTIFY_PAYLOAD_STEP_BYTES * id;
            Buffer_0_tmp[idx + 0] = addr;
            Buffer_0_tmp[idx + 1] = id;
            Buffer_0_tmp[idx + 2] = byte_value;
            if (_CALIB_HW_API_DEBUG_)
                printf("%3.0d (cnt_0:%d): addr: 0x%x, id: 0x%x (%d), value: 0x%x \n", i, cnt_0, addr, id, id, byte_value);
            cnt_0++;
        }
        // put other data one by one (reorder), but will have some zeros (no data), will be removed later.
        else { // others
            if (addr != 0) {
                int idx = N_RECTIFY_PAYLOAD_STEP_BYTES * cnt_1;
                Buffer_1[idx + 0] = addr;
                Buffer_1[idx + 1] = id;
                Buffer_1[idx + 2] = byte_value;
                if (_CALIB_HW_API_DEBUG_)
                    printf("%3.0d (cnt_1:%d): addr: 0x%x, id: 0x%x (%d), value: 0x%x \n", i, cnt_1, addr, id, id, byte_value);
                cnt_1++;
            }
        }
    }

    // reorder + remove zero data (reorder)
    if (_CALIB_HW_API_DEBUG_) {
        cout << endl;
        printf("0xf5 REORDER TEST\n");
        printf("maxid : %d\n", max_id);
    }
    int cnt = 0;
    for (int i = 0; i <=max_id; i++) {
        int idx = N_RECTIFY_PAYLOAD_STEP_BYTES * i;
        unsigned char addr = Buffer_0_tmp[idx + 0];
        unsigned char id = Buffer_0_tmp[idx + 1];
        unsigned char byte_value = Buffer_0_tmp[idx + 2];

        if (addr == 0xF5) {
            if (_CALIB_HW_API_DEBUG_)
                printf("%3.0d (cnt:%d): addr: 0x%x, id: 0x%x (%d), value: 0x%x \n", i, cnt, addr, id, id, byte_value);
            int idx = N_RECTIFY_PAYLOAD_STEP_BYTES * cnt;
            Buffer_0[idx + 0] = addr;
            Buffer_0[idx + 1] = id;
            Buffer_0[idx + 2] = byte_value;
            cnt++;
        }
    }


    // 3. combine data
    // bytes prefix
    p_buffer_reordered[0] = p_buffer[0];
    p_buffer_reordered[1] = p_buffer[1];
    // copy rectification data
    memcpy(p_buffer_reordered + N_RECTIFY_BUFFER_PREFIX_BYTES, Buffer_0, sizeof(unsigned char) * cnt * N_RECTIFY_PAYLOAD_STEP_BYTES);
    // copy other data
    memcpy(p_buffer_reordered + N_RECTIFY_BUFFER_PREFIX_BYTES + cnt * N_RECTIFY_PAYLOAD_STEP_BYTES, Buffer_1, sizeof(unsigned char) * cnt_1 * N_RECTIFY_PAYLOAD_STEP_BYTES);

    //printf("buffer_size_0xf5: %d, buffer_size_others: %d, buffer_size_0xf5_zero_removed: %d\n", cnt_0, cnt_1, cnt);

    output_data_length = cnt_1 + cnt;

    if (_CALIB_HW_API_DEBUG_) {
        cout << endl;
        printf("OUTPUT TEST\n");
        printf("length_F5: %d, length_others: %d\n", cnt, cnt_1);
        printf("output data_length: %d\n", output_data_length);
        for (int i = 0; i < output_data_length; i++) {
            printf("%3.0d: addr: 0x%x, id: 0x%x , value: 0x%x \n", i,
                p_buffer_reordered[N_RECTIFY_BUFFER_PREFIX_BYTES + i * N_RECTIFY_PAYLOAD_STEP_BYTES + 0],
                p_buffer_reordered[N_RECTIFY_BUFFER_PREFIX_BYTES + i * N_RECTIFY_PAYLOAD_STEP_BYTES + 1],
                p_buffer_reordered[N_RECTIFY_BUFFER_PREFIX_BYTES + i * N_RECTIFY_PAYLOAD_STEP_BYTES + 2]);
        }
    }




    // release
    delete [] Buffer_0_tmp;
    delete [] Buffer_0;
    delete [] Buffer_1;

    if (_COUT_FUNCTION_CALL_LOG_)
        cout << "reorder_rectify_buffer() done." << endl;
    return 0;
}

int reorder_buffer(uint8_t* p_read_buffer_reordered, uint8_t* p_read_buffer) {

    int input_data_length = (APC_RECTIFY_FILE_SIZE - N_RECTIFY_BUFFER_PREFIX_BYTES) / (float)N_RECTIFY_PAYLOAD_STEP_BYTES; // possible max data size
    reorder_rectify_buffer(p_read_buffer_reordered, p_read_buffer, input_data_length, n_read_buffer_data_items);

    return 0;
}

int decode_buffer_data(void* p_buffer_tmp, void* p_data_structure_tmp) {
    if (_COUT_FUNCTION_CALL_LOG_)
        cout <<"@ CFile_IO_Rectify::decode_buffer_data()."<< endl;

    FL_t* p_data_structure = (FL_t*)p_data_structure_tmp;

    uint8_t* p_buffer = (uint8_t*)p_buffer_tmp;
    uint8_t* p_buffer_const = (uint8_t*)p_buffer_tmp;
    p_buffer_const += 2;

    //printf("0: %x, (0x%p)\n", p_buffer[0], &p_buffer[0]);
    //printf("1: %x, (0x%p)\n", p_buffer[1], &p_buffer[1]);

    int prefix_size = 2;
    int payload_offset = 2; // 0xf5, 0xXX(byte_offset), payload
    uint8_t addr = 0;
    uint8_t id = 0;
    uint8_t byte_value = 0;
    int offset = 0;
    int data_size = 0;

    // Prefix:
    p_buffer += prefix_size;

#if 0
    //1. RECTIFY_CONTROL
    data_size = Rectification_Data_Structure_Size_in_Bytes[0];
    addr = *(p_buffer++);
    id = *(p_buffer++);
    byte_value = *(p_buffer++);
    *(p_data_structure + 0) = (FL_t)((int8_t)byte_value);
    if (_CALIB_HW_API_DEBUG_) {
        printf("addr: 0x%x, id : 0x%x, byte_value : 0x%x\n", addr, id, byte_value);
    }
    offset++;

    //2. RECTIFY_MODULE_ENABLE
    data_size = Rectification_Data_Structure_Size_in_Bytes[1];
    addr = *(p_buffer++);
    id = *(p_buffer++);
    byte_value = *(p_buffer++);
    if (_CALIB_HW_API_DEBUG_) {
        printf("addr: 0x%x, id : 0x%x, byte_value : 0x%x\n", addr, id, byte_value);
    }
    *(p_data_structure + 1) = (byte_value & 0x01) >> 0; // 0000 0001
    *(p_data_structure + 2) = (byte_value & 0x02) >> 1; // 0000 0010
    *(p_data_structure + 3) = (byte_value & 0x04) >> 2; // 0000 0100
    *(p_data_structure + 4) = (byte_value & 0x18) >> 3; // 0001 1000
    *(p_data_structure + 5) = (byte_value & 0x20) >> 5; // 0010 0000
    *(p_data_structure + 6) = (byte_value & 0x40) >> 6; // 0100 0000
    *(p_data_structure + 7) = (byte_value & 0x80) >> 7; // 1000 0000
    offset++;
#endif

    for (int i = 0; i < n_data_structure_elements; i++) {
        if (_CALIB_HW_API_DEBUG_) {
            cout << endl;
            cout << i << endl;
        }

        //1. check if it's don't care bytes
        if (Rectification_Data_Structure_DontCare[i]) {
            if (_CALIB_HW_API_DEBUG_)
                printf("%d|dont care bytes, offset = 0x%x, lengths = %d bytes\n", i, Rectification_Data_Structure_Bytes_Offset[i], Rectification_Data_Structure_Size_in_Bytes[i]);
            continue;
        }

        //2. read relative meta information of this data
        int byte_offset = Rectification_Data_Structure_Bytes_Offset[i];
        int fractional_bits = Rectification_Data_Structure_Fractional_Bits[i];
        bool b_little_endian = Rectification_Data_Structure_Little_Endian[i];
        int data_size_bits = Rectification_Data_Structure_Size_in_Bits[i];
        int high_byte_msb_idx = Rectification_Data_Structure_HighByte_MSB_Index[i];
        int high_byte_lsb_idx = Rectification_Data_Structure_HighByte_LSB_Index[i];
        int tmp = pow(2, data_size_bits);
        data_size = Rectification_Data_Structure_Size_in_Bytes[i];
        //p_buffer = prefix_size + (uint8_t*)p_buffer_tmp + Rectification_Data_Structure_Bytes_Offset[i] * N_RECTIFY_PAYLOAD_STEP_BYTES + payload_offset;

        FL_t scaling = 0;
        if (fractional_bits > 0)
            scaling = pow(2, fractional_bits);
        else
            scaling = 1.0 / (FL_t)pow(2, -fractional_bits);

        if (_CALIB_HW_API_DEBUG_) {
            printf("bytes_offset: %d (0x%x)\n", byte_offset, byte_offset);
            cout << "data_size (bytes): " << data_size << endl;
            cout << "data_size (bits): " << data_size_bits << endl;
            printf("scaling: %f (%d)\n", scaling, fractional_bits);
            printf("b_little_endian: %d\n", b_little_endian);
        }

        // Scan, check if input buffer has the item of this (Addr +ID)?
        int offset = 0;
        bool b_found_item = false;
        int max_data_length = (APC_RECTIFY_FILE_SIZE - N_RECTIFY_BUFFER_PREFIX_BYTES) / (float)N_RECTIFY_PAYLOAD_STEP_BYTES; // possible max data size
        for (int i = 0; i < max_data_length; i++) {

            // data of buffer
            uint8_t addr = p_buffer_const[N_RECTIFY_PAYLOAD_STEP_BYTES * i + 0];
            uint8_t id = (uint8_t)p_buffer_const[N_RECTIFY_PAYLOAD_STEP_BYTES * i + 1];
            uint8_t byte_value = p_buffer_const[N_RECTIFY_PAYLOAD_STEP_BYTES * i + 2];
            if (addr == 0) {
                break;
            }
            else {
                if ((id == byte_offset) & (addr == REG_ADDR_RECTIFY)) { // id (address) match?
                    offset = N_RECTIFY_PAYLOAD_STEP_BYTES * i;
                    p_buffer = p_buffer_const + offset;
                    b_found_item = true;
                }
            }
        }
        if (b_found_item == false) {
            printf("(b_found_item == false)\n");
            continue;
        }

        // set byte value
        int32_t int_value = 0;
        for (int n = 0; n < data_size; n++) {

            addr = *(p_buffer++);
            id = *(p_buffer++);
            byte_value = *(p_buffer++);

            // @high byte, use mask to prevent data of others influence the result.
            bool b_high_byte = (b_little_endian) ? (n == data_size - 1) : (n == 0);
            if (b_high_byte) {
                // create mask
                uint8_t mask = 0x00;
                for (int b = high_byte_lsb_idx; b <= high_byte_msb_idx; b++) {
                    mask |= (1 << b);
                }
                if (_CALIB_HW_API_DEBUG_)
                    printf("mask : 0x%x\n", mask);

                // use mask to extract data
                byte_value &= mask;
                byte_value = byte_value >> high_byte_lsb_idx;
            }

            if (b_little_endian) {
                int_value |= ((int32_t)byte_value) << (n * 8);
            }
            else { // Big-Endian
                int_value |= ((int32_t)byte_value) << (((data_size - 1) - n) * 8);
            }

            if (_CALIB_HW_API_DEBUG_) {
                printf("addr: 0x%x, id : 0x%x, byte_value : 0x%x\n", addr, id, byte_value);
            }
        }

        if (int_value > (tmp / 2)) { // sign = neg
            if (_CALIB_HW_API_DEBUG_)
                printf("NEG: int_value: %d, tmp:%d\n ", int_value, tmp);
            int_value = int_value - tmp;
        }

        // float value
        FL_t float_value = (FL_t)int_value / scaling;
        *((FL_t*)&data_structure_scaled + i) = int_value;
        *(p_data_structure + i) = float_value;

        if (_CALIB_HW_API_DEBUG_) {
            printf("int_value : %d (0x%x)\n", int_value, int_value);
            cout << "float value: " << float_value << endl;
        }

    } //  for (int i = 8; i < n_data_structure_elements; i++) {N_RECTIFY_BUFFER_PREFIX_BYTES

    if (_COUT_FUNCTION_CALL_LOG_)
        cout << "@ CFile_IO_Rectify::decode_buffer_data() done." << endl;

    return 0;
}

int get_data_from_data_structure(CFile_IO_User_Data_Structure_Rectify& c_data) {

    if (_COUT_FUNCTION_CALL_LOG_)
        cout << "@ CFile_IO_Rectify::get_data_from_data_structure(CFile_IO_User_Data_Structure_Rectify)." << endl;

    memcpy(c_data.M_L, &data_structure.RECTIFY_L_RECT_M00, sizeof(double) * 9);
    memcpy(c_data.M_R, &data_structure.RECTIFY_R_RECT_M00, sizeof(double) * 9);

    memcpy(c_data.K_L, &data_structure.RECTIFY_L_RECT_K1, sizeof(double) * 6);
    memcpy(c_data.K_R, &data_structure.RECTIFY_R_RECT_K1, sizeof(double) * 6);



    c_data.p1_L = data_structure.RECTIFY_L_RECT_P1;
    c_data.p2_L = data_structure.RECTIFY_L_RECT_P2;
    c_data.p1_R = data_structure.RECTIFY_R_RECT_P1;
    c_data.p2_R = data_structure.RECTIFY_R_RECT_P2;

    c_data.cx_L = data_structure.RECTIFY_L_RECT_CX;
    c_data.cy_L = data_structure.RECTIFY_L_RECT_CY;
    c_data.fx_L = data_structure.RECTIFY_L_RECT_FX;
    c_data.fy_L = data_structure.RECTIFY_L_RECT_FY;

    c_data.cx_R = data_structure.RECTIFY_R_RECT_CX;
    c_data.cy_R = data_structure.RECTIFY_R_RECT_CY;
    c_data.fx_R = data_structure.RECTIFY_R_RECT_FX;
    c_data.fy_R = data_structure.RECTIFY_R_RECT_FY;

    if (_COUT_FUNCTION_CALL_LOG_)
        cout << "@ CFile_IO_Rectify::get_data_from_data_structure() done." << endl;

    return 0;
}

int set_data_to_data_structure(CFile_IO_User_Data_Structure_Rectify c_data) {
    if (_COUT_FUNCTION_CALL_LOG_)
        cout << "@ CFile_IO_Rectify::set_data_to_data_structure(CFile_IO_User_Data_Structure_Rectify)." << endl;

    CFile_IO_User_Data_Structure_Rectify* p_c_data = &c_data;

    memcpy(&data_structure.RECTIFY_L_RECT_M00, p_c_data->M_L, sizeof(double) * 9);
    memcpy(&data_structure.RECTIFY_R_RECT_M00, p_c_data->M_R, sizeof(double) * 9);

    memcpy(&data_structure.RECTIFY_L_RECT_K1, p_c_data->K_L, sizeof(double) * 6);
    memcpy(&data_structure.RECTIFY_R_RECT_K1, p_c_data->K_R, sizeof(double) * 6);

    data_structure.RECTIFY_L_RECT_P1 = p_c_data->p1_L;
    data_structure.RECTIFY_L_RECT_P2 = p_c_data->p2_L;
    data_structure.RECTIFY_R_RECT_P1 = p_c_data->p1_R;
    data_structure.RECTIFY_R_RECT_P2 = p_c_data->p2_R;

    data_structure.RECTIFY_L_RECT_CX = p_c_data->cx_L;
    data_structure.RECTIFY_L_RECT_CY = p_c_data->cy_L;
    data_structure.RECTIFY_L_RECT_FX = p_c_data->fx_L;
    data_structure.RECTIFY_L_RECT_FY = p_c_data->fy_L;

    data_structure.RECTIFY_R_RECT_CX = p_c_data->cx_R;
    data_structure.RECTIFY_R_RECT_CY = p_c_data->cy_R;
    data_structure.RECTIFY_R_RECT_FX = p_c_data->fx_R;
    data_structure.RECTIFY_R_RECT_FY = p_c_data->fy_R;

    if (_COUT_FUNCTION_CALL_LOG_)
        cout << "@ CFile_IO_Rectify::set_data_to_data_structure() done." << endl;
    return 0;
}

int cal_data_to_bytes_big_endian(uint16_t data, uint8_t* p_data) {
    p_data[0] = (data >> 8) & 0x00ff;
    p_data[1] = data & 0x00ff;

    return 0;
}

int encode_buffer_data(void* p_buffer_tmp, void* p_data_structure_tmp) {
    if (_COUT_FUNCTION_CALL_LOG_)
        cout<<"@CFile_IO_Rectify::encode_buffer_data()."<<endl;

    FL_t float_value = 0;
    FL_t* p_data_structure = (FL_t*)p_data_structure_tmp;
    uint8_t addr = 0;
    uint8_t id = 0;
    uint8_t byte_value = 0;
    int offset = 0;
    int data_size = 0;

    /*
    1. create p_buffer_tmp2 to save original data read from flash + new data to be updated.
    2.  zero items in p_buffer (new data to be updated) will be removed by buffer reordering function.
    3. items in read data will be replaced by items in p_buffer @ reordering function.
    4. Finally,  copy the renew data to write buffer.
    */

    /* 1. download data to p_buffer_tmp2 */
    uint8_t* p_buffer_tmp2 = new uint8_t[2048];
    memset(p_buffer_tmp2, 0, 2048);

    // copy read data to p_buffer_tmp2
    memcpy(p_buffer_tmp2, p_buffer_tmp, N_RECTIFY_BUFFER_PREFIX_BYTES + n_read_buffer_data_items * N_RECTIFY_PAYLOAD_STEP_BYTES);

    /*
    2. to combine read data and new data (new data must be placed after read data),
    p_buffer is put after the read items = (p_buffer_tmp2 + prefix_size) + n_read_buffer_data_items * N_RECTIFY_PAYLOAD_STEP_BYTES.
    */
    uint8_t* p_buffer = (p_buffer_tmp2 + N_RECTIFY_BUFFER_PREFIX_BYTES) + n_read_buffer_data_items * N_RECTIFY_PAYLOAD_STEP_BYTES;


#if 0
    //1. RECTIFY_CONTROL
    float_value = *(p_data_structure + 0);
    data_size = Rectification_Data_Structure_Size_in_Bytes[0];
    addr = 0xF5;
    id = Rectification_Data_Structure_Bytes_Offset[0];
    byte_value = (unsigned char)float_value; // byte value
    *(p_buffer    + 0* N_RECTIFY_PAYLOAD_STEP_BYTES+0) = addr; // addr
    *(p_buffer + 0 * N_RECTIFY_PAYLOAD_STEP_BYTES + 1) = id; // id
    *(p_buffer + 0 * N_RECTIFY_PAYLOAD_STEP_BYTES + 2) = byte_value;
    if (_CALIB_HW_API_DEBUG_) {
        printf("addr: 0x%x, id : 0x%x, byte_value : 0x%x\n", addr, id, byte_value);
    }



    //2. RECTIFY_MODULE_ENABLE (1~7 elements in data structure.)
    uint8_t tmp_value = 0;
    tmp_value |= (((uint8_t) * (p_data_structure + 1)) & 0x01) << 0;  // 0000 0001
    tmp_value |= (((uint8_t) * (p_data_structure + 2)) & 0x01) << 1; // 0000 0010
    tmp_value |= (((uint8_t) * (p_data_structure + 3)) & 0x01) << 2; // 0000 0100
    tmp_value |= (((uint8_t) * (p_data_structure + 4)) & 0x03) << 3; // 0001 1000
    tmp_value |= (((uint8_t) * (p_data_structure + 5)) & 0x01) << 5; // 0010 0000
    tmp_value |= (((uint8_t) * (p_data_structure + 6)) & 0x01) << 6; // 0100 0000
    tmp_value |= (((uint8_t) * (p_data_structure + 7)) & 0x01) << 7; // 1000 0000
    addr = 0xF5;
    id = Rectification_Data_Structure_Bytes_Offset[1];
    byte_value = tmp_value;
    *(p_buffer + 1 * N_RECTIFY_PAYLOAD_STEP_BYTES + 0) = addr; // addr
    *(p_buffer + 1 * N_RECTIFY_PAYLOAD_STEP_BYTES + 1) = id; // id
    *(p_buffer + 1 * N_RECTIFY_PAYLOAD_STEP_BYTES + 2) = byte_value;
    if (_CALIB_HW_API_DEBUG_) {
        printf("addr: 0x%x, id : 0x%x, byte_value : 0x%x\n", addr, id, byte_value);
    }
#endif

    //3.  for each item
    for (int i = 0; i < n_data_structure_elements; i++) {

        if (_CALIB_HW_API_DEBUG_){
            cout << endl;
            cout << i << endl;
        }

        //1. check if it's don't care bytes
        if (Rectification_Data_Structure_DontCare[i]) {
            if (_CALIB_HW_API_DEBUG_)
                printf("%d| dont care bytes, offset = 0x%x, lengths = %d bytes\n", i, Rectification_Data_Structure_Bytes_Offset[i], Rectification_Data_Structure_Size_in_Bytes[i]);
            continue;
        }

        //2. read relative meta information of this data
        int byte_offset = Rectification_Data_Structure_Bytes_Offset[i];
        int fractional_bits = Rectification_Data_Structure_Fractional_Bits[i];
        bool b_little_endian = Rectification_Data_Structure_Little_Endian[i];
        int data_size_bits = Rectification_Data_Structure_Size_in_Bits[i];
        int high_byte_msb_idx = Rectification_Data_Structure_HighByte_MSB_Index[i];
        int high_byte_lsb_idx = Rectification_Data_Structure_HighByte_LSB_Index[i];
        int tmp = pow(2, data_size_bits);
        data_size = Rectification_Data_Structure_Size_in_Bytes[i];

        FL_t scaling = 0;
        if (fractional_bits > 0)
            scaling = pow(2, fractional_bits);
        else
            scaling = 1.0 / (FL_t)pow(2, -fractional_bits);

        if (_CALIB_HW_API_DEBUG_) {
            printf("bytes_offset: %d (0x%x)\n", byte_offset, byte_offset);
            cout << "data_size (bytes): " << data_size << endl;
            cout << "data_size (bits): " << data_size_bits << endl;
            printf("scaling: %f (%d)\n", scaling, fractional_bits);
            printf("b_little_endian: %d\n", b_little_endian);
        }

        // set byte value
        // read input float value
        float_value = *(p_data_structure + i);
        if (_CALIB_HW_API_DEBUG_)
            printf("float_value: %f\n", float_value);
        // do scaling
        float_value = float_value * scaling;
        if (_CALIB_HW_API_DEBUG_)
            printf("float_value (scaled): %f\n", float_value);
        *((FL_t*)&data_structure_scaled + i) = float_value;

        // 2's complement for sign data
        if (float_value < 0) { // sign = neg
            float_value = tmp + float_value;
            if (_CALIB_HW_API_DEBUG_)
                printf("NEG: float_value: %d, tmp:%d\n ", float_value, tmp);
        }


        // byte value
        int32_t int_value = 0;
        uint32_t int_value_tmp = (uint32_t)(round(float_value)); // @20230621: round

        for (int n = 0; n < data_size; n++) {
            // shift to extract byte values
            uint8_t byte_value = 0;
            // assign value
            if (b_little_endian) {
                byte_value = (int_value_tmp >> (n * 8)) & 0x000000ffu;
            }
            else { // Big-Endian
                byte_value = (int_value_tmp >> (((data_size - 1) - n) * 8)) & 0x000000ffu;
            }

            // @high byte, use mask to keep original data in p_buffer.
            bool b_high_byte = (b_little_endian) ? (n == data_size - 1) : (n == 0);
            if (b_high_byte) {
                // create mask
                uint8_t mask = 0x00;
                for (int b = high_byte_lsb_idx; b <= high_byte_msb_idx; b++) {
                    mask |= (1 << b);
                }
                if (_CALIB_HW_API_DEBUG_)
                    printf("mask : 0x%x\n", mask);

                // keep original data and assign new data
                uint8_t org_byte_value = *(p_buffer + (byte_offset + n) * N_RECTIFY_PAYLOAD_STEP_BYTES + 2);//*(p_buffer + 2); // get original data 1110xxx1
                if (_CALIB_HW_API_DEBUG_)
                    printf("org_byte_value : 0x%x\n", org_byte_value);
                org_byte_value &= (~mask); //  use mask to rest data of mask bits.  1110 <000> 1
                if (_CALIB_HW_API_DEBUG_)
                    printf("org_byte_value &= (~mask) : 0x%x\n", org_byte_value);
                org_byte_value |= (byte_value<< high_byte_lsb_idx); // shift data to specific bit position, data = 00000<101>, --> after shift 1 bit= 00001010 --> after or =  1110<101>1
                if (_CALIB_HW_API_DEBUG_)
                    printf("org_byte_value |= (byte_value<< high_byte_lsb_idx) : 0x%x\n", org_byte_value);
                byte_value = org_byte_value;
            }

            // assign value
            addr = 0xf5;
            id = Rectification_Data_Structure_Bytes_Offset[i] + n;
            *(p_buffer + (byte_offset + n) * N_RECTIFY_PAYLOAD_STEP_BYTES + 0) = addr; // addr
            *(p_buffer + (byte_offset + n) * N_RECTIFY_PAYLOAD_STEP_BYTES + 1) = id; // id
            *(p_buffer + (byte_offset + n) * N_RECTIFY_PAYLOAD_STEP_BYTES + 2) = byte_value;
            //*(p_buffer++) = addr; // addr
            //*(p_buffer++) = id; // id
            //*(p_buffer++) = byte_value;

            if (_CALIB_HW_API_DEBUG_) {
                printf("addr: 0x%x, id : 0x%x, byte_value : 0x%x\n", addr, id, byte_value);
            }
        } // for (int n = 0; n < data_size; n++) {
    } //  for (int i = 8; i < n_data_structure_elements; i++) {


    // 4. cal total item numbers (readed+ new)
    int n_encoded_items =
        Rectification_Data_Structure_Bytes_Offset[HWAPI_RECTIFICATION_N_DATA_STRUCTURE_ELEMENTS - 1] +
        Rectification_Data_Structure_Size_in_Bytes[HWAPI_RECTIFICATION_N_DATA_STRUCTURE_ELEMENTS - 1]-1; // including zero items

    if (_CALIB_HW_API_DEBUG_)
        printf("n_read_buffer_data_items %d, size_tmp %d\n", n_read_buffer_data_items, n_encoded_items);

    int input_data_length = n_read_buffer_data_items + n_encoded_items; // size_tmp including zero bytes (skipped ID), and will removed by reorder_rectify_buffer.


    // 5. reorder
    reorder_rectify_buffer(
        (uint8_t*)p_buffer_tmp,  // write buffer
        p_buffer_tmp2, input_data_length,
        n_write_buffer_data_items); // will remove zero items

    //6.  set size information to the prefix of buffer (2bytes), use big endian
    uint16_t org_table_size = (uint16_t)(((uint8_t*)p_buffer_tmp)[0])<<8 | (uint16_t)(((uint8_t*)p_buffer_tmp)[1]);
    table_size = N_RECTIFY_BUFFER_PREFIX_BYTES + n_write_buffer_data_items * N_RECTIFY_PAYLOAD_STEP_BYTES;
    cal_data_to_bytes_big_endian(table_size, (uint8_t*)p_buffer_tmp);

    if (_CALIB_HW_API_DEBUG_) {
        printf("table_size:%d\n", table_size);
        printf("p_buffer_tmp[0]:0x%x    ,[1]:0x%x\n", ((uint8_t*)p_buffer_tmp)[0], ((uint8_t*)p_buffer_tmp)[1]);
    }

    if (_COUT_FUNCTION_CALL_LOG_)
        cout << "@CFile_IO_Rectify::encode_buffer_data() done." << endl;

    return 0;
}

void SelfCalibration2Dlg::OnBnClickedBtnWrite()
{
    if (m_cy_compensator == nullptr) return;

    CString csMessage;
    csMessage.Format(L"Write comp_cy : %.9lf to flash ?", m_cy_compensator->oputs.comp_cy);
    if (AfxMessageBox(csMessage, MB_YESNO) != IDYES) return;

    CFile_IO_User_Data_Structure_Rectify c_file_io_user_data_structure_rectify;
    BYTE p_read_buffer_reordered[APC_RECTIFY_FILE_SIZE] = { 0 };
    BYTE p_read_buffer[APC_RECTIFY_FILE_SIZE] = { 0 };
    BYTE p_write_buffer[APC_RECTIFY_FILE_SIZE] = { 0 };
    int actualLen = 0;
    int ret = APC_GetRectifyTable(m_hApcDI, &m_DevSelInfo, p_read_buffer, APC_RECTIFY_FILE_SIZE, &actualLen, m_FileSystemIndex + FW_FID_GROUP_OFFSET);

    /*FILE * pFile;
    pFile = fopen("D:/p_read_buffer.bin", "wb");
    fwrite(p_read_buffer, 1, APC_RECTIFY_FILE_SIZE, pFile);
    fclose(pFile);*/

    reorder_buffer(p_read_buffer_reordered, p_read_buffer);
    decode_buffer_data(p_read_buffer_reordered, &data_structure);

    int n_read_data_buffer_bytes = 2 + n_read_buffer_data_items * N_RECTIFY_PAYLOAD_STEP_BYTES;
    memcpy(p_write_buffer, p_read_buffer_reordered, n_read_data_buffer_bytes);

    CFile_IO_User_Data_Structure_Rectify c_data;
    get_data_from_data_structure(c_data);

    float default_cy = c_data.cy_R;
    TRACE("default_cy:%f\n", default_cy);
    c_data.cy_R = m_cy_compensator->oputs.comp_cy;

    set_data_to_data_structure(c_data);
    encode_buffer_data(p_write_buffer, &data_structure);

    /*pFile = fopen("D:/p_write_buffer.bin", "wb");
    fwrite(p_write_buffer, 1, APC_RECTIFY_FILE_SIZE, pFile);
    fclose(pFile);*/
    ret = APC_SetRectifyTable(m_hApcDI, &m_DevSelInfo, p_write_buffer, APC_RECTIFY_FILE_SIZE, &actualLen, m_FileSystemIndex + FW_FID_GROUP_OFFSET);
}
