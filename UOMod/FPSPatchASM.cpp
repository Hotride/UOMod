/***********************************************************************************
**
** FPSPatchASM.cpp
**
** Copyright (C) February 2017 Hotride
**
************************************************************************************
*/
//----------------------------------------------------------------------------------
#include "stdafx.h"
#include "FPSPatchASM.h"
//----------------------------------------------------------------------------------
bool g_FPSPatch = false;
DWORD g_FPS_FunctionPointer = 0;
DWORD g_FPS_ReturnAddress = 0;
DWORD g_FPS_DataAddress = 0;
DWORD g_FPS_Offset1 = 0;
DWORD g_FPS_Offset2 = 0;
//----------------------------------------------------------------------------------
__declspec(naked) void FixFPS()
{
	__asm
	{
		push esi
		call dword ptr[g_FPS_FunctionPointer]
		jmp g_FPS_ReturnAddress
	}
}
//1.26.0 - 2.0.4a;     3.0.8z - 5.0.8.4
//3.0.6e - 3.0.8q
//----------------------------------------------------------------------------------
__declspec(naked) void FixFPS_V1()
{
	__asm
	{
		cmp byte ptr[g_FPSPatch], 0
		je loc_fps_patch_v1
		mov dl, 1
		jmp loc_fps_patch_v2
		loc_fps_patch_v1 :
		xor dl, dl
		loc_fps_patch_v2 :
		mov eax, [esp + 4]
		push ebx
		xor ebx, ebx
		mov ebx, dword ptr[g_FPS_Offset1]
		cmp dword ptr[eax + ebx], 0
		mov ebx, dword ptr[g_FPS_Offset2]
		movzx eax, word ptr[eax + ebx]
		pop ebx
		je loc_fps_patch_v3
		shl eax, 1
		add eax, dword ptr[g_FPS_DataAddress]
		add eax, 1
		movzx eax, byte ptr[eax]
		xor ecx, ecx
		test dl, dl
		setne cl
		sub eax, ecx
		ret 4
		loc_fps_patch_v3:
		shl eax, 1
		add eax, dword ptr[g_FPS_DataAddress]
		movzx eax, byte ptr[eax]
		movzx ecx, dl
		neg ecx
		sbb ecx, ecx
		and ecx, 2
		sub eax, ecx
		dec eax
		ret 4
	}
}
//2.0.4c - 3.0.6a
//----------------------------------------------------------------------------------
__declspec(naked) void FixFPS_V2()
{
	__asm
	{
		cmp byte ptr[g_FPSPatch], 0
		je loc_fps_patch_v1
		mov dl, 1
		jmp loc_fps_patch_v2
		loc_fps_patch_v1 :
		xor dl, dl
		loc_fps_patch_v2 :
		mov eax, [esp + 4]
		push ebx
		xor ebx, ebx
		mov ebx, dword ptr[g_FPS_Offset1]
		cmp dword ptr[eax + ebx], 0
		mov ebx, dword ptr[g_FPS_Offset2]
		movzx eax, word ptr[eax + ebx]
		pop ebx
		je loc_fps_patch_v3
		shl eax, 1
		add eax, dword ptr[g_FPS_DataAddress]
		add eax, 1
		movzx eax, byte ptr[eax]
		xor ecx, ecx
		test dl, dl
		setne cl
		sub eax, ecx
		xor edx, edx
		mov edx, eax
		ret 4
		loc_fps_patch_v3:
		shl eax, 1
		add eax, dword ptr[g_FPS_DataAddress]
		movzx eax, byte ptr[eax]
		movzx ecx, dl
		neg ecx
		sbb ecx, ecx
		and ecx, 2
		sub eax, ecx
		dec eax
		xor edx, edx
		mov edx, eax
		ret 4
	}
}
//5.0.9.0 - 6.0.6.1
//----------------------------------------------------------------------------------
__declspec(naked) void FixFPS_V3()
{
	__asm
	{
		cmp byte ptr[g_FPSPatch], 0
		je loc_fps_patch_v1
		mov dl, 1
		jmp loc_fps_patch_v2
		loc_fps_patch_v1 :
		xor dl, dl
		loc_fps_patch_v2 :
		push ebx
		xor ebx, ebx
		mov ebx, dword ptr[g_FPS_Offset1]
		cmp dword ptr[esp + 8 + ebx], 0
		mov ebx, dword ptr[g_FPS_Offset2]
		movzx eax, word ptr[esi + ebx]
		pop ebx
		je loc_fps_patch_v3
		shl eax, 1
		add eax, dword ptr[g_FPS_DataAddress]
		add eax, 1
		movzx eax, byte ptr[eax]
		xor ecx, ecx
		test dl, dl
		setne cl
		sub eax, ecx
		ret 4
		loc_fps_patch_v3:
		shl eax, 1
		add eax, dword ptr[g_FPS_DataAddress]
		movzx eax, byte ptr[eax]
		movzx ecx, dl
		neg ecx
		sbb ecx, ecx
		and ecx, 2
		sub eax, ecx
		dec eax
		ret 4
	}
}
//6.0.6.2 - ...
//----------------------------------------------------------------------------------
__declspec(naked) void FixFPS_V4()
{
	__asm
	{
		cmp byte ptr[g_FPSPatch], 0
		je loc_fps_patch_v1
		mov dl, 1
		jmp loc_fps_patch_v2
		loc_fps_patch_v1 :
		xor dl, dl
		loc_fps_patch_v2 :
		mov eax, ecx
		push ebx
		xor ebx, ebx
		mov ebx, dword ptr[g_FPS_Offset1]
		cmp dword ptr[eax + ebx], 0
		mov ebx, dword ptr[g_FPS_Offset2]
		movzx eax, word ptr[eax + ebx]
		pop ebx
		je loc_fps_patch_v3
		shl eax, 1
		add eax, dword ptr[g_FPS_DataAddress]
		add eax, 1
		movzx eax, byte ptr[eax]
		xor ecx, ecx
		test dl, dl
		setne cl
		sub eax, ecx
		ret 4
		loc_fps_patch_v3:
		shl eax, 1
		add eax, dword ptr[g_FPS_DataAddress]
		movzx eax, byte ptr[eax]
		movzx ecx, dl
		neg ecx
		sbb ecx, ecx
		and ecx, 2
		sub eax, ecx
		dec eax
		ret 4
	}
}
//----------------------------------------------------------------------------------
