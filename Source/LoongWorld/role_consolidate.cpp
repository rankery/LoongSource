//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: role_consolidate.cpp
// author: Wjiang
// actor:
// data: 
// last:
// brief: 装备强化
//-----------------------------------------------------------------------------
#include "StdAfx.h"

#include "../WorldDefine/msg_compose.h"
#include "../WorldDefine/specfunc_define.h"
#include "../WorldDefine/compose_define.h"
#include "../ServerDefine/consolidate_define.h"
#include "../ServerDefine/log_cmdid_define.h"
#include "../ServerDefine/item_define.h"

#include "role.h"
#include "map.h"
#include "creature.h"
#include "item_creator.h"
#include "title_mgr.h"
//-----------------------------------------------------------------------------
// 铭纹
// Jason 2009-12-1 增加紫色品质材料的加成因子
// Jason 2009-12-4 修改：只有指定类型的材料，才计入影响系数统计
//-----------------------------------------------------------------------------
DWORD Role::PosyEquip(DWORD dwNPCID, DWORD dwFormulaID, INT64 n64ItemID, INT64 n64IMID, INT64 n64StuffID[], INT32 nArraySz, DWORD dwCmdID)
{
	// 获得地图
	Map *pMap = GetMap();
	if(!P_VALID(pMap))
	{
		ASSERT(P_VALID(pMap));
		return GT_INVALID;
	}

	Creature* pNPC = pMap->FindCreature(dwNPCID);
	if( !P_VALID(pNPC) ) 
		return E_Compose_NPC_Not_Exist;

	if( FALSE == pNPC->CheckNPCTalkDistance(this) )
		return E_Compose_NPC_Distance;

	if( FALSE == pNPC->IsFunctionNPC(EFNPCT_Posy) )
		return E_Consolidate_NPCCanNotPosy;

	// 找到强化数据
	const tagPosyProtoSer *pPosyProto = g_attRes.GetPosyProto(dwFormulaID);
	if(!P_VALID(pPosyProto))
		return E_Compose_Formula_Not_Exist;

	// 找到被强化装备
	tagEquip *pEquip = (tagEquip*)GetItemMgr().GetBagItem(n64ItemID);
	if(!P_VALID(pEquip))
		return E_Consolidate_Equip_Not_Exist;

	if(!MIsEquipment(pEquip->dwTypeID))
		return E_Consolidate_NotEquipment;

	// 检测铭纹次数
	if(pEquip->equipSpec.byPosyTimes >= MAX_ENGRAVE_TIMES)
		return E_Consolidate_BeyondPosyTime;

	// 检测强化配方是否合法
	if((pEquip->equipSpec.byPosyTimes + 1 ) != pPosyProto->byPosyTimes)
		return E_Consolidate_FormulaNotMatch;

	// 检查是否能强化配方对应的装备属性
	if(!g_attRes.IsPosyPos(pPosyProto->ePosyAtt, pEquip->pEquipProto->eEquipPos))
		return E_Consolidate_EquipCanNotPosy;

	// 检查装备潜力值是否足够
	if(pEquip->equipSpec.nPotVal < (INT)pPosyProto->nPotValConsume)
		return E_Consolidate_ValConsume_Inadequate;

	// 检查玩家金钱是否足够
	if(GetCurMgr().GetBagSilver() < pPosyProto->dwMoneyConsume)
		return E_Consolidate_NotEnough_Money;

	// 检测玩家材料是否足够(顺便计算总材料数量）
	INT nItemQualityNum[EIQ_End];
	ZeroMemory(nItemQualityNum, sizeof(nItemQualityNum));

	INT nTotleStuff = 0;

	for(INT n = 0; n < MAX_CONSOLIDATE_STUFF_QUANTITY; ++n)
	{
		if(pPosyProto->ConsolidateStuff[n].dwStuffID == GT_INVALID 
			&& pPosyProto->ConsolidateStuff[n].eStuffType == EST_Null)
			break;

		tagItem *pItemStuff = GetItemMgr().GetBagItem(n64StuffID[n]); 

		if(!P_VALID(pItemStuff))
			return E_Consolidate_NotEnough_Stuff;

		// 检测材料64位ID是否重复
		for(INT i = 0; i < n; ++i)
		{
			if(n64StuffID[i] == n64StuffID[n])
				return E_Consolidate_NotEnough_Stuff;
		}

		if(pItemStuff->dwTypeID != pPosyProto->ConsolidateStuff[n].dwStuffID 
			&& pItemStuff->pProtoType->eStuffType != pPosyProto->ConsolidateStuff[n].eStuffType)
			return E_Consolidate_NotEnough_Stuff;

		if(pItemStuff->n16Num < (INT)pPosyProto->ConsolidateStuff[n].dwStuffNum)
			return E_Consolidate_NotEnough_Stuff;

		// Jason 必须是指定某种类型的材料，否则，不统计
		if( EST_Null != pPosyProto->ConsolidateStuff[n].eStuffType )
		{
			nItemQualityNum[pItemStuff->pProtoType->byQuality] += pPosyProto->ConsolidateStuff[n].dwStuffNum;
			nTotleStuff += pPosyProto->ConsolidateStuff[n].dwStuffNum;
		}
	}

	// 计算IM道具的影响
	tagIMEffect			IMEffect;
	tagPosyProtoSer *pProto = const_cast<tagPosyProtoSer*>(pPosyProto);		
	CalIMEffect(ECTS_Posy, IMEffect, n64IMID, pProto);

	// 输入材料消耗
	for(INT n = 0; n < nArraySz; ++n)
	{
		 GetItemMgr().DelFromBag(n64StuffID[n], (DWORD)ELCID_Compose_Posy, (INT16)pPosyProto->ConsolidateStuff[n].dwStuffNum);
	}

	// 金钱消耗
	GetCurMgr().DecBagSilver(pPosyProto->dwMoneyConsume, ELCID_Compose_Posy);

	// 计算成功率
	// 最终成功率=基础成功率+（-10%×白品材料数量/总材料数量+0%×黄品材料数量/总材料数量+5%×绿品材料数量
	// /总材料数量+10%×蓝品材料数量/总材料数量+20%×橙品材料数量/总材料数量）×[1+（装备等级-75）/150]
	// +（角色福缘/1000）+IM道具加成                                      最终成功率的取值为0%-100%
	FLOAT fProp = 0;
	if( nTotleStuff )
		fProp = (FLOAT)pPosyProto->nSuccessRate + (((-0.1f * (FLOAT)nItemQualityNum[EIQ_White] 
				+ 0.05f * (FLOAT)nItemQualityNum[EIQ_Green] + 0.1f * (FLOAT)nItemQualityNum[EIQ_Blue] 
				+ 0.2f * (FLOAT)nItemQualityNum[EIQ_Orange] + 0.5 * (FLOAT)nItemQualityNum[EIQ_Purple] ) / (FLOAT)nTotleStuff) 
				* (1.0f + ((FLOAT)pEquip->pProtoType->byLevel - 75.0f) / 150.0f) + ((FLOAT) GetAttValue(ERA_Fortune) / 1000.0f)) * 10000;

	// 计算B类属性对成功率影响
	fProp = CalSpecAttEffectSuc((EEquipSpecAtt)(pEquip->equipSpec.bySpecAtt), fProp, EESA_Guard_Posy);

	// 检测玩家是否用了提高成功率的IM
	if( IMEffect.eEffect ==  EIME_ComAdvance)
	{
		fProp += (FLOAT)IMEffect.dwParam2;
	}

	BOOL bResult = IUTIL->Rand() % 10000 <= (INT)fProp;

	if(bResult)			// 成功
	{
		// 称号事件触发
		GetTitleMgr()->SigEvent(ETE_STRENGTHEN_SKILL_SUCCESS, GT_INVALID, GT_INVALID);
		
		// 装备铭纹次数加一
		pEquip->equipSpec.byPosyTimes++;

		// 增加装备对应强化值
		// 加值=（装备等级×fcoefficientA + fcoefficientB）/ fcoefficientC ×（1+材料品质加成）
		/* 材料品质加成=（-20%×白品材料数量/总材料数量+0%×黄品材料数量/总材料数量+20%
		   ×绿品材料数量/总材料数量+50%×蓝品材料数量/总材料数量+100%×橙品材料数量/总材料数量）
		   材料品质加成的取值为0%-100%  */

		// 基本加成
		FLOAT fBaseAttInc = ((FLOAT)pEquip->pProtoType->byLevel * pPosyProto->fcoefficientA 
							+ pPosyProto->fcoefficientB) / pPosyProto->fcoefficientC;

		// 材料品质加成
		FLOAT fStuffAttInc = 0;
		if( nTotleStuff )
			fStuffAttInc = (-0.3f * (FLOAT)nItemQualityNum[EIQ_White] + 0.5f * (FLOAT)nItemQualityNum[EIQ_Green] 
							  + (FLOAT)nItemQualityNum[EIQ_Blue] + 2.0f * (FLOAT)nItemQualityNum[EIQ_Orange] 
							  + 3.0 * (FLOAT)nItemQualityNum[EIQ_Purple] )
							  / (FLOAT)nTotleStuff; 

		//if(fStuffAttInc < 0.0f)
		//	fStuffAttInc = 0.0f;
		//if(fStuffAttInc > 1.0f)
		//	fStuffAttInc = 1.0f;

		// 属性转换 
		ERoleAttribute eRoleAtt = ERA_Null;
		ConsolidatePosyAttConvert(eRoleAtt, pPosyProto->ePosyAtt);

		for(INT n = 0; n < MAX_ROLEATT_POSY_EFFECT; ++n)
		{
			if(pEquip->equipSpec.PosyEffect[n].eRoleAtt == eRoleAtt)
			{
				pEquip->equipSpec.PosyEffect[n].nValue += (INT32)(fBaseAttInc * (1.0f + fStuffAttInc));
				break;
			}
			else if(pEquip->equipSpec.PosyEffect[n].eRoleAtt == ERA_Null)
			{
				pEquip->equipSpec.PosyEffect[n].eRoleAtt = eRoleAtt;
				pEquip->equipSpec.PosyEffect[n].nValue += (INT32)(fBaseAttInc * (1.0f + fStuffAttInc));
				break;
			}
		}

		// 潜力值消耗
		// 生产完美率=生产成功率×1/20+角色福缘/1000
		FLOAT fPefectProp = (fProp * 0.05f) / 10000.0f + (FLOAT)GetAttValue(ERA_Fortune) / 1000.0f;

		// 计算B类属性对完美率影响
		fPefectProp = CalSpecAttEffectPef((EEquipSpecAtt)(pEquip->equipSpec.bySpecAtt), fPefectProp);

		// 计算装备光晕
		CalEquipFlare(pEquip);

		BOOL bPefect = IUTIL->Rand() % 10000 <= (fPefectProp * 10000);

		if(bPefect)
		{
			// 消耗装备潜力值为铭纹成功的75%
			pEquip->ChangePotVal(-pPosyProto->nPotValConsume * 75 / 100);
			GetItemMgr().UpdateEquipSpec(*pEquip);
			return E_Compose_Consolidate_Perfect;
		}
		else
		{
			pEquip->ChangePotVal(-pPosyProto->nPotValConsume);
			GetItemMgr().UpdateEquipSpec(*pEquip);
			return E_Compose_Consolidate_Success;
		}
	}
	else				// 失败
	{
		// 装备潜力值消耗5点
		if(IMEffect.eEffect !=  EIME_ProtectSign)
			pEquip->ChangePotVal(-20/*5*/); // Jason 2009-11-28

		GetItemMgr().UpdateEquipSpec(*pEquip);
		return E_Compose_Consolidate_Lose;
	}
}

