#include "stdafx.h"
#include "FrameGrabber.h"
#ifndef ESPDI_EG
#include "eSPDI_DM.h"
#endif

FrameGrabber::FrameGrabber(
	size_t queueSize,
	FrameGrabberCallbackFn callbackFn, void* callbackParam) :
	m_callbackFn(callbackFn), 
	m_callbackParam(callbackParam)	
{
	//m_framePools.resize(FRAME_POOL_MAX_SIZE); // color and 1 depth
	//m_framePools[FRAME_POOL_INDEX_COLOR] = FramePool(queueSize);
	//m_framePools[FRAME_POOL_INDEX_DEPTH] = FramePool(queueSize);

    m_threadStart = true;
	m_checkFrameReadyThread = ::CreateThread( NULL, NULL, CheckFrameReadyThreadFn, this, NULL, NULL );
}

FrameGrabber::~FrameGrabber()
{
}

void FrameGrabber::UpdateFrameData(int index,int serialNumber, unsigned char*buf, size_t size)
{
    std::lock_guard<std::mutex> lock( FRAME_POOL_INDEX_COLOR == index ? m_mutex_color : m_mutex_depth );

    FramePool& fp = ( FRAME_POOL_INDEX_COLOR == index ? m_color : m_depth );

    if ( fp.data.size() < size ) fp.data.resize( size );

    memcpy( fp.data.data(), buf, size );

    fp.sn = serialNumber;

	//if (index >= 0 && index < m_framePools.size())
	//{
	//	m_framePools[index].UpdateData(serialNumber, buf, size);
	//}
}

void FrameGrabber::SetFrameFormat(int index, int width, int height, int bytesPerPixel)
{
    FramePool& fp = ( FRAME_POOL_INDEX_COLOR == index ? m_color : m_depth );

	fp.m_width = width;
	fp.m_height = height;
}

//void FrameGrabber::SetDisableSerialSyn(bool value)
//{
//	disableSerialSyn = value;
//}

void FrameGrabber::Close()
{
    m_threadStart = false;

    ::WaitForSingleObject( m_checkFrameReadyThread, INFINITE );
}

//bool FrameGrabber::FindAndDropOutOfDateItem(int targetSn, FramePool & framePool) const
//{
//	while (!framePool.m_frames.empty())
//	{
//		if (framePool.m_frames.front().sn == targetSn)
//		{
//			return true;
//		}
//		else if (framePool.m_frames.front().sn > targetSn)
//		{
//			return false;
//		}
//		else// depthPool.dataItems.front().sn < targetSn
//		{			
//			framePool.m_frames.pop();
//		}
//	}
//
//	return false;
//}


//void FrameGrabber::DropItem(int index ,int targetSn)
//{
//    //std::lock_guard<std::mutex> lock(m_FrameDataMutex[index]);
//	if (index >= 0 && index < m_framePools.size()){
//		while (!m_framePools[index].m_frames.empty()){
//			if (m_framePools[index].m_frames.front().sn > targetSn){
//				break;
//			}
//			m_framePools[index].m_frames.pop();
//		}
//	}
//}

//bool FrameGrabber::GetFrameData(int index ,int targetSn, std::vector<unsigned char>& outBuf)
//{
//	std::lock_guard<std::mutex> lock(m_FrameDataMutex[index]);
//
//    while (!m_framePools[index].m_frames.empty())
//    {
//        if (m_framePools[index].m_frames.front().sn == targetSn)
//        {
//            if (outBuf.size() != m_framePools[index].m_frames.front().data.size())
//            {
//                outBuf.resize(m_framePools[index].m_frames.front().data.size());
//            }
//
//            memcpy(&outBuf[0], &m_framePools[index].m_frames.front().data[0], m_framePools[index].m_frames.front().data.size());
//            return true;
//        }
//        else if (m_framePools[index].m_frames.front().sn > targetSn)
//        {
//            break;
//        }
//
//		m_framePools[index].m_frames.pop();
//    }
//
//    return false;
//}

