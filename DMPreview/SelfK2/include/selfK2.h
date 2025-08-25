
/**
  ******************************************************************************
  * @file    selfK2.h
  * @author  Red.Kuo@eYs3D.com
  * @version V1.0.0
  * @date    17-OCT-2023
  * @brief   compensating structure compensating.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright 2022 (c) eYs3D Microelectronics,Co. All rights reserved. </center></h2>
  ******************************************************************************
  */



#pragma once


#ifdef SELFK2_EXPORTS
#ifndef SelfK2_API
#define SelfK2_API   __declspec(dllexport)
#endif
#else
#ifndef SelfK2_API
#define SelfK2_API  __declspec(dllimport)
#endif
#endif

#ifndef _SELFK2_H_
#define _SELFK2_H_




  /* Includes ------------------------------------------------------------------*/

#ifdef __linux__
#include <unistd.h>
#endif
#include <iostream>
#include <stdlib.h>
#include <omp.h>
#include <numeric>      // std::accumulate
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>    // std::min
#include <cstddef>  // for size_t


// sub modules
#include <deque>
#include <limits>
#include <random>
#include <map>
#include <thread>
#include <chrono>

// SDK
#include "eSPDI_Common.h"
#include "eSPDI_DM.h"

#define INDEX_OF_CENTER_CY (0)



static const unsigned short PUMA_REG_ADDR_CY2[2] = { 0xf56A,0xf56B };
static const unsigned short PUMA_REG_ADDR_FX1[3] = { 0xf554,0xf555,0xf556 };
static const unsigned short PUMA_REG_ADDR_FY1[3] = { 0xf557,0xf558,0xf559 };
static const unsigned short PUMA_REG_ADDR_FX2[3] = { 0xf56C,0xf56D,0xf56E };
static const unsigned short PUMA_REG_ADDR_FY2[3] = { 0xf56F,0xf570,0xf571 };

extern "C" SelfK2_API  void getSortedOrder(const float arr[], int sortedOrder[], int size, int& minIndex, int& maxIndex);

template<typename T>
double calculateNonZeroRatio(const T* data, size_t size);

template<typename T>
double calculateNonZeroRatio(const T* data, int size, int n_cols, int n_rows, float blind_zone_ratio = 0.25, bool b_subwin = false, int sx = 0, int sy = 0, int roi_w = 0, int roi_h = 0);

extern "C" SelfK2_API  float Get_HW_Reg_2Bytes_Data(unsigned short addrs[2], int fractional);

extern "C" SelfK2_API  float Get_HW_Reg_3Bytes_Data(unsigned short addrs[3], int fractional);

extern "C" SelfK2_API  int Set_HW_Reg_2Bytes_Data(unsigned short addrs[2], float value, int fractional);

extern "C" SelfK2_API  int Set_HW_Reg_3Bytes_Data(unsigned short addrs[3], float value, int fractional);

// Define a function to find the maximum value in an array
extern "C" SelfK2_API  float _findMax(const float arr[], int size);

// Define a function to find the minimum value in an array
extern "C" SelfK2_API  float _findMin(const float arr[], int size);

extern "C" SelfK2_API  float _findAvg(const float arr[], int size);


class SelfK2_API RollingVariance {
public:
	RollingVariance(int windowSize) : windowSize(windowSize), mean(0.0), M2(0.0), n(0) {}

	void addDataPoint(double x) {
		std::cout << "@addDataPoint:" << std::endl;

		n++;
		std::cout << "n :" << n << std::endl;
		double delta = x - mean;
		mean += delta / n;
		double delta2 = x - mean;
		M2 += delta * delta2;

		// If we have more than 'windowSize' data points, remove the oldest one
		if (n > windowSize) {
			double removedValue = dataPoints.front();
			dataPoints.pop_front();
			double removedDelta = removedValue - mean;
			mean -= removedDelta / windowSize;
			double removedDelta2 = removedValue - mean;
			M2 -= removedDelta * removedDelta2;
		}

		// Ensure M2 is not negative
		if (M2 < 0.0) {
			M2 = 0.0;
		}

		dataPoints.push_back(x);

		std::cout << "x:" << x << std::endl;
		for (double tmp : dataPoints) {
			std::cout << tmp << " ,";
		}
		std::cout << std::endl;
	}

