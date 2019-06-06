 //-----------------------------------------------------------------------------
//!\file famehall_part.cpp
//!\author xlguo
//!
//!\date 2009-02-24
//! last 
//!
//!\brief 名人堂定义
//!
//!	Copyright (c) 2004 TENGWU Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "famehall_part.h"
#include "../WorldDefine/msg_famehall.h"
#include "../ServerDefine/msg_famehall.h"
#include "../ServerDefine/log_cmdid_define.h"

#include "role.h"
#include "role_mgr.h"
#include "clan_treasury.h"
#include "map_creator.h"

#include "item_creator.h"

// 初始化读取所有氏族珍宝结构
BOOL ClanTreasureActList::Init( ECLanType eClantype )
{
	tagClanTreasure* pTreasure					= NULL;
	const tagClanTreasureProto*	pTreasureProto	= NULL;

	m_mapAllTreasure.clear();
	for (UINT16 u16TreasureID = 0; u16TreasureID < CLAN_TREASURE_NUM; ++u16TreasureID)
	{
		pTreasureProto = g_attRes.GetClanTreasureProto(u16TreasureID);
		ASSERT_P_VALID(pTreasureProto);
		if (!P_VALID(pTreasureProto))
			return FALSE;

		if (pTreasureProto->eClanType == eClantype)
		{
			pTreasure					= new tagClanTreasure;

			ASSERT_P_VALID(pTreasure);
			if (!P_VALID(pTreasure))
				return FALSE;

			pTreasure->eState			= ETRS_UNACT;
			pTreasure->pProto			= pTreasureProto;
			m_mapAllTreasure.insert(PairU16ClanTreasure(u16TreasureID, pTreasure));
		}
	}
	m_nActNum = 0;
	return TRUE;
}

// 销毁
VOID ClanTreasureActList::Destroy()
{
	MapU16ClanTreasure::iterator itr = m_mapAllTreasure.begin();
	while(itr != m_mapAllTreasure.end())
	{
		SAFE_DEL(itr->second);
		++itr;
	}
	m_mapAllTreasure.clear();
	m_nActNum = 0;
}

// 珍宝数据获得
VOID	ClanTreasureActList::GetActivatedTreasure(PVOID pData)
{
	tagTreasureData* pTreasure = reinterpret_cast<tagTreasureData*>(pData);
	INT nIndex = 0; 

	for (	MapU16ClanTreasure::iterator itr = m_mapAllTreasure.begin();
			nIndex < m_nActNum && itr != m_mapAllTreasure.end();
			++itr
		)
	{
		const tagClanTreasure* pClanTreasure = itr->second;
		ASSERT_P_VALID(pClanTreasure);
		if (!P_VALID(pClanTreasure))
		{
			continue;
		}

		if (ETRS_ACT == pClanTreasure->eState)
		{
			pTreasure[nIndex].n16TreasureID		= itr->first;
			pTreasure[nIndex].dwNamePrefixID	= pClanTreasure->dwNamePrefixID;

			++nIndex;
		}
	}
	ASSERT( nIndex == m_nActNum );
	for (; nIndex != m_nActNum; ++nIndex)
	{
		pTreasure[nIndex].n16TreasureID		= GT_INVALID;
		pTreasure[nIndex].dwNamePrefixID	= GT_INVALID;
	}
}

// 珍宝数量获得
INT16 ClanTreasureActList::GetActivatedTreasureNum()
{
	if (m_nActNum > (INT)m_mapAllTreasure.size() && m_nActNum < 0)
	{
		ASSERT(0);
		return 0;
	}
	return m_nActNum;	
}

// 尝试激活珍宝
DWORD	ClanTreasureActList::TryActiveTreasure(Role* pRole, UINT16 u16TreasureID, ECLanType eclantype)
{
	DWORD dwRtv = CanActiveTreasure(pRole, u16TreasureID, eclantype);
	if (E_FrameHall_Success == dwRtv)
	{
		return ActiveTreasure(pRole, u16TreasureID, eclantype);
	}
	return dwRtv;
}

