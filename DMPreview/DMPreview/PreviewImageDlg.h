#pragma once
#ifdef ESPDI_EG
#include "eSPDI_EG.h"
#else
#include "eSPDI_DM.h"
#endif

#include "utility\PointCloudViewer\PointCloudViewer.h"
#include "utility/FrameGrabber.h"
#include "utility\FrameProcess\PlyFilter.h"
#include "RegisterSettings.h"

typedef enum {
	TRANSFER_TO_COLORFULRGB,
	TRANSFER_TO_GRAYRGB
}DEPTHMAP_TYPE;

enum {COLOR=0,DEPTH}; 

#include "VideoDeviceDlg.h"
class CVideoDeviceDlg;
class CDepthFusionHelper;
class CDepthDlg;
class WaitDlg;
class AEAWB_PropertyDlg;
class DistanceAccuracyDlg;
class DepthFilterDlg;
struct DepthfilterParam;

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam);

class CPreviewImageDlg : public CDialog
{
	DECLARE_DYNAMIC(CPreviewImageDlg)
    DECLARE_MESSAGE_MAP()
public:

	CPreviewImageDlg(void*& hApcDI, DEVSELINFO& devSelInfo, const USHORT& devType, const USB_PORT_TYPE eUSB_Port_Type);   // standard constructor
	virtual ~CPreviewImageDlg();

    enum { IDD = IDD_DIALOG_PREVIEW_IMAGE };

    inline void SetPropertyDlg( AEAWB_PropertyDlg* pPropertyDlg ) { m_pPropertyDlg = pPropertyDlg; }
    inline void SetAccuracyDlg( DistanceAccuracyDlg* pAccuracyDlg ) { m_pAccuracyDlg = pAccuracyDlg; }
    inline void SetDepthFilterDlg( DepthFilterDlg* pDepthFilterDlg ) { m_pDepthFilterDlg = pDepthFilterDlg; }
    inline void DepthIndexAccuracy( const int iDepthIndex ) { m_i8038DepthIndex = iDepthIndex; }

    void SetFilterParam( DepthfilterParam& xDfParam );
    void EnableAccuracy( const BOOL bEnableAccuracy );
	BOOL IsDevicePid(const int pid);

    afx_msg LRESULT OnSnapshot(WPARAM wParam, LPARAM lParam);
    afx_msg void OnBnClickedPreviewBtn();

	BOOL IsInterLeaveMode();
	CVideoDeviceDlg * m_pdlgVideoDeviceDlg;
private:

    int GetDepthStreamIndex(int depthIndex) const;

    void OpenMainStream();
	void OpenTrackStream();
	void OpenKColorStream();

    bool UpdateStreamInfo();
    void UpdateUI();
	void UpdateKcolorStreamUI(int selectIndex);
	void UpdateTrackStreamUI(int selectIndex);
	void UpdateColorStreamUI(int selectIndex);
	void UpdateDepthStreamUI(int selectIndex);
    void PrepareRectifyLogData(eSPCtrl_RectLogData*& pRectifyLog, int index);
    void UpdatePreviewParams();
    int CurColorOption();
	CPoint CurKColorImgRes();
	CPoint CurTrackImgRes();
    CPoint CurColorImgRes();
    CPoint CurDepthImgRes();
	int AdjDepthBitIndex( const int depthType );
    int GetDepthStreamIndex(CPoint depthRes) const;
	int GetColorStreamIndex(CPoint colorRes, bool IsMJPG) const;
    BOOL IsColorStreamMJPEG();
    int EnableDenoise(bool enable);
    int InitEysov(LONG& outWidth, LONG& outHeight);
    void PreparePreviewDlg();
    void InitPreviewDlgPos();
    void CloseDeviceAndStopPreview(CDialog* pCallerDlg);
    static void ImgCallback(APCImageType::Value imgType, int imgId, unsigned char* imgBuf, int imgSize,
        int width, int height, int serialNumber, void* pParam);
	void ProcessImgCallback(APCImageType::Value imgType, int imgId, std::vector<unsigned char> imgBuf, int imgSize,
		int width, int height, int serialNumber);

