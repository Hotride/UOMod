/***********************************************************************************
**
** UOMod.cpp
**
** Copyright (C) January 2017 Hotride
**
************************************************************************************
*/
//----------------------------------------------------------------------------------
#include "stdafx.h"
#include "UOMod.h"
#include "Signature.h"
#include "ProcessManager.h"
#include "FPSPatchASM.h"
//----------------------------------------------------------------------------------
CPatcher g_Patcher;
WNDPROC g_ClientWindowProc = NULL;
//----------------------------------------------------------------------------------
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return g_Patcher.OnWindowProc(hWnd, message, wParam, lParam);
}
//----------------------------------------------------------------------------------
LRESULT CALLBACK HookWindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	if (msg == PM_INSTALL)
		g_Patcher.Install();

	return CallWindowProc(g_ClientWindowProc, hwnd, msg, wparam, lparam);
}
//----------------------------------------------------------------------------------
CPatcher::CPatcher()
{
	//Patches
	m_Patches.push_back(NULL);								//null index
	m_Patches.push_back(new CPatchDataFPS());				//PT_FPS
	m_Patches.push_back(new CPatchData());					//PT_STAMINA
	m_Patches.push_back(new CPatchDataLight());				//PT_ALWAYS_LIGHT
	m_Patches.push_back(new CPatchDataPaperdollSlots());	//PT_PAPERDOLL_SLOTS
	m_Patches.push_back(new CPatchData());					//PT_SPLASH_SCREEN
	m_Patches.push_back(new CPatchData());					//PT_RESOLUTION
	m_Patches.push_back(new CPatchData());					//PT_OPTIONS_NOTIFICATION
	m_Patches.push_back(new CPatchDataMultiUO());			//PT_MULTI_UO
	m_Patches.push_back(new CPatchDataNoCrypt());			//PT_NO_CRYPT
	m_Patches.push_back(new CPatchData());					//PT_GLOBAL_SOUND
	m_Patches.push_back(new CPatchDataViewRange());			//PT_VIEW_RANGE
	m_Patches.push_back(new CPatchData());					//PT_GM_STEP

	//File patches
	m_FilePatches.push_back(NULL);							//null index
	m_FilePatches.push_back(NULL);							//PT_FPS
	m_FilePatches.push_back(new CPatchData());				//PT_STAMINA
	m_FilePatches.push_back(new CPatchDataLight());			//PT_ALWAYS_LIGHT
	m_FilePatches.push_back(new CPatchDataPaperdollSlots());//PT_PAPERDOLL_SLOTS
	m_FilePatches.push_back(new CPatchData());				//PT_SPLASH_SCREEN
	m_FilePatches.push_back(new CPatchData());				//PT_RESOLUTION
	m_FilePatches.push_back(new CPatchData());				//PT_OPTIONS_NOTIFICATION
	m_FilePatches.push_back(new CPatchDataMultiUO());		//PT_MULTI_UO
	m_FilePatches.push_back(new CPatchDataNoCrypt());		//PT_NO_CRYPT
	m_FilePatches.push_back(new CPatchData());				//PT_GLOBAL_SOUND
	m_FilePatches.push_back(new CPatchDataViewRange());		//PT_VIEW_RANGE
	m_FilePatches.push_back(new CPatchData());				//PT_GM_STEP
}
//----------------------------------------------------------------------------------
CPatcher::~CPatcher()
{
}
//----------------------------------------------------------------------------------
void CPatcher::SearchClientHandle()
{
	if (m_ClientHandle == 0)
	{
		wchar_t *uoClassName[2] = { L"Ultima Online", L"Ultima Online Third Dawn" };

		for (int i = 0; i < 2; i++)
		{
			m_ClientHandle = FindWindow(uoClassName[i], NULL);

			do
			{
				DWORD processID = 0;
				GetWindowThreadProcessId(m_ClientHandle, &processID);

				if (processID == GetCurrentProcessId())
					break;

				m_ClientHandle = FindWindowEx(NULL, m_ClientHandle, uoClassName[i], NULL);

			} while (m_ClientHandle != 0);

			if (m_ClientHandle != 0)
				break;
		}
	}
}
//----------------------------------------------------------------------------------
void CPatcher::Inject()
{
	if (g_ClientWindowProc == NULL)
	{
		SearchClientHandle();

		if (m_ClientHandle != 0)
		{
			g_ClientWindowProc = (WNDPROC)SetWindowLongPtr(m_ClientHandle, GWL_WNDPROC, (LONG)HookWindowProc);
			PostMessage(m_ClientHandle, PM_INSTALL, 0, 0);
		}
	}
}
//----------------------------------------------------------------------------------
LRESULT CPatcher::OnWindowProc(HWND &hWnd, UINT &message, WPARAM &wParam, LPARAM &lParam)
{
	if (message < WM_USER)
		return DefWindowProc(hWnd, message, wParam, lParam);

	switch (message)
	{
		case PM_INFO:
		{
			if (lParam == 0xFFFFFFFF)
			{
				for (int i = 1; i < PT_COUNT; i++)
					InfoPatch((HWND)wParam, i);
			}
			else
				InfoPatch((HWND)wParam, lParam);

			break;
		}
		case PM_ENABLE:
		case PM_DISABLE:
		{
			SetPatch(message == PM_ENABLE, (HWND)wParam, lParam);
			break;
		}
		case PM_VIEW_RANGE_VALUE:
		{
			ViewRange((HWND)wParam, lParam);
			break;
		}
		default:
			break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}
//----------------------------------------------------------------------------------
void CPatcher::InfoPatch(const HWND &sender, const unsigned int &patchType)
{
	int state = PS_NOT_FOUND;

	if (patchType < PT_COUNT)
	{
		CPatchData *patch = m_Patches[patchType];

		if (patch != NULL && patch->Found)
			state = patch->IsPatched();
	}

	SendMessage(sender, PM_INFO, (patchType << 16) | state, (DWORD)m_ClientHandle);
}
//----------------------------------------------------------------------------------
void CPatcher::SetPatch(const bool &state, const HWND &sender, const unsigned int &patchType)
{
	switch (patchType)
	{
		case PT_FPS:
		case PT_STAMINA:
		case PT_ALWAYS_LIGHT:
		case PT_PAPERDOLL_SLOTS:
		case PT_OPTIONS_NOTIFICATION:
		case PT_VIEW_RANGE:
		case PT_GM_STEP:
		{
			CPatchData *patch = m_Patches[patchType];

			if (patch != NULL && !patch->Patch(state))
				MessageBox(NULL, L"Your client version is not supported for this patch.\nContact with developers for this question.", L"Error", MB_OK);

			break;
		}
		default:
			break;
	}

	InfoPatch(sender, patchType);
}
//----------------------------------------------------------------------------------
void CPatcher::ViewRange(const HWND &sender, const unsigned int &value)
{
	CPatchDataViewRange *patch = (CPatchDataViewRange*)m_Patches[PT_VIEW_RANGE];

	if (patch != NULL && patch->Found)
	{
		if (!value)
			SendMessage(sender, PM_VIEW_RANGE_VALUE, patch->GetCurrentClientViewRange(), (DWORD)m_ClientHandle);
		else
			patch->SetPatchViewRange(value);
	}
}
//----------------------------------------------------------------------------------
void CPatcher::FindPatches(PBYTE data, int size)
{
	FindFPSPatch(data, size, (CPatchDataFPS*)m_Patches[PT_FPS]);
	FindStaminaPatch(data, size, m_Patches[PT_STAMINA]);
	FindAlwaysLightPatch(data, size, (CPatchDataLight*)m_Patches[PT_ALWAYS_LIGHT]);
	FindPaperdollSlotsPatch(data, size, (CPatchDataPaperdollSlots*)m_Patches[PT_PAPERDOLL_SLOTS]);
	FindOptionsNotificationPatch(data, size, m_Patches[PT_OPTIONS_NOTIFICATION]);
	FindViewRangePatch(data, size, (CPatchDataViewRange*)m_Patches[PT_VIEW_RANGE]);
	FindGMStepPatch(data, size, (CPatchDataViewRange*)m_Patches[PT_GM_STEP]);
}
//----------------------------------------------------------------------------------
void CPatcher::Install()
{
	if (m_Initalized)
		return;

	SearchClientHandle();

	if (m_ClientHandle == 0)
	{
		MessageBox(NULL, L"Client window not found", L"Error", MB_OK);
		return;
	}

	if (g_ProcessManager.Open(m_ClientHandle))
	{
		char clientExePath[MAX_PATH] = { 0 };
		GetModuleFileNameA(NULL, clientExePath, MAX_PATH);

		FILE *file = NULL;
		fopen_s(&file, clientExePath, "rb");

		if (file != NULL)
		{
			fseek(file, 0, SEEK_END);
			int fileSize = ftell(file);
			fseek(file, 0, SEEK_SET);

			std::vector<BYTE> fileData(fileSize, 0);

			fread(&fileData[0], 1, fileSize, file);

			fclose(file);

			if (fileSize > 100)
			{
				FindPatches(&fileData[0], fileSize);

				char *bf = strrchr(clientExePath, '\\');

				if (bf)
					*bf = 0;

				std::string animinfo = std::string(clientExePath) + "\\Animinfo.mul";

				fopen_s(&file, animinfo.c_str(), "rb");

				if (file != NULL)
				{
					fseek(file, 0, SEEK_END);
					CPatchDataFPS *fpsPatch = (CPatchDataFPS*)m_Patches[PT_FPS];
					fpsPatch->DataSize = ftell(file);

					fclose(file);

					if (fpsPatch->Found && g_FPS_FunctionPointer != 0)
					{
						DWORD VPTmp = 0;
						VirtualProtect((PVOID)fpsPatch->AddressOfCutting, 5, PAGE_EXECUTE_READWRITE, &VPTmp);
						DWORD MemoryData = fpsPatch->AddressOfCutting;
						*(PBYTE)MemoryData = 0xE9;
						MemoryData = fpsPatch->AddressOfCutting + 1;
						*(PDWORD)MemoryData = (DWORD)FixFPS - fpsPatch->AddressOfCutting - 5;
					}
					else
						fpsPatch->Found = false;

					WNDCLASSEX wcex = { 0 };
					wcex.cbSize = sizeof(WNDCLASSEX);
					wcex.style = CS_HREDRAW | CS_VREDRAW;
					wcex.lpfnWndProc = WindowProc;
					wcex.cbClsExtra = 0;
					wcex.cbWndExtra = 0;
					wcex.hInstance = NULL; // hinstance;
					wcex.hCursor = NULL;
					wcex.hbrBackground = NULL;
					wcex.lpszMenuName = NULL;
					wchar_t className[50] = { 0 };
					wsprintf(className, L"UOModWindow_%08X", m_ClientHandle);
					wcex.lpszClassName = className;
					wcex.hIconSm = NULL;

					RegisterClassEx(&wcex);

					m_Handle = CreateWindow(className, className, WS_OVERLAPPEDWINDOW, 0, 0, 100, 20, NULL, NULL, /*hinstance*/NULL, NULL);

					m_Initalized = (m_Handle != NULL);

					if (!m_Initalized)
						MessageBox(NULL, L"Failed to create window", L"Error", MB_OK);
				}
				else
					MessageBoxA(NULL, ("File not found:\n" + animinfo).c_str(), "Error", MB_OK);
			}
		}
		else
			MessageBox(NULL, L"Client.exe not found", L"Error", MB_OK);
	}
	else
		MessageBox(NULL, L"Failed to open process.", L"Error", MB_OK);
}
//----------------------------------------------------------------------------------
void CPatcher::Uninstall()
{
	if (!m_Initalized || !m_Injected)
		return;

	SetWindowLongPtr(m_ClientHandle, GWL_WNDPROC, (LONG)g_ClientWindowProc);
	m_ClientHandle = NULL;
	g_ClientWindowProc = NULL;
	DestroyWindow(m_Handle);
	m_Handle = 0;
	m_Initalized = false;
	m_Injected = false;
}
//----------------------------------------------------------------------------------
void CPatcher::FindFPSPatch(unsigned char *file, const int &size, CPatchDataFPS *patch)
{
	patch->Reset();

	SIGNATURE(signature1, "\xB8\xD3\x4D\x62\x10", 5, 0xFF);
	SIGNATURE(signature2, "\x61\x6E\x69\x6D\x69\x6E\x66\x6F\x2E\x6D\x75\x6C\x00", 13, 0xFF);

	CSignature m_SignatureArray[4] =
	{
		SIGNATURE_ITEM("\x83\xFF\x4C", 3, 0xFF), //cmp *, 4Ch
		SIGNATURE_ITEM("\x83\xFF\x14", 3, 0xFF), //cmp *, 14h
		SIGNATURE_ITEM("\x83\xFF\x3C", 3, 0xFF), //cmp *, 3Ch
		SIGNATURE_ITEM("\x83\xFF\x18", 3, 0xFF)  //cmp *, 18h
	};

	int count = size - 100;

	for (int i = 0; i < count; i++)
	{
		if (signature1.Find(file + i))
		{
			int startSearchPos = i - 200;

			if (startSearchPos < 0)
				startSearchPos = 0;

			int endSearchPos = i + 2500;

			if (endSearchPos >= count)
				endSearchPos = count;

			bool found = false;

			unsigned int timerAddress = 0;

			for (int z = 0; z < 4; z++)
			{
				found = false;

				CSignature &sig = m_SignatureArray[z];

				for (int j = startSearchPos; j < endSearchPos; j++)
				{
					if (sig.Find(file + j))
					{
						found = true;

						if (!z)
							timerAddress = j + 0x00400002;

						break;
					}
				}

				if (!found)
					break;
			}

			if (found)
			{
				found = false;

				//check string "animinfo.mul\0"
				for (int j = 0; j < count; j++)
				{
					if (signature2.Find(file + j))
					{
						found = false;

						unsigned int addr = j + 0x00400000;

						unsigned char databuf[5] = { 0 };

						databuf[0] = 0x68;
						databuf[1] = addr & 0xFF;
						databuf[2] = (addr >> 8) & 0xFF;
						databuf[3] = (addr >> 16) & 0xFF;
						databuf[4] = (addr >> 24) & 0xFF;

						SIGNATURE(sig, databuf, 5, 0xFF);

						//check push offset aAniminfo_mul
						for (j = 0; j < count; j++)
						{
							if (sig.Find(file + j))
							{
								int maxJ = j + 120;

								if (maxJ >= count)
									maxJ = count;

								for (int k = j; k < maxJ; k++)
								{
									if (file[k] == 0xBE && file[k + 4] == 0x00)
									{
										patch->TimerAddress = timerAddress;
										patch->Address = ((file[k + 4] << 24) | (file[k + 3] << 16) | (file[k + 2] << 8) | file[k + 1]) - 1;

										found = true;

										break;
									}
								}

								if (found)
									break;
							}
						}

						if (found)
							break;
					}
				}

				if (found)
					break;
			}
		}
	}

	if (patch->TimerAddress && patch->Address)
	{
		//1.26.0 - 2.0.4a;     3.0.8z - 5.0.8.4
		SIGNATURE(checkSig1, "\x33\xC9\xFF\xE9\xA2\x8B\x2E\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xF7\xE9\xD1\xFA\xFF"
			"\xFF\xC1\xFF\x1F\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\x85\xC0\x74\xFF\x33\xC9\xFF\xFF\xFF\xFF\xFF\xFF\xFF"
			"\xFF\xFF\xFF\xFF\xFF\xEB\xFF\x33\xD2\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\x48", 82, 0xFF);

		//2.0.4c - 3.0.6a
		SIGNATURE(checkSig2, "\xE9\xA2\x8B\x2E\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xF7\xE9\xD1\xFA\xFF\xFF\xC1\xFF\x1F\xFF\xFF\xFF\xFF\xFF\xEB", 27, 0xFF);
		SIGNATURE(checkSig3, "\x85\xFF\x74\xFF\x33\xC9\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xEB\xFF\x33\xD2\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\x4A", 35, 0xFF);

		//3.0.6e - 3.0.8q
		SIGNATURE(checkSig4, "\xE9\xA2\x8B\x2E\xF7\xE9\xD1\xFA\xFF\xFF\xC1\xFF\x1F\xFF\xFF\xFF\xFF\xFF\xEB\xFF\xFF\xFF\xFF\xFF\xFF\xFF\x85\xFF\x74\xFF"
			"\x33\xD2\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xEB\xFF\x33\xC0\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\x48", 61, 0xFF);

		//5.0.9.0 - 6.0.6.1
		SIGNATURE(checkSig5, "\xE9\xA2\x8B\x2E\xF7\xE9\xD1\xFA\xFF\xFF\xC1\xFF\x1F\xFF\xFF\xFF\xFF\xFF\xEB\xFF\xFF\xFF\xFF\xFF\x85\xFF\x74\xFF\x33\xD2"
			"\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xEB\xFF\x33\xC0\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\x48", 59, 0xFF);

		//6.0.6.2 - ...
		SIGNATURE(checkSig6, "\x56\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xE9\xA2\x8B\x2E\xF7\xE9\xD1\xFA\xFF\xFF\xC1\xFF\x1F\xFF\xFF\xFF\xFF\xFF\x5E\xC3"
			"\xFF\xFF\xFF\xFF\xFF\xFF\xFF\x74\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xC3\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF"
			"\xFF\xFF\xFF\xFF\xC3", 65, 0xFF);

		for (int i = 0; i < count; i++)
		{
			unsigned char *ptr = file + i;

			//1.26.0 - 2.0.4a;     3.0.8z - 5.0.8.4
			if (checkSig1.Find(ptr))
			{
				patch->Found = true;
				g_FPS_FunctionPointer = (DWORD)FixFPS_V1;
				patch->ReturnAddress = i + 0x00400051;
				patch->AddressOfCutting = i + 0x00400029;
				patch->Offset1 = ((file[i + 44] << 8) | file[i + 43]);
				patch->Offset2 = file[i + 56];

				break;
			}
			//2.0.4c - 3.0.6a
			else if (checkSig2.Find(ptr))
			{
				bool found = false;

				for (int j = i; j < i + 150; j++)
				{
					if (checkSig3.Find(file + j))
					{
						patch->Found = true;
						found = true;
						g_FPS_FunctionPointer = (DWORD)FixFPS_V2;
						patch->ReturnAddress = j + 0x00400022;
						patch->AddressOfCutting = j + 0x00400000 - 6;
						patch->Offset1 = ((file[j - 3] << 8) | file[j - 4]);
						patch->Offset2 = file[j + 9];

						break;
					}
				}

				if (found)
					break;
			}
			//3.0.6e - 3.0.8q
			else if (checkSig4.Find(ptr))
			{
				patch->Found = true;
				g_FPS_FunctionPointer = (DWORD)FixFPS_V1;
				patch->ReturnAddress = i + 0x0040003C;
				patch->AddressOfCutting = i + 0x00400014;
				patch->Offset1 = ((file[i + 23] << 8) | file[i + 22]);
				patch->Offset2 = file[i + 35];

				break;
			}
			//5.0.9.0 - 6.0.6.1
			else if (checkSig5.Find(ptr))
			{
				patch->Found = true;
				g_FPS_FunctionPointer = (DWORD)FixFPS_V3;
				patch->ReturnAddress = i + 0x0040003A;
				patch->AddressOfCutting = i + 0x00400014;
				patch->Offset1 = file[i + 23];
				patch->Offset2 = file[i + 33];

				break;
			}
			//6.0.6.2 - ...
			else if (checkSig6.Find(ptr))
			{
				patch->Found = true;
				g_FPS_FunctionPointer = (DWORD)FixFPS_V4;
				patch->ReturnAddress = i + 0x00400040;
				patch->AddressOfCutting = i + 0x0040001E;
				patch->Offset1 = ((file[i + 33] << 8) | file[i + 32]);
				patch->Offset2 = file[i + 42];

				break;
			}
		}
	}

	if (patch->Found)
	{
		g_FPS_ReturnAddress = patch->ReturnAddress;
		g_FPS_DataAddress = patch->Address;
		g_FPS_Offset1 = patch->Offset1;
		g_FPS_Offset2 = patch->Offset2;
	}
}
//----------------------------------------------------------------------------------
void CPatcher::FindStaminaPatch(unsigned char *file, const int &size, CPatchData *patch)
{
	patch->Reset();

	SIGNATURE(signature, "\x0F\xBF\xFF\x26\x39\xFF\x24", 7, 0xFF);

	int count = size - 100;

	for (int i = 0; i < count; i++)
	{
		unsigned char *ptr = file + i;

		if (signature.Find(ptr))
		{
			int maxJ = i + 100;

			for (int j = i; j < maxJ; j++)
			{
				ptr = file + j;

				if (ptr[0] == 0x8B && ptr[6] == 0x8B && ptr[12] == 0x3B && ptr[14] == 0x74)
				{
					patch->Found = true;
					patch->Address = j + 13 + 0x00400000;
					patch->OriginalSignature.push_back(ptr[13]);
					patch->NewSignature.push_back(0xC0);

					break;
				}
				else if (ptr[0] == 0x8B && ptr[6] == 0x3B && ptr[12] == 0x74)
				{
					patch->Found = true;
					patch->Address = j + 7 + 0x00400000;

					for (int v = 0; v < 5; v++)
						patch->OriginalSignature.push_back(ptr[7 + v]);

					patch->NewSignature.push_back(0xC0);
					patch->NewSignature.push_back(0x90);
					patch->NewSignature.push_back(0x90);
					patch->NewSignature.push_back(0x90);
					patch->NewSignature.push_back(0x90);

					break;
				}
			}

			if (patch->Found)
				break;
		}
	}
}
//----------------------------------------------------------------------------------
void CPatcher::FindAlwaysLightPatch(unsigned char *file, const int &size, CPatchDataLight *patch)
{
	patch->Reset();

	SIGNATURE(signature1, "\x25\xFF\x00\x00\x00", 5, 0xFF);
	SIGNATURE(signature2, "\x83\xC4\x0C\x39", 4, 0xFF);

	int count = size - 100;

	for (int i = 0; i < count; i++)
	{
		unsigned char *ptr = file + i;

		if (ptr[0] == 0x8B && ptr[2] == 0x24)
		{
			int maxJ = i + 100;

			for (int j = i; j < maxJ; j++)
			{
				ptr = file + j;

				if (signature1.Find(ptr))
				{
					for (int k = j; k < j + 10; k++)
					{
						ptr = file + k;

						if (ptr[0] == 0x3B && ptr[2] == 0x74 && ((ptr[4] == 0x80 && ptr[6] == 0x1F) || (ptr[4] == 0x8B && ptr[10] == 0xA3 && ptr[15] == 0x85)))
						{
							patch->Found = true;
							patch->Address = k + 1 + 0x00400000;
							patch->OriginalSignature.push_back(ptr[1]);
							patch->NewSignature.push_back(0xC0);

							for (int k = j; k > j - 20; k--)
							{
								ptr = file + k;

								if (ptr[0] == 0x8B && ptr[5] == 0x00)
								{
									patch->ValueAddress = (ptr[5] << 24) | (ptr[4] << 16) | (ptr[3] << 8) | ptr[2];

									break;
								}
							}

							break;
						}
					}

					if (patch->Found)
						break;
				}
				else if (ptr[9] == 0x74 && ptr[11] == 0x80 && ptr[13] == 0x1F && signature2.Find(ptr))
				{
					patch->Found = true;
					patch->Address = j + 3 + 0x00400000;
					patch->ValueAddress = (ptr[8] << 24) | (ptr[7] << 16) | (ptr[6] << 8) | ptr[5];

					for (int v = 0; v < 6; v++)
						patch->OriginalSignature.push_back(file[j + 3 + v]);

					patch->NewSignature.push_back(0x3B);
					patch->NewSignature.push_back(0xC0);
					patch->NewSignature.push_back(0x90);
					patch->NewSignature.push_back(0x90);
					patch->NewSignature.push_back(0x90);
					patch->NewSignature.push_back(0x90);

					break;
				}
			}

			if (patch->Found)
				break;
		}
	}
}
//----------------------------------------------------------------------------------
void CPatcher::FindPaperdollSlotsPatch(unsigned char *file, const int &size, CPatchDataPaperdollSlots *patch)
{
	patch->Reset();

	SIGNATURE(signature1, "\x6A\x60\x6A\x02", 4, 0xFF);
	SIGNATURE(signature2, "\x6A\x75\x6A\x02", 4, 0xFF);

	int count = size - 100;

	for (int i = 0; i < count; i++)
	{
		unsigned char *ptr = file + i;

		if (signature1.Find(ptr))
		{
			bool accepted = false;

			for (int j = i; j < i + 30; j++)
			{
				ptr = file + j;

				if (signature2.Find(ptr))
				{
					accepted = true;

					break;
				}
			}

			if (accepted)
			{
				for (int j = i; j > i - 100; j--)
				{
					ptr = file + j;

					if (ptr[0] == 0x8B && ptr[2] == 0x4C)
					{
						int maxJ = j + 30;

						for (; j < maxJ; j++)
						{
							ptr = file + j;

							if (ptr[0] == 0x3B && (ptr[2] == 0x75 || ptr[6] == 0x75 || ptr[6] == 0x0F))
							{
								patch->Found = true;
								int count = 2;
								patch->Address = j + 2 + 0x00400000;
								unsigned char *originalData = &ptr[2];

								if (ptr[6] == 0x75 || ptr[6] == 0x0F)
								{
									if (ptr[6] == 0x0F)
										count = 6;

									originalData = &ptr[6];
									patch->Address += 4;
								}

								for (int v = 0; v < count; v++)
								{
									patch->OriginalSignature.push_back(originalData[v]);
									patch->NewSignature.push_back(0x90);
								}
							}
						}

						break;
					}
				}

				if (patch->Found)
					break;
			}
		}
	}

	if (!patch->Found)
		return;

	patch->Found = false;

	for (int i = 0; i < count; i++)
	{
		unsigned char *ptr = file + i;
		int offset = 0;

		if (ptr[0] == 0x8B && ptr[6] == 0x8B)
		{
			if (ptr[12] == 0x3B && ptr[19] == 0x0F && ptr[25] == 0x8B && ptr[29] == 0x83 && ptr[31] == 0x03)
				offset = 19;
			else if (ptr[12] == 0x8B && ptr[18] == 0x3B && ptr[20] == 0x0F && ptr[26] == 0x8D && ptr[30] == 0x8D && ptr[36] == 0x68 && ptr[37] == 0x44 && ptr[38] == 0x23)
				offset = 20;
		}
		else if (ptr[0] == 0x8B && ptr[3] == 0x8B && ptr[9] == 0x3B && ptr[16] == 0x0F && ptr[22] == 0x8B && ptr[26] == 0x8B && ptr[30] == 0x6A && ptr[31] == 0x4B)
			offset = 16;

		if (offset)
		{
			patch->Found = true;
			patch->Address2 = i + offset + 0x00400000;

			for (int j = 0; j < 6; j++)
			{
				patch->OriginalSignature2.push_back(ptr[i + offset + j]);
				patch->NewSignature2.push_back(0x90);
			}

			break;
		}
	}

	if (!patch->Found)
		patch->Reset();
}
//----------------------------------------------------------------------------------
void CPatcher::FindSplashScreenPatch(unsigned char *file, const int &size, CPatchData *patch)
{
	patch->Reset();

	SIGNATURE(signature1, "\x68\x88\x13\x00\x00", 5, 0xFF);
	SIGNATURE(signature2, "\x8B\x3D", 2, 0xFF);

	int count = size - 100;

	for (int i = 0; i < count; i++)
	{
		unsigned char *ptr = file + i;

		if (signature1.Find(ptr))
		{
			bool accepted = false;

			for (int j = i; j < i + 20; j++)
			{
				ptr = file + j;

				if (signature2.Find(ptr))
				{
					patch->Found = true;
					patch->Address = i + 1 + 0x00400000;

					patch->OriginalSignature.push_back(file[i + 1]);
					patch->OriginalSignature.push_back(file[i + 2]);

					patch->NewSignature.push_back(0x01);
					patch->NewSignature.push_back(0x00);

					break;
				}
			}

			if (patch->Found)
				break;
		}
	}
}
//----------------------------------------------------------------------------------
void CPatcher::FindResolutionPatch(unsigned char *file, const int &size, CPatchData *patch)
{
	patch->Reset();

	SIGNATURE(signature1, "\x00\x99\x2B\xC2\xD1\xF8\xC3", 7, 0xFF);
	SIGNATURE(signature2, "\x80\x02\x00\x00\xE0\x01\x00\x00", 8, 0xFF);

	int count = size - 100;
	unsigned int addr = 0;

	BYTE signatureNew[19] =
	{
		0x8B, 0x44, 0x24, 0x04, 0xA3, 0x3C, 0xCC, 0x50, 0x00, 0x8B,
		0x44, 0x24, 0x08, 0xA3, 0x40, 0xCC, 0x50, 0x00, 0xC3
	};

	for (int i = 0; i < count; i++)
	{
		unsigned char *ptr = file + i;

		if (!patch->Address && signature1.Find(ptr))
		{
			addr = i + 12;

			if (signature1.Find(ptr + 16))
				addr += 16;

			for (int j = 0; j < 19; j++)
				patch->OriginalSignature.push_back(file[addr + j]);
		}

		if (signature2.Find(ptr) && addr)
		{
			patch->Found = true;
			patch->Address = addr + 0x00400000;

			addr = i + 0x00400000;

			signatureNew[8] = (addr >> 24) & 0xFF;
			signatureNew[7] = (addr >> 16) & 0xFF;
			signatureNew[6] = (addr >> 8) & 0xFF;
			signatureNew[5] = addr & 0xFF;

			addr += 4;

			signatureNew[17] = (addr >> 24) & 0xFF;
			signatureNew[16] = (addr >> 16) & 0xFF;
			signatureNew[15] = (addr >> 8) & 0xFF;
			signatureNew[14] = addr & 0xFF;

			for (int j = 0; j < 19; j++)
				patch->NewSignature.push_back(signatureNew[j]);

			break;
		}
	}
}
//----------------------------------------------------------------------------------
void CPatcher::FindOptionsNotificationPatch(unsigned char *file, const int &size, CPatchData *patch)
{
	patch->Reset();

	SIGNATURE(signature1, "\x8D\x4E\x2C\xE8", 4, 0xFF);
	SIGNATURE(signature2, "\x50\x8D\x4E\x18\xE8", 5, 0xFF);
	SIGNATURE(signature3, "\x83\xC4\x08\x5E\xC3", 5, 0xFF);

	int count = size - 100;

	for (int i = 0; i < count; i++)
	{
		unsigned char *ptr = file + i;

		if (signature1.Find(ptr) && signature2.Find(ptr + 8) && signature3.Find(ptr + 23))
		{
			patch->Found = true;
			patch->Address = i + 18 + 0x00400000;

			for (int j = 0; j < 5; j++)
			{
				patch->OriginalSignature.push_back(file[i + 18 + j]);
				patch->NewSignature.push_back(0x90);
			}

			break;
		}
	}
}
//----------------------------------------------------------------------------------
void CPatcher::FindMultiUOPatch(unsigned char *file, const int &size, CPatchDataMultiUO *patch)
{
	SIGNATURE(signature, "\xB7\x00\x00\x00\x75", 5, 0xFF);

	patch->Reset();

	int count = size - 100;

	for (int i = 0; i < count; i++)
	{
		unsigned char *ptr = file + i;

		if (ptr[0] == 0x6A)
		{
			if (ptr[1] == 0x40 && ptr[3] == 0x68 && ptr[9] == 0xFF)
			{
				for (int j = i; j > i - 12; j--)
				{
					unsigned char *temp = file + j;

					if ((temp[0] == 0x3B || temp[0] == 0x85) && temp[2] == 0x74)
					{
						patch->Found = true;
						patch->Address = j + 0x00400000;

						patch->OriginalSignature.push_back(temp[0]);
						patch->OriginalSignature.push_back(temp[1]);

						patch->NewSignature.push_back(0x3B);
						patch->NewSignature.push_back(0xC0);

						break;
					}
				}
			}
			else if (ptr[1] == 0x04 && ptr[2] == 0x68 && ptr[7] == 0x68)
			{
				for (int j = i; j > i - 12; j--)
				{
					unsigned char *temp = file + j;

					if (temp[0] == 0x85 && temp[2] == 0x8B && temp[8] == 0x74)
					{
						patch->Found = true;
						patch->Address = j + 0x00400000;

						patch->OriginalSignature.push_back(temp[0]);
						patch->OriginalSignature.push_back(temp[1]);

						patch->NewSignature.push_back(0x3B);
						patch->NewSignature.push_back(0xC0);

						break;
					}
				}
			}

			if (patch->Found)
				break;
		}
	}
	
	if (patch->Found)
	{
		patch->Found = false;

		for (int i = 0; i < count; i++)
		{
			unsigned char *ptr = file + i;

			if (ptr[0] == 0xFF && ptr[13] != 0xFF && ptr[13] != 0x83 && signature.Find(ptr + 7))
			{
				patch->Found = true;
				patch->Address2 = i + 0x00400000;

				for (i = 0; i < 6; i++)
					patch->OriginalSignature2.push_back(ptr[i]);

				patch->NewSignature2.push_back(0x33);
				patch->NewSignature2.push_back(0xC0);
				patch->NewSignature2.push_back(0x90);
				patch->NewSignature2.push_back(0x90);
				patch->NewSignature2.push_back(0x90);
				patch->NewSignature2.push_back(0x90);

				break;
			}
		}

		if (!patch->Found)
			patch->Reset();
	}
}
//----------------------------------------------------------------------------------
void CPatcher::FindNoCryptPatch(unsigned char *file, const int &size, CPatchDataNoCrypt *patch)
{
	SIGNATURE(signatureDunno, "dunno, select with invalid socket", 34, 0xFF);
	SIGNATURE(signature1, "\x81\xF9\x00\x00\x01\x00\x0F\x8F", 8, 0xFF);
	SIGNATURE(signature2, "\x00\x00\x00\x00\x75\x12\x8B\x54", 8, 0xFF);
	SIGNATURE(signature3, "\x33\xDB\x3B\xC3\x0F\x84", 6, 0xFF);
	SIGNATURE(signature4, "\x4A\x83\xCA\xF0\x42\x8A\x94\x32", 8, 0xFF);
	SIGNATURE(signature5, "\x00\x00\x74\x37\x83\xBE\xB4\x00\x00\x00\x00", 11, 0xFF);
	SIGNATURE(signature6, "\x8B\xD9\x8B\xC8\x48\x85\xC9\x0F\x84", 9, 0xFF);
	SIGNATURE(signature7, "\x74\x0F\x83\xB9\xB4\x00\x00\x00\x00", 9, 0xFF);

	patch->Reset();

	int cryptOffset = 0;
	int cryptOffsetNew = 0;
	int decryptOffset1 = 0;
	int decryptOffset2 = 0;
	bool newClient = false;
	int decryptTwoLoc = 0;
	int TFOldPos = 0;
	int TFNewPos = 0;

	for (int i = 0; i < size; i++)
	{
		if (signature1.Find(file + i))
		{
			for (int j = i; j < i + 100; j++)
			{
				if (file[j] == 0x0F && file[j + 1] == 0x84)
				{
					cryptOffset = j + 1;
					break;
				}
			}

			break;
		}
		else if (signature2.Find(file + i))
		{
			cryptOffsetNew = i + 4;
			newClient = true;
			break;
		}
	}

	for (int i = 0; i < size; i++)
	{
		if (signatureDunno.Find(file + i))
		{
			int dunnopos = i + 0x00400000;

			unsigned char dunnopush[5] = { 0x68 };
			memcpy(&dunnopush[1], &dunnopos, 4);
			dunnopos = 0;

			bool pushfound = false;

			for (int j = 0; j < size; j++)
			{
				if (!memcmp(&file[j], &dunnopush[0], 5))
				{
					if(!pushfound)
						pushfound = true;
					else if(pushfound)
					{
						dunnopos = j + 0x30;

						for (; j < dunnopos; j++)
						{
							if (signature3.Find(file + j))
							{
								decryptOffset1 = j + 2;

								break;
							}
							else if (file[j] == 0x85 && file[j + 1] == 0xC0)
							{
								decryptOffset1 = j;
								break;
							}
						}

						break;
					}
				}
			}
		}
		else if (signature4.Find(file + i))
		{
			for (int j = i - 0x100; j < i; j++)
			{
				if (file[j] == 0x85 && file[j + 2] == 0x74 && file[j + 4] == 0x33 && file[j + 6] == 0x85 && file[j + 8] == 0x7E)
				{
					decryptOffset2 = j;
					break;
				}
			}
		}
		else if (signature5.Find(file + i))
			decryptTwoLoc = i + 2;
		else if (signature6.Find(file + i))
			TFOldPos = i + 8;
		else if (signature7.Find(file + i))
			TFNewPos = i;
	}

	if (cryptOffset)
	{
		patch->Found = true;
		patch->Address = 0x00400000 + cryptOffset;
	}

	if (cryptOffsetNew)
	{
		patch->Found = true;
		patch->AddressNew = 0x00400000 + cryptOffsetNew;
	}

	if (decryptOffset1)
	{
		patch->Found = true;
		patch->DecryptAddress1 = 0x00400000 + decryptOffset1;
	}

	if (!newClient && decryptOffset2)
	{
		patch->Found = true;
		patch->DecryptAddress2 = 0x00400000 + decryptOffset2;
	}

	if (newClient && decryptTwoLoc)
	{
		patch->Found = true;
		patch->Decrypt2Address = 0x00400000 + decryptTwoLoc;
	}

	if (TFOldPos)
	{
		patch->Found = true;
		patch->TFOldAddress = 0x00400000 + TFOldPos;
	}

	if (TFNewPos)
	{
		patch->Found = true;
		patch->TFNewAddress = 0x00400000 + TFNewPos;
	}
}
//----------------------------------------------------------------------------------
void CPatcher::FindGlobalSoundPatch(unsigned char *file, const int &size, CPatchData *patch)
{
	patch->Reset();

	SIGNATURE(signature, "\xC7\x44\x24\xFF\xE0\x00\x01\x00\xC7", 9, 0xFF);

	int count = size - 100;

	for (int i = 0; i < count; i++)
	{
		unsigned char *ptr = file + i;

		if (signature.Find(ptr))
		{
			patch->Found = true;
			patch->Address = i + 5 + 0x00400000;

			patch->OriginalSignature.push_back(file[i + 5]);
			patch->NewSignature.push_back(0x80);

			break;
		}
	}
}
//----------------------------------------------------------------------------------
void CPatcher::FindViewRangePatch(unsigned char *file, const int &size, CPatchDataViewRange *patch)
{
	patch->Reset();

	SIGNATURE(signature1, "\x33\xFF\x66\x89\x7C\x24\xFF\x66\x89\x7C\x24\xFF\x66\x89\x7C\x24\xFF\xE8", 18, 0xFF);
	SIGNATURE(signature2, "\x85\xFF\x74\x07\x0F\xB6\xFF\xFF\x02\x00\x00", 11, 0xFF);
	SIGNATURE(signature3, "\xE8\xFF\xFF\xFF\xFF\x83\xFF\x04\x85\xFF\x74\xFF\xFF\x8B", 14, 0xFF);
	SIGNATURE(signature4, "\xE8\xFF\xFF\xFF\xFF\x83\xFF\x04\x85\xFF\x74\xFF\x8B\xFF\xFF\xFF\x8B\xFF\xFF\xFF\xFF\xFF\xFF\xE8", 24, 0xFF);
	SIGNATURE(signature5_1, "\x83\xFF\x06\x83\xFF\x12\xFF\xFF\x01\x00\x00\x00\xF6\xFF\x1B\xFF\x83\xFF\x06\x83\xFF\x12", 22, 0xFF);
	SIGNATURE(signature5_2, "\xB8\x12\x00\x00\x00\xFF\xFF\xC3\x83\xFF\x18\x7E\xFF\xB8\x18\x00\x00\x00", 18, 0xFF);
	SIGNATURE(signature6_1, "\x1B\xFF\x83\xFF\x06\x83\xFF\x12", 8, 0xFF);
	SIGNATURE(signature6_2, "\x3D\x58\x02\x00\x00\xFF\x12\x00\x00\x00\x7E\xFF\xFF\x18\x00\x00\x00", 17, 0xFF);
	SIGNATURE(signature11, "\x3B\xFF\x7E\xFF\x88\xFF\xFF\xFF\x00\x00\xFF\xC2\x04\x00\x88\xFF\xFF\xFF\x00\x00\xFF\xC2\x04\x00", 24, 0xFF);

	int count = size - 100;

	for (int i = 0; i < count; i++)
	{
		unsigned char *ptr = file + i;

		if (signature1.Find(ptr))
		{
			patch->Address = i + 17 + 0x00400000;

			for (int j = 0; j < 4; j++)
				patch->OriginalSignature.push_back(file[i + j + 17]);

			patch->NewSignature.push_back(0xB8);
			patch->NewSignature.push_back(0x12);
			patch->NewSignature.push_back(0x00);
			patch->NewSignature.push_back(0x00);
		}
		else if (signature2.Find(ptr))
		{
			patch->Address2 = i + 4 + 0x00400000;

			for (int j = 0; j < 7; j++)
				patch->OriginalSignature2.push_back(file[i + j + 4]);

			patch->NewSignature2.push_back(0xB8);
			patch->NewSignature2.push_back(0x12);
			patch->NewSignature2.push_back(0x00);
			patch->NewSignature2.push_back(0x00);
			patch->NewSignature2.push_back(0x00);
			patch->NewSignature2.push_back(0x90);
			patch->NewSignature2.push_back(0x90);
		}
		else if (signature3.Find(ptr))
		{
			patch->Address3 = i + 10 + 0x00400000;

			patch->OriginalSignature3.push_back(file[i + 10]);
			patch->OriginalSignature3.push_back(file[i + 11]);

			patch->NewSignature3.push_back(0xEB);
			patch->NewSignature3.push_back(0x00);
		}
		else if (signature4.Find(ptr))
		{
			patch->Address4 = i + 10 + 0x00400000;

			patch->OriginalSignature4.push_back(file[i + 10]);
			patch->OriginalSignature4.push_back(file[i + 11]);

			patch->NewSignature4.push_back(0xEB);
			patch->NewSignature4.push_back(0x00);
		}
		else if (signature5_1.Find(ptr))
		{
			patch->Address5 = i + 5 + 0x00400000;

			patch->OriginalSignature5.push_back(file[i + 5]);

			patch->NewSignature5.push_back(0x1F);
		}
		else if (signature5_2.Find(ptr))
		{
		}
		else if (signature6_1.Find(ptr))
		{
			if (ptr[8] == 0xA3)
			{
				patch->Address6 = i + 7 + 0x00400000;

				patch->OriginalSignature6.push_back(file[i + 7]);
				patch->NewSignature6.push_back(0x1F);
			}
			else if (ptr[8] == 0x83 && ptr[11] == 0x7D && ptr[13] == 0xB8 && ptr[18] == 0xC3)
			{
				patch->Address7 = i + 7 + 0x00400000;

				patch->OriginalSignature7.push_back(file[i + 7]);
				patch->NewSignature7.push_back(0x1F);
			}
			else if (ptr[8] == 0x83 && ptr[11] == 0x7D && ptr[13] == 0xB9)
			{
				patch->Address8 = i + 7 + 0x00400000;

				patch->OriginalSignature8.push_back(file[i + 7]);
				patch->NewSignature8.push_back(0x1F);

				for (int j = 0; j < 50; j++)
				{
					if (ptr[j - 7] == 0xC6 && ptr[j - 1] == 0x01 && ptr[j] == 0x89 && ptr[j + 6] == 0x89)
					{
						patch->Address9 = i + j + 1 + 0x00400000;

						patch->OriginalSignature9.push_back(file[i + j + 1]);
						patch->NewSignature9.push_back(0x05);

						patch->Address10 = i + j + 7 + 0x00400000;

						patch->OriginalSignature10.push_back(file[i + j + 7]);
						patch->NewSignature10.push_back(0x05);

						patch->AddressViewRangeValue1 = (ptr[j + 5] << 24) | (ptr[j + 4] << 16) | (ptr[j + 3] << 8) | ptr[j + 2];
						patch->AddressViewRangeValue2 = (ptr[j + 11] << 24) | (ptr[j + 10] << 16) | (ptr[j + 9] << 8) | ptr[j + 8];

						break;
					}
				}
			}
		}
		/*else if (signature6_2.Find(ptr))
		{
			if (ptr[17] == 0xC3)
				qDebug("found 6.2 at 0x%08X", i + 0x00400000);
			else if (ptr[17] == 0x8B)
				qDebug("found 7.2 at 0x%08X", i + 0x00400000);
			else if (ptr[17] == 0x83 && ptr[19] == 0x05 && ptr[20] == 0x7D && ptr[22] == 0xBF && ptr[27] == 0xEB && ptr[29] == 0x3B && ptr[31] == 0x7E && ptr[33] == 0x8B && ptr[35] == 0x89 && ptr[41] == 0x89)
			{
				qDebug("found 8.2 at 0x%08X", i + 0x00400000);

				for (int j = 0; j < 50; j++)
				{
					if (ptr[j] == 0x89 && ptr[j + 6] == 0x89)
					{
						qDebug("found 9 at 0x%08X", i + j + 0x00400000);
						break;
					}
				}
			}
		}*/
		else if (signature11.Find(ptr))
		{
			patch->Address11 = i + 15 + 0x00400000;

			patch->OriginalSignature11.push_back(file[i + 15]);

			patch->NewSignature11.push_back(file[i + 5]);
		}
	}

	patch->Found = (patch->Address && patch->Address2 && patch->Address3 && patch->Address4 && patch->Address5 && patch->Address6 && patch->Address7 && patch->Address8 && patch->Address9 && patch->Address10 && patch->Address11);
}
//----------------------------------------------------------------------------------
void CPatcher::FindGMStepPatch(unsigned char *file, const int &size, CPatchData *patch)
{
	/*
	F6 81 A4 00 00 00 20 74  06 B8 FF 00 00 00 C3 0F BE 41 3C 0F B7 49 38 03  C1 8D 14 80 A1 84 86 83 00 0F B6 44 D0 04 C3
	*/

	SIGNATURE(signature, "\xF6\xFF\xFF\xFF\xFF\x00\x20\x74\x06\xB8\xFF\x00\x00\x00\xC3", 15, 0xFF);

	int count = size - 100;

	for (int i = 0; i < count; i++)
	{
		unsigned char *ptr = file + i;

		if (signature.Find(ptr))
		{
			patch->Address = i + 0x00400000 + 10;

			patch->OriginalSignature.push_back(file[i + 10]);
			patch->NewSignature.push_back(0x00);

			patch->Found = true;

			break;
		}
	}
}
//----------------------------------------------------------------------------------
DWORD CPatcher::OnGetFilePatches(const char *path, std::vector<unsigned char> &fileData)
{
	DWORD result = 0;

	FILE *file = NULL;
	fopen_s(&file, path, "rb");

	if (file != NULL)
	{
		fseek(file, 0, SEEK_END);
		int fileSize = ftell(file);
		fseek(file, 0, SEEK_SET);

		fileData.resize(fileSize, 0);

		fread(&fileData[0], 1, fileSize, file);

		fclose(file);

		FindStaminaPatch(&fileData[0], fileSize, m_FilePatches[PT_STAMINA]);
		FindAlwaysLightPatch(&fileData[0], fileSize, (CPatchDataLight*)m_FilePatches[PT_ALWAYS_LIGHT]);
		FindPaperdollSlotsPatch(&fileData[0], fileSize, (CPatchDataPaperdollSlots*)m_FilePatches[PT_PAPERDOLL_SLOTS]);
		FindSplashScreenPatch(&fileData[0], fileSize, m_FilePatches[PT_SPLASH_SCREEN]);
		FindResolutionPatch(&fileData[0], fileSize, m_FilePatches[PT_RESOLUTION]);
		FindOptionsNotificationPatch(&fileData[0], fileSize, m_FilePatches[PT_OPTIONS_NOTIFICATION]);
		FindMultiUOPatch(&fileData[0], fileSize, (CPatchDataMultiUO*)m_FilePatches[PT_MULTI_UO]);
		FindNoCryptPatch(&fileData[0], fileSize, (CPatchDataNoCrypt*)m_FilePatches[PT_NO_CRYPT]);
		FindGlobalSoundPatch(&fileData[0], fileSize, m_FilePatches[PT_GLOBAL_SOUND]);
		FindViewRangePatch(&fileData[0], fileSize, (CPatchDataViewRange*)m_FilePatches[PT_VIEW_RANGE]);
		FindGMStepPatch(&fileData[0], fileSize, (CPatchData*)m_FilePatches[PT_GM_STEP]);

		for (int i = PT_STAMINA; i < PT_COUNT; i++)
		{
			if (m_FilePatches[i] != NULL && m_FilePatches[i]->Found)
				result |= (1 << i);
		}
	}

	return result;
}
//----------------------------------------------------------------------------------
DWORD CPatcher::OnSetFilePatches(const char *path, const DWORD &patches, int viewRange)
{
	std::vector<unsigned char> fileData;

	OnGetFilePatches(path, fileData);

	int fileSize = (int)fileData.size();
	DWORD result = 0;

	if (!patches || !fileSize)
		return result;

	FILE *file = NULL;
	std::string backupPath = path;
	backupPath.resize(backupPath.length() - 4);

	char backupTime[100] = { 0 };

	SYSTEMTIME st;
	GetLocalTime(&st);

	sprintf_s(backupTime, "_backup_%i_%i_%i_%i_%i_%i.exe", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

	backupPath += backupTime;

	fopen_s(&file, backupPath.c_str(), "wb");

	if (file != NULL)
	{
		fwrite(&fileData[0], fileSize, 1, file);
		fclose(file);
		file = NULL;
	}
	else
		backupPath = "";

	for (int i = PT_STAMINA; i < PT_COUNT; i++)
	{
		if ((patches & (1 << i)) && m_FilePatches[i] != NULL)
		{
			if (i == PT_VIEW_RANGE)
				((CPatchDataViewRange*)m_FilePatches[i])->SetPatchViewRange(viewRange);

			if (m_FilePatches[i]->PatchFile(&fileData[0], fileSize))
				result |= (1 << i);
		}
	}

	if (result)
	{
		fopen_s(&file, path, "wb");

		if (file != NULL)
		{
			fwrite(&fileData[0], fileSize, 1, file);
			fclose(file);
			file = NULL;
		}
	}

	if (backupPath.length())
		MessageBoxA(0, ("Backup file saved:\n" + backupPath).c_str(), "Notification", 0);

	return result;
}
//----------------------------------------------------------------------------------
