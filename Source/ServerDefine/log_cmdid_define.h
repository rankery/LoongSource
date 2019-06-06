//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: log_cmdid_define.h
// author: 
// actor:
// data: 2008-11-11
// last:
// brief: 服务器记录log用的网络命令代码枚举 -- 方便查找
//-----------------------------------------------------------------------------
#pragma once

#pragma pack(push, 1)
//-----------------------------------------------------------------------------
// log用网络消息枚举 -- 一旦定义了最好不要修改(1.数据库已有相应记录；2.脚本可能已写死)
//-----------------------------------------------------------------------------
enum ELogCmdID
{
	ELCID_Null					= 0,	// 无

	// 商店相关
	ELCID_Shop_BuyItem			= 1,	// 买物品(非装备)
	ELCID_Shop_BuyEquip			= 2,	// 买装备
	ELCID_Shop_Sell				= 3,	// 卖物品&装备

	// PK保护相关
	ELCID_Open_SafeGuard		= 11,	// 开启PK保护

	// 死亡惩罚
	ELCID_Dead_Penalty			= 12,	// PK惩罚

	// 百宝袋相关
	ELCID_BaiBao_LoadFromDB		= 20,	// 从item_baibao表中读取的物品
	ELCID_BaiBao_Bill_YuanBao	= 21,	// 元宝直冲

	// 行囊相关
	ELCID_Bag_PosOverlap		= 30,	// 初始化角色背包时，发现物品位置重叠

	// 洗点
	ELCID_Clear_Att				= 40,	// 洗属性点
	ELCID_Clear_Talent			= 41,	// 洗天资点


	// 角色仓库相关
	ELCID_RoleWare_PosOverlap	= 50,	// 初始化角色仓库时，发现物品位置重叠
	ELCID_RoleWare_SaveSilver	= 51,	// 存钱
	//ELCID_RoleWare_SaveYuanBao	= 52,	// 存元宝
	ELCID_RoleWare_GetSilver	= 53,	// 取钱
	ELCID_RoleWare_GetYuanBao	= 54,	// 取元宝

	
	// 商城
	ELCID_Mall_BuyItem			= 60,	// 购买商城物品
	ELCID_Mall_BuyPack			= 61,	// 购买商城礼品包
	ELCID_Mall_FreeItem			= 62,	// 领取商城免费物品
	ELCID_Mall_BuyItem_Add		= 63,	// 购买商城物品时附带
	ELCID_Mall_BuyPack_Add		= 64,	// 购买商城礼品包时附带
	ELCID_Mall_PresentItem		= 65,	// 赠送商城物品
	ELCID_Mall_PresentPack		= 66,	// 赠送商城礼品包
	ELCID_Mall_PresentItem_Add	= 67,	// 赠送商城物品时附带
	ELCID_Mall_PresentPack_Add	= 68,	// 赠送商城礼品包时附带

	// 元宝交易
	ELCID_Trade_SaveYuanBao		= 70,	// 向交易账户存元宝
	ELCID_Trade_SaveSilver		= 71,	// 向交易账户存金钱
	ELCID_Trade_DepositYuanBao  = 72,	// 从交易账户取出元宝
	ELCID_Trade_DepositSilver	= 73,	// 从交易账户取出金钱
	ELCID_Trade_Tax				= 74,	// 提交订单时扣除手续费

	// 商城 - 续
	ELCLD_Mall_ExchangeItem		= 80,	// 兑换商城物品
	ELCLD_Mall_ExchangePack		= 81,	// 兑换商城礼品包
	ELCLD_Mall_ExchangeItem_Add	= 82,	// 兑换商城物品时附带
	ELCLD_Mall_ExchangePack_Add	= 83,	// 兑换商城礼品包时附带

	// 团购
	ELCID_GroupPurchase_Faild		= 90,	// 团购失败返还所得
	ELCLD_GroupPurchase_BuyItem		= 91,	// 团购购买商城物品
	ELCLD_GroupPurchase_BuyItem_Add	= 92,	// 团购商城物品附带

	// 任务相关
	ELCID_Quest_Loot			= 100,	// 任务掉落
	ELCID_Quest_Complete		= 101,	// 完成任务
	ELCID_Quest_Accept			= 102,	// 接取任务
	ELCID_Quest_Discard			= 103,	// 放弃任务
	ELCID_Quest_rewards			= 104,  // 任务奖励

	// 装备强化相关
	ELCID_Compose_Posy			= 150,	// 铭文
	ELCID_Compose_Engrave		= 151,	// 镌刻
	ELCID_Compose_Enchase		= 152,	// 镶嵌
	ELCID_Compose_Brand			= 153,	// 烙印
	ELCID_Compose_LongFu		= 154,	// 龙附
	ELCID_Compose_Produce		= 155,	// 合成
	ELCID_Compose_Quench		= 156,	// 淬火
	ELCID_Compose_Decompose		= 157,	// 点化,装备分解
	ELCID_Compose_Chisel		= 158,	// 开凿
	ELCID_Compose_Dye			= 159,	// 染色

