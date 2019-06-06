//-----------------------------------------------------------------------------
//!\file safe_map.h
//!\author Lyp
//!
//!\date 2004-07-09
//! last 2004-08-16
//!
//!\brief	提供轻量级的线程安全的map管理
//!			
//!
//-----------------------------------------------------------------------------
#pragma once
namespace vEngine {

//-----------------------------------------------------------------------------
// 提供轻量级的线程安全的map管理
//-----------------------------------------------------------------------------
template<typename KeyType, typename ValueType> class TSafeMap
{
public:
	//! 添加元素
	VOID Add(KeyType key, ValueType value)
	{
		EnterCriticalSection(&m_Lock);
		m_map.insert(std::pair<KeyType, ValueType>(key, value));
		LeaveCriticalSection(&m_Lock);
	}

	//! 读取元素
	ValueType Peek(KeyType key)
	{
		ValueType temp;
		std::map<KeyType, ValueType>::iterator it;
		EnterCriticalSection(&m_Lock);
		it = m_map.find(key);
		if( it == m_map.end() )
			temp = ValueType(GT_INVALID);
		else
			temp = it->second;
		LeaveCriticalSection(&m_Lock);
		return temp;
	}

	//! 改变指定key对应的值
	BOOL ChangeValue(KeyType key, ValueType new_value)
	{
		std::map<KeyType, ValueType>::iterator it;
		EnterCriticalSection(&m_Lock);
		it = m_map.find(key);
		
		if( it == m_map.end() )
		{
			LeaveCriticalSection(&m_Lock);
			return FALSE;
		}
		else
		{
			it->second = new_value;
			LeaveCriticalSection(&m_Lock);
			return TRUE;
		}
	}

	//! 检查指定元素是否存在
	BOOL IsExist(KeyType key)
	{
		BOOL bResult = FALSE;
		std::map<KeyType, ValueType>::iterator it;
		EnterCriticalSection(&m_Lock);
		it = m_map.find(key);
		
		if( it == m_map.end() )
			bResult = FALSE;
		else
			bResult = TRUE;
		LeaveCriticalSection(&m_Lock);

		return bResult;
	}


	//! 删除指定元素
	BOOL Erase(KeyType key)
	{
		BOOL bResult = FALSE;
		std::map<KeyType, ValueType>::iterator it;
		
		EnterCriticalSection(&m_Lock);
		it = m_map.find(key);
		if( it == m_map.end() )
		{
			bResult = FALSE;
		}
		else
		{
			m_map.erase(it);
			bResult = TRUE;
		}
		LeaveCriticalSection(&m_Lock);
		return bResult;
	}


	//! 清空所有元素
	VOID Clear()
	{
		EnterCriticalSection(&m_Lock);
		m_map.clear();
		LeaveCriticalSection(&m_Lock);
	}

	//! 得到元素个数
	INT	Size() { return (INT)m_map.size(); }


	//! 将所有key导出到一个给定std::list,并返回导出的个数
	INT ExportAllKey(std::list<KeyType>& listDest)
	{
		INT n=0;
		EnterCriticalSection(&m_Lock);
		std::map<KeyType, ValueType>::iterator it;
		for(it = m_map.begin(); it != m_map.end(); ++it, ++n)
			listDest.push_back(it->first);
		LeaveCriticalSection(&m_Lock);
		return n;
	}

	//! 将所有value导出到一个给定std::list,并返回导出的个数
	INT ExportAllValue(std::list<ValueType>& listDest)
	{
		INT n=0;
		EnterCriticalSection(&m_Lock);
		std::map<KeyType, ValueType>::iterator it;
		for(it = m_map.begin(); it != m_map.end(); ++it, ++n)
			listDest.push_back(it->second);
		LeaveCriticalSection(&m_Lock);
		return n;
	}

	VOID Lock() { EnterCriticalSection(&m_Lock); }
	VOID Unlock() { LeaveCriticalSection(&m_Lock); }


	TSafeMap() { InitializeCriticalSection(&m_Lock); }
	~TSafeMap()	{ DeleteCriticalSection(&m_Lock); }


private:
	std::map<KeyType, ValueType>			m_map;
	CRITICAL_SECTION						m_Lock;
	
	

};



}