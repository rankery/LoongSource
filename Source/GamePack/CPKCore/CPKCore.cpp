//-----------------------------------------------------------------------------
// File: CPKCore.cpp
// Desc: game resource file system 2.0
// Time: 2003-5-12
// Auth: CTCRST
//
// Copyright (c) 2003 Softstar Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "cpkcore.h"

#include "Util.h"
#include "disciomgr.h"
#include "..\memmgr\memmgr.h"
#include "..\filtermgr\filtermgr.h"
#include "..\md5\md5.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static TCHAR THIS_FILE[] = __FILE__;
#endif

#define   COMPRESS_SIZE				50 * 1024 * 1024

//-----------------------------------------------------------------------------
// CPK construction / destruction
//-----------------------------------------------------------------------------
CPK::CPK()
{
	ZeroMemory(this, sizeof(*this));
	
	Reset();

	m_pRawBuffer = CMemMgr::Alloc(GT_CPK_RAW_BUFFER_SIZE);
	
	
	m_dwPackBufferSize = COMPRESS_SIZE + 2 * 1024 * 1024;;
	m_pPackBuffer = CMemMgr::Alloc(m_dwPackBufferSize );
	m_dwRawBufferSize = GT_CPK_RAW_BUFFER_SIZE;
	

	m_pMD5 = new CMD5;
	m_dwAllocGranularity = this->GetAllocationGranularity();

	m_dwMaxCPKTable = GT_CPK_DEFAULT_FILE;
	m_pCPKTable = new tagCPKTable[m_dwMaxCPKTable];
	ZeroMemory(m_pCPKTable, sizeof(tagCPKTable)*m_dwMaxCPKTable);
	m_pFragment = new INT[m_dwMaxCPKTable];
	m_pIndexToFragment = new INT[m_dwMaxCPKTable];

	// 记录碎片所属的Table项
	memset(m_pFragment, -1, sizeof(INT)*m_dwMaxCPKTable);	
	// 记录Table所属的碎片项
	memset(m_pIndexToFragment, -1, sizeof(INT)*m_dwMaxCPKTable);

}

CPK::~CPK() 
{
	CMemMgr::Free(m_pRawBuffer);
	CMemMgr::Free(m_pPackBuffer);
	m_dwRawBufferSize = 0;
	m_dwPackBufferSize = 0;

	FREE(m_pMD5);
	FREEARR(m_pCPKTable);
	FREEARR(m_pFragment);
	FREEARR(m_pIndexToFragment);
}


VOID CPK::Reset()
{
	ZeroMemory(m_szCPKFileName, sizeof(m_szCPKFileName));
	ZeroMemory(m_szCurDir, sizeof(m_szCurDir));

	
	ZeroMemory(m_pCPKTable, sizeof(tagCPKTable)*m_dwMaxCPKTable);
	// 记录碎片所属的Table项
	memset(m_pFragment, -1, sizeof(INT)*m_dwMaxCPKTable);	
	// 记录Table所属的碎片项
	memset(m_pIndexToFragment, -1, sizeof(INT)*m_dwMaxCPKTable);

	m_dwCurDirCRC = 0;
	m_dwCPKHeaderAddr = 0;

}


//-----------------------------------------------------------------------------
// 创建新的压缩包
//-----------------------------------------------------------------------------
BOOL CPK::Create(LPCTSTR pFileName)
{
	ASSERT(pFileName);


	if( m_bLoaded )	// 必须先释放再载入
		this->Unload();
	else 
		this->Reset();

	// 如果包存在则自动打开包
	if (CDiscIOMgr::IsExist(pFileName) == TRUE)
	{
		this->Load(pFileName);
		return TRUE;
	}

	lstrcpy(m_szCPKFileName, pFileName);

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
		sizeof(tagCPKTable) * m_CPKHeader.dwMaxTableNum;


	m_dwCPKHandle = CDiscIOMgr::Create(pFileName,
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
	
	CDiscIOMgr::Write(m_dwCPKHandle, &m_CPKHeader, sizeof(m_CPKHeader));

	// 将空的Table写入CPK
	ZeroMemory(m_pCPKTable, sizeof(tagCPKTable)*m_dwMaxCPKTable);
	CDiscIOMgr::Write(m_dwCPKHandle, m_pCPKTable, 
		sizeof(tagCPKTable)*m_CPKHeader.dwMaxTableNum);

	m_bLoaded = TRUE;
	return TRUE;
	
}




//-----------------------------------------------------------------------------
// 载入压缩包
//-----------------------------------------------------------------------------
BOOL CPK::Load(LPCTSTR pFileName)
{
	if( m_bLoaded )	// 必须先释放再载入
		this->Unload();
	else 
		this->Reset();

	ASSERT(pFileName);
	if (CDiscIOMgr::IsExist(pFileName) == FALSE)
		return FALSE;

	lstrcpy(m_szCPKFileName, pFileName);

	m_dwCPKHandle = CDiscIOMgr::Create(pFileName,
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE, // 非独占模式
		NULL, 
		OPEN_ALWAYS, 
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, 
		NULL);

	if (m_dwCPKHandle == (DWORD)INVALID_HANDLE_VALUE)
	{
		Msg(_T("Couldn't open file %s"), pFileName);
		CDiscIOMgr::Close(m_dwCPKHandle);
		ASSERT(0);
		return FALSE;
	}

	// 读入文件头
	ZeroMemory(&m_CPKHeader, sizeof(m_CPKHeader));
	CDiscIOMgr::Read(m_dwCPKHandle, &m_CPKHeader, sizeof(m_CPKHeader));
	if (m_CPKHeader.dwLable != GT_CPK_LABEL)
	{
		Msg(_T("Unknow file format %s"), pFileName);
		CDiscIOMgr::Close(m_dwCPKHandle);
		ASSERT(0);
		return FALSE;
	}

	//定位表项位置
	if( FALSE == CDiscIOMgr::Seek(m_dwCPKHandle, m_CPKHeader.dwTableStart) )
	{
		Msg(_T("Could not seek table from the CPK %s, seekpos=%ld"), \
			pFileName, m_CPKHeader.dwTableStart);

		return FALSE;
	}
	
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

		// 记录碎片所属的Table项
		memset(m_pFragment, -1, sizeof(INT)*m_dwMaxCPKTable);	
		// 记录Table所属的碎片项
		memset(m_pIndexToFragment, -1, sizeof(INT)*m_dwMaxCPKTable);
	}

	// 读入文件Table
	BOOL bResult = CDiscIOMgr::Read(m_dwCPKHandle, m_pCPKTable, 
		sizeof(tagCPKTable)*m_CPKHeader.dwMaxTableNum);
	if (FALSE == bResult)
	{
		Msg(_T("Could not load table from the CPK %s"), pFileName);
		return FALSE;
	}
	
	// 得到所有碎片信息
	GetAllFragment();

	m_bLoaded = TRUE;
	return TRUE;
}




//-----------------------------------------------------------------------------
// 卸载压缩包
//-----------------------------------------------------------------------------
BOOL CPK::Unload()
{
	if (m_bLoaded)
	{
		CDiscIOMgr::Close(m_dwCPKHandle);
		m_dwCPKHandle = 0;

		ZeroMemory(&m_CPKHeader, sizeof(m_CPKHeader));
		m_dwCurDirCRC = 0;
		m_bLoaded = FALSE;
		this->Reset();

		return TRUE;
	}

	return FALSE;
}
//-----------------------------------------------------------------------------
//增加表项空间
//-----------------------------------------------------------------------------
BOOL CPK::IncreaseTable()
{
	if (m_CPKHeader.dwFileNum + 1 < m_CPKHeader.dwMaxTableNum)
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

		FREEARR(m_pCPKTable);
		m_pCPKTable = pTable;


		INT *p1 = new INT[dwTable];
		INT *p2 = new INT[dwTable];
		ZeroMemory(p1, sizeof(INT)*dwTable);
		ZeroMemory(p2, sizeof(INT)*dwTable);
		memcpy(p1, m_pFragment, sizeof(INT)*m_CPKHeader.dwMaxTableNum);
		memcpy(p2, m_pIndexToFragment, sizeof(INT)*m_CPKHeader.dwMaxTableNum);
		
		FREEARR(m_pFragment);
		FREEARR(m_pIndexToFragment);
		m_pFragment = p1;
		m_pIndexToFragment = p2;

		m_dwMaxCPKTable = dwTable;

	}
	
	m_CPKHeader.dwMaxTableNum = dwTable;
	m_CPKHeader.dwPackageSize += sizeof(tagCPKTable)*GT_CPK_TABLE_ADD;

	return TRUE;
}

