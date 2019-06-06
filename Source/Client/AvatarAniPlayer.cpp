#include "StdAfx.h"
#include "AvatarAniPlayer.h"
#include "Player.h"
#include "Pet.h"
#include "CombatSysUtil.h"
#include "PetProtoData.h"

AvatarAniPlayer::AvatarAniPlayer(void)
{}

AvatarAniPlayer::~AvatarAniPlayer(void)
{}

void AvatarAniPlayer::SetPlayer( Player* pPlayer )
{
	m_aniMixer.SetPlayer(pPlayer);
}

void AvatarAniPlayer::PlayTrack( EAniType aniType,AvatarAniMixer::EAniPart aniPart,const TCHAR* szTrackName,bool bLoop,bool bForceReplay/*=false*/,float curTime/*=0.0f*/,float fadeInTime/*=0.2f*/,AvatarAniMixer::EPriority pri/*=AvatarAniMixer::EPri_Normal*/ )
{
	Pet *pMount = m_aniMixer.GetPlayer()->GetMount();
	if( P_VALID(pMount) )
	{
		if( pMount->GetOwnerID() == m_aniMixer.GetPlayer()->GetID() )
		{
			//根据aniType获得坐骑动作，交给宠物系统播放
			LPCTSTR petTrack = CombatSysUtil::Inst()->GetMountTrackName(aniType,m_aniMixer.GetPlayer()->GetMoveSpeed());
			if( _tcslen(petTrack) > 0 )
			{
				pMount->PlayTrack(petTrack,bLoop);

				tstring playerTrack = PetProtoData::Inst()->GetAction1(pMount->GetTypeID());
				m_aniMixer.PlayTrack(AvatarAniMixer::EAP_All,false,playerTrack.c_str(),true,false,0.0f,0.2f,AvatarAniMixer::EPri_Normal);
			}
		}
		else//搭乘状态
		{
			tstring playerTrack = PetProtoData::Inst()->GetAction2(pMount->GetTypeID());
			m_aniMixer.PlayTrack(AvatarAniMixer::EAP_All,false,playerTrack.c_str(),true,false,0.0f,0.2f,AvatarAniMixer::EPri_Normal);
		}
	}
	else
	{
		bool bIsStand=(aniType==EAT_Stand||aniType==EAT_Ready);
		m_aniMixer.PlayTrack(aniPart,bIsStand,szTrackName,bLoop,bForceReplay,curTime,fadeInTime,pri);
	}
}

void AvatarAniPlayer::Sync2Lower()
{
	m_aniMixer.Sync2Lower();
}

bool AvatarAniPlayer::IsTrackEnd( EAniType aniType,const TCHAR* szTrackName )
{
	Pet *pMount = m_aniMixer.GetPlayer()->GetMount();
	if( P_VALID(pMount) )
	{
		//根据aniType获得坐骑动作，判断
		LPCTSTR petTrack = CombatSysUtil::Inst()->GetMountTrackName(aniType,m_aniMixer.GetPlayer()->GetMoveSpeed());
		if( _tcslen(petTrack) > 0 )
		{
			return pMount->IsTrackEnd(petTrack);
		}
		else
			return true;
	}
	else
	{
		return m_aniMixer.IsTrackEnd(szTrackName);
	}
}

void AvatarAniPlayer::Update()
{
	m_aniMixer.Update();
}