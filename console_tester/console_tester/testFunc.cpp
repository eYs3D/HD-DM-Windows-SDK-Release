#include "main.h"

void Read3X()
{
    void* pHandleApcDI;
    int ret_init = APC_Init(&pHandleApcDI, false);

    DEVSELINFO devSelInfo;
    devSelInfo.index = 0;

    int index;
    int nbfferLength = APC_Y_OFFSET_FILE_SIZE;
    int pActualLength = 0;
    BYTE *data = new BYTE[nbfferLength];
    memset(data, 0x0, nbfferLength);
    for (index = 0; index <= 9; index++)
    {
        if (APC_OK == APC_GetYOffset(pHandleApcDI, &devSelInfo, data, nbfferLength, &pActualLength, index))
        {
            printf("\n Read3%d \n", index);
            for (int i = 0; i < nbfferLength; i++)
            {
                printf("%02x ", data[i]);
            }
            printf("\n");
        }
        else
        {
            printf("\n Read3%d Failed\n", index);
        }
    }

    delete[] data;
    APC_CloseDevice(pHandleApcDI, &devSelInfo);
    APC_Release(&pHandleApcDI);
}

void Write3X()
{
    void* pHandleApcDI;
    int ret_init = APC_Init(&pHandleApcDI, false);

    DEVSELINFO devSelInfo;
    devSelInfo.index = 0;

    int index;
    int nbfferLength = APC_Y_OFFSET_FILE_SIZE;
    int pActualLength = 0;
    BYTE *data = new BYTE[nbfferLength];
    memset(data, 0x0, nbfferLength);
    for (index = 0; index <= 9; index++)
    {
        if (APC_OK == APC_GetYOffset(pHandleApcDI, &devSelInfo, data, nbfferLength, &pActualLength, index))
        {
            if (APC_OK == APC_SetYOffset(pHandleApcDI, &devSelInfo, data, nbfferLength, &pActualLength, index))
            {
                printf("Write3%d Success \n", index);
            }
            else
            {
                printf("Write3%d Failed \n", index);
            }
        }
    }

    delete[] data;
    APC_CloseDevice(pHandleApcDI, &devSelInfo);
    APC_Release(&pHandleApcDI);
}

void Read4X()
{
    void* pHandleApcDI;
    int ret_init = APC_Init(&pHandleApcDI, false);

    DEVSELINFO devSelInfo;
    devSelInfo.index = 0;

    int index;
    int nbfferLength = APC_RECTIFY_FILE_SIZE;
    int pActualLength = 0;
    BYTE *data = new BYTE[nbfferLength];
    memset(data, 0x0, nbfferLength);
    for (index = 0; index <= 9; index++)
    {
        if (APC_OK == APC_GetRectifyTable(pHandleApcDI, &devSelInfo, data, nbfferLength, &pActualLength, index))
        {
            printf("\n Read4%d \n", index);
            for (int i = 0; i < nbfferLength; i++)
            {
                printf("%02x ", data[i]);
            }
            printf("\n");
        }
        else
        {
            printf("\n Read4%d Failed\n", index);
        }
    }
    delete[] data;
    APC_CloseDevice(pHandleApcDI, &devSelInfo);
    APC_Release(&pHandleApcDI);
}

void Write4X()
{
    void* pHandleApcDI;
    int ret_init = APC_Init(&pHandleApcDI, false);

    DEVSELINFO devSelInfo;
    devSelInfo.index = 0;

    int index;
    int nbfferLength = APC_RECTIFY_FILE_SIZE;
    int pActualLength = 0;
    BYTE *data = new BYTE[nbfferLength];
    memset(data, 0x0, nbfferLength);
    for (index = 0; index <= 9; index++)
    {
        if (APC_OK == APC_GetRectifyTable(pHandleApcDI, &devSelInfo, data, nbfferLength, &pActualLength, index))
        {
            if (APC_OK == APC_SetRectifyTable(pHandleApcDI, &devSelInfo, data, nbfferLength, &pActualLength, index))
            {
                printf("Write4%d Success \n", index);
            }
            else
            {
                printf("Write4%d Failed \n", index);
            }
        }
    }
    delete[] data;
    APC_CloseDevice(pHandleApcDI, &devSelInfo);
    APC_Release(&pHandleApcDI);
}

