#pragma once
#ifdef ESPDI_EG
#include "eSPDI_EG.h"
#else
#include "eSPDI_DM.h"
#endif

class DistanceAccuracyDlg : public CDialogEx
{
	DECLARE_DYNAMIC( DistanceAccuracyDlg )
    DECLARE_MESSAGE_MAP()

public:

    enum { IDD = IDD_DLG_DISTANCE_ACCURACY };

	DistanceAccuracyDlg( void*& hEtronDI, DEVSELINFO& devSelInfo, CPreviewImageDlg* pPreviewDlg );
	virtual ~DistanceAccuracyDlg();

    void UpdateDepthMap( const int Width, const int Height, CDepthDlg* pDepthDlg );
    void EnableDepthList( const BOOL bEnable );
    void Reset( const BOOL bRunAccuracy );
    const CRect& GetAccuracyRegion();

	void UpdatePixelUnit();
	void UpdateFocalLength();

private:

    volatile BOOL  m_bEnable;
    volatile BOOL  m_bStop;
    volatile BOOL  m_bRunAccuracy;
    volatile int   m_Width;
    volatile int   m_Height;
    volatile float m_fRegion;

    CRect      m_rtRegion;
    CDepthDlg* m_pDepthDlg;

    std::mutex                     m_mutex;
    std::unique_ptr< std::thread > m_ThreadAccuracy;
    std::unique_ptr< std::thread > m_ThreadSpatialNoise;
    std::unique_ptr< std::thread > m_ThreadTemporalNoise;
    std::condition_variable        m_event;

    CPreviewImageDlg* m_pPreviewDlg;

    CString m_csDistance;
    CString m_csFillRate;
    CString m_csZAccuracy;
    CString m_csTrmporalNoise;
    CString m_csSpatialNoise;
    CString m_csAngle;
    CString m_csAngleX;
    CString m_csAngleY;
    UINT    m_iGroundTruth;

    void Thread_UpdateAccuracy( void* );
    void Thread_UpdateSpatialNoise( void* );
    void Thread_UpdateTemporalNoise( void* );

	std::vector< WORD > GetDepthZOfROI(int &nWidth, int &nHeight);
	void CalculateFittedPlane(double &a, double &b, double &d,
		std::vector< WORD > &vecDepthZ, int nWidth, int nHeight);

	double CalculateZAccuracy(std::vector< WORD > &vecDepthZ,
		int nWidth, int nHeight,
		double grandtrue, double fitted_GT,
		double vecBefore[3], double vecAfter[3]);

	void SortZ(std::vector< WORD > &vecDepthZ, double dblDeleteBoundaryRatio = 0.005);

	void*& m_hEtronDI;
	DEVSELINFO& m_devSelInfo;

	CString m_csPixelUnit;
	CString m_csLeftFx, m_csLeftFy, m_csRightFx, m_csRightFy;

	virtual void DoDataExchange( CDataExchange* pDX );
    virtual BOOL OnInitDialog();
    virtual void OnOK() {}
    virtual void OnCancel() {}

    afx_msg void    OnCbnSelchangeCbInterest();
    afx_msg void    OnBnClickedChkGroundtruth();
    afx_msg LRESULT OnUpdateUI( WPARAM wp, LPARAM lp );
    afx_msg void    OnEnChangeEdGroundtruth();
    afx_msg void    OnDestroy();
    afx_msg void    OnCbnSelchangeCbDepthlist();
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
};
