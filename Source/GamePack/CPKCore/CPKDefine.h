//-----------------------------------------------------------------------------
// File: CPKDefine.h
// Desc: game resource file system 2.0  system define
// Time: 2003-5-12
// Auth: CTCRST
//
// Copyright (c) 2003 Softstar Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#ifndef GT_CPK_DEFINE
#define GT_CPK_DEFINE
#pragma once

#define GT_INVALID					-1
#define GT_INVALID_ID				0xFFFFFFFFL
								
#define GT_CPK_LABEL				0x1A545352L
#define GT_CPK_VERSION				1
#define GT_CPK_MAX_FILE				0x0007FFFFL
#define GT_CPK_MAX_OPEN_CPK			16
#define GT_CPK_MAX_OPEN_FILE		32

#define GT_CPK_DEFAULT_FILE			(5000)	// 每包初始容纳文件个数，即表项数
#define GT_CPK_TABLE_ADD			5000		//每次扩容的表项数

#define GT_CPK_RAW_BUFFER_SIZE		(8L*1024*1024)
#define GT_CPK_PACK_BUFFER_SIZE		((8L*1024*1024) + (8L*1024*1024/64) + 16 + 3)
#define GT_CPK_LEN_OF_STRING_END	2

#define INVALID_FILE_HANDLE (DWORD)INVALID_HANDLE_VALUE
#define INVALID_MAP_HANDLE (DWORD)INVALID_HANDLE_VALUE

// table : 文件表
// block : 文件数据块
// 1.当一个文件小于4KB时，不使用压缩
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


#define IS_VALID_CPK_TABLE(n) \
		(n>=(INT)0 && n<(INT)m_CPKHeader.dwMaxTableNum)



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

#define IS_TABLE_VALID(f)		((f) & 0x00000001)
#define IS_DIR(f)				((f) & 0x00000002)
#define IS_LARGE_FILE(f)		((f) & 0x00000004)
#define IS_FILE_CRC_REPEAT(f)	((f) & 0x00008000)
#define GET_FILTER(f)			((EFMFilter)(((f) & 0xffff0000) >> 16))
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

#define GT_SEEK_SET				0
#define GT_SEEK_CUR				1
#define GT_SEEK_END				2

// 除文件本体外的文件附加信息
struct tagFileExtraInfo
{
	FILETIME	CreateTime;			// 建立时间
	FILETIME	LastAccessTime;		// 最后的访问时间
	FILETIME	LastWriteTime;		// 最后的修改时间
	FILETIME	AddToPackageTime;	// 加入包的时间
	FILETIME	DelFromPackageTime;	// 从包中删去的时间
	BYTE		byMD5[16];			//该文件的md5校验值
};


struct CPKFile
{
	TCHAR	szFullName[MAX_PATH];
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

#ifdef  MAX_PATH
#undef  MAX_PATH
#endif
// 定义一个安全的MAX_PATH大小
#define MAX_PATH (_MAX_PATH + sizeof(tagFileExtraInfo) + 8)

#pragma pack(pop, pack_1)



//-----------------------------------------------------------------------------
// Error code
//-----------------------------------------------------------------------------
// For short
#define csi			const static int
#define css			const static TCHAR* const
#define ERR(m)	    do{CUtil::ErrMsg(MB_ICONERROR | MB_OK, \
						_T("Error : "#m"\nFile : %s\nLine : %d"), __FILE__, \
						__LINE__); \
						ASSERT(m>0); ASSERT(0);}while(0)

// 无错
csi CE_NOERROR						= 0x00000000;

// 内部逻辑相关
csi CE_CPK_ALREADY_LOADED			= 0x00000001;
csi CE_CPK_HASNOT_LOADED			= 0x00000002;
csi CE_UNKNOW_CPK_FORMAT			= 0x00000003;
csi CE_VERSION_UNSUPPROT			= 0x00000004;
csi CE_FUTURE_VERSION				= 0x00000005;
csi CE_CANNOT_LOAD_TABLE			= 0x00000006;
csi CE_CPK_WAS_NOT_FORMATTED		= 0x00000007;
csi CE_CANNOT_OPEN_MORE_FILES		= 0x00000008;
csi CE_CANNOT_FIND_FILE				= 0x00000009;	// 在包中找不到指定文件
csi CE_CANNOT_COMPRESS				= 0x0000000A;	// 不能进行压缩
csi CE_CANNOT_DECOMPRESS			= 0x0000000B;	// 不能进行解压


// 内存相关
csi CE_INVALID_ADDRESS				= 0x10000001;	// 非法内存地址
csi CE_CANNOT_CREATE_MAIN_HEAP		= 0x10000002;	// 不能建立主堆栈
csi CE_CANNOT_DESTROY_MAIN_HEAP		= 0x10000002;	// 不能销毁主堆栈
csi CE_CANNOT_ALLOC_MEM				= 0x10000004;	// 不能申请内存
csi CE_CANNOT_FREE_MEM				= 0x10000005;	// 不能释放内存
csi CE_CANNOT_REALLOC_MEM			= 0x10000006;	// 不能重新分配内存


// 磁盘操作相关
csi CE_CANNOT_OPEN_FILE				= 0x20000001;
csi CE_FILE_DOES_NOT_EXIST			= 0x20000002;
csi CE_CANNOT_READ_DISC_FILE		= 0x20000003;
csi CE_CANNOT_CREATE_FILE_MAPPING	= 0x20000004;
csi CE_MAPPING_ALREADY_EXISTS		= 0x20000005;
csi CE_CANNOT_MAP_FILE				= 0x20000006;	// MapViewOfFile失败
csi CE_CANNOT_UNMAP_FILE			= 0x20000007;	// UnmapViewOfFile失败


// 外部接口相关
csi CE_LZO_INIT_FAILED				= 0x30000001;	// lzo初始化失败


#endif // GT_CPK_DEFINE