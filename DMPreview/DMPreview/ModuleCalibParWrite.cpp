#include "ModuleCalibParWrite.h"

int ReadRectify_Table_Data(eSPCtrl_RectLogData &rectifyData, void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, int index)
{

    // Get rectify log data
    APC_GetRectifyMatLogData(pHandleEtronDI, pDevSelInfo, &rectifyData, index);

    double NP1[9] = {0}, NP2[9] = {0}, R1[9] = {0}, R2[9] = {0}, M1[9] = {0}, M2[9] = {0}, iM1[9] = {0}, iM2[9] = {0};
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            NP1[3 * i + j] = rectifyData.NewCamMat1[4 * i + j];
            NP2[3 * i + j] = rectifyData.NewCamMat2[4 * i + j];
            R1[3 * i + j] = rectifyData.LRotaMat[3 * i + j];
            R2[3 * i + j] = rectifyData.RRotaMat[3 * i + j];
        }
    }

    double fix_ratio1 = rectifyData.ParameterRatio[0];
    if (fix_ratio1 == 0)
    {
        fix_ratio1 = 1;
    }
    // Get Rectify Table Data
    int nbfferLength = 1024;
    int pActualLength = 0;
    BYTE *data = new BYTE[nbfferLength];
    memset(data, 0x0, nbfferLength);

    double A_NP1[9] = {0}, A_R1[9] = {0}, A_M1[9] = {0}, A_iM1[9] = {0};
    if (APC_OK == APC_GetRectifyTable(pHandleEtronDI, pDevSelInfo, data, nbfferLength, &pActualLength, index))
    {
        cout << "success to read rectify table data" << endl;

        A_iM1[0] = double(readBuffer({22, 25}, data, 16)) / pow(2, 21) * fix_ratio1;
        A_iM1[1] = double(readBuffer({28, 31}, data, 11)) / pow(2, 21) * fix_ratio1;
        A_iM1[2] = double(readBuffer({34, 37, 40}, data, 19)) / pow(2, 17) * fix_ratio1;
        A_iM1[3] = -double(readBuffer({28, 31}, data, 11)) / pow(2, 21) * fix_ratio1;
        A_iM1[4] = double(readBuffer({22, 25}, data, 16)) / pow(2, 21) * fix_ratio1;
        A_iM1[5] = double(readBuffer({55, 58, 61}, data, 19)) / pow(2, 17) * fix_ratio1;
        A_iM1[6] = double(readBuffer({64, 67}, data, 12)) / pow(2, 21);
        A_iM1[7] = double(readBuffer({70, 73}, data, 10)) / pow(2, 21);
        A_iM1[8] = double(readBuffer({76, 79, 82}, data, 19)) / pow(2, 17);

        Mat_Inverse(A_iM1, A_M1);
        Mat_Inverse(R1, A_R1);
        Mat_Product(A_M1, A_R1, A_NP1);

        int fx = round(A_NP1[0] - NP1[0]);
        int fy = round(A_NP1[4] - NP1[4]);
        int cx = round(A_NP1[2] - NP1[2]);
        int cy = round(A_NP1[5] - NP1[5]);
        cout << fx << endl;
        cout << fy << endl;
        cout << cx << endl;
        cout << cy << endl;
    }
    else
    {
        cout << "fail to read rectify table data" << endl;
    }

    return 0;
}

int Updata_ROI(eSPCtrl_RectLogData &rectifyData, void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, int index)
{
    // Get rectify log data
    eSPCtrl_RectLogData *tmp_rectifyLogData = nullptr;
    tmp_rectifyLogData = (eSPCtrl_RectLogData *)malloc(sizeof(eSPCtrl_RectLogData));
    APC_GetRectifyMatLogData(pHandleEtronDI, pDevSelInfo, tmp_rectifyLogData, index);

    double NP1[9] = {0}, R1[9] = {0};
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            NP1[3 * i + j] = tmp_rectifyLogData->NewCamMat1[4 * i + j];
            R1[3 * i + j] = tmp_rectifyLogData->LRotaMat[3 * i + j];
        }
    }

    double fix_ratio1 = tmp_rectifyLogData->ParameterRatio[0];
    if (fix_ratio1 == 0)
    {
        fix_ratio1 = 1;
    }

    // Get Rectify Table Data
    int nbfferLength = 1024;
    int pActualLength = 0;
    BYTE *data = new BYTE[nbfferLength];
    memset(data, 0x0, nbfferLength);

    double A_NP1[9] = {0}, A_R1[9] = {0}, A_M1[9] = {0}, A_iM1[9] = {0};
    if (APC_OK == APC_GetRectifyTable(pHandleEtronDI, pDevSelInfo, data, nbfferLength, &pActualLength, index))
    {
        cout << "success to read rectify table data" << endl;
        A_iM1[0] = double(readBuffer({22, 25}, data, 16)) / pow(2, 21) * fix_ratio1;
        A_iM1[1] = double(readBuffer({28, 31}, data, 11)) / pow(2, 21) * fix_ratio1;
        A_iM1[2] = double(readBuffer({34, 37, 40}, data, 19)) / pow(2, 17) * fix_ratio1;
        A_iM1[3] = -double(readBuffer({28, 31}, data, 11)) / pow(2, 21) * fix_ratio1;
        A_iM1[4] = double(readBuffer({22, 25}, data, 16)) / pow(2, 21) * fix_ratio1;
        A_iM1[5] = double(readBuffer({55, 58, 61}, data, 19)) / pow(2, 17) * fix_ratio1;
        A_iM1[6] = double(readBuffer({64, 67}, data, 12)) / pow(2, 21);
        A_iM1[7] = double(readBuffer({70, 73}, data, 10)) / pow(2, 21);
        A_iM1[8] = double(readBuffer({76, 79, 82}, data, 19)) / pow(2, 17);

        Mat_Inverse(A_iM1, A_M1);
        Mat_Inverse(R1, A_R1);
        Mat_Product(A_M1, A_R1, A_NP1);

        int fx = round(A_NP1[0] - NP1[0]);
        int fy = round(A_NP1[4] - NP1[4]);
        int cx = round(A_NP1[2] - NP1[2]);
        int cy = round(A_NP1[5] - NP1[5]);
        cout << fx << endl;
        cout << fy << endl;
        cout << cx << endl;
        cout << cy << endl;
        rectifyData.NewCamMat1[0] = rectifyData.NewCamMat1[0] + fx;
        rectifyData.NewCamMat1[2] = rectifyData.NewCamMat1[2] + cx;
        rectifyData.NewCamMat1[5] = rectifyData.NewCamMat1[5] + fy;
        rectifyData.NewCamMat1[6] = rectifyData.NewCamMat1[6] + cy;

        rectifyData.NewCamMat2[0] = rectifyData.NewCamMat2[0] + fx;
        rectifyData.NewCamMat2[2] = rectifyData.NewCamMat2[2] + cx;
        rectifyData.NewCamMat2[5] = rectifyData.NewCamMat2[5] + fy;
        rectifyData.NewCamMat2[6] = rectifyData.NewCamMat2[6] + cy;
    }
    else
    {
        cout << "fail to read rectify table data" << endl;
    }

    return 0;
}

