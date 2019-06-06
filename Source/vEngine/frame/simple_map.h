//-----------------------------------------------------------------------------
//!\file simple_map.h
//!\author Lyp
//!
//!\date 2004-10-27
//! last 2004-10-27
//!
//!\brief	提供简单的map管理
//!
//!
//-----------------------------------------------------------------------------
#pragma once
namespace vEngine {

//-----------------------------------------------------------------------------
// 提供简单的map管理
//-----------------------------------------------------------------------------
template<class KeyType, class ValueType> class TMap
{
public:
	typedef typename std::map<KeyType, ValueType>::iterator TMapIterator;

public:
	//! 添加元素
	VOID Add(KeyType key, ValueType value)
	{ m_map.insert(std::make_pair(key, value)); }
	
	//! 读取元素
	ValueType Peek(KeyType key)
	{
		std::map<KeyType, ValueType>::iterator it = m_map.find(key);
		if( it == m_map.end() )
			return ValueType(GT_INVALID);
		else
			return it->second;
	}
	
	//! 改变指定key对应的值
	BOOL ChangeValue(KeyType key, ValueType new_value)
	{
		std::map<KeyType, ValueType>::iterator it = m_map.find(key);
		if( it == m_map.end() )
			return FALSE;

		it->second = new_value;
		return TRUE;
	}

	//! 累加指定key对应的值，如果key不存在，则直接添加（要求ValueType必须有operator+和operator=重载）
	VOID ModifyValue(KeyType key, ValueType mod_value)
	{
		std::map<KeyType, ValueType>::iterator it = m_map.find(key);
		if( it == m_map.end() )
		{
			m_map.insert(std::make_pair(key, mod_value));
		}
		else
		{
			it->second = it->second + mod_value;
		}
	}

	//! 检查指定元素是否存在
	BOOL IsExist(KeyType key)
	{
		std::map<KeyType, ValueType>::iterator it = m_map.find(key);
		if( it == m_map.end() )
			return FALSE;
		else
			return TRUE;
	}

	//! 删除一个指定元素
	BOOL Erase(KeyType key)
	{ 
		std::map<KeyType, ValueType>::iterator it = m_map.find(key);
		if( it == m_map.end() )
			return FALSE;

        m_map.erase(it);
		return TRUE;
	}

	//! 清空所有元素
	VOID Clear() { m_map.clear(); }

	//! 得到元素个数
	INT	Size() { return (INT)m_map.size(); }

	BOOL Empty() { return m_map.empty(); }

	//! 将内部的迭代器初始化到map的开始
	VOID ResetIterator()
	{ m_it = m_map.begin(); }

	// ! 得到map的头
	TMapIterator Begin()
	{ return m_map.begin(); }

	//! 取出内部的迭代器指向的元素，并后移迭代器
	BOOL PeekNext(ValueType& value)
	{
		if( m_it == m_map.end() )
			return FALSE;
		value = m_it->second;
		++m_it;
		return TRUE;
	}

	//! 取出指定迭代器指向的元素，并后移该迭代器
	BOOL PeekNext(TMapIterator& it, ValueType& value)
	{
		if( it == m_map.end() )
			return FALSE;
		value = it->second;
		++it;
		return TRUE;
	}

	//! 取出内部的迭代器指向的元素，并后移迭代器
	BOOL PeekNext(KeyType& key, ValueType& value)
	{
		if( m_it == m_map.end() )
			return FALSE;
		key = m_it->first;
		value = m_it->second;
		++m_it;
		return TRUE;
	}

	//! 取出指定的迭代器指向的元素，并后移迭代器
	BOOL PeekNext(TMapIterator& it, KeyType& key, ValueType& value)
	{
		if( it == m_map.end() )
			return FALSE;
		key = it->first;
		value = it->second;
		++it;
		return TRUE;
	}

	//! 从list里面随机抽取一个元素
	BOOL RandPeek(KeyType& key, ValueType& value)
	{
		INT nSize = m_map.size();
		if( nSize <= 0 )
			return FALSE;

		INT nRand = rand() % nSize;

		std::map<KeyType, ValueType>::iterator it = m_map.begin();
		for(INT n=0; n<nRand; n++)
			++it;

		key = it->first;
		value = it->second;
		return TRUE;
	}


	//! 将所有key导出到一个给定std::list
	VOID ExportAllKey(std::list<KeyType>& listDest)
	{
		std::map<KeyType, ValueType>::iterator it;
		for(it = m_map.begin(); it != m_map.end(); ++it)
			listDest.push_back(it->first);
	}
	
	//! 将所有value导出到一个给定std::list
	VOID ExportAllValue(std::list<ValueType>& listDest)
	{
		std::map<KeyType, ValueType>::iterator it;
		for(it = m_map.begin(); it != m_map.end(); ++it)
			listDest.push_back(it->second);
	}


private:
	std::map<KeyType, ValueType>					m_map;
	typename std::map<KeyType, ValueType>::iterator m_it;
};


}