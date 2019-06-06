//-----------------------------------------------------------------------------
//!\file process.cpp
//!\author Lyp
//!
//!\date 2004-08-03
//! last 2004-08-03
//!
//!\brief 进程相关功能
//-----------------------------------------------------------------------------
#include "..\stdafx.h"
#include "process.h"


namespace vEngine {
EnumProcess::EnumProcess() : m_pProcesses(NULL), m_pModules(NULL), m_pCurrentP(NULL), m_pCurrentM(NULL)
{
	m_hProcessSnap = INVALID_HANDLE_VALUE;
	m_hModuleSnap = INVALID_HANDLE_VALUE;

	PSAPI = ::LoadLibrary(_TEXT("PSAPI"));
	if (PSAPI)  
	{// Setup variables
		m_MAX_COUNT = 256;
		m_cProcesses = 0;
		m_cModules   = 0;

		// Find PSAPI functions
		FEnumProcesses = (PFEnumProcesses)::GetProcAddress(PSAPI, "EnumProcesses");
		FEnumProcessModules = (PFEnumProcessModules)::GetProcAddress(PSAPI, "EnumProcessModules");
#ifdef UNICODE
		FGetModuleFileNameEx = (PFGetModuleFileNameEx)::GetProcAddress(PSAPI, "GetModuleFileNameExW");
#else
		FGetModuleFileNameEx = (PFGetModuleFileNameEx)::GetProcAddress(PSAPI, "GetModuleFileNameExA");
#endif
	}

	TOOLHELP = ::LoadLibrary(_TEXT("Kernel32"));
	if (TOOLHELP)  
	{// Setup variables
		m_pe.dwSize = sizeof(m_pe);
		m_me.dwSize = sizeof(m_me);
		// Find ToolHelp functions

		FCreateToolhelp32Snapshot = (PFCreateToolhelp32Snapshot)::GetProcAddress(TOOLHELP, "CreateToolhelp32Snapshot");
		FProcess32First = (PFProcess32First)::GetProcAddress(TOOLHELP, "Process32First");
		FProcess32Next = (PFProcess32Next)::GetProcAddress(TOOLHELP, "Process32Next");
		FModule32First = (PFModule32First)::GetProcAddress(TOOLHELP, "Module32First");
		FModule32Next = (PFModule32Next)::GetProcAddress(TOOLHELP, "Module32Next");
	}

	// Find the preferred method of enumeration
	m_method = ENUM_METHOD::NONE;
	int method = GetAvailableMethods();
	if (method == (method|ENUM_METHOD::PSAPI))    m_method = ENUM_METHOD::PSAPI;
	if (method == (method|ENUM_METHOD::TOOLHELP)) m_method = ENUM_METHOD::TOOLHELP;
	if (method == (method|ENUM_METHOD::PROC16))   m_method += ENUM_METHOD::PROC16;

}

EnumProcess::~EnumProcess()
{
	if (m_pProcesses) {delete[](m_pProcesses);}
	if (m_pModules)   {delete[](m_pModules);}
	if (PSAPI) FreeLibrary(PSAPI);
	if (TOOLHELP) FreeLibrary(TOOLHELP);
	if (INVALID_HANDLE_VALUE != m_hProcessSnap) ::CloseHandle(m_hProcessSnap);
	if (INVALID_HANDLE_VALUE != m_hModuleSnap)  ::CloseHandle(m_hModuleSnap);

}



int EnumProcess::GetAvailableMethods()
{
	int res = 0;
	// Does all psapi functions exist?
	if (PSAPI&&FEnumProcesses&&FEnumProcessModules&&FGetModuleFileNameEx) 
		res += ENUM_METHOD::PSAPI;
	// How about Toolhelp?
	if (TOOLHELP&&FCreateToolhelp32Snapshot&&FProcess32Next&&FProcess32Next&&FModule32First&&FModule32Next) 
		res += ENUM_METHOD::TOOLHELP;
	return res;
}

int EnumProcess::SetMethod(int method)
{
	int avail = GetAvailableMethods();

	if (method != ENUM_METHOD::PROC16 && avail == (method|avail)) 
		m_method = method;

	return m_method;
}

int EnumProcess::GetSuggestedMethod()
{
	return m_method;
}

// Retrieves the first process in the enumeration. Should obviously be called before
// GetProcessNext
////////////////////////////////////////////////////////////////////////////////////
BOOL EnumProcess::GetProcessFirst(EnumProcess::ProcessEntry *pEntry)
{
	if (ENUM_METHOD::NONE == m_method) return FALSE; 


	if ((ENUM_METHOD::TOOLHELP|m_method) == m_method)
		// Use ToolHelp functions
		// ----------------------
	{
		m_hProcessSnap = FCreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (INVALID_HANDLE_VALUE == m_hProcessSnap) return FALSE;
		if (!FProcess32First(m_hProcessSnap, &m_pe)) return FALSE;
		pEntry->dwPID = m_pe.th32ProcessID;
		lstrcpy(pEntry->lpFilename, m_pe.szExeFile);
	}
	else
		// Use PSAPI functions
		// ----------------------
	{
		if (m_pProcesses) {delete[](m_pProcesses);}
		m_pProcesses = new DWORD[m_MAX_COUNT];
		m_pCurrentP = m_pProcesses;
		DWORD cbNeeded = 0;
		BOOL OK = FEnumProcesses(m_pProcesses, m_MAX_COUNT*sizeof(DWORD), &cbNeeded);

		// We might need more memory here..
		if (cbNeeded >= m_MAX_COUNT*sizeof(DWORD)) 
		{
			m_MAX_COUNT += 256;
			return GetProcessFirst(pEntry); // Try again.
		}

		if (!OK) return FALSE;
		m_cProcesses = cbNeeded/sizeof(DWORD); 
		return FillPStructPSAPI(*m_pProcesses, pEntry);
	}

	return TRUE;
}

// Returns the following process
////////////////////////////////////////////////////////////////
BOOL EnumProcess::GetProcessNext(EnumProcess::ProcessEntry *pEntry)
{
	if (ENUM_METHOD::NONE == m_method) 
		return FALSE; 
	pEntry->hTask16 = 0;

	// Use ToolHelp functions
	// ----------------------
	if ((ENUM_METHOD::TOOLHELP|m_method) == m_method)
	{
		if (!FProcess32Next(m_hProcessSnap, &m_pe)) return FALSE;
		pEntry->dwPID = m_pe.th32ProcessID;
		lstrcpy(pEntry->lpFilename, m_pe.szExeFile);
	}
	else
		// Use PSAPI functions
		// ----------------------
	{
		if (--m_cProcesses <= 0) 
			return FALSE;
		FillPStructPSAPI(*++m_pCurrentP, pEntry);
	}

	return TRUE;
}


BOOL EnumProcess::GetModuleFirst(DWORD dwPID, EnumProcess::ModuleEntry *pEntry)
{
	if (ENUM_METHOD::NONE == m_method) return FALSE; 
	// Use ToolHelp functions
	// ----------------------
	if ((ENUM_METHOD::TOOLHELP|m_method) == m_method)
	{
		if (INVALID_HANDLE_VALUE != m_hModuleSnap)  ::CloseHandle(m_hModuleSnap);
		m_hModuleSnap = FCreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID);

		if(!FModule32First(m_hModuleSnap, &m_me)) return FALSE;

		pEntry->pLoadBase = m_me.modBaseAddr;
		lstrcpy(pEntry->lpFilename, m_me.szExePath);
		pEntry->pPreferredBase = GetModulePreferredBase(dwPID, m_me.modBaseAddr);
		return TRUE;
	}
	else
		// Use PSAPI functions
		// ----------------------
	{
		if (m_pModules) {delete[](m_pModules);}
		m_pModules = new HMODULE[m_MAX_COUNT];
		m_pCurrentM = m_pModules;
		DWORD cbNeeded = 0;
		HANDLE hProc = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwPID);
		if (hProc)
		{
			BOOL OK = FEnumProcessModules(hProc, m_pModules, m_MAX_COUNT*sizeof(HMODULE), &cbNeeded);
			CloseHandle(hProc);

			// We might need more memory here..
			if (cbNeeded >= m_MAX_COUNT*sizeof(HMODULE)) 
			{
				m_MAX_COUNT += 256;
				return GetModuleFirst(dwPID, pEntry); // Try again.
			}

			if (!OK) 
				return FALSE;

			m_cModules = cbNeeded/sizeof(HMODULE); 
			return FillMStructPSAPI(dwPID, *m_pCurrentM, pEntry);
		}
		return FALSE;
	}
}


