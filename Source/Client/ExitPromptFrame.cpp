#include "StdAfx.h"
#include "ExitPromptFrame.h"
#include "SysOptionMenuFrame.h"
#include "QuitFrame.h"
#include "RoleMgr.h"
#include "CombatSysUtil.h"
#include "LocalPlayer.h"
#include "QuestMgr.h"
#include "MapMgr.h"
#include "CombatEvent.h"
#include "../WorldDefine/msg_player_preventlost.h"

ExitPromptFrame::ExitPromptFrame(void):
m_Trunk(this),
m_pWnd( NULL ),
m_pBtnContinue( NULL ),
m_pBtnQuit( NULL ),
m_pBtnClose( NULL ),
m_dwTime(0)
{
	memset( m_stcPrompts, 0, sizeof(m_stcPrompts) );
}


ExitPromptFrame::~ExitPromptFrame(void)
{
}


BOOL ExitPromptFrame::Init( GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam )
{
	if( !GameFrame::Init(pMgr, pFather, dwParam) )
		return FALSE;

	m_pCmdMgr = TObjRef<NetCmdMgr>();
	m_pSession = TObjRef<NetSession>();
	m_pFrameMgr = TObjRef<GameFrameMgr>();

	//注册网络事件，从服务器获得要提示哪些内容
	m_pCmdMgr->Register( "NS_ExitGamePrompt", 
						(NETMSGPROC)m_Trunk.sfp2(&ExitPromptFrame::OnNS_ExitGamePrompt),
						_T("NS_ExitGamePrompt") );

	//发送退出游戏请求
	tagNC_ExitGamePrompt evt;
	m_pSession->Send( &evt );

	m_dwTime = timeGetTime();
	return TRUE;
}


BOOL ExitPromptFrame::Destroy()
{
	GameFrame::Destroy();

	m_pCmdMgr->UnRegister( "NS_ExitGamePrompt", 
		(NETMSGPROC)m_Trunk.sfp2(&ExitPromptFrame::OnNS_ExitGamePrompt) );

	SafeDestroyUI();

	return TRUE;
}

BOOL ExitPromptFrame::ReloadUI()
{
	SafeDestroyUI();

	// 创建
	XmlElement element;
	tstring strPath = g_strLocalPath + _T("\\ui\\exit_tips.xml");
	if( !m_pGUI->LoadXml( &element, "VFS_System", strPath.c_str() ) )
		return FALSE;

	//m_pGUI->ChangeXml( &element, "exittips_win\\exittips_pic\\back\\word1", "ClassName", "GUIStaticEx");
	
	m_pWnd = m_pGUI->CreateWnd( _T("\\desktop"), &element );
	m_stcPrompts[0] = (GUIStaticEx*)m_pWnd->GetChild( _T("exittips_pic\\back\\word1") );
	m_stcPrompts[1] = (GUIStaticEx*)m_pWnd->GetChild( _T("exittips_pic\\back\\word2") );
	m_pBtnContinue = (GUIButton*)m_pWnd->GetChild( _T("exittips_pic\\cancle") );
	m_pBtnQuit = (GUIButton*)m_pWnd->GetChild( _T("exittips_pic\\surebutt") );
	m_pBtnClose = (GUIButton*)m_pWnd->GetChild( _T("exittips_pic\\close") );
	
	// 注册事件处理函数
	m_pGUI->RegisterEventHandler(m_pWnd->GetFullName().c_str(), m_Trunk.sfp1(&ExitPromptFrame::GUIEventHandler));

	m_pWnd->SetInvisible( TRUE );
	return TRUE;
}

VOID ExitPromptFrame::Update()
{
	if( !P_VALID(m_pSession) || !m_pSession->IsConnect() )
	{
		m_pMgr->AddToDestroyList( this );
		return;
	}

	//如果用户点击退出，800毫秒内没有收到服务器的返回来的需要显示的信息，弹出倒计时窗口
	if( m_pWnd->IsInvisible() && (timeGetTime() - m_dwTime) > 900 )
	{
		SysOptionMenuFrame* pSysOptMenuFrame = (SysOptionMenuFrame*)(m_pMgr->GetFrame(_T("SysOptionMenu")));
		if( P_VALID( pSysOptMenuFrame) )
			m_pMgr->AddToDestroyList( pSysOptMenuFrame );

		m_pMgr->AddToDestroyList( this );

		OnLogOut( QuitFrame::ELOT_QuitGame );
	}
}

