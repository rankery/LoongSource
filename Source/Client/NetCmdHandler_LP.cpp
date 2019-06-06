#include "StdAfx.h"
#include "NetCmdHandler_LP.h"
#include "CombatEvent.h"
#include "FSM_LP.h"
#include "LocalPlayer.h"
#include "..\WorldDefine\action.h"
#include "CombatEvent.h"
#include "EffectMgr.h"
#include "SkillProtoData.h"
#include "ItemProtoData.h"
#include "GameFrameMgr.h"
#include "RoleMgr.h"
#include "GroundItemMgr.h"
#include "AudioSys.h"
#include "WorldPickerFrame.h"
#include "CombatSys.h"
#include "OnlineTips.h"
#include "WorldFrame.h"
#include "CombatSysUtil.h"
#include "CreatureData.h"
#include "NPC.h"

#include "Item.h"
#include "ItemFrame.h"
#include "ItemMgr.h"
#include "CombatSysUtil.h"
#include "RoleMgr.h"
#include "LocalPlayer.h"
#include "ItemProtoData.h"
#include "FishMgr.h"
#include "../WorldDefine/msg_fishing.h"

//--class NetCmdHander_LP-------------------------------------------
NetCmdHandler_LP::NetCmdHandler_LP(void):m_pFSM(NULL)
{}

NetCmdHandler_LP::~NetCmdHandler_LP(void)
{}






//--class NS_MoveFailedHandler_LP-------------------------------------------
NS_MoveFailedHandler_LP::NS_MoveFailedHandler_LP()
{}

NS_MoveFailedHandler_LP::~NS_MoveFailedHandler_LP()
{}

void NS_MoveFailedHandler_LP::OnNetCmd( tagNetCmd* pNetCmd )
{
	if(pNetCmd->dwID==m_pUtil->Crc32("NS_MoveFailed"))//移动失败命令
	{
		tagNS_MoveFailed* pCmd=(tagNS_MoveFailed*)pNetCmd;

		if(!m_pFSM->IsDeadState())
		{
			float yaw=CalcYaw(pCmd->faceTo);
			m_pFSM->GetLocalPlayer()->SetPos(pCmd->curPos);
			m_pFSM->GetLocalPlayer()->SetYaw(yaw);
			m_pFSM->GetLocalPlayer()->SetMoveYaw(yaw);

			m_pFSM->Change2LowerState_AutoStand();
			m_pFSM->Change2UpperState_Idle();
		}
	}
}

void NS_MoveFailedHandler_LP::OnGameEvent( tagGameEvent* pEvent )
{}

void NS_MoveFailedHandler_LP::Update()
{}




//--class NS_StopActionHandler_LP-------------------------------------------
NS_StopActionHandler_LP::NS_StopActionHandler_LP()
{}

NS_StopActionHandler_LP::~NS_StopActionHandler_LP()
{}

void NS_StopActionHandler_LP::OnNetCmd( tagNetCmd* pNetCmd )
{
	if(pNetCmd->dwID==m_pUtil->Crc32("NS_StopAction"))
	{
		tagNS_StopAction* pCmd=(tagNS_StopAction*)pNetCmd;

		float yaw=CalcYaw(pCmd->faceTo);
		m_pFSM->GetLocalPlayer()->SetPos(pCmd->curPos);
		m_pFSM->GetLocalPlayer()->SetYaw(yaw);
		m_pFSM->GetLocalPlayer()->SetMoveYaw(yaw);

		m_pFSM->Change2LowerState_AutoStand();
		m_pFSM->Change2UpperState_Idle();
	}
}

void NS_StopActionHandler_LP::OnGameEvent( tagGameEvent* pEvent )
{}

void NS_StopActionHandler_LP::Update()
{}




//--class NS_HPChangeHandler_LP-------------------------------------------
NS_HPChangeHandler_LP::NS_HPChangeHandler_LP()
{}

NS_HPChangeHandler_LP::~NS_HPChangeHandler_LP()
{}

