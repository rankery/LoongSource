//------------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: Container.h
// author: Sxg
// actor:
// data: 2008-06-16
// last:
// brief: 物品容器类 -- 模版基类
//------------------------------------------------------------------------------
#pragma once

#include "../WorldDefine/container_define.h"
#include "../ServerDefine/base_define.h"

#include "world.h"

//****************** 容器类 该部分操作不修改物品的所有者 *********************
template<class T, class MapKey = INT64>
class Container
{
public:
	Container();
	Container(EItemConType eConType, INT16 n16CurSize, INT16 n16MaxSize);
	virtual ~Container();

	VOID Init(EItemConType eConType, INT16 n16CurSize, INT16 n16MaxSize);

private:
	VOID Destroy();	// 在析构函数中自动调用

private:
	Container(const Container&);
	Container& operator = (const Container&);

public:
	// 容器类型
	EItemConType GetConType()	const;

	// 得到容器中空闲空间的大小
	INT16 GetFreeSpaceSize()	const;

	// 得到容器总空间大小
	INT16 GetMaxSpaceSize()		const;

	// 得到容器当前空间大小
	INT16 GetCurSpaceSize()		const;

	// 判断指定位置是否为空
	BOOL IsOnePlaceFree(INT16 n16Index) const;

	// 根据key得到物品指针,返回NULL表示指定物品不在该容器中
	T* GetItem(MapKey key);

	// 根据index得到物品指针,返回NULL表示指定物品不在该容器中
	T* GetItem(INT16 n16Index);

	// 获取初始迭代器
	typename TMap<MapKey, INT16>::TMapIterator Begin()	{ return m_mapItem.Begin(); }

	// 根据迭代器遍历容器中物品
	BOOL GetNextItem(typename TMap<MapKey, INT16>::TMapIterator& iter, T*& pItem);

	BOOL GetRandom(MapKey &key);

public:
	// 将物品存入指定位置,并返回添加成功个数(目的位置若有物品,将会被覆盖)
	INT16 Add(T* pItem, INT16 n16Index);

	// 从容器中清除物品，返回被删除物品的指针(NULL表示指定物品不存在容器中)
	T* Remove(MapKey key);	// 该接口调用方式为Container::Remove()
	T* Remove(INT16 n16Index);

protected:
	// 两物品位置交换,返回物品1个数(直接交换，不做任何验证)
	INT16 Swap(INT16 n16Index1, INT16 n16Index2);

	// 返回在容器中的位置,GT_INVALID表示不在容器中
	INT16 GetIndex(MapKey key);
	INT16 GetIndex(const T* pItem);

	// 重新设置容器大小
	VOID Resize(INT16 n16Size);

	// 按指定位置重排
	BOOL Reorder(IN LPVOID pData, OUT LPVOID pOutData, const INT16 n16Num);
	BOOL ReorderEx(IN LPVOID pData, OUT LPVOID pOutData, OUT INT16& n16OutNum, const INT16 n16Num);

private:
	T**					m_ppItem;			// 容器(该容器中存储的是物品结构的指针)
	TMap<MapKey, INT16>	m_mapItem;			// <64为ID，容器中位置>
	EItemConType		m_eContainerType;	// 容器类型
	INT16				m_n16MaxSize;		// 容器空间最大值
	INT16				m_n16CurSize;		// 容器当前空间大小
	INT16				m_n16RemainSize;	// 未使用的容器大小
};

//******************** 实现 该部分操作不修改物品的所有者 *********************

//------------------------------------------------------------------------------
// 构造函数
//------------------------------------------------------------------------------
template<class T, class MapKey>
Container<T, MapKey>::Container()
{
	m_ppItem = NULL;

	m_mapItem.Clear();
	m_n16CurSize		= 0;
	m_n16RemainSize		= 0;
	m_n16MaxSize		= 0;
	m_eContainerType	= EICT_Null;
}

template<class T, class MapKey>
Container<T, MapKey>::Container(EItemConType eConType, INT16 n16CurSize, INT16 n16MaxSize)
{
	Init(eConType, n16CurSize, n16MaxSize);	
}

//------------------------------------------------------------------------------
// 析构函数
//------------------------------------------------------------------------------
template<class T, class MapKey>
Container<T, MapKey>::~Container()
{
	Destroy();
}

//------------------------------------------------------------------------------
// 初始化
//------------------------------------------------------------------------------
template<class T, class MapKey>
VOID Container<T, MapKey>::Init(EItemConType eConType, INT16 n16CurSize, INT16 n16MaxSize)
{
	m_ppItem = new T*[n16CurSize];
	ZeroMemory(m_ppItem, n16CurSize * sizeof(T*));

	m_mapItem.Clear();
	m_n16CurSize		= n16CurSize;
	m_n16RemainSize		= n16CurSize;
	m_n16MaxSize		= n16MaxSize;
	m_eContainerType	= eConType;
}

