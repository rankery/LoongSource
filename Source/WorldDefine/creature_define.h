//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: creature_define.h
// author: Aslan
// actor:
// data: 2008-08-25
// last:
// brief: 怪物属性定义
//-----------------------------------------------------------------------------
#pragma once

#include "reputation.h"
#include "RoleDefine.h"

enum ENPCTeamOrder;

//----------------------------------------------------------------
// 游戏内生物种类
//----------------------------------------------------------------
enum ECreatureType
{
	ECT_NULL		=	0,
	ECT_Monster		=	1,		// 怪物
	ECT_GameObject	=	2,		// 可交互的地物
	ECT_NPC			=	3,		// 普通NPC
	ECT_Pet			=	4,		// 宠物
};

//----------------------------------------------------------------
// 动画类型
//----------------------------------------------------------------
enum EAniType
{
	EAT_None		=	0,		// 静态
	EAT_KeyFrame	=	1,		// 关键帧
	EAT_Skeleton				// 骨骼动画
};

//----------------------------------------------------------------
// 怪物种类
//----------------------------------------------------------------
enum EMonsterType
{
	EMTT_Normal		=	1,		// 普通
	EMTT_Nest		=	2,		// 巢穴
	EMTT_Team		=	10,		// 怪物小队队长
};

//----------------------------------------------------------------
// 普通NPC种类
//----------------------------------------------------------------
enum ENPCType
{
	ENPCT_Normal	=	0,		// 普通NPC
};

//-----------------------------------------------------------------
// 可交互地物种类
//-----------------------------------------------------------------
enum EGameObjectType
{
	EGOT_Normal		=	0,		// 普通地物
	EGOT_Gather		=	3,		// 可采集地物
	EGOT_Chest		=	4,		// 箱子
	EGOT_Flag		=	5,		// 阵旗
	EGOT_Door		=	6,		// 门
	EGOT_QuestInves =   7,      // 任务可调查地物
	EGOT_QuestTalk  =   8,      // 任务对话地物
	EGOT_CommonInves=	9,		// 非任务调查地物
};

//------------------------------------------------------------------
// 宠物种类
//------------------------------------------------------------------
enum EPetType
{
	EPT_Normal		=	0,		// 普通宠物
};

//------------------------------------------------------------------
// 职能类型
//------------------------------------------------------------------
enum EFunctionNPCType
{
	EFNPCT_Null		=	-1,		// 无
	EFNPCT_Shop		=	0,		// 商店
	EFNPCT_Dak		=	1,		// 驿站
	EFNPCT_Ware		=	2,		// 仓库
	EFNPCT_Compose	=	3,		// 合成
	EFNPCT_Guild	=	4,		// 门派
	EFNPCT_Guard	=	5,		// 卫兵
	EFNPCT_Yamun	=	6,		// 衙门
	EFNPCT_Quench	=	7,		// 淬火
	EFNPCT_Engrave	=	8,		// 镌刻
	EFNPCT_Posy		=	9,		// 铭文
	EFNPCT_Teacher	=	10,		// 技能授予者
	EFNPCT_Revive	=	11,		// 复活点
	EFNPCT_GuildMgr	=	12,		// 帮派管事
	EFNPCT_GuildWare=	13,		// 帮派仓库
	EFNPCT_CofC		=	14,		// 商会
	EFNPCT_Class	=	15,		// 职业

	// Jason v1.3.1 2009-12-21 宝石拆除npc类型
	EFNPCT_GemRemoval	=	16,	// 宝石拆除npc
};

enum EAIACTION
{
	AIAT_Attack		=	0,									// 攻击型
	AIAT_Guard		=	1,									// 防守型
	AIAT_Barbette	=	2,									// 炮塔型
	AIAT_Lamster	=	3,									// 逃跑型
	AIAT_CallHelp	=	4,									// 呼救型
	AIAT_SpaceOut	=	5,									// 风筝型
};

//------------------------------------------------------------------
// 逃跑怪物AI类型的中间状态
//------------------------------------------------------------------
enum EFLEETIME
{
	EFT_NotEnter	=	0,									// 没有进入过逃跑状态
	EFT_FirstEnter	=	1,									// 第一次进入逃跑状态
	EFT_Enter		=	2,									// 进入逃跑状态
	EFT_CallHelp	=	3,									// 呼救状态
};

