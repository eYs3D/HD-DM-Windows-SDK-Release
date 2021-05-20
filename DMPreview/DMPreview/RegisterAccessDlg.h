#pragma once
#ifdef ESPDI_EG
#include "eSPDI_EG.h"
#else
#include "eSPDI_DM.h"
#endif

// CRegisterAccessDlg dialog
class std::thread;

class CRegisterAccessDlg : public CDialog
{
	DECLARE_DYNAMIC(CRegisterAccessDlg)

public:
	CRegisterAccessDlg(void*& hEtronDI, DEVSELINFO& devSelInfo, CWnd* pParent = NULL);   // standard constructor
	virtual ~CRegisterAccessDlg();

	enum { IDD = IDD_DIALOG_REGISTER_ACCESS };

private:
    class LogDataItem
    {
    public:
        LogDataItem() : addr(0xFFFF), value(0xFFFF) {}
        LogDataItem(const std::chrono::time_point<std::chrono::system_clock>& start,
            const std::chrono::time_point<std::chrono::system_clock>& end,
            const std::pair<unsigned short, unsigned short>& addrAndValue)
            : startTime(start), endTime(end), addr(addrAndValue.first), value(addrAndValue.second) {}

        std::chrono::time_point<std::chrono::system_clock> startTime;
        std::chrono::time_point<std::chrono::system_clock> endTime;
        unsigned short addr;
        unsigned short value;
    };

    class PeriodicReadLog
    {
    public:
        PeriodicReadLog(const std::string& filename);
        ~PeriodicReadLog();

        void AddData(const std::chrono::time_point<std::chrono::system_clock>& startTime,
            const std::chrono::time_point<std::chrono::system_clock>& endTime,
            const std::pair<unsigned short, unsigned short>& addrAndValue);

    private:
        bool threadStart;
        std::thread* logThread;
        std::mutex dataMutex;
        std::vector<LogDataItem> data;

    private:
        static void WriteLogThreadFn(CRegisterAccessDlg::PeriodicReadLog* pThis, const std::string& filename);
    };

    class PeriodicReadParams
    {
    public:
        PeriodicReadParams()
        {
            regType = 0;
            id = flag = sensorMode = 0;
            logThread = nullptr;
        }

        ~PeriodicReadParams()
        {
            ReleaseLogFile();
        }

        void CreateLogFile()
        {
            ReleaseLogFile();

            char moduleFilePath[MAX_PATH + 1] = { '\0' };
            GetModuleFileNameA(NULL, moduleFilePath, MAX_PATH);
            std::string sModuleFilePath(moduleFilePath);
            std::ostringstream filename;
            filename << sModuleFilePath.substr(0, sModuleFilePath.find_last_of('\\')).c_str() << "\\periodicRead_" << GetTickCount() << ".log";
            logFilename = filename.str();
            logThread = new PeriodicReadLog(logFilename);
        }

        void ReleaseLogFile()
        {
            logFilename = "";
            if (logThread != nullptr)
            {
                delete logThread;
                logThread = nullptr;
            }
        }

        UINT regType;
        std::map<UINT, std::pair<unsigned short, unsigned short>> addrAndValue;
        int id;
        int flag;
        int sensorMode;
        std::string logFilename;
        PeriodicReadLog* logThread;
        std::mutex recentUpdateTimeMutex;
        std::queue<std::chrono::time_point<std::chrono::system_clock>> recentUpdateTime;
    };

private:
    void*& m_hEtronDI;
    DEVSELINFO& m_devSelInfo;
    PeriodicReadParams m_periodicReadParams;
    HANDLE m_hTimerQueue;
    HANDLE m_hTimer;

private:
    void CreateAccurateTimer(int period, WAITORTIMERCALLBACK callbackFn);
    void ReleaseAccurateTimer();
    void ReadRegisterData(UINT regType, unsigned short address, unsigned short& value,
        int id = 0, int flag = 0, int sensorMode = 0);
    void GetRegAddrAndValue(std::map<UINT, std::pair<unsigned short, unsigned short>>& validAddrAndValue);//key: value id, value: <addr, value>
    static void CALLBACK ReadRegDataTimerCallback(void* pParam, BOOLEAN timerOrWaitFired);
    void WriteRegisterData(UINT regType, unsigned short address, unsigned short value,
        int id, int flag, int sensorMode);
    int GetSensorMode() const;
    UINT GetRegType() const;
    int IsGyroCase() const;
    void GetAndShowGyroData(bool periodicVersion, int GyroCase);
    void UpdateGotFrameTime(std::chrono::time_point<std::chrono::system_clock> startTime);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog();
    virtual void OnOK();
    virtual void OnCancel();

	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedCheckPeriodicRead();
    afx_msg void OnBnClickedRegReadBtn();
    afx_msg void OnBnClickedRegWriteBtn();
    afx_msg void OnDestroy();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
};
