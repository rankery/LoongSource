#include "StdAfx.h"
#include "UpperStateUseItem_LP.h"
#include "RoleMgr.h"
#include "FSM_LP.h"
#include "LocalPlayer.h"
#include "CombatEvent.h"
#include "..\WorldDefine\msg_combat.h"
#include "SkillMgr.h"
#include "ItemProtoData.h"
#include "CombatSysUtil.h"
#include "CombatActionChecker.h"
#include "FishMgr.h"
#include "Item.h"

UpperStateUseItem_LP::UpperStateUseItem_LP(void)
{}

UpperStateUseItem_LP::~UpperStateUseItem_LP(void)
{}

void UpperStateUseItem_LP::Active( tagGameEvent* pEvent,NavMap* pNav,LocalPlayer* pRole )
{
	ASSERT(pEvent->strEventName==_T("tagUseItemEvent"));
	tagUseItemEvent* pAttackEvent=(tagUseItemEvent*)pEvent;

	m_targetID=pAttackEvent->dwTargetRoleID;
	m_itemID=pAttackEvent->n64ItemID;
	m_itemTypeID=pAttackEvent->dwItemTypeID;
	m_dwSerial=CombatSysUtil::MakeSkillMsgSerial();
	m_eventCache.eventType=tagEventCache::EET_None;

	m_step=EStep_None;

	//--获得技能相关属性
	const tagItemDisplayInfo* pItemDisplay=ItemProtoData::Inst()->FindItemDisplay(m_itemTypeID);
	THROW_FALSE(P_VALID(pItemDisplay),_T("cannot find item display info"));

	const tagItemProto* pItemProto=ItemProtoData::Inst()->FindItemProto(m_itemTypeID);
	THROW_FALSE(P_VALID(pItemProto),_T("cannot find item proto"));

	m_bAttackItem	= false;
	m_bSpellItem	= pItemProto->nPrepareTime>0;
	m_bMoveable		= pItemProto->bMoveable==TRUE;
	m_bAniBlend		= pItemDisplay->bActionBlend==TRUE;
	m_bMoveSpell	= pItemProto->bInterruptMove==FALSE;

	//--以自已为中心的爆炸物品
	if(m_targetID==GT_INVALID)
	{
		if(m_bSpellItem)
		{
			//停止移动
			if(!m_bMoveSpell)
				m_pFSM->Change2LowerState_AutoStand();

			m_actSpell.Active(pRole,NULL,m_itemID,m_itemTypeID,m_dwSerial);
			m_step=EStep_Spell;
		}
		else
		{
			///停止移动
			if(!m_bMoveable)
				m_pFSM->Change2LowerState_AutoStand();

			m_actAttack.Active(pRole,NULL,m_itemID,m_itemTypeID,m_dwSerial,true);
			m_step=EStep_Attack;
		}
	}
	//--对自已使用
	else if(m_targetID==RoleMgr::Inst()->GetLocalPlayerID())
	{
		if(m_bSpellItem)
		{
			//停止移动
			if(!m_bMoveSpell)
				m_pFSM->Change2LowerState_AutoStand();

			m_actSpell.Active(pRole,pRole,m_itemID,m_itemTypeID,m_dwSerial);
			m_step=EStep_Spell;
		}
		else
		{
			//停止移动
			if(!m_bMoveable)
				m_pFSM->Change2LowerState_AutoStand();

			m_actAttack.Active(pRole,pRole,m_itemID,m_itemTypeID,m_dwSerial,true);
			m_step=EStep_Attack;
		}
	}
	//--对别人使用
	else
	{
		//不支持
		m_pFSM->Change2UpperState_Idle();
		return;
	}
}

void UpperStateUseItem_LP::Update( NavMap* pNav,LocalPlayer* pRole )
{
	//--起手-----------------------------------------------------------
	if(m_step==EStep_Spell)
	{
		//--检查目标是否存在
		Role* pTarget=NULL;
		if(m_targetID!=GT_INVALID)
		{
			pTarget=RoleMgr::Inst()->FindRole(m_targetID,false,true);
			if(pTarget==NULL)
			{
				m_pFSM->Change2UpperState_Idle();
				return;
			}
		}

		//--
		m_actSpell.Update(pRole,pTarget);
		if(m_actSpell.IsClosed())//吟唱时间结束
		{
			m_actAttack.Active(pRole,pTarget,m_itemID,m_itemTypeID,m_dwSerial,false);
			m_step=EStep_Attack;
			return;
		}		
	}
	//--攻击-----------------------------------------------------------
	else if(m_step==EStep_Attack)
	{
		//--检查目标是否存在
		Role* pTarget=NULL;
		if(m_targetID!=GT_INVALID)
		{
			pTarget=RoleMgr::Inst()->FindRole(m_targetID,false,true);
			if(pTarget==NULL)
			{
				m_pFSM->Change2UpperState_Idle();
				return;
			}
		}

		//--
		m_actAttack.Update(pRole,pTarget);
		if(m_actAttack.IsClosed())
		{
			//--检查是否有缓存的游戏事件
			if(m_eventCache.eventType!=tagEventCache::EET_None)
			{
				if(m_eventCache.eventType==tagEventCache::EET_UseSkill)
				{
					if(!m_pFSM->TryChange2UpperState(FSM_LP::EUpperState_Skill,&m_eventCache.useSkill))
						m_pFSM->Change2UpperState_Idle();
				}
				else if(m_eventCache.eventType==tagEventCache::EET_UseItem)
				{
					if(!m_pFSM->TryChange2UpperState(FSM_LP::EUpperState_UseItem,&m_eventCache.useItem))
						m_pFSM->Change2UpperState_Idle();
				}
				else
					m_pFSM->Change2UpperState_Idle();
			}
			//--切换到空闲状态
			else
				m_pFSM->Change2UpperState_Idle();
		}
	}
}

