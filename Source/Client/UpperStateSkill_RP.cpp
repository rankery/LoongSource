#include "StdAfx.h"
#include "UpperStateSkill_RP.h"
#include "RoleMgr.h"
#include "FSM_RP.h"
#include "Player.h"
#include "..\WorldDefine\msg_combat.h"
#include "SkillProtoData.h"
#include "PlayerActionData.h"

UpperStateSkill_RP::UpperStateSkill_RP(void)
{}

UpperStateSkill_RP::~UpperStateSkill_RP(void)
{}

void UpperStateSkill_RP::Active( tagNetCmd* pNetCmd,NavMap* pNav,Player* pRole )
{
	m_startSpellTime = 0;
	m_spellRealTime = GT_INVALID;

	ASSERT(pNetCmd->dwID==pNetCmd->Crc32("NS_Skill"));
	tagNS_Skill* pSkillCmd=(tagNS_Skill*)pNetCmd;

	m_targetID=pSkillCmd->dwTarRoleID;
	m_skillID=pSkillCmd->dwSkillID;
	m_serialID=pSkillCmd->dwSerial;

	//--获得技能相关属性
	const tagSkillProtoClient* pSkillProto=SkillProtoData::Inst()->FindSkillProto(m_skillID);
	THROW_FALSE(P_VALID(pSkillProto),_T("cannot find skill proto"));
	m_bMoveable		= pSkillProto->bMoveable==TRUE;
	m_bAniBlend		= pSkillProto->bActionBlend==TRUE;

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
	if(pSkillCmd->nSpellTime>0)
	{
		// 缩短吟唱动作播放时间，预播攻击动作
		DWORD dwAttackAdvanceTime = 0;
		const tagPlayerActionData* pActData = PlayerActionData::Inst()->FindData( pSkillProto->szActionMod );
		if( NULL != pActData )
			dwAttackAdvanceTime = pActData->dwAttackTime;

		if( (DWORD)pSkillCmd->nSpellTime > dwAttackAdvanceTime )
		{
			m_actSpell.Active(pRole,pTarget,m_skillID,pSkillCmd->nSpellTime-dwAttackAdvanceTime);
			m_step=EStep_Spell;
		}
		else
		{
			m_actAttack.Active(pRole,pTarget,pSkillCmd->dwSkillID,pSkillCmd->dwSerial);
			m_step=EStep_Attack;
		}

		m_spellRealTime=pSkillCmd->nSpellTime;
		m_startSpellTime=Kernel::Inst()->GetAccumTimeDW();

		//--发送角色开始吟唱事件
		tagRoleSpellStartEvent evt;
		evt.dwRoleID=pRole->GetID();
		evt.bSkill=true;
		evt.dwTypeID=m_skillID;
		evt.dwCurTime=0;
		evt.dwTimeCount=m_spellRealTime;
		TObjRef<GameFrameMgr>()->SendEvent(&evt);
	}
	else
	{
		m_actAttack.Active(pRole,pTarget,pSkillCmd->dwSkillID,pSkillCmd->dwSerial);
		m_step=EStep_Attack;
	}
}

void UpperStateSkill_RP::Update( NavMap* pNav,Player* pRole )
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
			m_actAttack.Active(pRole,pTarget,m_skillID,m_serialID);
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

	if( GT_INVALID != m_spellRealTime &&
		Kernel::Inst()->GetAccumTimeDW()-m_startSpellTime>=m_spellRealTime )
	{
		//--发送角色结束吟唱事件
		tagRoleSpellStopEvent evt;
		evt.dwRoleID=pRole->GetID();
		evt.bInterrupt=false;
		TObjRef<GameFrameMgr>()->SendEvent(&evt);

		m_startSpellTime = 0;
		m_spellRealTime = GT_INVALID;
	}
}

void UpperStateSkill_RP::OnNetCmd( tagNetCmd* pNetCmd )
{
	if(pNetCmd->dwID==pNetCmd->Crc32("NS_SkillInterrupt"))//打断技能
	{
		tagNS_SkillInterrupt* pCmd = (tagNS_SkillInterrupt*)pNetCmd;
		if( pCmd->dwSkillID == m_skillID )
			m_pFSM->Change2UpperState_Idle();
	}
	else
	{
		m_pFSM->Change2UpperState(pNetCmd);
	}
}

void UpperStateSkill_RP::OnGameEvent(tagGameEvent* pEvent)
{
	if(m_step==EStep_Spell && pEvent->strEventName==_T("tagTargetSelectEvent"))
	{
		//--发送角色开始吟唱事件
		tagRoleSpellStartEvent evt;
		evt.dwRoleID=m_pFSM->GetPlayer()->GetID();
		evt.bSkill=true;
		evt.dwTypeID=m_skillID;
		evt.dwCurTime=Kernel::Inst()->GetAccumTimeDW()-m_startSpellTime;
		evt.dwTimeCount=m_spellRealTime;
		TObjRef<GameFrameMgr>()->SendEvent(&evt);
	}
}

void UpperStateSkill_RP::OnExit()
{
	if(m_step==EStep_Spell)
		m_actSpell.Stop(m_pFSM->GetPlayer());
	else if(m_step==EStep_Attack)
		m_actAttack.Stop(m_pFSM->GetPlayer());

	//--发送角色结束吟唱事件
	if( GT_INVALID != m_spellRealTime )
	{
		tagRoleSpellStopEvent evt;
		evt.dwRoleID=m_pFSM->GetPlayer()->GetID();
		evt.bInterrupt=true;
		TObjRef<GameFrameMgr>()->SendEvent(&evt);

		m_startSpellTime = 0;
		m_spellRealTime = GT_INVALID;
	}
}

bool UpperStateSkill_RP::IfCanPlayLowerAni()
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

bool UpperStateSkill_RP::IfLowerCanSetYaw()
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