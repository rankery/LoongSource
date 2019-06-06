//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: character_handler.cpp
// author: lh
// actor:
// data: 2008-08-27
// last:
// brief: 客户端和服务器端间消息处理 -- 聊天相关
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "player_session.h"
#include "role.h"
#include "role_mgr.h"
#include "map.h"
#include "../WorldDefine/chat.h"
#include "../WorldDefine/chat_define.h"
#include "../WorldDefine/msg_show_item.h"
#include "../ServerDefine/log_cmdid_define.h"
#include "group_mgr.h"
#include "guild.h"
#include "guild_mgr.h"
#include "ps_bomb.h"
#include "chat_mgr.h"
//------------------------------------------------------------------------------------
// 聊天
//------------------------------------------------------------------------------------
DWORD PlayerSession::HandleRoleChat(tagNetCmd *pCmd)
{
	Role* pRole = GetRole();
	if (!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	if (pRole->SpeakOff())
	{
		tagNS_RoleChat send;
		send.dwErrorCode = E_RoleChat_ForbidChat;
		SendMessage(&send, send.dwSize);

		return 0;
	}
	
	tagNC_RoleChat* pRecv = (tagNC_RoleChat*)pCmd;
	DWORD dwRtv = g_msgMgr.HandleChatMsg(pRecv, GetRole());
	if (E_Success != dwRtv)
	{
		tagNS_RoleChat send;
		send.dwErrorCode = dwRtv;
		SendMessage(&send, send.dwSize);
	}

	return 0 ;

}

//------------------------------------------------------------------------
// 装备展示
//------------------------------------------------------------------------
DWORD PlayerSession::HandleRoleShowEquip(tagNetCmd* pCmd)
{
	tagNC_RoleShowEquip *pRecv = (tagNC_RoleShowEquip*)pCmd;

	DWORD dwRtv = g_msgMgr.HandleChatMsg(pRecv, GetRole());
	if (E_Success != dwRtv)
	{
		tagNS_RoleShowEquip send;
		send.dwErrorCode = dwRtv;
		SendMessage((LPVOID)&send, send.dwSize);
	}

	return E_Success;
}

//------------------------------------------------------------------------
// 物品展示
//------------------------------------------------------------------------
DWORD PlayerSession::HandleRoleShowItem(tagNetCmd* pCmd)
{
	tagNC_RoleShowItem *pRecv = (tagNC_RoleShowItem*)pCmd;

	DWORD dwRtv = g_msgMgr.HandleChatMsg(pRecv, GetRole());
	if (E_Success != dwRtv)
	{
		tagNS_RoleShowItem send;
		send.dwErrorCode = dwRtv;
		SendMessage(&send, send.dwSize);
	}
	return E_Success;

}

//------------------------------------------------------------------------
// 获取留言
//------------------------------------------------------------------------
DWORD PlayerSession::HandleRoleLoadLeftMsg(tagNetCmd* pCmd)
{
	if (!P_VALID(GetRole()))
	{
		return GT_INVALID;
	}

	DWORD dwRtv = g_msgMgr.LoadLeftMsg(GetRole()->GetID());
	
	return E_Success;
}
