#include "StdAfx.h"
#include "NetCmdHandler_NPC.h"
#include "CombatEvent.h"
#include "FSM_NPC.h"
#include "NPC.h"
#include "..\WorldDefine\action.h"
#include "CombatEvent.h"
#include "SkillProtoData.h"
#include "ItemProtoData.h"
#include "EffectMgr.h"
#include "GroundItemMgr.h"
#include "RoleMgr.h"
#include "CombatSysUtil.h"
#include "AudioSys.h"
#include "LocalPlayer.h"
#include "WorldFrame.h"
#include "ChatShowPop.h"
#include "MonsterSayTable.h"
#include "CreatureData.h"

//--class NetCmdHander_NPC-------------------------------------------
NetCmdHandler_NPC::NetCmdHandler_NPC(void):m_pFSM(NULL)
{}

NetCmdHandler_NPC::~NetCmdHandler_NPC(void)
{}





//--class NS_SyncStandHandler_NPC-------------------------------------------
NS_SyncStandHandler_NPC::NS_SyncStandHandler_NPC()
{}

NS_SyncStandHandler_NPC::~NS_SyncStandHandler_NPC()
{}

void NS_SyncStandHandler_NPC::OnNetCmd( tagNetCmd* pNetCmd )
{
	if(pNetCmd->dwID==m_pUtil->Crc32("NS_SyncStand"))
	{
		tagNS_SyncStand* pCmd=(tagNS_SyncStand*)pNetCmd;

		m_pFSM->GetNPC()->SetPos(pCmd->curPos);
		m_pFSM->GetNPC()->SetYaw(CalcYaw(pCmd->faceTo));
		m_pFSM->Change2IdleState();
	}
}

void NS_SyncStandHandler_NPC::OnGameEvent( tagGameEvent* pEvent )
{}

void NS_SyncStandHandler_NPC::Update()
{}





//--class NS_StopActionHandler_NPC-------------------------------------------
NS_StopActionHandler_NPC::NS_StopActionHandler_NPC()
{}

NS_StopActionHandler_NPC::~NS_StopActionHandler_NPC()
{}

void NS_StopActionHandler_NPC::OnNetCmd( tagNetCmd* pNetCmd )
{
	if(pNetCmd->dwID==m_pUtil->Crc32("NS_StopAction"))
	{
		tagNS_StopAction* pCmd=(tagNS_StopAction*)pNetCmd;

		m_pFSM->GetNPC()->SetPos(pCmd->curPos);
		m_pFSM->GetNPC()->SetYaw(CalcYaw(pCmd->faceTo));
		m_pFSM->Change2IdleState();
	}
}

void NS_StopActionHandler_NPC::OnGameEvent( tagGameEvent* pEvent )
{}

void NS_StopActionHandler_NPC::Update()
{}









//--class NS_HPChangeHandler_NPC-------------------------------------------
NS_HPChangeHandler_NPC::NS_HPChangeHandler_NPC()
{}

NS_HPChangeHandler_NPC::~NS_HPChangeHandler_NPC()
{}

void NS_HPChangeHandler_NPC::OnNetCmd( tagNetCmd* pNetCmd )
{
	if(pNetCmd->dwID==m_pUtil->Crc32("NS_RoleHPChange"))
	{
		tagNS_RoleHPChange* pCmd=(tagNS_RoleHPChange*)pNetCmd;
		if(pCmd->dwRoleID==m_pFSM->GetNPC()->GetID())
		{
			if(IfNeedCacheCmd(pCmd))
			{
				tagData data;
				data.recvTime=Kernel::Inst()->GetAccumTimeDW();
				data.cmd=*pCmd;
				m_cache.push_back(data);

				//IMSG(_T("[%d]recv NS_RoleHPChange,skill=%d,channel=%d,serial=%d\n"),timeGetTime(),
				//	pCmd->dwMisc,pCmd->dwMisc2,pCmd->dwSerial);
			}
			else
			{
				SendShowHPChangeEvent(pCmd);
			}
		}
	}
}

