// RegisterAccessDlg.cpp : implementation file
//

#include "stdafx.h"
#include "EtronDI_Test.h"
#include "RegisterAccessDlg.h"
#include "mmsystem.h"

__pragma(warning(disable : 4996)) 

#define ACTUAL_PERIODIC_TIME_TIMERID 1
const size_t maxRecentUpdateTimeQueueSize = 101;

#define AXIS_3_GYRO 1
#define AXIS_6_GYRO 2

// CRegisterAccessDlg dialog

IMPLEMENT_DYNAMIC(CRegisterAccessDlg, CDialog)

CRegisterAccessDlg::CRegisterAccessDlg(void*& hEtronDI, DEVSELINFO& devSelInfo, CWnd* pParent /*=NULL*/)
	: CDialog(IDD_DIALOG_REGISTER_ACCESS, pParent), 
    m_hEtronDI(hEtronDI), m_devSelInfo(devSelInfo), m_hTimerQueue(nullptr), m_hTimer(nullptr)
{

}

CRegisterAccessDlg::~CRegisterAccessDlg()
{
}

void CRegisterAccessDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BOOL CRegisterAccessDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    ((CButton*)GetDlgItem(IDC_RADIO_I2C))->SetCheck(BST_CHECKED);
    ((CButton*)GetDlgItem(IDC_RADIO_SENSOR1))->SetCheck(BST_CHECKED);
    SetDlgItemText(IDC_STATIC_ACTUAL_PERIOD, L"");

    return TRUE;
}

// in dialog with WS_CHILD style, press Enter/Esc will call OnOK/OnCancel, 
// this dialog will be closed and disappear on parent's UI.
// to prevent this, instead of calling base class's OnOK/OnCancel, call parent's 
// OnOK/OnCancel.
void CRegisterAccessDlg::OnOK()
{
    GetParent()->PostMessage(WM_COMMAND, IDOK);
}

void CRegisterAccessDlg::OnCancel()
{
    GetParent()->PostMessage(WM_COMMAND, IDCANCEL);
}


BEGIN_MESSAGE_MAP(CRegisterAccessDlg, CDialog)
    ON_BN_CLICKED(IDC_CHECK_PERIODIC_READ, &CRegisterAccessDlg::OnBnClickedCheckPeriodicRead)
    ON_BN_CLICKED(IDC_REG_READ_BTN, &CRegisterAccessDlg::OnBnClickedRegReadBtn)
    ON_BN_CLICKED(IDC_REG_WRITE_BTN, &CRegisterAccessDlg::OnBnClickedRegWriteBtn)
    ON_WM_DESTROY()
    ON_WM_TIMER()
END_MESSAGE_MAP()


// CRegisterAccessDlg message handlers

void CRegisterAccessDlg::OnBnClickedCheckPeriodicRead()
{
    if (((CButton*)GetDlgItem(IDC_CHECK_PERIODIC_READ))->GetCheck() == BST_CHECKED)
    {
        GetDlgItem(IDC_EDIT_ACCESS_FREQUENCY)->EnableWindow(TRUE);
        GetDlgItem(IDC_CHECK_SAVELOG_PERIODIC_READ)->EnableWindow(TRUE);
    }
    else
    {
        GetDlgItem(IDC_EDIT_ACCESS_FREQUENCY)->EnableWindow(FALSE);
        ((CButton*)GetDlgItem(IDC_CHECK_SAVELOG_PERIODIC_READ))->SetCheck(BST_UNCHECKED);
        GetDlgItem(IDC_CHECK_SAVELOG_PERIODIC_READ)->EnableWindow(FALSE);
    }
}

void CRegisterAccessDlg::CreateAccurateTimer(int period, WAITORTIMERCALLBACK callbackFn)
{
    ReleaseAccurateTimer();

    timeBeginPeriod(1);

    m_hTimerQueue = CreateTimerQueue();
    CreateTimerQueueTimer(&m_hTimer, m_hTimerQueue, callbackFn, this,
        0, period, NULL);
}

