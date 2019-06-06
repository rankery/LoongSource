//-------------------------------------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: NPCTeam.h
// author: wjiang
// actor:
// data: 2009-6-15
// last:
// brief: NPC队伍类
//-------------------------------------------------------------------------------------------------------
#pragma once
#include "creature.h"

enum ENPCTeamEvent;

class NPCTeam
{
public:
	NPCTeam(DWORD dwID, Creature* pLeader);
	~NPCTeam();

	DWORD GetID() { return m_dwID; }

	VOID AddNPCTeamMem(Creature* pCreature);
	VOID OnEvent(ENPCTeamEvent eType, Unit* pUnit, DWORD dwEventMisc1=0, DWORD dwEventMisc2=0, DWORD dwEventMisc3=0);
	VOID OrderTeammateMove(Creature* pLeader, Vector3& vDes);
	DWORD GetLeaderID();
	Creature* IsTeamateHPLower(DWORD dwValue);
	Creature* IsTeamateMPLower(DWORD dwValue);

private:

	// 同步怪物小队仇恨
	VOID OnSynEnmity(Unit* pUnit, DWORD dwValue);

private:
	DWORD						m_dwID;
	TMap<DWORD, Creature*>		m_mapMemPtr;
	const tagNestProto			m_pNestProto;
};

