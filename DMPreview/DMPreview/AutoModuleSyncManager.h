#pragma once
#ifdef ESPDI_EG
#include "eSPDI_EG.h"
#else
#include "eSPDI_DM.h"
#endif

#include <vector>
#include <map>

class AutoModuleSyncManager
{
public:
	static AutoModuleSyncManager *GetInstance()
	{
		static AutoModuleSyncManager *pInstance = nullptr;
		if (!pInstance)
		{
			pInstance = new AutoModuleSyncManager();
		}

		return pInstance;
	}

	bool IsModuleSyncAvailable() 
	{
		return m_vecModuleSyncDevice.size() > 1;
	}

	bool IsAutoModuleSyncEnabled()
	{
		return m_bIsAutoModuleSyncEnabled;
	}
	void EnableAutoModuleSync(bool bEnable)
	{
		m_bIsAutoModuleSyncEnabled = bEnable;
	}

	bool HasMasterDevice();

	std::vector<DEVSELINFO> GetModuleSyncDeviceVector();
	void SetMasterDevice(DEVSELINFO devSelInfo);
	bool IsMasterDevice(DEVSELINFO devSelInfo);
	CString GetSerialNumber(DEVSELINFO devSelInfo);
	int GetIndex(DEVSELINFO devSelInfo);

	void RegisterFrameResetCallback(std::function<void()> callback);
	void FrameStart(DEVSELINFO devSelInfo);
	void FrameStop(DEVSELINFO devSelInfo);

private:

	AutoModuleSyncManager();
	~AutoModuleSyncManager();

	bool IsTureVisionFlagEnable();
	void Init();
	bool EnumerateModuleSyncDevice();
	bool FindMasterDevice();
	bool SaveConfig();
	void Sort();

	void DeviceCheck(DEVSELINFO devSelInfo);

	bool IsModuleSyncSupport(unsigned short pid)
	{
		return ETronDI_PID_8053 == pid ||
			   ETronDI_PID_8059 == pid || 
			   ETronDI_PID_8062 == pid ;
	}

	void SetMasterDeviceSerialNumber(wchar_t *pSerialNumber, bool bSaveConfig = true);

private:	
	
	bool m_bIsAutoModuleSyncEnabled = false;
	
	wchar_t m_masterSerialNumber[512];
	DEVSELINFO m_masterDevice = { -1 };

	std::map<int, bool> m_mapModuleFrameStart;
	std::map<int, void*> m_maphEtronDI;

	std::vector<DEVSELINFO> m_vecModuleSyncDevice;
	std::vector<DEVINFORMATIONEX> m_vecDevInfo;

	std::function<void()> m_FrameResetCallback;
};