DWORD ExitPromptFrame::OnNS_ExitGamePrompt( tagNS_ExitGamePrompt* pNetCmd, DWORD dwParams )
{
	ZeroMemory( m_vecIdLinks, sizeof(m_vecIdLinks) );
	ZeroMemory( m_vecPosLinks,sizeof(m_vecPosLinks) );
	
	BOOL bHaveContent = FALSE;
	int index = 0;

	if( !pNetCmd->bDualExpQuestDo && P_VALID(m_stcPrompts[index]) )
	{
		tstring strDesc = g_StrTable[_T("PreventLost_DualExpKillMonster")];
		QuestMgr::Inst()->ParseQuestView( strDesc, m_vecIdLinks[index], m_vecPosLinks[index] );
		m_stcPrompts[index]->SetText( strDesc.c_str() );
		
		bHaveContent = TRUE;
		index++;
	}

	if( !pNetCmd->bFishQuestDo && P_VALID(m_stcPrompts[index]) )
	{
		TCHAR szInfo[X_LONG_NAME] = {0};
		_sntprintf(szInfo, X_LONG_NAME, g_StrTable[_T("PreventLost_CanDoFishToday")], pNetCmd->nExprence );

		tstring strDesc = szInfo;
		//strDesc += tstring( _T("\\n") );
		//strDesc += tstring( g_StrTable[_T("PreventLost_CanDoFishNPC")] );
		QuestMgr::Inst()->ParseQuestView( strDesc, m_vecIdLinks[index], m_vecPosLinks[index] );
		m_stcPrompts[index]->SetText( strDesc.c_str() );

		bHaveContent = TRUE;
		index++;
	}
	
	if( bHaveContent )							//有内容可以提示
	{
		m_pWnd->SetInvisible( FALSE );
		m_pWnd->SetTopMost( TRUE );
		m_pGUI->SetActive( m_pWnd );
		m_pWnd->SetRefresh( TRUE );
	}
	else //没有提示的内容，直接弹出倒计时提示框
	{
		SysOptionMenuFrame* pSysOptMenuFrame = (SysOptionMenuFrame*)(m_pMgr->GetFrame(_T("SysOptionMenu")));
		if( P_VALID( pSysOptMenuFrame) )
			m_pMgr->AddToDestroyList( pSysOptMenuFrame );
		OnLogOut( QuitFrame::ELOT_QuitGame );
	}

	return 0;
}

BOOL ExitPromptFrame::EscCancel()
{
	m_pMgr->AddToDestroyList( this );
	return TRUE;
}

VOID ExitPromptFrame::SafeDestroyUI()
{
	if( P_VALID( m_pWnd ) )
	{
		m_pGUI->DestroyWnd( m_pWnd );
		m_pWnd = NULL;
	}
}