//-----------------------------------------------------------------------------
// GM铭纹
//-----------------------------------------------------------------------------
DWORD Role::GMPosyEquip(DWORD dwFormulaID, INT16 n16ItemIndex)
{
	// 找到强化数据
	const tagPosyProtoSer *pPosyProto = g_attRes.GetPosyProto(dwFormulaID);
	if(!P_VALID(pPosyProto))
		return E_Compose_Formula_Not_Exist;

	// 找到被强化装备
	tagEquip *pEquip = (tagEquip*)GetItemMgr().GetBagItem(n16ItemIndex);
	if(!P_VALID(pEquip))
		return E_Consolidate_Equip_Not_Exist;

	if(!MIsEquipment(pEquip->dwTypeID))
		return E_Consolidate_NotEquipment;

	// 检测铭纹次数
	if(pEquip->equipSpec.byPosyTimes >= MAX_ENGRAVE_TIMES)
		return E_Consolidate_BeyondPosyTime;

	// 检测强化配方是否合法
	if((pEquip->equipSpec.byPosyTimes + 1 ) != pPosyProto->byPosyTimes)
		return E_Consolidate_FormulaNotMatch;

	// 检查是否能强化配方对应的装备属性
	if(!g_attRes.IsPosyPos(pPosyProto->ePosyAtt, pEquip->pEquipProto->eEquipPos))
		return E_Consolidate_EquipCanNotPosy;

	// 装备铭纹次数加一
	pEquip->equipSpec.byPosyTimes++;

	// 增加装备对应强化值
	// 加值=（装备等级×fcoefficientA + fcoefficientB）/ fcoefficientC ×（1+材料品质加成）

	// 基本加成
	FLOAT fBaseAttInc = ((FLOAT)pEquip->pProtoType->byLevel * pPosyProto->fcoefficientA 
		+ pPosyProto->fcoefficientB) / pPosyProto->fcoefficientC;

	// 属性转换 
	ERoleAttribute eRoleAtt = ERA_Null;
	ConsolidatePosyAttConvert(eRoleAtt, pPosyProto->ePosyAtt);

	for(INT n = 0; n < MAX_ROLEATT_POSY_EFFECT; ++n)
	{
		if(pEquip->equipSpec.PosyEffect[n].eRoleAtt == eRoleAtt)
		{
			pEquip->equipSpec.PosyEffect[n].nValue += (INT32)(fBaseAttInc * (1.0f + 0));
			break;
		}
		else if(pEquip->equipSpec.PosyEffect[n].eRoleAtt == ERA_Null)
		{
			pEquip->equipSpec.PosyEffect[n].eRoleAtt = eRoleAtt;
			pEquip->equipSpec.PosyEffect[n].nValue += (INT32)(fBaseAttInc * (1.0f + 0));
			break;
		}
	}

	// 计算装备光晕
	CalEquipFlare(pEquip);
		
	GetItemMgr().UpdateEquipSpec(*pEquip);
	return E_Compose_Consolidate_Success;
}

//-----------------------------------------------------------------------------
// 镌刻
// Jason 2009-12-1 增加紫色品级材料影响
// Jason 2009-12-4 只统计材料类型明确的材料
//-----------------------------------------------------------------------------
DWORD Role::EngraveEquip(DWORD dwNPCID, DWORD dwFormulaID, INT64 n64ItemID, INT64 n64IMID, INT64 n64StuffID[], INT32 nArraySz, DWORD dwCmdID)
{
	// 获得地图
	Map *pMap = GetMap();
	if(!P_VALID(pMap))
	{
		ASSERT(P_VALID(pMap));
		return GT_INVALID;
	}

	Creature* pNPC = pMap->FindCreature(dwNPCID);
	if( !P_VALID(pNPC) ) 
		return E_Compose_NPC_Not_Exist;

	if( FALSE == pNPC->CheckNPCTalkDistance(this) )
		return E_Compose_NPC_Distance;

	if( FALSE == pNPC->IsFunctionNPC(EFNPCT_Engrave) )
		return E_Compose_NPCCanNotEngrave;

	// 找到强化数据
	const tagEngraveProtoSer *pEngraveProto = g_attRes.GetEngraveProto(dwFormulaID);
	if(!P_VALID(pEngraveProto))
		return E_Compose_Formula_Not_Exist;

	// 找到被强化装备
	tagEquip *pEquip = (tagEquip*)GetItemMgr().GetBagItem(n64ItemID);
	if(!P_VALID(pEquip))
		return E_Consolidate_Equip_Not_Exist;

	if(!MIsEquipment(pEquip->dwTypeID))
		return E_Consolidate_NotEquipment;

	// 检测镌刻次数
	if(pEquip->equipSpec.byEngraveTimes >= MAX_ENGRAVE_TIMES)
		return E_Consolidate_BeyondEngraveTime;

	// 检测强化配方是否合法
	if((pEquip->equipSpec.byEngraveTimes + 1 ) != pEngraveProto->byEngraveTimes)
		return E_Consolidate_FormulaNotMatch;

	// 检查是否能强化配方对应的装备属性
	if(!g_attRes.IsEngravePos(pEngraveProto->eEngraveAtt, pEquip->pEquipProto->eEquipPos))
		return E_Consolidate_EquipCanNotEngrave;

	// 检查装备潜力值是否足够
	if(pEquip->equipSpec.nPotVal < (INT)pEngraveProto->nPotValConsume)
		return E_Consolidate_ValConsume_Inadequate;

	// 检查玩家金钱是否足够
	if(GetCurMgr().GetBagSilver() < pEngraveProto->dwMoneyConsume)
		return E_Consolidate_NotEnough_Money;

	// 检测玩家材料是否足够(顺便计算总材料数量）
	INT nItemQualityNum[EIQ_End];
	ZeroMemory(nItemQualityNum, sizeof(nItemQualityNum));

	INT nTotleStuff = 0;

	for(INT n = 0; n < MAX_CONSOLIDATE_STUFF_QUANTITY; ++n)
	{
		if(pEngraveProto->ConsolidateStuff[n].dwStuffID == GT_INVALID 
			&& pEngraveProto->ConsolidateStuff[n].eStuffType == EST_Null)
			break;

		tagItem *pItemStuff = GetItemMgr().GetBagItem(n64StuffID[n]); 

		if(!P_VALID(pItemStuff))
			return E_Consolidate_NotEnough_Stuff;

		// 检测材料64位ID是否重复
		for(INT i = 0; i < n; ++i)
		{
			if(n64StuffID[i] == n64StuffID[n])
				return E_Consolidate_NotEnough_Stuff;
		}

		if(pItemStuff->dwTypeID != pEngraveProto->ConsolidateStuff[n].dwStuffID 
			&& pItemStuff->pProtoType->eStuffType != pEngraveProto->ConsolidateStuff[n].eStuffType)
			return E_Consolidate_NotEnough_Stuff;

		if(pItemStuff->n16Num < (INT)pEngraveProto->ConsolidateStuff[n].dwStuffNum)
			return E_Consolidate_NotEnough_Stuff;

		if( EST_Null != pEngraveProto->ConsolidateStuff[n].eStuffType )
		{
			nItemQualityNum[pItemStuff->pProtoType->byQuality] += pEngraveProto->ConsolidateStuff[n].dwStuffNum;
			nTotleStuff += pEngraveProto->ConsolidateStuff[n].dwStuffNum;
		}
	}

	// 计算IM道具的影响
	tagIMEffect			IMEffect;
	tagEngraveProtoSer  *pProto = const_cast<tagEngraveProtoSer*>(pEngraveProto);
	CalIMEffect(ECTS_Engrave, IMEffect, n64IMID, pProto);

	// 输入材料消耗
	for(INT n = 0; n < nArraySz; ++n)
	{
		GetItemMgr().DelFromBag(n64StuffID[n], (DWORD)ELCID_Compose_Engrave, (INT16)pEngraveProto->ConsolidateStuff[n].dwStuffNum);
	}

	// 金钱消耗
	GetCurMgr().DecBagSilver(pEngraveProto->dwMoneyConsume, ELCID_Compose_Engrave);

	// 计算成功率
	// 最终成功率=基础成功率+（-10%×白品材料数量/总材料数量+0%×黄品材料数量/总材料数量+5%×绿品材料数量
	// /总材料数量+10%×蓝品材料数量/总材料数量+20%×橙品材料数量/总材料数量）×[1+（装备等级-75）/150]
	// +（角色福缘/1000）+IM道具加成                                      最终成功率的取值为0%-100%
	FLOAT fProp = 0;
	if( nTotleStuff )
		fProp = (FLOAT)pEngraveProto->nSuccessRate + (((-0.1f * (FLOAT)nItemQualityNum[EIQ_White] 
				+ 0.05f * (FLOAT)nItemQualityNum[EIQ_Green] + 0.1f * (FLOAT)nItemQualityNum[EIQ_Blue] 
				+ 0.2f * (FLOAT)nItemQualityNum[EIQ_Orange] + 0.5 * (FLOAT)nItemQualityNum[EIQ_Purple] ) / (FLOAT)nTotleStuff) 
				* (1.0f + ((FLOAT)pEquip->pProtoType->byLevel - 75.0f) / 150.0f) 
				+ ((FLOAT) GetAttValue(ERA_Fortune) / 1000.0f)) * 10000;

	// 计算B类属性对成功率影响
	fProp = CalSpecAttEffectSuc((EEquipSpecAtt)(pEquip->equipSpec.bySpecAtt), fProp, EESA_Guard_Engrave);

	// 检测玩家是否用了提高成功率的IM
	if( IMEffect.eEffect ==  EIME_ComAdvance)
	{
		fProp += (FLOAT)IMEffect.dwParam2;
	}

	BOOL bResult = IUTIL->Rand() % 10000 <= (INT)fProp;

	if(bResult)			// 成功
	{
		// 称号事件触发
		GetTitleMgr()->SigEvent(ETE_STRENGTHEN_SKILL_SUCCESS, GT_INVALID, GT_INVALID);

		// 装备铭纹次数加一
		pEquip->equipSpec.byEngraveTimes++;

		// 增加装备对应强化值
		// 加值=（装备等级×fcoefficientA + fcoefficientB）/ fcoefficientC ×（1+材料品质加成）
		/* 材料品质加成=（-20%×白品材料数量/总材料数量+0%×黄品材料数量/总材料数量+20%
		×绿品材料数量/总材料数量+50%×蓝品材料数量/总材料数量+100%×橙品材料数量/总材料数量）
		材料品质加成的取值为0%-50%  */

		// 基本加成
		FLOAT fBaseAttInc = ((FLOAT)pEquip->pProtoType->byLevel * pEngraveProto->fcoefficientA 
			+ pEngraveProto->fcoefficientB) / pEngraveProto->fcoefficientC;

		// 材料品质加成
		FLOAT fStuffAttInc = 0;
		if( nTotleStuff )
			fStuffAttInc = (-0.3f * (FLOAT)nItemQualityNum[EIQ_White] + 0.5f * (FLOAT)nItemQualityNum[EIQ_Green] 
							  + (FLOAT)nItemQualityNum[EIQ_Blue] + 2.0f * (FLOAT)nItemQualityNum[EIQ_Orange] 
							  + 3.0f * (FLOAT)nItemQualityNum[EIQ_Purple] )
							  / (FLOAT)nTotleStuff; 

		//if(fStuffAttInc < 0.0f)
		//	fStuffAttInc = 0.0f;
		//if(fStuffAttInc > 1.0f)
		//	fStuffAttInc = 1.0f;

		// 最终加成
		INT nEngraveAttInc = (INT)(fBaseAttInc * (1.0f + fStuffAttInc));

		switch(pEngraveProto->eEngraveAtt)
		{
		case EEngraveAtt_WeaponDmg:
			pEquip->equipSpec.nEngraveAtt[0] += nEngraveAttInc;
			pEquip->equipSpec.nEngraveAtt[1] += nEngraveAttInc;
			break;
		case EEngraveAtt_WeaponSoul:
			pEquip->equipSpec.nEngraveAtt[2] += nEngraveAttInc;
			break;
		case EEngraveAtt_Armor:
			pEquip->equipSpec.nEngraveAtt[3] += nEngraveAttInc;
			break;
		case EEngraveAtt_Deration:
			pEquip->equipSpec.nEngraveAtt[3] += nEngraveAttInc;
			break;
		default:
			break;
		}

		// 潜力值消耗
		// 生产完美率=生产成功率×1/20+角色福缘/1000
		FLOAT fPefectProp = (fProp * 0.05f) / 10000.0f + (FLOAT)GetAttValue(ERA_Fortune) / 1000.0f;

		// 计算B类属性对完美率影响
		fPefectProp = CalSpecAttEffectPef((EEquipSpecAtt)(pEquip->equipSpec.bySpecAtt), fPefectProp);

		// 计算装备光晕
		CalEquipFlare(pEquip);

		BOOL bPefect = IUTIL->Rand() % 10000 <= (fPefectProp * 10000);

		if(bPefect)
		{
			// 消耗装备潜力值为铭纹成功的75%
			pEquip->ChangePotVal(-pEngraveProto->nPotValConsume * 75 / 100);
			GetItemMgr().UpdateEquipSpec(*pEquip);
			return E_Compose_Consolidate_Perfect;
		}
		else
		{
			pEquip->ChangePotVal(-pEngraveProto->nPotValConsume);
			GetItemMgr().UpdateEquipSpec(*pEquip);
			return E_Compose_Consolidate_Success;
		}
	}
	else				// 失败
	{
		// 装备潜力值消耗5点
		if(IMEffect.eEffect !=  EIME_ProtectSign)
			pEquip->ChangePotVal(-20/*5*/); //Jason 2009-11-28

		GetItemMgr().UpdateEquipSpec(*pEquip);
		return E_Compose_Consolidate_Lose;
	}
}

