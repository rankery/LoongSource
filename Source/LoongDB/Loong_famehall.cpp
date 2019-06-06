//-----------------------------------------------------------------------------
//!\file Loong_famehall.cpp
//!\author xlguo
//!
//!\date 2009-03-13
//! last 
//!
//!\brief 名人堂数据库处理
//!
//!	Copyright (c) 2004 TENGWU Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "Loong.h"
#include "LoongDB.h"
#include "../ServerDefine/role_data.h"
#include "../ServerDefine/msg_famehall.h"
//-------------------------------------------------------------------------------------------------------
// 读取声望排名
//-------------------------------------------------------------------------------------------------------
DWORD CLoong::LoadRepRank(DWORD pDBMsg, DWORD dwDummy)
{
	MGET_MSG(pRecv, pDBMsg, NDBC_GetRepRankList);

	tagNDBS_GetRepRankList Send;
	Send.dwErrorCode = E_Success;
	Send.byClanType = pRecv->byClanType;

	INT32 nRecNum = 0;
	if(!m_pDB->LoadRepRank(pRecv->byClanType, reinterpret_cast<PVOID>(Send.repRank), nRecNum))
	{
		Send.dwErrorCode = E_DBLoad_RepRankData_Failed;
	}
	Send.nRecNum = nRecNum;

	// 重新计算大小
	Send.dwSize += (Send.nRecNum - MAX_REP_RANK_NUM) * sizeof(tagRepRankData);

	// 反馈给游戏服务器
	SendMsg( &Send, Send.dwSize);

	return 0;
}



//-------------------------------------------------------------------------------------------------------
// 名人堂初始化消息处理
//-------------------------------------------------------------------------------------------------------
DWORD CLoong::LoadFameHallInit(DWORD pDBMsg, DWORD dwDummy)
{
	PVOID	pData = NULL;
	INT		nTotalNum = 0;

	tagNDBS_GetActTreasureList		actTreasureLists;
	pData = reinterpret_cast<PVOID>(&actTreasureLists);
	m_pDB->LoadActTreasures(pData, nTotalNum);
	actTreasureLists.dwSize += (nTotalNum - MAX_ACT_TREASURE_NUM) * sizeof(tagTreasureData);
	SendMsg( &actTreasureLists, actTreasureLists.dwSize);

	tagNDBS_GetFameHallEnterSnap	enterSnaps;
	pData = reinterpret_cast<PVOID>(&enterSnaps);
	m_pDB->LoadFameHallSnaps(pData, nTotalNum);
	enterSnaps.dwSize += (nTotalNum - MAX_ENTER_FAMEHALL_SNAP_NUM * ECLT_NUM) * sizeof(tagFameHallEnterSnap);
	SendMsg( &enterSnaps, enterSnaps.dwSize);

	for (INT nClanType = ECLT_BEGIN; nClanType < ECLT_END; ++nClanType)
	{	
		tagNDBS_GetRepRankList			repRankLists;
		pData = reinterpret_cast<PVOID>(&repRankLists.repRank);

		m_pDB->LoadRepRank(nClanType, pData, nTotalNum);
		repRankLists.nRecNum = nTotalNum;
		repRankLists.byClanType	= nClanType;
		repRankLists.dwSize += (repRankLists.nRecNum - MAX_REP_RANK_NUM) * sizeof(tagRepRankData);

		SendMsg( &repRankLists, repRankLists.dwSize);
	}

	tagNDBS_GetRepResetTimestamp	timeStamps;//famehall收到此条消息，初始化结束
	pData = reinterpret_cast<PVOID>(&timeStamps);
	m_pDB->LoadRepRstTimeStamp(pData);
	SendMsg( &timeStamps, timeStamps.dwSize);

	return 0;
}


//-------------------------------------------------------------------------------------------------------
// 激活氏族珍宝
//-------------------------------------------------------------------------------------------------------
DWORD CLoong::InsertActTreasure(DWORD pDBMsg, DWORD dwDummy)
{
	return m_pDB->InsertActTreasure(reinterpret_cast<PVOID>(pDBMsg));
}

//-------------------------------------------------------------------------------------------------------
// 进入名人堂
//-------------------------------------------------------------------------------------------------------
DWORD CLoong::InsertFameHallEnterSnap(DWORD pDBMsg, DWORD dwDummy)
{
	return m_pDB->InsertFameHallEnterSnap(reinterpret_cast<PVOID>(pDBMsg));
}

//-------------------------------------------------------------------------------------------------------
// 更新声望重置时间戳
//-------------------------------------------------------------------------------------------------------
DWORD CLoong::UpdateRepRstTime(DWORD pDBMsg, DWORD dwDummy)
{
	MTRANS_POINTER(p, pDBMsg, tagNDBC_UpDateRepRstTime);
	return m_pDB->UpdateRepRstTime(p->byClanType, p->dwRepRstTimeStamp);
}