//------------------------------------------------------------------------------
// 销毁(不用显示调用)
//------------------------------------------------------------------------------
template<class T, class MapKey>
VOID Container<T, MapKey>::Destroy()
{
	// 删除物品
	for(INT32 i=0; i<m_n16CurSize; ++i)
	{
		::Destroy(m_ppItem[i]);
	}

	SAFE_DEL_ARRAY(m_ppItem);

	m_mapItem.Clear();
	m_n16CurSize	= 0;
	m_n16RemainSize = 0;
	m_n16MaxSize	= 0;
}

//------------------------------------------------------------------------------
// 将物品存入指定位置,并返回添加成功个数(目的位置若有物品,将会被覆盖)
//------------------------------------------------------------------------------
template<class T, class MapKey>
INT16 Container<T, MapKey>::Add(T* pItem, INT16 n16Index)
{
	// 此处做判断,是为了外部调用方便(负面效果: 可能会做了多次判断)
	//if(n16Index < 0 || n16Index >= m_n16CurSize)
	//{
	//	return 0;
	//}

	ASSERT(IsOnePlaceFree(n16Index));

	m_ppItem[n16Index] = pItem;
	m_mapItem.Add(pItem->GetKey(), n16Index);

	// 容器中可用空间减少1
	--m_n16RemainSize;

	// 设置物品所在位置<容器类型,下标>，方便子类容器的操作(不用考虑物品中记录的位置信息变化)
	pItem->SetPos(m_eContainerType, n16Index);

	// 设置更新数据信息位
	pItem->SetUpdate(EUDBS_Update);

	return pItem->GetNum();
}

//------------------------------------------------------------------------------
// 根据64位id从容器中清除物品，返回被删除物品的指针
//------------------------------------------------------------------------------
template<class T, class MapKey>
T* Container<T, MapKey>::Remove(MapKey key)
{
	INT16 n16Index = m_mapItem.Peek(key);
	if(!GT_VALID(n16Index))
	{
		return NULL;
	}

	// 从map中清除
	m_mapItem.Erase(key);

	// 数组中指针清空
	T* pRetItem = m_ppItem[n16Index];
	if(NULL == pRetItem)
	{
		ASSERT(pRetItem != NULL);
		return NULL;
	}

	m_ppItem[n16Index] = NULL;

	// 容器中可以空间增加1
	++m_n16RemainSize;

	pRetItem->SetPos(EICT_Ground, GT_INVALID);

	return pRetItem;
}

//------------------------------------------------------------------------------
// 根据16位index从容器中清除物品，返回被删除物品的指针
//------------------------------------------------------------------------------
template<class T, class MapKey>
T* Container<T, MapKey>::Remove(INT16 n16Index)
{
	// 此处做判断,是为了外部调用方便(负面效果: 可能会做了多次判断)
	if(n16Index < 0 || n16Index >= m_n16CurSize)
	{
		return NULL;
	}

	// 数组中指针清空
	T* pRetItem = m_ppItem[n16Index];
	if(NULL == pRetItem)
	{
		ASSERT(pRetItem != NULL);
		return NULL;
	}

	m_ppItem[n16Index] = NULL;

	// 从map中清除
	m_mapItem.Erase(pRetItem->GetKey());

	// 容器中可以空间增加1
	++m_n16RemainSize;

	pRetItem->SetPos(EICT_Ground, GT_INVALID);

	return pRetItem;
}

