//-----------------------------------------------------------------------------
//!\file reputation_handler.cpp
//!\author xlguo
//!
//!\date 2009-02-19
//! last 
//!
//!\brief 声望消息处理过程
//!
//!	Copyright (c) 2004 TENGWU Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "player_session.h"
#include "role_mgr.h"
#include "role.h"
#include "../WorldDefine/msg_reputation.h"

DWORD PlayerSession::HandleGetRoleClanData(tagNetCmd* pCmd)
{
	MGET_MSG(pRecv, pCmd, NC_GetReputation);
	
	Role* pRole = GetOtherInMap(pRecv->dwRoleID);
	if (!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	tagNS_GetReputation send;
	send.dwRoleID = pRecv->dwRoleID;
	for (INT nClanType = ECLT_BEGIN; nClanType < ECLT_END; ++nClanType)
	{
		send.ReputeData.nrValue[nClanType]		= pRole->GetClanData().RepGetVal((ECLanType)nClanType);
		send.ReputeData.ncValue[nClanType]		= pRole->GetClanData().ClanConGetVal((ECLanType)nClanType);
		send.ReputeData.nActiveCount[nClanType]	= pRole->GetClanData().ActCountGetVal((ECLanType)nClanType);
		send.ReputeData.bisFame[nClanType]		= pRole->GetClanData().IsFame((ECLanType)nClanType);
	}
	SendMessage(&send, send.dwSize);

	return 0;
}
