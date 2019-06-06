#include "StdAfx.h"
#include ".\selfextract.h"
#include "./CPKCore/DiscIOMgr.h"
#include "./CPKCore/Util.h"


BOOL CSelfExtract::CreateSaveFile( const TCHAR* szFileName, BOOL bAddExe )
{
	if( bAddExe )	// 制作手动更新包
	{
		//如果包存在则自动打开包
		//创建要保存的文件
			m_dwCPKHandle = CDiscIOMgr::Create(szFileName,
			GENERIC_READ | GENERIC_WRITE,
			0, // 独占模式
			NULL, 
			OPEN_ALWAYS, 
			FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, 
			NULL);

		if (m_dwCPKHandle == (DWORD)INVALID_HANDLE_VALUE)
		{
			Msg(_T("Couldn't open file"));
			return FALSE;
		}


		lstrcpy(m_szCPKFileName, szFileName);	//保存文件名
	
		//加载自动解压程序资源，并写入文件
		HRSRC hrSrc = FindResource( AfxGetResourceHandle(), MAKEINTRESOURCE(IDR_EXE3), "EXE" );
		if( hrSrc == NULL )
			return FALSE;

		HGLOBAL hGlobal = LoadResource( AfxGetResourceHandle(), hrSrc );
		if( hGlobal == NULL )
			return FALSE;

		LPVOID lpExe = LockResource( hGlobal );
		if( lpExe == NULL )
			return FALSE;

		BOOL bResult = CDiscIOMgr::Write( m_dwCPKHandle, lpExe, (DWORD)SizeofResource( AfxGetResourceHandle(), hrSrc ) );
		if( bResult == FALSE )
			return FALSE;
		
		//记录自解压程序的大小，即更新文件的开始位置
		m_dwCPKHeaderAddr = SizeofResource( AfxGetResourceHandle(), hrSrc );

		return WriteHeader();

		
	}
	else
		return Create( szFileName );
}


BOOL CSelfExtract::WriteHeader()
{
	ASSERT( m_dwCPKHandle != GT_INVALID );

	if( TRUE == m_bLoaded && FALSE == this->Unload() )	//先确保卸载
		return FALSE;

	m_CPKHeader.dwLable = GT_CPK_LABEL;
	m_CPKHeader.dwVersion = GT_CPK_VERSION;
	m_CPKHeader.dwTableStart = m_dwCPKHeaderAddr + sizeof(m_CPKHeader);
	m_CPKHeader.dwSizeOfHeader =  sizeof(m_CPKHeader);
	m_CPKHeader.dwDataStart = m_dwCPKHeaderAddr + sizeof(m_CPKHeader);

	m_CPKHeader.dwFileNum = 0;
	m_CPKHeader.dwValidTableNum = 0;
	m_CPKHeader.dwMaxTableNum = GT_CPK_DEFAULT_FILE;
	m_CPKHeader.dwFragmentNum = 0;
	m_CPKHeader.dwPackageSize = m_CPKHeader.dwTableStart + 
		sizeof(tagCPKTable) * m_CPKHeader.dwMaxTableNum;

	CDiscIOMgr::Write(m_dwCPKHandle, &m_CPKHeader, sizeof(m_CPKHeader));

	// 将空的Table写入CPK
	ZeroMemory(m_pCPKTable, sizeof(tagCPKTable)*m_dwMaxCPKTable);
	CDiscIOMgr::Write(m_dwCPKHandle, m_pCPKTable, 
		sizeof(tagCPKTable)*m_CPKHeader.dwMaxTableNum);


	m_bLoaded = TRUE;
	return TRUE;
}

CONST INT VERSION_SIZE = 64;

BOOL CSelfExtract::WriteTail( CString& strVersionSrc, CString& strVersionDest )
{
	ASSERT( m_dwCPKHandle != INVALID_FILE_HANDLE );

	CHAR szSrcVersion[VERSION_SIZE]={0};
	CHAR szDestVersion[VERSION_SIZE]={0};

	strncpy(szSrcVersion, strVersionSrc.GetBuffer(), VERSION_SIZE);
	strncpy(szDestVersion, strVersionDest.GetBuffer(), VERSION_SIZE);

	CDiscIOMgr::Seek(m_dwCPKHandle, 0, FILE_END );
	CDiscIOMgr::Write( m_dwCPKHandle, szSrcVersion, VERSION_SIZE );
	CDiscIOMgr::Write( m_dwCPKHandle, szDestVersion, VERSION_SIZE );
	return  CDiscIOMgr::Write( m_dwCPKHandle, &m_dwCPKHeaderAddr, sizeof( DWORD ) );
}