//-----------------------------------------------------------------------------
// GM 镌刻
//-----------------------------------------------------------------------------
DWORD Role::GMEngraveEquip(DWORD dwFormulaID, INT16 n16ItemIndex)
{
	// 找到强化数据
	const tagEngraveProtoSer *pEngraveProto = g_attRes.GetEngraveProto(dwFormulaID);
	if(!P_VALID(pEngraveProto))
		return E_Compose_Formula_Not_Exist;

	// 找到被强化装备
	tagEquip *pEquip = (tagEquip*)GetItemMgr().GetBagItem(n16ItemIndex);
	if(!P_VALID(pEquip))
		return E_Consolidate_Equip_Not_Exist;

	if(!MIsEquipment(pEquip->dwTypeID))
		return E_Consolidate_NotEquipment;

	// 检测镌刻次数
	if(pEquip->equipSpec.byEngraveTimes >= MAX_ENGRAVE_TIMES)
		return E_Consolidate_BeyondEngraveTime;

	// 检测强化配方是否合法
	if((pEquip->equipSpec.byEngraveTimes + 1 ) != pEngraveProto->byEngraveTimes)
		return E_Consolidate_FormulaNotMatch;

	// 检查是否能强化配方对应的装备属性
	if(!g_attRes.IsEngravePos(pEngraveProto->eEngraveAtt, pEquip->pEquipProto->eEquipPos))
		return E_Consolidate_EquipCanNotEngrave;

	// 装备铭纹次数加一
	pEquip->equipSpec.byEngraveTimes++;

	// 增加装备对应强化值
	// 加值=（装备等级×fcoefficientA + fcoefficientB）/ fcoefficientC ×（1+材料品质加成）

	// 基本加成
	FLOAT fBaseAttInc = ((FLOAT)pEquip->pProtoType->byLevel * pEngraveProto->fcoefficientA 
		+ pEngraveProto->fcoefficientB) / pEngraveProto->fcoefficientC;

	// 最终加成
	INT nEngraveAttInc = (INT)(fBaseAttInc * (1.0f + 0));

	switch(pEngraveProto->eEngraveAtt)
	{
	case EEngraveAtt_WeaponDmg:
		pEquip->equipSpec.nEngraveAtt[0] += nEngraveAttInc;
		pEquip->equipSpec.nEngraveAtt[1] += nEngraveAttInc;
		break;
	case EEngraveAtt_WeaponSoul:
		pEquip->equipSpec.nEngraveAtt[2] += nEngraveAttInc;
		break;
	case EEngraveAtt_Armor:
		pEquip->equipSpec.nEngraveAtt[3] += nEngraveAttInc;
		break;
	case EEngraveAtt_Deration:
		pEquip->equipSpec.nEngraveAtt[3] += nEngraveAttInc;
		break;
	default:
		break;
	}
	
	// 计算装备光晕
	CalEquipFlare(pEquip);

	GetItemMgr().UpdateEquipSpec(*pEquip);
	return E_Compose_Consolidate_Success;
}

//-----------------------------------------------------------------------------
// 镶嵌
//-----------------------------------------------------------------------------
DWORD Role::InlayEquip(INT64 n64SrcItemID, INT64 n64DstItemID, DWORD dwCmdID)
{
	// 获得地图
	Map *pMap = GetMap();
	if(!P_VALID(pMap))
	{
		ASSERT(P_VALID(pMap));
		return GT_INVALID;
	}

	// 找到被强化装备
	tagEquip *pEquip = (tagEquip*)GetItemMgr().GetBagItem(n64DstItemID);
	if(!P_VALID(pEquip))
		return E_Consolidate_Equip_Not_Exist;

	if(!MIsEquipment(pEquip->dwTypeID))
		return E_Consolidate_NotEquipment;

	// 找到宝石
	tagItem *pItemGem = GetItemMgr().GetBagItem(n64SrcItemID);
	if(!P_VALID(pItemGem) || pItemGem->pProtoType->eSpecFunc != EISF_HoleGem )
	{
		return E_Consolidate_Gem_Not_Exit;
	}

	// 找到强化数据
	const tagConsolidateItem *pConsolidateProto = g_attRes.GetConsolidateProto(pItemGem->dwTypeID);
	if(!P_VALID(pConsolidateProto))
		return E_Compose_Formula_Not_Exist;

	if((INT)pEquip->equipSpec.byHoleNum == 0)
		return E_Consolidate_Gem_Not_Hole;

	// 检测是否宝石已经镶满
	if(pEquip->equipSpec.dwHoleGemID[pEquip->equipSpec.byHoleNum - 1] != GT_INVALID 
		&& pEquip->equipSpec.dwHoleGemID[pEquip->equipSpec.byHoleNum - 1] != 0 )
		return E_Consolidate_Gem_Full;

	// 检测宝石是否能镶嵌到装备上
	BOOL bConsolidate = FALSE;
	for(INT m = 0; m < MAX_CONSOLIDATE_POS_QUANTITY; ++m)
	{
		if(pEquip->pEquipProto->eEquipPos != pConsolidateProto->ConsolidatePos[m].ePos)
			continue;
		else
		{
			if( 1 == pConsolidateProto->ConsolidatePos[m].byConsolidate)
			{
				bConsolidate = TRUE;
				break;
			}
		}
	}

	if(!bConsolidate)
		return E_Consolidate_EquipCanNotInlay;

	// 检查装备潜力值是否足够
	if(pEquip->equipSpec.nPotVal < (INT)pConsolidateProto->dwPotValConsume)
		return E_Consolidate_ValConsume_Inadequate;

	// 镶嵌宝石
	for(INT i = 0; i < (INT)pEquip->equipSpec.byHoleNum; ++i)
	{
		if(pEquip->equipSpec.dwHoleGemID[i] == GT_INVALID || pEquip->equipSpec.dwHoleGemID[i] == 0)
		{
			pEquip->equipSpec.dwHoleGemID[i] = pItemGem->dwTypeID;
			break;
		}
	}

	// 计算装备光晕
	CalEquipFlare(pEquip);

	// 消耗装备潜力值
	pEquip->ChangePotVal(-(INT)pConsolidateProto->dwPotValConsume);
	GetItemMgr().UpdateEquipSpec(*pEquip);

	// 删除材料
	GetItemMgr().DelFromBag(n64SrcItemID, (DWORD)ELCID_Compose_Enchase, 1);

	// 称号事件触发
	GetTitleMgr()->SigEvent(ETE_STRENGTHEN_SKILL_SUCCESS, GT_INVALID, GT_INVALID);

	return E_Compose_Consolidate_Success;	
}

//-----------------------------------------------------------------------------
// 开凿
//-----------------------------------------------------------------------------
DWORD Role::ChiselEquip(INT64 n64SrcItemID, INT64 n64SuffID, DWORD dwCmdID)
{
	// 获得地图
	Map *pMap = GetMap();
	if(!P_VALID(pMap))
	{
		ASSERT(P_VALID(pMap));
		return GT_INVALID;
	}

	// 找到被强化装备
	tagEquip *pEquip = (tagEquip*)GetItemMgr().GetBagItem(n64SrcItemID);
	if(!P_VALID(pEquip))
		return E_Consolidate_Equip_Not_Exist;

	if(!MIsEquipment(pEquip->dwTypeID))
		return E_Consolidate_NotEquipment;

	if(!MIsIdentified(pEquip))
		return E_Consolidate_NotIdentified;

	if(pEquip->equipSpec.bCanCut == false)
		return E_Consolidate_Equip_CanNot_Chisel;

	// 找到凿石
	tagItem *pItemChisel = GetItemMgr().GetBagItem(n64SuffID);
	if(!P_VALID(pItemChisel) || pItemChisel->pProtoType->eSpecFunc != EISF_Chisel )
	{
		return E_Consolidate_Chisel_Not_Exit;
	}

	// 检测装备镶嵌空数
	if(pEquip->equipSpec.byHoleNum >= MAX_EQUIPHOLE_NUM)
		return E_Consolidate_Equip_Hole_Full;

	// 等级小于所开凿的装备等级，则不可进行开凿
	if(pEquip->pProtoType->byLevel > (pItemChisel->pProtoType->byLevel * 10))
		return E_Consolidate_Chisel_Not_Match;

	// 计算成功率(开凿最终成功率=凿石基础成功率×（1-装备当前镶嵌孔数量/5）+（角色福缘/1000）)
	FLOAT fProp = (((FLOAT)pItemChisel->pProtoType->nSpecFuncVal1 * ( 1.0f - (FLOAT)pEquip->equipSpec.byHoleNum / 5.0f)) / 10000.0f 
						+ (FLOAT)GetAttValue(ERA_Fortune) / 1000.0f) *10000.0f;

	// 材料消耗
	GetItemMgr().DelFromBag(n64SuffID, (DWORD)ELCID_Compose_Chisel, 1);

	// 计算B类属性对成功率影响
	fProp = CalSpecAttEffectSuc((EEquipSpecAtt)(pEquip->equipSpec.bySpecAtt), fProp, EESA_Guard_Cut);

	BOOL bResult = IUTIL->Rand() % 10000 <= (INT)fProp;

	if(bResult)		// 成功
	{
		// 计算完美率
		// 完美率=1%+（角色福缘/1000）【取值在0-100%】
		FLOAT fPefectProp = 0.01f + (FLOAT)GetAttValue(ERA_Fortune) / 1000.0f;

		// 计算B类属性对完美率影响
		fPefectProp = CalSpecAttEffectPef((EEquipSpecAtt)(pEquip->equipSpec.bySpecAtt), fPefectProp);

		BOOL bPefect = IUTIL->Rand() % 10000 <= (fPefectProp * 10000);

		if(bPefect)
		{
			pEquip->equipSpec.byHoleNum = ((pEquip->equipSpec.byHoleNum + 2 > MAX_EQUIPHOLE_NUM) ? MAX_EQUIPHOLE_NUM : (pEquip->equipSpec.byHoleNum + 2));
			GetItemMgr().UpdateEquipSpec(*pEquip);
			return  E_Compose_Consolidate_Perfect;
		}
		else
		{
			++(pEquip->equipSpec.byHoleNum);
			GetItemMgr().UpdateEquipSpec(*pEquip);
			return E_Compose_Consolidate_Success;
		}
	}

	return E_Compose_Consolidate_Lose;
}

//-----------------------------------------------------------------------------
// 时装染色
//-----------------------------------------------------------------------------
DWORD Role::DyeFashion(INT64 n64DyeSN, INT64 n64EquipSerial, DWORD dwCmdID)
{
	// 获得地图
	Map *pMap = GetMap();
	if(!P_VALID(pMap))
	{
		ASSERT(P_VALID(pMap));
		return GT_INVALID;
	}

	// 找到被染色时装
	tagEquip *pEquip = (tagEquip*)GetItemMgr().GetBagItem(n64EquipSerial);
	if(!P_VALID(pEquip))
		return E_Dye_NotExist;

	if(!MIsEquipment(pEquip->dwTypeID))
		return E_Dye_OtherItem;

	if(!MIsFashion(pEquip))
		return E_Dye_OtherItem;

	if (pEquip->pEquipProto->bCanDye == false)
		return E_Dye_OtherItem;

	// 找到染料
	tagItem *pItemDye = GetItemMgr().GetBagItem(n64DyeSN);
	if(!P_VALID(pItemDye) || pItemDye->pProtoType->eSpecFunc != EISF_Dye )
	{
		return E_Dye_NotExist;
	}

	// 检测颜色是否相同
	if(pEquip->equipSpec.n8ColorID == pItemDye->pProtoType->nSpecFuncVal1)
		return E_Dye_Exist;

	// 等级小于所染色的装备等级，则不可进行染色
	if(pEquip->pProtoType->byLevel > (pItemDye->pProtoType->byLevel * 10))
		return E_Dye_LvLimit;

	// 改变时装颜色
	pEquip->equipSpec.n8ColorID = pItemDye->pProtoType->nSpecFuncVal1;
	GetItemMgr().UpdateEquipSpec(*pEquip);

	// 材料消耗
	GetItemMgr().DelFromBag(n64DyeSN, (DWORD)ELCID_Compose_Dye, 1);

	return E_Success;
}

//-----------------------------------------------------------------------------
// 计算B类属性对强化成功率的影响
//-----------------------------------------------------------------------------
FLOAT Role::CalSpecAttEffectSuc(EEquipSpecAtt eSpecAtt, FLOAT fProp, EEquipSpecAtt eSpecType)
{
	switch (eSpecAtt)
	{
	case EESA_Guard_Posy:
	case EESA_Guard_Brand:
	case EESA_Guard_Engrave:
	case EESA_Guard_LongFu:
	case EESA_Guard_Quench:
	case EESA_Guard_Cut:
		{
			if(eSpecAtt != eSpecType)
				return fProp;
			else
				return 10000.0f;
		}
	case EESA_Guard_AllProduce:
		return 10000.0f;
		break;
	case EESA_Guard_Talisman:
		return fProp * 1.05f;
		break;
	default:
		return fProp;
		break;
	}
}

