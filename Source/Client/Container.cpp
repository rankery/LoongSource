#include "stdafx.h"
#include "Container.h"
#include "ItemEvent.h"
#include "..\WorldDefine\container_define.h"
#include "..\WorldDefine\msg_item.h"



ItemContainer::ItemContainer()
{
	m_eConType = EICT_Null;
	m_bSettle  = false;
}

ItemContainer::~ItemContainer()
{

}

ItemContainer::ItemContainer(EItemConType eType, INT nMaxSize, INT nPageSize)
{
	m_eConType  = eType;
	m_nMaxSize  = nMaxSize;
	m_nPageSize = nPageSize;
	m_bSettle	= false;
}

VOID ItemContainer::Lock()
{
	m_bLocked = TRUE;
	// 将容器内的物品都设置成为不可操作
	Item* pItem;
	m_mapContainer.ResetIterator();
	while (m_mapContainer.PeekNext(pItem))
	{
		pItem->SetOperable(FALSE);
	}
	
}

VOID ItemContainer::Unlock(bool bForSettle)
{
	m_bLocked = FALSE;
	// 将容器内的物品都设置成为可操作
	Item* pItem;
	m_mapContainer.ResetIterator();
	while (m_mapContainer.PeekNext(pItem))
	{
		pItem->SetOperable(TRUE);
	}
	if (bForSettle)
		m_bSettle = false;
}

INT ItemContainer::GetItemQuantity(DWORD dwTypeID)
{
	Item* pItem;
	INT nRet = 0;
	m_mapContainer.ResetIterator();
	while (m_mapContainer.PeekNext(pItem))
	{	
		if (dwTypeID == pItem->GetItemTypeID())
		{
			nRet += pItem->GetItemQuantity();
		}
	}
	
	return nRet;
}

BOOL ItemContainer::Add( Item* value )
{
	BOOL bRet = Container<INT64, Item>::Add(value);

	return bRet;
}

Item* ItemContainer::Remove(INT16 pos)
{
	Item* pRet = Container<INT64, Item>::Remove(pos);

	return pRet;
}

INT16 ItemContainer::GetItemPosition( DWORD dwTypeID )
{
	Item* pItem;
	INT16 n16Ret = GT_INVALID;
	INT nQuantity = GT_INVALID;
	m_mapContainer.ResetIterator();
	while (m_mapContainer.PeekNext(pItem))
	{	
		if (pItem->GetItemTypeID() == dwTypeID)
		{
			if(GT_INVALID == nQuantity)
			{
				n16Ret = pItem->GetPos();
				nQuantity = pItem->GetItemQuantity();
			}
			else if(pItem->GetItemQuantity() < nQuantity)
			{
				n16Ret = pItem->GetPos();
				nQuantity = pItem->GetItemQuantity();
			}
		}
	}

	return n16Ret;
}

INT64 ItemContainer::GetSpecFuncItemID( EItemSpecFunc eType )
{
	Item* pItem;
	INT64 n64Ret = GT_INVALID;
	m_mapContainer.ResetIterator();
	INT nQuantity = GT_INVALID;
	while (m_mapContainer.PeekNext(pItem))
	{	
		if (pItem->GetItemSpecFunc() == eType)
		{
			if(GT_INVALID == nQuantity)
			{
				n64Ret = pItem->GetItemId();
				nQuantity = pItem->GetItemQuantity();
			}
			else if(pItem->GetItemQuantity() < nQuantity)
			{
				n64Ret = pItem->GetItemId();
				nQuantity = pItem->GetItemQuantity();
			}
		}
	}

	return n64Ret;
}




INT16 ItemContainer::GetBlank( INT nIndex /*= 0*/ )
{
	INT16 begin = GetFirst(nIndex);
	INT16 end	= GetLast(nIndex);
	//起始和结束位都是有效值才做处理
	if(GT_VALID(begin) && GT_VALID(end))
	{
		for(INT i=begin; i<=end; ++i)
		{
			if(!m_mapContainer.IsExist(i))
				return i;
		}
	}
	
	return GT_INVALID;
}

