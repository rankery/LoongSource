#include "StdAfx.h"
#include "role.h"
#include "map.h"
#include "../WorldDefine/msg_pet_exchange.h"
#include "../ServerDefine/log_cmdid_define.h"
#include "../WorldDefine/msg_pet.h"
#include "pet_pocket.h"
#include "pet_soul.h"
#include "pet_define.h"
//------------------------------------------------------------------------
// 请求交易
//------------------------------------------------------------------------
DWORD Role::ProcPetExchangeReq(OUT Role* &pTarget, DWORD dwTgtRoleID)
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
	if(!CanPetExchange())
	{
		return E_Pet_Exchange_SelfBusy;
	}

	// 被申请者检查
	pTarget = pMap->FindRole(dwTgtRoleID);
	if(!P_VALID(pTarget))
	{
		// 被申请者不在线,或与申请者不在同一张地图中
		return E_Pet_Exchange_NotInSame_Map;
	}

	// 被申请者是否可交易
	if(!pTarget->CanPetExchange())
	{
		return E_Pet_Exchange_RoleBusy;
	}

	// 交易距离
	if(!IsInDistance(*pTarget, MAX_EXCHANGE_DISTANCE))
	{
		return E_Pet_Exchange_OutOfRange;
	}

	// 设置申请者交易状态
	BeginPetExchange(dwTgtRoleID);

	// 设置被申请者状态
	pTarget->GetPetExchMgr().SetTgtRoleID(GetID());

	return E_Pets_Success;
}

//------------------------------------------------------------------------
// 玩家对请求交易的反馈
//------------------------------------------------------------------------
DWORD Role::ProcPetExchangeReqRes(OUT Role* &pApplicant, DWORD dwTgtRoleID, DWORD dwErrorCode)
{
	// 是否自己和自己交易
	if(GetID() == dwTgtRoleID)
	{
		return GT_INVALID;
	}

	// 判断交易对象是否正确
	if(GetPetExchMgr().GetTgtRoleID() != dwTgtRoleID)
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
		return E_Pet_Exchange_NotInSame_Map;
	}

	// 申请者的交易目标是否变化
	PetExchangeMgr &exchMgrApp = pApplicant->GetPetExchMgr();
	if(pApplicant->IsPetExchanging() && exchMgrApp.GetTgtRoleID() != GetID())
	{
		return E_Pet_Exchange_ApplicantTgt_Change;
	}

	// 检查交易返回码
	if(dwErrorCode != E_Pet_Exchange_Accept)
	{
		// 结束交易发起者的交易状态
		pApplicant->EndPetExchange();
		return dwErrorCode;
	}

	// 申请者交易状态检查
	if(!pApplicant->IsPetExchanging())
	{
		exchMgrApp.SetTgtRoleID(GT_INVALID);
		return GT_INVALID;
	}

	// 交易距离
	if(!IsInDistance(*pApplicant, MAX_EXCHANGE_DISTANCE))
	{
		return E_Pet_Exchange_OutOfRange;
	}

	// 设置申请者交易状态
	BeginPetExchange(dwTgtRoleID);

	return E_Pet_Exchange_Accept;
}

//------------------------------------------------------------------------
// 添加交易物品
//------------------------------------------------------------------------
DWORD Role::ProcPetExchangeAdd( OUT Role* &pTarget, DWORD dwPetID )
{
	DWORD dwErrorCode = ProcPreparePetExchange(pTarget);
	if(dwErrorCode != E_Success)
	{
		return dwErrorCode;
	}

	// 取得交易物品
	PetSoul* pSoul = GetPetPocket()->GetPetSoul(dwPetID);
	if(!P_VALID(pSoul))
	{
		ASSERT(P_VALID(pSoul));
		return GT_INVALID;
	}

	// 物品是否可别交易
	DWORD dwRtv = GetPetPocket()->CanExchange(dwPetID);
	if(dwRtv != E_Success)
	{
		return dwRtv;
	}

	if( pSoul->GetProto()->nRoleLvlLim > pTarget->GetLevel())
	{	
		return E_Pet_Exchange_RoleLvlNotEnough;
	}

	// 放入到待交易物品中
	dwErrorCode = GetPetExchMgr().AddPet(dwPetID);
	if(E_Success != dwErrorCode)
	{
		// 已在交易列表中，或交易物品个数已达到上限
		return GT_INVALID;
	}

	// 解除目标锁定状态
	pTarget->GetPetExchMgr().Unlock();

	return E_Success;
}

