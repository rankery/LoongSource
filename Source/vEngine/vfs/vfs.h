//-----------------------------------------------------------------------------
// File: VFS
// Desc: interface for audio
// Auth: Lyp
// Date: 2004/02/09
//
//-----------------------------------------------------------------------------
#pragma once
namespace vEngine{

class VENGINE_API IVFS
{
public:
	// 设置当前工作的文件包
	virtual BOOL SetWorkingPackage(LPCTSTR szPackage)=0;
	// 当前是否有包打开
	virtual BOOL HasWorkingPackage()=0;

	virtual DWORD	Open(LPCTSTR szFileName)=0;
	virtual BOOL	Close(DWORD dwHandle)=0;
	virtual DWORD	Read(PVOID lpMemOut, DWORD dwSize, DWORD dwHandle)=0;
	virtual DWORD	Seek(DWORD dwHandle, INT nOffset, DWORD dwOrigin)=0;
	virtual DWORD	Tell(DWORD dwHandle)=0;

	// 优化函数
	virtual DWORD	GetSize(LPCTSTR szFileName)=0;	// 如果文件不存在，则返回GT_INVALID
	virtual DWORD	LoadFile(LPVOID lpMemOut, LPCTSTR szFileName)=0;

	IVFS(){}
	virtual ~IVFS(){}
};


//-----------------------------------------------------------------------------
// class VirtualFileSys 虚拟文件系统
//-----------------------------------------------------------------------------
class VENGINE_API VirtualFileSys : public IVFS
{
public:
	// 设置当前工作的文件包, szPackage == NULL表示Unload
	BOOL SetWorkingPackage(LPCTSTR szPackage);
	// 当前是否有包打开
	BOOL HasWorkingPackage();

	DWORD	Open(LPCTSTR szFileName);
	BOOL	Close(DWORD dwHandle);
	DWORD	Read(PVOID lpMemOut, DWORD dwSize, DWORD dwHandle);
	DWORD	Seek(DWORD dwHandle, INT nOffset, DWORD dwOrigin);
	DWORD	Tell(DWORD dwHandle);

	// 得到指定文件大小
	DWORD	GetSize(LPCTSTR szFileName);	// 如果文件不存在，则返回GT_INVALID
	// 读包中单个文件到目标内存，返回原文件字节大小，
	// 与Open+Read相比因为直接读入指定内存，少了一次Memcpy
	DWORD	LoadFile(LPVOID lpMemOut, LPCTSTR szFileName);

	VirtualFileSys();
	~VirtualFileSys();


private:
	TObjRef<DiscIOMgr>		m_pDiscIOMgr;
	TObjRef<Log>			m_pLog;
	Package*				m_pPackage;
	
	DWORD GetMagicCode(LPCTSTR szFileName);
};



}