//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: stall_define.h
// author: 
// actor:
// data: 2008-12-17
// last:
// brief: 摆摊
//-----------------------------------------------------------------------------
#pragma once

#include "currency_define.h"

#pragma pack(push, 1)
//-----------------------------------------------------------------------------
// 常量
//-----------------------------------------------------------------------------
const INT STALL_MIN_ROLE_LEVEL			= 1;				// 最低摆摊等级
const INT STALL_MAX_DISPLAY				= 20;				// 摊位格子数

const INT64 STALL_MIN_SOLD_SLIVER		= 1;
const INT64 STALL_MAX_SOLD_SLIVER		= MGold2Silver(999999) + 9999;

const INT STALL_MAX_TITLE_NUM			= 10 + 1;			// 摊位标题字数(字符数+1个字符串结束符)
const INT STALL_MAX_ADTEXT_NUM			= 20 + 1;			// 摊位广告字数

const INT STALL_MAX_TAX					= MGold2Silver(8000);// 一个摊位的税收最大值

const INT VIP_STALL_MAX_NUM				= 40;				// VIP摊位最大数量

//-------------------------------------------------------------------------------
// 错误码
//-------------------------------------------------------------------------------
enum
{
	E_Stall_ItemNotFound_InBag		= 1,	// 没有从背包中找到物品
	E_Stall_ItemCanNot_Sold			= 2,	// 指定物品不可摆摊出售

	E_Stall_Role_InStall			= 5,	// 角色已处于摆摊装备
	E_Stall_Role_Level_2Low			= 6,	// 角色等级未达到30级
	E_Stall_Area_NotAllowed			= 7,	// 所在场景不允许摆摊
	E_Stall_Space_NotEnough			= 8,	// 角色4个格子之内有摊位
	E_Stall_RoleState_NotPermit		= 9,	// 处于战姿、行凶和PVP状态不可摆摊

	E_Stall_Role_Pull				= 20,	// 已收摊
	E_Stall_Goods_BeSold			= 21,	// 已卖出
	E_Stall_Goods_Refresh			= 22,	// 待售物品已更新
	E_Stall_Goods_Refresh_Price		= 23,	// 售价已更新
	E_Stall_GoodsNum_NotEnough		= 24,	// 待售物品个数不够
	E_Stall_Goods_Empty				= 25,	// 没有待售商品
	E_Stall_Goods_InUsed			= 26,	// 商品已处于使用状态
	E_Stall_Goods_NotInBag			= 27,	// 没有在背包中找到指定物品(该物品被使用，丢弃或移动等)

	E_Stall_CustomerMoney_NotEnough	= 30,	// 购买者金钱不够
	E_Stall_CustomerBug_NoFreeSpace	= 31,	// 购买者背包空间不足

	E_Stall_VIP_Not_Init			= 40,	// 摊位没有初始化
	E_Stall_VIP_Own_Another			= 41,	// 已经租赁了其他摊位
	E_Stall_VIP_Info_NotExist		= 42,	// 摊位信息不存在
	E_Stall_VIP_Already_Rent		= 43,	// 该摊位已经被租赁
	E_Stall_VIP_Rent_Error			= 44,	// 租金错误
	E_Stall_VIP_For_Hire			= 45,	// 摊位在待租状态
	E_Stall_VIP_GetGoods_Needless	= 46,	// 商品信息不需要更新
	E_Stall_VIP_Self_Stall			= 47,	// 不可操作自己的摊位

//	E_Stall_Cannot_Find_StallRole	= 50,	// 找不到摊主
};

//-----------------------------------------------------------------------------
// 摊位商品结构
//-----------------------------------------------------------------------------
struct tagMsgStallGoods
{
	INT64	n64UnitPrice;		// 单价
	BYTE	byIndex;			// 所在格子索引(从0开始)
	BYTE	byItem;				// 1表示物品，0表示装备
	BYTE	byData[1];			// 物品或装备数据
};

//-----------------------------------------------------------------------------
// 摊位模型枚举
//-----------------------------------------------------------------------------
enum EStallMode
{
	ESM_Wood		= 0,	// 木招牌
	ESM_Copper		= 1,	// 铜招牌
	ESM_Silver		= 2,	// 银招牌
	ESM_Gold		= 3,	// 黄金招牌
	ESM_Platinum	= 4,	// 白金招牌
	ESM_Diamond		= 5,	// 钻石招牌
};

//-----------------------------------------------------------------------------
// VIP摊位状态
//-----------------------------------------------------------------------------
enum EVIPStallStatus
{
	EVSS_Open,				// 营业中
	EVSS_Close,				// 收摊
	EVSS_ForHire,			// 待租

	EVSS_End
};

//-----------------------------------------------------------------------------
// VIP摊位结构
//-----------------------------------------------------------------------------
struct tagVIPStall 
{
	EVIPStallStatus	eStallStatus;						// 摊位状态
	DWORD			dwLastGoodsTime;					// 商品最后更新时间
	DWORD			dwLastUpdateTime;					// 摊位信息更新时间
	DWORD			dwOwnerID;							// 摊主ID
	INT32			nRemainTime;						// 摊位租赁剩余时间
	INT32			nRent;								// 租金
	BYTE			byStallIndex;						// 摊位序号
	TCHAR			szStallTitle[STALL_MAX_TITLE_NUM];	// 摊位标题

	VOID Reset()
	{
		eStallStatus		= EVSS_ForHire;
		dwLastGoodsTime		= GT_INVALID;
		dwLastUpdateTime	= GT_INVALID;
		dwOwnerID			= GT_INVALID;
		nRemainTime			= GT_INVALID;
		ZeroMemory(szStallTitle, sizeof(TCHAR)*STALL_MAX_TITLE_NUM);
	}
};

//-----------------------------------------------------------------------------
// 宏
//-----------------------------------------------------------------------------

// 根据缴税总额计算摊位招牌模型等级和税收减免
#define MCalStallMode(byLevel, fDecTax, nTotalTax)		\
	if(nTotalTax >= STALL_MAX_TAX)				\
	{											\
		byLevel = ESM_Diamond;					\
		fDecTax = 0.0f;							\
	}											\
	else if(nTotalTax >= MGold2Silver(2200))	\
	{											\
		byLevel = ESM_Platinum;					\
		fDecTax = 0.01f;						\
	}											\
	else if(nTotalTax >= MGold2Silver(700))		\
	{											\
		byLevel = ESM_Gold;						\
		fDecTax = 0.02f;						\
	}											\
	else if(nTotalTax >= MGold2Silver(210))		\
	{											\
		byLevel = ESM_Silver;					\
		fDecTax = 0.03f;						\
	}											\
	else if(nTotalTax >= MGold2Silver(30))		\
	{											\
		byLevel = ESM_Copper;					\
		fDecTax = 0.04f;						\
	}											\
	else										\
	{											\
		byLevel = ESM_Wood;						\
		fDecTax = 0.05f;						\
	}

// 当前等级升级所需经验
#define MCalStallLevelExp(byLevel)	((byLevel) * (byLevel) * 2000)

//-----------------------------------------------------------------------------
#pragma pack(pop)