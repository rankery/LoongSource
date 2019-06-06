//-------------------------------------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: ItemMgr.cpp
// author: Sxg
// actor:
// data: 2008-06-16
// last:
// brief: 物品/装备管理器
//-------------------------------------------------------------------------------------------------------
#pragma once

#include "StdAfx.h"
#include "../WorldDefine/msg_item.h"
#include "../WorldDefine/msg_func_npc.h"
#include "../ServerDefine/base_define.h"
#include "../ServerDefine/item_define.h"
#include "../ServerDefine/log_cmdid_define.h"
#include "../ServerDefine/msg_item.h"
#include "../ServerDefine/msg_log.h"
#include "../ServerDefine/role_data.h"
#include "container_restrict.h"
#include "item_creator.h"
#include "title_mgr.h"
#include "role.h"
#include "role_mgr.h"
#include "db_session.h"
#include "att_res.h"
#include "map.h"
#include "item_mgr.h"
#include "pet_soul.h"
#include "pet_pocket.h"
//-------------------------------------------------------------------------------------------------------
// 构造/析构函数
//-------------------------------------------------------------------------------------------------------
ItemMgr::ItemMgr(Role* pRole, DWORD dwAcctID, DWORD dwRoleID, INT16 n16BagSize, INT16 n16WareSize)
: m_Bag(EICT_Bag, n16BagSize, SPACE_ALL_BAG, dwRoleID, dwAcctID), 
m_QuestItemBag(EICT_Quest, SPACE_QUEST_BAG, SPACE_QUEST_BAG, dwRoleID, dwAcctID, new QusetItemBarRestrict),
m_BaiBaoBag(EICT_Baibao, SPACE_BAIBAO_BAG, SPACE_BAIBAO_BAG, dwRoleID, dwAcctID, new BaiBaoBagRestrict),
m_RoleWare(EICT_RoleWare, n16WareSize, SPACE_ALL_WARE, dwRoleID, dwAcctID, new WareRestrict),
m_EquipBar(EICT_Equip, X_EQUIP_BAR_SIZE, X_EQUIP_BAR_SIZE) 
{
	m_pRole = pRole;
	m_mapCDTime.Clear();
	m_mapMaxHold.Clear();
}

ItemMgr::~ItemMgr()
{
	tagCDInfo* pCDInfo = NULL;
	MapCDTime::TMapIterator iter = m_mapCDTime.Begin();
	while (m_mapCDTime.PeekNext(iter, pCDInfo))
	{
		if (P_VALID(pCDInfo))
			g_MemPoolSafe.Free(pCDInfo);
	}
	m_mapCDTime.Clear();
}



//-------------------------------------------------------------------------------------------------------
// 获得容器指针。注只能得到ItemContainer对象。
//-------------------------------------------------------------------------------------------------------
ItemContainer* ItemMgr::GetContainer(EItemConType eConType)
{
	switch(eConType)
	{
	case EICT_Bag:
		return &m_Bag;
		break;
	case EICT_Quest:
		return &m_QuestItemBag;
		break;
	case EICT_Baibao:
		return &m_BaiBaoBag;
		break;
	case EICT_RoleWare:
		return &m_RoleWare;
		break;
	case EICT_Equip:
	case EICT_Shop:
	case EICT_Ground:
		return NULL;
		break;
	}

	return NULL;
}

//-------------------------------------------------------------------------------------------------------
// 检查物品是否需要记录log。若需要，则向LoongDB发送消息
//-------------------------------------------------------------------------------------------------------
VOID ItemMgr::LogItem(const tagItem &item1, const tagItem *pItem2, 
					  INT16 n16OptNum, DWORD dwCmdID, DWORD dwRoleIDRel/* = GT_INVALID*/)
{
	if(!(IsItemNeedLog(item1) || (P_VALID(pItem2) && IsItemNeedLog(*pItem2))))
	{
		return;
	}

	tagNDBC_LogItem send;
	send.sLogItem.dwRoleID		= m_pRole->GetID();
	send.sLogItem.dwTypeID		= item1.pProtoType->dwTypeID;

	send.sLogItem.n64Serial1	= item1.n64Serial;
	send.sLogItem.n8ConType1	= item1.eConType;
	send.sLogItem.n16ResNum1	= item1.n16Num;

	send.sLogItem.n16OptNum		= n16OptNum;
	send.sLogItem.dwCmdID		= dwCmdID;

	send.sLogItem.dwRoleIDRel	= dwRoleIDRel;

	if(P_VALID(pItem2))
	{
		send.sLogItem.n64Serial2	= pItem2->n64Serial;
		send.sLogItem.n8ConType2	= pItem2->eConType;
		send.sLogItem.n16ResNum2	= pItem2->n16Num;
	}
	else
	{
		send.sLogItem.n64Serial2	= 0;
		send.sLogItem.n8ConType2	= EICT_Null;
		send.sLogItem.n16ResNum2	= 0;
	}

	g_dbSession.Send(&send, send.dwSize);
}

//-------------------------------------------------------------------------------------------------------
// 检查物品是否需要记录log。若需要，则向LoongDB发送消息
//-------------------------------------------------------------------------------------------------------
VOID ItemMgr::LogItemTimes(const tagItem &item, DWORD dwCmdID)
{
	if(!IsItemNeedLog(item))
	{
		return;
	}

	tagNDBC_LogItemTimes send;
	send.sLogItemTimes.dwRoleID		= m_pRole->GetID();
	send.sLogItemTimes.dwCmdID		= dwCmdID;
	send.sLogItemTimes.n64Serial	= item.n64Serial;
	send.sLogItemTimes.dwTypeID		= item.dwTypeID;
	send.sLogItemTimes.nUsedTimes	= item.nUseTimes;
	send.sLogItemTimes.nMaxUseTimes	= item.pProtoType->nMaxUseTimes;
	send.sLogItemTimes.n8ConType	= item.eConType;

	g_dbSession.Send(&send, send.dwSize);
}

//-------------------------------------------------------------------------------------------------------
// 装备鉴定
//-------------------------------------------------------------------------------------------------------
DWORD ItemMgr::IdentifyEquip(INT64 n64SerialReel, INT64 n64SerialEquip, DWORD dwCmdID)
{
	ItemContainer &Bag = GetBag();

	// 检查鉴定卷
	tagItem *pReel = Bag.GetItem(n64SerialReel);
	if(!P_VALID(pReel))
	{
		return E_Item_NotFound;
	}

	if(pReel->pProtoType->eSpecFunc != EISF_IdetifyEquip)
	{
		return GT_INVALID;
	}

	// 检查装备
	tagEquip *pEquip = (tagEquip*)Bag.GetItem(n64SerialEquip);
	if(!P_VALID(pEquip))
	{
		return E_Item_NotFound;
	}

	if(!MIsEquipment(pEquip->dwTypeID))
	{
		return E_Item_NotEquipment;
	}

	if(MIsIdentified(pEquip))
	{
		return E_Equip_Idendtifed;
	}

	// 检查鉴定卷等级是否满足要求(鉴定卷等级≥装备等级/10)
	if(pReel->pProtoType->byLevel < (pEquip->pProtoType->byLevel / 10))
	{
		return E_Item_LevelLimit;
	}

	// 生成鉴定后属性
	ItemCreator::IdentifyEquip(pEquip);

	// 称号事件触发
	m_pRole->GetTitleMgr()->SigEvent(ETE_IDENTIFY_BLUE_EQUIP, GT_INVALID, GT_INVALID);

	// 向DBServer和客户端发消息
	UpdateEquipSpec(*pEquip);

	// 使用一次
	ItemUsedFromBag(n64SerialReel, 1, dwCmdID);

	return E_Success;
}

//----------------------------------------------------------------------------------------
// 初始化物品装备 -- 发给客户端
//----------------------------------------------------------------------------------------
VOID ItemMgr::SendInitStateItem()
{
	ItemContainer	&Bag		= GetBag();
	ItemContainer	&QuestBag	= GetQuestItemBag();
	ItemContainer	&BaiBao		= GetBaiBaoBag();
	ItemContainer	&RoleWare	= GetRoleWare();
	EquipContainer	&EquipBar	= GetEquipBar();

	INT16 n16NumBag			= Bag.GetCurSpaceSize() - Bag.GetFreeSpaceSize();
	INT16 n16NumQuestBag	= QuestBag.GetCurSpaceSize() - QuestBag.GetFreeSpaceSize();
	INT16 n16NumBaiBao		= BaiBao.GetCurSpaceSize() - BaiBao.GetFreeSpaceSize();
	INT16 n16NumRoleWare	= RoleWare.GetCurSpaceSize() - RoleWare.GetFreeSpaceSize();
	INT16 n16NumEquipBar	= EquipBar.GetCurSpaceSize() - EquipBar.GetFreeSpaceSize();

	INT32 nItemNum = n16NumBag + n16NumQuestBag + n16NumBaiBao + n16NumRoleWare + n16NumEquipBar;

	// 计算可能的最大空间
	INT32 nMaxMsgSize = SIZE_EQUIP * nItemNum + sizeof(tagNS_GetRoleInitState_Item);

	// 申请消息
	MCREATE_MSG(pSend, nMaxMsgSize, NS_GetRoleInitState_Item);

	pSend->n16SzBag			= Bag.GetCurSpaceSize();
	pSend->n16SzRoleWare	= RoleWare.GetCurSpaceSize();
	pSend->nNum				= nItemNum;

	INT32 nSizeTotal	= 0;
	INT32 nSizeOne		= 0;

	GetAllItem(Bag, n16NumBag, pSend->byData + nSizeTotal, nSizeOne);
	nSizeTotal += nSizeOne;

	GetAllItem(QuestBag, n16NumQuestBag, pSend->byData + nSizeTotal, nSizeOne);
	nSizeTotal += nSizeOne;

	GetAllItem(BaiBao, n16NumBaiBao, pSend->byData + nSizeTotal, nSizeOne);
	nSizeTotal += nSizeOne;

	GetAllItem(RoleWare, n16NumRoleWare, pSend->byData + nSizeTotal, nSizeOne);
	nSizeTotal += nSizeOne;

	GetAllItem(EquipBar, n16NumEquipBar, pSend->byData + nSizeTotal, nSizeOne);
	nSizeTotal += nSizeOne;

	pSend->dwSize = sizeof(tagNS_GetRoleInitState_Item) - 1 + nSizeTotal;

	SendMessage(pSend, pSend->dwSize);

	// 释放消息内存
	MDEL_MSG(pSend);
}