BOOL EnumProcess::GetModuleNext(DWORD dwPID, EnumProcess::ModuleEntry *pEntry)
{
	if (ENUM_METHOD::NONE == m_method) return FALSE; 

	// Use ToolHelp functions
	// ----------------------
	if ((ENUM_METHOD::TOOLHELP|m_method) == m_method)
	{
		if(!FModule32Next(m_hModuleSnap, &m_me)) return FALSE;

		pEntry->pLoadBase = m_me.modBaseAddr;
		lstrcpy(pEntry->lpFilename, m_me.szExePath);
		pEntry->pPreferredBase = GetModulePreferredBase(dwPID, m_me.modBaseAddr);
		return TRUE;
	}
	else
		// Use PSAPI functions
		// ----------------------
	{
		if (--m_cModules <= 0) 
			return FALSE;
		return FillMStructPSAPI(dwPID, *++m_pCurrentM, pEntry);
	}

}



BOOL EnumProcess::FillPStructPSAPI(DWORD dwPID, EnumProcess::ProcessEntry* pEntry)
{
	pEntry->dwPID = dwPID;
	// Open process to get filename
	HANDLE hProc = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwPID);
	if (hProc)
	{
		HMODULE hMod;
		DWORD size;
		// Get the first module (the process itself)
		if( FEnumProcessModules(hProc, &hMod, sizeof(hMod), &size) )
		{
			//Get filename
			if( !FGetModuleFileNameEx( hProc, hMod, pEntry->lpFilename, MAX_FILENAME) )
			{ 
				lstrcpy(pEntry->lpFilename, _T("N/A (error)"));  
			}
		}
		CloseHandle(hProc);
	}
	else
		lstrcpy(pEntry->lpFilename, _T("N/A (security restriction)"));

	return TRUE;
}


