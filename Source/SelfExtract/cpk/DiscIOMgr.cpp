//-----------------------------------------------------------------------------
// File: DiscIOMgr.cpp
// Desc: game resource file system 1.0
// Time: 2003-1-2
// Auth: CTCRST
//
// Copyright (c) 2003 Softstar Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#include "stdafx.h"
//#include <TChar.h>


#include "cpkdefine.h"

#include "disciomgr.h"
#include "util.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//-----------------------------------------------------------------------------
EDiscIOInterface CDiscIOMgr::m_eInterface = DIOI_Win32;





//-----------------------------------------------------------------------------
// construction / destruction
//-----------------------------------------------------------------------------
CDiscIOMgr::CDiscIOMgr()
{
}


CDiscIOMgr::~CDiscIOMgr()
{
}



//-----------------------------------------------------------------------------
// open or create a file
//-----------------------------------------------------------------------------
DWORD CDiscIOMgr::Create(LPCTSTR lpFileName, DWORD dwDesiredAccess, 
						 DWORD dwShareMode,  LPVOID lpSecurityAttributes, 
						 DWORD dwCreationDisposition, 
						 DWORD dwFlagsAndAttributes, DWORD dwTemplateFile)
{
	ASSERT(lpFileName);

	switch(m_eInterface)
	{
	case DIOI_Win32:
		return (DWORD)::CreateFile(lpFileName, dwDesiredAccess, dwShareMode,
			(LPSECURITY_ATTRIBUTES)lpSecurityAttributes, dwCreationDisposition,
			dwFlagsAndAttributes, (HANDLE)dwTemplateFile);

		break;

	default:
		ASSERT(0);
		break;
	}
	
	ASSERT(0);
	return 0;
}






//-----------------------------------------------------------------------------
// open or create a file mapping
//-----------------------------------------------------------------------------
DWORD CDiscIOMgr::CreateMapping(DWORD dwFileHandle, LPVOID lpAttrib, 
								DWORD dwProtect, DWORD dwMaxSize, LPCTSTR lpName)
{
	switch(m_eInterface)
	{
	case DIOI_Win32:
		return (DWORD)::CreateFileMapping((HANDLE)dwFileHandle, 
			(LPSECURITY_ATTRIBUTES)lpAttrib, dwProtect, 0, dwMaxSize, lpName);

		break;

	default:
		ASSERT(0);
		break;
	}
	
	ASSERT(0);
	return 0;
}


//-----------------------------------------------------------------------------
// close a file mapping handle
//-----------------------------------------------------------------------------
DWORD CDiscIOMgr::CloseMappingHandle(DWORD dwFileMappingHandle)
{
	ASSERT((HANDLE)dwFileMappingHandle != INVALID_HANDLE_VALUE);

	switch(m_eInterface)
	{
	case DIOI_Win32:
		return ::CloseHandle((HANDLE)dwFileMappingHandle); 
		break;

	default:
		ASSERT(0);
		break;
	}

	ASSERT(0);
	return FALSE;

}




//-----------------------------------------------------------------------------
// map view of file, return the base address
//-----------------------------------------------------------------------------
LPVOID CDiscIOMgr::MapFile(DWORD dwFileMappingHandle, DWORD dwDesireAccess, 
						   DWORD dwFileOffset, SIZE_T dwNumberOfBytesToMap)
{

	switch(m_eInterface)
	{
	case DIOI_Win32:
		return ::MapViewOfFile((HANDLE)dwFileMappingHandle, dwDesireAccess,
			0, dwFileOffset, dwNumberOfBytesToMap);

		break;

	default:
		ASSERT(0);
		break;
	}
	
	ASSERT(0);
	return 0;	

}



//-----------------------------------------------------------------------------
// Unmap view of file, require the base address
//-----------------------------------------------------------------------------
BOOL CDiscIOMgr::UnMapFile(LPCVOID lpBaseAddress)
{
	switch(m_eInterface)
	{
	case DIOI_Win32:
		return ::UnmapViewOfFile(lpBaseAddress);
		break;

	default:
		ASSERT(0);
		break;
	}
	
	ASSERT(0);
	return 0;	
}