void Read5X()
{
    void* pHandleApcDI;
    int ret_init = APC_Init(&pHandleApcDI, false);

    DEVSELINFO devSelInfo;
    devSelInfo.index = 0;

    int index;
    int nbfferLength = APC_ZD_TABLE_FILE_SIZE;
    int pActualLength = 0;

    ZDTABLEINFO zdTableInfo;
    zdTableInfo.nDataType = APC_DEPTH_DATA_11_BITS;

    BYTE *data = new BYTE[nbfferLength];
    memset(data, 0x0, nbfferLength);
    for (index = 0; index <= 9; index++)
    {
        zdTableInfo.nIndex = index;
        if (APC_OK == APC_GetZDTable(pHandleApcDI, &devSelInfo, data, nbfferLength, &pActualLength, &zdTableInfo))
        {
            printf("\n Read5%d \n", index);
            for (int i = 0; i < nbfferLength; i++)
            {
                printf("%02x ", data[i]);
            }
            printf("\n");
        }
        else
        {
            printf("\n Read5%d Failed\n", index);
        }
    }
    delete[] data;
    APC_CloseDevice(pHandleApcDI, &devSelInfo);
    APC_Release(&pHandleApcDI);
}

void Write5X()
{
    void* pHandleApcDI;
    int ret_init = APC_Init(&pHandleApcDI, false);

    DEVSELINFO devSelInfo;
    devSelInfo.index = 0;

    int index;
    int nbfferLength = APC_ZD_TABLE_FILE_SIZE;
    int pActualLength = 0;

    ZDTABLEINFO zdTableInfo;
    zdTableInfo.nDataType = APC_DEPTH_DATA_11_BITS;

    BYTE *data = new BYTE[nbfferLength];
    memset(data, 0x0, nbfferLength);

    for (index = 0; index <= 9; index++)
    {
        zdTableInfo.nIndex = index;
        if (APC_OK == APC_GetZDTable(pHandleApcDI, &devSelInfo, data, nbfferLength, &pActualLength, &zdTableInfo))
        {
            if (APC_OK == APC_SetZDTable(pHandleApcDI, &devSelInfo, data, nbfferLength, &pActualLength, &zdTableInfo))
            {
                printf("Write5%d Success \n", index);
            }
            else
            {
                printf("Write5%d Failed \n", index);
            }
        }
    }
    delete[] data;
    APC_CloseDevice(pHandleApcDI, &devSelInfo);
    APC_Release(&pHandleApcDI);
}

void Read24X()
{
    void* pHandleApcDI;
    int ret_init = APC_Init(&pHandleApcDI, false);

    DEVSELINFO devSelInfo;
    devSelInfo.index = 0;

    int index;
    int nbfferLength = APC_CALIB_LOG_FILE_SIZE;
    int pActualLength = 0;

    BYTE *data = new BYTE[nbfferLength];
    memset(data, 0x0, nbfferLength);

    for (index = 0; index <= 9; index++)
    {
        if (APC_OK == APC_GetLogData(pHandleApcDI, &devSelInfo, data, nbfferLength, &pActualLength, index))
        {
            printf("\n Read24%d ALL_LOG \n", index);
            for (int i = 0; i < nbfferLength; i++)
            {
                printf("%02x ", data[i]);
            }
            printf("\n");
        }
        else
        {
            printf("\n Read24%d ALL_LOG Failed\n", index);
        }
    }
    delete[] data;
    APC_CloseDevice(pHandleApcDI, &devSelInfo);
    APC_Release(&pHandleApcDI);
}

void Write24X()
{
    void* pHandleApcDI;
    int ret_init = APC_Init(&pHandleApcDI, false);

    DEVSELINFO devSelInfo;
    devSelInfo.index = 0;

    int index;
    int nbfferLength = APC_CALIB_LOG_FILE_SIZE;
    int pActualLength = 0;

    BYTE *data = new BYTE[nbfferLength];
    memset(data, 0x0, nbfferLength);

    for (index = 0; index <= 9; index++)
    {
        if (APC_OK == APC_GetLogData(pHandleApcDI, &devSelInfo, data, nbfferLength, &pActualLength, index))
        {
            if (APC_OK == APC_SetLogData(pHandleApcDI, &devSelInfo, data, nbfferLength, &pActualLength, index))
            {
                printf("Write24%d Success \n", index);
            }
            else
            {
                printf("Write24%d Failed \n", index);
            }
        }
    }
    delete[] data;
    APC_CloseDevice(pHandleApcDI, &devSelInfo);
    APC_Release(&pHandleApcDI);
}

void ResetUNPData()
{
    void* pHandleApcDI;
    int ret_init = APC_Init(&pHandleApcDI, false);

    DEVSELINFO devSelInfo;
    devSelInfo.index = 0;

    int isReset = APC_ResetUNPData(pHandleApcDI, &devSelInfo);
    if (isReset != APC_OK)
    {
        printf("APC_ResetUNPData fail.\n");
    }
    APC_CloseDevice(pHandleApcDI, &devSelInfo);
    APC_Release(&pHandleApcDI);
}

