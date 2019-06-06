//-----------------------------------------------------------------------------
// File: zipreader
// Time: 2004-2-19
// Auth: CTCRST
//
//-----------------------------------------------------------------------------
#pragma once
namespace vEngine{

CONST DWORD GT_ZIP_MAGIC = 0x04034b50;
CONST INT MAX_FILENAME_LENGTH = 1000;
//-----------------------------------------------------------------------------
// 适合快速读取ZIP文件,不处理加密文件
// ZIP文件总是INTEL Litter-Endian格式
//-----------------------------------------------------------------------------
class ZipReader : public Package
{
public:
	BOOL	Load(LPCTSTR szZipFileName);
	BOOL	Unload();

	// 针对包内单个文件的操作
	DWORD	Open(LPCTSTR szFileName);	// return handle
	BOOL	Close(DWORD dwHandle);
	DWORD	Read(PVOID lpMemOut, DWORD dwSize, DWORD dwHandle);
	DWORD	Seek(DWORD dwHandle, INT nOffset, DWORD dwOrigin);
	DWORD	Tell(DWORD dwHandle);


	// 读包中单个文件到目标内存，返回原文件字节大小，
	// 与Open+Read相比因为直接读入指定内存，少了一次Memcpy
	// 如果内存目标lpMemOut为空，就仅返回原文件(未压缩时)大小
	DWORD	LoadFile(LPVOID lpMemOut, LPCTSTR szFileName);

	ZipReader();~ZipReader();

private:
	BOOL		m_bLoaded;	// 是否有Zip载入
	DWORD		m_dwZipHandle;
	DWORD		m_dwZipFileSize;
	TCHAR		m_szZipFileName[MAX_PATH];	// for debug 
	
	DWORD		m_dwFileNumInZip;
	DWORD		m_dwCentralDirOffset;
	PVOID		m_lpFileTable;
	
	INT			m_nOpenedFileNum;

	// 全小写给定字符串，将所有斜杠改成反斜杠，并为尾部添加结束符"\0"
	VOID		HandleFileNameString(LPTSTR szString, DWORD dwLength);
	

};











}	// namespace vEngine{
