 /*! \file eSPDI_DM.h
  	\brief eYs3D SDK API export functions, data structure and variable definition for depth map module
  	\copyright This file copyright (C) 2017 by eYs3D company

	\details An unpublished work.  All rights reserved.
	This file is proprietary information, and may not be disclosed or
	copied without the prior permission of eYs3D company.
 */
#pragma once
#include "eSPDI_Common.h"

// for APC_PostSetParam/APC_PostGetParam
#define POSTPAR_HR_MODE        5
#define POSTPAR_HR_CURVE_0     6
#define POSTPAR_HR_CURVE_1     7
#define POSTPAR_HR_CURVE_2     8
#define POSTPAR_HR_CURVE_3     9
#define POSTPAR_HR_CURVE_4    10
#define POSTPAR_HR_CURVE_5    11
#define POSTPAR_HR_CURVE_6    12
#define POSTPAR_HR_CURVE_7    13
#define POSTPAR_HR_CURVE_8    14
#define POSTPAR_HF_MODE       17
#define POSTPAR_DC_MODE       20
#define POSTPAR_DC_CNT_THD    21
#define POSTPAR_DC_GRAD_THD   22
#define POSTPAR_SEG_MODE      23
#define POSTPAR_SEG_THD_SUB   24
#define POSTPAR_SEG_THD_SLP   25
#define POSTPAR_SEG_THD_MAX   26
#define POSTPAR_SEG_THD_MIN   27
#define POSTPAR_SEG_FILL_MODE 28
#define POSTPAR_HF2_MODE      31
#define POSTPAR_GRAD_MODE     34
#define POSTPAR_TEMP0_MODE    37
#define POSTPAR_TEMP0_THD     38
#define POSTPAR_TEMP1_MODE    41
#define POSTPAR_TEMP1_LEVEL   42
#define POSTPAR_TEMP1_THD     43
#define POSTPAR_FC_MODE       46
#define POSTPAR_FC_EDGE_THD   47
#define POSTPAR_FC_AREA_THD   48
#define POSTPAR_MF_MODE       51
#define POSTPAR_ZM_MODE       52
#define POSTPAR_RF_MODE       53
#define POSTPAR_RF_LEVEL      54

