//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: trigger_define.h
// author: Aslan
// actor:
// data: 2008-09-08
// last:
// brief: 触发器静态属性
//-----------------------------------------------------------------------------
#pragma once

#pragma pack(push, 1)

//------------------------------------------------------------------------------
// 触发器事件类型
//------------------------------------------------------------------------------
enum ETriggerEventType
{
	ETEE_Null					=	0,
	ETEE_Use					=	1,		// 使用：物品使用，主动技能使用
	ETEE_Equip					=	2,		// 装备：装备装备时
	ETEE_BeAttacked				=	3,		// 被攻击：某些被动技能触发，装备buff触发，AI触发
	ETEE_Hit					=	4,		// 命中：主动技能命中目标时触发，被动技能和装备触发
	ETEE_Hited					=	5,		// 被命中：被动技能和装备触发，AI触发
	ETEE_Dodged					=	6,		// 被闪避：主动技能被目标闪避时触发，被动技能或装备被闪避时触发
	ETEE_Dodge					=	7,		// 闪避：闪避了攻击时触发
	ETEE_Blocked				=	8,		// 被格挡：主动技能被目标闪避时触发，被动技能或装备被闪避时触发
	ETEE_Block					=	9,		// 格挡：格挡了攻击时触发
	ETEE_Crit					=	10,		// 致命：主动技能产生暴击时触发，被动技能和装备触发
	ETEE_Crited					=	11,		// 被致命：被暴击时触发
	ETEE_Die					=	12,		// 死亡：死亡时触发，某些被动技能和装备会触发，AI触发
	ETEE_Random					=	13,		// 随机：被动技能或者装备触发，AI触发
	ETEE_InterCombat			=	14,		// 进入备战状态：这个是AI的
	ETEE_DetectTarget			=	19,		// 发现目标：AI的
	ETEE_InterCombatTime		=	20,		// 进入战斗：AI的
	ETEE_InterCombatInterTime	=	21,		// 进入战斗每隔一定时间：AI的  ?????
	ETEE_TeammateDie			=	22,		// 队友死亡：AI的
	ETEE_Attack					=	23,		// 攻击：某些被动技能触发，装备buff触发
	ETEE_Kill					=	24,		// 击杀：某些被动技能触发，装备buff触发
};

//------------------------------------------------------------------------------
// 触发器状态条件
//------------------------------------------------------------------------------
enum ETriggerStateType
{
	ETST_Null				=	0,
	ETST_HPLower			=	1,		// 体力低于
	ETST_HPHigher			=	2,		// 体力高于
	ETST_MPLower			=	3,		// 真气低于
	ETST_MPHigher			=	4,		// 真气高于
	ETST_RageLower			=	5,		// 怒气低于
	ETST_RageHigher			=	6,		// 怒气高于
	ETST_TeammateHPLower	=	7,		// 同队成员体力低于一定值
	ETST_TeammateMPLower	=	8,		// 同队成员真气低于一定值
	ETST_SelfHaveBuff		=	9,		// 自身拥有某状态
	ETST_TargetHaveBuff		=	10,		// 目标拥有某状态
	ETST_EnduranceLower		=	11,		// 持久力低于
	ETST_EnduranceHigher	=	12,		// 持久力高于
	ETST_Distance			=	13,		// 和攻击目标的距离是否在某一范围之内
};

enum ECreatureTargetFriendEnemy
{
	ECFE_NULL				=	0,		// 没有目标（群体技能）
	ECFE_Self				=	1,		// 自己
	ECFE_Leader				=	2,		// 队长

	ECFE_CurPlayer			=	3,		// 怪物的当前目标玩家
	ECFE_RandPlayer			=	4,		// 怪物仇恨列表内随机一个玩家
	ECFE_MinEnimty			=	5,		// 怪物仇恨列表内仇恨最小的玩家
};


//------------------------------------------------------------------------------
// 触发器静态属性
//------------------------------------------------------------------------------
struct tagTriggerProto
{
	DWORD				dwID;
	ETriggerEventType	eEventType;		// 事件条件类型
	DWORD				dwEventMisc1;	// 事件条件参数
	DWORD				dwEventMisc2;	// 事件条件参数
	INT					nEventProp;		// 事件触发几率
	ETriggerStateType	eStateType;		// 状态条件类型
	DWORD				dwStateMisc1;	// 状态条件参数
	DWORD				dwStateMisc2;	// 状态条件参数
};

#pragma pack(pop)