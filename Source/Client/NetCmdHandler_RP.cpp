#include "StdAfx.h"
#include "NetCmdHandler_RP.h"
#include "CombatEvent.h"
#include "FSM_RP.h"
#include "LocalPlayer.h"
#include "..\WorldDefine\action.h"
#include "CombatEvent.h"
#include "SkillProtoData.h"
#include "ItemProtoData.h"
#include "EffectMgr.h"
#include "GameFrameMgr.h"
#include "RoleMgr.h"
#include "GroundItemMgr.h"
#include "RoleEvent.h"
#include "AudioSys.h"
#include "WorldFrame.h"
#include "CreatureData.h"
#include "NPC.h"

//--class NetCmdHander_RP-------------------------------------------
NetCmdHandler_RP::NetCmdHandler_RP(void):m_pFSM(NULL)
{}

NetCmdHandler_RP::~NetCmdHandler_RP(void)
{}




//--class NS_SyncStandHandler_RP-------------------------------------------
NS_SyncStandHandler_RP::NS_SyncStandHandler_RP()
{}

NS_SyncStandHandler_RP::~NS_SyncStandHandler_RP()
{}

bool NS_SyncStandHandler_RP::OnNetCmd( tagNetCmd* pNetCmd )
{
	if(pNetCmd->dwID==m_pUtil->Crc32("NS_SyncStand"))
	{
		tagNS_SyncStand* pCmd=(tagNS_SyncStand*)pNetCmd;

		float yaw=CalcYaw(pCmd->faceTo);
		m_pFSM->GetPlayer()->SetPos(pCmd->curPos);
		m_pFSM->GetPlayer()->SetYaw(yaw);
		m_pFSM->GetPlayer()->SetMoveYaw(yaw);

		m_pFSM->Change2LowerState_AutoStand();
		m_pFSM->Change2UpperState_Idle();
	}

	return true;
}

void NS_SyncStandHandler_RP::OnGameEvent( tagGameEvent* pEvent )
{}

void NS_SyncStandHandler_RP::Update()
{}






//--class NS_StopActionHandler_RP-------------------------------------------
NS_StopActionHandler_RP::NS_StopActionHandler_RP()
{}

NS_StopActionHandler_RP::~NS_StopActionHandler_RP()
{}

bool NS_StopActionHandler_RP::OnNetCmd( tagNetCmd* pNetCmd )
{
	if(pNetCmd->dwID==m_pUtil->Crc32("NS_StopAction"))
	{
		tagNS_StopAction* pCmd=(tagNS_StopAction*)pNetCmd;

		float yaw=CalcYaw(pCmd->faceTo);
		m_pFSM->GetPlayer()->SetPos(pCmd->curPos);
		m_pFSM->GetPlayer()->SetYaw(yaw);
		m_pFSM->GetPlayer()->SetMoveYaw(yaw);

		m_pFSM->Change2LowerState_AutoStand();
		m_pFSM->Change2UpperState_Idle();
	}

	return true;
}

void NS_StopActionHandler_RP::OnGameEvent( tagGameEvent* pEvent )
{}

void NS_StopActionHandler_RP::Update()
{}







//--class NS_HPChangeHandler_RP-------------------------------------------
NS_HPChangeHandler_RP::NS_HPChangeHandler_RP()
{}

NS_HPChangeHandler_RP::~NS_HPChangeHandler_RP()
{}

bool NS_HPChangeHandler_RP::OnNetCmd( tagNetCmd* pNetCmd )
{
	if(pNetCmd->dwID==m_pUtil->Crc32("NS_RoleHPChange"))
	{
		tagNS_RoleHPChange* pCmd=(tagNS_RoleHPChange*)pNetCmd;
		if(pCmd->dwRoleID==m_pFSM->GetPlayer()->GetID())
		{
			if(IfNeedCacheCmd(pCmd))
			{
				tagData data;
				data.recvTime=Kernel::Inst()->GetAccumTimeDW();
				data.cmd=*pCmd;
				m_cache.push_back(data);
			}
			else
			{
				SendShowHPChangeEvent(pCmd);
			}
		}
	}

	return true;
}