void CRegisterAccessDlg::ReleaseAccurateTimer()
{
    if (m_hTimer != NULL)
    {
        DeleteTimerQueueTimer(m_hTimerQueue, m_hTimer, NULL);
        m_hTimer = NULL;
    }

    if (m_hTimerQueue != NULL)
    {
        DeleteTimerQueue(m_hTimerQueue);
        m_hTimerQueue = NULL;
    }

    timeEndPeriod(1);
}

void CRegisterAccessDlg::GetRegAddrAndValue(std::map<UINT, std::pair<unsigned short, unsigned short>>& validAddrAndValue)
{
    std::map<UINT, UINT> editCtrlID;
    editCtrlID[IDC_EDIT_ADDR_CONTENT0] = IDC_EDIT_VAL_CONTENT0;
    editCtrlID[IDC_EDIT_ADDR_CONTENT1] = IDC_EDIT_VAL_CONTENT1;
    editCtrlID[IDC_EDIT_ADDR_CONTENT2] = IDC_EDIT_VAL_CONTENT2;
    editCtrlID[IDC_EDIT_ADDR_CONTENT3] = IDC_EDIT_VAL_CONTENT3;
    editCtrlID[IDC_EDIT_ADDR_CONTENT4] = IDC_EDIT_VAL_CONTENT4;
    editCtrlID[IDC_EDIT_ADDR_CONTENT5] = IDC_EDIT_VAL_CONTENT5;
    editCtrlID[IDC_EDIT_ADDR_CONTENT6] = IDC_EDIT_VAL_CONTENT6;
    editCtrlID[IDC_EDIT_ADDR_CONTENT7] = IDC_EDIT_VAL_CONTENT7;

    validAddrAndValue.clear();
    CString csText;
    for (std::map<UINT, UINT>::const_iterator iter = editCtrlID.begin(); iter != editCtrlID.end(); ++iter)
    {
        GetDlgItemText(iter->first, csText);
        if (!csText.IsEmpty())
        {
            unsigned short nRegAddr = (unsigned short)wcstol(csText, NULL, 16);
            GetDlgItemText(iter->second, csText);
            unsigned short nValue = (unsigned short)wcstol(csText, NULL, 16);
            validAddrAndValue[iter->second] = std::pair<unsigned short, unsigned short>(nRegAddr, nValue);
        }
    }
}

void CRegisterAccessDlg::ReadRegisterData(UINT regType, unsigned short address, unsigned short& value,
    int id, int flag, int sensorMode)
{
    switch (regType)
    {
    case IDC_RADIO_I2C:
    {
        EtronDI_GetSensorRegister(m_hEtronDI, &m_devSelInfo, id, address, &value, flag, sensorMode);
        break;
    }
    case IDC_RADIO_ASIC:
    {
        EtronDI_GetHWRegister(m_hEtronDI, &m_devSelInfo, address, &value, flag);
        break;
    }
    case IDC_RADIO_FW:
    {
        EtronDI_GetFWRegister(m_hEtronDI, &m_devSelInfo, address, &value, flag);
        break;
    }
    default:
        break;
    }
}

int CRegisterAccessDlg::IsGyroCase() const
{
    unsigned short addr = 0;
    CString csText;
    GetDlgItemText(IDC_EDIT_ADDR_CONTENT0, csText);
    if (!csText.IsEmpty())
    {
        addr = (unsigned short)wcstol(csText, NULL, 16);
    }

    if (addr == 0x8c && GetRegType() == IDC_RADIO_FW)
        return 0;//AXIS_3_GYRO;
    else if (addr == 0x8d && GetRegType() == IDC_RADIO_FW)
        return AXIS_6_GYRO;
    else 
        return 0;
}

