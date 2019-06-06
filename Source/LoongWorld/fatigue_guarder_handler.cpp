//-----------------------------------------------------------------------------
//!\file fatigue_guarder_handler.cpp
//!\author xlguo
//!
//!\date 2009-02-09
//! last 
//!
//!\brief 防沉迷系统消息处理过程
//!
//!	Copyright (c) 2004 TENGWU Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "player_session.h"
DWORD PlayerSession::HandleGetFatigueInfo(tagNetCmd* pCmd)
{
	tagNC_GetFatigueInfo* pRecv = (tagNC_GetFatigueInfo*)pCmd;

	GetFatigueGuarder().NotifyClient();

	return 0;
}