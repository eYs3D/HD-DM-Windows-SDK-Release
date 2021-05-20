// AutoModuelSyncDlg.cpp : implementation file
//
#include "stdafx.h"
#include "EtronDI_Test.h"
#include "AutoModuelSyncDlg.h"
#include "AutoModuleSyncManager.h"


// AutoModuelSyncDlg dialog

AutoModuelSyncDlg::AutoModuelSyncDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(AutoModuelSyncDlg::IDD, pParent)
{	

}

AutoModuelSyncDlg::~AutoModuelSyncDlg()
{
}

void AutoModuelSyncDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DEVCOMBO, m_Device_Ctrl);
}

BEGIN_MESSAGE_MAP(AutoModuelSyncDlg, CDialog)
	ON_BN_CLICKED(ID_SELECT_MASTER, &AutoModuelSyncDlg::OnBnClickedSelectMaster)
END_MESSAGE_MAP()
// AutoModuelSyncDlg message handlers

BOOL AutoModuelSyncDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	if (!InitAutoModuleSync()) EndDialog(IDCANCEL);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

bool AutoModuelSyncDlg::InitAutoModuleSync()
{
	if (!AutoModuleSyncManager::GetInstance()->IsModuleSyncAvailable()) return false;

	if ((IDOK != MessageBox(L"Do you need to run the function 'Multi-Module Sync'", L"Notice", MB_OKCANCEL))) return false;

	for (DEVSELINFO devSelInfo : AutoModuleSyncManager::GetInstance()->GetModuleSyncDeviceVector())
	{
		if (AutoModuleSyncManager::GetInstance()->IsMasterDevice(devSelInfo))
		{
			CString message;
			message.Format(_T("%s is the serial number of Master Device ?"), AutoModuleSyncManager::GetInstance()->GetSerialNumber(devSelInfo));

			if (IDOK == MessageBox(message, L"Notice", MB_OKCANCEL))
			{
				AutoModuleSyncManager::GetInstance()->EnableAutoModuleSync(true);
				return false;
			}
		}
	}

	m_Device_Ctrl.ResetContent();	
	for (DEVSELINFO devSelInfo : AutoModuleSyncManager::GetInstance()->GetModuleSyncDeviceVector())
	{
		m_Device_Ctrl.AddString(CString(AutoModuleSyncManager::GetInstance()->GetSerialNumber(devSelInfo)));
	}
	m_Device_Ctrl.SetCurSel(0);

	return true;
}


void AutoModuelSyncDlg::OnBnClickedSelectMaster()
{
	// TODO: Add your control notification handler code here
	AutoModuleSyncManager::GetInstance()->SetMasterDevice(AutoModuleSyncManager::GetInstance()->GetModuleSyncDeviceVector()[m_Device_Ctrl.GetCurSel()]);
	AutoModuleSyncManager::GetInstance()->EnableAutoModuleSync(true);
	EndDialog(IDOK);
}
