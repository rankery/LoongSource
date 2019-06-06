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



BOOL	CMemMgr::m_bRaiseException = TRUE;
HANDLE	CMemMgr::m_hMainHeap = 0;
DWORD	CMemMgr::m_dwMainHeapSize = 0;

//-----------------------------------------------------------------------------
// construction / destruction
//-----------------------------------------------------------------------------
CMemMgr::CMemMgr(BOOL bRaiseException)
{
	DWORD flOptions = HEAP_NO_SERIALIZE;
	if (bRaiseException)
		flOptions |= HEAP_GENERATE_EXCEPTIONS;
	
	m_bRaiseException = bRaiseException;
	m_hMainHeap = ::HeapCreate(flOptions, HEAP_INIT_SIZE, 0);
	m_dwMainHeapSize = HEAP_INIT_SIZE;

	if (m_hMainHeap == NULL)
	{
		ERR(CE_CANNOT_CREATE_MAIN_HEAP);
	}
}


CMemMgr::~CMemMgr()
{
	BOOL bResult = FALSE;
	bResult = ::HeapDestroy(m_hMainHeap);

	if (bResult == FALSE)
	{
		ERR(CE_CANNOT_DESTROY_MAIN_HEAP);
	}

	m_hMainHeap = 0;
	m_bRaiseException = false;
}




//-----------------------------------------------------------------------------
// 申请内存
//-----------------------------------------------------------------------------
LPVOID CMemMgr::Alloc(DWORD dwBytes)
{
	LPVOID pMem = NULL;
	DWORD dwFlag = HEAP_NO_SERIALIZE;
	if (m_bRaiseException == TRUE)
		dwFlag |= HEAP_GENERATE_EXCEPTIONS;

	pMem = ::HeapAlloc(m_hMainHeap, dwFlag, dwBytes);
	if (NULL == pMem)
	{
		ERR(CE_CANNOT_ALLOC_MEM);
	}

	return pMem;
}



//-----------------------------------------------------------------------------
// 改变已申请内存的大小
//-----------------------------------------------------------------------------
LPVOID CMemMgr::ReSize(LPVOID pMem, DWORD dwBytes)
{
	ASSERT(pMem);

	DWORD dwFlag = HEAP_NO_SERIALIZE;
	if (m_bRaiseException == TRUE)
		dwFlag |= HEAP_GENERATE_EXCEPTIONS;

	pMem = ::HeapReAlloc(m_hMainHeap, dwFlag, pMem, dwBytes);
	if (NULL == pMem)
	{
		ERR(CE_CANNOT_REALLOC_MEM);
	}
	return pMem;

}



//-----------------------------------------------------------------------------
// 释放内存
//-----------------------------------------------------------------------------
BOOL CMemMgr::Free(LPVOID pMem)
{
	ASSERT(pMem);
	BOOL bResult = FALSE;
	
	DWORD dwFlag = HEAP_NO_SERIALIZE;
	if (m_bRaiseException == TRUE)
		dwFlag |= HEAP_GENERATE_EXCEPTIONS;

	bResult = HeapFree(m_hMainHeap, dwFlag, pMem);
	if (FALSE == bResult)
	{
		ERR(CE_CANNOT_FREE_MEM);
	}
	return bResult;
}


