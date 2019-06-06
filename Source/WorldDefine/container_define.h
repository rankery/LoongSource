//--------------------------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: container_define.h
// author: 
// actor:
// data: 2008-8-4
// last:
// brief: 人物装备栏、行囊、仓库等相关定义
//--------------------------------------------------------------------------------------------
#pragma once

//-----------------------------------------------------------------------------
// 常量定义
//-----------------------------------------------------------------------------
const INT32 MAX_BAG_NUM				= 7;							// 背包最大页数
const INT32 SPACE_ONE_BAG			= 20;							// 单个背包格数
const INT32 SPACE_ALL_BAG			= SPACE_ONE_BAG * MAX_BAG_NUM;	// 最大背包格数

const INT32 SPACE_QUEST_BAG			= 20;							// 任务物品栏最大格数	
const INT32 SPACE_BAIBAO_BAG		= 20;							// 百宝袋最大格数

const INT32 MAX_BAIBAO_LOG_NUM		= 40;							// 百宝袋最大Log数

const INT32 MAX_WARE_NUM			= 10;							// 仓库最大页数
const INT32 SPACE_ONE_WARE			= 24;							// 单个仓库格数
const INT32 SPACE_ALL_WARE			= SPACE_ONE_WARE * MAX_WARE_NUM;// 最大仓库格数

const INT32 MIN_PASSWORD_NUM		= 6;							// 密码最小长度
const INT32 MAX_PASSWORD_NUM		= 14;							// 密码最大长度

const INT32 SPACE_WARE_PER_EXTEND	= 6;							// 角色仓库一次扩充格子数
const INT32 SPACE_BAG_PER_EXTEND	= 5;							// 背包一次扩充格子数

const INT32 MAX_WARE_EX_TIMES_SILVER= 4;							// 使用游戏币最多能扩充的仓库次数
const INT32 MAX_BAG_EX_TIMES_SILVER	= 4;							// 使用游戏币最多能扩充的背包次数

const INT32 MAX_GUILDWARE_NUM       =   5;                          // 帮派仓库最大页数
const INT32 SPACE_ONE_GUILDWARE     =   35;                         // 单个帮派仓库格数
const INT32 SPACE_ALL_GUILDWARE     =   SPACE_ONE_GUILDWARE * MAX_GUILDWARE_NUM;


//-----------------------------------------------------------------------------
// 枚举定义
//-----------------------------------------------------------------------------
// 容器类型
enum EItemConType
{
	EICT_Start		= 0,
	EICT_Null		= 0,

	EICT_Bag		= 1, // 背包
	EICT_Quest		= 2, // 任务物品栏
	EICT_Baibao		= 3, // 百宝袋
	EICT_RoleWare	= 4, // 角色仓库
	EICT_Equip		= 5, // 装备栏

	EICT_Shop		= 6, // 商店
	EICT_Ground		= 7, // 掉落

    EICT_PetEquip   = 8, // 宠物装备栏

	EICT_GuildWare	= 9, // 帮派仓库

	EICT_End		= 10,
};

// 装备栏
enum EEquipPos	// 装备部位
{
	EEP_Null			= -1,
	EEP_Start			= -1, 

	EEP_Equip_Start		= 0,
	EEP_Head			= 0, // 头部
	EEP_Face			= 1, // 面部
	EEP_Body			= 2, // 上身
	EEP_Legs			= 3, // 下身
	EEP_Back			= 4, // 背部
	EEP_Wrist			= 5, // 腕部
	EEP_Feet			= 6, // 足部

	EEP_Finger1			= 7, // 戒指
	EEP_Finger2			= 8, // 戒指
	EEP_Waist			= 9, // 腰饰
	EEP_Neck			= 10, // 项链

	EEP_RightHand		= 11, // 主手武器
	EEP_LeftHand		= 12, // 副手武器
	EEP_Equip_End		= 12,

	EEP_MaxEquip		= 13, // 装备栏栏位数

	EEP_FashionHead		= 13, // 头部
	EEP_FashionFace		= 14, // 面部
	EEP_FashionBody		= 15, // 上身
	EEP_FashionLegs		= 16, // 下身
	EEP_FashionBack		= 17, // 背部
	EEP_FashionWrist	= 18, // 腕部
	EEP_FashionFeet		= 19, // 足部

	EEP_End				= 20,

};

const INT32 X_EQUIP_BAR_SIZE = EEP_End - EEP_Start - 1;
#define MIsInEquipPos(n16Pos)	(n16Pos > EEP_Start && n16Pos < EEP_End)

// 武器装备部位
enum EWeaponPos
{
	EWP_NULL			= 0,	// 无
	EWP_Waist			= 1,	// 腰间
	EWP_WaistBack		= 2,	// 腰后
	EWP_Back			= 3,	// 背后
	EWP_Hand			= 4,	// 手部
};

const INT32 X_WEAPONPOS_NUM = EWP_Hand + 1;

// 百宝袋记录类型
enum EBaiBaoRecordType
{
	EBBRT_System,			// 系统划拨
	EBBRT_Mall,				// 商城赠品
	EBBRT_Friend,			// 好友赠送
	EBBRT_Myself,			// 自己领取
	EBBRT_GroupPurchase,	// 帮派团购
	EBBRT_VipNetBarGift,	// 金牌网吧礼品
};

//-----------------------------------------------------------------------------
// 结构
//-----------------------------------------------------------------------------
// 自动整理
struct tagItemOrder
{
	INT16	n16OldIndex;
	INT16	n16NewIndex;
};

// 百宝袋记录
struct tagBaiBaoRecord
{
	INT16	n16Size;		// 单条记录长度
	INT16	n16Type;		// EBaiBaoRecordType
	DWORD	dwTypeID;		// 物品的TypeID
	DWORD	dwRoleID;		// 操作玩家的ID，GT_INVALID为系统划拨
	DWORD	dwTime;			// 操作时间 tagDWORDTime
	TCHAR	szWords[1];		// 赠言
};

//-----------------------------------------------------------------------------
// 宏
//-----------------------------------------------------------------------------

// 仓库扩容次数=（当前仓库以开放栏位数-24）/6
#define MCalWareExTimes(n16CurWareSpace)	((n16CurWareSpace - 24) / 6)

// 若使用游戏币扩充，收费=（仓库扩容次数+1）^2×3000[单位游戏币]
#define MCalWareExSilver(nExTimes)	((nExTimes + 1) * (nExTimes + 1) * 3000)

// 若使用元宝扩充，则收费=（仓库扩容次数+1）×5[单位元宝]
#define MCalWareExYuanBao(nExTimes)	((nExTimes + 1) * 5)


// 行囊扩容次数=（当前行囊以开放栏位数-20）/5
#define MCalBagExTimes(n16CurBagSpace)	((n16CurBagSpace - 20) / 5)

// 若使用游戏币扩充，则收费=（行囊扩容次数+1）^2×8000[单位游戏币]
#define MCalBagExSilver(nExTimes)	((nExTimes + 1) * (nExTimes + 1) * 8000)

// 若使用元宝扩充，则收费=（行囊扩容次数+1）×10[单位元宝]
#define MCalBagExYuanBao(nExTimes)	((nExTimes + 1) * 10)

