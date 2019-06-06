//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: class_handler.cpp
// author: zr
// actor:
// data: 2009-09-26
// last:
// brief: 客户端和服务器端间消息处理 -- 角色职业相关
//-----------------------------------------------------------------------------
#include "StdAfx.h"

#include "player_session.h"
#include "Role.h"
#include "../WorldDefine/role_att.h"

//------------------------------------------------------------------------
// 设置职业
//------------------------------------------------------------------------
DWORD PlayerSession::HandleSetClass(tagNetCmd* pCmd)
{
	tagNC_SetClass* pRecv = (tagNC_SetClass*) pCmd;

	Role* pRole = GetRole();
	if( !P_VALID(pRole)) return GT_INVALID;
	
	DWORD dwRet = pRole->RoleSetVocation(pRecv->dwNpcId, pRecv->eClass);

	// 通知客户端
	tagNS_SetClass ret;
	ret.dwErrCode = (EClassErrorCode)dwRet;
	SendMessage(&ret, ret.dwSize);


	return dwRet;
}

//------------------------------------------------------------------------
// 更改专精职业
//------------------------------------------------------------------------
DWORD PlayerSession::HandleChangeClass(tagNetCmd* pCmd)
{
	tagNC_ChangeClass* pRecv = (tagNC_ChangeClass*) pCmd;

	Role* pRole = GetRole();
	if( !P_VALID(pRole)) return GT_INVALID;

	DWORD dwRet = pRole->ChangeVocation(pRecv->dwNpcId, pRecv->eClass);

	// 通知客户端
	tagNS_ChangeClass ret;
	ret.dwErrCode = (EClassErrorCode)dwRet;
	SendMessage(&ret, ret.dwSize);

	return dwRet;
}