void NS_HPChangeHandler_RP::OnGameEvent( tagGameEvent* pEvent )
{
	if(pEvent->strEventName==_T("tagHitTargetEvent"))
	{
		tagHitTargetEvent* pHitEvent=(tagHitTargetEvent*)pEvent;
		if(pHitEvent->eCause==EHTC_Skill)
		{
			for(list<tagData>::iterator iter=m_cache.begin();
				iter!=m_cache.end();)
			{
				if(IfNeedClearCmd(&iter->cmd,pHitEvent))
				{
					//显示伤害
					SendShowHPChangeEvent(&iter->cmd);

					//发送被攻击事件
					SendBeAttackEvent(&iter->cmd);

					//暴击音效
					PlayCritSound(&iter->cmd);

					//镜头震动
					if(iter->cmd.bCrit
						&&iter->cmd.dwSrcRoleID==RoleMgr::Inst()->GetLocalPlayerID())
						PlayQuake();

					iter=m_cache.erase(iter);
				}
				else
					iter++;
			}
		}
	}
}

void NS_HPChangeHandler_RP::Update()
{
	for(list<tagData>::iterator iter=m_cache.begin();
		iter!=m_cache.end();)
	{
		if(Kernel::Inst()->GetAccumTimeDW()-iter->recvTime>=3000)
		{
			SendShowHPChangeEvent(&iter->cmd);
			iter=m_cache.erase(iter);
		}
		else
			iter++;
	}
}

void NS_HPChangeHandler_RP::SendShowHPChangeEvent( tagNS_RoleHPChange* pCmd )
{
	tagShowHPChangeEvent event(_T("tagShowHPChangeEvent"),NULL);
	event.dwRoleID		= pCmd->dwRoleID;
	event.dwSrcRoleID	= pCmd->dwSrcRoleID;
	event.eCause		= pCmd->eCause;
	event.bMiss			= pCmd->bMiss;
	event.bCrit			= pCmd->bCrit;
	event.bBlocked		= pCmd->bBlocked;
	event.nHPChange		= pCmd->nHPChange;
	m_pGameFrameMgr->SendEvent(&event);
}

void NS_HPChangeHandler_RP::SendBeAttackEvent( tagNS_RoleHPChange* pCmd )
{
	tagBeAttackEvent event(_T("tagBeAttackEvent"),NULL);
	event.bMiss			= pCmd->bMiss;
	event.bBlock		= pCmd->bBlocked;
	m_pFSM->OnGameEvent(&event);
}

bool NS_HPChangeHandler_RP::IfNeedCacheCmd( tagNS_RoleHPChange* pCmd )
{
	if(pCmd->eCause==ERHPCC_SkillDamage)
	{
		tagHitTargetEvent* pEvent=m_pFSM->GetLastHitTargetEvent(pCmd->dwSrcRoleID);
		if(pEvent==NULL)
			return true;

		if(pCmd->dwSerial>pEvent->dwSerial)
			return true;

		if(pCmd->dwSerial==pEvent->dwSerial
			&&pCmd->dwMisc2>pEvent->dwMisc2)
			return true;
	}

	return false;
}

bool NS_HPChangeHandler_RP::IfNeedClearCmd( tagNS_RoleHPChange* pCacheCmd,tagHitTargetEvent* pEvent )
{
	if(pCacheCmd->dwSrcRoleID==pEvent->dwSrcRoleID)
	{
		if(pEvent->dwSerial>pCacheCmd->dwSerial)
			return true;

		if(pEvent->dwSerial==pCacheCmd->dwSerial 
			&&pEvent->dwMisc2>=pCacheCmd->dwMisc2)
			return true;
	}

	return false;
}

void NS_HPChangeHandler_RP::PlayCritSound( tagNS_RoleHPChange* pCmd )
{
	if(!pCmd->bCrit)
		return;

	if(pCmd->dwSrcRoleID!=RoleMgr::Inst()->GetLocalPlayerID())
		return;

	TObjRef<AudioSys>()->Play3DSound(_T("crit"),100.0f,100.0f*50.0f,RoleMgr::Inst()->GetLocalPlayer()->GetPos(),SOUND_NORMAL);
}

void NS_HPChangeHandler_RP::PlayQuake()
{
	WorldFrame* pWorldFrm=GetWorldFrm();
	if(!P_VALID(pWorldFrm))
		return;

	pWorldFrm->GetCamera()->PlayQuake();
}

