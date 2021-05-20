#pragma once
#ifdef ESPDI_EG
#include "eSPDI_EG.h"
#else
#include "eSPDI_DM.h"
#endif

class CEtronDIWrap
{
public:
    CEtronDIWrap();
    ~CEtronDIWrap();

    void* hEtronDI;

private:
    void Release();
};

bool GetEtronDIDevice(std::vector<DEVINFORMATIONEX>& device);