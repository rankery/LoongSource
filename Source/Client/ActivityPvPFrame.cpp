#include "stdafx.h"
#include "ActivityPvPFrame.h"
#include "ActivityPvPMgr.h"
#include "..\WorldDefine\msg_map.h"
#include "..\WorldDefine\msg_script.h"
#include "..\WorldDefine\ScriptMsgInfo.h"

ActivityPvPFrame::ActivityPvPFrame( void ):m_Trunk(this)
{
	m_pWnd_Rank			= NULL;
	m_pLB_RankInfo		= NULL;
	m_pBtn_TeamID		= NULL;
	m_pBtn_NameSort		= NULL;
	m_pBtn_Score		= NULL;
	m_pBtn_Cancel		= NULL;

	m_pWnd_Self			= NULL;
	m_pStc_Info			= NULL;
	m_pBtn_Rank			= NULL;
	m_pBtn_Exit			= NULL;

	m_dwActivityID		 = GT_INVALID;
}

ActivityPvPFrame::~ActivityPvPFrame( void )
{

}

BOOL ActivityPvPFrame::Init( GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam )
{
	//初始化前,保存活动类型
	m_dwActivityID = dwParam;

	if( !GameFrame::Init(pMgr, pFather, dwParam) )
		return FALSE;

	return TRUE;
}

BOOL ActivityPvPFrame::Destroy()
{
	BOOL bRet = GameFrame::Destroy();

	if( P_VALID(m_pWnd_Self) )
		m_pGUI->DestroyWnd(m_pWnd_Self);

	if( P_VALID(m_pWnd_Rank) )
		m_pGUI->DestroyWnd(m_pWnd_Rank);	

	return bRet;
}

BOOL ActivityPvPFrame::ReloadUI()
{
	XmlElement element;
	tstring strPath = g_strLocalPath +  _T("\\ui\\team_pk.xml");
	m_pGUI->LoadXml(&element, "VFS_System", strPath.c_str());
	m_pWnd_Self = m_pGUI->CreateWnd(_T("\\desktop"), &element);	
	m_pStc_Info	= (GUIStatic*)m_pWnd_Self->GetChild(_T("tpk_backpic"));
	m_pBtn_Rank	= (GUIButton*)m_pStc_Info->GetChild(_T("listbutt"));
	m_pBtn_Exit = (GUIButton*)m_pStc_Info->GetChild(_T("outbutt"));

	tstring strPath1;
	
	if( m_dwActivityID>=14 && m_dwActivityID <=17 )
	{
		strPath1 = g_strLocalPath +  _T("\\ui\\pvplist.xml");		
	}
	else if( m_dwActivityID>=18 && m_dwActivityID <=21 )
	{
		strPath1 = g_strLocalPath +  _T("\\ui\\team_pvplist.xml");	
	}
	m_pGUI->LoadXml(&element, "VFS_System", strPath1.c_str());
	m_pWnd_Rank  = m_pGUI->CreateWnd(_T("\\desktop"), &element);	
	m_pWnd_Rank->SetInvisible(TRUE);
	m_pWnd_Rank->Align();
	m_pLB_RankInfo = (GUIListBox*)m_pWnd_Rank->GetChild(_T("pvplistpic\\listpic\\ptlist"));
	m_pBtn_NameSort = (GUIButton*)m_pWnd_Rank->GetChild(_T("pvplistpic\\listpic\\pname"));
	m_pBtn_Score = (GUIButton*)m_pWnd_Rank->GetChild(_T("pvplistpic\\listpic\\num"));
	m_pBtn_Cancel = (GUIButton*)m_pWnd_Rank->GetChild(_T("pvplistpic\\closebutt"));

	if( m_dwActivityID>=14 && m_dwActivityID <=17 )
	{
		m_pLB_RankInfo->SetColWidth(0, 116);
		m_pLB_RankInfo->SetColWidth(1, 62);
	}
	else if( m_dwActivityID>=18 && m_dwActivityID <=21 )
	{
		m_pBtn_TeamID = (GUIButton*)m_pWnd_Rank->GetChild(_T("t_pvplistpic\\t_listpic\\teamid"));
		m_pLB_RankInfo->SetColWidth(0, 44);
		m_pLB_RankInfo->SetColWidth(1, 115);
		m_pLB_RankInfo->SetColWidth(2, 62);
	}

	//注册事件处理函数
	m_pGUI->RegisterEventHandler(m_pWnd_Self->GetFullName().c_str(), m_Trunk.sfp1(&ActivityPvPFrame::EventHandler));
	m_pGUI->RegisterEventHandler(m_pWnd_Rank->GetFullName().c_str(), m_Trunk.sfp1(&ActivityPvPFrame::EventHandler));

	return TRUE;
}

VOID ActivityPvPFrame::Update()
{

}

