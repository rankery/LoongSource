//-----------------------------------------------------------------------------
//!\file map_handler.cpp
//!\author Aslan
//!
//!\date 2008-09-15
//! last 2008-09-15
//!
//!\brief 客户端地图消息处理类
//!
//!	Copyright (c) 2004 TENGWU Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#include "StdAfx.h"

#include "../WorldDefine/msg_map.h"

#include "player_session.h"
#include "role.h"
#include "map.h"


//------------------------------------------------------------------------------
// 玩家踩到地图触发器
//------------------------------------------------------------------------------
DWORD PlayerSession::HandleRoleMapTrigger(tagNetCmd* pCmd)
{
	tagNC_RoleMapTrigger* pRecv = (tagNC_RoleMapTrigger*)pCmd;

	Role* pRole = GetRole();

	if( !P_VALID(pRole) ) return GT_INVALID;

	// 玩家踩到触发器
	pRole->MapTrigger(pRecv->dwTriggerID, pRecv->dwMisc);

	return 0;
}

//------------------------------------------------------------------------------
// 通知队友进入
//------------------------------------------------------------------------------
DWORD PlayerSession::HandleRoleInstanceNotify(tagNetCmd* pCmd)
{
	tagNC_InstanceNotify* pRecv = (tagNC_InstanceNotify*)pCmd;

	Role* pRole = GetRole();

	if( !P_VALID(pRole) ) return GT_INVALID;

	// 通知队友
	DWORD dwErrorCode = pRole->InstanceNotify(pRecv->bNotify);

	if(dwErrorCode == GT_INVALID)
	{
		return GT_INVALID;
	}
	
	return 0;
}

//------------------------------------------------------------------------------
// 玩家是否同意其它玩家进入副本的邀请
//------------------------------------------------------------------------------
DWORD PlayerSession::HandleRoleInstanceAgree(tagNetCmd* pCmd)
{
	tagNC_InstanceAgree* pRecv = (tagNC_InstanceAgree*)pCmd;

	Role* pRole = GetRole();

	if( !P_VALID(pRole) ) return GT_INVALID;

	pRole->InstanceAgree(pRecv->bAgree);

	return 0;
}

//------------------------------------------------------------------------------
// 玩家请求离开副本
//------------------------------------------------------------------------------
DWORD PlayerSession::HandleRoleLeaveInstance(tagNetCmd* pCmd)
{
	tagNC_LeaveInstance* pRecv = (tagNC_LeaveInstance*)pCmd;

	Role* pRole = GetRole();

	if(!P_VALID(pRole))	return GT_INVALID;

	pRole->LeaveInstance();

	return 0;
}