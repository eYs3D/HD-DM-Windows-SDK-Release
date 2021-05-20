#include "stdafx.h"
#include "audioCapture.h"
#include <functiondiscoverykeys.h>
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <atlstr.h>
#include <iostream>
#define UNICODE
#include <Strsafe.h> 

int TargetLatency = 20;
int TargetDurationInSec = 10;
bool ShowHelp;
bool UseConsoleDevice;
bool UseCommunicationsDevice;
bool UseMultimediaDevice;
bool DisableMMCSS;

FILE *fdebugAudio;
#define DEBUG_ON	false

extern int g_Recording; // 0 : stop, 1: start
extern int g_fb_live_stream; // 0 : stop, 1: start
extern int g_yt_live_stream; // 0 : stop, 1: start

extern DWORD g_first_Audio_tick_count;

char tmp_buffer[512];

void audioCapture::debugLog(const char *format, ...)
{
	va_list arg;

	if (DEBUG_ON) {
		va_start(arg, format);
		vfprintf(fdebugAudio, format, arg);
		va_end(arg);
		fflush(fdebugAudio);
	}
}

audioCapture::audioCapture(IMMDevice *Endpoint, bool EnableStreamSwitch, ERole EndpointRole) :
	_RefCount(1),
	_Endpoint(Endpoint),
	_AudioClient(NULL),
	_CaptureClient(NULL),
	_CaptureThread(NULL),
	_EncodeThread(NULL),
	_ShutdownEvent(NULL),
	_MixFormat(NULL),
	_AudioSamplesReadyEvent(NULL),
	_CurrentCaptureIndex(0),
	_EnableStreamSwitch(EnableStreamSwitch),
	_EndpointRole(EndpointRole),
	_StreamSwitchEvent(NULL),
	_StreamSwitchCompleteEvent(NULL),
	_AudioSessionControl(NULL),
	_DeviceEnumerator(NULL),
	_InStreamSwitch(false)
{
	int err;
	_Endpoint->AddRef();    // Since we're holding a copy of the endpoint, take a reference to it.  It'll be released in Shutdown();
	if (DEBUG_ON) {
		err = fopen_s(&fdebugAudio, "AudioLog.txt", "wt");
		if (err != 0) AfxMessageBox(_T(" fopen_s failed !!"));
	}
}

//
//  Initialize WASAPI in event driven mode, associate the audio client with our samples ready event handle, retrieve 
//  a capture client for the transport, create the capture thread and start the audio engine.
//
bool audioCapture::InitializeAudioEngine()
{
	HRESULT hr = _AudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, AUDCLNT_STREAMFLAGS_EVENTCALLBACK | AUDCLNT_STREAMFLAGS_NOPERSIST, _EngineLatencyInMS * 10000, 0, _MixFormat, NULL);

	if (FAILED(hr)) {
		debugLog("Unable to initialize audio client: %x.\n", hr);
		return false;
	}

	//
	//  Retrieve the buffer size for the audio client.
	//
	hr = _AudioClient->GetBufferSize(&_BufferSize);
	if (FAILED(hr)) {
		debugLog("Unable to get audio client buffer: %x. \n", hr);
		return false;
	}

	hr = _AudioClient->SetEventHandle(_AudioSamplesReadyEvent);
	if (FAILED(hr)) {
		debugLog("Unable to set ready event: %x.\n", hr);
		return false;
	}

	hr = _AudioClient->GetService(IID_PPV_ARGS(&_CaptureClient));
	if (FAILED(hr)) {
		debugLog("Unable to get new capture client: %x.\n", hr);
		return false;
	}

	return true;
}

//
//  Retrieve the format we'll use to capture samples.
//
//  We use the Mix format since we're capturing in shared mode.
//
bool audioCapture::LoadFormat()
{
	HRESULT hr = _AudioClient->GetMixFormat(&_MixFormat);
	if (FAILED(hr)) {
		debugLog("Unable to get mix format on audio client: %x.\n", hr);
		return false;
	}
	_FrameSize = (_MixFormat->wBitsPerSample / 8) * _MixFormat->nChannels;
	return true;
}

