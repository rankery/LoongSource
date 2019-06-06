//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: role_exchange.cpp
// author: Sxg
// actor:
// data: 2008-09-01
// last:
// brief: 玩家间交易处理
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "role.h"
#include "map.h"
#include "../WorldDefine/msg_exchange.h"
#include "../WorldDefine/msg_item.h"
#include "../ServerDefine/log_cmdid_define.h"

//------------------------------------------------------------------------
// 请求交易
//------------------------------------------------------------------------
DWORD Role::ProcExchangeReq(OUT Role* &pTarget, DWORD dwTgtRoleID)
{
	// 是否自己和自己交易
	if(GetID() == dwTgtRoleID)
	{
		return GT_INVALID;
	}
	
	// 获取地图
	Map *pMap = GetMap();
	if(!P_VALID(pMap))
	{
		return GT_INVALID;
	}

	// 申请者是否可以发起交易
	if(!CanExchange())
	{
		return E_Exchange_SelfBusy;
	}

	// 被申请者检查
	pTarget = pMap->FindRole(dwTgtRoleID);
	if(!P_VALID(pTarget))
	{
		// 被申请者不在线,或与申请者不在同一张地图中
		return E_Exchange_NotInSame_Map;
	}

	// 被申请者是否可交易
	if(!pTarget->CanExchange())
	{
		return E_Exchange_RoleBusy;
	}

	// 交易距离
	if(!IsInDistance(*pTarget, MAX_EXCHANGE_DISTANCE))
	{
		return E_Exchange_OutOfRange;
	}

	// 设置申请者交易状态
	BeginExchange(dwTgtRoleID);

	// 设置被申请者状态
	pTarget->GetExchMgr().SetTgtRoleID(GetID());

	return E_Success;
}

//------------------------------------------------------------------------
// 玩家对请求交易的反馈
//------------------------------------------------------------------------
DWORD Role::ProcExchangeReqRes(OUT Role* &pApplicant, DWORD dwTgtRoleID, DWORD dwErrorCode)
{
	// 是否自己和自己交易
	if(GetID() == dwTgtRoleID)
	{
		return GT_INVALID;
	}

	// 判断交易对象是否正确
	if(GetExchMgr().GetTgtRoleID() != dwTgtRoleID)
	{
		return GT_INVALID;
	}

	//// 再次判断是否忙
	//if(!CanExchange())
	//{
	//	return E_Exchange_RoleBusy;
	//}

	// 获取地图
	Map *pMap = GetMap();
	if(!P_VALID(pMap))
	{
		return GT_INVALID;
	}

	// 交易申请者检查
	pApplicant = pMap->FindRole(dwTgtRoleID);
	if(!P_VALID(pApplicant))
	{
		// 申请者下线，或者更换地图
		return E_Exchange_NotInSame_Map;
	}

	// 申请者的交易目标是否变化
	ExchangeMgr &exchMgrApp = pApplicant->GetExchMgr();
	if(pApplicant->IsExchanging() && exchMgrApp.GetTgtRoleID() != GetID())
	{
		return E_Exchange_ApplicantTgt_Change;
	}

	// 检查交易返回码
	if(dwErrorCode != E_Exchange_Accept)
	{
		// 结束交易发起者的交易状态
		pApplicant->EndExchange();
		return dwErrorCode;
	}

	// 申请者交易状态检查
	if(!pApplicant->IsExchanging())
	{
		exchMgrApp.SetTgtRoleID(GT_INVALID);
		return GT_INVALID;
	}

	// 交易距离
	if(!IsInDistance(*pApplicant, MAX_EXCHANGE_DISTANCE))
	{
		return E_Exchange_OutOfRange;
	}

	// 设置申请者交易状态
	BeginExchange(dwTgtRoleID);

	return E_Exchange_Accept;
}