//-----------------------------------------------------------------------------
// 按指定位置重排
//-----------------------------------------------------------------------------
template<class T, class MapKey>
BOOL Container<T, MapKey>::Reorder(IN LPVOID pData, OUT LPVOID pOutData, const INT16 n16Num)
{
	// 弱检测：个数是否与现有数量相符
	if(n16Num != GetCurSpaceSize() - GetFreeSpaceSize())
	{
		return FALSE;
	}
	
	MTRANS_POINTER(pSrcPos, pData, INT16);
	MTRANS_POINTER(pOut, pOutData, INT16);

	// 将容器中物品数据备份
	T** ppBak = new T*[GetCurSpaceSize()];
	memcpy(ppBak, m_ppItem, sizeof(T*) * GetCurSpaceSize());

	// 清空容器中所以物品数据
	m_mapItem.Clear();
	ZeroMemory(m_ppItem, sizeof(T*) * GetCurSpaceSize());
	m_n16RemainSize = GetCurSpaceSize();

	// 根据最新的顺序将物品放入容器
	INT16 n16CurIndex = 0;
	for(INT16 i=0; i<n16Num; ++i)
	{
		// 消息中数据不正确，或指定位置没有物品
		if(pSrcPos[i] < 0 || pSrcPos[i] > GetCurSpaceSize()
			|| !P_VALID(ppBak[pSrcPos[i]]))
		{
			continue;
		}
		
		// 放入容器中
		Add(ppBak[pSrcPos[i]], n16CurIndex);

		// 更新
		pOut[n16CurIndex] = pSrcPos[i];
		++n16CurIndex;
		ppBak[pSrcPos[i]] = NULL;
	}

	// 检查是否所有物品都已放到容器中
	if(n16CurIndex != n16Num)
	{
		// 执行到此，说明消息中有错误数据，需特殊处理
		for(INT16 i=0; i<GetCurSpaceSize(); ++i)
		{
			if(!P_VALID(ppBak[i]))
			{
				continue;
			}

			// 放入容器中
			Add(ppBak[i], n16CurIndex);

			// 更新
			pOut[n16CurIndex] = i;
			++n16CurIndex;
			ppBak[i] = NULL;
		}

		ASSERT(n16Num == n16CurIndex);
	}

	// 释放临时内存
	SAFE_DEL_ARRAY(ppBak);

	return TRUE;
}

//-----------------------------------------------------------------------------
// 按指定位置重排 -- 待移动物品的数据结构与上面函数不同
//-----------------------------------------------------------------------------
template<class T, class MapKey>
BOOL Container<T, MapKey>::ReorderEx(IN LPVOID pData, OUT LPVOID pOutData, 
									 OUT INT16& n16OutNum, const INT16 n16Num)
{
	n16OutNum = 0;
	
	// 弱检测：个数是否与现有数量相符
	if(n16Num > GetCurSpaceSize() - GetFreeSpaceSize())
	{
		return FALSE;
	}

	MTRANS_POINTER(pSrcPos, pData, tagItemOrder);
	MTRANS_POINTER(pOut, pOutData, tagItemOrder);

	// 将容器中需移动的物品数据备份
	T** ppBak = new T*[n16Num];
	ZeroMemory(ppBak, sizeof(T*) * n16Num);

	for(INT16 i=0; i<n16Num; ++i)
	{
		// 消息中数据不正确，或指定位置没有物品
		if(pSrcPos[i].n16OldIndex < 0 || pSrcPos[i].n16OldIndex > GetCurSpaceSize()
			|| pSrcPos[i].n16NewIndex < 0 || pSrcPos[i].n16NewIndex > GetCurSpaceSize()
			|| !P_VALID(GetItem(pSrcPos[i].n16OldIndex)))
		{
			continue;
		}

		// 从容器中取出,并放入备份数组中
		ppBak[i] = Remove(pSrcPos[i].n16OldIndex);
	}

	for(INT16 i=0; i<n16Num; ++i)
	{
		// 检查是否有物品
		if(!P_VALID(ppBak[i]))
		{
			continue;
		}

		// 检查目标位置是否为空
		if(!IsOnePlaceFree(pSrcPos[i].n16NewIndex))
		{
			continue;
		}

		// 放入容器中
		Add(ppBak[i], pSrcPos[i].n16NewIndex);

		// 更新
		pOut[n16OutNum++] = pSrcPos[i];
		ppBak[i] = NULL;
	}

	// 检查是否所有物品都已放到容器中
	if(n16OutNum != n16Num)
	{
		// 执行到此，说明消息中有错误数据 -- 玩家修改消息造成，不做处理，数据库中物品不会消失
	}

	// 释放临时内存
	SAFE_DEL_ARRAY(ppBak);

	return TRUE;
}


//*************************** 内联函数 ****************************************

//-----------------------------------------------------------------------------
// 容器类型
//-----------------------------------------------------------------------------
template<class T, class MapKey>
inline EItemConType Container<T, MapKey>::GetConType() const
{
	return m_eContainerType;
}

//-----------------------------------------------------------------------------
// 得到容器中空闲空间的大小
//-----------------------------------------------------------------------------
template<class T, class MapKey>
inline INT16 Container<T, MapKey>::GetFreeSpaceSize() const
{
	return m_n16RemainSize;
}

//-----------------------------------------------------------------------------
// 得到容器总空间大小
//-----------------------------------------------------------------------------
template<class T, class MapKey>
inline INT16 Container<T, MapKey>::GetMaxSpaceSize() const
{
	return m_n16MaxSize;
}