//
//  Initialize the capturer.
//
bool audioCapture::Initialize(UINT32 EngineLatency)
{
	//
	//  Create our shutdown and samples ready events- we want auto reset events that start in the not-signaled state.
	//
	_ShutdownEvent = CreateEventEx(NULL, NULL, 0, EVENT_MODIFY_STATE | SYNCHRONIZE);
	if (_ShutdownEvent == NULL) {
		debugLog("Unable to create shutdown event: %d.\n", GetLastError());
		return false;
	}

	_AudioSamplesReadyEvent = CreateEventEx(NULL, NULL, 0, EVENT_MODIFY_STATE | SYNCHRONIZE);
	if (_AudioSamplesReadyEvent == NULL) {
		debugLog("Unable to create samples ready event: %d.\n", GetLastError());
		return false;
	}

	//
	//  Create our stream switch event- we want auto reset events that start in the not-signaled state.
	//  Note that we create this event even if we're not going to stream switch - that's because the event is used
	//  in the main loop of the capturer and thus it has to be set.
	//
	_StreamSwitchEvent = CreateEventEx(NULL, NULL, 0, EVENT_MODIFY_STATE | SYNCHRONIZE);
	if (_StreamSwitchEvent == NULL) {
		debugLog("Unable to create stream switch event: %d.\n", GetLastError());
		return false;
	}

	//
	//  Now activate an IAudioClient object on our preferred endpoint and retrieve the mix format for that endpoint.
	//
	HRESULT hr = _Endpoint->Activate(__uuidof(IAudioClient), CLSCTX_INPROC_SERVER, NULL, reinterpret_cast<void **>(&_AudioClient));
	if (FAILED(hr)) {
		debugLog("Unable to activate audio client: %x.\n", hr);
		return false;
	}

	hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&_DeviceEnumerator));
	if (FAILED(hr)) {
		debugLog("Unable to instantiate device enumerator: %x\n", hr);
		return false;
	}

	//
	// Load the MixFormat.  This may differ depending on the shared mode used
	//
	if (!LoadFormat()) {
		debugLog("Failed to load the mix format \n");
		return false;
	}

	//
	//  Remember our configured latency in case we'll need it for a stream switch later.
	//
	_EngineLatencyInMS = EngineLatency;

	if (!InitializeAudioEngine()) {
		return false;
	}

	if (_EnableStreamSwitch) {
		if (!InitializeStreamSwitch()) {
			return false;
		}
	}

	return true;
}

//
//  Shut down the capture code and free all the resources.
//
void audioCapture::Shutdown()
{

	stillPlaying = false;

//	delete pcmBuffer;
//	delete m_aacEncode;



	if (_CaptureThread) {
		SetEvent(_ShutdownEvent);
		WaitForSingleObject(_CaptureThread, INFINITE);
		CloseHandle(_CaptureThread);
		_CaptureThread = NULL;
	}

	if (_ShutdownEvent) {
		CloseHandle(_ShutdownEvent);
		_ShutdownEvent = NULL;
	}
	if (_AudioSamplesReadyEvent) {
		CloseHandle(_AudioSamplesReadyEvent);
		_AudioSamplesReadyEvent = NULL;
	}
	if (_StreamSwitchEvent) {
		CloseHandle(_StreamSwitchEvent);
		_StreamSwitchEvent = NULL;
	}

	//evan SafeRelease(&_Endpoint);
	//evan SafeRelease(&_AudioClient);
	//evan SafeRelease(&_CaptureClient);

	if (_MixFormat) {
		CoTaskMemFree(_MixFormat);
		_MixFormat = NULL;
	}

	if (_EnableStreamSwitch) {
		TerminateStreamSwitch();
	}
}


//
//  Start capturing...
//
bool audioCapture::Start(BYTE *CaptureBuffer, size_t CaptureBufferSize)
{
	HRESULT hr;
	_CaptureBuffer = CaptureBuffer;
	_CaptureBufferSize = CaptureBufferSize;

	//
	//  Now create the thread which is going to drive the capture.
	//
	_CaptureThread = CreateThread(NULL, 0, AudioCaptureThread, this, 0, NULL);
	if (_CaptureThread == NULL) {
		debugLog("Unable to create transport thread: %x.", GetLastError());
		return false;
	}
	
//	Sleep(8000); // evan for Audio/Video Sync 2018/8/6
	
	//
	//  We're ready to go, start capturing!
	//
	hr = _AudioClient->Start();
	if (FAILED(hr)) {
		debugLog("Unable to start capture client: %x.\n", hr);
		return false;
	}



	return true;
}

