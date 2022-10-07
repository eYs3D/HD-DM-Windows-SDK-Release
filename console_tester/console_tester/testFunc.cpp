#include "main.h"
#include <mutex>
#define APC_USER_SETTING_OFFSET 5
#define APC_ZD_TABLE_FILE_SIZE_11_BITS 4096
#define CT_DEBUG(format, ...) printf("[%s][%d]" format, __func__, __LINE__, ##__VA_ARGS__)

void Read3X()
{
    void* pHandleApcDI;
    int ret_init = APC_Init(&pHandleApcDI, false);

    DEVSELINFO devSelInfo;
    devSelInfo.index = 0;

    int index;
    int nbfferLength = APC_Y_OFFSET_FILE_SIZE;
    int pActualLength = 0;
    BYTE *data = new BYTE[nbfferLength];
    memset(data, 0x0, nbfferLength);
    for (index = 0; index <= 9; index++)
    {
        if (APC_OK == APC_GetYOffset(pHandleApcDI, &devSelInfo, data, nbfferLength, &pActualLength, index))
        {
            printf("\n Read3%d \n", index);
            for (int i = 0; i < nbfferLength; i++)
            {
                printf("%02x ", data[i]);
            }
            printf("\n");
        }
        else
        {
            printf("\n Read3%d Failed\n", index);
        }
    }

    delete[] data;
    APC_CloseDevice(pHandleApcDI, &devSelInfo);
    APC_Release(&pHandleApcDI);
}

void Write3X()
{
    void* pHandleApcDI;
    int ret_init = APC_Init(&pHandleApcDI, false);

    DEVSELINFO devSelInfo;
    devSelInfo.index = 0;

    int index;
    int nbfferLength = APC_Y_OFFSET_FILE_SIZE;
    int pActualLength = 0;
    BYTE *data = new BYTE[nbfferLength];
    memset(data, 0x0, nbfferLength);
    for (index = 0; index <= 9; index++)
    {
        if (APC_OK == APC_GetYOffset(pHandleApcDI, &devSelInfo, data, nbfferLength, &pActualLength, index))
        {
            if (APC_OK == APC_SetYOffset(pHandleApcDI, &devSelInfo, data, nbfferLength, &pActualLength, index))
            {
                printf("Write3%d Success \n", index);
            }
            else
            {
                printf("Write3%d Failed \n", index);
            }
        }
    }

    delete[] data;
    APC_CloseDevice(pHandleApcDI, &devSelInfo);
    APC_Release(&pHandleApcDI);
}

void Read4X()
{
    void* pHandleApcDI;
    int ret_init = APC_Init(&pHandleApcDI, false);

    DEVSELINFO devSelInfo;
    devSelInfo.index = 0;

    int index;
    int nbfferLength = APC_RECTIFY_FILE_SIZE;
    int pActualLength = 0;
    BYTE *data = new BYTE[nbfferLength];
    memset(data, 0x0, nbfferLength);
    for (index = 0; index <= 9; index++)
    {
        if (APC_OK == APC_GetRectifyTable(pHandleApcDI, &devSelInfo, data, nbfferLength, &pActualLength, index))
        {
            printf("\n Read4%d \n", index);
            for (int i = 0; i < nbfferLength; i++)
            {
                printf("%02x ", data[i]);
            }
            printf("\n");
        }
        else
        {
            printf("\n Read4%d Failed\n", index);
        }
    }
    delete[] data;
    APC_CloseDevice(pHandleApcDI, &devSelInfo);
    APC_Release(&pHandleApcDI);
}

void Write4X()
{
    void* pHandleApcDI;
    int ret_init = APC_Init(&pHandleApcDI, false);

    DEVSELINFO devSelInfo;
    devSelInfo.index = 0;

    int index;
    int nbfferLength = APC_RECTIFY_FILE_SIZE;
    int pActualLength = 0;
    BYTE *data = new BYTE[nbfferLength];
    memset(data, 0x0, nbfferLength);
    for (index = 0; index <= 9; index++)
    {
        if (APC_OK == APC_GetRectifyTable(pHandleApcDI, &devSelInfo, data, nbfferLength, &pActualLength, index))
        {
            if (APC_OK == APC_SetRectifyTable(pHandleApcDI, &devSelInfo, data, nbfferLength, &pActualLength, index))
            {
                printf("Write4%d Success \n", index);
            }
            else
            {
                printf("Write4%d Failed \n", index);
            }
        }
    }
    delete[] data;
    APC_CloseDevice(pHandleApcDI, &devSelInfo);
    APC_Release(&pHandleApcDI);
}

