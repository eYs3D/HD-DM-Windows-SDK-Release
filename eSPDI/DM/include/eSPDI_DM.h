 /*! \file eSPDI_DM.h
  	\brief Etron SDK API export functions, data structure and variable definition for depth map module
  	\copyright This file copyright (C) 2017 by eYs3D an Etron company

	\details An unpublished work.  All rights reserved.
	This file is proprietary information, and may not be disclosed or
	copied without the prior permission of eYs3D an Etron company.
 */
#pragma once
#include "eSPDI_Common.h"

// for EtronDI_PostSetParam/EtronDI_PostGetParam
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

/*! \fn int EtronDI_GetSlaveYOffset(
		void *pHandleEtronDI,
		PDEVSELINFO pDevSelInfo,
		BYTE *buffer,
		int BufferLength,
		int *pActualLength,
		int index)
	\brief get Y offset data
	\param pHandleEtronDI	 the pointer to the initilized EtronDI SDK instance
	\param pDevSelInfo	pointer of device select index
	\param buffer	buffer to store
	\param BufferLength	length of buffer
	\param pActualLength	actual byte of reading
	\param index	index of Y offset file ID
	\return success:EtronDI_OK, others:see eSPDI_ErrCode.h
*/
int ETRONDI_API EtronDI_GetSlaveYOffset(void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, BYTE *buffer, int BufferLength, int *pActualLength, int index);

/*! \fn int EtronDI_GetYOffset(
		void *pHandleEtronDI,
		PDEVSELINFO pDevSelInfo,
		BYTE *buffer,
		int BufferLength,
		int *pActualLength,
		int index)
	\brief get Y offset data
	\param pHandleEtronDI	 the pointer to the initilized EtronDI SDK instance
	\param pDevSelInfo	pointer of device select index
	\param buffer	buffer to store
	\param BufferLength	length of buffer
	\param pActualLength	actual byte of reading
	\param index	index of Y offset file ID
	\return success:EtronDI_OK, others:see eSPDI_ErrCode.h
*/
int ETRONDI_API EtronDI_GetYOffset(void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, BYTE *buffer, int BufferLength, int *pActualLength, int index);

/*!	\fn EtronDI_GetSlaveRectifyTable(
		void *pHandleEtronDI,
		PDEVSELINFO pDevSelInfo,
		BYTE *buffer,
		int BufferLength,
		int *pActualLength,
		int index)
	\brief get rectify values from flash
	\param pHandleEtronDI	 the pointer to the initilized EtronDI SDK instance
	\param pDevSelInfo	pointer of device select index
	\param buffer	buffer to store rectify table data
	\param BufferLength	input buffer length
	\param pActualLength	actual length has written to buffer
	\param index	index to identify rectify table for corresponding depth
	\return success:EtronDI_OK, others: see eSPDI_ErrCode.h
*/
int ETRONDI_API EtronDI_GetSlaveRectifyTable(void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, BYTE *buffer, int BufferLength, int *pActualLength, int index);

/*!	\fn EtronDI_GetRectifyTable(
		void *pHandleEtronDI,
		PDEVSELINFO pDevSelInfo,
		BYTE *buffer,
		int BufferLength,
		int *pActualLength,
		int index)
	\brief get rectify values from flash
	\param pHandleEtronDI	 the pointer to the initilized EtronDI SDK instance
	\param pDevSelInfo	pointer of device select index
	\param buffer	buffer to store rectify table data
	\param BufferLength	input buffer length
	\param pActualLength	actual length has written to buffer
	\param index	index to identify rectify table for corresponding depth
	\return success:EtronDI_OK, others: see eSPDI_ErrCode.h
*/
int ETRONDI_API EtronDI_GetRectifyTable(void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, BYTE *buffer, int BufferLength, int *pActualLength, int index);