void CRegisterAccessDlg::GetAndShowGyroData(bool periodicVersion, int GyroCase)
{
    unsigned short xValue = 0;
    unsigned short yValue = 0;
    unsigned short zValue = 0;
    unsigned short frameCount = 0;
    unsigned short d1Value = 0;
    unsigned short d2Value = 0;
    unsigned short d3Value = 0;
	unsigned short d4Value = 0;
    unsigned char buf[256] = {0};

    std::chrono::time_point<std::chrono::system_clock> startTime, endTime;
    if (periodicVersion)
    {
        startTime = std::chrono::system_clock::now();
        UpdateGotFrameTime(startTime);
    }

    if (GyroCase == 1)
        ;
    else
    {
        unsigned short GyroLen;
		
        int nRet = EtronDI_GetFlexibleGyroLength(m_hEtronDI, &m_devSelInfo, &GyroLen);
        if (nRet != ETronDI_OK)
        {
            MessageBox(L"Read Gyro Data Failed !");
            return;
        }
        EtronDI_GetFlexibleGyroData(m_hEtronDI, &m_devSelInfo, GyroLen, buf);
        frameCount = *((unsigned short*)&buf[0]);
        xValue = *((unsigned short*)&buf[2]);
        yValue = *((unsigned short*)&buf[4]);
        zValue = *((unsigned short*)&buf[6]);
        d1Value = *((unsigned short*)&buf[8]);
        d2Value = *((unsigned short*)&buf[10]);
        d3Value = *((unsigned short*)&buf[12]);
        d4Value = *((unsigned short*)&buf[14]);
    }
/*
	unsigned char buf[256] = {0};
	char dump[1024];
	EtronDI_GetFlexibleGyroData(m_hEtronDI, &m_devSelInfo, 142, buf);
	for (int i = 0; i < 142; i++)
		{
		sprintf(dump+i*3, "%02x ",buf[i]);
		}
	MessageBox((LPCTSTR)dump);
*/
    if (periodicVersion && m_periodicReadParams.logThread != nullptr)
    {
		endTime = std::chrono::system_clock::now();
        if (GyroCase == AXIS_3_GYRO)
        {
        m_periodicReadParams.logThread->AddData(startTime, endTime, std::pair<unsigned short, unsigned short>(0x8c, xValue));
        m_periodicReadParams.logThread->AddData(startTime, endTime, std::pair<unsigned short, unsigned short>(0x0, yValue));
        m_periodicReadParams.logThread->AddData(startTime, endTime, std::pair<unsigned short, unsigned short>(0x0, zValue));
        m_periodicReadParams.logThread->AddData(startTime, endTime, std::pair<unsigned short, unsigned short>(0x0, frameCount));
        }
		else
		{
		m_periodicReadParams.logThread->AddData(startTime, endTime, std::pair<unsigned short, unsigned short>(0x8d, frameCount));
		m_periodicReadParams.logThread->AddData(startTime, endTime, std::pair<unsigned short, unsigned short>(0x0, xValue));
		m_periodicReadParams.logThread->AddData(startTime, endTime, std::pair<unsigned short, unsigned short>(0x0, yValue));
		m_periodicReadParams.logThread->AddData(startTime, endTime, std::pair<unsigned short, unsigned short>(0x0, zValue));
		m_periodicReadParams.logThread->AddData(startTime, endTime, std::pair<unsigned short, unsigned short>(0x0, d1Value));
		m_periodicReadParams.logThread->AddData(startTime, endTime, std::pair<unsigned short, unsigned short>(0x0, d2Value));
		m_periodicReadParams.logThread->AddData(startTime, endTime, std::pair<unsigned short, unsigned short>(0x0, d3Value));
		m_periodicReadParams.logThread->AddData(startTime, endTime, std::pair<unsigned short, unsigned short>(0x0, d4Value));
		}

    }

    std::ostringstream textX;
    textX << std::setw(4) << std::setfill('0') << std::hex << (GyroCase == AXIS_3_GYRO ? xValue : frameCount);
    SetDlgItemText(IDC_EDIT_VAL_CONTENT0, CString(textX.str().c_str()));
    std::ostringstream textY;
    textY << std::setw(4) << std::setfill('0') << std::hex << (GyroCase == AXIS_3_GYRO ? yValue : xValue);
    SetDlgItemText(IDC_EDIT_VAL_CONTENT1, CString(textY.str().c_str()));
    std::ostringstream textZ;
    textZ << std::setw(4) << std::setfill('0') << std::hex << (GyroCase == AXIS_3_GYRO ? zValue : yValue);
    SetDlgItemText(IDC_EDIT_VAL_CONTENT2, CString(textZ.str().c_str()));
    std::ostringstream textFrameCount;
    textFrameCount << std::setw(4) << std::setfill('0') << std::hex << (GyroCase == AXIS_3_GYRO ? frameCount : zValue);
    SetDlgItemText(IDC_EDIT_VAL_CONTENT3, CString(textFrameCount.str().c_str()));
    if (GyroCase == AXIS_6_GYRO)
    {
        std::ostringstream textD1;
        textD1 << std::setw(4) << std::setfill('0') << std::hex << d1Value;
        SetDlgItemText(IDC_EDIT_VAL_CONTENT4, CString(textD1.str().c_str()));
        std::ostringstream textD2;
        textD2 << std::setw(4) << std::setfill('0') << std::hex << d2Value;
        SetDlgItemText(IDC_EDIT_VAL_CONTENT5, CString(textD2.str().c_str()));
        std::ostringstream textD3;
        textD3 << std::setw(4) << std::setfill('0') << std::hex << d3Value;
        SetDlgItemText(IDC_EDIT_VAL_CONTENT6, CString(textD3.str().c_str()));
        std::ostringstream textD4;
        textD4 << std::setw(4) << std::setfill('0') << std::hex << d4Value;
        SetDlgItemText(IDC_EDIT_VAL_CONTENT7, CString(textD4.str().c_str()));
    }   
}