//-------------------------------------------------------------------------------------------------------
// 角色初始化时，将物品&装备放入指定容器位置
//-------------------------------------------------------------------------------------------------------
DWORD ItemMgr::Put2Container(tagItem *pItem)
{	
	if(!P_VALID(m_pRole))
	{
		ASSERT(P_VALID(m_pRole));
		return GT_INVALID;
	}

	DWORD dwErrorCode = GT_INVALID;

	//加入物品上限管理
	if(!CanAddMaxHoldItem(*pItem)) return E_Item_MaxHold;

	switch(pItem->eConType)
	{
	case EICT_Bag:
		dwErrorCode = GetBag().Add(pItem, pItem->n16Index, FALSE, FALSE);
		break;
	case EICT_Quest:
		dwErrorCode = GetQuestItemBag().Add(pItem, pItem->n16Index, FALSE, FALSE);
		break;
	case EICT_Baibao:
		dwErrorCode = GetBaiBaoBag().Add(pItem, pItem->n16Index, FALSE, FALSE);
		break;
	case EICT_RoleWare:
		dwErrorCode = GetRoleWare().Add(pItem, pItem->n16Index, FALSE, FALSE);
		break;
	case EICT_Equip:
		dwErrorCode = GetEquipBar().Add((tagEquip*)pItem, (EEquipPos)pItem->n16Index);
		break;
	default:
		ASSERT(0);
		break;
	}

	// 物品上限管理
	if(dwErrorCode == E_Success)
		AddMaxHoldItem(*pItem);

	// 修改保存标志位
	pItem->SetUpdate(EUDBS_Null);

	return dwErrorCode;
}

//-----------------------------------------------------------------------------
// 是否可以交易
//-----------------------------------------------------------------------------
BOOL ItemMgr::CanExchange(const tagItem& item) const
{
	return !(item.bLock || item.IsBind()) && m_pRole->GetRoleStateEx().IsInState(ERSE_BagPsdPass);
}

//-----------------------------------------------------------------------------
// 玩家获得物品&装备 -- 普通物品放入背包，任务物品放入任务栏
//-----------------------------------------------------------------------------
BOOL ItemMgr::Add2Role(EItemCreateMode eCreateMode, DWORD dwCreateID, 
					   DWORD dwTypeID, INT32 nNum, EItemQuality eQlty, DWORD dwCmdID)
{
	INT16 n16BagSpace = 0;
	INT16 n16QuestSpace = 0;
	INT16 n16MaxLap;

	if(!CalSpaceUsed(dwTypeID, nNum, n16BagSpace, n16QuestSpace, n16MaxLap))
	{
		return FALSE;
	}

	if(n16BagSpace > 0 && GetBagFreeSize() < n16BagSpace
		|| n16QuestSpace > 0 && GetQuestItemBagFreeSize() < n16QuestSpace)
	{
		return FALSE;
	}

	tagItem *pNew;
	ItemContainer &con = n16BagSpace > 0 ? GetBag() : GetQuestItemBag();
	INT16 n16Space = max(n16BagSpace, n16QuestSpace);

	INT16 n16NewLeft = nNum;
	for(INT16 i=0; i<n16Space-1; ++i)
	{
		pNew = ItemCreator::CreateEx(eCreateMode, dwCreateID, dwTypeID, n16MaxLap, eQlty);
		if(!P_VALID(pNew))
		{
			return FALSE;
		}

		if(AddItem(con, pNew, dwCmdID, TRUE, TRUE) != E_Success)
		{
			Destroy(pNew);
			return FALSE;
		}

		n16NewLeft -= n16MaxLap;
	}

	pNew = ItemCreator::CreateEx(eCreateMode, dwCreateID, dwTypeID, n16NewLeft, eQlty);
	if(!P_VALID(pNew))
	{
		return FALSE;
	}

	if(AddItem(con, pNew, dwCmdID, TRUE, TRUE) != E_Success)
	{
		Destroy(pNew);
		return FALSE;
	}

	return TRUE;
}

//-------------------------------------------------------------------------------------------------------
// 从背包或任务栏中清除物品
//-------------------------------------------------------------------------------------------------------
DWORD ItemMgr::RemoveFromRole(DWORD dwTypeID, INT32 nNum, DWORD dwCmdID)
{
	if(IsQuestItem(dwTypeID))
	{
		if(GT_INVALID == nNum)
		{
			return RemoveItems(GetQuestItemBag(), dwTypeID, dwCmdID);
		}
		else
		{
			return RemoveItems(GetQuestItemBag(), dwTypeID, nNum, dwCmdID);
		}
	}
	else
	{
		if(GT_INVALID == nNum)
		{
			return RemoveItems(GetBag(), dwTypeID, dwCmdID);
		}
		else
		{
			return RemoveItems(GetBag(), dwTypeID, nNum, dwCmdID);
		}
	}
}

//-------------------------------------------------------------------------------------------------------
// 向容器中添加物品
//-------------------------------------------------------------------------------------------------------
DWORD ItemMgr::AddItem(ItemContainer& container, tagItem *&pItem, DWORD dwCmdID, 
					   BOOL bInsert2DB/* = FALSE*/, BOOL bCheckAdd/* = TRUE*/, DWORD dwRoleIDRel/* = GT_INVALID*/, BOOL bChangeOwner/* = TRUE*/)
{
	ASSERT(P_VALID(pItem));
	ASSERT(P_VALID(pItem->n64Serial));

	INT16		n16Index;
	DWORD		dwErrorCode;
	tagItemMove	itemMove;
	INT16		n16AddNum = pItem->n16Num;

	// 检查是否有获取时间
	if(0 == pItem->dw1stGainTime)
	{
		pItem->dw1stGainTime = g_world.GetWorldTime();
	}

	// 物品上限
	if ( CanAddMaxHoldItem(*pItem) )
	{
		// 加入容器
		dwErrorCode = container.Add(pItem, n16Index, itemMove, bCheckAdd);
	}
	else
	{
		dwErrorCode = E_Item_MaxHold;
	}

	if(dwErrorCode != E_Success)
	{
		ASSERT(E_Success == dwErrorCode);
		IMSG(_T("\nAdd item to container failed! \nroleid: %u, item serial: %lld, container type: %d!!!!\n\n"),
			container.GetOwnerID(), pItem->n64Serial, container.GetConType());

		//?? 考虑将该物品保存到log中

		return dwErrorCode;
	}

	// 管理物品上限列表
	AddMaxHoldItem(*pItem);

	INT16 n16Num = itemMove.n16NumRes1 != 0 ? itemMove.n16NumRes1: itemMove.n16NumRes2;

	// 如果是新物品,且没有和其他物品堆叠,则向LoongDB发消息
	if(bInsert2DB && !itemMove.bOverlap)
	{
		// 绑定处理
		if((EBS_Unknown == pItem->byBind) && bChangeOwner)
		{
			pItem->Bind();
		}

		InsertItem2DB(*pItem);
		pItem->SetUpdate(EUDBS_Null);

		SendAddNew2Client(pItem);
	}
	else
	{
		SendAddItem2Client(container.GetConType(), n16Index, pItem->n64Serial, n16Num, itemMove.bOverlap);
	}

	pItem->n16Num = itemMove.n16NumRes1;

	// 检查是否需要记录log
	LogItem(*pItem, itemMove.pItem2, n16AddNum, dwCmdID, dwRoleIDRel);

	// 任务触发
	if(container.GetConType() == EICT_Bag || container.GetConType() == EICT_Quest)
	{
		m_pRole->OnQuestEvent(EQE_Item, pItem->dwTypeID, n16AddNum, TRUE);
	}

	// 检查公用冷却时间
	CheckCDTimeMap(pItem->dwTypeID);

	// 触发称号事件
	if (container.GetConType() == EICT_Bag)
	{
		m_pRole->GetTitleMgr()->SigEvent(ETE_BAG_ITEM, pItem->dwTypeID, GT_INVALID);
	}

	// 检查是否需释放该物品内存
	if(itemMove.bOverlap || pItem->eConType == EICT_Ground)
	{
		DeleteItemFromDB(pItem->n64Serial, pItem->dwTypeID);

		Destroy(pItem);
		pItem = itemMove.pItem2;
	}

	return dwErrorCode;
}

//-------------------------------------------------------------------------------------------------------
// 向容器中指定位置添加物品
//-------------------------------------------------------------------------------------------------------
DWORD ItemMgr::Add2BagByIndex(tagItem *&pItem, DWORD dwCmdID, INT16 n16Index)
{
	ASSERT(P_VALID(pItem));
	ASSERT(P_VALID(pItem->n64Serial));

	DWORD		dwErrorCode;

	// 加入容器
	dwErrorCode =  GetBag().Add(pItem, n16Index, FALSE, FALSE);
	if(dwErrorCode != E_Success)
	{
		ASSERT(E_Success == dwErrorCode);
		IMSG(_T("\nAdd item to container failed! \nroleid: %u, item serial: %lld, container type: %d!!!!\n\n"),
			GetBag().GetOwnerID(), pItem->n64Serial, GetBag().GetConType());

		//?? 考虑将该物品保存到log中

		return dwErrorCode;
	}

	// 检查是否需要记录log
	LogItem(*pItem, NULL, pItem->n16Num, dwCmdID);

	return dwErrorCode;
}

//-------------------------------------------------------------------------------------------------------
// 向容器中指定位置添加物品并存入数据库
//-------------------------------------------------------------------------------------------------------
DWORD ItemMgr::Add2BagByIndexAndInsertDB( tagItem *&pItem, DWORD dwCmdID, INT16 n16Index )
{
	ASSERT(P_VALID(pItem));
	ASSERT(P_VALID(pItem->n64Serial));

	DWORD		dwErrorCode;

	// 加入容器
	dwErrorCode =  GetBag().Add(pItem, n16Index, TRUE, FALSE);
	if(dwErrorCode != E_Success)
	{
		ASSERT(E_Success == dwErrorCode);
		IMSG(_T("\nAdd item to container failed! \nroleid: %u, item serial: %lld, container type: %d!!!!\n\n"),
			GetBag().GetOwnerID(), pItem->n64Serial, GetBag().GetConType());

		//?? 考虑将该物品保存到log中

		return dwErrorCode;
	}

	//变态，绑定物品在ItemContainer::Add里面设置不了所有者，所以得在这里再弄一下
	pItem->SetOwner(m_pRole->GetID(), m_pRole->GetSession()->GetSessionID());

	// 记录数据库
	InsertItem2DB(*pItem);
	pItem->SetUpdate(EUDBS_Null);

	// 通知客户端
	SendAddNew2Client(pItem);
	//	SendAddItem2Client(EICT_Bag, pItem->n16Index, pItem->GetKey(), pItem->n16Num, pItem->pProtoType->n16MaxLapNum > 1);

	// 检查是否需要记录log
	LogItem(*pItem, NULL, pItem->n16Num, dwCmdID);

	return dwErrorCode;
}

