#include "stdafx.h"
#include "ActivityPvPMgr.h"
#include "..\WorldDefine\ScriptMsgInfo.h"
#include "ActivityPvPFrame.h"
#include "CountDownFrame.h"
#include "PlayerNameTab.h"

bool ComparePvPString(const tagPvPHero& hero1, const tagPvPHero& hero2)
{
	return hero1.strRoleName < hero2.strRoleName;
}

bool ComparePvPScore(const tagPvPHero& hero1, const tagPvPHero& hero2)
{
	return hero1.nScore > hero2.nScore;
}

bool ComparePvPMultiString(const tagPvPMultiHero& hero1, const tagPvPMultiHero& hero2)
{
	return hero1.strTeamLeaderName < hero2.strTeamLeaderName;
}	

bool ComarePvPMultiTeamID(const tagPvPMultiHero& hero1, const tagPvPMultiHero& hero2)
{
	return hero1.dwTeamID < hero2.dwTeamID;
}


bool ComparePvPMultiTeamScore(const tagPvPMultiHero& hero1, const tagPvPMultiHero& hero2)
{
	return hero1.nTeamScore > hero2.nTeamScore;
}

ActivityPvPMgr::ActivityPvPMgr( void ):m_Trunk(this)
{
	m_dwActivityType		= GT_INVALID;
	m_dwActivityID			= GT_INVALID;
	m_dwScore				= GT_INVALID;
	m_dwRevive				= GT_INVALID;
	m_dwTeamID				= GT_INVALID;
	m_strActivityName		= _T("");
}

ActivityPvPMgr::~ActivityPvPMgr( void )
{

}

ActivityPvPMgr g_ActivityPvpMgr;
ActivityPvPMgr* ActivityPvPMgr::Inst()
{
	return &g_ActivityPvpMgr;
}

void ActivityPvPMgr::Init()
{
	m_pNetCmdMgr		= TObjRef<NetCmdMgr>();
	m_pGameFrameMgr		= TObjRef<GameFrameMgr>();
	m_pSession			= TObjRef<NetSession>();

	//注册游戏事件
	m_pGameFrameMgr->RegisterEventHandle( _T("tagRoleGetNameEvent"), (FRAMEEVENTPROC)m_Trunk.sfp1(&ActivityPvPMgr::OnRoleGetNameEvent));
}

void ActivityPvPMgr::Destroy()
{
	m_vecPvPHero.clear();
	m_vecPvPMhero.clear();

	m_vecPvPHeroTmp.clear();
	m_vecPvPMheroTmp.clear();

	//注销游戏事件
	m_pGameFrameMgr->UnRegisterEventHandler( _T("tagRoleGetNameEvent"), (FRAMEEVENTPROC)m_Trunk.sfp1(&ActivityPvPMgr::OnRoleGetNameEvent));

	ActivityPvPFrame* pFrame = (ActivityPvPFrame*)m_pGameFrameMgr->GetFrame(_T("ActivityPvP"));
	if( P_VALID(pFrame) )
	{
		m_pGameFrameMgr->DestroyFrame(pFrame);
	}
	CountDownFrame* pCDFrame = (CountDownFrame*)m_pGameFrameMgr->GetFrame(_T("CountDown"));
	if( P_VALID(pCDFrame) )
	{
		m_pGameFrameMgr->DestroyFrame(pCDFrame);
	}
}

void ActivityPvPMgr::ClearScoreInfo()
{
	m_vecPvPHero.clear();
	m_vecPvPMhero.clear();
	m_vecPvPHeroTmp.clear();
	m_vecPvPMheroTmp.clear();
	m_dwActivityType		= GT_INVALID;
	m_dwActivityID			= GT_INVALID;
	m_dwScore				= GT_INVALID;
	m_dwRevive				= GT_INVALID;
	m_dwTeamID				= GT_INVALID;
	m_strActivityName		= _T("");
}