DWORD FrameGrabber::CheckFrameReadyThreadFn( void* pvoid )
{
    FrameGrabber* pThis = ( FrameGrabber* )pvoid;

	//std::vector<unsigned char> outDepth;// (pThis->m_framePools[FRAME_POOL_INDEX_DEPTH].m_dataSize);
	//std::vector<unsigned char> outColor;// (pThis->m_framePools[FRAME_POOL_INDEX_COLOR].m_dataSize);  
	//int outWidthColor = 0;
	//int outHeightColor = 0;
	//int outWidthDepth = 0;
	//int outHeightDepth = 0;
	//int poolSizeColor = 0;
	//int poolSizeDepth = 0;
    BOOL synchronized = TRUE;
    int last_sn = -1;
    FramePool color;
    FramePool depth;

    while (pThis->m_threadStart)
    {
        {
            std::lock_guard<std::mutex> lock(pThis->m_mutex_depth);

            if ( pThis->m_depth.sn == last_sn || pThis->m_color.data.empty() )
            {
                synchronized = FALSE;
            }
            else
            {
                synchronized = TRUE;

                std::lock_guard<std::mutex> lock2(pThis->m_mutex_color);

                last_sn = pThis->m_depth.sn;
                color = pThis->m_color;
                depth = pThis->m_depth;
            }
        }
        if ( synchronized )
        {
            pThis->m_callbackFn( depth.data, depth.m_width, depth.m_height,
                                    color.data, color.m_width, color.m_height,
                                    last_sn, pThis->m_callbackParam);
        }
        else std::this_thread::sleep_for(std::chrono::milliseconds(1));
        //int synchronizedSn = -1;
        //{
			//int framePoolIndexBase = FRAME_POOL_INDEX_DEPTH;
			//std::lock_guard<std::mutex> lock(pThis->m_FrameDataMutex/*[FRAME_POOL_INDEX_COLOR]*/);
			//{
				//std::lock_guard<std::mutex> lock(pThis->m_FrameDataMutex[FRAME_POOL_INDEX_DEPTH]);
				//FramePool& frameDataPoolBase = pThis->m_framePools[framePoolIndexBase];
				//if (!frameDataPoolBase.m_frames.empty())
				//{
				//	synchronized = TRUE;
					//bool baseItemOutOfDate = false;
					//if (!pThis->disableSerialSyn)
					//{
					//	for (size_t i = 0; i < pThis->m_framePools.size(); ++i) {
					//		// should search all to drop out of date items, so don't call "break"
					//		if (!pThis->FindAndDropOutOfDateItem(frameDataPoolBase.m_frames.front().sn, pThis->m_framePools[i])) {
					//			if (!pThis->m_framePools[i].m_frames.empty())
					//			{
					//				baseItemOutOfDate = true;
					//			}
					//			synchronized = false;
					//		}
					//	}
					//	/*The serial numbers of each m_frameDataPool[i].dataItems.front().sn are the same after the loop*/
					//}
					//else {
						//for (size_t i = 0; i < pThis->m_framePools.size(); ++i) {
						//	if ( pThis->m_framePools[i].m_frames.empty() )synchronized = FALSE;
						//}
					//}

					//if (synchronized)
					//{
					//	FramePool& frameDataPoolDepth = pThis->m_framePools[FRAME_POOL_INDEX_DEPTH];
					//	FramePool& frameDataPoolColor = pThis->m_framePools[FRAME_POOL_INDEX_COLOR];

					//	Frame& frameColor = frameDataPoolColor.m_frames.front();						
					//	outColor.resize(frameColor.data.size());
					//	memcpy(&outColor[0], &frameColor.data[0], frameColor.data.size());
					//	outWidthColor = frameDataPoolColor.m_width;
					//	outHeightColor = frameDataPoolColor.m_height;

					//	Frame& frameDepth = frameDataPoolDepth.m_frames.front();						
					//	outDepth.resize(frameDepth.data.size());
					//	memcpy(&outDepth[0], &frameDepth.data[0], frameDepth.data.size());
					//	outWidthDepth = frameDataPoolDepth.m_width;
					//	outHeightDepth = frameDataPoolDepth.m_height;

					//	poolSizeColor = frameDataPoolColor.m_frames.size();
					//	poolSizeDepth = frameDataPoolDepth.m_frames.size();

					//	synchronizedSn = frameDataPoolBase.m_frames.front().sn;
					//	frameDataPoolColor.m_frames.pop();
					//	frameDataPoolDepth.m_frames.pop();
					//}
					//else if (baseItemOutOfDate)
					//{
					//	TRACE("baseItemOutOfDate\n");
					//	pThis->DropItem(framePoolIndexBase, frameDataPoolBase.m_frames.front().sn);
					//	if(!frameDataPoolBase.m_frames.empty())frameDataPoolBase.m_frames.pop();
					//}
				//}
			//}//release lock
        //}

   //     if (synchronizedSn != -1)// callback after unlock mutex to prevent deadlock
   //     {					
			////TRACE("         m_callbackFn:%d colorPool.size=%d depthPool.size=%d\n", synchronizedSn, poolSizeColor, poolSizeDepth);
   //         pThis->m_callbackFn(outDepth, outWidthDepth, outHeightDepth,
			//					outColor, outWidthColor, outHeightColor,
			//					synchronizedSn, pThis->m_callbackParam);
   //     }

        //std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    return NULL;
}

//FrameGrabber::Frame::Frame(int serialNumber, unsigned char* depthBuf, size_t depthSize)
//    : sn(serialNumber)
//{
//    if (data.size() != depthSize)
//    {
//        data.resize(depthSize);
//    }
//
//    memcpy(&data[0], depthBuf, depthSize);
//}
//
//void FrameGrabber::FramePool::UpdateData(int serialNumber, unsigned char* dataBuf, size_t dataSize)
//{
//    if (m_frames.size() >= maxSize)
//    {
//        m_frames.pop();
//    }
//
//    m_frames.push(Frame(serialNumber, dataBuf, dataSize));
//}
//
//
//FrameGrabber::FramePool::FramePool()
//{
//	maxSize = 1;
//}
//
//FrameGrabber::FramePool::FramePool(size_t poolSize) : maxSize(poolSize)
//{
//}
