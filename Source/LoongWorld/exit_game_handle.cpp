//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: exit_game_handle.cpp
// author: twan
// actor:
// data: 2009-12-7
// last:
// brief: 离线提示
//-----------------------------------------------------------------------------

#include "StdAfx.h"
#include "player_session.h"
#include "role.h"
#include "role_mgr.h"
#include "script_mgr.h"

#include "../WorldDefine/msg_player_preventlost.h"

//------------------------------------------------------------------------------------
// 离线提示
//------------------------------------------------------------------------------------
DWORD PlayerSession::HandleRoleLeaveNotify(tagNetCmd *pCmd)
{
	// 获取角色
	Role* pRole = GetRole();
	if(!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	// 获取等级对应的单次钓鱼经验
	INT nExp = g_ScriptMgr.GetRoleScript()->OnGetRoleFishingExp(pRole);

	BOOL bDoubleQuest = pRole->IsHaveDoneQuest(DOUBLE_EXP_QUEST_ID);
	BOOL bFishingQuest = pRole->IsHaveDoneQuest(FISHING_QUEST_ID);

	// 玩家等级小于10级
	if (pRole->GetLevel() < 10)
	{
		bDoubleQuest = TRUE;					// 小于10级没有双倍和钓鱼任务
		bFishingQuest = TRUE;
	}

	if (pRole->GetLevel() < 20)
	{
		bFishingQuest = TRUE;					// 小于20级没有钓鱼任务
	}

	tagNS_ExitGamePrompt send;
	send.bDualExpQuestDo = bDoubleQuest;		// 双倍经验任务
	send.bFishQuestDo = bFishingQuest;			// 钓鱼任务
	send.nExprence = 120 * nExp;				// 1小时钓鱼经验

	SendMessage(&send, send.dwSize);
	
	return E_Success;
}