//-----------------------------------------------------------------------------
// do file read
//-----------------------------------------------------------------------------
BOOL CDiscIOMgr::Read(DWORD dwFileHandle, LPVOID lpBuffer, 
					  DWORD dwNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead, 
					  LPVOID lpOverlapped)
{
	ASSERT(!IS_INVALID_HANDLE(dwFileHandle));
	ASSERT(lpBuffer);

	switch(m_eInterface)
	{
	case DIOI_Win32:
		return ::ReadFile((HANDLE)dwFileHandle, lpBuffer, dwNumberOfBytesToRead,
			lpNumberOfBytesRead, (LPOVERLAPPED)lpOverlapped);

		break;

	default:
		ASSERT(0);
		break;
	}

	ASSERT(0);
	return FALSE;
}




//-----------------------------------------------------------------------------
// do file read
//-----------------------------------------------------------------------------
BOOL CDiscIOMgr::Read(DWORD dwFileHandle, LPVOID lpBuffer, 
					  DWORD dwNumberOfBytesToRead)
{
	static DWORD dwNumberOfBytesRead = 0;
	static BOOL bResult = FALSE;

	ASSERT(!IS_INVALID_HANDLE(dwFileHandle));
	ASSERT(lpBuffer);

	switch(m_eInterface)
	{
	case DIOI_Win32:
		bResult = ::ReadFile((HANDLE)dwFileHandle, lpBuffer, dwNumberOfBytesToRead,
			&dwNumberOfBytesRead, NULL);

		if (dwNumberOfBytesRead != dwNumberOfBytesToRead)
		{
			ERR(CE_CANNOT_READ_DISC_FILE);
			return FALSE;
		}

		return bResult;
		break;

	default:
		ASSERT(0);
		break;
	}

	ASSERT(0);
	return FALSE;
}



//-----------------------------------------------------------------------------
// do file write
//-----------------------------------------------------------------------------
BOOL CDiscIOMgr::Write(DWORD dwFileHandle, LPCVOID lpBuffer, 
					   DWORD dwNumberOfBytesToWrite, 
					   LPDWORD lpNumberOfBytesWritten, LPVOID lpOverlapped, DWORD dwStartPos)
{
	switch(m_eInterface)
	{
	case DIOI_Win32:
		return WriteFile((HANDLE)dwFileHandle, lpBuffer, dwNumberOfBytesToWrite,
			lpNumberOfBytesWritten, (LPOVERLAPPED)lpOverlapped);

		break;

	default:
		ASSERT(0);
		break;
	}

	ASSERT(0);
	return 0;
}




//-----------------------------------------------------------------------------
// do file write
//-----------------------------------------------------------------------------
BOOL CDiscIOMgr::Write(DWORD dwFileHandle, LPCVOID lpBuffer, 
					   DWORD dwNumberOfBytesToWrite, DWORD dwStartPos)
{
	static DWORD dwNumberOfBytesWritten = 0;
	static BOOL bResult = FALSE;

	ASSERT(!IS_INVALID_HANDLE(dwFileHandle));
	ASSERT(lpBuffer);

	switch(m_eInterface)
	{
	case DIOI_Win32:
		bResult = ::WriteFile((HANDLE)dwFileHandle, lpBuffer, dwNumberOfBytesToWrite,
			&dwNumberOfBytesWritten, NULL);

		while(dwNumberOfBytesWritten != dwNumberOfBytesToWrite && dwStartPos != GT_INVALID )
		{
			if( FALSE == ClueFreeSpace(NULL, dwNumberOfBytesToWrite) )
			{
				return FALSE;
			}

			Seek(dwFileHandle, dwStartPos);

			dwNumberOfBytesWritten = 0;
			bResult = ::WriteFile((HANDLE)dwFileHandle, lpBuffer, dwNumberOfBytesToWrite,
				&dwNumberOfBytesWritten, NULL);
		}

		return bResult;
		break;

	default:
		ASSERT(0);
		break;
	}

	ASSERT(0);
	return FALSE;

}