//-------------------------------------------------------------------------------------------------------
// 从容器中删除物品
//-------------------------------------------------------------------------------------------------------
DWORD ItemMgr::RemoveItem(ItemContainer& container, INT64 n64Serial,  
						  DWORD dwCmdID, BOOL bDelFromDB, BOOL bDelMem, BOOL bCheckRemove, DWORD dwRoleIDRel/* = GT_INVALID*/)
{
	tagItem *pItem = container.GetItem(n64Serial);
	if(!P_VALID(pItem))
	{
		ASSERT(P_VALID(pItem));
		IMSG(_T("Can not find item<serial: %lld> in container<Type: %d, RoleID: %u>!!!\r\n"),
			n64Serial, container.GetConType(), container.GetOwnerID());
		return GT_INVALID;
	}

	INT16 n16OldIndex	= pItem->n16Index;
	INT16 n16Num		= pItem->n16Num;

	DWORD dwErrorCode = container.Remove(n64Serial, TRUE, bCheckRemove);
	if(dwErrorCode != E_Success)
	{
		ASSERT(E_Success == dwErrorCode);
		IMSG(_T("\nRemove item<roleid:%u, container type: %d, serial:%lld, error:%u> from container failed!\n\n"),
			container.GetOwnerID(), container.GetConType(), n64Serial, dwErrorCode);

		return dwErrorCode;
	}

	// 物品上限管理
	RemoveMaxHoldItem(pItem->dwTypeID, pItem->n16Num);

	// 向客户端发消息
	SendDelItem2Client(container.GetConType(), n16OldIndex, n64Serial, 0);

	// 检查是否需要记录log -- 即使此时物品要从游戏世界中删除,此时内存也没有释放
	LogItem(*pItem, NULL, -n16Num, dwCmdID, dwRoleIDRel);

	// 任务触发
	if(container.GetConType() == EICT_Bag || container.GetConType() == EICT_Quest)
	{
		m_pRole->OnQuestEvent(EQE_Item, pItem->dwTypeID, n16Num, FALSE);
	}

	// 释放内存
	if(bDelMem)
	{
		DeleteItemFromDB(n64Serial, pItem->dwTypeID);
		Destroy(pItem);
	}
	else if(bDelFromDB)
	{
		DeleteItemFromDB(n64Serial, pItem->dwTypeID);
	}

	return E_Success;
}

//-------------------------------------------------------------------------------------------------------
// 从容器中删除指定物品的个数(或次数)
//-------------------------------------------------------------------------------------------------------
DWORD ItemMgr::RemoveItem(ItemContainer& container, INT64 n64Serial, INT16 n16Num, DWORD dwCmdID,
						  BOOL bCheckRemove/* = TRUE*/, BOOL bDelete/* = FALSE*/)
{
	if(n16Num <= 0)
	{
		ASSERT(0);
		return GT_INVALID;
	}

	tagItem *pItem = container.GetItem(n64Serial);
	if(!P_VALID(pItem))
	{
		ASSERT(P_VALID(pItem));
		IMSG(_T("Can not find item<serial: %lld> in container<Type: %d, RoleID: %u>!!!\r\n"),
			n64Serial, container.GetConType(), container.GetOwnerID());
		return GT_INVALID;
	}

	/******************** 消耗使用次数 *******************/
	if(!bDelete && !pItem->CanOverlap() && pItem->pProtoType->nMaxUseTimes != 1)
	{
		ASSERT(!pItem->CanOverlap());

		// 不可堆叠物品减少的是使用次数
		pItem->IncUseTimes(n16Num);
		ASSERT( pItem->pProtoType->nMaxUseTimes == GT_INVALID || pItem->nUseTimes <= pItem->pProtoType->nMaxUseTimes );

		// 检查是否需要记录log
		LogItemTimes(*pItem, dwCmdID);

		//ASSERT(FALSE == bCheckRemove);
		if(pItem->pProtoType->nMaxUseTimes != GT_INVALID)
		{
			if(pItem->nUseTimes >= pItem->pProtoType->nMaxUseTimes)
			{
				return RemoveItem(container, n64Serial, dwCmdID, TRUE, TRUE, FALSE);
			}
			else
			{
				SendDelItem2Client(container.GetConType(), pItem->n16Index, n64Serial, pItem->nUseTimes);
			}
		}

		return E_Success;
	}

	/********************* 消耗物品 ******************/
	ASSERT(n16Num <= pItem->n16Num);

	// 检查是否完全删除
	if(n16Num == pItem->n16Num)
	{
		return RemoveItem(container, n64Serial, dwCmdID, TRUE, TRUE, bCheckRemove);
	}

	INT16 n16OldIndex = pItem->n16Index;

	DWORD dwErrorCode = container.Remove(n64Serial, n16Num, bCheckRemove);
	if(dwErrorCode != E_Success)
	{
		ASSERT(E_Success == dwErrorCode);
		IMSG(_T("\nRemove item<roleid:%u, container type: %d, serial:%lld, error:%u> from container failed!\n\n"),
			container.GetOwnerID(), container.GetConType(), n64Serial, dwErrorCode);

		return dwErrorCode;
	}

	// 物品上限管理
	RemoveMaxHoldItem(pItem->dwTypeID, n16Num);

	// 向客户端发消息
	SendDelItem2Client(container.GetConType(), n16OldIndex, n64Serial, pItem->n16Num);

	// 检查是否需要记录log
	LogItem(*pItem, NULL, -n16Num, dwCmdID);

	// 任务触发
	if(container.GetConType() == EICT_Bag || container.GetConType() == EICT_Quest)
	{
		m_pRole->OnQuestEvent(EQE_Item, pItem->dwTypeID, n16Num, FALSE);
	}

	return E_Success;
}

//-------------------------------------------------------------------------------------------------------
// 同一容器内移动物品(n16Num = 0 表示全部移动)
//-------------------------------------------------------------------------------------------------------
DWORD ItemMgr::Move(EItemConType eConType, INT64 n64Serial,	INT16 n16Num, INT16 n16PosDst, DWORD dwCmdID)
{
	ASSERT(n16Num >= 0);

	ItemContainer *pItemCon = GetContainer(eConType);
	if(!P_VALID(pItemCon))
	{
		return GT_INVALID;
	}

	tagItem *pItem1 = pItemCon->GetItem(n64Serial);
	if(!P_VALID(pItem1))
	{
		return GT_INVALID;
	}

	DWORD dwErrorCode	= GT_INVALID;
	INT16 n16SrcPos		= pItem1->n16Index;	// n64Serial原位置

	tagItemMove	itemMove;

	if(0 == n16Num)
	{
		n16Num = pItem1->n16Num;
		dwErrorCode = pItemCon->MoveTo(n64Serial, n16PosDst, itemMove);
	}
	else if(n16Num > 0)
	{
		dwErrorCode = pItemCon->MoveTo(n64Serial, n16Num, n16PosDst, itemMove);
	}

	if(dwErrorCode != E_Success)
	{
		ASSERT(E_Success == dwErrorCode);
#ifdef _DEBUG
		IMSG(_T("\nMove item<roleid:%u, container type: %d, serial:%lld, error:%u> in container failed!\n\n"),
			pItemCon->GetOwnerID(), pItemCon->GetConType(), n64Serial, dwErrorCode);
#endif // _DEBUG

		return dwErrorCode;
	}

	// 向客户端发消息
	tagNS_ItemPosChange send;
	send.eConType	= eConType;
	send.n64Serial1	= n64Serial;
	send.n64Serial2	= itemMove.pItem2 != NULL ? itemMove.pItem2->n64Serial : GT_INVALID;

	send.n16Num1	= itemMove.n16NumRes1;
	send.n16Num2	= itemMove.n16NumRes2;
	send.bCreateItem = itemMove.bCreateItem;

	if(itemMove.bChangePos)
	{
		send.n16PosDst1	= n16PosDst;
		send.n16PosDst2	= n16SrcPos;
	}
	else
	{
		send.n16PosDst1	= n16SrcPos;
		send.n16PosDst2	= n16PosDst;
	}

	if(itemMove.bOverlap)
	{
		send.n16PosDst1	= n16SrcPos;
		send.n16PosDst2	= n16PosDst;
	}

	SendMessage(&send, send.dwSize);

	// 检查是否需要记录log
	if(itemMove.bCreateItem || itemMove.bOverlap)
	{
		LogItem(*pItem1, itemMove.pItem2, -n16Num, dwCmdID);
	}

	// 检查移动的物品堆是否存在，如不存在,需与LoongDB同步和回收内存
	if(0 == itemMove.n16NumRes1 || pItem1->eConType == EICT_Ground)
	{
		DeleteItemFromDB(n64Serial, pItem1->dwTypeID);

		// 检查是否需释放该物品内存
		Destroy(pItem1);
	}

	// 检查是否创建了新的物品堆
	if(itemMove.bCreateItem)
	{
		tagNDBC_NewItem send;
		IFASTCODE->MemCpy(&send.item, itemMove.pItem2, SIZE_ITEM);

		g_dbSession.Send(&send, send.dwSize);

	}
	return dwErrorCode;
}