void NS_HPChangeHandler_LP::OnNetCmd( tagNetCmd* pNetCmd )
{
	if(pNetCmd->dwID==m_pUtil->Crc32("NS_RoleHPChange"))
	{
		tagNS_RoleHPChange* pCmd=(tagNS_RoleHPChange*)pNetCmd;
		if(pCmd->dwRoleID==m_pFSM->GetLocalPlayer()->GetID())
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
}

void NS_HPChangeHandler_LP::OnGameEvent( tagGameEvent* pEvent )
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

					//播放格档音效
					PlayBlockSound(&iter->cmd);

					//发送选中目标事件
					SendSelectTargetEvent(&iter->cmd);

					//播放暴击音效
					if(iter->cmd.bCrit)
						PlayQuake();

					iter=m_cache.erase(iter);
				}
				else
					iter++;
			}
		}
	}
}

void NS_HPChangeHandler_LP::Update()
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

void NS_HPChangeHandler_LP::SendShowHPChangeEvent( tagNS_RoleHPChange* pCmd )
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

void NS_HPChangeHandler_LP::SendBeAttackEvent( tagNS_RoleHPChange* pCmd )
{
	tagBeAttackEvent event(_T("tagBeAttackEvent"),NULL);
	event.bMiss			= pCmd->bMiss;
	event.bBlock		= pCmd->bBlocked;
	m_pFSM->OnGameEvent(&event);
}

void NS_HPChangeHandler_LP::SendSelectTargetEvent(tagNS_RoleHPChange* pCmd)
{
	Role* pRole = RoleMgr::Inst()->FindRole( pCmd->dwSrcRoleID );
	if( !P_VALID( pRole ) || !pRole->IsAttributeInited() )
		return;

	CombatSys* pCombatSys = (CombatSys*)m_pGameFrameMgr->GetFrame(_T("CombatSys"));
	if( !P_VALID( pCombatSys ) )
		return;
	if( GT_INVALID != pCombatSys->GetCurTargetID() )
		return;

	tagRolePickEvent evt( _T("tagRolePickEvent"), NULL );
	evt.eType = EWPT_LClick;
	evt.dwRoleID = pCmd->dwSrcRoleID;
	m_pGameFrameMgr->SendEvent( &evt );
}

bool NS_HPChangeHandler_LP::IfNeedCacheCmd( tagNS_RoleHPChange* pCmd )
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

bool NS_HPChangeHandler_LP::IfNeedClearCmd( tagNS_RoleHPChange* pCacheCmd,tagHitTargetEvent* pEvent )
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

void NS_HPChangeHandler_LP::PlayBlockSound( tagNS_RoleHPChange* pCmd )
{
	if(pCmd->bBlocked)
	{
		const tagSkillProtoClient* pSkillProto=SkillProtoData::Inst()->FindSkillProto(pCmd->dwMisc);
		if(!P_VALID(pSkillProto))
			return;

		if(pSkillProto->eDmgType==ESDGT_Ordinary//普通伤害
			||pSkillProto->eDmgType==ESDGT_Bleeding//出血伤害
			||pSkillProto->eDmgType==ESDGT_Brunt//冲击伤害
			||pSkillProto->eDmgType==ESDGT_Bang)//重击伤害
		{
			if(m_pFSM->GetLocalPlayer()->GetRWeaponType()==EITE_Sword
				||m_pFSM->GetLocalPlayer()->GetRWeaponType()==EITE_Blade)
			{
				TObjRef<AudioSys>()->Play3DSound(_T("block"),100.0f,100.0f*50.0f,m_pFSM->GetLocalPlayer()->GetPos(),SOUND_NORMAL);
			}
		}
	}
}

void NS_HPChangeHandler_LP::PlayQuake()
{
	WorldFrame* pWorldFrm=GetWorldFrm();
	if(!P_VALID(pWorldFrm))
		return;

	pWorldFrm->GetCamera()->PlayQuake();
}

//--class NS_RoleDeadHandler_LP-------------------------------------------
NS_RoleDeadHandler_LP::NS_RoleDeadHandler_LP()
{}

NS_RoleDeadHandler_LP::~NS_RoleDeadHandler_LP()
{}

