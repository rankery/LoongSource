//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: world_event_mgr.cpp
// author: wjiang
// actor:
// data: 2009-04-2
// last:
// brief: 游戏世界事件触发类
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "world_event_mgr.h"
#include "script_mgr.h"
#include "activity_mgr.h"
#include "role_mgr.h"
#include "world.h"
#include "role.h"

BOOL CWorldEventMgr::Init()
{
	// 初始化脚本
	m_pScript = g_ScriptMgr.GetWorldScript();
	if(!P_VALID(m_pScript))
		return FALSE;

	return TRUE;
}

//-----------------------------------------------------------------------------
// 整点Update
//-----------------------------------------------------------------------------
VOID CWorldEventMgr::OnClock(BYTE byHour)
{
	OnAdventure();
}

//-----------------------------------------------------------------------------
// 产生奇遇
//-----------------------------------------------------------------------------
VOID CWorldEventMgr::OnAdventure()
{
	// 是否有固定活动在进行中
	if(TRUE == g_activityMgr.HaveActivitStart())
		return;

	// 随机选取3名在线玩家获得奇遇
	Role	*pRole = (Role*)GT_INVALID;
	std::vector<Role*>	vecRole;
	INT		nNum	= 0;
	FLOAT	fProp	= 0.0f;
	BOOL	bResult	= FALSE;

	// 第一次抽取500次，计算角色福缘属性影响
	for(INT i = 0; i < 500; ++i)
	{
		// 已经找到三个玩家
		if(nNum == 3)
			break;

		pRole = g_roleMgr.GetRandRolePtr();
		if(!P_VALID(pRole))
			continue;

		// 小于15级以下不予抽取
		if(pRole->GetLevel() < 15)
			continue;

		// 副本内玩家不予抽取
		

		// 计算获得奇遇的概率
		fProp = 1.0f / 20.0f * (1.0f + (FLOAT)pRole->GetAttValue(ERA_Fortune) / 10.0f);
		bResult = IUTIL->Rand() % 100 <= (INT)fProp;
		if(bResult)
		{
			vecRole.push_back(pRole);
			++nNum;
		}
	}

	// 抽取的玩家小于3个，则进行第二轮抽取
	if(nNum < 3)
	{
		for(INT n = 0; n < 500; ++n)
		{
			// 已经找到三个玩家
			if(nNum == 3)
				break;

			pRole = g_roleMgr.GetRandRolePtr();
			if(!P_VALID(pRole))
				continue;

			// 小于15级以下不予抽取
			if(pRole->GetLevel() < 15)
				continue;

			// 副本内玩家不予抽取


			vecRole.push_back(pRole);
			++nNum;
		}
	}

	// 对抽取的玩家产生奇遇
	std::vector<Role*>::iterator it = vecRole.begin();
	while (it != vecRole.end())
	{
		if (P_VALID(m_pScript))
		{
			m_pScript->OnAdventure(*it);
		}
		++it;
	}
}