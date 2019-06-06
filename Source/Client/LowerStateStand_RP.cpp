#include "StdAfx.h"
#include "LowerStateStand_RP.h"
#include "EffectMgr.h"
#include "Player.h"
#include "FSM_RP.h"
#include "CombatSysUtil.h"
#include "..\WorldDefine\role_att.h"
#include "RoleEvent.h"

LowerStateStand_RP::LowerStateStand_RP(void):m_sprayEffectID(GT_INVALID)
{}

LowerStateStand_RP::~LowerStateStand_RP(void)
{}

void LowerStateStand_RP::Active( tagNetCmd* pNetCmd,NavMap* pNav,Player* pRole )
{
	//--播放动作
	if(PlayBuffAni(pRole))
	{
		m_step=EStep_Buff;
	}
	else if(pRole->IsReadyState())
	{
		PlayReadyAni(pRole);
		m_step=EStep_Ready;
	}
	else
	{
		PlayStandAni(pRole);
		m_lastPlayEaseTime=Kernel::Inst()->GetAccumTimeDW();
		m_step=EStep_Stand;
	}

	//--播放水花特效
	m_sprayEffectID=GT_INVALID;
	float waterH;
	if(pNav->GetCollider()->GetWaterHeight(pRole->GetPos(),waterH)
		&& pRole->GetPos().y<waterH)
	{
		Vector3 effectPos=pRole->GetPos();
		effectPos.y=waterH;
		m_sprayEffectID=EffectMgr::Inst()->PlayEffect(effectPos,pRole->GetDestYaw(),_T("txqt05"));
	}
}

void LowerStateStand_RP::Update(NavMap* pNav,Player* pRole)
{
	//--站立状态-------------------------------------------------------------
	if(m_step==EStep_Stand)
	{
		DWORD intervalTime=30000+(rand()%20000);

		if(Kernel::Inst()->GetAccumTimeDW()-m_lastPlayEaseTime>=intervalTime
			&&m_pFSM->IsUpperIdleState())
		{
			m_lastPlayEaseTime=Kernel::Inst()->GetAccumTimeDW();

			PlayEaseAni(pRole);
			m_step=EStep_Ease;
		}
	}
	//--备战状态--------------------------------------------------------------
	else if(m_step==EStep_Ready)
	{
	}
	//--悠闲动作--------------------------------------------------------------
	else if(m_step==EStep_Ease)
	{
		if( pRole->GetAniPlayer()->IsTrackEnd( AvatarAniPlayer::EAT_Ease, _T("std3") ) )
		{
			PlayStandAni(pRole);
			m_step=EStep_Stand;
		}
	}
	//--Buff动作--------------------------------------------------------------
	else if(m_step==EStep_Buff)
	{
	}
}

void LowerStateStand_RP::OnNetCmd( tagNetCmd* pNetCmd )
{
	if(pNetCmd->dwID==pNetCmd->Crc32("NS_SetRoleState"))//进入备战
	{
		tagNS_SetRoleState* pCmd=(tagNS_SetRoleState*)pNetCmd;

		if(pCmd->eState==ERS_Combat)
		{
			Player* pRole=m_pFSM->GetPlayer();

			if(m_step==EStep_Stand
				||m_step==EStep_Ease)
			{
				PlayReadyAni(pRole);
				m_step=EStep_Ready;
			}
		}
	}
	else if(pNetCmd->dwID==pNetCmd->Crc32("NS_UnSetRoleState"))//离开备战
	{
		tagNS_UnSetRoleState* pCmd=(tagNS_UnSetRoleState*)pNetCmd;

		if(pCmd->eState==ERS_Combat)
		{
			Player* pRole=m_pFSM->GetPlayer();

			if(m_step==EStep_Ready)
			{
				PlayStandAni(pRole);
				m_lastPlayEaseTime=Kernel::Inst()->GetAccumTimeDW();
				m_step=EStep_Stand;
			}
		}
	}
	else
		m_pFSM->Change2LowerState(pNetCmd);
}