void NS_RoleDeadHandler_LP::OnNetCmd( tagNetCmd* pNetCmd )
{
	if(pNetCmd->dwID==m_pUtil->Crc32("NS_RoleDead"))
	{
		tagNS_RoleDead* pCmd=(tagNS_RoleDead*)pNetCmd;
		if(pCmd->dwRoleID==m_pFSM->GetLocalPlayer()->GetID())
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
				CombatSysUtil::Inst()->PullLocalPlayer();
				m_pFSM->Change2LowerState_AutoStand();
				m_pFSM->Change2UpperState_Dead(true,pCmd->eSuggestRevive);
			}
		}
	}
}

void NS_RoleDeadHandler_LP::OnGameEvent( tagGameEvent* pEvent )
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

					CombatSysUtil::Inst()->PullLocalPlayer();
					m_pFSM->Change2LowerState_AutoStand();
					m_pFSM->Change2UpperState_Dead(true,iter->cmd.eSuggestRevive);

					iter=m_cache.erase(iter);
				}
				else
					iter++;
			}
		}
	}
}

void NS_RoleDeadHandler_LP::Update()
{
	for(list<tagData>::iterator iter=m_cache.begin();
		iter!=m_cache.end();)
	{
		if(Kernel::Inst()->GetAccumTimeDW()-iter->recvTime>=3000)
		{
			SendCostEvent(&iter->cmd);
			SendRewardEvent(&iter->cmd);

			CombatSysUtil::Inst()->PullLocalPlayer();
			m_pFSM->Change2LowerState_AutoStand();
			m_pFSM->Change2UpperState_Dead(true,iter->cmd.eSuggestRevive);

			iter=m_cache.erase(iter);
		}
		else
			iter++;
	}
}

void NS_RoleDeadHandler_LP::SendRewardEvent( tagNS_RoleDead* pCmd )
{
	tagProcessRewardMsgEvent event(_T("tagProcessRewardMsgEvent"),NULL);
	event.dwRoleID = pCmd->dwSrcRoleID;

	Role* pRole=RoleMgr::Inst()->FindRole(event.dwRoleID);
	if(pRole!=NULL)
		pRole->OnGameEvent(&event);
}

void NS_RoleDeadHandler_LP::SendCostEvent( tagNS_RoleDead* pCmd )
{
	tagProcessCostMsgEvent event(_T("tagProcessCostMsgEvent"),NULL);
	event.dwRoleID = m_pFSM->GetLocalPlayer()->GetID();
	m_pFSM->OnGameEvent(&event);
}

bool NS_RoleDeadHandler_LP::IfNeedCacheCmd( tagNS_RoleDead* pCmd )
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

bool NS_RoleDeadHandler_LP::IfNeedClearCmd( tagNS_RoleDead* pCacheCmd,tagHitTargetEvent* pEvent )
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







//--class NS_HitFlyHandler_LP-------------------------------------------
NS_HitFlyHandler_LP::NS_HitFlyHandler_LP()
{
	m_bRecvedCmd=false;
}

NS_HitFlyHandler_LP::~NS_HitFlyHandler_LP()
{}

void NS_HitFlyHandler_LP::OnNetCmd( tagNetCmd* pNetCmd )
{
	if(pNetCmd->dwID==m_pUtil->Crc32("NS_HitFly"))
	{
		tagNS_HitFly* pCmd=(tagNS_HitFly*)pNetCmd;
		if(pCmd->dwRoleID==m_pFSM->GetLocalPlayer()->GetID())
		{
			m_recvTime=Kernel::Inst()->GetAccumTimeDW();
			m_cmd=*pCmd;
			m_bRecvedCmd=true;
		}
	}
}

void NS_HitFlyHandler_LP::OnGameEvent( tagGameEvent* pEvent )
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
			m_pFSM->Change2LowerState(FSM_LP::ELowerState_HitFly,&m_cmd);
			m_pFSM->Change2UpperState_Idle();
		}
	}
}

void NS_HitFlyHandler_LP::Update()
{
	if(!m_bRecvedCmd)
		return;
	m_bRecvedCmd=false;

	if(Kernel::Inst()->GetAccumTimeDW()-m_recvTime>=3000)
	{
		m_pFSM->Change2LowerState(FSM_LP::ELowerState_HitFly,&m_cmd);
		m_pFSM->Change2UpperState_Idle();
	}
}