//
//  Stop the capturer.
//
void audioCapture::Stop()
{
	HRESULT hr;

	stillPlaying = false;


	//
	//  Tell the capture thread to shut down, wait for the thread to complete then clean up all the stuff we 
	//  allocated in Start().
	//
	if (_ShutdownEvent) {
		SetEvent(_ShutdownEvent);
	}

	hr = _AudioClient->Stop();
	if (FAILED(hr)) {
		debugLog("Unable to stop audio client: %x\n", hr);
	}

	if (_CaptureThread) {
		WaitForSingleObject(_CaptureThread, INFINITE);

		CloseHandle(_CaptureThread);
		_CaptureThread = NULL;
	}

	if (_EncodeThread) {
		WaitForSingleObject(_EncodeThread, INFINITE);

		CloseHandle(_EncodeThread);
		_EncodeThread = NULL;
	}
}


//
//  Capture thread - processes samples from the audio engine
//
DWORD audioCapture::AudioCaptureThread(LPVOID Context)
{
	audioCapture *capturer = static_cast<audioCapture *>(Context);
	return capturer->DoAudioCaptureThread();
}


void audioCapture::dump(unsigned char *in, int len)
{
	int i, count = 1;
	for (i = 0; i < len; i++) {
		debugLog("%02x ", in[i]);
		if (count % 16 == 0) {
			debugLog("\n");
			count = 0;
		}
		count++;
	}
	debugLog("\n");

}

LPWSTR audioCapture::GetDeviceName(IMMDeviceCollection *DeviceCollection, UINT DeviceIndex)
{
	IMMDevice *device;
	LPWSTR deviceId;
	HRESULT hr;

	hr = DeviceCollection->Item(DeviceIndex, &device);
	if (FAILED(hr))
	{
		printf("Unable to get device %d: %x\n", DeviceIndex, hr);
		return NULL;
	}
	hr = device->GetId(&deviceId);
	if (FAILED(hr))
	{
		printf("Unable to get device %d id: %x\n", DeviceIndex, hr);
		return NULL;
	}

	IPropertyStore *propertyStore;
	hr = device->OpenPropertyStore(STGM_READ, &propertyStore);
	SafeRelease(&device);
	if (FAILED(hr))
	{
		printf("Unable to open device %d property store: %x\n", DeviceIndex, hr);
		return NULL;
	}

	PROPVARIANT friendlyName;
	PropVariantInit(&friendlyName);
	hr = propertyStore->GetValue(PKEY_Device_FriendlyName, &friendlyName);
	SafeRelease(&propertyStore);

	if (FAILED(hr))
	{
		printf("Unable to retrieve friendly name for device %d : %x\n", DeviceIndex, hr);
		return NULL;
	}

	wchar_t deviceName[128];

	hr = StringCbPrintf(deviceName, sizeof(deviceName), L"%s (%s)", friendlyName.vt != VT_LPWSTR ? L"Unknown" : friendlyName.pwszVal, deviceId);
	if (FAILED(hr))
	{
		printf("Unable to format friendly name for device %d : %x\n", DeviceIndex, hr);
		return NULL;
	}

	PropVariantClear(&friendlyName);
	CoTaskMemFree(deviceId);

	wchar_t *returnValue = _wcsdup(deviceName);
	if (returnValue == NULL)
	{
		printf("Unable to allocate buffer for return\n");
		return NULL;
	}
	return returnValue;
}

