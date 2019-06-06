#pragma once
#include "ifs.h"

namespace Cool3D
{
	/**	\class MemFS 
		\brief 缓存在内存中的文件数据，用于减少磁盘读取量
	*/
	class Cool3D_API MemFS : public IFS
	{
	public:
		MemFS(void);
		virtual ~MemFS(void);

		//--IFS
		virtual BOOL SetWorkingPackage(LPCTSTR szPackage);
		virtual BOOL HasWorkingPackage();

		virtual DWORD	Open(LPCTSTR szFileName);
		virtual BOOL	Close(DWORD dwHandle);
		virtual DWORD	Read(PVOID lpMemOut, DWORD dwSize, DWORD dwHandle);

		virtual DWORD	Seek(DWORD dwHandle, INT nOffset, DWORD dwOrigin);
		virtual DWORD	Tell(DWORD dwHandle);

		virtual DWORD	GetSize(LPCTSTR szFileName);
		virtual DWORD	LoadFile(LPVOID lpMemOut, LPCTSTR szFileName);

		//--
		/** 将文件数据从磁盘读取到内存
		*/
		BOOL ReadFile(IFS* pFS,LPCTSTR szFileName);
		void Release();

	private:
		BYTE*	m_pData;
		DWORD	m_dwDataSize;
		DWORD	m_dwCurPos;
	};
}//namespace Cool3D