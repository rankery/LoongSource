//-----------------------------------------------------------------------------
//!\file x_list.h
//!\author Lyp
//!
//!\date 2004-07-07
//! last 2009-06-11
//!
//!\brief	将std::list包装成线程安全
//!
//-----------------------------------------------------------------------------
#pragma once
namespace vEngine {

//-----------------------------------------------------------------------------
// 将std::list包装成线程安全
//-----------------------------------------------------------------------------
template<typename Type> class XList
{
public:
	__forceinline VOID PushBack(Type val)	// 添加元素到队列尾
	{
		Lock();
		m_list.push_back(val);
		++m_nItemNum;
		Unlock();
	}

	__forceinline Type PopFront()	// 从队列头取元素
	{
		if( m_nItemNum <= 0 )
			return Type(GT_INVALID);

		Lock();
		if( m_nItemNum <= 0 )
		{
			Unlock();
			return Type(GT_INVALID);
		}
		Type val = m_list.front();
		m_list.pop_front();
		m_nItemNum--;
		Unlock();
		return val;
	}
	
	__forceinline BOOL Erase(Type& val)	// 删除指定元素
	{
		std::list<Type>::iterator it;
		Lock();
		for(it=m_list.begin(); it!=m_list.end(); ++it)
		{
			if( val == *it )
			{
				m_list.erase(it);
				--m_nItemNum;
				Unlock();
				return TRUE;
			}
		}
		Unlock();
		return FALSE;
	}

	__forceinline BOOL IsExist(Type& val)	// 检查指定元素是否存在
	{
		Lock();
		std::list<Type>::iterator it;
		for(it=m_list.begin(); it!=m_list.end(); ++it)
		{
			if( val == *it )
			{
				Unlock();
				return TRUE
			}
		}
		Unlock();
		return FALSE;
	}

	__forceinline VOID Clear()	// 清空所有元素
	{
		Lock();
		m_list.clear();
		m_nItemNum=0;
		Unlock();
	}

	// 得到元素数目,std::list.size()并不能保证线程安全,所以我们自己保存一份个数数据
	__forceinline INT	Size() { return m_nItemNum;	}

	//! 取出内部的迭代器指向的元素，并后移迭代器,注意要lock后才能使用
	__forceinline BOOL _Peek(Type& value)
	{
		if( m_it == m_list.end() )
			return FALSE;
		value = *m_it;
		return TRUE;
	}

	//! 将内部的迭代器初始化到map的开始,注意要lock后才能使用
	__forceinline VOID _ResetIterator(){ m_it = m_list.begin(); }
	__forceinline VOID _AddIterator(){ ++m_it; }
	__forceinline VOID _EraseCurrent(){ m_list.erase(m_it); }

	__forceinline XList():m_nItemNum(0), m_lLock(0) {}
	__forceinline ~XList(){ }//ASSERT( 0 == m_nItemNum); }
	__forceinline VOID	Lock() { while(InterlockedCompareExchange((LPLONG)&m_lLock, 1, 0)!= 0) Sleep(0); }
	__forceinline VOID	Unlock() { InterlockedExchange((LPLONG)(&m_lLock), 0); }

private:
	std::list<Type>						m_list;
	typename std::list<Type>::iterator	m_it;
	
	INT	VOLATILE	m_nItemNum;
	LONG VOLATILE	m_lLock;
};

}