//------------------------------------------------------------------------
// 取消交易物品
//------------------------------------------------------------------------
DWORD Role::ProcPetExchangeDec( OUT Role* &pTarget, DWORD dwPetID )
{
	DWORD dwErrorCode = ProcPreparePetExchange(pTarget);
	if(dwErrorCode != E_Success)
	{
		return dwErrorCode;
	}

	// 解除目标锁定状态
	pTarget->GetPetExchMgr().Unlock();

	return GetPetExchMgr().DecPet(dwPetID);
}

//------------------------------------------------------------------------
// 修改交易金钱
//------------------------------------------------------------------------
DWORD Role::ProcPetExchangeMoney(OUT Role* &pTarget, INT64 n64Silver)
{
	DWORD dwErrorCode = ProcPreparePetExchange(pTarget);
	if(dwErrorCode != E_Success)
	{
		return dwErrorCode;
	}

	// 解除目标锁定状态
	pTarget->GetPetExchMgr().Unlock();

	// 背包中是否有足够的金钱
	CurrencyMgr &CurMgr = GetCurMgr();
	if(n64Silver > CurMgr.GetBagSilver())
	{
		return E_Pet_Exchange_NotEnough_Money;
	}

	GetPetExchMgr().ResetMoney(n64Silver);

	return E_Success;
}

//------------------------------------------------------------------------
// 锁定交易
//------------------------------------------------------------------------
DWORD Role::ProcPetExchangeLock(OUT Role* &pTarget)
{
	DWORD dwErrorCode = ProcPreparePetExchange(pTarget);
	if(dwErrorCode != E_Success)
	{
		return dwErrorCode;
	}

	GetPetExchMgr().Lock();

	return E_Success;
}

//------------------------------------------------------------------------
// 玩家取消交易
//------------------------------------------------------------------------
DWORD Role::ProcPetExchangeCancel(OUT Role* &pTarget)
{
	// 是否处于交易状态
	if(!IsPetExchanging())
	{
		return GT_INVALID;
	}

	// 获得交易管理器和交易对象id
	PetExchangeMgr &exchMgr = GetPetExchMgr();
	DWORD dwTargetRoleID = exchMgr.GetTgtRoleID();

	// 取消交易
	EndPetExchange();

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
		return E_Pet_Exchange_NotInSame_Map;
	}

	// 目标没有处于交易状态，或正在和别人交易
	if(!pTarget->IsPetExchanging() || pTarget->GetPetExchMgr().GetTgtRoleID() != GetID())
	{
		return E_Pet_Exchange_ApplicantTgt_Change;
	}

	pTarget->EndPetExchange();

	return E_Success;
}

