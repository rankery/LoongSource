//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: return_roleselect_handler.cpp
// author: twan
// actor:
// data: 2009-08-26
// last:
// brief: 客户端返回角色选择时与服务器之间的消息处理
//-----------------------------------------------------------------------------

#include "StdAfx.h"
#include "player_session.h"
#include "map_creator.h"
#include "role.h"
#include "../WorldDefine/msg_return_roleselect.h"
#include "world_session.h"
#include "role_mgr.h"
//--------------------------------------------------------------------------
// 客户端返回到角色选择界面
//--------------------------------------------------------------------------
DWORD PlayerSession::HandleReturnRoleSelect(tagNetCmd* pCmd)
{
	//	获取角色
	Role* pRole = GetRole();
	tagNS_ReturnRoleSelect send;

	if(!P_VALID(pRole))
	{
		send.dwErrorCode = GT_INVALID;
		SendMessage(&send, send.dwSize);
		return GT_INVALID;
	}
	
	g_mapCreator.RoleReturnCharacter(pRole->GetID());

	send.dwErrorCode = E_Success;
	SendMessage(&send, send.dwSize);
	
	return E_Success;
}