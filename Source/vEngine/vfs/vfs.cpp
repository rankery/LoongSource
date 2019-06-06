//-----------------------------------------------------------------------------
// File: VFS
// Desc: interface for audio
// Auth: Lyp
// Date: 2004/02/09
//
//-----------------------------------------------------------------------------
#include "..\stdafx.h"
#include "package.h"
#include "vfs.h"

// 使用其它部件// 使用其它部件
#include "cpkdefine.h"
#include "cpkfast.h"
#include "zipreader.h"

#include "..\system\disciomgr.h"


namespace vEngine{
//-----------------------------------------------------------------------------
// class VirtualFileSys 虚拟文件系统
//-----------------------------------------------------------------------------
VirtualFileSys::VirtualFileSys()
{
	m_pPackage = NULL;
}
VirtualFileSys::~VirtualFileSys()
{
	SAFE_DEL(m_pPackage);
}

//-----------------------------------------------------------------------------
// 设置当前工作的文件包
//-----------------------------------------------------------------------------
BOOL VirtualFileSys::SetWorkingPackage(LPCTSTR szPackage)
{
	if( NULL == szPackage )
	{
		if( m_pPackage )
			m_pPackage->Unload();

		return TRUE;
	}

	DWORD dwMagicCode = GetMagicCode( szPackage );	// 得到包类型标识
	if( GT_INVALID == dwMagicCode )
		return FALSE;

	switch( dwMagicCode )	// 选择合适的类来打开包
	{
	case GT_CPK_MAGIC:
		m_pPackage = new CPK;
		break;
	case GT_ZIP_MAGIC:
		m_pPackage = new ZipReader;
		break;
	}
	if( m_pPackage )
		return m_pPackage->Load(szPackage);

	return FALSE;
}

//-----------------------------------------------------------------------------
// 当前是否有包打开
//-----------------------------------------------------------------------------
BOOL VirtualFileSys::HasWorkingPackage()
{
	return (P_VALID(m_pPackage));
}


//-----------------------------------------------------------------------------
// fopen
//-----------------------------------------------------------------------------
DWORD VirtualFileSys::Open(LPCTSTR szFileName)
{
	DWORD dwHandle = INVALID_FILE_HANDLE;

	if( m_pPackage )
	{
		dwHandle = (DWORD)m_pPackage->Open(szFileName);
	}
	else
	{
		// 没有激活任何包管理器，使用普通文件操作
		dwHandle = m_pDiscIOMgr->Create(szFileName,	GENERIC_READ,// | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 
			FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, NULL);

		if( dwHandle == INVALID_FILE_HANDLE )
			dwHandle = (DWORD)GT_INVALID;
	}

	return dwHandle;
}

//-----------------------------------------------------------------------------
// fclose
//-----------------------------------------------------------------------------
BOOL VirtualFileSys::Close(DWORD dwHandle)
{
	ASSERT(dwHandle);
	BOOL bResult = FALSE;

	if( m_pPackage )
	{
		bResult = m_pPackage->Close(dwHandle);
	}
	else
	{
		// 没有激活任何包管理器，使用普通文件操作
		bResult = m_pDiscIOMgr->Close(dwHandle);
	}

	return bResult;
}


//-----------------------------------------------------------------------------
// fread
//-----------------------------------------------------------------------------
DWORD VirtualFileSys::Read(PVOID lpMemOut, DWORD dwSize, DWORD dwHandle)
{
	ASSERT(lpMemOut);
	ASSERT(dwHandle);
	DWORD dwResult = 0;

	if( m_pPackage )
	{
		dwResult = m_pPackage->Read(lpMemOut, dwSize, dwHandle);
	}
	else
	{
		// 没有激活任何包管理器，使用普通文件操作
		dwResult = m_pDiscIOMgr->Read(dwHandle, lpMemOut, dwSize);
	}

	return dwResult;
}


//-----------------------------------------------------------------------------
// fread
//-----------------------------------------------------------------------------
DWORD VirtualFileSys::Seek(DWORD dwHandle, INT nOffset, DWORD dwOrigin)
{
	ASSERT(dwHandle);
	DWORD dwResult = 0;
	
	if( m_pPackage )
	{
		dwResult = m_pPackage->Seek(dwHandle, nOffset, dwOrigin);
	}	
	else
	{
		// 没有激活任何包管理器，使用普通文件操作
		dwResult = m_pDiscIOMgr->Seek(dwHandle, nOffset, dwOrigin);
	}

	return dwResult;
}


//-----------------------------------------------------------------------------
// ftell
//-----------------------------------------------------------------------------
DWORD VirtualFileSys::Tell(DWORD dwHandle)
{
	ASSERT(dwHandle);
	DWORD dwResult = 0;

	if( m_pPackage )
	{
		dwResult = m_pPackage->Tell(dwHandle);
	}	
	else
	{
		// 没有激活任何包管理器，使用普通文件操作
		dwResult = m_pDiscIOMgr->Seek(dwHandle, 0, GT_SEEK_CUR);
	}

	return dwResult;
}


//-----------------------------------------------------------------------------
// getfilesize
//-----------------------------------------------------------------------------
DWORD VirtualFileSys::GetSize(LPCTSTR szFileName)
{
	ASSERT(szFileName);
	DWORD dwResult = 0;

	if( m_pPackage )
	{
		dwResult = m_pPackage->LoadFile(NULL, szFileName);
	}	
	else
	{
		// 没有激活任何包管理器，使用普通文件操作

		// 使用GetSize比使用Seek(handle,0,SEEK_END)更安全
		dwResult = m_pDiscIOMgr->GetSize(szFileName);
	}
	return dwResult;
}



//-----------------------------------------------------------------------------
// 对所有包来说,使用此函数比使用open\read要快速
//-----------------------------------------------------------------------------
DWORD VirtualFileSys::LoadFile(LPVOID lpMemOut, LPCTSTR szFileName)
{
	ASSERT(szFileName);
	DWORD dwResult = 0;
	

	if( m_pPackage )
	{
		dwResult = m_pPackage->LoadFile(lpMemOut, szFileName);
	}	
	else
	{
		// 没有激活任何包管理器，使用普通文件操作
		dwResult = m_pDiscIOMgr->LoadToMem(lpMemOut, szFileName);
	}

	return dwResult;
}



//-----------------------------------------------------------------------------
// 得到包的magic code
//-----------------------------------------------------------------------------
DWORD VirtualFileSys::GetMagicCode(LPCTSTR szFileName)
{
	DWORD dwHandle = m_pDiscIOMgr->Create(szFileName,
		GENERIC_READ, FILE_SHARE_READ, // 其它应用程序可读此包
		NULL, OPEN_EXISTING, 
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, NULL);

	if( dwHandle == INVALID_FILE_HANDLE )
	{
		// 无法正确打开文件
		m_pLog->Write(_T("Couldn't open package file %s\r\n"), szFileName);
		return (DWORD)GT_INVALID;
	}

	DWORD dwMagic = (DWORD)GT_INVALID;
	
	if( 4 != m_pDiscIOMgr->Read(dwHandle, &dwMagic, 4) )
	{
		// 前四字节没法正确读出
		m_pLog->Write(_T("read first 4 bytes in file %s failed\r\n"), szFileName);
		dwMagic = (DWORD)GT_INVALID;
	}

	m_pDiscIOMgr->Close(dwHandle);
	return dwMagic;
}

}