//------------------------------------------------------------------------
// 添加交易物品
//------------------------------------------------------------------------
DWORD Role::ProcExchangeAdd(OUT Role* &pTarget, OUT tagItem* &pItem, INT32 &nInsIndex, INT64 n64Serial)
{
	DWORD dwErrorCode = ProcPrepareExchange(pTarget);
	if(dwErrorCode != E_Success)
	{
		return dwErrorCode;
	}

	// 取得交易物品
	pItem = GetItemMgr().GetBagItem(n64Serial);
	if(!P_VALID(pItem))
	{
		ASSERT(P_VALID(pItem));
		return GT_INVALID;
	}

	// 物品是否可别交易
	if(!GetItemMgr().CanExchange(*pItem))
	{	
		return E_Exchange_ItemCanNot_Exchange;
	}

	// 放入到待交易物品中
	nInsIndex = GetExchMgr().AddItem(n64Serial, pItem->n16Num);
	if(GT_INVALID == nInsIndex)
	{
		// 已在交易列表中，或交易物品个数已达到上限
		return GT_INVALID;
	}

	// 解除目标锁定状态
	pTarget->GetExchMgr().Unlock();

	return E_Success;
}

//------------------------------------------------------------------------
// 取消交易物品
//------------------------------------------------------------------------
DWORD Role::ProcExchangeDec(OUT Role* &pTarget, INT64 n64Serial)
{
	DWORD dwErrorCode = ProcPrepareExchange(pTarget);
	if(dwErrorCode != E_Success)
	{
		return dwErrorCode;
	}

	// 解除目标锁定状态
	pTarget->GetExchMgr().Unlock();
	
	return GetExchMgr().DecItem(n64Serial);
}

//------------------------------------------------------------------------
// 修改交易金钱
//------------------------------------------------------------------------
DWORD Role::ProcExchangeMoney(OUT Role* &pTarget, INT64 n64Silver)
{
	DWORD dwErrorCode = ProcPrepareExchange(pTarget);
	if(dwErrorCode != E_Success)
	{
		return dwErrorCode;
	}

	// 解除目标锁定状态
	pTarget->GetExchMgr().Unlock();

	// 背包中是否有足够的金钱
	CurrencyMgr &CurMgr = GetCurMgr();
	if(n64Silver > CurMgr.GetBagSilver())
	{
		return E_Exchange_NotEnough_Money;
	}

	GetExchMgr().ResetMoney(n64Silver);

	return E_Success;
}

//------------------------------------------------------------------------
// 锁定交易
//------------------------------------------------------------------------
DWORD Role::ProcExchangeLock(OUT Role* &pTarget)
{
	DWORD dwErrorCode = ProcPrepareExchange(pTarget);
	if(dwErrorCode != E_Success)
	{
		return dwErrorCode;
	}

	GetExchMgr().Lock();

	return E_Success;
}

//------------------------------------------------------------------------
// 玩家取消交易
//------------------------------------------------------------------------
DWORD Role::ProcExchangeCancel(OUT Role* &pTarget)
{
	// 是否处于交易状态
	if(!IsExchanging())
	{
		return GT_INVALID;
	}

	// 获得交易管理器和交易对象id
	ExchangeMgr &exchMgr = GetExchMgr();
	DWORD dwTargetRoleID = exchMgr.GetTgtRoleID();
	
	// 取消交易
	EndExchange();

	//// 交易数据是否处于锁定状态
	//if(exchMgr.IsLock())
	//{
	//	return GT_INVALID;
	//}

	// 获取地图
	Map *pMap = GetMap();
	if(!P_VALID(pMap))
	{
		return GT_INVALID;
	}

	pTarget = pMap->FindRole(dwTargetRoleID);
	if(!P_VALID(pTarget))
	{
		// 被申请者不在线,或与申请者不在同一张地图中
		return E_Exchange_NotInSame_Map;
	}

	// 目标没有处于交易状态，或正在和别人交易
	if(!pTarget->IsExchanging() || pTarget->GetExchMgr().GetTgtRoleID() != GetID())
	{
		return E_Exchange_ApplicantTgt_Change;
	}

	pTarget->EndExchange();

	return E_Success;
}

