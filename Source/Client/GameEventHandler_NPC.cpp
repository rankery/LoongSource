#include "StdAfx.h"
#include "GameEventHandler_NPC.h"
#include "CombatEvent.h"
#include "FSM_NPC.h"
#include "NPC.h"
#include "CombatEvent.h"
#include "EffectMgr.h"
#include "SkillProtoData.h"
#include "ItemProtoData.h"

//--class NetCmdHander_NPC-------------------------------------------
GameEventHandler_NPC::GameEventHandler_NPC(void):m_pFSM(NULL)
{}

GameEventHandler_NPC::~GameEventHandler_NPC(void)
{}





//--class HitTargetEventHandler_NPC-------------------------------------------
HitTargetEventHandler_NPC::HitTargetEventHandler_NPC( void )
{}

HitTargetEventHandler_NPC::~HitTargetEventHandler_NPC( void )
{}

void HitTargetEventHandler_NPC::OnGameEvent( tagGameEvent* pEvent )
{
	if(pEvent->strEventName==_T("tagHitTargetEvent"))
	{
		tagHitTargetEvent* pHitEvent=(tagHitTargetEvent*)pEvent;

		//IMSG(_T("[%d]HitTargetEventHandler_NPC::OnGameEvent recv hittarget,skill=%d,srcrole=%d,serial=%d,channel=%d\n"),
		//	timeGetTime(),pHitEvent->dwMisc,pHitEvent->dwSrcRoleID,pHitEvent->dwSerial,pHitEvent->dwMisc2);

		map<DWORD,tagHitTargetEvent>::iterator iter=m_eventMap.find(pHitEvent->dwSrcRoleID);
		if(iter==m_eventMap.end())
		{
			m_eventMap[pHitEvent->dwSrcRoleID]=*pHitEvent;
		}
		else
		{
			if(pHitEvent->dwSerial<iter->second.dwSerial)
				return;
			if(pHitEvent->dwSerial==iter->second.dwSerial
				&&pHitEvent->dwMisc2<iter->second.dwMisc2)
				return;
			iter->second=*pHitEvent;
		}
	}
}

void HitTargetEventHandler_NPC::OnNetCmd( tagNetCmd* pNetCmd )
{}

tagHitTargetEvent* HitTargetEventHandler_NPC::GetLastHitTargetEvent( DWORD roleID )
{
	map<DWORD,tagHitTargetEvent>::iterator iter=m_eventMap.find(roleID);
	if(iter==m_eventMap.end())
		return NULL;

	return &iter->second;
}