//-------------------------------------------------------------------------------------------------------
// 容器间物品移动
//-------------------------------------------------------------------------------------------------------
DWORD ItemMgr::Move2Other(EItemConType eConTypeSrc, INT64 n64Serial1, 
						  EItemConType eConTypeDst, INT16 n16PosDst, DWORD dwCmdID)
{
	ItemContainer *pConSrc = GetContainer(eConTypeSrc);
	if(!P_VALID(pConSrc))
	{
		return GT_INVALID;
	}

	ItemContainer *pConDst = GetContainer(eConTypeDst);
	if(!P_VALID(pConDst))
	{
		return GT_INVALID;
	}

	tagItem *pItem1 = pConSrc->GetItem(n64Serial1);

	if(!P_VALID(pItem1) || pItem1->eConType!= eConTypeSrc)
	{
		return GT_INVALID;
	}

	tagItem  item1Old; //used for log
	memcpy(&item1Old, pItem1, sizeof(tagItem));

	if ( (eConTypeDst == EICT_RoleWare ) && ( eConTypeSrc == EICT_Bag ) && (pItem1->pProtoType->eSpecFunc == EISF_PortableWare))
	{
		return GT_INVALID;
	}

	DWORD dwErrorCode	= GT_INVALID;
	INT16 n16PosSrc1	= pItem1->n16Index;	// n64Serial原位置
	INT16 n16Num		= pItem1->n16Num;
	DWORD dwTypeID		= pItem1->dwTypeID;

	tagItemMove	itemMove;

	if(GT_INVALID == n16PosDst)	// 右击，没有指定目标位置
	{
		dwErrorCode = pConSrc->MoveTo(n64Serial1, *pConDst, itemMove, n16PosDst);
	}
	else
	{
		dwErrorCode = pConSrc->MoveTo(n64Serial1, *pConDst, n16PosDst, itemMove);
	}

	if(dwErrorCode != E_Success)
	{
		ASSERT(E_Success == dwErrorCode);
#ifdef _DEBUG
		IMSG(_T("\nMove item<roleid:%u, container type: %d, serial:%lld, error:%u> in container failed!\n\n"),
			pConSrc->GetOwnerID(), pConSrc->GetConType(), n64Serial1, dwErrorCode);
#endif // _DEBUG

		return dwErrorCode;
	}

	// 向客户端发消息
	tagNS_ItemPosChangeEx send;

	send.eConTypeSrc1	= eConTypeSrc;
	send.eConTypeSrc2	= eConTypeDst;
	send.n64Serial1		= n64Serial1;
	send.n64Serial2		= itemMove.pItem2 != NULL ? itemMove.pItem2->n64Serial : GT_INVALID;
	send.n16Num1		= itemMove.n16NumRes1;
	send.n16Num2		= itemMove.n16NumRes2;

	if(itemMove.bOverlap)
	{
		send.eConTypeDst1	= eConTypeSrc;
		send.eConTypeDst2	= eConTypeDst;
		send.n16PosDst1		= n16PosSrc1;
		send.n16PosDst2		= n16PosDst;
	}
	else
	{
		send.eConTypeDst1	= eConTypeDst;
		send.eConTypeDst2	= eConTypeSrc;
		send.n16PosDst1		= n16PosDst;
		send.n16PosDst2		= n16PosSrc1;
	}

	SendMessage(&send, send.dwSize);

	// 记录百宝袋领取历史记录
	if (eConTypeSrc == EICT_Baibao && eConTypeDst == EICT_Bag)
	{
		if ((itemMove.n16NumRes1 != 0) && (EBS_Unknown == pItem1->byBind))
		{
			pItem1->Bind();
			pItem1->SetUpdate(EUDBS_Update);
			SendItemBind(pItem1->GetKey(), pItem1->eConType);
		}
		ProcBaiBaoRecord(dwTypeID, m_pRole->GetID(), m_pRole->GetID(), EBBRT_Myself);
	}

	// 任务触发
	if( (eConTypeSrc == EICT_RoleWare || eConTypeSrc == EICT_GuildWare) && eConTypeDst == EICT_Bag )
	{
		m_pRole->OnQuestEvent(EQE_Item, dwTypeID, n16Num, TRUE);
	}

	if(eConTypeSrc == EICT_Bag && (eConTypeDst == EICT_RoleWare || eConTypeDst == EICT_GuildWare))
	{
		m_pRole->OnQuestEvent(EQE_Item, dwTypeID, n16Num, FALSE);
	}

	// 检查是否需要记录log
	if(itemMove.pItem2)
	{
		if(itemMove.pItem2->eConType == item1Old.eConType) // swap
		{
			item1Old.n16Num = 0;
			LogItem(item1Old, pItem1, -pItem1->n16Num, dwCmdID);
			memcpy(&item1Old, itemMove.pItem2, sizeof(tagItem));
			item1Old.n16Num = 0;
			item1Old.eConType = eConTypeDst;
			LogItem(item1Old, itemMove.pItem2, -itemMove.pItem2->n16Num, dwCmdID);
		}
		else
		{
			item1Old.n16Num = pItem1->eConType==EICT_Ground? 0 : pItem1->n16Num;
			LogItem(item1Old, itemMove.pItem2, item1Old.n16Num - n16Num, dwCmdID);
		}
	}
	else
	{
		item1Old.n16Num = 0;
		LogItem(item1Old, pItem1, -n16Num, dwCmdID);
	}	

	// 检查移动的物品堆是否存在，如不存在,需与LoongDB同步和回收内存
	if(0 == itemMove.n16NumRes1 || pItem1->eConType == EICT_Ground)
	{
		DeleteItemFromDB(n64Serial1, pItem1->dwTypeID);

		// 释放该物品内存
		Destroy(pItem1);
	}

	return dwErrorCode;
}

//-------------------------------------------------------------------------------------------------------
// 添加背包中多个物品(玩家间交易,邮件) -- private
//-------------------------------------------------------------------------------------------------------
VOID ItemMgr::AddItems(ItemContainer& container, tagItem* pItem[], INT32 nSize, DWORD dwCmdID, DWORD dwRoleIDRel)
{
	DWORD dwErrorCode;
	for(INT32 i=0; i<nSize; ++i)
	{
		if(NULL == pItem[i])
		{
			return;
		}

		dwErrorCode = AddItem(container, pItem[i], dwCmdID, TRUE, TRUE, dwRoleIDRel);
		ASSERT(E_Success == dwErrorCode);
	}
}

//-------------------------------------------------------------------------------------------------------
// 删除背包中多个物品(玩家间交易,邮件)
//-------------------------------------------------------------------------------------------------------
VOID ItemMgr::RemoveItems(ItemContainer& container, INT64 n64Serial[], INT32 nSize, DWORD dwCmdID, DWORD dwRoleIDRel)
{
	for(INT32 i=0,j=0; i<nSize; ++i)
	{
		if(n64Serial[i] != 0)
		{
			DWORD dwErrorCode = RemoveItem(container, n64Serial[i], dwCmdID, TRUE, FALSE, FALSE, dwRoleIDRel);
			ASSERT(E_Success == dwErrorCode);
		}
	}
}

//-------------------------------------------------------------------------------------------------------
// 从容器中删除某类物品(该类物品被销毁)，返回成功个数
//-------------------------------------------------------------------------------------------------------
DWORD ItemMgr::RemoveItems(ItemContainer& container, DWORD dwTypeID, DWORD dwCmdID)
{
	DWORD dwErrorCode;

	TList<tagItem*> itemList;
	container.GetSameItemList(itemList, dwTypeID);

	tagItem *pItem;
	itemList.ResetIterator();
	while(itemList.PeekNext(pItem))
	{		
		dwErrorCode = RemoveItem(container, pItem->n64Serial, dwCmdID, TRUE, TRUE, FALSE);
		ASSERT(E_Success == dwErrorCode);
	}

	return E_Success;
}

//-------------------------------------------------------------------------------------------------------
// 从容器中删除指定个数的某类物品(该物品被销毁)，返回成功个数
//-------------------------------------------------------------------------------------------------------
DWORD ItemMgr::RemoveItems(ItemContainer& container, DWORD dwTypeID, INT32 nNum, DWORD dwCmdID)
{
	if(nNum <= 0)
	{
		ASSERT(nNum > 0);
		return GT_INVALID;
	}

	TList<tagItem*> itemList;
	INT32 nNumTotal = container.GetSameItemList(itemList, dwTypeID);

	// 不够
	if(nNumTotal < nNum)
	{
		// 删除失败,发送监控信息 //??

		ASSERT(nNumTotal >= nNum);
		return GT_INVALID;
	}

	// 刚好, 或富余
	DWORD dwErrorCode;
	INT16 n16NumDel = 0;
	INT32 nNumNeedDel = nNum;
	tagItem *pItem = NULL;
	itemList.ResetIterator();
	while(itemList.PeekNext(pItem) && nNumNeedDel != 0)
	{
		if(pItem->n16Num <= nNumNeedDel)
		{
			n16NumDel = pItem->n16Num;
			dwErrorCode = RemoveItem(container, pItem->n64Serial, dwCmdID, TRUE, TRUE, FALSE);
			if(dwErrorCode != E_Success)
			{
				ASSERT(E_Success == dwErrorCode);
				n16NumDel = 0;
			}

			nNumNeedDel -= n16NumDel;
		}
		else
		{
			n16NumDel = nNumNeedDel;
			dwErrorCode = RemoveItem(container, pItem->n64Serial, nNumNeedDel, dwCmdID, FALSE);
			if(dwErrorCode != E_Success)
			{
				ASSERT(E_Success == dwErrorCode);
				n16NumDel = 0;
			}

			nNumNeedDel -= n16NumDel;
		}
	}

	ASSERT(0 == nNumNeedDel);

	return E_Success;
}

//-----------------------------------------------------------------------------
// 从游戏中删除和指定任务相关的物品
//-----------------------------------------------------------------------------
VOID ItemMgr::RemoveItems(ItemContainer& container, UINT16 u16QuestID, DWORD dwCmdID)
{
	const tagItem *pItem;
	for(INT16 i=0; i<container.GetCurSpaceSize(); ++i)
	{
		pItem = container.GetItem(i);
		if(!P_VALID(pItem))
		{
			continue;
		}

		// 和任务相关则删除该物品
		if(u16QuestID == pItem->pProtoType->dwQuestID)
		{
			RemoveItem(container, pItem->n64Serial, dwCmdID, TRUE, TRUE, FALSE);
		}
	}
}

//-----------------------------------------------------------------------------
// 从装备栏中删除和指定任务相关的物品
//-----------------------------------------------------------------------------
VOID ItemMgr::RemoveItems(EquipContainer& container, UINT16 u16QuestID, DWORD dwCmdID)
{
	const tagItem *pItem;
	for(INT16 i=0; i<container.GetCurSpaceSize(); ++i)
	{
		pItem = container.GetItem(i);
		if(!P_VALID(pItem))
		{
			continue;
		}

		// 和任务相关则删除该物品
		if(u16QuestID == pItem->pProtoType->dwQuestID)
		{
			RemoveFromEquipBar(pItem->n64Serial, dwCmdID, TRUE);
		}
	}
}

//-------------------------------------------------------------------------------------------------------
// 检查背包中多个物品是否存在(玩家间交易,邮件)
//-------------------------------------------------------------------------------------------------------
BOOL ItemMgr::CheckItemsExist(OUT tagItem* pItem[], ItemContainer& container, 
							  INT64 n64Serial[], INT16 n16Num[], INT32 nSize)
{
	ASSERT(pItem != NULL);
	ASSERT(n64Serial != NULL);
	ASSERT(n16Num != NULL);

	for(INT32 i=0,j=0; i<nSize; ++i)
	{
		if(n64Serial[i] != 0)
		{
			pItem[j] = container.GetItem(n64Serial[i]);
			if(!P_VALID(pItem[j]) || pItem[j]->n16Num != n16Num[i])
			{
				return FALSE;
			}

			++j;
		}
	}

	return TRUE;
}

//-------------------------------------------------------------------------------------------------------
// 向客户端发送消息
//-------------------------------------------------------------------------------------------------------
VOID ItemMgr::SendMessage(LPVOID pMsg, DWORD dwSize)
{
	ASSERT(P_VALID(m_pRole));

	PlayerSession *pSession = m_pRole->GetSession();
	if(P_VALID(pSession))
	{
		pSession->SendMessage(pMsg, dwSize);
	}

	ASSERT(P_VALID(pSession));
}

