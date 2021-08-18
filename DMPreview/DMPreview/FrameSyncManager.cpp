
#include "stdafx.h"
#include "FrameSyncManager.h"
#include <memory>
#include <future>

int FrameSyncManager::RegisterDevice(void *hApcDI, DEVSELINFO devSelInfo)
{
	std::lock_guard<std::mutex> locker(m_mutex);

	auto key = GetKey(hApcDI, devSelInfo);
	if (m_mapSyncList.count(key) != 0) return APC_OK;

	m_mapSyncList[key] = std::make_shared<SyncList>();
	m_mapSyncList[key]->thread = std::thread([=]() {
		while (m_mapSyncList[key]->bRunning)
		{
			AccomplishFrameCallback(hApcDI, devSelInfo);
		}
	});

	return APC_OK;
}

int FrameSyncManager::UnregisterDevice(void *hApcDI, DEVSELINFO devSelInfo)
{
	auto key = GetKey(hApcDI, devSelInfo);
	if (0 == m_mapSyncList.count(key)) return APC_NullPtr;

	std::lock_guard<std::mutex> locker(m_mutex);
	m_mapSyncList[key]->bRunning = false;
	m_mapSyncList[key]->thread.join();
	m_mapSyncList.erase(key);

	return APC_OK;
}

int FrameSyncManager::SyncImageCallback(void *hApcDI, DEVSELINFO devSelInfo,
	ApcDIImageType::Value imageType, int imageId,
	int serailNumber, std::function<void()> &&imageCallback)
{
	std::lock_guard<std::mutex> locker(m_mutex);

	auto key = GetKey(hApcDI, devSelInfo);
	if (0 == m_mapSyncList.count(key) || serailNumber <= 0) {
		imageCallback();
		return APC_OK;
	}

	int nCallbackIndex;

	if (ApcDIImageType::IsImageColor(imageType))
	{
		nCallbackIndex = 0;
		m_mapSyncList[key]->syncConditionMask |= Condition_Color;
		m_mapSyncList[key]->mapSyncObject[serailNumber].syncMask |= Condition_Color;
		
	}
	else if (ApcDIImageType::IsImageDepth(imageType))
	{
		nCallbackIndex = 1;
		m_mapSyncList[key]->syncConditionMask |= Condition_Depth;
		m_mapSyncList[key]->mapSyncObject[serailNumber].syncMask |= Condition_Depth;
	}
	else
	{
		return APC_NotSupport;
	}

	std::shared_ptr<CallbackObject> pNewCallbackObj(new CallbackObject);
	pNewCallbackObj->callback = imageCallback;
	pNewCallbackObj->bHandled = false;
	std::lock_guard<std::mutex> objLocker(m_mapSyncList[key]->mutexObject);
	m_mapSyncList[key]->mapSyncObject[serailNumber].imageCallback[nCallbackIndex] = pNewCallbackObj;

	DoFrameSync(hApcDI, devSelInfo, serailNumber);

	return APC_OK;
}

