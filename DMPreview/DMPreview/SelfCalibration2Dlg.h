#pragma once

#include <atomic>

#ifdef ESPDI_EG
#include "eSPDI_EG.h"
#else
#include "eSPDI_DM.h"
#endif
#include "PreviewImageDlg.h"
#include "selfK2.h"

static const unsigned short _THERMAL_SENSOR_SLAVE_ADDR_80362 = (0x30);
static const unsigned short _THERMAL_SENSOR_SLAVE_ADDR_8083 = (0x20);

#define _ONSEMI_AR0144_PRESET_ADDR (0x30B4)
#define _ONSEMI_AR0144_PRESET_VALUE (0x0011)
#define _ONSEMI_AR0144_CALIB_DATA_READ_ADDR (0x30C6)
#define _ONSEMI_AR0144_CALIB_DATA_TEMP (55)
#define _ONSEMI_AR0144_TEMP_CURVE_SLOPE (0.7)
#define _ONSEMI_AR0144_TEMP_READ_ADDR (0x30B2)

#define _ONSEMI_AR0135_PRESET_ADDR (0x30B4)
#define _ONSEMI_AR0135_PRESET_VALUE (0x0011)
#define _ONSEMI_AR0135_CALIB_DATA_READ_ADDR (0x30C8)
#define _ONSEMI_AR0135_CALIB_DATA_TEMP (55)
#define _ONSEMI_AR0135_TEMP_CURVE_SLOPE (0.7)
#define _ONSEMI_AR0135_TEMP_READ_ADDR (0x30B2)

#define _STM_VD5X_TEMP_READ_ADDR (0x004C)

#define _1st_ (0) // can be left or top camera
#define _2nd_ (1) // can be right or bottom camera

enum class E_THERMAL_SENSOR_MODEL {
    ONSEMI_AR0135 = 100,
    ONSEMI_AR0144 = 101,

    STM_VD5X = 200,

    OV_OG02 = 300,
    OV_OG01 = 301,

    TI_TMP1075 = 1000,
};

struct PARAM_t {
    float mean_shift_kernel_size_acquisition;
    float mean_shift_kernel_size_tracking;
    bool b_auto_adjust_period;
    float update_period_in_seconds_of_cy_in_acquisition;
    float update_period_in_seconds_of_cy_in_tracking;
    float cy_sampling_period_in_seconds;
    float temperatureThreshold;
    float valid_min_fill_rate_threshold;
    bool b_dynamic_kernel_size;
    float max_devication_of_cy;
    SelfK2::C_Cy_Compensator::E_ESTIMATOR_TYPES e_estimator_type;
    float blind_zone_ratio;
    float fillrate_threshold_to_enter_tracking;
    int convergency_cnt_threshold;
    int max_acq_iterations;
    float smoothing_factor;
};

/* FOR:  calibration pars data set and get */
class CFile_IO_User_Data_Structure_Rectify {
public:
	double M_L[9]; // Inv(NewCamMat1*LRotaMat)
	double M_R[9];

	double K_L[6]; // k1,k2, k3,k4,k5,k6
	double K_R[6]; // k1,k2, k3,k4,k5,k6
	double p1_L;
	double p2_L;
	double p1_R;
	double p2_R;

	double cx_L;
	double cy_L;
	double fx_L;
	double fy_L;

	double cx_R;
	double cy_R;
	double fx_R;
	double fy_R;
};
typedef double FL_t;
// DO NOT change / modify this structure : CFile_IO_Data_Structure_Rectify
class CFile_IO_Data_Structure_Rectify {
public:

	FL_t 	RECTIFY_CONTROL;	/*	, bytes_shift= 0 ,	,High_bit =  7	,Low_bit =  0	, n_bits = 8	, n_bytes = 1	, little_endian= 1 ,	, FractionalBit = 0	, DontCare= 0 , 	*/
	FL_t 	RECTIFY_MODULE_ENABLE;	/*	, bytes_shift= 1 ,	,High_bit =  7	,Low_bit =  0	, n_bits = 8	, n_bytes = 1	, little_endian= 1 ,	, FractionalBit = 0	, DontCare= 0 , 	*/
	FL_t 	RECTIFY_WIDTH;	/*	, bytes_shift= 2 ,	,High_bit =  3	,Low_bit =  0	, n_bits = 12	, n_bytes = 2	, little_endian= 1 ,	, FractionalBit = 0	, DontCare= 0 , 	*/


