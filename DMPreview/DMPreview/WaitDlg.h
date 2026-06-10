#pragma once

class WaitDlg : public CDialogEx
{
	DECLARE_DYNAMIC( WaitDlg )
    DECLARE_MESSAGE_MAP()

public:

	WaitDlg( CWnd* pParent = nullptr );
	virtual ~WaitDlg();

    inline void SetText( const CString& csText ) { m_csText = csText; }

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_WAIT };
#endif

private:

    CString m_csText;

	virtual void DoDataExchange( CDataExchange* pDX );
    virtual BOOL OnInitDialog();

    afx_msg void OnBnClickedOk() {}
    afx_msg void OnBnClickedCancel() {}
};
