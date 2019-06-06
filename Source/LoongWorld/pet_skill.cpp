//-------------------------------------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: pet_skill.cpp
// author: xlguo
// actor:
// data: 2009-07-13
// last:
// brief: 宠物技能
//-------------------------------------------------------------------------------------------------------
#include "StdAfx.h"
#include "pet_skill.h"

#include "../ServerDefine/pet_skill_define.h"
#include "../ServerDefine/log_cmdid_define.h"
#include "../ServerDefine/msg_pet.h"
#include "../WorldDefine/msg_pet_skill.h"
#include "../WorldDefine/msg_pet.h"
#include "../WorldDefine/msg_loot.h"
#include "../WorldDefine/shop_define.h"
#include "../WorldDefine/time.h"

#include "loot_mgr.h"
#include "role.h"
#include "pet.h"
#include "world.h"
#include "item_creator.h"
#include "pet_define.h"
#include "pet_soul.h"
#include "att_res.h"
#include "title_mgr.h"

PetSkill::PetSkill(DWORD dwSkillTypeID, PetSoul* pSoul, INT nPara1, INT nPara2) 
	:m_pProto(NULL), m_pSoul(pSoul), m_nWorkCountTick(nPara2)
{
	DWORD dwPara1 = GT_VALID(nPara1) ? (DWORD)nPara1 : 0;
	m_nCoolDownTick = CalcTimeDiff(dwPara1, g_world.GetWorldTime()) * TICK_PER_SECOND;
	if (m_nCoolDownTick <= 0)	m_nCoolDownTick = GT_INVALID;

	// get proto
	m_pProto = g_attRes.GetPetSkillProto(dwSkillTypeID);
	ASSERT(P_VALID(m_pProto));
}

PetSkill* PetSkill::CreatePetSkill( DWORD dwSkillTypeID, PetSoul* pSoul, INT nPara1, INT nPara2 )
{
	const tagPetSkillProto* pProto = g_attRes.GetPetSkillProto(dwSkillTypeID);
	if (!P_VALID(pProto))
	{
		ASSERT(0);
		return NULL;
	}

	PetSkill* pSkill = NULL;

	switch(pProto->eType)
	{
	case EPT_Gather:
		pSkill = new PetGatherSkill(dwSkillTypeID, pSoul, nPara1, nPara2);
		break;
	case EPT_PickUp:
		pSkill = new PetPickUpSkill(dwSkillTypeID, pSoul, nPara1, nPara2);
		break;
	case EPT_MedicineFeed:
		pSkill = new PetMedicineFeedSkill(dwSkillTypeID, pSoul, nPara1, nPara2);
		break;
	case EPT_Forage:
		break;
	case EPT_Experience:
		pSkill = new PetExperanceSkill(dwSkillTypeID, pSoul, nPara1, nPara2);
		break;
	case EPT_Deliver:
		break;
	case EPT_Sale:
		pSkill = new PetSaleSkill(dwSkillTypeID, pSoul, nPara1, nPara2);
		break;
	case EPT_StoreHouse:
		pSkill = new PetStoreHouseSkill(dwSkillTypeID, pSoul, nPara1, nPara2);
		break;
	case EPT_Strengthen:
		pSkill = new PetStrengthSkill(dwSkillTypeID, pSoul, nPara1, nPara2);
		break;
	case EPT_Buff:
		pSkill = new PetBuffSkill(dwSkillTypeID, pSoul, nPara1, nPara2);
		break;
	case EPT_WuXing:
		pSkill = new PetWuXingSkill(dwSkillTypeID, pSoul, nPara1, nPara2);
		break;
	case EPT_MountAdd:
		pSkill = new PetMountAddSkill(dwSkillTypeID, pSoul, nPara1, nPara2);
		break;
	default:
		break;
	}

	return pSkill;
}

Role* PetSkill::GetMaster() const
{
	return GetSoul()->GetMaster();
}

VOID PetSkill::FillClientInfo( tagPetSkillMsgInfo* pInfo )
{
	// tbc:
	pInfo->dwTypeID			= m_pProto->dwTypeID;
	pInfo->nCurrCoolDown	= m_nCoolDownTick;
	pInfo->nMaxCoolDown		= m_nWorkCountTick;
}

BOOL PetSkill::SaveToDB( tagDBPetSkill* pDBSkill )
{
	pDBSkill->dwTypeID = GetSkillTypeID();
	pDBSkill->nPara1 = IncreaseTime(g_world.GetWorldTime(), m_nCoolDownTick / TICK_PER_SECOND) ;
	pDBSkill->nPara2 = GT_INVALID;

	return TRUE;
}

