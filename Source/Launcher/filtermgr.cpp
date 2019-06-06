//-----------------------------------------------------------------------------
// File: FilterMgr.cpp
// Desc: game tool filter manager 1.0
// Time: 2003-1-8
// Auth: CTCRST
//
// Copyright (c) 2003 Softstar Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include <TChar.h>

#include "FilterMgr.h"
#include "cpkdefine.h"
#include "util.h"

#include "minilzo.h"


#define HEAP_ALLOC(var,size) \
	long __LZO_MMODEL var [ ((size) + (sizeof(long) - 1)) / sizeof(long) ]

static HEAP_ALLOC(pLzoWorkMem, LZO1X_1_MEM_COMPRESS);


//-----------------------------------------------------------------------------
// construction / destruction
//-----------------------------------------------------------------------------
CFilterMgr::CFilterMgr()
{
	InitFilter(FMF_MiniLzo1040);
}

CFilterMgr::~CFilterMgr()
{}



//-----------------------------------------------------------------------------
// 初始化过滤器
//-----------------------------------------------------------------------------
BOOL CFilterMgr::InitFilter(EFMFilter eFilter)
{
	switch(eFilter)
	{
	case FMF_MiniLzo1040:

		if (lzo_init() != LZO_E_OK)
		{
			ErrMsg(_T("Lzo init failed"));
			return FALSE;
		}
		
		break;

	case FMF_ZLib:
		break;
	}

	return TRUE;
}



//-----------------------------------------------------------------------------
// 检查各种过滤器算法需要的额外运算空间，不是指Work Space
//-----------------------------------------------------------------------------
BOOL CFilterMgr::CheckBufferSize(DWORD dwInSize, DWORD dwOutSize,
								 EFMFilter eFilter)
{
	DWORD dwRequireSpace = 0;
	switch(eFilter)
	{
	case FMF_MiniLzo1040:
		dwRequireSpace = dwInSize + (dwInSize/64) + 16 + 3;
		break;

	case FMF_ZLib:

		break;
	}

	if (dwOutSize < dwRequireSpace)
		return FALSE;

	return TRUE;
}



//-----------------------------------------------------------------------------
// 开始过滤过程
//-----------------------------------------------------------------------------
VOID CFilterMgr::Filter(tagFilterParam *pParam)
{
	ASSERT(pParam);
	int nResult = 0;
	
	switch(pParam->eFilter)
	{
	case FMF_MiniLzo1040:
		if (pParam->bCompress)	// compress
		{
			nResult = lzo1x_1_compress((lzo_byte*)pParam->lpMemIn,
				pParam->dwInDataSize,
				(lzo_byte*)pParam->lpMemOut,
				(lzo_uint*)&(pParam->dwOutDataSize),
				pLzoWorkMem);
			if (nResult == LZO_E_OK)
			{
				pParam->bSuccess = TRUE;
				//Msg("Compressed %lu bytes into %lu bytes\n",
				//	pParam->dwInDataSize, pParam->dwOutDataSize);
			}
			else
			{
				pParam->bSuccess = FALSE;
				ErrMsg(_T("MiniLzo compression failed: %d\n"), nResult);
			}
		}
		else	// decompress
		{
			nResult = lzo1x_decompress((lzo_byte*)pParam->lpMemIn,
				pParam->dwInDataSize,
				(lzo_byte*)pParam->lpMemOut,
				(lzo_uint*)&(pParam->dwOutDataSize),
				NULL);
			if (nResult == LZO_E_OK)
			{
				pParam->bSuccess = TRUE;
				//Msg("Decompressed %lu bytes back into %lu bytes\n",
				//	pParam->dwInDataSize, pParam->dwOutDataSize);
			}
			else
			{
				// for LZO, this should never happen
				pParam->bSuccess = FALSE;
				ErrMsg(_T("MiniLzo decompression failed: %d\n"), nResult);
			}
		}
		break;

	case FMF_ZLib:
		ASSERT(0);
		break;

	case FMF_Changeless:
		pParam->dwOutDataSize = pParam->dwInDataSize;
		pParam->bSuccess = TRUE;
		break;
	default:
		ASSERT(0);
		break;
	}

}