//------------------------------------------------------------------
// 职能ID
//------------------------------------------------------------------
typedef union
{
	DWORD	dwCommonID;					// 读取静态属性表时，使用该字段
	DWORD	dwShopID;					// 商店NPC所挂的商店ID
	INT		nProduceType;				// 合成NPC所对应的合成类型
	DWORD	dwDakID;					// 驿站NPC所挂的ID
} FunctionID;


//------------------------------------------------------------------
// 可采集地物类型
//------------------------------------------------------------------
enum EGatherType
{
	EGT_Mine		=	1,		// 矿物
	EGT_Herb		=	2,		// 草药

	EGT_Food		=	3,		// 食材
	EGT_Cloth		=	4,		// 布料
};

//------------------------------------------------------------------
// 怪物级别
//------------------------------------------------------------------
enum ERank
{
	ER_Null,			// 无
	ER_Normal,			// 普通
	ER_Elite,			// 精英
	ER_Rare,			// 稀有
	ER_Quest,			// 任务
};

//-------------------------------------------------------------------
// 巡逻类型
//-------------------------------------------------------------------
enum ECreaturePatrolType
{
	ECPT_Null = 0,
	ECPT_Stand,			// 站立
	ECPT_Range,			// 自由
	ECPT_Path,			// 路径
	ECPT_Wood,			// 木桩
};

//-------------------------------------------------------------------
// 追击类型
//-------------------------------------------------------------------
enum ECreaturePursueType
{
	ECPST_Null = 0,
	ECPST_Normal,		// 普通追击
	ECPST_Guard,		// 炮台
};

//-------------------------------------------------------------------
// 重生类型
//-------------------------------------------------------------------
enum ECreatureRespawnType
{
	ECRT_Normal,			// 普通重生方式
};

//-------------------------------------------------------------------
// 怪物攻击所属类型（经验，声望）
//-------------------------------------------------------------------
enum ECreatureTaggedType
{
	ECTT_NULL = 0,
	ECTT_Hit,					// 命中
	ECTT_Kill,					// 击杀
	ECTT_All,					// 全体	
};

//-------------------------------------------------------------------
// 怪物AI生成类型
//-------------------------------------------------------------------
enum EAICreateType
{
	EACT_Null			=		-1,			// 无AI
	EACT_NoAI			=		0,			// 无AI数据
	EACT_Indicate		=		1,			// 指定
	EACT_Random			=		2,			// 完全随机
	EACT_GroupRandom	=		3,			// 在组ID范围内随机
};

//---------------------------------------------------------------------
// AI特殊行为
//---------------------------------------------------------------------
enum EAIFlag
{
	EAIF_Flee			=		1,			// 逃跑
	EAIF_Call			=		2,			// 召唤队友
};


// Jason 2009-11-26 怪物物品掉落衰减类型
enum ECreatureItemLootAttenuationType
{
	ECILAT_None		= 0,	// 不衰减
	ECILAT_Normal	= 1,	// 衰减
	ECILAT_Other	= 20
};

//---------------------------------------------------------------------
// 巢穴属性结构
//---------------------------------------------------------------------
const INT MAX_SPAWN_CREATURE_NUM = 10;		// 巢穴最大可刷新的怪物数量

struct tagNestProto
{
	ENPCTeamOrder		eOrderType;								// 小队队形
	FLOAT				fSpace;									// 怪物小队成员间的间距
	INT					nSpawnRadius;							// 刷新怪物的范围
	INT					nCreatureNum;							// 刷新的怪物数量
	DWORD				dwSpawnID[MAX_SPAWN_CREATURE_NUM];		// 巢穴刷新怪物的ID
	INT					nSpawnMax[MAX_SPAWN_CREATURE_NUM];		// 巢穴刷新每种怪物的最大数量
};

//-------------------------------------------------------------------
// 怪物，NPC，地物，宠物属性结构
//-------------------------------------------------------------------
struct tagCreatureProto
{
	// 基本
	DWORD					dwTypeID;					// 属性ID
	ECreatureType			eType;						// 怪物类别
	INT						nType2;						// 怪物子类别
	INT						nType3;						// 怪物类别3
	INT						nFamily;					// 怪物种类
	BYTE					bySex;						// 性别
	BYTE					byReserved[3];				// 保留字3字节
	ERank					eLite;						// 级别
	INT						nLevel;						// 等级
	Vector3					vSize;						// 包裹盒大小

