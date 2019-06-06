//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: map_instance_script.cpp
// author: wjiang
// actor:
// data: 2008-7-31
// last:
// brief: 脚本创建的地图副本
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "map_instance_script.h"
#include "map_creator.h"
#include "role.h"
#include "creature.h"
#include "map_mgr.h"
#include "NPCTeam.h"
#include "NPCTeam_mgr.h"
#include "../WorldDefine/msg_map.h"
#include "../WorldDefine/MapAttDefine.h"

MapInstanceScript::MapInstanceScript() : MapInstance()
{
}

MapInstanceScript::~MapInstanceScript()
{
	Destroy();
}

//------------------------------------------------------------------------------------------------------
// 初始化（创建者不需要）
//------------------------------------------------------------------------------------------------------
BOOL MapInstanceScript::Init(const tagMapInfo* pInfo, DWORD dwInstanceID, Role* pCreator, DWORD dwMisc)
{
	ASSERT( P_VALID(pInfo) );
	ASSERT( EMT_ScriptCreate == pInfo->eType );

	// 地图相关属性
	m_pUtil = "Util";
	m_pInfo = pInfo;
	m_mapSession.Clear();
	m_mapRole.Clear();
	m_mapShop.Clear();
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
VOID MapInstanceScript::Update()
{
	Map::Update();
}

//---------------------------------------------------------------------------------
// 销毁
//---------------------------------------------------------------------------------
VOID MapInstanceScript::Destroy()
{

}

//---------------------------------------------------------------------------------
// 正式加入一个玩家，这只能由管理该地图的MapMgr调用
//---------------------------------------------------------------------------------
VOID MapInstanceScript::AddRole(Role* pRole)
{
	Map::AddRole(pRole);
}

//---------------------------------------------------------------------------------
// 玩家离开地图，只可能在主线程里面调用
//---------------------------------------------------------------------------------
VOID MapInstanceScript::RoleLeaveMap(DWORD dwID, BOOL bChgMap)
{
	Map::RoleLeaveMap(dwID, bChgMap);
}
//---------------------------------------------------------------------------------
// 是否能进入副本
//---------------------------------------------------------------------------------
INT MapInstanceScript::CanEnter(Role *pRole)
{
	return E_Success;
}

//---------------------------------------------------------------------------------
// 是否可以删除
//---------------------------------------------------------------------------------
BOOL MapInstanceScript::CanDestroy()
{
	return MapInstance::CanDestroy();
}


//---------------------------------------------------------------------------------
// 初始化刷怪点怪物
//---------------------------------------------------------------------------------
BOOL MapInstanceScript::InitAllSpawnPointCreature()
{
	return Map::InitAllSpawnPointCreature();
}

//---------------------------------------------------------------------------------
// 副本结束
//---------------------------------------------------------------------------------
VOID MapInstanceScript::OnDelete()
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
VOID MapInstanceScript::OnDestroy()
{
}