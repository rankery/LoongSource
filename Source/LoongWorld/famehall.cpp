//-----------------------------------------------------------------------------
//!\file famehall.cpp
//!\author xlguo
//!
//!\date 2009-02-24
//! last 
//!
//!\brief 名人堂
//!
//!	Copyright (c) 2004 TENGWU Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#include "StdAfx.h"

#include "db_session.h"
#include "att_res.h"
#include "clandata.h"
#include "famehall.h"
#include "role.h"

#include "../ServerDefine/msg_famehall.h"

FameHall g_fameHall;

// 主线程调用
VOID FameHall::Update()
{
	for (INT32 nClanType = ECLT_BEGIN; nClanType < ECLT_END; ++nClanType)
	{
		m_ArrClanTrunk[nClanType].Update();
	}
}

// 处理声望排名消息
VOID FameHall::HandleUpdateRepRank(tagNetCmd* pCmd)
{
	MGET_MSG(pRecv, pCmd, NDBS_GetRepRankList);
	ASSERT(JDG_VALID(ECLT, pRecv->byClanType)); 
	m_ArrClanTrunk[pRecv->byClanType].HandleUpdateRepRank(pRecv->repRank, pRecv->nRecNum);
}


// 地图线程调用
BOOL FameHall::TryEnterFameHall(Role* pRole, ECLanType eClanType)
{
	return m_ArrClanTrunk[eClanType].TryEnterFameHall(pRole);
}

// 获取名人堂最多前50（nameid）
VOID FameHall::GetMemberTop50(BYTE* pData, ECLanType eClanType)
{
	m_ArrClanTrunk[eClanType].GetFameHallTop50(pData);
}

// 获取名人堂最多前50具体数目
INT32 FameHall::GetMemberTop50Num(ECLanType byClanType)
{
	return m_ArrClanTrunk[byClanType].GetFameHallTop50Num();
}

// 获得声望排名（tagRepRankData）
VOID FameHall::GetRepRankTop(PVOID pData, ECLanType eClanType)
{
	m_ArrClanTrunk[eClanType].GetRepRank(pData);
}

// 获得声望排名大小
INT32 FameHall::GetRepRankNum(ECLanType byClanType)
{
	return m_ArrClanTrunk[byClanType].GetRepRankNum();
}
// 获得已激活氏族珍宝列表
VOID FameHall::GetActClanTreasure(PVOID pData, ECLanType eClanType)
{
	m_ArrClanTrunk[eClanType].GetActTreasureList(pData);
}

// 获得已激活氏族珍宝大小
INT32 FameHall::GetActClanTreasureNum( ECLanType byClanType )
{
	return m_ArrClanTrunk[byClanType].GetActTreasureNum();
}

BOOL FameHall::Init()
{
	for (INT32 nClanType = ECLT_BEGIN; nClanType < ECLT_END; ++nClanType)
	{
		if (FALSE == m_ArrClanTrunk[nClanType].Init((ECLanType)nClanType, ENTER_FAMEHALL_REP_LIM, ENTER_ACTCOUNT_REWARD))
			return FALSE;
	}
	m_bInitOK = FALSE;
	return TRUE;
}

// 发送数据库消息
VOID FameHall::SendLoadDBData()
{
	tagNDBC_FameHallInitStart	fameHallInitStart;

	g_dbSession.Send(&fameHallInitStart, fameHallInitStart.dwSize);
}

// 处理初始化氏族珍宝列表消息
VOID FameHall::HandleInitActTreasureList(tagNetCmd* pCmd)
{
	MGET_MSG(pRecv, pCmd, NDBS_GetActTreasureList);
	INT nOffset = 0;
	for (INT nClanType = ECLT_BEGIN; nClanType < ECLT_END; ++nClanType)
	{
		m_ArrClanTrunk[nClanType].HandleInitActTreasureList(&(pRecv->treasureData[nOffset]), pRecv->nRecNum[nClanType]);
		nOffset += pRecv->nRecNum[nClanType];
		ASSERT( nOffset <= MAX_ACT_TREASURE_NUM );
	}

}

// 初始化名人堂消息
VOID FameHall::HandleInitFameHallTop50(tagNetCmd* pCmd)
{
	MGET_MSG(pRecv, pCmd, NDBS_GetFameHallEnterSnap);
	INT nOffset = 0;
	for (INT nClanType = ECLT_BEGIN; nClanType < ECLT_END; ++nClanType)
	{
		ASSERT ( pRecv->nRecNum[nClanType] <= MAX_ENTER_FAMEHALL_SNAP_NUM );
		m_ArrClanTrunk[nClanType].HandleInitFameHallTop50(&(pRecv->enterSnap[nOffset]), pRecv->nRecNum[nClanType]);
		nOffset += pRecv->nRecNum[nClanType];
	}
	
}

// 初始化声望重置时间
VOID FameHall::HandleInitRepRstTimeStamp(tagNetCmd* pCmd)
{
	MGET_MSG(pRecv, pCmd, NDBS_GetRepResetTimestamp);

	for (INT16 nClanType = ECLT_BEGIN; nClanType < ECLT_END; ++nClanType)
	{
		m_ArrClanTrunk[nClanType].HandleInitRepRstTimeStamp(tagDWORDTime(pRecv->dwResetTime[nClanType]));
	}
	InterlockedExchange((LPLONG)&m_bInitOK, TRUE);
}

// 激活氏族珍宝（保留到主线程）
DWORD FameHall::ActClanTreasure( Role* pRole, UINT16 u16TreasureID )
{
	const tagClanTreasureProto* pTreasureProto = g_attRes.GetClanTreasureProto(u16TreasureID);
	ASSERT( P_VALID(pTreasureProto) );
	return m_ArrClanTrunk[pTreasureProto->eClanType].ActiveClanTreasure(pRole, u16TreasureID);
}

VOID FameHall::RoleRepUpdate( Role* pRole, ECLanType eClanType )
{
	ASSERT(P_VALID(pRole));
	ClanData* pClanData = &pRole->GetClanData();

	BOOL bBeReset = FALSE;

	if (eClanType != ECLT_NULL)
	{
		bBeReset = pClanData->ResetReputation(m_ArrClanTrunk[eClanType].GetEnterSnapUpdateTime(), eClanType, ERL_Legend);
	}
	else
	{
		for (INT nClanType = ECLT_BEGIN; nClanType < ECLT_END; ++nClanType)
		{
			if ( pClanData->ResetReputation(m_ArrClanTrunk[nClanType].GetEnterSnapUpdateTime(), (ECLanType)nClanType, ERL_Legend) )
			{
				bBeReset = TRUE;
			}
		}

	}
	if (bBeReset)
	{
		pClanData->SetRepRstTimeStamp(g_world.GetWorldTime());
	}
}