	// 移动及战斗
	ECreaturePatrolType		ePatrolType;				// 移动类型
	INT						nPatrolRadius;				// 的移动半径
	ECreaturePursueType		ePursueType;				// 追击类型
	DWORD					dwNormalSkillIDMelee;		// 普通攻击技能ID
	DWORD					dwNormalSkillIDRanged;		// 远程攻击技能ID
	INT						nAttackInterTick;			// 怪物攻击间隔
	INT						nMaxPursueTick;				// 怪物攻击间隔
	BOOL					bCanAttack;					// 是否可攻击
	BOOL					bCanBeAttack;				// 是否可被攻击
	BOOL					bVisble;					// 是否可见
	BOOL					bCanHitFly;					// 是否可以被击飞

	// 刷新
	ECreatureRespawnType	eRespawnType;				// 重生模式
	INT						nRespawnTick;				// 重生时间（单位：tick）
	INT						nLiveTick;					// 生存时间

	// 掉落
	ECreatureTaggedType		eTaggedType;				// 所属权类型
	INT						nExpGive;					// 给予经验
	EReputationType			eRepType1;					// 所给声望1类型
	INT						nRepNum1;					// 所给声望1值
	EReputationType			eRepType2;					// 所给声望2类型
	INT						nRepNum2;					// 所给声望2值
	DWORD					dwLootID;					// 物品掉落编号

	// AI
	EAICreateType			eAICreateType;				// AI生成模式
	DWORD					dwAIID;						// AIID
	EAIACTION				eAIAction;					// AI行为方式

	// 职能
	EFunctionNPCType		eFunctionType;				// 职能类型
	FunctionID				uFunctionID;				// 职能ID

	// 属性
	INT						nBaseAtt[ERA_End];			// 基本属性

	// 任务
	UINT16					u16QuestID;					// 可调查地物所属的任务ID
	BYTE					byReserved2[2];				// 保留字

	// 巢穴指针
	tagNestProto*			pNest;						// 巢穴指针

	// 其他
	EAniType				eAnimationType;
	TCHAR					szModelName[X_LONG_NAME];	// 模型名称
	TCHAR					szName[X_SHORT_NAME];		// 怪物名称
	TCHAR					szTitle[X_SHORT_NAME];		// 称号
	TCHAR					szTitleIcon[X_SHORT_NAME];	// 称号贴图
	TCHAR					szEffect[X_SHORT_NAME];		// 特效编号
	TCHAR					szIdleSpeech[X_LONG_NAME];	// 悠闲喊话
	TCHAR					szCombatSpeech[X_LONG_NAME];// 战斗喊话
	TCHAR					szSkillSpeech[X_LONG_NAME];	// 技能喊话
	TCHAR					szDeathSpeech[X_LONG_NAME];	// 死亡喊话
	TCHAR					szMonsterAI[X_SHORT_NAME];	// 怪物AI

	// Jason 2009-11-26 怪物死亡物品掉落衰减
	ECreatureItemLootAttenuationType eItemLootAttenuType;// 物品掉落衰减类型

	tagCreatureProto()
	{
		pNest = NULL;
	}

	~tagCreatureProto()
	{
		if( pNest )
			delete pNest;
	}

};

//---------------------------------------------------------------------
// 怪物的AI结构
//---------------------------------------------------------------------
const INT X_MAX_CREATURE_AI_BUFF	=	3;	// 怪物AI所带的最多Buff数量
const INT X_MAX_CREATURE_AI_SKILL	=	5;	// 怪物AI所带的最多技能数量

struct tagCreatureAI
{
	DWORD		dwID;										// AI ID;
	EAIACTION	eAIAction;									// AI行为类型
	DWORD		dwGroupID;									// AI组ID
	DWORD		dwBuffTypeID[X_MAX_CREATURE_AI_BUFF];		// 初始的3个buff ID
	DWORD		dwTriggerID[X_MAX_CREATURE_AI_SKILL];		// 5个触发器
	DWORD		dwSkillTypeID[X_MAX_CREATURE_AI_SKILL];		// 5个技能ID
	INT			nTargetType[X_MAX_CREATURE_AI_SKILL];		// 技能对应的目标类型
};

const INT	X_MAX_FLEE_RADIUS			=	2000;				// 怪物逃跑最大半径
const INT	X_MIN_FLEE_RADIUS			=	1500;				// 怪物逃跑最小半径
const INT	X_RANGE_FLEE_RADIUS			=	X_MAX_FLEE_RADIUS - X_MIN_FLEE_RADIUS;			// 逃跑半径差值
const FLOAT X_MAX_SOS_RADIUS			=	2500;				// 呼救状态的最大逃跑距离
const FLOAT	X_MAX_SOS_RADIUS_SQ			=	X_MAX_SOS_RADIUS * X_MAX_SOS_RADIUS;
	

