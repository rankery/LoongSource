//-----------------------------------------------------------------------------
// File: package
// Time: 2004-2-20
// Auth: CTCRST
//
//-----------------------------------------------------------------------------
#pragma once
namespace vEngine{
//-----------------------------------------------------------------------------
// 各种打包文件的统一接口
//-----------------------------------------------------------------------------
class Package
{
public:
	virtual BOOL	Load(LPCTSTR szZipFileName)=0;
	virtual BOOL	Unload()=0;

	// 针对包内单个文件的操作
	virtual DWORD	Open(LPCTSTR szFileName)=0;	// return handle
	virtual BOOL	Close(DWORD dwHandle)=0;
	virtual DWORD	Read(PVOID lpMemOut, DWORD dwSize, DWORD dwHandle)=0;
	virtual DWORD	Seek(DWORD dwHandle, INT nOffset, DWORD dwOrigin)=0;
	virtual	DWORD	Tell(DWORD dwHandle)=0;


	// 读包中单个文件到目标内存，返回原文件字节大小，
	// 与Open+Read相比因为直接读入指定内存，少了一次Memcpy
	// 如果内存目标lpMemOut为空，就仅返回原文件(未压缩时)大小
	virtual DWORD	LoadFile(LPVOID lpMemOut, LPCTSTR szFileName)=0;
	virtual ~Package(){};

protected:
	TObjRef<DiscIOMgr>	m_pDiscIOMgr;
	TObjRef<FilterMgr>	m_pFilterMgr;
	TObjRef<Util>		m_pUtil;
};

}