int WriteRectLogData_temp(eSPCtrl_RectLogData &rectifyData, void *pHandleEtronDI, PDEVSELINFO pDevSelInfo)
{
    double NP1[9] = {0}, NP2[9] = {0}, R1[9] = {0}, R2[9] = {0}, M1[9] = {0}, M2[9] = {0}, iM1[9] = {0}, iM2[9] = {0};
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            NP1[3 * i + j] = rectifyData.NewCamMat1[4 * i + j];
            NP2[3 * i + j] = rectifyData.NewCamMat2[4 * i + j];
            R1[3 * i + j] = rectifyData.LRotaMat[3 * i + j];
            R2[3 * i + j] = rectifyData.RRotaMat[3 * i + j];
        }
    }
    Mat_Product(NP1, R1, M1);
    Mat_Product(NP2, R2, M2);
    Mat_Inverse(M1, iM1);
    Mat_Inverse(M2, iM2);

    unsigned short tmp1, tmp2, tmp3;
    unsigned int data;
    int effective_bits;
    int scaling_bits;
    double fix_ratio1 = rectifyData.ParameterRatio[0];
    double fix_ratio2 = rectifyData.ParameterRatio[1];
    if (isnan(fix_ratio1))
    {
        fix_ratio1 = 1;
    }
    if (isnan(fix_ratio2))
    {
        fix_ratio2 = 1;
    }
    if (fix_ratio1 == 0)
    {
        fix_ratio1 = 1;
    }
    if (fix_ratio2 == 0)
    {
        fix_ratio2 = fix_ratio1;
    }

    // L_M00
    effective_bits = 16;
    scaling_bits = 21;
    data = int(iM1[0] * pow(2, scaling_bits) / fix_ratio1);
    data = data & (((unsigned int)(-1)) >> ((8 * sizeof(int)) - effective_bits));
    tmp1 = 0;
    tmp2 = 0;
    tmp3 = 0;
    tmp1 = data & 0x00FF;
    tmp2 = (data >> 8) & 0x00FF;
    tmp3 = (data >> 16) & 0x00FF;
    APC_SetHWRegister(pHandleEtronDI, pDevSelInfo, 0xF506, tmp1, FG_Address_2Byte | FG_Value_1Byte);
    APC_SetHWRegister(pHandleEtronDI, pDevSelInfo, 0xF507, tmp2, FG_Address_2Byte | FG_Value_1Byte);

    // L_M01
    effective_bits = 11;
    scaling_bits = 21;
    data = int(iM1[1] * pow(2, scaling_bits) / fix_ratio1);
    data = data & (((unsigned int)(-1)) >> ((8 * sizeof(int)) - effective_bits));
    tmp1 = 0;
    tmp2 = 0;
    tmp3 = 0;
    tmp1 = data & 0x00FF;
    tmp2 = (data >> 8) & 0x00FF;
    tmp3 = (data >> 16) & 0x00FF;
    APC_SetHWRegister(pHandleEtronDI, pDevSelInfo, 0xF508, tmp1, FG_Address_2Byte | FG_Value_1Byte);
    APC_SetHWRegister(pHandleEtronDI, pDevSelInfo, 0xF509, tmp2, FG_Address_2Byte | FG_Value_1Byte);
    // L_M02
    effective_bits = 19;
    scaling_bits = 17;
    data = int(iM1[2] * pow(2, scaling_bits) / fix_ratio1);
    data = data & (((unsigned int)(-1)) >> ((8 * sizeof(int)) - effective_bits));
    tmp1 = 0;
    tmp2 = 0;
    tmp3 = 0;
    tmp1 = data & 0x00FF;
    tmp2 = (data >> 8) & 0x00FF;
    tmp3 = (data >> 16) & 0x00FF;
    APC_SetHWRegister(pHandleEtronDI, pDevSelInfo, 0xF50A, tmp1, FG_Address_2Byte | FG_Value_1Byte);
    APC_SetHWRegister(pHandleEtronDI, pDevSelInfo, 0xF50B, tmp2, FG_Address_2Byte | FG_Value_1Byte);
    APC_SetHWRegister(pHandleEtronDI, pDevSelInfo, 0xF50C, tmp3, FG_Address_2Byte | FG_Value_1Byte);
    // L_M10
    effective_bits = 11;
    scaling_bits = 21;
    data = int(iM1[3] * pow(2, scaling_bits) / fix_ratio1);
    data = data & (((unsigned int)(-1)) >> ((8 * sizeof(int)) - effective_bits));
    tmp1 = 0;
    tmp2 = 0;
    tmp3 = 0;
    tmp1 = data & 0x00FF;
    tmp2 = (data >> 8) & 0x00FF;
    tmp3 = (data >> 16) & 0x00FF;
    APC_SetHWRegister(pHandleEtronDI, pDevSelInfo, 0xF50D, tmp1, FG_Address_2Byte | FG_Value_1Byte);
    APC_SetHWRegister(pHandleEtronDI, pDevSelInfo, 0xF50E, tmp2, FG_Address_2Byte | FG_Value_1Byte);
    // L_M11
    effective_bits = 16;
    scaling_bits = 21;
    data = int(iM1[4] * pow(2, scaling_bits) / fix_ratio1);
    data = data & (((unsigned int)(-1)) >> ((8 * sizeof(int)) - effective_bits));
    tmp1 = 0;
    tmp2 = 0;
    tmp3 = 0;
    tmp1 = data & 0x00FF;
    tmp2 = (data >> 8) & 0x00FF;
    tmp3 = (data >> 16) & 0x00FF;
    APC_SetHWRegister(pHandleEtronDI, pDevSelInfo, 0xF50F, tmp1, FG_Address_2Byte | FG_Value_1Byte);
    APC_SetHWRegister(pHandleEtronDI, pDevSelInfo, 0xF510, tmp2, FG_Address_2Byte | FG_Value_1Byte);
    // L_M12
    effective_bits = 19;
    scaling_bits = 17;
    data = int(iM1[5] * pow(2, scaling_bits) / fix_ratio1);
    data = data & (((unsigned int)(-1)) >> ((8 * sizeof(int)) - effective_bits));
    tmp1 = 0;
    tmp2 = 0;
    tmp3 = 0;
    tmp1 = data & 0x00FF;
    tmp2 = (data >> 8) & 0x00FF;
    tmp3 = (data >> 16) & 0x00FF;
    APC_SetHWRegister(pHandleEtronDI, pDevSelInfo, 0xF511, tmp1, FG_Address_2Byte | FG_Value_1Byte);
    APC_SetHWRegister(pHandleEtronDI, pDevSelInfo, 0xF512, tmp2, FG_Address_2Byte | FG_Value_1Byte);
    APC_SetHWRegister(pHandleEtronDI, pDevSelInfo, 0xF513, tmp3, FG_Address_2Byte | FG_Value_1Byte);
    // L_M20
    effective_bits = 12;
    scaling_bits = 21;
    data = int(iM1[6] * pow(2, scaling_bits));
    data = data & (((unsigned int)(-1)) >> ((8 * sizeof(int)) - effective_bits));
    tmp1 = 0;
    tmp2 = 0;
    tmp3 = 0;
    tmp1 = data & 0x00FF;
    tmp2 = (data >> 8) & 0x00FF;
    tmp3 = (data >> 16) & 0x00FF;
    APC_SetHWRegister(pHandleEtronDI, pDevSelInfo, 0xF514, tmp1, FG_Address_2Byte | FG_Value_1Byte);
    APC_SetHWRegister(pHandleEtronDI, pDevSelInfo, 0xF515, tmp2, FG_Address_2Byte | FG_Value_1Byte);
    // L_M21
    effective_bits = 10;
    scaling_bits = 21;
    data = int(iM1[7] * pow(2, scaling_bits));
    data = data & (((unsigned int)(-1)) >> ((8 * sizeof(int)) - effective_bits));
    tmp1 = 0;
    tmp2 = 0;
    tmp3 = 0;
    tmp1 = data & 0x00FF;
    tmp2 = (data >> 8) & 0x00FF;
    tmp3 = (data >> 16) & 0x00FF;
    APC_SetHWRegister(pHandleEtronDI, pDevSelInfo, 0xF516, tmp1, FG_Address_2Byte | FG_Value_1Byte);
    APC_SetHWRegister(pHandleEtronDI, pDevSelInfo, 0xF517, tmp2, FG_Address_2Byte | FG_Value_1Byte);
    // L_M22
    effective_bits = 19;
    scaling_bits = 17;
    data = int(iM1[8] * pow(2, scaling_bits));
    data = data & (((unsigned int)(-1)) >> ((8 * sizeof(int)) - effective_bits));
    tmp1 = 0;
    tmp2 = 0;
    tmp3 = 0;
    tmp1 = data & 0x00FF;
    tmp2 = (data >> 8) & 0x00FF;
    tmp3 = (data >> 16) & 0x00FF;
    APC_SetHWRegister(pHandleEtronDI, pDevSelInfo, 0xF518, tmp1, FG_Address_2Byte | FG_Value_1Byte);
    APC_SetHWRegister(pHandleEtronDI, pDevSelInfo, 0xF519, tmp2, FG_Address_2Byte | FG_Value_1Byte);
    APC_SetHWRegister(pHandleEtronDI, pDevSelInfo, 0xF51A, tmp3, FG_Address_2Byte | FG_Value_1Byte);
    // L_CX
    effective_bits = 16;
    scaling_bits = 2;
    data = int(rectifyData.CamMat1[2] * pow(2, scaling_bits));
    data = data & (((unsigned int)(-1)) >> ((8 * sizeof(int)) - effective_bits));
    tmp1 = 0;
    tmp2 = 0;
    tmp3 = 0;
    tmp1 = data & 0x00FF;
    tmp2 = (data >> 8) & 0x00FF;
    tmp3 = (data >> 16) & 0x00FF;
    APC_SetHWRegister(pHandleEtronDI, pDevSelInfo, 0xF550, tmp1, FG_Address_2Byte | FG_Value_1Byte);
    APC_SetHWRegister(pHandleEtronDI, pDevSelInfo, 0xF551, tmp2, FG_Address_2Byte | FG_Value_1Byte);
    // L_CY
    effective_bits = 16;
    scaling_bits = 2;
    data = int(rectifyData.CamMat1[5] * pow(2, scaling_bits));
    data = data & (((unsigned int)(-1)) >> ((8 * sizeof(int)) - effective_bits));
    tmp1 = 0;
    tmp2 = 0;
    tmp3 = 0;
    tmp1 = data & 0x00FF;
    tmp2 = (data >> 8) & 0x00FF;
    tmp3 = (data >> 16) & 0x00FF;
    APC_SetHWRegister(pHandleEtronDI, pDevSelInfo, 0xF552, tmp1, FG_Address_2Byte | FG_Value_1Byte);
    APC_SetHWRegister(pHandleEtronDI, pDevSelInfo, 0xF553, tmp2, FG_Address_2Byte | FG_Value_1Byte);
    // L_FX
    effective_bits = 16;
    scaling_bits = 3;
    data = int(rectifyData.CamMat1[0] * pow(2, scaling_bits) * fix_ratio1);
    data = data & (((unsigned int)(-1)) >> ((8 * sizeof(int)) - effective_bits));
    tmp1 = 0;
    tmp2 = 0;
    tmp3 = 0;
    tmp1 = data & 0x00FF;
    tmp2 = (data >> 8) & 0x00FF;
    tmp3 = (data >> 16) & 0x00FF;
    APC_SetHWRegister(pHandleEtronDI, pDevSelInfo, 0xF554, tmp1, FG_Address_2Byte | FG_Value_1Byte);
    APC_SetHWRegister(pHandleEtronDI, pDevSelInfo, 0xF555, tmp2, FG_Address_2Byte | FG_Value_1Byte);
    APC_SetHWRegister(pHandleEtronDI, pDevSelInfo, 0xF556, tmp3, FG_Address_2Byte | FG_Value_1Byte);
    // L_FY
    effective_bits = 16;
    scaling_bits = 3;
    data = int(rectifyData.CamMat1[4] * pow(2, scaling_bits) * fix_ratio1);
    data = data & (((unsigned int)(-1)) >> ((8 * sizeof(int)) - effective_bits));
    tmp1 = 0;
    tmp2 = 0;
    tmp3 = 0;
    tmp1 = data & 0x00FF;
    tmp2 = (data >> 8) & 0x00FF;
    tmp3 = (data >> 16) & 0x00FF;
    APC_SetHWRegister(pHandleEtronDI, pDevSelInfo, 0xF557, tmp1, FG_Address_2Byte | FG_Value_1Byte);
    APC_SetHWRegister(pHandleEtronDI, pDevSelInfo, 0xF558, tmp2, FG_Address_2Byte | FG_Value_1Byte);
    APC_SetHWRegister(pHandleEtronDI, pDevSelInfo, 0xF559, tmp3, FG_Address_2Byte | FG_Value_1Byte);

    // R_M00
    effective_bits = 16;
    scaling_bits = 21;
    data = int(iM2[0] * pow(2, scaling_bits) / fix_ratio2);
    data = data & (((unsigned int)(-1)) >> ((8 * sizeof(int)) - effective_bits));
    tmp1 = 0;
    tmp2 = 0;
    tmp3 = 0;
    tmp1 = data & 0x00FF;
    tmp2 = (data >> 8) & 0x00FF;
    tmp3 = (data >> 16) & 0x00FF;
    APC_SetHWRegister(pHandleEtronDI, pDevSelInfo, 0xF523, tmp1, FG_Address_2Byte | FG_Value_1Byte);
    APC_SetHWRegister(pHandleEtronDI, pDevSelInfo, 0xF524, tmp2, FG_Address_2Byte | FG_Value_1Byte);
    // R_M01
    effective_bits = 11;
    scaling_bits = 21;
    data = int(iM2[1] * pow(2, scaling_bits) / fix_ratio2);
    data = data & (((unsigned int)(-1)) >> ((8 * sizeof(int)) - effective_bits));
    tmp1 = 0;
    tmp2 = 0;
    tmp3 = 0;
    tmp1 = data & 0x00FF;
    tmp2 = (data >> 8) & 0x00FF;
    tmp3 = (data >> 16) & 0x00FF;
    APC_SetHWRegister(pHandleEtronDI, pDevSelInfo, 0xF525, tmp1, FG_Address_2Byte | FG_Value_1Byte);
    APC_SetHWRegister(pHandleEtronDI, pDevSelInfo, 0xF526, tmp2, FG_Address_2Byte | FG_Value_1Byte);
    // R_M02
    effective_bits = 19;
    scaling_bits = 17;
    data = int(iM2[2] * pow(2, scaling_bits) / fix_ratio2);
    data = data & (((unsigned int)(-1)) >> ((8 * sizeof(int)) - effective_bits));
    tmp1 = 0;
    tmp2 = 0;
    tmp3 = 0;
    tmp1 = data & 0x00FF;
    tmp2 = (data >> 8) & 0x00FF;
    tmp3 = (data >> 16) & 0x00FF;
    APC_SetHWRegister(pHandleEtronDI, pDevSelInfo, 0xF527, tmp1, FG_Address_2Byte | FG_Value_1Byte);
    APC_SetHWRegister(pHandleEtronDI, pDevSelInfo, 0xF528, tmp2, FG_Address_2Byte | FG_Value_1Byte);
    APC_SetHWRegister(pHandleEtronDI, pDevSelInfo, 0xF529, tmp3, FG_Address_2Byte | FG_Value_1Byte);
    // R_M10
    effective_bits = 11;
    scaling_bits = 21;
    data = int(iM2[3] * pow(2, scaling_bits) / fix_ratio2);
    data = data & (((unsigned int)(-1)) >> ((8 * sizeof(int)) - effective_bits));
    tmp1 = 0;
    tmp2 = 0;
    tmp3 = 0;
    tmp1 = data & 0x00FF;
    tmp2 = (data >> 8) & 0x00FF;
    tmp3 = (data >> 16) & 0x00FF;
    APC_SetHWRegister(pHandleEtronDI, pDevSelInfo, 0xF52A, tmp1, FG_Address_2Byte | FG_Value_1Byte);
    APC_SetHWRegister(pHandleEtronDI, pDevSelInfo, 0xF52B, tmp2, FG_Address_2Byte | FG_Value_1Byte);
    // R_M11
    effective_bits = 16;
    scaling_bits = 21;
    data = int(iM2[4] * pow(2, scaling_bits) / fix_ratio2);
    data = data & (((unsigned int)(-1)) >> ((8 * sizeof(int)) - effective_bits));
    tmp1 = 0;
    tmp2 = 0;
    tmp3 = 0;
    tmp1 = data & 0x00FF;
    tmp2 = (data >> 8) & 0x00FF;
    tmp3 = (data >> 16) & 0x00FF;
    APC_SetHWRegister(pHandleEtronDI, pDevSelInfo, 0xF52C, tmp1, FG_Address_2Byte | FG_Value_1Byte);
    APC_SetHWRegister(pHandleEtronDI, pDevSelInfo, 0xF52D, tmp2, FG_Address_2Byte | FG_Value_1Byte);
    // R_M12
    effective_bits = 19;
    scaling_bits = 17;
    data = int(iM2[5] * pow(2, scaling_bits) / fix_ratio2);
    data = data & (((unsigned int)(-1)) >> ((8 * sizeof(int)) - effective_bits));
    tmp1 = 0;
    tmp2 = 0;
    tmp3 = 0;
    tmp1 = data & 0x00FF;
    tmp2 = (data >> 8) & 0x00FF;
    tmp3 = (data >> 16) & 0x00FF;
    APC_SetHWRegister(pHandleEtronDI, pDevSelInfo, 0xF52E, tmp1, FG_Address_2Byte | FG_Value_1Byte);
    APC_SetHWRegister(pHandleEtronDI, pDevSelInfo, 0xF52F, tmp2, FG_Address_2Byte | FG_Value_1Byte);
    APC_SetHWRegister(pHandleEtronDI, pDevSelInfo, 0xF530, tmp3, FG_Address_2Byte | FG_Value_1Byte);
    // R_M20
    effective_bits = 12;
    scaling_bits = 21;
    data = int(iM2[6] * pow(2, scaling_bits));
    data = data & (((unsigned int)(-1)) >> ((8 * sizeof(int)) - effective_bits));
    tmp1 = 0;
    tmp2 = 0;
    tmp3 = 0;
    tmp1 = data & 0x00FF;
    tmp2 = (data >> 8) & 0x00FF;
    tmp3 = (data >> 16) & 0x00FF;
    APC_SetHWRegister(pHandleEtronDI, pDevSelInfo, 0xF531, tmp1, FG_Address_2Byte | FG_Value_1Byte);
    APC_SetHWRegister(pHandleEtronDI, pDevSelInfo, 0xF532, tmp2, FG_Address_2Byte | FG_Value_1Byte);
    // R_M21
    effective_bits = 10;
    scaling_bits = 21;
    data = int(iM2[7] * pow(2, scaling_bits));
    data = data & (((unsigned int)(-1)) >> ((8 * sizeof(int)) - effective_bits));
    tmp1 = 0;
    tmp2 = 0;
    tmp3 = 0;
    tmp1 = data & 0x00FF;
    tmp2 = (data >> 8) & 0x00FF;
    tmp3 = (data >> 16) & 0x00FF;
    APC_SetHWRegister(pHandleEtronDI, pDevSelInfo, 0xF533, tmp1, FG_Address_2Byte | FG_Value_1Byte);
    APC_SetHWRegister(pHandleEtronDI, pDevSelInfo, 0xF534, tmp2, FG_Address_2Byte | FG_Value_1Byte);
    // R_M22
    effective_bits = 19;
    scaling_bits = 17;
    data = int(iM2[8] * pow(2, scaling_bits));
    data = data & (((unsigned int)(-1)) >> ((8 * sizeof(int)) - effective_bits));
    tmp1 = 0;
    tmp2 = 0;
    tmp3 = 0;
    tmp1 = data & 0x00FF;
    tmp2 = (data >> 8) & 0x00FF;
    tmp3 = (data >> 16) & 0x00FF;
    APC_SetHWRegister(pHandleEtronDI, pDevSelInfo, 0xF535, tmp1, FG_Address_2Byte | FG_Value_1Byte);
    APC_SetHWRegister(pHandleEtronDI, pDevSelInfo, 0xF536, tmp2, FG_Address_2Byte | FG_Value_1Byte);
    APC_SetHWRegister(pHandleEtronDI, pDevSelInfo, 0xF537, tmp3, FG_Address_2Byte | FG_Value_1Byte);
    // R_CX
    effective_bits = 16;
    scaling_bits = 2;
    data = int(rectifyData.CamMat2[2] * pow(2, scaling_bits));
    data = data & (((unsigned int)(-1)) >> ((8 * sizeof(int)) - effective_bits));
    tmp1 = 0;
    tmp2 = 0;
    tmp3 = 0;
    tmp1 = data & 0x00FF;
    tmp2 = (data >> 8) & 0x00FF;
    tmp3 = (data >> 16) & 0x00FF;
    APC_SetHWRegister(pHandleEtronDI, pDevSelInfo, 0xF568, tmp1, FG_Address_2Byte | FG_Value_1Byte);
    APC_SetHWRegister(pHandleEtronDI, pDevSelInfo, 0xF569, tmp2, FG_Address_2Byte | FG_Value_1Byte);
    // R_CY
    effective_bits = 16;
    scaling_bits = 2;
    data = int(rectifyData.CamMat2[5] * pow(2, scaling_bits));
    data = data & (((unsigned int)(-1)) >> ((8 * sizeof(int)) - effective_bits));
    tmp1 = 0;
    tmp2 = 0;
    tmp3 = 0;
    tmp1 = data & 0x00FF;
    tmp2 = (data >> 8) & 0x00FF;
    tmp3 = (data >> 16) & 0x00FF;
    APC_SetHWRegister(pHandleEtronDI, pDevSelInfo, 0xF56A, tmp1, FG_Address_2Byte | FG_Value_1Byte);
    APC_SetHWRegister(pHandleEtronDI, pDevSelInfo, 0xF56B, tmp2, FG_Address_2Byte | FG_Value_1Byte);
    // R_FX
    effective_bits = 16;
    scaling_bits = 3;
    data = int(rectifyData.CamMat2[0] * pow(2, scaling_bits) * fix_ratio2);
    data = data & (((unsigned int)(-1)) >> ((8 * sizeof(int)) - effective_bits));
    tmp1 = 0;
    tmp2 = 0;
    tmp3 = 0;
    tmp1 = data & 0x00FF;
    tmp2 = (data >> 8) & 0x00FF;
    tmp3 = (data >> 16) & 0x00FF;
    APC_SetHWRegister(pHandleEtronDI, pDevSelInfo, 0xF56C, tmp1, FG_Address_2Byte | FG_Value_1Byte);
    APC_SetHWRegister(pHandleEtronDI, pDevSelInfo, 0xF56D, tmp2, FG_Address_2Byte | FG_Value_1Byte);
    APC_SetHWRegister(pHandleEtronDI, pDevSelInfo, 0xF56E, tmp3, FG_Address_2Byte | FG_Value_1Byte);
    // R_FY
    effective_bits = 16;
    scaling_bits = 3;
    data = int(rectifyData.CamMat2[4] * pow(2, scaling_bits) * fix_ratio2);
    data = data & (((unsigned int)(-1)) >> ((8 * sizeof(int)) - effective_bits));
    tmp1 = 0;
    tmp2 = 0;
    tmp3 = 0;
    tmp1 = data & 0x00FF;
    tmp2 = (data >> 8) & 0x00FF;
    tmp3 = (data >> 16) & 0x00FF;
    APC_SetHWRegister(pHandleEtronDI, pDevSelInfo, 0xF56F, tmp1, FG_Address_2Byte | FG_Value_1Byte);
    APC_SetHWRegister(pHandleEtronDI, pDevSelInfo, 0xF570, tmp2, FG_Address_2Byte | FG_Value_1Byte);
    APC_SetHWRegister(pHandleEtronDI, pDevSelInfo, 0xF571, tmp3, FG_Address_2Byte | FG_Value_1Byte);

    return 0;
}

