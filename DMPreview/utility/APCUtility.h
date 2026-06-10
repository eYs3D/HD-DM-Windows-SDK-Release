#pragma once
#ifdef ESPDI_EG
#include "eSPDI_EG.h"
#else
#include "eSPDI_DM.h"
#endif

class CApcDIWrap
{
public:
    CApcDIWrap();
    ~CApcDIWrap();

    void* hApcDI;

private:
    void Release();
};

bool GetApcDIDevice(std::vector<DEVINFORMATIONEX>& device);
