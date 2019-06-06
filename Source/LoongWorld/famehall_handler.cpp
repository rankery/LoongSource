//-----------------------------------------------------------------------------
//!\file title_handler.cpp
//!\author xlguo
//!
//!\date 2009-01-20
//! last 2009-02-02
//!
//!\brief 称号消息处理过程
//!
//!	Copyright (c) 2004 TENGWU Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "player_session.h"
#include "famehall.h"
#include "world.h"
#include "../WorldDefine/msg_famehall.h"


//player session
DWORD PlayerSession::HandleGetFameHallRoles(tagNetCmd* pCmd)
{
	
	tagNC_GetFameHallRoles* pRecv = (tagNC_GetFameHallRoles*)pCmd;

	if (!JDG_VALID(ECLT, pRecv->byClanType))
		return GT_INVALID;

	tagDWORDTime dwtUpdate = g_fameHall.GetFameSnapTimeStamp((ECLanType)pRecv->byClanType);

	if (dwtUpdate <= pRecv->dwUpdateTime)
	{
		tagNS_GetFameHallRoles send;
		send.eClanType = pRecv->byClanType;
		send.byErrCode = E_FrameHall_MemberOrderUnchanged;
		send.dwUpdateTime = dwtUpdate;
		send.byRoleNum = 0;
		
		SendMessage(&send, send.dwSize);
		return 0;
	}
	
	
	DWORD dwNum = g_fameHall.GetMemberTop50Num((ECLanType)pRecv->byClanType);

	DWORD dwSize =  dwNum * sizeof(DWORD) - 1 + sizeof(tagNS_GetFameHallRoles);

	MCREATE_MSG(pSend, dwSize, NS_GetFameHallRoles);

	pSend->byRoleNum = (BYTE)dwNum;
	pSend->eClanType = pRecv->byClanType;
	pSend->dwUpdateTime = g_world.GetWorldTime();
	g_fameHall.GetMemberTop50(pSend->byData, (ECLanType)pRecv->byClanType);

	SendMessage(pSend, pSend->dwSize);

	MDEL_MSG(pSend);

	return 0;
}

//player session
DWORD PlayerSession::HandleGetReputeTop(tagNetCmd* pCmd)
{
	MGET_MSG(pRecv, pCmd, NC_GetReputeTop);

	if (!JDG_VALID(ECLT, pRecv->byClanType))
		return GT_INVALID;

	tagDWORDTime dwtUpdate = g_fameHall.GetRepRankTimeStamp((ECLanType)pRecv->byClanType);

	// 已经是最新的不用更新
	if (dwtUpdate <= pRecv->dwUpdateTime)
	{
		tagNS_GetReputeTop send;
		send.byRoleNum = pRecv->byClanType;
		send.byErrCode = E_FrameHall_RepOrderUnchanged;
		send.dwUpdateTime = dwtUpdate;
		send.byRoleNum = 0;

		SendMessage(&send, send.dwSize);

		return 0;
	}

	DWORD dwNum = g_fameHall.GetRepRankNum((ECLanType)pRecv->byClanType);

	if (dwNum > 0)
	{
		DWORD dwSize = dwNum * sizeof(tagRepRankData) - 1 + sizeof(tagNS_GetReputeTop);

		MCREATE_MSG(pSend, dwSize, NS_GetReputeTop);

		pSend->byRoleNum = (BYTE)dwNum;
		pSend->byClanType = pRecv->byClanType;
		pSend->dwUpdateTime = g_world.GetWorldTime();
		g_fameHall.GetRepRankTop(pSend->byData, (ECLanType)pRecv->byClanType);

		SendMessage(pSend, pSend->dwSize);

		MDEL_MSG(pSend);
	}
	else
	{
		tagNS_GetReputeTop send;
		send.byClanType = pRecv->byClanType;
		send.byRoleNum = 0;
		send.dwUpdateTime = dwtUpdate;
		SendMessage(&send, send.dwSize);
	}
	
	return 0;
}

//player session
DWORD PlayerSession::HandleGetActClanTreasure(tagNetCmd* pCmd)
{

	MGET_MSG(pRecv, pCmd, NC_GetActClanTreasure);

	if (!JDG_VALID(ECLT, pRecv->byClanType))
		return GT_INVALID;

	DWORD dwNum = g_fameHall.GetActClanTreasureNum((ECLanType)pRecv->byClanType);
	if (dwNum > 0)
	{
		DWORD dwSize = dwNum * sizeof(tagTreasureData) - 1 + sizeof(tagNS_GetActClanTreasure);

		MCREATE_MSG(pSend, dwSize, NS_GetActClanTreasure);

		pSend->n16ActTreasureNum = (BYTE)dwNum;
		pSend->byClanType = pRecv->byClanType;

		g_fameHall.GetActClanTreasure(pSend->byData, (ECLanType)pRecv->byClanType);

		SendMessage(pSend, pSend->dwSize);

		MDEL_MSG(pSend);
	}
	else
	{
		tagNS_GetActClanTreasure send;
		send.byClanType = pRecv->byClanType;
		send.n16ActTreasureNum = 0;

		SendMessage(&send, send.dwSize);
	}
	return 0;
}

//world session
DWORD PlayerSession::HandleActiveTreasure(tagNetCmd* pCmd)
{

	MGET_MSG(pRecv, pCmd, NC_ActiveTreasure);

	DWORD dwRtv = g_fameHall.ActClanTreasure(m_pRole, pRecv->u16TreasureID);

	tagNS_ActiveTreasure send;
	send.u16TreasureID = pRecv->u16TreasureID;
	send.dwErrCode = dwRtv;
	SendMessage(&send, send.dwSize);

	return 0;
}