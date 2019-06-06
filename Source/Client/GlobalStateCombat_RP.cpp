#include "StdAfx.h"
#include "GlobalStateCombat_RP.h"
#include "Player.h"
#include "FSM_RP.h"
#include "..\WorldDefine\role_att.h"
#include "CombatSysUtil.h"

GlobalStateCombat_RP::GlobalStateCombat_RP(void)
{}

GlobalStateCombat_RP::~GlobalStateCombat_RP(void)
{}

void GlobalStateCombat_RP::Active( tagNetCmd* pNetCmd,NavMap* pNav,Player* pRole,DWORD dwParam )
{
	ASSERT(pNetCmd->dwID==m_pUtil->Crc32("NS_SetRoleState"));

	if(dwParam==0)
	{
		//--播放拿出武器动作
		m_szEnterTrackName=CombatSysUtil::Inst()->GetEnterCombatTrackName(pRole->GetRWeaponType());
		pRole->GetAniPlayer()->PlayTrack(AvatarAniPlayer::EAT_Other,
			AvatarAniMixer::EAP_Upper,
			m_szEnterTrackName.c_str(),
			false,
			false,
			0.0f,
			0.2f,
			AvatarAniMixer::EPri_Lower);

		//--武器挂载到手上
		pRole->HandUpWeapon(true);

		m_step=EStep_HandUpWeapon;
	}
	else
	{
		//--武器挂载到手上
		pRole->HandUpWeapon(true);

		//--展开装备
		pRole->OpenAllEquip(true);
		m_step=EStep_OpenEquip;
	}

	m_bClosed=false;
}

void GlobalStateCombat_RP::Update( NavMap* pNav,Player* pRole )
{
	switch(m_step)
	{
	case EStep_HandUpWeapon:   //拔武器
		{
			if(pRole->GetAniPlayer()->IsTrackEnd(AvatarAniPlayer::EAT_Other,m_szEnterTrackName.c_str()))
			{
				//--上身同步到下身动作
				pRole->GetAniPlayer()->Sync2Lower();

				pRole->OpenAllEquip(true);
				m_step=EStep_OpenEquip;
			}
		}
		break;
	case EStep_OpenEquip:      //展开装备（变形）
		{
			if(pRole->IsAllEquipOpened())
			{
				m_step=EStep_Fighting;
			}
		}
		break;
	case EStep_Fighting:	   //战斗状态
		{
		}
		break;
	case EStep_CloseEquip:     //关闭装备（变形）
		{
			if( pRole->IsAllEquipClosed() )
			{
				//--播放收回武器动作
				m_szLeaveTrackName=CombatSysUtil::Inst()->GetLeaveCombatTrackName(pRole->GetRWeaponType());
				pRole->GetAniPlayer()->PlayTrack(AvatarAniPlayer::EAT_Other,
					AvatarAniMixer::EAP_Upper,
					m_szLeaveTrackName.c_str(),
					false,
					false,
					0.0f,
					0.2f,
					AvatarAniMixer::EPri_Lower);
				m_step=EStep_HandDownWeapon;
			}
		}
		break;
	case EStep_HandDownWeapon: //收武器
		{
			if( pRole->GetAniPlayer()->IsTrackEnd(AvatarAniPlayer::EAT_Other,m_szLeaveTrackName.c_str()) )
			{
				pRole->HandUpWeapon(false);

				//--上身同步到下身动作
				pRole->GetAniPlayer()->Sync2Lower();

				m_bClosed=true;
			}
			else
			{
				//--武器挂载到身上
				const DWORD dwPutMsg = _MakeFourCC(_T("put"));
				const vector<DWORD>& msgCodes = pRole->GetMsgCodes();
				for( size_t nMsgCode = 0; nMsgCode < msgCodes.size(); nMsgCode++ )
				{
					if( dwPutMsg == msgCodes[nMsgCode] )
					{
						pRole->HandUpWeapon(false);
					}
				}
			}
		}
		break;
	default:
		break;
	}
}

void GlobalStateCombat_RP::OnGameEvent( tagGameEvent* pEvent )
{}

void GlobalStateCombat_RP::OnNetCmd( tagNetCmd* pNetCmd )
{
	if(pNetCmd->dwID==m_pUtil->Crc32("NS_SetRoleState"))
	{
		if(m_step==EStep_HandDownWeapon)
		{
			tagNS_SetRoleState* pCmd=(tagNS_SetRoleState*)pNetCmd;
			if(pCmd->eState==ERS_Combat)
			{
				Player* pRole=m_pFSM->GetPlayer();

				//--播放拿出武器动作
				m_szEnterTrackName=CombatSysUtil::Inst()->GetEnterCombatTrackName(pRole->GetRWeaponType());
				pRole->GetAniPlayer()->PlayTrack(AvatarAniPlayer::EAT_Other,
					AvatarAniMixer::EAP_Upper,
					m_szEnterTrackName.c_str(),
					false,
					false,
					0.0f,
					0.2f,
					AvatarAniMixer::EPri_Lower);

				m_step=EStep_HandUpWeapon;
			}
		}
	}
	else if(pNetCmd->dwID==m_pUtil->Crc32("NS_UnSetRoleState"))
	{
		tagNS_UnSetRoleState* pCmd=(tagNS_UnSetRoleState*)pNetCmd;
		if(pCmd->eState==ERS_Combat)
		{
			Player* pRole=m_pFSM->GetPlayer();

			pRole->OpenAllEquip(false);
			m_step=EStep_CloseEquip;
		}
	}
}