#pragma once
#include "..\Cool3D.h"
/**	\class IFS
	\brief 虚拟文件系统接口
	\remarks 一个虚拟文件系统对应一个文件包或者一个工作目录
*/
class Cool3D_API IFS
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

	IFS(){}
	virtual ~IFS(){}
};
