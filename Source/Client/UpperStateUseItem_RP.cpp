#include "StdAfx.h"
#include "UpperStateUseItem_RP.h"
#include "RoleMgr.h"
#include "FSM_RP.h"
#include "Player.h"
#include "..\WorldDefine\msg_combat.h"
#include "ItemProtoData.h"

UpperStateUseItem_RP::UpperStateUseItem_RP(void)
{}

UpperStateUseItem_RP::~UpperStateUseItem_RP(void)
{}

void UpperStateUseItem_RP::Active( tagNetCmd* pNetCmd,NavMap* pNav,Player* pRole )
{
	ASSERT(pNetCmd->dwID==pNetCmd->Crc32("NS_UseItem"));
	tagNS_UseItem* pItemCmd=(tagNS_UseItem*)pNetCmd;

	m_targetID=pItemCmd->dwTarRoleID;
	m_itemTypeID=pItemCmd->dwTypeID;
	m_itemID=pItemCmd->n64ItemID;
	m_serialID=pItemCmd->dwSerial;

	//--获得技能相关属性
	const tagItemDisplayInfo* pItemDisplay=ItemProtoData::Inst()->FindItemDisplay(m_itemTypeID);
	THROW_FALSE(P_VALID(pItemDisplay),_T("cannot find item display info"));

	const tagItemProto* pItemProto=ItemProtoData::Inst()->FindItemProto(m_itemTypeID);
	THROW_FALSE(P_VALID(pItemProto),_T("cannot find item proto"));

	m_bMoveable		= pItemProto->bMoveable==TRUE;
	m_bAniBlend		= pItemDisplay->bActionBlend==TRUE;

	//--检查目标是否存在
	Role* pTarget=NULL;
	if(m_targetID!=GT_INVALID)
	{
		pTarget=RoleMgr::Inst()->FindRole(m_targetID);
		if(pTarget==NULL)
		{
			m_pFSM->Change2UpperState_Idle();
			return;
		}
	}

	//--
	if(pItemCmd->nSpellTime>0)
	{
		m_actSpell.Active(pRole,pTarget,m_itemTypeID,pItemCmd->nSpellTime);
		m_step=EStep_Spell;
	}
	else
	{
		m_actAttack.Active(pRole,pTarget,m_itemTypeID,pItemCmd->dwSerial);
		m_step=EStep_Attack;
	}
}

void UpperStateUseItem_RP::Update( NavMap* pNav,Player* pRole )
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

	if(m_step==EStep_Spell)
	{
		m_actSpell.Update(pRole,pTarget);
		if(m_actSpell.IsClosed())
		{
			m_actAttack.Active(pRole,pTarget,m_itemTypeID,m_serialID);
			m_step=EStep_Attack;
		}
	}
	else if(m_step==EStep_Attack)
	{
		m_actAttack.Update(pRole,pTarget);
		if(m_actAttack.IsClosed())
		{
			m_pFSM->Change2UpperState_Idle();
		}
	}
}

void UpperStateUseItem_RP::OnNetCmd( tagNetCmd* pNetCmd )
{
	if(pNetCmd->dwID==pNetCmd->Crc32("NS_UseItemInterrupt"))//打断技能
	{
		tagNS_UseItemInterrupt* pCmd = (tagNS_UseItemInterrupt*)pNetCmd;
		if( pCmd->n64ItemID == m_itemID )
		{
			m_pFSM->Change2UpperState_Idle();
		}
	}
	else
	{
		m_pFSM->Change2UpperState(pNetCmd);
	}
}

void UpperStateUseItem_RP::OnGameEvent(tagGameEvent* pEvent)
{
	if(m_step==EStep_Spell && pEvent->strEventName==_T("tagTargetSelectEvent"))
		m_actSpell.OnTargetSelectEvent((tagTargetSelectEvent*)pEvent,m_pFSM->GetPlayer());
}

void UpperStateUseItem_RP::OnExit()
{
	if(m_step==EStep_Spell)
		m_actSpell.Stop(m_pFSM->GetPlayer());
	else if(m_step==EStep_Attack)
		m_actAttack.Stop(m_pFSM->GetPlayer());
}

bool UpperStateUseItem_RP::IfCanPlayLowerAni()
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
			if(m_bAniBlend)
				return true;
		}
		else//不可以移动攻击
		{}
	}

	return false;
}

bool UpperStateUseItem_RP::IfLowerCanSetYaw()
{
	return true;
}