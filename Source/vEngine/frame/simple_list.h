//-----------------------------------------------------------------------------
//!\file simple_list.h
//!\author Lyp
//!
//!\date 2005-3-2
//! last 2005-3-2
//!
//!\brief	提供简单的list管理
//!			
//!
//-----------------------------------------------------------------------------
#pragma once
namespace vEngine {

//-----------------------------------------------------------------------------
// 方便的 std::list,并带有内部元素计数
//-----------------------------------------------------------------------------
template<typename Type> class TList
{
public:
	typedef typename std::list<Type>::iterator TListIterator;

public:
	// 添加元素到队列尾
	VOID PushBack(Type val)
	{
		m_list.push_back(val);
		++m_nItemNum;
	}

	// 从队列头取元素
	Type PopFront()
	{
		Type val;
		if( m_nItemNum <= 0 )
			return Type(GT_INVALID);

		val = m_list.front();
		m_list.pop_front();
		m_nItemNum--;
		return val;
	}

	// 添加元素到队列头
	VOID PushFront(Type val)
	{
		m_list.push_front(val);
		++m_nItemNum;
	}

	// 得到第一个元素的值，但并不取出来
	Type Front()
	{
		if( m_nItemNum <= 0 )
			return Type(GT_INVALID);

		return m_list.front();
	}

	// 删除指定元素
	BOOL Erase(Type& val)
	{
		std::list<Type>::iterator it;
		for(it=m_list.begin(); it!=m_list.end(); ++it)
		{
			if( val == *it )
			{
				m_list.erase(it);
				--m_nItemNum;	// 需要处理nItemNum,所以不能直接用list::remove
				return TRUE;
			}
		}
		return FALSE;
	}

	// 检查指定元素是否存在
	BOOL IsExist(Type& val)
	{
		std::list<Type>::iterator it;
		for(it=m_list.begin(); it!=m_list.end(); ++it)
		{
			if( val == *it )
				return TRUE;
		}
		return FALSE;
	}

	// 清空所有元素
	VOID Clear()
	{
		m_list.clear();
		m_nItemNum=0;
	}

	// 得到元素数目,std::list.size()并不能保证线程安全,
	// 所以我们自己保存一份个数数据
	INT	Size() { return m_nItemNum;	}
	BOOL Empty() { return (0 == m_nItemNum); }


	//! 从list里面随机读一个元素
	BOOL RandPeek(Type& value, BOOL bPop=FALSE)
	{
		if( m_list.empty() )
			return FALSE;

		INT nRand = rand() % m_nItemNum;
		std::list<Type>::iterator it = m_list.begin();
		for(INT n=0; n<nRand; n++)
			++it;
       
		value = *it;
		if( bPop )
		{
			m_list.erase(it);
			m_nItemNum--;
		}
			
		return TRUE;
	}


	//! 取出内部的迭代器指向的元素，并后移迭代器
	BOOL PeekNext(Type& value)
	{
		if( m_it == m_list.end() )
			return FALSE;
		value = *m_it;
		++m_it;
		return TRUE;
	}

	//! 取出外部的迭代器指向的元素，并后移迭代器
	BOOL PeekNext(TListIterator& it, Type& value)
	{
		if( it == m_list.end() )
			return FALSE;
		value = *it;
		++it;
		return TRUE;
	}

	//! 将内部的迭代器初始化到map的开始
	VOID ResetIterator()
	{ m_it = m_list.begin();	}

	//! 得到指向list开始位置的迭代器
	TListIterator Begin()
	{
		return m_list.begin();
	}

	std::list<Type>& GetList() { return m_list; }


	VOID operator=(std::list<Type>& list) { m_list = list;	}
	VOID operator=(TList<Type>& list) { m_list = list.GetList(); m_nItemNum = m_list.size(); m_it = m_list.end(); }


	TList():m_nItemNum(0) {}
	TList(std::list<Type>& list){ m_list = list; }
	~TList(){}

private:
	std::list<Type>						m_list;
	INT									m_nItemNum;
	typename std::list<Type>::iterator	m_it;
};



}