int FrameSyncManager::SyncIMUCallback(void *hApcDI, DEVSELINFO devSelInfo,
	int serailNumber, std::function<void()> &&imuCallback)
{
	std::lock_guard<std::mutex> locker(m_mutex);

	auto key = GetKey(hApcDI, devSelInfo);
	if (0 == m_mapSyncList.count(key) || serailNumber <= 0) {
		imuCallback();
		return APC_OK;
	}

	std::shared_ptr<CallbackObject> pNewCallbackObj(new CallbackObject);

	pNewCallbackObj->callback = imuCallback;
	pNewCallbackObj->bHandled = false;

	std::lock_guard<std::mutex> objLocker(m_mapSyncList[key]->mutexObject);
	m_mapSyncList[key]->mapSyncObject[serailNumber].imuCallback = pNewCallbackObj;

	m_mapSyncList[key]->syncConditionMask |= Condition_IMU;
	m_mapSyncList[key]->mapSyncObject[serailNumber].syncMask |= Condition_IMU;

	DoFrameSync(hApcDI, devSelInfo, serailNumber);

	return APC_OK;
}
int FrameSyncManager::DoFrameSync(void *hApcDI, DEVSELINFO devSelInfo, FrameCount frameCount)
{
	auto key = GetKey(hApcDI, devSelInfo);
	if (0 == m_mapSyncList.count(key)) return APC_NullPtr;
	if (frameCount <= 0) return APC_NullPtr;

	bool bIsSync;	
	if (m_mapSyncList[key]->bIsInterleave)
	{
		if (m_mapSyncList[key]->syncConditionMask & Condition_IMU)
		{
			if (m_mapSyncList[key]->mapSyncObject[frameCount].syncMask & Condition_IMU)
			{
				bIsSync = (m_mapSyncList[key]->mapSyncObject[frameCount].syncMask & ~Condition_IMU);
			}
			else
			{
				bIsSync = false;
			}
		}
		else
		{
			bIsSync = true;
		}
	}
	else
	{
		bIsSync = m_mapSyncList[key]->syncConditionMask == m_mapSyncList[key]->mapSyncObject[frameCount].syncMask;
	}

	if (bIsSync) {

		SyncObject syncObject;

		for (size_t i = 0; i < 2; ++i)
		{
			std::shared_ptr<CallbackObject> imageObject = m_mapSyncList[key]->mapSyncObject[frameCount].imageCallback[i];
			if (imageObject.get() && !imageObject->bHandled) {
				imageObject->bHandled = true;
				syncObject.imageCallback[i] = imageObject;
			}
		}

		std::shared_ptr<CallbackObject> imuObject = m_mapSyncList[key]->mapSyncObject[frameCount].imuCallback;
		if (imuObject.get() && !imuObject->bHandled) {
			imuObject->bHandled = true;
			syncObject.imuCallback = imuObject;
		}

		for (auto it = m_mapSyncList[key]->setHistory.begin(); it != m_mapSyncList[key]->setHistory.end(); ) {
			if (frameCount <= *it) break;
			m_mapSyncList[key]->mapSyncObject.erase(*it);
			it = m_mapSyncList[key]->setHistory.erase(it);
		}

		{
			std::lock_guard<std::mutex> accomplishLock(m_mapSyncList[key]->mutexAccomplish);
			m_mapSyncList[key]->vectorAccomplishFrame.push_back(std::move(syncObject));
		}
	}

	m_mapSyncList[key]->setHistory.emplace(frameCount);
}

int FrameSyncManager::AccomplishFrameCallback(void *hApcDI, DEVSELINFO devSelInfo)
{
	auto key = GetKey(hApcDI, devSelInfo);
	
	std::lock_guard<std::mutex> accomplishLock(m_mapSyncList[key]->mutexAccomplish);

	if (m_mapSyncList[key]->vectorAccomplishFrame.empty()) return APC_NullPtr;

	std::vector<std::future<void>> vecSyncFuture;

	for (SyncObject &object : m_mapSyncList[key]->vectorAccomplishFrame)
	{
		for (std::shared_ptr<CallbackObject> imageObject : object.imageCallback) {
			if (imageObject.get()) {
				vecSyncFuture.push_back(std::async(std::launch::async, imageObject->callback));
			}
		}

		CallbackObject *pIMUObject = object.imuCallback.get();
		if (pIMUObject) {
			vecSyncFuture.push_back(std::async(std::launch::async, pIMUObject->callback));
		}
	}

	for (std::future<void>& future : vecSyncFuture) {
		future.wait();
	}

	m_mapSyncList[key]->vectorAccomplishFrame.clear();

	return APC_OK;
}

int FrameSyncManager::SetIsInterleave(void *hApcDI, DEVSELINFO devSelInfo, bool bIsInterleave)
{
	std::lock_guard<std::mutex> locker(m_mutex);

	auto key = GetKey(hApcDI, devSelInfo);
	if (0 == m_mapSyncList.count(key)) return APC_NullPtr;

	m_mapSyncList[key]->bIsInterleave = bIsInterleave;

	return APC_OK;
}