// 计算B类属性对强化完美率的影响
FLOAT Role::CalSpecAttEffectPef(EEquipSpecAtt eSpecAtt, FLOAT fPerfect)
{
	switch(eSpecAtt)
	{
	case EESA_Guard_Fortune:
		return fPerfect * 1.05f;
		break;
	default:
		return fPerfect;
		break;
	}
}

//-----------------------------------------------------------------------------
// 烙印
//-----------------------------------------------------------------------------
DWORD Role::BrandEquip(INT64 n64SrcItemID, INT64 n64DstItemID, INT64 n64IMID, DWORD dwCmdID)
{
	// 获得地图
	Map *pMap = GetMap();
	if(!P_VALID(pMap))
	{
		ASSERT(P_VALID(pMap));
		return GT_INVALID;
	}

	// 找到被强化装备
	tagEquip *pEquip = (tagEquip*)GetItemMgr().GetBagItem(n64DstItemID);
	if(!P_VALID(pEquip))
		return E_Consolidate_Equip_Not_Exist;

	if(!MIsEquipment(pEquip->dwTypeID))
		return E_Consolidate_NotEquipment;

	// 找到印记
	tagItem *pItemBrand = GetItemMgr().GetBagItem(n64SrcItemID);
	if(!P_VALID(pItemBrand) || pItemBrand->pProtoType->eSpecFunc != EISF_Brand )
		return E_Consolidate_Brand_Not_Exit;

	// 找到强化数据
	const tagConsolidateItem *pConsolidateProto = g_attRes.GetConsolidateProto(pItemBrand->dwTypeID);
	if(!P_VALID(pConsolidateProto))
		return E_Compose_Formula_Not_Exist;

	// 检测印记等级是否达到上限
	if((pEquip->equipSpec.byBrandLevel + pItemBrand->pProtoType->byLevel) > MAX_BRAND_LEVEL)
		return E_Consolidate_Brand_TopLevel;

	// 检测装备是否能被印记
	BOOL bConsolidate = FALSE;
	for(INT m = 0; m < MAX_CONSOLIDATE_POS_QUANTITY; ++m)
	{
		if(pEquip->pEquipProto->eEquipPos != pConsolidateProto->ConsolidatePos[m].ePos)
			continue;
		else
		{
			if( 1 == pConsolidateProto->ConsolidatePos[m].byConsolidate)
			{
				bConsolidate = TRUE;
				break;
			}
		}
	}

	if(!bConsolidate)
		return E_Consolidate_EquipCanNotBrand;

	// 检查装备潜力值是否足够
	if(pEquip->equipSpec.nPotVal < (INT)pConsolidateProto->dwPotValConsume)
		return E_Consolidate_ValConsume_Inadequate;

	// 记录印记等级
	BYTE byLevel = pItemBrand->pProtoType->byLevel;

	// 删除材料
	GetItemMgr().DelFromBag(n64SrcItemID, (DWORD)ELCID_Compose_Brand, 1);

	// 计算B类属性对成功率影响
	FLOAT fProp = CalSpecAttEffectSuc((EEquipSpecAtt)(pEquip->equipSpec.bySpecAtt), (FLOAT)(pConsolidateProto->nSuccessRate), EESA_Guard_Brand);

	// 计算IM道具的影响
	tagIMEffect			IMEffect;
	tagConsolidateItem *pProto = const_cast<tagConsolidateItem*>(pConsolidateProto);		
	CalIMEffect(ECTS_Brand, IMEffect, n64IMID, pProto);

	// 计算成功率
	BOOL bResult = IUTIL->Rand() % 10000 <= fProp;

	if(bResult)		// 成功
	{
		// 称号事件触发
		GetTitleMgr()->SigEvent(ETE_STRENGTHEN_SKILL_SUCCESS, GT_INVALID, GT_INVALID);

		// 装备烙印等级(增加量为印记等级)
		pEquip->equipSpec.byBrandLevel += byLevel;

		// 装备增加对应强化属性
		for(INT n = 0; n < MAX_CONSOLIDATE_ROLEATT; ++n)
		{
			pEquip->equipSpec.nBrandAtt[MTransERADerate2Index(pConsolidateProto->tagRoleAtt[n].eRoleAtt)] 
															+= pConsolidateProto->tagRoleAtt[n].nAttVal;
		}

		// 计算是否完美（生产完美率=生产成功率×1/20+角色福缘/1000)
		FLOAT fPefectProp = (pConsolidateProto->nSuccessRate * 0.05f) / 10000.0f+ (FLOAT)GetAttValue(ERA_Fortune) / 1000.0f;

		// 计算B类属性对完美率影响
		fPefectProp = CalSpecAttEffectPef((EEquipSpecAtt)(pEquip->equipSpec.bySpecAtt), fPefectProp);

		// 计算装备光晕
		CalEquipFlare(pEquip);

		BOOL bPefect = IUTIL->Rand() % 10000 <= (fPefectProp * 10000);

		if(bPefect)
		{
			// 消耗装备潜力值为烙印成功的75%
			pEquip->ChangePotVal(-((INT)pConsolidateProto->dwPotValConsume * 75 / 100));
			GetItemMgr().UpdateEquipSpec(*pEquip);
			return E_Compose_Consolidate_Perfect;
		}
		else
		{
			pEquip->ChangePotVal(-((INT)pConsolidateProto->dwPotValConsume));
			GetItemMgr().UpdateEquipSpec(*pEquip);
			return E_Compose_Consolidate_Success;
		}
	}
	else
	{
		// 装备潜力值消耗10点
		if(IMEffect.eEffect !=  EIME_ProtectSign)
			pEquip->ChangePotVal(-40/*10*/);// jason 2009-11-28

		GetItemMgr().UpdateEquipSpec(*pEquip);
		return E_Compose_Consolidate_Lose;
	}
}

//-----------------------------------------------------------------------------
// 龙附
//-----------------------------------------------------------------------------
DWORD Role::LoongSoulEquip(INT64 n64SrcItemID, INT64 n64DstItemID, DWORD dwCmdID)
{
	// 获得地图
	Map *pMap = GetMap();
	if(!P_VALID(pMap))
	{
		ASSERT(P_VALID(pMap));
		return GT_INVALID;
	}

	// 找到被强化装备
	tagEquip *pEquip = (tagEquip*)GetItemMgr().GetBagItem(n64DstItemID);
	if(!P_VALID(pEquip))
		return E_Consolidate_Equip_Not_Exist;

	if(!MIsEquipment(pEquip->dwTypeID))
		return E_Consolidate_NotEquipment;

	// 找到龙魂石
	tagItem *pItemLoongSoul = GetItemMgr().GetBagItem(n64SrcItemID);
	if(!P_VALID(pItemLoongSoul) || pItemLoongSoul->pProtoType->eSpecFunc != EISF_LongHunShi )
		return E_Consolidate_LoongSoul_Not_Exit;

	// 找到强化数据
	const tagConsolidateItem *pConsolidateProto = g_attRes.GetConsolidateProto(pItemLoongSoul->dwTypeID);
	if(!P_VALID(pConsolidateProto))
		return E_Compose_Formula_Not_Exist;

	// 检测印记等级是否达到上限
	if((pEquip->equipSpec.byLongfuLevel + pItemLoongSoul->pProtoType->byLevel) > MAX_LONGHUN_LEVEL)
		return E_Consolidate_LoongSoul_TopLevel;

	BOOL bConsolidate = FALSE;
	for(INT m = 0; m < MAX_CONSOLIDATE_POS_QUANTITY; ++m)
	{
		if(pEquip->pEquipProto->eEquipPos != pConsolidateProto->ConsolidatePos[m].ePos)
			continue;
		else
		{
			if( 1 == pConsolidateProto->ConsolidatePos[m].byConsolidate)
			{
				bConsolidate = TRUE;
				break;
			}
		}
	}

	if(!bConsolidate)
		return E_Consolidate_EquipCanNotLoongSoul;

	// 检查装备潜力值是否足够
	if(pEquip->equipSpec.nPotVal < (INT)pConsolidateProto->dwPotValConsume)
		return E_Consolidate_ValConsume_Inadequate;

	// 记录龙魂石等级
	BYTE byLevel = pItemLoongSoul->pProtoType->byLevel;

	// 删除材料
	GetItemMgr().DelFromBag(n64SrcItemID, (DWORD)ELCID_Compose_LongFu, 1);

	// 计算成功率(成功率=基础成功率×[1+（装备等级-75）/300]+（角色福缘/1000))
	FLOAT fProp = pConsolidateProto->nSuccessRate * (1.0f + (FLOAT)(pEquip->pProtoType->byLevel - 75.0f) / 300.0f)
					+ ((FLOAT)GetAttValue(ERA_Fortune) / 1000.0f) * 10000;

	// 计算B类属性对成功率影响
	fProp = CalSpecAttEffectSuc((EEquipSpecAtt)(pEquip->equipSpec.bySpecAtt), fProp, EESA_Guard_LongFu);

	BOOL bResult = IUTIL->Rand() % 10000 <= (INT)fProp;

	if(bResult)		// 成功
	{
		// 称号事件触发
		GetTitleMgr()->SigEvent(ETE_STRENGTHEN_SKILL_SUCCESS, GT_INVALID, GT_INVALID);

		// 装备龙魂等级(增加量为龙魂石等级)
		pEquip->equipSpec.byLongfuLevel += byLevel;

		// 装备增加对应强化属性
		pEquip->equipSpec.byLongfuAtt[pConsolidateProto->eWuXing] += byLevel;
		
		// 计算是否完美（完美率=1%+（角色福缘/1000))
		FLOAT fPefectProp = 0.01f + (FLOAT)GetAttValue(ERA_Fortune) / 1000.0f;

		// 计算B类属性对完美率影响
		fPefectProp =  CalSpecAttEffectPef((EEquipSpecAtt)(pEquip->equipSpec.bySpecAtt), fPefectProp);

		// 计算装备光晕
		CalEquipFlare(pEquip);

		BOOL bPefect = IUTIL->Rand() % 10000 <= (fPefectProp * 10000);

		if(bPefect)
		{
			// 消耗装备潜力值为龙魂成功的75%
			pEquip->ChangePotVal(-((INT)pConsolidateProto->dwPotValConsume * 75 / 100));
			GetItemMgr().UpdateEquipSpec(*pEquip);
			return E_Compose_Consolidate_Perfect;
		}
		else
		{
			pEquip->ChangePotVal(-((INT)pConsolidateProto->dwPotValConsume));
			GetItemMgr().UpdateEquipSpec(*pEquip);
			return E_Compose_Consolidate_Success;
		}
	}
	else
	{
		// 装备对应的龙附等级归0，消耗龙魂，不消耗装备潜力值，装备的龙魂属性清空
		pEquip->equipSpec.byLongfuLevel = 0;
		ZeroMemory(pEquip->equipSpec.byLongfuAtt, sizeof(pEquip->equipSpec.byLongfuAtt));
		GetItemMgr().UpdateEquipSpec(*pEquip);
		return E_Compose_Consolidate_Lose;
	}
}

//-----------------------------------------------------------------------------
// GM龙附
//-----------------------------------------------------------------------------
DWORD Role::GMLoongSoulEquip(INT64 n64SrcItemID, INT16 n16DstItemIndex)
{
	// 找到被强化装备
	tagEquip *pEquip = (tagEquip*)GetItemMgr().GetBagItem(n16DstItemIndex);
	if(!P_VALID(pEquip))
		return E_Consolidate_Equip_Not_Exist;

	if(!MIsEquipment(pEquip->dwTypeID))
		return E_Consolidate_NotEquipment;

	// 找到龙魂石
	tagItem *pItemLoongSoul = GetItemMgr().GetBagItem(n64SrcItemID);
	if(!P_VALID(pItemLoongSoul) || pItemLoongSoul->pProtoType->eSpecFunc != EISF_LongHunShi )
		return E_Consolidate_LoongSoul_Not_Exit;

	// 找到强化数据
	const tagConsolidateItem *pConsolidateProto = g_attRes.GetConsolidateProto(pItemLoongSoul->dwTypeID);
	if(!P_VALID(pConsolidateProto))
		return E_Compose_Formula_Not_Exist;

	// 检测印记等级是否达到上限
	if((pEquip->equipSpec.byLongfuLevel + pItemLoongSoul->pProtoType->byLevel) > MAX_LONGHUN_LEVEL)
		return E_Consolidate_LoongSoul_TopLevel;

	BOOL bConsolidate = FALSE;
	for(INT m = 0; m < MAX_CONSOLIDATE_POS_QUANTITY; ++m)
	{
		if(pEquip->pEquipProto->eEquipPos != pConsolidateProto->ConsolidatePos[m].ePos)
			continue;
		else
		{
			if( 1 == pConsolidateProto->ConsolidatePos[m].byConsolidate)
			{
				bConsolidate = TRUE;
				break;
			}
		}
	}

	if(!bConsolidate)
		return E_Consolidate_EquipCanNotLoongSoul;

	// 记录龙魂石等级
	BYTE byLevel = pItemLoongSoul->pProtoType->byLevel;

	// 装备龙魂等级(增加量为龙魂石等级)
	pEquip->equipSpec.byLongfuLevel += byLevel;

	// 装备增加对应强化属性
	pEquip->equipSpec.byLongfuAtt[pConsolidateProto->eWuXing] += byLevel;

	// 计算装备光晕
	CalEquipFlare(pEquip);

	GetItemMgr().UpdateEquipSpec(*pEquip);
	return E_Compose_Consolidate_Success;
}

