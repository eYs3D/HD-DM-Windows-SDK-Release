// CMeasureDistanceDlg.cpp: 實作檔案
//

#include "stdafx.h"
#include "APC_Test.h"
#include "DepthDlg.h"
#include "WmMsgDef.h"
#include "MeasureDistanceDlg.h"

#define CHECK_MEASUREDISTANCE_CALLBACK_TIMER 1
#define CHECK_MEASUREDISTANCE_CALLBACK_TIME_INTERVAL 1000//8000

// CMeasureDistanceDlg 對話方塊

// for paint thread +
UINT PaintMeasureDistanceThreadFunc(LPVOID pParam);
// for paint thread -

IMPLEMENT_DYNAMIC(CMeasureDistanceDlg, CResizableDlg)

CMeasureDistanceDlg::CMeasureDistanceDlg(CWnd* pParent /*=nullptr*/)
	: CResizableDlg(CMeasureDistanceDlg::IDD, pParent)
{
	m_MemDC.CreateCompatibleDC(NULL);

	// for paint thread +
	m_PaintThread = NULL;
	m_bThreadContinue = TRUE;
	// for paint thread -
}

CMeasureDistanceDlg::~CMeasureDistanceDlg()
{
	m_MemDC.DeleteDC();
}

void CMeasureDistanceDlg::DoDataExchange(CDataExchange* pDX)
{
	CResizableDlg::DoDataExchange(pDX);
}

BOOL CMeasureDistanceDlg::OnInitDialog() {

	CResizableDlg::OnInitDialog();

	SetWindowText(L"MeasureDistanceDlg");

	m_dataNum = 24;

	LZAvg_mm = 0;
	LZ_total.clear();

	MZAvg_mm = 0;
	MZ_total.clear();

	RZAvg_mm = 0;
	RZ_total.clear();
	
	m_IsForEX8040 = false;

	SetResizableInfo(m_nDepthResWidth, m_nDepthResHeight, 320);

	OpenPaintThread();

	SetTimer(CHECK_MEASUREDISTANCE_CALLBACK_TIMER, CHECK_MEASUREDISTANCE_CALLBACK_TIME_INTERVAL, nullptr);

	return TRUE;
}

void CMeasureDistanceDlg::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent)
	{
		case CHECK_MEASUREDISTANCE_CALLBACK_TIMER:
			m_UpdateMeasureDistance = TRUE;
			break;

		default:
			break;
	}

	CResizableDlg::OnTimer(nIDEvent);
}

BEGIN_MESSAGE_MAP(CMeasureDistanceDlg, CResizableDlg)
	ON_WM_CLOSE()
	ON_WM_SYSCOMMAND()
	ON_WM_NCLBUTTONDBLCLK()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CMeasureDistanceDlg 訊息處理常式
void CMeasureDistanceDlg::SetDlgParams(CDepthDlg *pDlg, unsigned int depthWidth, unsigned int depthHeight, unsigned int LZ, unsigned int MZ, unsigned int RZ, bool IsForEX8040)
{
	pDepthDlg = pDlg;
	m_LZ = LZ;
	m_MZ = MZ;
	m_RZ = RZ;
	m_IsForEX8040 = IsForEX8040;
}

void CMeasureDistanceDlg::ApplyImage()
{
	std::lock_guard<std::mutex> lock(m_MeasureDistanceDataMutex);
	m_PaintEvent.SetEvent();
}

// for paint thread +
void CMeasureDistanceDlg::OpenPaintThread() {

	//OutputDebugString("Create Paint Thread...\n");
	m_PaintThread = AfxBeginThread(PaintMeasureDistanceThreadFunc, this, THREAD_PRIORITY_NORMAL, 0, 0, NULL);
}

void CMeasureDistanceDlg::TerminatePaintDepthThread() {

	m_bThreadContinue = FALSE;
	m_PaintEvent.SetEvent();
	WaitForSingleObject(m_PaintThread->m_hThread, INFINITE);
}

UINT PaintMeasureDistanceThreadFunc(LPVOID pParam) {

	CMeasureDistanceDlg *pdlg = (CMeasureDistanceDlg*)pParam;
	std::ostringstream dlgTitle;
	std::string str;

	CDC *pDC = NULL;
	pDC = pdlg->GetDC();

	while (pdlg->m_bThreadContinue) {

		::WaitForSingleObject(pdlg->m_PaintEvent, INFINITE);

		if (pdlg->m_bThreadContinue) {

			pdlg->ZAvg_mm();

			if (pdlg->m_UpdateMeasureDistance)
			{
				pdlg->m_UpdateMeasureDistance = FALSE;

				dlgTitle.str("");
				dlgTitle << (pdlg->LZAvg_mm) / 10 << " cm ";
				str = dlgTitle.str();
				pdlg->SetDlgItemText(IDC_LEFT_CROSS, CString(str.c_str()));

				dlgTitle.str("");
				dlgTitle << (pdlg->MZAvg_mm) / 10 << " cm ";
				str = dlgTitle.str();
				pdlg->SetDlgItemText(IDC_MIDDLE_CROSS, CString(str.c_str()));

				dlgTitle.str("");
				dlgTitle << (pdlg->RZAvg_mm) / 10 << " cm ";
				str = dlgTitle.str();
				pdlg->SetDlgItemText(IDC_RIGHT_CROSS, CString(str.c_str()));

			}
		}
	}

	if (pDC) {

		pdlg->ReleaseDC(pDC);
		pDC = NULL;
	}

	return 0;
}
// for paint thread -