// 读取珍宝
VOID	ClanTreasureActList::HandleLoadDBTreasure(tagTreasureData* pTreasureData, const INT32 nNum)
{
	ResetAll();

	for (INT32 nIndex = 0; nIndex < nNum; ++nIndex)
	{
		SetAct(pTreasureData[nIndex].n16TreasureID, pTreasureData[nIndex].dwNamePrefixID);
	}		
}

// 可否激活
DWORD	ClanTreasureActList::CanActiveTreasure(Role* pRole, UINT16 u16TreasureID, ECLanType eclantype)
{
	ClanData* pClanData = &(pRole->GetClanData());

	DWORD dwRtv = E_FrameHall_Success;
	MapU16ClanTreasure::iterator itr = m_mapAllTreasure.find(u16TreasureID);
	
	// 检查宝物状态
	ASSERT_P_VALID(pClanData);
	if (!P_VALID(pClanData))
	{
		dwRtv = E_FrameHall_UnknownError;
	}
	else if (m_mapAllTreasure.end() == itr)
	{
		// 不是氏族珍宝
		dwRtv = E_FrameHall_ItemIsNotTreasure;
	}
	else if (ETRS_ACT == itr->second->eState)
	{
		// 已被激活
		dwRtv = E_FrameHall_TreasureAlreadyAct;
	}
	else if (pClanData->ActCountGetVal(eclantype) <= 0)
	{
		// 检查激活次数
		dwRtv = E_FrameHall_ActCountNotEnough;
	}
	else if (pClanData->ClanConGetVal(eclantype) < itr->second->pProto->nActClanConNeed)
	{
		// 氏族贡献满足
		dwRtv = E_FrameHall_ClanColNotEnough;
	}
	return dwRtv;
}

// 激活
DWORD	ClanTreasureActList::ActiveTreasure(Role* pRole, UINT16 u16TreasureID, ECLanType eclantype)
{
	MapU16ClanTreasure::iterator itr = m_mapAllTreasure.find(u16TreasureID);
	ASSERT( itr != m_mapAllTreasure.end());
	if (itr == m_mapAllTreasure.end())
	{
		return E_FrameHall_UnknownError;
	}

	const tagClanTreasure* pClanTreasure = itr->second;
	ASSERT_P_VALID(pClanTreasure);
	if (!P_VALID(pClanTreasure))
	{
		return E_FrameHall_UnknownError;
	}

	ASSERT_P_VALID(pRole);
	if (!P_VALID(pRole))
	{
		return E_FrameHall_UnknownError;
	}
	ClanData& clanData = pRole->GetClanData();

	// 设置消耗氏族贡献
	clanData.ClanConDec(pClanTreasure->pProto->nActClanConNeed, eclantype, TRUE);

	// 设置消耗激活次数
	BOOL bRtv = clanData.ActCountDec(eclantype);
	ASSERT( bRtv );
	if (bRtv <= 0)
	{
		return E_FrameHall_UnknownError;
	}

	DWORD dwNameID	= pRole->GetNameID();
	DWORD dwTypeID	= pClanTreasure->pProto->dwTypeID;

	// 激活
	SetAct(u16TreasureID, dwNameID);

	// 给东西
	GiveRoleItem(dwNameID, pRole, dwTypeID);

	// 插入数据库
	tagNDBC_InsertActClanTreasure	sendDb;
	sendDb.dwActNameID	= dwNameID;
	sendDb.dwRoleID		= pRole->GetID();
	sendDb.u16TreasureID	= u16TreasureID;
	sendDb.byClanType		= eclantype;
	sendDb.dwActTime		= GetCurrentDWORDTime();
	g_dbSession.Send(&sendDb, sendDb.dwSize);

	// 广播
	tagNS_NewActivedTreasure		sendC;
	sendC.dwNameID		= dwNameID;
	sendC.n16TreasureID	= u16TreasureID;
	g_roleMgr.SendWorldMsg(&sendC, sendC.dwSize);

	return E_FrameHall_Success;
}