//---------------------------------------------------------------------------------
// 淬火
//---------------------------------------------------------------------------------
DWORD Role::QuenchEquip(DWORD dwNPCID, DWORD dwFormulaID, INT64 n64ItemID, INT64 n64IMID, 
											INT64 n64StuffID[], INT32 nArraySz, DWORD dwCmdID)
{
	// 获得地图
	Map *pMap = GetMap();
	if(!P_VALID(pMap))
	{
		ASSERT(P_VALID(pMap));
		return GT_INVALID;
	}

	Creature* pNPC = pMap->FindCreature(dwNPCID);
	if( !P_VALID(pNPC) ) 
		return E_Compose_NPC_Not_Exist;

	if( FALSE == pNPC->CheckNPCTalkDistance(this) )
		return E_Compose_NPC_Distance;

	if( FALSE == pNPC->IsFunctionNPC(EFNPCT_Quench) )
		return E_Consolidate_NPCCanNotQuench;

	// 找到强化数据
	const tagQuenchProtoSer *pQuenchProto = g_attRes.GetQuenchProto(dwFormulaID);
	if(!P_VALID(pQuenchProto))
		return E_Compose_Formula_Not_Exist;

	// 找到被强化装备
	tagEquip *pEquip = (tagEquip*)GetItemMgr().GetBagItem(n64ItemID);
	if(!P_VALID(pEquip))
		return E_Consolidate_Equip_Not_Exist;

	if(!MIsEquipment(pEquip->dwTypeID))
		return E_Consolidate_NotEquipment;

	// 检查装备潜力值是否足够
	if(pEquip->equipSpec.nPotVal < (INT)pQuenchProto->dwPotValConsume)
		return E_Consolidate_ValConsume_Inadequate;

	// 检查玩家金钱是否足够
	if(GetCurMgr().GetBagSilver() < pQuenchProto->dwMoneyConsume)
		return E_Consolidate_NotEnough_Money;

	// 原属性的属性值是否足够
	if(pEquip->equipSpec.byLongfuAtt[pQuenchProto->srcQuenchAtt.eWuXing] < pQuenchProto->srcQuenchAtt.nWuXingValue)
		return E_Consolidate_WXValueInadequate;

	for(INT n = 0; n < MAX_CONSOLIDATE_STUFF_QUANTITY; ++n)
	{
		if(pQuenchProto->ConsolidateStuff[n].dwStuffID == GT_INVALID 
			&& pQuenchProto->ConsolidateStuff[n].eStuffType == EST_Null)
			break;

		tagItem *pItemStuff = GetItemMgr().GetBagItem(n64StuffID[n]); 

		if(!P_VALID(pItemStuff))
			return E_Consolidate_NotEnough_Stuff;

		// 检测材料64位ID是否重复
		for(INT i = 0; i < n; ++i)
		{
			if(n64StuffID[i] == n64StuffID[n])
				return E_Consolidate_NotEnough_Stuff;
		}

		if(pItemStuff->dwTypeID != pQuenchProto->ConsolidateStuff[n].dwStuffID 
			&& pItemStuff->pProtoType->eStuffType != pQuenchProto->ConsolidateStuff[n].eStuffType)
			return E_Consolidate_NotEnough_Stuff;

		if(pItemStuff->n16Num < (INT)pQuenchProto->ConsolidateStuff[n].dwStuffNum)
			return E_Consolidate_NotEnough_Stuff;
	}

	// 计算IM道具的影响
	tagIMEffect			IMEffect;
	tagQuenchProtoSer *pProto = const_cast<tagQuenchProtoSer*>(pQuenchProto);
	CalIMEffect(ECTS_Quench, IMEffect, n64IMID, pProto);

	// 输入材料消耗
	for(INT n = 0; n < nArraySz; ++n)
	{
		GetItemMgr().DelFromBag(n64StuffID[n], (DWORD)ELCID_Compose_Quench, (INT16)pQuenchProto->ConsolidateStuff[n].dwStuffNum);
	}

	// 装备潜力值消耗
	pEquip->ChangePotVal(-(INT)(pQuenchProto->dwPotValConsume));

	// 金钱消耗
	GetCurMgr().DecBagSilver(pQuenchProto->dwMoneyConsume, ELCID_Compose_Quench);

	// 计算B类属性对成功率影响
	FLOAT fProp = CalSpecAttEffectSuc((EEquipSpecAtt)(pEquip->equipSpec.bySpecAtt), (FLOAT)pQuenchProto->nSuccessRate, EESA_Guard_Quench);

	// 检测玩家是否用了提高成功率的IM
	if( IMEffect.eEffect ==  EIME_ComAdvance)
	{
		fProp += (FLOAT)IMEffect.dwParam2;
	}

	BOOL bResult = IUTIL->Rand() % 10000 <= fProp;
	if(bResult)
	{
		// 增加目标属性
		pEquip->equipSpec.byLongfuAtt[pQuenchProto->srcQuenchAtt.eWuXing] -= pQuenchProto->srcQuenchAtt.nWuXingValue;
		pEquip->equipSpec.byLongfuAtt[pQuenchProto->dstQuenchAtt.eWuXing] += pQuenchProto->dstQuenchAtt.nWuXingValue;
		GetItemMgr().UpdateEquipSpec(*pEquip);
		return E_Compose_Consolidate_Success;
	}
	else		// 失误
	{
		// 增加的属性类型在除了原属性和目标属性外的其它三种属性中随机
		EWuXing		eType = EWX_Null; 
		INT			nWuXingValue = pQuenchProto->dstQuenchAtt.nWuXingValue;
		BYTE		byTotalValue = 0;
		while (TRUE)
		{
			eType =(EWuXing)(IUTIL->Rand() % 5);
			if(pQuenchProto->srcQuenchAtt.eWuXing != eType && pQuenchProto->dstQuenchAtt.eWuXing != eType)
				break;
		}

		// 配方表中所填的目标属性增加值为n，则失误时实际增加值在[n-2，n+1]间随机，
		// 取值范围为[1，10]，且淬火后所有龙魂属性总合不能超过10
		nWuXingValue = IUTIL->Rand() % 3 + (nWuXingValue - 2);

		for(INT i = 0; i < EWX_End; ++i)
		{
			byTotalValue += pEquip->equipSpec.byLongfuAtt[i];
		}

		nWuXingValue = (nWuXingValue > (10 - byTotalValue)) ? (10 - byTotalValue) : nWuXingValue;
		pEquip->equipSpec.byLongfuAtt[eType] += nWuXingValue;
		pEquip->equipSpec.byLongfuAtt[pQuenchProto->srcQuenchAtt.eWuXing] -= pQuenchProto->srcQuenchAtt.nWuXingValue;

		GetItemMgr().UpdateEquipSpec(*pEquip);
		return E_Consolidate_False;

	}
}
//---------------------------------------------------------------------------------
// 生产合成物品
//---------------------------------------------------------------------------------
DWORD Role::ProduceItem(DWORD dwNPCID, DWORD dwSkillID, INT64 n64ItemID, DWORD dwFormulaID,
								INT64 n64IMID, INT64 n64StuffID[], INT32 nArraySz, DWORD dwCmdID)
{
	// 找到合成表数据
	const tagProduceProtoSer* pProduceProto = g_attRes.GetProduceProto(dwFormulaID);

	if(!P_VALID(pProduceProto))
		return E_Compose_Formula_Not_Exist;

	DWORD dwErrorCode = E_Success;

	dwErrorCode = CheckProduceLimit(dwNPCID, dwSkillID, n64ItemID, dwFormulaID, pProduceProto, n64StuffID, nArraySz);

	if( dwErrorCode != E_Success)
		return dwErrorCode;

	// 计算IM道具的影响
	tagIMEffect			IMEffect;
	tagProduceProtoSer *pProto = const_cast<tagProduceProtoSer*>(pProduceProto);
	CalIMEffect(ECTS_Produce, IMEffect, n64IMID, pProto);

	// 输入材料消耗
	for(INT n = 0; n < nArraySz; ++n)
	{
		GetItemMgr().DelFromBag(n64StuffID[n], (DWORD)ELCID_Compose_Produce, (INT16)pProduceProto->ProduceStuff[n].dwStuffNum);
	}

	// 金钱消耗
	GetCurMgr().DecBagSilver(pProduceProto->dwMoneyConsume, ELCID_Compose_Produce);

	// 增加合成技能熟练度(只有技能合成才会增加技能熟练度）
	Skill* pSkill = GetSkill(dwSkillID);

	if(P_VALID(pSkill) && GT_INVALID == dwNPCID && GT_INVALID == n64ItemID)
	{
		DWORD	dwSkillExp = (DWORD)((FLOAT)pProduceProto->dwMasterIncre * (1.0f + (FLOAT)GetAttValue(ERA_Savvy) / 100.0f));
		ChangeSkillExp(pSkill, dwSkillExp);
	}

	// 扣除玩家的活力值
	ChangeVitality(-(INT)pProduceProto->dwVitalityConsume);

	// 计算成功率
	FLOAT	fProp = (FLOAT)pProduceProto->nSuccessRate;

	// 检测玩家是否用了提高成功率的IM
	if( IMEffect.eEffect ==  EIME_ComAdvance)
	{
		fProp += (FLOAT)IMEffect.dwParam2;
	}

	BOOL bResult = IUTIL->Rand() % 10000 <= (INT)fProp;

	// 称号事件触发
	if (bResult && GT_VALID(dwNPCID))
		GetTitleMgr()->SigEvent(ETE_COMPOSITE_SKILL_SUCCESS	, GT_INVALID, GT_INVALID);

	if(bResult)			// 成功
	{
		// 获得合成道具
		if(pProduceProto->eComType == ECOMT_RefineDan)
		{
			// 计算完美率
			FLOAT fPefectProp = 0.01f + (FLOAT)GetAttValue(ERA_Fortune) / 1000.0f;
			BOOL bPefect = IUTIL->Rand() % 10000 <= (fPefectProp * 10000);

			// 完美后所制造的物品数量翻倍
			if(bPefect)
			{
				tagItem *pItem = CreateItem(EICM_Product,  m_dwID, 
								pProduceProto->dwProItemTypeID, (INT16)(pProduceProto->dwProQuantity * 2), m_dwID);
				if( P_VALID(pItem) )
				{
					GetItemMgr().Add2Bag(pItem, ELCID_Compose_Produce, TRUE);
				}

				return E_Compose_Consolidate_Perfect;
			}
			else
			{
				tagItem *pItem = CreateItem(EICM_Product,  m_dwID, 
								pProduceProto->dwProItemTypeID, (INT16)pProduceProto->dwProQuantity, m_dwID);
				if( P_VALID(pItem) )
				{
					GetItemMgr().Add2Bag(pItem, ELCID_Compose_Produce, TRUE);
				}

				return E_Compose_Consolidate_Success;
			}
		}
		else
		{
			tagItem *pItem = CreateItem(EICM_Product,  m_dwID, 
								pProduceProto->dwProItemTypeID, (INT16)pProduceProto->dwProQuantity, m_dwID);
			if( P_VALID(pItem) )
			{
				GetItemMgr().Add2Bag(pItem, ELCID_Compose_Produce, TRUE);
			}
			
			return E_Compose_Consolidate_Success;
		}
	}
	else
	{
		return E_Compose_Consolidate_Lose;
	}

}

