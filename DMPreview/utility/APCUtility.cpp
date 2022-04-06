#include "stdafx.h"
#include "APCUtility.h"


CApcDIWrap::CApcDIWrap() : hApcDI(NULL)
{
    if (APC_Init(&hApcDI, false) < 0)
    {
        Release();
    }
}

CApcDIWrap::~CApcDIWrap()
{
    Release();
}

void CApcDIWrap::Release()
{
    if (hApcDI != NULL)
    {
        APC_Release(&hApcDI);
        hApcDI = NULL;
    }
}


bool GetApcDIDevice(std::vector<DEVINFORMATIONEX>& device)
{
    CApcDIWrap ApcDI;
    if (ApcDI.hApcDI != NULL)
    {
        int deviceCount = APC_GetDeviceNumber(ApcDI.hApcDI);
        if (deviceCount > 0)
        {
            device.clear();
            for (int i = 0; i < deviceCount; ++i)
            {
                DEVSELINFO devSelInfo;
                devSelInfo.index = i;
                DEVINFORMATIONEX deviceInfo;
                APC_GetDeviceInfoEx(ApcDI.hApcDI, &devSelInfo, &deviceInfo);
                
                device.push_back(deviceInfo);
            }

            return true;
        }
    }

    return false;
}