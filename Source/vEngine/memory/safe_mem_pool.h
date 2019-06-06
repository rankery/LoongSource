//-----------------------------------------------------------------------------
// File: safe_mem_pool
// Desc: game tool mem pool 2.0
// Auth: Lyp
// Date: 2009-1-8	
// Last: 2009-3-18
//-----------------------------------------------------------------------------
#pragma once
namespace vEngine {
//-----------------------------------------------------------------------------
// 内存池(注意：内存池的大小表示:内存池当前空闲内存的大小)
// 初始化时：
// 1 外部设定内存池的最大允许大小
// 2 内存池不做任何内存预分配
//
// 外部申请内存时,依次判断：
// 1 如果内存池有对应尺寸的空闲块,使用空闲块返回
// 2 没有空闲块时,直接申请内存并在内存前加入tagMemItem,然后返回
//
// 外部释放内存时,依次判断:
// 1 如果释放内存块大小>内存池最大大小,直接释放到系统内存
// 2 如果当前内存池大小+释放内存块大小<内存池最大大小,直接放入内存池
// 3 进行垃圾收集：垃圾收集后重新检查上面的第2步，如果不通过，则进行第4步
// 4 否则直接释放入系统内存
//-----------------------------------------------------------------------------
class VENGINE_API SafeMemPool
{
public:
	__forceinline LPVOID	Alloc(DWORD dwBytes);
	__forceinline VOID		Free(LPVOID pMem);
	__forceinline LPVOID	TryAlloc(DWORD dwBytes);	// 尝试进入锁定区域
	__forceinline BOOL		TryFree(LPVOID pMem);		// 尝试进入锁定区域
	__forceinline VOID		SetMaxSize(DWORD dwSize) { m_dwMaxSize = dwSize; }
	__forceinline DWORD		GetSize() { return m_dwCurrentSize; }
	__forceinline DWORD		GetMalloc() { return m_dwMalloc; }
	__forceinline DWORD		GetGC() { return m_dwGCTimes; }

	SafeMemPool(DWORD dwMaxPoolSize=16*1024*1024);
	~SafeMemPool();

private:
	struct tagNode	// 内存块头描述
	{
		tagNode*	pNext;
		tagNode*	pPrev;
		INT			nIndex;
		DWORD		dwSize;
		DWORD		dwUseTime;
		DWORD		pMem[1];	// 实际内存空间
	};

	struct
	{
		tagNode*	pFirst;
		tagNode*	pLast;
	} m_Pool[16];

	DWORD m_dwMaxSize;		// 外部设定的最大允许空闲内存
	DWORD m_dwMalloc;			// 统计用，实际Malloc次数
	DWORD m_dwGCTimes;		// 统计用，实际垃圾收集次数

	DWORD VOLATILE 	m_dwCurrentSize;	// 内存池中空闲内存总数
	LONG VOLATILE 	m_lLock;

	__forceinline VOID Lock() { while(InterlockedCompareExchange((LPLONG)&m_lLock, 1, 0)!= 0) Sleep(0); }
	__forceinline VOID Unlock() { InterlockedExchange((LPLONG)(&m_lLock), 0); }
	__forceinline bool TryLock() { return InterlockedCompareExchange((LPLONG)(&m_lLock), 1, 0) == 0;	}

