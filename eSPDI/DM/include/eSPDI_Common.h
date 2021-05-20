/*! \file eSPDI_Common.h
  	\brief Etron SDK API export functions, data structure and variable definition
  	\copyright This file copyright (C) 2017 by eYs3D an Etron company

	\details An unpublished work.  All rights reserved.
	This file is proprietary information, and may not be disclosed or
	copied without the prior permission of eYs3D an Etron company.
 */
#pragma once

#ifndef ETRONDI_API
#ifdef __WEYE__
#define ETRONDI_API
#else
#ifdef ETRONDI_EXPORTS
#define ETRONDI_API __declspec(dllexport)
#else
#define ETRONDI_API __declspec(dllimport)
#endif
#endif
#endif

#ifndef BYTE
typedef unsigned char BYTE;
#endif //BYTE

#ifndef WORD
typedef unsigned short WORD;
#endif //WORD

#ifndef WCHAR
typedef wchar_t WCHAR;
#endif //WCHAR

#ifndef BOOL
typedef signed int BOOL;
#endif //BOOL

//
// C++ compatibility
//
#ifdef  __cplusplus
extern "C" {
#endif

#ifndef CALLBACK
#define CALLBACK __stdcall
#endif //CALLBACK


#define ETronDI_MAX_STREAM_COUNT 64
#define ETronDI_MAX_DEPTH_STREAM_COUNT 8
#pragma pack(push, 1)
typedef struct tagETRONDI_STREAM_INFO {
	int		nWidth;
	int		nHeight;
	BOOL	bFormatMJPG;
} ETRONDI_STREAM_INFO, *PETRONDI_STREAM_INFO;
#pragma pack(pop)

#ifndef WM_MYMSG_NOTICE_CAPTURE
#define WM_MYMSG_NOTICE_CAPTURE	(WM_USER+101)
#endif

#include "eSPDI_ErrCode.h"

/* Etron Stream Index */
#define ETron_Stream_Color 0
#define ETron_Stream_Track 4
#define ETron_Stream_Kolor 5

/* Etron VID */
#define ETronDI_VID_0x1E4E 0x1E4E

/* Etron PID */
#define ETronDI_PID_8029    0x0568
#define ETronDI_PID_8030    ETronDI_PID_8029
#define ETronDI_PID_8039    ETronDI_PID_8029
#define ETronDI_PID_8031    0x0117
#define ETronDI_PID_8032    0x0118
#define ETronDI_PID_8036    0x0120
#define ETronDI_PID_8037    0x0121
#define ETronDI_PID_8038    0x0124
#define ETronDI_PID_8038_M0 ETronDI_PID_8038
#define ETronDI_PID_8038_M1 0x0147
#define ETronDI_PID_8040W   0x0130
#define ETronDI_PID_8040S   0x0131
#define ETronDI_PID_8040S_K 0x0149
#define ETronDI_PID_8041    0x0126
#define ETronDI_PID_8042    0x0127
#define ETronDI_PID_8043    0x0128
#define ETronDI_PID_8044    0x0129
#define ETronDI_PID_8045K   0x0134
#define ETronDI_PID_8046K   0x0135
#define ETronDI_PID_8051    0x0136
#define ETronDI_PID_8052    0x0137
#define ETronDI_PID_8053    0x0138
#define ETronDI_PID_8054    0x0139
#define ETronDI_PID_8054_K  0x0143
#define ETronDI_PID_8059    0x0146
#define ETronDI_PID_8060    0x0152
#define ETronDI_PID_8060_K  0x0150
#define ETronDI_PID_8060_T  0x0151
#define ETronDI_PID_AMBER   0x0112
#define ETronDI_PID_SALLY   0x0158
#define ETronDI_PID_8062    0x0162
#define ETronDI_PID_Hypatia 0x0160

#define BIT_SET(a,b) ((a) |= (1<<(b)))
#define BIT_CLEAR(a,b) ((a) &= ~(1<<(b)))
#define BIT_FLIP(a,b) ((a) ^= (1<<(b)))
#define BIT_CHECK(a,b) ((a) & (1<<(b)))

#define FG_Address_1Byte 0x01
#define FG_Address_2Byte 0x02
#define FG_Value_1Byte   0x10
#define FG_Value_2Byte   0x20

// For Depth Data Type - 2016/12/14 by Sean
#define ETronDI_DEPTH_DATA_DEFAULT						0
#define ETronDI_DEPTH_DATA_OFF_RAW						0 // raw (depth off, only raw color)
#define ETronDI_DEPTH_DATA_8_BITS						1 // rectify
#define ETronDI_DEPTH_DATA_14_BITS						2 // rectify
#define ETronDI_DEPTH_DATA_8_BITS_x80					3 // rectify
#define ETronDI_DEPTH_DATA_11_BITS						4 // rectify
#define ETronDI_DEPTH_DATA_OFF_RECTIFY					5 // rectify (depth off, only rectify color)
#define ETronDI_DEPTH_DATA_8_BITS_RAW					6 // raw
#define ETronDI_DEPTH_DATA_14_BITS_RAW					7 // raw
#define ETronDI_DEPTH_DATA_8_BITS_x80_RAW				8 // raw
#define ETronDI_DEPTH_DATA_11_BITS_RAW					9 // raw
#define ETronDI_DEPTH_DATA_8_BITS_COMBINED_RECTIFY		10// multi-baseline
#define ETronDI_DEPTH_DATA_14_BITS_COMBINED_RECTIFY		11// multi-baseline
#define ETronDI_DEPTH_DATA_8_BITS_x80_COMBINED_RECTIFY	12// multi-baseline
#define ETronDI_DEPTH_DATA_11_BITS_COMBINED_RECTIFY		13// multi-baseline

// For Inter-Leave-Mode Depth Data Type
#define ETronDI_DEPTH_DATA_ILM_DEFAULT						16
#define ETronDI_DEPTH_DATA_ILM_OFF_RAW						16 // raw (depth off, only raw color)
#define ETronDI_DEPTH_DATA_ILM_8_BITS						17 // rectify
#define ETronDI_DEPTH_DATA_ILM_14_BITS						18 // rectify
#define ETronDI_DEPTH_DATA_ILM_8_BITS_x80					19 // rectify
#define ETronDI_DEPTH_DATA_ILM_11_BITS						20 // rectify
#define ETronDI_DEPTH_DATA_ILM_OFF_RECTIFY					21 // rectify (depth off, only rectify color)
#define ETronDI_DEPTH_DATA_ILM_8_BITS_RAW					22 // raw
#define ETronDI_DEPTH_DATA_ILM_14_BITS_RAW					23 // raw
#define ETronDI_DEPTH_DATA_ILM_8_BITS_x80_RAW				24 // raw
#define ETronDI_DEPTH_DATA_ILM_11_BITS_RAW					25 // raw
#define ETronDI_DEPTH_DATA_ILM_8_BITS_COMBINED_RECTIFY		26// multi-baseline
#define ETronDI_DEPTH_DATA_ILM_14_BITS_COMBINED_RECTIFY		27// multi-baseline
#define ETronDI_DEPTH_DATA_ILM_8_BITS_x80_COMBINED_RECTIFY	28// multi-baseline
#define ETronDI_DEPTH_DATA_ILM_11_BITS_COMBINED_RECTIFY		29// multi-baseline


// For Flash Read/Write
// Firmware (size in KBytes)
#define ETronDI_READ_FLASH_TOTAL_SIZE			128
#define ETronDI_READ_FLASH_FW_PLUGIN_SIZE		104
#define ETronDI_WRITE_FLASH_TOTAL_SIZE			128
#define ETronDI_READ_FLASH_TOTAL_SIZE_256		256
#define ETronDI_WRITE_FLASH_TOTAL_SIZE_256	256


// PlugIn data (size in bytes)
#define ETronDI_Y_OFFSET_FILE_ID_0				30
#define ETronDI_Y_OFFSET_FILE_SIZE			    256
#define ETronDI_RECTIFY_FILE_ID_0				40
#define ETronDI_RECTIFY_FILE_SIZE				1024
#define ETronDI_ZD_TABLE_FILE_ID_0				50
#define ETronDI_CALIB_LOG_FILE_ID_0				240
#define ETronDI_CALIB_LOG_FILE_SIZE				4096
#define ETronDI_USER_DATA_FILE_ID_0				200
#define ETronDI_USER_DATA_FILE_SIZE_0			1024
#define ETronDI_USER_DATA_FILE_SIZE_1			4096
#define ETronDI_USER_DATA_FILE_SIZE_2			128
#define ETronDI_USER_DATA_FILE_SIZE_3			1024
#define ETronDI_USER_DATA_FILE_SIZE_4			4096

//================================================================
//
// Property Type
//
#define PROP_TYPE_PU	0
#define PROP_TYPE_CT	1
//
// PU Property ID
//
#define    PU_PROPERTY_ID_BRIGHTNESS                0
#define    PU_PROPERTY_ID_CONTRAST                  1
#define    PU_PROPERTY_ID_HUE                       2
#define    PU_PROPERTY_ID_SATURATION                3
#define    PU_PROPERTY_ID_SHARPNESS                 4
#define    PU_PROPERTY_ID_GAMMA                     5
#define    PU_PROPERTY_ID_COLORENABLE               6
#define    PU_PROPERTY_ID_WHITEBALANCE              7
#define    PU_PROPERTY_ID_BACKLIGHT_COMPENSATION    8
#define    PU_PROPERTY_ID_GAIN                      9
#define    PU_PROPERTY_ID_DIGITAL_MULTIPLIER        10
#define    PU_PROPERTY_ID_DIGITAL_MULTIPLIER_LIMIT  11
#define    PU_PROPERTY_ID_WHITEBALANCE_COMPONENT    12
#define    PU_PROPERTY_ID_POWERLINE_FREQUENCY       13
//
// CT Property ID
//
#define    CT_PROPERTY_ID_PAN               			0
#define    CT_PROPERTY_ID_TILT                         1
#define    CT_PROPERTY_ID_ROLL                         2
#define    CT_PROPERTY_ID_ZOOM                         3
#define    CT_PROPERTY_ID_EXPOSURE                     4
#define    CT_PROPERTY_ID_IRIS                         5
#define    CT_PROPERTY_ID_FOCUS                        6
#define    CT_PROPERTY_ID_SCANMODE                     7
#define    CT_PROPERTY_ID_PRIVACY                      8
#define    CT_PROPERTY_ID_PANTILT                      9
#define    CT_PROPERTY_ID_PAN_RELATIVE                 10
#define    CT_PROPERTY_ID_TILT_RELATIVE                11
#define    CT_PROPERTY_ID_ROLL_RELATIVE                12
#define    CT_PROPERTY_ID_ZOOM_RELATIVE                13
#define    CT_PROPERTY_ID_EXPOSURE_RELATIVE            14
#define    CT_PROPERTY_ID_IRIS_RELATIVE                15
#define    CT_PROPERTY_ID_FOCUS_RELATIVE               16
#define    CT_PROPERTY_ID_PANTILT_RELATIVE             17
#define    CT_PROPERTY_ID_AUTO_EXPOSURE_PRIORITY       19  

//================================================================


typedef struct tagZDTableInfo
{
    int nIndex;
    int nDataType;
} ZDTABLEINFO, *PZDTABLEINFO;

/**
 * \brief eSPCtrl_RectLogData
 *
 * Rectified log data structure
 */
typedef struct eSPCtrl_RectLogData
{
    union {
        BYTE uByteArray[1024];
        struct {
            WORD	InImgWidth;
            WORD	InImgHeight;
            WORD	OutImgWidth;
            WORD	OutImgHeight;
            int		RECT_ScaleEnable;
            int		RECT_CropEnable;
            WORD	RECT_ScaleWidth;
            WORD	RECT_ScaleHeight;
            float	CamMat1[9];
            float	CamDist1[8];
            float	CamMat2[9];
            float	CamDist2[8];
            float	RotaMat[9];
            float	TranMat[3];
            float	LRotaMat[9];
            float	RRotaMat[9];
            float	NewCamMat1[12];
            float	NewCamMat2[12];
            WORD	RECT_Crop_Row_BG;
            WORD	RECT_Crop_Row_ED;
            WORD	RECT_Crop_Col_BG_L;
            WORD	RECT_Crop_Col_ED_L;
            BYTE	RECT_Scale_Col_M;
            BYTE	RECT_Scale_Col_N;
            BYTE	RECT_Scale_Row_M;
            BYTE	RECT_Scale_Row_N;
            float	RECT_AvgErr;
            WORD	nLineBuffers;
            float ReProjectMat[16];
        };
    };
} eSPCtrl_RectLogData;

/**
 * \brief ParaLUT
 *
 * Spherical look-up table conversion parameters
 */
typedef struct ParaLUT
{
    long long file_ID_header;       //!< [00]-[000] File ID header : 2230
    long long file_ID_version;      //!< [01]-[008] File ID version : 4
    double    FOV;                  //!< [02]-[016] Field of view with degree
    long long semi_FOV_pixels;      //!< [03]-[024] Pixels for semi-FOV
    long long img_src_cols;         //!< [04]-[032] Width for source image (single image)
    long long img_src_rows;         //!< [05]-[040] Height for source image
    double    img_L_src_col_center; //!< [06]-[048] Center of width for L side source image
    double    img_L_src_row_center; //!< [07]-[056] Center of height for L side source image
    double    img_R_src_col_center; //!< [08]-[064] Center of width for R side source image
    double    img_R_src_row_center; //!< [09]-[072] Center of height for R side source image
    double    img_L_rotation;       //!< [10]-[080] Rotation for L side image
    double    img_R_rotation;       //!< [11]-[088] Rotation for R side image
    double    spline_control_v1;    //!< [12]-[096] Spline control value for row = DIV x 0 pixel, DIV = rows/6
    double    spline_control_v2;    //!< [13]-[104] Spline control value for row = DIV x 1 pixel, DIV = rows/6
    double    spline_control_v3;    //!< [14]-[112] Spline control value for row = DIV x 2 pixel, DIV = rows/6
    double    spline_control_v4;    //!< [15]-[120] Spline control value for row = DIV x 3 pixel, DIV = rows/6
    double    spline_control_v5;    //!< [16]-[128] Spline control value for row = DIV x 4 pixel, DIV = rows/6
    double    spline_control_v6;    //!< [17]-[136] Spline control value for row = DIV x 5 pixel, DIV = rows/6
    double    spline_control_v7;    //!< [18]-[144] Spline control value for row = DIV x 6 pixel, DIV = rows/6
    long long img_dst_cols;         //!< [19]-[152] Width for output image (single image), according to "Original" parameters  
    long long img_dst_rows;         //!< [20]-[160] Height for output image, according to "Original" parameters
    long long img_L_dst_shift;      //!< [21]-[168] Output L side image shift in row 
    long long img_R_dst_shift;      //!< [22]-[176] Output R side image shift in row  
    long long img_overlay_LR;       //!< [23]-[184] Overlay between L/R in pixels, far field, (YUV must be even) 
    long long img_overlay_RL;       //!< [24]-[192] Overlay between R/L in pixels, far field, (YUV must be even) 
    long long img_stream_cols;      //!< [25]-[200] Output image stream of cols 
    long long img_stream_rows;      //!< [26]-[208] Output image stream of rows 
    long long video_stream_cols;    //!< [27]-[216] Output video stream of cols 
    long long video_stream_rows;    //!< [28]-[224] Output video stream of rows 
    long long usb_type;             //!< [29]-[232] 2 for usb2, 3 for usb3
    long long img_type;             //!< [30]-[240] 1 for yuv422, 2 for BGR, 3 for RGB
    long long lut_type;             //!< [31]-[248] Output LUT tye @ref eys::LutModes
    long long blending_type;        //!< [32]-[256] 0 for choosed by function, 1 for alpha-blending, 2 for Laplacian pyramid blending
    double    overlay_ratio;        //!< [33]-[264] far field overlay value is equal to img_overlay_LR(RL)  = overlay_value + overlay_ratio 

    long long serial_number_date0;  //!< [34]-[272] 8 bytes, yyyy-mm-dd
    long long serial_number_date1;  //!< [35]-[280] 8 bytes, hh-mm-ss-xxx, xxx for machine number

    double    unit_sphere_radius;   //!< [36]-[288] Original : Unit spherical radius for dewarping get x and y
    double    min_col;              //!< [37]-[296] Original : Parameters of min position of image width
    double    max_col;              //!< [38]-[304] Original : Parameters of max position of image width
    double    min_row;              //!< [39]-[312] Original : Parameters of min position of image height
    double    max_row;              //!< [40]-[320] Original : Parameters of max position of image height

    long long AGD_LR;               //!< [41]-[328] Err : Average gray-level value discrepancy at LR boundary
    long long AGD_RL;               //!< [42]-[336] Err : Average gray-level value discrepancy at RL boundary

    long long out_img_resolution;   //!< [43]-[344] Set output resolution @ref eys::ImgResolutionModes 
    long long out_lut_cols;         //!< [44]-[352] Output side-by-side lut width, according to the set of out_img_resolution
    long long out_lut_rows;         //!< [45]-[360] Output lut height, according to the set of out_img_resolution
    long long out_lut_cols_eff;     //!< [46]-[368] Output effective pixels in out_lut_cols, 0 is for all
    long long out_lut_rows_eff;     //!< [47]-[376] Output effecitve pixels in out_lut_rows, 0 is for all
    long long out_img_cols;         //!< [48]-[384] Output side-by-side image width after dewarping and stitching, according to the set of out_img_resolution
    long long out_img_rows;         //!< [49]-[392] Output image height, according to the set of out_img_resolution
    long long out_overlay_LR;       //!< [50]-[340] Output L/R overlay value, according to the set of out_img_resolution
    long long out_overlay_RL;       //!< [51]-[408] Output R/L overlay value, according to the set of out_img_resolution
    long long reserve[44];          //!< [52]-[416] Reserve 44 parameter to use
    BYTE      serial_number[256];
} PARALUT, *PPARALUT;

struct EtronDIImageType
{
    enum Value
    {
        IMAGE_UNKNOWN = -1,
        COLOR_YUY2 = 0,
        COLOR_RGB24, 
        COLOR_MJPG, 
        DEPTH_8BITS = 100,
        DEPTH_8BITS_0x80,
        DEPTH_11BITS,
        DEPTH_14BITS
    };

    static bool IsImageColor(EtronDIImageType::Value type)
    {
        return (type == COLOR_YUY2 || type == COLOR_RGB24 || type == COLOR_MJPG);
    }

    static bool IsImageDepth(EtronDIImageType::Value type)
    {
        return (type != IMAGE_UNKNOWN && !IsImageColor(type));
    }

    static EtronDIImageType::Value DepthDataTypeToDepthImageType(WORD dataType)
    {
        switch (dataType)
        {
        case ETronDI_DEPTH_DATA_ILM_8_BITS:
        case ETronDI_DEPTH_DATA_ILM_8_BITS_RAW:
        case ETronDI_DEPTH_DATA_8_BITS:
        case ETronDI_DEPTH_DATA_8_BITS_RAW:
            return EtronDIImageType::DEPTH_8BITS;
        case ETronDI_DEPTH_DATA_ILM_8_BITS_x80:
        case ETronDI_DEPTH_DATA_ILM_8_BITS_x80_RAW:
        case ETronDI_DEPTH_DATA_8_BITS_x80:
        case ETronDI_DEPTH_DATA_8_BITS_x80_RAW:
            return EtronDIImageType::DEPTH_8BITS_0x80;
        case ETronDI_DEPTH_DATA_ILM_11_BITS:
        case ETronDI_DEPTH_DATA_ILM_11_BITS_RAW:
        case ETronDI_DEPTH_DATA_11_BITS:
        case ETronDI_DEPTH_DATA_11_BITS_RAW:
        case ETronDI_DEPTH_DATA_ILM_11_BITS_COMBINED_RECTIFY:
        case ETronDI_DEPTH_DATA_11_BITS_COMBINED_RECTIFY:
            return EtronDIImageType::DEPTH_11BITS;
        case ETronDI_DEPTH_DATA_ILM_14_BITS:
        case ETronDI_DEPTH_DATA_ILM_14_BITS_RAW:
        case ETronDI_DEPTH_DATA_14_BITS:
        case ETronDI_DEPTH_DATA_14_BITS_RAW:
        case ETronDI_DEPTH_DATA_ILM_14_BITS_COMBINED_RECTIFY:
		case ETronDI_DEPTH_DATA_14_BITS_COMBINED_RECTIFY:
            return EtronDIImageType::DEPTH_14BITS;
        default: return EtronDIImageType::IMAGE_UNKNOWN;
        }
    }
};

struct EtronDIDepthSwitch
{
    enum Value
    {
        Depth0 = 0x01, 
        Depth1 = 0x02, 
        Depth2 = 0x04
    };

    static bool IsOn(EtronDIDepthSwitch::Value target, int setting)
    {
        return ((target & setting) != 0);
    }
};
/*! \fn void(*EtronDI_ImgCallbackFn)(EtronDIImageType::Value imgType, int imgId, unsigned char* imgBuf, int imgSize,
    int width, int height, int serialNumber, void* pParam)
\brief Callback function when video or data is ready
\param pid	product id of the USB device
\param vid	vender id of the USB device
\param bAttached TRUE if this is a USB device attached event, otherwise, it is a detached event.
\param pData user defined data to pass to the callback function
\return none
*/
typedef void(*EtronDI_ImgCallbackFn)(EtronDIImageType::Value imgType, int imgId, unsigned char* imgBuf, int imgSize,
    int width, int height, int serialNumber, void* pParam);
	
/**
 * \enum ETRONDI_DEVICE_TYPE
 *
 * chip enum value
 */
typedef enum {
    OTHERS = 0,		/**< Other */
    AXES1,			/**< AXIS1 */
    PUMA,			/**< PUMA */
    PLUM
}ETRONDI_DEVICE_TYPE;

struct EtronDI_SensorMode
{
    enum Value
    {
        Sensor1 = 0,
        Sensor2 = 1,
        SensorAll = 2, 
        Sensor3 = 3, 
        Sensor4 = 4
    };
};
/**
 * \brief DEVINFORMATION
 *
 * device information
 */
typedef struct tagDEVINFORMATION {
    unsigned short wPID;			/**< product ID */
									/**<	<table>
									<caption id="multi_row">PID List</caption>
									<tr><th>Chip Name      <th>Chip ID             <th>PID
									<tr><td rowspan="5">AXES1 <td td rowspan="5">0x18 <td>0x0568
									<tr> <td>0x0668
									<tr> <td>0x0113
									<tr> <td>0x0115
									<tr> <td>0x0116
									<tr><td>KIWI <td>0x1C <td>0x0118
									<tr><td rowspan="2">PUMA <td td rowspan="2">0x15 <td>0x0112
									<tr> <td>0x0120
									</table>
									*/
    unsigned short wVID;			/**< vender ID, 0x1E4E for EtronDI device */
    char *strDevName;				/**< pointer to device name stored inside the SDK*/
    unsigned short  nChipID;		/**< chip ID, 0x18 for AXES1, 0x1C for KIWI, 0x15 for PUMA */
    unsigned short  nDevType;		/**< chip enum value, @see ETRONDI_DEVICE_TYPE */
	unsigned short wUsbNode;		/**< USB Node */
} DEVINFORMATION;


/**
 * \class DEVINFORMATIONEX
 *
 * extended device information class
 */
class DEVINFORMATIONEX 
{
public:
    DEVINFORMATIONEX()
    {
        wPID = wVID = nChipID = 0;
        nDevType = OTHERS;
        strDevName[0] = '\0';
		wUsbNode = -1;
    }

    DEVINFORMATIONEX& operator=(const DEVINFORMATIONEX& rhs)
    {
        wPID = rhs.wPID;
        wVID = rhs.wVID;
        strcpy_s(strDevName, rhs.strDevName);
        nChipID = rhs.nChipID;
        nDevType = rhs.nDevType;
		wUsbNode = rhs.wUsbNode;

        return *this;
    }

    DEVINFORMATIONEX& operator=(const DEVINFORMATION& rhs)
    {
        wPID = rhs.wPID;
        wVID = rhs.wVID;
        strcpy_s(strDevName, rhs.strDevName);
        nChipID = rhs.nChipID;
        nDevType = rhs.nDevType;
		wUsbNode = rhs.wUsbNode;

        return *this;
    }

    DEVINFORMATIONEX(const DEVINFORMATIONEX& rhs)
    {
        *this = rhs;
    }

    unsigned short wPID;		/**< product ID */
									/**<	<table>
									<caption id="multi_row">PID List</caption>
									<tr><th>Chip Name      <th>Chip ID             <th>PID
									<tr><td rowspan="5">AXES1 <td td rowspan="5">0x18 <td>0x0568
									<tr> <td>0x0668
									<tr> <td>0x0113
									<tr> <td>0x0115
									<tr> <td>0x0116
									<tr><td>KIWI <td>0x1C <td>0x0118
									<tr><td rowspan="2">PUMA <td td rowspan="2">0x15 <td>0x0112
									<tr> <td>0x0120
									</table>
									*/
    unsigned short wVID;		/**< vender ID, 0x1E4E for EtronDI device */
    char strDevName[512];		/**< device name */
    unsigned short nChipID;		/**< chip ID, 0x18 for AXES1, 0x1C for KIWI, 0x15 for PUMA */
    unsigned short nDevType;	/**< chip enum value, see ETRONDI_DEVICE_TYPE */
	unsigned short wUsbNode;		/**< USB Node */
};
/*! \struct DEVSELINFO*/
typedef struct tagDEVSEL
{
  int index;					/**< device index */
} DEVSELINFO, *PDEVSELINFO;

/*! \enum User data section id */
typedef enum
{
  USERDATA_SECTION_0 = 0,	/**< Section 0 */
  USERDATA_SECTION_1,		/**< Section 1 */
  USERDATA_SECTION_2,		/**< Section 2 */
  USERDATA_SECTION_3,		/**< Section 3 */
  USERDATA_SECTION_4,		/**< Section 4 */
  USERDATA_SECTION_5,		/**< Section 5 */
  USERDATA_SECTION_6,		/**< Section 6 */
  USERDATA_SECTION_7,		/**< Section 7 */
  USERDATA_SECTION_8,		/**< Section 8 */
  USERDATA_SECTION_9,		/**< Section 9 */
  USERDATA_SECTION_10,		/**< Section 10 */
  USERDATA_SECTION_NUM      /**< Total Section Number */
} USERDATA_SECTION_INDEX;

// for total and fw+plugin read/write +
typedef enum
{
    Total = 0,
    FW_PLUGIN,
    Total_Slave,	/* total for first slave device */
    FW_PLUGIN_Slave	/* fw_plugin for first slave device */
} FLASH_DATA_TYPE;

typedef enum
{
    USB_PORT_TYPE_2_0 = 2,
    USB_PORT_TYPE_3_0,
    USB_PORT_TYPE_UNKNOW
} USB_PORT_TYPE;

typedef struct tagKEEP_DATA_CTRL {
	bool  bIsSerialNumberKeep;
	bool  bIsSensorPositionKeep;
	bool  bIsRectificationTableKeep;
	bool  bIsZDTableKeep;
	bool  bIsCalibrationLogKeep;
} KEEP_DATA_CTRL;
// for total and fw+plugin read/write -

/*! \fn int EtronDI_Init(
		void **ppHandleEtronDI,
		bool bIsLogEnabled)
	\brief entry point of Etron camera SDK. This API allocates resource and find all the eSPI camera devices connected to the system.
	\param ppHandleEtronDI	a pointer of pointer to receive EtronDI SDK instance
	\param bIsLogEnabled	set to true to generate log file, named log.txt in current folder
	\return success: none negative integer to indicate numbers of devices found in the system.
			
*/
int  ETRONDI_API EtronDI_Init( void **ppHandleEtronDI, bool bIsLogEnabled);

/*! \fn int EtronDI_Init2(
		void **ppHandleEtronDI,
		bool bIsLogEnabled,
		bool bEnableAutoRestart)
	\brief entry point of Etron camera SDK. This API allocates resource and find all the eSPI camera devices connected to the system.
	\param ppHandleEtronDI	a pointer of pointer to receive EtronDI SDK instance
	\param bIsLogEnabled	set to true to generate log file, named log.txt in current folder
	\param bEnableAutoRestart	set true to auto-restart the device if the device was detached and attached again.
	\return success: none negative integer to indicate numbers of devices found in the system.
	\note Calls EtronDI_Init or EtronDI_Init2 to initilize the EtronDI SDK. EtronDI_Init2 adds the auto-restart function to the initilization options.
	      If you call EtronDI_Init, the bEnableAutoRestart is set as disabled.
*/
int  ETRONDI_API EtronDI_Init2( void **ppHandleEtronDI, bool bIsLogEnabled, bool bAutoRestart);

/*! \fn void (*EtronDI_DeviceEventFn)(UINT pid, UINT vid, BOOL bAttached, void* pData)
\brief Callback function to receive any USB capture device attachment or detachment events
\param pid	product id of the USB device
\param vid	vender id of the USB device
\param bAttached TRUE if this is a USB device attached event, otherwise, it is a detached event.
\param pData user defined data to pass to the callback function
\return none
*/
#ifndef EtronDI_DeviceEventFn_
typedef void(*EtronDI_DeviceEventFn)(UINT pid, UINT vid, BOOL bAttached, void* pData);
#define EtronDI_DeviceEventFn_
#endif

/*! \fn int EtronDI_RegisterDeviceEvents(
		void *pHandleEtronDI,
		EtronDI_DeviceEventFn cbFunc, void *pData)
	\brief Register the USB device plug or unplug events. Any USB capture device attachment or detachment events will call the callback function cbFunc
	\param pHandleEtronDI	a pointer to EtronDI SDK instance
	\param cbFunc a callback function of type #EtronDI_DeviceEventFn that will receive USB cappure device events when the device is attached or detached.
	\param pData user defined data which will send to the callback function
	\return success: EtronDI_OK, others: see eSPDI_ErrCode.h
*/
int  ETRONDI_API EtronDI_RegisterDeviceEvents(void *pHandleEtronDI, EtronDI_DeviceEventFn cbFunc, void *pData);

/*! \fn void EtronDI_Release(
		void **ppHandleEtronDI)
	\brief release all resource that EtronDI_Init had allocated
	\param ppHandleEtronDI	pointer of the pointer to the initilized EtronDI SDK instance.
	\return none
	\note the pointer to ppHandleEtronDI will be set to NULL when this call returns successfully. 
*/
void ETRONDI_API EtronDI_Release( void **ppHandleEtronDI);

/*! \fn int EtronDI_FindDevice(
		void *pHandleEtronDI)
	\brief find out all Etron USB devices by PID, VID and ChipID, also remember device types
	\param pHandleEtronDI the pointer to the initilized EtronDI SDK instance
	\return success: EtronDI_OK, others: see eSPDI_ErrCode.h
 */
int  ETRONDI_API EtronDI_FindDevice( void *pHandleEtronDI);

/*! \fn int EtronDI_RefreshDevice(
		void *pHandleEtronDI)
	\brief refresh all Etron UVC devices
	\param pHandleEtronDI	 the pointer to the initilized EtronDI SDK instance
	\return success: EtronDI_OK, others: see eSPDI_ErrCode.h
 */
int  ETRONDI_API EtronDI_RefreshDevice( void *pHandleEtronDI);

/*! \fn int EtronDI_GetDeviceNumber(
		void *pHandleEtronDI)
	\brief get Etron USB device numbers
	\param pHandleEtronDI	 the pointer to the initilized EtronDI SDK instance
	\return number of Etron device
*/
int  ETRONDI_API EtronDI_GetDeviceNumber( void *pHandleEtronDI);

/*! \fn int EtronDI_GetDeviceInfo(
		void *pHandleEtronDI,
		PDEVSELINFO pDevSelInfo,
		DEVINFORMATION* pdevinfo)
	\brief get informations of Etron UVC devices, see @DEVINFORMATION
	\param pHandleEtronDI	 the pointer to the initilized EtronDI SDK instance
	\param pDevSelInfo	pointer of device select index
	\param pdevinfo	pointer of device information
	\return success: EtronDI_OK, others: see eSPDI_ErrCode.h
*/
int  ETRONDI_API EtronDI_GetDeviceInfo( void *pHandleEtronDI, PDEVSELINFO pDevSelInfo ,DEVINFORMATION* pdevinfo);

/*! \fn int EtronDI_GetDeviceInfoEx(
		void *pHandleEtronDI,
		PDEVSELINFO pDevSelInfo,
		DEVINFORMATIONEX* pdevinfo)
	\brief get target device info,
	\param pHandleEtronDI	 the pointer to the initilized EtronDI SDK instance
	\param pDevSelInfo	pointer of device select index
	\param pdevinfo	pointer of buffer to store DEVINFORMATIONEX
	\return success: EtronDI_OK, others: see eSPDI_ErrCode.h
*/
int  ETRONDI_API EtronDI_GetDeviceInfoEx( void *pHandleEtronDI, PDEVSELINFO pDevSelInfo ,DEVINFORMATIONEX* pdevinfo);

// register APIs +
/*! \fn int EtronDI_GetSlaveSensorRegister(
		void *pHandleEtronDI,
		PDEVSELINFO pDevSelInfo,
		int nId,
		unsigned short address,
		unsigned short *pValue,
		int flag,
		int nSensorMode)
	\brief get value from sensor register
	\param pHandleEtronDI	CEtronDI handler
	\param pDevSelInfo	pointer of device select index
	\param nId	sensor slave address. see #SENSOR_TYPE_NAME enum definition
	\param address	register address
	\param pValue	pointer of value got from register address
	\param flag	address and value data length(2 or 1 byte)
		ie FG_Address_2Byte | FG_Value_2Byte is 2 byte address and 2 byte value
		#define FG_Address_1Byte 0x01
		#define FG_Address_2Byte 0x02
		#define FG_Value_1Byte   0x10
		#define FG_Value_2Byte   0x20
	\param nSensorMode	sensor mode(sensor A, B or Both)
		A is 0, B is 1, Both is 2
	\return success: EtronDI_OK, others: see eSPDI_ErrCode.h
*/
int ETRONDI_API EtronDI_GetSlaveSensorRegister(void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, int nId, unsigned short address, unsigned short *pValue, int flag, int nSensorMode);

/*! \fn int EtronDI_GetSensorRegister(
		void *pHandleEtronDI,
		PDEVSELINFO pDevSelInfo,
		int nId,
		unsigned short address,
		unsigned short *pValue,
		int flag,
		int nSensorMode)
	\brief get value from sensor register
	\param pHandleEtronDI	CEtronDI handler
	\param pDevSelInfo	pointer of device select index
	\param nId	sensor slave address. see #SENSOR_TYPE_NAME enum definition
	\param address	register address
	\param pValue	pointer of value got from register address
	\param flag	address and value data length(2 or 1 byte)
		ie FG_Address_2Byte | FG_Value_2Byte is 2 byte address and 2 byte value
		#define FG_Address_1Byte 0x01
		#define FG_Address_2Byte 0x02
		#define FG_Value_1Byte   0x10
		#define FG_Value_2Byte   0x20
	\param nSensorMode	sensor mode(sensor A, B or Both)
		A is 0, B is 1, Both is 2
	\return success: EtronDI_OK, others: see eSPDI_ErrCode.h
*/
int ETRONDI_API EtronDI_GetSensorRegister ( void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, int nId, unsigned short address, unsigned short *pValue, int flag, int nSensorMode);

/*! \fn int EtronDI_SetSlaveSensorRegister(
		void *pHandleEtronDI,
		PDEVSELINFO pDevSelInfo,
		int nId,
		unsigned short address,
		unsigned short *pValue,
		int flag,
		int nSensorMode)
	\brief set value from sensor register
	\param pHandleEtronDI	CEtronDI handler
	\param pDevSelInfo	pointer of device select index
	\param nId	sensor slave address. see #SENSOR_TYPE_NAME enum definition
	\param address	register address
	\param pValue	pointer of value got from register address
	\param flag	address and value data length(2 or 1 byte)
		ie FG_Address_2Byte | FG_Value_2Byte is 2 byte address and 2 byte value
		#define FG_Address_1Byte 0x01
		#define FG_Address_2Byte 0x02
		#define FG_Value_1Byte   0x10
		#define FG_Value_2Byte   0x20
	\param nSensorMode	sensor mode(sensor A, B or Both)
		A is 0, B is 1, Both is 2
	\return success: EtronDI_OK, others: see eSPDI_ErrCode.h
*/
int ETRONDI_API EtronDI_SetSlaveSensorRegister(void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, int nId, unsigned short address, unsigned short nValue, int flag, int nSensorMode);

/*! \fn int EtronDI_SetSensorRegister(
		void *pHandleEtronDI,
		PDEVSELINFO pDevSelInfo,
		int nId,
		unsigned short address,
		unsigned short *pValue,
		int flag,
		int nSensorMode)
	\brief set value from sensor register
	\param pHandleEtronDI	CEtronDI handler
	\param pDevSelInfo	pointer of device select index
	\param nId	sensor slave address. see #SENSOR_TYPE_NAME enum definition
	\param address	register address
	\param pValue	pointer of value got from register address
	\param flag	address and value data length(2 or 1 byte)
		ie FG_Address_2Byte | FG_Value_2Byte is 2 byte address and 2 byte value
		#define FG_Address_1Byte 0x01
		#define FG_Address_2Byte 0x02
		#define FG_Value_1Byte   0x10
		#define FG_Value_2Byte   0x20
	\param nSensorMode	sensor mode(sensor A, B or Both)
		A is 0, B is 1, Both is 2
	\return success: EtronDI_OK, others: see eSPDI_ErrCode.h
*/
int ETRONDI_API EtronDI_SetSensorRegister ( void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, int nId, unsigned short address, unsigned short nValue,  int flag, int nSensorMode);

/*! \fn int EtronDI_GetFWRegister(
		void *pHandleEtronDI,
		PDEVSELINFO pDevSelInfo,
		unsigned short address,
		unsigned short *pValue,
		int flag)
	\brief get firmware register value
	\param pHandleEtronDI	CEtronDI handler
	\param pDevSelInfo	pointer of device select index
	\param address	register address
	\param pValue	pointer of value got from register address
	\param flag	address and value data length(2 or 1 byte)
		ie FG_Address_2Byte | FG_Value_2Byte is 2 byte address and 2 byte value
		#define FG_Address_1Byte 0x01
		#define FG_Address_2Byte 0x02
		#define FG_Value_1Byte   0x10
		#define FG_Value_2Byte   0x20
	\return success: EtronDI_OK, others: see eSPDI_ErrCode.h
*/
int ETRONDI_API EtronDI_GetFWRegister     ( void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, unsigned short address, unsigned short *pValue, int flag);

/*! \fn int EtronDI_SetFWRegister(
		void *pHandleEtronDI,
		PDEVSELINFO pDevSelInfo,
		unsigned short address,
		unsigned short nValue,
		int flag)
	\brief set firmware register value
	\param pHandleEtronDI	CEtronDI handler
	\param pDevSelInfo	pointer of device select index
	\param address	register address
	\param nValue	register value to set
	\param flag	address and value data length(2 or 1 byte)
		ie FG_Address_1Byte | FG_Value_1Byte is 1 byte address and 1 byte value
		#define FG_Address_1Byte 0x01
		#define FG_Address_2Byte 0x02
		#define FG_Value_1Byte   0x10
		#define FG_Value_2Byte   0x20
	\return success: EtronDI_OK, others: see eSPDI_ErrCode.h
*/
int ETRONDI_API EtronDI_SetFWRegister     ( void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, unsigned short address, unsigned short nValue,  int flag);

/*! \fn int EtronDI_GetSlaveHWRegister(
		void *pHandleEtronDI,
		PDEVSELINFO pDevSelInfo,
		unsigned short address,
		unsigned short *pValue,
		int flag)
	\brief get hardware register value
	\param pHandleEtronDI	CEtronDI handler
	\param pDevSelInfo	pointer of device select index
	\param address	register address
	\param pValue	pointer of value got from register address
	\param flag	address and value data length(2 or 1 byte)
		ie FG_Address_2Byte | FG_Value_2Byte is 2 byte address and 2 byte value
		#define FG_Address_1Byte 0x01
		#define FG_Address_2Byte 0x02
		#define FG_Value_1Byte   0x10
		#define FG_Value_2Byte   0x20
	\return success: EtronDI_OK, others: see eSPDI_ErrCode.h
*/
int ETRONDI_API EtronDI_GetSlaveHWRegister(void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, unsigned short address, unsigned short *pValue, int flag);

/*! \fn int EtronDI_GetHWRegister(
		void *pHandleEtronDI,
		PDEVSELINFO pDevSelInfo,
		unsigned short address,
		unsigned short *pValue,
		int flag)
	\brief get hardware register value
	\param pHandleEtronDI	CEtronDI handler
	\param pDevSelInfo	pointer of device select index
	\param address	register address
	\param pValue	pointer of value got from register address
	\param flag	address and value data length(2 or 1 byte)
		ie FG_Address_2Byte | FG_Value_2Byte is 2 byte address and 2 byte value
		#define FG_Address_1Byte 0x01
		#define FG_Address_2Byte 0x02
		#define FG_Value_1Byte   0x10
		#define FG_Value_2Byte   0x20
	\return success: EtronDI_OK, others: see eSPDI_ErrCode.h
*/
int ETRONDI_API EtronDI_GetHWRegister     ( void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, unsigned short address, unsigned short *pValue, int flag);

/*! \fn int EtronDI_SetSlaveHWRegister(
		void *pHandleEtronDI,
		PDEVSELINFO pDevSelInfo,
		unsigned short address,
		unsigned short nValue,
		int flag)
	\brief set hardware register
	\param pHandleEtronDI	CEtronDI handler
	\param pDevSelInfo	pointer of device select index
	\param address	register address
	\param nValue	register value to set
	\param flag	address and value data length(2 or 1 byte)
		ie FG_Address_1Byte | FG_Value_1Byte is 1 byte address and 1 byte value
		#define FG_Address_1Byte 0x01
		#define FG_Address_2Byte 0x02
		#define FG_Value_1Byte   0x10
		#define FG_Value_2Byte   0x20
	\return success: EtronDI_OK, others: see eSPDI_ErrCode.h
*/
int ETRONDI_API EtronDI_SetSlaveHWRegister(void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, unsigned short address, unsigned short nValue, int flag);

/*! \fn int EtronDI_SetHWRegister(
		void *pHandleEtronDI,
		PDEVSELINFO pDevSelInfo,
		unsigned short address,
		unsigned short nValue,
		int flag)
	\brief set hardware register
	\param pHandleEtronDI	CEtronDI handler
	\param pDevSelInfo	pointer of device select index
	\param address	register address
	\param nValue	register value to set
	\param flag	address and value data length(2 or 1 byte)
		ie FG_Address_1Byte | FG_Value_1Byte is 1 byte address and 1 byte value
		#define FG_Address_1Byte 0x01
		#define FG_Address_2Byte 0x02
		#define FG_Value_1Byte   0x10
		#define FG_Value_2Byte   0x20
	\return success: EtronDI_OK, others: see eSPDI_ErrCode.h
*/
int ETRONDI_API EtronDI_SetHWRegister     ( void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, unsigned short address, unsigned short nValue,  int flag);
// register APIs -

// File ID +
/*! \fn int EtronDI_GetFwVersion(
		void *pHandleEtronDI,
		PDEVSELINFO pDevSelInfo,
		char *pszFwVersion,
		int nBufferSize,
		int *pActualLength)
	\brief get the firmware version of device, the version is a string
	\param pHandleEtronDI	CEtronDI handler
	\param pDevSelInfo	pointer of device select index
	\param pszFwVersion	firmware version string
	\param nBufferSize	input buffer length to receive FW version
	\param pActualLength	the actual length of FW version in byte
	\return success: EtronDI_OK, others: see eSPDI_ErrCode.h
*/
int ETRONDI_API EtronDI_GetFwVersion    ( void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, char *pszFwVersion, int nBufferSize, int *pActualLength);

/*! \fn int EtronDI_GetPidVid(
		void *pHandleEtronDI,
		PDEVSELINFO pDevSelInfo,
		unsigned short *pPidBuf,
		unsigned short *pVidBuf)
	\brief get PID(product ID) and VID(vendor ID) of device
	\param pHandleEtronDI	CEtronDI handler
	\param pDevSelInfo	pointer of device select index
	\param pPidBuf	4 byte buffer to store PID value
	\param pVidBuf	4 byte buffer to store VID value
	\return success: EtronDI_OK, others: see eSPDI_ErrCode.h
*/
int ETRONDI_API EtronDI_GetPidVid       ( void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, unsigned short *pPidBuf, unsigned short *pVidBuf );

/*! \fn int EtronDI_GetSerialNumber( void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, char *pSerialNum, int nBufferSize, int *pActualLength)
	\brief get the module serial number (the devixe trace code, 14 or 17 digits of ASCII code)
	\param pHandleEtronDI	CEtronDI handler
	\param pDevSelInfo	pointer of device select index
	\param pSerialNum	a pre-allocated buffer to store the serial number.
	\param nBufferSize	length of pBuf
	\param pActualLength	actual numbers of byte copied o the buffer	
	\return success: EtronDI_OK, others: see eSPDI_ErrCode.h
*/
int ETRONDI_API EtronDI_GetSerialNumber( void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, BYTE *pSerialNum, int nBufferSize, int *pACtualSNLenByByte);

/*! \fn int EtronDI_GetSlaveLogData(
		void *pHandleEtronDI,
		PDEVSELINFO pDevSelInfo,
		BYTE *buffer,
		int BufferLength,
		int *pActualLength,
		int index)
	\brief get log data from flash
	\param pHandleEtronDI	 the pointer to the initilized EtronDI SDK instance
	\param pDevSelInfo	pointer of device select index
	\param buffer	buffer to store log data
	\param BufferLength	input buffer length
	\param pActualLength	actual length has written to buffer
	\param index	index to identify log data for corresponding depth
	\return success: EtronDI_OK, others: see eSPDI_ErrCode.h
*/
int ETRONDI_API EtronDI_GetSlaveLogData(void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, BYTE *buffer, int BufferLength, int *pActualLength, int index);

/*! \fn int EtronDI_GetLogData(
		void *pHandleEtronDI,
		PDEVSELINFO pDevSelInfo,
		BYTE *buffer,
		int BufferLength,
		int *pActualLength,
		int index)
	\brief get log data from flash
	\param pHandleEtronDI	 the pointer to the initilized EtronDI SDK instance
	\param pDevSelInfo	pointer of device select index
	\param buffer	buffer to store log data
	\param BufferLength	input buffer length
	\param pActualLength	actual length has written to buffer
	\param index	index to identify log data for corresponding depth
	\return success: EtronDI_OK, others: see eSPDI_ErrCode.h
*/
int ETRONDI_API EtronDI_GetLogData      ( void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, BYTE *buffer, int BufferLength, int *pActualLength, int index);

/*! \fn int EtronDI_GetUserData(
		void *pHandleEtronDI,
		PDEVSELINFO pDevSelInfo,
		BYTE *buffer,
		int BufferLength,
		#USERDATA_SECTION_INDEX usi)
	\brief get user data from flash
	\param pHandleEtronDI	 the pointer to the initilized EtronDI SDK instance
	\param pDevSelInfo	pointer of device select index
	\param buffer	buffer to store user data
	\param BufferLength	input buffer length
	\param usi	which user index data to select
	\return success: EtronDI_OK, others: see eSPDI_ErrCode.h
*/
int ETRONDI_API EtronDI_GetUserData     ( void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, BYTE *buffer, int BufferLength, USERDATA_SECTION_INDEX usi);

/*! \fn int EtronDI_SetSlaveLogData(void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, BYTE *buffer, int BufferLength, int *pActualLength, int index)
	\brief set log data to flash
	\param pHandleEtronDI	 the pointer to the initilized EtronDI SDK instance
	\param pDevSelInfo	pointer of device select index
	\param buffer	buffer to store log data
	\param BufferLength	input buffer length
	\param pActualLength	actual length has written to buffer
	\param index	index to identify log data for corresponding depth
	\return success: EtronDI_OK, others: see eSPDI_ErrCode.h
*/
int ETRONDI_API EtronDI_SetSlaveLogData(void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, BYTE *buffer, int BufferLength, int *pActualLength, int index);

/*! \fn int EtronDI_SetLogData(void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, BYTE *buffer, int BufferLength, int *pActualLength, int index)
	\brief set log data to flash
	\param pHandleEtronDI	 the pointer to the initilized EtronDI SDK instance
	\param pDevSelInfo	pointer of device select index
	\param buffer	buffer to store log data
	\param BufferLength	input buffer length
	\param pActualLength	actual length has written to buffer
	\param index	index to identify log data for corresponding depth
	\return success: EtronDI_OK, others: see eSPDI_ErrCode.h
*/
int ETRONDI_API EtronDI_SetLogData      ( void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, BYTE *buffer, int BufferLength, int *pActualLength, int index);

/*! \fn int  EtronDI_SetUserData(void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, BYTE *buffer, int BufferLength, USERDATA_SECTION_INDEX usi)
	\brief set user data to flash
	\param pHandleEtronDI	 the pointer to the initilized EtronDI SDK instance
	\param pDevSelInfo	pointer of device select index
	\param buffer	buffer to store user data
	\param BufferLength	input buffer length
	\param usi	which user index data to select
	\return success: EtronDI_OK, others: see eSPDI_ErrCode.h
*/
int ETRONDI_API EtronDI_SetUserData     ( void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, BYTE *buffer, int BufferLength, USERDATA_SECTION_INDEX usi);


/*! \fn int EtronDI_ReadFlashData(
		void *pHandleEtronDI,
		PDEVSELINFO pDevSelInfo,
		FLASH_DATA_TYPE fdt,
		BYTE *pBuffer,
		unsigned long int nLengthOfBuffer,
		unsigned long int *pActualBufferLen)
	\brief read firmware code(.bin) form flash
		The firmware code is the combination of boot loader, firmware body and plug-in data.
		This input buffer length has to match with the flash data type
	\param pHandleEtronDI	 the pointer to the initilized EtronDI SDK instance
	\param pDevSelInfo	pointer of device select index
	\param fdt	segment type of flash be read
	\param pBuffer	buffer to store firmware code
	\param nLengthOfBuffer	input buffer length 
	\param pActualBufferLen	actual length has written to pBuffer
	\return success: EtronDI_OK, others: see eSPDI_ErrCode.h
*/
int ETRONDI_API EtronDI_ReadFlashData   ( void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, FLASH_DATA_TYPE fdt, BYTE *pBuffer, 
                                          unsigned long int nLengthOfBuffer, unsigned long int *pActualBufferLen);

										  
int ETRONDI_API EtronDI_WriteFlashData  ( void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, FLASH_DATA_TYPE fdt, BYTE *pBuffer, 
									      unsigned long int nLengthOfBuffer, BOOL bIsDataVerify, KEEP_DATA_CTRL kdc);

/*! \fn int EtronDI_OpenDevice(
		void* pHandleEtronDI,
		PDEVSELINFO pDevSelInfo,
		int colorStreamIndex,
		int depthStreamIndex,
		int depthStreamSwitch,
		int iFps,
        EtronDI_ImgCallbackFn callbackFn,
		void* pCallbackParam,
		int pid)
	\brief open camera device with image callback support
	\param pHandleEtronDI	 the pointer to the initilized EtronDI SDK instance
	\param pDevSelInfo	pointer of device select index
	\param colorStreamIndex	index of the desired color stream
	\param depthStreamIndex index of the desired sdepth tream
	\param depthStreamSwitch	depth switch for S0, S1 or S2
	\param iFps pointer to the desired frame rate, returns the actual frame rate.
    \param callbackFn	set image callback function
	\param pCallbackParam	the data to associate with the callback function
	\param pid Specify device pid.
<table>
<caption id="multi_row">Image Control Mode</caption>
<tr><th>Mode        <th>Description
<tr><td>0x01 <td>color and depth frame output synchrously, for depth map module only
<tr><td>0x02 <td>enable post-process, for Depth Map module only
<tr><td>0x04 <td>stitch images if this bit is set, for fisheye spherical module only
<tr><td>0x08 <td>use OpenCL in stitching. This bit effective only when bit-2 is set.
</table>
	\return success:EtronDI_OK, others:see eSPDI_ErrCode.h
 */
int ETRONDI_API EtronDI_OpenDevice( void* pHandleEtronDI, PDEVSELINFO pDevSelInfo,
                                    int colorStreamIndex, int depthStreamIndex, int depthStreamSwitch, int iFps,
	                                EtronDI_ImgCallbackFn callbackFn, void* pCallbackParam, int pid = -1 );

/*! \fn int EtronDI_CloseDevice(
		void *pHandleEtronDI,
		PDEVSELINFO pDevSelInfo)
	\brief close device and stop video render
	\param pHandleEtronDI	 the pointer to the initilized EtronDI SDK instance
	\param pDevSelInfo	pointer of device select index
	\return success:EtronDI_OK, others:see eSPDI_ErrCode.h
*/                                     
int ETRONDI_API EtronDI_CloseDevice( void *pHandleEtronDI, PDEVSELINFO pDevSelInfo);

/*! \fn int EtronDI_GetDeviceResolutionListEx(
		void *pHandleEtronDI,
		PDEVSELINFO pDevSelInfo,
		int nMaxCount0,
		ETRONDI_STREAM_INFO *pStreamInfo0,
			 int nMaxCount1,
			 ETRONDI_STREAM_INFO *pStreamInfo1)
	\brief get the device resolution list
	\param pHandleEtronDI	 the pointer to the initilized EtronDI SDK instance
	\param pDevSelInfo	pointer of device select index
	\param nMaxCount0	max count of endpoint1 resolutions
	\param pStreamInfo0	resolution infos of endpoint1
	\param nMaxCount1	max count of endpoint2 resolutions
	\param pStreamInfo1	resolutions infos of endpoint2
	\return success: nCount0*256+nCount1, others: see eSPDI_ErrCode.h
 */
int ETRONDI_API EtronDI_GetDeviceResolutionListEx( void *pHandleEtronDI, PDEVSELINFO pDevSelInfo,
												   int nMaxCount0, ETRONDI_STREAM_INFO *pStreamInfo0,
												   int nMaxCount1, ETRONDI_STREAM_INFO *pStreamInfo1,
												   int pid);

/*! \fn int EtronDI_GetDeviceResolutionList(
		void *pHandleEtronDI,
		PDEVSELINFO pDevSelInfo,
		int nMaxCount0,
		ETRONDI_STREAM_INFO *pStreamInfo0, 
             int nMaxCount1,
             ETRONDI_STREAM_INFO *pStreamInfo1)
	\brief get the device resolution list
	\param pHandleEtronDI	 the pointer to the initilized EtronDI SDK instance
	\param pDevSelInfo	pointer of device select index
	\param nMaxCount0	max count of endpoint1 resolutions
	\param pStreamInfo0	resolution infos of endpoint1
	\param nMaxCount1	max count of endpoint2 resolutions
	\param pStreamInfo1	resolutions infos of endpoint2
	\return success: nCount0*256+nCount1, others: see eSPDI_ErrCode.h
 */									 
int ETRONDI_API EtronDI_GetDeviceResolutionList( void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, 
                                                 int nMaxCount0, ETRONDI_STREAM_INFO *pStreamInfo0, 
                                                 int nMaxCount1, ETRONDI_STREAM_INFO *pStreamInfo1);

/*! \fn bool EtronDI_Is360Device(
		void *pHandleEtronDI,
		PDEVSELINFO pDevSelInfo)
	\brief check module is spherical device or not
	\param pHandleEtronDI	 the pointer to the initilized EtronDI SDK instance
	\param pDevSelInfo	pointer of device select index
	\return true: module support 360, false: not support
*/
bool ETRONDI_API EtronDI_Is360Device(void *pHandleEtronDI, PDEVSELINFO pDevSelInfo);

/*! \fn int EtronDI_GetSerialNumberFromLog( void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, char *pSerialNum, int nBufferSize, int *pActualLength)
	\brief get the module serial number 
*/
int ETRONDI_API EtronDI_GetSerialNumberFromLog( void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, char *pSerialNum, int nBufferSize, int *pActualLength);

// IR support
/*! \fn int EtronDI_SetCurrentIRValue(void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, WORD wType)
	\brief set current infrared radiation(IR) value
	\param pHandleEtronDI	CEtronDI handler
	\param pDevSelInfo	pointer of device select index
	\param wType	value to set
	\return success: EtronDI_OK, others: see eSPDI_ErrCode.h
*/
int ETRONDI_API EtronDI_SetCurrentIRValue(void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, WORD wType);

/*! \fn int EtronDI_GetCurrentIRValue(void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, WORD *pwType)
	\brief get current infrared radiation(IR) value
	\param pHandleEtronDI	CEtronDI handler
	\param pDevSelInfo	pointer of device select index	
	\param pwType	value of current IR
	\return success: EtronDI_OK, others: see eSPDI_ErrCode.h	
*/
int ETRONDI_API EtronDI_GetCurrentIRValue(void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, WORD *pwType);

/*! \fn int EtronDI_GetIRMinValue(void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, WORD *pwType)
	\brief get minimum IR value the module support
	\param pHandleEtronDI	 the pointer to the initilized EtronDI SDK instance
	\param pDevSelInfo	pointer of device select index	
	\param pwType	pointer strors minimum IR value
	\return success: EtronDI_OK, others: see eSPDI_ErrCode.h	
*/
int ETRONDI_API EtronDI_GetIRMinValue(void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, WORD *pwType);

/*! \fn int EtronDI_SetIRMaxValue(void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, WORD wType)
	\brief set maximum IR value the module support
	\param pHandleEtronDI	 the pointer to the initilized EtronDI SDK instance
	\param pDevSelInfo	pointer of device select index	
	\param wType	pointer strors maximum IR value
	\return success: EtronDI_OK, others: see eSPDI_ErrCode.h	
*/
int ETRONDI_API EtronDI_SetIRMaxValue(void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, WORD wType);

/*! \fn int EtronDI_GetIRMaxValue(void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, WORD *pwType)
	\brief get maximum IR value the module support
	\param pHandleEtronDI	 the pointer to the initilized EtronDI SDK instance
	\param pDevSelInfo	pointer of device select index	
	\param pwType	pointer strors maximum IR value
	\return success: EtronDI_OK, others: see eSPDI_ErrCode.h	
*/
int ETRONDI_API EtronDI_GetIRMaxValue(void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, WORD *pwType);

/*! \fn int EtronDI_SetIRMode(void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, WORD wType)
	\brief set IR mode, left, right or both
*/
int ETRONDI_API EtronDI_SetIRMode(void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, WORD wType);

/*! \fn int EtronDI_GetIRMode(void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, WORD *pwType)
	\brief set IR mode, left, right or both
*/
int ETRONDI_API EtronDI_GetIRMode(void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, WORD *pwType);

// for sensorif
/*! \fn int EtronDI_EnableSensorIF( void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, bool bIsEnable)
	\brief turn on/off sensor IF function
*/
int ETRONDI_API EtronDI_EnableSensorIF( void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, bool bIsEnable);

 
//int ETRONDI_API EtronDI_GetMotorCurrentState( void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, bool* bIsRunning);

// for Gyro

typedef enum
{
    DPS_245 = 0,
    DPS_500,
    DPS_2000
} SENSITIVITY_LEVEL_L3G;
 
// bPowerMode :
//     true  : Normal
//     false : Power Down
// bIsZEnable :
//     true  : Enable
//     false : Disable
// bIsYEnable :
//     true  : Enable
//     false : Disable
// bIsXEnable :
//     true  : Enable
//     false : Disable

//typedef struct GyroTag
//{
//	short x; 		
//  short y;
//  short z;
//} GYRO_ANGULAR_RATE_DATA;


/*! \enum sensor model name */
typedef enum
{
    ETRONDI_SENSOR_TYPE_H22 = 0,	/**< H22 */
    ETRONDI_SENSOR_TYPE_OV7740,		/**< OV7740 */			
    ETRONDI_SENSOR_TYPE_AR0134,		/**< AR0134 */
    ETRONDI_SENSOR_TYPE_AR0135,		/**< AR0135 */
	ETRONDI_SENSOR_TYPE_AR0144,		/**< AR0144 */
	ETRONDI_SENSOR_TYPE_OV9714,		/**< OV9714 */
	ETRONDI_SENSOR_TYPE_OV9282,		/**< OV9282 */
	ETRONDI_SENSOR_TYPE_AR0330,		/**< AR0330 */
	ETRONDI_SENSOR_TYPE_AR1335,		/**< AR1335 */
    ETRONDI_SENSOR_TYPE_H65,
    ETRONDI_SENSOR_TYPE_AR0522,
} SENSOR_TYPE_NAME; 

//
// Sensor Mode: Left, Right, or Both
//
#define ESPAEAWB_SENSOR_MODE_LEFT		0
#define ESPAEAWB_SENSOR_MODE_RIGHT      1
#define ESPAEAWB_SENSOR_MODE_BOTH		2
//
// PU Property ID
//
#define    PU_PROPERTY_ID_BRIGHTNESS                0
#define    PU_PROPERTY_ID_CONTRAST                  1
#define    PU_PROPERTY_ID_HUE                       2
#define    PU_PROPERTY_ID_SATURATION                3
#define    PU_PROPERTY_ID_SHARPNESS                 4
#define    PU_PROPERTY_ID_GAMMA                     5
#define    PU_PROPERTY_ID_COLORENABLE               6
#define    PU_PROPERTY_ID_WHITEBALANCE              7
#define    PU_PROPERTY_ID_BACKLIGHT_COMPENSATION    8
#define    PU_PROPERTY_ID_GAIN                      9
#define    PU_PROPERTY_ID_DIGITAL_MULTIPLIER        10
#define    PU_PROPERTY_ID_DIGITAL_MULTIPLIER_LIMIT  11
#define    PU_PROPERTY_ID_WHITEBALANCE_COMPONENT    12
#define    PU_PROPERTY_ID_POWERLINE_FREQUENCY       13
//
// CT Property ID
//
#define    CT_PROPERTY_ID_PAN               		   0
#define    CT_PROPERTY_ID_TILT                         1
#define    CT_PROPERTY_ID_ROLL                         2
#define    CT_PROPERTY_ID_ZOOM                         3
#define    CT_PROPERTY_ID_EXPOSURE                     4
#define    CT_PROPERTY_ID_IRIS                         5
#define    CT_PROPERTY_ID_FOCUS                        6
#define    CT_PROPERTY_ID_SCANMODE                     7
#define    CT_PROPERTY_ID_PRIVACY                      8
#define    CT_PROPERTY_ID_PANTILT                      9
#define    CT_PROPERTY_ID_PAN_RELATIVE                 10
#define    CT_PROPERTY_ID_TILT_RELATIVE                11
#define    CT_PROPERTY_ID_ROLL_RELATIVE                12
#define    CT_PROPERTY_ID_ZOOM_RELATIVE                13
#define    CT_PROPERTY_ID_EXPOSURE_RELATIVE            14
#define    CT_PROPERTY_ID_IRIS_RELATIVE                15
#define    CT_PROPERTY_ID_FOCUS_RELATIVE               16
#define    CT_PROPERTY_ID_PANTILT_RELATIVE             17
#define    CT_PROPERTY_ID_AUTO_EXPOSURE_PRIORITY       19 

/*! \fn int EtronDI_SetSensorTypeName(
		void *pHandleEtronDI,
		SENSOR_TYPE_NAME stn)
	\brief select which sensor to operate
	\param pHandleEtronDI	CEtronDI handler
	\param stn	sensor type
	\return ETronDI_OK
*/                        
int ETRONDI_API EtronDI_SetSensorTypeName( void *pHandleEtronDI, SENSOR_TYPE_NAME stn);

/*! \fn int EtronDI_EnableAE(
		void *pHandleEtronDI,
		PDEVSELINFO pDevSelInfo)
	\brief enable auto exposure function of ISP
	\param pHandleEtronDI	CEtronDI handler
	\param pDevSelInfo	pointer of device select index
	\return success: EtronDI_OK, others: see eSPDI_ErrCode.h
*/
int ETRONDI_API EtronDI_EnableAE  ( void *pHandleEtronDI, PDEVSELINFO pDevSelInfo);

/*! \fn int EtronDI_DisableAE(
		void *pHandleEtronDI,
		PDEVSELINFO pDevSelInfo)
	\brief disable auto exposure function of ISP
	\param pHandleEtronDI	CEtronDI handler
	\param pDevSelInfo	pointer of device select index
	\return success: EtronDI_OK, others: see eSPDI_ErrCode.h
*/
int ETRONDI_API EtronDI_DisableAE ( void *pHandleEtronDI, PDEVSELINFO pDevSelInfo);

/*! \fn int EtronDI_EnableAWB(
		void *pHandleEtronDI,
		PDEVSELINFO pDevSelInfo)
	\brief enable auto white balance function of ISP
	\param pHandleEtronDI	CEtronDI handler
	\param pDevSelInfo	pointer of device select index
	\return success: EtronDI_OK, others: see eSPDI_ErrCode.h
*/
int ETRONDI_API EtronDI_EnableAWB ( void *pHandleEtronDI, PDEVSELINFO pDevSelInfo);

/*! \fn int EtronDI_DisableAWB(
		void *pHandleEtronDI,
		PDEVSELINFO pDevSelInfo)
	\brief disable auto white balance of ISP
	\param pHandleEtronDI	CEtronDI handler
	\param pDevSelInfo	pointer of device select index
	\return success: EtronDI_OK, others: see eSPDI_ErrCode.h
*/
int ETRONDI_API EtronDI_DisableAWB( void *pHandleEtronDI, PDEVSELINFO pDevSelInfo);


/*! \fn int EtronDI_GetExposureTime(
		void *pHandleEtronDI,
		PDEVSELINFO pDevSelInfo,
		int nSensorMode,
		float *pfExpTimeMS)
	\brief get exposure time of ISP setting in millisecond
		the target sensor type was set in EtronDI_SetSensorTypeName()
	\param pHandleEtronDI	CEtronDI handler
	\param pDevSelInfo	pointer of device select index
	\param nSensorMode	which sensor(sensor A, B or Both) to get
		A is 0, B is 1, Both is 2
    \param pid	product id of the USB device
	\param pfExpTimeMS	pointer of getting exposure time in millisecond
	\return success: EtronDI_OK, others: see eSPDI_ErrCode.h
*/
int ETRONDI_API EtronDI_GetExposureTime( void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, int nSensorMode, int pid, float *pfExpTimeMS);

/*! \fn int EtronDI_SetExposureTime(
		void *pHandleEtronDI,
		PDEVSELINFO pDevSelInfo,
		int nSensorMode,
		float fExpTimeMS)
	\brief set exposure time of ISP sensor setting
		the target sensor type was set in EtronDI_SetSensorTypeName()
	\param *pHandleEtronDI	CEtronDI handler
	\param pDevSelInfo	pointer of device select index
	\param nSensorMode	which sensor(sensor A, B or Both) to set
		A is 0, B is 1, Both is 2
    \param pid	product id of the USB device
	\param fExpTimeMS	pointer of setting exposure time in millisecond
	\return success: EtronDI_OK, others: see eSPDI_ErrCode.h
*/
int ETRONDI_API EtronDI_SetExposureTime( void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, int nSensorMode, int pid, float fExpTimeMS);

/*! \fn int EtronDI_GetGlobalGain(
		void *pHandleEtronDI,
		PDEVSELINFO pDevSelInfo,
		int nSensorMode,
		float *pfGlobalGain)
	\brief get global gain of ISP setting
		the target sensor type was set in EtronDI_SetSensorTypeName()
	\param pHandleEtronDI	CEtronDI handler
	\param pDevSelInfo	pointer of device select index
	\param nSensorMode	which sensor(sensor A, B or Both) to get
		A is 0, B is 1, Both is 2
    \param pid	product id of the USB device
	\param pfGlobalGain	pointer of global gain value
		see source code for value calculation of different sensor type
	\return success: EtronDI_OK, others: see eSPDI_ErrCode.h
*/
int ETRONDI_API EtronDI_GetGlobalGain( void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, int nSensorMode, int pid, float *pfGlobalGain);

/*! \fn int EtronDI_SetGlobalGain(
		void *pHandleEtronDI,
		PDEVSELINFO pDevSelInfo,
		int nSensorMode,
		float fGlobalGain)
	\brief set global gain of ISP sensor setting
		the target sensor type was set in EtronDI_SetSensorTypeName()
	\param pHandleEtronDI	CEtronDI handler
	\param pDevSelInfo	pointer of device select index
	\param nSensorMode	which sensor(sensor A, B or Both) to get
		A is 0, B is 1, Both is 2
    \param pid	product id of the USB device
	\param fGlobalGain	pointer of global gain value
		see source code for value calculation of different sensor type
	\return success: EtronDI_OK, others: see eSPDI_ErrCode.h
*/
int ETRONDI_API EtronDI_SetGlobalGain( void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, int nSensorMode, int pid, float fGlobalGain);

/*! \fn int EtronDI_GetGPIOValue(
		void *pHandleEtronDI,
		PDEVSELINFO pDevSelInfo,
		int nGPIOIndex,
		BYTE *pValue)
	\brief get general purpose IO value
	\param pHandleEtronDI	CEtronDI handler
	\param pDevSelInfo	pointer of device select index	
	\param nGPIOIndex	GPIO index, 1 or 2 is valid
	\param pValue	pointer of GPIO value
	\return success: EtronDI_OK, others: see eSPDI_ErrCode.h
*/
int ETRONDI_API EtronDI_GetGPIOValue( void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, int nGPIOIndex, BYTE *pValue);

/*! \fn int EtronDI_SetGPIOValue(
		void *pHandleEtronDI,
		PDEVSELINFO pDevSelInfo,
		int nGPIOIndex,
		BYTE nValue)
	\brief set GPIO value
	\param pHandleEtronDI	CEtronDI handler
	\param pDevSelInfo	pointer of device select index
	\param nGPIOIndex	GPIO index, 1 or 2 is valid
	\param nValue	GPIO value to set
	\return success: EtronDI_OK, others: see eSPDI_ErrCode.h
*/
int ETRONDI_API EtronDI_SetGPIOValue( void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, int nGPIOIndex, BYTE nValue);

/*! \fn int EtronDI_SetGPIOCtrl(void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, int nGPIOIndex, BYTE nValue)
	\brief set GPIO control address
	\param nGPIOIndex	index of GPIO (1 ~ 4)
	\param nValue	register value to set
	\return success: EtronDI_OK, others: see eSPDI_ErrCode.h
*/
int ETRONDI_API EtronDI_SetGPIOCtrl(void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, int nGPIOIndex, BYTE nValue);

int ETRONDI_API EtronDI_GetAccMeterValue( void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, int *pX, int *pY, int *pZ);

/*! \fn int EtronDI_GetPUPropVal(
		void *pHandleEtronDI,
		PDEVSELINFO pDevSelInfo,
		int nId,
		int *pValue)
	\brief get processing unit property value
		https://msdn.microsoft.com/en-us/library/windows/hardware/ff568122(v=vs.85).aspx
		The PROPSETID_VIDCAP_VIDEOPROCAMP property set controls devices that can adjust
		image color attributes of analog or digital signals.

		The KSPROPERTY_VIDCAP_VIDEOPROCAMP enumeration in ksmedia.h specifies the
		properties of this set.

		Support for this property set is optional and should be implemented only by devices that
		allow adjustment of brightness, contrast, hue, and other image quality settings.

		Prior to USB video class, this enumeration contained the following property items:
		KSPROPERTY_VIDEOPROCAMP_BACKLIGHT_COMPENSATION
		KSPROPERTY_VIDEOPROCAMP_BRIGHTNESS
		KSPROPERTY_VIDEOPROCAMP_COLORENABLE
		KSPROPERTY_VIDEOPROCAMP_CONTRAST
		KSPROPERTY_VIDEOPROCAMP_GAMMA
		KSPROPERTY_VIDEOPROCAMP_HUE
		KSPROPERTY_VIDEOPROCAMP_SATURATION
		KSPROPERTY_VIDEOPROCAMP_SHARPNESS
		KSPROPERTY_VIDEOPROCAMP_WHITEBALANCE
		KSPROPERTY_VIDEOPROCAMP_GAIN

		https://msdn.microsoft.com/en-us/library/windows/hardware/ff566089(v=vs.85).aspx
		The KSPROPERTY_VIDEOPROCAMP_S structure describes filter-based property settings
		in the PROPSETID_VIDCAP_VIDEOPROCAMP property set.
	\param pHandleEtronDI	CEtronDI handler
	\param pDevSelInfo	pointer of device select index
	\param nId	specifies the member of the property set
	\param pValue	pointer of store PU property value
	\return success: EtronDI_OK, others: see eSPDI_ErrCode.h
*/
int ETRONDI_API EtronDI_GetPUPropVal( void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, int nId, int *pValue);

/*! \fn int EtronDI_SetPUPropVal(
		void *pHandleEtronDI,
		PDEVSELINFO pDevSelInfo,
		int nId,
		int nValue)
	\brief get processing unit property value
		https://msdn.microsoft.com/en-us/library/windows/hardware/ff568122(v=vs.85).aspx
		https://msdn.microsoft.com/en-us/library/windows/hardware/ff566089(v=vs.85).aspx
	\param pHandleEtronDI	CEtronDI handler
	\param pDevSelInfo	pointer of device select index
	\param nId	specifies the member of the property set
	\param nValue	value to set
	\return success: EtronDI_OK, others: see eSPDI_ErrCode.h
*/
int ETRONDI_API EtronDI_SetPUPropVal( void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, int nId, int nValue);

/*! \fn int EtronDI_GetCTPropVal(
		void *pHandleEtronDI,
		PDEVSELINFO pDevSelInfo,
		int nId,
		int *pValue)
	\brief set control terminal property value
		https://msdn.microsoft.com/en-us/library/windows/hardware/ff567802(v=vs.85).aspx
		The PROPSETID_VIDCAP_CAMERACONTROL property set controls camera device settings.
		The controls it provides are a subset of the ITU T.RDC standard.

		The KSPROPERTY_VIDCAP_CAMERACONTROL enumeration in Ksmedia.h specifies the
		properties of this set.

		Support for this property set is optional and should be implemented only by minidrivers of
		devices that offer camera control settings. For more information, see the ITU website.

		Prior to USB video class, this enumeration contained the following properties:
		KSPROPERTY_CAMERACONTROL_EXPOSURE
		KSPROPERTY_CAMERACONTROL_FOCUS
		KSPROPERTY_CAMERACONTROL_IRIS
		KSPROPERTY_CAMERACONTROL_ZOOM
		KSPROPERTY_CAMERACONTROL_PAN
		KSPROPERTY_CAMERACONTROL_ROLL
		KSPROPERTY_CAMERACONTROL_TILT

		https://msdn.microsoft.com/en-us/library/windows/hardware/ff566089(v=vs.85).aspx
	\param *pHandleEtronDI	CEtronDI handler
	\param pDevSelInfo	pointer of device select index
	\param nId	specifies the member of the property set
	\param pValue	pointer of store CT property value
	\return success: EtronDI_OK, others: see eSPDI_ErrCode.h
*/
int ETRONDI_API EtronDI_GetCTPropVal( void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, int nId, int *pValue);

/*! \fn int EtronDI_SetCTPropVal(
		void *pHandleEtronDI,
		PDEVSELINFO pDevSelInfo,
		int nId,
		int nValue)
	\brief get control terminal property value
		https://msdn.microsoft.com/en-us/library/windows/hardware/ff567802(v=vs.85).aspx
		https://msdn.microsoft.com/en-us/library/windows/hardware/ff566089(v=vs.85).aspx
	\param pHandleEtronDI	CEtronDI handler
	\param pDevSelInfo	pointer of device select index
	\param nId	specifies the member of the property set
	\param nValue	CT property value to set
	\return success: EtronDI_OK, others: see eSPDI_ErrCode.h
*/
int ETRONDI_API EtronDI_SetCTPropVal( void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, int nId, int nValue);

// For AEAWB - 2015/01/28 by Wolf

int ETRONDI_API EtronDI_EncryptMP4(void* pHandleEtronDI, PDEVSELINFO pDevSelInfo, const char* filename);
int ETRONDI_API EtronDI_DecryptMP4(void* pHandleEtronDI, PDEVSELINFO pDevSelInfo, const char* filename);
int ETRONDI_API EtronDI_RetrieveMp4ExtraData(void* pHandleEtronDI, PDEVSELINFO pDevSelInfo, 
    const char* filename, char* dataBuf, int* dataSize);
int ETRONDI_API EtronDI_FlushMp4ExtraData(void* pHandleEtronDI, PDEVSELINFO pDevSelInfo, 
    const char* filename, const char* dataBuf, int dataSize);
/*! \fn int EtronDI_GetAutoExposureMode(void* pHandleEtronDI, PDEVSELINFO pDevSelInfo, unsigned short* mode)
	\brief misc function : get auto exposure mode
*/	
int ETRONDI_API EtronDI_GetAutoExposureMode(void* pHandleEtronDI, PDEVSELINFO pDevSelInfo, unsigned short* mode);

/*! \fn int EtronDI_SetAutoExposureMode(void* pHandleEtronDI, PDEVSELINFO pDevSelInfo, unsigned short mode)
	\brief misc function : set auto exposure mode
*/
int ETRONDI_API EtronDI_SetAutoExposureMode(void* pHandleEtronDI, PDEVSELINFO pDevSelInfo, unsigned short mode);

/*! \fn int EtronDI_GetFlexibleGyroData(void* pHandleEtronDI, PDEVSELINFO pDevSelInfo,
	    int length, BYTE *pGyroData)
	\brief get IMU(Gyro) data
	\param pHandleEtronDI	CEtronDI handler
	\param pDevSelInfo	pointer of device select index
	\param length	length of IMU data to read, should be get from EtronDI_GetFlexibleGyroLength
	\param pGyroData	data buffer to store IMU data
*/
int ETRONDI_API EtronDI_GetFlexibleGyroData(void * pHandleEtronDI, PDEVSELINFO pDevSelInfo,
    int length, BYTE *pGyroData);

/*! \fn int EtronDI_GetFlexibleGyroLength(void* pHandleEtronDI, PDEVSELINFO pDevSelInfo, unsigned short* GyroLen)
	\brief get the IMU(Gyro) data length
	\param pHandleEtronDI	CEtronDI handler
	\param pDevSelInfo	pointer of device select index
	\param GyroLen	pointer to store IMU data length
	\return success: EtronDI_OK, others: see eSPDI_ErrCode.h
*/	
int ETRONDI_API EtronDI_GetFlexibleGyroLength(void* pHandleEtronDI, PDEVSELINFO pDevSelInfo, unsigned short* GyroLen);

/*! \fn int EtronDI_SetHuffmanTableData(
	void *pHandleEtronDI,
	PDEVSELINFO pDevSelInfo,
	const char *filename,
	bool bLogFile)
	\brief set huffman table data for jpeg encode
	\param pHandleEtronDI	CEtronDI handler
	\param pDevSelInfo	pointer of device select index
	\param filename	huffman table file, see jh_vga_422.dat sample file
	\param bLogFile	if true then puma_htable.dat file is generated
	\return success: EtronDI_OK, others: see eSPDI_ErrCode.h
*/
int ETRONDI_API EtronDI_SetHuffmanTableData(void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, const char *filename, bool bLogFile);

/*! \fn int EtronDI_SetQuantizationTableData(
	void *pHandleEtronDI,
	PDEVSELINFO pDevSelInfo,
	const char *filename)
	\brief set quantication table data for jpeg encode
	\param pHandleEtronDI	CEtronDI handler
	\param pDevSelInfo	pointer of device select index
	\param filename	quantization table file, see FS_DEF_010.txt sample file
	\return success: EtronDI_OK, others: see eSPDI_ErrCode.h
*/
int ETRONDI_API EtronDI_SetQuantizationTableData(void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, const char *filename);

/*! \fn int ETRONDI_API EtronDI_SetPlumAR0330(void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, bool bEnable)
\brief Set Plum Sensor AR0330.
*/
int ETRONDI_API EtronDI_SetPlumAR0330(void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, bool bEnable);

/*! \fn int ETRONDI_API EtronDI_GetDevicePortType(void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, USB_PORT_TYPE* pUSB_Port_Type)
\brief Get Device USB-port-type.
*/
int ETRONDI_API EtronDI_GetDevicePortType(void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, USB_PORT_TYPE* pUSB_Port_Type);

/*! \fn int ETRONDI_API EtronDI_SubSample(void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, unsigned char* depthBuf, int bytesPerPixel, int width, int height, int& new_width, int& new_height, int mode = 0, int factor = 3)
*/
int ETRONDI_API EtronDI_SubSample(void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, unsigned char** SubSample, unsigned char* depthBuf, int bytesPerPixel, int width, int height, int& new_width, int& new_height, int mode = 0, int factor = 3);

/*! \fn int ETRONDI_API EtronDI_HoleFill(void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, unsigned char* depthBuf, int bytesPerPixel, int kernel_size, int width, int height, int level, bool horizontal)
*/
int ETRONDI_API EtronDI_HoleFill(void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, unsigned char* depthBuf, int bytesPerPixel, int kernel_size, int width, int height, int level, bool horizontal);

/*! \fn int ETRONDI_API EtronDI_TemporalFilter(void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, unsigned char* depthBuf, int bytesPerPixel, int width, int height, float alpha, int history).
*/
int ETRONDI_API EtronDI_TemporalFilter(void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, unsigned char* depthBuf, int bytesPerPixel, int width, int height, float alpha, int history);

/*! \fn int ETRONDI_API EtronDI_EdgePreServingFilter(void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, unsigned char* depthBuf, int type, int width, int height, int level, float sigma, float lumda)
*/
int ETRONDI_API EtronDI_EdgePreServingFilter(void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, unsigned char* depthBuf, int type, int width, int height, int level, float sigma, float lumda);

/*! \fn int ETRONDI_API EtronDI_ApplyFilters(void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, unsigned char* depthBuf, unsigned char* subDisparity, int bytesPerPixel, int width, int height, int sub_w, int sub_h, int threshold=64)
*/
int ETRONDI_API EtronDI_ApplyFilters(void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, unsigned char* depthBuf, unsigned char* subDisparity, int bytesPerPixel, int width, int height, int sub_w, int sub_h, int threshold=64);

/*! \fn int ETRONDI_API EtronDI_EnableGPUAcceleration(void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, bool enable)
	\brief enable depth filter with GPU acceleration or not
	\param pHandleEtronDI: the pointer to the initilized EtronDI SDK instance
	\param pDevSelInfo: pointer of device select index
	\param enable: true:enable, fales:diable
	\return success: EtronDI_OK, others: see eSPDI_ErrCode.h
*/
int ETRONDI_API EtronDI_EnableGPUAcceleration(void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, bool enable);

/*! \fn char* ETRONDI_API EtronDI_GetDepthFilterVersion(void *pHandleEtronDI, PDEVSELINFO pDevSelInfo)
	\brief get depth filter version
	\param pHandleEtronDI	 the pointer to the initilized EtronDI SDK instance
	\param pDevSelInfo	pointer of device select index
	\return success: get version string, others: get N/A string
*/
ETRONDI_API char* EtronDI_GetDepthFilterVersion(void *pHandleEtronDI, PDEVSELINFO pDevSelInfo);

/*! \fn int ETRONDI_API EtronDI_ResetFilters(void *pHandleEtronDI, PDEVSELINFO pDevSelInfo)
*/
int ETRONDI_API EtronDI_ResetFilters(void *pHandleEtronDI, PDEVSELINFO pDevSelInfo);

/*! \fn int ETRONDI_API EtronDI_TableToData(void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, int width, int height, unsigned char* Table, unsigned char* Src, unsigned char* Dst)
    \brief transfer Src to Dst by Table
	\param pHandleEtronDI	 the pointer to the initilized EtronDI SDK instance
	\param pDevSelInfo	pointer of device select index
    \param width 	 input  image width
	\param height	 input  image height
    \param TableSize input  Table size in bytes
	\param Table	 input  Table buffer
	\param Src	     input  Src   buffer
    \param Dst       output Dst   buffer
	\return success: EtronDI_OK, others: see eSPDI_ErrCode.h
*/
int ETRONDI_API EtronDI_TableToData(void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, int width, int height, int TableSize, unsigned short* Table, unsigned short* Src, unsigned short* Dst);

/*! \fn int EtronDI_ColorFormat_to_RGB24(void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, unsigned char* ImgDst, unsigned char* ImgSrc, int width, int height, EtronDIImageType::Value type)
	\brief get hardware post processing status
	\param pHandleEtronDI	 the pointer to the initilized EtronDI SDK instance
	\param pDevSelInfo	pointer of device select index
    \param ImgDst 	output image buffer
	\param ImgSrc	input  image buffer
	\param width	input  image width
	\param height	input  image height
    \param type     input  image-format
	\return success: EtronDI_OK, others: see eSPDI_ErrCode.h	
*/
int ETRONDI_API EtronDI_ColorFormat_to_RGB24( void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, unsigned char* ImgDst, unsigned char* ImgSrc, int SrcSize, int width, int height, EtronDIImageType::Value type );

int ETRONDI_API EtronDI_PropertyPU_GetRange(void * pHandleEtronDI, PDEVSELINFO pDevSelInfo, long nProperty, long * pMin, long * pMax, long * pStep, long * pDefault, long * pCapsFlag, int pid);
int ETRONDI_API EtronDI_PropertyCT_GetRange(void * pHandleEtronDI, PDEVSELINFO pDevSelInfo, long nProperty, long * pMin, long * pMax, long * pStep, long * pDefault, long * pCapsFlag, int pid);
int ETRONDI_API EtronDI_PropertyPU_GetCurrent(void * pHandleEtronDI, PDEVSELINFO pDevSelInfo, long nProperty, long *pCur, long *pCur2, long *pCapsFlag, int pid);
int ETRONDI_API EtronDI_PropertyCT_GetCurrent(void * pHandleEtronDI, PDEVSELINFO pDevSelInfo, long nProperty, long *pCur, long *pCur2, long *pCapsFlag, int pid);
int ETRONDI_API EtronDI_PropertyPU_SetCurrent(void * pHandleEtronDI, PDEVSELINFO pDevSelInfo, long nProperty, long nCur, long nCur2, long nCapsFlag, int pid);
int ETRONDI_API EtronDI_PropertyCT_SetCurrent(void * pHandleEtronDI, PDEVSELINFO pDevSelInfo, long nProperty, long nCur, long nCur2, long nCapsFlag, int pid);
int ETRONDI_API EtronDI_PropertyItem_Write(void * pHandleEtronDI, PDEVSELINFO pDevSelInfo, REFGUID guid, int nPropertyItem, LONG nValue, int pid);
int ETRONDI_API EtronDI_PropertyItem_Read(void * pHandleEtronDI, PDEVSELINFO pDevSelInfo, REFGUID guid, int nPropertyItem, LONG *pValue, int pid);

int ETRONDI_API EtronDI_ShowPropertyPage( void * pHandleEtronDI, PDEVSELINFO pDevSelInfo, int pid );

#ifdef __cplusplus
}
#endif
