//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: world_event_mgr.h
// author: wjiang
// actor:
// data: 2009-04-2
// last:
// brief: 游戏世界事件触发类
//-----------------------------------------------------------------------------
#pragma once

class WorldScript;

class CWorldEventMgr
{
public:
	CWorldEventMgr() {}
	~CWorldEventMgr() { m_pScript = NULL; }

	BOOL	Init();
	VOID	OnClock(BYTE byHour);

	VOID	OnAdventure();

private:
	const WorldScript*	m_pScript; 
};