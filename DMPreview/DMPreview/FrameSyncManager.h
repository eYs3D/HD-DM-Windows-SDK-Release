#pragma once
#ifdef ESPDI_EG
#include "eSPDI_EG.h"
#else
#include "eSPDI_DM.h"
#endif

#include <functional>
#include <map>
#include <mutex>
#include <set>
#include <vector>


class FrameSyncManager
{
public:
	
	static FrameSyncManager *GetInstance()
	{
		static FrameSyncManager *pInstance = nullptr;

		if (!pInstance)
		{
			pInstance = new FrameSyncManager();
		}

		return pInstance;
	}

private:

	enum SyncCondition
	{
		Condition_Color = 0x1,
		Condition_Depth = 0x2,
		Condition_IMU   = 0x4
	};

	using SyncMask = unsigned int;
	using FrameCount = int;

	struct CallbackObject 
	{
		std::function<void()> callback;
		bool bHandled;
	};

	struct SyncObject
	{
		std::shared_ptr<CallbackObject> imageCallback[2];
		std::shared_ptr<CallbackObject> imuCallback;
		SyncMask syncMask;
	};

	struct SyncList
	{
		bool bRunning = true;

		std::mutex mutexObject;
		SyncMask   syncConditionMask;

		std::set<FrameCount> setHistory;
		std::map<FrameCount, SyncObject> mapSyncObject;

		std::mutex mutexAccomplish;
		std::vector<SyncObject> vectorAccomplishFrame;
	};

public:
	int RegisterDevice(void *hEtronDI, DEVSELINFO devSelInfo);
	int UnregisterDevice(void *hEtronDI, DEVSELINFO devSelInfo);

	int SyncImageCallback(void *hEtronDI, DEVSELINFO devSelInfo,
						  EtronDIImageType::Value imageType, int imageId,
						  int serailNumber, std::function<void()> &&imageCallback);
	int SyncIMUCallback(void *hEtronDI, DEVSELINFO devSelInfo,
						int serailNumber, std::function<void()> &&imuCallback);

	void SetEnabled(bool bEnabled)
	{
		m_bEnabled = bEnabled;
	}

	bool IsEnable() { return m_bEnabled;  }

private:
	int DoFrameSync(void *hEtronDI, DEVSELINFO devSelInfo, FrameCount frameCount);
	int AccomplishFrameCallback(void *hEtronDI, DEVSELINFO devSelInfo);

	std::pair<void *, DEVSELINFO> GetKey(void *hEtronDI, DEVSELINFO devSelInfo)
	{
		return std::make_pair(hEtronDI, devSelInfo);
	}

private:
	FrameSyncManager() = default;
	~FrameSyncManager() = default;

private:
	std::mutex m_mutex;
	struct Compare
	{
		bool operator()(const std::pair<void *, DEVSELINFO> &l, const std::pair<void *, DEVSELINFO> &r) const
		{
			if (l.first == r.first)
				return l.second.index > r.second.index;

			return l.first < r.first;
		}
	};

	std::map<std::pair<void *, DEVSELINFO>, std::shared_ptr<SyncList>, Compare> m_mapSyncList;

	bool m_bEnabled = false;

};

