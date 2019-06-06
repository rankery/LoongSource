//-------------------------------------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: team.h
// author: wjiang
// actor:
// data: 2008-12-10
// last:
// brief: 队伍类
//-------------------------------------------------------------------------------------------------------
#pragma once

#include "../WorldDefine/group_define.h"

#include <list>
#include "mutex.h"
#include "role.h"

class Creature;

class Team
{
	friend class GroupMgr;

public:
	//---------------------------------------------------------------------------------------------------
	// CONSTRUCT
	//---------------------------------------------------------------------------------------------------
	Team(DWORD dwTeamID, Role* pLeader, Role* pFirstMem);
	~Team();

	//---------------------------------------------------------------------------------------------------
	// 各种Get
	//---------------------------------------------------------------------------------------------------
	DWORD			GetTeamID()	const							{ return m_dwTeamID; }
	DWORD			GetGroupID() const							{ return m_dwGroupID; }
	INT				GetMemNum()	const							{ return m_nMemNum; }
	DWORD			GetTeamSynTick() const						{ return m_dwTeamSynTick; }
	INT				GetTeamMaxLevel() const						{ return m_nMaxLevel; }
	INT				GetTeamMinLevel() const						{ return m_nMinLevel; }
	INT				GetTeamAvgLevel() const;
	Role*			GetMemPtr(INT nIndex) const; 
	DWORD			GetMemID(INT nIndex) const;
	BOOL			IsLeader(DWORD dwRoleID) const				{ return m_dwMemID[0] == dwRoleID; }
	VOID			ExportAllMemID(DWORD dwMemID[MAX_TEAM_NUM]) const;
	BOOL			IsNeedDelete()								{ return m_bNeedDelete; }

	EPickMode		GetPickMode() const							{ return m_ePickMode; }
	FLOAT			GetExpFactor() const						{ return m_fExpFactor; }
	Role*			GetTeamPickRole(Creature* pCreature) const;

	DWORD			GetOwnInstanceMapID() const					{ return m_dwOwnMapID; }
	DWORD			GetOwnInstanceID() const					{ return m_dwOwnInstanceID; }


	//---------------------------------------------------------------------------------------------------
	// 各种Set
	//---------------------------------------------------------------------------------------------------
	VOID			SetMaxLevel(INT nLevel)						{ m_nMaxLevel = nLevel; }
	VOID			SetMinLevel(INT nLevel)						{ m_nMinLevel = nLevel; }
	VOID			SetSynTick(DWORD dwTick)					{ m_dwTeamSynTick = dwTick; }

	VOID			SetGroupID(DWORD dwGroupID)					{ m_dwGroupID = dwGroupID; }

	VOID			SetOwnInstanceID(DWORD dwInstanceID) const	{ m_dwOwnInstanceID = dwInstanceID; }
	VOID			SetOwnInstanceMapID(DWORD dwMapID) const	{ m_dwOwnMapID = dwMapID; }

	VOID			CalExpFactor();

	//---------------------------------------------------------------------------------------------------
	// 队伍功能相关
	//---------------------------------------------------------------------------------------------------
	INT				AddMember(Role* pInviter, Role* pReplyer);
	INT				KickMember(Role* pSrc, Role* pDest);
	INT				LeaveTeam(Role* pRole);
	INT				SetPickMode(Role* pRole, EPickMode ePickMode);
	INT				ChangeLeader(Role* pSrc, Role* pDest);
	
	VOID			OnMemLevelChange(Role* pRole);
	VOID			OnLeaderReinChange(Role* pRole);

	VOID			SendTeamMesInSameMap(DWORD dwMapID, LPVOID pMsg, DWORD dwSize);

	template<typename UnitOperation>
	VOID			ForEachRoleInTeam( UnitOperation oper ) const;

	static VOID			BroadcastLootItemMsgInTeam(Role * pOwner,tagItem * pItem);
	static VOID			BroadcastLootItemMsgInTeamExceptPicker(Role * pPicker,tagItem * pItem);
private:
	//----------------------------------------------------------------------------------------------------
	// 队伍消息相关
	//----------------------------------------------------------------------------------------------------
	VOID			SendTeamInstanceNotigy(Role* pRole, LPVOID pMsg, DWORD dwSize);
	VOID			SendTeamMessage(LPVOID pMsg, DWORD dwSize);

