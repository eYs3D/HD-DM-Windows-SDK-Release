#include "stdafx.h"
#include "AutoModuleSyncManager.h"
#include "utility/EtronDIUtility.h"

#define CONFIG_FILE_NAME L"AutoModuleSync.cfg"

AutoModuleSyncManager::AutoModuleSyncManager()
{
	Init();
}

AutoModuleSyncManager::~AutoModuleSyncManager()
{
	for (auto hEtronDI : m_maphEtronDI)
	{
		EtronDI_Release(&hEtronDI.second);
	}
}

bool AutoModuleSyncManager::IsTureVisionFlagEnable()
{
	BOOL bRet = TRUE;
	TCHAR buff[MAX_PATH];//
	memset(buff, 0, MAX_PATH);
	::GetModuleFileName(NULL, buff, sizeof(buff));
	CString strFolder = buff;
	CString strPath_Application = strFolder.Left(strFolder.ReverseFind(_T('\\')) + 1);

	CString profile_file = strPath_Application + _T("EtronDI_SdkConfig.ini");

	CString strAppName, strKeyName;
	CString strData;
	strAppName = _T("Setting");

	// AutoOpenAll_TrueVision = 
	strData.Empty();
	strKeyName = _T("AutoOpenAll_TrueVision");
	GetPrivateProfileString(strAppName, strKeyName, _T(""), strData.GetBuffer(MAX_PATH), MAX_PATH, profile_file);
	strData.ReleaseBuffer();
	return strData == _T("1");
}

void AutoModuleSyncManager::Init()
{
	if (!IsTureVisionFlagEnable()) return;

	if (!GetEtronDIDevice(m_vecDevInfo)) return;
	
	if (!EnumerateModuleSyncDevice()) return;

	FILE *pFile = _wfopen(CONFIG_FILE_NAME, L"rb");
	if (!pFile) return;
	
	wchar_t serialNumber[512] = { 0 };
	if (fwscanf(pFile, L"eys3d:%ls", serialNumber) >= 0)
	{
		SetMasterDeviceSerialNumber(serialNumber, false);
	}
	fclose(pFile);	
}

bool AutoModuleSyncManager::EnumerateModuleSyncDevice()
{
	m_vecModuleSyncDevice.clear();	

	unsigned short targetPID = 0;
	for (size_t i = 0 ; i < m_vecDevInfo.size() ; ++i)
	{		
		if (!targetPID) {
			targetPID = m_vecDevInfo[i].wPID;
		}

		if (!IsModuleSyncSupport(m_vecDevInfo[i].wPID) || 
			m_vecDevInfo[i].wPID != targetPID) {
			m_vecModuleSyncDevice.clear();
			return false;
		}

		m_vecModuleSyncDevice.push_back({ (int)i });
	}

	return m_vecModuleSyncDevice.size() > 1;
}

bool AutoModuleSyncManager::FindMasterDevice()
{
	m_masterDevice.index = -1;
	for (DEVSELINFO devSelInfo : m_vecModuleSyncDevice)
	{
		if (IsMasterDevice(devSelInfo)) {
			m_masterDevice = devSelInfo;

			Sort();

			return true;
		}
	}

	return false;
}

bool AutoModuleSyncManager::HasMasterDevice()
{
	return m_masterDevice.index != -1;
}

std::vector<DEVSELINFO> AutoModuleSyncManager::GetModuleSyncDeviceVector()
{
	return m_vecModuleSyncDevice;
}

void AutoModuleSyncManager::SetMasterDevice(DEVSELINFO devSelInfo)
{
	DeviceCheck(devSelInfo);
	
	wchar_t szBuf[MAX_PATH] = { NULL };
	int nActualSNLenByByte = 0;
	if (ETronDI_OK == EtronDI_GetSerialNumber(m_maphEtronDI[devSelInfo.index], &devSelInfo, (BYTE*)szBuf, MAX_PATH, &nActualSNLenByByte))
	{
		SetMasterDeviceSerialNumber(szBuf);
	}
}