	// 玩家间交易
	ELCID_Exchange_Verify		= 160,	// 交易确认
	// Jason 没有位置了，暂时放这里，2009-12-6
	ELCID_Raise_PotVal			= 165,	// 没有空隙了，看看怎么处理啊，郁闷
	ELCID_LessingLoong			= 166,	// 神龙赐福
	ELCID_PhilosophersStone		= 167,	// 点金石

	// 原地复活
	ELCLD_Revive_Locus			= 180,

	// 摆摊
	ELCLD_Stall_BeSold			= 190,	// 摆摊卖出
	ELCLD_Stall_Buy				= 191,	// 从摊位上购买

	// 换场景
	ELCLD_Dak					= 200,

	// 物品
	ELCLD_Item_Use				= 300,	// 物品使用
	ELCLD_Item_Move				= 301,	// 同一容器内移动
	ELCLD_Item_Move2Other		= 302,	// 不同容器间移动

	ELCLD_Item_AtTerm			= 310,	// 有时限物品，到期删除

	// 装备
	ELCLD_Equip_Identify		= 350,	// 装备鉴定
	ELCLD_Equip_ReGet			= 351,	// 装备恢复
	
	// 新手
	ELCD_1stOnline_Item			= 380,	// 新手发放

	// 玩家吃从背包扔掉物品
	ELCLD_Role_Discard_Item		= 401,

	// 玩家拾取物品
	ELCLD_PickUp_Item			= 402,
	ELCLD_PickUp_Money			= 403,

	// 给好友送礼物
	ELCLD_Send_Gift				= 410,

	// 礼品包
	ELCLD_Gift_Bag				= 420,

	// 职能NPC相关
	ELCLD_Bag_Extend			= 450,	// 扩充背包
	ELCLD_RoleWare_Extend		= 451,	// 角色仓库扩充
	ELCLD_CureInjure			= 452,	// npc治疗内伤
	
	// 普通掉落
	ELCID_Loot					= 500,	// 普通掉落

	// 帮派相关
	ELCLD_Guild_Create			= 600,	// 创建帮派
	ELCLD_Guild_Upgrade			= 601,	// 帮派设施升级
	ELCLD_Guild_SpreadAffair	= 602,	// 发布帮务
	ELCLD_Guild_DailyCost		= 603,	// 日常消耗
	ELCLD_Guild_SkillResearch	= 604,	// 帮派技能升级
	ELCLD_Guild_SkillLearn		= 605,	// 学习帮派技能
	ELCLD_Guild_Commerce		= 606,	// 跑商

	// 聊天相关
	ELCLD_CHAT_WORLD_CHANNEL		= 700,	//在世界频道发言
	ELCLD_SHOW_EQUIP_WORLD_CHANNEL	= 701,	//在世界频道展示装备
	ELCLD_SHOW_ITEM_WORLD_CHANNEL	= 702,	//在世界频道展示物品
	ELCLD_CAST_TALK_IM				= 703,	//传音使用IM道具

	// 宠物相关
	ELCLD_PET_ITEM_CREATE_PET		= 800,	// 生成宠物
	ELCLD_PET_ITEM_FEED_PET			= 801,	// 宠物喂食
	ELCLD_PET_ITEM_PET_LEARNSKILL	= 802,	// 宠物学习技能
	ELCLD_PET_ITEM_PET_REVIVE		= 803,	// 宠物复活
	ELCLD_PET_ITEM_PET_EQUIP		= 804,	// 宠物装备
	ELCLD_RolePourExpPet			= 805,	// 灌注经验
	ELCLD_PET_ITEM_PET_Enhance		= 806,	// 宠物装备
	ELCLD_PET_ITEM_PET_Lock			= 807,	// 宠物锁定
	ELCLD_PET_ITEM_PET_UnLock		= 808,	// 宠物解锁
	ELCLD_PET_ITEM_PET_Food			= 809,	// 宠物食物
	ELCLD_PET_SKILL_SALE			= 810,	// 宠物小贩技能
	
	ELCLD_PET_CREATE_PET			= 811,	// 创建宠物
	ELCLD_PET_GAIN_PET				= 812,	// 获得宠物
	ELCLD_PET_DEL_PET_CREATE		= 813,	// 删除宠物（创建失败）
	ELCLD_PET_DEL_PET_INIT			= 814,	// 删除宠物（初始化失败）
	ELCLD_PET_DEL_PET_CLIENT		= 815,	// 删除宠物（客户端命令）
	ELCLD_PET_LOSE_PET				= 816,	// 失去宠物

	// 氏族相关
	ELCLD_ACT_TREASURE				= 900,	// 激活氏族珍宝获得

	//固定活动相关
	ELCLD_WU_SE_SHI					= 1000, // 固定活动删除五色石
	ELCLD_PVP						= 1001, // pvp固定活动

	// VIP摊位相关
	ELCLD_VIP_Stall_Rent			= 1100,	// VIP摊位租赁

	// 金牌网吧
	ELCLD_VIP_NetBar				= 1200,	// 金牌网吧