int WriteRectification(eSPCtrl_RectLogData &rectifyData, void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, int index)
{
    // Read buffer from RectifyTable
    int nothin;
    unsigned char buffer[1024];
    memset(buffer, 0, 1024);
    int net = APC_GetRectifyTable(pHandleEtronDI, pDevSelInfo, buffer, 1024, &nothin, index);

    if (APC_OK == net)
    {
        cout << "Get RectifyTable success" << endl;
    }
    else
    {
        cout << "Get RectifyTable fail" << endl;
        return net;
    }

    // calculate matrix
    double NP1[9] = {0}, NP2[9] = {0}, R1[9] = {0}, R2[9] = {0}, M1[9] = {0}, M2[9] = {0}, iM1[9] = {0}, iM2[9] = {0};
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            NP1[3 * i + j] = rectifyData.NewCamMat1[4 * i + j];
            NP2[3 * i + j] = rectifyData.NewCamMat2[4 * i + j];
            R1[3 * i + j] = rectifyData.LRotaMat[3 * i + j];
            R2[3 * i + j] = rectifyData.RRotaMat[3 * i + j];
        }
    }
    Mat_Product(NP1, R1, M1);
    Mat_Product(NP2, R2, M2);
    Mat_Inverse(M1, iM1);
    Mat_Inverse(M2, iM2);

    double fix_ratio1 = rectifyData.ParameterRatio[0];
    double fix_ratio2 = rectifyData.ParameterRatio[1];
    if (isnan(fix_ratio1))
    {
        fix_ratio1 = 1;
    }
    if (isnan(fix_ratio2))
    {
        fix_ratio2 = 1;
    }

    if (fix_ratio1 == 0)
    {
        fix_ratio1 = 1;
    }
    if (fix_ratio2 == 0)
    {
        fix_ratio2 = fix_ratio1;
    }

    // wrtie values to buffer
    // L_M
    writeBuffer({22, 25}, buffer, 16, int(iM1[0] * pow(2, 21) / fix_ratio1));
    writeBuffer({28, 31}, buffer, 11, int(iM1[1] * pow(2, 21) / fix_ratio1));
    writeBuffer({34, 37, 40}, buffer, 19, int(iM1[2] * pow(2, 17) / fix_ratio1));
    writeBuffer({43, 46}, buffer, 11, int(iM1[3] * pow(2, 21) / fix_ratio1));
    writeBuffer({49, 52}, buffer, 16, int(iM1[4] * pow(2, 21) / fix_ratio1));
    writeBuffer({55, 58, 61}, buffer, 19, int(iM1[5] * pow(2, 17) / fix_ratio1));
    writeBuffer({64, 67}, buffer, 12, int(iM1[6] * pow(2, 21)));
    writeBuffer({70, 73}, buffer, 10, int(iM1[7] * pow(2, 21)));
    writeBuffer({76, 79, 82}, buffer, 19, int(iM1[8] * pow(2, 17)));
    // L_FX
    writeBuffer({163, 166, 169}, buffer, 18, int(rectifyData.CamMat1[0] * pow(2, 3) * fix_ratio1));
    // L_FY
    writeBuffer({172, 175, 178}, buffer, 18, int(rectifyData.CamMat1[4] * pow(2, 3) * fix_ratio1));
    // L_CX
    writeBuffer({151, 154}, buffer, 16, int(rectifyData.CamMat1[2] * pow(2, 2)));
    // L_CY
    writeBuffer({157, 160}, buffer, 16, int(rectifyData.CamMat1[5] * pow(2, 2)));

    // R_M
    writeBuffer({181, 184}, buffer, 16, int(iM2[0] * pow(2, 21) / fix_ratio2));
    writeBuffer({187, 190}, buffer, 11, int(iM2[1] * pow(2, 21) / fix_ratio2));
    writeBuffer({193, 196, 199}, buffer, 19, int(iM2[2] * pow(2, 17) / fix_ratio2));
    writeBuffer({202, 205}, buffer, 11, int(iM2[3] * pow(2, 21) / fix_ratio2));
    writeBuffer({208, 211}, buffer, 16, int(iM2[4] * pow(2, 21) / fix_ratio2));
    writeBuffer({214, 217, 220}, buffer, 19, int(iM2[5] * pow(2, 17) / fix_ratio2));
    writeBuffer({223, 226}, buffer, 12, int(iM2[6] * pow(2, 21)));
    writeBuffer({229, 232}, buffer, 10, int(iM2[7] * pow(2, 21)));
    writeBuffer({235, 238, 241}, buffer, 19, int(iM2[8] * pow(2, 17)));
    // R_FX
    writeBuffer({322, 325, 328}, buffer, 18, int(rectifyData.CamMat2[0] * pow(2, 3) * fix_ratio2));
    // R_FY
    writeBuffer({331, 334, 337}, buffer, 18, int(rectifyData.CamMat2[4] * pow(2, 3) * fix_ratio2));
    // R_CX
    writeBuffer({310, 313}, buffer, 16, int(rectifyData.CamMat2[2] * pow(2, 2)));
    // R_CY
    writeBuffer({316, 319}, buffer, 16, int(rectifyData.CamMat2[5] * pow(2, 2)));

    // Write buffer into RectifyTable

    net = APC_SetRectifyTable(pHandleEtronDI, pDevSelInfo, buffer, 1024, &nothin, index);
    if (APC_OK == net)
    {
        cout << "Set RectifyTable success" << endl;
        return net;
    }
    else
    {
        cout << "Set RectifyTable fail" << endl;
        return net;
    }

    return 0;
}