/*! \fn int EtronDI_GetZDTable(
		void *pHandleEtronDI,
		PDEVSELINFO pDevSelInfo,
		BYTE *buffer,
		int BufferLength,
		int *pActualLength,
		PZDTABLEINFO pZDTableInfo)
	\brief get disparity and Z values from flash
		1. if depth data type is ETronDI_DEPTH_DATA_14_BITS then
			just get Z value from depth buffer 
		2. if depth data type is ETronDI_ZD_TABLE_FILE_SIZE_11_BITS then
			using depth buffer value as a index to get Z value inside ZD table
		3. see GetZValue() of example.c to get Z value from different depth data type
	\param pHandleEtronDI	 the pointer to the initilized EtronDI SDK instance
	\param pDevSelInfo	pointer of device select index
	\param buffer	bufer to store ZD table
	\param BufferLength	input buffer length
	\param pActualLength	actual length has written to buffer
	\param pZDTableInfo	index to identify ZD table and data type for corrresponding depth
	\return success: EtronDI_OK, others: see eSPDI_ErrCode.h
*/
int ETRONDI_API EtronDI_GetZDTable(void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, BYTE *buffer, int BufferLength, int *pActualLength, PZDTABLEINFO pZDTableInfo);

/*! \fn int EtronDI_SetSlaveYOffset(
		void *pHandleEtronDI,
		PDEVSELINFO pDevSelInfo,
		BYTE *buffer,
		int BufferLength,
		int *pActualLength,
		int index)
	\brief set Y offset data
	\param pHandleEtronDI	 the pointer to the initilized EtronDI SDK instance
	\param pDevSelInfo	pointer of device select index
	\param buffer	buffer to store
	\param BufferLength	length of buffer
	\param pActualLength	actual byte of reading
	\param index	index of Y offset file ID
	\return success:EtronDI_OK, others:see eSPDI_ErrCode.h
*/
int ETRONDI_API EtronDI_SetSlaveYOffset(void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, BYTE *buffer, int BufferLength, int *pActualLength, int index);

/*! \fn int EtronDI_SetYOffset(
		void *pHandleEtronDI, 
		PDEVSELINFO pDevSelInfo, 
		BYTE *buffer, 
		int BufferLength, 
		int *pActualLength, 
		int index)
	\brief set Y offset data
	\param pHandleEtronDI	 the pointer to the initilized EtronDI SDK instance
	\param pDevSelInfo	pointer of device select index
	\param buffer	buffer to store
	\param BufferLength	length of buffer
	\param pActualLength	actual byte of reading
	\param index	index of Y offset file ID
	\return success:EtronDI_OK, others:see eSPDI_ErrCode.h
*/
int ETRONDI_API EtronDI_SetYOffset(void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, BYTE *buffer, int BufferLength, int *pActualLength, int index);

/*! \fn int EtronDI_SetSlaveRectifyTable(void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, BYTE *buffer, int BufferLength, int *pActualLength, int index)
	\brief set rectify data to flash, see EtronDI_SetRectifyTable except set
*/
int ETRONDI_API EtronDI_SetSlaveRectifyTable(void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, BYTE *buffer, int BufferLength, int *pActualLength, int index);

/*! \fn int EtronDI_SetRectifyTable(void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, BYTE *buffer, int BufferLength, int *pActualLength, int index)
	\brief set rectify data to flash, see EtronDI_SetRectifyTable except set
*/
int ETRONDI_API EtronDI_SetRectifyTable(void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, BYTE *buffer, int BufferLength, int *pActualLength, int index);

/*! \fn EtronDI_SetZDTable(void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, BYTE *buffer, int BufferLength, int *pActualLength, PZDTABLEINFO pZDTableInfo)
	\brief set disparity and Z values to flash, see EtronDI_GetZDTable except get
*/
int ETRONDI_API EtronDI_SetZDTable(void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, BYTE *buffer, int BufferLength, int *pActualLength, PZDTABLEINFO pZDTableInfo);

/*! \fn int EtronDI_GetRectifyMatLogData(
		void *pHandleEtronDI,
		PDEVSELINFO pDevSelInfo,
		eSPCtrl_RectLogData *pData,
		int index)
	\brief get rectify log data from flash for Puma IC
	\param pHandleEtronDI	 the pointer to the initilized EtronDI SDK instance
	\param pDevSelInfo	pointer of device select index
	\param pData	 rectify log data, its buffer size is 4096 bytes  
		see eSPCtrl_RectLogData for detailed members
	\param index	index to identify rectify log data for corresponding depth
	\return success: EtronDI_OK, others: see eSPDI_ErrCode.h
*/
int ETRONDI_API EtronDI_GetRectifyMatLogData(void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, eSPCtrl_RectLogData *pData, int index);