bool audioCapture::PickDevice(IMMDevice **DeviceToUse, bool *IsDefaultDevice, ERole *DefaultDeviceRole)
{
	HRESULT hr;
	bool retValue = true;
	IMMDeviceEnumerator *deviceEnumerator = NULL;
	IMMDeviceCollection *deviceCollection = NULL;
	*IsDefaultDevice = false;  // Assume we're not using the default device.
	hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&deviceEnumerator));
	if (FAILED(hr))
	{
		printf("Unable to instantiate device enumerator: %x\n", hr);
		retValue = false;
		goto Exit;
	}

	IMMDevice *device = NULL;


	//
	//  The user didn't specify an output device, prompt the user for a device and use that.
	//
	hr = deviceEnumerator->EnumAudioEndpoints(eCapture, DEVICE_STATE_ACTIVE, &deviceCollection);
	if (FAILED(hr))
	{
		printf("Unable to retrieve device collection: %x\n", hr);
		retValue = false;
		goto Exit;
	}


	UINT deviceCount;
	hr = deviceCollection->GetCount(&deviceCount);
	if (FAILED(hr))
	{
		printf("Unable to get device collection length: %x\n", hr);
		retValue = false;
		goto Exit;
	}
	UINT i;
	bool bDevice_Status = false;
	for (i = 0; i < deviceCount; i += 1)
	{
		LPWSTR deviceName = NULL;
		bDevice_Status = false;
		deviceName = GetDeviceName(deviceCollection, i);
		if (deviceName == NULL)
		{
			retValue = false;
			goto Exit;
		}

		//wstring str(deviceName);

		CString MyString = deviceName;
		if (MyString.Find(L"EX8040SA") >  0 || MyString.Find(L"FrogEye2_Audio") > 0 || MyString.Find(L"FrogEye2") > 0) {
			printf("Device ready\n");
			bDevice_Status = true;
			break;
		}
		free(deviceName);
	}

	if (bDevice_Status == false) {
		printf("Audio Device not ready\n");
		retValue = false;
		goto Exit;
	}

	hr = deviceCollection->Item(i, &device);
	if (FAILED(hr))
	{
		printf("Unable to retrieve device %d: %x\n", i, hr);
		retValue = false;
		goto Exit;
	}
	
	*DeviceToUse = device;
	retValue = true;
Exit:
	SafeRelease(&deviceCollection);
	SafeRelease(&deviceEnumerator);

	return retValue;
}

DWORD audioCapture::DoAudioCaptureThread()
{
	bool stillPlaying = true;
	HANDLE waitArray[3] = { _ShutdownEvent, _StreamSwitchEvent, _AudioSamplesReadyEvent };
	HANDLE mmcssHandle = NULL;
	DWORD mmcssTaskIndex = 0;

	HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	if (FAILED(hr))
	{
		printf("Unable to initialize COM in render thread: %x\n", hr);
		return hr;
	}

	if (!DisableMMCSS)
	{
		mmcssHandle = AvSetMmThreadCharacteristics(L"Audio", &mmcssTaskIndex);
		if (mmcssHandle == NULL)
		{
			printf("Unable to enable MMCSS on capture thread: %d\n", GetLastError());
		}
	}
	while (stillPlaying)
	{
		//HRESULT hr;
		DWORD waitResult = WaitForMultipleObjects(3, waitArray, FALSE, INFINITE);
		switch (waitResult)
		{
		case WAIT_OBJECT_0 + 0:     // _ShutdownEvent
			stillPlaying = false;       // We're done, exit the loop.
			break;
		case WAIT_OBJECT_0 + 1:     // _StreamSwitchEvent
									//
									//  We need to stop the capturer, tear down the _AudioClient and _CaptureClient objects and re-create them on the new.
									//  endpoint if possible.  If this fails, abort the thread.
									//
			if (!HandleStreamSwitchEvent())
			{
				stillPlaying = false;
			}
			break;
		case WAIT_OBJECT_0 + 2:     // _AudioSamplesReadyEvent
									//
									//  We need to retrieve the next buffer of samples from the audio capturer.
									//
			BYTE *pData;
			UINT32 framesAvailable;
			DWORD  flags;

			//
			//  Find out how much capture data is available.  We need to make sure we don't run over the length
			//  of our capture buffer.  We'll discard any samples that don't fit in the buffer.
			//
			hr = _CaptureClient->GetBuffer(&pData, &framesAvailable, &flags, NULL, NULL);
			if (SUCCEEDED(hr))
			{
				UINT32 framesToCopy = min(framesAvailable, static_cast<UINT32>((_CaptureBufferSize - _CurrentCaptureIndex) / _FrameSize));
				if (framesToCopy != 0)
				{
					//
					//  The flags on capture tell us information about the data.
					//
					//  We only really care about the silent flag since we want to put frames of silence into the buffer
					//  when we receive silence.  We rely on the fact that a logical bit 0 is silence for both float and int formats.
					//
					if (flags & AUDCLNT_BUFFERFLAGS_SILENT)
					{
						//
						//  Fill 0s from the capture buffer to the output buffer.
						//
						ZeroMemory(&_CaptureBuffer[_CurrentCaptureIndex], framesToCopy*_FrameSize);
					}
					else
					{
						//
						//  Copy data from the audio engine buffer to the output buffer.
						//
						CopyMemory(&_CaptureBuffer[_CurrentCaptureIndex], pData, framesToCopy*_FrameSize);
					}
					//
					//  Bump the capture buffer pointer.
					//
					_CurrentCaptureIndex += framesToCopy*_FrameSize;
				}
				hr = _CaptureClient->ReleaseBuffer(framesAvailable);
				if (FAILED(hr))
				{
					printf("Unable to release capture buffer: %x!\n", hr);
				}
			}
			break;
		}
	}
	if (!DisableMMCSS)
	{
		AvRevertMmThreadCharacteristics(mmcssHandle);
	}

	CoUninitialize();
	return 0;
}


