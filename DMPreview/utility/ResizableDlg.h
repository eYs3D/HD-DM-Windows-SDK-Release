#pragma once


// CResizableDlg dialog

class CResizableDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CResizableDlg)

public:
	CResizableDlg(UINT nIDTemplate, CWnd* pParent = NULL);
	virtual ~CResizableDlg();

// Dialog Data

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog();

    // client window size will be (fixedWidth + variableWidth, fixedHeight + variableHeight).
    // variableWidth and variableHeight will be fixed ratio.
    void SetResizableInfo(int maxVariableWidth, int maxVariableHeight, int minVariableWidth,
        int fixedWidth = 0, int fixedHeight = 0);
    void MaximizeWindow();
    bool GetMaxWindowRect(CRect& wndRect);

private:
    int m_minVariableWidth;
    double m_variableRatio;
    SIZE m_maxVariableSize;
    SIZE m_fixedSize;
    int m_widthOffset;
    int m_heightOffset;

	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};
