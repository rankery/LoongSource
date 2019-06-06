#include "StdAfx.h"
#include "UpperStatePickup_LP.h"
#include "RoleMgr.h"
#include "FSM_LP.h"
#include "LocalPlayer.h"
#include "CombatEvent.h"
#include "GroundItemMgr.h"
#include "..\WorldDefine\msg_loot.h"
#include "CombatSysUtil.h"

const float PICKUP_ITEM_DIST=200.0f;

UpperStatePickup_LP::UpperStatePickup_LP(void) :
m_itemPos(0.0f, 0.0f, 0.0f),
m_itemID(GT_INVALID)
{}

UpperStatePickup_LP::~UpperStatePickup_LP(void)
{}

void UpperStatePickup_LP::Active( tagGameEvent* pEvent,NavMap* pNav,LocalPlayer* pRole )
{
	ASSERT(pEvent->strEventName==_T("tagPickupItemEvent"));
	tagPickupItemEvent* pPickupEvent=(tagPickupItemEvent*)pEvent;

	m_itemID=pPickupEvent->n64ItemID;

	//--检查物品是否存在
	tagGroundItem* pItem=GroundItemMgr::Inst()->FindItem(m_itemID);
	if(!P_VALID(pItem))
	{
		m_pFSM->Change2UpperState_Idle();
		GroundItemMgr::Inst()->ShowErrMsg(E_Loot_Item_Not_Found);
		return;
	}

	m_itemPos=pItem->space.GetPos();

	//--判断是否在拾取范围内
	if(Vec3DistSq(pRole->GetPos(),m_itemPos)<PICKUP_ITEM_DIST*PICKUP_ITEM_DIST)
	{
		//停止移动
		m_pFSM->Change2LowerState_AutoStand();

		GroundItemMgr::Inst()->PickUpItem(m_itemID,0);
		m_pFSM->Change2UpperState_Idle();
		return;
	}

	//--如果不在范围内，就跑过去吧
	SendMouseMoveEvent(pRole->GetPos(),m_itemPos);
}

void UpperStatePickup_LP::Update( NavMap* pNav,LocalPlayer* pRole )
{
	//--判断是否在拾取范围内
	if(Vec3DistSq(pRole->GetPos(),m_itemPos)<PICKUP_ITEM_DIST*PICKUP_ITEM_DIST)
	{
		//停止移动
		if(m_pFSM->IsMoveState())
			m_pFSM->Change2LowerState_AutoStand();

		GroundItemMgr::Inst()->PickUpItem(m_itemID,0);
		m_pFSM->Change2UpperState_Idle();
	}
}

void UpperStatePickup_LP::OnGameEvent( tagGameEvent* pEvent )
{
	if(pEvent->strEventName==_T("tagUseSkillEvent"))
	{
		m_pFSM->TryChange2UpperState(FSM_LP::EUpperState_Skill,pEvent);
	}
	else if(pEvent->strEventName==_T("tagUseItemEvent"))
	{
		m_pFSM->TryChange2UpperState(FSM_LP::EUpperState_UseItem,pEvent);
	}
	else if(pEvent->strEventName==_T("tagFollowRoleEvent"))
	{
		m_pFSM->TryChange2UpperState(FSM_LP::EUpperState_Follow,pEvent);
	}
	else if(pEvent->strEventName==_T("tagTalkNPCEvent"))
	{
		m_pFSM->TryChange2UpperState(FSM_LP::EUpperState_TalkNPC,pEvent);
	}
	else if(pEvent->strEventName==_T("tagPickupItemEvent"))
	{
		m_pFSM->TryChange2UpperState(FSM_LP::EUpperState_PickupItem,pEvent);
	}
	else if(pEvent->strEventName==_T("tagPlayPersonalityActEvent"))
	{
		m_pFSM->TryChange2UpperState(FSM_LP::EUpperState_StyleAction,pEvent);
	}
}

void UpperStatePickup_LP::OnNetCmd( tagNetCmd* pNetCmd )
{}

bool UpperStatePickup_LP::IfCanChange2UpperState( FSM_LP::EUpperState state,tagGameEvent* pEvent )
{
	return true;
}

bool UpperStatePickup_LP::IfCanChange2LowerState( FSM_LP::ELowerState state,tagGameEvent* pEvent )
{
	return true;
}

void UpperStatePickup_LP::OnChangeLowerState( FSM_LP::ELowerState newState,tagGameEvent* pEvent )
{
	switch(newState)
	{
	case FSM_LP::ELowerState_Jump:
	case FSM_LP::ELowerState_Slide:
	case FSM_LP::ELowerState_VDrop:
	case FSM_LP::ELowerState_Drop:
	case FSM_LP::ELowerState_Stand:
	case FSM_LP::ELowerState_Float:
	case FSM_LP::ELowerState_WaterStand:
		break;
	case FSM_LP::ELowerState_MouseMove:
	case FSM_LP::ELowerState_MouseSwim:
	case FSM_LP::ELowerState_MouseWaterWalk:
		{
			tagMouseMoveEvent* pMouseMove=(tagMouseMoveEvent*)pEvent;
			if(pMouseMove->bUserCtrl)
				m_pFSM->Change2UpperState_Idle();
		}
		break;
	default:
		{
			m_pFSM->Change2UpperState_Idle();
		}
		break;
	}
}

bool UpperStatePickup_LP::IfCanPlayLowerAni()
{
	return true;
}

bool UpperStatePickup_LP::IfLowerCanSetYaw()
{
	return true;
}

void UpperStatePickup_LP::SendMouseMoveEvent(const Vector3& start, const Vector3& end)
{
	tagMouseMoveEvent event;
	event.start=start;
	event.end=end;
	event.bUserCtrl=false;
	event.validDist=PICKUP_ITEM_DIST*0.9f;
	m_pFSM->OnGameEvent(&event);
}
