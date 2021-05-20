#include "stdafx.h"
#include "EtronDIUtility.h"


CEtronDIWrap::CEtronDIWrap() : hEtronDI(NULL)
{
    if (EtronDI_Init(&hEtronDI, false) < 0)
    {
        Release();
    }
}

CEtronDIWrap::~CEtronDIWrap()
{
    Release();
}

void CEtronDIWrap::Release()
{
    if (hEtronDI != NULL)
    {
        EtronDI_Release(&hEtronDI);
        hEtronDI = NULL;
    }
}


bool GetEtronDIDevice(std::vector<DEVINFORMATIONEX>& device)
{
    CEtronDIWrap etronDI;
    if (etronDI.hEtronDI != NULL)
    {
        int deviceCount = EtronDI_GetDeviceNumber(etronDI.hEtronDI);
        if (deviceCount > 0)
        {
            device.clear();
            for (int i = 0; i < deviceCount; ++i)
            {
                DEVSELINFO devSelInfo;
                devSelInfo.index = i;
                DEVINFORMATIONEX deviceInfo;
                EtronDI_GetDeviceInfoEx(etronDI.hEtronDI, &devSelInfo, &deviceInfo);
                
                device.push_back(deviceInfo);
            }

            return true;
        }
    }

    return false;
}