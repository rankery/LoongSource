//-----------------------------------------------------------------------------
// File: CPKCore.h
// Desc: game resource file system 2.0
// Time: 2003-5-12
// Auth: CTCRST
//
// Copyright (c) 2003 Softstar Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#ifndef GT_CPK_CORE
#define GT_CPK_CORE
#pragma once

#include "cpkdefine.h"

class CPK;
class CPKMgr;
class CMD5;
//-----------------------------------------------------------------------------
// 单包管理
//-----------------------------------------------------------------------------
class CPK
{
public:
//-----------------------------------------------------------------------------
// 标准函数
//-----------------------------------------------------------------------------
	// 创建新的管理包
	BOOL Create(LPCTSTR pFileName);
	BOOL Load(LPCTSTR pFileName);
	BOOL Unload();
	BOOL IncreaseTable();	//增加表项空间

	// 检查是否是合法的CPK文件包
	BOOL IsValidCPK(LPCTSTR pFileName);
	
	//添加文件
	BOOL AddFile(LPCTSTR pFullName, DWORD dwFatherCRC);
	BOOL AddFile(LPCTSTR pFullName, DWORD dwFatherCRC, tagCPKTable *pTable, LPVOID p);
	//这种方式只添加他的路径，根路径作为dwFatherCrc的子目录．为其它操作做准备
	DWORD AddDir(LPCTSTR pFullName, DWORD dwFatherCRC);
	//与上面的函数不同,这种方式添加文件他的路径需要保存,根路径作为dwFatherCrc的子目录
	BOOL AddFileAndDir(LPCTSTR pFullName, DWORD dwFatherCRC=0);
	BOOL RemoveFile(LPCTSTR pFullName);
	BOOL ExtractFile(LPCTSTR pDestDir, LPCTSTR pFullName);

	// 设置包中的当前目录
	BOOL  SetCurDir(LPCTSTR pFullDirName);
	// 得到包中当前目录的CRC,如果已知目录名，可由GetCRCFromCPKFullName得到CRC
	DWORD GetCurDirCRC() { return m_dwCurDirCRC; }

	// 得到包中指定文件或者目录的父目录CRC
	DWORD GetFatherCRC(DWORD dwCRC);
	// 得到包中指定目录下的所有一级子文件
	BOOL GetFirstLevelChild(DWORD dwDirCRC, DWORD& dwChildNum, DWORD* pdwCRCArray=NULL);
	// 得到包中指定目录下的所有一级子目录
	BOOL GetFirstLevelChildDir(DWORD dwDirCRC, DWORD& dwChildNum, DWORD* pdwCRCArray=NULL);

	// 由绝对路径带文件名得到包中CRC标识，如果包中不存在此文件则返回FALSE
	BOOL GetCRCFromCPKFullName(DWORD& dwCRC, LPCTSTR pFullName);
	// 由CRC标识得到简单文件名
	BOOL GetSimpleFileNameFromCRC(LPTSTR pSimpleFileName, DWORD dwCRC);
	// 由表项序号得到CPK包中该文件的md5校验值
	BOOL GetMD5FromTableIndex(BYTE digest[16], DWORD dwTableIndex);
	// 由CRC标识得到CPK包中该文件的md5校验值
	BOOL GetMD5FromCrc(BYTE digest[16], DWORD dwCrc);
	// 由CRC标识得到CPK包中带路径文件名
	BOOL GetFullNameFromCRC(LPSTR pFullName, DWORD dwCRC);
	// 由CRC标识和FatherCRC得到CPK包中带相对路径的文件名
	BOOL GetRelativeNameFromCRC(LPSTR pNameOut, DWORD dwCRC, DWORD dwFatherCRC);
	
	// 得到文件大小
	BOOL GetFileSize(DWORD& dwPackSize, DWORD& dwOriginSize, DWORD dwCRC);
	// 由CRC标识得到对象是否是目录
	BOOL IsDir(DWORD dwCRC);
	// 检查是否加载了某个包
	BOOL IsLoaded() { return m_bLoaded; }

	DWORD GetMapStartPos(DWORD dwCRC);

	//创建ｍａｐ
	DWORD	CreateMap();
	//关闭map_
	VOID	CloseMap(DWORD dwFileMap);
	//影射包中的某个文件，取得文件内容的地址
	LPVOID	MapFile(DWORD dwFileMap, DWORD dwTableIndex, LPVOID &pMapAdd);
	//取消映射
	VOID	UnMapFile(LPVOID p);

	CPK();
	~CPK();
public:
	tagCPKHeader*	GetCPKHeader() { return &m_CPKHeader;	}
	tagCPKTable*	GetCPKTable()  { return m_pCPKTable; }

//-----------------------------------------------------------------------------
// 非标优化函数
//-----------------------------------------------------------------------------

	// 得到包中指定目录下的所有一级子目录和文件
	BOOL _GetFirstLevelChild(DWORD dwDirCRC, DWORD& dwChildNum);

	// 由TableIndex得到简单文件名
	__forceinline BOOL _GetSimpleFileName(LPTSTR pFileName, INT nTableIndex)
	{
		return GetSimpleFileName(pFileName, nTableIndex);
	}
	
