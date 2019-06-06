//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: group_define.h
// actor:
// data: 2008-11-20
// brief: 队伍属性定义
//-----------------------------------------------------------------------------
#pragma once

//------------------------------------------------------------------------
// 队伍拾取规则
//------------------------------------------------------------------------

const INT      MAX_TEAM_NUM					= 6;		// 小队最大人数
const DWORD    TEAM_SYN_TICK				= TICK_PER_SECOND * 5;
const DWORD    TEAM_DISPLAY_NUM				= 3;

enum EPickMode
{
	EPUM_Free,										// 自由拾取
	EPUM_Order,										// 顺序拾取
};

enum ERefuseApplyMode
{
	ERAM_Accept,								    // 接受申请
	ERAM_Refuse,									// 拒绝申请
};

struct tagLeaderRein
{
	INT				nLeaderRein;					// 队长统御力
};

struct tagAllRoleBuff
{
	DWORD			dwBuffTypeID;							
};

struct tagCreateTeam
{
	DWORD			dwSrcRoleID;
	DWORD			dwDesRoleID;
};

struct tagAddTeamMember
{
	DWORD			dwTeamID;
	DWORD			dwDesRoleID;
};