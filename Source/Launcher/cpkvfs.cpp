//-----------------------------------------------------------------------------
// File: CPKCore.cpp
// Desc: game resource file system 3.0
// Time: 2005-5-31
// Auth: tidy
//
// Copyright (c) 2003 Softstar Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "cpkvfs.h"
// 使用其它部件
#include "Util.h"
#include "filtermgr.h"
#include "disciomgr.h"
#include "md5.h"
//-----------------------------------------------------------------------------
//构造函数
//-----------------------------------------------------------------------------
CPKVfs::CPKVfs()
{
	m_bLoaded = FALSE;
	m_pUtil = new Util;

	m_dwAllocGranularity = this->GetAllocationGranularity();

	m_dwMaxCPKTable = GT_CPK_DEFAULT_FILE;
	m_pCPKTable = new tagCPKTable[m_dwMaxCPKTable];
	ZeroMemory(m_pCPKTable, sizeof(tagCPKTable)*m_dwMaxCPKTable);
	m_pFragment = new INT[m_dwMaxCPKTable];
	m_pIndexToFragment = new INT[m_dwMaxCPKTable];

	memset(m_pFragment, -1, sizeof(INT)*m_dwMaxCPKTable);
	memset(m_pIndexToFragment, -1, sizeof(INT)*m_dwMaxCPKTable);
}
//-----------------------------------------------------------------------------
//析构函数
//-----------------------------------------------------------------------------
CPKVfs::~CPKVfs()
{
	SAFE_DEL_ARRAY(m_pCPKTable);
	SAFE_DEL_ARRAY(m_pFragment);
	SAFE_DEL_ARRAY(m_pIndexToFragment);
	SAFE_DEL( m_pUtil );

	m_mapValidTable.clear();
}
//-----------------------------------------------------------------------------
// 得到内存分配尺度
//-----------------------------------------------------------------------------
DWORD	CPKVfs::GetAllocationGranularity()
{
	SYSTEM_INFO si;
	::GetSystemInfo(&si);
	return si.dwAllocationGranularity;
}
//-----------------------------------------------------------------------------
// 创建新的管理包,如果存在就直接打开，不存在就创建并打开，其实包含了load功能
//-----------------------------------------------------------------------------
BOOL CPKVfs::Create(LPCSTR pFileName)
{
	ASSERT( pFileName );
	if( m_bLoaded && !this->Unload() )	//先确保卸载
		return FALSE;

	m_mapValidTable.clear();
	// 如果包存在则自动打开包
	if( CDiscIOMgr::IsExist(pFileName) )
	{
		return this->Load(pFileName);
	}

	strcpy(m_szCPKFileName, pFileName);	//保存文件名

	m_CPKHeader.dwLable = GT_CPK_LABEL;
	m_CPKHeader.dwVersion = GT_CPK_VERSION;
	m_CPKHeader.dwTableStart = sizeof(m_CPKHeader);
	m_CPKHeader.dwSizeOfHeader = sizeof(m_CPKHeader);
	m_CPKHeader.dwDataStart = sizeof(m_CPKHeader);

	m_CPKHeader.dwFileNum = 0;
	m_CPKHeader.dwValidTableNum = 0;
	m_CPKHeader.dwMaxTableNum = GT_CPK_DEFAULT_FILE;
	m_CPKHeader.dwFragmentNum = 0;
	m_CPKHeader.dwPackageSize = m_CPKHeader.dwTableStart + 
		sizeof(tagCPKTable) * GT_CPK_DEFAULT_FILE;

	m_dwCPKFileHandle = CDiscIOMgr::Create(pFileName,
		GENERIC_READ | GENERIC_WRITE,
		0, // 独占模式
		NULL, 
		OPEN_ALWAYS, 
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, 
		NULL);

	if (m_dwCPKFileHandle == INVALID_FILE_HANDLE)
	{
		ErrMsg(g_StrTable[_T("OpenFileFailed")], m_pUtil->AnsiToUnicode(pFileName));
		return FALSE;
	}

	CDiscIOMgr::Write(m_dwCPKFileHandle, &m_CPKHeader, sizeof(m_CPKHeader));

	// 将空的Table写入CPK
	ZeroMemory(m_pCPKTable, sizeof(tagCPKTable)*m_dwMaxCPKTable);
	CDiscIOMgr::Write(m_dwCPKFileHandle, m_pCPKTable, 
		sizeof(tagCPKTable)*m_CPKHeader.dwMaxTableNum);

	// 建立文件映射
	m_dwCPKMappingHandle = CDiscIOMgr::CreateMapping(m_dwCPKFileHandle, NULL,
		PAGE_READONLY, 0, NULL);

	if( m_dwCPKMappingHandle == NULL )
	{
		ErrMsg(g_StrTable[_T("CreateFileMappingFailed")], m_pUtil->AnsiToUnicode(pFileName));
		CDiscIOMgr::Close(m_dwCPKFileHandle);
		m_dwCPKFileHandle = INVALID_FILE_HANDLE;

		return FALSE;
	}
	else
	{
		// 文件映射已经存在
		if( ::GetLastError() == ERROR_ALREADY_EXISTS )
		{
			ErrMsg(g_StrTable[_T("FileMappingAlreadyExist")], m_pUtil->AnsiToUnicode(pFileName));
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
//加载cpk文件，不存在就返回错误
//-----------------------------------------------------------------------------
BOOL CPKVfs::Load(LPCSTR pFileName)
{
	if( TRUE == m_bLoaded && FALSE == this->Unload() )	//先确保卸载
		return FALSE;

	m_mapValidTable.clear();

	ASSERT(pFileName);
	if( CDiscIOMgr::IsExist(pFileName) == FALSE )
		return FALSE;

	strcpy(m_szCPKFileName, pFileName);

	m_dwCPKFileHandle = CDiscIOMgr::Create(pFileName,
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE, // 非独占模式
		NULL, 
		OPEN_ALWAYS, 
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, 
		NULL);

	if (m_dwCPKFileHandle == INVALID_FILE_HANDLE )
	{
		ErrMsg(g_StrTable[_T("OpenFileFailed")], m_pUtil->AnsiToUnicode(pFileName));
		CDiscIOMgr::Close(m_dwCPKFileHandle);
		ASSERT(0);
		return FALSE;
	}

	// 读入文件头
	ZeroMemory(&m_CPKHeader, sizeof(m_CPKHeader));
	if( FALSE == CDiscIOMgr::Read(m_dwCPKFileHandle, &m_CPKHeader, sizeof(m_CPKHeader)) )
	{
		ErrMsg(g_StrTable[_T("ReadCPKHeadFailed")], m_pUtil->AnsiToUnicode(pFileName));
		return FALSE;
	}
	if (m_CPKHeader.dwLable != GT_CPK_LABEL)
	{
		ErrMsg(g_StrTable[_T("UnknowCPKFormat")], m_pUtil->AnsiToUnicode(pFileName));
		CDiscIOMgr::Close(m_dwCPKFileHandle);
		ASSERT(0);
		return FALSE;
	}


	// 读入文件Table
	// 我们看看表项空间够不够
	if( m_dwMaxCPKTable < m_CPKHeader.dwMaxTableNum )
	{
		SAFE_DEL_ARRAY(m_pCPKTable);
		m_pCPKTable = new tagCPKTable[m_CPKHeader.dwMaxTableNum];
		ZeroMemory(m_pCPKTable, sizeof(tagCPKTable)*m_CPKHeader.dwMaxTableNum);
		m_dwMaxCPKTable = m_CPKHeader.dwMaxTableNum;

		SAFE_DEL_ARRAY(m_pFragment);
		SAFE_DEL_ARRAY(m_pIndexToFragment);

		m_pFragment = new INT[m_dwMaxCPKTable];
		m_pIndexToFragment = new INT[m_dwMaxCPKTable];

		memset(m_pFragment, -1, sizeof(INT)*m_dwMaxCPKTable);
		memset(m_pIndexToFragment, -1, sizeof(INT)*m_dwMaxCPKTable);
	}

	//定位表项位置
	if( FALSE == CDiscIOMgr::Seek(m_dwCPKFileHandle, m_CPKHeader.dwTableStart) )
	{
		ErrMsg(g_StrTable[_T("CPKSeekError")],
			m_pUtil->AnsiToUnicode(pFileName), m_CPKHeader.dwTableStart);

		return FALSE;
	}

	if( FALSE == CDiscIOMgr::Read(m_dwCPKFileHandle, m_pCPKTable,  
		sizeof(tagCPKTable)*m_CPKHeader.dwMaxTableNum) )
	{
		ErrMsg(g_StrTable[_T("ReadCPKTableFailed")], m_pUtil->AnsiToUnicode(pFileName));
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
		ErrMsg(g_StrTable[_T("CreateFileMappingFailed")], m_pUtil->AnsiToUnicode(pFileName));
		CDiscIOMgr::Close(m_dwCPKFileHandle);
		m_dwCPKFileHandle = INVALID_FILE_HANDLE;

		return FALSE;
	}
	else
	{
		// 文件映射已经存在
		if( ::GetLastError() == ERROR_ALREADY_EXISTS )
		{
			ErrMsg(g_StrTable[_T("FileMappingAlreadyExist")], m_pUtil->AnsiToUnicode(pFileName));
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
//卸载cpk文件包
//-----------------------------------------------------------------------------
BOOL	CPKVfs::Unload()
{
	if (m_bLoaded)
	{
		CDiscIOMgr::CloseMappingHandle(m_dwCPKMappingHandle);
		m_dwCPKMappingHandle = 0;
		CDiscIOMgr::Close(m_dwCPKFileHandle);
		m_dwCPKFileHandle = INVALID_FILE_HANDLE;

		ZeroMemory(&m_CPKHeader, sizeof(m_CPKHeader));
		ZeroMemory(m_szCPKFileName, sizeof(m_szCPKFileName));
		ZeroMemory(m_pCPKTable, sizeof(tagCPKTable)*m_dwMaxCPKTable);

		memset(m_pFragment, 0xFF, sizeof(INT)*m_dwMaxCPKTable);
		memset(m_pIndexToFragment, 0xFF, sizeof(INT)*m_dwMaxCPKTable);

		m_mapValidTable.clear();

		m_bLoaded = FALSE;
	}

	return TRUE;
}


//-----------------------------------------------------------------------------
//增加表项空间
//-----------------------------------------------------------------------------
BOOL CPKVfs::IncreaseTable()
{
	if (m_CPKHeader.dwValidTableNum + 1 < m_CPKHeader.dwMaxTableNum)
		return TRUE;

	DWORD dwTable = m_CPKHeader.dwMaxTableNum + GT_CPK_TABLE_ADD;
	if( m_dwMaxCPKTable >= dwTable )
	{
		ZeroMemory(m_pCPKTable+m_CPKHeader.dwMaxTableNum, \
			sizeof(tagCPKTable)*(m_dwMaxCPKTable-m_CPKHeader.dwMaxTableNum));
	}
	else
	{
		//扩容
		tagCPKTable *pTable = new tagCPKTable[dwTable];
		memcpy(pTable, m_pCPKTable, sizeof(tagCPKTable)*m_CPKHeader.dwMaxTableNum);
		ZeroMemory(pTable+m_CPKHeader.dwMaxTableNum, \
			sizeof(tagCPKTable)*(dwTable-m_CPKHeader.dwMaxTableNum));

		SAFE_DEL_ARRAY(m_pCPKTable);
		m_pCPKTable = pTable;


		INT *p1 = new INT[dwTable];
		INT *p2 = new INT[dwTable];
		memset(p1, 0xFF, sizeof(INT)*dwTable);
		memset(p2, 0xFF, sizeof(INT)*dwTable);

		memcpy(p1, m_pFragment, sizeof(INT)*m_CPKHeader.dwMaxTableNum);
		memcpy(p2, m_pIndexToFragment, sizeof(INT)*m_CPKHeader.dwMaxTableNum);

		SAFE_DEL_ARRAY(m_pFragment);
		SAFE_DEL_ARRAY(m_pIndexToFragment);
		m_pFragment = p1;
		m_pIndexToFragment = p2;

		m_dwMaxCPKTable = dwTable;
	}

	m_CPKHeader.dwMaxTableNum = dwTable;
	m_CPKHeader.dwPackageSize += sizeof(tagCPKTable)*GT_CPK_TABLE_ADD;

	return TRUE;
}
//-----------------------------------------------------------------------------
// 在Load Package时调用一次以便得到所有碎片信息
//-----------------------------------------------------------------------------
INT CPKVfs::GetAllFragment()
{
	INT nFragment = 0;
	for( INT n=0; n<(INT)m_CPKHeader.dwMaxTableNum; n++ )
	{
		if( IS_FILE_REMOVED(m_pCPKTable[n].dwFlag) )	// 是碎片
		{
			m_pFragment[nFragment] = n;
			m_pIndexToFragment[n] = nFragment;
			nFragment++;
		}
		else
			m_pIndexToFragment[n] = GT_INVALID;
	}

	ASSERT((DWORD)nFragment == m_CPKHeader.dwFragmentNum);

	return nFragment;
}

//-----------------------------------------------------------------------------
//从crc得到对应的表项
//-----------------------------------------------------------------------------
INT		CPKVfs::GetTableIndexFromeCrc(DWORD dwCRC)
{
	// 在一堆相同的CRC中有效的CRC肯定在最后
	// 二分查找
	INT nStart = 0, nEnd = m_CPKHeader.dwValidTableNum;
	INT nPos = 0;

	while( nEnd != nStart )
	{
		nPos = nStart + (nEnd-nStart)/2;
		if( dwCRC == m_pCPKTable[nPos].dwCRC
			&& IS_FILE_OR_DIR_VALID(m_pCPKTable[nPos].dwFlag) ) 
			return nPos;

		if( nEnd == nStart+1 )
			return GT_INVALID;

		if( dwCRC >=  m_pCPKTable[nPos].dwCRC )
			nStart = nPos;
		else
			nEnd = nPos;
	}

	return GT_INVALID;
}


//-----------------------------------------------------------------------------
// 检测该文件名是否表示解包之后要打入cpk包，比如: data.cpk\system\gongfu.exe,
// 外部必须确保是文件,而不是目录，比如,data.cpk，这里无从知道他是否目录
//-----------------------------------------------------------------------------
BOOL CPKVfs::IsFileForCPK(LPCSTR szFullName, LPCSTR szCPKName, LPCSTR szFileName)
{
	if( NULL == szFullName ) 
		return FALSE;

	INT nLen = strlen(szFullName);
	for(int i=0; szFullName[i] != '\0' && i < 500; i++)
	{
		if( szFullName[i] != '\\' )
			continue;
		if( i >= 5 )
		{
			if( (szFullName[i-1] == 'k' || szFullName[i-1] == 'K') &&
				(szFullName[i-2] == 'p' || szFullName[i-2] == 'P') &&
				(szFullName[i-3] == 'c' || szFullName[i-1] == 'C') &&
				(szFullName[i-4] == '.') )
			{
				if( NULL != szCPKName )
				{
					memcpy((void*)szCPKName, (void*)szFullName, i);
					((char*)szCPKName)[i] = 0;
				}
				if( NULL != szFileName )
				{
					strcpy((char*)szFileName, szFullName+i+1);
					((char*)szFileName)[nLen-i-1] = 0;
				}

				return TRUE;
			}
		}
	}

	return FALSE;
}


//---------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------
DWORD CPKVfs::Open(INT nTableIndex, DWORD dwLength, BOOL bCpk )
{
	ASSERT( IS_VALID_CPK_TABLE(nTableIndex) );

	// 计算实际开始映射的位置
	DWORD dwRealStartPos = m_pCPKTable[nTableIndex].dwStartPos + dwLength;
	dwRealStartPos = dwRealStartPos - (dwRealStartPos % m_dwAllocGranularity);

	// 实际映射与需要映射位置的偏差
	DWORD dwOffset = m_pCPKTable[nTableIndex].dwStartPos + dwLength - dwRealStartPos;

	// 计算需要映射的字节
	DWORD dwBytesToRead = m_pCPKTable[nTableIndex].dwPackedSize
		+ m_pCPKTable[nTableIndex].dwExtraInfoSize + dwOffset;

	// 如果是大文件则分开映射,重新计算需要映射的字节
	dwBytesToRead -= dwLength;
	if(  IS_COMPRESSED( m_pCPKTable[nTableIndex].dwFlag ) == false
		&& IS_LARGE_FILE( m_pCPKTable[nTableIndex].dwFlag  ) )
	{
		if( dwBytesToRead >= GT_CPK_RAW_BUFFER_SIZE )
			dwBytesToRead = GT_CPK_RAW_BUFFER_SIZE + dwOffset;
	}

	// 创建文件对象
	CPKFile* fp = (CPKFile*)malloc(sizeof(CPKFile));
	if( !fp )
	{
		ErrMsg(g_StrTable[_T("NotEnoughMemoryForCreateFP")], m_pUtil->AnsiToUnicode(m_szCPKFileName));
		return 0;
	}
	ZeroMemory(fp->szFullName, sizeof(fp->szFullName));


	fp->bValid = TRUE;
	fp->lpMapAddress = NULL;
	// 映射
	fp->lpMapAddress = CDiscIOMgr::MapFile
		(m_dwCPKMappingHandle, FILE_MAP_READ, dwRealStartPos, dwBytesToRead);

	if( fp->lpMapAddress == NULL )
	{
		fp->bValid = FALSE;
		return 0;
	}

	// 填写fp基本信息
	fp->pTableItem = &m_pCPKTable[nTableIndex];
	fp->lpStartAddress = (LPVOID)((BYTE*)fp->lpMapAddress + dwOffset);
	fp->bCompressed = IS_COMPRESSED( m_pCPKTable[nTableIndex].dwFlag );
	fp->dwFileSize = m_pCPKTable[nTableIndex].dwOriginSize;
	fp->dwOffset = dwOffset;
	fp->dwPointer = 0;	// 文件指针初始
	fp->dwMapSize = dwBytesToRead - dwOffset;

	GetFullNameFromCRC(fp->pTableItem->dwCRC, fp->szFullName);

	ASSERT( fp->dwFileSize>= 0 && fp->dwFileSize<0x40000000 );

	// 如果是压缩文件，则需要先解压缩
	if( fp->bCompressed == TRUE && fp->dwFileSize > 0 && !bCpk )
	{
		// 为解压申请内存
		fp->lpMem = malloc(fp->dwFileSize);
		if( fp->lpMem == NULL )
		{
			CDiscIOMgr::UnMapFile(fp->lpMapAddress);
			fp->bValid = FALSE;
			return 0;
		}

		// 反过滤（解压）文件
		tagFilterParam param;
		param.bCompress = FALSE;
		param.bSuccess = FALSE;
		param.dwInBufferSize = m_pCPKTable[nTableIndex].dwPackedSize;
		param.dwInDataSize = m_pCPKTable[nTableIndex].dwPackedSize;
		param.dwOutBufferSize = m_pCPKTable[nTableIndex].dwOriginSize;
		param.dwOutDataSize = m_pCPKTable[nTableIndex].dwOriginSize;
		param.eFilter = GET_FILTER(m_pCPKTable[nTableIndex].dwFlag);
		param.lpMemIn = fp->lpStartAddress;
		param.lpMemOut = fp->lpMem;

		// 进行过滤文件信息
		CFilterMgr::Filter(&param);

		if( FALSE == param.bSuccess )
		{
			ErrMsg(g_StrTable[_T("DecompressFailed")]);
			CDiscIOMgr::UnMapFile(fp->lpMapAddress);
			free(fp->lpMem);
			fp->bValid = FALSE;
			return 0;
		}
	}
	else	// 如果文件没有被压缩过
	{
		// 将其指向Map中的文件实际起始地址
		fp->lpMem = fp->lpStartAddress;
	}

	//	::InterlockedIncrement((LONG*)&m_nOpenedFileNum);
	return (DWORD)fp;
}
//-----------------------------------------------------------------------------
// 定位包中单个文件的指针,返回当前文件指针位置
//-----------------------------------------------------------------------------
DWORD	CPKVfs::Seek(DWORD dwHandle, INT nOffset, DWORD dwOrigin)
{
	CPKFile* fp = (CPKFile*)dwHandle;
	ASSERT( fp != NULL );
	ASSERT( fp->bValid );

	switch( dwOrigin )
	{
	case GT_SEEK_SET:
		fp->dwPointer = nOffset;
		break;
	case GT_SEEK_CUR:
		fp->dwPointer += nOffset;
		break;
	case GT_SEEK_END:
		fp->dwPointer = fp->dwFileSize + nOffset;
		break;
	default:
		ASSERT(0);
		return (DWORD)GT_INVALID;		
	}

	return fp->dwPointer;
}
//-----------------------------------------------------------------------------
// 读包中单个文件的内容
//-----------------------------------------------------------------------------
DWORD	CPKVfs::Read(LPVOID lpMemOut, DWORD dwSize, DWORD dwHandle)
{
	CPKFile* fp = (CPKFile*)dwHandle;
	ASSERT(fp != NULL);
	ASSERT(fp->bValid);


	if( fp->dwPointer + dwSize > fp->dwFileSize )
		dwSize = fp->dwFileSize -  fp->dwPointer;

	memcpy(lpMemOut, (BYTE*)fp->lpMem + fp->dwPointer, dwSize);


	fp->dwPointer += dwSize;

	// 必须返回实际"成功"读取的字节数
	return dwSize;
}


//-----------------------------------------------------------------------------
// 关闭包中的单个文件, 并且取消文件映射，参数是文件序号
//-----------------------------------------------------------------------------
BOOL CPKVfs::Close(DWORD dwHandle)
{
	CPKFile* fp = (CPKFile*)dwHandle;
	ASSERT(fp != NULL);
	ASSERT(fp->bValid == TRUE);

	if( fp->lpMapAddress == 0 )
		return FALSE;

	if( fp->bCompressed && fp->dwFileSize > 0  && fp->lpStartAddress != fp->lpMem )
		free( fp->lpMem );

	BOOL bResult = CDiscIOMgr::UnMapFile(fp->lpMapAddress);
	if( FALSE == bResult )
	{
		fp->bValid = FALSE;
		return FALSE;
	}

	free(fp);
	return TRUE;
}


//---------------------------------------------------------------------------------
// 通过crc得到全路径文件名
//---------------------------------------------------------------------------------
BOOL CPKVfs::GetFullNameFromCRC(DWORD dwCRC, LPSTR szFullName)
{
	static CHAR szTempNameNew[MAX_PATH*2];
	static CHAR szTempNameOld[MAX_PATH*2];
	BOOL bResult = FALSE;

	ZeroMemory(szTempNameNew, sizeof(szTempNameNew));
	ZeroMemory(szTempNameOld, sizeof(szTempNameOld));

	if( dwCRC == 0 )	// 根目录
	{
		strcpy(szFullName, "");
		return TRUE;
	}

	//简单文件名
	if( FALSE == this->GetSimpleFileNameFromCRC(szTempNameOld, dwCRC) )
	{
		return FALSE;
	}

	INT nTableIndex = this->GetTableIndexFromeCrc(dwCRC);
	if( !IS_VALID_CPK_TABLE(nTableIndex) )
	{
		return FALSE;
	}

	dwCRC = (m_pCPKTable + nTableIndex)->dwFatherCRC;

	while(dwCRC != 0)
	{
		// 进行文件名拼接
		strcpy(szTempNameNew, "\\");
		strcat(szTempNameNew, szTempNameOld); // new: \abc

		this->GetSimpleFileNameFromCRC(szTempNameOld, dwCRC);
		strcat(szTempNameOld, szTempNameNew); // old: abc\abc

		if (strlen(szTempNameOld) >= MAX_PATH)
		{
			ErrMsg(g_StrTable[_T("TooLongFileName")], m_pUtil->AnsiToUnicode(szTempNameOld));
			return FALSE;
		}

		nTableIndex = this->GetTableIndexFromeCrc(dwCRC);
		ASSERT(nTableIndex != GT_INVALID);
		dwCRC = (m_pCPKTable + nTableIndex)->dwFatherCRC;

	}

	strcpy(szFullName, szTempNameOld);
	return TRUE;
}
//-----------------------------------------------------------------------------
// 由CRC标识得到简单文件名
//-----------------------------------------------------------------------------
BOOL CPKVfs::GetSimpleFileNameFromCRC(LPSTR pSimpleFileName, DWORD dwCRC)
{
	BOOL bResult = FALSE;
	ASSERT(pSimpleFileName);

	if (dwCRC == 0)
		return FALSE;

	INT nTableIndex = this->GetTableIndexFromeCrc(dwCRC);
	if( !IS_VALID_CPK_TABLE(nTableIndex) )
		return FALSE;

	// 从包中得到简单文件名(没有路径)
	static CHAR szTemp[MAX_PATH];

	tagCPKTable* pTableItem = m_pCPKTable + nTableIndex;
	if( FALSE == IS_FILE_OR_DIR_VALID(pTableItem->dwFlag) )
		return FALSE;

	// 读入文件附加信息，其中包含文件名信息
	DWORD dwReadPos = pTableItem->dwStartPos + pTableItem->dwPackedSize;
	DWORD dwBytesToRead = pTableItem->dwExtraInfoSize;

	CDiscIOMgr::Seek(m_dwCPKFileHandle, dwReadPos);
	CDiscIOMgr::Read(m_dwCPKFileHandle, szTemp, dwBytesToRead);

	strcpy(pSimpleFileName, szTemp);

	return TRUE;
}

//-----------------------------------------------------------------------------
// 检查是否是合法的CPK文件包
//-----------------------------------------------------------------------------
BOOL CPKVfs::IsValidCPK(LPCSTR pFileName)
{
	ASSERT(pFileName);
	if( CDiscIOMgr::IsExist(pFileName) == FALSE )
	{
		ErrMsg(g_StrTable[_T("FileNotFound")], m_pUtil->AnsiToUnicode(pFileName));
		return FALSE;
	}
	//然后我们判断扩展名是否是.cpk
	static CHAR szFullName[MAX_PATH];
	LPSTR lpExtName = NULL;

	strcpy(szFullName, pFileName);
	lpExtName = strrchr(szFullName, '.');

	if( !lpExtName )
		return FALSE;

	if( _stricmp(lpExtName, ".cpk") != 0 )
		return FALSE;

	DWORD dwHandle = CDiscIOMgr::Create(pFileName,
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL, 
		OPEN_EXISTING, 
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, 
		NULL);

	if (dwHandle == (DWORD)INVALID_HANDLE_VALUE )
	{
		ErrMsg(g_StrTable[_T("OpenFileFailed")], m_pUtil->AnsiToUnicode(pFileName));
		return FALSE;
	}

	// 读入文件头
	tagCPKHeader header;
	ZeroMemory(&header, sizeof(header));
	CDiscIOMgr::Read(dwHandle, &header, sizeof(header));
	CDiscIOMgr::Close(dwHandle);

	// 检查文件标志
	if( header.dwLable != GT_CPK_LABEL )
		return FALSE;


	// 检查文件版本号
	if( header.dwVersion != GT_CPK_VERSION )
		return FALSE;

	// 检查文件Table表起始位置
	if( header.dwTableStart == 0 )
		return FALSE;

	// 检查包中文件数
	if( header.dwFileNum > header.dwMaxTableNum )
		return FALSE;

	// 检查包中有效项个数
	if( header.dwValidTableNum > header.dwMaxTableNum )
		return FALSE;

	// 检查文件数与有效项数的关系
	if( header.dwFileNum > header.dwValidTableNum )
		return FALSE;

	// 以后版本必须检查文件项是否从小到大排序
	return TRUE;
}
//-----------------------------------------------------------------------------
//*********初始化
// 往包里的当前目录添加文件,这种方式比较特殊，它本身是从另外一个ｃｐｋ包里转移过来
//因此我们坚持一个原则:对文件内容不压缩(也许他已经压缩过),扩展信息不变
//*********初始化
//-----------------------------------------------------------------------------
INT CPKVfs::InitFileCPKToCPK(LPCSTR pFullName, DWORD dwFatherCRC, tagCPKTable *pTable)
{
	ASSERT(pFullName);


	// 是否还能放下文件
	if( FALSE == IncreaseTable() )
		return FALSE;

	// 将文件名拼上全路径名转化为小写再计算CRC
	CHAR szFullName[MAX_PATH];
	this->ConvertFullNameToCPKFullName(szFullName, pFullName, dwFatherCRC);

	// 还要处理有重复文件的情况
	DWORD dwCRC = m_pUtil->Crc32(szFullName);
	if( this->HaveRepeatCRC(dwCRC) == TRUE )
	{
		RemoveFile(dwCRC);
	}

	// 得到一个空闲的表项空间
	int nTableIndex = GetFreeTableItem(dwCRC);
	if (nTableIndex == GT_INVALID)
	{
		ErrMsg(g_StrTable[_T("FindFreeCPKItemFailed")]);
		return FALSE;
	}	

	// 计算信息总长度
	m_pCPKTable[nTableIndex].dwExtraInfoSize = pTable->dwExtraInfoSize;
	DWORD dwDataSize = pTable->dwPackedSize + m_pCPKTable[nTableIndex].dwExtraInfoSize;

	// 找到文件可插入数据的地方
	DWORD dwWritePos = 0;
	dwWritePos = this->FindCPKFreePlace(dwDataSize);

	// 填写数据表项Table
	m_pCPKTable[nTableIndex].dwCRC = dwCRC;
	m_pCPKTable[nTableIndex].dwFlag = pTable->dwFlag;
	m_pCPKTable[nTableIndex].dwFatherCRC = dwFatherCRC;
	m_pCPKTable[nTableIndex].dwOriginSize = pTable->dwOriginSize;
	m_pCPKTable[nTableIndex].dwPackedSize = pTable->dwPackedSize;
	m_pCPKTable[nTableIndex].dwStartPos = dwWritePos;

	return nTableIndex;

}
//-----------------------------------------------------------------------------
//*********写入数据
// 往包里的当前目录添加文件,这种方式比较特殊，它本身是从另外一个ｃｐｋ包里转移过来
//因此我们坚持一个原则:对文件内容不压缩(也许他已经压缩过),扩展信息不变
//*********写入数据
//-----------------------------------------------------------------------------
BOOL CPKVfs::WriteFileCPKToCPK(INT nTableIndex, DWORD dwWritePos, LPVOID p, DWORD dwDataSize)
{
	tagCPKTable *pTable = m_pCPKTable + nTableIndex;

	// 向文件中插入数据
	// 移动文件指针
	DWORD dwDistanceToMove = m_pCPKTable[nTableIndex].dwStartPos + dwWritePos;
	CDiscIOMgr::Seek(m_dwCPKFileHandle, dwDistanceToMove);
	DWORD dwDestFileSize = CDiscIOMgr::GetSize(m_dwCPKFileHandle);
	if (dwDistanceToMove >= dwDestFileSize)
		CDiscIOMgr::SetEOF(m_dwCPKFileHandle);

	// 写入实际信息
	return CDiscIOMgr::Write(m_dwCPKFileHandle, p, dwDataSize);
}
//-----------------------------------------------------------------------------
//*********结束添加文件，写入表和头部
// 往包里的当前目录添加文件,这种方式比较特殊，它本身是从另外一个ｃｐｋ包里转移过来
//因此我们坚持一个原则:对文件内容不压缩(也许他已经压缩过),扩展信息不变
//*********结束添加文件，写入表和头部
//-----------------------------------------------------------------------------
BOOL CPKVfs::FinishFileCPKToCPK(INT nTableIndex)
{
	// 文件计数加一
	m_CPKHeader.dwFileNum++;
	// 有效表项计数加一
	m_CPKHeader.dwValidTableNum++;


	// 将文件头、表项写入磁盘
	this->WriteHeaderToDisc();
	this->WriteTableToDisc();

	m_mapValidTable.insert(std::pair<DWORD, DWORD>(m_pCPKTable[nTableIndex].dwCRC, nTableIndex));
	return TRUE;
}
//-----------------------------------------------------------------------------
// 往包里的当前目录添加文件,这种方式比较特殊，它本身是从另外一个ｃｐｋ包里转移过来
//因此我们坚持一个原则:对文件内容不压缩(也许他已经压缩过),扩展信息不变
//-----------------------------------------------------------------------------
BOOL CPKVfs::AddFile(LPCSTR pFullName, DWORD dwFatherCRC, tagCPKTable *pTable, LPVOID p)
{
	ASSERT(pFullName);

	char *szName = (char *)p + pTable->dwPackedSize;

	// 是否还能放下文件
	if( FALSE == IncreaseTable() )
		return FALSE;

	// 将文件名拼上全路径名转化为小写再计算CRC
	CHAR szFullName[MAX_PATH];
	this->ConvertFullNameToCPKFullName(szFullName, pFullName, dwFatherCRC);

	// 还要处理有重复文件的情况
	DWORD dwCRC = m_pUtil->Crc32(szFullName);
	if( this->HaveRepeatCRC(dwCRC) == TRUE )
	{
		RemoveFile(dwCRC);
	}

	// 得到一个空闲的表项空间
	int nTableIndex = GetFreeTableItem(dwCRC);
	if (nTableIndex == GT_INVALID)
	{
		ErrMsg(g_StrTable[_T("FindFreeCPKItemFailed")]);
		return FALSE;
	}

	// 计算信息总长度
	m_pCPKTable[nTableIndex].dwExtraInfoSize = pTable->dwExtraInfoSize;
	DWORD dwDataSize = pTable->dwPackedSize + m_pCPKTable[nTableIndex].dwExtraInfoSize;

	// 找到文件可插入数据的地方
	DWORD dwWritePos = 0;
	dwWritePos = this->FindCPKFreePlace(dwDataSize);

	// 填写数据表项Table
	m_pCPKTable[nTableIndex].dwCRC = dwCRC;
	m_pCPKTable[nTableIndex].dwFlag = pTable->dwFlag;
	m_pCPKTable[nTableIndex].dwFatherCRC = dwFatherCRC;
	m_pCPKTable[nTableIndex].dwOriginSize = pTable->dwOriginSize;
	m_pCPKTable[nTableIndex].dwPackedSize = pTable->dwPackedSize;
	m_pCPKTable[nTableIndex].dwStartPos = dwWritePos;

	// 文件计数加一
	m_CPKHeader.dwFileNum++;
	// 有效表项计数加一
	m_CPKHeader.dwValidTableNum++;


	// 向文件中插入数据
	// 移动文件指针
	DWORD dwDistanceToMove = m_pCPKTable[nTableIndex].dwStartPos;
	CDiscIOMgr::Seek(m_dwCPKFileHandle, dwDistanceToMove);
	DWORD dwDestFileSize = CDiscIOMgr::GetSize(m_dwCPKFileHandle);
	if (dwDistanceToMove >= dwDestFileSize)
		CDiscIOMgr::SetEOF(m_dwCPKFileHandle);

	// 写入实际信息
	CDiscIOMgr::Write(m_dwCPKFileHandle, p, dwDataSize);			


	// 将文件头、表项写入磁盘
	this->WriteHeaderToDisc();
	this->WriteTableToDisc();

	m_mapValidTable.insert(std::pair<DWORD, DWORD>(dwCRC, nTableIndex));

	return TRUE;
}
//-----------------------------------------------------------------------------
// 往包里的当前目录添加文件
//-----------------------------------------------------------------------------
BOOL CPKVfs::AddFile(LPCSTR pFullName, DWORD dwFatherCRC)
{
	ASSERT(pFullName);

	EFMFilter eFilter = FMF_Null;
	if( FALSE == IncreaseTable() )
		return FALSE;

	// 将文件名拼上全路径名转化为小写再计算CRC
	CHAR szFullName[MAX_PATH];
	this->ConvertFullNameToCPKFullName(szFullName, pFullName, dwFatherCRC);

	// 还要处理有重复文件的情况
	DWORD dwCRC = m_pUtil->Crc32(szFullName);
	if( this->HaveRepeatCRC(dwCRC) == TRUE )
	{
		RemoveFile(dwCRC);
	}

	// 得到一个空闲的表项空间
	int nTableIndex = GetFreeTableItem(dwCRC);
	if (nTableIndex == GT_INVALID)
	{
		ErrMsg(g_StrTable[_T("FindFreeCPKItemFailed")]);
		return FALSE;
	}

	// 打开源文件
	DWORD dwSrcHandle = CDiscIOMgr::Create(pFullName,
		GENERIC_READ,
		FILE_SHARE_READ, // 独占模式
		NULL, 
		OPEN_EXISTING, 
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, 
		NULL);

	if (dwSrcHandle == (DWORD)INVALID_HANDLE_VALUE )
	{
		ErrMsg(g_StrTable[_T("OpenFileFailed")], m_pUtil->AnsiToUnicode(pFullName));
		return FALSE;
	}


	// 检查文件长度
	BOOL bResult = FALSE;
	DWORD dwSrcFileSize = CDiscIOMgr::GetSize(dwSrcHandle);
	DWORD dwFileFlag = 0;
	VOID* pSrc = NULL;
	VOID* pDest = NULL;
	pSrc = malloc(dwSrcFileSize);
	pDest = malloc(dwSrcFileSize);

	if( dwSrcFileSize >= GT_CPK_RAW_BUFFER_SIZE 
		|| IsUnCompressable(pFullName) )	// mp3/bink donot take compress
	{
		// 将大文件读到内存
		eFilter = FMF_Changeless;
		dwFileFlag = 0x1 | (eFilter<<16) | 0x4;	// 有效 | 不压缩 | 大文件
		bResult = CDiscIOMgr::_LoadToMem(pSrc, dwSrcHandle, dwSrcFileSize);
		if( bResult == FALSE )
		{
			ErrMsg(g_StrTable[_T("ReadFileFailed")], m_pUtil->AnsiToUnicode(pFullName));
			CDiscIOMgr::Close(dwSrcHandle);
			free(pSrc);
			free(pDest);

			return FALSE;
		}
	}
	else
	{
		eFilter = FMF_MiniLzo1040;
		dwFileFlag = 0x1 | (eFilter<<16); // 有效 | LZO压缩

		// 将源文件读入内存
		bResult = CDiscIOMgr::_LoadToMem(pSrc, dwSrcHandle, dwSrcFileSize);
		if (bResult == FALSE)
		{
			ErrMsg(g_StrTable[_T("ReadFileFailed")], m_pUtil->AnsiToUnicode(pFullName));
			CDiscIOMgr::Close(dwSrcHandle);
			free(pSrc);
			free(pDest);
			return FALSE;
		}
	}


	// 填写过滤器所需参数
	tagFilterParam param;
	param.eFilter = eFilter;
	param.bCompress = TRUE;
	param.lpMemIn = pSrc;
	param.dwInBufferSize = dwSrcFileSize;
	param.lpMemOut = pDest;
	param.dwOutBufferSize = dwSrcFileSize;
	param.dwInDataSize = dwSrcFileSize;

	// 过滤文件
	CFilterMgr::Filter(&param);
	if (param.bSuccess == FALSE)
	{
		free(pSrc);
		free(pDest);
		return FALSE;
	}

	// 计算信息总长度
	m_pCPKTable[nTableIndex].dwExtraInfoSize = this->CalFileExtraInfoSize(pFullName);
	DWORD dwDataSize = param.dwOutDataSize + m_pCPKTable[nTableIndex].dwExtraInfoSize;

	// 找到文件可插入数据的地方
	DWORD dwWritePos = 0;
	dwWritePos = this->FindCPKFreePlace(dwDataSize);

	// 填写数据表项Table
	m_pCPKTable[nTableIndex].dwCRC = dwCRC;
	m_pCPKTable[nTableIndex].dwFlag = dwFileFlag;
	m_pCPKTable[nTableIndex].dwFatherCRC = dwFatherCRC;
	m_pCPKTable[nTableIndex].dwOriginSize = param.dwInDataSize;
	m_pCPKTable[nTableIndex].dwPackedSize = param.dwOutDataSize;
	m_pCPKTable[nTableIndex].dwStartPos = dwWritePos;

	// 文件计数加一
	m_CPKHeader.dwFileNum++;
	// 有效表项计数加一
	m_CPKHeader.dwValidTableNum++;


	// 向文件中插入数据
	if( (EFMFilter)(m_pCPKTable[nTableIndex].dwFlag >> 16) == FMF_Changeless )
	{//不压缩
		bResult = AddRealDataToCPK(nTableIndex, pFullName, pSrc, dwSrcFileSize);
	}
	else
	{//已经压缩过
		bResult = AddRealDataToCPK(nTableIndex, pFullName, pDest, \
			m_pCPKTable[nTableIndex].dwPackedSize);
	}
	if (bResult == FALSE)
	{
		ErrMsg(g_StrTable[_T("FailInAddFileToCPK")], m_pUtil->AnsiToUnicode(pFullName));
		ASSERT(0);
		CDiscIOMgr::Close(dwSrcHandle);
		free(pSrc);
		free(pDest);
		return FALSE;
	}
	// 将文件头、表项写入磁盘
	this->WriteHeaderToDisc();
	this->WriteTableToDisc();
	m_mapValidTable.insert(std::pair<DWORD, DWORD>(dwCRC, nTableIndex));


	free(pSrc);
	free(pDest);

	CDiscIOMgr::Close(dwSrcHandle);

	return TRUE;

}
//-----------------------------------------------------------------------------
// 删除包里的文件或者目录，使用完整路径
//-----------------------------------------------------------------------------
BOOL CPKVfs::RemoveFile(DWORD dwCRC)
{
	// 找到包中的文件Table项
	INT nTableIndex = this->GetTableIndexFromeCrc(dwCRC);
	if( nTableIndex == GT_INVALID )
	{
		return FALSE;
	}

	// 进入递归调用函数
	this->RemoveFileOrDir(nTableIndex);

	// 将Header\Table存到包中
	this->WriteHeaderToDisc();
	this->WriteTableToDisc();

	return TRUE;
}
//-----------------------------------------------------------------------------
// 由磁盘全名转为包中文件名，必须知道父CRC, 并全部转为小写
//-----------------------------------------------------------------------------
BOOL CPKVfs::ConvertFullNameToCPKFullName(LPSTR pOutName, LPCSTR pInName, 
										  DWORD dwFatherCRC)
{
	ASSERT(pOutName);
	ASSERT(pInName);
	BOOL bResult = FALSE;

	static CHAR szSimpleName[MAX_PATH];
	static CHAR szCPKFullName[MAX_PATH];

	bResult = this->GetFullNameFromCRC(dwFatherCRC, szCPKFullName);

	ASSERT(bResult);

	if (FALSE == bResult)
		return FALSE;

	bResult = this->GetSimpleFileName(szSimpleName, pInName);
	ASSERT(bResult);
	if (FALSE == bResult)
		return FALSE;

	if (strlen(szCPKFullName) + strlen(szSimpleName) > MAX_PATH)
	{
		ErrMsg(g_StrTable[_T("TooLongFileName")], m_pUtil->AnsiToUnicode(szSimpleName));
		return FALSE;
	}

	// 下面的函数会自动全部转为小写
	this->LinkSimpleNameToDirName(szCPKFullName, szSimpleName);
	strcpy(pOutName, szCPKFullName);
	return TRUE;
}


//-----------------------------------------------------------------------------
// 连接简单文件名或目录名到目标目录名
//-----------------------------------------------------------------------------
BOOL CPKVfs::LinkSimpleNameToDirName(LPSTR pFullName, LPCSTR pSimpleName)
{
	ASSERT(pFullName);
	ASSERT(pSimpleName);

	INT nFullNameLen = strlen(pFullName);

	// 粗略检查字符串长度是否能够容纳
	if( nFullNameLen >= MAX_PATH - 10 )
	{
		ErrMsg(g_StrTable[_T("TooLongFileName")], m_pUtil->AnsiToUnicode(pFullName));
		return FALSE;
	}

	// 先查找目录名后有没有"\"符号
	if( nFullNameLen>0 && pFullName[nFullNameLen-1] != '\\' )
		strcat(pFullName, "\\");

	strcat(pFullName, pSimpleName);
	_strlwr(pFullName);	// 转成小写
	return TRUE;
}

//-----------------------------------------------------------------------------
// 检查是否包中有与所给CRC重复的CRC
//-----------------------------------------------------------------------------
BOOL CPKVfs::HaveRepeatCRC(DWORD dwCRC)
{
	std::map<DWORD, DWORD>::iterator it;
	it = m_mapValidTable.find(dwCRC);
	if( it != m_mapValidTable.end() )
	{
		return TRUE;
	}
	return FALSE;			
}
//-----------------------------------------------------------------------------
// 在包里指定目录创建一个空目录, 返回CRC
//-----------------------------------------------------------------------------
DWORD CPKVfs::CreateDir(LPCSTR pSimpleDirName, DWORD dwFatherCRC)
{
	ASSERT(pSimpleDirName);

	if( FALSE == IncreaseTable() )
		return FALSE;

	// 将目录名转化为小写再计算CRC
	CHAR szFullDirName[MAX_PATH];
	this->GetFullNameFromCRC(dwFatherCRC, szFullDirName);
	this->LinkSimpleNameToDirName(szFullDirName, pSimpleDirName);
	DWORD dwCRC = m_pUtil->Crc32(szFullDirName);

	if( this->HaveRepeatCRC(dwCRC) == TRUE )
	{
		//该目录已经存在
		return dwCRC;
	}


	// 得到一个空闲的表项空间
	int nTableIndex = GetFreeTableItem(dwCRC);
	if (nTableIndex == GT_INVALID)
	{
		ErrMsg(g_StrTable[_T("FindFreeCPKItemFailed")]);
		return GT_INVALID;
	}

	// 计算信息总长度
	m_pCPKTable[nTableIndex].dwExtraInfoSize = 
		this->CalFileExtraInfoSize(pSimpleDirName);	

	DWORD dwDataSize = m_pCPKTable[nTableIndex].dwExtraInfoSize;

	// 找到文件可插入数据的地方
	DWORD dwWritePos = 0;
	dwWritePos = this->FindCPKFreePlace(dwDataSize);

	// 填写数据表项Table
	m_pCPKTable[nTableIndex].dwCRC = dwCRC;
	m_pCPKTable[nTableIndex].dwFlag = 0x1 | 0x2;	// 目录
	m_pCPKTable[nTableIndex].dwFatherCRC = dwFatherCRC;
	m_pCPKTable[nTableIndex].dwOriginSize = 0;
	m_pCPKTable[nTableIndex].dwPackedSize = 0;
	m_pCPKTable[nTableIndex].dwStartPos = dwWritePos;

	// 文件计数加一
	m_CPKHeader.dwFileNum++;
	// 有效表项计数加一
	m_CPKHeader.dwValidTableNum++;


	// 向文件中插入数据
	this->AddRealDataToCPK(nTableIndex, pSimpleDirName, NULL, 0);
	// 将文件头、表项写入磁盘
	this->WriteHeaderToDisc();
	this->WriteTableToDisc();

	m_mapValidTable.insert(std::pair<DWORD, DWORD>(dwCRC, nTableIndex));

	return dwCRC;

}

//-----------------------------------------------------------------------------
//这种方式不添加文件，只添加他的路径，根路径作为dwFatherCrc的子目录．为其它操作做准备
//-----------------------------------------------------------------------------
DWORD CPKVfs::AddDirCyc(LPCSTR pFullName, DWORD dwFatherCRC)
{
	ASSERT(pFullName);
	//首先一步一步剥离路径,比如c:\bin\tidy, c:\, c:\bin\tidy

	CHAR szPathFile[MAX_PATH];
	INT i=0, j=0;
	for(i=0, j=0; pFullName[i] != '\0'; i++)
	{
		if( pFullName[i] == '.' )
		{ //文件，我们自动忽略
			return dwFatherCRC;
		}

		if( pFullName[i] == '\\' && i > 0 )
		{
			ZeroMemory(szPathFile, sizeof(szPathFile));
			strncpy(szPathFile, pFullName+j, i-j);
			dwFatherCRC = CreateDir(szPathFile, dwFatherCRC);

			j = i+1;
		}
	}

	ZeroMemory(szPathFile, sizeof(szPathFile));
	strncpy(szPathFile, pFullName+j, i-j);
	return CreateDir(szPathFile, dwFatherCRC);
}

//-----------------------------------------------------------------------------
// 向文件中插入数据
//-----------------------------------------------------------------------------
BOOL CPKVfs::AddRealDataToCPK(INT nTableIndex, LPCSTR pFileName,  VOID* pSrc, 
							  DWORD dwSrcFileSize)
{
	ASSERT(pFileName);
	ASSERT(IS_VALID_CPK_TABLE(nTableIndex));
	ASSERT(!IS_INVALID_HANDLE(m_dwCPKFileHandle));

	static CHAR szFileName[MAX_PATH];

	BOOL bResult = FALSE;
	DWORD dwDistanceToMove = 0;
	DWORD dwBytesToWrite = 0;

	static CHAR szSimpleName[MAX_PATH];
	this->GetSimpleFileName(szSimpleName, pFileName);
	dwBytesToWrite = (DWORD)strlen(szSimpleName);

	// 移动文件指针
	dwDistanceToMove = m_pCPKTable[nTableIndex].dwStartPos;
	CDiscIOMgr::Seek(m_dwCPKFileHandle, dwDistanceToMove);
	DWORD dwDestFileSize = CDiscIOMgr::GetSize(m_dwCPKFileHandle);
	if (dwDistanceToMove >= dwDestFileSize)
		CDiscIOMgr::SetEOF(m_dwCPKFileHandle);

	//计算实际信息的md5
	tagFileExtraInfo ExtraInfo;

	// 写入实际信息
	if( m_pCPKTable[nTableIndex].dwPackedSize > 0 )
	{
		CMD5::MD5ForBuffer((BYTE*)pSrc, dwSrcFileSize, ExtraInfo.byMD5);
		CDiscIOMgr::Write(m_dwCPKFileHandle, pSrc, dwSrcFileSize);			
	}
	else
	{ //没有文件内容,没关系,我们计算名字的md5
		CMD5::MD5ForString((char*)szSimpleName, ExtraInfo.byMD5);
	}


	// 写入附加信息
	CDiscIOMgr::Write(m_dwCPKFileHandle, szSimpleName, dwBytesToWrite);

	// 在文件名后加两字节结束符号, 为与Unicode兼容
	CDiscIOMgr::Write(m_dwCPKFileHandle, _T("\0\0\0\0"), GT_CPK_LEN_OF_STRING_END);

	dwBytesToWrite = sizeof(ExtraInfo);
	CDiscIOMgr::Write(m_dwCPKFileHandle, &ExtraInfo, dwBytesToWrite);

	return TRUE;
}


//-----------------------------------------------------------------------------
// 计算附加信息长度
//-----------------------------------------------------------------------------
DWORD CPKVfs::CalFileExtraInfoSize(LPCSTR pFileName)
{
	DWORD dwSize = 0;
	dwSize += strlen(pFileName) + GT_CPK_LEN_OF_STRING_END;
	dwSize += sizeof(tagFileExtraInfo);
	return dwSize;
}


//-----------------------------------------------------------------------------
// 根据所给的CRC得到空闲的Table项
//-----------------------------------------------------------------------------
INT	CPKVfs::GetFreeTableItem(DWORD dwCRC)
{
	/*
	if( m_CPKHeader.dwValidTableNum >= m_CPKHeader.dwMaxTableNum -1 )
	{
	ASSERT(0);

	// TODO: 需要将无空间碎片或一部分小型空间碎片的项转成空闲项
	return GT_INVALID;
	}

	for(INT n=0; n<m_CPKHeader.dwMaxTableNum; n++)
	{
	if( IS_FILE_OR_DIR_VALID(m_pCPKTable[n].dwFlag) )
	continue;
	return n;
	}/**/

	if( m_CPKHeader.dwValidTableNum >= m_CPKHeader.dwMaxTableNum -1 )
	{
		ASSERT(0);

		// TODO: 需要将无空间碎片或一部分小型空间碎片的项转成空闲项
		return GT_INVALID;
	}

	// 逻辑确认有效表项的最后是否是空位
	ASSERT( !IS_TABLE_VALID(m_pCPKTable[m_CPKHeader.dwValidTableNum].dwFlag) );

	// 在有效表项中找到合适的插入点
	for( INT n=0; (DWORD)n<m_CPKHeader.dwValidTableNum; n++ )
	{
		if( dwCRC < m_pCPKTable[n].dwCRC )
		{
			// 将其后的Table向后移动
			for( INT m=m_CPKHeader.dwValidTableNum; m>n; m-- )
			{
				memcpy(&m_pCPKTable[m], &m_pCPKTable[m-1], sizeof(tagCPKTable));

				// 检查是否是碎片，如果是，还要处理碎片信息的更动
				if( m_pIndexToFragment[m-1] != GT_INVALID )
				{
					m_pFragment[m_pIndexToFragment[m-1]] = m;
					m_pIndexToFragment[m] = m_pIndexToFragment[m-1];
					m_pIndexToFragment[m-1] = GT_INVALID;
				}
			}

			// ZeroMemory(&m_pCPKTable[n], sizeof(tagCPKTable)); 不需要
			return n;
		}
	}

	// 所给的CRC是最大的CRC，所以放在最后
	// ZeroMemory(&m_pCPKTable[m_CPKHeader.dwValidTableNum], sizeof(tagCPKTable)); 不需要
	return (INT)m_CPKHeader.dwValidTableNum;

}
//-----------------------------------------------------------------------------
// 得到CPK中可以放指定大小内容的空间起始点
//-----------------------------------------------------------------------------
DWORD CPKVfs::FindCPKFreePlace(DWORD dwSize)
{
	INT nFrag = GT_INVALID;
	DWORD dwPos = 0;

	// 先检查有没有能容纳dwSize的空间碎片
	for( INT n=0; (DWORD)n<m_CPKHeader.dwFragmentNum; n++ )
	{
		nFrag = m_pFragment[n];

		ASSERT( IS_VALID_CPK_TABLE(nFrag) );
		ASSERT( IS_FILE_REMOVED(m_pCPKTable[nFrag].dwFlag) );	// 确认是碎片

		if( m_pCPKTable[nFrag].dwPackedSize >= dwSize )
		{
			dwPos = m_pCPKTable[nFrag].dwStartPos;

			m_pCPKTable[nFrag].dwStartPos += dwSize;
			m_pCPKTable[nFrag].dwPackedSize -= dwSize;	// 碎片变小
			return dwPos;
		}
	}

	// 如果没有则直接附加到包的table文件之前
	DWORD dwPackSize = m_CPKHeader.dwPackageSize;
	m_CPKHeader.dwPackageSize += dwSize;
	m_CPKHeader.dwTableStart = m_CPKHeader.dwPackageSize - \
		sizeof(tagCPKTable) * m_CPKHeader.dwMaxTableNum;

	return dwPackSize - sizeof(tagCPKTable)*m_CPKHeader.dwMaxTableNum;

}
//-----------------------------------------------------------------------------
// 删除文件或目录，会删除其下所有子项
// 可优化
//-----------------------------------------------------------------------------
BOOL CPKVfs::RemoveFileOrDir(INT nTableIndex)
{
	ASSERT( IS_VALID_CPK_TABLE(nTableIndex) );

	BOOL bDir = IS_DIR( m_pCPKTable[nTableIndex].dwFlag );
	DWORD dwCRC = m_pCPKTable[nTableIndex].dwCRC;

	m_mapValidTable.erase(m_pCPKTable[nTableIndex].dwCRC);
	// 标志文件为空间碎片
	SET_FILE_REMOVED(m_pCPKTable[nTableIndex].dwFlag);

	// 逻辑确认
	ASSERT(m_CPKHeader.dwFragmentNum < m_CPKHeader.dwMaxTableNum);
	ASSERT(m_CPKHeader.dwFragmentNum == 
		m_CPKHeader.dwValidTableNum - m_CPKHeader.dwFileNum);

	// 记录碎片
	m_pFragment[m_CPKHeader.dwFragmentNum] = nTableIndex;
	m_pIndexToFragment[nTableIndex] = m_CPKHeader.dwFragmentNum;

	m_CPKHeader.dwFileNum--; // 文件个数减一
	m_CPKHeader.dwFragmentNum++;	// 碎片数目加一

	// 对碎片进行处理
	// 将碎片的所有空间记录到dwPackSize
	m_pCPKTable[nTableIndex].dwPackedSize += m_pCPKTable[nTableIndex].dwExtraInfoSize;
	m_pCPKTable[nTableIndex].dwExtraInfoSize = 0;
	m_pCPKTable[nTableIndex].dwFatherCRC = 0;

	if( FALSE == bDir )
	{
		return TRUE;	//直接返回
	}

	// 寻找子节点并删除，因为并未按照父节点排序，所以只能顺序查找
	for( INT n=0; n<(INT)m_CPKHeader.dwValidTableNum; n++ )
	{
		if( FALSE == IS_FILE_OR_DIR_VALID(m_pCPKTable[n].dwFlag) )
			continue;

		if( m_pCPKTable[n].dwFatherCRC == dwCRC )
		{
			// 递归删除
			RemoveFileOrDir(n);	
		}
	}

	return TRUE;
}
//-----------------------------------------------------------------------------
// 将文件头写入磁盘包
//-----------------------------------------------------------------------------
BOOL CPKVfs::WriteHeaderToDisc()
{

	// 将CPK Header写入

	DWORD dwDistanceToMove = 0; 
	CDiscIOMgr::Seek(m_dwCPKFileHandle, dwDistanceToMove);
	DWORD dwBytesToWrite = sizeof(tagCPKHeader);
	CDiscIOMgr::Write(m_dwCPKFileHandle, &m_CPKHeader, dwBytesToWrite);

	return TRUE;

}

//-----------------------------------------------------------------------------
// 将文件表写入磁盘包
//-----------------------------------------------------------------------------
BOOL CPKVfs::WriteTableToDisc()
{
	// 将Table写入
	DWORD dwDistanceToMove = m_CPKHeader.dwTableStart; 

	CDiscIOMgr::Seek(m_dwCPKFileHandle, dwDistanceToMove);
	DWORD dwBytesToWrite = sizeof(tagCPKTable) * m_CPKHeader.dwMaxTableNum;
	CDiscIOMgr::Write(m_dwCPKFileHandle, m_pCPKTable, dwBytesToWrite);

	return TRUE;
}


//-----------------------------------------------------------------------------
// 由任意文件名得到简单文件名(不带路径)
//-----------------------------------------------------------------------------
BOOL CPKVfs::GetSimpleFileName(LPSTR pSimpleName, LPCSTR pFullName)
{
	ASSERT(pSimpleName);
	ASSERT(pFullName);

	LPCSTR lpSimpleName = strrchr(pFullName, '\\');
	if (NULL == lpSimpleName)
	{
		strcpy(pSimpleName, pFullName);
	}
	else
	{
		strcpy(pSimpleName, lpSimpleName+1);
	}
	return TRUE;
}


//-----------------------------------------------------------------------------
// 是否是不可压缩文件  mp3/bik
//-----------------------------------------------------------------------------
BOOL CPKVfs::IsUnCompressable(LPCSTR pFullName)
{
	static CHAR szFullName[MAX_PATH];
	LPSTR lpExtName = NULL;

	strcpy(szFullName, pFullName);
	lpExtName = strrchr(szFullName, _T('.'));

	if( !lpExtName )
		return FALSE;

	if( _stricmp(lpExtName, ".mp3") == 0 || _stricmp(lpExtName, ".bik") == 0 )
		return TRUE;
	else
		return FALSE;
}


//-----------------------------------------------------------------------------
// 得到包中文件的扩展名(带"."号)
//-----------------------------------------------------------------------------
BOOL CPKVfs::GetFileExtName(LPTSTR pExtName, INT nTableIndex)
{
	ASSERT(IS_VALID_CPK_TABLE(nTableIndex));
	ASSERT(pExtName);

	static TCHAR szTemp[MAX_PATH];
	static TCHAR szFullName[MAX_PATH];
	LPTSTR lpExtName = NULL;

	tagCPKTable* pTableItem = m_pCPKTable+nTableIndex;

	if( FALSE == IS_FILE_OR_DIR_VALID(pTableItem->dwFlag) )
	{
		ErrMsg(g_StrTable[_T("InvalidTableIndex")], nTableIndex);
		return FALSE;
	}

	// table item 可能指向一个目录
	if (TRUE == IS_DIR(pTableItem->dwFlag))
	{
		lstrcpy(pExtName, _T(""));
		return TRUE;
	}

	// 读入文件附加信息，其中包含文件名信息
	DWORD dwReadPos = pTableItem->dwStartPos + pTableItem->dwPackedSize;
	DWORD dwBytesToRead = pTableItem->dwExtraInfoSize;

	CDiscIOMgr::Seek(m_dwCPKFileHandle, dwReadPos);
	CDiscIOMgr::Read(m_dwCPKFileHandle, szTemp, dwBytesToRead);

	// 寻找扩展名
	ZeroMemory(szFullName, sizeof(szFullName));
	lstrcpy(szFullName, szTemp);
	lpExtName = _tcsrchr(szFullName, _T('.'));
	if (NULL == lpExtName)
		lstrcpy(pExtName, _T(""));
	else
		lstrcpy(pExtName, lpExtName);

	return TRUE;
}
//-----------------------------------------------------------------------------
// 得到包中指定目录的所有一级子文件
// 将结果输出到pdwCRCArray所指向的双字数组
// 如果pdwCRCArray==NULL, 则不输出CRC，只计算并输出nChildNum
//-----------------------------------------------------------------------------
BOOL CPKVfs::GetFirstLevelChild(DWORD dwDirCRC, DWORD& dwChildNum, DWORD* pdwCRCArray)
{
	INT n = 0;
	DWORD dwCounter = 0;

	for (n=0; n<(INT)m_CPKHeader.dwValidTableNum; n++)
	{
		// 非子对象
		if( dwDirCRC != m_pCPKTable[n].dwFatherCRC )
			continue;

		// 非法
		if( FALSE == IS_FILE_OR_DIR_VALID(m_pCPKTable[n].dwFlag) )
			continue;

		// 是目录
		if( IS_DIR(m_pCPKTable[n].dwFlag) )
			continue;

		if (pdwCRCArray)
			pdwCRCArray[dwCounter] = m_pCPKTable[n].dwCRC;

		dwCounter++;
	}

	dwChildNum = dwCounter;
	return TRUE;
}
//-----------------------------------------------------------------------------
// 得到包中指定目录下的所有一级子目录
// 将结果输出到pdwCRCArray所指向的双字数组
// 如果pdwCRCArray==NULL, 则不输出CRC，只计算并输出nChildNum
//-----------------------------------------------------------------------------
BOOL CPKVfs::GetFirstLevelChildDir(DWORD dwDirCRC, DWORD& dwChildNum, 
								   DWORD* pdwCRCArray)
{
	INT n = 0;
	DWORD dwCounter = 0;

	for( n=0; n<(INT)m_CPKHeader.dwValidTableNum; n++ )
	{
		// 非目录
		if( FALSE == IS_DIR(m_pCPKTable[n].dwFlag) )
			continue;

		// 非子对象
		if( dwDirCRC != m_pCPKTable[n].dwFatherCRC )
			continue;

		// 非法
		if( FALSE == IS_FILE_OR_DIR_VALID(m_pCPKTable[n].dwFlag) )
			continue;

		if( pdwCRCArray )
			pdwCRCArray[dwCounter] = m_pCPKTable[n].dwCRC;

		dwCounter++;	// 必须单独出来
	}

	dwChildNum = dwCounter;
	return TRUE;
}
//-----------------------------------------------------------------------------
// 由CRC标识得到对象是否是目录
//-----------------------------------------------------------------------------
BOOL CPKVfs::IsDir(DWORD dwCRC)
{

	if( dwCRC == 0 )	// 零代表根目录
		return TRUE;

	// 由CRC找到对应表项
	INT n = GetTableIndexFromeCrc(dwCRC);

	if( IS_DIR((m_pCPKTable+n)->dwFlag) )
		return TRUE;

	return FALSE;
}
//-----------------------------------------------------------------------------