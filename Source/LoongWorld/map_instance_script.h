//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: map_instance_script.h
// author: weijiang
// actor:
// data: 2009-07-31
// last:
// brief: 脚本创建的副本类
//-----------------------------------------------------------------------------
#pragma once

#include "map.h"

class MapInstance;

//------------------------------------------------------------------------------
// 脚本创建的副本类
//------------------------------------------------------------------------------
class MapInstanceScript : public MapInstance
{
public:
	MapInstanceScript();
	virtual ~MapInstanceScript();

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
};