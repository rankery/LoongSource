//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: pk_define.h
// author: Aslan
// actor:
// data: 2008-11-11
// last:
// brief: PK相关参数设定
//-----------------------------------------------------------------------------
#pragma once

#include "RoleDefine.h"

#pragma pack(push, 1)

const INT MAX_HOSTILITY							=	9;							// 玩家最大戾气值
const INT DEAD_PENALTY_LEVEL					=	20;							// 玩家死亡惩罚的等级下限
const INT SAFE_GUARD_FORCE_LEVEL				=	30;							// 必须处在PK保护的等级上限
const INT SAFE_GUARD_FREE_LEVEL					=	120;						// 可自由切换PK保护的等级上限
const INT OPEN_SAFE_GUARD_TIME					=	120*60;						// 由上次切换到自由模式到再次开启PK保护所需要等待的时间（单位：秒）

const INT UNSET_PK_TICK							=	120*TICK_PER_SECOND;		// 开启行凶状态后，需要多长时间恢复到未行凶状态（单位：秒）
const INT UNSET_PVP_TICK						=	30*TICK_PER_SECOND;			// PVP状态下，需要多长时间恢复到未PVP状态（单位：秒）
const INT REVIVE_UNSETSAFEGUARD_TICK			=	10*60*TICK_PER_SECOND;		// 戾气状态下玩家死亡后若戾气归0，则回城复活时开启pk保护状态的时间（单位：秒）

const INT PK_STATE_MORALITY[ERPKS_End]			=	{-30, -100, -1000, -10000};	// 各个阶段的PK状态对应的道德值

const INT KILL_MORALITY_DEC[ERPKS_End]			=	{-10, -5, -1, 0};			// 击杀各种PK状态玩家对应的道德值减少

const INT LOOT_RATE_PER_PK_STATE[ERPKS_End]		=	{0, 3000, 7000, 10000};		// 不同PK状态下玩家死亡掉落物品或装备的几率

// 不同戾气值下给玩家添加的惩罚buff id
const INT HOSTILITY_DEADPENALTY_BUFF_ID[MAX_HOSTILITY]		=	{2022301, 2022401, 2022501, 2022601, 2022701, 2022801, 2022901, 2023001, 2023101};					

#pragma pack(pop)