//-----------------------------------------------------------------------------
// 得到容器当前空间大小
//-----------------------------------------------------------------------------
template<class T, class MapKey>
inline INT16 Container<T, MapKey>::GetCurSpaceSize() const
{
	return m_n16CurSize;
}

//-----------------------------------------------------------------------------
// 判断指定位置是否为空
//-----------------------------------------------------------------------------
template<class T, class MapKey>
inline BOOL Container<T, MapKey>::IsOnePlaceFree(INT16 n16Index) const
{
	if(n16Index < 0 || n16Index >= m_n16CurSize)
	{
		ASSERT(0);
		return FALSE;
	}
	
	return NULL == m_ppItem[n16Index];
}

//-----------------------------------------------------------------------------
// 根据key得到物品指针,返回NULL表示指定物品不在该容器中
//-----------------------------------------------------------------------------
template<class T, class MapKey>
inline T* Container<T, MapKey>::GetItem(MapKey key)
{
	INT16 n16Index = m_mapItem.Peek(key);
	if(!GT_VALID(n16Index))
	{
		return NULL;
	}

	return m_ppItem[n16Index];
}

//-----------------------------------------------------------------------------
// 根据index得到物品指针,返回NULL表示指定物品不在该容器中
//-----------------------------------------------------------------------------
template<class T, class MapKey>
inline T* Container<T, MapKey>::GetItem(INT16 n16Index) 
{
	if(n16Index>=0 && n16Index<m_n16CurSize) 
	{
		return m_ppItem[n16Index];
	}
	return NULL; 
}

//-----------------------------------------------------------------------------
// 根据迭代器遍历容器中内容
//-----------------------------------------------------------------------------
template<class T, class MapKey>
inline BOOL Container<T, MapKey>::GetNextItem(typename TMap<MapKey, INT16>::TMapIterator& iter, T*& pItem)
{
	INT64 n64Serial	= GT_INVALID;
	INT16 n16Index	= GT_INVALID;
	if (m_mapItem.PeekNext(iter, n64Serial, n16Index) && GT_VALID(n16Index))
	{
		pItem = m_ppItem[n16Index];
		return TRUE;
	}

	pItem = NULL;

	return FALSE;
}

//-----------------------------------------------------------------------------
// 随机抽取
//-----------------------------------------------------------------------------
template<class T, class MapKey>
inline BOOL Container<T, MapKey>::GetRandom(MapKey &key)
{
	INT16 n16Index;
	return m_mapItem.RandPeek(key, n16Index);
}

//-----------------------------------------------------------------------------
// 两物品位置交换,返回物品1个数(直接交换，不做任何验证)
//-----------------------------------------------------------------------------
template<class T, class MapKey>
inline INT16 Container<T, MapKey>::Swap(INT16 n16Index1, INT16 n16Index2)
{
	// 删除
	T *pItem1 = Remove(n16Index1);
	T *pItem2 = Remove(n16Index2);

	// 添加
	Add(pItem1, n16Index2);
	Add(pItem2, n16Index1);

	// 设置数据库保存状态
	pItem1->SetUpdate(EUDBS_Update);
	pItem2->SetUpdate(EUDBS_Update);

	return pItem1->GetNum();
}

//-----------------------------------------------------------------------------
// 返回物品指针存储在容器中的位置,GT_INVALID表示不在容器中
//-----------------------------------------------------------------------------
template<class T, class MapKey>
inline INT16 Container<T, MapKey>::GetIndex(MapKey key)
{
	return m_mapItem.Peek(key);
}

template<class T, class MapKey>
inline INT16 Container<T, MapKey>::GetIndex(const T* pItem)
{
	return m_mapItem.Peek(pItem->GetKey());
}

//-----------------------------------------------------------------------------
// 重新设置容器大小
//-----------------------------------------------------------------------------
template<class T, class MapKey>
inline VOID Container<T, MapKey>::Resize(INT16 n16Size) 
{ 
	ASSERT(n16Size > m_n16CurSize);
	ASSERT(n16Size <= m_n16MaxSize);

	// 申请空间，并复制数据
	T **ppNew = new T*[n16Size];
	ZeroMemory(ppNew, n16Size * sizeof(T*));
	IFASTCODE->MemCpy(ppNew, m_ppItem, m_n16CurSize * sizeof(T*));

	// 重置容器属性
	SAFE_DEL_ARRAY(m_ppItem);
	m_ppItem = ppNew;
	m_n16RemainSize += n16Size - m_n16CurSize;
	m_n16CurSize = n16Size;
}