void CRegisterAccessDlg::ReadRegDataTimerCallback(void* pParam, BOOLEAN timerOrWaitFired)
{
    CRegisterAccessDlg* pThis = (CRegisterAccessDlg*)pParam;
    int GyroCase = pThis->IsGyroCase();

    if (GyroCase)
    {
        pThis->GetAndShowGyroData(true, GyroCase);
    }
    else
    {
        std::chrono::time_point<std::chrono::system_clock> startTime, endTime;
        for (std::map<UINT, std::pair<unsigned short, unsigned short>>::iterator iter = pThis->m_periodicReadParams.addrAndValue.begin();
            iter != pThis->m_periodicReadParams.addrAndValue.end(); ++iter)
        {
            startTime = std::chrono::system_clock::now();

            pThis->UpdateGotFrameTime(startTime);

            pThis->ReadRegisterData(pThis->m_periodicReadParams.regType, iter->second.first, iter->second.second,
                pThis->m_periodicReadParams.id, pThis->m_periodicReadParams.flag, pThis->m_periodicReadParams.sensorMode);

            if (pThis->m_periodicReadParams.logThread != nullptr)
            {
                endTime = std::chrono::system_clock::now();
                pThis->m_periodicReadParams.logThread->AddData(startTime, endTime, iter->second);
            }

            std::ostringstream text;
            text << std::setw(4) << std::setfill('0') << std::hex << iter->second.second;
            pThis->SetDlgItemText(iter->first, CString(text.str().c_str()));
        }
    }
}

void CRegisterAccessDlg::UpdateGotFrameTime(std::chrono::time_point<std::chrono::system_clock> startTime)
{
    std::lock_guard<std::mutex> lock(m_periodicReadParams.recentUpdateTimeMutex);

    if (m_periodicReadParams.recentUpdateTime.size() >= maxRecentUpdateTimeQueueSize)
    {
        m_periodicReadParams.recentUpdateTime.pop();
    }

    m_periodicReadParams.recentUpdateTime.push(startTime);
}