void NS_HPChangeHandler_NPC::OnGameEvent( tagGameEvent* pEvent )
{
	if(pEvent->strEventName==_T("tagHitTargetEvent"))
	{
		tagHitTargetEvent* pHitEvent=(tagHitTargetEvent*)pEvent;

		//IMSG(_T("[%d]recv hittarget,skill=%d,channel=%d,serial=%d\n"),timeGetTime(),
		//	pHitEvent->dwMisc,pHitEvent->dwMisc2,pHitEvent->dwSerial);

		if(pHitEvent->eCause==EHTC_Skill)
		{
			for(list<tagData>::iterator iter=m_cache.begin();
				iter!=m_cache.end();)
			{
				if(IfNeedClearCmd(&iter->cmd,pHitEvent))
				{
					//发送显示减血事件
					SendShowHPChangeEvent(&iter->cmd);

					//IMSG(_T("[%d]SendShowHPChange,skill=%d,channel=%d\n"),timeGetTime(),iter->cmd.dwMisc,iter->cmd.dwMisc2);

					//发送被攻击事件 
					if(!iter->cmd.bMiss
						&&!iter->cmd.bBlocked)
						SendBeAttackEvent();

					//播放爆击音效
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

void NS_HPChangeHandler_NPC::Update()
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

void NS_HPChangeHandler_NPC::SendShowHPChangeEvent( tagNS_RoleHPChange* pCmd )
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

void NS_HPChangeHandler_NPC::SendBeAttackEvent()
{
	tagBeAttackEvent event(_T("tagBeAttackEvent"),NULL);
	m_pFSM->OnGameEvent(&event);
}

bool NS_HPChangeHandler_NPC::IfNeedCacheCmd( tagNS_RoleHPChange* pCmd )
{
	if(pCmd->eCause==ERHPCC_SkillDamage)
	{
		tagHitTargetEvent* pEvent=m_pFSM->GetLastHitTargetEvent(pCmd->dwSrcRoleID);
		if(pEvent==NULL)
			return true;

		//IMSG(_T("[%d]recv NS_RoleHPChange,skill=%d,srcrole=%d,channel=%d,serial=%d,event serial=%d,event channel=%d\n"),timeGetTime(),
		//	pCmd->dwMisc,pCmd->dwSrcRoleID,pCmd->dwMisc2,pCmd->dwSerial,
		//	pEvent->dwSerial,pEvent->dwMisc2);

		if(pCmd->dwSerial>pEvent->dwSerial)
			return true;

		if(pCmd->dwSerial==pEvent->dwSerial
			&&pCmd->dwMisc2>pEvent->dwMisc2)
			return true;
	}

	return false;
}

bool NS_HPChangeHandler_NPC::IfNeedClearCmd( tagNS_RoleHPChange* pCacheCmd,tagHitTargetEvent* pEvent )
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

void NS_HPChangeHandler_NPC::PlayCritSound( tagNS_RoleHPChange* pCmd )
{
	if(!pCmd->bCrit)
		return;

	if(pCmd->dwSrcRoleID!=RoleMgr::Inst()->GetLocalPlayerID())
		return;

	TObjRef<AudioSys>()->Play3DSound(_T("crit"),100.0f,100.0f*50.0f,RoleMgr::Inst()->GetLocalPlayer()->GetPos(),SOUND_NORMAL);
}

void NS_HPChangeHandler_NPC::PlayQuake()
{
	WorldFrame* pWorldFrm=GetWorldFrm();
	if(!P_VALID(pWorldFrm))
		return;

	pWorldFrm->GetCamera()->PlayQuake();
}

//--class NS_RoleDeadHandler_NPC-------------------------------------------
NS_RoleDeadHandler_NPC::NS_RoleDeadHandler_NPC()
{}

NS_RoleDeadHandler_NPC::~NS_RoleDeadHandler_NPC()
{}

void NS_RoleDeadHandler_NPC::OnNetCmd( tagNetCmd* pNetCmd )
{
	if(pNetCmd->dwID==m_pUtil->Crc32("NS_RoleDead"))
	{
		tagNS_RoleDead* pCmd=(tagNS_RoleDead*)pNetCmd;
		if(pCmd->dwRoleID==m_pFSM->GetNPC()->GetID())
		{
            // 保存死亡
            m_pFSM->SetNSDead(true);
			if(IfNeedCacheCmd(pCmd))
			{
				tagData data;
				data.recvTime=Kernel::Inst()->GetAccumTimeDW();
				data.cmd=*pCmd;
				m_cache.push_back(data);
			}
			else
			{
				m_pFSM->Change2DeadState();
			}
		}
	}
}

void NS_RoleDeadHandler_NPC::OnGameEvent( tagGameEvent* pEvent )
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

					m_pFSM->Change2DeadState();
					iter=m_cache.erase(iter);
				}
				else
					iter++;
			}
		}
	}
}

