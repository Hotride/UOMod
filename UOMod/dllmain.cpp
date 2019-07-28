/***********************************************************************************
**
** dllmain.h
**
** Copyright (C) January 2017 Hotride
**
************************************************************************************
*/
//----------------------------------------------------------------------------------
#include "stdafx.h"
#include "UOMod.h"
//----------------------------------------------------------------------------------
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	static bool installed = false;
	static bool uninstalled = false;

	if (ul_reason_for_call == DLL_PROCESS_ATTACH && !installed)
	{
		installed = true;
		g_Patcher.Inject();
	}
	else if (ul_reason_for_call == DLL_PROCESS_DETACH && installed && !uninstalled)
	{
		uninstalled = true;
		g_Patcher.Uninstall();
	}

	return TRUE;
}
//----------------------------------------------------------------------------------
extern "C" __declspec(dllexport) DWORD GetFilePatches(const char *path)
{
	std::vector<unsigned char> fileData;
	return g_Patcher.OnGetFilePatches(path, fileData);
}
//----------------------------------------------------------------------------------
extern "C" __declspec(dllexport) DWORD SetFilePatches(const char *path, DWORD patches, int viewRange)
{
	return g_Patcher.OnSetFilePatches(path, patches, viewRange);
}
//----------------------------------------------------------------------------------
