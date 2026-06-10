#include "stdafx.h"
#include "AudioPlayer.h"

#pragma comment(lib, "Winmm.lib")


template<class T>
inline void SafeRelease(T& pInstance)
{
    if (pInstance != nullptr)
    {
        pInstance->Release();
        pInstance = nullptr;
    }
}


CRawAudioBuffer::CRawAudioBuffer(const std::vector<unsigned char>& data)
    : buf(data)
{
    memset(&waveHdr, 0, sizeof(WAVEHDR));
    waveHdr.dwBufferLength = data.size();
    waveHdr.lpData = (char*)&buf[0];
}

CRawAudioBuffer::~CRawAudioBuffer()
{
}

WAVEHDR* CRawAudioBuffer::header()
{
    return &waveHdr;
}


CAudioPlayer::CAudioPlayer()
    : minVolume(0), maxVolume(100), 
    m_audioDeviceEnumerator(nullptr), m_audioDevice(nullptr), m_sessionMgr(nullptr), m_sessionEnumerator(nullptr), 
    m_sessionCtrl(nullptr), m_sessionCtrl2(nullptr), m_volumeCtrl(nullptr), m_volumeInitRetryThread(nullptr), m_startVolumeInitRetryThread(false)
{
    CoInitialize(nullptr);
    if (!InitVolumeCtrl())
    {// sometimes process info isn't registered in system yet, init volume control will fail, need to retry
        m_startVolumeInitRetryThread = true;
        m_volumeInitRetryThread = new std::thread(CAudioPlayer::VolumeInitRetryThread, this);
    }
    InitWaveOut(1, 16, 8000);
}

CAudioPlayer::~CAudioPlayer()
{
    UninitWaveOut();
    if (m_volumeInitRetryThread != nullptr)
    {
        m_startVolumeInitRetryThread = false;
        m_volumeInitRetryThread->join();
        delete m_volumeInitRetryThread;
    }
    UninitVolumeCtrl();
    CoUninitialize();
}

void CAudioPlayer::InitWaveOut(unsigned short channelNumber, unsigned short bitsPerSample, unsigned int samplesPerSec)
{
    waveFmt.wFormatTag = WAVE_FORMAT_PCM;
    waveFmt.nChannels = channelNumber;
    waveFmt.wBitsPerSample = bitsPerSample;
    waveFmt.nSamplesPerSec = samplesPerSec;
    waveFmt.nBlockAlign = (waveFmt.wBitsPerSample / 8) * waveFmt.nChannels;
    waveFmt.nAvgBytesPerSec = waveFmt.nBlockAlign * waveFmt.nSamplesPerSec;
    waveFmt.cbSize = 0;

    MMRESULT ret = waveOutOpen(&hWaveOut, WAVE_MAPPER, &waveFmt, 0, 0, CALLBACK_NULL);

    threadStart = true;
    playThread = new std::thread(CAudioPlayer::PlayAudioThread, this);
}

void CAudioPlayer::UninitWaveOut()
{
    if (playThread != nullptr)
    {
        threadStart = false;
        playThread->join();
        delete playThread;
        playThread = nullptr;
    }

    MMRESULT ret = waveOutClose(hWaveOut);
}

void CAudioPlayer::VolumeInitRetryThread(CAudioPlayer* pThis)
{
    while (pThis->m_startVolumeInitRetryThread)
    {
        if (pThis->InitVolumeCtrl())
        {
            break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(15));
    }
}

bool CAudioPlayer::InitVolumeCtrl()
{
    std::lock_guard<std::mutex> lock(m_volumeCtrlMutex);
    if (m_volumeCtrl != nullptr)
    {
        return true;
    }

    int sessionCount = 0;
    if (CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), 
            (void**)&m_audioDeviceEnumerator) == S_OK && 
        m_audioDeviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &m_audioDevice) == S_OK && 
        m_audioDevice->Activate(__uuidof(IAudioSessionManager2), CLSCTX_ALL, nullptr, (void**)&m_sessionMgr) == S_OK && 
        m_sessionMgr->GetSessionEnumerator(&m_sessionEnumerator) == S_OK && 
        m_sessionEnumerator->GetCount(&sessionCount) == S_OK)
    {
        for (int i = 0; i < sessionCount; ++i)
        {
            IAudioSessionControl* sessionCtrl = nullptr;
            if (m_sessionEnumerator->GetSession(i, &sessionCtrl) == S_OK)
            {
                IAudioSessionControl2* sessionCtrl2 = nullptr;
                if (sessionCtrl->QueryInterface(__uuidof(IAudioSessionControl2), (void**)&sessionCtrl2) == S_OK)
                {
                    DWORD processId = 0;
                    if (sessionCtrl2->GetProcessId(&processId) == S_OK &&
                        processId == GetCurrentProcessId())
                    {
                        ISimpleAudioVolume* volumeCtrl = nullptr;
                        if (sessionCtrl2->QueryInterface(__uuidof(ISimpleAudioVolume), (void**)&volumeCtrl) == S_OK)
                        {
                            m_sessionCtrl = sessionCtrl;
                            m_sessionCtrl2 = sessionCtrl2;
                            m_volumeCtrl = volumeCtrl;
                            return true;
                        }

                        SafeRelease(volumeCtrl);
                    }
                }

                SafeRelease(sessionCtrl2);
            }

            SafeRelease(sessionCtrl);
        }
    }

    return false;
}