void UpperStateUseItem_LP::OnGameEvent( tagGameEvent* pEvent )
{
	//--起手-----------------------------------------------------------
	if(m_step==EStep_Spell)
	{
/*		if(pEvent->strEventName==_T("tagUseSkillEvent"))
		{
			m_actSpell.Stop(m_pFSM->GetLocalPlayer(),true);
			m_pFSM->TryChange2UpperState(FSM_LP::EUpperState_Skill,pEvent);
		}
		else if(pEvent->strEventName==_T("tagUseItemEvent"))
		{
			m_actSpell.Stop(m_pFSM->GetLocalPlayer(),true);
			m_pFSM->TryChange2UpperState(FSM_LP::EUpperState_UseItem,pEvent);
		}
		else */if(pEvent->strEventName==_T("tagFollowRoleEvent"))
		{
			m_actSpell.Stop(m_pFSM->GetLocalPlayer(),true);
			m_pFSM->TryChange2UpperState(FSM_LP::EUpperState_Follow,pEvent);
		}
		else if(pEvent->strEventName==_T("tagTalkNPCEvent"))
		{
			m_actSpell.Stop(m_pFSM->GetLocalPlayer(),true);
			m_pFSM->TryChange2UpperState(FSM_LP::EUpperState_TalkNPC,pEvent);
		}
		else if(pEvent->strEventName==_T("tagPickupItemEvent"))
		{
			m_actSpell.Stop(m_pFSM->GetLocalPlayer(),true);
			m_pFSM->TryChange2UpperState(FSM_LP::EUpperState_PickupItem,pEvent);
		}
		else if(pEvent->strEventName==_T("tagPlayPersonalityActEvent"))
		{
			m_actSpell.Stop(m_pFSM->GetLocalPlayer(),true);
			m_pFSM->TryChange2UpperState(FSM_LP::EUpperState_StyleAction,pEvent);
		}
		else if(pEvent->strEventName==_T("tagESCCancelEvent"))
		{
			tagESCCancelEvent* pCancelEvent=(tagESCCancelEvent*)pEvent;

			m_actSpell.Stop(m_pFSM->GetLocalPlayer(),true);
			m_pFSM->Change2UpperState_Idle();

			pCancelEvent->bCanceled=TRUE;
		}
	}
	//--攻击-----------------------------------------------------------
	else if(m_step==EStep_Attack)
	{
		if(pEvent->strEventName==_T("tagUseSkillEvent"))
		{
			tagUseSkillEvent* pSkillEvent=(tagUseSkillEvent*)pEvent;

			//缓存起来
			m_eventCache.eventType=tagEventCache::EET_UseSkill;
			m_eventCache.useSkill=*pSkillEvent;
		}
		else if(pEvent->strEventName==_T("tagUseItemEvent"))
		{
			tagUseItemEvent* pItemEvent=(tagUseItemEvent*)pEvent;

			//缓存起来
			m_eventCache.eventType=tagEventCache::EET_UseItem;
			m_eventCache.useItem=*pItemEvent;
		}
	}
}

void UpperStateUseItem_LP::OnNetCmd( tagNetCmd* pNetCmd )
{
	if(pNetCmd->dwID==TObjRef<Util>()->Crc32("NS_UseItem"))
	{
		tagNS_UseItem* pItemCmd=(tagNS_UseItem*)pNetCmd;
		if(pItemCmd->dwSerial==m_dwSerial)
		{
			if(pItemCmd->dwErrorCode==E_Success)
			{
				if(m_step==EStep_Spell)
				{
					m_actSpell.OnRecvSpellTime(pItemCmd->nSpellTime);
				}
			}
			else//使用物品失败
			{
				IMSG(_T("使用物品时服务器返回错误代码：%d"),pItemCmd->dwErrorCode);
				m_pFSM->Change2UpperState_Idle();
			}
		}
	}
	if(pNetCmd->dwID==pNetCmd->Crc32("NS_UseItemInterrupt"))//打断
	{
		tagNS_UseItemInterrupt* pCmd = (tagNS_UseItemInterrupt*)pNetCmd;
		if( pCmd->n64ItemID == m_itemID )
		{
			m_pFSM->Change2UpperState_Idle();
		}
	}
}

