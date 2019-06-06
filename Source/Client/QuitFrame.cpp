//-----------------------------------------------------------------------------
//!\file QuitFrame.cpp
//!\author Lyp
//!
//!\date 2008-08-17
//! last 2008-08-26
//!
//!\brief quit game frame
//!
//!	Copyright (c) 1985-2008 CTCRST Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "QuitFrame.h"
#include "WorldFrame.h"
#include "HackShield.h"
#include "..\WorldDefine\msg_return_roleselect.h"
#include "RoleMgr.h"
#include "SeleRoleMgr.h"
#include "CombatSysUtil.h"
#include "LoadingFrame.h"

#define QUIT_GAME_TIME (10*1000)	// 10秒离开游戏
//-----------------------------------------------------------------------------
//! construction
//-----------------------------------------------------------------------------
QuitFrame::QuitFrame():m_Trunk(this)
{
	m_pWnd = NULL;
	m_pWndBack = NULL;
	m_pWndText = NULL;
	m_pButtonQuit = NULL;
	m_pButtonCancel = NULL;
	m_dwStartTime = GT_INVALID;
	m_bSendMsg = false;
}


//-----------------------------------------------------------------------------
//! destruction
//-----------------------------------------------------------------------------
QuitFrame::~QuitFrame()
{
}


//-----------------------------------------------------------------------------
// init
//-----------------------------------------------------------------------------
BOOL QuitFrame::Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam)
{
	if( !GameFrame::Init(pMgr, pFather, dwParam) )
		return FALSE;

	m_eLogoutType = (ELogoutType)dwParam;
	switch( m_eLogoutType )
	{
	case ELOT_ResumeRoleSelect: 
		m_pWndBack->SetText( g_StrTable[_T("ResumeRoleSelect")] );
		break;
	case ELOT_ResumLogin: 
		m_pWndBack->SetText( g_StrTable[_T("ResumLogin")] );
		break;
	case ELOT_QuitGame:
		m_pWndBack->SetText( g_StrTable[_T("QuitGame")] );
		break;
	default:
		ASSERT( 0 );
		break;
	}

	m_dwStartTime = timeGetTime();
	m_pGUI->SetModal( m_pWnd );

	TObjRef<NetCmdMgr>()->Register("NS_ReturnRoleSelect", (NETMSGPROC)m_Trunk.sfp2(&QuitFrame::OnNS_ReturnRoleSelect), _T("NS_ReturnRoleSelect"));

	return TRUE;
}

//-----------------------------------------------------------------------------
// destroy
//-----------------------------------------------------------------------------
BOOL QuitFrame::Destroy()
{
	GameFrame::Destroy();

	m_pWndText = NULL;
	if( P_VALID(m_pWnd) )
		m_pGUI->DestroyWnd(m_pWnd);

	m_pGUI->SetModal( NULL );

	TObjRef<NetCmdMgr>()->UnRegister("NS_ReturnRoleSelect", (NETMSGPROC)m_Trunk.sfp2(&QuitFrame::OnNS_ReturnRoleSelect));

	return TRUE;
}



//-----------------------------------------------------------------------------
//! 事件处理
//! 
//-----------------------------------------------------------------------------
BOOL QuitFrame::EventHandler(tagGUIEvent* pEvent)
{
	GUIWnd* pWnd = m_pGUI->GetWnd(pEvent->dwWndID);
	if( !P_VALID(pWnd) )
		return FALSE;

	switch( pEvent->eEvent )
	{
	case EGUIE_Click:
		if( pWnd == m_pButtonQuit )
			this->Logout( m_eLogoutType );
		if( pWnd == m_pButtonCancel )
			m_pFrameMgr->AddToDestroyList(this);
		break;
	case EGUIE_KeyDown:
		switch( pEvent->msg.dwParam1 )
		{
		case VK_RETURN:
			this->Logout( m_eLogoutType );
			break;
		}
		break;
	}

	return FALSE;
}



