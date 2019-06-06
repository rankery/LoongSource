//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: buff_define.h
// author: Aslan
// actor:
// data: 2008-09-08
// last:
// brief: 状态静态属性
//-----------------------------------------------------------------------------
#pragma once

#include "RoleDefine.h"

#pragma pack(push, 1)

//-------------------------------------------------------------------------
// 常量
//-------------------------------------------------------------------------
const DWORD MAX_BUFF_PERSIST_TICK	= 7 * 24 * 60 * 60 * TICK_PER_SECOND;	// Buff最长持续时间(单位：tick)

//-------------------------------------------------------------------------
// Buff类型
//-------------------------------------------------------------------------
enum EBuffType
{
	EBT_Null	=	0,
	EBT_Skill	=	1,		// 技能状态
	EBT_Item	=	2,		// 物品状态
	EBT_Action	=	3,		// 行为状态
	EBT_Other	=	4,		// 其他状态
};

//--------------------------------------------------------------------------
// Buff抗性类型
//--------------------------------------------------------------------------
enum EBuffResistType
{
	EBRT_Null		=	0,
	EBRT_Bleeding	=	1,	// 失血
	EBRT_Weak		=	2,	// 衰弱
	EBRT_Choas		=	3,	// 混乱
	EBRT_Special	=	4,	// 特殊
	EBRT_Regain		=	5,	// 恢复
};

//---------------------------------------------------------------------------
// Buff作用类型
//---------------------------------------------------------------------------
enum EBuffOPType
{
	EBOPT_Null		=	0,	// 无
	EBOPT_Rect		=	1,	// 矩形
	EBOPT_Explode	=	2,	// 爆炸
};

//----------------------------------------------------------------------------
// Buff打断枚举
//----------------------------------------------------------------------------
enum EBuffInterruptFlag
{
	EBIF_Move			=	0x00000001,		// 移动
	EBIF_InterCombat	=	0x00000002,		// 进入备战
	EBIF_Manual			=	0x00000004,		// 手动
	EBIF_BeAttacked		=	0x00000008,		// 被攻击
	EBIF_Die			=	0x00000010,		// 死亡
	EBIF_ChangeMap		=	0x00000020,		// 切换场景
	EBIF_BuffFull		=	0x00000040,		// 状态满
	EBIF_OffLine		=	0x00000080,		// 下线
	EBIF_HPLower		=	0x00000100,		// 体力低于
	EBIF_MPLower		=	0x00000200,		// 真气低于
	EBIF_RageLower		=	0x00000400,		// 怒气低于
	EBIF_EnduranceLower	=	0x00000800,		// 持久力低于
	EBIF_VitalityLower	=	0x00001000,		// 活力低于
};

//-----------------------------------------------------------------------------
// Buff效果类型
//-----------------------------------------------------------------------------
enum EBuffEffectType
{
	EBET_Null			=	0,		// 无
	EBET_Teleport		=	1,		// 瞬间移动
	EBET_InterruptSpell	=	2,		// 打断起手
	EBET_Dizzy			=	3,		// 眩晕
	EBET_Repel			=	4,		// 击退
	EBET_Assault		=	5,		// 冲锋
	EBET_NoSkill		=	6,		// 封技
	EBET_Spor			=	7,		// 昏睡
	EBET_Tie			=	8,		// 定身
	EBET_Dispel			=	9,		// 驱散
	EBET_Invincible		=	10,		// 无敌
	EBET_InstantDie		=	11,		// 即死
	EBET_Pull			=	12,		// 拖拽
	EBET_ReboundDmg		=	13,		// 反弹伤害
	EBET_AbsorbDmg		=	14,		// 吸收伤害
	EBET_TransmitDmg	=	15,		// 转移伤害
	EBET_HPDrain		=	16,		// 吸血
	EBET_MPDrain		=	17,		// 抽蓝
	EBET_HPTransfer		=	18,		// 转魂
	EBET_MPTransfer		=	19,		// 转精
	EBET_Revive			=	20,		// 复活
	EBET_InstantCD		=	21,		// 瞬间冷却
	EBET_Lurk			=	22,		// 隐身
	EBET_Fly			=	23,		// 飞行
	EBET_ChangeEnmity	=	24,		// 改变仇恨
	EBET_Transmit		=	25,		// 传送
	EBET_Gather			=	26,		// 采集
	EBET_DisArm			=	27,		// 缴械
	EBET_ExchangePos	=	28,		// 交换位置
	EBET_Explode		=	29,		// 引爆
	EBET_Funk			=	30,		// 恐惧
	EBET_Pursue			=	31,		// 追踪
	EBET_NoPrepare		=	32,		// 无起手时间
	EBET_OnWater		=	33,		// 水上行走
	EBET_MoveHPDmg		=	34,		// 移动掉血
	EBET_IgnoreArmor	=	35,		// 无视护甲
	EBET_Sneer			=	36,		// 嘲讽
	EBET_Cancel			=	37,		// 去除
	EBET_Immunity		=	38,		// 免疫
	EBET_Mount			=	39,		// 骑乘
	EBET_MountInvite	=	40,		// 邀请骑乘
	EBET_End			,
};

//---------------------------------------------------------------------------------
// Buff效果的阶段枚举
//---------------------------------------------------------------------------------
enum EBuffEffectMode
{
	EBEM_Null		=	-1,		// 无
	EBEM_Persist	=	0,		// 持续效果
	EBEM_Instant	=	1,		// 瞬间效果
	EBEM_Inter		=	2,		// 间隔效果
	EBEM_Finish		=	3,		// 结束效果
	EBEM_End		=	4,
};