//
//  Initialize the stream switch logic.
//
bool audioCapture::InitializeStreamSwitch()
{
	HRESULT hr = _AudioClient->GetService(IID_PPV_ARGS(&_AudioSessionControl));
	if (FAILED(hr)) {
		debugLog("Unable to retrieve session control: %x\n", hr);
		return false;
	}

	//
	//  Create the stream switch complete event- we want a manual reset event that starts in the not-signaled state.
	//
	_StreamSwitchCompleteEvent = CreateEventEx(NULL, NULL, CREATE_EVENT_INITIAL_SET | CREATE_EVENT_MANUAL_RESET, EVENT_MODIFY_STATE | SYNCHRONIZE);
	if (_StreamSwitchCompleteEvent == NULL) {
		debugLog("Unable to create stream switch event: %d.\n", GetLastError());
		return false;
	}
	//
	//  Register for session and endpoint change notifications.  
	//
	//  A stream switch is initiated when we receive a session disconnect notification or we receive a default device changed notification.
	//
	hr = _AudioSessionControl->RegisterAudioSessionNotification(this);
	if (FAILED(hr)) {
		debugLog("Unable to register for stream switch notifications: %x\n", hr);
		return false;
	}

	hr = _DeviceEnumerator->RegisterEndpointNotificationCallback(this);
	if (FAILED(hr)) {
		debugLog("Unable to register for stream switch notifications: %x\n", hr);
		return false;
	}

	return true;
}

void audioCapture::TerminateStreamSwitch()
{
	HRESULT hr = _AudioSessionControl->UnregisterAudioSessionNotification(this);
	if (FAILED(hr)) {
		debugLog("Unable to unregister for session notifications: %x\n", hr);
	}

	_DeviceEnumerator->UnregisterEndpointNotificationCallback(this);
	if (FAILED(hr)) {
		debugLog("Unable to unregister for endpoint notifications: %x\n", hr);
	}

	if (_StreamSwitchCompleteEvent) {
		CloseHandle(_StreamSwitchCompleteEvent);
		_StreamSwitchCompleteEvent = NULL;
	}

	//evan SafeRelease(&_AudioSessionControl);
	//evan SafeRelease(&_DeviceEnumerator);
}