//-----------------------------------------------------------------------------
// close the file
//-----------------------------------------------------------------------------
BOOL CDiscIOMgr::Close(DWORD dwFileHandle)
{
	ASSERT(!IS_INVALID_HANDLE(dwFileHandle));
	switch(m_eInterface)
	{
	case DIOI_Win32:
		return CloseHandle((HANDLE)dwFileHandle);
		break;

	default:
		ASSERT(0);
		break;
	}

	ASSERT(0);
	return 0;

}



//-----------------------------------------------------------------------------
// set file pointer
//-----------------------------------------------------------------------------
DWORD CDiscIOMgr::Seek(DWORD dwFileHandle, DWORD dwDistanceToMove, 
					   DWORD dwMoveMethod)
{
	ASSERT(!IS_INVALID_HANDLE(dwFileHandle));

	switch(m_eInterface)
	{
	case DIOI_Win32:
		return SetFilePointer((HANDLE)dwFileHandle, (LONG)dwDistanceToMove,
			NULL, dwMoveMethod);
		break;

	default:
		ASSERT(0);
		break;
	}

	ASSERT(0);
	return 0;
}



//-----------------------------------------------------------------------------
// set file pointer from beginning of the file
//-----------------------------------------------------------------------------
DWORD CDiscIOMgr::Seek(DWORD dwFileHandle, DWORD dwOffsetFromStart)
{
	ASSERT(!IS_INVALID_HANDLE(dwFileHandle));
	
	switch(m_eInterface)
	{
	case DIOI_Win32:
		return SetFilePointer((HANDLE)dwFileHandle, (LONG)dwOffsetFromStart,
			NULL, FILE_BEGIN);
		break;

	default:
		ASSERT(0);
		break;
	}

	ASSERT(0);
	return 0;
}




//-----------------------------------------------------------------------------
// get length of the file
//-----------------------------------------------------------------------------
DWORD CDiscIOMgr::GetSize(LPCTSTR pFileName)
{
	DWORD dwFileSize = 0;

	ASSERT(pFileName);
	switch(m_eInterface)
	{
	case DIOI_Win32:
		{
			HANDLE hHandle = INVALID_HANDLE_VALUE;
			hHandle = CreateFile(pFileName, GENERIC_READ,
				FILE_SHARE_READ, NULL, OPEN_EXISTING,
				FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, NULL);
			
			if (hHandle == INVALID_HANDLE_VALUE)
				return 0;
			
			dwFileSize = ::GetFileSize(hHandle, NULL);
			CloseHandle(hHandle);
			return dwFileSize;
		}
		break;

	default:
		ASSERT(0);
		break;
	}

	ASSERT(0);
	return 0;
}




//-----------------------------------------------------------------------------
// get length of the file
//-----------------------------------------------------------------------------
DWORD CDiscIOMgr::GetSize(DWORD dwFileHandle)
{
	ASSERT(!IS_INVALID_HANDLE(dwFileHandle));

	switch(m_eInterface)
	{
	case DIOI_Win32:
		return ::GetFileSize((HANDLE)dwFileHandle, NULL);
		break;

	default:
		ASSERT(0);
		break;
	}

	ASSERT(0);
	return 0;

}



//-----------------------------------------------------------------------------
// the file may be truncate or extend, if succeeds, return nonzero
//-----------------------------------------------------------------------------
DWORD CDiscIOMgr::SetEOF(DWORD dwFileHandle)
{
	ASSERT(!IS_INVALID_HANDLE(dwFileHandle));

	switch(m_eInterface)
	{
	case DIOI_Win32:
		return ::SetEndOfFile((HANDLE)dwFileHandle);
		break;

	default:
		ASSERT(0);
		break;
	}

	ASSERT(0);
	return 0;
}