//--class NS_RoleDeadHandler_RP-------------------------------------------
NS_RoleDeadHandler_RP::NS_RoleDeadHandler_RP()
{}

NS_RoleDeadHandler_RP::~NS_RoleDeadHandler_RP()
{}

bool NS_RoleDeadHandler_RP::OnNetCmd( tagNetCmd* pNetCmd )
{
	if(pNetCmd->dwID==m_pUtil->Crc32("NS_RoleDead"))
	{
		tagNS_RoleDead* pCmd=(tagNS_RoleDead*)pNetCmd;
		if(pCmd->dwRoleID==m_pFSM->GetPlayer()->GetID())
		{
			if(IfNeedCacheCmd(pCmd))
			{
				tagData data;
				data.recvTime=Kernel::Inst()->GetAccumTimeDW();
				data.cmd=*pCmd;
				m_cache.push_back(data);
			}
			else
			{
				m_pFSM->Change2LowerState_AutoStand();
				m_pFSM->Change2UpperState_Dead(true);
			}
		}
	}

	return true;
}

void NS_RoleDeadHandler_RP::OnGameEvent( tagGameEvent* pEvent )
{
	if(pEvent->strEventName==_T("tagHitTargetEvent"))
	{
		tagHitTargetEvent* pHitEvent=(tagHitTargetEvent*)pEvent;
		if(pHitEvent->eCause==EHTC_Skill)
		{
			for(list<tagData>::iterator iter=m_cache.begin();
				iter!=m_cache.end();)
			{
				if(IfNeedClearCmd(&iter->cmd,pHitEvent))
				{
					SendCostEvent(&iter->cmd);
					SendRewardEvent(&iter->cmd);

					m_pFSM->Change2LowerState_AutoStand();
					m_pFSM->Change2UpperState_Dead(true);

					iter=m_cache.erase(iter);
				}
				else
					iter++;
			}
		}
	}
}

void NS_RoleDeadHandler_RP::Update()
{
	for(list<tagData>::iterator iter=m_cache.begin();
		iter!=m_cache.end();)
	{
		if(Kernel::Inst()->GetAccumTimeDW()-iter->recvTime>=3000)
		{
			SendCostEvent(&iter->cmd);
			SendRewardEvent(&iter->cmd);

			m_pFSM->Change2LowerState_AutoStand();
			m_pFSM->Change2UpperState_Dead(true);

			iter=m_cache.erase(iter);
		}
		else
			iter++;
	}
}


void NS_RoleDeadHandler_RP::SendRewardEvent( tagNS_RoleDead* pCmd )
{
	tagProcessRewardMsgEvent event(_T("tagProcessRewardMsgEvent"),NULL);
	event.dwRoleID = pCmd->dwSrcRoleID;

	Role* pRole=RoleMgr::Inst()->FindRole(event.dwRoleID);
	if(pRole!=NULL)
		pRole->OnGameEvent(&event);
}

void NS_RoleDeadHandler_RP::SendCostEvent( tagNS_RoleDead* pCmd )
{
	tagProcessCostMsgEvent event(_T("tagProcessCostMsgEvent"),NULL);
	event.dwRoleID = m_pFSM->GetPlayer()->GetID();
	m_pFSM->OnGameEvent(&event);
}

bool NS_RoleDeadHandler_RP::IfNeedCacheCmd( tagNS_RoleDead* pCmd )
{
	if(pCmd->eCause==ERDC_Skill)
	{
		tagHitTargetEvent* pEvent=m_pFSM->GetLastHitTargetEvent(pCmd->dwSrcRoleID);
		if(pEvent==NULL)
			return true;

		if(pCmd->dwSerial>pEvent->dwSerial)
			return true;

		if(pCmd->dwSerial==pEvent->dwSerial
			&&pCmd->dwMisc2>pEvent->dwMisc2)
			return true;
	}

	return false;
}

bool NS_RoleDeadHandler_RP::IfNeedClearCmd( tagNS_RoleDead* pCacheCmd,tagHitTargetEvent* pEvent )
{
	if(pCacheCmd->dwSrcRoleID==pEvent->dwSrcRoleID)
	{
		if(pEvent->dwSerial>pCacheCmd->dwSerial)
			return true;

		if(pEvent->dwSerial==pCacheCmd->dwSerial 
			&&pEvent->dwMisc2>=pCacheCmd->dwMisc2)
			return true;
	}

	return false;
}