	double getVariance() {
		float n_tmp = dataPoints.size();
		if (n_tmp < 2) {
			return std::numeric_limits<double>::quiet_NaN(); // 返回NaN表示方差不可用
		}
		//cout << "M2 :" << M2 << endl;
		//cout << "n_tmp :" << n_tmp << endl;
		return M2 / (n_tmp - 1);
	}

private:
	int windowSize;
	double mean;
	double M2;
	int n;
	std::deque<double> dataPoints;
};

 class SelfK2_API SelfK2 {
public:

	class SelfK2_API ROI {
	public:
		int sx = 0;
		int sy = 0;
		int width = 0;
		int height = 0;

		ROI(int sx, int sy, int width, int height) {
			this->sx = sx;
			this->sy = sy;
			this->width = width;
			this->height = height;
		};
		ROI() {
		};

	};


	static void set_hw_handler(
		void* pHandleApcDI,
		DEVSELINFO* pDevSelInfo,
		DEVINFORMATIONEX* pDevinfo // @20230825
	);

	static ROI getRandomROI(int n_cols, int n_rows, double rat_x, double rat_y);

	class SelfK2_API C_Cy_Compensator {
	public:
		enum class E_STATES {
			_init = 0,
			_idle = 1,
			_estimating = 2,
		};

		std::map<int, std::string> map_error_code = {
			{0, "_OK"},
			{-1, "_Not_Valid_Current_Temperature"},
			{-2, "_Not_Valid_Calibration_Temperature"},
			{-3, "_Data_Sampling_Not_Successed"},
			{-99, "_Unknown"}
		};

		enum class E_ERROR {
			_OK = 0,
			_Not_Valid_Current_Temperature = -1,
			_Not_Valid_Calibration_Temperature = -2,
			_Data_Sampling_Not_Successed = -3,
			_Unknown = -99,
		};

		enum class E_ESTIMATOR_TYPES {
			_mean_shift = 0,
			_confidence_weighted_ranking = 1,
		};

		class Pars {
		public: // can be adjust @on the fly.
			float smoothing_factor = 0.5;
			float blind_zone_ratio = 0.00;
			int n_cols = 0;
			int n_rows = 0;
			float fillrate_threshold_to_enter_tracking = 0.997;
			//float min_sampling_dist_in_mm = 200; // used for calculating Fillrate of valid ROI. 
			bool b_auto_adjust_period = true;
			float update_period_in_seconds_of_cy_in_acquisition = 1.0;
			float update_period_in_seconds_of_cy_in_tracking = 60;
			float cy_sampling_period_in_seconds = 0.15;
			float tracking_stable_range = 0.25;
			float tracking_fail_cnt_threshold = 3;
			bool b_random_sampling = false;


			//float calib_temprature_in_celsius = NAN;
			float temperatureThreshold = 1.0;
			int convergency_cnt_threshold = 3;
			int max_acq_iterations = 30;
			float valid_min_fill_rate_threshold = 0.10;
			int n_valid_test_cys_threahold = 2;
			int mean_shift_kernel_size_for_acquisition = 1;
			int mean_shift_kernel_size_for_tracking = 1;
			bool b_dynamic_kernel_size = true;
			float max_devication_of_cy = 2.5;
			E_ESTIMATOR_TYPES e_estimator_type = E_ESTIMATOR_TYPES::_confidence_weighted_ranking;


		private: // these pars are highly related to/ IC config, please DO NOT adjust.
			float increment_step_in_cy_test = 0.25; // puma
			float magnification_for_setting_register_cy = 4.0;
			unsigned short reg_addrs_cy[2] = { PUMA_REG_ADDR_CY2[0],PUMA_REG_ADDR_CY2[1] };
			float window_size_for_variance_estimation_fill_rate = 10;
			float window_size_for_variance_estimation_cy = 10;
			float window_size_for_variance_estimation_temp = 10;

			friend class  C_Cy_Compensator;
		};

		class C_Oputs {
		public:
			float comp_cy = 0;
			float default_cy = 0;
			float estimaed_cy = 0;
			float default_cy_in_register = 0;
		};

		class C_Info {
		public:
			bool* p_f_in_acquisition = nullptr;
			int* p_n_acq_iterations = nullptr;
			unsigned long* p_i_test = nullptr;
			unsigned long* p_i_successfully_test = nullptr;
			unsigned long* p_i_update_pars = nullptr;
			std::vector <float> v_fill_rate_of_cys;
			std::vector <float> v_test_cys;
			unsigned int* p_convergency_cnt = nullptr;
			E_STATES e_states = E_STATES::_idle;
		};

		Pars pars;
		C_Oputs oputs;
		C_Info info;

		int acq_or_tracking_mode_switch();

		// can be used to set init cy, user can use the init cy based on the compensation tracking history.
		int _set_current_cy(float cy);

		float _get_current_cy();

		int _reset(float init_cy = NAN);

		template<typename T>
		int run(const T* p_depth, int n_depth_pixels, float current_temp);

		C_Cy_Compensator(
			const int mean_shift_kernel_size_for_acquisition = 1,
			const int mean_shift_kernel_size_for_tracking = 1,
			float init_cy = NAN,
			const bool b_locked_in_acquisition_mode = false,
			float increment_step_in_cy_test = 0.25,
			float magnification_for_setting_register_cy = 4.0
		) {
			this->pars.increment_step_in_cy_test = increment_step_in_cy_test;
			this->pars.magnification_for_setting_register_cy = magnification_for_setting_register_cy;

			this->b_locked_in_acquisition_mode = b_locked_in_acquisition_mode;

			info.p_f_in_acquisition = &f_in_acquisition;
			info.p_n_acq_iterations = &n_acq_iterations;
			info.p_i_test = &i_test;
			info.p_i_successfully_test = &i_successfully_test;
			info.p_i_update_pars = &i_update_pars;
			info.p_convergency_cnt = &accmulated_cnt;

			period_for_running_oc_section = pars.update_period_in_seconds_of_cy_in_acquisition;

			this->pars.mean_shift_kernel_size_for_acquisition = mean_shift_kernel_size_for_acquisition;
			this->pars.mean_shift_kernel_size_for_tracking = mean_shift_kernel_size_for_tracking;
			this->mean_shift_kernel_size = mean_shift_kernel_size_for_acquisition;
			_update_ms_kernel_size(this->mean_shift_kernel_size);


			T_now = clock();
			T_last_oc = T_now;

			p_c_var_estimator_fill_rate = new RollingVariance(pars.window_size_for_variance_estimation_fill_rate);
			p_c_var_estimator_cy = new RollingVariance(pars.window_size_for_variance_estimation_cy);
			p_c_var_estimator_temp = new RollingVariance(pars.window_size_for_variance_estimation_temp);



			oputs.default_cy_in_register = Get_HW_Reg_2Bytes_Data(pars.reg_addrs_cy, pars.magnification_for_setting_register_cy);
			std::cout << "oputs.default_cy_in_register: "<< oputs.default_cy_in_register << std::endl;

			if (isnan(init_cy)) {
				std::cout << "no init cy set." << std::endl;
				// get default cy (calibratored value)
				default_cy = oputs.default_cy_in_register;
				estimated_cy = default_cy;
				oputs.default_cy = default_cy;
			}
			else {
				std::cout << "default_cy_in_register: " << oputs.default_cy_in_register << std::endl;
				std::cout << "init cy = " << init_cy << std::endl;
				Set_HW_Reg_2Bytes_Data(pars.reg_addrs_cy, init_cy, pars.magnification_for_setting_register_cy);
				default_cy = init_cy;
				estimated_cy = default_cy;
				oputs.default_cy = default_cy;
			}
		}

		~C_Cy_Compensator() {
			delete[] p_test_cys;
			delete[] p_fill_rates;
			delete[] p_rank_of_fill_rates;
			delete[] p_increments;

			delete p_c_var_estimator_fill_rate;
			delete p_c_var_estimator_cy;
			delete p_c_var_estimator_temp;

			info.v_test_cys.clear();
			info.v_fill_rate_of_cys.clear();
		}


	private:
		float _smoothing_factor_tracking = 0.5;
		float _smoothing_factor = 0.5;
		bool b_bypass = false;
		float period_for_running_oc_section = 0;
		unsigned int accmulated_cnt = 0;
		bool f_estimating_cy = false;

		unsigned long i_test = 0;
		unsigned long i_successfully_test = 0;
		unsigned long i_update_pars = 0;

		int i_order = 0;
		//float default_cy_in_register = 0;
		float default_cy = 0;
		float center_cy = 0;
		float last_center_cy = 0;
		int n_center_cy_no_change = 0;
		float estimated_cy = 0;
		float cy_to_update = 0;
		int mean_shift_kernel_size = 1;
		int n_mean_shift_samples = 0; // = 2*mean_shift_kernel_size+1
		float* p_test_cys = nullptr;
		float* p_fill_rates = nullptr;
		int* p_rank_of_fill_rates = nullptr;
		float* p_increments = nullptr;
		int last_max_fillrate_index = 0;
		int n_winning_streak = 0;

		// time
		clock_t T_now = 0;
		clock_t T_last_oc = 0;
		float current_temperature = 0;
		float last_temperature = 0;
		bool b_locked_in_acquisition_mode = false;
		bool f_in_acquisition = true;
		bool f_stable_mode_switch = false;
		bool f_switch_to_tracking_mode = false;
		int n_persuit = 0;
		int n_acq_iterations = 0;
		RollingVariance* p_c_var_estimator_fill_rate;
		RollingVariance* p_c_var_estimator_cy;
		RollingVariance* p_c_var_estimator_temp;
		float stability = 0;
		bool f_error = false;
		ROI sampling_roi = ROI(0, 0, 0, 0);
		float min_random_sampling_roi_ratio = 0.5;

		int _update_ms_kernel_size(int _size);

		float _cal_blind_zone(
			float baseline_in_mm,
			float fx_in_pixels,
			float z_in_mm,
			float margin_in_pixels
		);

		int _reset_pars_and_status(
			bool f_in_acquisition = true,
			float center_cy = NAN
		);


	};

	class SelfK2_API C_Focal_Compensator {
	public:

		enum class E_STATES {
			_init = 0,
			_idle = 1,
			_estimating = 2,
		};

		class Pars {
		public:
			// can be adjust @on the fly.
			float update_period_in_seconds_of_focal_init = 1;
			float update_period_in_seconds_of_focal_ordinary = 30;
			float alpha = 0; //cam_thermal_variation_rate_of_focal
			float calib_temperature = NAN;
			float temperatureThreshold = NAN;


		private:

			// these pars are highly related to IC config, please DO NOT adjust.
			float magnification_for_setting_register_focal = 8.0;

			unsigned short reg_addrs_fx1[3] = { PUMA_REG_ADDR_FX1[0], PUMA_REG_ADDR_FX1[1], PUMA_REG_ADDR_FX1[2] };
			unsigned short reg_addrs_fy1[3] = { PUMA_REG_ADDR_FY1[0], PUMA_REG_ADDR_FY1[1], PUMA_REG_ADDR_FY1[2] };
			unsigned short reg_addrs_fx2[3] = { PUMA_REG_ADDR_FX2[0], PUMA_REG_ADDR_FX2[1], PUMA_REG_ADDR_FX2[2] };
			unsigned short reg_addrs_fy2[3] = { PUMA_REG_ADDR_FY2[0], PUMA_REG_ADDR_FY2[1], PUMA_REG_ADDR_FY2[2] };

			friend class  C_Focal_Compensator;
		};

		std::map<int, std::string> map_error_code = {
			{0, "_OK"},
			{-1, "_Not_Valid_Current_Temperature"},
			{-2, "_Not_Valid_Calibration_Temperature"},
			{-3, "_Not_Valid_LR_cam_thermal_variation_rate_of_focal"},
			{-99,"_Unknown"}
		};

		enum class E_ERROR {
			_OK = 0,
			_Not_Valid_Current_Temperature = -1,
			_Not_Valid_Calibration_Temperature = -2,
			_Not_Valid_LR_cam_thermal_variation_rate_of_focal = -3,
			_Unknown = -99,
		};

		class C_Oputs {
		public:
			float comp_fx1 = 0;
			float comp_fy1 = 0;
			float comp_fx2 = 0;
			float comp_fy2 = 0;

		};

		class C_Info {
		public:
			E_STATES e_states = E_STATES::_init;
		};

	private:
		float init_fx1 = 0, init_fy1 = 0;
		float init_fx2 = 0, init_fy2 = 0;
		clock_t T_now = clock();
		clock_t T_last_focal = T_now;
		float period_for_running_focal_section = 0;

		float current_temperature = 0;
		float last_temperature = 0;

	public:
		Pars pars;
		C_Oputs oputs;
		C_Info info;

		C_Focal_Compensator(
			float calib_temperature,
			float alpha,
			float update_period_in_seconds_of_focal_init = 0.5,
			float update_period_in_seconds_of_focal_ordinary = 30.0,
			float magnification_for_setting_register_focal = 8
		) {
			this->pars.magnification_for_setting_register_focal = magnification_for_setting_register_focal;

			pars.calib_temperature = calib_temperature;
			pars.alpha = alpha;

			pars.update_period_in_seconds_of_focal_init = update_period_in_seconds_of_focal_init;
			pars.update_period_in_seconds_of_focal_ordinary = update_period_in_seconds_of_focal_ordinary;
			period_for_running_focal_section = update_period_in_seconds_of_focal_init;

			T_now = clock();
			clock_t T_last_focal = 0;
			T_last_focal = T_now;

			_read_reg_and_set_to_the_init_focal();
		}

		~C_Focal_Compensator() {

		}

		int set_focal_reg_addrs(unsigned short* p_reg_addrs_fx1, unsigned short* p_reg_addrs_fy1, unsigned short* p_reg_addrs_fx2, unsigned short* p_reg_addrs_fy2);

		int _read_reg_and_set_to_the_init_focal();

		int _reset_focal_reg_to_the_init_value();

		int _reset();

		int run(float current_temp);

	};




};



#endif //_SELFK2_H_


/************************ Copyright 2022 (C) eYs3D Microelectronics,Co. All rights reserved.  *****END OF FILE****/



