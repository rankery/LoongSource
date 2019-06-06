#pragma once
#pragma warning(disable:4251)

namespace Cool3D
{
//-----------------------------------------------------------------------------
// 将std::list包装成线程安全
//-----------------------------------------------------------------------------
template<typename Type> class TSafeList
{
public:
	// 添加元素到队列尾
	VOID PushBack(Type val)
	{
		EnterCriticalSection(&m_Lock);
		m_list.push_back(val);
		++m_nItemNum;
		LeaveCriticalSection(&m_Lock);
		::SetEvent(m_hEvent);
	}

	// 从队列头取元素
	Type PopFront()
	{
		Type val;
		if( m_nItemNum <= 0 )
			return Type(-1);

		EnterCriticalSection(&m_Lock);
		if( m_nItemNum <= 0 )
		{
			LeaveCriticalSection(&m_Lock);
			return Type(-1);
		}

		val = m_list.front();
		m_list.pop_front();
		m_nItemNum--;

		LeaveCriticalSection(&m_Lock);
		return val;
	}

	// 删除指定元素
	BOOL Erase(Type& val)
	{
		std::list<Type>::iterator it;
		EnterCriticalSection(&m_Lock);
		for(it=m_list.begin(); it!=m_list.end(); ++it)
		{
			if( val == *it )
			{
				m_list.erase(it);
				--m_nItemNum;
				LeaveCriticalSection(&m_Lock);
				return TRUE;
			}
		}
		LeaveCriticalSection(&m_Lock);
		return FALSE;
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

	// 安全的将数据导入一个普通std::list,返回数据个数
	INT Export(list<Type>& listDest)
	{
		INT n=0;
		EnterCriticalSection(&m_Lock);
		list<Type>::iterator it;
		for(it=m_list.begin(); it!=m_list.end(); ++it, ++n)
			listDest.push_back(*it);

		LeaveCriticalSection(&m_Lock);
		return n;
	}

	// 取得事件，当队列中加入消息时，此事件被激活
	HANDLE	GetEvent() { return m_hEvent; }

	TSafeList():m_nItemNum(0) { InitializeCriticalSection(&m_Lock); m_hEvent = ::CreateEvent(NULL, FALSE, TRUE, NULL); }	
	~TSafeList(){ DeleteCriticalSection(&m_Lock); CloseHandle(m_hEvent); }

private:
	std::list<Type>		m_list;
	INT					m_nItemNum;
	CRITICAL_SECTION	m_Lock;
	HANDLE				m_hEvent;
};

};