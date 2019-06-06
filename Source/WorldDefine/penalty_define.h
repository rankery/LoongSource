//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: penalty_define.h
// author: twan
// actor:
// data: 2009-12-06
// last:
// brief: 死亡惩罚定义
//-----------------------------------------------------------------------------

#pragma once
#pragma pack(push, 1)

//-----------------------------------------------------------------------------
// 死亡惩罚类型
//-----------------------------------------------------------------------------
enum EDeadPenaltyMode
{
	EDPM_EquipLoot		=		0,				// 装备掉落
	EDPM_Exp			=		1,				// 经验
	EDPM_Injury			=		2,				// 内伤
	EDPM_Morality		=		3,				// 道德
	EDPM_Hostility		=		4,				// 戾气
	EDPM_Commerce		=		5,				// 跑商商银
};



#pragma pack(pop)