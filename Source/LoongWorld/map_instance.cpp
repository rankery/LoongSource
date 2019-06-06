//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: map_instance.cpp
// author: wjiang
// actor:
// data: 2008-1-15
// last:
// brief: 普通副本地图
//-----------------------------------------------------------------------------
#include "StdAfx.h"

#include "../WorldDefine/msg_map.h"
#include "../WorldDefine/MapAttDefine.h"

#include "map_instance.h"
#include "map_creator.h"
#include "team.h"
#include "group_mgr.h"
#include "att_res.h"
#include "role.h"
#include "role_mgr.h"
#include "creature.h"
#include "map_mgr.h"
#include "NPCTeam.h"
#include "NPCTeam_mgr.h"


MapInstanceNormal::MapInstanceNormal() : MapInstance(), m_pInstance(NULL), m_dwCreatorID(GT_INVALID),
										 m_dwTeamID(GT_INVALID), m_dwStartTick(GT_INVALID), m_dwEndTick(GT_INVALID),
										 m_eInstanceHardMode(EIHM_NULL), m_bNoEnter(TRUE)
{
}

MapInstanceNormal::~MapInstanceNormal()
{
	Destroy();
}

//------------------------------------------------------------------------------------------------------
// 初始化
//------------------------------------------------------------------------------------------------------
BOOL MapInstanceNormal::Init(const tagMapInfo* pInfo, DWORD dwInstanceID, Role* pCreator, DWORD dwMisc)
{
	ASSERT( P_VALID(pInfo) );
	ASSERT( EMT_Instance == pInfo->eType );

	if( !P_VALID(pCreator) ) return FALSE;	// 必须要有创建者的

	// 读取副本静态属性
	m_pInstance = g_attRes.GetInstanceProto(pInfo->dwID);
	if( !P_VALID(m_pInstance) )	return FALSE;

	// 地图相关属性
	m_pUtil = "Util";
	m_pInfo = pInfo;
	m_mapSession.Clear();
	m_mapRole.Clear();
	m_mapShop.Clear();
	m_mapCofC.Clear();
	m_mapGroundItem.Clear();

	// 副本相关属性
	m_dwID = m_pInfo->dwID;
	m_dwInstanceID = dwInstanceID;
	m_dwCreatorID = pCreator->GetID();
	m_eInstanceHardMode = (EInstanceHardMode)dwMisc;
	m_dwStartTick = g_world.GetWorldTick();
	m_dwEndTick = GT_INVALID;
	m_dwTeamID = pCreator->GetTeamID();

	m_pTeamMgr = new NPCTeamMgr(this);
	if(!P_VALID(m_pTeamMgr))
		return FALSE;

	// 载入导航图数据（Clone出来）
	NavMap* pNavMap = g_mapCreator.GetInstanceTemplate(pInfo->dwID);
	if( !P_VALID(pNavMap) ) return FALSE;
	m_pNav = new NavMap;
	pNavMap->Clone(m_pNav);
	if( !P_VALID(m_pNav) ) return FALSE;

	// 根据mapinfo来初始化地图的怪物，可视列表等信息
	if( FALSE == InitLogicalInfo() )
	{
		SAFE_DEL(m_pNav);
		return FALSE;
	}

	// 都创建成功了，设置玩家或其所在队伍的所属副本ID
	if( P_VALID(m_dwTeamID) )
	{
		const Team* pTeam = g_groupMgr.GetTeamPtr(m_dwTeamID);
		if( P_VALID(pTeam) )
		{
			pTeam->SetOwnInstanceMapID(m_pInfo->dwID);
			pTeam->SetOwnInstanceID(m_dwInstanceID);
		}
	}
	else
	{
		pCreator->SetMyOwnInstanceMapID(m_pInfo->dwID);
		pCreator->SetMyOwnInstanceID(m_dwInstanceID);
	}

	return TRUE;		
}

//---------------------------------------------------------------------------------
// 更新
//---------------------------------------------------------------------------------
VOID MapInstanceNormal::Update()
{
	Map::Update();
	UpdateTimedIssue();
}