//--class NS_HitFlyHandler_RP-------------------------------------------
NS_HitFlyHandler_RP::NS_HitFlyHandler_RP()
{
	m_bRecvedCmd=false;
}

NS_HitFlyHandler_RP::~NS_HitFlyHandler_RP()
{}

bool NS_HitFlyHandler_RP::OnNetCmd( tagNetCmd* pNetCmd )
{
	if(pNetCmd->dwID==m_pUtil->Crc32("NS_HitFly"))
	{
		tagNS_HitFly* pCmd=(tagNS_HitFly*)pNetCmd;
		if(pCmd->dwRoleID==m_pFSM->GetPlayer()->GetID())
		{
			m_recvTime=Kernel::Inst()->GetAccumTimeDW();
			m_cmd=*pCmd;
			m_bRecvedCmd=true;
		}
	}

	return true;
}

void NS_HitFlyHandler_RP::OnGameEvent( tagGameEvent* pEvent )
{
	if(!m_bRecvedCmd)
		return;

	if(pEvent->strEventName==_T("tagHitTargetEvent"))
	{
		tagHitTargetEvent* pHitEvent=(tagHitTargetEvent*)pEvent;
		if(pHitEvent->eCause==EHTC_Skill
			&&pHitEvent->dwSrcRoleID==m_cmd.dwSrcRoleID
			&&pHitEvent->dwMisc==m_cmd.dwSkillID
			&&pHitEvent->dwMisc2>=m_cmd.dwChannelIndex)
		{
			m_bRecvedCmd=false;

			m_pFSM->Change2UpperState_Idle();
			m_pFSM->Change2LowerState(FSM_RP::ELowerState_HitFly,&m_cmd);
		}
	}
}

void NS_HitFlyHandler_RP::Update()
{
	if(!m_bRecvedCmd)
		return;
	m_bRecvedCmd=false;

	if(Kernel::Inst()->GetAccumTimeDW()-m_recvTime>=3000)
	{
		m_pFSM->Change2UpperState_Idle();
		m_pFSM->Change2LowerState(FSM_RP::ELowerState_HitFly,&m_cmd);
	}
}










//--class NS_SpecialMove_RP-------------------------------------------
NS_SpecialMove_RP::NS_SpecialMove_RP()
{
}

NS_SpecialMove_RP::~NS_SpecialMove_RP()
{}

bool NS_SpecialMove_RP::OnNetCmd( tagNetCmd* pNetCmd )
{
	if(pNetCmd->dwID==m_pUtil->Crc32("NS_SpecialMove"))
	{
		tagNS_SpecialMove* pCmd=(tagNS_SpecialMove*)pNetCmd;
		switch( pCmd->eType )
		{
		case ESMT_Repel:	m_pFSM->Change2LowerState( FSM_RP::ELowerState_Repel, pCmd ); break;
		case ESMT_Assault:	m_pFSM->Change2LowerState( FSM_RP::ELowerState_Assault, pCmd ); break;
		case ESMT_Pull:		m_pFSM->Change2LowerState( FSM_RP::ELowerState_Pull, pCmd ); break;
		case ESMT_Teleport:  // 瞬移
			{
				m_pFSM->GetPlayer()->SetPos( pCmd->vDestPos );
			}
			break;
		default:
			break;
		}
	}
	return true;
}

void NS_SpecialMove_RP::OnGameEvent( tagGameEvent* pEvent )
{
}

void NS_SpecialMove_RP::Update()
{
}









//--class NS_AddRoleBuffHandler_RP-------------------------------------------
NS_AddRoleBuffHandler_RP::NS_AddRoleBuffHandler_RP()
{}

NS_AddRoleBuffHandler_RP::~NS_AddRoleBuffHandler_RP()
{}

