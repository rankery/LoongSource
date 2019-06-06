//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: baibao_handler.cpp
// author: Yg
// actor:
// data: 2009-05-19
// last:
// brief: 客户端和服务器端间消息处理 -- 百宝袋相关
//-----------------------------------------------------------------------------

#include "StdAfx.h"

#include "../ServerDefine/role_data.h"

#include "player_session.h"
#include "db_session.h"

DWORD PlayerSession::HandleInitBaiBaoRecord(tagNetCmd* pCmd)
{
	if( !P_VALID(m_pRole) ) return GT_INVALID;

	tagNDBC_LoadBaiBaoLog sendDB;
	sendDB.dwAccountID = m_dwAccountID;
	g_dbSession.Send(&sendDB, sendDB.dwSize);

	return E_Success;
}