//---------------------------------------------------------------------------------
// 时限相关的更新
//---------------------------------------------------------------------------------
VOID MapInstanceNormal::UpdateTimedIssue()
{
	CHECK_TIME();

	// 如果已经处于待删除状态，就不更新了
	if( IsDelete() ) return;

	// 时限副本
	if( IsTimeLimit() && !IsEnd() )
	{
		DWORD dwTick = g_world.GetWorldTick();
		if( (dwTick - m_dwStartTick) >= m_pInstance->dwTimeLimit * TICK_PER_SECOND )
		{
			m_dwEndTick = g_world.GetWorldTick();
			SetEnd();
		}
	}

	// 关闭倒计时
	if( IsEnd() )
	{
		DWORD dwTick = g_world.GetWorldTick();
		if( (dwTick - m_dwEndTick) > m_pInstance->dwEndTime * TICK_PER_SECOND )
		{
			SetDelete();
		}
	}

	// 更新所有待退出的角色的时间
	if( !m_mapWillOutRoleID.Empty() )
	{
		TMap<DWORD, INT>::TMapIterator it = m_mapWillOutRoleID.Begin();
		DWORD dwRoleID = GT_INVALID;
		INT nTick = GT_INVALID;

		while( m_mapWillOutRoleID.PeekNext(it, dwRoleID, nTick) )
		{
			--nTick;	// 减一下倒计时
			if( nTick <= 0 )
			{
				// 时间到了，将玩家传送出去
				Role* pRole = FindRole(dwRoleID);
				if( P_VALID(pRole) )
				{
					MapMgr* pMapMgr = g_mapCreator.GetMapMgr(m_pInfo->dwID);
					if( P_VALID(pMapMgr) )
					{
						pMapMgr->RoleInstanceOut(pRole);
						// Jason 2009-11-26 当队长（队中最后一个人），倒计时结束时，删除副本（置删除标记）。
						// 没有考虑时限
						if( m_dwCreatorID == pRole->GetID  () )
							SetDelete();
					}
				}
				
				m_mapWillOutRoleID.Erase(dwRoleID);
			}
			else
			{
				m_mapWillOutRoleID.ChangeValue(dwRoleID, nTick);
			}
		}
	}
}

//---------------------------------------------------------------------------------
// 销毁
//---------------------------------------------------------------------------------
VOID MapInstanceNormal::Destroy()
{

}

//---------------------------------------------------------------------------------
// 正式加入一个玩家，这只能由管理该地图的MapMgr调用
//---------------------------------------------------------------------------------
VOID MapInstanceNormal::AddRole(Role* pRole)
{
	Map::AddRole(pRole);

	// 重置关闭等待
	if( IsEnd() )
	{
		m_dwEndTick = GT_INVALID;
		m_bEnd = FALSE;
	}

	// 如果是第一个进入副本的玩家
	if( m_bNoEnter )
	{
		m_bNoEnter = FALSE;

		// 如果有队伍，则发送通知队友确认
		if( GT_INVALID != pRole->GetTeamID() && m_pInstance->bNoticeTeamate )
		{
			tagNS_InstanceNofity send;
			pRole->SendMessage(&send, send.dwSize);
		}
	}
	
	// 发送进入副本消息
	tagNS_EnterInstance send;
	send.dwErrorCode = E_Success;
	send.dwTimeLimit = CalTimeLimit();
	pRole->SendMessage(&send, send.dwSize);
}

//---------------------------------------------------------------------------------
// 玩家离开地图，只可能在主线程里面调用
//---------------------------------------------------------------------------------
VOID MapInstanceNormal::RoleLeaveMap(DWORD dwID, BOOL bChgMap)
{
	Map::RoleLeaveMap(dwID, bChgMap);

	// 是否进入等待关闭
	if( m_mapRole.Empty() && !IsEnd() && m_pInstance->dwEndTime != GT_INVALID )
	{
		m_dwEndTick = g_world.GetWorldTick();
		m_bEnd = TRUE;
	}

	// 如果这个玩家在等待离开的列表里，则移除
	m_mapWillOutRoleID.Erase(dwID);
}
//---------------------------------------------------------------------------------
// 是否能进入副本
//---------------------------------------------------------------------------------
INT MapInstanceNormal::CanEnter(Role *pRole)
{
	// 先检测通用判断
	INT nErrorCode = MapInstance::CanEnter(pRole);
	if( E_Success != nErrorCode ) return nErrorCode;

	// 检查人数上限
	if( m_pInstance->nNumUpLimit <= GetRoleNum() )
		return E_Instance_Role_Full;

	// 检测队伍
	if( m_dwTeamID != GT_INVALID )
	{
		if( pRole->GetTeamID() != m_dwTeamID )
			return E_Instance_Not_Same_Team;
	}
	// 检测玩家
	else
	{
		if( pRole->GetID() != m_dwCreatorID )
			return E_Instance_Not_Same_Team;
	}

	return E_Success;
}

