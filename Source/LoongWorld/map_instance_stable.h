//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: map_instance.h
// author: aslan
// actor:
// data: 2009-04-08
// last:
// brief: 稳定副本类
// remark: 主要用于新手村或者序章副本
//-----------------------------------------------------------------------------
#pragma once

#include "map.h"

class MapInstance;

//------------------------------------------------------------------------------
// 稳定副本类
//------------------------------------------------------------------------------
class MapInstanceStable : public MapInstance
{
public:
	MapInstanceStable();
	virtual ~MapInstanceStable();

	virtual BOOL		Init(const tagMapInfo* pInfo, DWORD dwInstanceID, Role* pCreator=NULL, DWORD dwMisc=GT_INVALID);
	virtual VOID		Update();
	virtual VOID		Destroy();

	virtual VOID		AddRole(Role* pRole);
	virtual	VOID		RoleLeaveMap(DWORD dwID, BOOL bChgMap);
	virtual INT			CanEnter(Role *pRole);
	virtual BOOL		CanDestroy();
	virtual VOID		OnDestroy();

protected:
	virtual VOID		OnDelete();
	virtual BOOL		InitAllSpawnPointCreature();

private:
};
