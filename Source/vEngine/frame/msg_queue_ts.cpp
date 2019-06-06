//-----------------------------------------------------------------------------
// File: msg_queue_ts
// Desc: 提供线程安全的先进先出信息队列管理
// Auth: Lyp
// Date: 2003-11-30
// Last: 2004-03-26
//-----------------------------------------------------------------------------
#include "..\stdafx.h"
#include "msg_queue_ts.h"

namespace vEngine {
//-----------------------------------------------------------------------------
// MsgQueueTS construction / destruction
//-----------------------------------------------------------------------------
MsgQueueTS::MsgQueueTS(BOOL bActiveEvent, BOOL bAddSizeAhead)
{
	m_bEvent = bActiveEvent;
	m_bAddSizeAhead = bAddSizeAhead;
	
	m_nMsg = 0;
	m_hEvent = NULL;
	m_pQueueBegin = NULL;
	m_pQueueEnd = NULL;
	
	InitializeCriticalSection(&m_Lock);

	if( m_bEvent )
		m_hEvent = ::CreateEvent(NULL, FALSE, TRUE, NULL);	// 自动激活

	m_pMemPool = new SafeMemPool(256*1024);	// 256KB 缓存
}

MsgQueueTS::~MsgQueueTS()
{
	MsgItem* pMsg = m_pQueueBegin; 
	while( m_pQueueBegin )
	{
		pMsg = m_pQueueBegin->pNext;
		m_pMemPool->Free(m_pQueueBegin->pData);
		m_pMemPool->Free(m_pQueueBegin);
		m_pQueueBegin = pMsg;
	}

	if( m_bEvent )
		CloseHandle(m_hEvent);

	DeleteCriticalSection(&m_Lock);
	SAFE_DEL(m_pMemPool);
}





}