//----------------------------------------------------------------------------------
#ifndef ProcessManagerH
#define ProcessManagerH
//----------------------------------------------------------------------------------
#include <windows.h>
//----------------------------------------------------------------------------------
const int PM_NO_ERROR = 0;
const int PM_HWND_EXISTS = 1;
const int PM_NO_PID = 2;
const int PM_ERROR_OPEN = 3;
const int PM_ERROR_READ = 4;
const int PM_ERROR_WRITE = 5;
const int PM_HWND_NOT_EXISTS = 6;
const int PM_PROCESS_NOT_EXISTS = 7;
//----------------------------------------------------------------------------------
class CProcessManager
{
private:
	HWND m_HWND;
	HANDLE m_Process;
	bool m_Active;
	int m_LastError;
	bool Exists();

public:
	CProcessManager();
	~CProcessManager();

	bool Open(HWND hWnd);
	void Close();

	bool Opened() { return m_Active; }
	HWND GetHWND() { return m_HWND; }
	int GetLastError() { return m_LastError; }
	void SetLastError(int errcode) { m_LastError = errcode; }

	//Safe read functions
	BYTE ReadByte(DWORD offset);
	short ReadShort(DWORD offset);
	float ReadFloat(DWORD offset);
	int ReadInt(DWORD offset);
	DWORD ReadUInt(DWORD offset);
	__int64 ReadInt64(DWORD offset);
	std::string ReadString(DWORD offset, int count = 32);
	void ReadPByte(DWORD offset, PBYTE buf, int len);

	//Write functions
	void WriteFloat(DWORD offset, float value);
	void WriteUInt(DWORD offset, DWORD value);
	void WriteByte(DWORD offset, BYTE buf);
	void WritePByte(DWORD offset, PBYTE buf, int len);
	void WritePWord(DWORD offset, PWORD buf, int len);

	//Unsafe read functions
	BYTE UnsafeReadByte(DWORD offset);
	short UnsafeReadShort(DWORD offset);
	float UnsafeReadFloat(DWORD offset);
	int UnsafeReadInt(DWORD offset);
	DWORD UnsafeReadUInt(DWORD offset);
	__int64 UnsafeReadInt64(DWORD offset);
	std::string UnsafeReadString(DWORD offset, int count = 32);
};
//----------------------------------------------------------------------------------
extern CProcessManager g_ProcessManager;
//----------------------------------------------------------------------------------
#endif
//----------------------------------------------------------------------------------
