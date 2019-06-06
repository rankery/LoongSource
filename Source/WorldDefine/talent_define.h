//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: talent_define.h
// author: Aslan
// actor:
// data: 2008-10-20
// last:
// brief: 触发器静态属性
//-----------------------------------------------------------------------------
#pragma once

#include "RoleDefine.h"

#pragma pack(push, 1)

//-----------------------------------------------------------------------------
// CONSTS
//-----------------------------------------------------------------------------
const INT32 X_MAX_TALENT_PER_ROLE		=		4;		// 一个玩家可最多开启的天资种类
const INT32 X_MAIN_TALENT_START			=		0;		// 主天资的起始位置，0
const INT32	X_ASS_TALENT_START			=		2;		// 辅助天资的起始位置，2


//-----------------------------------------------------------------------------
// 天资结构
//-----------------------------------------------------------------------------
struct tagRoleTalent
{
	ETalentType		eType;		// 天资类型
	INT32			nPoint;		// 该天资的总投点数
};

#pragma pack(pop)