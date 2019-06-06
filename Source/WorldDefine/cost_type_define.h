//--------------------------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: cost_type_define.h
// author: 
// actor:
// data: 2008-11-10
// last:
// brief: 消耗类型定义
//--------------------------------------------------------------------------------------------
#pragma once

//--------------------------------------------------------------------------------------------
// 消耗类型枚举
//--------------------------------------------------------------------------------------------
enum ECurCostType
{
	ECCT_Null				= 0,	// 无

	ECCT_BagSilver			= 1,	// 银	-- 背包中
	ECCT_BagYuanBao			= 2,	// 元宝 -- 背包中
	ECCT_WareSilver			= 3,	// 银	-- 角色仓库中
	ECCT_BaiBaoYuanBao		= 4,	// 元宝 -- 角色仓库中
	ECCT_ExchangeVolume		= 5,	// 兑换券 -- 角色商城

	ECCT_GuildContribe		= 10,	// 帮派贡献
	ECCT_GuildExploit		= 11,	// 帮派功勋

	ECCT_ClanConXuanYuan	= 120,	// 轩辕贡献
	ECCT_ClanConShenNong	= 121,	// 神农贡献
	ECCT_ClanConFuXi		= 122,	// 伏羲贡献
	ECCT_ClanConSanMiao		= 123,	// 三苗贡献
	ECCT_ClanConJiuLi		= 124,	// 九黎贡献
	ECCT_ClanConYueZhi		= 125,	// 月氏贡献
	ECCT_ClanConNvWa		= 126,	// 女娲贡献
	ECCT_ClanConGongGong	= 127,	// 共工贡献

	ECCT_ItemExchange		= 40,	// 物品兑换
};

// 消耗枚举转化为氏族类型
#define MTRANS_ECCT2ECLT( ecct ) 	((ECLanType)((ecct) - ECCT_ClanConXuanYuan))