int WriteRectifyLogData(eSPCtrl_RectLogData &rectifyData, void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, int index)
{
    int nothin;
    unsigned char buffer[4096];
    memset(buffer, 0, 4096);
    int net = APC_GetLogData(pHandleEtronDI, pDevSelInfo, buffer, 4096, &nothin, index);
    if (APC_OK == net)
    {
        cout << "Get RectifyLog Data success" << endl;
    }
    else
    {
        cout << "Get RectifyLog Data fail" << endl;
        return net;
    }

    int log_size = readBuffer({0, 1}, buffer, 16);

    int num_of_ID = readBuffer({22, 23}, buffer, 16);

    vector<int> TAB_Offset, TAB_ElementCount, TAB_DataSize, TAB_FractionBit;

    int tmp = 0;
    for (int i = 0; i < num_of_ID; i++)
    {
        tmp = readBuffer(50 + 16 * i, 2, buffer, 16) + 2;
        TAB_Offset.push_back(tmp);
        // cout << i<<" Data Offset is "<<tmp<<endl;

        tmp = readBuffer(50 + 16 * i + 2, 2, buffer, 16);
        TAB_ElementCount.push_back(tmp);
        // cout << i<<" Element Count is "<<tmp<<endl;

        tmp = readBuffer(50 + 16 * i + 4, 1, buffer, 8);
        TAB_DataSize.push_back(tmp);
        // cout << i<<" Data Size is "<<tmp<<endl;
        tmp = readBuffer(50 + 16 * i + 5, 1, buffer, 8);
        // cout << i<<" Tab ID is "<<tmp<<endl;
        tmp = readBuffer(50 + 16 * i + 7, 1, buffer, 8);
        // cout << i<<" Fractional Bit is "<<tmp<<endl;

        TAB_FractionBit.push_back(tmp);
    }
    int idx = 0;

    // Cam Mat1 => 4
    idx = 4;
    for (int i = 0; i < TAB_ElementCount[idx]; i++)
    {
        double tmp_d = rectifyData.CamMat1[i];
        int tmp_i = int(tmp_d * pow(2, TAB_FractionBit[idx]));
        writeBuffer(TAB_Offset[idx] + i * TAB_DataSize[idx], TAB_DataSize[idx], buffer, 8 * TAB_DataSize[idx] - TAB_FractionBit[idx] - 1, tmp_i);
    }
    // Cam Mat2 => 6
    idx = 6;
    for (int i = 0; i < TAB_ElementCount[idx]; i++)
    {
        double tmp_d = rectifyData.CamMat2[i];
        int tmp_i = int(tmp_d * pow(2, TAB_FractionBit[idx]));
        writeBuffer(TAB_Offset[idx] + i * TAB_DataSize[idx], TAB_DataSize[idx], buffer, 8 * TAB_DataSize[idx] - TAB_FractionBit[idx] - 1, tmp_i);
    }
    // Rota Mat => 8
    idx = 8;
    for (int i = 0; i < TAB_ElementCount[idx]; i++)
    {
        double tmp_d = rectifyData.RotaMat[i];
        int tmp_i = int(tmp_d * pow(2, TAB_FractionBit[idx]));
        writeBuffer(TAB_Offset[idx] + i * TAB_DataSize[idx], TAB_DataSize[idx], buffer, 8 * TAB_DataSize[idx] - TAB_FractionBit[idx] - 1, tmp_i);
    }
    // Tran Mat => 9
    idx = 9;
    for (int i = 0; i < TAB_ElementCount[idx]; i++)
    {
        double tmp_d = rectifyData.TranMat[i];
        int tmp_i = int(tmp_d * pow(2, TAB_FractionBit[idx]));
        writeBuffer(TAB_Offset[idx] + i * TAB_DataSize[idx], TAB_DataSize[idx], buffer, 8 * TAB_DataSize[idx] - TAB_FractionBit[idx] - 1, tmp_i);
    }
    // L Rota Mat => 10
    idx = 10;
    for (int i = 0; i < TAB_ElementCount[idx]; i++)
    {
        double tmp_d = rectifyData.LRotaMat[i];
        int tmp_i = int(tmp_d * pow(2, TAB_FractionBit[idx]));
        writeBuffer(TAB_Offset[idx] + i * TAB_DataSize[idx], TAB_DataSize[idx], buffer, 8 * TAB_DataSize[idx] - TAB_FractionBit[idx] - 1, tmp_i);
    }
    // R Rota Mat => 11
    idx = 11;
    for (int i = 0; i < TAB_ElementCount[idx]; i++)
    {
        double tmp_d = rectifyData.RRotaMat[i];
        int tmp_i = int(tmp_d * pow(2, TAB_FractionBit[idx]));
        writeBuffer(TAB_Offset[idx] + i * TAB_DataSize[idx], TAB_DataSize[idx], buffer, 8 * TAB_DataSize[idx] - TAB_FractionBit[idx] - 1, tmp_i);
    }
    // New Cam Mat1 => 12
    idx = 12;
    for (int i = 0; i < TAB_ElementCount[idx]; i++)
    {
        double tmp_d = rectifyData.NewCamMat1[i];
        int tmp_i = int(tmp_d * pow(2, TAB_FractionBit[idx]));
        writeBuffer(TAB_Offset[idx] + i * TAB_DataSize[idx], TAB_DataSize[idx], buffer, 8 * TAB_DataSize[idx] - TAB_FractionBit[idx] - 1, tmp_i);
    }
    // New Cam Mat2 => 13
    idx = 13;
    for (int i = 0; i < TAB_ElementCount[idx]; i++)
    {
        double tmp_d = rectifyData.NewCamMat2[i];
        int tmp_i = int(tmp_d * pow(2, TAB_FractionBit[idx]));
        writeBuffer(TAB_Offset[idx] + i * TAB_DataSize[idx], TAB_DataSize[idx], buffer, 8 * TAB_DataSize[idx] - TAB_FractionBit[idx] - 1, tmp_i);
    }

    net = APC_SetLogData(pHandleEtronDI, pDevSelInfo, buffer, 4096, &nothin, 5);
    if (APC_OK == net)
    {
        cout << "Set RectifyLog Data success" << endl;
        return net;
    }
    else
    {
        cout << "Set RectifyLog Data fail" << endl;
        return net;
    }

    return 0;
}