DWORD ActivityPvPMgr::OnRoleGetNameEvent( tagRoleGetNameEvent* pGameEvent )
{
	if( pGameEvent->bResult )
	{
		if( m_dwActivityType == ActivityPvPFrame::E_PvP )
		{
			vector<tagPvPHero>::iterator it1 = m_vecPvPHeroTmp.begin();
			for( ; it1!=m_vecPvPHeroTmp.end(); ++it1 )
			{
				if( it1->dwRoleID == pGameEvent->dwRoleID )
				{
					tagPvPHero tmp;
					tmp.dwRoleID	= it1->dwRoleID;
					tmp.nScore		= it1->nScore;
					tmp.strRoleName = pGameEvent->strRoleName;
					m_vecPvPHero.push_back(tmp);
					it1 = m_vecPvPHeroTmp.erase(it1);
					break;
				}
			}
		}
		else if( m_dwActivityType == ActivityPvPFrame::E_PvPMulti )
		{
			vector<tagPvPMultiHero>::iterator it2 = m_vecPvPMheroTmp.begin();
			for( ; it2!=m_vecPvPMheroTmp.end(); ++it2 )
			{
				if( it2->dwTeamLeaderID == pGameEvent->dwRoleID )
				{
					tagPvPMultiHero tmp;
					tmp.dwTeamID			= it2->dwTeamID;
					tmp.dwTeamLeaderID		= it2->dwTeamLeaderID;
					tmp.nTeamScore			= it2->nTeamScore;
					tmp.strTeamLeaderName	= pGameEvent->strRoleName;
					m_vecPvPMhero.push_back(tmp);
					it2 = m_vecPvPMheroTmp.erase(it2);
					break;
				}
			}
		}

		RefreshScoreSort();
	}

	return 0;
}

void ActivityPvPMgr::SetActivityName( const TCHAR* name, DWORD id )
{
	if( (INT)id==GT_INVALID )
	{
		ClearPvPUI();
		return;
	}
	m_strActivityName	= name;
	m_dwActivityID		= id;

	//根据不同PVP活动创建不同内容的控件
	ActivityPvPFrame* pPvPFrame = (ActivityPvPFrame*)m_pGameFrameMgr->GetFrame(_T("ActivityPvP"));
	if( !P_VALID( pPvPFrame ) )
	{
		m_pGameFrameMgr->CreateFrame(_T("World"), _T("ActivityPvP"), _T("ActivityPvPFrame"), m_dwActivityID);
	}
	else if( P_VALID(pPvPFrame) )
		//刷新活动界面内容
	{
		pPvPFrame->RefreshSelfUI();
	}
}

void ActivityPvPMgr::SetActivityCD( DWORD dwCD )
{
	//设置活动时间
	CountDownFrame* pCDFrame = (CountDownFrame*)m_pGameFrameMgr->GetFrame(_T("CountDown"));
	if( !P_VALID(pCDFrame) )
	{
		pCDFrame = (CountDownFrame*)m_pGameFrameMgr->CreateFrame(_T("World"), _T("CountDown"), _T("CountDownFrame"), 0);
		pCDFrame->Show();
		pCDFrame->SetCDTime(dwCD*1000);
	}
	else 
	{
		pCDFrame->Show();
		pCDFrame->SetCDTime(dwCD*1000);
	}
}

void ActivityPvPMgr::SetActivityScore( DWORD dwScore )
{
	m_dwScore	= dwScore;
	ActivityPvPFrame* pPvPFrame = (ActivityPvPFrame*)m_pGameFrameMgr->GetFrame(_T("ActivityPvP"));
	if( P_VALID(pPvPFrame) )
	{
		pPvPFrame->RefreshSelfUI();
	}
}

void ActivityPvPMgr::SetActivityRebornTime( DWORD dwRevive )
{
	m_dwRevive = dwRevive;
	ActivityPvPFrame* pPvPFrame = (ActivityPvPFrame*)m_pGameFrameMgr->GetFrame(_T("ActivityPvP"));
	if( P_VALID(pPvPFrame) )
	{
		pPvPFrame->RefreshSelfUI();
	}
}

