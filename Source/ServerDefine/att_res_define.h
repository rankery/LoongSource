//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: att_res_define.h
// author: Sxg
// actor:
// data: 2009-02-19
// last:
// brief: 资源管理器数据结构定义
//-----------------------------------------------------------------------------
#pragma once

#include "../WorldDefine/longhun_define.h"
//-----------------------------------------------------------------------------
// 品质影响的装备属性相关参数
//-----------------------------------------------------------------------------
struct tagEquipQltyEffect
{
	// 基础属性影响系数
	FLOAT fWeaponFactor;
	FLOAT fArmorFactor;

	// 一级属性
	INT32 nAttAFactor;
	FLOAT fAttAFactor;
	INT32 nAttANumEffect;

	// 潜力值
	FLOAT fPotFactor;

	// 镶嵌孔数量 -- 记录出现的几率
	INT32 nHoleNumPct[MAX_EQUIPHOLE_NUM + 1];

	// 龙魂能力 -- 表里
	INT32 nLonghunPct[X_LONGHUN_TYPE_NUM];

	// 特殊属性
	INT32 nSpecAttPct;
};

//-----------------------------------------------------------------------------
// 技能主从之间的影响结构
//-----------------------------------------------------------------------------
struct tagSkillModify
{
	TList<DWORD>	listModify;		// 直接影响该主技能的从技能
};

//-----------------------------------------------------------------------------
// 时装生成规则相关定义
//-----------------------------------------------------------------------------
struct tagFashionGen
{
	FLOAT		fAppearanceFactor;	// 仪容属性(AppearancePct)
	INT16		n16ReinPct;			// 统御属性加成(ReinPct)
	INT16		n16SavvyPct;		// 悟性属性加成(SavvyPct)
	INT16		n16FortunePct;		// 福缘属性加成(FortunePct)
	INT8		n8ReinVal;			// 值=物品等级÷ReinVal[取整](ReinVal)
	INT8		n8SavvyVal;			// 值=物品等级÷SavvyVal[取整](SavvyVal)
	INT8		n8FortuneVal1;		// 值=FortuneVal1（上下浮动20%）+装备品级/FortuneVal2
	INT8		n8FortuneVal2;		// (FortuneVal1, FortuneVal2)
	INT8		n8Dummy[2];
};

struct tagFashionColorPct	// 时装生成时颜色概率
{
	INT16	n16ColorPct[X_COLOR_NUM];	// 颜色概率
};