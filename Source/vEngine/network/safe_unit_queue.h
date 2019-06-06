//-----------------------------------------------------------------------------
// File: safe_unit_queue.h
// Desc: 提供线程安全的先进先出unit队列管理
// Auth: Lyp
// Date: 2003-11-30
// Last: 2005-11-01
//-----------------------------------------------------------------------------
#pragma once
namespace vEngine {

//-----------------------------------------------------------------------------
// SafeQueue: 提供线程安全的先进先出指针队列管理
//-----------------------------------------------------------------------------
template<typename ITEM_POINTER>
class SafeQueue
{
public:
	// 添加到队列尾
	__forceinline BOOL Add(ITEM_POINTER pItem)
	{
		EnterCriticalSection(&m_Lock);
		if( 0 == m_lItemNum )
			m_pFirst = pItem;
		else
			m_pLast->pNext = pItem;
		m_pLast = pItem;
		++m_lItemNum;
		LeaveCriticalSection(&m_Lock);
		if( m_bSetEvent )	// 激活event
			::SetEvent(m_hEvent);
		return TRUE;
	}

	// 从队列头取item，外边需负责释放
	__forceinline ITEM_POINTER Get()
	{
		if( m_lItemNum <= 0 )	// 先测试再进critical section
			return NULL;
		EnterCriticalSection(&m_Lock);
		if( m_lItemNum <= 0 )
		{
			LeaveCriticalSection(&m_Lock);
			return NULL;
		}
		ITEM_POINTER pItem = m_pFirst;	// 取出ITEM_POINTER
		m_pFirst = m_pFirst->pNext;
		--m_lItemNum;
		LeaveCriticalSection(&m_Lock);
		return pItem;
	}

	// 尝试进入锁定区并添加到队列尾
	__forceinline BOOL TryAdd(ITEM_POINTER pItem)
	{
#if _WIN32_WINNT < 0x0400
		EnterCriticalSection(&m_Lock);
		if( 0 == m_lItemNum )
			m_pFirst = pItem;
		else
			m_pLast->pNext = pItem;
		m_pLast = pItem;
		++m_lItemNum;
		LeaveCriticalSection(&m_Lock);
		if( m_bSetEvent )	// 激活event
			::SetEvent(m_hEvent);
		return TRUE;
#else
		if( TryEnterCriticalSection(&m_Lock) )
		{
			if( 0 == m_lItemNum )
				m_pFirst = pItem;
			else
				m_pLast->pNext = pItem;
			m_pLast = pItem;
			++m_lItemNum;
			LeaveCriticalSection(&m_Lock);
			if( m_bSetEvent )	// 激活event
				::SetEvent(m_hEvent);
			return TRUE;
		}
		return FALSE;
#endif
	}

	// 尝试进入锁定区并从队列头取
	__forceinline ITEM_POINTER TryGet()
	{
		if( m_lItemNum <= 0 )
			return NULL;
#if _WIN32_WINNT < 0x0400
		EnterCriticalSection(&m_Lock);
		if( m_lItemNum <= 0 )
		{
			LeaveCriticalSection(&m_Lock);
			return NULL;
		}
		ITEM_POINTER pItem = m_pFirst;	// 取出ITEM_POINTER
		m_pFirst = m_pFirst->pNext;
		--m_lItemNum;
		LeaveCriticalSection(&m_Lock);
		return pItem;
#else
		if( TryEnterCriticalSection(&m_Lock) )
		{
			if( m_lItemNum <= 0 )
			{
				LeaveCriticalSection(&m_Lock);
				return NULL;
			}
			ITEM_POINTER pItem = m_pFirst;	// 取出ITEM_POINTER
			m_pFirst = m_pFirst->pNext;
			--m_lItemNum;
			LeaveCriticalSection(&m_Lock);
			return pItem;
		}
		return NULL;
#endif
	}

	// 得到队列中的items数目
	__forceinline LONG GetNum() { return m_lItemNum; }
	// 取得事件，当队列中加入消息时，此事件被激活
	__forceinline HANDLE GetEvent() { return m_hEvent; }

	SafeQueue(BOOL bSetEvent=FALSE):m_lItemNum(0),m_pFirst(0), m_pLast(0)
	{
		InitializeCriticalSection(&m_Lock);
		m_bSetEvent = bSetEvent;
		if( m_bSetEvent )
			m_hEvent = ::CreateEvent(NULL, FALSE, TRUE, NULL);	// 自动激活
	}
	~SafeQueue()
	{
		// 确认缓冲中没有unit
		ASSERT( 0 == m_lItemNum );
		DeleteCriticalSection(&m_Lock);
		if( m_bSetEvent )
			CloseHandle(m_hEvent);
	}

private:
	CRITICAL_SECTION	m_Lock;
	HANDLE				m_hEvent;

	ITEM_POINTER		m_pFirst;
	ITEM_POINTER		m_pLast;

	BOOL				m_bSetEvent;
	VOLATILE LONG		m_lItemNum;
	
};

}