void Read5X()
{
    void* pHandleApcDI;
    int ret_init = APC_Init(&pHandleApcDI, false);

    DEVSELINFO devSelInfo;
    devSelInfo.index = 0;

    int index;
    int nbfferLength = APC_ZD_TABLE_FILE_SIZE;
    int pActualLength = 0;

    ZDTABLEINFO zdTableInfo;
    zdTableInfo.nDataType = APC_DEPTH_DATA_11_BITS;

    BYTE *data = new BYTE[nbfferLength];
    memset(data, 0x0, nbfferLength);
    for (index = 0; index <= 9; index++)
    {
        zdTableInfo.nIndex = index;
        if (APC_OK == APC_GetZDTable(pHandleApcDI, &devSelInfo, data, nbfferLength, &pActualLength, &zdTableInfo))
        {
            printf("\n Read5%d \n", index);
            for (int i = 0; i < nbfferLength; i++)
            {
                printf("%02x ", data[i]);
            }
            printf("\n");
        }
        else
        {
            printf("\n Read5%d Failed\n", index);
        }
    }
    delete[] data;
    APC_CloseDevice(pHandleApcDI, &devSelInfo);
    APC_Release(&pHandleApcDI);
}

void Write5X()
{
    void* pHandleApcDI;
    int ret_init = APC_Init(&pHandleApcDI, false);

    DEVSELINFO devSelInfo;
    devSelInfo.index = 0;

    int index;
    int nbfferLength = APC_ZD_TABLE_FILE_SIZE;
    int pActualLength = 0;

    ZDTABLEINFO zdTableInfo;
    zdTableInfo.nDataType = APC_DEPTH_DATA_11_BITS;

    BYTE *data = new BYTE[nbfferLength];
    memset(data, 0x0, nbfferLength);

    for (index = 0; index <= 9; index++)
    {
        zdTableInfo.nIndex = index;
        if (APC_OK == APC_GetZDTable(pHandleApcDI, &devSelInfo, data, nbfferLength, &pActualLength, &zdTableInfo))
        {
            if (APC_OK == APC_SetZDTable(pHandleApcDI, &devSelInfo, data, nbfferLength, &pActualLength, &zdTableInfo))
            {
                printf("Write5%d Success \n", index);
            }
            else
            {
                printf("Write5%d Failed \n", index);
            }
        }
    }
    delete[] data;
    APC_CloseDevice(pHandleApcDI, &devSelInfo);
    APC_Release(&pHandleApcDI);
}

void Read24X()
{
    void* pHandleApcDI;
    int ret_init = APC_Init(&pHandleApcDI, false);

    DEVSELINFO devSelInfo;
    devSelInfo.index = 0;

    int index;
    int nbfferLength = APC_CALIB_LOG_FILE_SIZE;
    int pActualLength = 0;

    BYTE *data = new BYTE[nbfferLength];
    memset(data, 0x0, nbfferLength);

    for (index = 0; index <= 9; index++)
    {
        if (APC_OK == APC_GetLogData(pHandleApcDI, &devSelInfo, data, nbfferLength, &pActualLength, index))
        {
            printf("\n Read24%d ALL_LOG \n", index);
            for (int i = 0; i < nbfferLength; i++)
            {
                printf("%02x ", data[i]);
            }
            printf("\n");
        }
        else
        {
            printf("\n Read24%d ALL_LOG Failed\n", index);
        }
    }
    delete[] data;
    APC_CloseDevice(pHandleApcDI, &devSelInfo);
    APC_Release(&pHandleApcDI);
}

void Write24X()
{
    void* pHandleApcDI;
    int ret_init = APC_Init(&pHandleApcDI, false);

    DEVSELINFO devSelInfo;
    devSelInfo.index = 0;

    int index;
    int nbfferLength = APC_CALIB_LOG_FILE_SIZE;
    int pActualLength = 0;

    BYTE *data = new BYTE[nbfferLength];
    memset(data, 0x0, nbfferLength);

    for (index = 0; index <= 9; index++)
    {
        if (APC_OK == APC_GetLogData(pHandleApcDI, &devSelInfo, data, nbfferLength, &pActualLength, index))
        {
            if (APC_OK == APC_SetLogData(pHandleApcDI, &devSelInfo, data, nbfferLength, &pActualLength, index))
            {
                printf("Write24%d Success \n", index);
            }
            else
            {
                printf("Write24%d Failed \n", index);
            }
        }
    }
    delete[] data;
    APC_CloseDevice(pHandleApcDI, &devSelInfo);
    APC_Release(&pHandleApcDI);
}

