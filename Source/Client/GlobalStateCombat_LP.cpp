#include "StdAfx.h"
#include "GlobalStateCombat_LP.h"
#include "LocalPlayer.h"
#include "CombatEvent.h"
#include "FSM_LP.h"
#include "CombatSys.h"
#include "CombatSysUtil.h"
#include "NetSession.h"

GlobalStateCombat_LP::GlobalStateCombat_LP(void)
{}

GlobalStateCombat_LP::~GlobalStateCombat_LP(void)
{}

void GlobalStateCombat_LP::Active( tagGameEvent* pEvent,NavMap* pNav,LocalPlayer* pRole )
{
	ASSERT(pEvent->strEventName==_T("tagEnterCombatEvent"));

	GameFrameMgr* pGameFrmMgr=TObjRef<GameFrameMgr>();
	m_pCombatSys=(CombatSys*)pGameFrmMgr->GetFrame(_T("CombatSys"));
	ASSERT(P_VALID(m_pCombatSys));

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

	//--发送消息
	tagNC_EnterCombat cmd;
	TObjRef<NetSession>()->Send(&cmd);

	m_bClosed=false;
	m_bStartTimer=false;
	m_step=EStep_HandUpWeapon;
}

void GlobalStateCombat_LP::Update( NavMap* pNav,LocalPlayer* pRole )
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
			if(m_bStartTimer)
			{
				if(m_pFSM->IsSkillState()
					||m_pCombatSys->IsSelectEnemy())
				{
					m_bStartTimer=false;
				}
				else
				{
					if(Kernel::Inst()->GetAccumTimeDW()-m_dwStartTime>=5000)
					{
						pRole->OpenAllEquip(false);
						m_step=EStep_CloseEquip;
					}
				}
			}
			else
			{
				if(!m_pFSM->IsSkillState()
					&&!m_pCombatSys->IsSelectEnemy())
				{
					m_bStartTimer=true;
					m_dwStartTime=Kernel::Inst()->GetAccumTimeDW();
				}
			}
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

				m_bClosed=true;

				//--发送游戏事件
				tagLeaveCombatEvent event(_T("tagLeaveCombatEvent"),NULL);
				m_pFSM->OnGameEvent(&event);
				TObjRef<GameFrameMgr>()->SendEvent(&event);

				//--上身同步到下身动作
				pRole->GetAniPlayer()->Sync2Lower();

				//--发送消息
				tagNC_LeaveCombat cmd;
				TObjRef<NetSession>()->Send(&cmd);
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

void GlobalStateCombat_LP::OnGameEvent( tagGameEvent* pEvent )
{
	if(pEvent->strEventName==_T("tagEnterCombatEvent"))
	{
		if(m_step==EStep_HandDownWeapon)
		{
			LocalPlayer* pSelf=m_pFSM->GetLocalPlayer();

			//--播放拿出武器动作
			m_szEnterTrackName=CombatSysUtil::Inst()->GetEnterCombatTrackName(pSelf->GetRWeaponType());
			pSelf->GetAniPlayer()->PlayTrack(AvatarAniPlayer::EAT_Other,
				AvatarAniMixer::EAP_Upper,
				m_szEnterTrackName.c_str(),
				false,
				false,
				0.0f,
				0.2f,
				AvatarAniMixer::EPri_Lower);

			//--武器挂载到手上
			pSelf->HandUpWeapon(true);

			m_step=EStep_HandUpWeapon;
			m_bStartTimer=false;
		}
	}
}

void GlobalStateCombat_LP::OnNetCmd( tagNetCmd* pNetCmd )
{}