//---------------------------------------------------------------------------------
// 是否可以删除
//---------------------------------------------------------------------------------
BOOL MapInstanceNormal::CanDestroy()
{
	return MapInstance::CanDestroy();
}


//---------------------------------------------------------------------------------
// 初始化刷怪点怪物
//---------------------------------------------------------------------------------
BOOL MapInstanceNormal::InitAllSpawnPointCreature()
{
	if( EICM_Appoint == m_pInstance->eInstanceCreateMode )
		return TRUE;

	if( FALSE == RecalHardMode() ) return FALSE;

	DWORD		dwTypeID = GT_INVALID;
	INT			nBaseLevel = 0;
	INT			nLevel = 0;
	Vector3		vec3;

	// 计算随机怪物基本等级
	if( FALSE == GetCreatureBaseLevel(nBaseLevel) )
		return FALSE;

	tagMapSpawnPointInfo* pMapSpawnInfo = NULL;
	const tagRandSpawnPointInfo* pSpawnProto = NULL;
	const tagCreatureProto* pProto = NULL;

	m_pInfo->mapSpawnPoint.ResetIterator();
	while( m_pInfo->mapSpawnPoint.PeekNext(pMapSpawnInfo) )
	{
		DWORD dwSpawnPoint = TransmitBigID(nBaseLevel, pMapSpawnInfo);

		pSpawnProto = g_attRes.GetSpawnPointProto(dwSpawnPoint);
		if( !P_VALID(pSpawnProto) )
		{
			IMSG(_T("Detect a unknown Spawn Point in map, dwSpawnPointID=%u\r\n"), pSpawnProto->dwSpawnPointID);
			continue;
		}

		dwTypeID = CalCreatureTypeID(pSpawnProto);
		if( !P_VALID(dwTypeID) )
		{
			IMSG(_T("Detect a unknown Instance Creature TypeID\n"));
			continue;
		}

		pProto = g_attRes.GetCreatureProto(dwTypeID);
		if( !P_VALID(pProto) )
		{
			IMSG(_T("Detect a unknown creature in map, dwObjID=%u\r\n"), pSpawnProto->dwSpawnPointID);
			continue;
		}

		// 取出一个ID
		DWORD dwID = m_CreatureIDGen.GetNewCreatureID();
		ASSERT( IS_CREATURE(dwID) );

		// 随机一个朝向
		Vector3 vFaceTo;
		FLOAT fYaw = FLOAT(IUTIL->Rand() % 360);
		vFaceTo.x = cosf(fYaw * 3.1415927f / 180.0f);
		vFaceTo.z = sinf(fYaw * 3.1415927f / 180.0f);
		vFaceTo.y = 0.0f;

		// 生成怪物
		Creature* pCreature = Creature::Create(dwID, m_dwID, pProto, pMapSpawnInfo->vPos, vFaceTo, ECST_Load, GT_INVALID, pMapSpawnInfo->bCollide, NULL);

		// 加入到地图中
		AddCreatureOnLoad(pCreature);

		// 如果是巢穴，则加载巢穴怪物
		if( pCreature->IsNest() )
		{
			InitNestCreature(pCreature);
		}

		// 如果是怪物小队，则加载小队怪物
		if( pCreature->IsTeam() )
		{
			InitTeamCreature(pCreature);
		}
	}

	return TRUE;


}

//---------------------------------------------------------------------------------
// 重新计算副本难度
//---------------------------------------------------------------------------------
BOOL MapInstanceNormal::RecalHardMode()
{
	// 不可选副本难度
	if( !m_pInstance->bSelectHard )
	{
		m_eInstanceHardMode = EIHM_Normal;
		return TRUE;
	}
	 // 可选
	else
	{
		switch(m_eInstanceHardMode)
		{
			// 普通
		case EIHM_Normal:
			{
				if( !m_pInstance->bSelectNormal ) return FALSE;
				else return TRUE;
			}
			break;

			// 精英
		case EIHM_Elite:
			{
				if( !m_pInstance->bSelectElite ) return FALSE;
				else return TRUE;
			}
			break;

			// 魔王
		case EIHM_Devil:
			{
				if( !m_pInstance->bSelectDevil ) return FALSE;
				else return TRUE;
			}
			break;

		default:
			return FALSE;
			break;
		}

	}
}

