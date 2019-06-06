//-----------------------------------------------------------------------------
// File: msg_queue_ts
// Desc: 提供线程安全的先进先出信息队列管理
// Auth: Lyp
// Date: 2003-11-30
// Last: 2004-03-26
//
// Copyright (c) 1985-2008 CTCRST Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

//-----------------------------------------------------------------------------
// MsgQueueTS: 提供线程安全的先进先出信息队列管理
// 在取消息的时候使用内部分配的内存,外边需负责释放内存
// 当有消息加入时，可以激活一个Event，适合网络多线程收发
// 可以在信息前面加入信息长度，适合网络传输
//-----------------------------------------------------------------------------
class MsgQueueTS
{
public:

	// bActiveEvent: 当有消息加入时，是否激活一个内建event
	// bAddSizeAhead: 是否在信息头部前加入信息长度，主要为网络通信准备
	MsgQueueTS(BOOL bActiveEvent, BOOL bAddSizeAhead);

	// 添加消息到队列尾，返回是否添加成功
	BOOL	AddMsg(LPCVOID pMsg, DWORD dwSize);

	// 从队列头取消息，外边需负责释放内存
	LPBYTE	GetMsg(DWORD& dwMsgSize);

	// 对应于GetMsg(),因为内存是内部分配的，所以应该交还内部来释放
	__declspec(noinline) VOID FreeMsg(LPBYTE pMsg) {delete[](pMsg);}

	// 取得事件，当队列中加入消息时，此事件被激活
	HANDLE	GetEvent() { return m_hEvent; }

	// 得到队列中的消息数目
	INT		GetMsgNum() { return m_nMsg; }

	~MsgQueueTS();

private:
	struct MsgItem
	{
		DWORD			dwDataSize;
		BYTE*			pData;
		MsgItem*		pNext;
	};

	CRITICAL_SECTION	m_Lock;
	HANDLE				m_hEvent;
	INT					m_nMsg;
	BOOL				m_bEvent;
	BOOL				m_bAddSizeAhead;	

	MsgItem*			m_pQueueBegin;
	MsgItem*			m_pQueueEnd;
};

