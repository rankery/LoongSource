//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: item_define.h
// author: 
// actor:
// data: 2008-10-17
// last:
// brief: 服务器用物品相关结构
//-----------------------------------------------------------------------------
#pragma once

#pragma pack(push, 1)

//-----------------------------------------------------------------------------
// 容器update时间(单位：tick)
//-----------------------------------------------------------------------------
const INT ITEM_UPDATE_TIME		= 60 * TICK_PER_SECOND;		// 60s

//-----------------------------------------------------------------------------
// 装备B类属性生成概率[0，10000]
//-----------------------------------------------------------------------------
const INT g_nEquipSpecAttPct[X_EQUIP_SPECATT_NUM] = 
{
	800,	400,	200,	800,	400,
	200,	800,	400,	200,	100,
	800,	400,	200,	100,	500,
	500,	500,	500,	500,	500,
	100,	500,	500,	100
};

//-----------------------------------------------------------------------------
// 同一容器中物品移动需由方法传出的参数
//-----------------------------------------------------------------------------
struct tagItemMove
{
	tagItem	*pItem2;		// 目标位置上的物品
	INT16	n16NumRes1;		// 待移动物品的最终个数
	INT16	n16NumRes2;		// 目标位置上物品的最终个数
	bool	bCreateItem;	// 目标位置上的物品是否为新创建的(拆分时为true)
	bool	bOverlap;		// 是否堆叠
	bool	bChangePos;		// 位置是否移动

	tagItemMove()
	{
		Init();
	}
	
	VOID Init()
	{
		pItem2		= NULL;
		n16NumRes1	= 0;
		n16NumRes2	= 0;
		bCreateItem = FALSE;
		bOverlap	= FALSE;
		bChangePos	= TRUE;
	}
};

//-----------------------------------------------------------------------------
// 保存角色数据时需更新的物品数据
//-----------------------------------------------------------------------------
struct tagItemUpdate
{
	INT64		n64Serial;
	DWORD		dwOwnerID;
	DWORD		dwAccountID;
	INT32		nUseTimes;		// 物品上技能使用次数
	INT16		n16Num;			// 物品个数
	INT16		n16Index;		// 容器中位置
	BYTE		byConType;		// 容器类型
	BYTE		byBindType;		// 绑定类型
};

struct tagEquipSpecUpdate
{
	INT64			n64Serial;
	tagEquipSpec	equipSpec;
};

//-----------------------------------------------------------------------------
// IM 物品影响属性枚举
//-----------------------------------------------------------------------------
enum EIMEffect
{
	EIME_Null			= 0,

	EIME_Color			= 1,	// 颜色
	EIME_ComAdvance		= 2,	// 提高成功几率
	EIME_ProtectSign	= 3,	// 保底符（强化失败时不消耗装备潜力值）
};

//-----------------------------------------------------------------------------
// IM 物品影响
//-----------------------------------------------------------------------------
struct tagIMEffect
{
	EIMEffect	eEffect;	// 影响类别
	DWORD		dwParam1;	// 影响值
	DWORD		dwParam2;
	tagIMEffect()
	{
		eEffect = EIME_Null;
		dwParam1 = 0;
		dwParam2 = 0;
	}
};

//-----------------------------------------------------------------------------
// 物品冷却信息
//-----------------------------------------------------------------------------
struct tagCDInfo
{
	DWORD		dwElapseTime;
	DWORD		dwRemainTime;
};


// Jason 2009-12-8 点金石配置结构
struct tagPhilosophersStoneConfig
{
	union unionKey
	{
		struct tagPair
		{
			INT nStoneType;
			DWORD dwSrcItemTypeID;
		} m_Pair;
		INT64 m_n64Key;
	} uniKey;
	DWORD dwDestItemTypeID;
};

//-----------------------------------------------------------------------------
// 获取物品数据的结构
//-----------------------------------------------------------------------------
struct tagItemData
{
	INT64	n64Serial;
	DWORD	dwTypeID;
};

#pragma pack(pop)
