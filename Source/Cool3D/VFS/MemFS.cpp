#include "StdAfx.h"
#include ".\MemFS.h"
#include "..\Util\Exception.h"
#include "..\Util\FMemory.h"
#include "..\Kernel.h"


namespace Cool3D
{
	MemFS::MemFS(void)
	{
		m_pData=NULL;
		m_dwDataSize=0;
		m_dwCurPos=0;
	}

	MemFS::~MemFS(void)
	{
		Release();
	}

	BOOL MemFS::SetWorkingPackage(LPCTSTR szPackage)
	{
		return TRUE;
	}

	BOOL MemFS::HasWorkingPackage()
	{
		return TRUE;
	}

	BOOL MemFS::ReadFile(IFS* pFS,LPCTSTR szFileName)
	{
		Release();

		ASSERT(pFS!=NULL);
		m_dwDataSize=pFS->GetSize(szFileName);
		if(m_dwDataSize==0 
			||m_dwDataSize==-1)
			return FALSE;

		m_pData=new BYTE[m_dwDataSize];
		if(pFS->LoadFile(m_pData,szFileName)==-1)
		{
			delete[] m_pData;
			return FALSE;
		}
		
		return TRUE;
	}

	void MemFS::Release()
	{
		SAFE_DELETEA(m_pData);
	}

	DWORD	MemFS::Open(LPCTSTR szFileName)
	{
		ASSERT(m_pData!=NULL);

		m_dwCurPos=0;

		return (DWORD)m_pData;
	}

	BOOL	MemFS::Close(DWORD dwHandle)
	{
		return TRUE;
	}

	DWORD	MemFS::Read(PVOID lpMemOut, DWORD dwSize, DWORD dwHandle)
	{
		ASSERT(lpMemOut!=NULL && dwSize>0);
		ASSERT(m_dwCurPos+dwSize<=m_dwDataSize);
		ASSERT(dwHandle==(DWORD)m_pData);

		FMemcpy(lpMemOut,&m_pData[m_dwCurPos],dwSize);
		m_dwCurPos+=dwSize;
		return dwSize;
	}

	DWORD	MemFS::Seek(DWORD dwHandle, INT nOffset, DWORD dwOrigin)
	{
		ASSERT(dwHandle==(DWORD)m_pData);

		switch(dwOrigin)
		{
		case FILE_CURRENT:
			m_dwCurPos+=nOffset;
			break;
		case FILE_END:
			m_dwCurPos=m_dwDataSize+nOffset;
			break;
		case FILE_BEGIN:
			m_dwCurPos=nOffset;
			break;
		default:
			ASSERT(0);
		}
		if(m_dwCurPos>=m_dwDataSize)
			m_dwCurPos=m_dwDataSize-1;
		return 0;
	}

	DWORD	MemFS::Tell(DWORD dwHandle)
	{
		return m_dwCurPos;
	}

	DWORD	MemFS::GetSize(LPCTSTR szFileName)
	{
		ASSERT(0 && _T("not support!"));
		return -1;
	}

	DWORD	MemFS::LoadFile(LPVOID lpMemOut, LPCTSTR szFileName)
	{
		ASSERT(0 && _T("not support!"));
		return -1;
	}

}//namespace Cool3D