//---------------------------------------------------------------------------------
// 根据副本生成规则，得到随机怪物的基本等级
//---------------------------------------------------------------------------------
BOOL MapInstanceNormal::GetCreatureBaseLevel(INT& nBaseLevel)
{
	if( m_dwTeamID != GT_INVALID && m_pInstance->nNumUpLimit > 1 )
	{
		const Team* pTeam = g_groupMgr.GetTeamPtr(m_dwTeamID);
		if( !P_VALID(pTeam) ) return FALSE;

		switch( m_pInstance->eInstanceCreateMode )
		{
			// 平均等级
		case EICM_AvgLevel:
			{
				nBaseLevel = pTeam->GetTeamAvgLevel();
				return TRUE;
			}
			break;

			// 队长等级
		case EICM_LeaderLevel:
			{
				Role* pLeader = pTeam->GetMemPtr(0);
				if( !P_VALID(pLeader) )
					return FALSE;
				else nBaseLevel = pLeader->GetLevel();
				return TRUE;
			}
			break;

			// 队伍最高等级
		case EICM_MaxLevel:
			{
				nBaseLevel = pTeam->GetTeamMaxLevel();
				return TRUE;
			}
			break;

			// 队伍最低等级
		case EICM_MinLevel:
			{
				nBaseLevel = pTeam->GetTeamMinLevel();
				return TRUE;
			}
			break;

		default:
			return FALSE;
			break;
		}
	}
	else
	{
		Role* pRole = g_roleMgr.GetRolePtrByID(m_dwCreatorID);
		if( !P_VALID(pRole) )
			return FALSE;

		nBaseLevel = pRole->GetLevel();
		return TRUE;
	}
}


//---------------------------------------------------------------------------------
// 根据怪物基本等级得到随机怪物的TypeID
//---------------------------------------------------------------------------------
DWORD MapInstanceNormal::CalCreatureTypeID(const tagRandSpawnPointInfo* pRandSpawnPoint)
{	
	INT nIndex = IUTIL->Rand() % RAND_CTEATUTE_NUM;
	
	switch(m_eInstanceHardMode)
	{
	case EIHM_Normal:
		return pRandSpawnPoint->dwNormalID[nIndex];
	case EIHM_Elite:
		return pRandSpawnPoint->dwEliteID[nIndex];
	case EIHM_Devil:
		return pRandSpawnPoint->dwDevilID[nIndex];
	default:
		return GT_INVALID;
	}
}




//---------------------------------------------------------------------------------
// 根据副本基础等级，刷怪点的等级增加量，刷怪点小ID，计算刷怪点大ID
//---------------------------------------------------------------------------------
DWORD MapInstanceNormal::TransmitBigID(INT nBaseLevel, tagMapSpawnPointInfo* pMapSpawnInfo)
{
	if(!P_VALID(pMapSpawnInfo))
		return GT_INVALID;

	const tagLevelMapping *pLevelMapping = g_attRes.GetLevelMapping(nBaseLevel + pMapSpawnInfo->nLevelInc);
	return pMapSpawnInfo->dwSpawnPointID + (DWORD)pLevelMapping->nTransmitLevel;
}

//---------------------------------------------------------------------------------
// 副本结束
//---------------------------------------------------------------------------------
VOID MapInstanceNormal::OnDelete()
{
	// 移除所有在地图内的玩家
	MapMgr* pMapMgr = g_mapCreator.GetMapMgr(m_dwID);
	if( !P_VALID(pMapMgr) ) return;

	Role* pRole = (Role*)GT_INVALID;

	RoleMap::TMapIterator it = m_mapRole.Begin();
	while( m_mapRole.PeekNext(it, pRole) )
	{
		pMapMgr->RoleInstanceOut(pRole);
	}
}

