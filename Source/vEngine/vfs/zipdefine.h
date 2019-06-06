//-----------------------------------------------------------------------------
// File: zipreader
// Time: 2004-2-19
// Auth: CTCRST
//
//-----------------------------------------------------------------------------
#pragma once
namespace vEngine{

/* ZIP文件格式大体结构(Overall zipfile format):
[local file header1] <- 我们所关心的
[file data 1] <- 我们所关心的
[data_descriptor 1]
.
.
.
[local file header n]
[file data n]
[data_descriptor n]
[central directory]	<- 我们所关心的
[zip64 end of central directory record]
[zip64 end of central directory locator]
[end of central directory record] <- 我们所关心的
*/

#pragma pack(push)
#pragma pack(1)
struct tagZipLocalFileHeader
{
	DWORD	dwSignature;
	WORD	wVersionNeededToExtract;
	WORD	wGeneralPurposeBitFlag;
	WORD	wCompressionMethod;
	WORD	wLastModifyTime;
	WORD	wLastModifyDate;
	DWORD	dwCrc32;
	DWORD	dwCompressedSize;
	DWORD	dwUncompressedSize;
	WORD	wFileNameLength;
	WORD	wExtraFieldLength;
};


struct tagZipCentralDirectoryFileHeader
{
	DWORD	dwSignature;
	WORD	wVersionMadeBy;
	WORD	wVersionNeededToExtract;
	WORD	wGeneralPurposeBitFlag;
	WORD	wCompresssionMethod;
	WORD	wLastModifyTime;
	WORD	wLastModifyDate;
	DWORD	dwCrc32;
	DWORD	dwCompressedSize;
	DWORD	dwUncompressedSize;
	WORD	wFileNameLength;
	WORD	wExtraFieldLength;
	WORD	wFileCommentLength;
	WORD	wDiskNumberStart;
	WORD	wInternalFileAttrib;
	DWORD	dwExternalFileAttrib;
	DWORD	dwRlativeOffsetOfLocalHeader;
/*	VOID	FileName;		// file name (variable size) 
	VOID	ExtraField;		// extra field (variable size) 
	VOID	FileComment;	// file comment (variable size) */
};

const unsigned long f_dwSignatureOfZipEndOfCentralDirRecord = 0x06054b50L;
struct tagZipEndOfCentralDirRecord
{
	DWORD	dwSignature;
	WORD	wNumberOfThisDisk;
	WORD	wStartNumberOfTheDisk;
	WORD	wTotalEntriesOnTheDisk;
	WORD	wTotalEntries;
	DWORD	dwSizeOfTheCentralDir;
	DWORD	dwOffsetOfTheCentralDir;
	WORD	wZipFileCommentLength;
//	VOID	ZipFileComment;	(variable size) 
};

#pragma pack(pop)



// 我们自己定义的ZIP中的文件项目结构
struct tagZipFileItem
{
	DWORD	dwFileNameCrc32;
	DWORD	dwFileHeaderOffset;
};


struct tagFileInZip
{
	PVOID	pMem;	// 内存开始位置

	// for seek/tell
	DWORD	dwFileSize;		// 文件实际内容大小
	DWORD	dwPointer;		// 文件指针

	BOOL	bValid;			// 是否有效
};

}