void CAudioPlayer::UninitVolumeCtrl()
{
    std::lock_guard<std::mutex> lock(m_volumeCtrlMutex);
    SafeRelease(m_volumeCtrl);
    SafeRelease(m_sessionCtrl2);
    SafeRelease(m_sessionCtrl);
    SafeRelease(m_sessionEnumerator);
    SafeRelease(m_sessionMgr);
    SafeRelease(m_audioDevice);
    SafeRelease(m_audioDeviceEnumerator);
}

void CAudioPlayer::SetAudioInfo(unsigned short channelNumber, unsigned short bitsPerSample, unsigned int samplesPerSec)
{
    UninitWaveOut();
    InitWaveOut(channelNumber, bitsPerSample, samplesPerSec);
}

void CAudioPlayer::DataIn(const std::vector<unsigned char>& data)
{
    std::lock_guard<std::mutex> lock(mutex);
    rawAudioData.push(new CRawAudioBuffer(data));
}

void CAudioPlayer::Clear()
{
    std::lock_guard<std::mutex> lock(mutex);
    while (!rawAudioData.empty())
    {
        delete rawAudioData.front();
        rawAudioData.pop();
    }
}

void CAudioPlayer::GetVolumeRange(int& min, int& max) const
{
    min = minVolume;
    max = maxVolume;
}

int CAudioPlayer::GetVolume()
{
    std::lock_guard<std::mutex> lock(m_volumeCtrlMutex);
    float value = 0.0;
    if (m_volumeCtrl != nullptr)
    {
        m_volumeCtrl->GetMasterVolume(&value);
    }
    
    return minVolume + (int)(value * (float)(maxVolume - minVolume));
}

void CAudioPlayer::SetVolume(int value)
{
    std::lock_guard<std::mutex> lock(m_volumeCtrlMutex);
    if (m_volumeCtrl != nullptr)
    {
        float fValue = (float)(value - minVolume) / (float)(maxVolume - minVolume);
        m_volumeCtrl->SetMasterVolume(fValue, nullptr);
    }
}

bool CAudioPlayer::IsMute()
{
    std::lock_guard<std::mutex> lock(m_volumeCtrlMutex);
    if (m_volumeCtrl != nullptr)
    {
        BOOL mute = FALSE;
        m_volumeCtrl->GetMute(&mute);
        return (mute == TRUE);
    }

    return false;
}

void CAudioPlayer::SetMute(bool mute)
{
    std::lock_guard<std::mutex> lock(m_volumeCtrlMutex);
    if (m_volumeCtrl != nullptr)
    {
        m_volumeCtrl->SetMute(mute ? TRUE : FALSE, nullptr);
    }
}

void CAudioPlayer::PlayAudioThread(CAudioPlayer* pThis)
{
    std::queue<CRawAudioBuffer*> rawAudioQueue;

    while (pThis->threadStart)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        CRawAudioBuffer* rawAudio = nullptr;
        {
            std::lock_guard<std::mutex> lock(pThis->mutex);
            if (pThis->rawAudioData.empty())
            {
                continue;
            }

            rawAudio = pThis->rawAudioData.front();
            pThis->rawAudioData.pop();
        }

        if (rawAudio != nullptr)
        {
            if (waveOutPrepareHeader(pThis->hWaveOut, rawAudio->header(), sizeof(WAVEHDR)) == MMSYSERR_NOERROR)
            {
                MMRESULT ret = waveOutWrite(pThis->hWaveOut, rawAudio->header(), sizeof(WAVEHDR));
                rawAudioQueue.push(rawAudio);
            }
            else
            {
                delete rawAudio;
            }
            rawAudio = nullptr;
        }

        while (!rawAudioQueue.empty() &&
            waveOutUnprepareHeader(pThis->hWaveOut, rawAudioQueue.front()->header(), sizeof(WAVEHDR)) != WAVERR_STILLPLAYING)
        {
            delete rawAudioQueue.front();
            rawAudioQueue.pop();
        }
    }

    waveOutReset(pThis->hWaveOut);

    while (!rawAudioQueue.empty() &&
        waveOutUnprepareHeader(pThis->hWaveOut, rawAudioQueue.front()->header(), sizeof(WAVEHDR)) != WAVERR_STILLPLAYING)
    {
        delete rawAudioQueue.front();
        rawAudioQueue.pop();
    }
}