BOOL ActivityPvPFrame::EventHandler( tagGUIEvent* pEvent )
{
	GUIWnd* pWnd = m_pGUI->GetWnd(pEvent->dwWndID);
	if( !P_VALID(pWnd) )
		return FALSE;
	switch(pEvent->eEvent)
	{
	case EGUIE_Click:
		{
			//排行查询
			if( pWnd == m_pBtn_Rank )
			{
				SendQueryScore();
			}
			//离开PvP活动
			else if( pWnd == m_pBtn_Exit )
			{
				//通知服务器离开活动
				SendQuitActivity();
			}
			//排行关闭
			else if( pWnd == m_pBtn_Cancel )
			{
				m_pWnd_Rank->SetInvisible(TRUE);
			}
		}
		break;
	}

	return TRUE;
}

void ActivityPvPFrame::SendQueryScore()
{
	tagNC_DefaultRequest msg;
	msg.eDlgTarget = EMUT_Request_Activity;
	msg.dwTargetID = m_dwActivityID;
	m_pSession->Send(&msg);
}
void ActivityPvPFrame::SendQuitActivity()
{
	tagNC_LeaveInstance msg;
	m_pSession->Send(&msg);
	//ActivityPvPMgr::Inst()->ClearPvPUI();
}


void ActivityPvPFrame::RefreshScoreUI()
{
	//清空
	m_pLB_RankInfo->Clear();
	TCHAR szBuff[X_SHORT_NAME] = {0};
	//设置
	if( ActivityPvPMgr::Inst()->GetActivityType() == E_PvP )
	{
		vector<tagPvPHero>& vecHero = ActivityPvPMgr::Inst()->GetPvPActivityScoreSort();
		vector<tagPvPHero>::iterator it = vecHero.begin();
		INT nRow = 0;
		for( ; it!=vecHero.end(); ++it ) 
		{
			m_pLB_RankInfo->SetText( nRow, 0, it->strRoleName.c_str() );
			_itot( it->nScore, szBuff, 10 );
			m_pLB_RankInfo->SetText( nRow, 1,  szBuff);
			++nRow;
		}
	}
	else if( ActivityPvPMgr::Inst()->GetActivityType() == E_PvPMulti )
	{
		vector<tagPvPMultiHero>& vecHero = ActivityPvPMgr::Inst()->GetPvPMultiActivityScoreSort();
		vector<tagPvPMultiHero>::iterator it = vecHero.begin();
		INT nRow = 0;
		for( ; it!=vecHero.end(); ++it )
		{
			_itot( it->dwTeamID, szBuff, 10 );
			m_pLB_RankInfo->SetText( nRow, 0, szBuff );
			m_pLB_RankInfo->SetText( nRow, 1, it->strTeamLeaderName.c_str() );
			_itot( it->nTeamScore, szBuff, 10 );
			m_pLB_RankInfo->SetText( nRow, 2, szBuff );
			++nRow;
		}
	}
	m_pWnd_Rank->SetInvisible(FALSE);
	m_pWnd_Rank->Align();
}

void ActivityPvPFrame::RefreshSelfUI()
{
	if( m_dwActivityID>=14 && m_dwActivityID <=17 )
	{
		tstringstream strInfo;
		tstring strTeam = ActivityPvPMgr::Inst()->GetActivityName();
		if( !strTeam.empty() )
		{
			strInfo << g_StrTable[_T("PvP_Activity0")] << strTeam << _T("\n");
		}
		DWORD dwScore = ActivityPvPMgr::Inst()->GetActivityScore();
		if( dwScore!=GT_INVALID )
		{
			strInfo << g_StrTable[_T("PvP_Activity1")] << (INT)dwScore << _T("\n");
		}
		DWORD dwReborn = ActivityPvPMgr::Inst()->GetActivityRebornTime();
		if( dwReborn!=0 && dwReborn!=GT_INVALID )
		{
			strInfo << g_StrTable[_T("PvP_Activity2")] << dwReborn << _T("\n");
		}
		m_pStc_Info->SetText(strInfo.str().c_str());
		strInfo.clear();
	}
	else if( m_dwActivityID>=18 && m_dwActivityID <=21 )
	{
		tstringstream strInfo;
		tstring strTeam = ActivityPvPMgr::Inst()->GetActivityName();
		if( !strTeam.empty() )
		{
			strInfo << g_StrTable[_T("PvP_Activity0")] << strTeam << _T("\n");
		}
		DWORD dwTeamID = ActivityPvPMgr::Inst()->GetActivityTeamID();
		if( dwTeamID!=GT_INVALID )
		{
			strInfo << g_StrTable[_T("PvP_Activity3")] << dwTeamID << _T("\n");
		}
		DWORD dwScore = ActivityPvPMgr::Inst()->GetActivityScore();
		if( dwScore!=GT_INVALID )
		{
			strInfo << g_StrTable[_T("PvP_Activity4")] << (INT)dwScore << _T("\n");
		}
		DWORD dwReborn = ActivityPvPMgr::Inst()->GetActivityRebornTime();
		if( dwReborn!=0 && dwReborn!=GT_INVALID )
		{
			strInfo << g_StrTable[_T("PvP_Activity2")] << dwReborn << _T("\n");
		}
		m_pStc_Info->SetText(strInfo.str().c_str());
		strInfo.clear();
	}
	m_pWnd_Self->SetRefresh(TRUE);
}
