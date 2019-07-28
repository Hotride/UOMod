/***********************************************************************************
**
** FPSPatchASM.h
**
** Copyright (C) February 2017 Hotride
**
************************************************************************************
*/
//----------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------
extern bool g_FPSPatch;
extern DWORD g_FPS_FunctionPointer;
extern DWORD g_FPS_ReturnAddress;
extern DWORD g_FPS_DataAddress;
extern DWORD g_FPS_Offset1;
extern DWORD g_FPS_Offset2;
//----------------------------------------------------------------------------------
void FixFPS();
void FixFPS_V1();
void FixFPS_V2();
void FixFPS_V3();
void FixFPS_V4();
//----------------------------------------------------------------------------------
