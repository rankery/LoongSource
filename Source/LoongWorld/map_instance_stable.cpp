//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: map_instance.cpp
// author: wjiang
// actor:
// data: 2008-1-15
// last:
// brief: 稳定地图副本
// remark: 主要用于新手村和序章等副本
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "map_instance_stable.h"
#include "map_creator.h"
#include "role.h"
#include "creature.h"
#include "map_mgr.h"
#include "NPCTeam.h"
#include "NPCTeam_mgr.h"
#include "../WorldDefine/msg_map.h"
#include "../WorldDefine/MapAttDefine.h"


MapInstanceStable::MapInstanceStable() : MapInstance()
{
}

MapInstanceStable::~MapInstanceStable()
{
	Destroy();
}

//------------------------------------------------------------------------------------------------------
// 初始化（创建者不需要）
//------------------------------------------------------------------------------------------------------
BOOL MapInstanceStable::Init(const tagMapInfo* pInfo, DWORD dwInstanceID, Role* pCreator, DWORD dwMisc)
{
	ASSERT( P_VALID(pInfo) );
	ASSERT( EMT_System == pInfo->eType );

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

	return TRUE;		
}

//---------------------------------------------------------------------------------
// 更新
//---------------------------------------------------------------------------------
VOID MapInstanceStable::Update()
{
	Map::Update();
}

//---------------------------------------------------------------------------------
// 销毁
//---------------------------------------------------------------------------------
VOID MapInstanceStable::Destroy()
{

}

//---------------------------------------------------------------------------------
// 正式加入一个玩家，这只能由管理该地图的MapMgr调用
//---------------------------------------------------------------------------------
VOID MapInstanceStable::AddRole(Role* pRole)
{
	Map::AddRole(pRole);
}

//---------------------------------------------------------------------------------
// 玩家离开地图，只可能在主线程里面调用
//---------------------------------------------------------------------------------
VOID MapInstanceStable::RoleLeaveMap(DWORD dwID, BOOL bChgMap)
{
	Map::RoleLeaveMap(dwID, bChgMap);
}
//---------------------------------------------------------------------------------
// 是否能进入副本
//---------------------------------------------------------------------------------
INT MapInstanceStable::CanEnter(Role *pRole)
{
	return E_Success;
}

//---------------------------------------------------------------------------------
// 是否可以删除
//---------------------------------------------------------------------------------
BOOL MapInstanceStable::CanDestroy()
{
	return MapInstance::CanDestroy();
}


//---------------------------------------------------------------------------------
// 初始化刷怪点怪物
//---------------------------------------------------------------------------------
BOOL MapInstanceStable::InitAllSpawnPointCreature()
{
	return Map::InitAllSpawnPointCreature();
}

//---------------------------------------------------------------------------------
// 副本结束
//---------------------------------------------------------------------------------
VOID MapInstanceStable::OnDelete()
{
	
}

//-----------------------------------------------------------------------------------
// 副本销毁
//-----------------------------------------------------------------------------------
VOID MapInstanceStable::OnDestroy()
{
}