//-----------------------------------------------------------------------------
// check the file whether exsit
//-----------------------------------------------------------------------------
BOOL CDiscIOMgr::IsExist(LPCTSTR pFileName)
{		
	ASSERT(pFileName);
	static WIN32_FIND_DATA FindData;

	switch(m_eInterface)
	{
	case DIOI_Win32:
		{
			
			HANDLE hHandle = INVALID_HANDLE_VALUE;
			hHandle = ::FindFirstFile(pFileName, &FindData);

			if (hHandle == INVALID_HANDLE_VALUE)
			{
				//	Msg(_T("File or directory %s does not exsit"), pFileName);
				return FALSE;
			}

			::FindClose(hHandle);

			return TRUE;


			/*
			//如果该文件已经被独享模式打开了，那么下面的判断就会认为该文件不存在
			HANDLE hHandle = INVALID_HANDLE_VALUE;
			hHandle = CreateFile(pFileName, GENERIC_READ,
				FILE_SHARE_READ, NULL, OPEN_EXISTING,
				FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, NULL);
			
			if (hHandle == INVALID_HANDLE_VALUE)
				return FALSE;
			
			CloseHandle(hHandle);
			return TRUE;
			/**/
		}
		break;


	default:
		ASSERT(0);
		break;
	}

	ASSERT(0);
	return FALSE;
}