void ActivityPvPMgr::SetActivityTeamID( DWORD dwTeamID )
{
	m_dwTeamID = dwTeamID;
	ActivityPvPFrame* pPvPFrame = (ActivityPvPFrame*)m_pGameFrameMgr->GetFrame(_T("ActivityPvP"));
	if( P_VALID(pPvPFrame) )
	{
		pPvPFrame->RefreshSelfUI();
	}
}

void ActivityPvPMgr::SetPvPActivityScoreSort( DWORD dwRank, DWORD dwRoleID, DWORD dwScore )
{
	tagPvPHero hero;
	hero.dwRank = dwRank;
	hero.dwRoleID = dwRoleID;
	hero.nScore = dwScore;

	tstring strRoleName = PlayerNameTab::Inst()->FindNameByID(dwRoleID);
	
	if( strRoleName.empty() )
	{
		m_vecPvPHeroTmp.push_back( hero );
	}
	else
	{
		bool bHave = false;
		vector<tagPvPHero>::iterator it = m_vecPvPHero.begin();
		for( ; it!=m_vecPvPHero.end(); ++it )
		{
			if( it->dwRoleID == dwRoleID )
			{
				it->dwRank = dwRank;
				it->nScore = dwScore;
				bHave = true;
				break;
			}
		}
		if( !bHave )
		{
			hero.strRoleName = strRoleName;
			m_vecPvPHero.push_back( hero );
		}
	}
}

void ActivityPvPMgr::SetPvPMultiActivityScoreSort( DWORD dwRank, DWORD dwRoleID, DWORD dwScore, DWORD dwTeamID )
{
	tagPvPMultiHero hero;
	hero.dwRank = dwRank;
	hero.dwTeamLeaderID = dwRoleID;
	hero.nTeamScore = dwScore;
	hero.dwTeamID = dwTeamID;

	tstring strRoleName = PlayerNameTab::Inst()->FindNameByID(dwRoleID);

	if(strRoleName.empty())
	{
		m_vecPvPMheroTmp.push_back(hero);
	}
	else
	{
		bool bHave = false;
		vector<tagPvPMultiHero>::iterator it = m_vecPvPMhero.begin();
		for( ; it!=m_vecPvPMhero.end(); ++it )
		{
			if( it->dwTeamID == dwTeamID )
			{
				it->dwRank = dwRank;
				it->nTeamScore = dwScore;
				bHave = true;
				break;
			}
		}
		if( !bHave )
		{
			hero.strTeamLeaderName = strRoleName;
			m_vecPvPMhero.push_back(hero);
		}
	}
}

void ActivityPvPMgr::RefreshScoreSort()
{
	ActivityPvPFrame* pPvPFrame = (ActivityPvPFrame*)m_pGameFrameMgr->GetFrame(_T("ActivityPvP"));
	if( P_VALID(pPvPFrame) )
	{
		if( m_dwActivityType == 0 )
		{
			std::sort( m_vecPvPHero.begin(), m_vecPvPHero.end(), ComparePvPScore );
		}
		else if( m_dwActivityType == 1 )
		{
			std::sort( m_vecPvPMhero.begin(), m_vecPvPMhero.end(), ComparePvPMultiTeamScore );
		}
		
		pPvPFrame->RefreshScoreUI();
	}
}

void ActivityPvPMgr::ClearPvPUI()
{
	ActivityPvPFrame* pFrame = (ActivityPvPFrame*)m_pGameFrameMgr->GetFrame(_T("ActivityPvP"));
	if( P_VALID(pFrame) )
	{
		m_pGameFrameMgr->DestroyFrame(pFrame);
	}

	CountDownFrame* pCDFrame = (CountDownFrame*)m_pGameFrameMgr->GetFrame(_T("CountDown"));
	if( P_VALID(pCDFrame) )
	{
		m_pGameFrameMgr->DestroyFrame(pCDFrame);
	}
}

DWORD ActivityPvPMgr::GetPvPType()
{
	if( m_dwActivityID>=14 && m_dwActivityID <=17 )
	{
		return 0;
	}
	else if( m_dwActivityID>=18 && m_dwActivityID <=21 )
	{
		return 1;
	}
	return 0;
}