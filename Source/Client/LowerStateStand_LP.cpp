#include "StdAfx.h"
#include "LowerStateStand_LP.h"
#include "FSM_LP.h"
#include "LocalPlayer.h"
#include "CombatEvent.h"
#include "..\WorldDefine\action.h"
#include "RoleEvent.h"
#include "EffectMgr.h"
#include "CombatSysUtil.h"

LowerStateStand_LP::LowerStateStand_LP(void)
: m_sprayEffectID(GT_INVALID)
{}

LowerStateStand_LP::~LowerStateStand_LP(void)
{}

void LowerStateStand_LP::Active( tagGameEvent* pEvent,NavMap* pNav,LocalPlayer* pRole,tagPreState* pPreState )
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

	//--发送游戏事件
	tagEnterStandStateEvent event(_T("tagEnterStandStateEvent"),NULL);
	TObjRef<GameFrameMgr>()->SendEvent(&event);

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

void LowerStateStand_LP::Update(NavMap* pNav,LocalPlayer* pRole)
{
	//--站立状态-------------------------------------------------------------
	if(m_step==EStep_Stand)
	{
		DWORD intervalTime=60000+(rand()%20000);

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

void LowerStateStand_LP::OnGameEvent( tagGameEvent* pEvent )
{
	if(pEvent->strEventName==_T("tagMouseMoveEvent"))
	{
		m_pFSM->TryChange2LowerState(FSM_LP::ELowerState_MouseMove,pEvent);
	}
	else if(pEvent->strEventName==_T("tagKeyboardMoveEvent"))
	{
		m_pFSM->TryChange2LowerState(FSM_LP::ELowerState_KeyboardMove,pEvent);
	}
	else if(pEvent->strEventName==_T("tagJumpEvent"))
	{
		m_pFSM->TryChange2LowerState(FSM_LP::ELowerState_Jump,pEvent);
	}
	else if(pEvent->strEventName==_T("tagEnterCombatEvent"))//进入备战
	{
		LocalPlayer* pSelf=m_pFSM->GetLocalPlayer();

		if(m_step==EStep_Stand
			||m_step==EStep_Ease)
		{
			PlayReadyAni(pSelf);
			m_step=EStep_Ready;
		}
	}
	else if(pEvent->strEventName==_T("tagLeaveCombatEvent"))//离开备战
	{
		LocalPlayer* pSelf=m_pFSM->GetLocalPlayer();

		if(m_step==EStep_Ready)
		{
			PlayStandAni(pSelf);
			m_lastPlayEaseTime=Kernel::Inst()->GetAccumTimeDW();
			m_step=EStep_Stand;
		}
	}
	else if(pEvent->strEventName==_T("tagWeaponChangeEvent"))//切换武器
	{	
		tagWeaponChangeEvent* pWeaponChangeEvent = (tagWeaponChangeEvent*)pEvent;
		if( pWeaponChangeEvent->bRightWeapon )
		{
			LocalPlayer* pSelf=m_pFSM->GetLocalPlayer();

			if(m_step==EStep_Stand)
			{
				PlayStandAni(pSelf);
			}
			else if(m_step==EStep_Ready)
			{
				PlayReadyAni(pSelf);
			}
		}
	}
	else if(pEvent->strEventName==_T("tagRoleBuffChangeEvent"))//增加、删除、更新Buff
	{
		LocalPlayer* pSelf=m_pFSM->GetLocalPlayer();

		if(m_step==EStep_Stand
			||m_step==EStep_Ready
			||m_step==EStep_Ease)
		{
			if(PlayBuffAni(pSelf))
			{
				m_step=EStep_Buff;
			}
		}
		else if(m_step==EStep_Buff)
		{
			if(!PlayBuffAni(pSelf))
			{
				if(pSelf->IsReadyState())
				{
					PlayReadyAni(pSelf);
					m_step=EStep_Ready;
				}
				else
				{
					PlayStandAni(pSelf);
					m_lastPlayEaseTime=Kernel::Inst()->GetAccumTimeDW();
					m_step=EStep_Stand;
				}
			}
		}
	}
}

void LowerStateStand_LP::OnNetCmd( tagNetCmd* pNetCmd )
{
	//--瞬移消息
	if(pNetCmd->dwID==pNetCmd->Crc32("NS_SpecialMove"))
	{
		tagNS_SpecialMove* pCmd=(tagNS_SpecialMove*)pNetCmd;
		if( ESMT_Teleport == pCmd->eType )
		{
			m_pFSM->Change2LowerState_AutoStand();
		}
	}
}

void LowerStateStand_LP::OnExit()
{
	//--停止水花特效
	if(m_sprayEffectID!=GT_INVALID)
	{
		EffectMgr::Inst()->StopEffect(m_sprayEffectID);
		m_sprayEffectID=GT_INVALID;
	}
}

void LowerStateStand_LP::ReplayAni( LocalPlayer* pRole )
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

bool LowerStateStand_LP::IfCanChange2UpperState( FSM_LP::EUpperState state,tagGameEvent* pEvent )
{
	return true;
}

bool LowerStateStand_LP::IfCanChange2LowerState( FSM_LP::ELowerState state,tagGameEvent* pEvent )
{
	return true;
}

void LowerStateStand_LP::PlayReadyAni(LocalPlayer* pRole,bool bCheckIfCanPlay)
{
	if(!bCheckIfCanPlay
		||m_pFSM->IfCanPlayLowerAni())
	{
		const TCHAR* szTrackName=CombatSysUtil::Inst()->GetReadyTrackName(pRole->GetRWeaponType());
		pRole->GetAniPlayer()->PlayTrack(AvatarAniPlayer::EAT_Ready,AvatarAniMixer::EAP_Lower,szTrackName,true);
	}
}

void LowerStateStand_LP::PlayStandAni(LocalPlayer* pRole,bool bCheckIfCanPlay)
{
	if(!bCheckIfCanPlay
		||m_pFSM->IfCanPlayLowerAni())
	{
		const TCHAR* szTrackName=CombatSysUtil::Inst()->GetStandTrackName(pRole->GetRWeaponType());
		pRole->GetAniPlayer()->PlayTrack(AvatarAniPlayer::EAT_Stand,AvatarAniMixer::EAP_Lower,szTrackName,true);
	}
}

void LowerStateStand_LP::PlayEaseAni(LocalPlayer* pRole,bool bCheckIfCanPlay)
{
	if(!bCheckIfCanPlay
		||m_pFSM->IfCanPlayLowerAni())
	{
		pRole->GetAniPlayer()->PlayTrack(AvatarAniPlayer::EAT_Ease,AvatarAniMixer::EAP_All,_T("std3"),false);
	}
}

bool LowerStateStand_LP::PlayBuffAni( LocalPlayer* pRole,bool bCheckIfCanPlay )
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



void LowerStateStand_LP::ResetLastEaseTime()
{
	m_lastPlayEaseTime=Kernel::Inst()->GetAccumTimeDW();
}