	// 技能
	ELCLD_Skill_Use					= 1300, // 技能
		
	// 职业相关
	ELCLD_Class_Change				= 1400, // 转职
	ELCLD_Class_Get					= 1401, // 就职

	// 其他相关在此处之上加
	ELCID_GemRemoval				= 1500,	// 宝石拆除
	
	// GM命令
	ELCID_GM_ClearBag				= 1000000,	// 清理背包
	ELCID_GM_CreateItem				= 1000001,	// 创建物品/装备/时装
	ELCID_GM_GetMoney				= 1000002,	// 获得金钱
	ELCID_GM_GetYuanBao				= 1000003,	// 获得元宝
	ELCID_GM_GetExVolume			= 1000004,	// 获得赠点
	ELCID_GM_GetFund				= 1000005,	// 获得帮派资金
	ELCID_GM_GetMaterial			= 1000006,	// 获得帮派资材
	ELCID_GM_GetContribute			= 1000007,	// 获得帮派个人贡献
};


//-----------------------------------------------------------------------------
// log用数据结构
//-----------------------------------------------------------------------------
struct tagLogSilver				// 金钱变化
{
	DWORD	dwAccountID;
	DWORD	dwRoleID;
	DWORD	dwCmdID;
	DWORD	dwRoleIDRel;
	INT64	n64Silver;			// +：获得；-：失去
	INT64	n64TotalSilver;		// 对应容器中总量
	INT8	n8LogConType;		// 背包，仓库
};

struct tagLogYuanBao			// 元宝变化
{
	DWORD	dwAccountID;
	DWORD	dwRoleID;
	DWORD	dwCmdID;
	INT		nYuanBao;			// +：获得；-：失去
	INT		nTotalYuanBao;		// 对应容器中总量
	INT8	n8LogConType;		// 背包，百宝袋
};

struct tagLogExVolume			// 赠点变化
{
	DWORD	dwAccountID;
	DWORD	dwRoleID;
	DWORD	dwCmdID;
	INT		nExVolume;			// +：获得；-：失去
	INT		nTotalExVolume;		// 对应容器中总量
};

struct tagLogTimeStat			// 在线玩家定时统计
{
	DWORD	dwAccountID;
	DWORD	dwRoleID;
	INT64	n64BagSilver;
	INT64	n64WareSilver;
	INT		nBagYuanBao;
	INT		nBaiBaoYuanBao;
	INT		nExVolume;
};

struct tagLogItem				// 物品log结构
{
	DWORD		dwRoleID;
	DWORD		dwTypeID;

	INT64		n64Serial1;
	INT64		n64Serial2;
	INT8		n8ConType1;
	INT8		n8ConType2;
	INT16		n16ResNum1;
	INT16		n16ResNum2;

	INT16		n16OptNum;		// 变化个数(+：表示获得；-：表示失去)
	DWORD		dwCmdID;
	DWORD		dwRoleIDRel;
};

struct tagLogMallSell			// 商城出售物品log结构
{
	INT64		n64Serial;
	DWORD		dwTypeID;
	DWORD		dwRoleID;		// 购买者
	DWORD		dwToRoleID;		// 获得物品者
	DWORD		dwFstGainTime;
	DWORD		dwExistTime;
	INT			nMaxUseTimes;
	INT			nCostYuanBao;
	INT			nCostExVolume;
	DWORD		dwCmdID;
	INT16		n16NumSell;
};

struct tagLogMallSellPack		// 商城出售礼品包log结构
{
	DWORD		dwPackID;
	DWORD		dwBuyRoleID;
	DWORD		dwToRoleID;
	INT			nCostYuanBao;
};

struct tagLogItemTims			// 物品使用次数log结构
{
	DWORD		dwRoleID;
	DWORD		dwCmdID;

	INT64		n64Serial;
	DWORD		dwTypeID;
	INT			nUsedTimes;
	INT			nMaxUseTimes;
	INT8		n8ConType;
};

struct tagLogFund
{
	DWORD		dwGuildID;
	DWORD		dwAccountID;
	DWORD		dwRoleID;
	DWORD		dwCmdID;

	INT			nFund;
	INT			nTotalFund;
};

struct tagLogMaterial
{
	DWORD		dwGuildID;
	DWORD		dwAccountID;
	DWORD		dwRoleID;
	DWORD		dwCmdID;

	INT			nMaterial;
	INT			nTotalMaterial;
};

struct tagLogReputation
{
	DWORD		dwGuildID;
	DWORD		dwAccountID;
	DWORD		dwRoleID;
	DWORD		dwCmdID;

	INT			nReputation;
	INT			nTotalReputation;
};

struct tagLogGMCmd
{
	DWORD		dwRoleID;
	TCHAR		szGMCmd[X_LONG_STRING];
	DWORD		dwErrorCode;
};

struct tagLogPet
{
	DWORD		dwMasterID;
	DWORD		dwPetID;
	DWORD		dwCmdID;
};


//-----------------------------------------------------------------------------
#pragma pack(pop)