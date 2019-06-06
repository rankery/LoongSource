//-----------------------------------------------------------------------------
// File: CPKFast.cpp
// Desc: game resource file system 1.0
// Time: 2003-1-2
// Auth: CTCRST
//
//-----------------------------------------------------------------------------
#include "..\stdafx.h"
#include "cpkdefine.h"
// 类继承(公共接口)
#include "package.h"
#include "cpkfast.h"

// 使用其它部件
#include "..\filter\filtermgr.h"
#include "..\system\disciomgr.h"
#include "..\memory\mem_pool.h"


namespace vEngine{


struct CPKFile
{
	tagCPKTable* pTableItem;	// 指向索引表

	// memory
	PVOID	lpMapAddress;	// Map开始位置
	PVOID	lpStartAddress;	// Map中的文件开始位置
	DWORD	dwOffset;		// 实际Map起始地址与文件开始地址的偏差
	PVOID	lpMem;			// 指向文件实际内容

	// for seek/tell
	DWORD	dwFileSize;		// 文件实际内容大小
	DWORD	dwPointer;		// 文件指针

	BOOL	bCompressed;	// 文件是否被压缩
	BOOL	bValid;			// 是否有效
};


//-----------------------------------------------------------------------------
// CPK construction / destruction
//-----------------------------------------------------------------------------
CPK::CPK()
{
	ZeroMemory(&m_CPKHeader, sizeof(m_CPKHeader));

	m_pCPKTable = NULL;	// 使用指针,以免构造函数压力过大
	m_bLoaded = FALSE;
	m_dwAllocGranularity = 0;
	m_dwCPKHandle = INVALID_FILE_HANDLE;
	m_dwCPKMappingHandle = INVALID_FILEMAP_HANDLE;
	m_nOpenedFileNum = 0;

	SYSTEM_INFO si;
	::GetSystemInfo(&si);
	m_dwAllocGranularity = si.dwAllocationGranularity;

	m_pMemPool = new SafeMemPool(8*1024*1024);
}

CPK::~CPK()
{
	Unload();
	SAFE_DEL(m_pMemPool);
}


//-----------------------------------------------------------------------------
// 载入压缩包
//-----------------------------------------------------------------------------
BOOL CPK::Load(LPCTSTR szFileName)
{
	ASSERT(szFileName);
	m_strCPKFileName = szFileName;

	if( m_bLoaded )	// 必须先释放再载入
		this->Unload();

	m_dwCPKHandle = m_pDiscIOMgr->Create(szFileName, GENERIC_READ,	// 只读
		FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, NULL);

	if( m_dwCPKHandle == INVALID_FILE_HANDLE )
		return FALSE;

	// 读入文件头
	m_pDiscIOMgr->Read(m_dwCPKHandle, &m_CPKHeader, sizeof(m_CPKHeader));
	
	// 检查文件标志
	if( m_CPKHeader.dwLable != GT_CPK_LABEL )
	{
		ERR(_T("Unknow file format %s"), szFileName);
		m_pDiscIOMgr->Close(m_dwCPKHandle);
		m_dwCPKHandle = INVALID_FILE_HANDLE;
		return FALSE;
	}

	// 检查文件版本号
	if( m_CPKHeader.dwVersion != GT_CPK_VERSION )
	{
		ERR(_T("Wrong file version %lu"), m_CPKHeader.dwVersion);
		m_pDiscIOMgr->Close(m_dwCPKHandle);
		m_dwCPKHandle = INVALID_FILE_HANDLE;
		return FALSE;
	}

	// 申请Table空间
	m_pCPKTable = (tagCPKTable*)malloc(sizeof(tagCPKTable)*m_CPKHeader.dwMaxTableNum);

	if( !m_pCPKTable )
	{
		m_pDiscIOMgr->Close(m_dwCPKHandle);
		m_dwCPKHandle = INVALID_FILE_HANDLE;
		return FALSE;
	}

	// 读入文件Table
	//定位表项位置
	if( FALSE == m_pDiscIOMgr->Seek(m_dwCPKHandle, m_CPKHeader.dwTableStart) )
	{
		ERR(_T("Could not seek table from the CPK %s, seekpos=%ld"), \
			szFileName, m_CPKHeader.dwTableStart);

		return FALSE;
	}


	if( sizeof(tagCPKTable)*m_CPKHeader.dwMaxTableNum
		!= m_pDiscIOMgr->Read(m_dwCPKHandle, m_pCPKTable,
		sizeof(tagCPKTable)*m_CPKHeader.dwMaxTableNum) )
	{
		ERR(_T("could not load cpk %s table"), szFileName);
		m_pDiscIOMgr->Close(m_dwCPKHandle);
		m_dwCPKHandle = INVALID_FILE_HANDLE;
		free(m_pCPKTable);
		return FALSE;
	}


	// 建立文件映射
	m_dwCPKMappingHandle = m_pDiscIOMgr->CreateMapping(m_dwCPKHandle, NULL,
		PAGE_READONLY, 0, NULL);
	
	if( m_dwCPKMappingHandle == NULL )
	{
		//无法映射文件,我们将采取普通方式操作文件
		ERR(_T("couldn't create file mapping %s"), szFileName);
		//转化为内部专用无效映射句柄标识
		m_dwCPKMappingHandle = INVALID_FILEMAP_HANDLE;
	}
	else
	{
		// 文件映射已经存在
		if( ::GetLastError() == ERROR_ALREADY_EXISTS )
		{
			ERR(_T("File mapping %s has already exist"), szFileName);
			m_pDiscIOMgr->CloseMappingHandle(m_dwCPKMappingHandle);
			m_dwCPKMappingHandle = INVALID_FILEMAP_HANDLE;
			m_pDiscIOMgr->Close(m_dwCPKHandle);
			m_dwCPKHandle = INVALID_FILE_HANDLE;
			free(m_pCPKTable);
			return FALSE;
		}
	}


	m_bLoaded = TRUE;
	return TRUE;
}




//-----------------------------------------------------------------------------
// 卸载压缩包
//-----------------------------------------------------------------------------
BOOL CPK::Unload()
{
	if( m_bLoaded )
	{
		ASSERT(m_nOpenedFileNum == 0);	// 确认所有文件已经关闭
	
		// 关闭文件映射句柄
		if( m_dwCPKMappingHandle != INVALID_FILEMAP_HANDLE )
		{
			m_pDiscIOMgr->CloseMappingHandle(m_dwCPKMappingHandle);
			m_dwCPKMappingHandle = INVALID_FILEMAP_HANDLE;
		}
		
		// 关闭CPK文件句柄
		m_pDiscIOMgr->Close(m_dwCPKHandle);
		m_dwCPKHandle = INVALID_FILE_HANDLE;

		ZeroMemory(&m_CPKHeader, sizeof(m_CPKHeader));
		free(m_pCPKTable);
		m_pCPKTable = NULL;

		m_strCPKFileName.clear();
		m_nOpenedFileNum = 0;

		m_bLoaded = FALSE;
		return TRUE;	// unload succeed
	}

	return FALSE; // nothing to unload
}




//-----------------------------------------------------------------------------
// 打开包中的单个文件, 并且建立文件映射，返回的是文件序号
//-----------------------------------------------------------------------------
DWORD CPK::Open(LPCTSTR szFileName)
{
	ASSERT(szFileName);

	// 没有任何CPK文件载入
	if( FALSE == m_bLoaded )
		return GT_INVALID;

	// 找到文件对应的CPK中的表项
	INT nTableIndex = this->GetTableIndex(szFileName);
	if( nTableIndex == GT_INVALID )
		return GT_INVALID;

	if( m_dwCPKMappingHandle != INVALID_FILEMAP_HANDLE )
	{
		DWORD dwHandle = OpenForMap(nTableIndex, szFileName);
		if( dwHandle )
			return dwHandle;
	}

	return OpenForCommon(nTableIndex, szFileName);
}



//-----------------------------------------------------------------------------
//文件映射方式打开单个文件
//-----------------------------------------------------------------------------
DWORD CPK::OpenForMap(INT nTableIndex, LPCTSTR szFileName)
{
	// 检查文件映射句柄
	ASSERT(m_dwCPKMappingHandle != INVALID_FILEMAP_HANDLE); 

	// 计算实际开始映射的位置
	DWORD dwRealStartPos = m_pCPKTable[nTableIndex].dwStartPos;
	dwRealStartPos = dwRealStartPos - (dwRealStartPos % m_dwAllocGranularity);

	// 实际映射与需要映射位置的偏差
	DWORD dwOffset = m_pCPKTable[nTableIndex].dwStartPos - dwRealStartPos;

	// 计算需要映射的字节
	DWORD dwBytesToRead = m_pCPKTable[nTableIndex].dwPackedSize
		+ m_pCPKTable[nTableIndex].dwExtraInfoSize + dwOffset;

	// 创建文件对象
	CPKFile* fp = (CPKFile*)m_pMemPool->Alloc(sizeof(CPKFile));
	if( !fp )
	{
		ERR(_T("could not malloc for fp in %s"), m_strCPKFileName.c_str());
		return NULL;
	}

	fp->bValid = TRUE;
	fp->lpMapAddress = NULL;

	// 映射
	fp->lpMapAddress = m_pDiscIOMgr->MapFile
			(m_dwCPKMappingHandle, FILE_MAP_READ, dwRealStartPos, dwBytesToRead);
	
	if( fp->lpMapAddress == NULL )
	{
		ERR(_T("could not map file %s\r\ncpk:%s, start pos:%lu, bytes:%lu"), 
			szFileName, m_strCPKFileName.c_str(), dwRealStartPos, dwBytesToRead);

		fp->bValid = FALSE;
		m_pMemPool->Free(fp);
		return NULL;
	}


	// 填写fp基本信息
	fp->pTableItem = &m_pCPKTable[nTableIndex];
	fp->lpStartAddress = (LPVOID)((BYTE*)fp->lpMapAddress + dwOffset);
	fp->bCompressed = IS_COMPRESSED( m_pCPKTable[nTableIndex].dwFlag );
	fp->dwFileSize = m_pCPKTable[nTableIndex].dwOriginSize;
	fp->dwOffset = dwOffset;
	fp->dwPointer = 0;	// 文件指针初始

	ASSERT( fp->dwFileSize>= 0 && fp->dwFileSize<0x40000000 );

	// 如果是压缩文件，则需要先解压缩
	if( fp->bCompressed == TRUE && fp->dwFileSize > 0 )
	{
		// 为解压申请内存
		fp->lpMem = m_pMemPool->Alloc(fp->dwFileSize);
		if( fp->lpMem == NULL )
		{
			m_pDiscIOMgr->UnMapFile(fp->lpMapAddress);
			fp->bValid = FALSE;
			m_pMemPool->Free(fp);
			return NULL;
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
		m_pFilterMgr->Filter(&param);

		if( FALSE == param.bSuccess )
		{
			ERR(_T("could not decompress"));
			m_pDiscIOMgr->UnMapFile(fp->lpMapAddress);
			m_pMemPool->Free(fp->lpMem);
			fp->bValid = FALSE;
			m_pMemPool->Free(fp);
			return NULL;
		}
	}
	else	// 如果文件没有被压缩过
	{
		// 将其指向Map中的文件实际起始地址
		fp->lpMem = fp->lpStartAddress;
	}

	::InterlockedIncrement((LONG*)&m_nOpenedFileNum);

	return (DWORD)fp;
}



//-----------------------------------------------------------------------------
//普通方式打开单个文件
//-----------------------------------------------------------------------------
DWORD CPK::OpenForCommon(INT nTableIndex, LPCTSTR szFileName)
{
	// 创建文件对象
	CPKFile* fp = (CPKFile*)m_pMemPool->Alloc(sizeof(CPKFile));
	if( !fp )
	{
		ERR(_T("could not malloc for fp in %s"), m_strCPKFileName.c_str());
		return NULL;
	}

	// 填写fp基本信息
	fp->bValid = TRUE;
	fp->lpMapAddress = NULL;
	fp->pTableItem = &m_pCPKTable[nTableIndex];
	fp->lpStartAddress = NULL;
	fp->bCompressed = IS_COMPRESSED( m_pCPKTable[nTableIndex].dwFlag );
	fp->dwFileSize = m_pCPKTable[nTableIndex].dwOriginSize;
	fp->dwOffset = 0;
	fp->dwPointer = 0;	// 文件指针初始

	ASSERT( fp->dwFileSize>= 0 && fp->dwFileSize<0x40000000 );

	//申请内存存储初始压缩文件内容
	LPVOID lpPackMem = m_pMemPool->Alloc(m_pCPKTable[nTableIndex].dwPackedSize);
	if( lpPackMem == NULL )
	{
		fp->bValid = FALSE;
		m_pMemPool->Free(fp);
		return NULL;
	}

	if( 0 == m_pDiscIOMgr->Seek(m_dwCPKHandle, m_pCPKTable[nTableIndex].dwStartPos) )
	{
		ERR(_T("could not read cpk file"));

		m_pMemPool->Free(lpPackMem);
		fp->bValid = FALSE;
		m_pMemPool->Free(fp);
		return NULL;
	}
	if( 0 == m_pDiscIOMgr->Read(m_dwCPKHandle, lpPackMem, m_pCPKTable[nTableIndex].dwPackedSize) )
	{
		ERR(_T("could not read cpk file"));

		m_pMemPool->Free(lpPackMem);
		fp->bValid = FALSE;
		m_pMemPool->Free(fp);
		return NULL;
	}

	// 如果是压缩文件，则需要先解压缩
	if( fp->bCompressed == TRUE && fp->dwFileSize > 0 )
	{

		// 为解压后真正的文件内容申请内存
		fp->lpMem = m_pMemPool->Alloc(fp->dwFileSize);
		if( fp->lpMem == NULL )
		{
			fp->bValid = FALSE;
			m_pMemPool->Free(lpPackMem);
			m_pMemPool->Free(fp);
			return NULL;
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
		param.lpMemIn = lpPackMem;
		param.lpMemOut = fp->lpMem;

		// 进行过滤文件信息
		m_pFilterMgr->Filter(&param);

		//释放初始压缩文件内容之内存
		m_pMemPool->Free(lpPackMem);


		if( FALSE == param.bSuccess )
		{
			ERR(_T("could not decompress"));
			m_pMemPool->Free(fp->lpMem);
			fp->bValid = FALSE;
			m_pMemPool->Free(fp);
			return NULL;
		}
	}
	else	// 如果文件没有被压缩过
	{
		// 将其指向初始文件内容存储的地址
		fp->lpMem = lpPackMem;
	}

	::InterlockedIncrement((LONG*)&m_nOpenedFileNum);

	return (DWORD)fp;
}


//-----------------------------------------------------------------------------
// 关闭包中的单个文件, 并且取消文件映射，参数是文件序号
//-----------------------------------------------------------------------------
BOOL CPK::Close(DWORD dwHandle)
{
	CPKFile* fp = (CPKFile*)dwHandle;
	ASSERT(fp != NULL);
	ASSERT(fp->bValid == TRUE);

	if( fp->lpMapAddress == 0 )
	{
		if( fp->lpMem != NULL )
		{
			//以普通文件读取方式
			m_pMemPool->Free(fp->lpMem);
		}
		else
		{
			ERR(_T("invalid address in cpk close"));
			return FALSE;
		}
	}
	else 
	{
		BOOL bResult = m_pDiscIOMgr->UnMapFile(fp->lpMapAddress);
		if( FALSE == bResult )
		{
			ERR(_T("could not unmap file in cpk close"));
			fp->bValid = FALSE;
			return FALSE;
		}
		
		if( fp->bCompressed && fp->dwFileSize > 0 )
			m_pMemPool->Free(fp->lpMem);
	}

	m_pMemPool->Free(fp);
	::InterlockedDecrement((LONG*)&m_nOpenedFileNum);

	return TRUE;
}



//-----------------------------------------------------------------------------
// 读包中单个文件的内容
//-----------------------------------------------------------------------------
DWORD CPK::Read(LPVOID lpMemOut, DWORD dwSize, DWORD dwHandle)
{
	CPKFile* fp = (CPKFile*)dwHandle;
	ASSERT(fp != NULL);
	ASSERT(fp->bValid);

	if( fp->dwPointer >= fp->dwFileSize )	// 文件指针已经到文件尾
		return 0;
	
	if( fp->dwPointer + dwSize > fp->dwFileSize )
		dwSize = fp->dwFileSize -  fp->dwPointer;

	memcpy(lpMemOut, (BYTE*)fp->lpMem + fp->dwPointer, dwSize);
	fp->dwPointer += dwSize;

	// 必须返回实际"成功"读取的字节数
	return dwSize;
}



//-----------------------------------------------------------------------------
// 定位包中单个文件的指针,返回当前文件指针位置
//-----------------------------------------------------------------------------
DWORD CPK::Seek(DWORD dwHandle, INT nOffset, DWORD dwOrigin)
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
// 返回当前文件指针位置
//-----------------------------------------------------------------------------
DWORD CPK::Tell(DWORD dwHandle)
{ 
	return ((CPKFile*)dwHandle)->dwPointer; 
}


//-----------------------------------------------------------------------------
// 读包中单个文件的全部内容，不会检查目标内存大小，返回原文件字节大小
//-----------------------------------------------------------------------------
DWORD CPK::LoadFile(LPVOID lpMemOut, LPCTSTR szFileName)
{
	ASSERT(szFileName);

	// 没有任何CPK文件载入
	if( m_bLoaded == FALSE )
	{
		ERR(_T("no cpk loaded"));
		return (DWORD)GT_INVALID;
	}
	
	// 找到文件对应的CPK中的表项
	INT nTableIndex = this->GetTableIndex(szFileName);
	if( nTableIndex == GT_INVALID )
	{
		if( NULL != lpMemOut )	// 外部只是想取得文件大小，则不产生明显提示
		{
			ERR(_T("could not find file %s in cpk"), szFileName);
		}

		return (DWORD)GT_INVALID;
	}

	// 如果内存目标为空，就仅返回原文件大小
	if( lpMemOut == NULL )
		return m_pCPKTable[nTableIndex].dwOriginSize;

	BOOL bCompressed = IS_COMPRESSED( m_pCPKTable[nTableIndex].dwFlag );


	// 文件映射句柄非法
	if( m_dwCPKMappingHandle != INVALID_FILEMAP_HANDLE)
	{
		// 计算实际开始映射的位置
		DWORD dwRealStartPos = m_pCPKTable[nTableIndex].dwStartPos;
		dwRealStartPos = dwRealStartPos - (dwRealStartPos % m_dwAllocGranularity);

		// 实际映射与需要映射位置的偏差
		DWORD dwOffset = m_pCPKTable[nTableIndex].dwStartPos - dwRealStartPos;
		
		// 计算需要映射的字节
		DWORD dwBytesToRead = m_pCPKTable[nTableIndex].dwPackedSize
			+ m_pCPKTable[nTableIndex].dwExtraInfoSize + dwOffset;
		
		// 映射
		VOID* lpMapAddress = m_pDiscIOMgr->MapFile
			(m_dwCPKMappingHandle, FILE_MAP_READ, dwRealStartPos, dwBytesToRead);

		if( lpMapAddress == NULL )
			goto __DiscIOMgr;
		
		// 反过滤（解压）文件
		tagFilterParam param;
		param.bCompress = FALSE;
		param.bSuccess = FALSE;
		param.dwInBufferSize = m_pCPKTable[nTableIndex].dwPackedSize;
		param.dwInDataSize = m_pCPKTable[nTableIndex].dwPackedSize;
		param.dwOutBufferSize = m_pCPKTable[nTableIndex].dwOriginSize;
		param.dwOutDataSize = m_pCPKTable[nTableIndex].dwOriginSize;
		param.eFilter = GET_FILTER( m_pCPKTable[nTableIndex].dwFlag );
		param.lpMemIn = (BYTE*)lpMapAddress + dwOffset;
		param.lpMemOut = lpMemOut;

		// 过滤文件信息
		m_pFilterMgr->Filter(&param);
		if( param.bSuccess == FALSE )
		{
			ERR(_T("decompress failed in cpk loadfile"));
			m_pDiscIOMgr->UnMapFile(lpMapAddress);
			return (DWORD)GT_INVALID;
		}

		// 解除映射
		if( m_pDiscIOMgr->UnMapFile(lpMapAddress) == FALSE )
		{
			ERR(_T("ummap failed in cpk loadfile"));
			return (DWORD)GT_INVALID;
		}
	}
	else
	{
__DiscIOMgr:
		if( 0 == m_pDiscIOMgr->Seek(m_dwCPKHandle, m_pCPKTable[nTableIndex].dwStartPos) )
		{
			ERR(_T("could not read cpk file"));
			return (DWORD)GT_INVALID;
		}

		// 如果是压缩文件，则需要先解压缩
		if( bCompressed == TRUE && m_pCPKTable[nTableIndex].dwOriginSize > 0 )
		{

			// 为解压后真正的文件内容申请内存
			LPVOID lpPackMem = m_pMemPool->Alloc(m_pCPKTable[nTableIndex].dwPackedSize);
			if( lpPackMem == NULL )
			{
				return (DWORD)GT_INVALID;
			}

			if( 0 == m_pDiscIOMgr->Read(m_dwCPKHandle, lpPackMem, 
						m_pCPKTable[nTableIndex].dwPackedSize) )
			{
				ERR(_T("could not read cpk file"));
				m_pMemPool->Free(lpPackMem);

				return (DWORD)GT_INVALID;
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
			param.lpMemIn = lpPackMem;
			param.lpMemOut = lpMemOut;

			// 进行过滤文件信息
			m_pFilterMgr->Filter(&param);

			//释放初始压缩文件内容之内存
			m_pMemPool->Free(lpPackMem);


			if( FALSE == param.bSuccess )
			{
				ERR(_T("could not decompress"));
				return (DWORD)GT_INVALID;
			}
		}
		else	// 如果文件没有被压缩过
		{
			if( 0 == m_pDiscIOMgr->Read(m_dwCPKHandle, lpMemOut, 
				m_pCPKTable[nTableIndex].dwPackedSize) )
			{
				ERR(_T("could not read cpk file"));
				return (DWORD)GT_INVALID;
			}
		}

	}

	return m_pCPKTable[nTableIndex].dwOriginSize;
}




//-----------------------------------------------------------------------------
// 根据文件名找到对应的表项
//-----------------------------------------------------------------------------
INT CPK::GetTableIndex(LPCTSTR pFullName)
{
	TCHAR szFullName[MAX_PATH];

	// 将文件名转化为小写再计算CRC
	lstrcpy(szFullName, pFullName);
	_tcslwr(szFullName);
	
	INT i = 0;
	while( i<MAX_PATH && szFullName[i] != _T('\0') )
	{
		if( szFullName[i] == _T('/') )
			szFullName[i] = _T('\\');
		i++;
	}


	CHAR szFullName8[MAX_PATH];
	INT nLen = WideCharToMultiByte(CP_ACP, 0, szFullName, -1, szFullName8, MAX_PATH, NULL, NULL);

	DWORD dwCRC = m_pUtil->Crc32((LPBYTE)szFullName8, nLen-1);
	return GetTableIndexFromCRC(dwCRC);
}


//-----------------------------------------------------------------------------
// 由CRC找到对应表项
//-----------------------------------------------------------------------------
INT CPK::GetTableIndexFromCRC(DWORD dwCRC)
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





}