//---------------------------------------------------------------------------------
// 生产合成装备
// Jason 2009-12-4 按材料类型计算加成
//--------------------------------------------------------------------------------
DWORD Role::ProduceEquip(DWORD dwNPCID, DWORD dwSkillID, INT64 n64ItemID, DWORD dwFormulaID, 
										INT64 n64IMID, INT64 n64StuffID[], INT32 nArraySz, DWORD dwCmdID)
{
	// 找到合成表数据
	const tagProduceProtoSer* pProduceProto = g_attRes.GetProduceProto(dwFormulaID);

	if(!P_VALID(pProduceProto))
		return E_Compose_Formula_Not_Exist;

	DWORD dwErrorCode = E_Success;

	dwErrorCode = CheckProduceLimit(dwNPCID, dwSkillID, n64ItemID, dwFormulaID, pProduceProto, n64StuffID, nArraySz);

	if( dwErrorCode != E_Success)
		return dwErrorCode;

	// 输入材料消耗(顺便计算材料品质）
	INT nStuffNum[EIQ_End];
	ZeroMemory(nStuffNum, sizeof(nStuffNum));
	INT nTotalNum = 0;

	for(INT n = 0; n < nArraySz; ++n)
	{	
		tagItem *pItemStuff = GetItemMgr().GetBagItem(n64StuffID[n]); 

		if( EST_Null != pProduceProto->ProduceStuff[n].eStuffType ) // Jason，只统计类型明确的材料数量
		{
			nStuffNum[pItemStuff->pProtoType->byQuality] += pProduceProto->ProduceStuff[n].dwStuffNum;
			nTotalNum += pProduceProto->ProduceStuff[n].dwStuffNum;
		}

		GetItemMgr().DelFromBag(n64StuffID[n], (DWORD)ELCID_Compose_Produce, (INT16)pProduceProto->ProduceStuff[n].dwStuffNum);
	}

	// 金钱消耗
	GetCurMgr().DecBagSilver(pProduceProto->dwMoneyConsume, ELCID_Compose_Produce);

	// 增加合成技能熟练度(只有技能合成才会增加技能熟练度）
	Skill* pSkill = GetSkill(dwSkillID);

	if(P_VALID(pSkill) && GT_INVALID == dwNPCID && GT_INVALID == n64ItemID)
	{
		DWORD	dwSkillExp = (DWORD)((FLOAT)pProduceProto->dwMasterIncre * (1.0f + (FLOAT)GetAttValue(ERA_Savvy) / 100.0f));
		ChangeSkillExp(pSkill, dwSkillExp);
	}

	// 扣除玩家的活力值
	ChangeVitality(-(INT)pProduceProto->dwVitalityConsume);

	// 获得合成道具
	/* 1级修正率=-50%×白品材料数量/材料总数量+10%×黄品材料数量/材料总数量+15%×绿品材料数量
				 /材料总数量+50%×蓝品材料数量/材料总数量+100%×橙品材料数量/材料总数量

	   2级修正率=5%×绿品材料数量/材料总数量+20%×蓝品材料数量/材料总数量+30%×橙品材料数量/材料总数量						*/
	FLOAT fModify1 = 0;
	FLOAT fModify2 = 0;
	FLOAT fModify3 = 0;
	FLOAT fPurplePct = 0;
	INT nPotAdding = 0;

	if( nTotalNum )
	{
		fModify1 = ((-0.5f * (FLOAT)nStuffNum[EIQ_White] + 0.1f * (FLOAT)nStuffNum[EIQ_Yellow]
		+ 0.15f * (FLOAT)nStuffNum[EIQ_Green] + 0.5f * (FLOAT)nStuffNum[EIQ_Blue] 
		+ (FLOAT)nStuffNum[EIQ_Orange]
		+ (FLOAT)nStuffNum[EIQ_Purple]  ) / (FLOAT)nTotalNum) * 10000.0f;

		fModify2 = (( 0.05f * (FLOAT)nStuffNum[EIQ_Green] + 0.2f * (FLOAT)nStuffNum[EIQ_Blue] + 0.3f * (FLOAT)nStuffNum[EIQ_Orange] + 0.5 * nStuffNum[EIQ_Purple] ) / (FLOAT)nTotalNum) * 10000.0f;

		// 潜力值修正率
		// 潜力值影响系数 = 0.7×白品材料数量/材料总数量+0.8×黄品材料数量/材料总数量+0.9×绿品材料数量/材料总数量+1.0×蓝品材料数量/材料总数量+1.1×橙品材料数量/材料总数量
		fModify3 = ((0.7f * (FLOAT)nStuffNum[EIQ_White] + 0.8f * (FLOAT)nStuffNum[EIQ_Yellow]
		+ 0.9f * (FLOAT)nStuffNum[EIQ_Green] + 1.0f * (FLOAT)nStuffNum[EIQ_Blue]
		+ 1.1f * (FLOAT)nStuffNum[EIQ_Orange] 
		+ 1.2  * nStuffNum[EIQ_Purple] ) / (FLOAT)nTotalNum) * 10000.0f;

		fPurplePct = ( 0.05 * (FLOAT)nStuffNum[EIQ_Blue] + 0.22 * (FLOAT)nStuffNum[EIQ_Orange] + nStuffNum[EIQ_Purple] ) / nTotalNum;

		// Jason 2009-12-1
		//生产装备可提升潜力值次数 = 
		//静态属性值 + （N白/N总*0+N黄/N总*0+N绿/N总*0+N蓝/N总*3+N橙/N总*6+N紫/N总*10）
		//该值向下取整
		nPotAdding = ( nStuffNum[EIQ_Blue] * 3 + nStuffNum[EIQ_Orange] * 6 + nStuffNum[EIQ_Purple] * 10 ) / nTotalNum;
	}


	FLOAT nRnd = ( (FLOAT) IUTIL->RandomInRange(0,100) ) / 100;
	


	tagItem *pItem = NULL;
	if( nRnd > fPurplePct || fPurplePct == 0 )//如果紫色品级鉴定几率为0，也得走老的鉴定方法，Jason 2009-12-23 fix
		pItem = ItemCreator::Create(EICM_Product,  m_dwID, 
			pProduceProto->dwProItemTypeID, (INT16)(pProduceProto->dwProQuantity), m_dwID, (INT16)fModify1, (INT16)fModify2, (INT16)fModify3,nPotAdding);
	else
		pItem = ItemCreator::CreateByProduce(EICM_Product,  m_dwID, 
		pProduceProto->dwProItemTypeID,EIQ_Purple, nPotAdding ,(DWORD)(fPurplePct * 100),(INT16)(pProduceProto->dwProQuantity), m_dwID, (INT16)fModify1, (INT16)fModify2, (INT16)fModify3);

	// 判断是否为时装，是则鉴定
	if(P_VALID(pItem) && MIsFashion(pItem))
	{
		// 计算IM道具的影响
		tagIMEffect			IMEffect;
		tagProduceProtoSer *pProto = const_cast<tagProduceProtoSer*>(pProduceProto);
		CalIMEffect(ECTS_Produce, IMEffect, n64IMID, pProto);

		ItemCreator::IdentifyEquip((tagEquip*)pItem, EIQ_Null, &IMEffect);
	}
	
	if( P_VALID(pItem) )
	{
		GetItemMgr().Add2Bag(pItem, ELCID_Compose_Produce, TRUE);
	}

	// 称号事件触发
	if (GT_VALID(dwNPCID))
		GetTitleMgr()->SigEvent(ETE_COMPOSITE_SKILL_SUCCESS, GT_INVALID, GT_INVALID);

	return E_Compose_Consolidate_Success;
}

//---------------------------------------------------------------------------------
// 点化,装备分解
//---------------------------------------------------------------------------------
DWORD Role::DeComposeItem(DWORD dwNPCID, DWORD dwSkillID, INT64 n64ItemID, DWORD dwFormulaID, 
								INT64 n64IMID, INT64 n64Item, DWORD dwCmdID)
{
	// 找到合成表数据
	const tagDeComposeProtoSer* pDeComposeProto = g_attRes.GetDeComposeProto(dwFormulaID);

	if(!P_VALID(pDeComposeProto))
		return E_Compose_Formula_Not_Exist;

	// 找到出入材料
	tagEquip *pEquip = (tagEquip*)GetItemMgr().GetBagItem(n64Item); 

	DWORD dwErrorCode = E_Success;

	dwErrorCode = CheckDeComposeLimit(dwNPCID, dwSkillID, n64ItemID, dwFormulaID, pDeComposeProto, n64Item, pEquip);

	if( dwErrorCode != E_Success)
		return dwErrorCode;

	// 点化只限于黄色及以上装备，分解则不限品质
	if(pDeComposeProto->eComType == ECOMT_PointUp && pEquip->equipSpec.byQuality == (BYTE)EIQ_White)
		return E_Compose_Quality_Not_Match;

	// 面具及时装不可点化或分解
	if(pEquip->pProtoType->eType == EIT_Fashion || pEquip->pEquipProto->eEquipPos == EEP_Face)
		return E_Compose_Not_Fashion;

	// 已锁定的装备不可点化或分解
	if(pEquip->bLock == true)
		return E_Compose_Equip_Lock;

	// 有时间限制的装备不可点化或分解
	if(pEquip->pProtoType->dwTimeLimit != GT_INVALID)
		return E_Compose_Equip_Time_Limit;

	// 未鉴定的装备不可点化或分解
	if(pEquip->equipSpec.byQuality == (BYTE)EIQ_Null)
		return E_Compose_Equip_Not_identify;

	// 金钱消耗
	GetCurMgr().DecBagSilver(pDeComposeProto->dwMoneyConsume, ELCID_Compose_Decompose);

	// 增加合成技能熟练度(只有技能合成才会增加技能熟练度）
	Skill* pSkill = GetSkill(dwSkillID);

	if(P_VALID(pSkill) && GT_INVALID == dwNPCID && GT_INVALID == n64ItemID)
	{
		DWORD	dwSkillExp = (DWORD)((FLOAT)pDeComposeProto->dwMasterIncre * (1.0f + (FLOAT)GetAttValue(ERA_Savvy) / 100.0f));
		ChangeSkillExp(pSkill, dwSkillExp);
	}

	// 扣除玩家的活力值
	ChangeVitality(-(INT)pDeComposeProto->dwVitalityConsume);

	// 计算是否完美
	// 已消耗潜力值 > 潜力值上限/5 
	// 当以上条件不满足时，计算完美率的公式为：完美率=1%+角色福缘/1000)
	BOOL	bPerfect = FALSE;
	BOOL	bSuccess = FALSE;
	if( pEquip->equipSpec.nPotValUsed > pEquip->pEquipProto->nMaxPotVal / 5)
		bPerfect = TRUE;
	else
	{
		FLOAT fPefectProp = 0.01f + (FLOAT)GetAttValue(ERA_Fortune) / 1000.0f;
		bPerfect = IUTIL->Rand() % 10000 <= (fPefectProp * 10000);
	}

	// 输入材料消耗
	GetItemMgr().DelFromBag(n64Item, (DWORD)ELCID_Compose_Decompose, (INT16)pEquip->n16Num);

	for(INT i = 0; i < MAX_OUTPUT_QUANTITY; ++i)
	{
		if(pDeComposeProto->OutputStuff[i].dwStuffTypeID == GT_INVALID)
			break;

		// 计算成功率
		BOOL bResult = IUTIL->Rand() % 10000 <= (pDeComposeProto->OutputStuff[i].nRate);
		
		if(bResult)
		{
			bSuccess = TRUE;

			// 完美
			if(bPerfect)
			{
				INT nStuffNum = IUTIL->Rand() % (pDeComposeProto->OutputStuff[i].nPerMaxVal + 1);

				if(nStuffNum < pDeComposeProto->OutputStuff[i].nPerMinVal)
					nStuffNum = pDeComposeProto->OutputStuff[i].nPerMinVal;

				tagItem *pItem = CreateItem(EICM_Product,  m_dwID, 
					pDeComposeProto->OutputStuff[i].dwStuffTypeID, (INT16)nStuffNum, m_dwID);

				if( P_VALID(pItem) )
				{
					GetItemMgr().Add2Bag(pItem, ELCID_Compose_Produce, TRUE);
				}
			}
			else	// 成功
			{
				INT nStuffNum = IUTIL->Rand() % (pDeComposeProto->OutputStuff[i].nSucMaxVal + 1);

				if(nStuffNum < pDeComposeProto->OutputStuff[i].nSucMinVal)
					nStuffNum = pDeComposeProto->OutputStuff[i].nSucMinVal;

				tagItem *pItem = CreateItem(EICM_Product,  m_dwID, 
					pDeComposeProto->OutputStuff[i].dwStuffTypeID, (INT16)nStuffNum, m_dwID);

				if( P_VALID(pItem) )
				{
					GetItemMgr().Add2Bag(pItem, ELCID_Compose_Produce, TRUE);
				}
			}
		}		
	}

	if(bSuccess)
	{
		if(bPerfect)
			return E_Compose_Consolidate_Perfect;
		else	
			return E_Compose_Consolidate_Success;
	}
	else
		return E_Compose_Consolidate_Lose;
}

