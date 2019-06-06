//-------------------------------------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: NPCTeam_mgr.cpp
// author: wjiang
// actor:
// data: 2009-6-15
// last:
// brief: NPC队伍管理类
//-------------------------------------------------------------------------------------------------------
#include "StdAfx.h"
#include "NPCTeam.h"
#include "NPCTeam_mgr.h"
#include "creature.h"

#include "../ServerDefine/NPCTeam_define.h"

TMap<ENPCTeamOrder, tagNPCTeamOrder*>	NPCTeamMgr::m_mapNPCOrder;

NPCTeamMgr::NPCTeamMgr(Map* pMap)
{
	m_mapNPCTeam.Clear();
	m_pMap = pMap;
	m_dwTeamIndex = 0;
}

NPCTeamMgr::~NPCTeamMgr()
{
	m_pMap = NULL;

	TMap<DWORD, NPCTeam*>::TMapIterator it = m_mapNPCTeam.Begin();
	NPCTeam* pTeam = NULL;

	while( m_mapNPCTeam.PeekNext(it, pTeam) )
	{
		SAFE_DEL(pTeam);
	}

	m_mapNPCTeam.Clear();
}

//----------------------------------------------------------------------------
// 加载小队队形
//----------------------------------------------------------------------------
BOOL NPCTeamMgr::LoadNPCOrder()
{
	FILE *pFile = fopen("server_data\\NPCOrder.ini", "r");
	if (pFile == NULL)
	{
		return FALSE;
	}

	for(INT i = 1; i < NTO_END; i++)
	{
		CHAR szField[TEAM_WIDTH];
		ZeroMemory(szField, TEAM_WIDTH);
		sprintf(szField, "[%d]", i);
		
		CHAR cTmp[TEAM_WIDTH];
		ZeroMemory(cTmp, TEAM_WIDTH);
		fgets(cTmp, TEAM_WIDTH, pFile);
		if(strncmp(cTmp, szField, strlen(szField)) != 0)
		{
			fclose(pFile);
			break;
		}

		CHAR szTeamOrder[TEAM_WIDTH][TEAM_HEIGH*2];

		for(INT n = 0; n < TEAM_HEIGH; n++)
			fgets(szTeamOrder[n], TEAM_WIDTH*2, pFile);

		tagNPCTeamOrder* pTeamOrder = new tagNPCTeamOrder;
		if(!P_VALID(pTeamOrder))
			break;

		for(INT k = 0; k < TEAM_WIDTH; k++)
		{
			for(INT j = 0; j < TEAM_HEIGH; j++)
			{
				if('1' == szTeamOrder[k][j])
				{
					POINT Point;
					Point.x = j-2;
					Point.y = k-2;
					pTeamOrder->NPCOrder.push_back(Point);
				}
			}
		}

		m_mapNPCOrder.Add((ENPCTeamOrder)i, pTeamOrder);
	}

	fclose(pFile);

	return TRUE;
}

//----------------------------------------------------------------------------
// 计算小队成员的位置
//----------------------------------------------------------------------------
Vector3 NPCTeamMgr::CalTeamMemPos(Creature *pLeader, POINT point, Vector3 vFace, const tagNestProto* pNestProto)
{
	Vector3 vPos;
	CVector2D vector((FLOAT)point.x, (FLOAT)point.y);
	CVector2D vface(vFace.x, -vFace.z);
	Vector3 vLeaderPos = pLeader->GetCurPos();

	// 按照队长的朝向旋转队员的位置
	CMatrix2D Tran;
	Tran = CMatrix2D::CreateRotateMatrix(vface);

	CMatrix2D  MATRIX90(0.0f, 1.0f, -1.0f, 0.0f);

	// 缩放队员间的间距
	CMatrix2D Scale(pNestProto->fSpace, 0.0f, 0.0f, pNestProto->fSpace);

	vector = Tran*vector;
	vector = MATRIX90*vector*Scale;
	vPos.x = vector.fx + vLeaderPos.x;
	vPos.z = vector.fy + vLeaderPos.z;

	if(pLeader->NeedCollide())
	{
		vPos.y = pLeader->GetCurPosTop().y;
	}
	else
	{
		vPos.y = m_pMap->GetGroundHeight(vPos.x, vPos.z);
	}

	return vPos;
}

//----------------------------------------------------------------------------
// 创建怪物小队
//----------------------------------------------------------------------------
NPCTeam* NPCTeamMgr::CreateTeam(Creature* pLeader)
{
	NPCTeam* pTeam = new NPCTeam(m_dwTeamIndex, pLeader);
	if(!P_VALID(pTeam))		return NULL;

	m_mapNPCTeam.Add(m_dwTeamIndex, pTeam);

	// 小队ID索引加一
	++m_dwTeamIndex;

	return pTeam;
}

