
#include "stdafx.h"
#include "EtronDI_Test.h"
#include "WaitDlg.h"

IMPLEMENT_DYNAMIC( WaitDlg, CDialogEx )
BEGIN_MESSAGE_MAP( WaitDlg, CDialogEx )
    ON_BN_CLICKED( IDOK,     &WaitDlg::OnBnClickedOk     )
    ON_BN_CLICKED( IDCANCEL, &WaitDlg::OnBnClickedCancel )
END_MESSAGE_MAP()

WaitDlg::WaitDlg( CWnd* pParent ) : CDialogEx( IDD_DLG_WAIT, pParent )
{
}

WaitDlg::~WaitDlg()
{
}

void WaitDlg::DoDataExchange( CDataExchange* pDX )
{
	CDialogEx::DoDataExchange( pDX );
}

BOOL WaitDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    GetDlgItem( IDC_ST_TEXT )->SetWindowText( m_csText );

    return TRUE;
}