//---------------------------------------------------------------------------------
// Buff的瞬时阶段，间隔阶段，结束阶段对人物的当前属性影响的枚举
//---------------------------------------------------------------------------------
enum EBuffEffectAtt
{
	EBEA_HP			=	0,		// 血量
	EBEA_MP			=	1,		// 真气
	EBEA_Rage		=	2,		// 怒气
	EBEA_Vitality	=	3,		// 活力
	EBEA_Endurance	=	4,		// 持久力
	EBEA_Morale		=	5,		// 士气
	EBEA_Injury		=	6,		// 内伤
	EBEA_End		=	7,
};

//---------------------------------------------------------------------------------
// Buff的敌我判断枚举
//---------------------------------------------------------------------------------
enum EBuffFriendEnemy
{
	EBFE_Friendly		=	0,		// 友方
	EBFE_Hostile		=	1,		// 敌方
	EBFE_All			=	2,		// 全体
	EBFE_Independent	=	3,		// 中立
};

//---------------------------------------------------------------------------------
// 状态静态属性结构
//---------------------------------------------------------------------------------
struct tagBuffProto
{
	DWORD				dwID;
	EBuffType			eType;						// 类型
	INT					nType2;						// 子类型
	INT					nType3;						// 子类型

	EBuffResistType		eResistType;				// 抗性类型

	BOOL				bBenifit;					// 是否有益
	EBuffFriendEnemy	eFriendly;					// 是否作用于友方，TRUE为友方，FALSE为敌方
	BOOL				bInstant;					// 是否瞬时buff

	DWORD				dwGroupFlag;				// 状态类型标记

	DWORD				dwTargetAddLimit;			// 添加对象限制
	DWORD				dwTargetLimit;				// 作用对象限制

	DWORD				dwTargetAddStateLimit;		// 添加目标状态限制
	DWORD				dwTargetStateLimit;			// 作用目标状态限制

	INT					nLevel;						// 等级

	EBuffOPType			eOPType;					// 作用类型
	FLOAT				fOPDistance;				// 作用距离
	FLOAT				fOPRadius;					// 作用范围

	INT					nPersistTick;				// 持续时间（单位：tick）
	INT					nInterOPTick;				// 间隔作用时间（单位：tick）
	INT					nWarpTimes;					// 叠加上限
	BOOL				bOfflineConsume;			// 离线计时

	DWORD				dwInterruptFlag;			// 消除标志（移动，进入备战，手动，攻击，死亡，切换场景，
													// 状态满，下线，体力低于，真气低于，持久力低于，怒气低于，活力低于，某状态消失）

	INT					nAttackInterruptRate;		// 攻击消除几率
	INT					nHPInterruptLimit;			// 体力消失值
	INT					nMPInterruptLimit;			// 真气消失值
	INT					nRageInterruptLimit;		// 怒气消失值
	INT					nEnduranceInterruptLimit;	// 持久力消失值
	INT					nVitalityInterruptLimit;	// 活力消失值
	INT					dwBuffInterruptID;			// 伴随消失BuffID

	DWORD				dwOPTrigger;				// 状态作用触发器

	EBuffEffectType		eEffect[EBEM_End];			// 效果类型
	DWORD				dwEffectMisc1[EBEM_End];	// 效果参数1
	DWORD				dwEffectMisc2[EBEM_End];	// 效果参数2

	INT					nInstantAttMod[EBEA_End];	// 瞬时阶段属性影响
	INT					nInterAttMod[EBEA_End];		// 间隔阶段属性影响
	INT					nFinishAttMod[EBEA_End];	// 结束阶段属性影响

	mutable TMap<ERoleAttribute, INT> mapRoleAttMod;	// 玩家属性影响（用于持续性影响）
	mutable TMap<ERoleAttribute, INT> mapRoleAttModPct;	// 玩家属性百分比影响（用于持续性影响）

	// 未加入的属性：名称，描述，动画标号，特效编号，图标编号，附属模型编号，自身模型编号，倒计时文字
};

struct tagBuffProtoClient : public tagBuffProto
{
	TCHAR				szName[X_SHORT_NAME];		// 名称
	TCHAR				szDesc[X_HUGE_NAME];		// 描述
	TCHAR				szIcon[X_SHORT_NAME];		// 图标编号
	TCHAR				szActionMod[X_SHORT_NAME];	// 动画编号
	TCHAR				szEffectMod[X_SHORT_NAME];	// 特效编号
	TCHAR				szAdjMod[X_SHORT_NAME];		// 附属模型编号
	TCHAR				szSelfMod[X_SHORT_NAME];	// 自身模型编号
	TCHAR				szCountDown[X_SHORT_NAME];	// 倒计时文字
};

//---------------------------------------------------------------------------
// Buff消息结构
//---------------------------------------------------------------------------
struct tagBuffMsgInfo
{
	DWORD				dwBuffTypeID;				// Buff属性ID
	INT					nPersistTimeLeft;			// Buff消失时间倒计时（毫秒）
	INT					nMaxPersistTime;			// Buff总持续时间（毫秒）
	INT					nWarpTimes;					// 当前叠加次数
};

// 9999601	御兽  驾御状态
CONST DWORD MOUNT_BUFF_ID	= 9999601;

// 9999701	同骑  被邀请状态
CONST DWORD MOUNT2_BUFF_ID	= 9999701;

// 9999501	摆摊
CONST DWORD STALL_BUFF_ID	= 9999501;


#pragma pack(pop)