// 设置激活（用于初始化）
VOID	ClanTreasureActList::SetAct(UINT16 u16TreasureID, DWORD dwNamePrefixID)
{
	MapU16ClanTreasure::iterator itr = m_mapAllTreasure.find(u16TreasureID);
	ASSERT( itr != m_mapAllTreasure.end() );
	if (itr == m_mapAllTreasure.end())
		return;
	
	tagClanTreasure* pClanTreasure = itr->second;
	ASSERT_P_VALID( pClanTreasure);
	if (!P_VALID(pClanTreasure))
		return;

	ASSERT(ETRS_UNACT == pClanTreasure->eState);
	const tagClanTreasureProto* pTreasureProto = pClanTreasure->pProto;

	pClanTreasure->eState			= ETRS_ACT;
	pClanTreasure->dwNamePrefixID	= dwNamePrefixID;
	
	//tbc:添加到氏族宝库中
	Map*	pMap = NULL;
	Shop*	pShop = NULL;

	if (P_VALID(pMap= g_mapCreator.GetMap(pTreasureProto->dwMapID, GT_INVALID)) &&
		P_VALID(pShop = pMap->GetShop(pTreasureProto->dwNpcID)))
	{
		BOOL bRtv = pShop->ActiveTreasure(pTreasureProto->dwTypeID, dwNamePrefixID);
		ASSERT(bRtv);
	}


	++m_nActNum;
}

// 重置所有
VOID	ClanTreasureActList::ResetAll()
{
	
	for (MapU16ClanTreasure::iterator itr = m_mapAllTreasure.begin();
		itr != m_mapAllTreasure.end();
		++itr)
	{
		itr->second->eState	= ETRS_UNACT;
		itr->second->pProto	= g_attRes.GetClanTreasureProto(itr->first);
	}
	m_nActNum = 0;
}

// 发给玩家已激活的物品
VOID ClanTreasureActList::GiveRoleItem( DWORD dwNameID, Role* pRole, DWORD dwTypeID )
{
	tagItem* pActItem = ItemCreator::CreateTreasure(dwNameID, EICM_ActTreasure, pRole->GetID(), dwTypeID);
	if (!P_VALID(pActItem))
	{
		return;
	}

	DWORD dwRtv = pRole->GetItemMgr().Add2Bag(pActItem, ELCLD_ACT_TREASURE, TRUE);
	if(E_Success != dwRtv)
	{
		SAFE_DEL(pActItem);
		return;
	}
}

// tbc：生成nameid
DWORD ClanTrunk::GetNameID( DWORD dwRoleID )
{
	Role* pRole = g_roleMgr.GetRolePtrByID(dwRoleID);
	if (!P_VALID(pRole))
	{
		return GT_INVALID;
	}
	return pRole->GetNameID();
}

// 尝试添加角色
VOID FameRoleTracker::TrackRole( DWORD dwRoleID, ECLanType eClanType )
{
	// 标记为名人堂成员
	MarkFameRole(dwRoleID, eClanType);

	// 记录快照
	if ( !IsSnapFull() && !IsSnapAdded(dwRoleID, ClanTrunk::GetNameID(dwRoleID)) )
	{
		AddRoleSnap(dwRoleID, eClanType);
	}
}

// 尝试添加角色
BOOL FameRoleTracker::IsSnapAdded(DWORD dwRoleID, DWORD dwNameID)
{
	tagFameHallEnterSnap member;
	member.dwEnterNameID = dwNameID;
	member.dwRoleID = dwRoleID;
	VecFameMember::iterator itr = find(m_vecAllMembers.begin(), m_vecAllMembers.end(), member);
	return itr != m_vecAllMembers.end();
}

// 获得排名靠前的成员
INT32 FameRoleTracker::GetTopSnap(BYTE* pData, INT32 nNum)
{
	DWORD* pMember = reinterpret_cast<DWORD*>(pData);

	VecFameMember::iterator itr = m_vecAllMembers.begin();
	INT32 nMemberIndex = 0;
	for (; nMemberIndex < nNum && itr != m_vecAllMembers.end(); ++nMemberIndex, ++itr)
	{
		pMember[nMemberIndex] = itr->dwEnterNameID;
	}
	return nMemberIndex;
}

