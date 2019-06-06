//-----------------------------------------------------------------------------
// File: DiscIOMgr.h
// Desc: game resource file system 1.0
// Time: 2003-1-2
// Auth: CTCRST
//-----------------------------------------------------------------------------
#pragma once
namespace vEngine{


#ifndef INVALID_FILE_HANDLE
	#define INVALID_FILE_HANDLE		((DWORD)INVALID_HANDLE_VALUE)
#endif
#ifndef INVALID_FILEMAP_HANDLE
#define INVALID_FILEMAP_HANDLE	((DWORD)INVALID_HANDLE_VALUE)
#endif
//-----------------------------------------------------------------------------
// 将系统文件操作包装了一层，以便移植
//-----------------------------------------------------------------------------
class VENGINE_API DiscIOMgr
{
public:

	DWORD Create(LPCTSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, 
					VOID* lpSecurityAttributes, DWORD dwCreationDisposition, 
					DWORD dwFlagsAndAttributes, DWORD dwTemplateFile);

	DWORD CreateMapping(DWORD dwFileHandle, VOID* lpAttrib, DWORD dwProtect,
						   DWORD dwMaxSize, LPCTSTR lpName);
	DWORD CloseMappingHandle(DWORD dwFileMappingHandle);

	VOID* MapFile(DWORD dwFileMappingHandle, DWORD dwDesireAccess, 
					  DWORD dwFileOffset, DWORD dwNumberOfBytesToMap);
	BOOL  UnMapFile(LPCVOID lpBaseAddress);
	
	BOOL  Read(DWORD dwFileHandle, LPVOID lpBuffer, DWORD dwNumberOfBytesToRead,
				  DWORD* lpNumberOfBytesRead, VOID* lpOverlapped);
	DWORD  Read(DWORD dwFileHandle, LPVOID lpBuffer, DWORD dwNumberOfBytesToRead);

	BOOL  Write(DWORD dwFileHandle, LPCVOID lpBuffer, DWORD dwNumberOfBytesToWrite,
				   DWORD* lpNumberOfBytesWritten, VOID* lpOverlapped);
	BOOL  Write(DWORD dwFileHandle, LPCVOID lpBuffer, DWORD dwNumberOfBytesToWrite);
	BOOL  Close(DWORD dwFileHandle);

	DWORD Seek(DWORD dwFileHandle, DWORD dwDistanceToMove, DWORD dwMoveMethod);
	DWORD Seek(DWORD dwFileHandle, DWORD dwOffsetFromStart);
	BOOL SeekEx(DWORD dwFileHandle, INT64 n64DistanceToMove, DWORD dwMoveMethod);

	DWORD GetSize(LPCTSTR pFileName);	// 如果文件不存在，则返回GT_INVALID
	DWORD GetSize(DWORD dwFileHandle);
	BOOL  GetSizeEx(DWORD dwFileHandle, INT64* pn64Size);
	DWORD SetEOF(DWORD dwFileHandle);

	BOOL  IsFileExist(LPCTSTR pFileName);
	BOOL  IsDir(LPCTSTR pFileName);
	DWORD  LoadToMem(VOID* pMem, LPCTSTR pFileName);

	DWORD FindFirstFile(LPCTSTR pFileName, VOID* lpFindData);
	BOOL  FindNextFile(DWORD dwFileHandle, VOID* lpFindData);
	BOOL  FindClose(DWORD dwFileHandle);
	
	DWORD GetFileNumInDir(LPCTSTR pFullName, DWORD& dwTotalBytes);
	VOID  GetFileNumInDir(LPCTSTR pFullName, DWORD& dwFileNum, DWORD& dwBytes);

	//得到该盘的剩余可用容量
	INT64 GetDriveFreeSpace(LPCTSTR szDriver);

	// 确认文件句柄是否无效
	__forceinline BOOL IsInvalidHandle(DWORD dwHandle)
	{ return (dwHandle == INVALID_FILE_HANDLE); }

};





} // namespace vEngine{