INT16 ItemContainer::GetFirst( INT nIndex /*= 0*/ )
{
	INT16 pos = nIndex * m_nPageSize;
	if(pos >= m_nMaxSize)
	{
		pos = GT_INVALID;
	}
	return pos;
}

INT16 ItemContainer::GetLast( INT nIndex /*= 0*/ )
{
	INT16 pos = (nIndex + 1) * m_nPageSize - 1;
	if(pos >= m_nMaxSize)
	{
		pos = m_nMaxSize - 1;
	}
	return pos;
}

void ItemContainer::GetSpecFuncItemTypeList( EItemSpecFunc eType, TList<DWORD>& List )
{
	List.Clear();
	
	Item* pItem = NULL;
	m_mapContainer.ResetIterator();
	while (m_mapContainer.PeekNext(pItem))
	{	
		if (pItem->GetItemSpecFunc() == eType)
		{
			DWORD dwTypeID = pItem->GetItemTypeID();
			if (!List.IsExist(dwTypeID))
			{
				List.PushBack(dwTypeID);
			}
		}
	}
	
}

INT64 ItemContainer::GetItemSerialID( DWORD dwTypeID )
{
	Item* pItem = GetValue( GetItemPosition(dwTypeID) );
	
	if (P_VALID(pItem))
	{
		return pItem->GetItemId();
	}
	return GT_INVALID;
}

/** \计算物品权重
*/
inline INT64 CalcItemWeight(Item* pItem)
{
	INT64 nRet = 0;
	INT64 nTypeID = (INT64)pItem->GetItemTypeID();
	
	if( 1310001 == nTypeID )
	{
		nRet = pItem->GetPos();
	}
	else if( 1009999 >= nTypeID)
	{
		nRet = nTypeID * 1000 + pItem->GetPos();
	}
	else if( 1109999 >= nTypeID)
	{
		nRet = nTypeID * 1000 + pItem->GetPos();
	}
	else if( MIsEquipment(nTypeID))
	{
		nRet = nTypeID * 1000 + pItem->GetPos();
	}
	else
	{
		nRet = nTypeID * 1000 + pItem->GetPos() + 10000000000; 
	}

	return nRet;
}

VOID ItemContainer::AutoSettle( DWORD dwNPCID /*= GT_INVALID*/ )
{
	if(m_bSettle)
		return;

	// 检查是否锁定
	Item* pItem = NULL;
	m_mapContainer.ResetIterator();
	while(m_mapContainer.PeekNext(pItem))
	{
		if(!(P_VALID(pItem) && pItem->IsOperable()))
		{
			// 有锁定的物品则无法整理
			return;
		}
	}

	m_bSettle = true;

	
	// 锁定所有物品,并计算物品的权重
	TMap<INT64, Item*> TmpMap;
	m_mapContainer.ResetIterator();
	while(m_mapContainer.PeekNext(pItem))
	{
		pItem->SetOperable(false);
		TmpMap.Add(CalcItemWeight(pItem), pItem);
	}
	
	INT nMax = m_mapContainer.Size()*sizeof(tagItemOrder) + sizeof(tagNC_ItemReorderEx);
	BYTE* pMsg = new BYTE[nMax];
	// 发送网络消息
	pItem = NULL;
	INT16 nPos = 0;
	tagNC_ItemReorderEx* pCmd1 = (tagNC_ItemReorderEx*)pMsg;
	pCmd1->dwID = TObjRef<Util>()->Crc32("NC_ItemReorderEx");
	pCmd1->eContainerType = m_eConType;
	pCmd1->dwNPCID = dwNPCID;
	pCmd1->n16ItemNum = 0;
	INT nSize = 0;
	TmpMap.ResetIterator();
	while(TmpMap.PeekNext(pItem))
	{
		if(pItem->GetPos() != nPos)
		{
			tagItemOrder m;			
			m.n16OldIndex = pItem->GetPos();
			m.n16NewIndex = nPos;
			memcpy(pCmd1->byData+nSize, &m, sizeof(tagItemOrder));
			pCmd1->n16ItemNum++;
			nSize += sizeof(tagItemOrder);
		}
		nPos++;
	}
	
	if(nSize <= 0)// 如果没有数据则不发送排序消息
	{
		Unlock(true);
		return;
	}
	
	// 如果可变序列所占长度超出总体序列，则直接发送整体序列
	INT nMaxSize = m_mapContainer.Size()*sizeof(INT16);
	if(nSize >= nMaxSize)
	{
		tagNC_ItemReorder* pCmd2 = (tagNC_ItemReorder*)pMsg;
		pCmd2->dwID = TObjRef<Util>()->Crc32("NC_ItemReorder");
		pCmd2->dwNPCID = dwNPCID;
		pCmd2->eContainerType = m_eConType;
		pCmd2->n16ItemNum = m_mapContainer.Size();
		nSize = 0;
		TmpMap.ResetIterator();
		while(TmpMap.PeekNext(pItem))
		{
			INT16 n16Pos= pItem->GetPos();
			memcpy(((BYTE*)pCmd2->n16Index)+nSize, &n16Pos, sizeof(INT16));
			nSize += sizeof(INT16); 
		}
		
		pCmd2->dwSize = sizeof(tagNC_ItemReorder) + nSize - sizeof(INT16);
		TObjRef<NetSession>()->Send(pCmd2);
	}
	else
	{
		pCmd1->dwSize = sizeof(tagNC_ItemReorderEx) + nSize - sizeof(BYTE);
		TObjRef<NetSession>()->Send(pCmd1);
	}
	
	delete []pMsg;
}

