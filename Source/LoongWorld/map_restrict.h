//------------------------------------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: map_restrict.h
// author: aslan
// actor:
// data: 2009-03-26
// last:
// brief: 地图限制类，主要用于地图的进入判断及出口判断
// remark: 嵌入到地图管理器中，用于某一类地图的总体判断，针对于副本实例的判断，则由各个副本的地图派生类实现
//------------------------------------------------------------------------------------------------------
#pragma once

class Map;
class MapMgr;
class MapScript;
class MapInstance;

struct tagMapInfo;

//------------------------------------------------------------------------------------------------------
// 地图限制抽象类
//------------------------------------------------------------------------------------------------------
class MapRestrict
{
public:
	virtual VOID Init(MapMgr* pMapMgr);
	virtual Map* CanEnter(Role* pRole, DWORD dwMisc) = 0;
	virtual Map* CanTakeOverWhenOnline(Role* pRole, DWORD& dwOutMapID, Vector3& vOut) = 0;
	virtual BOOL GetExportMapAndCoord(Role* pRole, DWORD& dwOutMapID, Vector3& vOut) = 0;

	const tagMapInfo* GetMapInfo()	{ return m_pInfo; }

protected:
	MapRestrict();
	virtual ~MapRestrict();

private:
	MapRestrict(const MapRestrict& rh);
	MapRestrict& operator=(const MapRestrict& rh);

protected:
	const tagMapInfo*	m_pInfo;
	MapMgr*				m_pMgr;
};

//--------------------------------------------------------------------------------------------------------
// 普通地图限制类
//--------------------------------------------------------------------------------------------------------
class MapRestrictNormal : public MapRestrict
{
public:
	virtual VOID Init(MapMgr* pMapMgr);
	virtual Map* CanEnter(Role* pRole, DWORD dwMisc);
	virtual Map* CanTakeOverWhenOnline(Role* pRole, DWORD& dwOutMapID, Vector3& vOut);
	virtual BOOL GetExportMapAndCoord(Role* pRole, DWORD& dwOutMapID, Vector3& vOut);
};

//--------------------------------------------------------------------------------------------------------
// 普通副本限制类
//--------------------------------------------------------------------------------------------------------
class MapRestrictInstance : public MapRestrict
{
public:
	virtual VOID Init(MapMgr* pMapMgr);
	virtual Map* CanEnter(Role* pRole, DWORD dwMisc);
	virtual Map* CanTakeOverWhenOnline(Role* pRole, DWORD& dwOutMapID, Vector3& vOut);
	virtual BOOL GetExportMapAndCoord(Role* pRole, DWORD& dwOutMapID, Vector3& vOut);

private:
	INT CanEnterByInstanceInfo(Role* pRole);
};

//---------------------------------------------------------------------------------------------------------
// 稳定副本限制类（新手副本）
//---------------------------------------------------------------------------------------------------------
class MapRestrictStable : public MapRestrict
{
public:
	virtual VOID Init(MapMgr* pMapMgr);
	virtual Map* CanEnter(Role* pRole, DWORD dwMisc);
	virtual Map* CanTakeOverWhenOnline(Role* pRole, DWORD& dwOutMapID, Vector3& vOut);
	virtual BOOL GetExportMapAndCoord(Role* pRole, DWORD& dwOutMapID, Vector3& vOut);

private:
	MapInstance* GetOnePerfectMap();
};

//---------------------------------------------------------------------------------------------------------
// 脚本创建的副本限制类
//---------------------------------------------------------------------------------------------------------
class MapRestrictScript : public MapRestrict
{
public:
	virtual VOID Init(MapMgr* pMapMgr);
	virtual Map* CanEnter(Role* pRole, DWORD dwMisc);
	virtual Map* CanTakeOverWhenOnline(Role* pRole, DWORD& dwOutMapID, Vector3& vOut);
	virtual BOOL GetExportMapAndCoord(Role* pRole, DWORD& dwOutMapID, Vector3& vOut);

private:
	MapInstance* GetOnePerfectMap(Role* pRole);

private:
	const MapScript*	m_pScript;
};