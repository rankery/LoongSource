//-----------------------------------------------------------------------------
// File: DiscIOMgr.cpp
// Desc: game resource file system 1.0
// Time: 2003-1-2
// Auth: CTCRST
//-----------------------------------------------------------------------------
#include "..\stdafx.h"
#include "disciomgr.h"


namespace vEngine{
//-----------------------------------------------------------------------------
// open or create a file
//-----------------------------------------------------------------------------
DWORD DiscIOMgr::Create(LPCTSTR lpFileName, DWORD dwDesiredAccess, 
						 DWORD dwShareMode,  VOID* lpSecurityAttributes, 
						 DWORD dwCreationDisposition, 
						 DWORD dwFlagsAndAttributes, DWORD dwTemplateFile)
{
	ASSERT(lpFileName);

	return (DWORD)::CreateFile(lpFileName, dwDesiredAccess, dwShareMode,
		(LPSECURITY_ATTRIBUTES)lpSecurityAttributes, dwCreationDisposition,
		dwFlagsAndAttributes, (HANDLE)dwTemplateFile);
}




//-----------------------------------------------------------------------------
// open or create a file mapping
//-----------------------------------------------------------------------------
DWORD DiscIOMgr::CreateMapping(DWORD dwFileHandle, LPVOID lpAttrib, 
								DWORD dwProtect, DWORD dwMaxSize, LPCTSTR lpName)
{
	return (DWORD)::CreateFileMapping((HANDLE)dwFileHandle, 
		(LPSECURITY_ATTRIBUTES)lpAttrib, dwProtect, 0, dwMaxSize, lpName);
}


//-----------------------------------------------------------------------------
// close a file mapping handle
//-----------------------------------------------------------------------------
DWORD DiscIOMgr::CloseMappingHandle(DWORD dwFileMappingHandle)
{
	ASSERT(dwFileMappingHandle != INVALID_FILEMAP_HANDLE);
	return ::CloseHandle((HANDLE)dwFileMappingHandle); 

}




//-----------------------------------------------------------------------------
// map view of file, return the base address
//-----------------------------------------------------------------------------
void* DiscIOMgr::MapFile(DWORD dwFileMappingHandle, DWORD dwDesireAccess, 
						   DWORD dwFileOffset, SIZE_T dwNumberOfBytesToMap)
{
	return ::MapViewOfFile((HANDLE)dwFileMappingHandle, dwDesireAccess,
		0, dwFileOffset, dwNumberOfBytesToMap);
}



//-----------------------------------------------------------------------------
// Unmap view of file, require the base address
//-----------------------------------------------------------------------------
BOOL DiscIOMgr::UnMapFile(LPCVOID lpBaseAddress)
{
	return ::UnmapViewOfFile(lpBaseAddress);
}




//-----------------------------------------------------------------------------
// do file read
//-----------------------------------------------------------------------------
BOOL DiscIOMgr::Read(DWORD dwFileHandle, VOID* lpBuffer, 
					  DWORD dwNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead, 
					  VOID* lpOverlapped)
{
	ASSERT(!IsInvalidHandle(dwFileHandle));
	ASSERT(lpBuffer);

	return ::ReadFile((HANDLE)dwFileHandle, lpBuffer, dwNumberOfBytesToRead,
		lpNumberOfBytesRead, (LPOVERLAPPED)lpOverlapped);

}




//-----------------------------------------------------------------------------
// do file read
//-----------------------------------------------------------------------------
DWORD DiscIOMgr::Read(DWORD dwFileHandle, LPVOID lpBuffer, 
					  DWORD dwNumberOfBytesToRead)
{
	DWORD dwNumberOfBytesRead = 0;
	BOOL bResult = FALSE;

	ASSERT(!IsInvalidHandle(dwFileHandle));
	ASSERT(lpBuffer);

	bResult = ::ReadFile((HANDLE)dwFileHandle, lpBuffer, dwNumberOfBytesToRead,
		&dwNumberOfBytesRead, NULL);

	return dwNumberOfBytesRead;
}



//-----------------------------------------------------------------------------
// do file write
//-----------------------------------------------------------------------------
BOOL DiscIOMgr::Write(DWORD dwFileHandle, LPCVOID lpBuffer, 
					   DWORD dwNumberOfBytesToWrite, 
					   LPDWORD lpNumberOfBytesWritten, LPVOID lpOverlapped)
{
	return WriteFile((HANDLE)dwFileHandle, lpBuffer, dwNumberOfBytesToWrite,
			lpNumberOfBytesWritten, (LPOVERLAPPED)lpOverlapped);
}




//-----------------------------------------------------------------------------
// do file write
//-----------------------------------------------------------------------------
BOOL DiscIOMgr::Write(DWORD dwFileHandle, LPCVOID lpBuffer, 
					   DWORD dwNumberOfBytesToWrite)
{
	DWORD dwNumberOfBytesWritten = 0;
	BOOL bResult = FALSE;

	ASSERT(!IsInvalidHandle(dwFileHandle));
	ASSERT(lpBuffer);

	bResult = ::WriteFile((HANDLE)dwFileHandle, lpBuffer, dwNumberOfBytesToWrite,
		&dwNumberOfBytesWritten, NULL) == TRUE;
	if (dwNumberOfBytesWritten != dwNumberOfBytesToWrite)
		ERR(_T("Error in write file"));

	return bResult;

}




//-----------------------------------------------------------------------------
// close the file
//-----------------------------------------------------------------------------
BOOL DiscIOMgr::Close(DWORD dwFileHandle)
{
	ASSERT(!IsInvalidHandle(dwFileHandle));
	return CloseHandle((HANDLE)dwFileHandle);

}



//-----------------------------------------------------------------------------
// set file pointer
//-----------------------------------------------------------------------------
DWORD DiscIOMgr::Seek(DWORD dwFileHandle, DWORD dwDistanceToMove, 
					   DWORD dwMoveMethod)
{
	ASSERT(!IsInvalidHandle(dwFileHandle));
	LARGE_INTEGER li, liOut;
	li.HighPart = 0;
	li.LowPart = dwDistanceToMove;
	liOut.HighPart = 0;
	liOut.LowPart = dwDistanceToMove;
	SetFilePointerEx((HANDLE)dwFileHandle, li, &liOut, dwMoveMethod);
	return liOut.LowPart;
}



//-----------------------------------------------------------------------------
// set file pointer from beginning of the file
//-----------------------------------------------------------------------------
DWORD DiscIOMgr::Seek(DWORD dwFileHandle, DWORD dwOffsetFromStart)
{
	ASSERT(!IsInvalidHandle(dwFileHandle));
	LARGE_INTEGER li, liOut;
	li.HighPart = 0;
	li.LowPart = dwOffsetFromStart;
	liOut.HighPart = 0;
	liOut.LowPart = dwOffsetFromStart;
	SetFilePointerEx((HANDLE)dwFileHandle, li, &liOut, FILE_BEGIN);
	return liOut.LowPart;
}


//-----------------------------------------------------------------------------
// set file pointer
//-----------------------------------------------------------------------------
BOOL DiscIOMgr::SeekEx(DWORD dwFileHandle, INT64 n64DistanceToMove, DWORD dwMoveMethod)
{
	ASSERT(!IsInvalidHandle(dwFileHandle));
	return SetFilePointerEx((HANDLE)dwFileHandle, *(LARGE_INTEGER*)&n64DistanceToMove, NULL, dwMoveMethod);
}



//-----------------------------------------------------------------------------
// get length of the file
//-----------------------------------------------------------------------------
DWORD DiscIOMgr::GetSize(LPCTSTR pFileName)
{
	DWORD dwFileSize = 0;
	HANDLE hHandle = ::CreateFile(pFileName, GENERIC_READ,
		FILE_SHARE_READ, NULL, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, NULL);
			
	if( hHandle == INVALID_HANDLE_VALUE )
		return GT_INVALID;	// 如果文件不存在，则返回GT_INVALID
			
	dwFileSize = ::GetFileSize(hHandle, NULL);
	CloseHandle(hHandle);
	return dwFileSize;
}




//-----------------------------------------------------------------------------
// get length of the file
//-----------------------------------------------------------------------------
DWORD DiscIOMgr::GetSize(DWORD dwFileHandle)
{
	ASSERT(!IsInvalidHandle(dwFileHandle));
	return ::GetFileSize((HANDLE)dwFileHandle, NULL);

}

//-----------------------------------------------------------------------------
// get length of the file
//-----------------------------------------------------------------------------
BOOL DiscIOMgr::GetSizeEx(DWORD dwFileHandle, INT64* pn64Size)
{
	ASSERT(!IsInvalidHandle(dwFileHandle));
	return ::GetFileSizeEx((HANDLE)dwFileHandle, (PLARGE_INTEGER)pn64Size);
}



//-----------------------------------------------------------------------------
// the file may be truncate or extend, if succeeds, return nonzero
//-----------------------------------------------------------------------------
DWORD DiscIOMgr::SetEOF(DWORD dwFileHandle)
{
	ASSERT(!IsInvalidHandle(dwFileHandle));
	return ::SetEndOfFile((HANDLE)dwFileHandle);
}


//-----------------------------------------------------------------------------
// check the file whether exsit
//-----------------------------------------------------------------------------
BOOL DiscIOMgr::IsFileExist(LPCTSTR pFileName)
{		
	ASSERT(pFileName);
	WIN32_FIND_DATA FindData;
	BOOL bResult = TRUE;

	HANDLE hHandle = ::FindFirstFile(pFileName, &FindData);
	if( hHandle == INVALID_HANDLE_VALUE )
		return FALSE;
			
	if( FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
		bResult = FALSE;

	::FindClose(hHandle);
	return bResult;
}




//-----------------------------------------------------------------------------
// is filename refer to a directory
//-----------------------------------------------------------------------------
BOOL DiscIOMgr::IsDir(LPCTSTR pFileName)
{
	ASSERT(pFileName);
	WIN32_FIND_DATA FindData;
	BOOL bResult = FALSE;

	HANDLE hHandle = ::FindFirstFile(pFileName, &FindData);
	if( hHandle == INVALID_HANDLE_VALUE )
		return FALSE;
			
	if( FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
		bResult = TRUE;

	::FindClose(hHandle);
	return bResult;
}




//-----------------------------------------------------------------------------
// 使用此函数比使用open\read要快速
//-----------------------------------------------------------------------------
DWORD DiscIOMgr::LoadToMem(LPVOID pMem, LPCTSTR pFileName)
{
	ASSERT(pFileName);

	HANDLE hHandle = ::CreateFile(pFileName, GENERIC_READ, FILE_SHARE_READ, 
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | 
		FILE_FLAG_RANDOM_ACCESS, NULL);

	if( hHandle == INVALID_HANDLE_VALUE )
		return (DWORD)GT_INVALID;

	DWORD dwSize = ::GetFileSize(hHandle, NULL);
	if( dwSize == GT_INVALID )
	{ 
		ERR(_T("Couldn't get file size : %s"), pFileName);
		CloseHandle(hHandle);
		return (DWORD)GT_INVALID;
	}

	if( NULL == pMem )	// 外部调用只需要返回文件大小
	{
		CloseHandle(hHandle);
		return dwSize;
	}

	DWORD dwBytesRead = 0;
	BOOL bResult = ::ReadFile(hHandle, pMem, dwSize, &dwBytesRead, NULL);
	if( bResult == FALSE || dwBytesRead != dwSize )
	{
		ERR(_T("Couldn't read file : %s"), pFileName);
		CloseHandle(hHandle);
		return GT_INVALID;
	}

	CloseHandle(hHandle);
	return dwSize;
}



//-----------------------------------------------------------------------------
// find 
//-----------------------------------------------------------------------------
DWORD DiscIOMgr::FindFirstFile(LPCTSTR pFileName, void* lpFindData)
{
	ASSERT(pFileName);
	ASSERT(lpFindData);

	HANDLE hHandle = ::FindFirstFile(pFileName, (LPWIN32_FIND_DATA)lpFindData);
	if (hHandle == INVALID_HANDLE_VALUE)
		return GT_INVALID;

	return (DWORD)hHandle;


}



//-----------------------------------------------------------------------------
// find next
//-----------------------------------------------------------------------------
BOOL DiscIOMgr::FindNextFile(DWORD dwFileHandle, LPVOID lpFindData)
{
	ASSERT(dwFileHandle != INVALID_FILE_HANDLE);
	ASSERT(lpFindData);

	return ::FindNextFile((HANDLE)dwFileHandle, (LPWIN32_FIND_DATA)lpFindData);
}



//-----------------------------------------------------------------------------
// close find handle
//-----------------------------------------------------------------------------
BOOL DiscIOMgr::FindClose(DWORD dwFileHandle)
{
	ASSERT(dwFileHandle != INVALID_FILE_HANDLE);
	return ::FindClose((HANDLE)dwFileHandle); 
}




//-----------------------------------------------------------------------------
// GetFileNumInDir
//-----------------------------------------------------------------------------
DWORD DiscIOMgr::GetFileNumInDir(LPCTSTR pFullName, DWORD& dwTotalBytes)
{
	DWORD dwFileNum = 0;
	ASSERT(pFullName != NULL);

	GetFileNumInDir(pFullName, dwFileNum, dwTotalBytes);
	return dwFileNum;
}


//-----------------------------------------------------------------------------
// 注意!此函数存在问题,需要修正!
//-----------------------------------------------------------------------------
VOID DiscIOMgr::GetFileNumInDir(LPCTSTR pFullName, DWORD& dwFileNum, DWORD& dwBytes)
{
	TCHAR szTemp[MAX_PATH];
	WIN32_FIND_DATA FindData;
	HANDLE hHandle = INVALID_HANDLE_VALUE;
	
	dwFileNum++;
	lstrcpy(szTemp, pFullName);
	lstrcat(szTemp, _T("\\*.*"));
	hHandle = ::FindFirstFile(szTemp, &FindData);
	if( hHandle != INVALID_HANDLE_VALUE)
	{
		while (0 != ::FindNextFile(hHandle, &FindData))
		{
			if (FindData.cFileName[0] != _T('.'))
			{
				lstrcpy(szTemp, pFullName);
				lstrcat(szTemp, _T("\\"));
				lstrcat(szTemp, FindData.cFileName);
				GetFileNumInDir(szTemp, dwFileNum, dwBytes);
			}
		}

		::FindClose(hHandle);
	}
	else
	{	/*// 计算字节数太慢了
		hHandle = INVALID_HANDLE_VALUE;
		hHandle = CreateFile(pFullName, GENERIC_READ,
			FILE_SHARE_READ, NULL, OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, NULL);
			
		if (hHandle == INVALID_HANDLE_VALUE)
			return;
			
		dwBytes += ::GetFileSize(hHandle, NULL);
		CloseHandle(hHandle);
		/**/
	}
}


//-------------------------------------------------------------------------------
// 得到磁盘的剩余空间
//-------------------------------------------------------------------------------
INT64 DiscIOMgr::GetDriveFreeSpace(LPCTSTR szDriver)
{
	INT64 n64FreeBytesToUse = 0;
	typedef INT( WINAPI *GETDISKFREESPACE_PROC)(LPCTSTR, PULARGE_INTEGER, PULARGE_INTEGER, PULARGE_INTEGER);
	GETDISKFREESPACE_PROC pGetDiskFreeSpaceEx = 
		(GETDISKFREESPACE_PROC)GetProcAddress(GetModuleHandle(_T("kernel32.dll")), "GetDiskFreeSpaceExA");

	if( pGetDiskFreeSpaceEx )
	{
		ULARGE_INTEGER TotalNumberOfBytes;
		ULARGE_INTEGER TotalNumberOfFreeBytes;

		BOOL bRet = (pGetDiskFreeSpaceEx)(szDriver,
			(PULARGE_INTEGER)&n64FreeBytesToUse,
			(PULARGE_INTEGER)&TotalNumberOfBytes,
			(PULARGE_INTEGER)&TotalNumberOfFreeBytes);
	}
	else 
	{
		DWORD dwSectPerClust = 0;
		DWORD dwBytesPerSect = 0;
		DWORD dwFreeClusters = 0;
		DWORD dwTotalClusters = 0;

		BOOL bRet = GetDiskFreeSpace (szDriver, 
			&dwSectPerClust, 
			&dwBytesPerSect,
			&dwFreeClusters, 
			&dwTotalClusters);

		// Process GetDiskFreeSpace results.
		n64FreeBytesToUse = dwFreeClusters * dwBytesPerSect * dwSectPerClust;
	}

	return n64FreeBytesToUse;
}


} // namespace vEngine{
