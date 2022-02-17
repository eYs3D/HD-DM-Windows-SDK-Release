#pragma once
#include <windows.h>
//#include "stdafx.h"
#include <stdio.h>

#ifndef APC_API
#ifdef __WEYE__
#define APC_API
#else
#ifdef APC_EXPORTS
#define APC_API __declspec(dllexport)
#else
#define APC_API __declspec(dllimport)
#endif
#endif
#endif

// Include these SDK header files.
#include "..\..\eSPDI\DM\include\eSPDI_DM.h"
#include "..\..\eSPDI\DM\include\eSPDI_Common.h"
#include "..\..\eSPDI\DM\include\eSPDI_ErrCode.h"

void Read3X();
void Write3X();
void Read4X();
void Write4X();
void Read5X();
void Write5X();
void Read24X();
void Write24X();
void ResetUNPData();
void GetUserData(); // Issue 6882

