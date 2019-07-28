/***********************************************************************************
**
** stdafx.h
**
** Copyright (C) January 2017 Hotride
**
************************************************************************************
*/
//----------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------
#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

#include <string>
#include <vector>
//----------------------------------------------------------------------------------
enum PATCH_TYPE
{
	PT_FPS = 1,
	PT_STAMINA,
	PT_ALWAYS_LIGHT,
	PT_PAPERDOLL_SLOTS,
	PT_SPLASH_SCREEN,
	PT_RESOLUTION,
	PT_OPTIONS_NOTIFICATION,
	PT_MULTI_UO,
	PT_NO_CRYPT,
	PT_GLOBAL_SOUND,
	PT_VIEW_RANGE,
	PT_GM_STEP,
	PT_COUNT
};
//----------------------------------------------------------------------------------
enum PATCH_STATE
{
	PS_DISABLE = 0,
	PS_ENABLE,
	PS_NOT_FOUND
};
//----------------------------------------------------------------------------------
enum PATCH_MESSAGES
{
	PM_INSTALL = WM_USER + 666,
	PM_INFO,
	PM_ENABLE,
	PM_DISABLE,
	PM_VIEW_RANGE_VALUE
};
//----------------------------------------------------------------------------------