//-----------------------------------------------------------------------------
// is filename refer to a directory
//-----------------------------------------------------------------------------
BOOL CDiscIOMgr::IsDir(LPCTSTR pFileName)
{
	ASSERT(pFileName);
	static WIN32_FIND_DATA FindData;
	BOOL bResult = FALSE;

	switch(m_eInterface)
	{
	case DIOI_Win32:
		{
			HANDLE hHandle = INVALID_HANDLE_VALUE;
			hHandle = ::FindFirstFile(pFileName, &FindData);

			if (hHandle == INVALID_HANDLE_VALUE)
			{
			//	Msg(_T("File or directory %s does not exsit"), pFileName);
				return FALSE;
			}
			
			if (FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				bResult = TRUE;

			::FindClose(hHandle);
		}
		break;


	default:
		ASSERT(0);
		break;
	}

	return bResult;
}




//-----------------------------------------------------------------------------
// load the whole file to mem
//-----------------------------------------------------------------------------
BOOL CDiscIOMgr::LoadToMem(LPVOID pMem, LPCTSTR pFileName)
{
	ASSERT(pMem);
	ASSERT(pFileName);
	switch(m_eInterface)
	{
	case DIOI_Win32:
		{
			HANDLE hHandle = INVALID_HANDLE_VALUE;
			hHandle = ::CreateFile(pFileName, GENERIC_READ, FILE_SHARE_READ, 
				NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | 
				FILE_FLAG_RANDOM_ACCESS, NULL);
			
			if (hHandle == INVALID_HANDLE_VALUE)
			{
				Msg(_T("Couldn't open file : %s"), pFileName);
				return FALSE;
			}
			
			DWORD dwSize = ::GetFileSize(hHandle, NULL);
			
			if (dwSize == GT_INVALID) 
			{ 
				Msg(_T("Couldn't get file size : %s"), pFileName);
				CloseHandle(hHandle);
				return FALSE;
			}
			
			DWORD dwBytesRead = 0;
			BOOL bResult = ReadFile(hHandle, pMem, dwSize, &dwBytesRead, NULL);
			if (bResult == FALSE || dwBytesRead != dwSize)
			{
				Msg(_T("Couldn't read file : %s"), pFileName);
				CloseHandle(hHandle);
				return FALSE;
			}
			
			CloseHandle(hHandle);	
			return TRUE;
		}
		break;
	
	default:
		ASSERT(0);
		break;
	}

	ASSERT(0);
	return FALSE;
}



//-----------------------------------------------------------------------------
// 非标优化用
//-----------------------------------------------------------------------------
BOOL CDiscIOMgr::_LoadToMem(LPVOID pMem, DWORD dwFileHandle, DWORD dwSize)
{
	ASSERT(pMem);
	switch(m_eInterface)
	{
	case DIOI_Win32:
		{
			ASSERT(dwFileHandle != (DWORD)INVALID_HANDLE_VALUE);
			ASSERT(dwSize != GT_INVALID);

			DWORD dwBytesRead = 0;
			BOOL bResult = ReadFile((HANDLE)dwFileHandle, pMem, dwSize, &dwBytesRead, NULL);
			if (bResult == FALSE || dwBytesRead != dwSize)
				return FALSE;
			
			return bResult;
		}
		break;
	
	default:
		ASSERT(0);
		break;
	}

	ASSERT(0);
	return FALSE;
}


//-----------------------------------------------------------------------------
// find 
//-----------------------------------------------------------------------------
DWORD CDiscIOMgr::FindFirstFile(LPCTSTR pFileName, LPVOID lpFindData)
{
	ASSERT(pFileName);
	ASSERT(lpFindData);

	switch(m_eInterface)
	{
	case DIOI_Win32:
		{
			HANDLE hHandle = INVALID_HANDLE_VALUE;
			hHandle = ::FindFirstFile(pFileName, 
				(LPWIN32_FIND_DATA)lpFindData);

			if (hHandle == INVALID_HANDLE_VALUE)
				return FALSE;
			
			return (DWORD)hHandle;
		}
		break;


	default:
		ASSERT(0);
		break;
	}

	ASSERT(0);
	return FALSE;
}



//-----------------------------------------------------------------------------
// find next
//-----------------------------------------------------------------------------
BOOL CDiscIOMgr::FindNextFile(DWORD dwFileHandle, LPVOID lpFindData)
{
	ASSERT((HANDLE)dwFileHandle != INVALID_HANDLE_VALUE);
	ASSERT(lpFindData);

	switch(m_eInterface)
	{
	case DIOI_Win32:
		{
			return ::FindNextFile((HANDLE)dwFileHandle, 
				(LPWIN32_FIND_DATA)lpFindData);
		}
		break;

	default:
		ASSERT(0);
		break;
	}

	ASSERT(0);
	return FALSE;
}



//-----------------------------------------------------------------------------
// close find handle
//-----------------------------------------------------------------------------
BOOL CDiscIOMgr::FindClose(DWORD dwFileHandle)
{
	ASSERT((HANDLE)dwFileHandle != INVALID_HANDLE_VALUE);

	switch(m_eInterface)
	{
	case DIOI_Win32:
		return ::FindClose((HANDLE)dwFileHandle); 
		break;

	default:
		ASSERT(0);
		break;
	}

	ASSERT(0);
	return FALSE;
}




//-----------------------------------------------------------------------------
// close find handle
//-----------------------------------------------------------------------------
DWORD CDiscIOMgr::GetFileNumInDir(LPCTSTR pFullName)
{
	DWORD dwFileNum = 0;
	ASSERT(pFullName != NULL);

	GetFileNumInDir(pFullName, dwFileNum);
	return dwFileNum;
}

//-----------------------------------------------------------------------------
// del file or dir
//-----------------------------------------------------------------------------
BOOL CDiscIOMgr::DelFileOrDir(LPCTSTR pFullName)
{
	TCHAR szTemp[MAX_PATH];
	WIN32_FIND_DATA FindData;
	HANDLE hHandle = INVALID_HANDLE_VALUE;
	
	lstrcpy(szTemp, pFullName);
	lstrcat(szTemp, _T("\\*.*"));
	hHandle = ::FindFirstFile(szTemp, &FindData);
	if( hHandle != INVALID_HANDLE_VALUE )
	{
		while( TRUE == ::FindNextFile(hHandle, &FindData) )
		{
			if( FindData.cFileName[0] != _T('.') )
			{
				lstrcpy(szTemp, pFullName);
				lstrcat(szTemp, _T("\\"));
				lstrcat(szTemp, FindData.cFileName);

				if( FILE_ATTRIBUTE_DIRECTORY & FindData.dwFileAttributes )
				{
					DelFileOrDir(szTemp);
				}
				else
				{
					::DeleteFile(szTemp);
				}
			}
		}

		::FindClose(hHandle);
	}

	::DeleteFile(pFullName);
	::RemoveDirectory(pFullName);
	return TRUE;
}



VOID CDiscIOMgr::GetFileNumInDir(LPCTSTR pFullName, DWORD& dwFileNum)
{
	TCHAR szTemp[MAX_PATH];
	WIN32_FIND_DATA FindData;
	HANDLE hHandle = INVALID_HANDLE_VALUE;
	
	lstrcpy(szTemp, pFullName);
	lstrcat(szTemp, _T("\\*.*"));
	hHandle = ::FindFirstFile(szTemp, &FindData);
	if( hHandle != INVALID_HANDLE_VALUE )
	{
		while( TRUE == ::FindNextFile(hHandle, &FindData) )
		{
			if( FindData.cFileName[0] != _T('.') )
			{
				dwFileNum++;
				if( FILE_ATTRIBUTE_DIRECTORY & FindData.dwFileAttributes )
				{
					lstrcpy(szTemp, pFullName);
					lstrcat(szTemp, _T("\\"));
					lstrcat(szTemp, FindData.cFileName);
					GetFileNumInDir(szTemp, dwFileNum);
				}
			}
		}

		::FindClose(hHandle);
	}
}

//-------------------------------------------------------------------------------
//得到该盘的剩余可用容量
//-------------------------------------------------------------------------------
ULARGE_INTEGER	CDiscIOMgr::GetDriveFreeSpace(char *strDriver)
{
	ULARGE_INTEGER i64FreeBytesToUse;
	i64FreeBytesToUse.QuadPart = 0;

	typedef INT( WINAPI *GETDISKFREESPACE_PROC)(LPCTSTR, PULARGE_INTEGER, PULARGE_INTEGER, PULARGE_INTEGER);

	GETDISKFREESPACE_PROC pGetDiskFreeSpaceEx = (GETDISKFREESPACE_PROC)GetProcAddress( GetModuleHandle("kernel32.dll"), 
		"GetDiskFreeSpaceExA" );


	if( pGetDiskFreeSpaceEx )
	{

		ULARGE_INTEGER TotalNumberOfBytes;
		ULARGE_INTEGER TotalNumberOfFreeBytes;

		BOOL bRet = (pGetDiskFreeSpaceEx)(strDriver,
			(PULARGE_INTEGER)&i64FreeBytesToUse,
			(PULARGE_INTEGER)&TotalNumberOfBytes,
			(PULARGE_INTEGER)&TotalNumberOfFreeBytes);

		// Process GetDiskFreeSpaceEx results.
	}
	else 
	{
		DWORD dwSectPerClust = 0;
		DWORD dwBytesPerSect = 0;
		DWORD dwFreeClusters = 0;
		DWORD dwTotalClusters = 0;

		BOOL bRet = GetDiskFreeSpace (strDriver, 
			&dwSectPerClust, 
			&dwBytesPerSect,
			&dwFreeClusters, 
			&dwTotalClusters);

		// Process GetDiskFreeSpace results.

		i64FreeBytesToUse.QuadPart = dwFreeClusters * dwBytesPerSect * dwSectPerClust;

	}

	return i64FreeBytesToUse;
}
//-------------------------------------------------------------------------------
//提示磁盘空间的状况, 不够就提示
//-------------------------------------------------------------------------------
BOOL CDiscIOMgr::ClueFreeSpace(char *strDriver, DWORD dwUseSize)
{
	char buf[500];
	sprintf(buf, "磁盘空间不足 %u byte, 建议先释放一部分空间", dwUseSize);

	ULARGE_INTEGER i64UseSie;
	i64UseSie.QuadPart = dwUseSize;

	while( GetDriveFreeSpace(strDriver).QuadPart < i64UseSie.QuadPart )
	{
		INT nRet = MessageBox(NULL, buf, "警告", MB_RETRYCANCEL);

		if( IDCANCEL == nRet )
		{
			//中止
			return FALSE;
		}
		else
		{
			continue;
		}
	}

	return TRUE;
}
//-------------------------------------------------------------------------------