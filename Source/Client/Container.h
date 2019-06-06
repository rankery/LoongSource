#pragma once
#include "Item.h"

/**	class	\Container
	brief	\容器类
	remarks	\对于需要通过键值和位置进行双向检索的对象进行存储管理的类
*/
template<typename K, typename V> class Container
{
public:
	Container():m_bLocked(FALSE),m_nMaxSize(SPACE_ONE_BAG){}
	virtual ~Container(void){}
	
	/** \添加
	*/
	virtual BOOL Add(V* value)
	{
		// 如果已满则不可再添加
		if(m_mapContainer.Size() >= m_nMaxSize)
			return FALSE;
		
		INT16 pos = GetPos(value);
		K key = GetKey(value);
		m_mapPosition.Add(key, pos);
		m_mapContainer.Add(pos, value);
		
		return TRUE;
	}
	
	/** \删除
	*/
	virtual V* Remove(INT16 pos)
	{
		V* value = m_mapContainer.Peek(pos);
		if(P_VALID(value))
		{
			K key = GetKey(value);
			m_mapPosition.Erase(key);
			m_mapContainer.Erase(pos);
		}
		
		return value;
	}

	/** \销毁
	*/
	virtual VOID Destroy()
	{
		V* value;
		m_mapContainer.ResetIterator();
		while(m_mapContainer.PeekNext(value))
		{
			if (P_VALID(value))
			{
				SAFE_DEL(value);
			}
		}
		m_mapContainer.Clear();
		m_mapPosition.Clear();
	}
	
	/** \清除
	*/
	virtual VOID Clear(TMap<K, V*> &mapContainer)
	{
		V* value;
		m_mapContainer.ResetIterator();
		while(m_mapContainer.PeekNext(value))
		{
			mapContainer.Add(GetKey(value), value);
		}
		m_mapContainer.Clear();
		m_mapPosition.Clear();
	}

	/** \获取
	*/
	virtual V* GetValue(INT16 pos)
	{
		return m_mapContainer.Peek(pos);
	}

	/** \锁定
	*/
	virtual VOID Lock(){m_bLocked = TRUE;}
	/** \解锁
	*/
	virtual VOID Unlock(){m_bLocked = FALSE;}

	/** \容器大小
	*/
	INT Size(){return m_mapContainer.Size();}
	/** \容器可存最大数量
	*/
	INT MaxSize(){return m_nMaxSize;}
	VOID SetMaxSize(INT nMax){m_nMaxSize = nMax;}


	/** \容器已满
	*/
	BOOL IsFull(INT &nBlank){nBlank = m_nMaxSize - Size(); return nBlank > 0;}

	/** \是否存在
	*/
	BOOL IsExist(INT16 pos)
	{
		return m_mapContainer.IsExist(pos);
	}
	/** \获取第一个空位
	*/
	INT16 GetBlank()
	{
		for (INT i=0; i<m_nMaxSize; ++i)
		{
			if ( !IsExist(i) )
				return i;
		}
		return GT_INVALID;
	}

protected:
	/** \根据键值获取位置
	*/
	INT16 FindPosition(K key){return m_mapPosition.Peek(key);}
	
	virtual INT16 GetPos(V* value)=0;
	virtual K GetKey(V* value)=0;

protected:
	TMap<INT16, V*>		m_mapContainer;
	TMap<K, INT16>		m_mapPosition;
	INT					m_nMaxSize;
	BOOL				m_bLocked;
};

/** class	\ItemContainer
	brief	\物品容器类
	remarks \存储实体物品
*/
class ItemContainer : public Container<INT64, Item>
{
public:
	ItemContainer();
	~ItemContainer();

	ItemContainer(EItemConType eType, INT nMaxSize, INT nPageSize);

	virtual BOOL Add(Item* value);
	virtual Item* Remove(INT16 pos);

	virtual VOID Lock();
	virtual VOID Unlock(bool bForSettle = false);

	/** \根据物品的模板编号查询容器中的该类物品数量
	*/
	INT GetItemQuantity(DWORD dwTypeID);
	/** \根据物品的模板编号查询容器中的该类物品最少位置
	*/
	INT16 GetItemPosition(DWORD dwTypeID);
	/** \根据物品的模板编号查询容器中的该类物品最少堆的id
	*/
	INT64 GetItemSerialID(DWORD dwTypeID);
	/** \获取背包中第一个指定特殊功用类型的物品ID
	*/
	INT64 GetSpecFuncItemID(EItemSpecFunc eType);
	/** \获取背包中特殊功用类型的物品TypeID列表
	*/
	void GetSpecFuncItemTypeList(EItemSpecFunc eType, TList<DWORD>& List);
	/** \根据物品ID获取物品
	*/
	Item* GetItem(INT64 n64ItemId){return GetValue(GetPos(n64ItemId));}

