//----------------------------------------------------------------------------------
#include "stdafx.h"
#include "ProcessManager.h"
//----------------------------------------------------------------------------------
CProcessManager g_ProcessManager;
//----------------------------------------------------------------------------------
//-----------------------------------CProcessManager--------------------------------
//----------------------------------------------------------------------------------
CProcessManager::CProcessManager()
: m_HWND(0), m_Process(0), m_Active(false), m_LastError(PM_NO_ERROR)
{
}
//----------------------------------------------------------------------------------
CProcessManager::~CProcessManager()
{
	Close();
}
//----------------------------------------------------------------------------------
bool CProcessManager::Exists()
{
	if (m_HWND == NULL)
	{
		m_LastError = PM_HWND_NOT_EXISTS;
		return false;
	}

	DWORD pid = 0;
	GetWindowThreadProcessId(m_HWND, &pid);

	if (pid == NULL)
	{
		m_LastError = PM_PROCESS_NOT_EXISTS;
	}

	return (pid != NULL);
}
//----------------------------------------------------------------------------------
bool CProcessManager::Open(HWND hWnd)
{
	if (m_Active)
	{
		m_LastError = PM_HWND_EXISTS;
		return false;
	}

	m_Active = false;
	m_LastError = PM_NO_ERROR;

	if (m_HWND != NULL)
	{
		m_LastError = PM_HWND_EXISTS;
		return false;
	}

	DWORD pid = 0;
	GetWindowThreadProcessId(hWnd, &pid);

	if (pid == NULL)
	{
		m_LastError = PM_NO_PID;
		return false;
	}

	m_Process = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);

	if (m_Process == NULL)
	{
		m_LastError = PM_ERROR_OPEN;
		return false;
	}

	m_Active = true;
	m_HWND = hWnd;
	return true;
}
//----------------------------------------------------------------------------------
void CProcessManager::Close()
{
	if (m_Process != NULL)
		CloseHandle(m_Process);
	m_Process = NULL;
	m_HWND = 0;
	m_Active = false;
	//m_LastError = PM_NO_ERROR;
}
//----------------------------------------------------------------------------------
BYTE CProcessManager::ReadByte(DWORD offset)
{
	if (m_LastError || !m_Active || !Exists()) return 0;

	BYTE b = 0;

	if (!ReadProcessMemory(m_Process, (PVOID)offset, &b, 1, NULL))
	{
		m_LastError = PM_ERROR_READ;
	}

	return b;
}
//----------------------------------------------------------------------------------
void CProcessManager::ReadPByte(DWORD offset, PBYTE buf, int len)
{
	if (m_LastError || !m_Active || !Exists() || len < 1) return;

	if (!ReadProcessMemory(m_Process, (PVOID)offset, &buf[0], len, NULL))
	{
		m_LastError = PM_ERROR_READ;
	}
}
//----------------------------------------------------------------------------------
short CProcessManager::ReadShort(DWORD offset)
{
	if (m_LastError || !m_Active || !Exists()) return 0;

	short b = 0;

	if (!ReadProcessMemory(m_Process, (PVOID)offset, &b, 2, NULL))
	{
		m_LastError = PM_ERROR_READ;
	}

	return b;
}
//----------------------------------------------------------------------------------
float CProcessManager::ReadFloat(DWORD offset)
{
	if (m_LastError || !m_Active || !Exists()) return 0.0f;

	float b = 0.0f;

	if (!ReadProcessMemory(m_Process, (PVOID)offset, &b, 4, NULL))
	{
		m_LastError = PM_ERROR_READ;
	}

	return b;
}
//----------------------------------------------------------------------------------
int CProcessManager::ReadInt(DWORD offset)
{
	if (m_LastError || !m_Active || !Exists()) return 0;

	int b = 0;

	if (!ReadProcessMemory(m_Process, (PVOID)offset, &b, 4, NULL))
	{
		m_LastError = PM_ERROR_READ;
	}

	return b;
}
//----------------------------------------------------------------------------------
DWORD CProcessManager::ReadUInt(DWORD offset)
{
	if (m_LastError || !m_Active || !Exists()) return 0;

	DWORD b = 0;

	if (!ReadProcessMemory(m_Process, (PVOID)offset, &b, 4, NULL))
	{
		m_LastError = PM_ERROR_READ;
	}

	return b;
}
//----------------------------------------------------------------------------------
__int64 CProcessManager::ReadInt64(DWORD offset)
{
	if (m_LastError || !m_Active || !Exists()) return 0;

	__int64 b = 0;

	if (!ReadProcessMemory(m_Process, (PVOID)offset, &b, sizeof(__int64), NULL))
	{
		m_LastError = PM_ERROR_READ;
	}

	return b;
}
//----------------------------------------------------------------------------------
std::string CProcessManager::ReadString(DWORD offset, int count)
{
	if (m_LastError || !m_Active || !Exists()) return "";

	wchar_t b[128] = { 0 };

	if (!ReadProcessMemory(m_Process, (PVOID)offset, &b, count, NULL))
	{
		m_LastError = PM_ERROR_READ;
	}

	unsigned char buf[64] = { 0 };
	int len = 0;
	for (int i = 0; i < 128; i++)
	{
		if (!b[i] && (i < 127 && !b[i + 1])) break;
		else len++;
	}

	len = WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)b, len, (char*)buf, 64, 0, 0);
	buf[len] = 0;

	return (char*)buf;
}
//----------------------------------------------------------------------------------
void CProcessManager::WriteFloat(DWORD offset, float value)
{
	if (m_LastError || !m_Active || !Exists()) return;

	if (!WriteProcessMemory(m_Process, (PVOID)offset, &value, 4, NULL))
	{
		m_LastError = PM_ERROR_WRITE;
	}
}
//----------------------------------------------------------------------------------
void CProcessManager::WriteUInt(DWORD offset, DWORD value)
{
	if (m_LastError || !m_Active || !Exists()) return;

	if (!WriteProcessMemory(m_Process, (PVOID)offset, &value, 4, NULL))
	{
		m_LastError = PM_ERROR_WRITE;
	}
}
//----------------------------------------------------------------------------------
void CProcessManager::WriteByte(DWORD offset, BYTE buf)
{
	if (m_LastError || !m_Active || !Exists()) return;

	if (!WriteProcessMemory(m_Process, (PVOID)offset, &buf, 1, NULL))
	{
		m_LastError = PM_ERROR_WRITE;
	}
}
//----------------------------------------------------------------------------------
void CProcessManager::WritePByte(DWORD offset, PBYTE buf, int len)
{
	if (m_LastError || !m_Active || !Exists() || len < 1) return;

	for (int i = 0; i < len; i++)
	{
		if (!WriteProcessMemory(m_Process, (PVOID)(offset + i), &buf[i], 1, NULL))
		{
			m_LastError = PM_ERROR_WRITE;
			break;
		}
	}
}
//----------------------------------------------------------------------------------
void CProcessManager::WritePWord(DWORD offset, PWORD buf, int len)
{
	if (m_LastError || !m_Active || !Exists()) return;

	for (int i = 0; i < len; i++)
	{
		WORD w = ((buf[i] << 8) | (buf[i] >> 8));
		if (!WriteProcessMemory(m_Process, (PVOID)(offset + (i * 2)), &buf[i], 2, NULL))
		{
			m_LastError = PM_ERROR_WRITE;
			break;
		}
	}
}
//----------------------------------------------------------------------------------
BYTE CProcessManager::UnsafeReadByte(DWORD offset)
{
	BYTE b = 0;

	ReadProcessMemory(m_Process, (PVOID)offset, &b, 1, NULL);

	return b;
}
//----------------------------------------------------------------------------------
short CProcessManager::UnsafeReadShort(DWORD offset)
{
	short b = 0;

	ReadProcessMemory(m_Process, (PVOID)offset, &b, 2, NULL);

	return b;
}
//----------------------------------------------------------------------------------
float CProcessManager::UnsafeReadFloat(DWORD offset)
{
	float b = 0.0f;

	ReadProcessMemory(m_Process, (PVOID)offset, &b, 4, NULL);

	return b;
}
//----------------------------------------------------------------------------------
int CProcessManager::UnsafeReadInt(DWORD offset)
{
	int b = 0;

	ReadProcessMemory(m_Process, (PVOID)offset, &b, 4, NULL);

	return b;
}
//----------------------------------------------------------------------------------
DWORD CProcessManager::UnsafeReadUInt(DWORD offset)
{
	DWORD b = 0;

	ReadProcessMemory(m_Process, (PVOID)offset, &b, 4, NULL);

	return b;
}
//----------------------------------------------------------------------------------
__int64 CProcessManager::UnsafeReadInt64(DWORD offset)
{
	__int64 b = 0;

	ReadProcessMemory(m_Process, (PVOID)offset, &b, sizeof(__int64), NULL);

	return b;
}
//----------------------------------------------------------------------------------
std::string CProcessManager::UnsafeReadString(DWORD offset, int count)
{
	wchar_t b[128] = { 0 };

	ReadProcessMemory(m_Process, (PVOID)offset, &b, count, NULL);

	unsigned char buf[64] = { 0 };
	int len = 0;

	for (int i = 0; i < 128; i++)
	{
		if (!b[i] && (i < 127 && !b[i + 1])) break;
		else len++;
	}

	len = WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)b, len, (char*)buf, 64, 0, 0);
	buf[len] = 0;

	return (char*)buf;
}
//----------------------------------------------------------------------------------