void ResetUNPData()
{
    void* pHandleApcDI;
    int ret_init = APC_Init(&pHandleApcDI, false);

    DEVSELINFO devSelInfo;
    devSelInfo.index = 0;

    int isReset = APC_ResetUNPData(pHandleApcDI, &devSelInfo);
    if (isReset != APC_OK)
    {
        printf("APC_ResetUNPData fail.\n");
    }
    else
    {
        printf("APC_ResetUNPData Success.\n");
    }
    APC_CloseDevice(pHandleApcDI, &devSelInfo);
    APC_Release(&pHandleApcDI);
}

void GetUserData() // Issue 6882
{
    void* pHandleApcDI = NULL;
    char buffer[1024];

    int ret = APC_Init(&pHandleApcDI, false);
    int deviceNumber = APC_GetDeviceNumber(pHandleApcDI);
    for (int i = 0; i < deviceNumber; i++)
    {
        memset(buffer, 0, 1024);
        DEVSELINFO devSelInfo;
        devSelInfo.index = i;
        USERDATA_SECTION_INDEX usi;
     
        if ((ret = APC_GetUserData(pHandleApcDI, &devSelInfo, (LPBYTE)buffer, 1024, usi)) == APC_OK)
        {
            //Retrieves correct data for each device
            for (int loop = 0; loop < 1024; loop++)
            {
                printf("%02x ", buffer[loop]);
            }
            printf("Done.\n\n");
        }
        APC_CloseDevice(pHandleApcDI, &devSelInfo);
    }
    printf("Wenling debug\n");
    APC_Release(&pHandleApcDI);
}

int last_color_sn, last_depth_sn;
void ColorDepthCallback(APCImageType::Value imgType, int imgId, unsigned char* imgBuf, int imgSize, int width, int height, int serialNumber, LONGLONG timestamp, void* pParam)
{
	int printAll = (int)pParam;
	if (imgType == APCImageType::COLOR_YUY2) {
		if (printAll)
			CT_DEBUG("[Color] SN: %d, timestamp: %lld\n", serialNumber, timestamp);

		if ((serialNumber - last_color_sn) != 1)
			CT_DEBUG("[Color] Lost Frame: %d - %d\n", last_color_sn, serialNumber);

		last_color_sn = serialNumber;
	}
	else if (imgType == APCImageType::DEPTH_11BITS) {
		if (printAll)
			CT_DEBUG("[Depth] SN: %d, timestamp: %lld\n", serialNumber, timestamp);

		if ((serialNumber - last_depth_sn) != 1)
			CT_DEBUG("[Depth] Lost Frame: %d - %d\n", last_depth_sn, serialNumber);

		last_depth_sn = serialNumber;
	}
}

void GetColorDepthImage(bool printAll)
{
	void* pHandleApcDI;
	int ret = APC_Init(&pHandleApcDI, false);
	if (ret != APC_OK) {
		CT_DEBUG("APC_Init error\n");
		return;
	}
	last_color_sn = 0;
	last_depth_sn = 0;

	DEVSELINFO devSelInfo;
	devSelInfo.index = 0;

	ret = APC_SetDepthDataType(pHandleApcDI, &devSelInfo, 4);
	if (ret != APC_OK) {
		CT_DEBUG("APC_SetDepthDataType error\n");
		goto finish;
	}

	ret = APC_OpenDevice(pHandleApcDI, &devSelInfo, 2, 1, 2, 30, ColorDepthCallback, (void*)printAll, -1);
	if (ret != APC_OK) {
		CT_DEBUG("APC_OpenDevice error\n");
		goto finish;
	}
	int input;
	scanf("%d", &input);

finish:
	APC_CloseDevice(pHandleApcDI, &devSelInfo);
	APC_Release(&pHandleApcDI);
}