INT32 ItemContainer::GetContainerItemQuantity()
{
	INT nCount = 0;
	Item* pItem = NULL;
	m_mapContainer.ResetIterator();
	while(m_mapContainer.PeekNext(pItem))
	{
		if(P_VALID(pItem))
			nCount += pItem->GetItemQuantity();
	}
	return nCount;
}

Item* ItemContainer::GetSpecFuncItem(  EItemConType eItemConType, EItemSpecFunc eSpecFunc )
{
	ASSERT( m_eConType == eItemConType && eSpecFunc != EISF_Null );

	//只有背包才能使用此功能，必须是查找特殊功能的物品
	if( m_eConType != eItemConType || eSpecFunc == EISF_Null )
		return NULL;

	Item* pItem = NULL;
	m_mapContainer.ResetIterator();
	while( m_mapContainer.PeekNext( pItem ) )
	{
		if( !P_VALID(pItem) )
			continue;

		if( pItem->GetItemSpecFunc() == eSpecFunc )
			return pItem;
	}

	return NULL;
}

Item* ItemContainer::GetSpecFuncItem(  EItemConType eItemConType, EItemSpecFunc eSpecFunc, DWORD dwItemTypeID )
{
	ASSERT( m_eConType == eItemConType && eSpecFunc != EISF_Null );

	//只有背包才能使用此功能，必须是查找特殊功能的物品
	if( m_eConType != eItemConType || eSpecFunc == EISF_Null )
		return NULL;

	Item* pItem = NULL;
	m_mapContainer.ResetIterator();
	while( m_mapContainer.PeekNext( pItem ) )
	{
		if( !P_VALID(pItem) )
			continue;

		if( pItem->GetItemSpecFunc() == eSpecFunc && pItem->GetItemTypeID() == dwItemTypeID )
			return pItem;
	}

	return NULL;
}

void ItemContainer::GetSpecFuncMap( EItemSpecFunc eSpecFunc, std::list<Item*>& lst )
{
	if( eSpecFunc == EISF_Null )
		return;

	Item* pItem = NULL;
	m_mapContainer.ResetIterator();
	while( m_mapContainer.PeekNext( pItem ) )
	{
		if( !P_VALID(pItem) )
			continue;

		if( pItem->GetItemSpecFunc() == eSpecFunc )
		{
			lst.push_back( pItem );
		}
	}
}
