#include "StdAfx.h"
#include "StateStandOnCarrier.h"
#include "Role.h"
#include "GameEvent.h"
#include "FSM.h"
#include "ClientApp.h"

StateStandOnCarrier::StateStandOnCarrier(void)
{
}

StateStandOnCarrier::~StateStandOnCarrier(void)
{
}

void StateStandOnCarrier::Active( tagGameEvent* pEvent )
{
	tagStandOnCarrierEvent* pStandEvent=(tagStandOnCarrierEvent*)pEvent;

	m_dwCarrierObjID=pStandEvent->dwCarrierObjID;

	Role* pRole=m_pFSM->GetRole();
	pRole->PlayTrack(_T("std1"),true);

	NavMap* pNav=App()->GetMapView()->GetNav();
	NavCarrierNode* pCarrier=pNav->GetCollider()->FindCarrier(m_dwCarrierObjID);
	if(pCarrier!=NULL)
	{
		pCarrier->PosToLocal(pRole->GetPos(),pStandEvent->startTime,m_localPos);
	}
}

void StateStandOnCarrier::Update()
{
	NavMap* pNav=App()->GetMapView()->GetNav();
	Role* pRole=m_pFSM->GetRole();

	NavCarrierNode* pCarrier=pNav->GetCollider()->FindCarrier(m_dwCarrierObjID);
	if(pCarrier!=NULL)
	{
		Vector3 nowPos;
		pCarrier->PosToWorld(m_localPos,Kernel::Inst()->GetAccumTime(),nowPos);
		pRole->SetPos(nowPos);
	}
}

void StateStandOnCarrier::OnGameEvent( tagGameEvent* pEvent )
{
	if(pEvent->event==EGE_Move)
	{
		tagMoveEvent* pMoveEvent=(tagMoveEvent*)pEvent;
		pMoveEvent->dwCarrierObjID=m_dwCarrierObjID;
		m_pFSM->Change2State(FSM::EState_MoveOnCarrier,pEvent);
	}
}