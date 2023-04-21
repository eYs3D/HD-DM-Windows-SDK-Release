#include "main.h"
#include <direct.h>
#include "ColorPaletteGenerator.h"
#include <mutex>
#include "IMUReader.h"
extern "C" {
#include "..\DMPreview\Quaternion1.h"
}
#define APC_USER_SETTING_OFFSET 5
#define APC_ZD_TABLE_FILE_SIZE_11_BITS 4096
#define CT_DEBUG(format, ...) printf("[%s][%d]" format, __func__, __LINE__, ##__VA_ARGS__)

void Read3X()
{
    void* pHandleApcDI;
    int ret_init = APC_Init(&pHandleApcDI, false);

    DEVSELINFO devSelInfo;
    devSelInfo.index = selectDeviceIndex(pHandleApcDI);

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
    devSelInfo.index = selectDeviceIndex(pHandleApcDI);

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
    devSelInfo.index = selectDeviceIndex(pHandleApcDI);

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
    devSelInfo.index = selectDeviceIndex(pHandleApcDI);

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
    devSelInfo.index = selectDeviceIndex(pHandleApcDI);

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
    devSelInfo.index = selectDeviceIndex(pHandleApcDI);

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
    devSelInfo.index = selectDeviceIndex(pHandleApcDI);

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
    devSelInfo.index = selectDeviceIndex(pHandleApcDI);

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


PreviewParam mPreviewParam;
RGBQUAD m_ColorPalette[16384];
int last_color_sn, last_depth_sn;
BYTE* m_zdTable;
int m_zdTableSize;
int m_zdTableIndex;

void UpdateD11DisplayImage_DIB24(const RGBQUAD* pColorPalette, const WORD* pDepth, BYTE* pResult, int m_nDepthResWidth, int m_nDepthResHeight)
{
    if (m_nDepthResWidth <= 0 || m_nDepthResHeight <= 0) return;

    int nBPS = ((m_nDepthResWidth * 3 + 3) / 4) * 4;
    BYTE* pDL = pResult + (m_nDepthResHeight - 1) * nBPS;
    BYTE* pD = NULL;
    const RGBQUAD* pClr = NULL;
    WORD colorPaletteSize = (1 << 14);

    for (int y = 0; y < m_nDepthResHeight; y++)
    {
        pD = pDL;
        for (int x = 0; x < m_nDepthResWidth; x++)
        {
            WORD z = ((WORD*)m_zdTable)[pDepth[x]];
            if (z < colorPaletteSize)
            {
                pClr = &(pColorPalette[z]);
                pD[0] = pClr->rgbBlue; //B
                pD[1] = pClr->rgbGreen; //G
                pD[2] = pClr->rgbRed; //R
            }
            else
            {
                pD[0] = 0; //B
                pD[1] = 0; //G
                pD[2] = 0; //R
            }
            pD += 3;
        }
        pDepth += m_nDepthResWidth;
        pDL -= nBPS;
    }
}

void UpdateZ14DisplayImage_DIB24(const RGBQUAD* pColorPalette, const WORD* pDepth, BYTE* pResult, int m_nDepthResWidth, int m_nDepthResHeight)
{
    if (m_nDepthResWidth <= 0 || m_nDepthResHeight <= 0) return;

    int x, y, nBPS = ((m_nDepthResWidth * 3 + 3) / 4) * 4;
    BYTE* pDL = pResult + (m_nDepthResHeight - 1) * nBPS;
    BYTE* pD = NULL;
    const RGBQUAD* pClr = NULL;

    for (y = 0; y < m_nDepthResHeight; y++)
    {
        pD = pDL;
        for (x = 0; x < m_nDepthResWidth; x++)
        {
            pClr = &(pColorPalette[pDepth[x]]);
            pD[0] = pClr->rgbBlue; //B
            pD[1] = pClr->rgbGreen; //G
            pD[2] = pClr->rgbRed; //R
            pD += 3;
        }
        pDepth += m_nDepthResWidth;
        pDL -= nBPS;
    }
}

int GetDateTime(char * psDateTime) {
    time_t timep;
    struct tm *p;

    time(&timep);
    p = localtime(&timep);

    sprintf(psDateTime, "%04d%02d%02d_%02d%02d%02d",
        (1900 + p->tm_year),
        (1 + p->tm_mon),
        p->tm_mday,
        p->tm_hour,
        p->tm_min,
        p->tm_sec);
    return 0;
}

int save_file(unsigned char *buf, int size, int width, int height, int type, bool isRGB)
{
    int ret = 0;
    int fd = -1;
    char fname[256] = { 0 };
    FILE * pFile;

    static unsigned int yuv_index = 0;
    static unsigned int mjpeg_index = 0;
    static unsigned int depth_index = 0;
    static unsigned int yuv_rgb_index = 0;
    static unsigned int mjpeg_rgb_index = 0;
    static unsigned int depth_rgb_index = 0;
    char DateTime[32] = { 0 };
    memset(DateTime, 0, sizeof(DateTime));

    ret = GetDateTime(DateTime);

    if (!isRGB) {
        switch (type) {
        case 0: // Color stream (YUYV)
            snprintf(fname, sizeof(fname), "./output/color_img_%02d_%03d_%s.yuv", mPreviewParam.devSelInfo.index, yuv_index++, DateTime);
            break;
        case 1: // Color stream (MJPEG)
            snprintf(fname, sizeof(fname), "./output/color_img_%02d_%03d_%s.jpg", mPreviewParam.devSelInfo.index, mjpeg_index++, DateTime);
            break;
        case 2: // Depth stream
            snprintf(fname, sizeof(fname), "./output/depth_img_%02d_%03d_%s.yuv", mPreviewParam.devSelInfo.index, depth_index++, DateTime);
            break;
        default:
            break;
        }
    }
    else {
        switch (type) {
        case 0: // YUV
            snprintf(fname, sizeof(fname), "./output/color_yuv2rgb_%02d_%03d_%s.raw", mPreviewParam.devSelInfo.index, yuv_rgb_index++, DateTime);
            break;
        case 1: // MJPEG
            snprintf(fname, sizeof(fname), "./output/color_mjpeg2rgb_%02d_%03d_%s.raw", mPreviewParam.devSelInfo.index, mjpeg_rgb_index++, DateTime);
            break;
        case 2: // Depth stream
            snprintf(fname, sizeof(fname), "./output/depth_imgrgb_%02d_%03d_%s.raw", mPreviewParam.devSelInfo.index, depth_rgb_index++, DateTime);
            break;
        default:
            break;
        }
    }

    CT_DEBUG("File Name: %s\n", fname);
    pFile = fopen(fname, "wb");

    if (!pFile)
    {
        CT_DEBUG("file open error (fd: %d)\n", fd);
        ret = -1;
    }
    else if (fwrite(buf, 1, size, pFile) != size) {
        CT_DEBUG("write(fd, buf, size) != size\n");
        ret = -1;
    }

    if (pFile > 0) {
        fclose(pFile);
    }

    return ret;
}

void generatePaletteColor(RGBQUAD* palette, int size, int mode, int customROI1, int customROI2, bool reverseRedToBlue) {
    float ROI2 = 1.0f;
    float ROI1 = 0.0f;

    //The value ranges from 0.0f ~ 1.0f as hue angle
    float ROI2Value = 1.0f;
    float ROI1Value = 0.0f;

    //BYTE* buf = (BYTE*)malloc(sizeof(BYTE) * 4 * size);
    //BYTE buf[(size) * 4];
    //Set ROI by mode setting.The bigger the disparity the nearer the distance
    switch (mode) {
    case 1: //near
        ROI2 = 0.8f;
        ROI1 = 0.5f;
        ROI2Value = 0.9f;
        ROI1Value = 0.1f;
        break;
    case 2: //midle
        ROI2 = 0.7f;
        ROI1 = 0.3f;
        ROI2Value = 0.9f;
        ROI1Value = 0.1f;
        break;
    case 3: //far
        ROI2 = 0.6f;
        ROI1 = 0.2f;
        ROI2Value = 0.9f;
        ROI1Value = 0.1f;
        break;
    case 4: //custom
        ROI2 = 1.0f*customROI2 / size;
        ROI1 = 1.0f*customROI1 / size;
        ROI2Value = 1.0f;
        ROI1Value = 0.0f;
        break;
    default: //normal
        ROI2 = 1.0f;
        ROI1 = 0.0f;
        ROI2Value = 1.0f;
        ROI1Value = 0.0f;
        break;
    }
    ColorPaletteGenerator::generatePalette((BYTE*)palette, size, ROI1 * size, ROI1Value, ROI2 * size, ROI2Value, reverseRedToBlue);

    //for ( int i = 0; i < size; i++ )
 //   {
 //       memcpy( &palette[ i ], &buf[ i * 4 ], sizeof( RGBQUAD ) );
    //}
 //   free( buf );
}

void ColorDepthCallback(APCImageType::Value imgType, int imgId, unsigned char* imgBuf, int imgSize, int width, int height, int serialNumber, LONGLONG timestamp, void* pParam)
{
	//DWORD treadID = ::GetCurrentThreadId();
	//printf("treadID = %d\n", treadID);
	std::vector<unsigned char> vecImgBuf(imgSize);
	memcpy(&vecImgBuf[0], imgBuf, imgSize);

	int printAll = (int)pParam;
	if (imgType == APCImageType::COLOR_YUY2 || imgType == APCImageType::COLOR_MJPG) {
		if (printAll)
			CT_DEBUG("[Color] SN: %d, timestamp: %lld\n", serialNumber, timestamp);

		if ((serialNumber - last_color_sn) != 1)
			CT_DEBUG("[Color] Lost Frame: %d - %d\n", last_color_sn, serialNumber);

		last_color_sn = serialNumber;

		save_file(&vecImgBuf[0], imgSize, width, height, (imgType == APCImageType::COLOR_MJPG), false);

		std::vector<unsigned char> m_vecRGBImageBuf(width * height * 3);
		if (APC_OK == APC_ColorFormat_to_RGB24(mPreviewParam.handleApcDI, &mPreviewParam.devSelInfo, &m_vecRGBImageBuf[0], &vecImgBuf[0],
				imgSize, width, height, imgType))
		{
			save_file(&m_vecRGBImageBuf[0], m_vecRGBImageBuf.size(), width, height, (imgType == APCImageType::COLOR_MJPG), true);
		}
	}
	else if (imgType == APCImageType::DEPTH_11BITS || imgType == APCImageType::DEPTH_14BITS) {
		if (printAll)
			CT_DEBUG("[Depth] SN: %d, timestamp: %lld\n", serialNumber, timestamp);

		if ((serialNumber - last_depth_sn) != 1)
			CT_DEBUG("[Depth] Lost Frame: %d - %d\n", last_depth_sn, serialNumber);

		last_depth_sn = serialNumber;

		save_file(&vecImgBuf[0], imgSize, width, height, 2, false);

		std::vector<unsigned char> m_vecRGBImageBuf(width * height * 3);
		if (imgType == APCImageType::DEPTH_11BITS)
		{
			if (m_zdTable) UpdateD11DisplayImage_DIB24(m_ColorPalette, (WORD*)&vecImgBuf[0], &m_vecRGBImageBuf[0], width, height);
		}
		else
		{
			UpdateZ14DisplayImage_DIB24(m_ColorPalette, (WORD*)&vecImgBuf[0], &m_vecRGBImageBuf[0], width, height);
		}
		save_file(&m_vecRGBImageBuf[0], m_vecRGBImageBuf.size(), width, height, 2, true);
	}
}

int selectDeviceIndex(void* pHandleApcDI)
{
    int deviceCount = APC_FindDevice(pHandleApcDI);
    printf("===========================================================\n");
    for (int i = 0; i < deviceCount; i++)
    {
        DEVINFORMATIONEX devinfo;
        DEVSELINFO devSelInfo;
        devSelInfo.index = i;
        int nRet = APC_GetDeviceInfoEx(pHandleApcDI, &devSelInfo, &devinfo);

        printf("[%d] PID:%d, strDevName:%s\n", i, devinfo.wPID, devinfo.strDevName);
    }
    printf("===========================================================\n");
    int device_index = 0;
    printf("\nPlease input device index : \n");
    scanf("%d", &device_index);
    if (device_index >= 0 && device_index < deviceCount)
        return device_index;
    return 0;
}

int selectColorDepth(void* pHandleApcDI, DEVSELINFO devSelInfo, int* color_index, int* depth_index, int* fps)
{
    APC_STREAM_INFO m_pStreamColorInfo[APC_MAX_STREAM_COUNT];
    APC_STREAM_INFO m_pStreamDepthInfo[APC_MAX_STREAM_COUNT];
    int m_colorStreamOptionCount;
    int m_depthStreamOptionCount;

    auto GetResolution = [ & ]( APC_STREAM_INFO* pStreamRes0, int* pStreamResCount0, APC_STREAM_INFO* pStreamRes1, int* pStreamResCount1)
    {
        memset( pStreamRes0, NULL, sizeof( APC_STREAM_INFO ) * APC_MAX_STREAM_COUNT );
        memset( pStreamRes1, NULL, sizeof( APC_STREAM_INFO ) * APC_MAX_STREAM_COUNT );

        int ret = APC_GetDeviceResolutionList(pHandleApcDI, &devSelInfo, APC_MAX_STREAM_COUNT, pStreamRes0, APC_MAX_STREAM_COUNT, pStreamRes1);
        *pStreamResCount0 = (int)((BYTE*)(&ret))[1];
        *pStreamResCount1 = (int)((BYTE*)(&ret))[0];
    };
    GetResolution(m_pStreamColorInfo, &m_colorStreamOptionCount, m_pStreamDepthInfo, &m_depthStreamOptionCount);

    printf("===========================================================\n");
    printf("[-1] No Color\n");
    for (int i = 0; i < m_colorStreamOptionCount; i++)
    {
        printf("[%d] %d x %d, %s\n", i, m_pStreamColorInfo[i].nWidth, m_pStreamColorInfo[i].nHeight, m_pStreamColorInfo[i].bFormatMJPG ? "MJPEG" : "YUYV");
    }
    printf("===========================================================\n");
    printf("\nPlease input color index : \n");
    scanf("%d", color_index);

    printf("===========================================================\n");
    printf("[-1] No Depth\n");
    for (int i = 0; i < m_depthStreamOptionCount; i++)
    {
        printf("[%d] %d x %d, %s\n", i, m_pStreamDepthInfo[i].nWidth, m_pStreamDepthInfo[i].nHeight, m_pStreamDepthInfo[i].bFormatMJPG ? "MJPEG" : "YUYV");
    }
    printf("===========================================================\n");
    printf("\nPlease input depth index : \n");
    scanf("%d", depth_index);

    printf("\nPlease input fps : \n");
    scanf("%d", fps);

    return 0;
}

void selectDepthType(void* pHandleApcDI, DEVSELINFO devSelInfo, int* depthType)
{
    printf("===========================================================\n");
    printf("APC_DEPTH_DATA_DEFAULT: %d\n", APC_DEPTH_DATA_DEFAULT);
    printf("APC_DEPTH_DATA_8_BITS: %d\n", APC_DEPTH_DATA_8_BITS);
    printf("APC_DEPTH_DATA_14_BITS: %d\n", APC_DEPTH_DATA_14_BITS);
    printf("APC_DEPTH_DATA_8_BITS_x80: %d\n", APC_DEPTH_DATA_8_BITS_x80);
    printf("APC_DEPTH_DATA_11_BITS: %d\n", APC_DEPTH_DATA_11_BITS);
    printf("APC_DEPTH_DATA_OFF_RECTIFY: %d\n", APC_DEPTH_DATA_OFF_RECTIFY);
    printf("APC_DEPTH_DATA_8_BITS_RAW: %d\n", APC_DEPTH_DATA_8_BITS_RAW);
    printf("APC_DEPTH_DATA_14_BITS_RAW: %d\n", APC_DEPTH_DATA_14_BITS_RAW);
    printf("APC_DEPTH_DATA_8_BITS_x80_RAW: %d\n", APC_DEPTH_DATA_8_BITS_x80_RAW);
    printf("APC_DEPTH_DATA_11_BITS_RAW: %d\n", APC_DEPTH_DATA_11_BITS_RAW);
    printf("APC_DEPTH_DATA_11_BITS_COMBINED_RECTIFY: %d\n", APC_DEPTH_DATA_11_BITS_COMBINED_RECTIFY);
    printf("===========================================================\n");
    printf("\nPlease input depth type value:\n");
    scanf("%d", depthType);
}

void GetDepthZValue(int& zFar, int& zNear)
{
    WORD wNear = MAXINT;
    WORD wFar = NULL;
    WORD* zdTable = (WORD*)m_zdTable;
    WORD  wTableSize = m_zdTableSize / 2;

    if (m_zdTable)
    {
        for (int i = 0; i < wTableSize; i++) if (zdTable[i] && zdTable[i] > wFar) { wFar = zdTable[i]; }
        for (int i = wTableSize - 1; i > -1; i--) if (zdTable[i] && zdTable[i] < wNear) { wNear = zdTable[i]; }

        if (zNear < wNear) zNear = wNear;
        if (zFar > wFar) zFar = wFar;
    }
}

void GetZDTable()
{
    int actualLength = 0;

    ZDTABLEINFO zdTableInfo;
    {
        zdTableInfo.nDataType = APC_DEPTH_DATA_11_BITS;
        zdTableInfo.nIndex = m_zdTableIndex;
    }
    do
    {
        if (APC_OK == APC_GetZDTable(mPreviewParam.handleApcDI, &mPreviewParam.devSelInfo, NULL, NULL, &m_zdTableSize, &zdTableInfo))
        {
            if (m_zdTable) delete[] m_zdTable;

            m_zdTable = new BYTE[m_zdTableSize];
            memset(m_zdTable, NULL, m_zdTableSize);

            if (APC_OK == APC_GetZDTable(mPreviewParam.handleApcDI, &mPreviewParam.devSelInfo, m_zdTable, m_zdTableSize, &actualLength, &zdTableInfo))
            {
                for (int i = 0; i < actualLength; i += 2)
                {
                    m_zdTable[i] ^= m_zdTable[i + 1];
                    m_zdTable[i + 1] ^= m_zdTable[i];
                    m_zdTable[i] ^= m_zdTable[i + 1];
                }
                m_zdTable[0] = NULL;
                m_zdTable[1] = NULL;

                break;
            }
        }
        delete[] m_zdTable;
        m_zdTable = NULL;
    } while (FALSE);
}

void GetColorDepthImage(bool printAll)
{
	int ret = APC_Init(&mPreviewParam.handleApcDI, false);
	if (ret != APC_OK) {
		CT_DEBUG("APC_Init error\n");
		return;
	}

	int color_index = -1, depth_index = -1, fps = 30;
	int depth_type = APC_DEPTH_DATA_11_BITS;
	int deviceIndex = selectDeviceIndex(mPreviewParam.handleApcDI);

	last_color_sn = 0;
	last_depth_sn = 0;

	mPreviewParam.devSelInfo.index = deviceIndex;

	selectColorDepth(mPreviewParam.handleApcDI, mPreviewParam.devSelInfo, &color_index, &depth_index, &fps);

	if (depth_index > -1)
	{
		m_zdTableIndex = depth_index;
		int mNear = 0, mFar = 1000;
		GetZDTable();
		GetDepthZValue(mFar, mNear);
		memset(m_ColorPalette, NULL, sizeof( m_ColorPalette ));
		generatePaletteColor(m_ColorPalette, 1 << 14, 4, mNear, mFar, true);
	}

	selectDepthType(mPreviewParam.handleApcDI, mPreviewParam.devSelInfo, &depth_type);
	ret = APC_SetDepthDataType(mPreviewParam.handleApcDI, &mPreviewParam.devSelInfo, depth_type);
	if (ret != APC_OK) {
		CT_DEBUG("APC_SetDepthDataType error\n");
		goto finish;
	}

	printf("color index : %d, depth index : %d, fps : %d, depth type : %d\n", color_index, depth_index, fps, depth_type);
	printf("Start to preview, press ESC to stop\n");

	ret = APC_OpenDevice(mPreviewParam.handleApcDI, &mPreviewParam.devSelInfo, color_index, depth_index, ApcDIDepthSwitch::Depth1, fps, ColorDepthCallback, (void*)printAll);
	if (ret != APC_OK) {
		CT_DEBUG("APC_OpenDevice error\n");
		goto finish;
	}
	_mkdir("output");

	while (GetAsyncKeyState(VK_ESCAPE) == 0) {
		Sleep(100);
	}

finish:
	APC_CloseDevice(mPreviewParam.handleApcDI, &mPreviewParam.devSelInfo);
	APC_Release(&mPreviewParam.handleApcDI);
}

void writePLY(char* fname, std::vector<CloudPoint> cloudpoint)
{
    std::string content;
    content.append("ply\n");
    content.append("format ascii 1.0\n");
    content.append("comment no face\n");
    char buffer[50] = { 0 };
    sprintf(buffer, "element vertex %d\n", cloudpoint.size());
    content.append(buffer);
    content.append("property uchar red\n");
    content.append("property uchar green\n");
    content.append("property uchar blue\n");
    content.append("property float x\n");
    content.append("property float y\n");
    content.append("property float z\n");
    content.append("end_header\n");

    for (int i = 0; i < cloudpoint.size(); i++) {
        char buf[512] = {0};
        sprintf(buf, "%d %d %d %.8f %.8f %.8f \n", cloudpoint[i].r, cloudpoint[i].g, cloudpoint[i].b, cloudpoint[i].x, -cloudpoint[i].y, -cloudpoint[i].z);
        content.append(buf);
    }
    FILE *ptr = fopen(fname, "w");
    fwrite(content.data(), 1, content.length(), ptr);
    fclose(ptr);
}

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
int m_near, m_far;

void ColorDepthCallbackForPointCloud(APCImageType::Value imgType, int imgId, unsigned char* imgBuf, int imgSize, int width, int height, int serialNumber, LONGLONG timestamp, void* pParam)
{
	int printAll = (int)pParam;

	std::lock_guard<std::mutex> lock((imgType == APCImageType::COLOR_YUY2 || imgType == APCImageType::COLOR_MJPG) ? m_mutex_color : m_mutex_depth );
	FramePool& fp = ( (imgType == APCImageType::COLOR_YUY2 || imgType == APCImageType::COLOR_MJPG) ? m_color : m_depth );

	if ( fp.data.size() < imgSize ) fp.data.resize( imgSize );

	memcpy( fp.data.data(), imgBuf, imgSize );
	fp.sn = serialNumber;
	fp.m_imgType = imgType;

	if (imgType == APCImageType::COLOR_YUY2 || imgType == APCImageType::COLOR_MJPG) {
		if ((serialNumber - last_color_sn) != 1)
			CT_DEBUG("[Color] Lost Frame: %d - %d\n", last_color_sn, serialNumber);

		last_color_sn = serialNumber;
		m_color.m_width = width;
		m_color.m_height = height;
	}
	else if (imgType == APCImageType::DEPTH_11BITS || imgType == APCImageType::DEPTH_14BITS) {
		if ((serialNumber - last_depth_sn) != 1)
			CT_DEBUG("[Depth] Lost Frame: %d - %d\n", last_depth_sn, serialNumber);

		last_depth_sn = serialNumber;
		m_depth.m_width = width;
		m_depth.m_height = height;
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
	float *pPointCloudXYZ = (float *)malloc(widthDepth * heightDepth * 3 * sizeof(float));

	if (!isStart) {
		starttime = clock();
		isStart = true;
	}
	ret = APC_ColorFormat_to_RGB24(mPreviewParam.handleApcDI, &mPreviewParam.devSelInfo, gColorRGBImgBuf, &bufColor[0], bufColor.size(), widthColor, heightColor, m_color.m_imgType);
	if (ret != APC_OK) {
		CT_DEBUG("APC_ColorFormat_to_RGB24 error\n");
		return;
	}

	ret = APC_GetPointCloud(mPreviewParam.handleApcDI, &mPreviewParam.devSelInfo, gColorRGBImgBuf, widthColor, heightColor, &bufDepth[0], widthDepth, heightDepth, &mPreviewParam.pointCloudInfo, pPointCloudRGB, pPointCloudXYZ, m_near, m_far);
	if (ret != APC_OK) {
		CT_DEBUG("APC_GetPointCloud error\n");
		return;
	}
	else {
		//transform and save PLY data, it's very time consuming
		std::vector<CloudPoint> cloud;
		CloudPoint cloudpoint = { 0 };
		char DateTime[32] = { 0 };
		static unsigned int yuv_index = 0;
		char fname[256] = { 0 };
		memset(DateTime, 0, sizeof(DateTime));
		GetDateTime(DateTime);
		snprintf(fname, sizeof(fname), "./output/cloud_%d_%s.ply", yuv_index++, DateTime);

		for (int i = 0; i < (widthDepth * heightDepth); i++) {
			if (pPointCloudXYZ[i * 3] != 0.0f && pPointCloudXYZ[i * 3 + 1] != 0.0f && pPointCloudXYZ[i * 3 + 2] != 0.0f) {
				cloudpoint.r = pPointCloudRGB[i * 3];
				cloudpoint.g = pPointCloudRGB[i * 3 + 1];
				cloudpoint.b = pPointCloudRGB[i * 3 + 2];
				cloudpoint.x = pPointCloudXYZ[i * 3];
				cloudpoint.y = pPointCloudXYZ[i * 3 + 1];
				cloudpoint.z = pPointCloudXYZ[i * 3 + 2];
				cloud.push_back(cloudpoint);
			}
		}
		writePLY(fname, cloud);
	}
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

	int color_index = -1, depth_index = -1, fps = 30;
	int depth_type = APC_DEPTH_DATA_11_BITS;
	int deviceIndex = selectDeviceIndex(mPreviewParam.handleApcDI);

	last_color_sn = 0;
	last_depth_sn = 0;

	mPreviewParam.devSelInfo.index = deviceIndex;

	selectColorDepth(mPreviewParam.handleApcDI, mPreviewParam.devSelInfo, &color_index, &depth_index, &fps);

	if (depth_index > -1)
	{
		m_zdTableIndex = depth_index;
		m_near = 0, m_far = 1000;
		GetZDTable();
		GetDepthZValue(m_far, m_near);
	}

	selectDepthType(mPreviewParam.handleApcDI, mPreviewParam.devSelInfo, &depth_type);
	mPreviewParam.pointCloudInfo.wDepthType = depth_type;

	eSPCtrl_RectLogData* rectifyData = (eSPCtrl_RectLogData*)malloc(sizeof(eSPCtrl_RectLogData));
	if (rectifyData == NULL) {
		CT_DEBUG("No memory allocated !\n");
		return;
	}

	ret = APC_GetRectifyMatLogData(mPreviewParam.handleApcDI, &mPreviewParam.devSelInfo, rectifyData, depth_index);
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

	m_threadStart = true;
	m_checkFrameReadyThread = ::CreateThread( NULL, NULL, CheckFrameReadyThreadFn, 0, NULL, NULL );

	ret = APC_SetDepthDataType(mPreviewParam.handleApcDI, &mPreviewParam.devSelInfo, depth_type);
	if (ret != APC_OK) {
		CT_DEBUG("APC_SetDepthDataType error\n");
		goto finish;
	}

	printf("Start to preview, press ESC to stop\n");
	ret = APC_OpenDevice(mPreviewParam.handleApcDI, &mPreviewParam.devSelInfo, color_index, depth_index, ApcDIDepthSwitch::Depth1, fps, ColorDepthCallbackForPointCloud, (void*)0);
	if (ret != APC_OK) {
		CT_DEBUG("APC_OpenDevice error\n");
		goto finish;
	}
	_mkdir("output");
	while (GetAsyncKeyState(VK_ESCAPE) == 0) {
		Sleep(100);
	}

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

void SetExposureTime(int pid)
{
    int ret = APC_OK;
    float fTime = 0.0f;

    ret = APC_GetExposureTime(mPreviewParam.handleApcDI, &mPreviewParam.devSelInfo, APC_SensorMode::SensorAll, pid, &fTime);
    if (ret != APC_OK) {
        CT_DEBUG("Failed to call APC_GetExposureTime()(%d)\n", ret);
    } else {
        CT_DEBUG("GetExposureTime: (%f)\n", fTime);
    }

    CT_DEBUG("Please Input ExposureTime: ");
    scanf("%f", &fTime);
    CT_DEBUG("Your input ExposureTime is %f\n", fTime);

    ret = APC_SetExposureTime(mPreviewParam.handleApcDI, &mPreviewParam.devSelInfo, APC_SensorMode::SensorAll, pid, fTime);
    if (ret != APC_OK) {
        CT_DEBUG("Failed to call APC_SetExposureTime()(%d)\n", ret);
    } else {
        ret = APC_GetExposureTime(mPreviewParam.handleApcDI, &mPreviewParam.devSelInfo, APC_SensorMode::SensorAll, pid, &fTime);
        if (ret != APC_OK) {
            CT_DEBUG("Failed to call APC_GetExposureTime()(%d)\n", ret);
        } else {
            CT_DEBUG("GetExposureTime: (%f)\n", fTime);
        }
    }
}

void SetAnalogGain(int pid)
{
    int ret = APC_OK;
    float fGain = 0.0f;

    ret = APC_GetAnalogGain(mPreviewParam.handleApcDI, &mPreviewParam.devSelInfo, APC_SensorMode::SensorAll, pid, &fGain);
    if (ret != APC_OK) {
        CT_DEBUG("Failed to call APC_GetAnalogGain()(%d)\n", ret);
    }
    else {
        CT_DEBUG("GetAnalogGain: (%f)\n", fGain);
    }

    CT_DEBUG("Please Input AnalogGain: ");
    scanf("%f", &fGain);
    CT_DEBUG("Your input AnalogGain is %f\n", fGain);

    ret = APC_SetAnalogGain(mPreviewParam.handleApcDI, &mPreviewParam.devSelInfo, APC_SensorMode::SensorAll, pid, fGain);
    if (ret != APC_OK) {
        CT_DEBUG("Failed to call APC_SetAnalogGain()(%d)\n", ret);
    }
    else {
        ret = APC_GetAnalogGain(mPreviewParam.handleApcDI, &mPreviewParam.devSelInfo, APC_SensorMode::SensorAll, pid, &fGain);
        if (ret != APC_OK) {
            CT_DEBUG("Failed to call APC_GetAnalogGain()(%d)\n", ret);
        }
        else {
            CT_DEBUG("GetAnalogGain: (%f)\n", fGain);
        }
    }
}

void SetDigitalGain(int pid)
{
    int ret = APC_OK;
    float fGain = 0.0f;

    ret = APC_GetDigitalGain(mPreviewParam.handleApcDI, &mPreviewParam.devSelInfo, APC_SensorMode::SensorAll, pid, &fGain);
    if (ret != APC_OK) {
        CT_DEBUG("Failed to call APC_GetDigitalGain()(%d)\n", ret);
    }
    else {
        CT_DEBUG("GetDigitalGain: (%f)\n", fGain);
    }

    CT_DEBUG("Please Input DigitalGain: ");
    scanf("%f", &fGain);
    CT_DEBUG("Your input DigitalGain is %f\n", fGain);

    ret = APC_SetDigitalGain(mPreviewParam.handleApcDI, &mPreviewParam.devSelInfo, APC_SensorMode::SensorAll, pid, fGain);
    if (ret != APC_OK) {
        CT_DEBUG("Failed to call APC_SetDigitalGain()(%d)\n", ret);
    }
    else {
        ret = APC_GetDigitalGain(mPreviewParam.handleApcDI, &mPreviewParam.devSelInfo, APC_SensorMode::SensorAll, pid, &fGain);
        if (ret != APC_OK) {
            CT_DEBUG("Failed to call APC_GetDigitalGain()(%d)\n", ret);
        }
        else {
            CT_DEBUG("GetDigitalGain: (%f)\n", fGain);
        }
    }
}

bool getImage = false;
void SetAnalogAndDigitalGainExample()
{
    auto ImageCallback = [](APCImageType::Value imgType, int imgId, unsigned char* imgBuf, int imgSize, int width, int height, int serialNumber, LONGLONG timestamp, void* pParam)
    {
        getImage = true;
    };

    getImage = false;
    int ret = APC_Init(&mPreviewParam.handleApcDI, false);
    if (ret != APC_OK) {
        CT_DEBUG("APC_Init error\n");
        return;
    }

    DEVINFORMATIONEX devinfo;
    int color_index = -1, depth_index = -1, fps = 30;
    int depth_type = APC_DEPTH_DATA_11_BITS;
    int puma_index = 0, plum_index = 0;;

    printf("Please select eSP876 device to get color image \n");
    puma_index = selectDeviceIndex(mPreviewParam.handleApcDI);

    printf("Please select eSP777 device to set/get gain related api \n");
    plum_index = selectDeviceIndex(mPreviewParam.handleApcDI);

    mPreviewParam.devSelInfo.index = puma_index;
    selectColorDepth(mPreviewParam.handleApcDI, mPreviewParam.devSelInfo, &color_index, &depth_index, &fps);

    selectDepthType(mPreviewParam.handleApcDI, mPreviewParam.devSelInfo, &depth_type);
    ret = APC_SetDepthDataType(mPreviewParam.handleApcDI, &mPreviewParam.devSelInfo, depth_type);
    if (ret != APC_OK) {
        CT_DEBUG("APC_SetDepthDataType error\n");
        goto finish;
    }

    printf("color index : %d, depth index : %d, fps : %d, depth type : %d\n", color_index, depth_index, fps, depth_type);

    ret = APC_OpenDevice(mPreviewParam.handleApcDI, &mPreviewParam.devSelInfo, color_index, depth_index, ApcDIDepthSwitch::Depth1, fps, ImageCallback, 0);
    if (ret != APC_OK) {
        CT_DEBUG("APC_OpenDevice error\n");
        goto finish;
    }

    while (!getImage)
    {
        Sleep(100);
    }
    Sleep(1000);

    // disable auto exposure
    long capsflag, cur1, cur2;
    ret = APC_PropertyCT_GetCurrent(mPreviewParam.handleApcDI, &mPreviewParam.devSelInfo, CT_PROPERTY_ID_EXPOSURE, &cur1, &cur2, &capsflag, APC_PID_IVY2_S );
    ret = APC_PropertyCT_SetCurrent(mPreviewParam.handleApcDI, &mPreviewParam.devSelInfo, CT_PROPERTY_ID_EXPOSURE, cur1, cur2, 2, APC_PID_IVY2_S);
    if (ret != APC_OK) {
        CT_DEBUG("Failed to call APC_DisableAE()(%d)\n", ret);
    }
    else {
        CT_DEBUG("Call APC_DisableAE()(%d)\n", ret);
    }

    ret = APC_GetDeviceInfoEx(mPreviewParam.handleApcDI, &mPreviewParam.devSelInfo, &devinfo);

    mPreviewParam.devSelInfo.index = plum_index;

    SetExposureTime(devinfo.wPID);
    printf("\n\n");
    SetAnalogGain(devinfo.wPID);
    printf("\n\n");
    SetDigitalGain(devinfo.wPID);
    printf("\n\n");
finish:
    APC_CloseDevice(mPreviewParam.handleApcDI, &mPreviewParam.devSelInfo);
    APC_Release(&mPreviewParam.handleApcDI);
}


bool m_bNeedInitQuaternion = true;
double m_QuaternionBeginInverse[4] = { 0 };
int m_OutputDataSize = 0;
int imuType = 0;
static void IMUCallback(IMUData imu)
{
    char textBuff[256] = { 0 };

    if (imuType)//IMU_9Axis
    {
        double Roll = 0;	//Angles.X
        double Pitch = 0;	//Angles.Y
        double Yaw = 0;	//Angles.Z

        if (m_bNeedInitQuaternion)
        {
            m_bNeedInitQuaternion = false;

            Quaternion begin;
            begin.w = imu._quaternion[0];
            begin.v[0] = imu._quaternion[1];
            begin.v[1] = imu._quaternion[2];
            begin.v[2] = imu._quaternion[3];

            Quaternion_inverse(&begin);
            Quaternion_normalize(&begin, &begin);

            m_QuaternionBeginInverse[0] = begin.w;
            m_QuaternionBeginInverse[1] = begin.v[0];
            m_QuaternionBeginInverse[2] = begin.v[1];
            m_QuaternionBeginInverse[3] = begin.v[2];
        }

        Quaternion raw;
        Quaternion begin;
        Quaternion output;

        raw.w = imu._quaternion[0];
        raw.v[0] = imu._quaternion[1];
        raw.v[1] = imu._quaternion[2];
        raw.v[2] = imu._quaternion[3];
        Quaternion_normalize(&raw, &raw);

        begin.w = m_QuaternionBeginInverse[0];
        begin.v[0] = m_QuaternionBeginInverse[1];
        begin.v[1] = m_QuaternionBeginInverse[2];
        begin.v[2] = m_QuaternionBeginInverse[3];
        Quaternion_normalize(&begin, &begin);
        Quaternion_multiply(&begin, &raw, &output);

        imu._quaternion[0] = output.w;
        imu._quaternion[1] = output.v[0];
        imu._quaternion[2] = output.v[1];
        imu._quaternion[3] = output.v[2];

        double theta[3];
        Quaternion_toEulerZYX(&output, theta);

        const double to_deg = 180.0 / M_PI;

        Roll = theta[0] * to_deg;
        Pitch = theta[1] * to_deg;
        Yaw = theta[2] * to_deg;

        snprintf(textBuff, sizeof(textBuff), "Frame count:%d\nTime:%2d:%2d:%4d\nRoll[%10.3f], Pitch[%10.3f], Yaw[%10.3f]\nQuaternion:\n0:%15.8f\n1:%15.8f\n2:%15.8f\n3:%15.8f",
            imu._frameCount,
            imu._min, imu._sec, imu._subSecond,
            Roll, Pitch, Yaw,
            imu._quaternion[0], imu._quaternion[1], imu._quaternion[2], imu._quaternion[3]);
    }
    else if (m_OutputDataSize == 27)
    {
        float g = sqrt((imu._accelX * imu._accelX) + (imu._accelY*imu._accelY) + (imu._accelZ*imu._accelZ));
        snprintf(textBuff, sizeof(textBuff), "Frame count:%d\nTime:%2d:%2d:%2d:%4d\nAccel X:%1.3f Y:%1.3f Z:%1.3f Total:%1.3f\nGyro X:%4.2f Y:%4.2f Z:%4.2f\n",
            imu._frameCount,
            imu._module_id, imu._min, imu._sec, imu._subSecond,
            imu._accelX, imu._accelY, imu._accelZ, g,
            imu._gyroScopeX, imu._gyroScopeY, imu._gyroScopeZ);
    }
    else if (m_OutputDataSize == 58)
    {
        snprintf(textBuff, sizeof(textBuff),
            "Frame count:%d\nTime:%2d:%2d:%2d:%4d\nAccel X:%04.1f\t\t     Y:%04.1f\t  Z:%04.1f\nGyro X:%04.0f\t\t     Y:%04.0f  \t  Z:%04.0f \nCompass X:%.1f   Y:%.1f  \t  Z:%.1f\nCompass_TBC X:%.1f\t     Y:%.1f  \t  Z:%.1f\nAccuracy_FLAG:%d\n\n",
            imu._frameCount,
            imu._module_id, imu._min, imu._sec, imu._subSecond,
            imu._accelX, imu._accelY, imu._accelZ,
            imu._gyroScopeX, imu._gyroScopeY, imu._gyroScopeZ,
            imu._compassX, imu._compassY, imu._compassZ,
            imu._compassX_TBC, imu._compassY_TBC, imu._compassZ_TBC,
            imu._accuracy_FLAG);
    }
    printf("%s\n", textBuff);
}

int selectIMUAxis()
{
    int index = 0;
    printf("===========================================================\n");
    printf("[0] IMU 6 Axis\n");
    printf("[1] IMU 9 Axis\n");
    printf("===========================================================\n");
    int device_index = 0;
    printf("\nPlease input index of imu type : \n");
    scanf("%d", &index);
    return index;
}

void IMUCallbackDemo()
{
    void* pHandleApcDI;
    int ret = APC_Init(&pHandleApcDI, false);
    if (ret != APC_OK) {
        CT_DEBUG("APC_Init error\n");
        return;
    }

    int deviceIndex = selectDeviceIndex(pHandleApcDI);

    DEVSELINFO devSelInfo;
    devSelInfo.index = deviceIndex;

    imuType = selectIMUAxis();

    IMUReader* imu_reader = new IMUReader(pHandleApcDI, devSelInfo, (imuType == 1) ? IMU_9Axis : IMU_6Axis);
    m_OutputDataSize = imu_reader->startGetImuData(IMUCallback);
    while (GetAsyncKeyState(VK_ESCAPE) == 0) {
        Sleep(100);
    }
    imu_reader->stopGetImuData();
    delete imu_reader;

    APC_Release(&pHandleApcDI);
    if (ret != APC_OK) {
        CT_DEBUG("APC_Release error\n");
        return;
    }
}

void IMUAPIDemo()
{
    void* pHandleApcDI;
    int ret = APC_Init(&pHandleApcDI, false);
    if (ret != APC_OK) {
        CT_DEBUG("APC_Init error\n");
        return;
    }

    int deviceIndex = selectDeviceIndex(pHandleApcDI);

    DEVSELINFO devSelInfo;
    devSelInfo.index = deviceIndex;

    imuType = selectIMUAxis();

    IMUReader* imu_reader = new IMUReader(pHandleApcDI, devSelInfo, (imuType == 1) ? IMU_9Axis : IMU_6Axis);

    //1. Read/Write RTC
    {
        char hour, minute, second;
        int iHour, iMinute, iSecond;

        imu_reader->GetRTC(&hour, &minute, &second);
        printf("Get RTC : %02d:%02d:%02d\n", hour, minute, second);

        printf("Set RTC with format hh:mm:ss \n");
        scanf("%d:%d:%d", &iHour, &iMinute, &iSecond);

        printf("Your input is %02d:%02d:%02d\n", iHour, iMinute, iSecond);
        imu_reader->SetRTC((char)iHour, (char)iMinute, (char)iSecond);

        imu_reader->GetRTC(&hour, &minute, &second);
        printf("Get RTC : %02d:%02d:%02d\n", hour, minute, second);
    }
    printf("\n");

    //2. Read/Write full scale of Accelerate
    {
        char value = 0;
        int iValue = 0;
        imu_reader->GetAccFs(&value);
        printf("Get Acc FS : %s\n", ACC_FS[value].c_str());
        printf("Set Acc FS\n[0]%s\n[1]%s\n[2]%s\n[3]%s\n", ACC_FS[0].c_str(), ACC_FS[1].c_str(), ACC_FS[2].c_str(), ACC_FS[3].c_str());
        scanf("%d", &iValue);
        if (iValue < size(ACC_FS))
        {
            printf("Your input is %s\n", ACC_FS[iValue].c_str());
            imu_reader->SetAccFs((char)iValue);
            imu_reader->GetAccFs(&value);
            printf("Get Acc FS : %s\n", ACC_FS[value].c_str());
        }
        else
        {
            printf("Invalid input\n");
        }
    }
    printf("\n");

    //3. Read/Write full scale of Gyro
    {
        char value = 0;
        int iValue = 0;
        imu_reader->GetGyrFs(&value);
        printf("Get Gyr FS : %s\n", GYR_FS[value].c_str());
        printf("Set Gyr FS\n[0]%s\n[1]%s\n[2]%s\n[3]%s\n[4]%s\n[5]%s\n", GYR_FS[0].c_str(), GYR_FS[1].c_str(), GYR_FS[2].c_str(), GYR_FS[3].c_str(), GYR_FS[4].c_str(), GYR_FS[5].c_str());
        scanf("%d", &iValue);
        if (iValue < size(GYR_FS))
        {
            printf("Your input is %s\n", GYR_FS[iValue].c_str());
            imu_reader->SetGyrFs((char)iValue);
            imu_reader->GetGyrFs(&value);
            printf("Get Gyr FS : %s\n", GYR_FS[value].c_str());
        }
        else
        {
            printf("Invalid input\n");
        }
    }
    printf("\n");

    delete imu_reader;

    APC_Release(&pHandleApcDI);
    if (ret != APC_OK) {
        CT_DEBUG("APC_Release error\n");
        return;
    }
}