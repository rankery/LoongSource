//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: map_instance.h
// author: wjing
// actor:
// data: 2008-1-15
// last:
// brief: 普通副本类
//-----------------------------------------------------------------------------
#pragma once

#pragma once
#include "map.h"
#include "../ServerDefine/instance_define.h"

class Team;

//-----------------------------------------------------------------------------
// CONST
//-----------------------------------------------------------------------------
const INT ROLE_LEAVE_INSTANCE_TICK_COUNT_DOWN	= 60 * TICK_PER_SECOND;		// 角色在副本内离开队伍时，传送出副本的倒计时，60秒


//-----------------------------------------------------------------------------
// 普通副本类，用于玩家或队伍创建的副本，奇遇副本等等
//-----------------------------------------------------------------------------
class MapInstanceNormal : public MapInstance
{
public:
	MapInstanceNormal();
	virtual ~MapInstanceNormal();

	virtual BOOL		Init(const tagMapInfo* pInfo, DWORD dwInstanceID, Role* pCreator=NULL, DWORD dwMisc=GT_INVALID);
	virtual VOID		Update();
	virtual VOID		Destroy();

	virtual VOID		AddRole(Role* pRole);
	virtual	VOID		RoleLeaveMap(DWORD dwID, BOOL bChgMap);
	virtual INT			CanEnter(Role *pRole);
	virtual BOOL		CanDestroy();
	virtual VOID		OnDestroy();

	DWORD				GetCreatorID()			{ return m_dwCreatorID; }
	EInstanceHardMode	GetInstanceHard()		{ return m_eInstanceHardMode; }
	DWORD				CalTimeLimit();
	const tagInstance*	GetInstanceProto()		{ return m_pInstance; }

	VOID				OnTeamCreate(const Team* pTeam);
	VOID				OnTeamDelete(const Team* pTeam);
	VOID				OnRoleLeaveTeam(DWORD dwRoleID, const Team* pTeam);
	VOID				OnRoleEnterTeam(DWORD dwRoleID, const Team* pTeam);

protected:
	virtual VOID		OnDelete();
	virtual BOOL		InitAllSpawnPointCreature();
	
private:
	VOID				UpdateTimedIssue();
	BOOL				IsTimeLimit()		{ return m_pInstance->dwTimeLimit > 0 && m_pInstance->dwTargetLimit != GT_INVALID; }

	BOOL				RecalHardMode();
	BOOL				GetCreatureBaseLevel(INT& nBaseLevel);
	DWORD				CalCreatureTypeID(const tagRandSpawnPointInfo* pRandSpawnPoint);
	DWORD				TransmitBigID(INT nBaseLevel, tagMapSpawnPointInfo* pMapSpawnInfo);

private:
	BOOL					m_bNoEnter;						// 副本是否还没人进入过
	DWORD					m_dwCreatorID;					// 副本创建者ID
	DWORD					m_dwTeamID;						// 创建副本的小队ID
	DWORD					m_dwStartTick;					// 开始时间
	DWORD					m_dwEndTick;					// 副本开始重置倒计时
	EInstanceHardMode		m_eInstanceHardMode;			// 副本难度

	TMap<DWORD, INT>		m_mapWillOutRoleID;				// 不再属于这个副本等待传输出去的玩家列表

	const tagInstance*		m_pInstance;					// 副本静态属性
};


