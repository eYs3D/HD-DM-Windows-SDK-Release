// AudioDlg.cpp: 實作檔案
//

#include "stdafx.h"
#include "EtronDI_Test.h"
#include "AudioDlg.h"
#include "afxdialogex.h"
#include "../utility/audioCapture.h"


// AudioDlg 對話方塊

IMPLEMENT_DYNAMIC(AudioDlg, CDialogEx)

AudioDlg::AudioDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_AUDIO_DLG, pParent)
{
	m_AudioRange = 10;
}

AudioDlg::~AudioDlg()
{
}

void AudioDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(AudioDlg, CDialogEx)
	ON_BN_CLICKED(IDC_AUDIO_BTN, &AudioDlg::OnBnClickedAudioBtn)
	ON_EN_CHANGE(IDC_AUDIO_RANGE, &AudioDlg::OnEnChangeAudioRange)
END_MESSAGE_MAP()


// AudioDlg 訊息處理常式


void AudioDlg::OnBnClickedAudioBtn()
{
	int result = 0;
	IMMDevice *device = NULL;
	bool isDefaultDevice;
    ERole role;
	int TargetLatency = 20;
	int TargetDurationInSec = m_AudioRange;
	CWnd *pEdit = GetDlgItem(IDC_AUDIO_BTN);
	ASSERT_VALID(pEdit);
	pEdit->EnableWindow(false);
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
	if (!audioCapture::PickDevice(&device, &isDefaultDevice, &role)) {
		SetDlgItemText(IDC_AudioStatus, L"Device not Ready");
		result = -1;
		goto End;
	}


	audioCapture *capturer = new (std::nothrow) audioCapture(device, isDefaultDevice, role);
	if (capturer == NULL) {
		SetDlgItemText(IDC_AudioStatus, L"Unable to allocate capturer");
		goto End;
	}

	if (capturer->Initialize(TargetLatency)) {
		size_t captureBufferSize = capturer->SamplesPerSecond() * TargetDurationInSec * capturer->FrameSize();
		BYTE *captureBuffer = new (std::nothrow) BYTE[captureBufferSize];

		if (captureBuffer == NULL) {
			SetDlgItemText(IDC_AudioStatus, L"Unable to allocate capture buffer");
			goto End;
		}

		if (capturer->Start(captureBuffer, captureBufferSize)) {
			do {
				SetDlgItemText(IDC_AudioStatus, L"Audio Recording....");
				Sleep(1000);
			} while (--TargetDurationInSec);
			printf("\n");
			SetDlgItemText(IDC_AudioStatus, L"Audio Completed !!");
			capturer->Stop();

			//
			//  We've now captured our wave data.  Now write it out in a wave file.
			//
			capturer->SaveWaveData("Capture.wav", captureBuffer, capturer->BytesCaptured(), capturer->MixFormat());


			//
			//  Now shut down the capturer and release it we're done.
			//
			capturer->Shutdown();
			SafeRelease(&capturer);
		}

		delete[]captureBuffer;
	}

End:
	pEdit = GetDlgItem(IDC_AUDIO_BTN);
	ASSERT_VALID(pEdit);
	pEdit->EnableWindow(true);
	SafeRelease(&device);
	CoUninitialize();
}


void AudioDlg::OnEnChangeAudioRange()
{
	wchar_t buffer[128];
	GetDlgItemText(IDC_AUDIO_RANGE, buffer, sizeof(buffer));
	m_AudioRange = _wtoi(buffer);
}