//---------------------------------------------------------------------------------
// 检测生产合成时的限制
//---------------------------------------------------------------------------------
DWORD Role::CheckProduceLimit(DWORD dwNPCID, DWORD dwSkillID, INT64 n64ItemID, DWORD dwFormulaID, 
								const tagProduceProtoSer* &pProduceProto, INT64 n64StuffID[], INT32 nArraySz)
{
	// 技能合成方式
	if(( pProduceProto->eFormFrom == EFormulaFrom_Skill || pProduceProto->eFormFrom == EFormulaFrom_SkillorItem
		|| pProduceProto->eFormFrom == EFormulaFrom_SkillorNPC) && GT_INVALID != dwSkillID)
	{
		Skill* pSkill = GetSkill(dwSkillID);

		if(!P_VALID(pSkill))
			return E_Compose_Skill_Not_Exist;

		// 检测技能类型是否匹配
		if(Skill2ProduceType(pSkill->GetTypeEx2()) != pProduceProto->eProType)
			return E_Compose_FormulaNotMatch;

		// 检测技能等级与配方等级是否匹配
		if(pSkill->GetLevel() < pProduceProto->nFormLvl)
			return E_Compose_FormulaNotMatch;		
	}
	// 物品合成方式
	else if(( pProduceProto->eFormFrom == EFormulaFrom_Item || pProduceProto->eFormFrom == EFormulaFrom_SkillorItem
		|| pProduceProto->eFormFrom == EFormulaFrom_NPCorItem) && GT_INVALID != n64ItemID)
	{
		// 物品是否在背包内

		// 是否为合成类物品
	}
	// NPC合成方式
	else if(( pProduceProto->eFormFrom == EFormulaFrom_NPC || pProduceProto->eFormFrom == EFormulaFrom_SkillorNPC
		|| pProduceProto->eFormFrom == EFormulaFrom_NPCorItem) && GT_INVALID != dwNPCID)
	{
		Creature* pNPC = GetMap()->FindCreature(dwNPCID);
		if( !P_VALID(pNPC)) 
			return E_Compose_NPC_Not_Exist;

		if( FALSE == pNPC->CheckNPCTalkDistance(this) )
			return E_Compose_NPC_Distance;

		if( FALSE == pNPC->IsFunctionNPC(EFNPCT_Compose) )
			return E_Compose_NPCCanNotCompose;

		// 还要检测NPC是否挂有合成配方
	}

	// 若角色背包已满				
	ItemMgr& itemMgr = GetItemMgr();	
	if (itemMgr.GetBagFreeSize() <= 0)
		return E_Compose_Bag_Full;

	// 玩家活力值是否足够
	if(GetAttValue(ERA_Vitality) < (INT)pProduceProto->dwVitalityConsume)
		return E_Compose_Vitality_Inadequate;

	// 检查玩家金钱是否足够
	if(GetCurMgr().GetBagSilver() < pProduceProto->dwMoneyConsume)
		return E_Compose_NotEnough_Money;

	// 检测玩家材料是否足够
	for(INT n = 0; n < MAX_PRODUCE_STUFF_QUANTITY; ++n)
	{
		if(pProduceProto->ProduceStuff[n].dwStuffID == GT_INVALID 
			&& pProduceProto->ProduceStuff[n].eStuffType == EST_Null)
			break;

		tagItem *pItemStuff = GetItemMgr().GetBagItem(n64StuffID[n]); 

		if(!P_VALID(pItemStuff))
			return E_Compose_Stuff_Not_Enough;

		// 检测材料64位ID是否重复
		for(INT i = 0; i < n; ++i)
		{
			if(n64StuffID[i] == n64StuffID[n])
				return E_Compose_Stuff_Not_Enough;
		}

		if(pItemStuff->dwTypeID != pProduceProto->ProduceStuff[n].dwStuffID 
			&& pItemStuff->pProtoType->eStuffType != pProduceProto->ProduceStuff[n].eStuffType)
			return E_Compose_Stuff_Not_Enough;

		if(pItemStuff->n16Num < (INT)pProduceProto->ProduceStuff[n].dwStuffNum)
			return E_Consolidate_NotEnough_Stuff;
	}

	return E_Success;
}

//---------------------------------------------------------------------------------
// 检测生产合成时的限制
//---------------------------------------------------------------------------------
DWORD Role::CheckDeComposeLimit(DWORD dwNPCID, DWORD dwSkillID, INT64 n64ItemID, DWORD dwFormulaID, 
						 const tagDeComposeProtoSer* &pDeComposeProto, INT64 n64Item, tagEquip *pEquip)
{
	// 技能合成方式
	if(( pDeComposeProto->eFormFrom == EFormulaFrom_Skill || pDeComposeProto->eFormFrom == EFormulaFrom_SkillorItem
		|| pDeComposeProto->eFormFrom == EFormulaFrom_SkillorNPC) && GT_INVALID != dwSkillID)
	{
		Skill* pSkill = GetSkill(dwSkillID);

		if(!P_VALID(pSkill))
			return E_Compose_Skill_Not_Exist;

		// 检测技能类型是否匹配
		if(pSkill->GetTypeEx2() != ESSTE2_PointUp)
			return E_Compose_FormulaNotMatch;

		// 检测技能等级与配方等级是否匹配
		if(pSkill->GetLevel() < pDeComposeProto->nFormLvl)
			return E_Compose_FormulaNotMatch;		
	}
	// 物品合成方式
	else if(( pDeComposeProto->eFormFrom == EFormulaFrom_NPC || pDeComposeProto->eFormFrom == EFormulaFrom_SkillorNPC
			|| pDeComposeProto->eFormFrom == EFormulaFrom_NPCorItem) && GT_INVALID != n64ItemID )
	{
		// 物品是否在背包内

		// 是否为合成类物品
	}
	// NPC合成方式
	else if( ( pDeComposeProto->eFormFrom == EFormulaFrom_Item || pDeComposeProto->eFormFrom == EFormulaFrom_SkillorItem
			|| pDeComposeProto->eFormFrom == EFormulaFrom_NPCorItem) && GT_INVALID != dwNPCID )
	{
		Creature* pNPC = GetMap()->FindCreature(dwNPCID);
		if( !P_VALID(pNPC)) 
			return E_Compose_NPC_Not_Exist;

		if( FALSE == pNPC->CheckNPCTalkDistance(this) )
			return E_Compose_NPC_Distance;

		if( FALSE == pNPC->IsFunctionNPC(EFNPCT_Compose) )
			return E_Compose_NPCCanNotCompose;

		// 还要检测NPC是否挂有合成配方
	}

	// 若角色背包已满				
	ItemMgr& itemMgr = GetItemMgr();	
	if (itemMgr.GetBagFreeSize() <= 0)
		return E_Compose_Bag_Full;

	// 玩家活力值是否足够
	if(GetAttValue(ERA_Vitality) < (INT)pDeComposeProto->dwVitalityConsume)
		return E_Compose_Vitality_Inadequate;

	if(!P_VALID(pEquip))
		return E_Compose_Stuff_Not_Enough;

	// 检查玩家金钱是否足够
	if(GetCurMgr().GetBagSilver() < pDeComposeProto->dwMoneyConsume)
		return E_Compose_NotEnough_Money;

	// 检查装备等级是否满足
	if((pEquip->pProtoType->byLevel / 10) < pDeComposeProto->byLevel)
		return E_Compose_Equip_Level_Inadequate;

	// 检测装备品质是否满足要求
	if(pEquip->equipSpec.byQuality < pDeComposeProto->byQuality)
		return E_Compose_Equip_Quality_Inadequate;

	// 检测武器类型是否满足配方要求
	if(pEquip->pProtoType->eType == EIT_Weapon && pEquip->pProtoType->eTypeEx != pDeComposeProto->eTypeEx)
		return E_Compose_Equip_Type_Inadequate;

	// 检测装备是否满足配方要求
	if((pEquip->pProtoType->eType == EIT_ClothArmor 
		|| pEquip->pProtoType->eType == EIT_Decoration 
		|| pEquip->pProtoType->eType == EIT_Armor) 
		&& pEquip->pEquipProto->eEquipPos != pDeComposeProto->ePos)

		return E_Compose_Equip_Type_Inadequate;

	return E_Success;
}

//---------------------------------------------------------------------------------
// 生产技能类型转换为生产类型
//---------------------------------------------------------------------------------
EProduceType Role::Skill2ProduceType(ESkillTypeEx2 eSkillType)					 
{													 				 
	switch(eSkillType)								 
	{												 
	case ESSTE2_Smilt:					return EPCT_Smilt;							 
	case ESSTE2_Artisan:				return EPCT_Artisan;						 
	case ESSTE2_DanTraining:			return EPCT_DanTraining;					 
	case ESSTE2_Smith:					return EPCT_Smith;							 
	case ESSTE2_Casting:				return EPCT_Casting;						 
	case ESSTE2_Dressmaker:				return EPCT_Dressmaker;						 
	case ESSTE2_Aechnics:				return EPCT_Aechnics;
	default:							return EPCT_NULL;
	}												 
}

//---------------------------------------------------------------------------------
// 生产物品（是否需要记生产者）
//---------------------------------------------------------------------------------
tagItem* Role::CreateItem(EItemCreateMode eCreateMode, DWORD dwCreateID, DWORD dwTypeID, INT16 n16Num, DWORD dwCreator)
{
	tagItemProto *pProto = g_attRes.GetItemProto(dwTypeID);
	if(!P_VALID(pProto))
		return (tagItem*)GT_INVALID;

	if(pProto->n16MaxLapNum > 1)
		return ItemCreator::Create(EICM_Product,  m_dwID, dwTypeID, n16Num);
	else
		return ItemCreator::Create(EICM_Product,  m_dwID, dwTypeID, n16Num, m_dwID);		
}

//---------------------------------------------------------------------------------
// 计算IM物品的影响
//---------------------------------------------------------------------------------
VOID Role::CalIMEffect(EConsolidateTypeSer eConType, tagIMEffect &IMEffect, INT64 n64IMID, const LPVOID pProto)
{

	// 检测玩家是否用了提高成功率的IM
	if( GT_INVALID == n64IMID)
		return;

	tagItem *pItemIM = GetItemMgr().GetBagItem(n64IMID); 
	if(!P_VALID(pItemIM))
		return;

	switch (eConType)
	{
	case ECTS_Posy:
		{
			const tagPosyProtoSer*  pPosyProto = (tagPosyProtoSer*)pProto;
			if(pItemIM->pProtoType->eSpecFunc == EISF_ComposeAdvance)
			{
			   if(pItemIM->pProtoType->nSpecFuncVal1 == (INT)EISFC_Posy 
				  || pItemIM->pProtoType->nSpecFuncVal1 == (INT)EISFC_AllConsolidate
				  || pItemIM->pProtoType->nSpecFuncVal1 == (INT)EISFC_All)
			   {
					IMEffect.eEffect = EIME_ComAdvance;
					IMEffect.dwParam2 = pItemIM->pProtoType->nSpecFuncVal2;

					// 删除IM道具
					GetItemMgr().DelFromBag(n64IMID, (DWORD)ELCID_Compose_Posy, 1);
			   }
			   break;
			}

			if(pItemIM->pProtoType->eSpecFunc == EISF_ProtectSign)
			{
				IMEffect.eEffect = EIME_ProtectSign;
				// 删除IM道具
				GetItemMgr().DelFromBag(n64IMID, (DWORD)ELCID_Compose_Posy, 1);
			}
		}
		break;
	case ECTS_Engrave:
		{
			const tagEngraveProtoSer* pEngraveProto = (tagEngraveProtoSer*)pProto;
			if(pItemIM->pProtoType->eSpecFunc == EISF_ComposeAdvance)
			{
				if(pItemIM->pProtoType->nSpecFuncVal1 == (INT)EISFC_Engrave 
					|| pItemIM->pProtoType->nSpecFuncVal1 == (INT)EISFC_AllConsolidate
					|| pItemIM->pProtoType->nSpecFuncVal1 == (INT)EISFC_All)
				{
					IMEffect.eEffect = EIME_ComAdvance;
					IMEffect.dwParam2 = pItemIM->pProtoType->nSpecFuncVal2;

					// 删除IM道具
					GetItemMgr().DelFromBag(n64IMID, (DWORD)ELCID_Compose_Engrave, 1);
				}
				break;
			}

			if(pItemIM->pProtoType->eSpecFunc == EISF_ProtectSign)
			{
				IMEffect.eEffect = EIME_ProtectSign;
				// 删除IM道具
				GetItemMgr().DelFromBag(n64IMID, (DWORD)ELCID_Compose_Engrave, 1);
			}
		}
		break;
	case ECTS_Inlay:
		{

		}
		break;
	case ECTS_Brand:
		{
			if(pItemIM->pProtoType->eSpecFunc == EISF_ProtectSign)
			{
				IMEffect.eEffect = EIME_ProtectSign;
				// 删除IM道具
				GetItemMgr().DelFromBag(n64IMID, (DWORD)ELCID_Compose_Brand, 1);
			}
		}
		break;
	case ECTS_LoongSoul:
		{

		}
		break;
	case ECTS_Quench:
		{
			const tagQuenchProtoSer *pQuenchProto  = (tagQuenchProtoSer*)pProto;
			if(pItemIM->pProtoType->eSpecFunc == EISF_ComposeAdvance)
			{
				if(pItemIM->pProtoType->nSpecFuncVal1 == (INT)EISFC_Quench 
					|| pItemIM->pProtoType->nSpecFuncVal1 == (INT)EISFC_AllConsolidate
					|| pItemIM->pProtoType->nSpecFuncVal1 == (INT)EISFC_All)
				{
					IMEffect.eEffect = EIME_ComAdvance;
					IMEffect.dwParam2 = pItemIM->pProtoType->nSpecFuncVal2;

					// 删除IM道具
					GetItemMgr().DelFromBag(n64IMID, (DWORD)ELCID_Compose_Quench, 1);
				}
				break;
			}
		}
		break;
	case ECTS_Produce:
		{
			const tagProduceProtoSer*  pProduceProto = (tagProduceProtoSer*)pProto;
			if(pItemIM->pProtoType->eSpecFunc == EISF_ComposeAdvance)
			{
				if(pItemIM->pProtoType->nSpecFuncVal1 == (INT)pProduceProto->eComType 
					|| pItemIM->pProtoType->nSpecFuncVal1 == (INT)ESFCA_AllProduce
					|| pItemIM->pProtoType->nSpecFuncVal1 == (INT)EISFC_All)
				{
					IMEffect.eEffect = EIME_ComAdvance;
					IMEffect.dwParam2 = pItemIM->pProtoType->nSpecFuncVal2;

					// 删除IM道具
					GetItemMgr().DelFromBag(n64IMID, (DWORD)ELCID_Compose_Produce, 1);
				}
				break;
			}
			else if (pItemIM->pProtoType->eSpecFunc == EISF_ColorProbability)
			{
				IMEffect.eEffect = EIME_Color;
				IMEffect.dwParam1 = pItemIM->pProtoType->nSpecFuncVal1;
				IMEffect.dwParam2 = pItemIM->pProtoType->nSpecFuncVal2;

				// 删除IM道具
				GetItemMgr().DelFromBag(n64IMID, (DWORD)ELCID_Compose_Produce, 1);
			}
		}
		break;
	case ECTS_Decompose:
		{

		}
		break;
	case ECTS_RaisePotVal:
		{
			if(pItemIM->pProtoType->eSpecFunc == EISF_ProtectSign/*EISF_EquipAddPotValue*/)
			{
				IMEffect.eEffect = EIME_ProtectSign;
				// 删除IM道具
			}
			GetItemMgr().DelFromBag(n64IMID, (DWORD)ELCID_Raise_PotVal, 1);
		}
		break;
	}
}