//
// C++ compatibility
//
#ifdef  __cplusplus
extern "C" {
#endif

/*! \fn int APC_GetSlaveYOffset(
		void *pHandleApcDI,
		PDEVSELINFO pDevSelInfo,
		BYTE *buffer,
		int BufferLength,
		int *pActualLength,
		int index)
	\brief get Y offset data
	\param pHandleApcDI	 the pointer to the initilized ApcDI SDK instance
	\param pDevSelInfo	pointer of device select index
	\param buffer	buffer to store
	\param BufferLength	length of buffer
	\param pActualLength	actual byte of reading
	\param index	index of Y offset file ID
	\return success:APC_OK, others:see eSPDI_ErrCode.h
*/
int APC_API APC_GetSlaveYOffset(void *pHandleApcDI, PDEVSELINFO pDevSelInfo, BYTE *buffer, int BufferLength, int *pActualLength, int index);

/*! \fn int APC_GetYOffset(
		void *pHandleApcDI,
		PDEVSELINFO pDevSelInfo,
		BYTE *buffer,
		int BufferLength,
		int *pActualLength,
		int index)
	\brief get Y offset data
	\param pHandleApcDI	 the pointer to the initilized ApcDI SDK instance
	\param pDevSelInfo	pointer of device select index
	\param buffer	buffer to store
	\param BufferLength	length of buffer
	\param pActualLength	actual byte of reading
	\param index	index of Y offset file ID
	\return success:APC_OK, others:see eSPDI_ErrCode.h
*/
int APC_API APC_GetYOffset(void *pHandleApcDI, PDEVSELINFO pDevSelInfo, BYTE *buffer, int BufferLength, int *pActualLength, int index);

/*!	\fn APC_GetSlaveRectifyTable(
		void *pHandleApcDI,
		PDEVSELINFO pDevSelInfo,
		BYTE *buffer,
		int BufferLength,
		int *pActualLength,
		int index)
	\brief get rectify values from flash
	\param pHandleApcDI	 the pointer to the initilized ApcDI SDK instance
	\param pDevSelInfo	pointer of device select index
	\param buffer	buffer to store rectify table data
	\param BufferLength	input buffer length
	\param pActualLength	actual length has written to buffer
	\param index	index to identify rectify table for corresponding depth
	\return success:APC_OK, others: see eSPDI_ErrCode.h
*/
int APC_API APC_GetSlaveRectifyTable(void *pHandleApcDI, PDEVSELINFO pDevSelInfo, BYTE *buffer, int BufferLength, int *pActualLength, int index);

/*!	\fn APC_GetRectifyTable(
		void *pHandleApcDI,
		PDEVSELINFO pDevSelInfo,
		BYTE *buffer,
		int BufferLength,
		int *pActualLength,
		int index)
	\brief get rectify values from flash
	\param pHandleApcDI	 the pointer to the initilized ApcDI SDK instance
	\param pDevSelInfo	pointer of device select index
	\param buffer	buffer to store rectify table data
	\param BufferLength	input buffer length
	\param pActualLength	actual length has written to buffer
	\param index	index to identify rectify table for corresponding depth
	\return success:APC_OK, others: see eSPDI_ErrCode.h
*/
int APC_API APC_GetRectifyTable(void *pHandleApcDI, PDEVSELINFO pDevSelInfo, BYTE *buffer, int BufferLength, int *pActualLength, int index);

/*! \fn int APC_GetZDTable(
		void *pHandleApcDI,
		PDEVSELINFO pDevSelInfo,
		BYTE *buffer,
		int BufferLength,
		int *pActualLength,
		PZDTABLEINFO pZDTableInfo)
	\brief get disparity and Z values from flash
		1. if depth data type is APC_DEPTH_DATA_14_BITS then
			just get Z value from depth buffer 
		2. if depth data type is APC_ZD_TABLE_FILE_SIZE_11_BITS then
			using depth buffer value as a index to get Z value inside ZD table
		3. see GetZValue() of example.c to get Z value from different depth data type
	\param pHandleApcDI	 the pointer to the initilized ApcDI SDK instance
	\param pDevSelInfo	pointer of device select index
	\param buffer	bufer to store ZD table
	\param BufferLength	input buffer length
	\param pActualLength	actual length has written to buffer
	\param pZDTableInfo	index to identify ZD table and data type for corrresponding depth
	\return success: APC_OK, others: see eSPDI_ErrCode.h
*/
int APC_API APC_GetZDTable(void *pHandleApcDI, PDEVSELINFO pDevSelInfo, BYTE *buffer, int BufferLength, int *pActualLength, PZDTABLEINFO pZDTableInfo);

/*! \fn int APC_SetSlaveYOffset(
		void *pHandleApcDI,
		PDEVSELINFO pDevSelInfo,
		BYTE *buffer,
		int BufferLength,
		int *pActualLength,
		int index)
	\brief set Y offset data
	\param pHandleApcDI	 the pointer to the initilized ApcDI SDK instance
	\param pDevSelInfo	pointer of device select index
	\param buffer	buffer to store
	\param BufferLength	length of buffer
	\param pActualLength	actual byte of reading
	\param index	index of Y offset file ID
	\return success:APC_OK, others:see eSPDI_ErrCode.h
*/
int APC_API APC_SetSlaveYOffset(void *pHandleApcDI, PDEVSELINFO pDevSelInfo, BYTE *buffer, int BufferLength, int *pActualLength, int index);

/*! \fn int APC_SetYOffset(
		void *pHandleApcDI, 
		PDEVSELINFO pDevSelInfo, 
		BYTE *buffer, 
		int BufferLength, 
		int *pActualLength, 
		int index)
	\brief set Y offset data
	\param pHandleApcDI	 the pointer to the initilized ApcDI SDK instance
	\param pDevSelInfo	pointer of device select index
	\param buffer	buffer to store
	\param BufferLength	length of buffer
	\param pActualLength	actual byte of reading
	\param index	index of Y offset file ID
	\return success:APC_OK, others:see eSPDI_ErrCode.h
*/
int APC_API APC_SetYOffset(void *pHandleApcDI, PDEVSELINFO pDevSelInfo, BYTE *buffer, int BufferLength, int *pActualLength, int index);

/*! \fn int APC_SetYOffset_Advanced(
        void *pHandleApcDI,
        PDEVSELINFO pDevSelInfo,
        BYTE *buffer,
        int BufferLength,
        int *pActualLength,
        int index)
    \brief set Y offset data in both Groups #1 and #2 when the firmware has the flash protection.
    \param void *pHandleApcDI	 the pointer to the initilized ApcDI SDK instance
    \param PDEVSELINFO pDevSelInfo	CApcDI handler
    \param BYTE *buffer	buffer to store
    \param int BufferLength	length of buffer
    \param int *pActualLength	actual byte of reading
    \param int index	index of Y offset file ID
    \return success:APC_OK, others:see eSPDI_ErrCode.h
*/
int APC_API APC_SetYOffset_Advanced(void *pHandleApcDI, PDEVSELINFO pDevSelInfo, BYTE *buffer, int BufferLength, int *pActualLength, int index);

/*! \fn int APC_SetSlaveRectifyTable(void *pHandleApcDI, PDEVSELINFO pDevSelInfo, BYTE *buffer, int BufferLength, int *pActualLength, int index)
	\brief set rectify data to flash, see APC_SetRectifyTable except set
*/
int APC_API APC_SetSlaveRectifyTable(void *pHandleApcDI, PDEVSELINFO pDevSelInfo, BYTE *buffer, int BufferLength, int *pActualLength, int index);

/*! \fn int APC_SetRectifyTable(void *pHandleApcDI, PDEVSELINFO pDevSelInfo, BYTE *buffer, int BufferLength, int *pActualLength, int index)
	\brief set rectify data to flash, see APC_SetRectifyTable except set
*/
int APC_API APC_SetRectifyTable(void *pHandleApcDI, PDEVSELINFO pDevSelInfo, BYTE *buffer, int BufferLength, int *pActualLength, int index);

/*! \fn int APC_SetRectifyTable_Advanced(void *pHandleApcDI, PDEVSELINFO pDevSelInfo, BYTE *buffer, int BufferLength, int *pActualLength, int index)
    \brief set rectify data to flash, see APC_SetRectifyTable except set
*/
int APC_API APC_SetRectifyTable_Advanced(void *pHandleApcDI, PDEVSELINFO pDevSelInfo, BYTE *buffer, int BufferLength, int *pActualLength, int index);

/*! \fn APC_SetZDTable(void *pHandleApcDI, PDEVSELINFO pDevSelInfo, BYTE *buffer, int BufferLength, int *pActualLength, PZDTABLEINFO pZDTableInfo)
	\brief set disparity and Z values to flash, see APC_GetZDTable except get
*/
int APC_API APC_SetZDTable(void *pHandleApcDI, PDEVSELINFO pDevSelInfo, BYTE *buffer, int BufferLength, int *pActualLength, PZDTABLEINFO pZDTableInfo);

/*! \fn APC_SetZDTable_Advanced(void *pHandleApcDI, PDEVSELINFO pDevSelInfo, BYTE *buffer, int BufferLength, int *pActualLength, PZDTABLEINFO pZDTableInfo)
    \brief set disparity and Z values to flash, see APC_GetZDTable except get
*/
int APC_API APC_SetZDTable_Advanced(void *pHandleApcDI, PDEVSELINFO pDevSelInfo, BYTE *buffer, int BufferLength, int *pActualLength, PZDTABLEINFO pZDTableInfo);

/*! \fn int APC_GetRectifyMatLogData(
		void *pHandleApcDI,
		PDEVSELINFO pDevSelInfo,
		eSPCtrl_RectLogData *pData,
		int index)
	\brief get rectify log data from flash for Puma IC
	\param pHandleApcDI	 the pointer to the initilized ApcDI SDK instance
	\param pDevSelInfo	pointer of device select index
	\param pData	 rectify log data, its buffer size is 4096 bytes  
		see eSPCtrl_RectLogData for detailed members
	\param index	index to identify rectify log data for corresponding depth
	\return success: APC_OK, others: see eSPDI_ErrCode.h
*/
int APC_API APC_GetRectifyMatLogData(void *pHandleApcDI, PDEVSELINFO pDevSelInfo, eSPCtrl_RectLogData *pData, int index);

int APC_API APC_GetRectifyMatLogDataSlave(void *pHandleApcDI, PDEVSELINFO pDevSelInfo, eSPCtrl_RectLogData *pData, int index);

/*! \fn APC_SetDepthDataTypeEx(
		void *pHandleApcDI,
		PDEVSELINFO pDevSelInfo,
		WORD wType)
	\brief set depth data type, 11 bit for disparity data, 14 bit for Z data
		notice: only PUMA type IC can support this setting
	\param pHandleApcDI	 the pointer to the initilized ApcDI SDK instance
	\param pDevSelInfo	pointer of device select index
	\param wType	depth data type you want to set
	\param int Pid	PID of devicee you want to set,
		see APC_DEPTH_DATA_xxx in APC_O.h
	\output success: APC_OK, others: see eSPDI_ErrCode.h
*/
int APC_API APC_SetDepthDataTypeEx(void *pHandleApcDI, PDEVSELINFO pDevSelInfo, WORD wType, int pid);

/*! \fn APC_SetDepthDataType(
		void *pHandleApcDI,
		PDEVSELINFO pDevSelInfo,
		WORD wType)
	\brief set depth data type, 11 bit for disparity data, 14 bit for Z data
		notice: only PUMA type IC can support this setting
	\param pHandleApcDI	 the pointer to the initilized ApcDI SDK instance
	\param pDevSelInfo	pointer of device select index
	\param wType	depth data type you want to set,
		see APC_DEPTH_DATA_xxx in APC_O.h
	\output success: APC_OK, others: see eSPDI_ErrCode.h
*/
int APC_API APC_SetDepthDataType(void *pHandleApcDI, PDEVSELINFO pDevSelInfo, WORD wType);

/*! \fn int APC_GetDepthDataType(
		void *pHandleApcDI,
		PDEVSELINFO pDevSelInfo,
		WORD *pwType)
	\brief get current depth data type setting
	\param pHandleApcDI	 the pointer to the initilized ApcDI SDK instance
	\param pDevSelInfo	pointer of device select index
	\param pwType	pointer of current depth data type in device
	\return success: APC_OK, others: see eSPDI_ErrCode.h
*/
int APC_API APC_GetDepthDataType(void *pHandleApcDI, PDEVSELINFO pDevSelInfo, WORD *pwType);

/*! \fn int APC_SetHWPostProcess(void *pHandleApcDI, PDEVSELINFO pDevSelInfo, bool enable)
	\brief enable or disable internal chip post processing function
	\param pHandleApcDI	 the pointer to the initilized ApcDI SDK instance
	\param pDevSelInfo	pointer of device select index
	\param enable	set true to enable post-process, or set false to disable post-process
	\return success: APC_OK, others: see eSPDI_ErrCode.h
*/
int APC_API APC_SetHWPostProcess(void *pHandleApcDI, PDEVSELINFO pDevSelInfo, bool enable);

/*! \fn int APC_GetHWPostProcess(void *pHandleApcDI, PDEVSELINFO pDevSelInfo, bool* enable)
	\brief get hardware post processing status
	\param pHandleApcDI	 the pointer to the initilized ApcDI SDK instance
	\param pDevSelInfo	pointer of device select index
	\param enable	returns current hardware post-process status
	\return success: APC_OK, others: see eSPDI_ErrCode.h	
*/
int APC_API APC_GetHWPostProcess(void *pHandleApcDI, PDEVSELINFO pDevSelInfo, bool* enable);

/*! \fn int APC_IsInterleaveDevice(
void *pHandleApcDI,
PDEVSELINFO pDevSelInfo)
\brief enable or disable interleave function
\param pHandleApcDI	 the pointer to the initilized ApcDI SDK instance
\param pDevSelInfo	pointer of device select index
\param enable	set true to enable interleave, or set false to disable interleave
\return success: APC_OK, others: see eSPDI_ErrCode.h
*/
int APC_API APC_EnableInterleave(void *pHandleApcDI, PDEVSELINFO pDevSelInfo, bool enable);

/*! \fn bool APC_IsInterleaveDevice(
void *pHandleEYSD,
PDEVSELINFO pDevSelInfo)
\brief check module support interleave function or not
\param pHandleApcDI	 the pointer to the initilized ApcDI SDK instance
\param pDevSelInfo	pointer of device select index
\return true: support interleave, false: not support
*/
bool APC_API APC_IsInterleaveDevice(void *pHandleApcDI, PDEVSELINFO pDevSelInfo);

/*! \fn int APC_EnableSerialCount(
void *pHandleApcDI,
PDEVSELINFO pDevSelInfo)
\brief enable or disable interleave function
\param pHandleApcDI	 the pointer to the initilized ApcDI SDK instance
\param pDevSelInfo	pointer of device select index
\param enable	set true to enable Serial-Count, or set false to disable Serial-Count
\return success: APC_OK, others: see eSPDI_ErrCode.h
*/
int APC_API APC_EnableSerialCount( void *pHandleApcDI, PDEVSELINFO pDevSelInfo, bool enable );

/*! \fn int APC_EnableSerialCount(
void *pHandleApcDI,
PDEVSELINFO pDevSelInfo)
\brief enable or disable interleave function
\param pHandleApcDI	 the pointer to the initilized ApcDI SDK instance
\param pDevSelInfo	pointer of device select index
\param ImgColor	RGB-buffer
\param CW	ImgColor width
\param CH	ImgColor height
\param ImgDepth	depth-buffer
\param DW	ImgDepth width
\param DH	ImgDepth height
\param pPointCloudInfo	point-cloud information
\param pPointCloudRGB	point-cloud RGB value
\param pPointCloudXYZ	point-cloud XYZ value
\param Near	filter range near dist.
\param Far	filter range far dist.
\return success: APC_OK, others: see eSPDI_ErrCode.h
*/
struct PointCloudInfo
{
//normal data
    float centerX;
    float centerY;
    float focalLength;
    float disparityToW[ 2048 ];
    int   disparity_len;
    WORD  wDepthType;
//multi-lens data
    float focalLength_K;
    float baseline_K;
    float diff_K;
//slave data
    float	CamMat1[9];
    float	RotaMat[9];
    float	TranMat[3];

    PointCloudInfo() { memset( this, NULL, sizeof( PointCloudInfo ) ); }
};

int APC_API APC_GetPointCloud( void *pHandleApcDI, PDEVSELINFO pDevSelInfo, unsigned char* ImgColor, int CW, int CH,
                                                                                      unsigned char* ImgDepth, int DW, int DH,
                                                                                      PointCloudInfo* pPointCloudInfo,
                                                                                      unsigned char* pPointCloudRGB, float* pPointCloudXYZ, float Near, float Far );
/*! \fn int APC_FlyingDepthCancellation_D8(
    \fn int APC_FlyingDepthCancellation_D11(
void *pHandleApcDI,
PDEVSELINFO pDevSelInfo)
\brief enable or disable interleave function
\param pHandleApcDI	 the pointer to the initilized ApcDI SDK instance
\param pDevSelInfo	pointer of device select index
\param pDImg	depth 8bit buffer, or 11bit buffer
\param width	depth width
\param height	depth height
\return success: APC_OK, others: see eSPDI_ErrCode.h
*/
int APC_API APC_FlyingDepthCancellation_D8( void *pHandleApcDI, PDEVSELINFO pDevSelInfo, unsigned char* pdepthD8, int width, int height );

int APC_API APC_FlyingDepthCancellation_D11( void *pHandleApcDI, PDEVSELINFO pDevSelInfo, unsigned char* pdepthD11, int width, int height );


/*! \fn int APC_DepthMerge(
void *pHandleApcDI,
PDEVSELINFO pDevSelInfo)
\brief enable or disable interleave function
\param pHandleApcDI	 the pointer to the initilized ApcDI SDK instance
\param pDevSelInfo	pointer of device select index
\return success: APC_OK, others: see eSPDI_ErrCode.h
*/

int APC_API APC_DepthMerge( void *pHandleApcDI, PDEVSELINFO pDevSelInfo, unsigned char** pDepthBufList, float *pDepthMergeOut,
    unsigned char *pDepthMergeFlag, int nDWidth, int nDHeight, float fFocus, float * pBaseline, float * pWRNear, float * pWRFar, float * pWRFusion, int nMergeNum );

/*! \fn int APC_API APC_AdjustFocalLengthFromFlash(void *pHandleApcDI, PDEVSELINFO pDevSelInfo, int width, int height);
*/
int APC_API APC_AdjustFocalLengthFromFlash(void *pHandleApcDI, PDEVSELINFO pDevSelInfo, int width, int height);

/*! \fn int APC_API APC_AdjustFocalLength(void *pHandleApcDI, PDEVSELINFO pDevSelInfo, int width, int height, int pixelUnit);
*/
int APC_API APC_AdjustFocalLength(void *pHandleApcDI, PDEVSELINFO pDevSelInfo, int width, int height, int pixelUnit);

/*! \fn int APC_API APC_GetDeviceFocalLength(void *pHandleApcDI, PDEVSELINFO pDevSelInfo,
								 int *pLeftFx, int *pLeftFy, int *pRightFx, int *pRightFy);
*/
int APC_API APC_GetDeviceFocalLength(void *pHandleApcDI, PDEVSELINFO pDevSelInfo,
	int *pLeftFx, int *pLeftFy, int *pRightFx, int *pRightFy);

/*! \fn APC_API APC_GetFlashFocalLength(void *pHandleApcDI, PDEVSELINFO pDevSelInfo, int width, int height,
								int *pLeftFx, int *pLeftFy, int *pRightFx, int *pRightFy, int *pPixelUnit);
*/
int APC_API APC_GetFlashFocalLength(void *pHandleApcDI, PDEVSELINFO pDevSelInfo, int width, int height,
	int *pLeftFx, int *pLeftFy, int *pRightFx, int *pRightFy, int *pPixelUnit);

#ifdef __cplusplus
}
#endif