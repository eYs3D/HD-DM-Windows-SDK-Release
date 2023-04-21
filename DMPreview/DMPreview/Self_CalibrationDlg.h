#pragma once

#ifdef ESPDI_EG
#include "eSPDI_EG.h"
#else
#include "eSPDI_DM.h"
#endif

#include "PreviewImageDlg.h"
#include "utility/WindowsMfcUtility.h"
#include "ModuleCalibParWrite.h"
#include "Self_Calibration_API.h"

//using namespace std;
using std::cout;


class Self_CalibrationDlg : public CDialog
{
    DECLARE_DYNAMIC( Self_CalibrationDlg )
    DECLARE_MESSAGE_MAP()

public:

    enum { IDD = IDD_SELF_K_DIALOG};

    Self_CalibrationDlg( void*& hApcDI, DEVSELINFO& devSelInfo, const DEVINFORMATIONEX& devinfoEx, CWnd* pParent , CPreviewImageDlg* pPreviewDlg);
    virtual ~Self_CalibrationDlg();

    void SetImageParams(const int imgWidth, const int imgHeight, const BOOL isLRD_Mode, const BOOL isRectified, const CPoint& cpDepth);
    void ApplyInputImage(APCImageType::Value imgType, int imgId, unsigned char *imgBuf, int imgSize, int width, int height, int serialNumber);
    void disableSelfK();

private:
    // camera
    void*&            m_hApcDI;
    DEVSELINFO&       m_DevSelInfo;
    const DEVINFORMATIONEX& m_xDevinfoEx;
    BOOL m_IsLRD_Mode;
    BOOL m_IsRectified;

    int m_nColorResWidth;
    int m_nColorResHeight;
    int m_colorNum = 0;
    CPoint m_cpDepth;
    std::mutex m_imgBufMutex;
    BOOL mRun;
    BOOL mDumpColorRGBFrame;
    int mBasicSchedule;


    //UI
    CPreviewImageDlg* m_pPreviewDlg;


    virtual BOOL OnInitDialog();
    virtual void DoDataExchange( CDataExchange* pDX );
    virtual void OnOK() {};
    virtual void OnCancel() {};
    void InitUI();

    //self-k main sample
    //class Resolution
    //{
    //public:
    //    int height = 720;
    //    int width = 1280;
    //};
    //int g_nDeviceCount;
    void *g_p_depth;
    void *g_p_color;
    int res_depth_option_id;

    unsigned char* m_color_sbs = nullptr;
    unsigned char* m_gray_sbs = nullptr;
    unsigned char* m_color = nullptr;
    unsigned char* m_depth = nullptr;

    self_calibration::self_calibration_api *p_self_k = new self_calibration::self_calibration_api;
    eSPCtrl_RectLogData *p_rectifyLogData = nullptr;
    int color_width;
    int color_height;
    int depth_width;
    int depth_height;
    int RectLogData2ModuleParameter(eSPCtrl_RectLogData &rectifyData, self_calibration::module_parameters &moduleParameter);
    int ModuleParameter2RectLogData(self_calibration::module_parameters &moduleParameter, eSPCtrl_RectLogData &rectifyData);

    void getErrorMessage(self_calibration::self_calibration_issue error, std::string& message);
    bool updateMonitorState();
    bool updateCurrentPars();
    bool updateEstimatedPars();

    int initSelfK();
    int executeSelfK();
    int releaseSelfK();
    bool checkLRDMode();
    void checkNumberAndPoint(int nID);
    void BGR2Gray(unsigned char *src, unsigned char *dst, int width, int height);
    void RGB2Gray(unsigned char *src, unsigned char *dst, int width, int height);

public:
    afx_msg void OnEnChangeSelfKBasicSchedule();
    afx_msg void OnBnClickedSelfKBasicRubChk();
    afx_msg void OnBnClickedIdcSelfKBasicResetallBtn();
    afx_msg void OnBnClickedSelfKBasicAdvanceChk();
    afx_msg void OnEnChangeEditSelfKAdvanceStrategydoqulitymonitor();
    afx_msg void OnEnChangeEditSelfKAdvanceStrategysleeptimeforqualitycheck();
    afx_msg void OnEnChangeEditSelfKAdvanceStrategythresholdforbadquality();
    afx_msg void OnEnChangeEditSelfKAdvanceStrategythresholdautocalibrarting();
    afx_msg void OnEnChangeEditSelfKAdvanceStrategyautocalibration();
    afx_msg void OnEnChangeEditSelfKAdvanceStrategytargetquality();
    afx_msg void OnEnChangeEditSelfKAdvanceStrategycalibrationmode();
    afx_msg void OnEnChangeEditSelfKAdvanceStrategyaccuracylevel();
    afx_msg void OnEnChangeEditSelfKAdvanceStrategymaxerrortrials();
    afx_msg void OnEnChangeEditSelfKAdvanceStrategytaretarget();
    afx_msg void OnBnClickedEditSelfKAdvanceStrategyreadbtn();
    afx_msg void OnBnClickedEditSelfKAdvanceStrategywritebtn();
    afx_msg void OnBnClickedEditSelfKAdvanceStrategyresetbtn();
    afx_msg void OnBnClickedEditSelfKAdvanceOperatorcalculatescorebtn();
    afx_msg void OnBnClickedEditSelfKAdvanceOperatordocalibrationbtn();
    afx_msg void OnBnClickedEditSelfKAdvanceOperatorapplybeforeregbtn();
    afx_msg void OnBnClickedEditSelfKAdvanceOperatorapplyafterregbtn();
    afx_msg void OnBnClickedEditSelfKAdvanceOperatorapplyafterflashbtn();
    afx_msg void OnBnClickedEditSelfKAdvanceOperatorresetallbtn();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
};
