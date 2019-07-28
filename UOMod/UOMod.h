/***********************************************************************************
**
** UOMod.h
**
** Copyright (C) January 2017 Hotride
**
************************************************************************************
*/
//----------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------
#include "PatchData.h"
//----------------------------------------------------------------------------------
class CPatcher
{
private:
	bool m_Initalized{ false };
	HWND m_ClientHandle{ 0 };
	HWND m_Handle{ 0 };
	bool m_Injected{ false };

	std::vector<CPatchData*> m_Patches;
	std::vector<CPatchData*> m_FilePatches;

	void SearchClientHandle();

	void FindPatches(PBYTE data, int size);

	void InfoPatch(const HWND &sender, const unsigned int &patchType);

	void SetPatch(const bool &state, const HWND &sender, const unsigned int &patchType);

	void ViewRange(const HWND &sender, const unsigned int &value);



	void FindFPSPatch(unsigned char *file, const int &size, CPatchDataFPS *patch);

	void FindStaminaPatch(unsigned char *file, const int &size, CPatchData *patch);

	void FindAlwaysLightPatch(unsigned char *file, const int &size, CPatchDataLight *patch);

	void FindPaperdollSlotsPatch(unsigned char *file, const int &size, CPatchDataPaperdollSlots *patch);

	void FindSplashScreenPatch(unsigned char *file, const int &size, CPatchData *patch);

	void FindResolutionPatch(unsigned char *file, const int &size, CPatchData *patch);

	void FindOptionsNotificationPatch(unsigned char *file, const int &size, CPatchData *patch);

	void FindMultiUOPatch(unsigned char *file, const int &size, CPatchDataMultiUO *patch);

	void FindNoCryptPatch(unsigned char *file, const int &size, CPatchDataNoCrypt *patch);

	void FindGlobalSoundPatch(unsigned char *file, const int &size, CPatchData *patch);

	void FindViewRangePatch(unsigned char *file, const int &size, CPatchDataViewRange *patch);

	void FindGMStepPatch(unsigned char *file, const int &size, CPatchData *patch);

public:
	CPatcher();
	~CPatcher();

	void Inject();

	void Install();
	void Uninstall();

	LRESULT OnWindowProc(HWND &hWnd, UINT &message, WPARAM &wParam, LPARAM &lParam);

	DWORD OnGetFilePatches(const char *path, std::vector<unsigned char> &fileData);

	DWORD OnSetFilePatches(const char *path, const DWORD &patches, int viewRange);
};
//----------------------------------------------------------------------------------
extern CPatcher g_Patcher;
//----------------------------------------------------------------------------------
