//-------------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: strength.h
// author: zhangrong
// actor:
// data: 2009-9-7
// last:
// brief: 实力系统
//-------------------------------------------------------------------------------

#pragma once
#include "mutex.h"
#include "../ServerDefine/strength_define.h"

class StrengthRankings
{
	TMap<INT, tagStrengthRoleNode*>		m_mapRankings;	// 最新的排行榜（前500名，实力值大于100的）
	Mutex								m_Lock;
public:
	// 请求初始化实力排行榜
	BOOL Init();
	// 初始化实力榜
	VOID InitRankings(tagNetCmd* pMsg);
	// 更新实力榜，每天24点更新
	//VOID UpdateWorldRankings();

	TMap<INT, tagStrengthRoleNode*>		GetGlobalRankings()	{ return m_mapRankings; }
};

extern StrengthRankings g_StrengthRankings;