//-----------------------------------------------------------------------------------
// 副本销毁
//-----------------------------------------------------------------------------------
VOID MapInstanceNormal::OnDestroy()
{
	// 清空一下队伍的副本ID
	if( P_VALID(m_dwTeamID) )
	{
		const Team* pTeam = g_groupMgr.GetTeamPtr(m_dwTeamID);
		if( P_VALID(pTeam) )
		{
			pTeam->SetOwnInstanceMapID(GT_INVALID);
			pTeam->SetOwnInstanceID(GT_INVALID);
		}
	}
	// 清空一下玩家的副本ID
	else
	{
		Role* pCreator = g_roleMgr.GetRolePtrByID(m_dwCreatorID);
		if( P_VALID(pCreator) )
		{
			pCreator->SetMyOwnInstanceMapID(GT_INVALID);
			pCreator->SetMyOwnInstanceID(GT_INVALID);
		}
	}

}

//-----------------------------------------------------------------------------------
// 计算时限副本所剩时间
//-----------------------------------------------------------------------------------
DWORD MapInstanceNormal::CalTimeLimit()
{
	DWORD dwTimeLeft = GT_INVALID;
	if(m_pInstance->dwTimeLimit > 0 && m_pInstance->dwTimeLimit != GT_INVALID)
	{
		DWORD dwCurrentTick = g_world.GetWorldTick();
		DWORD dwTimePass = (dwCurrentTick - m_dwStartTick) / TICK_PER_SECOND;
		dwTimeLeft = m_pInstance->dwTimeLimit - dwTimePass;
	}

	return dwTimeLeft;
}

//---------------------------------------------------------------------------------------------------
// 当队伍创建
//---------------------------------------------------------------------------------------------------
VOID MapInstanceNormal::OnTeamCreate(const Team* pTeam)
{
	if( !P_VALID(pTeam) ) return;

	m_dwTeamID = pTeam->GetTeamID();
}

//----------------------------------------------------------------------------------------------------
// 当队伍删除
//----------------------------------------------------------------------------------------------------
VOID MapInstanceNormal::OnTeamDelete(const Team* pTeam)
{
	if( m_dwTeamID != pTeam->GetTeamID() ) return;
	if( pTeam->GetMemNum() != 1 ) return;
	if( pTeam->GetMemID(0) != m_dwCreatorID ) return;

	m_dwTeamID = GT_INVALID;

	// 找到队伍里唯一一个玩家的玩家指针，重置为单人副本
	Role* pRole = g_roleMgr.GetRolePtrByID(m_dwCreatorID);
	if( P_VALID(pRole) )
	{
		pRole->SetMyOwnInstanceID(m_dwInstanceID);
		pRole->SetMyOwnInstanceMapID(m_dwID);
		// Jason 2009-11-26 当队伍倒数第二个退队时（也即队伍即将被删除时）
		// 改变原来的做法（即单人副本），最后的人也倒计时退出
		m_mapWillOutRoleID.Add(m_dwCreatorID, ROLE_LEAVE_INSTANCE_TICK_COUNT_DOWN);
	}
}

//-----------------------------------------------------------------------------------------------------
// 当玩家离队
//-----------------------------------------------------------------------------------------------------
VOID MapInstanceNormal::OnRoleLeaveTeam(DWORD dwRoleID, const Team* pTeam)
{
	if( m_dwTeamID != pTeam->GetTeamID() ) return;

	// 如果该玩家是副本的创建者，则将创建者移交到当前队伍的队长
	if( dwRoleID == m_dwCreatorID )
	{
		ASSERT( pTeam->GetMemNum() > 0 );
		m_dwCreatorID = pTeam->GetMemID(0);
	}

	// 如果该玩家目前在副本里，则设置玩家的离开副本倒计时
	Role* pRole = FindRole(dwRoleID);
	if( P_VALID(pRole) )
	{
		m_mapWillOutRoleID.Add(dwRoleID, ROLE_LEAVE_INSTANCE_TICK_COUNT_DOWN);
	}
}

//------------------------------------------------------------------------------------------------------
// 当队伍进入
//------------------------------------------------------------------------------------------------------
VOID MapInstanceNormal::OnRoleEnterTeam(DWORD dwRoleID, const Team* pTeam)
{
	if( m_dwTeamID != pTeam->GetTeamID() ) return;

	// 如果该玩家目前在副本里，则清空玩家的离开副本倒计时
	Role* pRole = FindRole(dwRoleID);
	if( P_VALID(pRole) )
	{
		m_mapWillOutRoleID.Erase(dwRoleID);
	}
}