	FL_t 	RECTIFY_HEIGHT;	/*	, bytes_shift= 4 ,	,High_bit =  3	,Low_bit =  0	, n_bits = 12	, n_bytes = 2	, little_endian= 1 ,	, FractionalBit = 0	, DontCare= 0 , 	*/


	FL_t 	RECTIFY_L_RECT_M00;	/*	, bytes_shift= 6 ,	,High_bit =  7	,Low_bit =  0	, n_bits = 16	, n_bytes = 2	, little_endian= 1 ,	, FractionalBit = 21	, DontCare= 0 , 	*/

	FL_t 	RECTIFY_L_RECT_M01;	/*	, bytes_shift= 8 ,	,High_bit =  2	,Low_bit =  0	, n_bits = 11	, n_bytes = 2	, little_endian= 1 ,	, FractionalBit = 21	, DontCare= 0 , 	*/


	FL_t 	RECTIFY_L_RECT_M02;	/*	, bytes_shift= 10 ,	,High_bit =  2	,Low_bit =  0	, n_bits = 19	, n_bytes = 3	, little_endian= 1 ,	, FractionalBit = 17	, DontCare= 0 , 	*/



	FL_t 	RECTIFY_L_RECT_M10;	/*	, bytes_shift= 13 ,	,High_bit =  2	,Low_bit =  0	, n_bits = 11	, n_bytes = 2	, little_endian= 1 ,	, FractionalBit = 21	, DontCare= 0 , 	*/


	FL_t 	RECTIFY_L_RECT_M11;	/*	, bytes_shift= 15 ,	,High_bit =  7	,Low_bit =  0	, n_bits = 16	, n_bytes = 2	, little_endian= 1 ,	, FractionalBit = 21	, DontCare= 0 , 	*/

	FL_t 	RECTIFY_L_RECT_M12;	/*	, bytes_shift= 17 ,	,High_bit =  2	,Low_bit =  0	, n_bits = 19	, n_bytes = 3	, little_endian= 1 ,	, FractionalBit = 17	, DontCare= 0 , 	*/



	FL_t 	RECTIFY_L_RECT_M20;	/*	, bytes_shift= 20 ,	,High_bit =  3	,Low_bit =  0	, n_bits = 12	, n_bytes = 2	, little_endian= 1 ,	, FractionalBit = 21	, DontCare= 0 , 	*/

	FL_t 	RECTIFY_L_RECT_M21;	/*	, bytes_shift= 22 ,	,High_bit =  1	,Low_bit =  0	, n_bits = 10	, n_bytes = 2	, little_endian= 1 ,	, FractionalBit = 21	, DontCare= 0 , 	*/

	FL_t 	RECTIFY_L_RECT_M22;	/*	, bytes_shift= 24 ,	,High_bit =  2	,Low_bit =  0	, n_bits = 19	, n_bytes = 3	, little_endian= 1 ,	, FractionalBit = 17	, DontCare= 0 , 	*/



	FL_t 	RECTIFY_L_RECT_H_STR;	/*	, bytes_shift= 27 ,	,High_bit =  1	,Low_bit =  0	, n_bits = 10	, n_bytes = 2	, little_endian= 1 ,	, FractionalBit = 0	, DontCare= 0 , 	*/

	FL_t 	RECTIFY_L_RECT_H_END;	/*	, bytes_shift= 29 ,	,High_bit =  2	,Low_bit =  0	, n_bits = 11	, n_bytes = 2	, little_endian= 1 ,	-, FractionalBit = 1	, DontCare= 0 , 	*/


	FL_t 	RECTIFY_L_RECT_V_STR;	/*	, bytes_shift= 31 ,	,High_bit =  1	,Low_bit =  0	, n_bits = 10	, n_bytes = 2	, little_endian= 1 ,	, FractionalBit = 0	, DontCare= 0 , 	*/

