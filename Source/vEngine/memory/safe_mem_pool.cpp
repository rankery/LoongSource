//-----------------------------------------------------------------------------
// File: safe_mem_pool
// Desc: game tool mem pool 2.0
// Auth: Lyp
// Date: 2009-1-8	
// Last: 2009-3-18
//-----------------------------------------------------------------------------
#include "..\stdafx.h"
#include "safe_mem_pool.h"

namespace vEngine {
//-----------------------------------------------------------------------------
// construction / destruction
//-----------------------------------------------------------------------------
SafeMemPool::SafeMemPool(DWORD dwMaxPoolSize)
{
	m_dwMaxSize = dwMaxPoolSize;
	m_dwCurrentSize = 0;
	m_dwMalloc = 0;
	m_dwGCTimes = 0;
	m_lLock = 0;
	ZeroMemory(m_Pool, sizeof(m_Pool));
}


SafeMemPool::~SafeMemPool()
{
	for( int n=0; n<16; n++ )
	{
		while( m_Pool[n].pFirst )
		{
			tagNode* pNode = m_Pool[n].pFirst;
			m_Pool[n].pFirst = m_Pool[n].pFirst->pNext;
			free(pNode);
		}
	}
}

} // namespace vEngine {