int ETRONDI_API EtronDI_GetRectifyMatLogDataSlave(void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, eSPCtrl_RectLogData *pData, int index);

/*! \fn EtronDI_SetDepthDataTypeEx(
		void *pHandleEtronDI,
		PDEVSELINFO pDevSelInfo,
		WORD wType)
	\brief set depth data type, 11 bit for disparity data, 14 bit for Z data
		notice: only PUMA type IC can support this setting
	\param pHandleEtronDI	 the pointer to the initilized EtronDI SDK instance
	\param pDevSelInfo	pointer of device select index
	\param wType	depth data type you want to set
	\param int Pid	PID of devicee you want to set,
		see ETronDI_DEPTH_DATA_xxx in EtronDI_O.h
	\output success: EtronDI_OK, others: see eSPDI_ErrCode.h
*/
int ETRONDI_API EtronDI_SetDepthDataTypeEx(void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, WORD wType, int pid);

/*! \fn EtronDI_SetDepthDataType(
		void *pHandleEtronDI,
		PDEVSELINFO pDevSelInfo,
		WORD wType)
	\brief set depth data type, 11 bit for disparity data, 14 bit for Z data
		notice: only PUMA type IC can support this setting
	\param pHandleEtronDI	 the pointer to the initilized EtronDI SDK instance
	\param pDevSelInfo	pointer of device select index
	\param wType	depth data type you want to set,
		see ETronDI_DEPTH_DATA_xxx in EtronDI_O.h
	\output success: EtronDI_OK, others: see eSPDI_ErrCode.h
*/
int ETRONDI_API EtronDI_SetDepthDataType(void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, WORD wType);

/*! \fn int EtronDI_GetDepthDataType(
		void *pHandleEtronDI,
		PDEVSELINFO pDevSelInfo,
		WORD *pwType)
	\brief get current depth data type setting
	\param pHandleEtronDI	 the pointer to the initilized EtronDI SDK instance
	\param pDevSelInfo	pointer of device select index
	\param pwType	pointer of current depth data type in device
	\return success: EtronDI_OK, others: see eSPDI_ErrCode.h
*/
int ETRONDI_API EtronDI_GetDepthDataType(void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, WORD *pwType);

/*! \fn int EtronDI_SetHWPostProcess(void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, bool enable)
	\brief enable or disable internal chip post processing function
	\param pHandleEtronDI	 the pointer to the initilized EtronDI SDK instance
	\param pDevSelInfo	pointer of device select index
	\param enable	set true to enable post-process, or set false to disable post-process
	\return success: EtronDI_OK, others: see eSPDI_ErrCode.h
*/
int ETRONDI_API EtronDI_SetHWPostProcess(void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, bool enable);

/*! \fn int EtronDI_GetHWPostProcess(void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, bool* enable)
	\brief get hardware post processing status
	\param pHandleEtronDI	 the pointer to the initilized EtronDI SDK instance
	\param pDevSelInfo	pointer of device select index
	\param enable	returns current hardware post-process status
	\return success: EtronDI_OK, others: see eSPDI_ErrCode.h	
*/
int ETRONDI_API EtronDI_GetHWPostProcess(void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, bool* enable);

/*! \fn int EtronDI_IsInterleaveDevice(
void *pHandleEtronDI,
PDEVSELINFO pDevSelInfo)
\brief enable or disable interleave function
\param pHandleEtronDI	 the pointer to the initilized EtronDI SDK instance
\param pDevSelInfo	pointer of device select index
\param enable	set true to enable interleave, or set false to disable interleave
\return success: EtronDI_OK, others: see eSPDI_ErrCode.h
*/
int ETRONDI_API EtronDI_EnableInterleave(void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, bool enable);

