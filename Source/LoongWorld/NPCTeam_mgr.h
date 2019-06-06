//-------------------------------------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: NPCTeam_mgr.h
// author: wjiang
// actor:
// data: 2009-6-15
// last:
// brief: NPC队伍管理类
//-------------------------------------------------------------------------------------------------------
#pragma once

class NPCTeam;
class CVector2D;
struct tagNPCTeamOrder;

class NPCTeamMgr
{
public:
	NPCTeamMgr(Map* pMap);
	~NPCTeamMgr();

	// 获得小队队形结构
	const tagNPCTeamOrder*	GetNPCTeamOrder(ENPCTeamOrder eTeamOrder)	{ return m_mapNPCOrder.Peek(eTeamOrder); }
	// 获得小队指针
	NPCTeam*	GetNPCTeam(DWORD dwID) { return m_mapNPCTeam.Peek(dwID); }

	// 加载小队队形
	static BOOL	LoadNPCOrder();
	// 刷怪时计算小队队员位置
	Vector3		CalTeamMemPos(Creature *pLeader, POINT point, Vector3 vFace, const tagNestProto* pNestProto);
	// 创建怪物小队
	NPCTeam*	CreateTeam(Creature* pLeader);

private:
	
private:
	TMap<DWORD, NPCTeam*>								m_mapNPCTeam;
	static TMap<ENPCTeamOrder, tagNPCTeamOrder*>		m_mapNPCOrder;
	Map*												m_pMap;
	DWORD												m_dwTeamIndex;		// 小队ID索引
};