PreviewParam mPreviewParam;
std::mutex m_mutex_color;
std::mutex m_mutex_depth;
FramePool m_color;
FramePool m_depth;
bool m_threadStart;
HANDLE m_checkFrameReadyThread;
DWORD framecount = 0;

bool isStart = false;
std::mutex m_mutex_pointcloud;
clock_t starttime;

void ColorDepthCallbackForPointCloud(APCImageType::Value imgType, int imgId, unsigned char* imgBuf, int imgSize, int width, int height, int serialNumber, LONGLONG timestamp, void* pParam)
{
	int printAll = (int)pParam;

	std::lock_guard<std::mutex> lock(imgType == APCImageType::COLOR_YUY2 ? m_mutex_color : m_mutex_depth );
	FramePool& fp = ( imgType == APCImageType::COLOR_YUY2 ? m_color : m_depth );

	if ( fp.data.size() < imgSize ) fp.data.resize( imgSize );

	memcpy( fp.data.data(), imgBuf, imgSize );
	fp.sn = serialNumber;

	if (imgType == APCImageType::COLOR_YUY2) {
		if ((serialNumber - last_color_sn) != 1)
			CT_DEBUG("[Color] Lost Frame: %d - %d\n", last_color_sn, serialNumber);

		last_color_sn = serialNumber;
	}
	else if (imgType == APCImageType::DEPTH_11BITS) {
		if ((serialNumber - last_depth_sn) != 1)
			CT_DEBUG("[Depth] Lost Frame: %d - %d\n", last_depth_sn, serialNumber);

		last_depth_sn = serialNumber;
	}
}
int count;
void BuildPointCloud(BOOL isDepthOnly,
	std::vector<unsigned char>& bufDepth,
	int widthDepth,
	int heightDepth,
	std::vector<unsigned char>& bufColor,
	int widthColor,
	int heightColor,
	int serialNumber,
	void* pParam)
{
	int ret = APC_OK;
	unsigned char *gColorRGBImgBuf = (unsigned char*)malloc(widthColor * heightColor * 3 * sizeof(unsigned char));
	unsigned char *pPointCloudRGB = (unsigned char *)malloc(widthColor * heightColor * 3 * sizeof(unsigned char));
	float *pPointCloudXYZ = (float *)malloc(widthColor * heightColor * 3 * sizeof(float));

	if (!isStart) {
		starttime = clock();
		isStart = true;
	}
	ret = APC_ColorFormat_to_RGB24(mPreviewParam.handleApcDI, &mPreviewParam.devSelInfo, gColorRGBImgBuf, &bufColor[0], bufColor.size(), widthColor, heightColor, APCImageType::Value::COLOR_YUY2);
	if (ret != APC_OK) {
		CT_DEBUG("APC_ColorFormat_to_RGB24 error\n");
		return;
	}

	ret = APC_GetPointCloud(mPreviewParam.handleApcDI, &mPreviewParam.devSelInfo, gColorRGBImgBuf, widthColor, heightColor, &bufDepth[0], widthColor, heightColor, &mPreviewParam.pointCloudInfo, pPointCloudRGB, pPointCloudXYZ, 64.0f, 1000.0f );
	if (ret != APC_OK) {
		CT_DEBUG("APC_GetPointCloud error\n");
		return;
	}
	/*else {
		if ((count % 30) == 0) {
			std::string plydata;
			for (int i = 0; i < widthColor*heightColor; i++) {
				char xyz[100] = { 0 };
				snprintf(xyz, sizeof(xyz), "%d %d %d %f %f %f\n", pPointCloudRGB[i * 3], pPointCloudRGB[i * 3 + 1], pPointCloudRGB[i * 3 + 2], pPointCloudXYZ[i * 3], pPointCloudXYZ[i * 3 + 1], pPointCloudXYZ[i * 3 + 2]);
				plydata.append(xyz);
			}
			FILE *ptr;
			ptr = fopen("pointcloud.ply", "w");
			fprintf(ptr, "ply\n");
			fprintf(ptr, "format ascii 1.0\n");
			fprintf(ptr, "comment no face\n");
			fprintf(ptr, "element vertex %d\n", widthColor*heightColor);
			fprintf(ptr, "property uchar red\n");
			fprintf(ptr, "property uchar green\n");
			fprintf(ptr, "property uchar blue\n");
			fprintf(ptr, "property float x\n");
			fprintf(ptr, "property float y\n");
			fprintf(ptr, "property float z\n");
			fprintf(ptr, "end_header\n");
			fprintf(ptr, "%s", plydata.c_str());
			fclose(ptr);
		}
	}*/
	if (gColorRGBImgBuf) free(gColorRGBImgBuf);
	if (pPointCloudRGB) free(pPointCloudRGB);
	if (pPointCloudXYZ) free(pPointCloudXYZ);

	{
		std::lock_guard< std::mutex > lock(m_mutex_pointcloud);
		framecount++;
		if (framecount > 20)
			CT_DEBUG("Point Cloud FPS: %6f\n", (float)framecount / ((clock() - starttime) / CLOCKS_PER_SEC)); 
	}
}

