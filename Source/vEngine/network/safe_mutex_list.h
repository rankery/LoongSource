//-----------------------------------------------------------------------------
// File: safe_mutex_list.h
// Desc: 提供线程安全的先进先出互斥队列管理
// Auth: Lyp
// Date: 2003-11-30
// Last: 2004-09-24
//-----------------------------------------------------------------------------
#pragma once
#include "server_define.h"
namespace vEngine {

//-----------------------------------------------------------------------------
// TSafeMutexList: 提供线程安全的先进先出互斥队列管理
//-----------------------------------------------------------------------------
template<typename Type> class TSafeMutexList
{
public:
	// 添加元素到队列尾
	VOID PushBack(Type val)
	{
		std::list<Type>::iterator it;
		EnterCriticalSection(&m_Lock);
		m_list.push_back(val);
		m_nItemNum++;
		::SetEvent(m_hEvent);	// 激活event	
		LeaveCriticalSection(&m_Lock);
	}

	// 从队列头取元素
	Type PopFront()
	{
		Type val;
		if( m_nItemNum <= 0 )
			return Type(GT_INVALID);

		EnterCriticalSection(&m_Lock);
		if( m_nItemNum <= 0 )
		{
			LeaveCriticalSection(&m_Lock);
			return Type(GT_INVALID);
		}

		val = m_list.front();
		m_list.pop_front();
		m_nItemNum--;
		
		LeaveCriticalSection(&m_Lock);
		return val;
	}


	// 删除指定元素
	BOOL Erase(Type val)
	{
		std::list<Type>::iterator it;
		BOOL bResult = FALSE;
		EnterCriticalSection(&m_Lock);
		for(it=m_list.begin(); it!=m_list.end(); ++it)
		{
			if( val == *it )
			{
				m_list.erase(it);
				m_nItemNum--;
				bResult = TRUE;
				break;
			}
		}
		LeaveCriticalSection(&m_Lock);
		return bResult;
	}

	// 检查指定元素是否存在
	BOOL IsExist(Type& val)
	{
		BOOL bFound = FALSE;
		EnterCriticalSection(&m_Lock);
		std::list<Type>::iterator it;
		for(it=m_list.begin(); it!=m_list.end(); ++it)
		{
			if( val == *it )
			{
				bFound = TRUE;
				break;
			}
		}
		LeaveCriticalSection(&m_Lock);
		return bFound;
	}

	// 清空所有元素
	VOID Clear()
	{
		EnterCriticalSection(&m_Lock);
		m_list.clear();
		m_nItemNum=0;
		LeaveCriticalSection(&m_Lock);
	}

	// 得到元素数目,std::list.size()并不能保证线程安全,
	// 所以我们自己保存一份个数数据
	INT	Size() { return m_nItemNum;	}

	VOID Lock() { EnterCriticalSection(&m_Lock); }
	VOID Unlock() { LeaveCriticalSection(&m_Lock); }

	//! 取出内部的迭代器指向的元素，并后移迭代器,注意要lock后才能使用
	BOOL PeekNext(Type& value)
	{
		if( m_it == m_list.end() )
			return FALSE;
		value = *m_it;
		++m_it;
		return TRUE;
	}

	//! 将内部的迭代器初始化到map的开始,注意要lock后才能使用
	VOID ResetIterator(){ m_it = m_list.begin(); }

	// 取得事件，当队列中加入消息时，此事件被激活
	HANDLE	GetEvent() { return m_hEvent; }

	TSafeMutexList():m_nItemNum(0) 
	{ 
		m_hEvent = ::CreateEvent(NULL, FALSE, TRUE, NULL);	// 自动激活
		InitializeCriticalSectionAndSpinCount(&m_Lock, DEFAULT_SPIN_COUNT); 
	}
	~TSafeMutexList()
	{ 
		DeleteCriticalSection(&m_Lock); 
		CloseHandle(m_hEvent);
	}

private:
	std::list<Type>						m_list;
	INT									m_nItemNum;
	HANDLE								m_hEvent;
	CRITICAL_SECTION					m_Lock;
	typename std::list<Type>::iterator	m_it;
};





}