
// EtronDI_TestDlg.cpp : 實作檔
//

#include "stdafx.h"
#include "EtronDI_Test.h"
#include "EtronDI_TestDlg.h"
#include "VideoDeviceDlg.h"
#include "IMUTestDlg.h"
#include "WmMsgDef.h"
#include "utility/WindowsMfcUtility.h"
#include "utility/EtronDIUtility.h"
#include "AutoModuleSyncManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 對 App About 使用 CAboutDlg 對話方塊

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 對話方塊資料
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援

// 程式碼實作
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CEtronDI_TestDlg 對話方塊
CEtronDI_TestDlg::CEtronDI_TestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEtronDI_TestDlg::IDD, pParent)
{

	///////////////////////////////////////////////////////////////////////// 
	///////////////////////////////////////////////////////////////////////// 
	WNDCLASS wc;

	// Get the info for this class.
	// #32770 is the default class name for dialogs boxes.
	::GetClassInfo(AfxGetInstanceHandle(), L"#32770", &wc);

	// Change the name of the class.
	//wc.lpszClassName = "EtronDITestDlg";
	CString strClassName;
	strClassName.LoadString(IDS_ERTON_DI_CLASS_NAME);
	wc.lpszClassName = strClassName;

	// Register this class so that MFC can use it.
	AfxRegisterClass(&wc);
	///////////////////////////////////////////////////////////////////////// 
	///////////////////////////////////////////////////////////////////////// 


	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
  
  // initial video device dialog array
  for(int i=0;i<200;i++) {
    m_pVideoDevDlg[i] = NULL;
  }
  IMUTestDlg::InitIMU();
}

CEtronDI_TestDlg::~CEtronDI_TestDlg()
{
  for(size_t i=0 ; i<m_devInfo.size() ; i++ ) {
    
    if(m_pVideoDevDlg[i] != NULL) {
    
      delete m_pVideoDevDlg[i];
      m_pVideoDevDlg[i] = NULL;
    }
  }
  IMUTestDlg::UninitIMU();
}

void CEtronDI_TestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DEVCOMBO, m_Device_Ctrl);
}

BEGIN_MESSAGE_MAP(CEtronDI_TestDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(ID_OPEN_DEVICE, &CEtronDI_TestDlg::OnBnClickedOpenDevice)
    ON_MESSAGE(WM_MSG_CLOSE_VIDEO_DEVICE_DLG, &CEtronDI_TestDlg::OnCloseVideoDeviceDlg)
    ON_WM_CLOSE()
	ON_MESSAGE(WM_MSG_SNAPSHOT_ALL, OnShapShotAll)
END_MESSAGE_MAP()

LRESULT CEtronDI_TestDlg::OnShapShotAll(WPARAM wParam, LPARAM lParam)
{

	/*int devIndex = m_Device_Ctrl.GetCurSel();
	if (m_pVideoDevDlg[devIndex] != NULL)
	{
		m_pVideoDevDlg[devIndex]->DoSnapShot();
	}*/

	for(int i = 0; i < m_Device_Ctrl.GetCount(); i++)
	if (m_pVideoDevDlg[i] != NULL)
	{
		m_pVideoDevDlg[i]->DoSnapShot();
	}

	return 0;
}

LRESULT CEtronDI_TestDlg::OnCloseVideoDeviceDlg(WPARAM wParam, LPARAM lParam)
{
    CVideoDeviceDlg* pDlg = (CVideoDeviceDlg*)wParam;
    for(size_t i = 0; i < m_devInfo.size(); ++i) 
    {
        if(m_pVideoDevDlg[i] == pDlg) 
        {
            delete m_pVideoDevDlg[i];
            m_pVideoDevDlg[i] = NULL;
			m_Device_Ctrl.DeleteString(i);
			m_Device_Ctrl.InsertString(i, CString(m_devInfo[i].strDevName));
			m_Device_Ctrl.SetCurSel(i);
            break;
        }
    }

    return 0;
}
// CEtronDI_TestDlg 訊息處理常式

BOOL CEtronDI_TestDlg::OnInitDialog() {

  CDialog::OnInitDialog();
  
  // 將 [關於...] 功能表加入系統功能表。
  
  // IDM_ABOUTBOX 必須在系統命令範圍之中。
  ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
  ASSERT(IDM_ABOUTBOX < 0xF000);
  
  CMenu* pSysMenu = GetSystemMenu(FALSE);
  if (pSysMenu != NULL)
  {
  	BOOL bNameValid;
  	CString strAboutMenu;
  	bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
  	ASSERT(bNameValid);
  	if (!strAboutMenu.IsEmpty())
  	{
  		pSysMenu->AppendMenu(MF_SEPARATOR);
  		pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
  	}
  }
  
  // 設定此對話方塊的圖示。當應用程式的主視窗不是對話方塊時，
  // 框架會自動從事此作業
  SetIcon(m_hIcon, TRUE);			// 設定大圖示
  SetIcon(m_hIcon, FALSE);		// 設定小圖示
  
  // Set window title from .rc
  CString str;

  str.Format( L"Windows SDK %s", GetProductVersion() );

  SetWindowText(str);
  if (!GetEtronDIDevice(m_devInfo))
  {
      AfxMessageBox(L"EtronDI_Init failed ..");
      EndDialog(0);  
      return FALSE;
  }

  m_Device_Ctrl.ResetContent();
  for (size_t i = 0; i < m_devInfo.size(); ++i)
  {
      m_Device_Ctrl.AddString(CString(m_devInfo[i].strDevName));
  }
  m_Device_Ctrl.SetCurSel(0);

  // to set "enable sdk log", application can't open device automatically when there is only one device.
  /*
  if (m_devInfo.size() == 1)
  {
      PostMessage(WM_COMMAND, MAKELONG(ID_OPEN_DEVICE, BN_CLICKED), (LPARAM)GetDlgItem(ID_OPEN_DEVICE)->GetSafeHwnd());
  }
  */

  //InitAutoModuleSync();



  int nDeviceCount = m_Device_Ctrl.GetCount();
  if ( 1 == nDeviceCount)
  {
      OnBnClickedOpenDevice();

      m_pVideoDevDlg[ 0 ]->PostMessage( WM_MSG_AUTO_PREVIEW );
  }
  else
  {
	  if (AutoModuleSyncManager::GetInstance()->IsAutoModuleSyncEnabled())
	  {
		  for (DEVSELINFO devSelInfo : AutoModuleSyncManager::GetInstance()->GetModuleSyncDeviceVector())
		  {
			  OpenDevice(devSelInfo.index);
		  }			  
	  }
  }

  return TRUE;  
}

void CEtronDI_TestDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}