VOID PetSkill::Update()
{
	// 更新冷却倒计时
	CoolDowning();

	if(WorkCounting())
	{
		SetWorking(FALSE);
		ActiveSkill *pActSkill = dynamic_cast<ActiveSkill *>(this);
		if (P_VALID(pActSkill))
		{
			pActSkill->PastHandleCmd();
		}
	}
}

PetSkill* PetSkill::CreateDBPetSkill( tagDBPetSkill* pDBPetSkill, PetSoul* pSoul )
{
	PetSkill* pSkill = CreatePetSkill(pDBPetSkill->dwTypeID, pSoul, pDBPetSkill->nPara1, pDBPetSkill->nPara2);
	return pSkill;
}

DWORD PetSkill::GetSkillTypeID() const
{
	return m_pProto->dwTypeID;
}

BYTE PetSkill::GetCastCondition() const
{
	return GetProto()->byCast_condition;
}

BOOL PetSkill::CanSetWorking( BOOL bWorking )
{
	return GetSoul()->CanSetWroking(bWorking, GetProto()->dwTypeID);
}

BOOL PetSkill::SetWorking( BOOL bWorking )
{
	return GetSoul()->SetWorking(bWorking, GetProto()->dwTypeID);
}

VOID PetSkill::SetWorkCounting( INT nWorkingTick /*= 0*/ )
{
	m_nWorkCountTick = nWorkingTick;
	if (m_nWorkCountTick < 0)
	{
		m_nWorkCountTick = GT_INVALID;
		SetWorking(FALSE);
	}
	else
	{
		SetWorking(TRUE);
	}

	SendTimerMsg(EPTT_WorkTime, m_nWorkCountTick);
}
VOID PetSkill::SetCoolDowning( INT nCoolDownTick /*= 0*/ )
{
	m_nCoolDownTick = nCoolDownTick;
	if (m_nCoolDownTick < 0)
	{
		m_nCoolDownTick = GT_INVALID;
	}

	SendTimerMsg(EPTT_CoolDown, m_nCoolDownTick);
}

BOOL PetSkill::WorkCounting()
{
	BOOL bTerm = FALSE;
	if (IsWorkCounting())	
	{
		--m_nWorkCountTick;
		if (!IsWorkCounting())
		{
			bTerm = TRUE;
		}
	}
	return bTerm;
}

VOID PetSkill::CoolDowning()
{
	if (IsCoolDowning())	
	{
		--m_nCoolDownTick;
	}	
		
}

VOID PetSkill::SendPetSkillMsg( PVOID pData, DWORD dwSize, DWORD dwErrCode )
{
	if (dwSize<0)	
	{
		return ;
	}
	else if (0 == dwSize)
	{
		tagNS_PetSkillUse send;
		send.dwErrCode	= dwErrCode;
		send.dwPetID	= GetSoul()->GetID();
		send.dwSkillID	= GetSkillTypeID();

		if (P_VALID(GetMaster()))
			GetMaster()->SendMessage(&send, send.dwSize);
	}
	else if (P_VALID(pData))
	{
		DWORD dwMsgSize = sizeof(tagNS_PetSkillUse) - 1 + dwSize;

		MCREATE_MSG(pSend, dwMsgSize, NS_PetSkillUse);

		// 准备发送的消息
		pSend->dwPetID		= GetSoul()->GetID();
		pSend->dwSkillID	= GetSkillTypeID();
		pSend->dwErrCode	= dwErrCode;
		IFASTCODE->MemCpy(pSend->byData, pData, dwSize);

		if (P_VALID(GetMaster()))
			GetMaster()->SendMessage(pSend, pSend->dwSize);

		MDEL_MSG(pSend);
	}
	else
	{
		ASSERT(0);
	}

}

VOID PetSkill::SendTimerMsg( DWORD dwPtt, INT nValue )
{
	tagNS_PetSkillTimer send;

	send.byType = (BYTE)dwPtt;
	send.nValue = nValue;
	send.dwPetID = GetSoul()->GetID();
	send.dwPetSkillID = GetSkillTypeID();

	if(P_VALID(GetMaster()))
		GetMaster()->SendMessage(&send,send.dwSize);
}

VOID PetSkill::BreakWork()
{
	if (CanSetWorking(FALSE))
	{
		SetWorkCounting(GT_INVALID);
	}
}