//--class NS_SpecialMove_LP-------------------------------------------
NS_SpecialMove_LP::NS_SpecialMove_LP()
{
}

NS_SpecialMove_LP::~NS_SpecialMove_LP()
{}

void NS_SpecialMove_LP::OnNetCmd( tagNetCmd* pNetCmd )
{
	if(pNetCmd->dwID==m_pUtil->Crc32("NS_SpecialMove"))
	{
		tagNS_SpecialMove* pCmd=(tagNS_SpecialMove*)pNetCmd;
		switch( pCmd->eType )
		{
		case ESMT_Repel:	m_pFSM->Change2LowerState( FSM_LP::ELowerState_Repel, pCmd ); break;
		case ESMT_Assault:	m_pFSM->Change2LowerState( FSM_LP::ELowerState_Assault, pCmd ); break;
		case ESMT_Pull:		m_pFSM->Change2LowerState( FSM_LP::ELowerState_Pull, pCmd ); break;
		case ESMT_Teleport:  // 瞬移
			{
				m_pFSM->GetLocalPlayer()->SetPos( pCmd->vDestPos );
			}
			break;
		default:
			break;
		}
	}
}

void NS_SpecialMove_LP::OnGameEvent( tagGameEvent* pEvent )
{
}

void NS_SpecialMove_LP::Update()
{
}







//--class NS_AddRoleBuffHandler_LP-------------------------------------------
NS_AddRoleBuffHandler_LP::NS_AddRoleBuffHandler_LP()
{}

NS_AddRoleBuffHandler_LP::~NS_AddRoleBuffHandler_LP()
{}

void NS_AddRoleBuffHandler_LP::OnNetCmd( tagNetCmd* pNetCmd )
{
	if(pNetCmd->dwID==m_pUtil->Crc32("NS_AddRoleBuff"))
	{
		tagNS_AddRoleBuff* pCmd=(tagNS_AddRoleBuff*)pNetCmd;

		const tagBuffProtoClient* pBuff=SkillProtoData::Inst()->FindBuffProto(pCmd->Buff.dwBuffTypeID);
		THROW_NULL(pBuff,_T("cannot find buffproto"));

		//--播放Buff特效
		if(pBuff->szEffectMod[0]!=_T('\0'))
		{
			EffectMgr::Inst()->PlayBuffEffect(m_pFSM->GetLocalPlayer()->GetID(),
				pCmd->Buff.dwBuffTypeID,pBuff->szEffectMod);
		}
	}
	else if(pNetCmd->dwID==m_pUtil->Crc32("NS_RemoveRoleBuff"))
	{
		tagNS_RemoveRoleBuff* pCmd=(tagNS_RemoveRoleBuff*)pNetCmd;

		const tagBuffProtoClient* pBuff=SkillProtoData::Inst()->FindBuffProto(pCmd->dwBuffTypeID);
		THROW_NULL(pBuff,_T("cannot find buffproto"));

		//--停止Buff特效
		EffectMgr::Inst()->StopBuffEffect(m_pFSM->GetLocalPlayer()->GetID(),pCmd->dwBuffTypeID);
	}
}

void NS_AddRoleBuffHandler_LP::OnGameEvent( tagGameEvent* pEvent )
{}

void NS_AddRoleBuffHandler_LP::Update()
{}











//--class NS_SetStateHandler_LP-------------------------------------------
NS_SetStateHandler_LP::NS_SetStateHandler_LP()
{}

NS_SetStateHandler_LP::~NS_SetStateHandler_LP()
{}

