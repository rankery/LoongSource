//-----------------------------------------------------------------------------
// File: MemMgr.cpp
// Desc: game tool mem manager 1.0
// Time: 2003-1-8
// Auth: CTCRST
//
// Copyright (c) 2003 Softstar Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#include "stdafx.h"

#include "..\cpkcore\cpkdefine.h"
#include "MemMgr.h"
#include "..\cpkcore\Util.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//-----------------------------------------------------------------------------
// construction / destruction
//-----------------------------------------------------------------------------
MemPool::MemPool(DWORD dwMaxPoolSize)
{
	m_dwMaxSize = dwMaxPoolSize;
	m_dwCurrentSize = 0;
	ZeroMemory(m_Pool, sizeof(m_Pool));
}


MemPool::~MemPool()
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