void CEtronDI_TestDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); 

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}


HCURSOR CEtronDI_TestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CEtronDI_TestDlg::OnBnClickedOpenDevice() {

    int devIndex = m_Device_Ctrl.GetCurSel();
	OpenDevice(devIndex);
}

void CEtronDI_TestDlg::OpenDevice(int devIndex)
{
	if (m_pVideoDevDlg[devIndex] == NULL)
    {
        CString str;

        str.Format( L"Windows SDK %s", GetProductVersion() );

		CString editString( m_devInfo[devIndex].strDevName );
		editString.Append(L" (Opened)");
		m_Device_Ctrl.DeleteString(devIndex);
		m_Device_Ctrl.InsertString(devIndex, editString);
		m_Device_Ctrl.SetCurSel(devIndex);

        bool enableSDKLog = ((CButton*)GetDlgItem(IDC_CHECK_ENABLE_SDK_LOG))->GetCheck() == BST_CHECKED;
        m_pVideoDevDlg[devIndex] = new CVideoDeviceDlg(nullptr, enableSDKLog);
        m_pVideoDevDlg[devIndex]->SetParams(devIndex, m_devInfo[devIndex]);
        m_pVideoDevDlg[devIndex]->Create (m_pVideoDevDlg[devIndex]->IDD, this);

        m_pVideoDevDlg[devIndex]->SetWindowText( str );
        m_pVideoDevDlg[devIndex]->ShowWindow (SW_SHOW);
    }
}

void CEtronDI_TestDlg::OnOK()
{
    OnCancel();
}

// in modal dialog, OnCancel will be called after calling OnClose, 
// thus put uninit codes in OnCancel, and both method will run the uninit codes.
// in modaless dialog, OnCancel won't be called after calling OnCLose, 
// thus put uninit codes in one method, and in another method manually call the method.
void CEtronDI_TestDlg::OnCancel()
{
    for (size_t i = 0; i < m_devInfo.size(); ++i)
    {
        if (m_pVideoDevDlg[i] != NULL)
        {
            m_pVideoDevDlg[i]->SendMessage(WM_CLOSE);
            delete m_pVideoDevDlg[i];
            m_pVideoDevDlg[i] = NULL;
        }
    }

    CDialog::OnCancel();
}
