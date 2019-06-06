//-------------------------------------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: path_finder.h
// author: wjiang
// actor:
// data: 2009-5-31
// last:
// brief: 路径寻找器
//-------------------------------------------------------------------------------------------------------
#pragma once
#include <list>

class Unit;
class Creature;

//-----------------------------------------------------------------------------
// 路径寻找器
//-----------------------------------------------------------------------------
class PathFinder
{
public:
	PathFinder() {}
	virtual ~PathFinder() {}

public:
	virtual BOOL	Init(Unit* pUnit) = 0;
	virtual BOOL	FindingPath(const Vector3& startPos,const Vector3& targetPos) = 0;
	virtual BOOL	GetCurPathPoint(const Vector3& startPos, Vector3& movePos) = 0;

protected:
	BOOL									m_bMoveingPathPoint;	// 是否正在使用寻路列表中的路径点
	INT										m_nSynMoveTick;			// 同步客户端移动的间隔
};

//-----------------------------------------------------------------------------
// 碰撞怪路径寻找器
//-----------------------------------------------------------------------------
class NPCPathFinderCol : public PathFinder
{
public:
	NPCPathFinderCol() {}
	~NPCPathFinderCol();

public:
	BOOL	Init(Unit* pUnit);
	BOOL	FindingPath(const Vector3& startPos,const Vector3& targetPos);
	BOOL    GetCurPathPoint(const Vector3& startPos, Vector3& movePos);

private:
	Creature*								m_pOwner;

	std::list<Vector3>						m_listPathPoint;		// 怪物追击寻路时的路径点
	std::list<Vector3>::iterator			m_PathPointIt;			// 当前寻到路径点的哪一个点
};

//-----------------------------------------------------------------------------
// 非碰撞怪路径寻找器
//-----------------------------------------------------------------------------
class NPCPathFinder : public PathFinder
{
public:
	NPCPathFinder() {}
	~NPCPathFinder();

public:
	BOOL	Init(Unit* pUnit);
	BOOL	FindingPath(const Vector3& startPos,const Vector3& targetPos);
	BOOL	GetCurPathPoint(const Vector3& startPos, Vector3& movePos);

private:
	Creature*								m_pOwner;

	std::list<POINT>						m_listPathPoint;		// 怪物追击寻路时的路径点
};