	// 垃圾收集
	__forceinline VOID GarbageCollect(DWORD dwExpectSize, DWORD dwUseTime);
	// 返回最匹配的大小
	__forceinline INT SizeToIndex(DWORD dwSize, DWORD& dwRealSize);
};


//-----------------------------------------------------------------------------
// 分配
//-----------------------------------------------------------------------------
LPVOID SafeMemPool::Alloc(DWORD dwBytes)
{
	DWORD dwRealSize = 0;
	INT nIndex = SizeToIndex(dwBytes, dwRealSize);
	if( GT_INVALID != nIndex )
	{
		if( m_Pool[nIndex].pFirst )	// 提前尝试
		{
			Lock();
			if( m_Pool[nIndex].pFirst )	// 池里有，就从池里分配
			{
				tagNode* pNode = m_Pool[nIndex].pFirst;
				m_Pool[nIndex].pFirst = m_Pool[nIndex].pFirst->pNext;
				if( m_Pool[nIndex].pFirst )
					m_Pool[nIndex].pFirst->pPrev = NULL;
				else
					m_Pool[nIndex].pLast = NULL;
				m_dwCurrentSize -= dwRealSize;
				++pNode->dwUseTime;
				Unlock();
				return pNode->pMem;	
			}
			Unlock();
		}
	}

	++m_dwMalloc;
	tagNode* pNode = (tagNode*)malloc(dwRealSize + sizeof(tagNode) - sizeof(DWORD));
	if( !pNode )
		return NULL;

	pNode->nIndex = nIndex;
	pNode->dwSize = dwRealSize;
	pNode->pNext = NULL;
	pNode->pPrev = NULL;
	pNode->dwUseTime = 0;
	return pNode->pMem;	// 从实际内存中分配
}


//-----------------------------------------------------------------------------
// 释放
//-----------------------------------------------------------------------------
VOID SafeMemPool::Free(LPVOID pMem)
{
	tagNode* pNode = (tagNode*)(((LPBYTE)pMem) - sizeof(tagNode) + sizeof(DWORD));
	if( GT_INVALID != pNode->nIndex )
	{
		Lock();
		if( pNode->dwSize + m_dwCurrentSize > m_dwMaxSize && pNode->dwUseTime > 0 )
			GarbageCollect(pNode->dwSize*2, pNode->dwUseTime);	// 垃圾收集

		if( pNode->dwSize + m_dwCurrentSize <= m_dwMaxSize ) // 内存池可以容纳
		{
			pNode->pPrev = NULL;
			pNode->pNext = m_Pool[pNode->nIndex].pFirst;
			if( m_Pool[pNode->nIndex].pFirst )
				m_Pool[pNode->nIndex].pFirst->pPrev = pNode;
			else
				m_Pool[pNode->nIndex].pLast = pNode;

			m_Pool[pNode->nIndex].pFirst = pNode;
			m_dwCurrentSize += pNode->dwSize;
			Unlock();
			return;
		}
		Unlock();
	}

	free(pNode);
}


//-----------------------------------------------------------------------------
// 分配
//-----------------------------------------------------------------------------
LPVOID SafeMemPool::TryAlloc(DWORD dwBytes)
{
#if _WIN32_WINNT < 0x0400
	return Alloc(dwBytes);
#else
	DWORD dwRealSize = 0;
	INT nIndex = SizeToIndex(dwBytes, dwRealSize);
	if( GT_INVALID != nIndex )
	{
		if( !TryLock() )
			return NULL;

		if( m_Pool[nIndex].pFirst )	// 池里有，就从池里分配
		{
			tagNode* pNode = m_Pool[nIndex].pFirst;
			m_Pool[nIndex].pFirst = m_Pool[nIndex].pFirst->pNext;
			if( m_Pool[nIndex].pFirst )
				m_Pool[nIndex].pFirst->pPrev = NULL;
			else
				m_Pool[nIndex].pLast = NULL;
			m_dwCurrentSize -= dwRealSize;
			++pNode->dwUseTime;
			Unlock();
			return pNode->pMem;	
		}
		Unlock();
	}

	++m_dwMalloc;
	tagNode* pNode = (tagNode*)malloc(dwRealSize + sizeof(tagNode) - sizeof(DWORD));
	if( !pNode )
		return NULL;

	pNode->nIndex = nIndex;
	pNode->dwSize = dwRealSize;
	pNode->pNext = NULL;
	pNode->pPrev = NULL;
	pNode->dwUseTime = 0;
	return pNode->pMem;	// 从实际内存中分配
#endif


}


//-----------------------------------------------------------------------------
// 释放
//-----------------------------------------------------------------------------
BOOL SafeMemPool::TryFree(LPVOID pMem)
{
#if _WIN32_WINNT < 0x0400
	Free(pMem);
	return TRUE;
#else
	tagNode* pNode = (tagNode*)(((LPBYTE)pMem) - sizeof(tagNode) + sizeof(DWORD));
	if( GT_INVALID != pNode->nIndex )
	{
		if( !TryLock() )
			return FALSE;

		if( pNode->dwSize + m_dwCurrentSize > m_dwMaxSize && pNode->dwUseTime > 0 )
			GarbageCollect(pNode->dwSize*2, pNode->dwUseTime);	// 垃圾收集

		if( pNode->dwSize + m_dwCurrentSize <= m_dwMaxSize ) // 内存池可以容纳
		{
			pNode->pPrev = NULL;
			pNode->pNext = m_Pool[pNode->nIndex].pFirst;
			if( m_Pool[pNode->nIndex].pFirst )
				m_Pool[pNode->nIndex].pFirst->pPrev = pNode;
			else
				m_Pool[pNode->nIndex].pLast = pNode;

			m_Pool[pNode->nIndex].pFirst = pNode;
			m_dwCurrentSize += pNode->dwSize;
			Unlock();
			return TRUE;
		}
		Unlock();
	}
	free(pNode);
	return TRUE;
#endif
}



//-----------------------------------------------------------------------------
// 垃圾收集
//-----------------------------------------------------------------------------
VOID SafeMemPool::GarbageCollect(DWORD dwExpectSize, DWORD dwUseTime)
{
	++m_dwGCTimes;
	DWORD dwFreeSize = 0;
	for(INT n=15; n>=0; --n)	// 从最大的开始回收
	{
		if( !m_Pool[n].pFirst )
			continue;

		tagNode* pNode = m_Pool[n].pLast; // 从最后开始释放，因为后面的Node使用次数少
		while( pNode )
		{
			tagNode* pTempNode = pNode;
			pNode = pNode->pPrev;
			if( pTempNode->dwUseTime < dwUseTime )	// 释放此节点
			{
				if( pNode )
					pNode->pNext = pTempNode->pNext;
				if( pTempNode->pNext )
					pTempNode->pNext->pPrev = pNode;
				if( m_Pool[n].pLast == pTempNode )
					m_Pool[n].pLast = pNode;
				if( m_Pool[n].pFirst == pTempNode )
					m_Pool[n].pFirst = pTempNode->pNext;

				m_dwCurrentSize -= pTempNode->dwSize;
				dwFreeSize += pTempNode->dwSize;
				free(pTempNode);
			}

			if( dwFreeSize >= dwExpectSize )
				return;
		}
	}
}


//-----------------------------------------------------------------------------
// 返回最匹配的大小
//-----------------------------------------------------------------------------
INT SafeMemPool::SizeToIndex(DWORD dwSize, DWORD& dwRealSize)
{
	if( dwSize<=32 )		{ dwRealSize = 32;			return 0; }
	if( dwSize<=64 )		{ dwRealSize = 64;			return 1; }
	if( dwSize<=128 )		{ dwRealSize = 128;			return 2; }
	if( dwSize<=256 )		{ dwRealSize = 256;			return 3; }
	if( dwSize<=512 )		{ dwRealSize = 512;			return 4; }
	if( dwSize<=1024 )		{ dwRealSize = 1024;		return 5; }
	if( dwSize<=2*1024 )	{ dwRealSize = 2*1024;		return 6; }
	if( dwSize<=4*1024 )	{ dwRealSize = 4*1024;		return 7; }
	if( dwSize<=8*1024 )	{ dwRealSize = 8*1024;		return 8; }
	if( dwSize<=16*1024 )	{ dwRealSize = 16*1024;		return 9; }
	if( dwSize<=32*1024 )	{ dwRealSize = 32*1024;		return 10; }
	if( dwSize<=64*1024 )	{ dwRealSize = 64*1024;		return 11; }
	if( dwSize<=128*1024 )	{ dwRealSize = 128*1024;	return 12; }
	if( dwSize<=256*1024 )	{ dwRealSize = 256*1024;	return 13; }
	if( dwSize<=512*1024 )	{ dwRealSize = 512*1024;	return 14; }
	if( dwSize<=1024*1024 )	{ dwRealSize = 1024*1024;	return 15; }
	dwRealSize = dwSize;
	return GT_INVALID;
}
} // namespace vEngine {