// 添加角色
VOID FameRoleTracker::AddRoleSnap( DWORD dwRoleID, ECLanType eClanType )
{
	tagFameHallEnterSnap member;
	// tbd: 名称id
	member.dwEnterNameID	= ClanTrunk::GetNameID(dwRoleID);
	member.dwRoleID			= dwRoleID;
	member.dwtEnterTime		= GetCurrentDWORDTime();
	m_vecAllMembers.push_back(member);

	// 数据库记录
	tagNDBC_InsertFameHallSnap snapSend;
	snapSend.dwEnterNameID	= member.dwEnterNameID;
	snapSend.dwEnterTime	= member.dwtEnterTime;
	snapSend.dwRoleID		= member.dwRoleID;
	snapSend.byClanType		= eClanType;
	g_dbSession.Send(&snapSend, snapSend.dwSize);
}
BOOL FameRoleTracker::IsSnapFull()
{
	return m_vecAllMembers.size() >= 50 ;
}
VOID FameRoleTracker::MarkFameRole( DWORD dwRoleID, ECLanType eClanType )
{
	Role* pRole = g_roleMgr.GetRolePtrByID(dwRoleID);
	ASSERT_P_VALID(pRole);
	if (!P_VALID(pRole))
		return;

	// 设置为名人
	pRole->GetClanData().SetFame(eClanType);
}

// 初始化加载成员角色
VOID FameRoleTracker::LoadInitFameHallEnterSnap(tagFameHallEnterSnap* pEnterSnap, const INT32 nNum)
{
	m_vecAllMembers.clear();
	m_vecAllMembers.resize(nNum);
	for (INT32 nIndex = 0; nIndex < nNum; ++nIndex)
	{
		m_vecAllMembers[nIndex] = pEnterSnap[nIndex];
	}
}

// 是否是名人
BOOL FameRoleTracker::IsFameRole( DWORD dwRoleID, ECLanType eClanType )
{
	Role* pRole = g_roleMgr.GetRolePtrByID(dwRoleID);
	ASSERT_P_VALID(pRole);
	if (!P_VALID(pRole))
	{
		return FALSE;
	}
	return pRole->GetClanData().IsFame(eClanType);
}

// 获得成员数量
INT32 FameRoleTracker::GetTopSnapNum(INT32 nNum) const 
{	
	INT32 nRtNum = m_vecAllMembers.size();
	return nRtNum >= nNum ? nNum : nRtNum;
}

// 尝试进入名人堂
BOOL ClanTrunk::TryEnterFameHall(Role* pRole)
{
	BOOL bEnter = FALSE;
	m_EnterLock.Acquire();		
	if (!P_VALID(m_pRoleEnter) && pRole->GetClanData().RepGetVal(m_eClanType) >= m_nEnterFameHallRepLim)
	{
		bEnter = TRUE;
		m_pRoleEnter = pRole;
	}
	m_EnterLock.Release();
	return bEnter;
}	

// 得到名人堂排名前50
VOID ClanTrunk::GetFameHallTop50( BYTE* pData )
{
	m_FameRoleTracker.GetTopSnap(pData, 50);
}


// 得到名人堂当前尺寸
INT32 ClanTrunk::GetFameHallTop50Num()
{
	return m_FameRoleTracker.GetTopSnapNum(50);
}

// 得到氏族声望排名
VOID ClanTrunk::GetRepRank(PVOID pData)
{
	tagRepRankData* pRankData = reinterpret_cast<tagRepRankData*>(pData);
	INT32 nNum = m_vecRepRank.size();
	for (INT32 nIndex = 0; nIndex < nNum; ++nIndex)
	{
		pRankData[nIndex] = m_vecRepRank[nIndex];
	}
}

// 得到氏族声望排名人数
INT32 ClanTrunk::GetRepRankNum()
{
	return m_vecRepRank.size();
}