BOOL ExitPromptFrame::GUIEventHandler( tagGUIEvent* pEvent )
{
	const GUIWnd* pWnd = m_pGUI->GetWnd( pEvent->dwWndID );
	
	if( !P_VALID(pWnd) )
		return FALSE;

	switch( pEvent->eEvent )
	{
	case EGUIE_Click:
		if( pWnd == m_pBtnClose )                        // 返回游戏
		{
			m_pMgr->AddToDestroyList( this );
		}
		else if( pWnd == m_pBtnContinue )
		{
			SysOptionMenuFrame* pSysOptMenuFrame = (SysOptionMenuFrame*)(m_pMgr->GetFrame(_T("SysOptionMenu")));
			if( P_VALID( pSysOptMenuFrame) )
				m_pMgr->AddToDestroyList( pSysOptMenuFrame );

			m_pMgr->AddToDestroyList( this );
		}
		else if( pWnd == m_pBtnQuit )
		{
			SysOptionMenuFrame* pSysOptMenuFrame = (SysOptionMenuFrame*)(m_pMgr->GetFrame(_T("SysOptionMenu")));
			if( P_VALID( pSysOptMenuFrame) )
				m_pMgr->AddToDestroyList( pSysOptMenuFrame );
			OnLogOut( QuitFrame::ELOT_QuitGame );
		}
		break;
	case EGUIE_StaticExLinkClick:
		{
			for( int i=0; i<2; i++ )
			{
				if( pWnd == m_stcPrompts[i] )
				{
					if( m_vecIdLinks[i].size() <=0 && m_vecPosLinks[i].size() <= 0 )
						return FALSE;
					Navigate( pEvent, i );
				}
			}
		}
		break;
	default:
		break;
	}

	return FALSE;
}


VOID ExitPromptFrame::OnLogOut( const int nLogType )
{
	if( RoleMgr::Inst()->GetLocalPlayer()->GetRoleState() & ERS_Combat )
	{
		CombatSysUtil::Inst()->ShowScreenCenterMsg( g_StrTable[_T("Logout_Err_InCombat")] );
	}
	else
	{
		GameFrame* pQuitFrame = m_pMgr->GetFrame( _T("Logout") );
		if( !P_VALID( pQuitFrame ) )
			m_pMgr->CreateFrame( _T("Root"), _T("Logout"), _T("QuitFrame"), nLogType );
	}
	m_pMgr->AddToDestroyList( this );
}


VOID ExitPromptFrame::Navigate( tagGUIEvent* pEvent,int nWndNum )
{
	if( !P_VALID(pEvent) )
		return;

	LPCTSTR szLink = (LPCTSTR)( pEvent->dwParam2 );
	Vector3 pos;

	//根据ID寻路
	if( (INT)pEvent->dwParam1 < 1000 )	
	{
		INT index = pEvent->dwParam1 - 1;
		if( index > (INT)m_vecIdLinks[nWndNum].size()-1 )
			return;

		tagIDLink& link = m_vecIdLinks[nWndNum][index];
		IMSG( _T("path map: %s typeid: %d\n"), link.mapName.c_str(), link.typeID );

		//不在本地图，进行跨场景寻路
		if( link.mapName != MapMgr::Inst()->GetCurMapName() )
		{
			if( QuestMgr::Inst()->MoveToMap( link.mapName.c_str(), pos ) != TRUE )
				return;
		}
		else
		{
			if( link.typeID == 1 )//触发器或者其他没有TypeID的东西
			{
				QuestData *pData = QuestMgr::Inst()->GetData();
				ASSERT( P_VALID(pData) );
				if( !pData->GetTriggerPosition( szLink, pos ) )
					return;
			}
			else
			{
				QuestQuery *pQuery = QuestMgr::Inst()->GetQuery();
				ASSERT( P_VALID(pQuery) );
				if( !pQuery->GetNPCPosition( link.typeID, pos ) )
					return;
			}
		}
	}
	else
	{
		INT index = pEvent->dwParam1-1000;
		if( index > (INT)(m_vecPosLinks[nWndNum].size()-1) )
			return;

		tagPosLink& link = m_vecPosLinks[nWndNum][index];

		//不在本地图，进行跨场景寻路
		if( link.mapName != MapMgr::Inst()->GetCurMapName() )
		{
			if( QuestMgr::Inst()->MoveToMap( link.mapName.c_str(),pos ) != TRUE )
				return;
		}
		else
		{
			pos = link.pos;
		}
	}

	LocalPlayer *pLP = RoleMgr::Inst()->GetLocalPlayer();
	tagMouseMoveEvent event( _T("tagMouseMoveEvent"), NULL );
	event.start = pLP->GetPos();
	event.end = pos;
	event.validDist = 100.0f;

	IMSG( _T("target pos: x-%f,y-%f,z-%f\n"),pos.x,pos.y,pos.z );
	m_pMgr->SendEvent( &event );
}