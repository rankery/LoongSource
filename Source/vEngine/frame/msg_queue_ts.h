//-----------------------------------------------------------------------------
// File: msg_queue_ts
// Desc: 提供线程安全的先进先出信息队列管理
// Auth: Lyp
// Date: 2003-11-30
// Last: 2004-03-26
//-----------------------------------------------------------------------------
#pragma once
namespace vEngine {

//-----------------------------------------------------------------------------
// MsgQueueTS: 提供线程安全的先进先出信息队列管理
// 在取消息的时候使用内部分配的内存,外边需负责释放内存
// 当有消息加入时，可以激活一个Event，适合网络多线程收发
// 可以在信息前面加入信息长度，适合网络传输
//-----------------------------------------------------------------------------
class VENGINE_API MsgQueueTS
{
public:

	// bActiveEvent: 当有消息加入时，是否激活一个内建event
	// bAddSizeAhead: 是否在信息头部前加入信息长度，主要为网络通信准备
	MsgQueueTS(BOOL bActiveEvent, BOOL bAddSizeAhead);
	~MsgQueueTS();


	// 添加消息到队列尾，返回是否添加成功
	__forceinline BOOL AddMsg(LPCVOID pMsg, DWORD dwSize);

	// 从队列头取消息，外边需负责释放内存
	__forceinline LPBYTE GetMsg(DWORD& dwMsgSize);

	// 对应于GetMsg(),因为内存是内部分配的，所以应该交还内部来释放
	__forceinline VOID FreeMsg(LPVOID pMsg);

	// 取得事件，当队列中加入消息时，此事件被激活
	__forceinline HANDLE GetEvent() { return m_hEvent; }

	// 得到队列中的消息数目
	__forceinline INT GetMsgNum() { return m_nMsg; }

	//	是否消息头含有长度信息
	__forceinline BOOL IsAddSizeAhead()	{ return m_bAddSizeAhead; }
	

private:
	struct MsgItem
	{
		DWORD			dwDataSize;
		BYTE*			pData;
		MsgItem*		pNext;
	};

	SafeMemPool*		m_pMemPool;
	CRITICAL_SECTION	m_Lock;
	HANDLE				m_hEvent;
	INT					m_nMsg;
	BOOL				m_bEvent;
	BOOL				m_bAddSizeAhead;	

	MsgItem*			m_pQueueBegin;
	MsgItem*			m_pQueueEnd;
};



//-----------------------------------------------------------------------------
// 添加消息到队列尾，返回是否添加成功
//-----------------------------------------------------------------------------
BOOL MsgQueueTS::AddMsg(LPCVOID pMsg, DWORD dwSize)
{
	DWORD dwOriginSize = dwSize;
	if( m_bAddSizeAhead ) // 此成员在对象创建后就不会改变
		dwSize += sizeof(DWORD);

	// 申请新的单元
	MsgItem* pMsgItem = (MsgItem*)m_pMemPool->Alloc(sizeof(MsgItem));
	if( pMsgItem == NULL )
		return FALSE;

	// 初始化
	pMsgItem->dwDataSize = dwSize;
	pMsgItem->pData = NULL;
	pMsgItem->pNext = NULL;

	// 申请内容空间
	pMsgItem->pData = (LPBYTE)m_pMemPool->Alloc(dwSize);
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

	m_pMemPool->Free(pMsgItem);
	return pMsg;
}


//-----------------------------------------------------------------------------
// 对应于GetMsg(),因为内存是内部分配的，所以应该交还内部来释放
//-----------------------------------------------------------------------------
VOID MsgQueueTS::FreeMsg(LPVOID pMsg)
{
	m_pMemPool->Free(pMsg);
}


}