//-----------------------------------------------------------------------------
// File: MemMgr
// Desc: game tool tread safe mem pool manager 1.0
// Auth: Lyp
// Date: 2003-1-8	
// Last: 2003-12-11
//
// Copyright (c) 2003 CTCRST Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

CONST INT GT_MAX_TS_CACHE_PER_TYPE = 32;

enum ETSMM_CACHE_TYPE	// 规格枚举
{
	TSMMCT_1k=0,
	TSMMCT_64k,
	TSMMCT_256k,
	TSMMCT_1024k,
	TSMMCT_4096k,
	TSMMCT_8192k,
	TSMMCT_End,
};

//-----------------------------------------------------------------------------
// 提供线程安全的内存池管理接口
//-----------------------------------------------------------------------------
class CTSMemMgr
{
public:

	VOID* Alloc(DWORD dwBytes);
	VOID  Free(VOID* pMem);

	CTSMemMgr();~CTSMemMgr();


private:

	// 每块缓冲的描述
	struct tagCache
	{
		PVOID pMem;
		DWORD dwSize;		// 缓冲大小
		DWORD dwUsage;		// 使用了多少
		DWORD dwMaxUsage;	// 最大使用空间
		DWORD dwUseTime;	// 总共使用次数
		
		BOOL bBeUsed;		// 现在被使用,注意此项必须为大写的BOOL
		BOOL bAllocated;	// 内存指针有效吗
		BOOL bEverBeenUsed;	// 曾被使用过
	};

	tagCache			m_Pool[TSMMCT_End][GT_MAX_TS_CACHE_PER_TYPE];
	CRITICAL_SECTION	m_PoolLock;

};
