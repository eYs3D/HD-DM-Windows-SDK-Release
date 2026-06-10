/*! \file eSPDI_ErrCode.h
  	\brief definition of eYs3D SDK error code
  	Copyright:
	This file copyright (C) 2017 by

	eYs3D company

	An unpublished work.  All rights reserved.

	This file is proprietary information, and may not be disclosed or
	copied without the prior permission of eYs3D company.
 */
 
//define Error Code by Wolf 2013/08/30
#define  APC_OK                             0
#define  APC_NoDevice                      -1
#define  APC_NullPtr                       -2
#define  APC_ErrBufLen                     -3
#define  APC_Init_Fail                     -4
#define  APC_NoZDTable                     -5 
#define  APC_READFLASHFAIL                 -6
#define  APC_WRITEFLASHFAIL                -7 
#define  APC_VERIFY_DATA_FAIL              -8
#define  APC_KEEP_DATA_FAIL                -9
#define  APC_RECT_DATA_LEN_FAIL           -10
#define  APC_RECT_DATA_PARSING_FAIL       -11
#define  APC_RET_BAD_PARAM                -12
#define  APC_RET_OPEN_FILE_FAIL           -13
#define  APC_NO_CALIBRATION_LOG           -14
#define  APC_POSTPROCESS_INIT_FAIL        -15
#define  APC_POSTPROCESS_NOT_INIT         -16
#define  APC_POSTPROCESS_FRAME_FAIL       -17
#define  APC_NotSupport                   -18
#define  APC_OpenFileFail                 -19
#define  APC_READ_REG_FAIL                -20
#define  APC_WRITE_REG_FAIL               -21
#define  APC_SET_FPS_FAIL                 -22
#define  APC_VIDEO_RENDER_FAIL            -23
#define  APC_OPEN_DEVICE_FAIL             -24
#define  APC_FIND_DEVICE_FAIL             -25
#define  APC_GET_IMAGE_FAIL               -26
#define  APC_NOT_SUPPORT_RES              -27
#define  APC_CALLBACK_REGISTER_FAIL       -28
#define  APC_DEVICE_NOT_SUPPORT			  -29

// for 3D Scanner +    
#define  APC_ILLEGAL_ANGLE                -30
#define  APC_ILLEGAL_STEP                 -31
#define  APC_ILLEGAL_TIMEPERSTEP          -32
#define  APC_MOTOR_RUNNING                -33 
#define  APC_GETSENSORREG_FAIL            -34
#define  APC_SETSENSORREG_FAIL            -35
#define  APC_READ_X_AXIS_FAIL             -36
#define  APC_READ_Y_AXIS_FAIL             -37
#define  APC_READ_Z_AXIS_FAIL             -38
#define  APC_READ_PRESS_DATA_FAIL         -39
#define  APC_READ_TEMPERATURE_FAIL        -40
#define  APC_RETURNHOME_RUNNING           -41
#define  APC_MOTOTSTOP_BY_HOME_INDEX      -42
#define  APC_MOTOTSTOP_BY_PROTECT_SCHEME  -43
#define  APC_MOTOTSTOP_BY_NORMAL          -44
#define  APC_ILLEGAL_FIRMWARE_VERSION     -45
#define  APC_ILLEGAL_STEPPERTIME          -46
// for 3D Scanner - 

// For AEAWB + 2015/01/28 by Wolf
#define  APC_GET_PU_PROP_VAL              -50
#define  APC_SET_PU_PROP_VAL              -51
#define  APC_GET_CT_PROP_VAL              -52
#define  APC_SET_CT_PROP_VAL              -53
// For AEAWB - 2015/01/28 by Wolf 

// for Dewarping + Stitching +
#define  APC_INVALID_USERDATA             -70
#define  APC_MAP_LUT_FAIL                 -71
#define  APC_APPEND_TO_FILE_FRONT_FAIL    -72
// for Dewarping + Stitching -

#define APC_TOO_MANY_DEVICE               -80
#define APC_ACCESS_MP4_EXTRA_DATA_FAIL    -81