//
//  Handle the stream switch.
//
//  When a stream switch happens, we want to do several things in turn:
//
//  1) Stop the current capturer.
//  2) Release any resources we have allocated (the _AudioClient, _AudioSessionControl (after unregistering for notifications) and 
//        _CaptureClient).
//  3) Wait until the default device has changed (or 500ms has elapsed).  If we time out, we need to abort because the stream switch can't happen.
//  4) Retrieve the new default endpoint for our role.
//  5) Re-instantiate the audio client on that new endpoint.  
//  6) Retrieve the mix format for the new endpoint.  If the mix format doesn't match the old endpoint's mix format, we need to abort because the stream
//      switch can't happen.
//  7) Re-initialize the _AudioClient.
//  8) Re-register for session disconnect notifications and reset the stream switch complete event.
//
bool audioCapture::HandleStreamSwitchEvent()
{
	HRESULT hr;

	assert(_InStreamSwitch);
	//
	//  Step 1.  Stop capturing.
	//
	hr = _AudioClient->Stop();
	if (FAILED(hr)) {
		debugLog("Unable to stop audio client during stream switch: %x\n", hr);
		goto ErrorExit;
	}

	//
	//  Step 2.  Release our resources.  Note that we don't release the mix format, we need it for step 6.
	//
	hr = _AudioSessionControl->UnregisterAudioSessionNotification(this);
	if (FAILED(hr)) {
		debugLog("Unable to stop audio client during stream switch: %x\n", hr);
		goto ErrorExit;
	}

	//evan SafeRelease(&_AudioSessionControl);
	//evan SafeRelease(&_CaptureClient);
	//evan SafeRelease(&_AudioClient);
	//evan SafeRelease(&_Endpoint);

	//
	//  Step 3.  Wait for the default device to change.
	//
	//  There is a race between the session disconnect arriving and the new default device 
	//  arriving (if applicable).  Wait the shorter of 500 milliseconds or the arrival of the 
	//  new default device, then attempt to switch to the default device.  In the case of a 
	//  format change (i.e. the default device does not change), we artificially generate  a
	//  new default device notification so the code will not needlessly wait 500ms before 
	//  re-opening on the new format.  (However, note below in step 6 that in this SDK 
	//  sample, we are unlikely to actually successfully absorb a format change, but a 
	//  real audio application implementing stream switching would re-format their 
	//  pipeline to deliver the new format).  
	//
	DWORD waitResult = WaitForSingleObject(_StreamSwitchCompleteEvent, 500);
	if (waitResult == WAIT_TIMEOUT) {
		debugLog("Stream switch timeout - aborting...\n");
		goto ErrorExit;
	}

	//
	//  Step 4.  If we can't get the new endpoint, we need to abort the stream switch.  If there IS a new device,
	//          we should be able to retrieve it.
	//
	hr = _DeviceEnumerator->GetDefaultAudioEndpoint(eCapture, _EndpointRole, &_Endpoint);
	if (FAILED(hr)) {
		debugLog("Unable to retrieve new default device during stream switch: %x\n", hr);
		goto ErrorExit;
	}
	//
	//  Step 5 - Re-instantiate the audio client on the new endpoint.
	//
	hr = _Endpoint->Activate(__uuidof(IAudioClient), CLSCTX_INPROC_SERVER, NULL, reinterpret_cast<void **>(&_AudioClient));
	if (FAILED(hr)) {
		debugLog("Unable to activate audio client on the new endpoint: %x.\n", hr);
		goto ErrorExit;
	}
	//
	//  Step 6 - Retrieve the new mix format.
	//
	WAVEFORMATEX *wfxNew;
	hr = _AudioClient->GetMixFormat(&wfxNew);
	if (FAILED(hr)) {
		debugLog("Unable to retrieve mix format for new audio client: %x.\n", hr);
		goto ErrorExit;
	}

	//
	//  Note that this is an intentionally naive comparison.  A more sophisticated comparison would
	//  compare the sample rate, channel count and format and apply the appropriate conversions into the capture pipeline.
	//
	if (memcmp(_MixFormat, wfxNew, sizeof(WAVEFORMATEX) + wfxNew->cbSize) != 0) {
		debugLog("New mix format doesn't match old mix format.  Aborting.\n");
		CoTaskMemFree(wfxNew);
		goto ErrorExit;
	}
	CoTaskMemFree(wfxNew);

	//
	//  Step 7:  Re-initialize the audio client.
	//
	if (!InitializeAudioEngine()) {
		goto ErrorExit;
	}

	//
	//  Step 8: Re-register for session disconnect notifications.
	//
	hr = _AudioClient->GetService(IID_PPV_ARGS(&_AudioSessionControl));
	if (FAILED(hr)) {
		debugLog("Unable to retrieve session control on new audio client: %x\n", hr);
		goto ErrorExit;
	}
	hr = _AudioSessionControl->RegisterAudioSessionNotification(this);
	if (FAILED(hr)) {
		debugLog("Unable to retrieve session control on new audio client: %x\n", hr);
		goto ErrorExit;
	}

	//
	//  Reset the stream switch complete event because it's a manual reset event.
	//
	ResetEvent(_StreamSwitchCompleteEvent);
	//
	//  And we're done.  Start capturing again.
	//
	hr = _AudioClient->Start();
	if (FAILED(hr)) {
		debugLog("Unable to start the new audio client: %x\n", hr);
		goto ErrorExit;
	}

	_InStreamSwitch = false;
	return true;

ErrorExit:
	_InStreamSwitch = false;
	return false;
}