//------------------------------------------------------------------------
// 确认交易
//------------------------------------------------------------------------
DWORD Role::ProcPetExchangeVerify(OUT Role* &pTarget, OUT DWORD &dwFailedRoleID)
{
	DWORD dwErrorCode = E_Success;

	// 是否处于交易状态
	if(!IsPetExchanging())
	{
		return GT_INVALID;
	}

	// 获得交易管理器
	PetExchangeMgr &exchMgr = GetPetExchMgr();

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
		dwErrorCode		= E_Pet_Exchange_NotInSame_Map;
		goto Exit;
	}

	// 目标没有处于交易状态，或正在和别人交易
	if(!pTarget->IsPetExchanging() || pTarget->GetPetExchMgr().GetTgtRoleID() != GetID())
	{
		//return E_Exchange_ApplicantTgt_Change;
		return GT_INVALID;
	}

	// 目标是否处于锁定状态
	if(!pTarget->GetPetExchMgr().IsLock())
	{
		return GT_INVALID;
	}

	exchMgr.Verify();
	if(!pTarget->GetPetExchMgr().IsVerify())
	{
		return GT_INVALID;
	}

	// 交易距离确认
	if(!IsInDistance(*pTarget, MAX_EXCHANGE_DISTANCE))
	{
		dwErrorCode = E_Pet_Exchange_OutOfRange;
		goto Exit;
	}

	// 背包空间检查 -- 背包空间不足时，不清楚交易数据
	BYTE byMyPetNum	= exchMgr.GetPetNum();
	BYTE byTgtPetNum	= pTarget->GetPetExchMgr().GetPetNum();
	if(byMyPetNum > byTgtPetNum)
	{
		// 检查对方背包
		if(byMyPetNum - byTgtPetNum > pTarget->GetPetPocket()->GetFreeSize())
		{
			// 解锁
			exchMgr.Unlock();
			pTarget->GetPetExchMgr().Unlock();

			dwFailedRoleID = pTarget->GetID();
			return E_Pet_Exchange_NotEnough_Space;
		}
	}
	else
	{
		// 检查自己背包
		if(byTgtPetNum - byMyPetNum >GetPetPocket()->GetFreeSize())
		{
			// 解锁
			exchMgr.Unlock();
			pTarget->GetPetExchMgr().Unlock();

			dwFailedRoleID = GetID();
			return E_Pet_Exchange_NotEnough_Space;
		}
	}

	/*************************************************
	* 交易物品处理
	*************************************************/

	PetSoul *pSrcItem[MAX_EXCHANGE_ITEM];
	PetSoul *pDstItem[MAX_EXCHANGE_ITEM];
	ZeroMemory(pSrcItem, sizeof(tagItem*) * MAX_EXCHANGE_ITEM);
	ZeroMemory(pDstItem, sizeof(tagItem*) * MAX_EXCHANGE_ITEM);

	// 交易物品检查
	dwErrorCode = VerifyPetExchangeData();
	if(dwErrorCode != E_Success)
	{
		dwFailedRoleID = GetID();
		goto Exit;
	}

	dwErrorCode = pTarget->VerifyPetExchangeData();
	if(dwErrorCode != E_Success)
	{
		dwFailedRoleID = pTarget->GetID();
		goto Exit;
	}
 
 	// 4.移动物品

	// 4.1 金钱 -- 先减金钱,避免损失
	GetCurMgr().DecBagSilver(exchMgr.GetMoney(), ELCID_Exchange_Verify, pTarget->GetID());
	pTarget->GetCurMgr().DecBagSilver(pTarget->GetPetExchMgr().GetMoney(), ELCID_Exchange_Verify, GetID());

	GetCurMgr().IncBagSilver(
		min(GetCurMgr().GetCanIncBagSilver(), pTarget->GetPetExchMgr().GetMoney()), 
		ELCID_Exchange_Verify, pTarget->GetID());

	pTarget->GetCurMgr().IncBagSilver(
		min(pTarget->GetCurMgr().GetCanIncBagSilver(), exchMgr.GetMoney()), 
		ELCID_Exchange_Verify, GetID());

	// 4.2 物品

	// 4.2.1 先从玩家身上删除交易物品 -- 避免可堆叠物品处理出错
	GetPetPocket()->TakeFromPocket(pSrcItem, MAX_EXCHANGE_ITEM, exchMgr.GetPetIDs(), MAX_EXCHANGE_ITEM);
	pTarget->GetPetPocket()->TakeFromPocket(pDstItem, MAX_EXCHANGE_ITEM, pTarget->GetPetExchMgr().GetPetIDs(), MAX_EXCHANGE_ITEM);

	// 4.2.2 将交易物品放到玩家身上
	GetPetPocket()->PutInPocket(pDstItem, MAX_EXCHANGE_ITEM);
	pTarget->GetPetPocket()->PutInPocket(pSrcItem, MAX_EXCHANGE_ITEM);

Exit:
	// 释放交易资源
	EndPetExchange();

	if(P_VALID(pTarget))
	{
		pTarget->EndPetExchange();
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
DWORD Role::VerifyPetExchangeData()
{
	// 获得交易管理器
	PetExchangeMgr &exchMgr = GetPetExchMgr();

	// 1.金钱是否还够
	if(GetCurMgr().GetBagSilver() < exchMgr.GetMoney())
	{
		return E_Pet_Exchange_NotEnough_Money;
	}

	//// 2.身上是否有足够的空间
	//if(GetItemMgr().GetBagFreeSize() < exchMgr.GetItemTypeNum())
	//{
	//	return E_Exchange_NotEnough_BagSpace;
	//}

	// 3.待交易物品是否还在身上, 若在，得到物品指针
	if(!GetPetPocket()->CheckExistInPocket(exchMgr.GetPetIDs(), MAX_EXCHANGE_ITEM))
	{
		return E_Pet_Exchange_PetCanNot_Find;
	}

	return E_Success;
}

//------------------------------------------------------------------------
// 修改交易数据前的检查，及得到相关对象
//------------------------------------------------------------------------
DWORD Role::ProcPreparePetExchange(OUT Role* &pTarget)
{
	// 是否处于交易状态
	if(!IsPetExchanging())
	{
		return GT_INVALID;
	}

	// 获得交易管理器
	PetExchangeMgr &exchMgr = GetPetExchMgr();

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
		return E_Pet_Exchange_NotInSame_Map;
	}

	// 目标没有处于交易状态，或正在和别人交易
	if(!pTarget->IsPetExchanging() || pTarget->GetPetExchMgr().GetTgtRoleID() != GetID())
	{
		return E_Pet_Exchange_ApplicantTgt_Change;
	}

	return E_Success;
}