//-----------------------------------------------------------------------------
// File: msg_queue_ts
// Desc: 提供线程安全的先进先出信息队列管理
// Auth: Lyp
// Date: 2003-11-30
// Last: 2004-03-26
//
// Copyright (c) 1985-2008 CTCRST Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "msg_queue_ts.h"

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
}

MsgQueueTS::~MsgQueueTS()
{
	MsgItem* pMsg = m_pQueueBegin; 
	while( m_pQueueBegin )
	{
		pMsg = m_pQueueBegin->pNext;
		delete[]( m_pQueueBegin->pData);
		delete(m_pQueueBegin);
		m_pQueueBegin = pMsg;
	}

	if( m_bEvent )
		CloseHandle(m_hEvent);

	DeleteCriticalSection(&m_Lock);
}


//-----------------------------------------------------------------------------
// 添加消息到队列尾，返回是否添加成功
//-----------------------------------------------------------------------------
BOOL MsgQueueTS::AddMsg(LPCVOID pMsg, DWORD dwSize)
{
	DWORD dwOriginSize = dwSize;
	if( m_bAddSizeAhead ) // 此成员在对象创建后就不会改变
		dwSize += sizeof(DWORD);

	// 申请新的单元
	MsgItem* pMsgItem = new MsgItem;
	if( pMsgItem == NULL )
		return FALSE;

	// 初始化
	pMsgItem->dwDataSize = dwSize;
	pMsgItem->pData = NULL;
	pMsgItem->pNext = NULL;
	
	// 申请内容空间
	pMsgItem->pData = new BYTE[dwSize];
	if( pMsgItem->pData == NULL )
	{
		delete(pMsgItem);
		return FALSE;
	}

	// 进入互斥区
	EnterCriticalSection(&m_Lock);	

	// 拷贝内容
	if( m_bAddSizeAhead )
	{
		//*(DWORD*)(pMsgItem->pData) = htonl(dwOriginSize);
		*(DWORD*)(pMsgItem->pData) = dwOriginSize;
		memcpy(pMsgItem->pData+sizeof(dwOriginSize), pMsg, dwOriginSize);
	}
	else
	{
		memcpy(pMsgItem->pData, pMsg, dwSize);
	}
	
	if( m_pQueueBegin == NULL )	// 队列空
	{
		m_pQueueBegin = pMsgItem;
		m_pQueueEnd = pMsgItem;
	}
	else
	{
		m_pQueueEnd->pNext = pMsgItem;
		m_pQueueEnd = pMsgItem;
	}

	m_nMsg++;

	if( m_bEvent )	// 激活线程
		::SetEvent(m_hEvent);	

	LeaveCriticalSection(&m_Lock);
	return TRUE;
}



//-----------------------------------------------------------------------------
// 从队列头取消息，返回消息长度，外边需负责释放内存
//-----------------------------------------------------------------------------
LPBYTE MsgQueueTS::GetMsg(DWORD &dwMsgSize)
{
	dwMsgSize = 0;
	if( m_nMsg <= 0 )	// 预先检查，以免不必要的进入critical section
		return NULL;	// no msg

	EnterCriticalSection(&m_Lock);

	if( m_nMsg <= 0 )	// 内部再行检查
	{
		LeaveCriticalSection(&m_Lock);
		return NULL;	// no msg
	}

	// 取出消息
	LPBYTE pMsg = m_pQueueBegin->pData;
	dwMsgSize = m_pQueueBegin->dwDataSize;
	
	MsgItem* pMsgItem = m_pQueueBegin;
	m_pQueueBegin = m_pQueueBegin->pNext;

	m_nMsg--;
	LeaveCriticalSection(&m_Lock);

	delete(pMsgItem);
	return pMsg;
}