	FL_t 	RECTIFY_L_RECT_V_END;	/*	, bytes_shift= 33 ,	,High_bit =  2	,Low_bit =  0	, n_bits = 11	, n_bytes = 2	, little_endian= 1 ,	-, FractionalBit = 1	, DontCare= 0 , 	*/


	FL_t 	RECTIFY_R_RECT_M00;	/*	, bytes_shift= 35 ,	,High_bit =  7	,Low_bit =  0	, n_bits = 16	, n_bytes = 2	, little_endian= 1 ,	, FractionalBit = 21	, DontCare= 0 , 	*/

	FL_t 	RECTIFY_R_RECT_M01;	/*	, bytes_shift= 37 ,	,High_bit =  2	,Low_bit =  0	, n_bits = 11	, n_bytes = 2	, little_endian= 1 ,	, FractionalBit = 21	, DontCare= 0 , 	*/


	FL_t 	RECTIFY_R_RECT_M02;	/*	, bytes_shift= 39 ,	,High_bit =  2	,Low_bit =  0	, n_bits = 19	, n_bytes = 3	, little_endian= 1 ,	, FractionalBit = 17	, DontCare= 0 , 	*/



	FL_t 	RECTIFY_R_RECT_M10;	/*	, bytes_shift= 42 ,	,High_bit =  2	,Low_bit =  0	, n_bits = 11	, n_bytes = 2	, little_endian= 1 ,	, FractionalBit = 21	, DontCare= 0 , 	*/


	FL_t 	RECTIFY_R_RECT_M11;	/*	, bytes_shift= 44 ,	,High_bit =  7	,Low_bit =  0	, n_bits = 16	, n_bytes = 2	, little_endian= 1 ,	, FractionalBit = 21	, DontCare= 0 , 	*/

	FL_t 	RECTIFY_R_RECT_M12;	/*	, bytes_shift= 46 ,	,High_bit =  2	,Low_bit =  0	, n_bits = 19	, n_bytes = 3	, little_endian= 1 ,	, FractionalBit = 17	, DontCare= 0 , 	*/



	FL_t 	RECTIFY_R_RECT_M20;	/*	, bytes_shift= 49 ,	,High_bit =  3	,Low_bit =  0	, n_bits = 12	, n_bytes = 2	, little_endian= 1 ,	, FractionalBit = 21	, DontCare= 0 , 	*/


	FL_t 	RECTIFY_R_RECT_M21;	/*	, bytes_shift= 51 ,	,High_bit =  1	,Low_bit =  0	, n_bits = 10	, n_bytes = 2	, little_endian= 1 ,	, FractionalBit = 21	, DontCare= 0 , 	*/


	FL_t 	RECTIFY_R_RECT_M22;	/*	, bytes_shift= 53 ,	,High_bit =  2	,Low_bit =  0	, n_bits = 19	, n_bytes = 3	, little_endian= 1 ,	, FractionalBit = 17	, DontCare= 0 , 	*/



	FL_t 	RECTIFY_R_RECT_H_STR;	/*	, bytes_shift= 56 ,	,High_bit =  1	,Low_bit =  0	, n_bits = 10	, n_bytes = 2	, little_endian= 1 ,	, FractionalBit = 0	, DontCare= 0 , 	*/


	FL_t 	RECTIFY_R_RECT_H_END;	/*	, bytes_shift= 58 ,	,High_bit =  2	,Low_bit =  0	, n_bits = 11	, n_bytes = 2	, little_endian= 1 ,	-, FractionalBit = 1	, DontCare= 0 , 	*/


	FL_t 	RECTIFY_R_RECT_V_STR;	/*	, bytes_shift= 60 ,	,High_bit =  1	,Low_bit =  0	, n_bits = 10	, n_bytes = 2	, little_endian= 1 ,	, FractionalBit = 0	, DontCare= 0 , 	*/


