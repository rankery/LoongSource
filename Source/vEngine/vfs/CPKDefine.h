//-----------------------------------------------------------------------------
// File: CPKDefine.h
// Desc: game resource file system 1.0  system define
// Time: 2003-1-2
// Auth: CTCRST
//
//-----------------------------------------------------------------------------
#pragma once
namespace vEngine{

const int CPK_MAX_PATH			= 260;

const long POLYNOMIAL			= 0x04C11DB7L;	// CRC种子
const long GT_CPK_LABEL			= 0x1A545352L;
const long GT_CPK_MAGIC			= GT_CPK_LABEL;
const long GT_CPK_VERSION		= 1;
const long GT_CPK_MAX_FILE		= 0x0007FFFFL;


const long GT_CPK_RAW_BUFFER_SIZE	= (8L*1024*1024);
const long GT_CPK_PACK_BUFFER_SIZE	= ((8L*1024*1024) + (8L*1024*1024/64) + 16 + 3);
const long GT_CPK_LEN_OF_STRING_END	= 2;


// table : 文件表
// block : 文件数据块
//-----------------------------------------------------------------------------
// base struct
//-----------------------------------------------------------------------------
#pragma pack(push, pack_1)
#pragma pack(4)

// CPK文件头结构
struct tagCPKHeader
{
	DWORD	dwLable;		// 类型标志
	DWORD	dwVersion;		// 版本
	DWORD	dwTableStart;	// 文件表起始字节
	DWORD	dwDataStart;	// 数据块起始地址
	DWORD	dwFileNum;		// 当前文件个数	
	DWORD	dwIsFormatted;	// 文件是否被整理过
	DWORD	dwSizeOfHeader;	// 文件头大小
	DWORD	dwValidTableNum;// 有效Table项个数，包括有效文件和碎片
	DWORD	dwMaxTableNum;	// 最大Table项个数
	DWORD	dwFragmentNum;	// 碎片数目，应该为dwValidTableNum - dwFileNum
	DWORD	dwPackageSize;	// 当前包大小
	DWORD	dwReserved[20];	
}; // 32*4




#define IS_VALID_CPK_TABLE(n) (n>=(INT)0 && n<(INT)m_CPKHeader.dwMaxFileNum)

// CPK文件表结构
struct tagCPKTable
{
	DWORD	dwCRC;				// CRC较验
	DWORD	dwFlag;				// 标志
	DWORD	dwFatherCRC;		// 父目录CRC
	DWORD	dwStartPos;			// 开始位置
	DWORD	dwPackedSize;		// 压缩后长度
	DWORD	dwOriginSize;		// 原文件长度
	DWORD	dwExtraInfoSize;	// 附加信息长度
};


// DWORD dwFlag:
// 0000 0000-0000 0000-0000 0000-0000 0001 (0x0001)有效
// 0000 0000-0000 0000-0000 0000-0000 0010 (0x0002)目录
// 0000 0000-0000 0000-0000 0000-0000 0100 (0x0004)大文件
// 0000 0000-0000 0000-0000 0000-0001 0000 (0x0010)文件已被删除
// 0000 0000-0000 0000-1000 0000-0000 0000 (0x8000)CRC有重复
// 高十六位为过滤器编号

#define IS_FILE_VALID(f)		((f) & 0x00000001)
#define IS_DIR(f)				((f) & 0x00000002)
#define IS_LARGE_FILE(f)		((f) & 0x00000004)
#define IS_FILE_CRC_REPEAT(f)	((f) & 0x00008000)
#define GET_FILTER(f)			((EFM_FILTER)(((f) & 0xffff0000) >> 16))
#define IS_COMPRESSED(f)		(GET_FILTER(f) != FMF_Changeless)


// 当文件被删除时，仅作一下标记，此表项就变成了空间碎片
// 以后加入新文件时，先找这些空间碎片，找到了插入空间
// 注意并非是表项空间，而是包中信息空间。
// 只有当这些表项空间所指向的碎片小于一定程度时，
// 表项空间才会被释放
#define IS_FILE_REMOVED(f)		((f) &  0x00000010)
#define SET_FILE_REMOVED(f)		((f) |= 0x00000010)

// 检查表项是否代表有效文件或者目录，而并非空间碎片
#define IS_FILE_OR_DIR_VALID(f) (((f) & 0x00000001) && !IS_FILE_REMOVED(f))



// 除文件本体外的文件附加信息
struct tagFileExtraInfo
{
	FILETIME	CreateTime;			// 建立时间
	FILETIME	LastAccessTime;		// 最后的访问时间
	FILETIME	LastWriteTime;		// 最后的修改时间
	FILETIME	AddToPackageTime;	// 加入包的时间
	FILETIME	DelFromPackageTime;	// 从包中删去的时间
	DWORD		dwReserved[4];		// 保留
};


#ifdef  MAX_PATH
#undef  MAX_PATH
#endif
// 定义一个安全的MAX_PATH大小
#define MAX_PATH (_MAX_PATH + sizeof(tagFileExtraInfo) + 8)

#pragma pack(pop, pack_1)
}