DWORD ActiveSkill::PreHandleCmd( PVOID pSkillData, DWORD dwSkillDataSize, PVOID pSendData, DWORD &dwSendSize )
{
	const tagPetSkillProto* pPetSkillProto = GetProto();
	if (!P_VALID(pPetSkillProto))
	{
		ASSERT(0);
		return E_Pets_PetSkill_Use_SkillProtoInvalid;
	}
	if (GetMaster()->IsDead())
	{
		return E_Pets_PetSkill_MasterDead;
	}
	if (pPetSkillProto->nSpirit_cost > GetSoul()->GetPetAtt().GetAttVal(EPA_Spirit))
	{
		return E_Pets_PetSkill_Use_SpiritNotEnough;
	}
	if (pPetSkillProto->nWuxing_cost > GetSoul()->GetPetAtt().GetAttVal(EPA_WuXingEnergy))
	{
		return E_Pets_PetSkill_Use_WuXingEnergyNotEnough;
	}
	if (IsCoolDowning())
	{
		return E_Pets_PetSkill_Use_CoolingDown;
	}
	if (IsWorkCounting())
	{
		return E_Pets_PetSkill_Use_WorkCounting;
	}

	INT nCoolDownAdd = 0;
	INT nWorkAdd = 0;

	SetSendSkillData(dwSendSize, pSendData);
	SetRecvSkillData(dwSkillDataSize, pSkillData);

	DWORD dwRtv = PreHandleCmdImpl(nWorkAdd);

	if (E_Pets_Success == dwRtv)
	{
		// 冷却时间
		SetWorkCounting(nWorkAdd + pPetSkillProto->nWorkTimeTick);
	}
	
	return dwRtv;
}

DWORD ActiveSkill::PastHandleCmd()
{
	INT nCoolDownAdd = 0;
	DWORD dwRtv = PastHandleCmdImpl(nCoolDownAdd);
	SetRecvSkillData(0, NULL);
	if (E_Pets_Success == dwRtv)
	{
		SetCoolDowning(GetProto()->nCooldownTick + nCoolDownAdd);

		FLOAT fRate = GetSoul()->GetPetAtt().GetAttVal(EPA_SpiritRate) / 10000.0f;

		// 消耗
		GetSoul()->GetPetAtt().ModAttVal(EPA_Spirit, -INT(GetProto()->nSpirit_cost * fRate + 0.5));

		// 获取
		GetSoul()->GetPetAtt().ModAttVal(EPA_WuXingEnergy, GetProto()->nWuxing_add);
	}

	return dwRtv;
}

VOID ActiveSkill::SetRecvSkillData( DWORD dwSize, PVOID pData )
{
	if(P_VALID(m_RecvSkillData.first))
	{
		g_MemPoolSafe.Free(m_RecvSkillData.second);
		m_RecvSkillData.first = 0;
	}
	if(P_VALID(pData))
	{
		m_RecvSkillData.second = g_MemPoolSafe.Alloc(dwSize);
		IFASTCODE->MemCpy(m_RecvSkillData.second, pData, dwSize);
		m_RecvSkillData.first = dwSize;
	}
}

VOID ActiveSkill::SetSendSkillData( DWORD& dwSize, PVOID pData )
{
	m_SendSkillData.first = &dwSize;
	m_SendSkillData.second = pData;
}

PVOID ActiveSkill::GetSendSkillData( DWORD* &dwSize )
{
	dwSize = m_SendSkillData.first;
	return m_RecvSkillData.second;
}

PVOID ActiveSkill::GetRecvSkillData( DWORD &dwSize )
{
	dwSize = m_RecvSkillData.first;
	return m_RecvSkillData.second;
}

BOOL PetStrengthSkill::Active()
{
	if(m_bInUsing)
		return FALSE;

	// 技能限制
	FLOAT fRate = GetSoul()->GetPetAtt().GetAttVal(EPA_StrengthEffect) / 10000.0f;

	// 计算加成
	Role* pMaster = GetMaster();
	if(!P_VALID(pMaster))
		return FALSE;
	const tagPetSkillProto* pSkillProto = GetProto();

	for (INT nIndex = 0; nIndex < MAX_PET_SKILL_MOD_ATT && GT_VALID(pSkillProto->AttIndexs[nIndex]); ++nIndex)
	{
		pMaster->ModAttModValue(pSkillProto->AttIndexs[nIndex], INT(pSkillProto->AttMods[nIndex] * fRate + 0.5));
	}
	pMaster->RecalAtt();

	m_bInUsing = TRUE;

	return TRUE;
}

