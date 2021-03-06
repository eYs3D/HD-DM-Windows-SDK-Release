#pragma once
#ifdef ESPDI_EG
#include "eSPDI_EG.h"
#else
#include "eSPDI_DM.h"
#endif
#include "utility/ResizableDlg.h"
#include "PreviewImageDlg.h"
#include "DepthDlg.h"

class CMeasureDistanceDlg : public CResizableDlg
{
	DECLARE_DYNAMIC(CMeasureDistanceDlg)

public:
	CMeasureDistanceDlg(CWnd* pParent = NULL);
	virtual ~CMeasureDistanceDlg();

	enum { IDD = IDD_MEASURE_DISTANCE_DLG };

	virtual BOOL OnInitDialog();

	void SetDlgParams(CDepthDlg *pDlg, unsigned int depthWidth, unsigned int depthHeight, unsigned int LZ, unsigned int MZ, unsigned int RZ, bool IsForEX8040);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援
	virtual void OnOK();
	virtual void OnCancel();

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnClose();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnNcLButtonDblClk(UINT nHitTest, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	void ApplyImage();
	void OpenPaintThread();
	void TerminatePaintDepthThread();

	CDC m_MemDC;

	// for paint thread +
	CWinThread* m_PaintThread;
	CEvent m_PaintEvent;
	BOOL m_bThreadContinue;
	// for paint thread -

	CDepthDlg *pDepthDlg;
	unsigned int m_nDepthResWidth;
	unsigned int m_nDepthResHeight;
	unsigned int m_LZ;
	unsigned int m_MZ;
	unsigned int m_RZ;

	unsigned int m_dataNum;
	std::vector<unsigned int> LZ_total;
	float LZAvg_mm;
	std::vector<unsigned int> MZ_total;
	float MZAvg_mm;
	std::vector<unsigned int> RZ_total;
	float RZAvg_mm;

	bool ZAvg_mm();
	bool m_UpdateMeasureDistance;
	bool m_IsForEX8040;
private:
	std::mutex m_MeasureDistanceDataMutex;
};