	FL_t 	RECTIFY_R_RECT_V_END;	/*	, bytes_shift= 62 ,	,High_bit =  2	,Low_bit =  0	, n_bits = 11	, n_bytes = 2	, little_endian= 1 ,	-, FractionalBit = 1	, DontCare= 0 , 	*/


	FL_t 	RECTIFY_L_RECT_P1;	/*	, bytes_shift= 64 ,	,High_bit =  7	,Low_bit =  0	, n_bits = 8	, n_bytes = 1	, little_endian= 1 ,	, FractionalBit = 13	, DontCare= 0 , 	*/
	FL_t 	DontCare_0x41_1byte;	/*	, bytes_shift= 65 ,	,High_bit =  7	,Low_bit =  0	, n_bits = 8	, n_bytes = 1	, little_endian= 1 ,	, FractionalBit = 0	, DontCare= 1 , 	*/
	FL_t 	RECTIFY_L_RECT_P2;	/*	, bytes_shift= 66 ,	,High_bit =  6	,Low_bit =  0	, n_bits = 7	, n_bytes = 1	, little_endian= 1 ,	, FractionalBit = 13	, DontCare= 0 , 	*/

	FL_t 	DontCare_0x43_1byte;	/*	, bytes_shift= 67 ,	,High_bit =  7	,Low_bit =  0	, n_bits = 8	, n_bytes = 1	, little_endian= 1 ,	, FractionalBit = 0	, DontCare= 1 , 	*/
	FL_t 	RECTIFY_L_RECT_K1;	/*	, bytes_shift= 68 ,	,High_bit =  7	,Low_bit =  0	, n_bits = 16	, n_bytes = 2	, little_endian= 1 ,	, FractionalBit = 10	, DontCare= 0 , 	*/

	FL_t 	RECTIFY_L_RECT_K2;	/*	, bytes_shift= 70 ,	,High_bit =  7	,Low_bit =  0	, n_bits = 16	, n_bytes = 2	, little_endian= 1 ,	, FractionalBit = 10	, DontCare= 0 , 	*/

	FL_t 	RECTIFY_L_RECT_K3;	/*	, bytes_shift= 72 ,	,High_bit =  7	,Low_bit =  0	, n_bits = 16	, n_bytes = 2	, little_endian= 1 ,	, FractionalBit = 10	, DontCare= 0 , 	*/

	FL_t 	RECTIFY_L_RECT_K4;	/*	, bytes_shift= 74 ,	,High_bit =  7	,Low_bit =  0	, n_bits = 16	, n_bytes = 2	, little_endian= 1 ,	, FractionalBit = 10	, DontCare= 0 , 	*/

	FL_t 	RECTIFY_L_RECT_K5;	/*	, bytes_shift= 76 ,	,High_bit =  7	,Low_bit =  0	, n_bits = 16	, n_bytes = 2	, little_endian= 1 ,	, FractionalBit = 10	, DontCare= 0 , 	*/

	FL_t 	RECTIFY_L_RECT_K6;	/*	, bytes_shift= 78 ,	,High_bit =  7	,Low_bit =  0	, n_bits = 16	, n_bytes = 2	, little_endian= 1 ,	, FractionalBit = 10	, DontCare= 0 , 	*/

	FL_t 	RECTIFY_L_RECT_CX;	/*	, bytes_shift= 80 ,	,High_bit =  7	,Low_bit =  0	, n_bits = 16	, n_bytes = 2	, little_endian= 1 ,	, FractionalBit = 2	, DontCare= 0 , 	*/

	FL_t 	RECTIFY_L_RECT_CY;	/*	, bytes_shift= 82 ,	,High_bit =  7	,Low_bit =  0	, n_bits = 16	, n_bytes = 2	, little_endian= 1 ,	, FractionalBit = 2	, DontCare= 0 , 	*/

	FL_t 	RECTIFY_L_RECT_FX;	/*	, bytes_shift= 84 ,	,High_bit =  1	,Low_bit =  0	, n_bits = 18	, n_bytes = 3	, little_endian= 1 ,	, FractionalBit = 3	, DontCare= 0 , 	*/



