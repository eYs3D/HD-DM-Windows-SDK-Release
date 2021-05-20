#pragma once
#include <mmsystem.h>
#include <mmdeviceapi.h>
#include <Audiopolicy.h>
#include <vector>
#include <queue>
#include <mutex>
#include <thread>


class CRawAudioBuffer
{
public:
    CRawAudioBuffer(const std::vector<unsigned char>& data);
    ~CRawAudioBuffer();

    WAVEHDR* header();

private:
    WAVEHDR waveHdr;
    std::vector<unsigned char> buf;
};

class CAudioPlayer
{
public:
    CAudioPlayer();
    ~CAudioPlayer();

    void SetAudioInfo(unsigned short channelNumber, unsigned short bitsPerSample, unsigned int samplesPerSec);
    void DataIn(const std::vector<unsigned char>& data);
    void Clear();
    void GetVolumeRange(int& min, int& max) const;
    int GetVolume();
    void SetVolume(int value);
    bool IsMute();
    void SetMute(bool mute);

private:
    std::queue<CRawAudioBuffer*> rawAudioData;
    std::mutex mutex;
    std::thread* playThread;
    bool threadStart;
    WAVEFORMATEX waveFmt;
    HWAVEOUT hWaveOut;
    const int minVolume;
    const int maxVolume;
    IMMDeviceEnumerator* m_audioDeviceEnumerator;
    IMMDevice* m_audioDevice;
    IAudioSessionManager2* m_sessionMgr;
    IAudioSessionEnumerator* m_sessionEnumerator;
    IAudioSessionControl* m_sessionCtrl;
    IAudioSessionControl2* m_sessionCtrl2;
    ISimpleAudioVolume* m_volumeCtrl;
    std::mutex m_volumeCtrlMutex;
    std::thread* m_volumeInitRetryThread;
    bool m_startVolumeInitRetryThread;

private:
    static void PlayAudioThread(CAudioPlayer* pThis);
    void InitWaveOut(unsigned short channelNumber, unsigned short bitsPerSample, unsigned int samplesPerSec);
    void UninitWaveOut();
    bool InitVolumeCtrl();
    void UninitVolumeCtrl();
    static void VolumeInitRetryThread(CAudioPlayer* pThis);
};