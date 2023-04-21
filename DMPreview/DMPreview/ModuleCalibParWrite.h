#pragma once

#ifndef _Module_Calib_Par_Write_H_
#define _Module_Calib_Par_Write_H_

#include <vector>
#include <iostream>
#include "eSPDI_Common.h"
#include "eSPDI_DM.h"

using namespace std;

// write camera parameter to module
int ReadRectify_Table_Data(eSPCtrl_RectLogData &rectifyData, void *pHandleEtronDI, PDEVSELINFO pDevSelInfo,int index);
int Updata_ROI(eSPCtrl_RectLogData &rectifyData, void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, int index);
int WriteRectLogData_temp(eSPCtrl_RectLogData &rectifyData, void *pHandleEtronDI, PDEVSELINFO pDevSelInfo);
int WriteRectification(eSPCtrl_RectLogData &rectifyData, void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, int index);
int WriteRectifyLogData(eSPCtrl_RectLogData &rectifyData, void *pHandleEtronDI, PDEVSELINFO pDevSelInfo, int index);
void Mat_Product(double *A, double *B, double *C);
void Mat_Inverse(double *A, double *B);
void writeBuffer(vector<int> nums, unsigned char *buffer, int bits, int input);
void writeBuffer(int start,int nums,unsigned char *buffer,int bits,int input);
int readBuffer(vector<int> nums, unsigned char *buffer, int bits);
int readBuffer(int start,int nums,unsigned char *buffer,int bits);

#endif