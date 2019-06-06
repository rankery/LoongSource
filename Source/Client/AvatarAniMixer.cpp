#include "StdAfx.h"
#include "AvatarAniMixer.h"
#include "Player.h"

const float g_fadeInTime=0.2f;//动作间渐变时间（秒）

AvatarAniMixer::AvatarAniMixer(void)
: m_pPlayer(NULL)
, m_state(EPS_None)
, m_bLowerIsStand(false)
, m_priAll(EPri_Lower)
, m_priUpper(EPri_Lower)
, m_priLower(EPri_Lower)
{
	m_cacheTrack.first=false;
}

AvatarAniMixer::~AvatarAniMixer(void)
{}

void AvatarAniMixer::PlayTrack( EAniPart aniPart,bool bIsStand,const TCHAR* szTrackName,bool bLoop,bool bForceReplay/*=false*/,float curTime/*=0.0f*/,float fadeInTime/*=0.2f*/,EPriority pri/*=EPri_Normal*/ )
{
	//IMSG(_T("roleid=%d,playtrack=%s,part=%d\n"),m_pPlayer->GetID(),szTrackName,aniPart);

	if(szTrackName==NULL
		||szTrackName[0]==0)
		return;

	NodeAnimationCtrl* pAniCtrl=m_pPlayer->GetAniCtrl();
	if(pAniCtrl==NULL)
	{
		//--把动画信息缓存起来
		m_cacheTrack.first=true;
		tagTrackInfo& track=m_cacheTrack.second;
		track.aniPart=aniPart;
		track.bIsStand=bIsStand;
		track.trackName=szTrackName;
		track.bLoop=bLoop;
		track.curTime=curTime;
		track.fadeInTime=fadeInTime;
		track.pri=pri;
		return;
	}

	//IMSG(_T("play track:%s,part:%d,state=%d\n"),szTrackName,aniPart,m_state);

	//--清除缓存标志
	m_cacheTrack.first=false;

	//--全身动作
	if(aniPart==EAP_All)
	{
		if(m_state==EPS_None)
		{
			pAniCtrl->OpenTrack(szTrackName,bLoop,curTime,fadeInTime,BPT_All);
			m_state=EPS_All;
			m_szAllTrack=szTrackName;
			m_priAll=pri;
		}
		else if(m_state==EPS_All)
		{
			if(m_szAllTrack==szTrackName
				&&!bForceReplay)
			{
				return;
			}

			if(pri>=m_priAll)
			{
				pAniCtrl->OpenTrack(szTrackName,bLoop,curTime,fadeInTime,BPT_All);
				m_state=EPS_All;
				m_szAllTrack=szTrackName;
				m_priAll=pri;
			}
		}
		else if(m_state==EPS_Lower)
		{
			if(pri>=m_priLower)
			{
				pAniCtrl->OpenTrack(szTrackName,bLoop,curTime,fadeInTime,BPT_All);
				m_state=EPS_All;
				m_szAllTrack=szTrackName;
				m_priAll=pri;
			}
		}
		else if(m_state==EPS_Upper)
		{
			if(pri>=m_priUpper)
			{
				pAniCtrl->OpenTrack(szTrackName,bLoop,curTime,fadeInTime,BPT_All);
				m_state=EPS_All;
				m_szAllTrack=szTrackName;
				m_priAll=pri;
			}
		}
		else if(m_state==EPS_Blend)
		{
			if(pri>=m_priUpper
				&&pri>=m_priLower)
			{
				pAniCtrl->OpenTrack(szTrackName,bLoop,curTime,fadeInTime,BPT_All);
				m_state=EPS_All;
				m_szAllTrack=szTrackName;
				m_priAll=pri;
			}
		}
		else if(m_state==EPS_UpperAndStand)
		{
			if(pri>=m_priUpper
				&&pri>=m_priLower)
			{
				pAniCtrl->OpenTrack(szTrackName,bLoop,curTime,fadeInTime,BPT_All);
				m_state=EPS_All;
				m_szAllTrack=szTrackName;
				m_priAll=pri;
			}
		}
	}
	//--上身动作
	else if(aniPart==EAP_Upper)
	{
		if(m_state==EPS_None)
		{
			pAniCtrl->OpenTrack(szTrackName,bLoop,curTime,fadeInTime,BPT_All);
			m_state=EPS_Upper;
			m_szUpperTrack=szTrackName;
			m_priUpper=pri;
		}
		else if(m_state==EPS_All)
		{
			if(pri>=m_priAll)
			{
				pAniCtrl->OpenTrack(szTrackName,bLoop,curTime,fadeInTime,BPT_All);
				m_state=EPS_Upper;
				m_szUpperTrack=szTrackName;
				m_priUpper=pri;
			}
		}
		else if(m_state==EPS_Lower)
		{
			if(m_bLowerIsStand)
			{
				pAniCtrl->OpenTrack(szTrackName,bLoop,curTime,fadeInTime,BPT_All);
				m_state=EPS_UpperAndStand;
				m_szUpperTrack=szTrackName;
				m_priUpper=pri;
			}
			else
			{
				pAniCtrl->OpenTrack(szTrackName,bLoop,curTime,fadeInTime,BPT_Upper);
				m_state=EPS_Blend;
				m_szUpperTrack=szTrackName;
				m_priUpper=pri;
			}
		}
		else if(m_state==EPS_Upper)
		{
			if(szTrackName==m_szUpperTrack
				&&!bForceReplay)
				return;

			if(pri>=m_priUpper)
			{
				pAniCtrl->OpenTrack(szTrackName,bLoop,curTime,fadeInTime,BPT_All);
				m_szUpperTrack=szTrackName;
				m_priUpper=pri;
			}
		}
		else if(m_state==EPS_Blend)
		{
			if(szTrackName==m_szUpperTrack
				&&!bForceReplay)
				return;

			if(pri>=m_priUpper)
			{
				pAniCtrl->OpenTrack(szTrackName,bLoop,curTime,fadeInTime,BPT_Upper);
				m_szUpperTrack=szTrackName;
				m_priUpper=pri;
			}
		}
		else if(m_state==EPS_UpperAndStand)
		{
			if(szTrackName==m_szUpperTrack
				&&!bForceReplay)
				return;

			if(pri>=m_priUpper)
			{
				pAniCtrl->OpenTrack(szTrackName,bLoop,curTime,fadeInTime,BPT_All);
				m_state=EPS_UpperAndStand;
				m_szUpperTrack=szTrackName;
				m_priUpper=pri;
			}
		}
	}
	//--下身动作
	else if(aniPart==EAP_Lower)
	{
		if(m_state==EPS_None)
		{
			pAniCtrl->OpenTrack(szTrackName,bLoop,curTime,fadeInTime,BPT_All);
			m_state=EPS_Lower;
			m_szLowerTrack=szTrackName;
			m_bLowerIsStand=bIsStand;
			m_priLower=pri;
		}
		else if(m_state==EPS_All)
		{
			if(pri>=m_priAll)
			{
				pAniCtrl->OpenTrack(szTrackName,bLoop,curTime,fadeInTime,BPT_All);
				m_state=EPS_Lower;
				m_szLowerTrack=szTrackName;
				m_bLowerIsStand=bIsStand;
				m_priLower=pri;
			}
		}
		else if(m_state==EPS_Lower)
		{
			if(szTrackName==m_szLowerTrack
				&&!bForceReplay)
				return;

			if(pri>=m_priLower)
			{
				pAniCtrl->OpenTrack(szTrackName,bLoop,curTime,fadeInTime,BPT_All);
				m_szLowerTrack=szTrackName;
				m_bLowerIsStand=bIsStand;
				m_priLower=pri;
			}
		}
		else if(m_state==EPS_Upper)
		{
			if(bIsStand)
			{
				m_state=EPS_UpperAndStand;
				m_szLowerTrack=szTrackName;
				m_bLowerIsStand=true;
				m_priLower=pri;
			}
			else
			{
				pAniCtrl->OpenTrack(szTrackName,bLoop,curTime,fadeInTime,BPT_Lower);
				m_state=EPS_Blend;
				m_szLowerTrack=szTrackName;
				m_bLowerIsStand=false;
				m_priLower=pri;
			}
		}
		else if(m_state==EPS_Blend)
		{
			if(szTrackName==m_szLowerTrack
				&&!bForceReplay)
				return;

			if(pri>=m_priLower)
			{
				if(bIsStand)
				{
					pAniCtrl->SyncTrack(m_szUpperTrack.c_str());
					m_state=EPS_UpperAndStand;
					m_szLowerTrack=szTrackName;
					m_bLowerIsStand=true;
					m_priLower=pri;
				}
				else
				{
					pAniCtrl->OpenTrack(szTrackName,bLoop,curTime,fadeInTime,BPT_Lower);
					m_szLowerTrack=szTrackName;
					m_bLowerIsStand=bIsStand;
					m_priLower=pri;
				}
			}
		}
		else if(m_state==EPS_UpperAndStand)
		{
			if(szTrackName==m_szLowerTrack
				&&!bForceReplay)
				return;

			if(pri>=m_priLower)
			{
				if(bIsStand)
				{
					m_szLowerTrack=szTrackName;
					m_bLowerIsStand=true;
					m_priLower=pri;
				}
				else
				{
					pAniCtrl->OpenTrack(szTrackName,bLoop,curTime,fadeInTime,BPT_Lower);
					m_state=EPS_Blend;
					m_szLowerTrack=szTrackName;
					m_bLowerIsStand=false;
					m_priLower=pri;
				}
			}
		}
	}
	else
	{
		ASSERT(0);
		return;
	}
}