	FL_t 	RECTIFY_L_RECT_FY;	/*	, bytes_shift= 87 ,	,High_bit =  1	,Low_bit =  0	, n_bits = 18	, n_bytes = 3	, little_endian= 1 ,	, FractionalBit = 3	, DontCare= 0 , 	*/



	FL_t 	RECTIFY_R_RECT_P1;	/*	, bytes_shift= 90 ,	,High_bit =  7	,Low_bit =  0	, n_bits = 8	, n_bytes = 1	, little_endian= 1 ,	, FractionalBit = 13	, DontCare= 0 , 	*/
	FL_t 	RECTIFY_R_RECT_P2;	/*	, bytes_shift= 91 ,	,High_bit =  6	,Low_bit =  0	, n_bits = 7	, n_bytes = 1	, little_endian= 1 ,	, FractionalBit = 13	, DontCare= 0 , 	*/

	FL_t 	RECTIFY_R_RECT_K1;	/*	, bytes_shift= 92 ,	,High_bit =  7	,Low_bit =  0	, n_bits = 16	, n_bytes = 2	, little_endian= 1 ,	, FractionalBit = 10	, DontCare= 0 , 	*/

	FL_t 	RECTIFY_R_RECT_K2;	/*	, bytes_shift= 94 ,	,High_bit =  7	,Low_bit =  0	, n_bits = 16	, n_bytes = 2	, little_endian= 1 ,	, FractionalBit = 10	, DontCare= 0 , 	*/

	FL_t 	RECTIFY_R_RECT_K3;	/*	, bytes_shift= 96 ,	,High_bit =  7	,Low_bit =  0	, n_bits = 16	, n_bytes = 2	, little_endian= 1 ,	, FractionalBit = 10	, DontCare= 0 , 	*/

	FL_t 	RECTIFY_R_RECT_K4;	/*	, bytes_shift= 98 ,	,High_bit =  7	,Low_bit =  0	, n_bits = 16	, n_bytes = 2	, little_endian= 1 ,	, FractionalBit = 10	, DontCare= 0 , 	*/

	FL_t 	RECTIFY_R_RECT_K5;	/*	, bytes_shift= 100 ,	,High_bit =  7	,Low_bit =  0	, n_bits = 16	, n_bytes = 2	, little_endian= 1 ,	, FractionalBit = 10	, DontCare= 0 , 	*/

	FL_t 	RECTIFY_R_RECT_K6;	/*	, bytes_shift= 102 ,	,High_bit =  7	,Low_bit =  0	, n_bits = 16	, n_bytes = 2	, little_endian= 1 ,	, FractionalBit = 10	, DontCare= 0 , 	*/

	FL_t 	RECTIFY_R_RECT_CX;	/*	, bytes_shift= 104 ,	,High_bit =  7	,Low_bit =  0	, n_bits = 16	, n_bytes = 2	, little_endian= 1 ,	, FractionalBit = 2	, DontCare= 0 , 	*/

	FL_t 	RECTIFY_R_RECT_CY;	/*	, bytes_shift= 106 ,	,High_bit =  7	,Low_bit =  0	, n_bits = 16	, n_bytes = 2	, little_endian= 1 ,	, FractionalBit = 2	, DontCare= 0 , 	*/

	FL_t 	RECTIFY_R_RECT_FX;	/*	, bytes_shift= 108 ,	,High_bit =  1	,Low_bit =  0	, n_bits = 18	, n_bytes = 3	, little_endian= 1 ,	, FractionalBit = 3	, DontCare= 0 , 	*/



	FL_t 	RECTIFY_R_RECT_FY;	/*	, bytes_shift= 111 ,	,High_bit =  1	,Low_bit =  0	, n_bits = 18	, n_bytes = 3	, little_endian= 1 ,	, FractionalBit = 3	, DontCare= 0 , 	*/