BOOL EnumProcess::FillMStructPSAPI(DWORD dwPID, HMODULE mMod, EnumProcess::ModuleEntry *pEntry)
{
	HANDLE hProc = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwPID);
	if (hProc)
	{
		if( !FGetModuleFileNameEx( hProc, mMod, pEntry->lpFilename, MAX_FILENAME) )
		{
			lstrcpy(pEntry->lpFilename, _T("N/A (error)"));  
		}  
		pEntry->pLoadBase = (PVOID) mMod;
		pEntry->pPreferredBase = GetModulePreferredBase(dwPID, (PVOID)mMod);
		CloseHandle(hProc);
		return TRUE;
	}
	return FALSE;
}



PVOID EnumProcess::GetModulePreferredBase(DWORD dwPID, PVOID pModBase)
{
	if (ENUM_METHOD::NONE == m_method) 
		return NULL; 
	
	HANDLE hProc = OpenProcess(PROCESS_VM_READ, FALSE, dwPID);
	if (hProc)
	{
		IMAGE_DOS_HEADER idh;
		IMAGE_NT_HEADERS inh;
		//Read DOS header
		ReadProcessMemory(hProc, pModBase, &idh, sizeof(idh), NULL);

		if (IMAGE_DOS_SIGNATURE == idh.e_magic) // DOS header OK?
			// Read NT headers at offset e_lfanew 
			ReadProcessMemory(hProc, (PBYTE)pModBase + idh.e_lfanew, &inh, sizeof(inh), NULL);

		CloseHandle(hProc); 

		if (IMAGE_NT_SIGNATURE == inh.Signature) //NT signature OK?
			// Get the preferred base...
			return (PVOID) inh.OptionalHeader.ImageBase; 
	}

	return NULL; //didn't find anything useful..
}



//--------------------------------------------------------------------
//判断后一串是否被包含在前一串的尾部
//--------------------------------------------------------------------
BOOL _tcsstr_tail(const TCHAR *szFirst, const TCHAR *szLast)
{
	INT nFirstLen = lstrlen(szFirst);
	INT nLastLen = lstrlen(szLast);

	if( nFirstLen <= nLastLen )
		return FALSE;
	INT m = nFirstLen / sizeof(TCHAR);
	INT n = nLastLen / sizeof(TCHAR);

	if( n<=0 )
		return FALSE;

	for(; n>=0; )
	{
		if( _tcsicmp( &szLast[n--], &szFirst[m--]) != 0 )
			return FALSE;
	}

	return TRUE;
}