bool CMeasureDistanceDlg::ZAvg_mm()
{
	if (LZ_total.empty())
	{
		LZ_total.push_back(m_LZ);
		LZAvg_mm = m_LZ;
	}
	else
	{
		if (((float)m_LZ > LZAvg_mm * 1.1) || ((float)m_LZ < LZAvg_mm * 0.9))
		{
			LZ_total.clear();
			LZ_total.push_back(m_LZ);
			LZAvg_mm = m_LZ;
		}
		else if (((float)m_LZ <= LZAvg_mm * 1.1) || ((float)m_LZ >= LZAvg_mm * 0.9))
		{
			LZ_total.push_back(m_LZ);
			if (LZ_total.size() > m_dataNum)
			{
				LZ_total.erase(LZ_total.begin() + 0);
			}
		}
	}

	if (MZ_total.empty())
	{
		MZ_total.push_back(m_MZ);
		MZAvg_mm = m_MZ;
	}
	else
	{
		if (((float)m_MZ > MZAvg_mm * 1.1) || ((float)m_MZ < MZAvg_mm * 0.9))
		{
			MZ_total.clear();
			MZ_total.push_back(m_MZ);
			MZAvg_mm = m_MZ;
		}
		else if (((float)m_MZ <= MZAvg_mm * 1.1) || ((float)m_MZ >= MZAvg_mm * 0.9))
		{
			MZ_total.push_back(m_MZ);
			if (MZ_total.size() > m_dataNum)
			{
				MZ_total.erase(MZ_total.begin() + 0);
			}
		}
	}


	if (RZ_total.empty())
	{
		RZ_total.push_back(m_RZ);
		RZAvg_mm = m_RZ;
	}
	else
	{
		if (((float)m_RZ > RZAvg_mm * 1.1) || ((float)m_RZ < RZAvg_mm * 0.9))
		{
			RZ_total.clear();
			RZ_total.push_back(m_RZ);
			RZAvg_mm = m_RZ;
		}
		else if (((float)m_RZ <= RZAvg_mm * 1.1) || ((float)m_RZ >= RZAvg_mm * 0.9))
		{
			RZ_total.push_back(m_RZ);
			if (RZ_total.size() > m_dataNum)
			{
				RZ_total.erase(RZ_total.begin() + 0);
			}
		}
	}
	
	float LZAvg_tmp = 0;
	float MZAvg_tmp = 0;
	float RZAvg_tmp = 0;

	for (int i = 0; i < LZ_total.size(); i++)
	{
		//TRACE("tree::LZ[%d]=%d\n", i, LZ_total[i]);
		LZAvg_tmp += LZ_total[i];
	}

	for (int i = 0; i < MZ_total.size(); i++)
	{
		//TRACE("tree::MZ[%d]=%d\n", i, MZ_total[i]);
		MZAvg_tmp += MZ_total[i];
	}

	for (int i = 0; i < RZ_total.size(); i++)
	{
		//TRACE("tree::RZ[%d]=%d\n", i, RZ_total[i]);
		RZAvg_tmp += RZ_total[i];
	}

	LZAvg_mm = (((int)LZAvg_tmp / LZ_total.size()) * 1.0f);
	MZAvg_mm = (((int)MZAvg_tmp / MZ_total.size()) * 1.0f);
	RZAvg_mm = (((int)RZAvg_tmp / RZ_total.size()) * 1.0f);

	return true;
}



void CMeasureDistanceDlg::OnClose() {

	KillTimer(CHECK_MEASUREDISTANCE_CALLBACK_TIMER);

	TerminatePaintDepthThread();

	// if dialog's parent has WS_CHILD style, GetParent() will return parent's parent, 
	// thus the only way to get dialog's real parent is using m_pParentWnd directly
	m_pParentWnd->PostMessage(WM_MSG_CLOSE_PREVIEW_DLG, (WPARAM)this);
	CResizableDlg::OnClose();
}

void CMeasureDistanceDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if (nID == SC_MAXIMIZE)
	{
		return MaximizeWindow();
	}

	CResizableDlg::OnSysCommand(nID, lParam);
}

void CMeasureDistanceDlg::OnNcLButtonDblClk(UINT nHitTest, CPoint point)
{
	if (nHitTest == HTCAPTION)
	{
		return MaximizeWindow();
	}

	CResizableDlg::OnNcLButtonDblClk(nHitTest, point);
}

void CMeasureDistanceDlg::OnOK()
{
	OnCancel();
}

// in modal dialog, OnCancel will be called after calling OnClose, 
// thus put uninit codes in OnCancel, and both method will run the uninit codes.
// in modaless dialog, OnCancel won't be called after calling OnCLose, 
// thus put uninit codes in one method, and in another method manually call the method.
void CMeasureDistanceDlg::OnCancel()
{
	SendMessage(WM_CLOSE);
}