BOOL PetStrengthSkill::DeActive()
{
	if(!m_bInUsing)
		return FALSE;

	// 技能限制
	FLOAT fRate = GetSoul()->GetPetAtt().GetAttVal(EPA_StrengthEffect) / 10000.0f;

	// 取消加成
	Role* pMaster = GetMaster();
	if(!P_VALID(pMaster))
		return FALSE;

	const tagPetSkillProto* pSkillProto = GetProto();

	for (INT nIndex = 0; nIndex < MAX_PET_SKILL_MOD_ATT && GT_VALID(pSkillProto->AttIndexs[nIndex]); ++nIndex)
	{
		pMaster->ModAttModValue(pSkillProto->AttIndexs[nIndex], -INT(pSkillProto->AttMods[nIndex] * fRate + 0.5));
	}
	pMaster->RecalAtt();

	m_bInUsing = FALSE;

	return TRUE;

}

DWORD PetMedicineFeedSkill::PreHandleCmdImpl( INT &nWorkingAdd )
{
	DWORD dwSize = 0;
	tagPetSupplyData* pMCmd = (tagPetSupplyData*)GetRecvSkillData(dwSize);

	MTRANS_ELSE_RET(pMaster, GetMaster(), Role, GT_INVALID);
	MTRANS_ELSE_RET(pItem, pMaster->GetItemMgr().GetBagItem(pMCmd->n64ItemID), tagItem, E_Pets_PetSkill_Use_ItemNotExist);

	// 等级限制
	if(pItem->pProtoType->byMinUseLevel > pMaster->GetLevel() 
		|| pItem->pProtoType->byMaxUseLevel < pMaster->GetLevel())
		return E_Pets_PetSkill_RoleLvlLimit;

	if (pMaster->GetItemMgr().IsItemCDTime(pItem->dwTypeID))
	{
		return E_Pets_PetSkill_MedicineCooldown;
	}

	return E_Pets_Success;
}

DWORD PetMedicineFeedSkill::PastHandleCmdImpl( INT &nCoolDownAdd )
{
	DWORD dwSize = 0;
	tagPetSupplyData* pMCmd = (tagPetSupplyData*)GetRecvSkillData(dwSize);

	MTRANS_ELSE_RET(pMaster, GetMaster(), Role, GT_INVALID);
	MTRANS_ELSE_RET(pItem, pMaster->GetItemMgr().GetBagItem(pMCmd->n64ItemID), tagItem, E_Pets_PetSkill_Use_ItemNotExist);
	MTRANS_ELSE_RET(pMap, pMaster->GetMap(), Map, E_Pets_UnkownErr);

	// 计算buff
	pMaster->OnActiveItemBuffTrigger(pItem, ETEE_Use);

	// 计算物品的脚本使用效果
	if(P_VALID(pItem->pScript) && P_VALID(pMap))
		pItem->pScript->UseItem(pMap, pItem->dwTypeID, pMaster->GetID());

	// 称号消息
	pMaster->GetTitleMgr()->SigEvent(ETE_USE_ITEM, pItem->dwTypeID, GT_INVALID);

	// 加入物品公共冷却时间
	pMaster->GetItemMgr().Add2CDTimeMap(pItem->dwTypeID);

	INT nSave = INT(GetSoul()->GetPetAtt().GetAttVal(EPA_MedicineSaving) / 100);

	if(!IUTIL->Probability(nSave))
	{
		// 处理物品消失
		pMaster->GetItemMgr().ItemUsedFromBag(pItem->GetKey(), 1, (DWORD)ELCLD_Item_Use);
	}

	return E_Pets_Success;
}

DWORD PetPickUpSkill::PreHandleCmdImpl( INT &nWorkingAdd )
{
	DWORD dwRecvSize = 0;
	tagPetPickUpData* pRecvData = (tagPetPickUpData*)GetRecvSkillData(dwRecvSize);
	DWORD* dwSendSize = NULL;
	INT64* pSendData = (INT64*)GetSendSkillData(dwSendSize);
	*dwSendSize	= 0;

	MTRANS_ELSE_RET(pMap, GetMaster()->GetMap(), Map, E_Loot_Map_Not_Found);
	MTRANS_ELSE_RET(pGroundItem, pMap->GetGroundItem(pRecvData->n64GroundSerial), tagGroundItem, E_Loot_Item_Not_Found);

	if(	E_Pets_Success != GetMaster()->CanPickUpItem(pRecvData->n64GroundSerial, FALSE))
	{
		*dwSendSize			= sizeof(INT64);
		*pSendData			= pRecvData->n64GroundSerial;

		return E_Pets_UnkownErr;
	}

	return E_Pets_Success;
}