// 更新声望排名
VOID ClanTrunk::HandleUpdateRepRank(tagRepRankData* pRepOrderData, const INT32 nNum)
{
	m_vecRepRank.clear();
	for (INT32 nIndex = 0; nIndex < nNum; ++nIndex)
	{
		m_vecRepRank.push_back(pRepOrderData[nIndex]);
	}
	m_dwtRepRankUpdateTime = GetCurrentDWORDTime();
}

// 初始化名人堂排名
VOID ClanTrunk::HandleInitFameHallTop50(tagFameHallEnterSnap* pEnterSnap, const INT32 nNum)
{	
	m_FameRoleTracker.LoadInitFameHallEnterSnap(pEnterSnap, nNum);
}

// 初始化氏族珍宝
VOID ClanTrunk::HandleInitActTreasureList(tagTreasureData* pTreasure, const INT32 nNum)
{
	m_ClanTreasure.HandleLoadDBTreasure(pTreasure, nNum);
}

BOOL ClanTrunk::Init( ECLanType eClanType, INT32 nEnterLim, INT8 n8ActCount )
{
	m_nRepRankUpdateTickCounter = TICK_PER_SECOND * 10;
	m_eClanType = eClanType;
	m_nEnterFameHallRepLim = nEnterLim;
	m_n8ActCountReward = n8ActCount;
	m_dwtRepRankUpdateTime		= 0;
	m_dwtRepResetTime		= 0;
	return m_ClanTreasure.Init(m_eClanType);
}

// 激活氏族珍宝
DWORD ClanTrunk::ActiveClanTreasure(Role* pRole, UINT16 u16TreasureID)
{
	return m_ClanTreasure.TryActiveTreasure(pRole, u16TreasureID, m_eClanType);
}

// 获得氏族珍宝列表
VOID ClanTrunk::GetActTreasureList(PVOID pData)
{
	m_ClanTreasure.GetActivatedTreasure(pData);
}

// 获得氏族珍宝尺寸
INT32 ClanTrunk::GetActTreasureNum()
{
	return m_ClanTreasure.GetActivatedTreasureNum();
}

// 更新进入名人堂
BOOL ClanTrunk::UpdateEnter()
{
	if (CanEnter())
		return Enter();
	return FALSE;
}

// 更新声望排名
VOID ClanTrunk::UpdateRepRank()
{
	if (--m_nRepRankUpdateTickCounter <= 0)
	{
		m_nRepRankUpdateTickCounter = REP_ORDER_UPDATE_INTERVAL_TICKS;

		tagNDBC_GetRepRankList send;
		send.byClanType = m_eClanType;
		g_dbSession.Send(&send, send.dwSize);
	}
}

// 进入名人堂
BOOL ClanTrunk::Enter()
{
	ASSERT_P_VALID(m_pRoleEnter);
	if (!P_VALID(m_pRoleEnter))
		return FALSE;

	BOOL bEnter = TRUE;
	ClanData* pClanData = &(m_pRoleEnter->GetClanData());

	// 添加
	m_FameRoleTracker.TrackRole(m_pRoleEnter->GetID(), m_eClanType);

	// 角色获得激活珍宝次数
	pClanData->ActCountSetVal(m_n8ActCountReward, m_eClanType);

	m_pRoleEnter = NULL;

	// 重置所有声望为传奇玩家的声望值(效率原因，决定在主线程中定时update)
	m_dwtRepResetTime = GetCurrentDWORDTime();

	tagNDBC_UpDateRepRstTime send;
	send.byClanType			= static_cast<BYTE>(m_eClanType);
	send.dwRepRstTimeStamp	= m_dwtRepResetTime;
	g_dbSession.Send(&send, send.dwSize);

	g_roleMgr.ResetRoleRep(m_eClanType, ERL_Legend, m_dwtRepResetTime);

	return TRUE;
}

// 能否进入名人堂
BOOL ClanTrunk::CanEnter()
{
	return P_VALID(m_pRoleEnter);
}