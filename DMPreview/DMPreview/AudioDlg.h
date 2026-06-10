#pragma once


// AudioDlg 對話方塊

class AudioDlg : public CDialogEx
{
	DECLARE_DYNAMIC(AudioDlg)

public:
	AudioDlg(CWnd* pParent = nullptr);   // 標準建構函式
	virtual ~AudioDlg();

	// Dialog Data
	enum { IDD = IDD_AUDIO_DLG };


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援

	DECLARE_MESSAGE_MAP()
private:
	int m_AudioRange;
public:
	afx_msg void OnBnClickedAudioBtn();
	afx_msg void OnEnChangeAudioRange();
};