    BOOL IsStream0ColorPlusDepth();
    void UpdateIRConfig();
    void SetIRValue(WORD value);
	void ChangeIRValue(WORD value);
    static void DepthFusionCallback(unsigned char* depthBuf, unsigned char* selectedIndex, int depthSize, 
        int width, int height, int serialNumber, void* pParam);
	static void FrameGrabberCallback(   BOOL isDepthOnly,
                                        std::vector<unsigned char>& bufDepth,
                                        int widthDepth,
                                        int heightDepth,
                                        std::vector<unsigned char>& bufColor,
                                        int widthColor,
                                        int heightColor,
                                        int serialNumber,
                                        void* pParam);
	static void PointcloudViewerMessageCallback(CPointCloudViewer::MessageType msg, int value, void* pParam);
    static DWORD __stdcall Thread_Preview( void* pvoid );
    void UpdateUIForDemo();
    bool IsSupportHwPostProc() const;
	void EnableIR(bool enable);
	void InitIR();
    void InitDepthROI();
    void InitModeConfig();
	void ResetStreamTimeStamp();
    void DoSerialNumCommand();
    void DoMultiModuleSynCommand();
    void DoMultiModuleSynReset();
#ifndef ESPDI_EG
	void GetDepthIndexFromColorStream();
#endif
	BOOL CheckRestart();
    BOOL CheckDepthImageReady();
    BOOL CheckColorImageReady();
	bool PointCloudViewer();
	int getRectifyLogData(eSPCtrl_RectLogData* rectifyLog,int index);
	int getRectifyLogDataSlave(eSPCtrl_RectLogData* rectifyLog, int index);

	void SaveDepthYuv(std::vector<unsigned char> bufDepth, APCImageType::Value depthImageType, int widthDepth, int heightDepth, const char* pFileName);
	void SaveDepthGrayBmp(int DepthNum, const char* pFileName);
	void SaveDepthColorBmp(int DepthNum, const char* pFileName);
	void DepthFusionBmp(APCImageType::Value depthImageType);
	//bool CPreviewImageDlg::usePlyFilter(APCImageType::Value depthImageType);
	static UINT DoSnapshot(LPVOID lpParam);//CPreviewImageDlg* pThis, WORD irValue, float zFar);
    static BOOL isDepthOnly(int depthWidth, int depthHeight, BOOL isColorBufEmpty);

	void InitAutoModuleSync();

    class CPreviewItem
    {
    public:
        CPreviewItem();
        ~CPreviewItem();

        void Reset(CDialog* pCallerDlg);
        void SetPreviewDlg(CDialog* pDlg);

        CDialog* m_previewDlg;
        CPoint m_imgRes;
    };

    class CPreviewParams
    {
    public:
        CPreviewParams();
        ~CPreviewParams();

        void Reset(CDialog* pCallerDlg);

        CMutex m_mutex;
        CPreviewItem m_colorPreview;
		CPreviewItem m_trackPreview;
		CPreviewItem m_kcolorPreview;
        int m_colorOption;
		int m_tcolorOption;
		int m_kcolorOption;
        bool m_360ModeEnabled;
		CPreviewItem m_depthInforPreview;
        CPreviewItem m_depthPreview[APC_MAX_DEPTH_STREAM_COUNT];
        CPreviewItem m_depthFusionPreview;
        std::vector<unsigned char> m_depthFusionSelectedIndex;
        bool m_showFusionSelectedDlg;
        CPreviewItem m_fusionSelectedTargetPreview;
        std::vector<unsigned char> m_fusionTargetRgbImgBuf;
        int m_depthOption;
        int m_depthSwitch;
        bool m_IsDepthFusionOn;
		bool m_rotate;
		WORD m_IrPreState;
        WORD m_depthType;				//bits of depth data
        bool m_rectifyData;
        float m_camFocus;
		eSPCtrl_RectLogData* m_rectifyLogData[APC_MAX_DEPTH_STREAM_COUNT] = {nullptr}; //support 3 max rectify log
		eSPCtrl_RectLogData* m_rectifyLogDataSlave =  nullptr ; //slave
		CPointCloudViewer *m_pointCloudViewer;
        std::vector<float> m_baselineDist;
    };
    void*& m_hApcDI;
    DEVSELINFO& m_DevSelInfo;
    const USHORT& m_DevType;
    int m_iInterLeaveModeFPS;
	int m_kcolorStreamOptionCount;
	int m_tcolorStreamOptionCount;
    int m_colorStreamOptionCount;
	APC_STREAM_INFO m_pStreamKColorInfo[APC_MAX_STREAM_COUNT];
	APC_STREAM_INFO m_pStreamTColorInfo[APC_MAX_STREAM_COUNT];
    APC_STREAM_INFO m_pStreamColorInfo[APC_MAX_STREAM_COUNT];
	int m_kdepthStreamOptionCount;
	int m_tdepthStreamOptionCount;
    int m_depthStreamOptionCount;
    int m_ZFar;
    int m_ZNear;
	APC_STREAM_INFO m_pStreamKDepthInfo[APC_MAX_STREAM_COUNT];
	APC_STREAM_INFO m_pStreamTDepthInfo[APC_MAX_STREAM_COUNT];
    APC_STREAM_INFO m_pStreamDepthInfo[APC_MAX_STREAM_COUNT];
    CPreviewParams m_previewParams;
    std::pair<WORD, WORD> m_irRange;
    void AdjustZDTableIndex(int *pzdTblIdx, int width, int height, APCImageType::Value DImgType);
	void AdjustColorResForDepth0(CPoint*colorRealRes);
	void AdjustRegister();
    void AdjustNearFar( int& zFar, int& zNear, CDepthDlg* pDlg );
#ifndef ESPDI_EG
    CDepthFusionHelper* m_depthFusionHelper;
	FrameGrabber* m_frameGrabber;
#endif
	RegisterSettings* m_registerSettings;
	char m_LogImgfile[128];
    DEVINFORMATIONEX m_devinfoEx;
	std::vector<unsigned char*> bufDepthTmp;
	bool m_isPreviewed;
	WORD m_irValue = -1;
    int m_imgSerialNumber;
    PointCloudInfo m_xPointCloudInfo;
    