//---------------------------------------------------------------------------------
// 计算装备光晕
//---------------------------------------------------------------------------------
VOID Role::CalEquipFlare(tagEquip* pEquip)
{
	BYTE byFlareVal = 0;
	BYTE byHoleBNum = 0;
	// 计算镶嵌次数
	for(INT i = 0; i < (INT)pEquip->equipSpec.byHoleNum; ++i)
	{
		byHoleBNum++;
		if(pEquip->equipSpec.dwHoleGemID[i] == GT_INVALID || pEquip->equipSpec.dwHoleGemID[i] == 0)
		{
			break;
		}
	}

	// 光晕值　=  1×铭纹次数 + 1×镌刻次数 + 2×镶嵌次数 + 2×当前烙印等级 + 3×装备当前龙附等级
	byFlareVal = pEquip->equipSpec.byPosyTimes + pEquip->equipSpec.byEngraveTimes + byHoleBNum * 2 
				+ pEquip->equipSpec.byBrandLevel * 2 + pEquip->equipSpec.byLongfuLevel * 3;

	pEquip->SetFlareVal(byFlareVal);
}



DWORD Role::RaiseEquipPotVal(INT64 n64SrcItemID, INT64 n64DstItemID, INT64 n64IMID, DWORD & dwPotValAdded)
{
	dwPotValAdded = 0;
	// 获得地图
	Map *pMap = GetMap();
	if(!P_VALID(pMap))
	{
		ASSERT(P_VALID(pMap));
		return GT_INVALID;
	}

	// 找到待提升装备
	tagEquip *pEquip = (tagEquip*)GetItemMgr().GetBagItem(n64DstItemID);
	if(!P_VALID(pEquip))
		return E_Consolidate_Equip_Not_Exist;

	if(!MIsEquipment(pEquip->dwTypeID))
		return E_Consolidate_NotEquipment;

	// 找到潜力提升道具
	tagItem *pItemRaiser = GetItemMgr().GetBagItem(n64SrcItemID);
	if(!P_VALID(pItemRaiser) || pItemRaiser->pProtoType->eSpecFunc != EISF_EquipAddPotValue )
		return E_Compose_Stuff_Not_Exist/*E_Consolidate_Brand_Not_Exit*/;

	// 找到强化数据，这个不需要
	//const tagConsolidateItem *pConsolidateProto = g_attRes.GetConsolidateProto(pItemRaiser->dwTypeID);
	//if(!P_VALID(pConsolidateProto))
	//	return E_Compose_Formula_Not_Exist;

	// 检测可提升次数限制
	if( 0 == pEquip->equipSpec.nPotIncTimes )
		return E_Raise_PotVal_Times_Zero;

	// 检测潜力提升上限，不要用原型里面的最大值
	INT32 nMaxPotVal = pEquip->pEquipProto->nMaxPotVal/*pEquip->equipSpec.nPotVal + pEquip->equipSpec.nPotValUsed*/;
	if( pEquip->equipSpec.nPotVal >= nMaxPotVal/*pEquip->pEquipProto->nMaxPotVal*/ )
		return E_Raise_PotVal_Not_Needing;

	INT nPotDelta = pItemRaiser->pProtoType->nSpecFuncVal2;

	// 最终可提升装备多少潜力
	if((pEquip->equipSpec.nPotVal + nPotDelta ) > nMaxPotVal/*pEquip->pEquipProto->nMaxPotVal*/ )
		nPotDelta = nMaxPotVal/*pEquip->pEquipProto->nMaxPotVal*/ - pEquip->equipSpec.nPotVal;

	// 删除材料
	GetItemMgr().DelFromBag(n64SrcItemID, (DWORD)ELCID_Raise_PotVal, 1);

	// 计算成功率
	FLOAT fProp = /*(*/ pItemRaiser->pProtoType->nSpecFuncVal1 + (FLOAT)GetAttValue(ERA_Fortune) /*)*/ / 1000.0f;

	// 计算IM道具的影响
	tagIMEffect			IMEffect;
	//tagConsolidateItem *pProto = const_cast<tagConsolidateItem*>(pConsolidateProto);		
	CalIMEffect(ECTS_RaisePotVal, IMEffect, n64IMID, NULL);

	// 计算成功率
	INT rand_n = IUTIL->Rand();
	BOOL bResult = /*IUTIL->RandomInRange(0,100) <= fProp;*/rand_n  % 10000 <= fProp;

	if(bResult)		// 成功
	{
		// 递减提升潜力值次数
		--pEquip->equipSpec.nPotIncTimes;
		if( pEquip->equipSpec.nPotIncTimes < 0 )
			pEquip->equipSpec.nPotIncTimes = 0;

		// 计算是否完美（生产完美率=生产成功率×1/20+角色福缘/1000)
		FLOAT fPefectProp = 0.01 + (FLOAT)GetAttValue(ERA_Fortune) / 1000.0f;

		rand_n = IUTIL->Rand();
		BOOL bPefect = rand_n % 10000 <= (fPefectProp * 10000);

		if(bPefect)
		{
			dwPotValAdded = pEquip->equipSpec.nPotVal;
			pEquip->ChangePotVal( pEquip->equipSpec.nPotVal );
			GetItemMgr().UpdateEquipSpec(*pEquip);
			return E_Compose_Consolidate_Perfect;
		}
		else
		{
			dwPotValAdded = nPotDelta;
			pEquip->ChangePotVal( nPotDelta );
			GetItemMgr().UpdateEquipSpec(*pEquip);
			return E_Compose_Consolidate_Success;
		}
	}
	else
	{
		// 装备潜力值消耗100点
		if(IMEffect.eEffect !=  EIME_ProtectSign)
			pEquip->ChangePotVal( -100 );

		GetItemMgr().UpdateEquipSpec(*pEquip);
		return E_Compose_Consolidate_Lose;
	}

	return 0;
}
// Jason Gem removal info v1.3.1 2009-12-21
DWORD Role::GetGemRemovalInfo(DWORD dwNPCID,INT64 equipID,INT8 & num,DWORD dwGemIDs[MAX_EQUIPHOLE_NUM])
{
	// 获得地图
	Map *pMap = GetMap();
	if(!P_VALID(pMap))
	{
		ASSERT(P_VALID(pMap));
		return GT_INVALID;
	}

	Creature* pNPC = pMap->FindCreature(dwNPCID);
	if( !P_VALID(pNPC) ) 
		return E_Compose_NPC_Not_Exist;

	if( FALSE == pNPC->CheckNPCTalkDistance(this) )
		return E_Compose_NPC_Distance;

	if( FALSE == pNPC->IsFunctionNPC(EFNPCT_GemRemoval) )
		return E_Gem_NPCCanNotRemoveGem;

	tagEquip *pEquip = (tagEquip*)GetItemMgr().GetBagItem(equipID);
	if(!P_VALID(pEquip))
		return E_Consolidate_Equip_Not_Exist;

	num = pEquip->equipSpec.byHoleNum;
	if(num > MAX_EQUIPHOLE_NUM)
		num = MAX_EQUIPHOLE_NUM;
	else if( num < 0 )
		num = 0;

	memset(dwGemIDs,0,sizeof(dwGemIDs));

	for(int i = 0; i < num ; ++i)
		dwGemIDs[i] = pEquip->equipSpec.dwHoleGemID[i];

	return E_Success;
}

DWORD Role::RemoveGemFromEquip(DWORD dwNPCID,INT64 equipID,INT8 num,INT64 SignIDs[MAX_EQUIPHOLE_NUM])
{
	// 获得地图
	Map *pMap = GetMap();
	if(!P_VALID(pMap))
	{
		ASSERT(P_VALID(pMap));
		return GT_INVALID;
	}

	Creature* pNPC = pMap->FindCreature(dwNPCID);
	if( !P_VALID(pNPC) ) 
		return E_Compose_NPC_Not_Exist;

	if( FALSE == pNPC->CheckNPCTalkDistance(this) )
		return E_Compose_NPC_Distance;

	if( FALSE == pNPC->IsFunctionNPC(EFNPCT_GemRemoval) )
		return E_Gem_NPCCanNotRemoveGem;

	tagEquip *pEquip = (tagEquip*)GetItemMgr().GetBagItem(equipID);
	if(!P_VALID(pEquip))
		return E_Consolidate_Equip_Not_Exist;

	INT8 n8Num = pEquip->equipSpec.byHoleNum;


	tagItem* pItems[MAX_EQUIPHOLE_NUM] = {0};
	INT64 signs[MAX_EQUIPHOLE_NUM] = {0};
	DWORD gems[MAX_EQUIPHOLE_NUM] = {0};
	int count = 0;

	for(int i = 0,j = 0; i < n8Num ; ++i)
	{
		tagItemProto * pItemProto = g_attRes.GetItemProto( pEquip->equipSpec.dwHoleGemID[i] );
		tagItem *pItemGemSign = GetItemMgr().GetBagItem(SignIDs[i]);
		if( P_VALID(pItemProto) && P_VALID(pItemGemSign) && pItemGemSign->pProtoType->eSpecFunc == EISF_GemRemovalSign )
		{
			if( pItemProto->byQuality == pItemGemSign->pProtoType->byQuality )
			{
				signs[j] = SignIDs[i];
				gems[j++] = pItemProto->dwTypeID;
				++count;
			}
		}
	}

	if( count > 0 )
	{
		INT64 totalSilver = count * 10 * 10000;
		if( GetCurMgr  ().GetBagSilver() >= totalSilver )
		{
			GetCurMgr().DecBagSilver(totalSilver, ELCID_GemRemoval);
			GetItemMgr  ().DelFromBag  (equipID,ELCID_GemRemoval,1);
			for (int i = 0; i < count ; ++i)
			{
				pItems[i] = ItemCreator::Create  (EICM_GemRemoval,GetID  (),gems[i]);
				if( !P_VALID(pItems[i]) )
				{
				}
				//else
				{
					GetItemMgr  ().DelFromBag  (signs[i],ELCID_GemRemoval,1);
				}
			}
			GetItemMgr().Add2Bag  (pItems,count,ELCID_GemRemoval,TRUE);
		}
		else
			return E_Consolidate_NotEnough_Money;
	}


	return E_Success;
}


