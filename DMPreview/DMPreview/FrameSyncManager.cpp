
#include "stdafx.h"
#include "FrameSyncManager.h"
#include <memory>

int FrameSyncManager::RegisterDevice(void *hEtronDI, DEVSELINFO devSelInfo)
{
	std::lock_guard<std::mutex> locker(m_mutex);

	auto key = GetKey(hEtronDI, devSelInfo);

	m_mapSyncList[key] = std::make_shared<SyncList>();

	return ETronDI_OK;
}

int FrameSyncManager::UnregisterDevice(void *hEtronDI, DEVSELINFO devSelInfo)
{
	auto key = GetKey(hEtronDI, devSelInfo);
	if (0 == m_mapSyncList.count(key)) return ETronDI_NullPtr;

	std::lock_guard<std::mutex> locker(m_mutex);
	m_mapSyncList.erase(key);

	return ETronDI_OK;
}

int FrameSyncManager::SyncImageCallback(void *hEtronDI, DEVSELINFO devSelInfo,
	EtronDIImageType::Value imageType, int imageId,
	int serailNumber, std::function<void()> &&imageCallback)
{
	std::lock_guard<std::mutex> locker(m_mutex);

	auto key = GetKey(hEtronDI, devSelInfo);
	if (0 == m_mapSyncList.count(key)) return ETronDI_NullPtr;

	if (serailNumber <= 0) {
		imageCallback();
		return ETronDI_OK;
	}

	int nCallbackIndex;

	if (EtronDIImageType::IsImageColor(imageType))
	{
		nCallbackIndex = 0;
		m_mapSyncList[key]->syncConditionMask |= Condition_Color;
		m_mapSyncList[key]->mapSyncObject[serailNumber].syncMask |= Condition_Color;
		
	}
	else if (EtronDIImageType::IsImageDepth(imageType))
	{
		nCallbackIndex = 1;
		m_mapSyncList[key]->syncConditionMask |= Condition_Depth;
		m_mapSyncList[key]->mapSyncObject[serailNumber].syncMask |= Condition_Depth;
	}
	else
	{
		return ETronDI_NotSupport;
	}

	std::shared_ptr<CallbackObject> pNewCallbackObj(new CallbackObject);
	pNewCallbackObj->callback = imageCallback;
	pNewCallbackObj->bHandled = false;
	std::lock_guard<std::mutex> objLocker(m_mapSyncList[key]->mutexObject);
	m_mapSyncList[key]->mapSyncObject[serailNumber].imageCallback[nCallbackIndex] = pNewCallbackObj;

	DoFrameSync(hEtronDI, devSelInfo, serailNumber);

	return ETronDI_OK;
}

int FrameSyncManager::SyncIMUCallback(void *hEtronDI, DEVSELINFO devSelInfo,
	int serailNumber, std::function<void()> &&imuCallback)
{
	std::lock_guard<std::mutex> locker(m_mutex);

	auto key = GetKey(hEtronDI, devSelInfo);
	if (0 == m_mapSyncList.count(key)) return ETronDI_NullPtr;
	if (serailNumber <= 0) {
		imuCallback();
		return ETronDI_OK;
	}

	std::shared_ptr<CallbackObject> pNewCallbackObj(new CallbackObject);

	pNewCallbackObj->callback = imuCallback;
	pNewCallbackObj->bHandled = false;

	std::lock_guard<std::mutex> objLocker(m_mapSyncList[key]->mutexObject);
	m_mapSyncList[key]->mapSyncObject[serailNumber].imuCallback = pNewCallbackObj;

	m_mapSyncList[key]->syncConditionMask |= Condition_IMU;
	m_mapSyncList[key]->mapSyncObject[serailNumber].syncMask |= Condition_IMU;

	DoFrameSync(hEtronDI, devSelInfo, serailNumber);

	return ETronDI_OK;
}

int FrameSyncManager::DoFrameSync(void *hEtronDI, DEVSELINFO devSelInfo, FrameCount frameCount)
{
	auto key = GetKey(hEtronDI, devSelInfo);
	if (0 == m_mapSyncList.count(key)) return ETronDI_NullPtr;
	if (frameCount <= 0) return ETronDI_NullPtr;

	if (m_mapSyncList[key]->syncConditionMask == m_mapSyncList[key]->mapSyncObject[frameCount].syncMask) {

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

	AccomplishFrameCallback(hEtronDI, devSelInfo);
}

int FrameSyncManager::AccomplishFrameCallback(void *hEtronDI, DEVSELINFO devSelInfo)
{
	auto key = GetKey(hEtronDI, devSelInfo);
	
	std::lock_guard<std::mutex> accomplishLock(m_mapSyncList[key]->mutexAccomplish);

	if (m_mapSyncList[key]->vectorAccomplishFrame.empty()) return ETronDI_NullPtr;

	for (SyncObject object : m_mapSyncList[key]->vectorAccomplishFrame) {
		for (std::shared_ptr<CallbackObject> imageObject : object.imageCallback) {
			if (imageObject.get()) {
				imageObject->callback();
			}
		}

		CallbackObject *pIMUObject = object.imuCallback.get();
		if (pIMUObject) {
			pIMUObject->callback();
		}
	}

	m_mapSyncList[key]->vectorAccomplishFrame.clear();

	return ETronDI_OK;
}