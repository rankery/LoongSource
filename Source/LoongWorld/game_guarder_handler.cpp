//-----------------------------------------------------------------------------
//!\file game_guarder_handler.cpp
//!\author xlguo
//!
//!\date 2009-03-02
//! last 
//!
//!\brief 反外挂处理过程
//!
//!	Copyright (c) 2004 TENGWU Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "player_session.h"
#include "game_guarder.h"

#include "../WorldDefine/msg_game_guarder.h"

DWORD PlayerSession::HandleGameGuarderMsg(tagNetCmd* pCmd)
{
	MGET_MSG(pRecv, pCmd, NC_GameGuarder);
	switch (pRecv->chCmd)	
	{
	case 'T':
		g_gameGuarder.Transport(GetSessionID(), pRecv->chData, pRecv->nLen);
		break;
	case 'R':
		g_gameGuarder.Ret(GetSessionID(), pRecv->chData);
		break;
	default:
		ASSERT(0);
		break;
	}
	return 0;
}