	FL_t 	RECTIFY_PREP_PROCESS;	/*	, bytes_shift= 114 ,	,High_bit =  7	,Low_bit =  0	, n_bits = 8	, n_bytes = 1	, little_endian= 1 ,	, FractionalBit = 0	, DontCare= 0 , 	*/
	FL_t 	RECTIFY_PREP_SCAL_H_FAC;	/*	, bytes_shift= 115 ,	,High_bit =  4	,Low_bit =  0	, n_bits = 13	, n_bytes = 2	, little_endian= 0 ,	, FractionalBit = 9	, DontCare= 0 , 	*/


	FL_t 	RECTIFY_PREP_SCAL_V_FAC;	/*	, bytes_shift= 117 ,	,High_bit =  4	,Low_bit =  0	, n_bits = 13	, n_bytes = 2	, little_endian= 0 ,	, FractionalBit = 9	, DontCare= 0 , 	*/


	FL_t 	RECTIFY_PREP_YUV_H_START;	/*	, bytes_shift= 119 ,	,High_bit =  2	,Low_bit =  0	, n_bits = 11	, n_bytes = 2	, little_endian= 0 ,	-, FractionalBit = 1	, DontCare= 0 , 	*/


	FL_t 	RECTIFY_PREP_YUV_H_SIZE;	/*	, bytes_shift= 121 ,	,High_bit =  2	,Low_bit =  0	, n_bits = 11	, n_bytes = 2	, little_endian= 0 ,	-, FractionalBit = 1	, DontCare= 0 , 	*/


	FL_t 	RECTIFY_PREP_YUV_V_START;	/*	, bytes_shift= 123 ,	,High_bit =  2	,Low_bit =  0	, n_bits = 11	, n_bytes = 2	, little_endian= 0 ,	-, FractionalBit = 1	, DontCare= 0 , 	*/


	FL_t 	RECTIFY_PREP_YUV_V_SIZE;	/*	, bytes_shift= 125 ,	,High_bit =  2	,Low_bit =  0	, n_bits = 11	, n_bytes = 2	, little_endian= 0 ,	-, FractionalBit = 1	, DontCare= 0 , 	*/


	FL_t 	RECTIFY_BLK_WIDTH;	/*	, bytes_shift= 127 ,	,High_bit =  7	,Low_bit =  0	, n_bits = 8	, n_bytes = 1	, little_endian= 1 ,	, FractionalBit = 0	, DontCare= 0 , 	*/
	FL_t 	DontCare_0x80_16bytes;	/*	, bytes_shift= 128 ,	,High_bit =  127	,Low_bit =  0	, n_bits = 128	, n_bytes = 16	, little_endian= 1 ,	, FractionalBit = 0	, DontCare= 1 , 	*/
	FL_t 	DontCare_0x90_16bytes;	/*	, bytes_shift= 144 ,	,High_bit =  127	,Low_bit =  0	, n_bits = 128	, n_bytes = 16	, little_endian= 1 ,	, FractionalBit = 0	, DontCare= 1 , 	*/
	FL_t 	DontCare_0xa0_15bytes;	/*	, bytes_shift= 160 ,	,High_bit =  119	,Low_bit =  0	, n_bits = 120	, n_bytes = 15	, little_endian= 1 ,	, FractionalBit = 0	, DontCare= 1 , 	*/
	FL_t 	RPOST_YUVP_ENB;	/*	, bytes_shift= 175 ,	,High_bit =  7	,Low_bit =  0	, n_bits = 8	, n_bytes = 1	, little_endian= 1 ,	, FractionalBit = 0	, DontCare= 0 , 	*/
	FL_t 	DontCare_0xb0_10bytes;	/*	, bytes_shift= 176 ,	,High_bit =  79	,Low_bit =  0	, n_bits = 80	, n_bytes = 10	, little_endian= 1 ,	, FractionalBit = 0	, DontCare= 1 , 	*/
	FL_t 	RPOST_SCAL_H_FAC;	/*	, bytes_shift= 186 ,	,High_bit =  4	,Low_bit =  0	, n_bits = 13	, n_bytes = 2	, little_endian= 1 ,	, FractionalBit = 9	, DontCare= 0 , 	*/