//------------------------------------------------------------------------
// 确认交易
//------------------------------------------------------------------------
DWORD Role::ProcExchangeVerify(OUT Role* &pTarget, OUT DWORD &dwFailedRoleID)
{
	DWORD dwErrorCode = E_Success;
	
	// 是否处于交易状态
	if(!IsExchanging())
	{
		return GT_INVALID;
	}

	// 获得交易管理器
	ExchangeMgr &exchMgr = GetExchMgr();

	// 交易数据是否处于锁定状态
	if(!exchMgr.IsLock())
	{
		return GT_INVALID;
	}

	// 获取地图
	Map *pMap = GetMap();
	if(!P_VALID(pMap))
	{
		return GT_INVALID;
	}

	pTarget = pMap->FindRole(exchMgr.GetTgtRoleID());
	if(!P_VALID(pTarget))
	{
		// 被申请者不在线,或与申请者不在同一张地图中
		dwFailedRoleID	= exchMgr.GetTgtRoleID();
		dwErrorCode		= E_Exchange_NotInSame_Map;
		goto Exit;
	}

	// 目标没有处于交易状态，或正在和别人交易
	if(!pTarget->IsExchanging() || pTarget->GetExchMgr().GetTgtRoleID() != GetID())
	{
		//return E_Exchange_ApplicantTgt_Change;
		return GT_INVALID;
	}

	// 目标是否处于锁定状态
	if(!pTarget->GetExchMgr().IsLock())
	{
		return GT_INVALID;
	}

	exchMgr.Verify();
	if(!pTarget->GetExchMgr().IsVerify())
	{
		return GT_INVALID;
	}

	// 交易距离确认
	if(!IsInDistance(*pTarget, MAX_EXCHANGE_DISTANCE))
	{
		dwErrorCode = E_Exchange_OutOfRange;
		goto Exit;
	}

	// 背包空间检查 -- 背包空间不足时，不清楚交易数据
	BYTE byExTypeNum	= exchMgr.GetItemTypeNum();
	BYTE byTgtExTypeNum	= pTarget->GetExchMgr().GetItemTypeNum();
	if(byExTypeNum > byTgtExTypeNum)
	{
		// 检查对方背包
		if(byExTypeNum - byTgtExTypeNum > pTarget->GetItemMgr().GetBagFreeSize())
		{
			// 解锁
			exchMgr.Unlock();
			pTarget->GetExchMgr().Unlock();

			dwFailedRoleID = pTarget->GetID();
			return E_Exchange_NotEnough_BagSpace;
		}
	}
	else
	{
		// 检查自己背包
		if(byTgtExTypeNum - byExTypeNum > GetItemMgr().GetBagFreeSize())
		{
			// 解锁
			exchMgr.Unlock();
			pTarget->GetExchMgr().Unlock();

			dwFailedRoleID = GetID();
			return E_Exchange_NotEnough_BagSpace;
		}
	}

	/*************************************************
	* 交易物品处理
	*************************************************/

	tagItem *pSrcItem[MAX_EXCHANGE_ITEM];
	tagItem *pDstItem[MAX_EXCHANGE_ITEM];
	ZeroMemory(pSrcItem, sizeof(tagItem*) * MAX_EXCHANGE_ITEM);
	ZeroMemory(pDstItem, sizeof(tagItem*) * MAX_EXCHANGE_ITEM);

	// 交易物品检查
	dwErrorCode = VerifyExchangeData(pSrcItem);
	if(dwErrorCode != E_Success)
	{
		dwFailedRoleID = GetID();
		goto Exit;
	}

	dwErrorCode = pTarget->VerifyExchangeData(pDstItem);
	if(dwErrorCode != E_Success)
	{
		dwFailedRoleID = pTarget->GetID();
		goto Exit;
	}

	// 4.移动物品

	// 4.1 金钱 -- 先减金钱,避免损失
	GetCurMgr().DecBagSilver(exchMgr.GetMoney(), ELCID_Exchange_Verify, pTarget->GetID());
	pTarget->GetCurMgr().DecBagSilver(pTarget->GetExchMgr().GetMoney(), ELCID_Exchange_Verify, GetID());

	GetCurMgr().IncBagSilver(
		min(GetCurMgr().GetCanIncBagSilver(), pTarget->GetExchMgr().GetMoney()), 
		ELCID_Exchange_Verify, pTarget->GetID());

	pTarget->GetCurMgr().IncBagSilver(
		min(pTarget->GetCurMgr().GetCanIncBagSilver(), exchMgr.GetMoney()), 
		ELCID_Exchange_Verify, GetID());

	// 4.2 物品

	// 4.2.1 先从玩家身上删除交易物品 -- 避免可堆叠物品处理出错
	GetItemMgr().RemoveFromBag(exchMgr.GetSerialArray(), MAX_EXCHANGE_ITEM, ELCID_Exchange_Verify, pTarget->GetID());
	pTarget->GetItemMgr().RemoveFromBag(pTarget->GetExchMgr().GetSerialArray(), 
										MAX_EXCHANGE_ITEM, ELCID_Exchange_Verify, GetID());

	// 4.2.2 将交易物品放到玩家身上
	GetItemMgr().Add2Bag(pDstItem, MAX_EXCHANGE_ITEM, ELCID_Exchange_Verify, pTarget->GetID());
	pTarget->GetItemMgr().Add2Bag(pSrcItem, MAX_EXCHANGE_ITEM, ELCID_Exchange_Verify, GetID());

Exit:
	// 释放交易资源
	EndExchange();

	if(P_VALID(pTarget))
	{
		pTarget->EndExchange();
	}
	else
	{
		// 如果交易目标在其他地图，该处也不可直接操作(多线程导致)
	}

	return dwErrorCode;
}