void NS_SetStateHandler_LP::OnNetCmd( tagNetCmd* pNetCmd )
{
	if(pNetCmd->dwID==m_pUtil->Crc32("NS_SetState"))
	{
		tagNS_SetState* pCmd=(tagNS_SetState*)pNetCmd;

		//--设置状态
		m_pFSM->GetLocalPlayer()->SetState(pCmd->eState);

		//--显示效果
		switch(pCmd->eState)
		{
		case ES_Lurk:	//隐身
			m_pFSM->GetLocalPlayer()->SetTransparent(0.5f);
			break;
		case ES_DisArm:	//缴械
			m_pFSM->GetLocalPlayer()->HideWeapon(true);
			break;
		}

		//--状态切换
		switch(pCmd->eState)
		{
		case ES_Dizzy:	//击晕
			if( !m_pFSM->IsSpecailMoveState() )
			{
				CombatSysUtil::Inst()->PullLocalPlayer();
				m_pFSM->Change2LowerState_AutoStand();
			}
			if( !m_pFSM->IsDeadState() )
				m_pFSM->Change2UpperState(FSM_LP::EUpperState_Dazzy,pNetCmd);
			break;
		case ES_Tie:	//定身
			if( !m_pFSM->IsSpecailMoveState() )
			{
				CombatSysUtil::Inst()->PullLocalPlayer();
				m_pFSM->Change2LowerState_AutoStand();
			}
			if( !m_pFSM->IsDeadState() )
				m_pFSM->Change2UpperState(FSM_LP::EUpperState_Tie,pNetCmd);
			break;
		case ES_Spor:	//昏睡
			if( !m_pFSM->IsSpecailMoveState() )
			{
				CombatSysUtil::Inst()->PullLocalPlayer();
				m_pFSM->Change2LowerState_AutoStand();
			}
			if( !m_pFSM->IsDeadState() )
				m_pFSM->Change2UpperState(FSM_LP::EUpperState_Sopor,pNetCmd);
			break;
		}
	}
	else if(pNetCmd->dwID==m_pUtil->Crc32("NS_UnSetState"))
	{
		tagNS_UnSetState* pCmd=(tagNS_UnSetState*)pNetCmd;

		//--取消状态
		m_pFSM->GetLocalPlayer()->UnSetState(pCmd->eState);

		//--删除效果
		switch(pCmd->eState)
		{
		case ES_Lurk:	//隐身
			m_pFSM->GetLocalPlayer()->SetTransparent(1.0f);
			break;
		case ES_DisArm:	//缴械
			m_pFSM->GetLocalPlayer()->HideWeapon(false);
			break;
		}
	}
}

void NS_SetStateHandler_LP::OnGameEvent( tagGameEvent* pEvent )
{}

void NS_SetStateHandler_LP::Update()
{}











//--class NS_HitTargetHandler_LP-------------------------------------------
NS_HitTargetHandler_LP::NS_HitTargetHandler_LP()
{}

NS_HitTargetHandler_LP::~NS_HitTargetHandler_LP()
{}