//-------------------------------------------------------------------------------------------------------
// 向容器中添加物品，发送到客户端消息封装
//-------------------------------------------------------------------------------------------------------
VOID ItemMgr::SendAddNew2Client(const tagItem *pItem)
{
	if(MIsEquipment(pItem->dwTypeID))
	{
		tagNS_NewEquipAdd Send;
		IFASTCODE->MemCpy(&Send.Equip, pItem, SIZE_EQUIP);
		Send.Equip.equipSpec.n16QltyModPctEx = 0;	// 对客户端隐藏二级修正率

		SendMessage(&Send, Send.dwSize);
	}
	else
	{
		tagNS_NewItemAdd Send;
		IFASTCODE->MemCpy(&Send.Item, pItem, SIZE_ITEM);

		SendMessage(&Send, Send.dwSize);
	}
}

//-------------------------------------------------------------------------------------------------------
// 向容器中添加物品，发送到客户端消息封装
//-------------------------------------------------------------------------------------------------------
VOID ItemMgr::SendAddItem2Client(EItemConType eConType, INT16 n16Index, INT64 n64Serial, INT16 n16Num, BOOL bOverlap)
{
	tagNS_ItemAdd	send;
	send.n64Serial	= n64Serial;
	send.n16Index	= n16Index;
	send.n16Num		= n16Num;
	send.eConType	= eConType;
	send.bOverlap	= bOverlap ? true : false;

	SendMessage(&send, send.dwSize);
}

//-------------------------------------------------------------------------------------------------------
// 从容器中删除物品，发送到客户端消息封装
//-------------------------------------------------------------------------------------------------------
VOID ItemMgr::SendDelItem2Client(EItemConType eConType, INT16 n16Index, INT64 n64Serial, INT16 n16Num)
{
	tagNS_ItemRemove send;
	//send.dwErrorCode= dwErrorCode;
	send.n64Serial	= n64Serial;
	send.n16Index	= n16Index;
	send.n16Num		= n16Num;
	send.eConType	= eConType;

	SendMessage(&send, send.dwSize);
}

//-------------------------------------------------------------------------------------------------------
// 向LoongDB发消息
//-------------------------------------------------------------------------------------------------------
VOID ItemMgr::InsertItem2DB(tagItem &item)
{
	if(MIsEquipment(item.dwTypeID))
	{
		tagNDBC_NewEquip send;
		IFASTCODE->MemCpy(&send.equip, &item, SIZE_EQUIP);
		g_dbSession.Send(&send, send.dwSize);
	}
	else
	{
		tagNDBC_NewItem send;
		IFASTCODE->MemCpy(&send.item, &item, SIZE_ITEM);
		g_dbSession.Send(&send, send.dwSize);
	}
}

//-------------------------------------------------------------------------------------------------------
// 向LoongDB发消息
//-------------------------------------------------------------------------------------------------------
VOID ItemMgr::DeleteItemFromDB(INT64 n64Serial, INT32 dwTypeID)
{
	if(!MIsEquipment(dwTypeID))
	{
		tagNDBC_DelItem send;
		send.n64Serial = n64Serial;

		g_dbSession.Send(&send, send.dwSize);
	}
	else
	{
		tagNDBC_DelEquip send;
		send.n64Serial = n64Serial;

		g_dbSession.Send(&send, send.dwSize);
	}
}

//-------------------------------------------------------------------------------------------------------
// 更新物品信息 -- 保存角色数据时调用
//-------------------------------------------------------------------------------------------------------
VOID ItemMgr::SaveItem2DB(OUT LPVOID pData, OUT LPVOID &pOutPointer, OUT INT32 &nNum)
{
	INT32 nTmpNum;
	nNum = 0;

	Save2DB(GetBag(), pData, pData, nTmpNum);
	nNum += nTmpNum;

	Save2DB(GetQuestItemBag(), pData, pData, nTmpNum);
	nNum += nTmpNum;

	Save2DB(GetBaiBaoBag(), pData, pData, nTmpNum);
	nNum += nTmpNum;

	Save2DB(GetRoleWare(), pData, pData, nTmpNum);
	nNum += nTmpNum;

	Save2DB(GetEquipBar(), pData, pData, nTmpNum);
	nNum += nTmpNum;

	pOutPointer = pData;
}

//-----------------------------------------------------------------------------
// 装备属性改变后，即时保存数据库
//-----------------------------------------------------------------------------
VOID ItemMgr::SendEquipSpec2DB(const tagEquip &equip)
{
	tagNDBC_UpdateEquipAtt send;
	send.equipSpecUpdate.n64Serial = equip.n64Serial;
	IFASTCODE->MemCpy(&send.equipSpecUpdate.equipSpec, &equip.equipSpec, sizeof(tagEquipSpec));

	g_dbSession.Send(&send, send.dwSize);
}

//-----------------------------------------------------------------------------
// 装备属性改变后，向客户端发消息
//-----------------------------------------------------------------------------
VOID ItemMgr::SendEquipSpec2Client(const tagEquip &equip)
{
	tagNS_EquipChange send;
	send.n64Serial = equip.n64Serial;
	IFASTCODE->MemCpy(&send.equipSpec, &equip.equipSpec, sizeof(tagEquipSpec));
	send.equipSpec.n16QltyModPctEx = 0;	// 对客户端隐藏二级修正率
	SendMessage(&send, send.dwSize);
}

//-------------------------------------------------------------------------------------------------------
// 每个tick调用一次
//-------------------------------------------------------------------------------------------------------
VOID ItemMgr::Update()
{
	UpdateCDTime();

	// 容器update
	UpdateContainer(m_Bag);
	UpdateContainer(m_QuestItemBag);
	UpdateContainer(m_BaiBaoBag);
	UpdateContainer(m_RoleWare);
	UpdateContainer(m_EquipBar);
}

//-------------------------------------------------------------------------------------------------------
// 每个tick调用一次
//-------------------------------------------------------------------------------------------------------
VOID ItemMgr::UpdateContainer(ItemContainer& sItemCon)
{
	if( sItemCon.GetConType() != EICT_Baibao ) // 百宝袋中的时限物品不需要计时
	{
		sItemCon.Update();
	}
	
	TList<INT64>& lstNeedDel = sItemCon.GetNeedDelList();
	if(lstNeedDel.Size() <= 0)
	{
		return;
	}

	INT64 n64Serial;
	TList<INT64>::TListIterator iter = lstNeedDel.Begin();
	while(lstNeedDel.PeekNext(iter, n64Serial))
	{
		if(this->RemoveItem(sItemCon, n64Serial, (DWORD)ELCLD_Item_AtTerm, TRUE, TRUE, FALSE) != E_Success)
		{
			ASSERT(0);
			IMSG(_T("Item<%ld> time at term, but delete failed! Please check!\n"), n64Serial);
		}
	}

	sItemCon.ClearNeedDelList();
}

//-------------------------------------------------------------------------------------------------------
// 每个tick调用一次
//-------------------------------------------------------------------------------------------------------
VOID ItemMgr::UpdateContainer(EquipContainer& sEquipCon)
{
	sEquipCon.Update();

	TList<INT64>& lstNeedDel = sEquipCon.GetNeedDelList();
	if(lstNeedDel.Size() <= 0)
	{
		return;
	}

	INT64 n64Serial;
	TList<INT64>::TListIterator iter = lstNeedDel.Begin();
	while(lstNeedDel.PeekNext(iter, n64Serial))
	{
		this->RemoveFromEquipBar(n64Serial, (DWORD)ELCLD_Item_AtTerm, TRUE);
	}

	sEquipCon.ClearNeedDelList();
}

//-------------------------------------------------------------------------------------------------------
// 每个tick更新一次冷却时间
//-------------------------------------------------------------------------------------------------------
VOID ItemMgr::UpdateCDTime()
{
	if(m_mapCDTime.Size() == 0)
	{
		return;
	}

	DWORD		dwTypeID;
	tagCDInfo*	pCDInfo = NULL;
	MapCDTime::TMapIterator iter = m_mapCDTime.Begin();
	while(m_mapCDTime.PeekNext(iter, dwTypeID, pCDInfo))
	{
		if (!P_VALID(pCDInfo))
		{
			m_mapCDTime.Erase(dwTypeID);
			continue;
		}
		if(pCDInfo->dwRemainTime <= TICK_TIME)
		{
			g_MemPoolSafe.Free(pCDInfo);
			m_mapCDTime.Erase(dwTypeID);
			continue;
		}

		pCDInfo->dwElapseTime += TICK_TIME;
		pCDInfo->dwRemainTime -= TICK_TIME;
	}
}

//-------------------------------------------------------------------------------------------------------
// 添加新类型物品的冷却时间
//-------------------------------------------------------------------------------------------------------
VOID ItemMgr::Add2CDTimeMap(DWORD dwTypeID, DWORD dwCDTime/* = GT_INVALID*/)
{
	if ( m_pRole->ObjectCoolOff() == TRUE)
	{
		return;
	}

	if(GT_INVALID == dwCDTime)
	{
		MapCDTime mapCDTime;
		GetSameCDItemList(mapCDTime, dwTypeID);

		DWORD dwTempTypeID	= GT_INVALID;
		tagCDInfo* pCDInfo	= NULL;
		MapCDTime::TMapIterator iter = mapCDTime.Begin();
		while (mapCDTime.PeekNext(iter, dwTempTypeID, pCDInfo))
		{
			if (!GT_VALID(dwTempTypeID) || !P_VALID(pCDInfo))
			{
				continue;
			}

			// 已经检查过原表中是否存在
			m_mapCDTime.Add(dwTempTypeID, pCDInfo);

			tagNS_ItemCDUpdate	send;
			send.dwTypeID = dwTempTypeID;
			send.dwCDTime = pCDInfo->dwRemainTime;
			m_pRole->SendMessage(&send, send.dwSize);
		}
	}
	else
	{
		tagCDInfo* pCDInfo = (tagCDInfo*)g_MemPoolSafe.Alloc(sizeof(tagCDInfo));
		if (!P_VALID(pCDInfo))	return;

		tagItemProto *pProto = g_attRes.GetItemProto(dwTypeID);
		if(!P_VALID(pProto))	return;

		pCDInfo->dwElapseTime = max(0, pProto->dwCooldownTime - dwCDTime);
		pCDInfo->dwRemainTime = dwCDTime;
		m_mapCDTime.Add(dwTypeID, pCDInfo);

		//tagNS_ItemCDUpdate	send;
		//send.dwTypeID = dwTypeID;
		//send.dwCDTime = dwCDTime;
		//m_pRole->SendMessage(&send, send.dwSize);
	}
}

