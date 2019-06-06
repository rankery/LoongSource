//---------------------------------------------------------------------------------------------------------
// File: StreamPool.h
// Desc: Mysql SQL statement pool
// Time: 2008-05-05
// Auth: Aslan
//
// Copyright (c) 2008 Tengreng All rights reserved.
//---------------------------------------------------------------------------------------------------------
#include "StdAfx.h"
#include "Config.h"
#include "MysqlStream.h"
#include "SafeStreamQueue.h"

namespace Beton
{
//-----------------------------------------------------------------------------------------------------------
// 构造函数析构函数
//-----------------------------------------------------------------------------------------------------------
StreamPool::StreamPool(INT nDefaultSize)
	: m_nDefaultSize(nDefaultSize), m_nTotalNum(0)
{
	//ASSERT( m_nDefaultSize > 0 );

	RealAlloc(m_nDefaultSize);
}

StreamPool::~StreamPool()
{
	RealFreeAll();
}

//------------------------------------------------------------------------------------------------------------
// 真实分配内存
//------------------------------------------------------------------------------------------------------------
VOID StreamPool::RealAlloc(INT nNum)
{
	if( nNum <= 0 ) return;

	m_Mutex.Acquire();
	for(INT n = 0; n < nNum; n++)
	{
		MyStream* pStream = new MyStream();
		if( NULL == pStream ) break;

		m_FreeQueue.Add(pStream);
		InterlockedExchangeAdd((LPLONG)&m_nTotalNum, 1);
	}
	m_Mutex.Release();
}

//-------------------------------------------------------------------------------------------------------------
// 真实释放内存
//-------------------------------------------------------------------------------------------------------------
VOID StreamPool::RealFree(INT nNum)
{
	if( nNum <= 0 ) return;

	m_Mutex.Acquire();
	for(INT n = 0; n < nNum; n++)
	{
		MyStream* pStream = m_FreeQueue.Get();
		if( pStream )
		{
			SAFE_DEL(pStream);
			InterlockedExchangeAdd((LPLONG)&m_nTotalNum, -1);
		}
	}
	m_Mutex.Release();
}

//-------------------------------------------------------------------------------------------------------------
// 释放所有申请的内存
//-------------------------------------------------------------------------------------------------------------
VOID StreamPool::RealFreeAll()
{
	while( m_nTotalNum != 0 )
	{
		MyStream* pStream = m_FreeQueue.Get();
		if( NULL == pStream ) continue;

		SAFE_DEL(pStream);
		InterlockedExchangeAdd((LPLONG)&m_nTotalNum, -1);
	}
}

//-------------------------------------------------------------------------------------------------------------
// 从池中取出，若取不出，则扩充池的内容
//--------------------------------------------------------------------------------------------------------------
MyStream* StreamPool::AllocStream()
{
	MyStream* pStream = m_FreeQueue.Get();

	while( NULL == pStream )
	{
		RealAlloc(m_nDefaultSize);
		pStream = m_FreeQueue.Get();
	}

	return pStream;
}

//---------------------------------------------------------------------------------------------------------------
// 归还池内容
//----------------------------------------------------------------------------------------------------------------
VOID StreamPool::FreeStream(MyStream* pStream)
{
	if( NULL == pStream ) return;

	m_FreeQueue.Add(pStream);

	// 申请了过多内存，则释放一部分
	INT nSize = m_FreeQueue.Size();
	if( nSize >= 8 * m_nDefaultSize )
	{
		RealFree(nSize/4);
	}
}

} // namespace Beton