//-----------------------------------------------------------------------------
// init
//-----------------------------------------------------------------------------
BOOL QuitFrame::ReloadUI()
{
	XmlElement element;
	tstring strPath = g_strLocalPath + _T("\\ui\\msgbox.xml");
	m_pGUI->LoadXml(&element, "VFS_System", strPath.c_str() );

	// 创建
	m_pWnd = m_pGUI->CreateWnd(_T("\\desktop"), &element);

	// 注册事件处理函数
	m_pGUI->RegisterEventHandler(m_pWnd->GetFullName().c_str(), m_Trunk.sfp1(&QuitFrame::EventHandler));
	m_pWndBack = (GUIPatch*)m_pWnd->GetChild(_T("back"));
	m_pWndText = (GUIStaticEx*)m_pWndBack->GetChild(_T("textback\\text"));
	m_pButtonQuit = (GUIButton*)m_pWndBack->GetChild(_T("ok"));
	m_pButtonCancel	= (GUIButton*)m_pWndBack->GetChild(_T("cancel"));
	m_pGUI->SetActive( m_pWnd );

	return TRUE;
}


//-----------------------------------------------------------------------------
// update
//-----------------------------------------------------------------------------
VOID QuitFrame::Update()
{
	GameFrame::Update();

	DWORD dwRunTime = timeGetTime() - m_dwStartTime;
	if( dwRunTime>QUIT_GAME_TIME && !m_bSendMsg )
	{
		//this->Logout( m_eLogoutType );
        tagGUIEvent event(m_pButtonQuit,EGUIE_Click);
        m_bSendMsg=TRUE;
        m_pButtonQuit->SendEvent(&event);

		return;
	}

	if(!m_bSendMsg)
	{
		TCHAR szBuff[128];
		_stprintf( szBuff, g_StrTable[_T("Logout_Text")], (QUIT_GAME_TIME - dwRunTime)/1000 + 1 );
		m_pWndText->SetText(szBuff);
	}
}

BOOL QuitFrame::EscCancel()
{
	m_pFrameMgr->AddToDestroyList( this );
	return TRUE;
}

//-----------------------------------------------------------------------------
// Logout
//-----------------------------------------------------------------------------
VOID QuitFrame::Logout( const ELogoutType eLogoutTye )
{
	switch( eLogoutTye )
	{
	case ELOT_ResumeRoleSelect:
		{
			tagNC_ReturnRoleSelect msg;
			TObjRef<NetSession>()->Send(&msg);
			m_bSendMsg = true;
			return ;
		}
		break;
	case ELOT_ResumLogin:
		{
			if( !m_pMgr->CreateFrame(_T("Root"), _T("Login"), _T("LoginFrame"), 0) )
				return;

			TObjRef<NetSession>()->Disconnect();
			HackShield::Inst()->StopApexClient();
			m_pMgr->AddToDestroyList( GetWorldFrm() );
		}
		break;
	case ELOT_QuitGame:
		{
			TObjRef<NetSession>()->Disconnect();
			HackShield::Inst()->StopApexClient();
			PostQuitMessage( 0 );
		}
		break;
	}
	m_bSendMsg = true;
	m_pFrameMgr->AddToDestroyList(this);
}

DWORD QuitFrame::OnNS_ReturnRoleSelect( tagNS_ReturnRoleSelect* pMsg, DWORD dwParam )
{
	switch( pMsg->dwErrorCode )
	{
	case E_Success:
		{
			//先清空下消息队列的无用消息
			TObjRef<NetSession>()->ClearMsgQueue();
			SeleRoleMgr::Inst()->Init();
			m_pMgr->CreateFrame(_T("Root"), _T("SelectRole"), _T("SelectRoleFrame"), 0);

			LoadingFrame* pFrame1 = (LoadingFrame*)m_pFrameMgr->CreateFrame(_T("Root"), _T("Loading"), _T("LoadingFrame"), 0);
			if( P_VALID(pFrame1) )
				pFrame1->SetLoadingType(LoadingFrame::ELT_SelectRole);

			m_pMgr->DestroyFrame( GetWorldFrm() );
		}
		break;
	case GT_INVALID:
		{	
			CombatSysUtil::Inst()->ShowScreenCenterMsg( g_StrTable[_T("Return_SelectROleError")] );
		}
		break;
	}

	m_pFrameMgr->AddToDestroyList(this);
	return 0;
}