void NS_RoleDeadHandler_NPC::Update()
{
	for(list<tagData>::iterator iter=m_cache.begin();
		iter!=m_cache.end();)
	{
		if(Kernel::Inst()->GetAccumTimeDW()-iter->recvTime>=3000)
		{
			SendCostEvent(&iter->cmd);
			SendRewardEvent(&iter->cmd);

			m_pFSM->Change2DeadState();
			iter=m_cache.erase(iter);
		}
		else
			iter++;
	}
}

void NS_RoleDeadHandler_NPC::SendRewardEvent( tagNS_RoleDead* pCmd )
{
	tagProcessRewardMsgEvent event(_T("tagProcessRewardMsgEvent"),NULL);
	event.dwRoleID = pCmd->dwSrcRoleID;

	Role* pRole=RoleMgr::Inst()->FindRole(event.dwRoleID);
	if(pRole!=NULL)
		pRole->OnGameEvent(&event);
}

void NS_RoleDeadHandler_NPC::SendCostEvent( tagNS_RoleDead* pCmd )
{
	tagProcessCostMsgEvent event(_T("tagProcessCostMsgEvent"),NULL);
	event.dwRoleID = m_pFSM->GetNPC()->GetID();
	m_pFSM->OnGameEvent(&event);
}

bool NS_RoleDeadHandler_NPC::IfNeedCacheCmd( tagNS_RoleDead* pCmd )
{
    //if (m_pFSM->IsHitFlyState())
    //    return true;

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

bool NS_RoleDeadHandler_NPC::IfNeedClearCmd( tagNS_RoleDead* pCacheCmd,tagHitTargetEvent* pEvent )
{
    //if (m_pFSM->IsHitFlyState())
    //    return false;

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








//--class NS_HitFlyHandler_NPC-------------------------------------------
NS_HitFlyHandler_NPC::NS_HitFlyHandler_NPC()
{
	m_bRecvedCmd=false;
}

NS_HitFlyHandler_NPC::~NS_HitFlyHandler_NPC()
{}

void NS_HitFlyHandler_NPC::OnNetCmd( tagNetCmd* pNetCmd )
{
	if(pNetCmd->dwID==m_pUtil->Crc32("NS_HitFly"))
	{
		tagNS_HitFly* pCmd=(tagNS_HitFly*)pNetCmd;
		if(pCmd->dwRoleID==m_pFSM->GetNPC()->GetID())
		{
			m_recvTime=Kernel::Inst()->GetAccumTimeDW();
			m_cmd=*pCmd;
			m_bRecvedCmd=true;
		}
	}
}

void NS_HitFlyHandler_NPC::OnGameEvent( tagGameEvent* pEvent )
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
			m_pFSM->Change2State(FSM_NPC::EState_HitFly,&m_cmd);
		}
	}
}

void NS_HitFlyHandler_NPC::Update()
{
	if(!m_bRecvedCmd)
		return;

	if(Kernel::Inst()->GetAccumTimeDW()-m_recvTime>=0)
	{
		m_pFSM->Change2State(FSM_NPC::EState_HitFly,&m_cmd);
        m_bRecvedCmd=false;
	}
}











//--class NS_SpecialMove_NPC-------------------------------------------
NS_SpecialMove_NPC::NS_SpecialMove_NPC()
{
}