//-----------------------------------------------------------------------------
// 检查是否是合法的CPK文件包
//-----------------------------------------------------------------------------
BOOL CPK::IsValidCPK(LPCTSTR pFileName)
{
	ASSERT(pFileName);
	if( CDiscIOMgr::IsExist(pFileName) == FALSE )
	{
		Msg(_T("File %s doesn't exsit"), pFileName);
		return FALSE;
	}

	DWORD dwHandle = CDiscIOMgr::Create(pFileName,
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL, 
		OPEN_EXISTING, 
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, 
		NULL);

	if (dwHandle == (DWORD)INVALID_HANDLE_VALUE )
	{
		Msg(_T("Couldn't open file %s"), pFileName);
		return FALSE;
	}

	// 读入文件头
	tagCPKHeader header;
	ZeroMemory(&header, sizeof(header));
	CDiscIOMgr::Read(dwHandle, &header, sizeof(header));
	CDiscIOMgr::Close(dwHandle);
	
	// 检查文件标志
	if( header.dwLable != GT_CPK_LABEL )
	{
		//Msg(_T("Unknow file format %s"), pFileName);
		return FALSE;
	}

	// 检查文件版本号
	if( header.dwVersion != GT_CPK_VERSION )
	{
		//Msg(_T("Wrong file version %lu"), header.dwVersion);
		return FALSE;
	}

	// 检查文件Table表起始位置
	if( header.dwTableStart == 0 )
	{
		//Msg(_T("Wrong table start pos"));
		return FALSE;
	}

	// 检查包中文件数
	if( header.dwFileNum > header.dwMaxTableNum )
	{
		//Msg(_T("Wrong files num in package"));
		return FALSE;
	}

	// 检查包中有效项个数
	if( header.dwValidTableNum > header.dwMaxTableNum )
	{
		//Msg(_T("Wrong valid table item num in package"));
		return FALSE;
	}

	// 检查文件数与有效项数的关系
	if( header.dwFileNum > header.dwValidTableNum )
	{
		//Msg(_T("File num is more than table item num in package"));
		return FALSE;
	}

	// 以后版本必须检查文件项是否从小到大排序

	return TRUE;
}