	/** \根据页数获取相应页的第一个空位
	*/
	INT16 GetBlank(INT nIndex = 0);
	/** \自动整理
	*/
	VOID AutoSettle(DWORD dwNPCID = GT_INVALID);

	/** \获悉容器中物品总数量
	*/
	INT32 GetContainerItemQuantity();

	/** \获得从背包中获得第一个特殊类型的物品
		\只能对背包使用，针对钓鱼系统添加的
	*/
	Item* GetSpecFuncItem( EItemConType eItemConType, EItemSpecFunc eSpecFunc );

	/** 同GetSpecFuncItem（EItemConType，EItemSpecFunc）钓鱼专用
	*/
	Item* GetSpecFuncItem(  EItemConType eItemConType, EItemSpecFunc eSpecFunc, DWORD dwItemTypeID );

	/* \统计容器中特殊功能的物品
	*/
	void GetSpecFuncMap( EItemSpecFunc eSpecFunc, std::list<Item*>& lst );

protected:
	virtual INT16 GetPos(Item* value){return value->GetPos();}
	virtual INT64 GetKey(Item* value){return value->GetItemId();}

	/** \根据页数获取相应页的起始位
	*/
	INT16 GetFirst(INT nIndex = 0);
	/** \根据页数获取相应页的终止位
	*/
	INT16 GetLast(INT nIndex = 0);

	
	/** \根据物品ID获取物品位置
	*/
	INT16 GetPos(INT64 n64ItemId){return m_mapPosition.Peek(n64ItemId);}

protected:
	EItemConType	m_eConType;		// 容器类型
	INT				m_nPageSize;	// 分页大小
	bool			m_bSettle;		// 自动整理标志位
};

/** struct	\tagWareItem
	brief	\单件商品
*/
struct tagWareItem
{
	DWORD	dwTypeID;		// 物品的模板ID
	INT16	n16Pos;			// 位置
	INT16	n16Quantity;	// 数量
	INT64	n64Price;		// 价格
	
	tagWareItem(){dwTypeID = n16Quantity = GT_INVALID; n64Price = n16Pos = 0;}
	tagWareItem(DWORD dwID){dwTypeID = dwID; n16Quantity = GT_INVALID; n64Price = n16Pos = 0;}
};

/** class	\WareContainer
	brief	\商品容器类
	remarks \存储商品
*/
class WareContainer : public Container<DWORD, tagWareItem>
{
public:
	WareContainer(){}
	~WareContainer(){}

protected:
	virtual INT16 GetPos(tagWareItem* value){return value->n16Pos;}
	virtual DWORD GetKey(tagWareItem* value){return value->dwTypeID;}
};

/** struct	\tagTemporaryItem
	brief	\临时物品
	remarks \用来表示交易、邮寄、摆摊等功能时自己背包里的物品
*/
struct tagTemporaryItem
{
	INT64	n64ItemId;		// 物品ID
	INT16	n16Pos;			// 位置
	bool	bShelf;			// 是否上架(摆摊中用，其他作为对齐位)
	INT8	n8Dummy;		// 对齐位
	INT64	n64Price;		// 物品单价(摆摊中用)
	Item*	pItem;			// 对应背包中的物品指针

	tagTemporaryItem(){n16Pos = 0; pItem = NULL;}
	tagTemporaryItem(Item* pt, INT16 n16Index)
	{
		n64ItemId = pt->GetItemId();
		n16Pos = n16Index;
		pItem = pt;
		bShelf = false;
		n64Price = 0;
	}
};

/** class	\TemporaryContainer
	brief	\临时容器类
	remarks	\用来存储临时物品
*/
class TemporaryContainer : public Container<INT64, tagTemporaryItem>
{
public:
	TemporaryContainer(INT nMaxSize){m_nMaxSize = nMaxSize;}
	~TemporaryContainer(){}

	tagTemporaryItem* RemoveById(INT64 n64Serial){return Remove(FindPosition(n64Serial));}

protected:
	virtual INT16 GetPos(tagTemporaryItem* value){return value->n16Pos;}
	virtual INT64 GetKey(tagTemporaryItem* value){return value->n64ItemId;}

	
};