	AEAWB_PropertyDlg* m_pPropertyDlg;
	bool m_bPrevLowLigh;

    DistanceAccuracyDlg* m_pAccuracyDlg;
    DepthFilterDlg* m_pDepthFilterDlg;
    const USB_PORT_TYPE m_eUSB_Port_Type;
    std::unique_ptr< WaitDlg > m_pWaitDlg;
	std::map< int, time_t> m_mapColorStreamTimeStamp;
	std::map< int, time_t> m_mapDepthStreamTimeStamp;
    std::vector<float> m_pointCloudDepth;
    std::vector<BYTE> m_pointCloudRGB;
    std::unique_ptr< DepthfilterParam > m_DfParam;
    
    volatile BOOL m_bPCV_NoColorStream;
    volatile BOOL m_bPCV_SingleColor;

    volatile BOOL m_bIsInterLeaveMode;
    volatile int m_i8038DepthIndex;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog();
    virtual void OnOK() {}
    virtual void OnCancel() {}
	
	LRESULT OnClosePreviewDlg(WPARAM wParam, LPARAM lParam);

    afx_msg LRESULT OnUpdateMousePos(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSnapshotAll(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnSnapshotComplete(WPARAM wParam, LPARAM lParam);
    afx_msg void OnBnClickedCheckColorStream();
    afx_msg void OnBnClickedCheck360mode();
    //afx_msg void OnBnClickedPreviewBtn();
    afx_msg void OnDestroy();
    afx_msg void OnCbnSelchangeComboColorStream();
    afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg void OnBnClickedCheckDepthFusion();
    afx_msg void OnBnClickedRadioDepthMappingZdtable();
    afx_msg void OnBnClickedRadioDepthMappingManual();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnCbnSelchangeComboDepthOutputCtrl();
	afx_msg void OnBnClickedCheckPumaPostproc();
    afx_msg void OnBnClickedCheckPp();
    afx_msg void OnBnClickedCheckFusionSwpp();
    afx_msg void OnBnClickedCheckInterleaveMode();
	afx_msg void OnBnClickedSnapshotBtn();
	afx_msg void OnBnClickedCheckRotateImg();
	afx_msg void OnBnClickedCheckPointcloudViewer();
	afx_msg void OnBnClickedFrameSync();
	afx_msg void OnBnClickedCheckTColorStream();
	afx_msg void OnCbnSelchangeComboTColorStream();
    afx_msg void OnCbnSelchangeComboModeConfig();
    afx_msg void OnBnClickedRadioRectifyAndRawData();
    afx_msg void OnCbnSelchangeComboFrameRate();
    afx_msg void OnBnClickedBtZSet();
    afx_msg void OnBnClickedBtZReset();
    afx_msg void OnBnClickedChkMaster();
    afx_msg void OnBnClickedChkIrmaxExt();
    afx_msg void OnBnClickedChkPcvNocolor();
    afx_msg void OnBnClickedChkPcvSingle();
public:
	afx_msg void OnBnClickedChkMultiSync();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);

public:
	int MappingIMU(bool reopen = false);
	int ReOpen_MappingIMU();
};