	FL_t 	RPOST_SCAL_V_FAC;	/*	, bytes_shift= 188 ,	,High_bit =  4	,Low_bit =  0	, n_bits = 13	, n_bytes = 2	, little_endian= 1 ,	, FractionalBit = 9	, DontCare= 0 , 	*/


	FL_t 	RPOST_YUV_H_STR;	/*	, bytes_shift= 190 ,	,High_bit =  2	,Low_bit =  0	, n_bits = 11	, n_bytes = 2	, little_endian= 1 ,	-, FractionalBit = 1	, DontCare= 0 , 	*/

	FL_t 	RPOST_YUV_H_SIZE;	/*	, bytes_shift= 191 ,	,High_bit =  6	,Low_bit =  4	, n_bits = 11	, n_bytes = 2	, little_endian= 0 ,	-, FractionalBit = 1	, DontCare= 0 , 	*/

	FL_t 	RPOST_YUV_V_STR;	/*	, bytes_shift= 193 ,	,High_bit =  2	,Low_bit =  0	, n_bits = 11	, n_bytes = 2	, little_endian= 1 ,	-, FractionalBit = 1	, DontCare= 0 , 	*/

	FL_t 	RPOST_YUV_V_SIZE;	/*	, bytes_shift= 194 ,	,High_bit =  6	,Low_bit =  4	, n_bits = 11	, n_bytes = 2	, little_endian= 0 ,	-, FractionalBit = 1	, DontCare= 0 , 	*/

	FL_t 	DontCare_0xc4_2bytes;	/*	, bytes_shift= 196 ,	,High_bit =  15	,Low_bit =  0	, n_bits = 16	, n_bytes = 2	, little_endian= 1 ,	, FractionalBit = 0	, DontCare= 1 , 	*/

};

static std::atomic<bool> gStopGettingTemperature(false);
static float gTemperature(0.0);

class SelfCalibration2Dlg : public CDialog
{
    /*DECLARE_DYNAMIC( SelfCalibration2Dlg )
    DECLARE_MESSAGE_MAP()*/

public:
    enum { IDD = IDD_SELF_K2_DIALOG};

    SelfCalibration2Dlg(void*& hApcDI, DEVSELINFO& devSelInfo, const DEVINFORMATIONEX& devinfoEx, CWnd* pParent, CPreviewImageDlg* pPreviewDlg);
    virtual ~SelfCalibration2Dlg();

    void SetImageParams(int imgWidth, int imgHeight, int FileSystemIndex, eSPCtrl_RectLogData* rectifyData);
    void ApplyInputImage(APCImageType::Value imgType, int imgId, std::vector<unsigned char>& bufDepth, int imgSize, int width, int height, int serialNumber);
    void StopSelfK2();

    void*& m_hApcDI;
    DEVSELINFO& m_DevSelInfo;
    const DEVINFORMATIONEX& m_xDevinfoEx;
    bool m_isSensorSlave;
    afx_msg void OnBnClickedBtnRun();

    CPreviewImageDlg* m_pPreviewDlg;
    int m_DepthWidth;
    int m_DepthHeight;
    eSPCtrl_RectLogData* m_RectifyData;
    std::mutex m_CompensatorMutex;
    std::vector<unsigned char> m_Depth;
    std::atomic<bool> m_Run;
    std::atomic<bool> m_Reset;
    std::atomic<bool> m_ThreadTerminated;
    int m_FileSystemIndex;
    stringstream m_OutAndInfo;

    struct PARAM_t m_cy_compensator_param;
    //Cy Compensator
    SelfK2::C_Cy_Compensator* m_cy_compensator;
    //Focal Compensator
    SelfK2::C_Focal_Compensator* m_focal_compensator;

private:
    virtual BOOL OnInitDialog();
    virtual void DoDataExchange( CDataExchange* pDX );
    virtual void OnOK() {};
    virtual void OnCancel() {};
    bool checkDepth();
    void StartSelfK2();

public:
    DECLARE_MESSAGE_MAP()
    afx_msg void OnBnClickedBtnReset();
    afx_msg LRESULT OnUpdateText( WPARAM wparam,LPARAM lparam );
    afx_msg void OnBnClickedBtnWrite();
};