DWORD __stdcall CheckFrameReadyThreadFn( void* pvoid )
{
	BOOL synchronized = TRUE;
	int last_sn = -1;
	FramePool color;
	FramePool depth;
	BOOL isDepthOnly = FALSE;

	while (m_threadStart)
	{
		{
			std::lock_guard<std::mutex> lock( m_mutex_depth );

			if ( m_depth.sn == last_sn || m_color.data.empty() )
			{
				synchronized = FALSE;

				if (   m_depth.data.empty() != TRUE // is the depth only mode ?
					&& m_depth.m_height > 0
					&& m_depth.m_width  > 0
					&& m_color.data.empty() == TRUE
					&& m_color.m_height == 0
					&& m_color.m_width  == 0
					)
				{
					isDepthOnly = TRUE;
					synchronized = TRUE;
					depth = m_depth;
					color.m_height = m_depth.m_height;
					color.m_width = m_depth.m_width;
					if (color.data.size() == 0)
					{
						color.data.resize(color.m_width * color.m_height * 3, 0);
					}
				}
			}
			else
			{
				synchronized = TRUE;

				std::lock_guard<std::mutex> lock2( m_mutex_color );

				last_sn = m_depth.sn;
				color = m_color;
				depth = m_depth;
			}
		}
		if ( synchronized )
		{
			BuildPointCloud(isDepthOnly, depth.data, depth.m_width, depth.m_height, color.data, color.m_width, color.m_height, last_sn, NULL);
		}
		else std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
	return NULL;
}

void GetPointCloud()
{
	int preview_width = 1104, preview_height = 848;
	float ratio_Mat = 1.0f;
	float baseline  = 1.0f;
	float diff      = 1.0f;

	isStart = false;
	framecount = 0;

	int ret = APC_Init(&mPreviewParam.handleApcDI, false);
	if (ret != APC_OK) {
		CT_DEBUG("APC_Init error\n");
		return;
	}
	last_color_sn = 0;
	last_depth_sn = 0;

	mPreviewParam.devSelInfo.index = 0;
	mPreviewParam.pointCloudInfo.wDepthType = 4;

	eSPCtrl_RectLogData* rectifyData = (eSPCtrl_RectLogData*)malloc(sizeof(eSPCtrl_RectLogData));
	if (rectifyData == NULL) {
		CT_DEBUG("No memory allocated !\n");
		return;
	}

	ret = APC_GetRectifyMatLogData(mPreviewParam.handleApcDI, &mPreviewParam.devSelInfo, rectifyData, 1);
	if (ret != APC_OK)
	{
		CT_DEBUG("GetRectifyMatLogDat failed\n");
		goto finish;
	}
	baseline  = 1.0f / rectifyData->ReProjectMat[14];
	diff      = rectifyData->ReProjectMat[15] * ratio_Mat;

	mPreviewParam.pointCloudInfo.centerX       = -1.0f*rectifyData->ReProjectMat[3] * ratio_Mat;
	mPreviewParam.pointCloudInfo.centerY       = -1.0f*rectifyData->ReProjectMat[7] * ratio_Mat;
	mPreviewParam.pointCloudInfo.focalLength   = rectifyData->ReProjectMat[11] * ratio_Mat;
	mPreviewParam.pointCloudInfo.disparity_len = 2048;

	for (int i = 0; i < mPreviewParam.pointCloudInfo.disparity_len; i++) { mPreviewParam.pointCloudInfo.disparityToW[i] = ( i * ratio_Mat / 8.0f ) / baseline + diff; }

	m_color.m_width = preview_width;
	m_color.m_height = preview_height;
	m_depth.m_width = preview_width;
	m_depth.m_height = preview_height;

	m_threadStart = true;
	m_checkFrameReadyThread = ::CreateThread( NULL, NULL, CheckFrameReadyThreadFn, 0, NULL, NULL );

	ret = APC_SetDepthDataType(mPreviewParam.handleApcDI, &mPreviewParam.devSelInfo, 4);
	if (ret != APC_OK) {
		CT_DEBUG("APC_SetDepthDataType error\n");
		goto finish;
	}

	ret = APC_OpenDevice(mPreviewParam.handleApcDI, &mPreviewParam.devSelInfo, 2, 1, 2, 30, ColorDepthCallbackForPointCloud, (void*)0, -1);
	int input;
	scanf("%d", &input);

	m_threadStart = false;

finish:
	if (rectifyData) free(rectifyData);
	APC_CloseDevice(mPreviewParam.handleApcDI, &mPreviewParam.devSelInfo);
	APC_Release(&mPreviewParam.handleApcDI);
}
static void copy_file_to_g2(void* openedHandle, DEVSELINFO gsDevSelInfo, int fileIndex) {

	if (fileIndex > APC_USER_SETTING_OFFSET) return;

	auto bufferYOffset = new BYTE[APC_Y_OFFSET_FILE_SIZE];
	auto bufferYOffsetBackup = new BYTE[APC_Y_OFFSET_FILE_SIZE];
	auto EYSD = openedHandle;
	auto devSelInfo = &gsDevSelInfo;
	int actualYOffsetBufLen = 0;

	int ret = APC_GetYOffset(EYSD, devSelInfo, bufferYOffset, APC_Y_OFFSET_FILE_SIZE, &actualYOffsetBufLen, fileIndex);

	if (APC_OK != ret || actualYOffsetBufLen != APC_Y_OFFSET_FILE_SIZE) {
		CT_DEBUG("### [Stage-YOffset] Read error \n");
	}
	else {
		CT_DEBUG("### [Stage-YOffset] Read actualYOffsetBufLen %d file 3%d ret=%d\n", actualYOffsetBufLen, fileIndex, ret);

		memcpy(bufferYOffsetBackup, bufferYOffset, actualYOffsetBufLen);
#ifdef COPY_CLEAN
		memset(bufferYOffset, 0x0, APC_Y_OFFSET_FILE_SIZE);
#endif
		ret = APC_SetYOffset(EYSD, devSelInfo, bufferYOffset, APC_Y_OFFSET_FILE_SIZE, &actualYOffsetBufLen,
			fileIndex + APC_USER_SETTING_OFFSET);

		if (ret != APC_OK || actualYOffsetBufLen != APC_Y_OFFSET_FILE_SIZE) {
			CT_DEBUG("### [Stage-YOffset] Write error \n");
		}
		else {
			CT_DEBUG("### [Stage-YOffset] Write actualYOffsetBufLen %d file %d ret=%d\n", actualYOffsetBufLen,
				APC_Y_OFFSET_FILE_ID_0 + fileIndex + APC_USER_SETTING_OFFSET,
				ret);

			memset(bufferYOffset, 0xff, APC_Y_OFFSET_FILE_SIZE);
			ret = APC_GetYOffset(EYSD, devSelInfo, bufferYOffset, APC_Y_OFFSET_FILE_SIZE, &actualYOffsetBufLen,
				fileIndex + APC_USER_SETTING_OFFSET);

			if (ret != APC_OK || actualYOffsetBufLen != APC_Y_OFFSET_FILE_SIZE ||
				memcmp(bufferYOffset, bufferYOffsetBackup, actualYOffsetBufLen)) {
				CT_DEBUG("### [Stage-YOffset] Verify error. Please check. \n");
			}
			else {
				CT_DEBUG("### [Stage-YOffset] Verify successfully. \n");
			}
		}
	}

	delete[] bufferYOffset;
	delete[] bufferYOffsetBackup;

	auto bufferRectifyTable = new BYTE[APC_RECTIFY_FILE_SIZE];
	auto bufferRectifyTableBackup = new BYTE[APC_RECTIFY_FILE_SIZE];
	int actualRectifyBufLen = 0;
	ret = APC_GetRectifyTable(EYSD, devSelInfo, bufferRectifyTable, APC_RECTIFY_FILE_SIZE,
		&actualRectifyBufLen, fileIndex);
	if (ret != APC_OK || actualRectifyBufLen != APC_RECTIFY_FILE_SIZE) {
		CT_DEBUG("### [Stage-Rectify] Read error \n");
	}
	else {
		CT_DEBUG("### [Stage-Rectify] Read actualRectifyBufLen %d file 4%d ret=%d\n", actualRectifyBufLen,
			fileIndex, ret);

		memcpy(bufferRectifyTableBackup, bufferRectifyTable, actualRectifyBufLen);
#ifdef COPY_CLEAN
		memset(bufferRectifyTable, 0x0, APC_RECTIFY_FILE_SIZE);
#endif
		ret = APC_SetRectifyTable(EYSD, devSelInfo, bufferRectifyTable, APC_RECTIFY_FILE_SIZE,
			&actualRectifyBufLen, fileIndex + APC_USER_SETTING_OFFSET);

		if (ret != APC_OK || actualRectifyBufLen != APC_RECTIFY_FILE_SIZE) {
			CT_DEBUG("### [Stage-Rectify] Write error \n");
		}
		else {
			CT_DEBUG("### [Stage-Rectify] Write actualRectifyBufLen %d file %d ret=%d\n", actualRectifyBufLen,
				APC_RECTIFY_FILE_ID_0 + fileIndex + APC_USER_SETTING_OFFSET, ret);

			memset(bufferRectifyTable, 0xff, APC_RECTIFY_FILE_SIZE);

			ret = APC_GetRectifyTable(EYSD, devSelInfo, bufferRectifyTable, APC_RECTIFY_FILE_SIZE,
				&actualRectifyBufLen, fileIndex + APC_USER_SETTING_OFFSET);

			if (ret != APC_OK || actualRectifyBufLen != APC_RECTIFY_FILE_SIZE ||
				memcmp(bufferRectifyTable, bufferRectifyTableBackup, actualRectifyBufLen)) {
				CT_DEBUG("### [Stage-Rectify] Verify error. Please check. \n");
			}
			else {
				CT_DEBUG("### [Stage-Rectify] Verify successfully. \n");
			}
		}
	}

	delete[] bufferRectifyTable;
	delete[] bufferRectifyTableBackup;

	auto bufferZDTable = new BYTE[APC_ZD_TABLE_FILE_SIZE_11_BITS];
	auto bufferZDTableBackup = new BYTE[APC_ZD_TABLE_FILE_SIZE_11_BITS];
	int actualZDBufLen = 0;
	ZDTABLEINFO tableInfo;
	tableInfo.nDataType = APC_DEPTH_DATA_11_BITS;
	tableInfo.nIndex = fileIndex;

	ret = APC_GetZDTable(EYSD, devSelInfo, bufferZDTable, APC_ZD_TABLE_FILE_SIZE_11_BITS,
		&actualZDBufLen, &tableInfo);

	if (ret != APC_OK || actualZDBufLen != APC_ZD_TABLE_FILE_SIZE_11_BITS) {
		CT_DEBUG("### [Stage-ZD] Read error \n");
	}
	else {
		CT_DEBUG("### [Stage-ZD] Read actualZDBufLen %d file 5%d ret=%d\n", actualZDBufLen,
			fileIndex, ret);

		memcpy(bufferZDTableBackup, bufferZDTable, APC_ZD_TABLE_FILE_SIZE_11_BITS);
#ifdef COPY_CLEAN
		memset(bufferZDTable, 0x0, APC_ZD_TABLE_FILE_SIZE_11_BITS);
#endif
		tableInfo.nIndex = fileIndex + APC_USER_SETTING_OFFSET;
		ret = APC_SetZDTable(EYSD, devSelInfo, bufferZDTable, APC_ZD_TABLE_FILE_SIZE_11_BITS,
			&actualZDBufLen, &tableInfo);

		if (ret != APC_OK || actualZDBufLen != APC_ZD_TABLE_FILE_SIZE_11_BITS) {
			CT_DEBUG("### [Stage-ZD] Write error \n");
		}
		else {
			CT_DEBUG("### [Stage-ZD] Write actualZDBufLen %d file %d ret=%d\n", actualZDBufLen,
				APC_ZD_TABLE_FILE_ID_0 + fileIndex + APC_USER_SETTING_OFFSET, ret);

			memset(bufferZDTable, 0xff, APC_ZD_TABLE_FILE_SIZE_11_BITS);
			tableInfo.nIndex = fileIndex + APC_USER_SETTING_OFFSET;
			ret = APC_GetZDTable(EYSD, devSelInfo, bufferZDTable, APC_ZD_TABLE_FILE_SIZE_11_BITS,
				&actualZDBufLen, &tableInfo);

			if (ret != APC_OK || actualZDBufLen != APC_ZD_TABLE_FILE_SIZE_11_BITS ||
				memcmp(bufferZDTable, bufferZDTableBackup, APC_ZD_TABLE_FILE_SIZE_11_BITS)) {
				CT_DEBUG("### [Stage-ZD] Verify error. Please check. \n");
			}
			else {
				CT_DEBUG("### [Stage-ZD] Verify successfully. \n");
			}
		}
	}

	delete[] bufferZDTable;
	delete[] bufferZDTableBackup;

	auto bufferCalibrationLogData = new BYTE[APC_CALIB_LOG_FILE_SIZE];
	auto bufferCalibrationLogDataBackup = new BYTE[APC_CALIB_LOG_FILE_SIZE];
	int actualCalibrationLogDataBufLen = 0;

	ret = APC_GetLogData(EYSD, devSelInfo, bufferCalibrationLogData, APC_CALIB_LOG_FILE_SIZE,
		&actualCalibrationLogDataBufLen, fileIndex);

	if (APC_OK != ret || actualCalibrationLogDataBufLen != APC_CALIB_LOG_FILE_SIZE) {
		CT_DEBUG("### [Stage-LOG] Read error \n");
	}
	else {
		CT_DEBUG("### [Stage-LOG] Read actualCalibrationLogDataBufLen %d file 24%d ret=%d\n",
			actualCalibrationLogDataBufLen, fileIndex, ret);

		memcpy(bufferCalibrationLogDataBackup, bufferCalibrationLogData, APC_CALIB_LOG_FILE_SIZE);
#ifdef COPY_CLEAN
		memset(bufferCalibrationLogData, 0x0, APC_CALIB_LOG_FILE_SIZE);
#endif
		ret = APC_SetLogData(EYSD, devSelInfo, bufferCalibrationLogData, APC_CALIB_LOG_FILE_SIZE,
			&actualCalibrationLogDataBufLen, fileIndex + APC_USER_SETTING_OFFSET);

		if (actualCalibrationLogDataBufLen != APC_CALIB_LOG_FILE_SIZE || ret != APC_OK) {
			CT_DEBUG("### [Stage-LOG] Write error \n");
		}
		else {
			CT_DEBUG("### [Stage-LOG] Write actualCalibrationLogDataBufLen %d file %d ret=%d\n",
				actualCalibrationLogDataBufLen, APC_CALIB_LOG_FILE_ID_0 + fileIndex + APC_USER_SETTING_OFFSET, ret);

			memset(bufferCalibrationLogData, 0xff, APC_CALIB_LOG_FILE_SIZE);
			ret = APC_GetLogData(EYSD, devSelInfo, bufferCalibrationLogData, APC_CALIB_LOG_FILE_SIZE,
				&actualCalibrationLogDataBufLen, fileIndex + APC_USER_SETTING_OFFSET);

			if (ret != APC_OK || actualCalibrationLogDataBufLen != APC_CALIB_LOG_FILE_SIZE ||
				memcmp(bufferCalibrationLogData, bufferCalibrationLogDataBackup, APC_CALIB_LOG_FILE_SIZE)) {
				CT_DEBUG("### [Stage-LOG] Verify error. Please check. \n");
			}
			else {
				CT_DEBUG("### [Stage-LOG] Verify successfully. \n");
			}
		}
	}

	delete[] bufferCalibrationLogData;
	delete[] bufferCalibrationLogDataBackup;
}
void CopyFromG1ToG2() {
	void* pHandleApcDI;
	int ret = APC_Init(&pHandleApcDI, false);
	if (ret != APC_OK) {
		CT_DEBUG("APC_Init error\n");
		return;
	}

	DEVSELINFO devSelInfo;
	devSelInfo.index = 0;
	for (int index = 0; index < APC_USER_SETTING_OFFSET; ++index) {
		copy_file_to_g2(pHandleApcDI, devSelInfo, index);
	}

	APC_Release(&pHandleApcDI);
	if (ret != APC_OK) {
		CT_DEBUG("APC_Release error\n");
		return;
	}
}