DWORD PetPickUpSkill::PastHandleCmdImpl( INT &nCoolDownAdd )
{
	DWORD dwRecvSize = 0;
	tagPetPickUpData* pRecvData = (tagPetPickUpData*)GetRecvSkillData(dwRecvSize);

	MTRANS_ELSE_RET(pMap, GetMaster()->GetMap(), Map, E_Loot_Map_Not_Found);
	MTRANS_ELSE_RET(pGroundItem, pMap->GetGroundItem(pRecvData->n64GroundSerial), tagGroundItem, E_Loot_Item_Not_Found);

	//注意把掉落物品拾取到背包里,这里tagGroundItem中的pItem指针会被销毁,所以放进背包应该是最后
	//注意把掉落物品拾取到背包里,这里tagGroundItem中的pItem指针会被销毁,所以放进背包应该是最后
	if (E_Pets_Success != GetMaster()->PickUpItem(pRecvData->n64GroundSerial, FALSE))
	{
		return E_Pets_UnkownErr;
	}

	// 节省时间
	nCoolDownAdd = GetSoul()->GetPetAtt().GetAttVal(EPA_PickUpResume);

	return E_Pets_Success;
}


DWORD PetWuXingSkill::PreHandleCmdImpl( INT &nWorkingAdd )
{
	if (!P_VALID(GetProto()))
	{
		return GT_INVALID;
	}

	FLOAT fRate = GetSoul()->GetPetAtt().GetAttVal(EPA_WuXingConsume) / 10000.0f;

	INT nTalentCount	= GetSoul()->GetPetAtt().GetAttVal(EPA_TalentCount);
	INT nMaxTalentCount	= GetSoul()->GetPetAtt().GetAttVal(EPA_TalentCountMax);

	if (nTalentCount >= nMaxTalentCount)
	{
		return E_Pets_PetSkill_Use_MaxTalentCount;
	}

	// 判断角色背包是否已满，若无剩余空间，
	if (0 == GetSoul()->GetMaster()->GetItemMgr().GetBagFreeSize())
	{
		//则提示：“背包已满，灵兽无法施展五行凝结”；
		return E_Pets_PetSkill_Wuxing_BagFull;
	}

	//读灵兽五行表，根据灵兽品质决定随机道具；
	const tagPetWuXingProto* pWuXingProto = g_attRes.GetPetWuXingProto(GetSoul()->GetPetAtt().GetAttVal(EPA_Quality));
	if (!P_VALID(pWuXingProto))
	{
		ASSERT(0);
		return E_Pets_UnkownErr;
	}
	return E_Pets_Success;
}

DWORD PetWuXingSkill::PastHandleCmdImpl( INT &nCoolDownAdd )
{

	tagItem* pActItem = NULL;
	//读灵兽五行表，根据灵兽品质决定随机道具；
	const tagPetWuXingProto* pWuXingProto = g_attRes.GetPetWuXingProto(GetSoul()->GetPetAtt().GetAttVal(EPA_Quality));
	if (!P_VALID(pWuXingProto))
	{
		ASSERT(0);
		return E_Pets_UnkownErr;
	}

	for (INT i=0; i<MAX_WUXING_ITEM_NUM; ++i)
	{
		if(!IUTIL->Probability(pWuXingProto->nProb[i] / 100))
			continue;

		// 生成物品
		pActItem = ItemCreator::Create(EICM_PetSkill, GetSoul()->GetID(), pWuXingProto->dwItemTypeID[i], pWuXingProto->nNum[i]);
		if (P_VALID(pActItem))
			break;
	}

	//道具直接奖励到角色背包
	if(!P_VALID(pActItem))
		return E_Pets_PetSkill_Wuxing_ItemNotValid;
	DWORD dwRtv = GetMaster()->GetItemMgr().Add2Bag(pActItem, ELCLD_ACT_TREASURE, TRUE);
	if(E_Success == dwRtv)
	{
		// 提示：“您的灵兽施展五行之力，为您凝聚XXX（获得物品名称，绿色显示）！”
		tagPetWuXingItem wuxing;
		wuxing.dwItemTypeID	= pActItem->pProtoType->dwTypeID;
		wuxing.nNum			= pActItem->n16Num;

		SendPetSkillMsg(&wuxing, sizeof(wuxing), E_Pets_Success);

		// 消耗五行力
		FLOAT fRate = GetSoul()->GetPetAtt().GetAttVal(EPA_WuXingConsume) / 10000.0f;
		GetSoul()->GetPetAtt().ModAttVal(EPA_WuXingEnergy,	-INT(GetProto()->nWuxing_cost * fRate + 0.5));
	}
	else
	{
		::Destroy(pActItem);
	}

	return dwRtv;
}

BOOL PetMountAddSkill::Open()
{
	if (m_bInUsing)
	{
		return FALSE;
	}
	m_bInUsing = TRUE;

	const tagPetSkillProto* pSkillProto = GetProto();

	if (GT_VALID(pSkillProto->nPetAttIndex))
	{
		GetSoul()->GetPetAtt().ModAttVal(pSkillProto->nPetAttIndex, pSkillProto->nPetAttMod);
	}

	return TRUE;
}