NS_SpecialMove_NPC::~NS_SpecialMove_NPC()
{}

void NS_SpecialMove_NPC::OnNetCmd( tagNetCmd* pNetCmd )
{
	if(pNetCmd->dwID==m_pUtil->Crc32("NS_SpecialMove"))
	{
		tagNS_SpecialMove* pCmd=(tagNS_SpecialMove*)pNetCmd;
		switch( pCmd->eType )
		{
		case ESMT_Repel:	m_pFSM->Change2State( FSM_NPC::EState_Repel, pCmd ); break;
		case ESMT_Assault:	m_pFSM->Change2State( FSM_NPC::EState_Assault, pCmd ); break;
		case ESMT_Pull:		m_pFSM->Change2State( FSM_NPC::EState_Pull, pCmd ); break;
		case ESMT_Teleport:  // 瞬移
			{
				m_pFSM->GetNPC()->SetPos( pCmd->vDestPos );
			}
			break;
		default:
			break;
		}
	}
}

void NS_SpecialMove_NPC::OnGameEvent( tagGameEvent* pEvent )
{
}

void NS_SpecialMove_NPC::Update()
{
}












//--class NS_AddRoleBuffHandler_NPC-------------------------------------------
NS_AddRoleBuffHandler_NPC::NS_AddRoleBuffHandler_NPC()
{}

NS_AddRoleBuffHandler_NPC::~NS_AddRoleBuffHandler_NPC()
{}

void NS_AddRoleBuffHandler_NPC::OnNetCmd( tagNetCmd* pNetCmd )
{
	if(pNetCmd->dwID==m_pUtil->Crc32("NS_AddRoleBuff"))
	{
		tagNS_AddRoleBuff* pCmd=(tagNS_AddRoleBuff*)pNetCmd;

		const tagBuffProtoClient* pBuff=SkillProtoData::Inst()->FindBuffProto(pCmd->Buff.dwBuffTypeID);
		THROW_NULL(pBuff,_T("cannot find buffproto"));

		//--播放Buff特效
		if(pBuff->szEffectMod[0]!=_T('\0'))
		{
			EffectMgr::Inst()->PlayBuffEffect(m_pFSM->GetNPC()->GetID(),
				pCmd->Buff.dwBuffTypeID,pBuff->szEffectMod);
		}
	}
	else if(pNetCmd->dwID==m_pUtil->Crc32("NS_RemoveRoleBuff"))
	{
		tagNS_RemoveRoleBuff* pCmd=(tagNS_RemoveRoleBuff*)pNetCmd;

		const tagBuffProtoClient* pBuff=SkillProtoData::Inst()->FindBuffProto(pCmd->dwBuffTypeID);
		THROW_NULL(pBuff,_T("cannot find buffproto"));

		//--停止Buff特效
		EffectMgr::Inst()->StopBuffEffect(m_pFSM->GetNPC()->GetID(),pCmd->dwBuffTypeID);
	}
}

void NS_AddRoleBuffHandler_NPC::OnGameEvent( tagGameEvent* pEvent )
{}

void NS_AddRoleBuffHandler_NPC::Update()
{}










//--class NS_SetStateHandler_NPC-------------------------------------------
NS_SetStateHandler_NPC::NS_SetStateHandler_NPC()
{}

NS_SetStateHandler_NPC::~NS_SetStateHandler_NPC()
{}

