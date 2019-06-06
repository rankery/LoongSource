//-------------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: skill_trigger.h
// author: Aslan
// actor:
// data: 2008-09-22
// last:
// brief: 技能触发器
//-------------------------------------------------------------------------------
#pragma once

#include "att_res.h"

//------------------------------------------------------------------------------
// 技能对触发器影响
//------------------------------------------------------------------------------
struct tagTriggerMod
{
	INT		nEventMisc1Mod;				// 事件触发值1加成
	INT		nEventMisc2Mod;				// 事件触发值2加成
	INT		nStateMisc1Mod;				// 状态触发值1加成
	INT		nStateMisc2Mod;				// 状态触发值2加成
	INT		nPropMod;					// 触发几率加成

	tagTriggerMod()
	{
		nEventMisc1Mod = 0;
		nEventMisc2Mod = 0;
		nStateMisc1Mod = 0;
		nStateMisc2Mod = 0;
		nPropMod = 0;
	}

	VOID SetMod(const tagSkillProto* pProto)
	{
		// 事件触发值1加成
		nEventMisc1Mod += pProto->nTriggerEventMisc1Add;
		// 事件触发值2加成
		nEventMisc2Mod += pProto->nTriggerEventMisc2Add;
		// 状态触发值1加成
		nStateMisc1Mod += pProto->nTriggerStateMisc1Add;
		// 状态触发值2加成
		nStateMisc2Mod += pProto->nTriggerStateMisc2Add;
		// 触发几率加成
		nPropMod += pProto->nTriggerPropAdd;
	}

	VOID UnSetMod(const tagSkillProto* pProto)
	{
		// 事件触发值1加成
		nEventMisc1Mod -= pProto->nTriggerEventMisc1Add;
		// 事件触发值2加成
		nEventMisc2Mod -= pProto->nTriggerEventMisc2Add;
		// 状态触发值1加成
		nStateMisc1Mod -= pProto->nTriggerStateMisc1Add;
		// 状态触发值2加成
		nStateMisc2Mod -= pProto->nTriggerStateMisc2Add;
		// 触发几率加成
		nPropMod -= pProto->nTriggerPropAdd;
	}
};