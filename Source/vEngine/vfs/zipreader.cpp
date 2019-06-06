//-----------------------------------------------------------------------------
// File: zipreader
// Time: 2004-2-19
// Auth: CTCRST
//
//-----------------------------------------------------------------------------
#include "..\stdafx.h"
#include "zipdefine.h"
// 类继承(公共接口)
#include "package.h"
#include "zipreader.h"

// 使用其它部件
#include "..\system\disciomgr.h"
#include "..\filter\filtermgr.h"

namespace vEngine{
//-----------------------------------------------------------------------------
// construction / destruction
//-----------------------------------------------------------------------------
ZipReader::ZipReader()
{
	m_bLoaded = FALSE;
	m_dwZipHandle = INVALID_FILE_HANDLE;
	m_dwZipFileSize = 0;
	ZeroMemory(m_szZipFileName, sizeof(m_szZipFileName));
	
	m_dwFileNumInZip = 0;
	m_dwCentralDirOffset = 0;
	m_lpFileTable = NULL;
	
	m_nOpenedFileNum = 0;
}

ZipReader::~ZipReader()
{
	Unload();
}

//-----------------------------------------------------------------------------
// load zip file
//-----------------------------------------------------------------------------
BOOL ZipReader::Load(LPCTSTR szZipFileName)
{
	ASSERT(szZipFileName);
	lstrcpy(m_szZipFileName, szZipFileName);

	if( m_bLoaded )	// 必须先释放再载入
		this->Unload();

	m_dwZipHandle = m_pDiscIOMgr->Create(szZipFileName,	GENERIC_READ,	// 只读
		FILE_SHARE_READ | FILE_SHARE_WRITE,	NULL, OPEN_EXISTING, 
			FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, NULL);

	if( m_dwZipHandle == INVALID_FILE_HANDLE )
	{
		ERR(_T("couldn't open package file %s"), szZipFileName);
		m_pDiscIOMgr->Close(m_dwZipHandle);
		return FALSE;
	}

	// 先要从文件末尾取得ZIP包中的文件数
	m_dwZipFileSize = m_pDiscIOMgr->GetSize(m_dwZipHandle);
	if( m_dwZipFileSize < sizeof(tagZipEndOfCentralDirRecord) )
	{
		ERR(_T("zip file too small %s"), szZipFileName);
		m_pDiscIOMgr->Close(m_dwZipHandle);
		return FALSE;
	}

	// 试着从文件末尾读入信息
	tagZipEndOfCentralDirRecord end_record;
	m_pDiscIOMgr->Seek(m_dwZipHandle, m_dwZipFileSize - sizeof(end_record));
	m_pDiscIOMgr->Read(m_dwZipHandle, &end_record, sizeof(end_record));

	// 并非合适的信息，再向前搜索
	if( end_record.dwSignature != 
		f_dwSignatureOfZipEndOfCentralDirRecord )
	{
		DWORD dwSign = 0;
		DWORD dwPointer = m_dwZipFileSize - sizeof(end_record);
		while( dwPointer > 32 )	// 我们需要的信息不可能这么靠前
		{
			dwPointer--;
			m_pDiscIOMgr->Seek(m_dwZipHandle, dwPointer);
			m_pDiscIOMgr->Read(m_dwZipHandle, &dwSign, 4);
			if( dwSign == f_dwSignatureOfZipEndOfCentralDirRecord )
			{
				m_pDiscIOMgr->Seek(m_dwZipHandle, dwPointer);
				m_pDiscIOMgr->Read(m_dwZipHandle, &end_record, sizeof(end_record));
				break;
			}
		}
		ERR(_T("couldn't recognise the zip file %s"), szZipFileName);
		m_pDiscIOMgr->Close(m_dwZipHandle);
		return FALSE;
	}

	if( end_record.wTotalEntriesOnTheDisk != 
		end_record.wTotalEntries )
	{
		// 不能处理分卷压缩的Zip文件
		ERR(_T("couldn't load split zip file %s"), szZipFileName);
		m_pDiscIOMgr->Close(m_dwZipHandle);
		return FALSE;
	}

	m_dwFileNumInZip = end_record.wTotalEntriesOnTheDisk;
	m_dwCentralDirOffset = end_record.dwOffsetOfTheCentralDir;

	
	// 读入所有文件名称信息，以加速后继查询
	m_lpFileTable = malloc(m_dwFileNumInZip*sizeof(tagZipFileItem));
	if( !m_lpFileTable )
	{
		ERR(_T("couldn't malloc for filetable in load zipfile:%s"), szZipFileName);
		m_pDiscIOMgr->Close(m_dwZipHandle);
		return FALSE;
	}

	// 依次读入所有文件项信息
	tagZipCentralDirectoryFileHeader file_header;
	TCHAR szFileName[MAX_PATH*2];
	m_pDiscIOMgr->Seek(m_dwZipHandle, m_dwCentralDirOffset);
	for(INT n=0; n<(INT)m_dwFileNumInZip; n++)
	{
		m_pDiscIOMgr->Read(m_dwZipHandle, &file_header, sizeof(file_header));
		if( file_header.wFileNameLength > MAX_FILENAME_LENGTH )
		{
			ERR(_T("Zipfile:%s have been destroyed"), szZipFileName );
			m_pDiscIOMgr->Close(m_dwZipHandle);
			return FALSE;
		}
		tagZipFileItem* pItem = &((tagZipFileItem*)m_lpFileTable)[n];
		pItem->dwFileHeaderOffset = file_header.dwRlativeOffsetOfLocalHeader;

		// 读取文件名
		m_pDiscIOMgr->Read(m_dwZipHandle, szFileName, file_header.wFileNameLength);
		this->HandleFileNameString(szFileName, file_header.wFileNameLength);
		pItem->dwFileNameCrc32 = m_pUtil->Crc32(szFileName);

		// 跳过ExtraField/FileComment
		if( file_header.wExtraFieldLength + 
			file_header.wFileCommentLength > 0 )
		{
			m_pDiscIOMgr->Seek(m_dwZipHandle, file_header.wExtraFieldLength +		
				file_header.wFileCommentLength, GT_SEEK_CUR);
		}
	}

	m_bLoaded = TRUE;
	return TRUE;
}



//-----------------------------------------------------------------------------
// unload zip file
//-----------------------------------------------------------------------------
BOOL ZipReader::Unload()
{
	if( !m_bLoaded )
		return FALSE;

	// 所有打开的文件是否都关闭了，没有关闭肯定存在内存泄漏
	ASSERT(m_nOpenedFileNum == 0);

	// 关闭zip文件句柄
	m_pDiscIOMgr->Close(m_dwZipHandle);
	m_dwZipHandle = INVALID_FILE_HANDLE;

	free(m_lpFileTable);
	m_lpFileTable = NULL;
	m_nOpenedFileNum = 0;

	m_bLoaded = FALSE;
	return TRUE;
}


//-----------------------------------------------------------------------------
// open single file in the zip package
//-----------------------------------------------------------------------------
DWORD ZipReader::Open(LPCTSTR szFileName)
{
	if( !m_bLoaded )
		return GT_INVALID;

	ASSERT(szFileName);
	INT n = 0;

	DWORD dwCrc = m_pUtil->Crc32(szFileName);

	// 搜索CRC符合的文件
	tagZipFileItem* pItem;

	
	for( n=0; n<(INT)m_dwFileNumInZip; n++)
	{
		pItem = &((tagZipFileItem*)m_lpFileTable)[n];
		if( pItem->dwFileNameCrc32 == dwCrc )
		{
			goto _founded;
		}
	}
	
	// 未找到指定文件
	return GT_INVALID;

_founded:

	// 读入此文件头信息
	tagZipLocalFileHeader file_header;
	m_pDiscIOMgr->Seek(m_dwZipHandle, pItem->dwFileHeaderOffset);
	m_pDiscIOMgr->Read(m_dwZipHandle, &file_header, sizeof(file_header));

	// 将文件指针移动到文件真正信息处
	m_pDiscIOMgr->Seek(m_dwZipHandle, file_header.wFileNameLength + 
		file_header.wExtraFieldLength, GT_SEEK_CUR);

	// 我们不处理加密文件
	if( file_header.wGeneralPurposeBitFlag & 1 )
	{
		ERR(_T("couldn't handle encrypted file %s in zip"), szFileName);
		return NULL;
	}

	// 压缩方法不能识别
	if( file_header.wCompressionMethod != 0	// 未压缩
		&& file_header.wCompressionMethod != 8 )	// Deflated
	{
		ERR(_T("couldn't handle %s's compression method in zip"), szFileName);
		return NULL;
	}

	// 申请一个文件指向体的空间
	tagFileInZip* fp = (tagFileInZip*)malloc(sizeof(tagFileInZip));
	if( !fp )
	{
		ERR(_T("malloc failed in open file %s in zip"), szFileName);
		return NULL;
	}

	// 申请原本文件信息的空间
	fp->dwFileSize = file_header.dwUncompressedSize;
	fp->pMem = malloc(fp->dwFileSize);
	if( !fp )
	{
		free(fp);
		ERR(_T("malloc failed in open file %s in zip"), szFileName);
		return NULL;
	}

	if( file_header.wCompressionMethod == 0	)// 未压缩
	{
		m_pDiscIOMgr->Read(m_dwZipHandle, fp->pMem, fp->dwFileSize);
	}
	else
	{
		// 申请一块临时空间用来解压
		LPVOID p = malloc(file_header.dwCompressedSize);
		if( !p )
		{
			free(fp->pMem);
			free(fp);
			ERR(_T("malloc failed in open file %s in zip"), szFileName);
			return NULL;
		}

		// 读入压缩信息
		m_pDiscIOMgr->Read(m_dwZipHandle, p, file_header.dwCompressedSize);

		// 反过滤（解压）文件
		tagFilterParam param;
		param.bCompress = FALSE;
		param.bSuccess = FALSE;
		param.dwInBufferSize = file_header.dwCompressedSize;
		param.dwInDataSize = file_header.dwCompressedSize;
		param.dwOutBufferSize = fp->dwFileSize;
		param.dwOutDataSize = fp->dwFileSize;
		param.eFilter = FMF_ZLibDeflated;
		param.lpMemIn = p;
		param.lpMemOut = fp->pMem;
		
		// 过滤文件信息
		m_pFilterMgr->Filter(&param);

		// 交还临时空间
		free(p);

		if( param.bSuccess == FALSE )
		{
			ERR(_T("uncompression %s failed in zip"), szFileName);
			free(fp->pMem);
			free(fp);
			return NULL;
		}
	}

	fp->bValid = TRUE;
	fp->dwPointer = 0;
	m_nOpenedFileNum++;
	return (DWORD)fp;
}



//-----------------------------------------------------------------------------
// close the file 
//-----------------------------------------------------------------------------
BOOL ZipReader::Close(DWORD dwHandle)
{
	tagFileInZip* fp = (tagFileInZip*)dwHandle;
	ASSERT(fp);

	if( !fp )
		return FALSE;

	free(fp->pMem);
	free(fp);
	m_nOpenedFileNum--;

	return TRUE;
}



//-----------------------------------------------------------------------------
// read the file 
//-----------------------------------------------------------------------------
DWORD ZipReader::Read(PVOID lpMemOut, DWORD dwSize, DWORD dwHandle)
{
	tagFileInZip* fp = (tagFileInZip*)dwHandle;
	ASSERT(fp != NULL);
	ASSERT(fp->bValid);

	if( fp->dwPointer + dwSize > fp->dwFileSize )
		dwSize = fp->dwFileSize -  fp->dwPointer;

	if( fp->dwFileSize >= fp->dwPointer + dwSize )	// 防止越界
		memcpy(lpMemOut, (BYTE*)fp->pMem + fp->dwPointer, dwSize);

	fp->dwPointer += dwSize;
	
	// 必须返回实际"成功"读取的字节数
	return dwSize;
}


//-----------------------------------------------------------------------------
// 定位包中单个文件的指针,返回当前文件指针位置
//-----------------------------------------------------------------------------
DWORD ZipReader::Seek(DWORD dwHandle, INT nOffset, DWORD dwOrigin)
{
	tagFileInZip* fp = (tagFileInZip*)dwHandle;
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
DWORD ZipReader::Tell(DWORD dwHandle)
{
	return ((tagFileInZip*)dwHandle)->dwPointer;
}


//-----------------------------------------------------------------------------
// 读包中单个文件到目标内存，返回原文件字节大小，
// 与Open+Read相比因为直接读入指定内存，少了一次Memcpy
// 如果内存目标lpMemOut为空，就仅返回原文件(未压缩时)大小
//-----------------------------------------------------------------------------
DWORD ZipReader::LoadFile(LPVOID lpMemOut, LPCTSTR szFileName)
{
	if( !m_bLoaded )
		return (DWORD)GT_INVALID;

	ASSERT(szFileName);
	INT n=0;

	DWORD dwCrc = m_pUtil->Crc32(szFileName);

	// 搜索CRC符合的文件
	tagZipFileItem* pItem;
	for( n=0; n<(INT)m_dwFileNumInZip; n++ )
	{
		pItem = &((tagZipFileItem*)m_lpFileTable)[n];
		if( pItem->dwFileNameCrc32 == dwCrc )
		{
			goto _load_founded;
		}
	}

	// 未找到指定文件
	if( NULL != lpMemOut )	// 外部只是想取得文件大小，则不产生明显提示
	{
		ERR(_T("could not find file %s in zip"), szFileName);
	}
	return (DWORD)GT_INVALID;

_load_founded:

	// 读入此文件头信息
	tagZipLocalFileHeader file_header;
	m_pDiscIOMgr->Seek(m_dwZipHandle, pItem->dwFileHeaderOffset);
	m_pDiscIOMgr->Read(m_dwZipHandle, &file_header, sizeof(file_header));

	// 将文件指针移动到文件真正信息处
	m_pDiscIOMgr->Seek(m_dwZipHandle, file_header.wFileNameLength + 
		file_header.wExtraFieldLength, GT_SEEK_CUR);

	// 我们不处理加密文件
	if( file_header.wGeneralPurposeBitFlag & 1 )
	{
		ERR(_T("couldn't handle encrypted file %s in zip"), szFileName);
		return (DWORD)GT_INVALID;
	}

	// 压缩方法不能识别
	if( file_header.wCompressionMethod != 0	// 未压缩
		&& file_header.wCompressionMethod != 8 )	// Deflated
	{
		ERR(_T("couldn't handle %s's compression method in zip"), szFileName);
		return (DWORD)GT_INVALID;
	}

	// 申请原本文件信息的空间
	if( lpMemOut == NULL )
		return file_header.dwUncompressedSize;

	if( file_header.wCompressionMethod == 0	)// 未压缩
	{
		m_pDiscIOMgr->Read(m_dwZipHandle, lpMemOut, file_header.dwUncompressedSize);
	}
	else
	{
		// 申请一块临时空间用来解压
		LPVOID p = malloc(file_header.dwCompressedSize);
		if( !p )
		{
			ERR(_T("malloc failed in open file %s in zip"), szFileName);
			return (DWORD)GT_INVALID;
		}

		// 读入压缩信息
		m_pDiscIOMgr->Read(m_dwZipHandle, p, file_header.dwCompressedSize);

		// 反过滤（解压）文件
		tagFilterParam param;
		param.bCompress = FALSE;
		param.bSuccess = FALSE;
		param.dwInBufferSize = file_header.dwCompressedSize;
		param.dwInDataSize = file_header.dwCompressedSize;
		param.dwOutBufferSize = file_header.dwUncompressedSize;
		param.dwOutDataSize = file_header.dwUncompressedSize;
		param.eFilter = FMF_ZLibDeflated;
		param.lpMemIn = p;
		param.lpMemOut = lpMemOut;
		
		// 过滤文件信息
		m_pFilterMgr->Filter(&param);

		// 交还临时空间
		free(p);

		if( param.bSuccess == FALSE )
		{
			ERR(_T("uncompression %s failed in zip"), szFileName);
			return (DWORD)GT_INVALID;
		}
	}

	return file_header.dwUncompressedSize;
}


//-----------------------------------------------------------------------------
// 全小写给定字符串，将所有斜杠改成反斜杠，并为尾部添加结束符"\0"
//-----------------------------------------------------------------------------
VOID ZipReader::HandleFileNameString(LPTSTR szString, DWORD dwLength)
{
	lstrcpy(szString, m_pUtil->Unicode8ToUnicode((LPCSTR)szString));
	for( INT n=0; n<(INT)dwLength; n++ )
	{
		if( szString[n] == _T('/') )
			szString[n] = _T('\\');
	}

	szString[dwLength] = _T('\0');
	_tcslwr(szString);
}



}