bool NS_AddRoleBuffHandler_RP::OnNetCmd( tagNetCmd* pNetCmd )
{
	if(pNetCmd->dwID==m_pUtil->Crc32("NS_AddRoleBuff"))
	{
		tagNS_AddRoleBuff* pCmd=(tagNS_AddRoleBuff*)pNetCmd;

		const tagBuffProtoClient* pBuff=SkillProtoData::Inst()->FindBuffProto(pCmd->Buff.dwBuffTypeID);
		THROW_NULL(pBuff,_T("cannot find buffproto"));

		//--播放Buff特效
		if(pBuff->szEffectMod[0]!=_T('\0'))
		{
			EffectMgr::Inst()->PlayBuffEffect(m_pFSM->GetPlayer()->GetID(),
				pCmd->Buff.dwBuffTypeID,pBuff->szEffectMod);
		}
	}
	else if(pNetCmd->dwID==m_pUtil->Crc32("NS_RemoveRoleBuff"))
	{
		tagNS_RemoveRoleBuff* pCmd=(tagNS_RemoveRoleBuff*)pNetCmd;

		const tagBuffProtoClient* pBuff=SkillProtoData::Inst()->FindBuffProto(pCmd->dwBuffTypeID);
		THROW_NULL(pBuff,_T("cannot find buffproto"));

		//--停止Buff特效
		EffectMgr::Inst()->StopBuffEffect(m_pFSM->GetPlayer()->GetID(),pCmd->dwBuffTypeID);
	}

	return true;
}

void NS_AddRoleBuffHandler_RP::OnGameEvent( tagGameEvent* pEvent )
{}

void NS_AddRoleBuffHandler_RP::Update()
{}










//--class NS_SetStateHandler_RP-------------------------------------------
NS_SetStateHandler_RP::NS_SetStateHandler_RP()
{}

NS_SetStateHandler_RP::~NS_SetStateHandler_RP()
{}

bool NS_SetStateHandler_RP::OnNetCmd( tagNetCmd* pNetCmd )
{
	if(pNetCmd->dwID==m_pUtil->Crc32("NS_SetState"))
	{
		tagNS_SetState* pCmd=(tagNS_SetState*)pNetCmd;

		//--设置状态
		m_pFSM->GetPlayer()->SetState(pCmd->eState);

		//--显示效果
		switch(pCmd->eState)
		{
		case ES_Lurk:		//隐身
			m_pFSM->GetPlayer()->SetTransparent( 0.5f );
			break;
		case ES_DisArm:		//缴械
			m_pFSM->GetPlayer()->HideWeapon(true);
			break;
		}

		//--状态切换
		switch(pCmd->eState)
		{
		case ES_Dizzy:		//击晕
			if( !m_pFSM->IsSpecailMoveState() )
				m_pFSM->Change2LowerState_AutoStand();
			if( !m_pFSM->IsDeadState() )
				m_pFSM->Change2UpperState(FSM_RP::EUpperState_Dazzy,pNetCmd);
			break;
		case ES_Tie:	//定身
			if( !m_pFSM->IsSpecailMoveState() )
				m_pFSM->Change2LowerState_AutoStand();
			if( !m_pFSM->IsDeadState() )
				m_pFSM->Change2UpperState(FSM_RP::EUpperState_Tie,pNetCmd);
			break;
		case ES_Spor:		//昏睡
			if( !m_pFSM->IsSpecailMoveState() )
				m_pFSM->Change2LowerState_AutoStand();
			if( !m_pFSM->IsDeadState() )
				m_pFSM->Change2UpperState(FSM_RP::EUpperState_Sopor,pNetCmd);
			break;
		}
	}
	else if(pNetCmd->dwID==m_pUtil->Crc32("NS_UnSetState"))
	{
		tagNS_UnSetState* pCmd=(tagNS_UnSetState*)pNetCmd;

		//--取消状态
		m_pFSM->GetPlayer()->UnSetState(pCmd->eState);

		//--删除效果
		switch(pCmd->eState)
		{
		case ES_Lurk:		//隐身
			m_pFSM->GetPlayer()->SetTransparent(1.0f);
			break;
		case ES_DisArm:		//缴械
			m_pFSM->GetPlayer()->HideWeapon(false);
			break;
		}
	}

	return true;
}

void NS_SetStateHandler_RP::OnGameEvent( tagGameEvent* pEvent )
{}

void NS_SetStateHandler_RP::Update()
{}