void NS_HitTargetHandler_LP::OnNetCmd( tagNetCmd* pNetCmd )
{
	if(pNetCmd->dwID==m_pUtil->Crc32("NS_HitTarget"))
	{
		tagNS_HitTarget* pCmd=(tagNS_HitTarget*)pNetCmd;
		if(pCmd->dwRoleID==m_pFSM->GetLocalPlayer()->GetID())
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
}

void NS_HitTargetHandler_LP::OnGameEvent( tagGameEvent* pEvent )
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

void NS_HitTargetHandler_LP::Update()
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

void NS_HitTargetHandler_LP::PlayBeAttackEffect( tagNS_HitTarget* pCmd )
{
	if(pCmd->eCause==EHTC_Skill)
	{
		const tagSkillProtoClient* pSkill=SkillProtoData::Inst()->FindSkillProto(pCmd->dwMisc);
		THROW_NULL(pSkill,_T("cannot find skillproto"));

		// 应策划要求：怪物技能的被攻击特效优先使用技能表，没填则使用模型表
		if(pSkill->szBatkSfx[0]!=_T('\0'))
		{
			EffectMgr::Inst()->PlayBeAttackEffect(m_pFSM->GetLocalPlayer()->GetID(),pSkill->szBatkSfx,pSkill->nNumBatkSfx);
		}
		else if( IS_CREATURE( pCmd->dwSrcRoleID ) )
		{
			Role* pRole = RoleMgr::Inst()->FindRole( pCmd->dwSrcRoleID );
			if( P_VALID(pRole) && pRole->IsAttributeInited() && pRole->IS_KIND_OF(NPC) )
			{
				NPC* pNPC = (NPC*)pRole;
				const tagCreatureMdlData* pData=pNPC->GetMdlData();
				if( P_VALID(pData) )
					EffectMgr::Inst()->PlayBeAttackEffect(m_pFSM->GetLocalPlayer()->GetID(),pData->szNormalAttackBeAtkSfx.c_str(),1);
			}
		}
	}
	else if(pCmd->eCause==EHTC_Item)
	{
		const tagItemDisplayInfo* pItem=ItemProtoData::Inst()->FindItemDisplay(pCmd->dwMisc);
		THROW_NULL(pItem,_T("cannot find itemdisplayinfo"));

		if(pItem->szDestEffect[0]!=_T('\0'))
		{
			EffectMgr::Inst()->PlayRoleEffect(m_pFSM->GetLocalPlayer()->GetID(),pItem->szDestEffect);
		}
	}
}

void NS_HitTargetHandler_LP::CacheCmd( tagNS_HitTarget* pCmd )
{
	tagData data;
	data.recvTime=Kernel::Inst()->GetAccumTimeDW();
	data.cmd=*pCmd;
	m_cache.push_back(data);
}

bool NS_HitTargetHandler_LP::IfNeedClearCmd( tagNS_HitTarget* pCacheCmd,tagHitTargetEvent* pEvent )
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

bool NS_HitTargetHandler_LP::IfNeedHandleCmd( tagNS_HitTarget* pCacheCmd,tagHitTargetEvent* pEvent )
{
	if(pCacheCmd->dwSrcRoleID==pEvent->dwSrcRoleID)
	{
		if(pEvent->dwSerial>=pCacheCmd->dwSerial)
			return true;
	}

	return false;
}


//--class NS_SyncGroundItemHandler_LP-------------------------------------------
NS_SyncGroundItemHandler_LP::NS_SyncGroundItemHandler_LP()
{}

NS_SyncGroundItemHandler_LP::~NS_SyncGroundItemHandler_LP()
{}

void NS_SyncGroundItemHandler_LP::OnNetCmd( tagNetCmd* pNetCmd )
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
}

void NS_SyncGroundItemHandler_LP::OnGameEvent( tagGameEvent* pEvent )
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

void NS_SyncGroundItemHandler_LP::Update()
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







//--class NS_RoleExpChangeHandler_LP-------------------------------------------
NS_RoleExpChangeHandler_LP::NS_RoleExpChangeHandler_LP()
{}

NS_RoleExpChangeHandler_LP::~NS_RoleExpChangeHandler_LP()
{}

void NS_RoleExpChangeHandler_LP::OnNetCmd( tagNetCmd* pNetCmd )
{
	if(pNetCmd->dwID==m_pUtil->Crc32("NS_RoleExpChange"))
	{
		tagNS_RoleExpChange* pCmd=(tagNS_RoleExpChange*)pNetCmd;
		if(pCmd->nChange>0
			&&pCmd->bKill)
		{
			tagData data;
			data.recvTime=Kernel::Inst()->GetAccumTimeDW();
			data.cmd=*pCmd;
			m_cache.push_back(data);
		}
		else
		{
			SendShowExpChangeEvent(pCmd);
		}
	}
}

void NS_RoleExpChangeHandler_LP::OnGameEvent( tagGameEvent* pEvent )
{
	if(pEvent->strEventName==_T("tagProcessRewardMsgEvent"))
	{
		tagProcessCostMsgEvent* pHitEvent=(tagProcessCostMsgEvent*)pEvent;

		for(list<tagData>::iterator iter=m_cache.begin();
			iter!=m_cache.end();)
		{
			SendShowExpChangeEvent(&iter->cmd);
			iter=m_cache.erase(iter);
		}
	}
}

void NS_RoleExpChangeHandler_LP::Update()
{
	for(list<tagData>::iterator iter=m_cache.begin();
		iter!=m_cache.end();)
	{
		if(Kernel::Inst()->GetAccumTimeDW()-iter->recvTime>=3000)
		{
			SendShowExpChangeEvent(&iter->cmd);
			iter=m_cache.erase(iter);
		}
		else
			iter++;
	}
}