	VOID			SendTeamMessageExceptSomeone(LPVOID pMsg, DWORD dwSize,Role* pExpRole);

	VOID			SendTeamateMessage(DWORD dwRoleID, LPVOID pMsg, DWORD dwSize);
	VOID            SendTeamMesOutBigVis(Role* pRole, LPVOID pMsg, DWORD dwSize);
	VOID			SendRoleInitStateToTeam(Role* pNewMem);
	VOID			SendTeamState(Role* pNewMem);
	VOID			UpdateTeamatePos();

	//----------------------------------------------------------------------------------------------------
	// 功能函数
	//----------------------------------------------------------------------------------------------------
	INT				IsRoleInTeam(DWORD dwMem) const;
	VOID			DeleteMem(const INT nIndex);
	VOID			ReCalTeamLevel();

	INT				CanAddMember(Role* pInviter, Role* pReplyer);
	INT				CanKickMember(Role* pSrc, Role* pDest, INT& nIndex);
	INT				CanLeaveTeam(Role* pRole, INT& nIndex);

	//----------------------------------------------------------------------------------------------------
	// 事件函数
	//----------------------------------------------------------------------------------------------------
	VOID			OnCreate();
	VOID			OnDelete();
	VOID			OnAddMember(Role* pRole);
	VOID			OnDelMember(Role* pRole);

private:
	DWORD				m_dwTeamID;					// 小队ID
	DWORD				m_dwGroupID;				// 所属团队ID
	INT					m_nMemNum;					// 成员数量
	DWORD				m_dwMemID[MAX_TEAM_NUM];	// 组员ID(第一个是队长)
	Role*				m_pMemPtr[MAX_TEAM_NUM];	// 组员指针
	DWORD				m_dwTeamSynTick;			// 小队玩家同步位置的Tick(每5秒同步一次)

	INT					m_nMaxLevel;				// 小队成员最高等级
	INT					m_nMinLevel;				// 小队成员最低等级

	mutable DWORD		m_dwOwnMapID;				// 小队创建的副本地图ID
	mutable DWORD		m_dwOwnInstanceID;			// 小队创建的副本ID

	EPickMode			m_ePickMode;				// 拾取模式
	FLOAT				m_fExpFactor;				// 杀怪经验因数
	mutable list<INT>	m_nPickList;				// 顺序拾取列表
	mutable Mutex		m_Lock;						// 拾取锁（有点搓，看能不能再想个好办法）

	BOOL				m_bNeedDelete;				// 是否需要删除
};

//-------------------------------------------------------------------------------------------
// 得到队员指针
//-------------------------------------------------------------------------------------------
inline Role* Team::GetMemPtr(INT nIndex) const 
{ 
	ASSERT(nIndex < MAX_TEAM_NUM && nIndex >= 0);	
	return m_pMemPtr[nIndex]; 
}

//--------------------------------------------------------------------------------------------
// 得到队员ID
//--------------------------------------------------------------------------------------------
inline DWORD Team::GetMemID(INT nIndex) const
{
	ASSERT(nIndex < MAX_TEAM_NUM && nIndex >= 0);	
	return m_dwMemID[nIndex];	
}

//---------------------------------------------------------------------------------------------
// 玩家是否在该队伍中，如果在，返回在队伍中的索引，否则返回GT_INVALID
//---------------------------------------------------------------------------------------------
inline INT Team::IsRoleInTeam(DWORD dwMemID) const
{
	for(INT i = 0; i < m_nMemNum; ++i)
	{
		if( m_dwMemID[i] == dwMemID )
			return i;
	}
	return GT_INVALID;
}

//---------------------------------------------------------------------------------------------
// 对于队伍中每个各成员执行UnitOperation
//---------------------------------------------------------------------------------------------
template<typename UnitOperation>
VOID Team::ForEachRoleInTeam( UnitOperation oper ) const
{
	for(INT i = 0; i < m_nMemNum; ++i)
	{
		if( !P_VALID(m_pMemPtr[i]) )
			break;
		oper(m_pMemPtr[i]);
	}
}