//--class NS_HitTargetHandler_RP-------------------------------------------
NS_HitTargetHandler_RP::NS_HitTargetHandler_RP()
{}

NS_HitTargetHandler_RP::~NS_HitTargetHandler_RP()
{}

bool NS_HitTargetHandler_RP::OnNetCmd( tagNetCmd* pNetCmd )
{
	if(pNetCmd->dwID==m_pUtil->Crc32("NS_HitTarget"))
	{
		tagNS_HitTarget* pCmd=(tagNS_HitTarget*)pNetCmd;
		if(pCmd->dwRoleID==m_pFSM->GetPlayer()->GetID())
		{
			tagHitTargetEvent* pLastEvent=m_pFSM->GetLastHitTargetEvent(pCmd->dwSrcRoleID);
			if(pLastEvent==NULL)
			{
				CacheCmd(pCmd);
			}
			else
			{
				if(pCmd->dwSerial<pLastEvent->dwSerial)//小于
				{
					PlayBeAttackEffect(pCmd);
				}
				else if(pCmd->dwSerial==pLastEvent->dwSerial)//等于
				{
					PlayBeAttackEffect(pCmd);
					if(!pLastEvent->bLast)
						CacheCmd(pCmd);
				}
				else//大于
				{
					CacheCmd(pCmd);
				}
			}
		}
	}

	return true;
}

void NS_HitTargetHandler_RP::OnGameEvent( tagGameEvent* pEvent )
{
	if(pEvent->strEventName==_T("tagHitTargetEvent"))
	{
		tagHitTargetEvent* pHitEvent=(tagHitTargetEvent*)pEvent;
		for(list<tagData>::iterator iter=m_cache.begin();
			iter!=m_cache.end();)
		{
			if(IfNeedHandleCmd(&iter->cmd,pHitEvent))
				PlayBeAttackEffect(&iter->cmd);

			if(IfNeedClearCmd(&iter->cmd,pHitEvent))
				iter=m_cache.erase(iter);
			else
				++iter;
		}
	}
}

void NS_HitTargetHandler_RP::Update()
{
	for(list<tagData>::iterator iter=m_cache.begin();
		iter!=m_cache.end();)
	{
		if(Kernel::Inst()->GetAccumTimeDW()-iter->recvTime>=5000)
		{
			PlayBeAttackEffect(&iter->cmd);
			iter=m_cache.erase(iter);
		}
		else
			iter++;
	}
}

void NS_HitTargetHandler_RP::PlayBeAttackEffect( tagNS_HitTarget* pCmd )
{
	if(pCmd->eCause==EHTC_Skill)
	{
		const tagSkillProtoClient* pSkill=SkillProtoData::Inst()->FindSkillProto(pCmd->dwMisc);
		THROW_NULL(pSkill,_T("cannot find skillproto"));

		// 应策划要求：怪物技能的被攻击特效优先使用技能表，没填则使用模型表
		if(pSkill->szBatkSfx[0]!=_T('\0'))
		{
			EffectMgr::Inst()->PlayBeAttackEffect(m_pFSM->GetPlayer()->GetID(),pSkill->szBatkSfx,pSkill->nNumBatkSfx);
		}
		else if( IS_CREATURE( pCmd->dwSrcRoleID ) )
		{
			Role* pRole = RoleMgr::Inst()->FindRole( pCmd->dwSrcRoleID );
			if( P_VALID(pRole) && pRole->IsAttributeInited() && pRole->IS_KIND_OF(NPC) )
			{
				NPC* pNPC = (NPC*)pRole;
				const tagCreatureMdlData* pData=pNPC->GetMdlData();
				if( P_VALID(pData) )
					EffectMgr::Inst()->PlayBeAttackEffect(m_pFSM->GetPlayer()->GetID(),pData->szNormalAttackBeAtkSfx.c_str(),1);
			}
		}
	}
	else if(pCmd->eCause==EHTC_Item)
	{
		const tagItemDisplayInfo* pItem=ItemProtoData::Inst()->FindItemDisplay(pCmd->dwMisc);
		THROW_NULL(pItem,_T("cannot find itemdisplayinfo"));

		if(pItem->szDestEffect[0]!=_T('\0'))
		{
			EffectMgr::Inst()->PlayRoleEffect(m_pFSM->GetPlayer()->GetID(),pItem->szDestEffect);
		}
	}
}

