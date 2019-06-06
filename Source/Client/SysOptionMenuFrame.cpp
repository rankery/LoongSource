#include "stdafx.h"
#include "SysOptionMenuFrame.h"
#include "SeleRoleMgr.h"
#include "WorldFrame.h"
#include "MapMgr.h"
#include "CombatSysUtil.h"
#include "QuitFrame.h"
#include "RoleMgr.h"
#include "Player.h"
#include "LocalPlayer.h"

SysOptionMenuFrame::SysOptionMenuFrame() :
m_Trunk( this ),
m_pWnd( NULL ),
m_pPanel( NULL ),
m_pBn_ResumeGame( NULL ),
m_pBn_GameSettings( NULL ),
m_pBn_InputSettings( NULL ),
m_pBn_ResumeRoleSelect( NULL ),
m_pBn_ResumeLogin( NULL ),
m_pBn_QuitGame( NULL )
{

}

SysOptionMenuFrame::~SysOptionMenuFrame()
{
}


//-----------------------------------------------------------------------------
// Init
//-----------------------------------------------------------------------------
BOOL SysOptionMenuFrame::Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam)
{
	if( !GameFrame::Init(pMgr, pFather, dwParam) )
		return FALSE;

	return TRUE;
}

//-----------------------------------------------------------------------------
// Destroy
//-----------------------------------------------------------------------------
BOOL SysOptionMenuFrame::Destroy()
{
	GameFrame::Destroy();

	SafeDestroyUI();

	return TRUE;
}


//-----------------------------------------------------------------------------
// ReloadUI
//-----------------------------------------------------------------------------
BOOL SysOptionMenuFrame::ReloadUI()
{
	SafeDestroyUI();

	// 创建
	tstring strPath = g_strLocalPath + _T("\\ui\\systemmenu.xml");
	m_pWnd = m_pGUI->CreateFromFile( "VFS_System", strPath.c_str() );

	// 注册事件处理函数
	m_pGUI->RegisterEventHandler(m_pWnd->GetFullName().c_str(), m_Trunk.sfp1(&SysOptionMenuFrame::GUIEventHandler));

	m_pPanel = m_pWnd->GetChild( _T("pic") );
	m_pBn_ResumeGame =		  (GUIButton*)m_pPanel->GetChild( _T("resumebutt") );
	m_pBn_GameSettings =      (GUIButton*)m_pPanel->GetChild( _T("setbutton") );
	m_pBn_InputSettings =     (GUIButton*)m_pPanel->GetChild( _T("hotkeybutt") );
	m_pBn_ResumeRoleSelect =  (GUIButton*)m_pPanel->GetChild( _T("backbutt") );
	m_pBn_ResumeLogin =       (GUIButton*)m_pPanel->GetChild( _T("restartbutt") );
	m_pBn_QuitGame =          (GUIButton*)m_pPanel->GetChild( _T("leavebutt") );

	//m_pBn_ResumeRoleSelect->SetEnable( false );
	m_pWnd->SetInvisible( FALSE );
	m_pWnd->SetTopMost( TRUE );
	m_pGUI->SetActive( m_pWnd );
	
	return TRUE;
}


BOOL SysOptionMenuFrame::EscCancel()
{
	m_pMgr->AddToDestroyList( this );
	return TRUE;
}

VOID SysOptionMenuFrame::SafeDestroyUI()
{
	if( P_VALID( m_pWnd ) )
	{
		m_pGUI->DestroyWnd( m_pWnd );
		m_pWnd = NULL;
	}
}

//-----------------------------------------------------------------------------
//! 系统选单事件处理
//! 
//-----------------------------------------------------------------------------
BOOL SysOptionMenuFrame::GUIEventHandler(tagGUIEvent* pEvent)
{
	const GUIWnd* pWnd = m_pGUI->GetWnd(pEvent->dwWndID);
	if( !P_VALID(pWnd) )
		return FALSE;

	switch( pEvent->eEvent )
	{
	case EGUIE_Click:
		if( pWnd == m_pBn_ResumeGame )                        // 返回游戏
		{
			m_pMgr->AddToDestroyList( this );
		}
		else if( pWnd == m_pBn_GameSettings )                 // 打开游戏设置界面
		{
			OnOpenGameSettings();
		}
		else if( pWnd == m_pBn_InputSettings )                // 打开自定义键位界面
		{
			OnOpenInputSettings();
		}
		else if( pWnd == m_pBn_ResumeRoleSelect )             // 返回角色选择
		{
			OnLogOut( QuitFrame::ELOT_ResumeRoleSelect );
		}
		else if( pWnd == m_pBn_ResumeLogin )                  // 返回帐号登陆
		{
			OnLogOut( QuitFrame::ELOT_ResumLogin );
		}
		else if( pWnd == m_pBn_QuitGame )                     // 离开游戏
		{
			if( RoleMgr::Inst()->GetLocalPlayer()->GetRoleLevel() < 10 )		//玩家小于10级不弹出提示框
			{
				OnLogOut( QuitFrame::ELOT_QuitGame );
			}
			else
			{
				GameFrame* pExitPromptFrame = m_pMgr->GetFrame( _T("ExitPromptFrame") );
				if( !P_VALID( pExitPromptFrame ) )
					m_pMgr->CreateFrame( _T("Root"), _T("ExitPromptFrame"), _T("ExitPromptFrame"), 0 );
				m_pMgr->AddToDestroyList( this );
			}
		}
		break;
	default:
		break;
	}

	return FALSE;
}

VOID SysOptionMenuFrame::OnOpenGameSettings()
{
	GameFrame* pFrame = m_pMgr->GetFrame( _T("GameSettings") );
	if( !P_VALID( pFrame ) )
		m_pMgr->CreateFrame( _T("World"), _T("GameSettings"), _T("GameSettingsFrame"), 0);

	m_pMgr->AddToDestroyList( this );
}

VOID SysOptionMenuFrame::OnOpenInputSettings()
{
	GameFrame* pFrame = m_pMgr->GetFrame( _T("GameInput") );
	if( !P_VALID( pFrame ) )
		m_pMgr->CreateFrame(_T("Root"), _T("GameInput"), _T("GameInputFrame"), 0);

	m_pMgr->AddToDestroyList( this );
}

VOID SysOptionMenuFrame::OnLogOut( const int nLogType )
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

