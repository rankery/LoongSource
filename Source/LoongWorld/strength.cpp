//-------------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: strength.cpp
// author: zhangrong
// actor:
// data: 2009-9-7
// last:
// brief: 实力系统
//-------------------------------------------------------------------------------
#include "stdafx.h"
#include "strength.h"
#include "db_session.h"
#include "role_mgr.h"
#include "world.h"

#include "../ServerDefine/msg_Strength.h"
#include "../ServerDefine/base_define.h"

StrengthRankings g_StrengthRankings;
BOOL StrengthRankings::Init()
{
	tagNDBC_InitStrengthRankings send;
	g_dbSession.Send(&send, send.dwSize);

	return TRUE;
}

VOID StrengthRankings::InitRankings(tagNetCmd* pMsg)
{	
	MGET_MSG(pRecv, pMsg, NDBS_StrengthRankings);
	INT nCount = pRecv->nSum;
	ASSERT(nCount <= X_STRENGTH_RANKINGS_LEN);
	
	m_Lock.Acquire();

	// 清空现有的排行榜
	tagStrengthRoleNode* pTemp = NULL;
	//m_mapRankings.ResetIterator();
	TMap<INT, tagStrengthRoleNode*>::TMapIterator it = m_mapRankings.Begin();

	while (m_mapRankings.PeekNext(it, pTemp))
	{
		SAFE_DEL(pTemp);
	}
	m_mapRankings.Clear();

	// 生成新的排行榜
	for(INT i=1 ; i<=nCount; ++i)
	{
		tagStrengthRoleNode* pRankingsNode = new tagStrengthRoleNode;
		pRankingsNode->nNum = pRecv->Rankings[i].nNum;
		pRankingsNode->nPreNum = pRecv->Rankings[i].nPreNum;
		_tcscpy(pRankingsNode->szRoleName, pRecv->Rankings[i].szRoleName);
		pRankingsNode->dwRoleID = g_roleMgr.GetRoleIDByNameCrc(IUTIL->Crc32(pRecv->Rankings[i].szRoleName));
		pRankingsNode->dwStrength = pRecv->Rankings[i].dwStrength;
		
		m_mapRankings.Add(i, pRankingsNode);
	}

	m_Lock.Release();
}

//VOID StrengthRankings::UpdateWorldRankings()
//{	
//	tagNDBC_UpdateStrengthRankings send;
//	g_dbSession.Send(&send, send.dwSize);
//}