void Mat_Product(double *A, double *B, double *C)
{
    for (int i = 0; i < 9; i++)
    {
        C[i] = 0;
    }
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            for (int k = 0; k < 3; k++)
            {
                C[3 * i + j] += A[3 * i + k] * B[3 * k + j];
            }
        }
    }
}

void Mat_Inverse(double *A, double *B)
{
    double det = 0;
    det += A[0] * A[4] * A[8];
    det += A[1] * A[5] * A[6];
    det += A[2] * A[3] * A[7];
    det -= A[2] * A[4] * A[6];
    det -= A[1] * A[3] * A[8];
    det -= A[0] * A[5] * A[7];
    B[0] = (A[4] * A[8] - A[5] * A[7]) / det;
    B[1] = (A[2] * A[7] - A[1] * A[8]) / det;
    B[2] = (A[1] * A[5] - A[2] * A[4]) / det;
    B[3] = (A[5] * A[6] - A[3] * A[8]) / det;
    B[4] = (A[0] * A[8] - A[2] * A[6]) / det;
    B[5] = (A[2] * A[3] - A[0] * A[5]) / det;
    B[6] = (A[3] * A[7] - A[4] * A[6]) / det;
    B[7] = (A[1] * A[6] - A[0] * A[7]) / det;
    B[8] = (A[0] * A[4] - A[1] * A[3]) / det;
}

