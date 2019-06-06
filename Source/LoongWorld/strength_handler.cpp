//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: class_handler.cpp
// author: zr
// actor:
// data: 2009-12-3
// last:
// brief: 客户端和服务器端间消息处理 -- 实力系统相关
//-----------------------------------------------------------------------------
#include "StdAfx.h"

#include "player_session.h"
#include "Role.h"
#include "Strength.h"
#include "../WorldDefine/role_att.h"
#include "../WorldDefine/msg_strength.h"

DWORD PlayerSession::HandleGetRankings(tagNetCmd* pCmd)
{
	MGET_MSG(pRecv, pCmd, NC_GetRankingInfo);

	DWORD dwRet = E_Success;
	if ( pRecv->eType == ERT_Strength )
	{
		dwRet = HandleGetStrengthRankings(pRecv->nPage);
	}
	else if ( pRecv->eType == ERT_Fame )
	{
		dwRet = HandleGetFameRankings(pRecv->nPage);
	}
	else if ( pRecv->eType == ERT_Level )
	{
		dwRet = HandleGetLevelRankings(pRecv->nPage);
	}

	return dwRet;
}

DWORD PlayerSession::HandleGetStrengthRankings( INT nPage )
{
	INT nStartNum = nPage*20 + 1;

	Role* pRole = GetRole();
	if(!P_VALID(pRole)) return GT_INVALID;

	tagNS_GetRankingInfo send;
	send.nPage = nPage;
	send.nLocalPlayerRank = pRole->GetStrengthNum(); // to do

	TMap<INT, tagStrengthRoleNode*> GlobalRankings = g_StrengthRankings.GetGlobalRankings();
	tagStrengthRoleNode* pStrengthRoleNode = NULL;

	for (INT i = 1; i>=nStartNum && i<=nStartNum+20-1; ++i)
	{
		//tagRankingInfo TmpNode;
		//tagRoleIDName RoleIDName;
		pStrengthRoleNode = GlobalRankings.Peek(i);
		if (!P_VALID(pStrengthRoleNode))
		{
			// 没有该名次的玩家
			/*TmpNode.nCurRanking = 0;
			TmpNode.nRankingChange = 0;
			TmpNode.dwStrength = 0;*/
			//TmpNode.sIDName = RoleIDName;

			//send.sData[i].dwStrength = 0;
			send.sData[i].nCurRanking = 0;
			send.sData[i].nRankingChange = 0;
			send.sData[i].sIDName.dwID = 0;
			ZeroMemory(send.sData[i].sIDName.szName, sizeof(send.sData[i].sIDName.szName));

			break;
		}

		_tcscpy_s( send.sData[i].sIDName.szName, X_SHORT_NAME, pStrengthRoleNode->szRoleName);
		send.sData[i].sIDName.dwID = pStrengthRoleNode->dwRoleID;

		send.sData[i].nCurRanking = pStrengthRoleNode->nNum;
		send.sData[i].nRankingChange = pStrengthRoleNode->nPreNum - pStrengthRoleNode->nNum;
		//send.sData[i].dwStrength = pStrengthRoleNode->dwStrength;
	}

	SendMessage(&send, send.dwSize);

	return E_Success;
}

DWORD PlayerSession::HandleGetFameRankings(INT nPage)
{
	return E_Success;
}

DWORD PlayerSession::HandleGetLevelRankings(INT nPage)
{
	return E_Success;
}

DWORD PlayerSession::HandleGetSelfStrength(tagNetCmd* pCmd)
{
	MGET_MSG(pRecv, pCmd, NC_GetSelfStrength);

	Role* pRole = GetRole();
	if(!P_VALID(pRole)) return GT_INVALID;

	tagNS_GetSelfStrength send;
	send.dwSelfStrength = pRole->GetStrength();
	send.dwHero = pRole->GetHeroism();
	send.dwSmart = pRole->GetWit();
	send.dwChivalrous = pRole->GetErrantry();
	send.dwWarfare = pRole->GetValor();

	SendMessage(&send, send.dwSize);

	return E_Success;
}