void CRegisterAccessDlg::OnBnClickedRegReadBtn()
{
    CString buttonName;
    GetDlgItemText(IDC_REG_READ_BTN, buttonName);
    if (buttonName == "Stop Reading")
    {
        ReleaseAccurateTimer();
        if (m_periodicReadParams.logThread != nullptr)
        {
            std::ostringstream msg;
            msg << "Logs are saved to \"" << m_periodicReadParams.logFilename.c_str() << "\"";
            MessageBox(CString(msg.str().c_str()));
            m_periodicReadParams.ReleaseLogFile();
        }

        KillTimer(ACTUAL_PERIODIC_TIME_TIMERID);
        m_periodicReadParams.recentUpdateTime = std::queue<std::chrono::time_point<std::chrono::system_clock>>();
        SetDlgItemText(IDC_STATIC_ACTUAL_PERIOD, L"");

        SetDlgItemText(IDC_REG_READ_BTN, L"Read");

        return;
    }

    int flag = 0;
    // Address 2 bytes or 1 byte
    if (BST_UNCHECKED == ((CButton*)GetDlgItem(IDC_CHECK_ADDR_SIZE))->GetCheck()) flag |= FG_Address_1Byte;
    else flag |= FG_Address_2Byte;

    // value 2 bytes or 1 byte
    if (BST_UNCHECKED == ((CButton*)GetDlgItem(IDC_CHECK_VAL_SIZE))->GetCheck()) flag |= FG_Value_1Byte;
    else flag |= FG_Value_2Byte;

    int nId = 0;
    if (BST_CHECKED == ((CButton*)GetDlgItem(IDC_RADIO_I2C))->GetCheck())
    {
        // Slave addr.
        wchar_t szBuf[16] = { 0 };
        GetDlgItemText(IDC_EDIT_SLAVE_ID, szBuf, 16);
        int temp = wcstol(szBuf, NULL, 16);
        nId = (unsigned short)temp;
    }

    UINT regType = GetRegType();
    int sensorMode = GetSensorMode();
    std::map<UINT, std::pair<unsigned short, unsigned short>> addrAndValue;
    GetRegAddrAndValue(addrAndValue);

    if (((CButton*)GetDlgItem(IDC_CHECK_PERIODIC_READ))->GetCheck() == BST_CHECKED)
    {
        m_periodicReadParams.regType = regType;
        m_periodicReadParams.addrAndValue = addrAndValue;
        m_periodicReadParams.id = nId;
        m_periodicReadParams.flag = flag;
        m_periodicReadParams.sensorMode = sensorMode;
        if (((CButton*)GetDlgItem(IDC_CHECK_SAVELOG_PERIODIC_READ))->GetCheck() == BST_CHECKED)
        {
            m_periodicReadParams.CreateLogFile();
        }

        CString sPeriod;
        GetDlgItemText(IDC_EDIT_ACCESS_FREQUENCY, sPeriod);
        int period = _wtoi(sPeriod);
        if (period < 1)
        {
            period = 1;
            SetDlgItemText(IDC_EDIT_ACCESS_FREQUENCY, L"1");
        }

        m_periodicReadParams.recentUpdateTime = std::queue<std::chrono::time_point<std::chrono::system_clock>>();
        SetDlgItemText(IDC_STATIC_ACTUAL_PERIOD, L"");
        SetTimer(ACTUAL_PERIODIC_TIME_TIMERID, 100, NULL);
        CreateAccurateTimer(period, CRegisterAccessDlg::ReadRegDataTimerCallback);
        SetDlgItemText(IDC_REG_READ_BTN, L"Stop Reading");
    }
    else
    {
        int GyroCase = IsGyroCase();
        if (GyroCase)
        {
            GetAndShowGyroData(false, GyroCase);
        }
        else
        {
            for (std::map<UINT, std::pair<unsigned short, unsigned short>>::iterator iter = addrAndValue.begin(); iter != addrAndValue.end(); ++iter)
            {
                ReadRegisterData(regType, iter->second.first, iter->second.second, nId, flag, sensorMode);

                char szBuf[16] = { 0 };
                sprintf_s(szBuf, 16, "%04x", iter->second.second);
                SetDlgItemText(iter->first, CString(szBuf));
            }
        }
    }
}