//-------------------------------------------------------------------------------------------------------
// 取得背包中同组CD物品类型列表
//-------------------------------------------------------------------------------------------------------
VOID ItemMgr::GetSameCDItemList(TMap<DWORD, tagCDInfo*> &mapSameCD, DWORD dwTypeID)
{
	mapSameCD.Clear();

	// 上层逻辑错误
	if (IsItemCDTime(dwTypeID))
	{
		return;
	}

	tagItemProto *pProto = g_attRes.GetItemProto(dwTypeID);
	if(!P_VALID(pProto))
	{
		ASSERT(P_VALID(pProto));
		return;
	}

	// 没有同组物品
	if (pProto->eTypeReserved == EITR_Null)
	{
		// 将自身类型加入

		tagCDInfo* pCDInfo = (tagCDInfo*)g_MemPoolSafe.Alloc(sizeof(tagCDInfo));
		if (!P_VALID(pCDInfo))
		{
			return;
		}
		pCDInfo->dwElapseTime = 0;
		pCDInfo->dwRemainTime = pProto->dwCooldownTime;

		mapSameCD.Add(dwTypeID, pCDInfo);
		return;
	}

	// 遍历背包
	tagItem* pItem = NULL;
	TMap<INT64, INT16>::TMapIterator iter = GetBag().Begin();

	while(GetBag().GetNextItem(iter, pItem))
	{
		if (!P_VALID(pItem))
		{
			continue;
		}
		if (mapSameCD.IsExist(pItem->dwTypeID) || m_mapCDTime.IsExist(pItem->dwTypeID))
		{
			continue;
		}
		if (pItem->pProtoType->eTypeReserved == pProto->eTypeReserved)
		{
			tagCDInfo* pCDInfo = (tagCDInfo*)g_MemPoolSafe.Alloc(sizeof(tagCDInfo));
			if (!P_VALID(pCDInfo))
			{
				continue;
			}
			pCDInfo->dwElapseTime = 0;

			if (pItem->pProtoType->dwCooldownTime != 0)
			{
				pCDInfo->dwRemainTime = pItem->pProtoType->dwCooldownTime;
			}
			else
			{
				pCDInfo->dwRemainTime = 1000;
			}
			mapSameCD.Add(pItem->dwTypeID, pCDInfo);
		}
	}
}

//-------------------------------------------------------------------------------------------------------
// 物品是否处于冷却时间
//-------------------------------------------------------------------------------------------------------
BOOL ItemMgr::IsItemCDTime(DWORD dwTypeID)
{
	if(m_mapCDTime.IsExist(dwTypeID))
		return TRUE;
	else
		return FALSE;
}

//-------------------------------------------------------------------------------------------------------
// 检查同类物品的公用冷却时间
//-------------------------------------------------------------------------------------------------------
VOID ItemMgr::CheckCDTimeMap(DWORD dwTypeID)
{
	// 该物品已经存在CD
	if (IsItemCDTime(dwTypeID))
	{
		return;
	}

	// 取得物品属性
	tagItemProto *pProto = g_attRes.GetItemProto(dwTypeID);
	if (!P_VALID(pProto))
	{
		return;
	}

	// 没有公用CD的物品忽略
	if (pProto->eTypeReserved == EITR_Null)
	{
		return;
	}

	// 遍历已存在CD的物品
	DWORD		dwTempTypeID;
	tagCDInfo*	pTempCDInfo = NULL;

	MapCDTime::TMapIterator iter = m_mapCDTime.Begin();
	while (m_mapCDTime.PeekNext(iter, dwTempTypeID, pTempCDInfo))
	{
		if (!GT_VALID(dwTempTypeID) || !P_VALID(pTempCDInfo))
		{
			continue;
		}

		// 取得物品属性
		tagItemProto *pTempProto = g_attRes.GetItemProto(dwTempTypeID);
		if(!P_VALID(pTempProto))
		{
			continue;
		}

		// 判断是否是同类物品
		if ((pTempProto->eTypeReserved == pProto->eTypeReserved)
			&& (pProto->dwCooldownTime > pTempCDInfo->dwElapseTime))
		{
			tagCDInfo* pCDInfo = (tagCDInfo*)g_MemPoolSafe.Alloc(sizeof(tagCDInfo));
			if (!P_VALID(pCDInfo))
			{
				continue;
			}

			pCDInfo->dwElapseTime	= pTempCDInfo->dwElapseTime;
			pCDInfo->dwRemainTime	= pProto->dwCooldownTime - pTempCDInfo->dwElapseTime;

			m_mapCDTime.Add(dwTypeID, pCDInfo);

			tagNS_ItemCDUpdate	send;
			send.dwTypeID = dwTypeID;
			send.dwCDTime = pCDInfo->dwRemainTime;
			m_pRole->SendMessage(&send, send.dwSize);

			// 跳出循环
			break;
		}
	}
}

//-------------------------------------------------------------------------------------------------------
// 将冷却时间保存到数据库中
//-------------------------------------------------------------------------------------------------------
VOID ItemMgr::FormatCDTime(OUT LPVOID pData, OUT LPVOID &pOutPointer, OUT INT32 &nNum)
{
	if(!P_VALID(m_pRole))
	{
		ASSERT(P_VALID(m_pRole));
		return;
	}

	nNum = m_mapCDTime.Size();
	pOutPointer = pData;

	if(nNum != 0)
	{
		DWORD		dwTypeID;
		tagCDInfo*	pCDInfo = NULL;
		INT32		nCounter = 0;

		MTRANS_POINTER(pCDTime, pData, tagCDTime);
		MapCDTime::TMapIterator iter = m_mapCDTime.Begin();
		while(m_mapCDTime.PeekNext(iter, dwTypeID, pCDInfo))
		{
			pCDTime[nCounter].dwTypeID	= dwTypeID;
			pCDTime[nCounter].dwTime	= pCDInfo->dwRemainTime;

			++nCounter;
		}

		pOutPointer = &pCDTime[nCounter];
	}
}

//-------------------------------------------------------------------------------------------------------
// 将冷却时间发送到客户端
//-------------------------------------------------------------------------------------------------------
VOID ItemMgr::SendInitStateItemCDTime()
{
	INT32 nNum = m_mapCDTime.Size();
	if(0 == nNum)
	{
		return;
	}

	INT32 nSize = sizeof(tagNS_GetRoleInitState_ItemCDTime) - 1 + sizeof(tagCDTime) * nNum;

	MCREATE_MSG(pSend, nSize, NS_GetRoleInitState_ItemCDTime);
	pSend->nNum = nNum;

	LPVOID pDummy;
	FormatCDTime(pSend->byData, pDummy, nNum);

	SendMessage(pSend, pSend->dwSize);

	MDEL_MSG(pSend);
}

//-----------------------------------------------------------------------------
// 数组中是否有重复ID
//-----------------------------------------------------------------------------
BOOL ItemMgr::IsRepeatID(INT64 n64ItemID[], INT32 nArraySz)
{
	//定以一个数组用来承接需要测试的数组
	set<INT64> idSet;

	for(INT n = 0; n < nArraySz; ++n)
	{
		idSet.insert(n64ItemID[n]);
	}

	if(idSet.size() != nArraySz)
		return TRUE;
	else
		return FALSE;
}

//-----------------------------------------------------------------------------
// 计算占用格子数
//-----------------------------------------------------------------------------
BOOL ItemMgr::CalSpaceUsed(DWORD dwTypeID, INT32 nNum, OUT INT16 &n16UseBagSpace, 
						   OUT INT16 &n16UseQuestSpace, OUT INT16 &n16MaxLap)
{
	n16UseBagSpace = n16UseQuestSpace = 0;

	const tagItemProto *pProto;
	if(!MIsEquipment(dwTypeID))
	{
		pProto = g_attRes.GetItemProto(dwTypeID);
	}
	else
	{
		pProto = g_attRes.GetEquipProto(dwTypeID);
	}

	if(!P_VALID(pProto))
	{
		ASSERT(0);
		IMSG(_T("Cannot find item/equip proto<id: %ld>!"), dwTypeID);
		return FALSE;
	}

	// 计算占用的格子
	INT16 nUseSpace = 0;
	if(nNum <= pProto->n16MaxLapNum)
	{
		nUseSpace = 1;
	}
	else
	{
		nUseSpace = (1 == pProto->n16MaxLapNum) ? nNum : ((nNum - 1) / pProto->n16MaxLapNum + 1);
	}

	// 看是背包还是任务栏物品
	if(MIsQuestItem(pProto))
	{
		n16UseQuestSpace = nUseSpace;
	}
	else
	{
		n16UseBagSpace = nUseSpace;
	}

	n16MaxLap = pProto->n16MaxLapNum;

	return TRUE;
}

//-----------------------------------------------------------------------------
// 是否是任务物品
//-----------------------------------------------------------------------------
BOOL ItemMgr::IsQuestItem(DWORD dwTypeID)
{
	const tagItemProto *pProto;
	if(!MIsEquipment(dwTypeID))
	{
		pProto = g_attRes.GetItemProto(dwTypeID);
	}
	else
	{
		pProto = g_attRes.GetEquipProto(dwTypeID);
	}

	if(!P_VALID(pProto))
	{
		ASSERT(0);
		IMSG(_T("Cannot find item/equip proto<id: %ld>!"), dwTypeID);
		return FALSE;
	}

	return MIsQuestItem(pProto);
}

//-----------------------------------------------------------------------------
// 从装备栏上直接删除一件装备
//-----------------------------------------------------------------------------
tagItem* ItemMgr::RemoveFromEquipBar(INT64 n64Serial, DWORD dwCmdID, BOOL bDelMem)
{
	tagItem *pItem = GetEquipBar().GetItem(n64Serial);
	if(!P_VALID(pItem))
	{
		ASSERT(P_VALID(pItem));
		IMSG(_T("Can not find item<serial: %lld> in container<Type: %d, RoleID: %u>!!!\r\n"),
			n64Serial, GetEquipBar().GetConType(), m_pRole->GetID());
		return NULL;
	}

	INT16 n16OldIndex = pItem->n16Index;

	// 脱下装备
	tagEquip *pEquipSrc = GetEquipBar().Remove(n64Serial);
	if(NULL == pEquipSrc)
	{
		ASSERT(pEquipSrc != NULL);
		return NULL;
	}

	// 向客户端发消息
	SendDelItem2Client(GetEquipBar().GetConType(), n16OldIndex, n64Serial, 0);

	// 计算影响
	m_pRole->ProcEquipEffect(NULL, (tagEquip*)pItem, n16OldIndex, TRUE);

	// 从数据库中删除
	DeleteItemFromDB(n64Serial, pItem->dwTypeID);

	// 检查是否需要记录log -- 即使此时物品要从游戏世界中删除,此时内存也没有释放
	LogItem(*pItem, NULL, 1, dwCmdID);

	// 检查是否需释放该物品内存
	if(bDelMem)
	{
		Destroy(pItem);
		return NULL;
	}

	return pItem;
}