void NS_HitTargetHandler_RP::CacheCmd( tagNS_HitTarget* pCmd )
{
	tagData data;
	data.recvTime=Kernel::Inst()->GetAccumTimeDW();
	data.cmd=*pCmd;
	m_cache.push_back(data);
}

bool NS_HitTargetHandler_RP::IfNeedClearCmd( tagNS_HitTarget* pCacheCmd,tagHitTargetEvent* pEvent )
{
	if(pCacheCmd->dwSrcRoleID==pEvent->dwSrcRoleID)
	{
		if(pEvent->dwSerial>pCacheCmd->dwSerial)
			return true;

		if(pEvent->dwSerial==pCacheCmd->dwSerial 
			&&pEvent->bLast)
			return true;
	}

	return false;
}

bool NS_HitTargetHandler_RP::IfNeedHandleCmd( tagNS_HitTarget* pCacheCmd,tagHitTargetEvent* pEvent )
{
	if(pCacheCmd->dwSrcRoleID==pEvent->dwSrcRoleID)
	{
		if(pEvent->dwSerial>=pCacheCmd->dwSerial)
			return true;
	}

	return false;
}




//--class NS_SyncGroundItemHandler_RP-------------------------------------------
NS_SyncGroundItemHandler_RP::NS_SyncGroundItemHandler_RP()
{}

NS_SyncGroundItemHandler_RP::~NS_SyncGroundItemHandler_RP()
{}

bool NS_SyncGroundItemHandler_RP::OnNetCmd( tagNetCmd* pNetCmd )
{
	if(pNetCmd->dwID==m_pUtil->Crc32("NS_SyncGroundItem"))
	{
		tagNS_SyncGroundItem* pCmd=(tagNS_SyncGroundItem*)pNetCmd;

		if(pCmd->bKilled)
		{
			tagData data;
			data.recvTime=Kernel::Inst()->GetAccumTimeDW();
			data.cmd=*pCmd;
			m_cache.push_back(data);
		}
		else
		{
			GroundItemMgr::Inst()->RoleSyncGroundItem(pCmd,0);
		}
	}

	return true;
}

void NS_SyncGroundItemHandler_RP::OnGameEvent( tagGameEvent* pEvent )
{
	if(pEvent->strEventName==_T("tagProcessCostMsgEvent"))
	{
		tagProcessCostMsgEvent* pHitEvent=(tagProcessCostMsgEvent*)pEvent;

		for(list<tagData>::iterator iter=m_cache.begin();
			iter!=m_cache.end();)
		{
			GroundItemMgr::Inst()->RoleSyncGroundItem(&iter->cmd,0);
			iter=m_cache.erase(iter);
		}
	}
}

void NS_SyncGroundItemHandler_RP::Update()
{
	for(list<tagData>::iterator iter=m_cache.begin();
		iter!=m_cache.end();)
	{
		if(Kernel::Inst()->GetAccumTimeDW()-iter->recvTime>=200)
		{
			GroundItemMgr::Inst()->RoleSyncGroundItem(&iter->cmd,0);
			iter=m_cache.erase(iter);
		}
		else
			iter++;
	}
}






//--class NS_RoleLevelChangeHandler_RP-------------------------------------------
NS_RoleLevelChangeHandler_RP::NS_RoleLevelChangeHandler_RP()
{}

NS_RoleLevelChangeHandler_RP::~NS_RoleLevelChangeHandler_RP()
{}

bool NS_RoleLevelChangeHandler_RP::OnNetCmd( tagNetCmd* pNetCmd )
{
	if(pNetCmd->dwID==m_pUtil->Crc32("NS_RoleLevelChange"))
	{
		tagNS_RoleLevelChange* pCmd=(tagNS_RoleLevelChange*)pNetCmd;
		if(pCmd->nLevel>0)
		{
			tagData data;
			data.recvTime=Kernel::Inst()->GetAccumTimeDW();
			data.cmd=*pCmd;
			m_cache.push_back(data);
		}
		else
		{
			SendShowLevelChangeEvent(pCmd);
		}
	}
	
	return true;
}