	// 得到CPKTable
	__forceinline tagCPKTable _GetTable(INT nIndex)
	{ 
		ASSERT(IS_VALID_CPK_TABLE(nIndex));
		return m_pCPKTable[nIndex];
	}

	// 得到临时数组中的数据
	__forceinline INT _GetTempArray(INT nIndex)
	{
		ASSERT(IS_VALID_CPK_TABLE(nIndex));
		return m_nTempArray[nIndex];	
	}
	// 由CRC找到对应表项
	__forceinline INT _GetTableIndexFromCRC(DWORD dwCRC)
	{
		return GetTableIndexFromCRC(dwCRC);
	}

protected:

	CMD5		*m_pMD5;
	// 与包中信息有关
	tagCPKHeader m_CPKHeader;	// 文件头
	tagCPKTable *m_pCPKTable;	//表项空间
	
	DWORD	m_dwMaxCPKTable;
	BOOL	m_bLoaded;	// 是否有CPK载入, 如果没有，则所有操作无效
	DWORD	m_dwCPKHandle;
	TCHAR	m_szCPKFileName[MAX_PATH];
	TCHAR	m_szCurDir[MAX_PATH];
	DWORD	m_dwCurDirCRC;

	DWORD	m_dwAllocGranularity;
	
	// 压缩解压工作缓存
	LPVOID	m_pRawBuffer;	// 为写模式开的缓存
	LPVOID	m_pPackBuffer;	// 为压缩准备的缓存
	DWORD	m_dwRawBufferSize;	// 为写模式开的缓存的大小
	DWORD	m_dwPackBufferSize; // 为压缩准备的缓存的大小

	INT*	m_pFragment;	// 记录碎片所属的Table项
	INT*	m_pIndexToFragment;	// 记录Table所属的碎片项

	DWORD	m_dwCPKHeaderAddr;			//CPK系统头文件所在文件地址，add by clj


	// 用来显示文件处理进度
	DWORD	m_dwFilesToProceed;		// 需要处理的文件数
	DWORD	m_dwFilesCurProceed;	// 已经处理的文件数

	// 主要用来给_GetFirstLevelChild使用
	INT		m_nTempArray[GT_CPK_DEFAULT_FILE];	

	VOID Reset();


public:
	// 根据所给的CRC得到空闲的Table项
	INT	GetFreeTableItem(DWORD dwCRC);	
	// 得到数据区空闲区域
	DWORD FindCPKFreePlace(DWORD dwSize);	
	// 检查是否包中有与所给CRC重复的CRC
	BOOL HaveRepeatCRC(DWORD dwCRC);

	// 在包里指定目录创建一个空目录, 返回CRC
	DWORD CreateDir(LPCTSTR pSimpleDirName, DWORD dwFatherCRC);
	// 添加单个文件到指定目录
	BOOL AddSingleFile(LPCTSTR pFullName, DWORD dwFatherCRC);
	// 添加文件或目录到指定目录，包括其下的所有子项，处理递归
	BOOL AddFileOrDir(LPCTSTR pFullName, DWORD dwFatherCRC, BOOL bDir);

	// 释放单个文件到指定目录
	BOOL ExtractSingleFile(LPCTSTR pDestDir, DWORD dwCRC);
	// 释放文件或目录到指定目录，包括其下的所有子项，处理递归
	BOOL ExtractFileOrDir(LPCTSTR pDestDir, DWORD dwCRC, DWORD dwFatherSrcCRC);

	// 删除文件或目录，会删除其下所有子项，处理递归
	BOOL RemoveFileOrDir(INT nTableIndex);

	// 由磁盘全名转为包中文件名，必须知道父CRC
	BOOL ConvertFullNameToCPKFullName(LPTSTR pOutName, LPCTSTR pInName, DWORD dwFatherCRC);
	// 由任意文件名得到简单文件名(不带路径)
	BOOL GetSimpleFileName(LPTSTR pSimpleName, LPCTSTR pFullName);
	// 连接简单文件名或目录名到目标目录名
	BOOL LinkSimpleNameToDirName(LPTSTR pFullName, LPCTSTR pSimpleName);
	// 得到包中文件的扩展名(带"."号)
	BOOL GetFileExtName(LPTSTR pExtName, INT nTableIndex);
	// 从包中得到简单文件名(没有路径)
	BOOL GetSimpleFileName(LPTSTR pFileName, INT nTableIndex);
	
	// 计算附加信息长度
	DWORD CalFileExtraInfoSize(LPCTSTR pFileName);

	// 向文件中插入数据
	BOOL AddRealDataToCPK(INT nTableIndex, LPCTSTR pFileName,  VOID* pSrc, 
		DWORD dwSrcFileSize, BYTE *pDigest=NULL);


	// 根据文件名找到对应的表项
	INT GetTableIndex(LPCTSTR pFullName);
	// 由CRC找到对应表项
	INT GetTableIndexFromCRC(DWORD dwCRC);

	// 在Load Package时调用一次以便得到所有碎片信息
	INT GetAllFragment();

	BOOL WriteHeaderToDisc();
	BOOL WriteTableToDisc();

	// 是否是不可压缩文件  mp3/bik
	BOOL IsUnCompressable(LPCTSTR pFullName);

	// 得到内存分配尺度
	DWORD GetAllocationGranularity();

};













#endif // GT_PACK_CORE