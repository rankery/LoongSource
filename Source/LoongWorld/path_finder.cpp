//-------------------------------------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: path_finder.cpp
// author: wjiang
// actor:
// data: 2009-5-31
// last:
// brief: 路径寻找器
//-------------------------------------------------------------------------------------------------------
#include "StdAfx.h"



#include "map.h"
#include "creature.h"
#include "unit.h"
#include "path_finder.h"

NPCPathFinderCol::~NPCPathFinderCol()
{
	m_listPathPoint.clear();
}

//-------------------------------------------------------------------------------------------
// Init
//-------------------------------------------------------------------------------------------
BOOL NPCPathFinderCol::Init(Unit* pUnit)
{
	if (!pUnit->IsCreature())	return FALSE;

	m_pOwner = dynamic_cast<Creature*>(pUnit);
	m_listPathPoint.clear();
	m_PathPointIt = m_listPathPoint.end();
	m_bMoveingPathPoint = FALSE;
	m_nSynMoveTick = 0;

	return TRUE;
}

//-------------------------------------------------------------------------------------------
// 得到下个路径点
//-------------------------------------------------------------------------------------------
BOOL NPCPathFinderCol::GetCurPathPoint(const Vector3& startPos, Vector3& movePos)
{
	if(m_PathPointIt == m_listPathPoint.end())	return FALSE;

	movePos = *m_PathPointIt;
	++m_PathPointIt;
	return TRUE;
}

//-------------------------------------------------------------------------------------------
// 计算生成移动路径
//-------------------------------------------------------------------------------------------
BOOL NPCPathFinderCol::FindingPath(const Vector3& startPos,const Vector3& targetPos)
{
	m_listPathPoint.clear();
	Map* pMap = m_pOwner->GetMap();
	if(!P_VALID(pMap))	return FALSE;

	pMap->GetNavMap()->GetNPCNavGraph()->FindPath(startPos, targetPos, m_pOwner->GetSize(), 10000.0f, pMap->GetNavCollider(),m_listPathPoint);
	m_PathPointIt = m_listPathPoint.begin();

	if(m_PathPointIt != m_listPathPoint.end())
		return TRUE;
	else
		return FALSE;
}

NPCPathFinder::~NPCPathFinder()
{
	m_listPathPoint.clear();
}

//-------------------------------------------------------------------------------------------
// Init
//-------------------------------------------------------------------------------------------
BOOL NPCPathFinder::Init(Unit* pUnit)
{
	if (!pUnit->IsCreature())	return FALSE;

	m_pOwner = dynamic_cast<Creature*>(pUnit);
	m_listPathPoint.clear();
	m_bMoveingPathPoint = FALSE;
	m_nSynMoveTick = 0;

	return TRUE;
}

//-------------------------------------------------------------------------------------------
// 计算生成移动路径
//-------------------------------------------------------------------------------------------
BOOL NPCPathFinder::FindingPath(const Vector3& startPos,const Vector3& targetPos)
{
	m_listPathPoint.clear();
	Map* pMap = m_pOwner->GetMap();
	if(!P_VALID(pMap))	return FALSE;

	pMap->GetNavMap()->GetNPCNavMap()->FindPath(startPos.x, startPos.z, targetPos.x, targetPos.z, 200, m_listPathPoint);

	if(m_listPathPoint.begin() != m_listPathPoint.end())
		return TRUE;
	else
		return FALSE;
}

//-------------------------------------------------------------------------------------------
// 得到下个路径点
//-------------------------------------------------------------------------------------------
BOOL NPCPathFinder::GetCurPathPoint(const Vector3& startPos, Vector3& movePos)
{
	Map* pMap = m_pOwner->GetMap();
	if(!P_VALID(pMap))	return FALSE;

	pMap->GetNavMap()->GetNPCNavMap()->SmoothPath(startPos.x, startPos.z, m_listPathPoint);

	std::list<POINT>::iterator it = m_listPathPoint.begin();
	if(it == m_listPathPoint.end())	return FALSE;

	POINT PathPoint = *it;
	m_listPathPoint.pop_front();

	movePos.x = FLOAT(PathPoint.x * TILE_SCALE);
	movePos.z = FLOAT(PathPoint.y * TILE_SCALE);

	return TRUE;
}