void NS_RoleLevelChangeHandler_RP::OnGameEvent( tagGameEvent* pEvent )
{
	if(pEvent->strEventName==_T("tagProcessRewardMsgEvent"))
	{
		tagProcessCostMsgEvent* pHitEvent=(tagProcessCostMsgEvent*)pEvent;

		for(list<tagData>::iterator iter=m_cache.begin();
			iter!=m_cache.end();)
		{
			SendShowLevelChangeEvent(&iter->cmd);
			iter=m_cache.erase(iter);
		}
	}
}

void NS_RoleLevelChangeHandler_RP::Update()
{
	for(list<tagData>::iterator iter=m_cache.begin();
		iter!=m_cache.end();)
	{
		if(Kernel::Inst()->GetAccumTimeDW()-iter->recvTime>=3000)
		{
			SendShowLevelChangeEvent(&iter->cmd);
			iter=m_cache.erase(iter);
		}
		else
			iter++;
	}
}

void NS_RoleLevelChangeHandler_RP::SendShowLevelChangeEvent( tagNS_RoleLevelChange* pCmd )
{
	//todo: play effect

	m_pFSM->GetPlayer()->SetRoleLevel(pCmd->nLevel,TRUE);
}













//--class NS_UseItemHandler_RP-------------------------------------------
NS_UseItemHandler_RP::NS_UseItemHandler_RP()
{}

NS_UseItemHandler_RP::~NS_UseItemHandler_RP()
{}

bool NS_UseItemHandler_RP::OnNetCmd( tagNetCmd* pNetCmd )
{
	if(pNetCmd->dwID==m_pUtil->Crc32("NS_UseItem"))
	{
		tagNS_UseItem* pCmd=(tagNS_UseItem*)pNetCmd;

		if(pCmd->bInmmediate)
		{
			if(pCmd->dwErrorCode==E_Success)
			{
				const tagItemDisplayInfo* pItemDisplay=ItemProtoData::Inst()->FindItemDisplay(pCmd->dwTypeID);
				if(!P_VALID(pItemDisplay))
					return false;

				const tagItemProto* pItemProto=ItemProtoData::Inst()->FindItemProto(pCmd->dwTypeID);
				if(!P_VALID(pItemProto))
					return false;

				//--发送命中目标事件
				tagHitTargetEvent event(_T("tagHitTargetEvent"),NULL);
				event.dwSrcRoleID=pCmd->dwSrcRoleID;
				event.dwTarRoleID=pCmd->dwTarRoleID;
				event.bSingle=pItemProto->fOPRadius==0;
				event.bLast=true;
				event.eCause=EHTC_Item;
				event.dwMisc=pCmd->dwTypeID;
				event.dwMisc2=0;
				event.dwSerial=pCmd->dwSerial;
				TObjRef<GameFrameMgr>()->SendEvent(&event);

				//--播放特效
				if(pItemDisplay->szSourceEffect[0]!=_T('\0'))
				{
					EffectMgr::Inst()->PlayRoleEffect(pCmd->dwTarRoleID,pItemDisplay->szSourceEffect);
				}
			}

			return false;
		}
	}

	return true;
}

void NS_UseItemHandler_RP::OnGameEvent( tagGameEvent* pEvent )
{}

void NS_UseItemHandler_RP::Update()
{}















//--class NS_SetRoleStateHandler_RP-------------------------------------------
NS_SetRoleStateHandler_RP::NS_SetRoleStateHandler_RP()
{}

NS_SetRoleStateHandler_RP::~NS_SetRoleStateHandler_RP()
{}

bool NS_SetRoleStateHandler_RP::OnNetCmd( tagNetCmd* pNetCmd )
{
	if(pNetCmd->dwID==m_pUtil->Crc32("NS_SetRoleState"))
	{
		tagNS_SetRoleState* pCmd=(tagNS_SetRoleState*)pNetCmd;

		if(pCmd->eState==ERS_Combat)
		{
			m_pFSM->ActiveGlobalState(FSM_RP::EGlobalState_Combat,pNetCmd,0);
		}
	}

	return true;
}

void NS_SetRoleStateHandler_RP::OnGameEvent( tagGameEvent* pEvent )
{}

void NS_SetRoleStateHandler_RP::Update()
{}