void NS_SetStateHandler_NPC::OnNetCmd( tagNetCmd* pNetCmd )
{
	if(pNetCmd->dwID==m_pUtil->Crc32("NS_SetState"))
	{
		tagNS_SetState* pCmd=(tagNS_SetState*)pNetCmd;

		//--设置状态
		m_pFSM->GetNPC()->SetState(pCmd->eState);

		//--显示效果
		switch(pCmd->eState)
		{
		case ES_Lurk:		//隐身
			m_pFSM->GetNPC()->SetTransparent(0.5f);
			break;
		}

		//--状态切换
		switch(pCmd->eState)
		{
		case ES_Dizzy:		//击晕
			if( !m_pFSM->IsSpecailMoveState() )
				m_pFSM->Change2State(FSM_NPC::EState_Dazzy,pNetCmd);
			break;
		case ES_Tie:		//定身
			if( !m_pFSM->IsSpecailMoveState() )
				m_pFSM->Change2State(FSM_NPC::EState_Tie,pNetCmd);
			break;
		case ES_Spor:		//昏睡
			if( !m_pFSM->IsSpecailMoveState() )
				m_pFSM->Change2State(FSM_NPC::EState_Sopor,pNetCmd);
			break;
		}
	}
	else if(pNetCmd->dwID==m_pUtil->Crc32("NS_UnSetState"))
	{
		tagNS_UnSetState* pCmd=(tagNS_UnSetState*)pNetCmd;

		//--取消状态
		m_pFSM->GetNPC()->UnSetState(pCmd->eState);

		//--删除效果
		switch(pCmd->eState)
		{
		case ES_Lurk:		//隐身
			m_pFSM->GetNPC()->SetTransparent(1.0f);
			break;
		}
	}
}

void NS_SetStateHandler_NPC::OnGameEvent( tagGameEvent* pEvent )
{}

void NS_SetStateHandler_NPC::Update()
{}





//--class NS_HitTargetHandler_NPC-------------------------------------------
NS_HitTargetHandler_NPC::NS_HitTargetHandler_NPC()
{}

NS_HitTargetHandler_NPC::~NS_HitTargetHandler_NPC()
{}