void NS_RoleExpChangeHandler_LP::SendShowExpChangeEvent( tagNS_RoleExpChange* pCmd )
{
	m_pFSM->GetLocalPlayer()->SetRoleLevelExp(pCmd->nExp, pCmd->nChange);
}







//--class NS_RoleLevelChangeHandler_LP-------------------------------------------
NS_RoleLevelChangeHandler_LP::NS_RoleLevelChangeHandler_LP()
{}

NS_RoleLevelChangeHandler_LP::~NS_RoleLevelChangeHandler_LP()
{}

void NS_RoleLevelChangeHandler_LP::OnNetCmd( tagNetCmd* pNetCmd )
{
	if(pNetCmd->dwID==m_pUtil->Crc32("NS_RoleLevelChange"))
	{
		tagNS_RoleLevelChange* pCmd=(tagNS_RoleLevelChange*)pNetCmd;
		if(pCmd->bKill)
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
}

void NS_RoleLevelChangeHandler_LP::OnGameEvent( tagGameEvent* pEvent )
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

void NS_RoleLevelChangeHandler_LP::Update()
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

void NS_RoleLevelChangeHandler_LP::SendShowLevelChangeEvent( tagNS_RoleLevelChange* pCmd )
{
	//todo: play effect

	m_pFSM->GetLocalPlayer()->SetRoleLevel(pCmd->nLevel,TRUE);
	// 策划案要求，等级变化弹出onlinetips
	((OnlineTips*)(TObjRef<GameFrameMgr>()->GetFrame(_T("OnlineTips"))))->ShowWnd(TRUE);

}


//-----------class NS_CompleteFishHandler_LP ------------------------------------------------
NS_CompleteFishHandler_LP::NS_CompleteFishHandler_LP()
{
	m_pItemFrame = (ItemFrame*)( m_pGameFrameMgr->GetFrame( _T("Item") ) );
}

NS_CompleteFishHandler_LP::~NS_CompleteFishHandler_LP()
{

}

void NS_CompleteFishHandler_LP::OnNetCmd( tagNetCmd* pNetCmd )
{
	if(pNetCmd->dwID != m_pUtil->Crc32("NS_Fishing"))
		return;

	tagNS_Fishing* pCmd = (tagNS_Fishing*)pNetCmd;

	//得到指定类型的鱼饵
	Item* pItem = ItemMgr::Inst()->GetFirstSpecFuncItemFromPocket( EISF_Fishing, FishMgr::Inst()->GetFishFoodType(), TRUE );
	if( !P_VALID(pItem) || pItem->GetItemSpecFunc() != EISF_Fishing )
	{
		//随便得到一个鱼饵
		pItem = ItemMgr::Inst()->GetFirstSpecFuncItemFromPocket( EISF_Fishing, FishMgr::Inst()->GetFishFoodType() );
	}

	if( !P_VALID(pItem) || pItem->GetItemSpecFunc() != EISF_Fishing )
	{
		CombatSysUtil::Inst()->ShowScreenCenterMsg( g_StrTable[_T("Fish_NoBait")] );

		//TODO 没有鱼饵了
		return;
	}

	if( !P_VALID(m_pItemFrame) )
	{
		m_pItemFrame = (ItemFrame*)( m_pGameFrameMgr->GetFrame( _T("Item") ) );
		if( !P_VALID( m_pItemFrame ) )
			return;
	}

	DWORD dwItemTypeID = m_pFSM->GetUsingItemTypeID();
	const tagItemProto* pItemProto = ItemProtoData::Inst()->FindItemProto( dwItemTypeID );
	if( P_VALID(pItemProto) && pItemProto->eSpecFunc == EISF_Fishing )
	{
		if( P_VALID(m_pFSM) )
			m_pFSM->Change2UpperState_Idle();
	}

	FishMgr::Inst()->StartFish( pItem );
}

void NS_CompleteFishHandler_LP::OnGameEvent( tagGameEvent* pEvent )
{
}

void NS_CompleteFishHandler_LP::Update()
{
}