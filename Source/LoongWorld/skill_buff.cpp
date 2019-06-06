//-------------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: skill_buff.h
// author: Aslan
// actor:
// data: 2008-9-23
// last:
// brief: 技能
//-------------------------------------------------------------------------------
#include "stdafx.h"

#include "../WorldDefine/skill_define.h"
#include "../WorldDefine/buff_define.h"

#include "att_res.h"
#include "skill_buff.h"
#include "world.h"


//------------------------------------------------------------------------------------
// 设置加成
//------------------------------------------------------------------------------------
VOID tagBuffMod::SetMod(const tagSkillProto* pProto)
{
	if( FALSE == P_VALID(pProto) ) return;
	if( ESTT_Buff != pProto->eTargetType ) return;

	// 持续时间
	nPersistTickMod += pProto->nBuffPersistTimeAdd / TICK_TIME;

	// 叠加次数
	nWarpTimesMod += pProto->nBuffWarpTimesAdd;

	// 攻击消除几率
	nAttackInterruptRateMod += pProto->nBuffInterruptResistAdd;

	// 效果加成
	if( EBEM_Null != pProto->eModBuffMode )
	{
		// 之前没有过特殊效果加成
		if( EBEM_Null == eModBuffEffectMode )
		{
			eModBuffEffectMode = pProto->eModBuffMode;
			nEffectMisc1Mod = pProto->nBuffMisc1Add;
			nEffectMisc2Mod = pProto->nBuffMisc2Add;

			if( EBEM_Persist != pProto->eModBuffMode )
			{
				IFASTCODE->MemCpy(nEffectAttMod, pProto->nBuffEffectAttMod, sizeof(nEffectAttMod));
			}
		}
		// 之前有特殊效果加成，则新技能的特殊效果加成的阶段必须和其一样
		else if( eModBuffEffectMode == pProto->eModBuffMode )
		{
			nEffectMisc1Mod += pProto->nBuffMisc1Add;
			nEffectMisc2Mod += pProto->nBuffMisc2Add;

			if( EBEM_Persist != pProto->eModBuffMode )
			{
				for(INT n = 0; n < EBEA_End; ++n)
				{
					nEffectAttMod[n] += pProto->nBuffEffectAttMod[n];
				}
			}
		}
		// 新技能的特殊效果加成与之前的加成不一样，不会出现这种情况
		else
		{
			ILOG->Write(_T("skill mod buff failed, skill type id is %u\r\n"), pProto->dwID);
		}
	}

	// 人物属性加成
	ERoleAttribute eAtt = ERA_Null;
	INT nMod = 0;

	TMap<ERoleAttribute, INT>& mapMod = pProto->mapRoleAttMod;
	TMap<ERoleAttribute, INT>::TMapIterator itMod = mapMod.Begin();
	while( mapMod.PeekNext(itMod, eAtt, nMod) )
	{
		mapRoleAttMod.ModifyValue(eAtt, nMod);
	}

	TMap<ERoleAttribute, INT>& mapModPct = pProto->mapRoleAttModPct;
	TMap<ERoleAttribute, INT>::TMapIterator itModPct = mapModPct.Begin();
	while( mapModPct.PeekNext(itModPct, eAtt, nMod) )
	{
		mapRoleAttModPct.ModifyValue(eAtt, nMod);
	}

	// 将该技能TypeID存入本地list，以便保存
	listModifier.PushBack(pProto->dwID);

	// 设置该Mod本身有效
	bValid = TRUE;
}

//------------------------------------------------------------------------------------
// 取消加成
//------------------------------------------------------------------------------------
VOID tagBuffMod::UnSetMod(const tagSkillProto* pProto)
{
	if( FALSE == P_VALID(pProto) ) return;
	if( ESTT_Buff != pProto->eTargetType ) return;

	// 持续时间
	nPersistTickMod -= pProto->nBuffPersistTimeAdd / TICK_TIME;

	// 叠加次数
	nWarpTimesMod -= pProto->nBuffWarpTimesAdd;

	// 攻击消除几率
	nAttackInterruptRateMod -= pProto->nBuffInterruptResistAdd;

	// 效果加成
	if( EBEM_Null != pProto->eModBuffMode )
	{
		// 有特殊效果加成，则技能的特殊效果加成的阶段必须和其一样
		if( eModBuffEffectMode == pProto->eModBuffMode )
		{
			nEffectMisc1Mod -= pProto->nBuffMisc1Add;
			nEffectMisc2Mod -= pProto->nBuffMisc2Add;

			if( EBEM_Persist != pProto->eModBuffMode )
			{
				for(INT n = 0; n < EBEA_End; ++n)
				{
					nEffectAttMod[n] -= pProto->nBuffEffectAttMod[n];
				}
			}
		}
	}

	// 人物属性加成
	ERoleAttribute eAtt = ERA_Null;
	INT nMod = 0;

	TMap<ERoleAttribute, INT>& mapMod = pProto->mapRoleAttMod;
	TMap<ERoleAttribute, INT>::TMapIterator itMod = mapMod.Begin();
	while( mapMod.PeekNext(itMod, eAtt, nMod) )
	{
		mapRoleAttMod.ModifyValue(eAtt, -nMod);
	}

	TMap<ERoleAttribute, INT>& mapModPct = pProto->mapRoleAttModPct;
	TMap<ERoleAttribute, INT>::TMapIterator itModPct = mapModPct.Begin();
	while( mapModPct.PeekNext(itModPct, eAtt, nMod) )
	{
		mapRoleAttModPct.ModifyValue(eAtt, -nMod);
	}

	listModifier.Erase(const_cast<tagSkillProto*>(pProto)->dwID);
}