BOOL PetMountAddSkill::Close()
{
	if (!m_bInUsing)
	{
		return FALSE;
	}
	m_bInUsing = FALSE;

	const tagPetSkillProto* pSkillProto = GetProto();

	if (GT_VALID(pSkillProto->nPetAttIndex))
	{
		GetSoul()->GetPetAtt().ModAttVal(pSkillProto->nPetAttIndex, -pSkillProto->nPetAttMod);
	}

	return TRUE;
}

DWORD PetBuffSkill::PreHandleCmdImpl( INT &nWorkingAdd )
{
	// 设置工作时间为0,马上执行PastHandleCmd
	nWorkingAdd = -GetProto()->nWorkTimeTick;

	return E_Success;
}

DWORD PetBuffSkill::PastHandleCmdImpl( INT &nCoolDownAdd )
{
	// 添加buff
	DWORD dwBuffID = Buff::GetIDFromTypeID(GetProto()->nBuffid);
	if (GetMaster()->IsHaveBuff(dwBuffID) && GetMaster()->RemoveBuff(dwBuffID, TRUE) == E_Success 
		||!GetMaster()->IsHaveBuff(dwBuffID))
	{
		GetMaster()->TryAddBuff(GetMaster(), g_attRes.GetBuffProto(GetProto()->nBuffid), NULL, NULL, NULL);
	}

	return E_Pets_Success;
}

DWORD PetGatherSkill::PreHandleCmdImpl(INT &nWorkingAdd )
{
	DWORD dwSize = 0;
	tagPetGatherDataNC* pData = (tagPetGatherDataNC*)GetRecvSkillData(dwSize);
	if (dwSize <= 0 || !P_VALID(pData) || P_VALID(pData) && dwSize != sizeof(*pData))
		return GT_INVALID;

	// 获取人物，资源
	MTRANS_ELSE_RET(pMaster, GetMaster(), Role, E_Pets_PetSkill_NoMaster);
	MTRANS_ELSE_RET(pBody, GetSoul()->GetBody(), Pet, E_Pets_PetSkill_NoBody);
	MTRANS_ELSE_RET(pMap, pBody->GetMap(), Map, E_Pets_PetSkill_NoMap);
	MTRANS_ELSE_RET(pRes, pMap->FindCreature(pData->dwResourceID), Creature, E_Pets_PetSkill_Gather_NoResource);

	// 判断采集技能
	Skill* pGatherSkill = NULL;
	if ( pRes->IsNatuRes() )
		pGatherSkill = pMaster->GetSkill(GATHER_SKILL_MINING);
	else if ( pRes->IsManRes() )
		pGatherSkill = pMaster->GetSkill(GATHER_SKILL_HARVEST);
	if ( !P_VALID(pGatherSkill) )
		return E_Pets_PetSkill_Gather_NoGatherSkill;

	// 检测30个格子范围内是否有可采集的资源点（角色学会对应采集技能），如果有，则就近采集，如果没有，则上方屏显系统提示：“附近没有您可以采集的资源点”；
// 	if (!pBody->IsInCombatDistance(*pRes, pGatherSkill->GetOPDist()))
// 		return GT_INVALID;	

	// 背包判断
	ItemMgr& itemMgr = pMaster->GetItemMgr();	
	if (itemMgr.GetBagFreeSize() <= 0)
		return E_Pets_PetSkill_Gather_NoBagSpace;

	// 瞬移到资源
	GetSoul()->GetBody()->Teleport(pRes);

	// 采集耗时
	nWorkingAdd = GetSoul()->GetPetAtt().GetAttVal(EPA_GatherConsume);

	return E_Pets_Success;
}