//------------------------------------------------------------------------
// 检查玩家身上的交易物品是否齐全
//------------------------------------------------------------------------
DWORD Role::VerifyExchangeData(OUT tagItem* pItem[])
{
	// 获得交易管理器
	ExchangeMgr &exchMgr = GetExchMgr();
	
	// 1.金钱是否还够
	if(GetCurMgr().GetBagSilver() < exchMgr.GetMoney())
	{
		return E_Exchange_NotEnough_Money;
	}

	//// 2.身上是否有足够的空间
	//if(GetItemMgr().GetBagFreeSize() < exchMgr.GetItemTypeNum())
	//{
	//	return E_Exchange_NotEnough_BagSpace;
	//}

	// 3.待交易物品是否还在身上, 若在，得到物品指针
	if(!GetItemMgr().CheckExistInBag(pItem, exchMgr.GetSerialArray(), exchMgr.GetNumArray(), MAX_EXCHANGE_ITEM))
	{
		return E_Exchange_ItemCanNot_Find;
	}

	return E_Success;
}

//------------------------------------------------------------------------
// 修改交易数据前的检查，及得到相关对象
//------------------------------------------------------------------------
DWORD Role::ProcPrepareExchange(OUT Role* &pTarget)
{
	// 是否处于交易状态
	if(!IsExchanging())
	{
		return GT_INVALID;
	}

	// 获得交易管理器
	ExchangeMgr &exchMgr = GetExchMgr();

	// 交易数据是否处于锁定状态
	if(exchMgr.IsLock())
	{
		return GT_INVALID;
	}

	// 获取地图
	Map *pMap = GetMap();
	if(!P_VALID(pMap))
	{
		return GT_INVALID;
	}

	pTarget = pMap->FindRole(exchMgr.GetTgtRoleID());
	if(!P_VALID(pTarget))
	{
		// 被申请者不在线,或与申请者不在同一张地图中
		return E_Exchange_NotInSame_Map;
	}

	// 目标没有处于交易状态，或正在和别人交易
	if(!pTarget->IsExchanging() || pTarget->GetExchMgr().GetTgtRoleID() != GetID())
	{
		return E_Exchange_ApplicantTgt_Change;
	}

	return E_Success;
}