void CRegisterAccessDlg::WriteRegisterData(UINT regType, unsigned short address, unsigned short value,
    int id, int flag, int sensorMode)
{
    switch (regType)
    {
    case IDC_RADIO_I2C:
    {
        EtronDI_SetSensorRegister(m_hEtronDI, &m_devSelInfo, id, address, value, flag, sensorMode);
        break;
    }
    case IDC_RADIO_ASIC:
    {
        EtronDI_SetHWRegister(m_hEtronDI, &m_devSelInfo, address, value, flag);
        break;
    }
    case IDC_RADIO_FW:
    {
        EtronDI_SetFWRegister(m_hEtronDI, &m_devSelInfo, address, value, flag);
        break;
    }
    default:
        break;
    }
}

int CRegisterAccessDlg::GetSensorMode() const
{
    if (BST_CHECKED == ((CButton*)GetDlgItem(IDC_RADIO_I2C))->GetCheck())
    {
        if (BST_CHECKED == ((CButton*)GetDlgItem(IDC_RADIO_SENSOR1))->GetCheck())
        {
            return EtronDI_SensorMode::Sensor1;
        }
        else if (BST_CHECKED == ((CButton*)GetDlgItem(IDC_RADIO_SENSOR2))->GetCheck())
        {
            return EtronDI_SensorMode::Sensor2;
        }
        else if (BST_CHECKED == ((CButton*)GetDlgItem(IDC_RADIO_SENSOR3))->GetCheck())
        {
            return EtronDI_SensorMode::Sensor3;
        }
        else if (BST_CHECKED == ((CButton*)GetDlgItem(IDC_RADIO_SENSOR4))->GetCheck())
        {
            return EtronDI_SensorMode::Sensor4;
        }
        else if (BST_CHECKED == ((CButton*)GetDlgItem(IDC_RADIO_SENSOR_ALL))->GetCheck())
        {
            return EtronDI_SensorMode::SensorAll;
        }
    }

    return 0;
}

UINT CRegisterAccessDlg::GetRegType() const
{
    if (BST_CHECKED == ((CButton*)GetDlgItem(IDC_RADIO_I2C))->GetCheck())
    {
        return IDC_RADIO_I2C;
    }
    else if (BST_CHECKED == ((CButton*)GetDlgItem(IDC_RADIO_ASIC))->GetCheck())
    {
        return IDC_RADIO_ASIC;
    }
    else if (BST_CHECKED == ((CButton*)GetDlgItem(IDC_RADIO_FW))->GetCheck())
    {
        return IDC_RADIO_FW;
    }

    return 0;
}

void CRegisterAccessDlg::OnBnClickedRegWriteBtn()
{
    int flag = 0;
    // Address 2 bytes or 1 byte
    if (BST_UNCHECKED == ((CButton*)GetDlgItem(IDC_CHECK_ADDR_SIZE))->GetCheck()) flag |= FG_Address_1Byte;
    else flag |= FG_Address_2Byte;

    // value 2 bytes or 1 byte
    if (BST_UNCHECKED == ((CButton*)GetDlgItem(IDC_CHECK_VAL_SIZE))->GetCheck()) flag |= FG_Value_1Byte;
    else flag |= FG_Value_2Byte;

    int nId = 0;
    if (BST_CHECKED == ((CButton*)GetDlgItem(IDC_RADIO_I2C))->GetCheck())
    {
        // Slave addr.
        CString csText;
        GetDlgItemText(IDC_EDIT_SLAVE_ID, csText);
        int temp = wcstol(csText, NULL, 16);
        nId = (unsigned short)temp;
    }

    UINT regType = GetRegType();
    int sensorMode = GetSensorMode();
    std::map<UINT, std::pair<unsigned short, unsigned short>> addrAndValue;
    GetRegAddrAndValue(addrAndValue);
    for (std::map<UINT, std::pair<unsigned short, unsigned short>>::iterator iter = addrAndValue.begin(); iter != addrAndValue.end(); ++iter)
    {
        WriteRegisterData(regType, iter->second.first, iter->second.second, nId, flag, sensorMode);
    }
}