//-----------------------------------------------------------------------------
// 背包&角色仓库扩容
//-----------------------------------------------------------------------------
DWORD ItemMgr::ExtendBag( INT64 n64serial )
{
	ItemContainer &bag = GetBag();

	// 是否可扩
	if(bag.GetMaxSpaceSize() <= bag.GetCurSpaceSize())
	{
		return E_Con_Cannot_Extend;
	}

	// 检查物品
	tagItem* pItem = GetBagItem(n64serial);
	if (!P_VALID(pItem) || pItem->pProtoType->eSpecFunc != EISF_BagExtend)
	{
		return E_Item_NotExist;
	}

	// 扣除物品
	if (E_Success != ItemUsedFromBag(n64serial, 1, ELCLD_Bag_Extend))
	{
		return E_Item_UseErr;
	}

	// 扩充背包
	bag.IncreaseSize(SPACE_BAG_PER_EXTEND);

	return E_Success;
}

DWORD ItemMgr::ExtendRoleWare( INT64 n64Serial )
{
	ItemContainer &ware = GetRoleWare();

	// 是否可扩
	if(ware.GetMaxSpaceSize() <= ware.GetCurSpaceSize())
	{
		return E_Con_Cannot_Extend;
	}

	// 检查物品
	tagItem* pItem = GetBagItem(n64Serial);
	if (!P_VALID(pItem) || pItem->pProtoType->eSpecFunc != EISF_RoleWareExtend)
	{
		return E_Item_NotExist;
	}

	// 扣除物品
	if (E_Success != ItemUsedFromBag(n64Serial, 1, ELCLD_RoleWare_Extend))
	{
		return E_Item_UseErr;
	}

	// 扩充背包
	ware.IncreaseSize(SPACE_WARE_PER_EXTEND);

	return E_Success;
}

//-----------------------------------------------------------------------------
// 从背包中取出，并丢弃 -- 从db中清除，绑定物品内释放
//-----------------------------------------------------------------------------
DWORD ItemMgr::DiscardFromBag(INT64 n64Serial, DWORD dwCmdID, OUT tagItem *&pOut)
{
	pOut = NULL;

	// 行囊是否解锁
	if(!m_pRole->GetRoleStateEx().IsInState(ERSE_BagPsdPass))
	{
		return E_Con_PswNotPass;
	}

	tagItem *pItem = GetBag().GetItem(n64Serial);
	if(!P_VALID(pItem))
	{
		ASSERT(0);
		return GT_INVALID;
	}

	if(pItem->bLock)
	{
		return E_Equip_Lock;
	}

	if(pItem->IsBind())
	{
		return RemoveItem(GetBag(), n64Serial, dwCmdID, TRUE, TRUE, TRUE);
	}

	pOut = pItem;
	return RemoveItem(GetBag(), n64Serial, dwCmdID, TRUE, FALSE, TRUE);
}

//-----------------------------------------------------------------------------
// 从背包中掉落 -- 从db中清除，绑定或锁定物品内存释放
//-----------------------------------------------------------------------------
DWORD ItemMgr::LootFromBag(INT64 n64Serial, DWORD dwCmdID, OUT tagItem *&pOut)
{
	pOut = NULL;

	tagItem *pItem = GetBag().GetItem(n64Serial);
	if(!P_VALID(pItem))
	{
		ASSERT(0);
		return GT_INVALID;
	}

	// 装备有死亡守护特殊属性
	if(MIsEquipment(pItem->dwTypeID) 
		&& EESA_Guard_DeadPunish == ((tagEquip*)pItem)->equipSpec.bySpecAtt)
	{
		return E_Equip_Guard_DeadPunish;
	}

	if(pItem->IsBind() || pItem->bLock)
	{
		return RemoveItem(GetBag(), n64Serial, dwCmdID, TRUE, TRUE, FALSE);
	}

	pOut = pItem;
	return RemoveItem(GetBag(), n64Serial, dwCmdID, TRUE, FALSE, FALSE);
}

//-----------------------------------------------------------------------------
// 从背包中掉落 -- 清除可掉落物品并传出掉落物品信息
//-----------------------------------------------------------------------------
DWORD ItemMgr::LootFromBag(INT nLootNum, TList<tagItem*>& listItems, DWORD dwCmdID)
{
	listItems.Clear();
	if( nLootNum <= 0 )
	{
		return E_Success;
	}

	// 读取所有可掉落物品
	TList<tagItem*> listCanLoot;

	// 轮询背包
	tagItem* pItem = NULL;
	TMap<INT64, INT16>::TMapIterator iter = GetBag().Begin();

	while( GetBag().GetNextItem(iter, pItem) )
	{
		// 不可掉落的物品
		if( !pItem->pProtoType->bDeadLoot )
		{
			continue;
		}

		// 装备有死亡守护特殊属性
		if( MIsEquipment(pItem->dwTypeID) 
			&& EESA_Guard_DeadPunish == ((tagEquip*)pItem)->equipSpec.bySpecAtt)
		{
			continue;
		}

		// 加入列表
		listCanLoot.PushBack(pItem);
	}

	// 没有可掉落物品
	if( listCanLoot.Empty() )
	{
		return E_Success;
	}

	// 可掉落物品不足
	if( listCanLoot.Size() <= nLootNum )
	{
		// 一个一个的拿出检测
		tagItem* pItem = listCanLoot.PopFront();

		while( P_VALID(pItem) )
		{
			if( pItem->IsBind() || pItem->bLock )
			{
				RemoveItem(GetBag(), pItem->n64Serial, dwCmdID, TRUE, TRUE, FALSE);
			}
			else
			{
				RemoveItem(GetBag(), pItem->n64Serial, dwCmdID, TRUE, FALSE, FALSE);
				listItems.PushBack(pItem);
			}

			pItem = listCanLoot.PopFront();
		}
	}
	// 大于要掉落的物品个数，随机抽取
	else
	{
		// 最多取掉落的最大数量
		for(INT n = 0; n < nLootNum; ++n)
		{
			if( !listCanLoot.RandPeek(pItem, TRUE) )
			{
				continue;
			}

			if( pItem->IsBind() || pItem->bLock )
			{
				RemoveItem(GetBag(), pItem->n64Serial, dwCmdID, TRUE, TRUE, FALSE);
			}
			else
			{
				RemoveItem(GetBag(), pItem->n64Serial, dwCmdID, TRUE, FALSE, FALSE);
				listItems.PushBack(pItem);
			}
		}
	}

	return E_Success;
}

//-----------------------------------------------------------------------------
// 从装备栏中掉落 -- 从db中清除，绑定或锁定物品内存释放
//-----------------------------------------------------------------------------
DWORD ItemMgr::LootFromEquipBar(INT64 n64Serial, DWORD dwCmdID, OUT tagItem *&pOut)
{
	pOut = RemoveFromEquipBar(n64Serial, dwCmdID, FALSE);
	if(!P_VALID(pOut))
	{
		return GT_INVALID;
	}

	// 检查是否需要清除内存中数据
	if(pOut->IsBind() || pOut->bLock)
	{
		Destroy(pOut);
	}

	return E_Success;
}

//-----------------------------------------------------------------------------
// 从装备栏中掉落 -- 清除可掉落物品并传出掉落物品信息
//-----------------------------------------------------------------------------
DWORD ItemMgr::LootFromEquipBar(INT nLootNum, TList<tagEquip*>& listItems, DWORD dwCmdID)
{
	listItems.Clear();
	if( nLootNum <= 0 )
	{
		return E_Success;
	}

	// 读取所有可掉落物品
	TList<tagEquip*> listCanLoot;

	// 轮询装备栏
	tagEquip* pEquip = NULL;
	TMap<INT64, INT16>::TMapIterator iter = GetEquipBar().Begin();

	while( GetEquipBar().GetNextItem(iter, pEquip) )
	{
		// 不可掉落的物品
		if( !pEquip->pProtoType->bDeadLoot )
		{
			continue;
		}

		// 装备有死亡守护特殊属性
		if( MIsEquipment(pEquip->dwTypeID) 
			&& EESA_Guard_DeadPunish == ((tagEquip*)pEquip)->equipSpec.bySpecAtt)
		{
			continue;
		}

		// 加入列表
		listCanLoot.PushBack(pEquip);
	}

	// 没有可掉落物品
	if( listCanLoot.Empty() )
	{
		return E_Success;
	}

	// 可掉落物品不足
	if( listCanLoot.Size() <= nLootNum )
	{
		// 一个一个的拿出检测
		tagEquip* pEquip = listCanLoot.PopFront();

		while( P_VALID(pEquip) )
		{
			if( pEquip->IsBind() || pEquip->bLock )
			{
				//RemoveItem(GetEquipBar(), pEquip->n64Serial, dwCmdID, TRUE, TRUE, FALSE);
				RemoveFromEquipBar(pEquip->n64Serial, dwCmdID, TRUE);
			}
			else
			{
				//RemoveItem(GetEquipBar(), pEquip->n64Serial, dwCmdID, TRUE, FALSE, FALSE);
				RemoveFromEquipBar(pEquip->n64Serial, dwCmdID, FALSE);
				listItems.PushBack(pEquip);
			}

			pEquip = listCanLoot.PopFront();
		}
	}
	// 大于要掉落的物品个数，随机抽取
	else
	{
		// 最多取掉落的最大数量
		for(INT n = 0; n < nLootNum; ++n)
		{
			if( !listCanLoot.RandPeek(pEquip, TRUE) )
			{
				continue;
			}

			if( pEquip->IsBind() || pEquip->bLock )
			{
				//RemoveItem(GetEquipBar(), pEquip->n64Serial, dwCmdID, TRUE, TRUE, FALSE);
				RemoveFromEquipBar(pEquip->n64Serial, dwCmdID, TRUE);
			}
			else
			{
				//RemoveItem(GetEquipBar(), pEquip->n64Serial, dwCmdID, TRUE, FALSE, FALSE);
				RemoveFromEquipBar(pEquip->n64Serial, dwCmdID, FALSE);
				listItems.PushBack(pEquip);
			}
		}
	}

	return E_Success;
}

//-----------------------------------------------------------------------------
// 从指定列表中删除指定个数的物品
//-----------------------------------------------------------------------------
DWORD ItemMgr::DelBagSameItem(TList<tagItem*> &list, INT32 nNum, DWORD dwCmdID)
{
	ASSERT(list.Size() > 0 && nNum > 0);

	INT32 nNumLeft	= nNum;
	INT32 n16NumDel	= 0;
	tagItem *pItem;
	TList<tagItem*>::TListIterator iter = list.Begin();
	while(list.PeekNext(iter, pItem))
	{
		if(nNumLeft <= 0)
		{
			break;
		}

		if(nNumLeft >= pItem->n16Num)
		{
			n16NumDel = pItem->n16Num;
		}
		else
		{
			n16NumDel = (INT16)nNumLeft;
		}

		DelFromBag(pItem->n64Serial, dwCmdID, n16NumDel);
		nNumLeft -= n16NumDel;
	}

	return E_Success;
}

