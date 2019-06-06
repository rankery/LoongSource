//-----------------------------------------------------------------------------
// File: DiscIOMgr.h
// Desc: game resource file system 1.0
// Time: 2003-1-2
// Auth: CTCRST
//
// Copyright (c) 2003 Softstar Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#ifndef GT_DISC_IO_MGR
#define GT_DISC_IO_MGR
#pragma once

#define IS_INVALID_HANDLE(dw) (dw == (DWORD)INVALID_HANDLE_VALUE)


enum EDiscIOInterface
{
	DIOI_Null,
	DIOI_C,
	DIOI_Cpp,
	DIOI_CppBuffer,
	DIOI_Win32,
	DIOI_End
};



class CDiscIOMgr
{
public:

	static DWORD Create(LPCTSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, 
						LPVOID lpSecurityAttributes, DWORD dwCreationDisposition, 
						DWORD dwFlagsAndAttributes, DWORD dwTemplateFile);

	static DWORD CreateMapping(DWORD dwFileHandle, LPVOID lpAttrib, DWORD dwProtect,
							   DWORD dwMaxSize, LPCTSTR lpName);
	static DWORD CloseMappingHandle(DWORD dwFileMappingHandle);

	static LPVOID MapFile(DWORD dwFileMappingHandle, DWORD dwDesireAccess, 
						  DWORD dwFileOffset, SIZE_T dwNumberOfBytesToMap);
	static BOOL	 UnMapFile(LPCVOID lpBaseAddress);
	
	static BOOL  Read(DWORD dwFileHandle, LPVOID lpBuffer, DWORD dwNumberOfBytesToRead,
					  LPDWORD lpNumberOfBytesRead, LPVOID lpOverlapped);
	static BOOL	 Read(DWORD dwFileHandle, LPVOID lpBuffer, DWORD dwNumberOfBytesToRead);

	static BOOL  Write(DWORD dwFileHandle, LPCVOID lpBuffer, DWORD dwNumberOfBytesToWrite,
					   LPDWORD lpNumberOfBytesWritten, LPVOID lpOverlapped);
	static BOOL  Write(DWORD dwFileHandle, LPCVOID lpBuffer, DWORD dwNumberOfBytesToWrite);
	static BOOL  Close(DWORD dwFileHandle);

	static DWORD Seek(DWORD dwFileHandle, DWORD dwDistanceToMove, DWORD dwMoveMethod);
	static DWORD Seek(DWORD dwFileHandle, DWORD dwOffsetFromStart);

	static DWORD GetSize(LPCTSTR pFileName);
	static DWORD GetSize(DWORD dwFileHandle);
	static DWORD SetEOF(DWORD dwFileHandle);

	static BOOL  IsExist(LPCTSTR pFileName);
	static BOOL	 IsDir(LPCTSTR pFileName);
	static BOOL  LoadToMem(LPVOID pMem, LPCTSTR pFileName);
	
	// 非标优化用
	static BOOL	 _LoadToMem(LPVOID pMem, DWORD dwFileHandle, DWORD dwSize);

	static DWORD FindFirstFile(LPCTSTR pFileName, LPVOID lpFindData);
	static BOOL	 FindNextFile(DWORD dwFileHandle, LPVOID lpFindData);
	static BOOL	 FindClose(DWORD dwFileHandle);
	

	static DWORD GetFileNumInDir(LPCTSTR pFullName);
	static BOOL  DelFileOrDir(LPCTSTR pFullName);


	CDiscIOMgr();
	~CDiscIOMgr();


private:
	static VOID  GetFileNumInDir(LPCTSTR pFullName, DWORD& dwFileNum);
	static EDiscIOInterface m_eInterface;








};





#endif // GT_DISC_IO_MGR