void writeBuffer(vector<int> nums, unsigned char *buffer, int bits, int input)
{
    int l = int(nums.size());
    int tmp = input;
    if (input < 0)
    {
        tmp = tmp + int(pow(2, bits));
    }
    int tmpp = 0;
    for (int i = l; i > 0; i--)
    {

        while (tmp >= pow(256, i - 1))
        {
            tmpp++;
            tmp = tmp - int(pow(256, i - 1));
        }
        buffer[nums[i - 1]] = (unsigned char)tmpp;
        tmpp = 0;
    }
}

void writeBuffer(int start, int nums, unsigned char *buffer, int bits, int input)
{
    int tmp = input;
    if (input < 0)
    {
        tmp = input + int(pow(2, bits));
    }
    int tmpp = 0;
    for (int i = nums; i > 0; i--)
    {
        while (tmp >= pow(256, i - 1))
        {
            tmpp++;
            tmp = tmp - int(pow(256, i - 1));
            buffer[start + i - 1] = (unsigned char)tmpp;
        }
        tmpp = 0;
    }
}

int readBuffer(vector<int> nums, unsigned char *buffer, int bits)
{
    int l = int(nums.size());
    int out = 0;
    for (int i = 0; i < l; i++)
    {
        out = out + (int)(buffer[nums[i]]) * int(pow(256, i));
    }
    if (out > pow(2, (bits - 1)))
    {
        out = out - int(pow(2, bits));
    }

    return out;
}

int readBuffer(int start, int nums, unsigned char *buffer, int bits)
{
    int out = 0;
    for (int i = 0; i < nums; i++)
    {
        // cout << out <<endl;
        out = out + (int)(buffer[start + i]) * int(pow(256, i));
    }
    if (out > pow(2, (bits - 1)))
    {
        out = out - int(pow(2, bits));
    }

    return out;
}
