//-----------------------------------------------------------------------------
// File: ts_memmgr
// Desc: game tool mem manager 1.0
// Auth: Lyp
// Date: 2004-2-26
//
// Copyright (c) 2004 CTCRST Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "ts_memmgr.h"

// 缓存大小规格
static CONST DWORD f_dwTSCacheSize[TSMMCT_End] = 
{ 1024, 64*1024, 256*1024, 1024*1024, 4096*1024, 8192*1024 };

static CONST INT   f_nTSCachePreCreate[TSMMCT_End] = 
{ 16, 2, 1, 0, 0, 0 };	// 预先每种缓冲个数

//-----------------------------------------------------------------------------
// construction / destruction
//-----------------------------------------------------------------------------
CTSMemMgr::CTSMemMgr()
{
	ZeroMemory( m_Pool, sizeof(m_Pool) );
	InitializeCriticalSection(&m_PoolLock);

	for( INT n=0; n<TSMMCT_End; n++ )
	{
		for( INT m=0; m<f_nTSCachePreCreate[n]; m++ )
		{
			m_Pool[n][m].pMem = malloc(f_dwTSCacheSize[n]);
			if( m_Pool[n][m].pMem != NULL)
			{
				m_Pool[n][m].bAllocated = TRUE;
				m_Pool[n][m].dwSize = f_dwTSCacheSize[n];
			}
		}
	}
}


CTSMemMgr::~CTSMemMgr()
{
	DeleteCriticalSection(&m_PoolLock);

	for( int n=0; n<TSMMCT_End; n++ )
	{
		for( int m=0; m<GT_MAX_TS_CACHE_PER_TYPE; m++ )
		{
			if( m_Pool[n][m].bAllocated )
			{
				free(m_Pool[n][m].pMem);
				m_Pool[n][m].bAllocated = FALSE;
			}
		}
	}

}




//-----------------------------------------------------------------------------
// 申请内存
//-----------------------------------------------------------------------------
PVOID CTSMemMgr::Alloc(DWORD dwBytes)
{
	PVOID pMem = NULL;
	ETSMM_CACHE_TYPE eMemType = TSMMCT_End;
	
	// 搜索申请内存大小类型
	ASSERT( dwBytes <= f_dwTSCacheSize[TSMMCT_End-1] );
	for( INT n=0; n<TSMMCT_End; n++ )
	{
		if( dwBytes <= f_dwTSCacheSize[n] )
		{
			eMemType = (ETSMM_CACHE_TYPE)n;
			break;
		}
	}

	if( eMemType < TSMMCT_End )
	{
		// 在此进入锁定区
		EnterCriticalSection(&m_PoolLock);
		
		// 检查空闲的缓存
		for( INT n=0; n<GT_MAX_TS_CACHE_PER_TYPE; n++ )
		{
			if( FALSE == m_Pool[eMemType][n].bBeUsed )
			{
				if( FALSE == m_Pool[eMemType][n].bAllocated )
					m_Pool[eMemType][n].pMem = 
					malloc(f_dwTSCacheSize[eMemType]);
				
				if( m_Pool[eMemType][n].pMem )
				{
					pMem = m_Pool[eMemType][n].pMem;
					m_Pool[eMemType][n].bAllocated = TRUE;
					m_Pool[eMemType][n].bBeUsed = TRUE;
					m_Pool[eMemType][n].dwSize = f_dwTSCacheSize[eMemType];
					m_Pool[eMemType][n].bEverBeenUsed = TRUE;
					m_Pool[eMemType][n].dwUsage = dwBytes;
					m_Pool[eMemType][n].dwUseTime ++;
					if( dwBytes > m_Pool[eMemType][n].dwMaxUsage )
						m_Pool[eMemType][n].dwMaxUsage = dwBytes;
					
					break;
				}
			}
		}
		// 离开锁定区
		LeaveCriticalSection(&m_PoolLock);
	}
	if( NULL == pMem )
		pMem = malloc(dwBytes);

	return pMem;
}
//-----------------------------------------------------------------------------
// 释放内存
//-----------------------------------------------------------------------------
VOID CTSMemMgr::Free(VOID* pMem)
{
	ASSERT(pMem);

	// 搜索对应缓存
	for( int n=0; n<TSMMCT_End; n++ )
	{
		for( int m=0; m<GT_MAX_TS_CACHE_PER_TYPE; m++ )
		{
			if( m_Pool[n][m].pMem == pMem )
			{
				m_Pool[n][m].dwUsage = 0;
				
				// 在多线程多CPU系统下必须这样来保证CPU-Cache同步
				InterlockedExchange((LPLONG)&m_Pool[n][m].bBeUsed, FALSE);
				return;
			}
		}
	}

	free( pMem );	// not found

	return;
}

