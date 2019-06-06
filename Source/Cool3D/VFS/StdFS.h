#pragma once
#include "ifs.h"

namespace Cool3D
{
	/**	\class StdFS 
		\brief 使用C runtime实现的IFS接口,用作默认的文件系统
	*/
	class Cool3D_API StdFS : public IFS
	{
	public:
		StdFS(void);
		virtual ~StdFS(void);

		virtual BOOL SetWorkingPackage(LPCTSTR szPackage);
		virtual BOOL HasWorkingPackage();
		
		virtual DWORD	Open(LPCTSTR szFileName);
		virtual BOOL	Close(DWORD dwHandle);
		virtual DWORD	Read(PVOID lpMemOut, DWORD dwSize, DWORD dwHandle);
		/**
			\param dwOrigin为FILE_BEGIN,FILE_CURRENT,FILE_END,它们的定义参见Win32 API的SetFilePointer()
		*/
		virtual DWORD	Seek(DWORD dwHandle, INT nOffset, DWORD dwOrigin);
		virtual DWORD	Tell(DWORD dwHandle);

		virtual DWORD	GetSize(LPCTSTR szFileName);
		virtual DWORD	LoadFile(LPVOID lpMemOut, LPCTSTR szFileName);
	};
}//namespace Cool3D