#pragma once
#ifdef ESPDI_EG
#include "eSPDI_EG.h"
#else
#include "eSPDI_DM.h"
#endif
#include "utility/ResizableDlg.h"
#include "PreviewImageDlg.h"

struct DepthfilterParam
{
	bool bDoDepthFilter;
	bool bSubSample;
	bool bEdgePreServingFilter;
	bool bHoleFill;
	bool bTempleFilter;
	bool bFlyingDepthCancellation;
//Common
    int bytesPerPixel;
//SubSample
	int SubSampleMode;
	int SubSampleFactor;
//EdgePreServingFilter
	int type;
	int nEdgeLevel;
	float sigma;
	float lumda;
//HoleFill	
	int kernelSize;
	int nLevel;
	bool bHorizontal;
//TemporalFilter
	float alpha;
	int history;

    DepthfilterParam() { memset( this, NULL, sizeof( DepthfilterParam ) ); }
};

class CDepthDlg : public CResizableDlg
{
DECLARE_DYNAMIC(CDepthDlg)
DECLARE_MESSAGE_MAP()

public:
  CDepthDlg(CWnd* pParent = NULL);
  virtual ~CDepthDlg();

  enum { IDD = IDD_DEPTH_DLG };
  
  virtual BOOL OnInitDialog();

  void SetImageParams(void* hEtronDI, DEVSELINFO devSelInfo, unsigned short devType, 
      EtronDIImageType::Value depthImageType,
      int depthId, int zdTableIndex, int depthWidth, int depthHeight, int iUpdateZ_period,
      float camFocus = 0.0, float baselineDist = 0.0, std::vector<float> multiBaselineDist = std::vector<float>() );
  void SetHandle( void* hEtronDI, const DEVSELINFO& devSelInfo );
  void UpdateColorPalette( DEPTHMAP_TYPE depthMapType );
  void UpdateColorPalette( int zFar, int zNear );
  void GetDepthZValue( int& zFar, int& zNear );
  void SetDepthPos(const CPoint pos);
  bool GetDepthData(std::vector<unsigned char>& depthBuf, EtronDIImageType::Value& depthImageType, 
      int& width, int& height, int& serialNumber);

  void UpdateFusionSelectedIndex(const std::vector<unsigned char>& selectedIndex);
  void ApplyImage(BYTE **pDepthBuf, const int dataSize, const int nDepthSerialNum, const CRect& rtAccuracyRegion, const DepthfilterParam& DfParam );
  void DepthFilter(BYTE* pDepthBuf, const DepthfilterParam& DfParam);
  void EnableRotate( const BOOL bRotate );

  unsigned char* GetDepthColorMapImage();
  void GetDepthColorfulImageFromColorPalette(std::vector<BYTE> &buffer);
  void GetDepthGrayImageFromColorPalette(std::vector<BYTE> &buffer);
  
  BITMAPINFO* GetbmiDept();

  inline void SetDepthROI( const int iDepthROI ) { m_iDepthROI = iDepthROI; }
  inline void SetBaselineFocalLength( const float fBaseline, const float fFocalLength ) { m_fBaseline = fBaseline; m_fFocalLength = fFocalLength; }
  inline float GetBaseline() { return m_fBaseline; }
  inline float GetFocalLength() { return m_fFocalLength; }

  WORD GetZValue(int x, int y);
  WORD GetZValue(int x, int y, BYTE* pDeptg);

private:

  //void ShowImage(CRect clientRect);
  void TerminatePaintDepthThread();
  float UpdateAndGetFramerate();
  LRESULT OnUpdateDlgTitle(WPARAM wParam, LPARAM lParam);
  WORD GetDepthData(int x, int y);
  void GenerateZDValueTitleText();
  void GetZDTable();
  bool m_isHalfImage;
  int m_iUpdateZ_period;
  int m_iUpdateZ_Count;
  CString m_cs_ZD_Text;
  BOOL m_bRotate;
  unsigned int m_nDepthResWidth;
  unsigned int m_nDepthResHeight;

  void GetDepthRGBImage(const RGBQUAD* pColorPalette, std::vector<BYTE> &buffer);

  void UpdateD8DisplayImage_DIB24( const RGBQUAD* pColorPalette, const BYTE* pDepth, BYTE* pResult );
  void UpdateD11DisplayImage_DIB24( const RGBQUAD* pColorPalette, const WORD* pDepth, BYTE* pResult);
  void UpdateZ14DisplayImage_DIB24( const RGBQUAD* pColorPalette, const WORD* pDepth, BYTE* pResult);
  
  void UpdateD11_Baseline_DisplayImage_DIB24( const RGBQUAD* pColorPalette, const WORD* pDepth, BYTE* pResult);
  void UpdateD11_Fusion_DisplayImage_DIB24( const RGBQUAD* pColorPalette, const WORD* pDepthFs, const WORD* pDepth, BYTE* pResult);

  static UINT __stdcall PaintDepthThreadFunc( LPVOID pParam );

  virtual void DoDataExchange(CDataExchange* pDX);
  virtual void OnOK() {}
  virtual void OnCancel();
  
  bool m_mouseTracking;
  CPoint m_curDepthPos;
  int m_depthId;
  LARGE_INTEGER m_liPerfFreq;
  LARGE_INTEGER m_liPerfTime;
  std::list<LONGLONG> m_recentFrameTimestamp;
  CString m_csDialogTitle;
  BYTE* m_zdTable;
  int m_zdTableSize;
  int m_zdTableIndex;
  std::vector<BYTE> m_depthData;
  std::vector<unsigned char> m_fusionSelectedIndex;
  int m_depthSerialNumber;
  std::mutex m_depthDataMutex;
  float m_camFocus;
  float m_baselineDist;
  std::vector<float> m_mblDistMultiplier;
  std::vector<unsigned char> m_tempBuf;
  std::vector<BYTE> m_depthData_original;
  std::vector<BYTE> halfDepthBuf;
  std::vector< WORD > m_vecTableZ14ToD11;
  std::vector< WORD > m_vecZ14ToD11;

  BYTE* m_pImageBuf;

  volatile int m_iDepthROI;
  CRect m_rtAccuracyRegion;

  BITMAPINFO m_bmiDepth;
  RGBQUAD m_ColorPalette[16384];
  RGBQUAD m_GrayPalette[16384];
  
  unsigned char m_nAlphaVal;
  COLORREF m_clr;
  CDC m_MemDC;
  
  // for paint thread +
  CWinThread* m_PaintThread;
  CEvent m_PaintEvent;
  BOOL m_bThreadContinue;
  // for paint thread -

  void *m_hEtronDI;
  DEVSELINFO m_DevSelInfo;
  
  DEPTHMAP_TYPE m_DepthMapType; 
  unsigned short m_DevType;
  EtronDIImageType::Value m_depthImageType;
 
  int m_Near;
  int m_Far;

  float m_fBaseline;
  float m_fFocalLength;

  afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
  afx_msg void OnNcLButtonDblClk(UINT nHitTest, CPoint point);
  afx_msg void OnMouseLeave();
  afx_msg void OnMouseMove(UINT nFlags, CPoint point);
  afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
  afx_msg void OnClose();
  afx_msg void OnPaint();
};
