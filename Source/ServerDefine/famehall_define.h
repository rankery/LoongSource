//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: famehall_define.h
// author: xlguo
// actor:
// data: 2009-02-26
// last:
// brief: 服务器通信用名人堂基本结构
//-----------------------------------------------------------------------------
#pragma once
#pragma pack(push, 1)
#include "../WorldDefine/famehall_define.h"
enum ETReasureState
{
	ETRS_NULL		= -1,
	ETRS_BEGIN		= 0,

	ETRS_UNACT		= 0,		// 未激活
	ETRS_ACT		= 1,		// 激活

	ETRS_END		,
	ETRS_NUM		= ETRS_END,
};

// 进入名人堂快照最大数目
const UINT16 MAX_ENTER_FAMEHALL_SNAP_NUM	= 50;

// 进入名人堂角色最大数目
const UINT16 MAX_ENTER_FAMEHALL_ROLE_NUM	= 5000;

// 激活珍宝最大数目
const UINT16 MAX_ACT_TREASURE_NUM			= CLAN_TREASURE_NUM;


//----------------------------------------------------------------------------
// 数据库存储结构
//----------------------------------------------------------------------------
struct tagFameHallEnterSnap
{
	DWORD			dwEnterNameID;
	DWORD			dwRoleID;
	tagDWORDTime	dwtEnterTime;

	BOOL operator ==(const tagFameHallEnterSnap& rhs)
	{
		// 进入规则：RoleID不重
		return dwRoleID == rhs.dwRoleID /*&& dwEnterNameID == rhs.dwEnterNameID*/;
	}
};



#pragma pack(pop)