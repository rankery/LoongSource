//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: func_npc_define.h
// author: 
// actor:
// data: 2008-11-10
// last:
// brief: 职能NPC相关原型定义
//-----------------------------------------------------------------------------
#pragma once

#include "cost_type_define.h"

/****************************** 驿站&乾坤石相关 *****************************/

//-----------------------------------------------------------------------------
// 常量
//-----------------------------------------------------------------------------
const INT32 MAX_DAK_SITE_NUM		= 20;	// 驿站能够传送到的目的地个数

//-----------------------------------------------------------------------------
// 结构
//-----------------------------------------------------------------------------
struct tagDakSite
{
	DWORD			dwMapID;	// map编号
	FLOAT			fX;			// (x,y,z)
	FLOAT			fY;
	FLOAT			fZ;
	ECurCostType	eCostType;	// 消耗类型
	INT32			nCostVal;	// 消耗值

	tagDakSite() { ZeroMemory(this, sizeof(*this)); dwMapID = GT_INVALID; }
};

struct tagDakProto
{
	DWORD		dwID;		// 驿站编号
	tagDakSite	dakSite[MAX_DAK_SITE_NUM];
};