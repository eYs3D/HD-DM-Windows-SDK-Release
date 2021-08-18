#pragma once
#ifdef ESPDI_EG
#include "eSPDI_EG.h"
#else
#include "eSPDI_DM.h"
#endif
#include "afxwin.h"

// AutoModuelSyncDlg dialog

class AutoModuelSyncDlg : public CDialog
{
public:
	AutoModuelSyncDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~AutoModuelSyncDlg();

	enum { IDD = IDD_AUTO_MODULE_SYNC_DIALOG };

public:
	virtual BOOL OnInitDialog();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	afx_msg void OnBnClickedSelectMaster();

	DECLARE_MESSAGE_MAP()	
private:
	bool InitAutoModuleSync();
private:
	CComboBox m_Device_Ctrl;
};