void NS_HitTargetHandler_NPC::OnNetCmd( tagNetCmd* pNetCmd )
{
	if(pNetCmd->dwID==m_pUtil->Crc32("NS_HitTarget"))
	{
		tagNS_HitTarget* pCmd=(tagNS_HitTarget*)pNetCmd;
		if(pCmd->dwRoleID==m_pFSM->GetNPC()->GetID())
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

void NS_HitTargetHandler_NPC::OnGameEvent( tagGameEvent* pEvent )
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

void NS_HitTargetHandler_NPC::Update()
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

void NS_HitTargetHandler_NPC::PlayBeAttackEffect( tagNS_HitTarget* pCmd )
{
	if(pCmd->eCause==EHTC_Skill)
	{
		const tagSkillProtoClient* pSkill=SkillProtoData::Inst()->FindSkillProto(pCmd->dwMisc);
		THROW_NULL(pSkill,_T("cannot find skillproto"));

		// 应策划要求：怪物技能的被攻击特效优先使用技能表，没填则使用模型表
		if(pSkill->szBatkSfx[0]!=_T('\0'))
		{
			EffectMgr::Inst()->PlayBeAttackEffect(m_pFSM->GetNPC()->GetID(),pSkill->szBatkSfx,pSkill->nNumBatkSfx);
		}
		else if( IS_CREATURE( pCmd->dwSrcRoleID ) )
		{
			Role* pRole = RoleMgr::Inst()->FindRole( pCmd->dwSrcRoleID );
			if( P_VALID(pRole) && pRole->IsAttributeInited() && pRole->IS_KIND_OF(NPC) )
			{
				NPC* pNPC = (NPC*)pRole;
				const tagCreatureMdlData* pData=pNPC->GetMdlData();
				if( P_VALID(pData) )
					EffectMgr::Inst()->PlayBeAttackEffect(m_pFSM->GetNPC()->GetID(),pData->szNormalAttackBeAtkSfx.c_str(),1);
			}
		}

	}
	else if(pCmd->eCause==EHTC_Item)
	{
		const tagItemDisplayInfo* pItem=ItemProtoData::Inst()->FindItemDisplay(pCmd->dwMisc);
		THROW_NULL(pItem,_T("cannot find itemdisplayinfo"));

		if(pItem->szDestEffect[0]!=_T('\0'))
		{
			EffectMgr::Inst()->PlayRoleEffect(m_pFSM->GetNPC()->GetID(),pItem->szDestEffect);
		}
	}
}

void NS_HitTargetHandler_NPC::CacheCmd( tagNS_HitTarget* pCmd )
{
	tagData data;
	data.recvTime=Kernel::Inst()->GetAccumTimeDW();
	data.cmd=*pCmd;
	m_cache.push_back(data);
}

bool NS_HitTargetHandler_NPC::IfNeedClearCmd( tagNS_HitTarget* pCacheCmd,tagHitTargetEvent* pEvent )
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

bool NS_HitTargetHandler_NPC::IfNeedHandleCmd( tagNS_HitTarget* pCacheCmd,tagHitTargetEvent* pEvent )
{
	if(pCacheCmd->dwSrcRoleID==pEvent->dwSrcRoleID)
	{
		if(pEvent->dwSerial>=pCacheCmd->dwSerial)
			return true;
	}

	return false;
}








//--class NS_SyncGroundItemHandler_NPC-------------------------------------------
NS_SyncGroundItemHandler_NPC::NS_SyncGroundItemHandler_NPC()
{}

NS_SyncGroundItemHandler_NPC::~NS_SyncGroundItemHandler_NPC()
{}

void NS_SyncGroundItemHandler_NPC::OnNetCmd( tagNetCmd* pNetCmd )
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

void NS_SyncGroundItemHandler_NPC::OnGameEvent( tagGameEvent* pEvent )
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

void NS_SyncGroundItemHandler_NPC::Update()
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
















//--class NS_MonsterEnterCombatHandler_NPC-------------------------------------------
NS_MonsterEnterCombatHandler_NPC::NS_MonsterEnterCombatHandler_NPC()
{}

NS_MonsterEnterCombatHandler_NPC::~NS_MonsterEnterCombatHandler_NPC()
{}

void NS_MonsterEnterCombatHandler_NPC::OnNetCmd( tagNetCmd* pNetCmd )
{
	if(pNetCmd->dwID==m_pUtil->Crc32("NS_MonsterEnterCombat"))
	{
		CombatSysUtil::Inst()->PlayMonsterCombatSound(m_pFSM->GetNPC());
	}
}

void NS_MonsterEnterCombatHandler_NPC::OnGameEvent( tagGameEvent* pEvent )
{}

void NS_MonsterEnterCombatHandler_NPC::Update()
{}


//--class NS_MonsterPlayActionHandler_NPC--------------------------------------------

void NS_MonsterPlayActionHandler_NPC::OnNetCmd(tagNetCmd *pNetCmd)
{
	if (pNetCmd->dwID == m_pUtil->Crc32("NS_MonsterPlayAction"))
	{
		tagNS_MonsterPlayAction *pCmd = (tagNS_MonsterPlayAction*)(pNetCmd);
		tstring strAction = _FourCC2Str(pCmd->dwActionFourCC);
		m_pFSM->GetNPC()->PlayTrack(strAction.c_str(), false);
	}
}

void NS_MonsterPlayActionHandler_NPC::OnGameEvent(tagGameEvent* pEvent)
{

}

void NS_MonsterPlayActionHandler_NPC::Update()
{
	
}

//--class NS_MonsterSaHandler_NPC-----------------------------------------------------

NS_MonsterSaHandler_NPC::NS_MonsterSaHandler_NPC()
{
}

NS_MonsterSaHandler_NPC::~NS_MonsterSaHandler_NPC()
{
}

void NS_MonsterSaHandler_NPC::OnNetCmd(tagNetCmd *pNetCmd)
{
	if (pNetCmd->dwID == m_pUtil->Crc32("NS_MonsterSay"))
	{
		tagNS_MonsterSay *pCmd = (tagNS_MonsterSay*)(pNetCmd);
		DWORD dwID = pCmd->dwSayID;
		tstring strSay = MonsterSayTable::Inst()->GetSay(dwID);
		ChatShowPop::Inst()->PushChatInfo(pCmd->dwRoleID, strSay.c_str());
	}
}

void NS_MonsterSaHandler_NPC::OnGameEvent(tagGameEvent* pEvent)
{

}

void NS_MonsterSaHandler_NPC::Update()
{

}