DWORD PetGatherSkill::PastHandleCmdImpl( INT &nCoolDownAdd )
{
	DWORD dwSize = 0;
	tagPetGatherDataNC* pData = (tagPetGatherDataNC*)GetRecvSkillData(dwSize);
	if (dwSize <= 0 || !P_VALID(pData) || P_VALID(pData) && dwSize != sizeof(*pData))
		return GT_INVALID;

	tagPetGatherDataNS::E_Result eResult = tagPetGatherDataNS::ER_Continue;

	// 获取人物，资源
	MTRANS_ELSE_RET(pMaster, GetMaster(), Role, E_Pets_PetSkill_NoMaster);
	MTRANS_ELSE_RET(pBody, GetSoul()->GetBody(), Pet, E_Pets_PetSkill_NoBody);
	MTRANS_ELSE_RET(pMap, pBody->GetMap(), Map, E_Pets_PetSkill_NoMap);
	MTRANS_ELSE_RET(pRes, pMap->FindCreature(pData->dwResourceID), Creature, E_Pets_PetSkill_Gather_NoResource);

	// 瞬移到资源
	GetSoul()->GetBody()->Teleport(GetMaster());

	// 背包判断
	ItemMgr& itemMgr = pMaster->GetItemMgr();	
	if (itemMgr.GetBagFreeSize() <= 0)
		return E_Pets_PetSkill_Gather_NoBagSpace;

	// 资源已被占有
	if( pRes->IsTagged() )
		return E_Pets_PetSkill_Gather_ResIsTaged;

	// 判断资源并获得技能
	Skill* pGatherSkill = NULL;
	if ( pRes->IsNatuRes() )
		pGatherSkill = pMaster->GetSkill(GATHER_SKILL_MINING);
	else if ( pRes->IsManRes() )
		pGatherSkill = pMaster->GetSkill(GATHER_SKILL_HARVEST);
	if ( !P_VALID(pGatherSkill) )
		return E_Pets_PetSkill_Gather_NoGatherSkill;

	// 以产出率掉落资源 	
	sLootMgr.ResourceLoot(pRes, pMaster);

	// 开采熟练度增加（包含升级技能）
	const tagCreatureProto* pCreatureProto = g_attRes.GetCreatureProto(pRes->GetTypeID());
	// 获得熟练度 = 资源点提供的熟练度×(1+玩家悟性/100)
	INT nExpAdd =  INT((FLOAT)pCreatureProto->nExpGive * (1.0f + (FLOAT)pMaster->GetAttValue(ERA_Savvy) / 100.0f));
	pMaster->ChangeSkillExp(pGatherSkill, nExpAdd);

	// 资源降血
	pRes->ChangeHP(-1);
	if (pRes->GetAttValue(ERA_HP) <= 0)
	{
		eResult = tagPetGatherDataNS::ER_Break;
	}

	// 发消息
	tagPetGatherDataNS data;
	data.dwItemTypeID = 0;
	data.dwResourceID = pData->dwResourceID;
	data.eRet = eResult;
	SendPetSkillMsg(&data, sizeof(data), E_Pets_Success);

	return E_Pets_Success;
}

DWORD PetSaleSkill::PreHandleCmdImpl( INT &nWorkingAdd )
{
	DWORD dwSkillDataSize = 0;
	tagPetSellData* pSkillData = (tagPetSellData*)GetRecvSkillData(dwSkillDataSize);

	MTRANS_ELSE_RET(pMaster, GetMaster(), Role, E_Pets_PetSkill_NoMaster);

	// 物品确认
	INT64 *itemIDs = (INT64*)pSkillData->byData;
	for(INT i=0; i<pSkillData->nNum; ++i)
	{
		MTRANS_ELSE_RET(pItem, pMaster->GetItemMgr().GetBagItem(itemIDs[i]), tagItem, E_Pets_PetSkill_Sale_NoBagItem);

		if (pMaster->GetItemMgr().IsItemCDTime(pItem->dwTypeID))
		{
			return E_Pets_PetSkill_Sale_BagItemCoolingDown;
		}
		// 是否可出售
		if(!pMaster->GetItemMgr().CanSell(*pItem))
		{
			return E_Pets_PetSkill_Sale_BagItemCantSale;
		}

	}

	// 贩卖耗时
	nWorkingAdd = GetSoul()->GetPetAtt().GetAttVal(EPA_SellConsume);

	return E_Pets_Success;
}

