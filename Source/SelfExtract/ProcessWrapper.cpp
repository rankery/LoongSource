#include "StdAfx.h"
#include ".\processwrapper.h"
#include "EnumProcess.h"

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

	STARTUPINFO	start_info = { sizeof(STARTUPINFO),NULL,"",
		NULL,0,0,0,0,0,0,0,STARTF_USESHOWWINDOW,0,0,NULL,0,0,0}; 

	start_info.lpDesktop = NULL;
	start_info.wShowWindow = SW_SHOW;

	std::string strPath;
	strPath = szPath;
	strPath.assign( strPath, 0, strPath.find_last_of( "\\") );

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

		} while( (GetTickCount()-dwTickCount) < 60000 );

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

HANDLE	CProcessWrapper::FindProcess( DWORD& dwProcessID, const TCHAR* strProcessName, BOOL bFullPath /* = FALSE  */)
{
	HANDLE hProcess = INVALID_HANDLE_VALUE;
	CEnumProcess	enumeration;
	CEnumProcess::CProcessEntry  entry;

	enumeration.SetMethod( ENUM_METHOD::TOOLHELP );


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
	

	CEnumProcess	enumeration;
	CEnumProcess::CProcessEntry  entry;
	enumeration.SetMethod( ENUM_METHOD::TOOLHELP );

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