void CRegisterAccessDlg::OnDestroy()
{
    CDialog::OnDestroy();

    ReleaseAccurateTimer();
    KillTimer(ACTUAL_PERIODIC_TIME_TIMERID);
    m_periodicReadParams.recentUpdateTime = std::queue<std::chrono::time_point<std::chrono::system_clock>>();
    SetDlgItemText(IDC_STATIC_ACTUAL_PERIOD, L"");
}

CRegisterAccessDlg::PeriodicReadLog::PeriodicReadLog(const std::string& filename)
    : threadStart(false), logThread(nullptr)
{
    threadStart = true;
    logThread = new std::thread(CRegisterAccessDlg::PeriodicReadLog::WriteLogThreadFn, this, filename);
}

CRegisterAccessDlg::PeriodicReadLog::~PeriodicReadLog()
{
    threadStart = false;
    logThread->join();
    delete logThread;
}

void CRegisterAccessDlg::PeriodicReadLog::AddData(
    const std::chrono::time_point<std::chrono::system_clock>& startTime,
    const std::chrono::time_point<std::chrono::system_clock>& endTime,
    const std::pair<unsigned short, unsigned short>& addrAndValue)
{
    std::lock_guard<std::mutex> lock(dataMutex);
    data.push_back(LogDataItem(startTime, endTime, addrAndValue));
}

void CRegisterAccessDlg::PeriodicReadLog::WriteLogThreadFn(CRegisterAccessDlg::PeriodicReadLog* pThis, const std::string& filename)
{
    std::ofstream logFile(filename, std::ofstream::out);

    while (pThis->threadStart)
    {
        {
            std::lock_guard<std::mutex> lock(pThis->dataMutex);
            for (auto& dataItem : pThis->data)
            {
                auto startTimeT = std::chrono::system_clock::to_time_t(dataItem.startTime);
                size_t startMs = std::chrono::duration_cast<std::chrono::milliseconds>(dataItem.startTime.time_since_epoch()).count() % 1000;
                auto endTimeT = std::chrono::system_clock::to_time_t(dataItem.endTime);
                size_t endMs = std::chrono::duration_cast<std::chrono::milliseconds>(dataItem.startTime.time_since_epoch()).count() % 1000;
                
                logFile << "[start] " << std::put_time(std::localtime(&startTimeT), "%Y-%m-%d %H:%M:%S") 
                    << "." << std::setw(3) << std::setfill('0') << std::dec << startMs 
                    << " [end] " << std::put_time(std::localtime(&endTimeT), "%Y-%m-%d %H:%M:%S")
                    << "." << std::setw(3) << std::setfill('0') << std::dec << endMs 
                    << " [address] 0x" << std::uppercase << std::setw(4) << std::setfill('0') << std::hex << dataItem.addr 
                    << " [value] 0x" << std::uppercase << std::setw(4) << std::setfill('0') << std::hex << dataItem.value << std::endl;
            }
            pThis->data.clear();
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    logFile.close();
}

void CRegisterAccessDlg::OnTimer(UINT_PTR nIDEvent)
{
    if (nIDEvent == ACTUAL_PERIODIC_TIME_TIMERID)
    {
        std::lock_guard<std::mutex> lock(m_periodicReadParams.recentUpdateTimeMutex);
        if (m_periodicReadParams.recentUpdateTime.size() > 1)
        {
            long long t0 = std::chrono::duration_cast<std::chrono::milliseconds>
                (m_periodicReadParams.recentUpdateTime.front().time_since_epoch()).count();
            long long t1 = std::chrono::duration_cast<std::chrono::milliseconds>
                (m_periodicReadParams.recentUpdateTime.back().time_since_epoch()).count();
            float avgPeriod = (float)(t1 - t0) / (m_periodicReadParams.recentUpdateTime.size() - 1);

            std::ostringstream text;
            text << std::setprecision(2) << std::fixed << avgPeriod << " ms";
            SetDlgItemText(IDC_STATIC_ACTUAL_PERIOD, CString(text.str().c_str()));
        }
    }

    CDialog::OnTimer(nIDEvent);
}
