#include "StdAfx.h"
#include "GameEventHandler_LP.h"
#include "CombatEvent.h"
#include "FSM_LP.h"
#include "LocalPlayer.h"
#include "CombatEvent.h"
#include "EffectMgr.h"
#include "SkillProtoData.h"
#include "ItemProtoData.h"
#include "..\WorldDefine\remote_role.h"
#include "CombatSysUtil.h"

//--class GameEventHandler_LP-------------------------------------------
GameEventHandler_LP::GameEventHandler_LP(void):m_pFSM(NULL)
{}

GameEventHandler_LP::~GameEventHandler_LP(void)
{}




//--class HitTargetEventHandler_LP-------------------------------------------
HitTargetEventHandler_LP::HitTargetEventHandler_LP( void )
{}

HitTargetEventHandler_LP::~HitTargetEventHandler_LP( void )
{}

bool HitTargetEventHandler_LP::OnGameEvent( tagGameEvent* pEvent )
{
	if(pEvent->strEventName==_T("tagHitTargetEvent"))
	{
		tagHitTargetEvent* pHitEvent=(tagHitTargetEvent*)pEvent;

		map<DWORD,tagHitTargetEvent>::iterator iter=m_eventMap.find(pHitEvent->dwSrcRoleID);
		if(iter==m_eventMap.end())
		{
			m_eventMap[pHitEvent->dwSrcRoleID]=*pHitEvent;
		}
		else
		{
			if(pHitEvent->dwSerial<iter->second.dwSerial)
				return true;
			if(pHitEvent->dwSerial==iter->second.dwSerial
				&&pHitEvent->dwMisc2<iter->second.dwMisc2)
				return true;
			iter->second=*pHitEvent;
		}
	}

	return true;
}

void HitTargetEventHandler_LP::OnNetCmd( tagNetCmd* pNetCmd )
{
	if(pNetCmd->dwID==pNetCmd->Crc32("NS_RemoveRemote"))
	{
		tagNS_RemoveRemote* pCmd=(tagNS_RemoveRemote*)pNetCmd;
		int num = ( pCmd->dwSize - sizeof(tagNS_RemoveRemote) ) / sizeof(DWORD) + 1;
		for(int i=0;i<num;i++)
		{
			m_eventMap.erase(pCmd->dwRoleID[i]);
		}
	}
}

tagHitTargetEvent* HitTargetEventHandler_LP::GetLastHitTargetEvent( DWORD roleID )
{
	map<DWORD,tagHitTargetEvent>::iterator iter=m_eventMap.find(roleID);
	if(iter==m_eventMap.end())
		return NULL;

	return &iter->second;
}






//--class UseItemEventHandler_LP-------------------------------------------
UseItemEventHandler_LP::UseItemEventHandler_LP( void )
{}

UseItemEventHandler_LP::~UseItemEventHandler_LP( void )
{}

bool UseItemEventHandler_LP::OnGameEvent( tagGameEvent* pEvent )
{
	if(pEvent->strEventName==_T("tagUseItemEvent"))
	{
		tagUseItemEvent* pItemEvent=(tagUseItemEvent*)pEvent;
		
		const tagItemDisplayInfo* pItemDisplay=ItemProtoData::Inst()->FindItemDisplay(pItemEvent->dwItemTypeID);
		THROW_FALSE(P_VALID(pItemDisplay),_T("cannot find itemdisplayinfo"));

		const tagItemProto* pItemProto=ItemProtoData::Inst()->FindItemProto(pItemEvent->dwItemTypeID);
		THROW_FALSE(P_VALID(pItemProto),_T("cannot find itemproto"));
	
		//--对于没有起手和动作的物品，在这里处理
		if(pItemDisplay->szActionMod[0]==_T('\0')
			&&pItemProto->nPrepareTime==0)
		{
			CombatSysUtil::Inst()->SelfUseItemImmediate(pItemEvent->n64ItemID,
				pItemEvent->dwItemTypeID,
				m_pFSM->GetLocalPlayer()->GetID(),
				pItemEvent->dwTargetRoleID,
				m_pFSM->GetLocalPlayer()->GetPos());
			return false;
		}
	}

	return true;
}

void UseItemEventHandler_LP::OnNetCmd( tagNetCmd* pNetCmd )
{}