void UpperStateUseItem_LP::OnExit()
{
	if(m_step==EStep_Attack)
		m_actAttack.Stop(m_pFSM->GetLocalPlayer());
	else if(m_step==EStep_Spell)
		m_actSpell.Stop(m_pFSM->GetLocalPlayer(),false);
}

bool UpperStateUseItem_LP::IfCanChange2UpperState(FSM_LP::EUpperState state,tagGameEvent* pEvent)
{
	return true;
}

bool UpperStateUseItem_LP::IfCanChange2LowerState(FSM_LP::ELowerState state,tagGameEvent* pEvent)
{
	//--起手---------------------------------------
	if(m_step==EStep_Spell)
	{
		return true;
	}
	//--攻击---------------------------------------
	else if(m_step==EStep_Attack)
	{
		if(m_bMoveable)//可以移动攻击
		{
			switch(state)
			{
			case FSM_LP::ELowerState_KeyboardMove:
			case FSM_LP::ELowerState_MouseMove:
			case FSM_LP::ELowerState_Stand:
			case FSM_LP::ELowerState_KeyboardSwim:
			case FSM_LP::ELowerState_MouseSwim:
			case FSM_LP::ELowerState_Float:
			case FSM_LP::ELowerState_KeyboardWaterWalk:
			case FSM_LP::ELowerState_MouseWaterWalk:
			case FSM_LP::ELowerState_WaterStand:
				return true;
			}
		}
		else//不可以移动攻击
		{}
	}

	return false;
}

void UpperStateUseItem_LP::OnChangeLowerState(FSM_LP::ELowerState newState,tagGameEvent* pEvent)
{
	//--起手---------------------------------------
	if(m_step==EStep_Spell)
	{
		if(m_bMoveSpell)//可以移动起手
		{
			switch(newState)
			{
			case FSM_LP::ELowerState_KeyboardMove:
			case FSM_LP::ELowerState_MouseMove:
			case FSM_LP::ELowerState_Stand:
			case FSM_LP::ELowerState_KeyboardSwim:
			case FSM_LP::ELowerState_MouseSwim:
			case FSM_LP::ELowerState_Float:
			case FSM_LP::ELowerState_KeyboardWaterWalk:
			case FSM_LP::ELowerState_MouseWaterWalk:
			case FSM_LP::ELowerState_WaterStand:
				break;
			default:
				{
					m_actSpell.Stop(m_pFSM->GetLocalPlayer(),true);
					m_pFSM->Change2UpperState_Idle();
				}
				break;
			}
		}
		else//不可以移动起手
		{
			switch(newState)
			{
			case FSM_LP::ELowerState_Stand:
			case FSM_LP::ELowerState_Float:
			case FSM_LP::ELowerState_WaterStand:
				break;
			default:
				{
					m_actSpell.Stop(m_pFSM->GetLocalPlayer(),true);
					m_pFSM->Change2UpperState_Idle();
				}
				break;
			}
		}
	}
	//--攻击---------------------------------------
	else if(m_step==EStep_Attack)
	{
		if(m_bMoveable)//可以移动攻击
		{
			switch(newState)
			{
			case FSM_LP::ELowerState_KeyboardMove:
			case FSM_LP::ELowerState_MouseMove:
			case FSM_LP::ELowerState_Stand:
			case FSM_LP::ELowerState_KeyboardSwim:
			case FSM_LP::ELowerState_MouseSwim:
			case FSM_LP::ELowerState_Float:
			case FSM_LP::ELowerState_KeyboardWaterWalk:
			case FSM_LP::ELowerState_MouseWaterWalk:
			case FSM_LP::ELowerState_WaterStand:
				break;
			default:
				{
					m_actAttack.Stop(m_pFSM->GetLocalPlayer());
					m_pFSM->Change2UpperState_Idle();
				}
				break;
			}
		}
		else//不可以移动攻击
		{}
	}

}

bool UpperStateUseItem_LP::IfCanPlayLowerAni()
{
	if(m_step==EStep_None)
	{
		return true;
	}
	//--起手---------------------------------------
	else if(m_step==EStep_Spell)
	{
		return true;
	}
	//--攻击---------------------------------------
	else if(m_step==EStep_Attack)
	{
		if(m_bMoveable)//可以移动攻击
		{
			if(m_bAniBlend)
				return true;
		}
		else//不可以移动攻击
		{}
	}

	return false;
}

bool UpperStateUseItem_LP::IfLowerCanSetYaw()
{
	return true;
}