DWORD PetSaleSkill::PastHandleCmdImpl( INT &nCoolDownAdd )
{
	DWORD dwSkillDataSize = 0;
	tagPetSellData* pSkillData = (tagPetSellData*)GetRecvSkillData(dwSkillDataSize);

	MTRANS_ELSE_RET(pMaster, GetMaster(), Role, E_Pets_PetSkill_NoMaster);

	// 行囊是否解锁
	if(!GetMaster()->GetRoleStateEx().IsInState(ERSE_BagPsdPass))
	{
		return E_Pets_PetSkill_Sale_BagLock;
	}
	
	// 开卖
	INT64 *itemIDs = (INT64*)pSkillData->byData;
	for(INT i=0; i<pSkillData->nNum; ++i)
	{
		// 没有这个物品，卖下一个
		tagItem* pItem = GetMaster()->GetItemMgr().GetBagItem(itemIDs[i]);
		if (!P_VALID(pItem))
			continue;

		// 物品正在冷却，卖下一个
		if (pMaster->GetItemMgr().IsItemCDTime(pItem->dwTypeID))
			continue;

		// 物品不可出售，卖下一个
		if(!pMaster->GetItemMgr().CanSell(*pItem))
			continue;

		// 计算售价//??没有计算税收影响
		INT64 n64Price = pItem->pProtoType->nBasePrice * pItem->n16Num;
		if(MIsEquipment(pItem->dwTypeID))
		{
			FLOAT fFactor;
			MCalPriceFactor(fFactor, ((tagEquip*)pItem)->equipSpec.byQuality);
			n64Price = (INT64)(n64Price * fFactor);
			if(n64Price < 0)
			{
				ASSERT(0);
				n64Price = 0;
			}
		}

		// 从玩家身上删除物品
		DWORD dwErrorCode = GetMaster()->GetItemMgr().DelFromBag(itemIDs[i], ELCLD_PET_SKILL_SALE, GT_INVALID, TRUE);
		if(dwErrorCode != E_Success)
			continue;

		// 玩家获得金钱
		GetMaster()->GetCurMgr().IncBagSilver(n64Price, ELCLD_PET_SKILL_SALE);
	}

	return E_Pets_Success;
}


DWORD PetExperanceSkill::PreHandleCmdImpl( INT &nWorkingAdd )
{
	DWORD dwRecvSkillDataSize = 0;
	DWORD dwRecvSkillData = *((DWORD*)GetRecvSkillData(dwRecvSkillDataSize));

	MTRANS_ELSE_RET(pProductSkill, GetMaster()->GetSkill(dwRecvSkillData), Skill, E_Pets_PetSkill_Experance_NoProductSkill);	

	return E_Pets_Success;
}

DWORD PetExperanceSkill::PastHandleCmdImpl( INT &nCoolDownAdd )
{
	DWORD dwRecvSkillDataSize = 0;
	DWORD dwRecvSkillData = *((DWORD*)GetRecvSkillData(dwRecvSkillDataSize));

	// 找到技能
	MTRANS_ELSE_RET(pProductSkill, GetMaster()->GetSkill(dwRecvSkillData), Skill, E_Pets_PetSkill_Experance_NoProductSkill);	

	// 增加熟练度
	GetMaster()->ChangeSkillExp(pProductSkill, 60);

	// 计算冷却
	nCoolDownAdd = GetSoul()->GetPetAtt().GetAttVal(EPA_TrainResume);

	return E_Pets_Success;
}

DWORD PetStoreHouseSkill::PreHandleCmdImpl( INT &nWorkingAdd )
{
	DWORD dwSkillDataSize = 0;
	tagPetStoreData* pSkillData = (tagPetStoreData*)GetRecvSkillData(dwSkillDataSize);

	MTRANS_ELSE_RET(pMaster, GetMaster(), Role, GT_INVALID);

	// 物品确认
	INT64 *itemIDs = (INT64*)pSkillData->byData;
	for(INT i=0; i<pSkillData->nNum; ++i)
	{
		MTRANS_ELSE_RET(pItem, pMaster->GetItemMgr().GetBagItem(itemIDs[i]), tagItem, E_Pets_PetSkill_Store_NoBagItem);

		if (pMaster->GetItemMgr().IsItemCDTime(pItem->dwTypeID))
		{
			return E_Pets_PetSkill_Store_NoBagItemCoolingDown;
		}
	}

	// 贩卖耗时
	nWorkingAdd = GetSoul()->GetPetAtt().GetAttVal(EPA_StorageConsume);

	return E_Pets_Success;
}

DWORD PetStoreHouseSkill::PastHandleCmdImpl( INT &nCoolDownAdd )
{
	DWORD dwSkillDataSize = 0;
	tagPetStoreData* pSkillData = (tagPetStoreData*)GetRecvSkillData(dwSkillDataSize);

	// 行囊是否解锁
	if(!GetMaster()->GetRoleStateEx().IsInState(ERSE_BagPsdPass))
	{
		return E_Pets_PetSkill_Store_BagLock;
	}

	// 物品确认
	INT64 *itemIDs = (INT64*)pSkillData->byData;
	for(INT i=0; i<pSkillData->nNum; ++i)
	{
		MTRANS_ELSE_RET(pItem, GetMaster()->GetItemMgr().GetBagItem(itemIDs[i]), tagItem, E_Pets_PetSkill_Store_NoBagItem);

		GetMaster()->GetItemMgr().Move2Other(EICT_Bag, itemIDs[i], EICT_RoleWare, GT_INVALID, ELCLD_Item_Move2Other);
	}

	return E_Pets_Success;
}