//--------------------------------------------------------------------
//枚举窗口回调函数,关闭指定的窗口
//--------------------------------------------------------------------
BOOL CALLBACK EnumProc(HWND hWnd, LPARAM lp)
{
	if( GetWindowLong( hWnd, GWL_STYLE ) & WS_VISIBLE) 
	{ 
		//窗口可视有效
		DWORD pidwin;
		GetWindowThreadProcessId(hWnd, &pidwin);
		if( pidwin == (DWORD)lp )
		{
			SendMessage(hWnd, WM_DESTROY, 0, 0);
			return TRUE;
		}
	}

	return TRUE;	
}

CProcessWrapper::CProcessWrapper(void)
{
	
}

CProcessWrapper::~CProcessWrapper(void)
{
	//CloseHandle( m_hSnapShot );
}


bool	CProcessWrapper::ExecuteProcess( const TCHAR* szProcessName )
{
	
	TCHAR	szPath[MAX_PATH];
	_tcsncpy( szPath, szProcessName, sizeof(szPath) - 1 );

	STARTUPINFO	start_info = { sizeof(STARTUPINFO),NULL, _T(""),
		NULL,0,0,0,0,0,0,0,STARTF_USESHOWWINDOW,0,0,NULL,0,0,0}; 

	start_info.lpDesktop = NULL;
	start_info.wShowWindow = SW_SHOW;

	tstring strPath;
	strPath = szPath;
	strPath.assign( strPath, 0, strPath.find_last_of( _T("\\")) );

	PROCESS_INFORMATION pi;
	if( CreateProcess(NULL, szPath, NULL, NULL, 
		TRUE, NORMAL_PRIORITY_CLASS, NULL, strPath.c_str(), &start_info, &pi) )
	{
		return true;
	}
	else
		return false;

}

bool	CProcessWrapper::QuitProcess( const TCHAR* szProcessName, BOOL bFullPath /* = FALSE  */)
{

	DWORD dwID;
	HANDLE ProcessHandle[1024];
	DWORD  dwProcessID[1024];
	ZeroMemory(ProcessHandle, sizeof(HANDLE)*1024);
	ZeroMemory(dwProcessID, sizeof(DWORD)*1024);
	INT nProcessNum = FindAllProcess( ProcessHandle, dwProcessID, szProcessName, bFullPath );

	if( nProcessNum != 0 )
	{
		for(INT i=0; i<nProcessNum; i++)
		{
			//枚举该进程并发送WM_DESTROY消息
			BOOL ret = EnumWindows(EnumProc,  dwProcessID[i] );
			CloseHandle(ProcessHandle[i]);
		}

		DWORD dwTickCount = GetTickCount();
		do
		{
			HANDLE hProcess  = FindProcess(dwID, szProcessName, bFullPath);
			if( INVALID_HANDLE_VALUE == hProcess )
			{
				return true;
			}
			else
			{
				CloseHandle(hProcess);
			}


			Sleep(1000);

		} while( (GetTickCount()-dwTickCount) < 180000 );

		//还没关闭完成,那么就直接杀死
		ZeroMemory(ProcessHandle, sizeof(HANDLE)*1024);
		nProcessNum = FindAllProcess(ProcessHandle,dwProcessID, szProcessName, bFullPath);

		for(INT i=0; i<nProcessNum; i++)
		{
			HANDLE Killprcshd = OpenProcess(PROCESS_TERMINATE, FALSE, dwProcessID[i] );
			TerminateProcess(Killprcshd, 0);
			DWORD ObjectRetn = WaitForSingleObject(Killprcshd, 5000);
			CloseHandle(Killprcshd);

		}
	}

	return true;
}