void AutoModuleSyncManager::SetMasterDeviceSerialNumber(wchar_t *pSerialNumber, bool bSaveConfig)
{
	if (!pSerialNumber) return;

	wcscpy(m_masterSerialNumber, pSerialNumber);

	if (bSaveConfig) SaveConfig();

	FindMasterDevice();
}

bool AutoModuleSyncManager::IsMasterDevice(DEVSELINFO devSelInfo)
{
	if (HasMasterDevice())
	{
		return m_masterDevice.index == devSelInfo.index;
	}
	
	DeviceCheck(devSelInfo);
	
	wchar_t szBuf[MAX_PATH] = { NULL };
	int nActualSNLenByByte = 0;
	if (ETronDI_OK == EtronDI_GetSerialNumber(m_maphEtronDI[devSelInfo.index], &devSelInfo, (BYTE*)szBuf, MAX_PATH, &nActualSNLenByByte))
	{
		if (!wcscmp(szBuf, m_masterSerialNumber))
		{
			return true;
		}
	}

	return false;
}

CString AutoModuleSyncManager::GetSerialNumber(DEVSELINFO devSelInfo)
{
	DeviceCheck(devSelInfo);
	
	wchar_t szBuf[MAX_PATH] = { NULL };
	int nActualSNLenByByte = 0;
	if (ETronDI_OK == EtronDI_GetSerialNumber(m_maphEtronDI[devSelInfo.index], &devSelInfo, (BYTE*)szBuf, MAX_PATH, &nActualSNLenByByte))
	{
		return CString(szBuf);
	}

	return CString("");
}

bool AutoModuleSyncManager::SaveConfig()
{
	FILE *pFile = _wfopen(CONFIG_FILE_NAME, L"wb");
	if (!pFile) return false;
	fwprintf(pFile, L"eys3d:%ls", m_masterSerialNumber);
	fclose(pFile);
}

void AutoModuleSyncManager::RegisterFrameResetCallback(std::function<void()> callback)
{
	if (!callback) return;

	m_FrameResetCallback = callback;
}

void AutoModuleSyncManager::FrameStart(DEVSELINFO devSelInfo)
{
	m_mapModuleFrameStart[devSelInfo.index] = true;

	for (DEVSELINFO dev : m_vecModuleSyncDevice)
	{
		if (!m_mapModuleFrameStart.count(dev.index)) return;

		if (!m_mapModuleFrameStart[dev.index]) return;
	}

	if (m_FrameResetCallback) m_FrameResetCallback();
}

void AutoModuleSyncManager::FrameStop(DEVSELINFO devSelInfo)
{
	m_mapModuleFrameStart[devSelInfo.index] = false;
}

void AutoModuleSyncManager::DeviceCheck(DEVSELINFO devSelInfo)
{
	if (m_maphEtronDI.count(devSelInfo.index) != 0) return;

	EtronDI_Init2(&m_maphEtronDI[devSelInfo.index], false, false);
}

void AutoModuleSyncManager::Sort()
{
	if (!HasMasterDevice()) return;

	std::vector<DEVSELINFO> sortModuleSyncDevice;
	sortModuleSyncDevice.push_back(m_masterDevice);

	for (DEVSELINFO devSelInfo : m_vecModuleSyncDevice)
	{
		if (IsMasterDevice(devSelInfo)) continue;
		sortModuleSyncDevice.push_back(devSelInfo);
	}

	m_vecModuleSyncDevice = sortModuleSyncDevice;

}

int AutoModuleSyncManager::GetIndex(DEVSELINFO devSelInfo)
{	
	for (size_t i = 0; i < m_vecModuleSyncDevice.size() ; ++i) 
	{
		if (devSelInfo.index == m_vecModuleSyncDevice[i].index) return i;
	}
}