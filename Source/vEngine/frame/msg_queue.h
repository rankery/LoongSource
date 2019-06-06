//-----------------------------------------------------------------------------
// File: msg_queue
// Desc: 提供简单的先进先出信息队列管理
// Auth: Lyp
// Date: 2003-11-30
// Last: 2008-06-12
//-----------------------------------------------------------------------------
#pragma once
namespace vEngine {

//-----------------------------------------------------------------------------
// MsgQueue: 提供简单的先进先出信息队列管理
// 在取消息的时候使用内部分配的内存,外边需负责释放内存
//-----------------------------------------------------------------------------
class VENGINE_API MsgQueue
{
public:
	// 添加消息到队列尾，返回是否添加成功
	__forceinline BOOL AddMsg(LPCVOID pMsg, DWORD dwSize);
	// 从队列头取消息，外边需负责释放内存
	__forceinline LPBYTE GetMsg(DWORD& dwMsgSize);
	// 对应于GetMsg(),因为内存是内部分配的，所以应该交还内部来释放
	__forceinline VOID FreeMsg(LPVOID pMsg);

	//-----------------------------------------------------------------------------
	// 特殊操作
	//-----------------------------------------------------------------------------
	// 将内部指针指向队列头
	__forceinline VOID ResetIterator();
	// 将内部指针后移
	__forceinline VOID IncIterator();
	// 获得当前内部指针指向消息，但并不取出
	__forceinline LPBYTE PeekMsg(DWORD& dwMsgSize);
	// 移除当前内部指针指向消息，并后移指针
	__forceinline VOID RemoveMsg();
	// 清除所有消息
	__forceinline VOID Clear();

	// 得到队列中的消息数目
	__forceinline INT GetMsgNum() { return m_nMsg; }
	
	MsgQueue();
	~MsgQueue();

private:
	struct MsgItem
	{
		DWORD			dwDataSize;
		BYTE*			pData;
		MsgItem*		pPrev;
		MsgItem*		pNext;
	};

	INT					m_nMsg;
	MsgItem*			m_pQueueBegin;
	MsgItem*			m_pQueueEnd;
	MsgItem*			m_pCurrentItem;

	MemPool*			m_pMemPool;
};

//-----------------------------------------------------------------------------
// 添加消息到队列尾，返回是否添加成功
//-----------------------------------------------------------------------------
BOOL MsgQueue::AddMsg(LPCVOID pMsg, DWORD dwSize)
{
	// 申请新的单元
	MsgItem* pNewMsg = (MsgItem*)m_pMemPool->Alloc(sizeof(MsgItem));
	if( pNewMsg == NULL )
		return FALSE;

	// 初始化
	pNewMsg->dwDataSize = dwSize;
	pNewMsg->pData = NULL;
	pNewMsg->pPrev = NULL;
	pNewMsg->pNext = NULL;

	// 申请内容空间
	pNewMsg->pData = (LPBYTE)m_pMemPool->Alloc(dwSize);
	if( pNewMsg->pData == NULL )
	{
		delete(pNewMsg);
		return FALSE;
	}

	// 拷贝内容
	memcpy(pNewMsg->pData, pMsg, dwSize);

	if( m_pQueueBegin == NULL )	// 队列空
	{
		m_pQueueBegin = pNewMsg;
		m_pQueueEnd = pNewMsg;
		m_pCurrentItem = pNewMsg;
	}
	else
	{
		m_pQueueEnd->pNext = pNewMsg;
		pNewMsg->pPrev = m_pQueueEnd;
		m_pQueueEnd = pNewMsg;

	}

	m_nMsg++;
	return TRUE;
}



//-----------------------------------------------------------------------------
// 从队列头取消息，返回消息长度，外边需负责释放内存
//-----------------------------------------------------------------------------
LPBYTE MsgQueue::GetMsg(DWORD &dwMsgSize)
{
	dwMsgSize = 0;
	if( m_nMsg <= 0 )
		return NULL;	// no msg

	// 取出消息
	LPBYTE pMsg = m_pQueueBegin->pData;
	dwMsgSize = m_pQueueBegin->dwDataSize;

	MsgItem* pItem = m_pQueueBegin;
	m_pQueueBegin = m_pQueueBegin->pNext;
	if( m_pQueueBegin )
		m_pQueueBegin->pPrev = NULL;
	m_pMemPool->Free(pItem);

	m_nMsg--;
	return pMsg;
}


//-----------------------------------------------------------------------------
// 对应于GetMsg(),因为内存是内部分配的，所以应该交还内部来释放
//-----------------------------------------------------------------------------
VOID MsgQueue::FreeMsg(LPVOID pMsg) 
{
	m_pMemPool->Free(pMsg);
}


//-----------------------------------------------------------------------------
// 将内部指针指向队列头
//-----------------------------------------------------------------------------
VOID MsgQueue::ResetIterator()
{
	m_pCurrentItem = m_pQueueBegin;
}


//-----------------------------------------------------------------------------
// 将内部指针后移
//-----------------------------------------------------------------------------
VOID MsgQueue::IncIterator()
{
	if( !m_pCurrentItem )
		return;

	m_pCurrentItem = m_pCurrentItem->pNext;
}


//-----------------------------------------------------------------------------
// 获得当前内部指针指向消息，但并不取出
//-----------------------------------------------------------------------------
LPBYTE MsgQueue::PeekMsg(DWORD& dwMsgSize)
{
	dwMsgSize = 0;
	if( !m_pCurrentItem || m_nMsg <= 0 )
		return NULL;

	dwMsgSize = m_pCurrentItem->dwDataSize;
	return m_pCurrentItem->pData;
}


//-----------------------------------------------------------------------------
// 移除当前内部指针指向消息，并后移指针
//-----------------------------------------------------------------------------
VOID MsgQueue::RemoveMsg()
{
	if( !m_pCurrentItem )
		return;

	MsgItem* pItem = m_pCurrentItem;
	if( m_pCurrentItem == m_pQueueBegin )
	{
		m_pQueueBegin = m_pQueueBegin->pNext;
		if( m_pQueueBegin )
			m_pQueueBegin->pPrev = NULL;
		m_pCurrentItem = m_pQueueBegin;

	}
	else if( m_pCurrentItem == m_pQueueEnd )
	{
		m_pQueueEnd = m_pQueueEnd->pPrev;
		if( m_pQueueEnd )
			m_pQueueEnd->pNext = NULL;

		m_pCurrentItem = NULL;
	}
	else
	{
		m_pCurrentItem->pPrev->pNext = m_pCurrentItem->pNext;
		m_pCurrentItem->pNext->pPrev = m_pCurrentItem->pPrev;
		m_pCurrentItem = m_pCurrentItem->pNext;
	}

	m_pMemPool->Free(pItem->pData);
	m_pMemPool->Free(pItem);
	m_nMsg--;
	if( 0 == m_nMsg )
	{
		m_pQueueBegin = m_pQueueEnd = m_pCurrentItem = NULL;
	}
}

//-----------------------------------------------------------------------------
// 清除所有消息
//-----------------------------------------------------------------------------
VOID MsgQueue::Clear()
{
	MsgItem* pMsg = m_pQueueBegin; 
	while( m_pQueueBegin )
	{
		pMsg = m_pQueueBegin->pNext;
		m_pMemPool->Free( m_pQueueBegin->pData);
		m_pMemPool->Free(m_pQueueBegin);
		m_pQueueBegin = pMsg;
	}
	m_nMsg = 0;
	m_pQueueBegin = NULL;
	m_pQueueEnd = NULL;
	m_pCurrentItem = NULL;
}

}