bool	CProcessWrapper::ShutDownProcess( const TCHAR* szProcessName, BOOL bFullPath /* = FALSE  */)
{
	HANDLE ProcessHandle[1024];
	DWORD dwProcessID[1024];
	ZeroMemory(ProcessHandle, sizeof(HANDLE)*1024);
	ZeroMemory(dwProcessID, sizeof(DWORD)*1024);
	INT nProcessNum = FindAllProcess( ProcessHandle, dwProcessID, szProcessName, bFullPath );

	for(INT i=0; i<nProcessNum; i++)
	{
		HANDLE Killprcshd = OpenProcess(PROCESS_TERMINATE, FALSE, dwProcessID[i]);

		TerminateProcess(Killprcshd, 0);
		DWORD ObjectRetn = WaitForSingleObject(Killprcshd, 5000);
		CloseHandle(Killprcshd);

	}	
	return true;
}

INT CProcessWrapper::GetProcNum( DWORD& dwProcessID, const TCHAR* szProcessName, BOOL bFullPath /* = FALSE  */ )
{
	EnumProcess	enumeration;
	EnumProcess::ProcessEntry  entry;
	enumeration.SetMethod( ENUM_METHOD::PSAPI );

	INT nNum = 0;
	for( BOOL bRet = enumeration.GetProcessFirst( &entry ); bRet; bRet = enumeration.GetProcessNext( &entry ) )
	{
		if( bFullPath == TRUE )
		{
			if( 0 == _tcsicmp(entry.lpFilename, szProcessName) )
			{
				dwProcessID = entry.dwPID;
				nNum++;
			}
		}
		else
		{
			//包含
			if( TRUE == _tcsstr_tail(szProcessName, entry.lpFilename ) )
			{
				nNum++;
			}
		}
		Sleep(0);
	}

	return nNum;
}

HANDLE CProcessWrapper::FindProcess( DWORD& dwProcessID, const TCHAR* strProcessName, BOOL bFullPath /* = FALSE  */)
{
	HANDLE hProcess = INVALID_HANDLE_VALUE;
	EnumProcess	enumeration;
	EnumProcess::ProcessEntry  entry;

	enumeration.SetMethod( ENUM_METHOD::PSAPI );


	for ( BOOL bRet = enumeration.GetProcessFirst( &entry ); bRet; bRet = enumeration.GetProcessNext( &entry ) )
	{//失败的话返回值是NULL
		hProcess = OpenProcess(  PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, entry.dwPID );

		{
			if( bFullPath == TRUE )
			{
				if( 0 == _tcsicmp(entry.lpFilename, strProcessName) )
				{
					dwProcessID = entry.dwPID;
					return hProcess;
				}
				else
				{
					CloseHandle( hProcess );
				}
		
			}
			else
			{
				//包含
				if( TRUE == _tcsstr_tail(strProcessName, entry.lpFilename ) )
				{
					return hProcess;
				}
				else
				{
					CloseHandle( hProcess );
				}
			}

		}
	}


	return INVALID_HANDLE_VALUE;
}

int		CProcessWrapper::FindAllProcess( HANDLE* pProcessHandle, DWORD* dwID, const TCHAR* strProcessName, BOOL bFullPath /* = FALSE  */)
{
	HANDLE hProcess;
	INT nProcessNum = 0;
	

	EnumProcess	enumeration;
	EnumProcess::ProcessEntry  entry;
	enumeration.SetMethod( ENUM_METHOD::PSAPI );

	for ( BOOL bRet = enumeration.GetProcessFirst( &entry ); bRet; bRet = enumeration.GetProcessNext( &entry ) )
	{
		//失败的话返回值是NULL
		hProcess = OpenProcess(  PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, entry.dwPID );
		
		{

			if( bFullPath == TRUE )
			{
				if( 0 == _tcsicmp(entry.lpFilename, strProcessName) )
				{
					dwID[nProcessNum] = entry.dwPID ;
					pProcessHandle[nProcessNum++] = hProcess;
				
				}
				else
				{
					CloseHandle( hProcess );
				}

			}
			else
			{
				if( TRUE == _tcsstr_tail(strProcessName, entry.lpFilename ) )
				{
					dwID[nProcessNum] = entry.dwPID ;
					pProcessHandle[nProcessNum++] = hProcess;
				}
				else
				{
					CloseHandle( hProcess );
				}
			}

		}
	}

	return nProcessNum;
}


}