//
//  Called when an audio session is disconnected.  
//
//  When a session is disconnected because of a device removal or format change event, we just want 
//  to let the capture thread know that the session's gone away
//
HRESULT audioCapture::OnSessionDisconnected(AudioSessionDisconnectReason DisconnectReason)
{
	if (DisconnectReason == DisconnectReasonDeviceRemoval) {
		//
		//  The stream was disconnected because the device we're capturing to was removed.
		//
		//  We want to reset the stream switch complete event (so we'll block when the HandleStreamSwitchEvent function
		//  waits until the default device changed event occurs).
		//
		//  Note that we don't set the _StreamSwitchCompleteEvent - that will be set when the OnDefaultDeviceChanged event occurs.
		//
		_InStreamSwitch = true;
		SetEvent(_StreamSwitchEvent);
	}
	if (DisconnectReason == DisconnectReasonFormatChanged) {
		//
		//  The stream was disconnected because the format changed on our capture device.
		//
		//  We want to flag that we're in a stream switch and then set the stream switch event (which breaks out of the capturer).  We also
		//  want to set the _StreamSwitchCompleteEvent because we're not going to see a default device changed event after this.
		//
		_InStreamSwitch = true;
		SetEvent(_StreamSwitchEvent);
		SetEvent(_StreamSwitchCompleteEvent);
	}
	return S_OK;
}
//
//  Called when the default capture device changed.  We just want to set an event which lets the stream switch logic know that it's ok to 
//  continue with the stream switch.
//
HRESULT audioCapture::OnDefaultDeviceChanged(EDataFlow Flow, ERole Role, LPCWSTR /*NewDefaultDeviceId*/)
{
	if (Flow == eCapture && Role == _EndpointRole) {
		//
		//  The default capture device for our configuredf role was changed.  
		//
		//  If we're not in a stream switch already, we want to initiate a stream switch event.  
		//  We also we want to set the stream switch complete event.  That will signal the capture thread that it's ok to re-initialize the
		//  audio capturer.
		//
		if (!_InStreamSwitch) {
			_InStreamSwitch = true;
			SetEvent(_StreamSwitchEvent);
		}
		SetEvent(_StreamSwitchCompleteEvent);
	}
	return S_OK;
}

//
//  IUnknown
//
HRESULT audioCapture::QueryInterface(REFIID Iid, void **Object)
{
	if (Object == NULL) {
		return E_POINTER;
	}
	*Object = NULL;

	if (Iid == IID_IUnknown) {
		*Object = static_cast<IUnknown *>(static_cast<IAudioSessionEvents *>(this));
		AddRef();
	}
	else if (Iid == __uuidof(IMMNotificationClient)) {
		*Object = static_cast<IMMNotificationClient *>(this);
		AddRef();
	}
	else if (Iid == __uuidof(IAudioSessionEvents)) {
		*Object = static_cast<IAudioSessionEvents *>(this);
		AddRef();
	}
	else {
		return E_NOINTERFACE;
	}
	return S_OK;
}
ULONG audioCapture::AddRef()
{
	return InterlockedIncrement(&_RefCount);
}
ULONG audioCapture::Release()
{
	ULONG returnValue = InterlockedDecrement(&_RefCount);
//evan	if (returnValue == 0) {
//		delete this;
//	}
	return returnValue;
}


//
//  WAV file writer.
//
//  This is a VERY simple .WAV file writer.
//

//
//  A wave file consists of:
//
//  RIFF header:    8 bytes consisting of the signature "RIFF" followed by a 4 byte file length.
//  WAVE header:    4 bytes consisting of the signature "WAVE".
//  fmt header:     4 bytes consisting of the signature "fmt " followed by a WAVEFORMATEX 
//  WAVEFORMAT:     <n> bytes containing a waveformat structure.
//  DATA header:    8 bytes consisting of the signature "data" followed by a 4 byte file length.
//  wave data:      <m> bytes containing wave data.
//
//
//  Header for a WAV file - we define a structure describing the first few fields in the header for convenience.
//
struct WAVEHEADER
{
	DWORD   dwRiff;                     // "RIFF"
	DWORD   dwSize;                     // Size
	DWORD   dwWave;                     // "WAVE"
	DWORD   dwFmt;                      // "fmt "
	DWORD   dwFmtSize;                  // Wave Format Size
};

