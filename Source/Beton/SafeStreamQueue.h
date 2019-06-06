//--------------------------------------------------------------------------------------------------
// File: SafeStreamQueue.h
// Desc: Thread safe Mystream queue 
// Time: 2008-05-05
// Auth: Aslan
//
// Copyright (c) 2008 Tengreng All rights reserved.
//---------------------------------------------------------------------------------------------------
#pragma once

#include "StdAfx.h"
#include "Config.h"
#include "MysqlStream.h"

namespace Beton {

class MyStream;
class Mutex;
//----------------------------------------------------------------------------------------------------
/**	SafeStream类 - 缓存MyStream对象指针，为非实时操作提供支持
1. 外部数据库所有不需要立即操作的sql语句，均可放入该队列中，由别的线程操作
2. 预先分配的Mystream对象可以暂时放入队列中，方便提取及使用
*/
//-----------------------------------------------------------------------------------------------------
class SafeStreamQueue
{
public:
	SafeStreamQueue() : m_lNum(0) { m_hEvent = ::CreateEvent(NULL, FALSE, TRUE, NULL); }
	virtual ~SafeStreamQueue();

private:
	SafeStreamQueue(const SafeStreamQueue&);
	SafeStreamQueue& operator = (const SafeStreamQueue&);

public:
	BETON_INLINE MyStream* Get();
	BETON_INLINE VOID Add(MyStream* pStream);
	BETON_INLINE LONG Size() const { return m_lNum; }

	BETON_INLINE HANDLE GetEvent() { return m_hEvent; }

private:
	Mutex					m_Mutex;
	HANDLE					m_hEvent;
	std::list<MyStream*>	m_listQueue;
	volatile LONG			m_lNum;
};

//---------------------------------------------------------------------------------------------------------
// 外部获取一个stream
//---------------------------------------------------------------------------------------------------------
MyStream* SafeStreamQueue::Get()
{
	if( m_lNum == 0 ) return NULL;

	m_Mutex.Acquire();

	if( m_lNum == 0 )
	{
		m_Mutex.Release();
		return NULL;
	}
	MyStream* pStream = m_listQueue.front();
	m_listQueue.pop_front();
	InterlockedExchangeAdd((LPLONG)&m_lNum, -1);

	m_Mutex.Release();

	return pStream;
}

//-----------------------------------------------------------------------------------------------------------
// 得到一个stream
//-----------------------------------------------------------------------------------------------------------
VOID SafeStreamQueue::Add(MyStream* pStream)
{
	if( NULL == pStream ) return;

	m_Mutex.Acquire();

	m_listQueue.push_back(pStream);
	InterlockedExchangeAdd((LPLONG)&m_lNum, 1);

	if( m_lNum == 1 )
		::SetEvent(m_hEvent);	// 设置事件

	m_Mutex.Release();
}

} // namespace Beton