void AvatarAniMixer::Update()
{
	//--播放缓存的动画
	if(m_cacheTrack.first)
	{
		const tagTrackInfo& track=m_cacheTrack.second;

		NodeAnimationCtrl* pAniCtrl=m_pPlayer->GetAniCtrl();
		if(pAniCtrl!=NULL)
		{
			PlayTrack(track.aniPart,
						track.bIsStand,
						track.trackName.c_str(),
						track.bLoop,
						true,
						track.curTime,
						track.fadeInTime,
						track.pri);
		}
	}
}

bool AvatarAniMixer::IsTrackEnd( const TCHAR* szTrackName )
{
	NodeAnimationCtrl* pAniCtrl=m_pPlayer->GetAniCtrl();
	if(pAniCtrl==NULL)
		return false;

	if(m_cacheTrack.first
		&&m_cacheTrack.second.trackName==szTrackName)
		return false;

	return pAniCtrl->IsTrackEnd(szTrackName);
}

void AvatarAniMixer::Sync2Lower()
{
	NodeAnimationCtrl* pAniCtrl=m_pPlayer->GetAniCtrl();
	if(pAniCtrl==NULL)
		return;

	//IMSG(_T("sync2lower,state=%d\n"),m_state);


	if(m_state==EPS_Blend)
	{
		pAniCtrl->SyncTrack(m_szLowerTrack.c_str());
		m_state=EPS_Lower;
	}
	else if(m_state==EPS_UpperAndStand)
	{
		pAniCtrl->OpenTrack(m_szLowerTrack.c_str(),true,0.0f,g_fadeInTime,BPT_All);
		m_state=EPS_Lower;
	}
}