//-----------------------------------------------------------------------------
// 在包里指定目录创建一个空目录, 返回CRC
//-----------------------------------------------------------------------------
DWORD CPK::CreateDir(LPCTSTR pSimpleDirName, DWORD dwFatherCRC)
{
	ASSERT(pSimpleDirName);

	if( FALSE == IncreaseTable() )
		return FALSE;

	// 将目录名转化为小写再计算CRC
	TCHAR szFullDirName[MAX_PATH];
	this->GetFullNameFromCRC(szFullDirName, dwFatherCRC);
	this->LinkSimpleNameToDirName(szFullDirName, pSimpleDirName);
	DWORD dwCRC = CRC32(szFullDirName);

	if( this->HaveRepeatCRC(dwCRC) == TRUE )
	{
		TCHAR szTempFullName[MAX_PATH];
		this->GetFullNameFromCRC(szTempFullName, dwCRC);
	//	Msg(_T("有重复的CRC:%lu, 目录:%s, 重复目录:%s"), 
	//		dwCRC, szFullDirName, szTempFullName);

		return dwCRC;
	}


	// 得到一个空闲的表项空间
	int nTableIndex = GetFreeTableItem(dwCRC);
	if (nTableIndex == GT_INVALID)
	{
		Msg(_T("Couldn't find any free table item"));
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

	return m_pCPKTable[nTableIndex].dwCRC;

}

//-----------------------------------------------------------------------------
//这种方式不添加文件，只添加他的路径，根路径作为dwFatherCrc的子目录．为其它操作做准备
//-----------------------------------------------------------------------------
DWORD CPK::AddDir(LPCTSTR pFullName, DWORD dwFatherCRC)
{
	ASSERT(pFullName);
	//首先一步一步剥离路径,比如c:\bin\tidy, c:\, c:\bin\tidy

	TCHAR szPathFile[MAX_PATH];
	INT i=0, j=0;
	for(i=0, j=0; pFullName[i] != '\0'; i++)
	{
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
//与上面的函数不同,这种方式添加文件他的路径也需要创建,根路径作为dwFatherCrc的子目录
//-----------------------------------------------------------------------------
BOOL CPK::AddFileAndDir(LPCTSTR pFullName, DWORD dwFatherCRC)
{
	ASSERT(pFullName);
	//首先一步一步剥离路径,比如c:\bin\tidy, c:\, c:\bin\tidy
	
	TCHAR szPathFile[MAX_PATH];
	for(int i=0, j=0; pFullName[i] != '\0'; i++)
	{
		if( pFullName[i] == '\\' && i > 0 )
		{
			ZeroMemory(szPathFile, sizeof(szPathFile));
			strncpy(szPathFile, pFullName+j, i-j);
			dwFatherCRC = CreateDir(szPathFile, dwFatherCRC);

			j = i+1;
		}
	}

	return AddFile(pFullName, dwFatherCRC);
}
//-----------------------------------------------------------------------------
// 往包里的当前目录添加文件,这种方式比较特殊，它本身是从另外一个ｃｐｋ包里转移过来
//-----------------------------------------------------------------------------
BOOL CPK::AddFile(LPCTSTR pFullName, DWORD dwFatherCRC, tagCPKTable *pTable, LPVOID p)
{
	ASSERT(pFullName);

	TCHAR *szName = (TCHAR *)p + pTable->dwPackedSize;

	// 是否还能放下文件
	if( FALSE == IncreaseTable() )
		return FALSE;	

	BOOL bDir = CDiscIOMgr::IsDir(pFullName);
	if( bDir == TRUE )
	{
		DWORD dwResult = CreateDir(pFullName, dwFatherCRC);

		return dwResult != GT_INVALID ? TRUE : FALSE;
	}

	// 将文件名拼上全路径名转化为小写再计算CRC
	TCHAR szFullName[MAX_PATH];
	this->ConvertFullNameToCPKFullName(szFullName, pFullName, dwFatherCRC);

	// 还要处理有重复文件的情况
	DWORD dwCRC = CRC32(szFullName);
	if( this->HaveRepeatCRC(dwCRC) == TRUE )
	{
		TCHAR szTempFullName[MAX_PATH];
		this->GetFullNameFromCRC(szTempFullName, dwCRC);
		//Msg(_T("有重复的CRC:%lu, 文件:%s, 重复文件:%s"),
		//	dwCRC, szFullName, szTempFullName);

		return FALSE;
	}

	// 得到一个空闲的表项空间
	int nTableIndex = GetFreeTableItem(dwCRC);
	if (nTableIndex == GT_INVALID)
	{
		Msg(_T("Couldn't find any free table item, AddFile failed"));
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
	CDiscIOMgr::Seek(m_dwCPKHandle, dwDistanceToMove);
	DWORD dwDestFileSize = CDiscIOMgr::GetSize(m_dwCPKHandle);
	if (dwDistanceToMove >= dwDestFileSize)
		CDiscIOMgr::SetEOF(m_dwCPKHandle);

	// 写入实际信息
	CDiscIOMgr::Write(m_dwCPKHandle, p, dwDataSize);			


	// 将文件头、表项写入磁盘
	this->WriteHeaderToDisc();
	this->WriteTableToDisc();

	return TRUE;
}
//-----------------------------------------------------------------------------
// 往包里的当前目录添加文件
//-----------------------------------------------------------------------------
BOOL CPK::AddFile(LPCTSTR pFullName, DWORD dwFatherCRC)
{
	ASSERT(pFullName);

	// 是否还能放下文件
	if( FALSE == IncreaseTable() )
		return FALSE;

	BOOL bDir = CDiscIOMgr::IsDir(pFullName);

	m_dwFilesCurProceed = 0;
	BOOL bResult = this->AddFileOrDir(pFullName, dwFatherCRC, bDir);


	// 将文件头、表项写入磁盘
	this->WriteHeaderToDisc();
	this->WriteTableToDisc();

	return bResult;
}





//-----------------------------------------------------------------------------
// 释放文件
//-----------------------------------------------------------------------------
BOOL CPK::ExtractFile(LPCTSTR pDestDir, LPCTSTR pFullName)
{
	ASSERT(pDestDir);
	ASSERT(pFullName);

	DWORD dwCRC = 0;
	this->GetCRCFromCPKFullName(dwCRC, pFullName);



	DWORD dwFatherCrc = this->GetFatherCRC(dwCRC);
	BOOL bResult = this->ExtractFileOrDir(pDestDir, dwCRC, dwFatherCrc);
	
	return bResult;

}



//-----------------------------------------------------------------------------
// 删除包里的文件或者目录，使用完整路径
//-----------------------------------------------------------------------------
BOOL CPK::RemoveFile(LPCTSTR pFullName)
{
	ASSERT(pFullName);

	// 找到包中的文件Table项
	INT nTableIndex = this->GetTableIndex(pFullName);
	if( nTableIndex == GT_INVALID )
	{
		Msg(_T("Couldn't find file : %s in this package"), pFullName);
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
// 改变当前目录
//-----------------------------------------------------------------------------
BOOL CPK::SetCurDir(LPCTSTR pFullDirName)
{
	ASSERT(pFullDirName);
	
	if( lstrlen(pFullDirName) == 0 )	// 根目录
	{
		// 记录根目录名
		lstrcpy(m_szCurDir, pFullDirName);
		// 记录根目录CRC
		m_dwCurDirCRC = 0;
		return TRUE;
	}

	if( lstrlen(pFullDirName) > MAX_PATH )
	{
		Msg(_T("Too long directory name : %s"), pFullDirName);
		return FALSE;
	}

	// 全部转成小写
	INT nTableIndex = GetTableIndex(pFullDirName);
	if( nTableIndex == GT_INVALID )
	{
		Msg(_T("Couldn't found directory : %s"), pFullDirName);
		return FALSE;
	}
	
	// 记录当前目录名
	lstrcpy(m_szCurDir, pFullDirName);
	// 记录当前目录CRC
	m_dwCurDirCRC = m_pCPKTable[nTableIndex].dwCRC;

	return TRUE;
}




//-----------------------------------------------------------------------------
// 得到包中指定文件或者目录的父目录CRC
//-----------------------------------------------------------------------------
DWORD CPK::GetFatherCRC(DWORD dwCRC)
{
	DWORD dwFatherCRC = 0;

	if( dwCRC == 0 )// 根目录没有父目录
		return 0;

	INT nTableIndex = this->GetTableIndexFromCRC(dwCRC);
	if( nTableIndex == GT_INVALID )
	{
		Msg(_T("Couldn't find file which CRC=%lu"), dwCRC);
		return 0;
	}

	return m_pCPKTable[nTableIndex].dwFatherCRC;
}

//-----------------------------------------------------------------------------
// 得到包中指定目录的所有一级子文件
// 将结果输出到pdwCRCArray所指向的双字数组
// 如果pdwCRCArray==NULL, 则不输出CRC，只计算并输出nChildNum
//-----------------------------------------------------------------------------
BOOL CPK::GetFirstLevelChild(DWORD dwDirCRC, DWORD& dwChildNum, DWORD* pdwCRCArray)
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
// 非标优化函数：得到包中指定目录的所有一级子文件
//-----------------------------------------------------------------------------
BOOL CPK::_GetFirstLevelChild(DWORD dwDirCRC, DWORD& dwChildNum)
{

	dwChildNum = 0;
	for( INT n=0; n<(INT)m_CPKHeader.dwValidTableNum; n++ )
	{
		// 非子对象
		if( dwDirCRC != m_pCPKTable[n].dwFatherCRC )
			continue;

		// 非法
		if( FALSE == IS_FILE_OR_DIR_VALID(m_pCPKTable[n].dwFlag) )
			continue;

		if( IS_DIR(m_pCPKTable[n].dwFlag) )
			m_nTempArray[dwChildNum++] = n;
	}

	for( INT n=0; n<(INT)m_CPKHeader.dwValidTableNum; n++ )
	{
		// 非子对象
		if( dwDirCRC != m_pCPKTable[n].dwFatherCRC )
			continue;

		// 非法
		if( FALSE == IS_FILE_OR_DIR_VALID(m_pCPKTable[n].dwFlag) )
			continue;

		if( !IS_DIR(m_pCPKTable[n].dwFlag) )
			m_nTempArray[dwChildNum++] = n;
	}

	return TRUE;
}



//-----------------------------------------------------------------------------
// 得到包中指定目录下的所有一级子目录
// 将结果输出到pdwCRCArray所指向的双字数组
// 如果pdwCRCArray==NULL, 则不输出CRC，只计算并输出nChildNum
//-----------------------------------------------------------------------------
BOOL CPK::GetFirstLevelChildDir(DWORD dwDirCRC, DWORD& dwChildNum, 
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
// 由绝对路径带文件名得到CRC标识，如果包中不存在此文件则返回FALSE
//-----------------------------------------------------------------------------
BOOL CPK::GetCRCFromCPKFullName(DWORD& dwCRC, LPCTSTR pFullName)
{
	ASSERT(pFullName);
	if (lstrlen(pFullName) > MAX_PATH)
	{
		Msg(_T("File name too long : %s"), pFullName);
		return FALSE;
	}

	if (lstrlen(pFullName) == 0)	// 根目录
	{
		dwCRC = 0;
		return TRUE;
	}

	// 先得到Table序号
	INT nTableIndex = this->GetTableIndex(pFullName);
	if (nTableIndex == GT_INVALID)
	{
		Msg(_T("File : %s not found in the package"), pFullName);
		ASSERT(0);
		return FALSE;
	}

	dwCRC = m_pCPKTable[nTableIndex].dwCRC;
	return TRUE;
}




//-----------------------------------------------------------------------------
// 由CRC标识得到简单文件名
//-----------------------------------------------------------------------------
BOOL CPK::GetSimpleFileNameFromCRC(LPTSTR pSimpleFileName, DWORD dwCRC)
{
	BOOL bResult = FALSE;
	ASSERT(pSimpleFileName);

	if (dwCRC == 0)
	{
		Msg( _T("CRC == 0 in function GetFileNameFromCRC"));
		return FALSE;
	}

	INT nTableIndex = this->GetTableIndexFromCRC(dwCRC);

	// 从包中得到简单文件名(没有路径)
	bResult = this->GetSimpleFileName(pSimpleFileName, nTableIndex);
	if (bResult == FALSE)
	{
		Msg(_T("Could not get the simple file name"));
		return FALSE;
	}
	return TRUE;

}

//-----------------------------------------------------------------------------
// 由表项序号得到CPK包中该文件的md5校验值
//-----------------------------------------------------------------------------
BOOL CPK::GetMD5FromTableIndex(BYTE digest[16], DWORD dwTableIndex)
{
	
	if( FALSE == IS_FILE_OR_DIR_VALID(m_pCPKTable[dwTableIndex].dwFlag) )
		return FALSE;

	DWORD dwReadPos = m_pCPKTable[dwTableIndex].dwStartPos + m_pCPKTable[dwTableIndex].dwPackedSize + \
		m_pCPKTable[dwTableIndex].dwExtraInfoSize - 16;
	CDiscIOMgr::Seek(m_dwCPKHandle, dwReadPos);
	CDiscIOMgr::Read(m_dwCPKHandle, digest, 16);

	return TRUE;
}
//-----------------------------------------------------------------------------
// 由CRC标识得到CPK包中该文件的md5校验值
//-----------------------------------------------------------------------------
BOOL CPK::GetMD5FromCrc(BYTE digest[16], DWORD dwCrc)
{
	INT nTableIndex =  GetTableIndexFromCRC(dwCrc);
	if( GT_INVALID == nTableIndex )
		return FALSE;
	if( FALSE == IS_FILE_OR_DIR_VALID(m_pCPKTable[nTableIndex].dwFlag) )
		return FALSE;

	DWORD dwReadPos = m_pCPKTable[nTableIndex].dwStartPos + m_pCPKTable[nTableIndex].dwPackedSize + \
						m_pCPKTable[nTableIndex].dwExtraInfoSize - 16;
	CDiscIOMgr::Seek(m_dwCPKHandle, dwReadPos);
	CDiscIOMgr::Read(m_dwCPKHandle, digest, 16);

	return TRUE;
}

//-----------------------------------------------------------------------------
// 由CRC标识得到CPK包中带路径文件全名
//-----------------------------------------------------------------------------
BOOL CPK::GetFullNameFromCRC(LPTSTR pFullName, DWORD dwCRC)
{
	ASSERT(pFullName);
	static TCHAR szTempNameNew[MAX_PATH*2];
	static TCHAR szTempNameOld[MAX_PATH*2];
	BOOL bResult = FALSE;
	INT nTableIndex = GT_INVALID;

	ZeroMemory(szTempNameNew, sizeof(szTempNameNew));
	ZeroMemory(szTempNameOld, sizeof(szTempNameOld));

	if (dwCRC == 0)	// 根目录
	{
		lstrcpy(pFullName, _T(""));
		return TRUE;
	}

	bResult = this->GetSimpleFileNameFromCRC(szTempNameOld, dwCRC);
	if (FALSE == bResult)
	{
		ASSERT(0);
		Msg(_T("没有在包中找到指定文件 CRC%lu"), dwCRC);
		return FALSE;
	}

	nTableIndex = this->GetTableIndexFromCRC(dwCRC);
	ASSERT(nTableIndex != GT_INVALID);
	dwCRC = (m_pCPKTable + nTableIndex)->dwFatherCRC;

	while(dwCRC != 0)
	{
		// 进行文件名拼接
		lstrcpy(szTempNameNew, _T("\\"));
		lstrcat(szTempNameNew, szTempNameOld); // new: \abc

		this->GetSimpleFileNameFromCRC(szTempNameOld, dwCRC);
		lstrcat(szTempNameOld, szTempNameNew); // old: abc\abc

		if (lstrlen(szTempNameOld) >= MAX_PATH)
		{
			ASSERT(0);
			Msg(_T("文件名太长 %s"), szTempNameOld);
			return FALSE;
		}

		nTableIndex = this->GetTableIndexFromCRC(dwCRC);
		ASSERT(nTableIndex != GT_INVALID);
		dwCRC = (m_pCPKTable + nTableIndex)->dwFatherCRC;

	}

	lstrcpy(pFullName, szTempNameOld);
	return TRUE;
}




//-----------------------------------------------------------------------------
// 由CRC标识和FatherCRC得到CPK包中带相对路径的文件名
//-----------------------------------------------------------------------------
BOOL CPK::GetRelativeNameFromCRC(LPSTR pNameOut, DWORD dwCRC, DWORD dwFatherCRC)
{
	ASSERT(pNameOut);
	static TCHAR szTempNameNew[MAX_PATH*2];
	static TCHAR szTempNameOld[MAX_PATH*2];
	BOOL bResult = FALSE;
	INT nTableIndex = GT_INVALID;

	ZeroMemory(szTempNameNew, sizeof(szTempNameNew));
	ZeroMemory(szTempNameOld, sizeof(szTempNameOld));

	if (dwCRC == 0)	// 根目录
	{
		lstrcpy(pNameOut, _T(""));
		return TRUE;
	}

	if (dwCRC == dwFatherCRC)	// 同一目录
	{
		lstrcpy(pNameOut, _T(""));
		return TRUE;
	}

	bResult = this->GetSimpleFileNameFromCRC(szTempNameOld, dwCRC);
	if (FALSE == bResult)
	{
		ASSERT(0);
		Msg(_T("没有在包中找到指定文件 CRC%lu"), dwCRC);
		return FALSE;
	}

	nTableIndex = this->GetTableIndexFromCRC(dwCRC);
	ASSERT(nTableIndex != GT_INVALID);
	dwCRC = (m_pCPKTable + nTableIndex)->dwFatherCRC;

	while(dwCRC != dwFatherCRC)
	{
		// 进行文件名拼接
		lstrcpy(szTempNameNew, _T("\\"));
		lstrcat(szTempNameNew, szTempNameOld); // new: \abc

		this->GetSimpleFileNameFromCRC(szTempNameOld, dwCRC);
		lstrcat(szTempNameOld, szTempNameNew); // old: abc\abc

		if (lstrlen(szTempNameOld) >= MAX_PATH)
		{
			ASSERT(0);
			Msg(_T("文件名太长 %s"), szTempNameOld);
			return FALSE;
		}

		nTableIndex = this->GetTableIndexFromCRC(dwCRC);
		ASSERT(nTableIndex != GT_INVALID);
		dwCRC = (m_pCPKTable + nTableIndex)->dwFatherCRC;
	}

	lstrcpy(pNameOut, szTempNameOld);
	return TRUE;
}

//-----------------------------------------------------------------------------
// 根据所给的CRC得到空闲的Table项
//-----------------------------------------------------------------------------
INT	CPK::GetFreeTableItem(DWORD dwCRC)
{
	if( m_CPKHeader.dwValidTableNum >= m_CPKHeader.dwMaxTableNum -1 )
	{
		ASSERT( 0 );


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
DWORD CPK::FindCPKFreePlace(DWORD dwSize)
{
	INT nFrag = GT_INVALID;
	DWORD dwPos = 0;

	// 先检查有没有能容纳dwSize的空间碎片,查找最先找到的空间
	//for( INT n=0; (DWORD)n<m_CPKHeader.dwFragmentNum; n++ )
	//{
	//	nFrag = m_pFragment[n];

	//	ASSERT( IS_VALID_CPK_TABLE(nFrag) );
	//	ASSERT( IS_FILE_REMOVED(m_pCPKTable[nFrag].dwFlag) );	// 确认是碎片

	//	if( m_pCPKTable[nFrag].dwPackedSize >= dwSize )
	//	{
	//		dwPos = m_pCPKTable[nFrag].dwStartPos;

	//		m_pCPKTable[nFrag].dwStartPos += dwSize;
	//		m_pCPKTable[nFrag].dwPackedSize -= dwSize;	// 碎片变小
	//		return dwPos;
	//	}
	//}

	DWORD dwMinDiff = -1, dwDiffTemp = 0, dwIndex = 0;
	// 先检查有没有能容纳dwSize的空间碎片，查找与申请的大小最相近的空间
	for( INT n=0; (DWORD)n<m_CPKHeader.dwFragmentNum; n++ )
	{
		nFrag = m_pFragment[n];

		ASSERT( IS_VALID_CPK_TABLE(nFrag) );
		ASSERT( IS_FILE_REMOVED(m_pCPKTable[nFrag].dwFlag) );	// 确认是碎片

		if( m_pCPKTable[nFrag].dwPackedSize >= dwSize )
		{
			dwDiffTemp = m_pCPKTable[nFrag].dwPackedSize - dwSize;

			//查找最与申请大小最相近的碎片
			if( dwMinDiff == -1 )
			{
				dwMinDiff = dwDiffTemp;
				dwIndex = nFrag;
			}
			else if( dwDiffTemp < dwMinDiff )
			{
				dwMinDiff = dwDiffTemp;
				dwIndex = nFrag;
			}
		}
	}

	//对碎片进行处理，缩减碎片的大小，更改碎片的起始位置
	if( dwMinDiff != -1 )
	{
		dwPos = m_pCPKTable[dwIndex].dwStartPos;

		m_pCPKTable[dwIndex].dwStartPos += dwSize;
		m_pCPKTable[dwIndex].dwPackedSize -= dwSize;	// 碎片变小
		return dwPos;
	}

	// 如果没有则直接附加到包的table文件之前
	DWORD dwPackSize = m_CPKHeader.dwPackageSize;
	m_CPKHeader.dwPackageSize += dwSize;
	m_CPKHeader.dwTableStart = m_CPKHeader.dwPackageSize - \
		sizeof(tagCPKTable) * m_CPKHeader.dwMaxTableNum + m_dwCPKHeaderAddr;  //绝对位置(非相对)

	return m_dwCPKHeaderAddr + dwPackSize - sizeof(tagCPKTable)*m_CPKHeader.dwMaxTableNum;

}




//-----------------------------------------------------------------------------
// 检查是否包中有与所给CRC重复的CRC
//-----------------------------------------------------------------------------
BOOL CPK::HaveRepeatCRC(DWORD dwCRC)
{
	INT nTableIndex = this->GetTableIndexFromCRC(dwCRC);
	if (nTableIndex == GT_INVALID)
		return FALSE;
	else
		return TRUE;
}


//-----------------------------------------------------------------------------
// 添加单个文件到指定目录
//-----------------------------------------------------------------------------
BOOL CPK::AddSingleFile(LPCTSTR pFullName, DWORD dwFatherCRC)
{
	ASSERT(pFullName);
	
	EFMFilter eFilter = FMF_Null;

	if( FALSE == IncreaseTable() )
		return FALSE;

	// 将文件名拼上全路径名转化为小写再计算CRC
	TCHAR szFullName[MAX_PATH];
	this->ConvertFullNameToCPKFullName(szFullName, pFullName, dwFatherCRC);

	// 还要处理有重复文件的情况
	DWORD dwCRC = CRC32(szFullName);
	if( this->HaveRepeatCRC(dwCRC) == TRUE )
	{
		TCHAR szTempFullName[MAX_PATH];
		this->GetFullNameFromCRC(szTempFullName, dwCRC);
		//Msg(_T("有重复的CRC:%lu, 文件:%s, 重复文件:%s"),
		//	dwCRC, szFullName, szTempFullName);

		return FALSE;
	}

	// 得到一个空闲的表项空间
	int nTableIndex = GetFreeTableItem(dwCRC);
	if (nTableIndex == GT_INVALID)
	{
		Msg(_T("Couldn't find any free table item, AddFile failed"));
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
		Msg(_T("Adding single file : couldn't open file %s"), pFullName);
		return FALSE;
	}


	// 检查文件长度
	BOOL bResult = FALSE;
	DWORD dwSrcFileSize = CDiscIOMgr::GetSize(dwSrcHandle);
	DWORD dwFileFlag = 0;
	VOID* pSrc = NULL;
//==================================================================================
	pSrc = CMemMgr::Alloc( dwSrcFileSize );
	bResult = CDiscIOMgr::_LoadToMem( pSrc, dwSrcHandle, dwSrcFileSize );
	if( bResult == FALSE )
	{
		Msg(_T("Couldn't Load File %s, AddFile failed"), pFullName);
		CDiscIOMgr::Close(dwSrcHandle);
		CMemMgr::Free(pSrc);
		return FALSE;
	}

	// 填写过滤器所需参数
	tagFilterParam param;

	param.lpMemIn = pSrc;
	param.dwInBufferSize = dwSrcFileSize;
	param.dwInDataSize = dwSrcFileSize;	

	//预先判断是否压缩
	if( dwSrcFileSize <  COMPRESS_SIZE )
	{
		param.bCompress = TRUE;
		param.eFilter = FMF_MiniLzo1040;
		dwFileFlag = 0x1 | (param.eFilter<<16); // 有效 | LZO压缩
	}
	else 
	{
		param.bCompress = FALSE;
		param.eFilter = FMF_Changeless;
		dwFileFlag = 0x1 | (param.eFilter<<16) | 0x4;	// 有效 | 不压缩 | 大文件

	}

	//设置输出内存指向的位置
	if( param.bCompress == TRUE )
	{
		param.lpMemOut = m_pPackBuffer;
		param.dwOutBufferSize = m_dwPackBufferSize;
	}
	else
	{
		param.lpMemOut = pSrc;
		param.dwOutBufferSize = dwSrcFileSize;
	}

	// 过滤文件
	CFilterMgr::Filter(&param);
	if (param.bSuccess == FALSE)
	{
		CMemMgr::Free( pSrc );
		/*if( pOut != NULL )
		CMemMgr::Free( pOut );*/

		return FALSE;
	}

	//判断压缩是否合理，以决定是否取消压缩
	if( param.bCompress == TRUE )
	{
		DWORD dwPercent = ((double)param.dwOutDataSize / (double)dwSrcFileSize) * 100;
		if(dwPercent > 95 )
		{
			param.bCompress = FALSE;
			param.eFilter = FMF_Changeless;
			dwFileFlag = 0x1 | (param.eFilter<<16) | 0x4;	// 有效 | 不压缩 | 大文件

			param.lpMemOut = pSrc;
			param.dwOutBufferSize = dwSrcFileSize;
			param.dwOutDataSize = dwSrcFileSize;
		}
	}
//==================================================================================
	//if( dwSrcFileSize >= GT_CPK_RAW_BUFFER_SIZE 
	//	|| IsUnCompressable(pFullName) )	// mp3/bink donot take compress	
	//{
	//	// 将大文件读到内存
	//	eFilter = FMF_Changeless;
	//	dwFileFlag = 0x1 | (eFilter<<16) | 0x4;	// 有效 | 不压缩 | 大文件
	//	pSrc = CMemMgr::Alloc(dwSrcFileSize);
	//	bResult = CDiscIOMgr::_LoadToMem(pSrc, dwSrcHandle, dwSrcFileSize);
	//	if( bResult == FALSE )
	//	{
	//		Msg(_T("Couldn't Load File %s, AddFile failed"), pFullName);
	//		CDiscIOMgr::Close(dwSrcHandle);
	//		CMemMgr::Free(pSrc);
	//		return FALSE;
	//	}
	//}
	//else
	//{
	//	eFilter = FMF_MiniLzo1040;
	//	dwFileFlag = 0x1 | (eFilter<<16); // 有效 | LZO压缩

	//	// 将源文件读入内存
	//	bResult = CDiscIOMgr::_LoadToMem(m_pRawBuffer, dwSrcHandle, dwSrcFileSize);
	//	if (bResult == FALSE)
	//	{
	//		Msg(_T("Couldn't Load File %s, AddFile failed"), pFullName);
	//		CDiscIOMgr::Close(dwSrcHandle);
	//	}
	//	pSrc = m_pRawBuffer;
	//}


	//// 填写过滤器所需参数
	//tagFilterParam param;
	//param.eFilter = eFilter;
	//param.bCompress = TRUE;
	//param.lpMemIn = pSrc;
	//param.dwInBufferSize = m_dwRawBufferSize;
	//param.lpMemOut = m_pPackBuffer;
	//param.dwOutBufferSize = m_dwPackBufferSize;
	//param.dwInDataSize = dwSrcFileSize;	
	//	
	//
	//// 过滤文件
	//CFilterMgr::Filter(&param);
	//if (param.bSuccess == FALSE)
	//	return FALSE;

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

	BYTE digest[16];
	CMD5::MD5ForBuffer((BYTE*)pSrc, dwSrcFileSize, digest);

	// 向文件中插入数据
	bResult = AddRealDataToCPK(nTableIndex, pFullName, param.lpMemOut, param.dwOutDataSize, digest);
	if (bResult == FALSE)
	{
		Msg(_T("Couldn't add data to cpk, file name : %s"),	pFullName);
		ASSERT(0);
	}


	// 释放
	if(pSrc != m_pRawBuffer)
		CMemMgr::Free(pSrc);

	CDiscIOMgr::Close(dwSrcHandle);

	return bResult;

}



//-----------------------------------------------------------------------------
// 添加文件或目录到指定目录，包括其下的所有子项，处理递归
//-----------------------------------------------------------------------------
BOOL CPK::AddFileOrDir(LPCTSTR pFullName, DWORD dwFatherCRC, BOOL bDir)
{
	static TCHAR szTempInfo[MAX_PATH];
	TCHAR szSimpleName[MAX_PATH];
	TCHAR szTemp[MAX_PATH];
	WIN32_FIND_DATA FindData;
	DWORD dwCRC = 0;
	BOOL bResult = FALSE;

	if( bDir )
	{
		this->GetSimpleFileName(szSimpleName, pFullName);
		dwCRC = this->CreateDir(szSimpleName, dwFatherCRC);
		if( dwCRC == GT_INVALID )
			return FALSE;


		// 搜索其下的文件并加入
		lstrcpy(szTemp, pFullName);
		lstrcat(szTemp, _T("\\*.*"));
		DWORD dwHandle = CDiscIOMgr::FindFirstFile(szTemp, &FindData);
		if( dwHandle == (DWORD)INVALID_HANDLE_VALUE )
			return FALSE;

		// 目录也算一个文件
		m_dwFilesCurProceed++;

		while( CDiscIOMgr::FindNextFile(dwHandle, &FindData) )
		{
			if( FindData.cFileName[0] != _T('.') )
			{
				lstrcpy(szTemp, pFullName);
				lstrcat(szTemp, _T("\\"));
				lstrcat(szTemp, FindData.cFileName);
				bResult = this->AddFileOrDir(szTemp, dwCRC, FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
				if( bResult == FALSE )
					break;
			}
		}

		CDiscIOMgr::FindClose(dwHandle);
	}
	else
	{
		m_dwFilesCurProceed++;


		// 加入单个文件
		bResult = this->AddSingleFile(pFullName, dwFatherCRC);
		//return FALSE;
	}

	return bResult;
}




//-----------------------------------------------------------------------------
// 释放单个文件到指定目录
//-----------------------------------------------------------------------------
BOOL CPK::ExtractSingleFile(LPCTSTR pDestDir, DWORD dwCRC)
{
	ASSERT(pDestDir);
	ASSERT(dwCRC);

	// 找到包中的文件Table项
	INT nTableIndex = this->GetTableIndexFromCRC(dwCRC);
	if (nTableIndex == GT_INVALID)
	{
		Msg(_T("Couldn't find file in package, CRC:%lu"), dwCRC);
		return FALSE;
	}

	// 读出文件名
	TCHAR szExtraInfo[MAX_PATH];
	TCHAR szFileName[MAX_PATH];
	DWORD dwDistanceToMove = m_pCPKTable[nTableIndex].dwStartPos + 
		m_pCPKTable[nTableIndex].dwPackedSize;

	CDiscIOMgr::Seek(m_dwCPKHandle, dwDistanceToMove);
	DWORD dwBytesToRead = m_pCPKTable[nTableIndex].dwExtraInfoSize;
	BOOL bResult = CDiscIOMgr::Read(m_dwCPKHandle, szExtraInfo, dwBytesToRead);
	if (bResult == 0)
		return FALSE;

	// 构建完整文件名
	lstrcpy(szFileName, pDestDir);
	lstrcat(szFileName, _T("\\"));
	lstrcat(szFileName, szExtraInfo);

	// 创建新解出来的文件
	DWORD dwHandle = CDiscIOMgr::Create(szFileName,
		GENERIC_READ | GENERIC_WRITE,
		0, // 独占模式
		NULL, 
		OPEN_ALWAYS, 
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, 
		NULL);

	if (IS_INVALID_HANDLE(dwHandle))
	{
		Msg(_T("Couldn't write file %s"), szFileName);
		return FALSE;
	}


	LPVOID lpInBuffer = m_pPackBuffer;
	LPVOID lpOutBuffer = m_pRawBuffer;
	DWORD dwInBufferSize = m_dwPackBufferSize;
	DWORD dwOutBufferSize = m_dwRawBufferSize;

	if( IS_LARGE_FILE((m_pCPKTable + nTableIndex)->dwFlag) )
	{
		lpInBuffer = CMemMgr::Alloc(m_pCPKTable[nTableIndex].dwOriginSize);
		if (lpInBuffer == NULL)
		{
			ERR(CE_CANNOT_ALLOC_MEM);
			return FALSE;
		}
		lpOutBuffer = lpInBuffer;
		dwInBufferSize = m_pCPKTable[nTableIndex].dwPackedSize;
		dwOutBufferSize = m_pCPKTable[nTableIndex].dwOriginSize;
	}


	// 读出包中文件内容
	dwDistanceToMove = m_pCPKTable[nTableIndex].dwStartPos;
	CDiscIOMgr::Seek(m_dwCPKHandle, dwDistanceToMove);
	dwBytesToRead = m_pCPKTable[nTableIndex].dwPackedSize;
	CDiscIOMgr::Read(m_dwCPKHandle, lpInBuffer, dwBytesToRead);

	if( IS_LARGE_FILE((m_pCPKTable + nTableIndex)->dwFlag) == false )
	{
		// 反过滤（解压）小文件
		tagFilterParam param;
		param.bCompress = FALSE;
		param.bSuccess = FALSE;
		param.lpMemIn = lpInBuffer;
		param.dwInBufferSize = dwInBufferSize;
		param.dwInDataSize = m_pCPKTable[nTableIndex].dwPackedSize;
		param.lpMemOut = lpOutBuffer;
		param.dwOutBufferSize = dwOutBufferSize;
		param.dwOutDataSize = m_pCPKTable[nTableIndex].dwOriginSize;
		param.eFilter = (EFMFilter)((m_pCPKTable[nTableIndex].dwFlag & 0xffff0000) >> 16);
		
		// 过滤文件信息
		CFilterMgr::Filter(&param);
		if (param.bSuccess == FALSE)
			return FALSE;
	}

	// 将数据写到新创建的文件
	CDiscIOMgr::Write(dwHandle, lpOutBuffer, m_pCPKTable[nTableIndex].dwOriginSize);
	CDiscIOMgr::Close(dwHandle);

	if( IS_LARGE_FILE((m_pCPKTable + nTableIndex)->dwFlag) )
	{
		if (CMemMgr::Free(lpInBuffer) == FALSE)
		{
			ERR(CE_CANNOT_FREE_MEM);
		}
	}

	return TRUE;
}



//-----------------------------------------------------------------------------
// 释放文件或目录到指定目录，包括其下的所有子项，处理递归
// dwCRC : 需要释放的目录或者文件
// dwFatherSrcCRC : 最开始释放的目录，用来处理目录名
// 例如需要将包中根目录下的test目录下的abc目录下所有的文件和目录释放到C:\temp
// 则 dwFatherSrcCRC 为 abc 目录的 CRC
//-----------------------------------------------------------------------------
BOOL CPK::ExtractFileOrDir(LPCTSTR pDestDir, DWORD dwCRC, DWORD dwFatherSrcCRC)
{
	ASSERT(pDestDir);

	TCHAR szFullName[MAX_PATH*2];
	TCHAR szTemp[MAX_PATH];
	BOOL bResult = FALSE;

	if (this->IsDir(dwCRC))
	{
		// 先检查目录是否已经创建，如果没有，就创建目录
		lstrcpy(szFullName, pDestDir);
		this->GetRelativeNameFromCRC(szTemp, dwCRC, dwFatherSrcCRC);
		// 看是否需要添加"\"
		if (szFullName[lstrlen(szFullName)-1] != _T('\\'))
			lstrcat(szFullName, _T("\\"));
		lstrcat(szFullName, szTemp);
		::CreateDirectory(szFullName, NULL);

		DWORD dwChildNum = 0;
		DWORD dwChildFileNum = 0;
		DWORD dwChildDirNum = 0;
		this->GetFirstLevelChild(dwCRC, dwChildFileNum);
		this->GetFirstLevelChildDir(dwCRC, dwChildDirNum);
		
		DWORD *pChildFileCRCArray = new DWORD[dwChildFileNum];
		DWORD *pChildDirCRCArray = new DWORD[dwChildDirNum];

		this->GetFirstLevelChild(dwCRC, dwChildFileNum, pChildFileCRCArray);
		this->GetFirstLevelChildDir(dwCRC, dwChildDirNum, pChildDirCRCArray);

		m_dwFilesToProceed += dwChildFileNum + dwChildDirNum;	// 需要处理的文件数

		// 先释放目录下所有目录
		INT n = 0;
		for (n=0; n<(INT)dwChildDirNum; n++)
		{
			lstrcpy(szFullName, pDestDir);
			
			// 看是否需要添加"\"
			if (szFullName[lstrlen(szFullName)-1] != _T('\\'))
				lstrcat(szFullName, _T("\\"));
			
			// 得到相对目录名
			this->GetRelativeNameFromCRC(szTemp, pChildDirCRCArray[n], dwFatherSrcCRC);
			lstrcat(szFullName, szTemp);

_retry:			
			// 在磁盘上创建目录
			m_dwFilesCurProceed++;	// 已经处理的文件数
			bResult = ::CreateDirectory(szFullName, NULL);
			if (bResult == FALSE)
			{
				DWORD dwErr = ::GetLastError();
				switch(dwErr)
				{
				case 183:
					Msg(_T("Directory has already exist %s, continue"), szFullName);
					break;
				
				default:
					INT nResult = Msg(MB_OKCANCEL, 
						_T("Couldn't create directory %s, retry?"), szFullName);

					if (nResult == IDOK)
						goto _retry;

					ASSERT(0);
				}
			}

			this->ExtractFileOrDir(pDestDir, pChildDirCRCArray[n], dwFatherSrcCRC);
		}

		lstrcpy(szFullName, pDestDir);
		// 看是否需要添加"\"
		if (szFullName[lstrlen(szFullName)-1] != _T('\\'))
			lstrcat(szFullName, _T("\\"));
		
		// 得到相对目录名
		this->GetRelativeNameFromCRC(szTemp, dwCRC, dwFatherSrcCRC);
		lstrcat(szFullName, szTemp);

		// 释放目录下所有文件
		for (n=0; n<(INT)dwChildFileNum; n++)
			this->ExtractFileOrDir(szFullName, pChildFileCRCArray[n], dwFatherSrcCRC);

		delete []pChildFileCRCArray;
		delete []pChildDirCRCArray;
		pChildFileCRCArray = NULL;
		pChildDirCRCArray = NULL;

		bResult = TRUE;
	}
	else
	{
		m_dwFilesCurProceed++;	// 已经处理的文件数
		bResult = this->ExtractSingleFile(pDestDir, dwCRC);
		
		if (bResult == FALSE)
		{
			Msg(_T("Couldn't extract file, crc:%lu"), dwCRC);
			ASSERT(0);
			return bResult;
		}
	}


	return bResult;
	
}



//-----------------------------------------------------------------------------
// 删除文件或目录，会删除其下所有子项
// 可优化
//-----------------------------------------------------------------------------
BOOL CPK::RemoveFileOrDir(INT nTableIndex)
{
	ASSERT(IS_VALID_CPK_TABLE(nTableIndex));

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


	// 寻找子节点并删除，因为并未按照父节点排序，所以只能顺序查找
	DWORD dwCRC = m_pCPKTable[nTableIndex].dwCRC;
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
// 由任意文件名得到简单文件名(不带路径)
//-----------------------------------------------------------------------------
BOOL CPK::GetSimpleFileName(LPTSTR pSimpleName, LPCTSTR pFullName)
{
	ASSERT(pSimpleName);
	ASSERT(pFullName);

	LPCTSTR lpSimpleName = _tcsrchr(pFullName, _T('\\'));
	if (NULL == lpSimpleName)
	{
		lstrcpy(pSimpleName, pFullName);
	}
	else
	{
		lstrcpy(pSimpleName, lpSimpleName+1);
	}
	return TRUE;
}




//-----------------------------------------------------------------------------
// 连接简单文件名或目录名到目标目录名
//-----------------------------------------------------------------------------
BOOL CPK::LinkSimpleNameToDirName(LPTSTR pFullName, LPCTSTR pSimpleName)
{
	ASSERT(pFullName);
	ASSERT(pSimpleName);

	INT nFullNameLen = lstrlen(pFullName);

	// 粗略检查字符串长度是否能够容纳
	if( nFullNameLen >= MAX_PATH - 10 )
	{
		Msg(_T("完整文件名太长"));
		ASSERT(0);
		return FALSE;
	}

	// 先查找目录名后有没有"\"符号
	if( nFullNameLen>0 && pFullName[nFullNameLen-1] != _T('\\') )
		lstrcat(pFullName, _T("\\"));
		
	lstrcat(pFullName, pSimpleName);
	_tcslwr(pFullName);	// 转成小写
	return TRUE;
}




//-----------------------------------------------------------------------------
// 由磁盘全名转为包中文件名，必须知道父CRC, 并全部转为小写
//-----------------------------------------------------------------------------
BOOL CPK::ConvertFullNameToCPKFullName(LPTSTR pOutName, LPCTSTR pInName, 
									   DWORD dwFatherCRC)
{
	ASSERT(pOutName);
	ASSERT(pInName);
	BOOL bResult = FALSE;

	static TCHAR szSimpleName[MAX_PATH];
	static TCHAR szCPKFullName[MAX_PATH];

	bResult = this->GetFullNameFromCRC(szCPKFullName, dwFatherCRC);
	ASSERT(bResult);
	if (FALSE == bResult)
		return FALSE;

	bResult = this->GetSimpleFileName(szSimpleName, pInName);
	ASSERT(bResult);
	if (FALSE == bResult)
		return FALSE;

	if (lstrlen(szCPKFullName) + lstrlen(szSimpleName) > MAX_PATH)
	{
		ASSERT(0);
		Msg(_T("文件名太长%s%s"), szCPKFullName, szSimpleName);
		return FALSE;
	}

	// 下面的函数会自动全部转为小写
	this->LinkSimpleNameToDirName(szCPKFullName, szSimpleName);
	lstrcpy(pOutName, szCPKFullName);
	return TRUE;
}



//-----------------------------------------------------------------------------
// 得到文件大小
//-----------------------------------------------------------------------------
BOOL CPK::GetFileSize(DWORD& dwPackSize, DWORD& dwOriginSize, DWORD dwCRC)
{
	if (dwCRC == 0)	// 零代表根目录
		return TRUE;

	// 由CRC找到对应表项
	INT n = GetTableIndexFromCRC(dwCRC);

	if (n == GT_INVALID)
		return FALSE;

	tagCPKTable* pTableItem = m_pCPKTable + n;
	ASSERT(pTableItem);

	if ( IS_DIR(pTableItem->dwFlag) )
	{
		dwPackSize = 0;
		dwOriginSize = 0;
	}
	else
	{
		dwPackSize = pTableItem->dwPackedSize;
		dwOriginSize = pTableItem->dwOriginSize;
	}

	return TRUE;
}



//-----------------------------------------------------------------------------
// 由CRC标识得到对象是否是目录
//-----------------------------------------------------------------------------
BOOL CPK::IsDir(DWORD dwCRC)
{

	if( dwCRC == 0 )	// 零代表根目录
		return TRUE;

	// 由CRC找到对应表项
	INT n = GetTableIndexFromCRC(dwCRC);

	if( IS_DIR((m_pCPKTable+n)->dwFlag) )
		return TRUE;

	return FALSE;
}




//-----------------------------------------------------------------------------
// 得到包中文件的扩展名(带"."号)
//-----------------------------------------------------------------------------
BOOL CPK::GetFileExtName(LPTSTR pExtName, INT nTableIndex)
{
	ASSERT(IS_VALID_CPK_TABLE(nTableIndex));
	ASSERT(pExtName);

	static TCHAR szTemp[MAX_PATH];
	static TCHAR szFullName[MAX_PATH];
	LPTSTR lpExtName = NULL;

	tagCPKTable* pTableItem = m_pCPKTable+nTableIndex;

	if( FALSE == IS_FILE_OR_DIR_VALID(pTableItem->dwFlag) )
	{
		DbgMsg(_T("Table %d is not valid"), nTableIndex);
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

	CDiscIOMgr::Seek(m_dwCPKHandle, dwReadPos);
	CDiscIOMgr::Read(m_dwCPKHandle, szTemp, dwBytesToRead);

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
// 从包中得到简单文件名(没有路径的文件名)
//-----------------------------------------------------------------------------
BOOL CPK::GetSimpleFileName(LPTSTR pFileName, INT nTableIndex)
{
	ASSERT(IS_VALID_CPK_TABLE(nTableIndex));
	ASSERT(pFileName);

	static TCHAR szTemp[MAX_PATH];

	tagCPKTable* pTableItem = m_pCPKTable+nTableIndex;
	if( FALSE == IS_FILE_OR_DIR_VALID(pTableItem->dwFlag) )
	{
		DbgMsg(_T("Table %d is not valid"), nTableIndex);
		return FALSE;
	}

	// 读入文件附加信息，其中包含文件名信息
	DWORD dwReadPos = pTableItem->dwStartPos + pTableItem->dwPackedSize;
	DWORD dwBytesToRead = pTableItem->dwExtraInfoSize;

	CDiscIOMgr::Seek(m_dwCPKHandle, dwReadPos);
	CDiscIOMgr::Read(m_dwCPKHandle, szTemp, dwBytesToRead);

	lstrcpy(pFileName, szTemp);
	return TRUE;
}






//-----------------------------------------------------------------------------
// 计算附加信息长度
//-----------------------------------------------------------------------------
DWORD CPK::CalFileExtraInfoSize(LPCTSTR pFileName)
{
	static TCHAR szSimpleName[MAX_PATH];

	ASSERT(pFileName);
	lstrcpy(szSimpleName, pFileName);
	this->GetSimpleFileName(szSimpleName, pFileName);
	DWORD dwSize = 0;
	dwSize += lstrlen(szSimpleName) + GT_CPK_LEN_OF_STRING_END;
	dwSize += sizeof(tagFileExtraInfo);
	return dwSize;
}



//-----------------------------------------------------------------------------
// 向文件中插入数据
//-----------------------------------------------------------------------------
BOOL CPK::AddRealDataToCPK(INT nTableIndex, LPCTSTR pFileName,  VOID* pSrc, 
						   DWORD dwSrcFileSize, BYTE *pDigest)
{
	ASSERT(pFileName);
	ASSERT(IS_VALID_CPK_TABLE(nTableIndex));
	ASSERT(!IS_INVALID_HANDLE(m_dwCPKHandle));

	static TCHAR szFileName[MAX_PATH];

	BOOL bResult = FALSE;
	DWORD dwDistanceToMove = 0;
	DWORD dwBytesToWrite = 0;

	// 移动文件指针
	dwDistanceToMove = m_pCPKTable[nTableIndex].dwStartPos;
	CDiscIOMgr::Seek(m_dwCPKHandle, dwDistanceToMove);
	DWORD dwDestFileSize = CDiscIOMgr::GetSize(m_dwCPKHandle);
	if (dwDistanceToMove >= dwDestFileSize)
		CDiscIOMgr::SetEOF(m_dwCPKHandle);


	tagFileExtraInfo ExtraInfo;
	// 写入实际信息
	if( m_pCPKTable[nTableIndex].dwPackedSize > 0 )
	{
		memcpy(ExtraInfo.byMD5, pDigest, 16);
		CDiscIOMgr::Write(m_dwCPKHandle, pSrc, dwSrcFileSize);			
	}
	else
	{ //没有文件内容,没关系,我们计算名字的md5
		CMD5::MD5ForString((char*)pFileName, ExtraInfo.byMD5);
	}


	// 写入附加信息
	static TCHAR szSimpleName[MAX_PATH];
	this->GetSimpleFileName(szSimpleName, pFileName);
	dwBytesToWrite = (DWORD)lstrlen(szSimpleName);
	CDiscIOMgr::Write(m_dwCPKHandle, szSimpleName, dwBytesToWrite);

	// 在文件名后加两字节结束符号, 为与Unicode兼容
	CDiscIOMgr::Write(m_dwCPKHandle, _T("\0\0\0\0"), GT_CPK_LEN_OF_STRING_END);

	dwBytesToWrite = sizeof(ExtraInfo);
	CDiscIOMgr::Write(m_dwCPKHandle, &ExtraInfo, dwBytesToWrite);

	return TRUE;
}




//-----------------------------------------------------------------------------
// 根据文件名找到对应的表项
//-----------------------------------------------------------------------------
INT CPK::GetTableIndex(LPCTSTR pFullName)
{
	static TCHAR szFullName[MAX_PATH];

	// 将文件名转化为小写再计算CRC
	lstrcpy(szFullName, pFullName);
	_tcslwr(szFullName);

	DWORD dwCRC = CRC32(szFullName);
	return this->GetTableIndexFromCRC(dwCRC);
}


DWORD CPK::GetMapStartPos(DWORD dwCRC)
{
	SYSTEM_INFO si;
	::GetSystemInfo(&si);
	DWORD dwG = si.dwAllocationGranularity;

	DWORD dwStartPos = m_pCPKTable[GetTableIndexFromCRC(dwCRC)].dwStartPos;
	dwStartPos = dwStartPos - (dwStartPos % dwG);
	return dwStartPos;

}



//-----------------------------------------------------------------------------
// 由CRC找到对应表项
// 可优化
//-----------------------------------------------------------------------------
INT CPK::GetTableIndexFromCRC(DWORD dwCRC)
{
	/*
	// 简单地顺序查找
	for( INT n=0; n<(INT)m_CPKHeader.dwValidTableNum; n++ )
	{
		if( (m_pCPKTable+n)->dwCRC == dwCRC
			&& IS_FILE_OR_DIR_VALID((m_pCPKTable+n)->dwFlag) ) 
		{
			return n;
		}
	}*/

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
// 在Load Package时调用一次以便得到所有碎片信息
//-----------------------------------------------------------------------------
INT CPK::GetAllFragment()
{
	INT nFragment = 0;
	for( INT n=0; (DWORD)n<m_CPKHeader.dwValidTableNum; n++ )
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
// 将文件头写入磁盘包
//-----------------------------------------------------------------------------
BOOL CPK::WriteHeaderToDisc()
{
	// 将CPK Header写入
	CDiscIOMgr::Seek(m_dwCPKHandle, m_dwCPKHeaderAddr);
	DWORD dwBytesToWrite = sizeof(tagCPKHeader);
	CDiscIOMgr::Write(m_dwCPKHandle, &m_CPKHeader, dwBytesToWrite);

	return TRUE;

}



//-----------------------------------------------------------------------------
// 将文件表写入磁盘包
//-----------------------------------------------------------------------------
BOOL CPK::WriteTableToDisc()
{
	// 将Table写入
	DWORD dwDistanceToMove = m_CPKHeader.dwTableStart; 

	CDiscIOMgr::Seek(m_dwCPKHandle, dwDistanceToMove);
	DWORD dwBytesToWrite = sizeof(tagCPKTable) * m_CPKHeader.dwMaxTableNum;
	CDiscIOMgr::Write(m_dwCPKHandle, m_pCPKTable, dwBytesToWrite);

	return TRUE;
}





//-----------------------------------------------------------------------------
// 是否是不可压缩文件  mp3/bik
//-----------------------------------------------------------------------------
BOOL CPK::IsUnCompressable(LPCTSTR pFullName)
{
	return FALSE;
	static TCHAR szFullName[MAX_PATH];
	LPTSTR lpExtName = NULL;

	lstrcpy(szFullName, pFullName);
	lpExtName = _tcsrchr(szFullName, _T('.'));

	if( !lpExtName )
		return FALSE;

	if( _tcsicmp(lpExtName, _T(".mp3")) == 0 
		|| _tcsicmp(lpExtName, _T(".bik")) == 0 )
		return TRUE;
	else
		return FALSE;
}
//-----------------------------------------------------------------------------
//开始映射
//-----------------------------------------------------------------------------
DWORD CPK::CreateMap()
{
	// 没有任何CPK文件载入
	if( FALSE == m_bLoaded )
		return NULL;

	return CDiscIOMgr::CreateMapping(m_dwCPKHandle, NULL, PAGE_READONLY, 0, NULL);
	
}
VOID	CPK::CloseMap(DWORD dwFileMap)
{
	CDiscIOMgr::CloseMappingHandle(dwFileMap);
}
//-----------------------------------------------------------------------------
//影射包中的某个文件，取得文件内容的地址
//-----------------------------------------------------------------------------
LPVOID CPK::MapFile(DWORD dwFileMap, DWORD dwTableIndex, LPVOID &pMapAdd)
{
	// 没有任何CPK文件载入
	if( FALSE == m_bLoaded || NULL == dwFileMap )
		return NULL;
	if( FALSE == IS_FILE_OR_DIR_VALID(m_pCPKTable[dwTableIndex].dwFlag) )
		return NULL;

	DWORD dwOffset = m_pCPKTable[dwTableIndex].dwStartPos % m_dwAllocGranularity;
	DWORD dwStartPos = m_pCPKTable[dwTableIndex].dwStartPos - dwOffset;
	DWORD dwRealToRead = m_pCPKTable[dwTableIndex].dwPackedSize+ \
						m_pCPKTable[dwTableIndex].dwExtraInfoSize + dwOffset;
	pMapAdd = CDiscIOMgr::MapFile(dwFileMap, FILE_MAP_READ, dwStartPos, dwRealToRead);

	return (LPVOID)((DWORD)pMapAdd + dwOffset);
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
VOID CPK::UnMapFile(LPVOID p)
{
	CDiscIOMgr::UnMapFile(p);
}
//-----------------------------------------------------------------------------
// 得到内存分配尺度
//-----------------------------------------------------------------------------
DWORD CPK::GetAllocationGranularity()
{

	SYSTEM_INFO si;
	::GetSystemInfo(&si);
	return si.dwAllocationGranularity;
}

