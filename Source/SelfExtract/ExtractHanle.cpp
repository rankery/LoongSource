#include "StdAfx.h"
#include ".\extracthanle.h"
#include "CPKCore/DiscIOMgr.h"
#include "CPKCore/Util.h"

CExtractHanle::CExtractHanle(CTSMemMgr* pMgr):CPKVfs(pMgr)
{
	m_dwCPKStartAddress = 0;
}

CONST INT VERSION_SIZE = 64;
//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
BOOL CExtractHanle::LoadExtractFile( const char* szFileName )
{
	ASSERT( szFileName != NULL );

	if( CDiscIOMgr::IsExist(szFileName) == FALSE )
		return FALSE;

	lstrcpy(m_szCPKFileName, szFileName);

	m_dwCPKFileHandle = CDiscIOMgr::Create(szFileName,
		GENERIC_READ,
		FILE_SHARE_READ, // 非独占模式
		NULL, 
		OPEN_EXISTING, 
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, 
		NULL);

	if (m_dwCPKFileHandle == INVALID_FILE_HANDLE )
	{
		Msg(_T("Couldn't open file %s"), szFileName);
		CDiscIOMgr::Close(m_dwCPKFileHandle);
		ASSERT(0);
		return FALSE;
	}

	DWORD dwFileSize = CDiscIOMgr::GetSize( m_dwCPKFileHandle );


	CHAR szSrcVersion[VERSION_SIZE]={0};
	CHAR szDestVersion[VERSION_SIZE]={0};

	CDiscIOMgr::Seek( m_dwCPKFileHandle, dwFileSize - VERSION_SIZE*2 - sizeof(DWORD) );
	CDiscIOMgr::Read( m_dwCPKFileHandle, &szSrcVersion, VERSION_SIZE );
	CDiscIOMgr::Read( m_dwCPKFileHandle, &szDestVersion, VERSION_SIZE );
	CDiscIOMgr::Read( m_dwCPKFileHandle, &m_dwCPKStartAddress, sizeof(DWORD) );
	
	m_strVersionSrc = szSrcVersion;
	m_strVersionDest = szDestVersion;

	this->CutSpaceOfStringHeadAndTail(m_strVersionSrc);
	this->CutSpaceOfStringHeadAndTail(m_strVersionDest);

	CDiscIOMgr::Seek(m_dwCPKFileHandle, m_dwCPKStartAddress, FILE_BEGIN );
	return ReadCPkHeader();
}


//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
BOOL CExtractHanle::ReadCPkHeader()
{
	// 读入文件头
	ZeroMemory(&m_CPKHeader, sizeof(m_CPKHeader));
	if( FALSE == CDiscIOMgr::Read(m_dwCPKFileHandle, &m_CPKHeader, sizeof(m_CPKHeader)) )
	{
		Msg(_T("Could not load table from the CPK "));
		return FALSE;
	}
	if (m_CPKHeader.dwLable != GT_CPK_LABEL)
	{
		Msg(_T("Unknow file format "));
		CDiscIOMgr::Close(m_dwCPKFileHandle);
		ASSERT(0);
		return FALSE;
	}


	// 读入文件Table

	//我们看看表项空间够不够
	if( m_dwMaxCPKTable < m_CPKHeader.dwMaxTableNum )
	{
		FREEARR(m_pCPKTable);
		m_pCPKTable = new tagCPKTable[m_CPKHeader.dwMaxTableNum];
		ZeroMemory(m_pCPKTable, sizeof(tagCPKTable)*m_CPKHeader.dwMaxTableNum);
		m_dwMaxCPKTable = m_CPKHeader.dwMaxTableNum;

		FREEARR(m_pFragment);
		FREEARR(m_pIndexToFragment);

		m_pFragment = new INT[m_dwMaxCPKTable];
		m_pIndexToFragment = new INT[m_dwMaxCPKTable];

		ZeroMemory(m_pFragment, sizeof(INT)*m_dwMaxCPKTable);
		ZeroMemory(m_pIndexToFragment, sizeof(INT)*m_dwMaxCPKTable);
	}

	//定位表项位置
	if( FALSE == CDiscIOMgr::Seek(m_dwCPKFileHandle, m_CPKHeader.dwTableStart) )
	{
		Msg(_T("Could not seek table from the CPK, seekpos=%ld"), \
			 m_CPKHeader.dwTableStart);

		return FALSE;
	}

	if( FALSE == CDiscIOMgr::Read(m_dwCPKFileHandle, m_pCPKTable,  
		sizeof(tagCPKTable)*m_CPKHeader.dwMaxTableNum) )
	{
		Msg(_T("Could not load table from the CPK"));
		return FALSE;
	}

	for(DWORD i=0; i<m_CPKHeader.dwMaxTableNum; i++)
	{
		if( IS_FILE_OR_DIR_VALID(m_pCPKTable[i].dwFlag) )
			m_mapValidTable.insert(std::make_pair(m_pCPKTable[i].dwCRC, i));
	}
	// 得到所有碎片信息
	GetAllFragment();

	// 建立文件映射
	m_dwCPKMappingHandle = CDiscIOMgr::CreateMapping(m_dwCPKFileHandle, NULL,
		PAGE_READONLY, 0, NULL);

	if( m_dwCPKMappingHandle == NULL )
	{
		ERR(_T("couldn't create file mapping %s"), szFileName);
		CDiscIOMgr::Close(m_dwCPKFileHandle);
		m_dwCPKFileHandle = INVALID_FILE_HANDLE;

		return FALSE;
	}
	else
	{
		// 文件映射已经存在
		if( ::GetLastError() == ERROR_ALREADY_EXISTS )
		{
			ERR(_T("File mapping %s has already exist"), szFileName);
			CDiscIOMgr::CloseMappingHandle(m_dwCPKMappingHandle);
			m_dwCPKMappingHandle = 0;
			CDiscIOMgr::Close(m_dwCPKFileHandle);
			m_dwCPKFileHandle = INVALID_FILE_HANDLE;
			return FALSE;
		}
	}

	m_bLoaded = TRUE;

	return TRUE;
}


//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
BOOL CExtractHanle::IsVersionCorrect( std::string& strVersion )
{
	this->CutSpaceOfStringHeadAndTail(strVersion);
	if( strVersion == m_strVersionSrc )
		return TRUE;

	return FALSE;
}


//-----------------------------------------------------------------------------
// 去掉tstring首尾的空格
//-----------------------------------------------------------------------------
VOID CExtractHanle::CutSpaceOfStringHeadAndTail(std::string& string)
{
	// 必须检查:如果字符串中只有空格,不能进行此操作
	if( string.find_first_not_of(_T(" ")) != -1 )
	{
		string.assign(string, string.find_first_not_of(_T(" ")),
			string.find_last_not_of(_T(" "))-string.find_first_not_of(_T(" "))+1);
	}
	else
	{
		string.clear();
	}
}