void LowerStateStand_RP::OnGameEvent( tagGameEvent* pEvent )
{
	if(pEvent->strEventName==_T("tagWeaponChangeEvent"))//切换武器
	{
		tagWeaponChangeEvent* pWeaponChangeEvent = (tagWeaponChangeEvent*)pEvent;
		if( pWeaponChangeEvent->bRightWeapon )
		{
			if(m_step==EStep_Stand)
			{
				Player* pRole=m_pFSM->GetPlayer();

				if(m_step==EStep_Stand)
				{
					PlayStandAni(pRole);
				}
				else if(m_step==EStep_Ready)
				{
					PlayReadyAni(pRole);
				}
			}
		}
	}
	else if(pEvent->strEventName==_T("tagRoleBuffChangeEvent"))//增加、删除、更新Buff
	{
		Player* pRole=m_pFSM->GetPlayer();

		if(m_step==EStep_Stand
			||m_step==EStep_Ready
			||m_step==EStep_Ease)
		{
			if(PlayBuffAni(pRole))
			{
				m_step=EStep_Buff;
			}
		}
		else if(m_step==EStep_Buff)
		{
			if(!PlayBuffAni(pRole))
			{
				if(pRole->IsReadyState())
				{
					PlayReadyAni(pRole);
					m_step=EStep_Ready;
				}
				else
				{
					PlayStandAni(pRole);
					m_lastPlayEaseTime=Kernel::Inst()->GetAccumTimeDW();
					m_step=EStep_Stand;
				}
			}
		}
	}
}

void LowerStateStand_RP::OnExit()
{
	//--停止水花特效
	if(m_sprayEffectID!=GT_INVALID)
	{
		EffectMgr::Inst()->StopEffect(m_sprayEffectID);
		m_sprayEffectID=GT_INVALID;
	}
}

void LowerStateStand_RP::ReplayAni( Player* pRole )
{
	if(m_step==EStep_Stand)
		PlayStandAni(pRole,false);
	else if(m_step==EStep_Ready)
		PlayReadyAni(pRole,false);
	else if(m_step==EStep_Buff)
		PlayBuffAni(pRole,false);
	else if(m_step==EStep_Ease)
		PlayEaseAni(pRole,false);
}

void LowerStateStand_RP::PlayReadyAni(Player* pRole,bool bCheckIfCanPlay)
{
	if(!bCheckIfCanPlay
		||m_pFSM->IfCanPlayLowerAni())
	{
		const TCHAR* szTrackName=CombatSysUtil::Inst()->GetReadyTrackName(pRole->GetRWeaponType());
		pRole->GetAniPlayer()->PlayTrack(AvatarAniPlayer::EAT_Ready,AvatarAniMixer::EAP_Lower,szTrackName,true);
	}
}

void LowerStateStand_RP::PlayStandAni(Player* pRole,bool bCheckIfCanPlay)
{
	if(!bCheckIfCanPlay
		||m_pFSM->IfCanPlayLowerAni())
	{
		const TCHAR* szTrackName=CombatSysUtil::Inst()->GetStandTrackName(pRole->GetRWeaponType());
		pRole->GetAniPlayer()->PlayTrack(AvatarAniPlayer::EAT_Stand,AvatarAniMixer::EAP_Lower,szTrackName,true);
	}
}

void LowerStateStand_RP::PlayEaseAni(Player* pRole,bool bCheckIfCanPlay)
{
	if(!bCheckIfCanPlay
		||m_pFSM->IfCanPlayLowerAni())
	{
		pRole->GetAniPlayer()->PlayTrack(AvatarAniPlayer::EAT_Ease,AvatarAniMixer::EAP_All,_T("std3"),false);
	}
}

bool LowerStateStand_RP::PlayBuffAni( Player* pRole,bool bCheckIfCanPlay )
{
	list<tagRoleBuff*>& buffList=pRole->GetBuffList().GetList();

	for(list<tagRoleBuff*>::const_reverse_iterator iter=buffList.rbegin();
		iter!=buffList.rend();++iter)
	{
		tagRoleBuff* pBuff=*iter;
		if(pBuff->pBuffProto->szActionMod[0]!=_T('\0'))
		{
			if(!bCheckIfCanPlay
				||m_pFSM->IfCanPlayLowerAni())
			{
				pRole->GetAniPlayer()->PlayTrack(AvatarAniPlayer::EAT_Other,AvatarAniMixer::EAP_All,pBuff->pBuffProto->szActionMod,true,true);
			}
			return true;
		}
	}

	return false;
}

bool LowerStateStand_RP::IfCanChange2UpperState( FSM_RP::EUpperState state )
{
	return true;
}