//  Static RIFF header, we'll append the format to it.
const BYTE WaveHeader[] =
{
	'R',   'I',   'F',   'F',  0x00,  0x00,  0x00,  0x00, 'W',   'A',   'V',   'E',   'f',   'm',   't',   ' ', 0x00, 0x00, 0x00, 0x00
};

//  Static wave DATA tag.
const BYTE WaveData[] = { 'd', 'a', 't', 'a' };

//
//  Write the contents of a WAV file.  We take as input the data to write and the format of that data.
//
bool WriteWaveFile(HANDLE FileHandle, const BYTE *Buffer, const size_t BufferSize, const WAVEFORMATEX *WaveFormat)
{
	DWORD waveFileSize = sizeof(WAVEHEADER) + sizeof(WAVEFORMATEX) + WaveFormat->cbSize + sizeof(WaveData) + sizeof(DWORD) + static_cast<DWORD>(BufferSize);
	BYTE *waveFileData = new (std::nothrow) BYTE[waveFileSize];
	BYTE *waveFilePointer = waveFileData;
	WAVEHEADER *waveHeader = reinterpret_cast<WAVEHEADER *>(waveFileData);

	if (waveFileData == NULL)
	{
		printf("Unable to allocate %d bytes to hold output wave data\n", waveFileSize);
		return false;
	}

	//
	//  Copy in the wave header - we'll fix up the lengths later.
	//
	CopyMemory(waveFilePointer, WaveHeader, sizeof(WaveHeader));
	waveFilePointer += sizeof(WaveHeader);

	//
	//  Update the sizes in the header.
	//
	waveHeader->dwSize = waveFileSize - (2 * sizeof(DWORD));
	waveHeader->dwFmtSize = sizeof(WAVEFORMATEX) + WaveFormat->cbSize;

	//
	//  Next copy in the WaveFormatex structure.
	//
	CopyMemory(waveFilePointer, WaveFormat, sizeof(WAVEFORMATEX) + WaveFormat->cbSize);
	waveFilePointer += sizeof(WAVEFORMATEX) + WaveFormat->cbSize;


	//
	//  Then the data header.
	//
	CopyMemory(waveFilePointer, WaveData, sizeof(WaveData));
	waveFilePointer += sizeof(WaveData);
	*(reinterpret_cast<DWORD *>(waveFilePointer)) = static_cast<DWORD>(BufferSize);
	waveFilePointer += sizeof(DWORD);

	//
	//  And finally copy in the audio data.
	//
	CopyMemory(waveFilePointer, Buffer, BufferSize);

	//
	//  Last but not least, write the data to the file.
	//
	DWORD bytesWritten;
	if (!WriteFile(FileHandle, waveFileData, waveFileSize, &bytesWritten, NULL))
	{
		printf("Unable to write wave file: %d\n", GetLastError());
		delete[]waveFileData;
		return false;
	}

	if (bytesWritten != waveFileSize)
	{
		printf("Failed to write entire wave file\n");
		delete[]waveFileData;
		return false;
	}
	delete[]waveFileData;
	return true;
}

void audioCapture::SaveWaveData(char *waveFileName, unsigned char *CaptureBuffer, size_t BufferSize, const WAVEFORMATEX *WaveFormat)
{
	
	HANDLE waveHandle = CreateFile(CString(waveFileName), GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,
		NULL);
	if (waveHandle != INVALID_HANDLE_VALUE) {
		if (WriteWaveFile(waveHandle, CaptureBuffer, BufferSize, WaveFormat)) {
			printf("Successfully wrote WAVE data to %S\n", waveFileName);
		} else {
			printf("Unable to write wave file\n");
		}
		CloseHandle(waveHandle);
	} else {
		printf("Unable to open output WAV file %S: %d\n", waveFileName, GetLastError());
	}
	
}

audioCapture::~audioCapture()
{
	if (DEBUG_ON) {
		fclose(fdebugAudio);
	}
//evan	if (m_audio_buffer != nullptr)
//		delete[] m_audio_buffer;
}