/*! \fn int EtronDI_EnableSerialCount(
void *pHandleEtronDI,
PDEVSELINFO pDevSelInfo)
\brief enable or disable interleave function
\param pHandleEtronDI	 the pointer to the initilized EtronDI SDK instance
\param pDevSelInfo	pointer of device select index
\param enable	set true to enable Serial-Count, or set false to disable Serial-Count
\return success: EtronDI_OK, others: see eSPDI_ErrCode.h
*/
int ETRONDI_API EtronDI_EnableSerialCount( void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, bool enable );

/*! \fn int EtronDI_EnableSerialCount(
void *pHandleEtronDI,
PDEVSELINFO pDevSelInfo)
\brief enable or disable interleave function
\param pHandleEtronDI	 the pointer to the initilized EtronDI SDK instance
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
\return success: EtronDI_OK, others: see eSPDI_ErrCode.h
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

    PointCloudInfo() { memset( this, NULL, sizeof( PointCloudInfo ) ); }
};

int ETRONDI_API EtronDI_GetPointCloud( void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, unsigned char* ImgColor, int CW, int CH,
                                                                                      unsigned char* ImgDepth, int DW, int DH,
                                                                                      PointCloudInfo* pPointCloudInfo,
                                                                                      unsigned char* pPointCloudRGB, float* pPointCloudXYZ, float Near, float Far );
/*! \fn int EtronDI_FlyingDepthCancellation_D8(
    \fn int EtronDI_FlyingDepthCancellation_D11(
void *pHandleEtronDI,
PDEVSELINFO pDevSelInfo)
\brief enable or disable interleave function
\param pHandleEtronDI	 the pointer to the initilized EtronDI SDK instance
\param pDevSelInfo	pointer of device select index
\param pDImg	depth 8bit buffer, or 11bit buffer
\param width	depth width
\param height	depth height
\return success: EtronDI_OK, others: see eSPDI_ErrCode.h
*/
int ETRONDI_API EtronDI_FlyingDepthCancellation_D8( void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, unsigned char* pdepthD8, int width, int height );

int ETRONDI_API EtronDI_FlyingDepthCancellation_D11( void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, unsigned char* pdepthD11, int width, int height );


/*! \fn int EtronDI_DepthMerge(
void *pHandleEtronDI,
PDEVSELINFO pDevSelInfo)
\brief enable or disable interleave function
\param pHandleEtronDI	 the pointer to the initilized EtronDI SDK instance
\param pDevSelInfo	pointer of device select index
\return success: EtronDI_OK, others: see eSPDI_ErrCode.h
*/

int ETRONDI_API EtronDI_DepthMerge( void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, unsigned char** pDepthBufList, float *pDepthMergeOut,
    unsigned char *pDepthMergeFlag, int nDWidth, int nDHeight, float fFocus, float * pBaseline, float * pWRNear, float * pWRFar, float * pWRFusion, int nMergeNum );

/*! \fn int ETRONDI_API EtronDI_AdjustFocalLengthFromFlash(void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, int width, int height);
*/
int ETRONDI_API EtronDI_AdjustFocalLengthFromFlash(void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, int width, int height);

/*! \fn int ETRONDI_API EtronDI_AdjustFocalLength(void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, int width, int height, int pixelUnit);
*/
int ETRONDI_API EtronDI_AdjustFocalLength(void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, int width, int height, int pixelUnit);

/*! \fn int ETRONDI_API EtronDI_GetDeviceFocalLength(void *pHandleEtronDI, PDEVSELINFO pDevSelInfo,
								 int *pLeftFx, int *pLeftFy, int *pRightFx, int *pRightFy);
*/
int ETRONDI_API EtronDI_GetDeviceFocalLength(void *pHandleEtronDI, PDEVSELINFO pDevSelInfo,
	int *pLeftFx, int *pLeftFy, int *pRightFx, int *pRightFy);

/*! \fn ETRONDI_API EtronDI_GetFlashFocalLength(void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, int width, int height,
								int *pLeftFx, int *pLeftFy, int *pRightFx, int *pRightFy, int *pPixelUnit);
*/
int ETRONDI_API EtronDI_GetFlashFocalLength(void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, int width, int height,
	int *pLeftFx, int *pLeftFy, int *pRightFx, int *pRightFy, int *pPixelUnit);

#ifdef __cplusplus
}
#endif