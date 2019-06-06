//-------------------------------------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: NPCTeam.cpp
// author: wjiang
// actor:
// data: 2009-6-15
// last:
// brief: NPC队伍类
//-------------------------------------------------------------------------------------------------------
#include "StdAfx.h"
#include "NPCTeam.h"
#include "NPCTeam_mgr.h"
#include "creature_ai.h"

#include "../ServerDefine/NPCTeam_define.h"

NPCTeam::NPCTeam(DWORD dwID, Creature* pLeader):m_dwID(dwID)
{
	// 加入小队队长
	m_mapMemPtr.Add(pLeader->GetID(), pLeader);
}

NPCTeam::~NPCTeam()
{
	m_mapMemPtr.Clear();
}

//----------------------------------------------------------------------------
// 增加小队成员
//----------------------------------------------------------------------------
VOID NPCTeam::AddNPCTeamMem(Creature* pCreature)
{
	m_mapMemPtr.Add(pCreature->GetID(), pCreature);
}

//-----------------------------------------------------------------------------
// 使队员行走
//-----------------------------------------------------------------------------
VOID NPCTeam::OrderTeammateMove(Creature* pLeader, Vector3& vDes)
{
	Creature* pCreature = (Creature*)GT_INVALID;
	Vector3 vNew;
	TMap<DWORD, Creature*>::TMapIterator it = m_mapMemPtr.Begin();

	while( m_mapMemPtr.PeekNext(it, pCreature) )
	{
		if(!P_VALID(pCreature) || pCreature->IsDead())
			continue;

		// 先计算出队长新朝向与初始朝向的夹角的cos值和sin值
		FLOAT fFaceX = (FLOAT)(vDes.x - pLeader->GetCurPos().x);
		FLOAT fFaceZ = (FLOAT)(vDes.z - pLeader->GetCurPos().z);
		FLOAT fOriFaceX = pLeader->GetFaceTo().x;
		FLOAT fOriFaceZ = pLeader->GetFaceTo().z;
		FLOAT fFaceLen = IFASTCODE->FastSqrt(fFaceX*fFaceX+fFaceZ*fFaceZ);
		FLOAT fOriFaceLen = IFASTCODE->FastSqrt(fOriFaceX*fOriFaceX+fOriFaceZ*fOriFaceZ);
		FLOAT fCos = (fFaceX*fOriFaceX + fFaceZ*fOriFaceZ) / fFaceLen / fOriFaceLen; 
		FLOAT fSin = (fOriFaceX*fFaceZ - fFaceX*fOriFaceZ) / fFaceLen / fOriFaceLen;

		// 先旋转
		FLOAT fX = pCreature->GetBornPos().x - pLeader->GetBornPos().x;
		FLOAT fZ = pCreature->GetBornPos().z - pLeader->GetBornPos().z;
		vNew.x = fX*fCos-fZ*fSin;
		vNew.z = fX*fSin+fZ*fCos;

		vNew.x += vDes.x;
		vNew.z += vDes.z;
		vNew.y = vDes.y;

		if( MoveData::IsPosInvalid(vNew))
			return;

		if( MoveData::EMR_Success == pCreature->GetMoveData().StartCreatureWalk(vNew, EMS_CreaturePatrol))
		{
			if( P_VALID(pCreature->GetAI()) )
			pCreature->GetAI()->SetIsPatroling(TRUE);
		}		
	}
}

//-----------------------------------------------------------------------------
// 小队事件触发
//-----------------------------------------------------------------------------
VOID NPCTeam::OnEvent(ENPCTeamEvent eType, Unit* pUnit, DWORD dwEventMisc1, DWORD dwEventMisc2, DWORD dwEventMisc3)
{
	switch(eType)
	{
	case NTE_SynEnmity:
		{
			OnSynEnmity(pUnit, dwEventMisc1);
		}
	}
}

//-----------------------------------------------------------------------------
// 同步怪物小队仇恨
//-----------------------------------------------------------------------------
VOID NPCTeam::OnSynEnmity(Unit* pUnit, DWORD dwValue)
{
	INT nValue = (INT)dwValue;

	Creature* pCreature = (Creature*)GT_INVALID;
	TMap<DWORD, Creature*>::TMapIterator it = m_mapMemPtr.Begin();
	while( m_mapMemPtr.PeekNext(it, pCreature) )
	{
		if( !P_VALID(pCreature) || pCreature->IsDead() || !P_VALID(pCreature->GetAI()))
			continue;

		pCreature->GetAI()->AddEnmity(pUnit, nValue, FALSE);
	}
}

//-----------------------------------------------------------------------------
// 获得小队队长
//-----------------------------------------------------------------------------
DWORD NPCTeam::GetLeaderID()
{
	DWORD dwID = GT_INVALID;
	Creature* pCreature = (Creature*)GT_INVALID;
	TMap<DWORD, Creature*>::TMapIterator it = m_mapMemPtr.Begin();
	while( m_mapMemPtr.PeekNext(it, dwID, pCreature) )
	{
		if(P_VALID(pCreature) && pCreature->IsTeam())
		{
			dwID = pCreature->GetID();
			break;
		}
	}
	return dwID;
}

//-----------------------------------------------------------------------------
// 同队成员体力低于一定值
//-----------------------------------------------------------------------------
Creature* NPCTeam::IsTeamateHPLower(DWORD dwValue)
{
	DWORD dwID = GT_INVALID;
	Creature* pCreature = (Creature*)GT_INVALID;
	TMap<DWORD, Creature*>::TMapIterator it = m_mapMemPtr.Begin();
	while( m_mapMemPtr.PeekNext(it, dwID, pCreature) )
	{
		if(P_VALID(pCreature) && !pCreature->IsDead())
		{
			INT nMPLimit = (INT)dwValue;

			if( nMPLimit < 100000 )
			{
				 if(pCreature->GetAttValue(ERA_HP) < nMPLimit)
					 return pCreature;
			}
			else
			{
				if( pCreature->GetAttValue(ERA_MaxHP) > 0 )
				{
					if(pCreature->GetAttValue(ERA_HP) * 10000 / pCreature->GetAttValue(ERA_MaxHP) < (nMPLimit - 100000))
						return pCreature;
				}
				else
					return NULL;
			}
		}
	}
	return NULL;
}

//-----------------------------------------------------------------------------
// 同队成员真气低于一定值
//-----------------------------------------------------------------------------
Creature* NPCTeam::IsTeamateMPLower(DWORD dwValue)
{
	DWORD dwID = GT_INVALID;
	Creature* pCreature = (Creature*)GT_INVALID;
	TMap<DWORD, Creature*>::TMapIterator it = m_mapMemPtr.Begin();
	while( m_mapMemPtr.PeekNext(it, dwID, pCreature) )
	{
		if(P_VALID(pCreature) && !pCreature->IsDead())
		{
			INT nMPLimit = (INT)dwValue;

			if( nMPLimit < 100000 )
			{
				if(pCreature->GetAttValue(ERA_MP) < nMPLimit)
					return pCreature;
			}
			else
			{
				if( pCreature->GetAttValue(ERA_MaxMP) > 0 )
				{
					if(pCreature->GetAttValue(ERA_MP) * 10000 / pCreature->GetAttValue(ERA_MaxMP) < (nMPLimit - 100000))
						return pCreature;
				}
				else
					return NULL;
			}
		}
	}
	return NULL;
}