//-----------------------------------------------------------------------------
// 百宝袋相关
//-----------------------------------------------------------------------------
DWORD ItemMgr::Add2BaiBao(tagItem *&pItem, DWORD dwCmdID, 
						  BOOL bReadFromDB/* = FALSE*/, DWORD dwRoleIDRel/* = GT_INVALID*/)
{
	// 检查是否有获取时间
	if(0 == pItem->dw1stGainTime)
	{
		pItem->dw1stGainTime = g_world.GetWorldTime();
	}

	if(bReadFromDB || GetBaiBaoFreeSize() > 0)
	{
		// 存储到item表中
		return AddItem(GetBaiBaoBag(), pItem, dwCmdID, TRUE, FALSE, dwRoleIDRel, FALSE);
	}
	else
	{
		// 存储到item_baibao表中
		ItemMgr::InsertBaiBao2DB(pItem, m_pRole->GetID(), dwCmdID);

		Destroy(pItem);
	}

	return E_Success;
}

DWORD ItemMgr::ClearBaibao()
{
	return E_Success;
}

VOID ItemMgr::InsertBaiBao2DB(tagItem *pItem, DWORD dwRoleID, DWORD dwCmdID)
{
	tagRoleInfo* RoleInfo = g_roleMgr.GetRoleInfo(dwRoleID);
	if(!P_VALID(RoleInfo))	return;

	ASSERT(pItem != NULL);

	pItem->dwAccountID = RoleInfo->dwAccountID;
	pItem->dwOwnerID = dwRoleID;
	// 临时设置物品属性以保存到item_baibao表
	pItem->n16Index = 0;
	pItem->eConType = EICT_Baibao;

	if(MIsEquipment(pItem->dwTypeID))
	{
		tagNDBC_NewBaiBaoEquip send;
		memcpy(&send.equip, pItem, SIZE_EQUIP);
		g_dbSession.Send(&send, send.dwSize);
	}
	else
	{
		tagNDBC_NewBaiBaoItem send;
		memcpy(&send.item, pItem, SIZE_ITEM);
		g_dbSession.Send(&send, send.dwSize);
	}

	// log处理//??
}

DWORD ItemMgr::ProcBaiBaoRecord(DWORD dwTypeID, DWORD dwDstRoleID, DWORD dwSrcRoleID, INT16 n16Type, DWORD dwTime, LPCTSTR szLeaveWords)
{
	// 上层需保证物品及角色的安全性,这里再做下简单判断
	tagRoleInfo* RoleInfo = g_roleMgr.GetRoleInfo(dwDstRoleID);

	if(!P_VALID(RoleInfo))
		return GT_INVALID;

	INT nLen = _tcsnlen(szLeaveWords, Max_LeaveWord_Length);
	INT nSize = sizeof(tagBaiBaoRecord) + (nLen * sizeof(TCHAR));

	//向DB发送保存请求
	MCREATE_MSG(pSendDB, sizeof(tagNDBC_AddBaiBaoLog)+nSize-sizeof(tagBaiBaoRecord), NDBC_AddBaiBaoLog);
	pSendDB->dwAccountID = RoleInfo->dwAccountID;

	tagBaiBaoRecord* pRecord = &(pSendDB->LogInfo);

	pRecord->n16Size = nSize;
	pRecord->dwRoleID = dwSrcRoleID;
	pRecord->dwTypeID = dwTypeID;
	pRecord->n16Type = n16Type;

	if (dwTime == GT_INVALID)
	{
		pRecord->dwTime = g_world.GetWorldTime();
	}
	else
	{
		pRecord->dwTime = dwTime;
	}

	_tcsncpy(pRecord->szWords, szLeaveWords, nLen);

	g_dbSession.Send(pSendDB, pSendDB->dwSize);


	// 判断目的玩家是否在线
	Role* pRole = g_roleMgr.GetRolePtrByID(dwDstRoleID);
	if (P_VALID(pRole))
	{
		// 向客户端发送记录同步消息
		MCREATE_MSG(pSend, sizeof(tagNS_SingleBaiBaoRecord)-sizeof(tagBaiBaoRecord)+nSize, NS_SingleBaiBaoRecord);
		IFASTCODE->MemCpy(&(pSend->sRecord), pRecord, nSize);
		pRole->SendMessage(pSend, pSend->dwSize);
		MDEL_MSG(pSend);
	}

	MDEL_MSG(pSendDB);	

	return E_Success;
}

//-----------------------------------------------------------------------------
// 物品可拥有上限相关
//-----------------------------------------------------------------------------
BOOL ItemMgr::IsMaxHoldLimitItem( DWORD dwTypeID )
{
	if (MIsEquipment(dwTypeID))
	{
		tagEquipProto* pTempEquipProto = g_attRes.GetEquipProto(dwTypeID);
		if (P_VALID(pTempEquipProto))
		{
			return pTempEquipProto->n16MaxHoldNum != GT_INVALID;
		}
	}
	else
	{
		tagItemProto* pTempItemProto = g_attRes.GetItemProto(dwTypeID);
		if (P_VALID(pTempItemProto))
		{
			return pTempItemProto->n16MaxHoldNum != GT_INVALID;
		}
	}

	return FALSE;
}

DWORD ItemMgr::AddMaxHoldItem( DWORD dwTypeID, INT nNum )
{
	//if (!IsMaxHoldLimitItem(dwTypeID)) return E_Success;
	//上层保证该物品已通过验证
	m_mapMaxHold.ModifyValue(dwTypeID, nNum);

	return E_Success;
}

DWORD ItemMgr::AddMaxHoldItem( const tagItem& item )
{
	if (item.IsBind() && item.dwOwnerID != m_pRole->GetID()) return E_Success;

	m_mapMaxHold.ModifyValue(item.dwTypeID, item.n16Num);

	return E_Success;
}

VOID ItemMgr::RemoveMaxHoldItem( DWORD dwTypeID, INT nNum )
{
	if (!IsMaxHoldLimitItem(dwTypeID)) return;

	INT HoldNum = m_mapMaxHold.Peek(dwTypeID);

	if (HoldNum == (INT)GT_INVALID || HoldNum < nNum)
	{
		IMSG(_T("\nDec Holdnum <item:%d, hold num: %d, dec num: %d> failed!\n\n"),
			dwTypeID, HoldNum, nNum);
		ASSERT(0);
		return;
	}

	m_mapMaxHold.ModifyValue(dwTypeID, -nNum);
}

BOOL ItemMgr::CanAddMaxHoldItem( DWORD dwTypeID, INT nNum )
{
	if (!IsMaxHoldLimitItem(dwTypeID)) return TRUE;

	INT HoldNum = m_mapMaxHold.Peek(dwTypeID);
	if ( HoldNum == (INT)GT_INVALID)
	{
		HoldNum = 0;
	}

	const tagItemProto* pProto = NULL;

	if (MIsEquipment(dwTypeID))
	{
		pProto = g_attRes.GetEquipProto(dwTypeID);
	}
	else
	{
		pProto = g_attRes.GetItemProto(dwTypeID);
	}
	
	return P_VALID(pProto) ? nNum + HoldNum <= pProto->n16MaxHoldNum : FALSE;
}

BOOL ItemMgr::CanAddMaxHoldItem( const tagItem& item )
{
	if (!IsMaxHoldLimitItem(item.dwTypeID)) return TRUE;

	if (item.IsBind() && item.dwOwnerID != m_pRole->GetID()) return TRUE;

	INT HoldNum = m_mapMaxHold.Peek(item.dwTypeID);
	if ( HoldNum == (INT)GT_INVALID)
	{
		HoldNum = 0;
	}

	return item.n16Num + HoldNum <= item.pProtoType->n16MaxHoldNum;
}

//-----------------------------------------------------------------------------
// 武器崭新度处理
//-----------------------------------------------------------------------------
VOID ItemMgr::ProcEquipNewness()
{
	tagEquip *pEquip = GetEquipBarEquip((INT16)EEP_RightHand);
	if(!P_VALID(pEquip))
	{
		return;
	}

	INT nAttackTimes = 0;
	pEquip->IncAttackTimes();
	if(pEquip->IsNewessChange(nAttackTimes))
	{
		tagNS_RWNewessChange send;
		send.nAttackTimes = nAttackTimes;
		SendMessage(&send, send.dwSize);


		// 武器伤害影响
		m_pRole->ResetWeaponDmg(*pEquip);
	}
}

VOID ItemMgr::InsertBaiBao2DBEx( tagItem *pItem, DWORD dwAccountId, DWORD dwCmdID )
{
	ASSERT(pItem != NULL);

	pItem->dwAccountID = dwAccountId;
	pItem->dwOwnerID = GT_INVALID;
	// 临时设置物品属性以保存到item_baibao表
	pItem->n16Index = 0;
	pItem->eConType = EICT_Baibao;

	if(MIsEquipment(pItem->dwTypeID))
	{
		tagNDBC_NewBaiBaoEquip send;
		memcpy(&send.equip, pItem, SIZE_EQUIP);
		g_dbSession.Send(&send, send.dwSize);
	}
	else
	{
		tagNDBC_NewBaiBaoItem send;
		memcpy(&send.item, pItem, SIZE_ITEM);
		g_dbSession.Send(&send, send.dwSize);
	}
}

//Jason 2009-11-25
BOOL ItemMgr::AddEquipPotVal(INT64 n64Serial,INT val)
{
// 	tagEquip *pEquip = (tagEquip*)GetBagItem(n64Serial);
// 	if(!P_VALID(pEquip))
// 		return FALSE;
// 
// 	if(!MIsEquipment(pEquip->dwTypeID))
// 		return FALSE;
// 
// 	//计算成功率
// 	//计算完美率
// 
// 	pEquip->equipSpec.nPotVal += val;
// 	UpdateEquipSpec(*pEquip);
	return TRUE;
}

//-----------------------------------------------------------------------------
// 绑定物品
//-----------------------------------------------------------------------------
VOID ItemMgr::SendItemBind( INT64 n64Seria, DWORD dwContTypel )
{
	tagNS_ItemBind send;
	send.n64Serial = n64Seria;
	send.eConType = (EItemConType)dwContTypel;
	if (P_VALID(m_pRole) && P_VALID(m_pRole->GetSession()))
	{
		m_pRole->GetSession()->SendMessage(&send, send.dwSize);
	}
}
