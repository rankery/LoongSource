//-----------------------------------------------------------------------------
//!\file GmConsole.cpp
//!\author Lyp
//!
//!\date 2008-02-17
//! last 2008-04-15
//!
//!\brief gm console frame
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "GmConsole.h"

#include "PlayerNameTab.h"
#include "WorldPickerFrame.h"
#include "Role.h"
#include "RoleMgr.h"
#include "LocalPlayer.h"

//-----------------------------------------------------------------------------
//! construction
//-----------------------------------------------------------------------------
GmConsoleFrame::GmConsoleFrame():m_Trunk(this)
{
	m_pWnd = NULL;
	m_pWndText = NULL;
	m_dwCurShowRoleID = GT_INVALID;
}


//-----------------------------------------------------------------------------
//! destruction
//-----------------------------------------------------------------------------
GmConsoleFrame::~GmConsoleFrame()
{

}


//-----------------------------------------------------------------------------
// init
//-----------------------------------------------------------------------------
BOOL GmConsoleFrame::Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam)
{
	m_pCommander = TObjRef<Console>()->GetCommand();
	if( !P_VALID(m_pCommander) )
		return FALSE;

	if( !GameFrame::Init(pMgr, pFather, dwParam) )
		return FALSE;

	m_pInput->Register(0, g_StrTable[_T("Hotkey_GmConsole")], (INPUTMAPEVENTHANDLE)
		m_Trunk.sfp2(&GmConsoleFrame::Switch), DIK_INSERT, TRUE, TRUE, 1);

	m_pMgr->RegisterEventHandle( _T("tagRolePickEvent"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&GmConsoleFrame::OnRolePickEvent));
	m_pMgr->RegisterEventHandle( _T("tagRoleGetNameEvent"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&GmConsoleFrame::OnRoleGetNameEvent));

	return TRUE;
}

//-----------------------------------------------------------------------------
// destroy
//-----------------------------------------------------------------------------
BOOL GmConsoleFrame::Destroy()
{
	m_pMgr->UnRegisterEventHandler( _T("tagRolePickEvent"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&GmConsoleFrame::OnRolePickEvent));
	m_pMgr->UnRegisterEventHandler( _T("tagRoleGetNameEvent"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&GmConsoleFrame::OnRoleGetNameEvent));

	GameFrame::Destroy();

	m_pWndText = NULL;
	if( P_VALID(m_pWnd) )
		m_pGUI->AddToDestroyList(m_pWnd);

	return TRUE;
}



//-----------------------------------------------------------------------------
//! 事件处理
//! 
//-----------------------------------------------------------------------------
BOOL GmConsoleFrame::EventHandler(tagGUIEvent* pEvent)
{
	GUIWnd* pWnd = m_pGUI->GetWnd(pEvent->dwWndID);
	if( !P_VALID(pWnd) )
		return FALSE;

	tstring strCommand = _T("gm ");
	switch( pEvent->eEvent )
	{
	case EGUIE_EditBoxEnter:
		if( pWnd == m_pWndCommand )
		{
			strCommand += m_pWndCommand->GetText();
			m_pCommander->Excute(strCommand.c_str());
			m_pWndCommand->SetText(_T(""));
		}
		break;

	case EGUIE_Click:
		if( pWnd == m_pWndSubmit )
		{
			strCommand += m_pWndCommand->GetText();
			m_pCommander->Excute(strCommand.c_str());
			m_pWndCommand->SetText(_T(""));
		}
		else if( pWnd == m_pWndKick )
		{
			strCommand += _T("kick ");
			strCommand += m_pWndEditKick->GetText();
			m_pCommander->Excute(strCommand.c_str());
		}
		else if( pWnd == m_pWndMoveRole )
		{
			strCommand += _T("moverole ");
			strCommand += m_pWndEditKick->GetText();
			m_pCommander->Excute(strCommand.c_str());
		}
		else if( pWnd == m_pWndMoveToRole )
		{
			strCommand += _T("gotorole ");
			strCommand += m_pWndEditKick->GetText();
			m_pCommander->Excute(strCommand.c_str());
		}
		else if( pWnd == m_pWndKill )
		{
			strCommand += _T("kill ");
			strCommand += m_pWndEditKick->GetText();
			m_pCommander->Excute(strCommand.c_str());
		}
		else if( pWnd == m_pWndClearBag )
		{
			strCommand += _T("clearbag");
			m_pCommander->Excute(strCommand.c_str());
		}
		else if( pWnd == m_pWndReDrop )
		{
			Vector3 vec = RoleMgr::Inst()->GetLocalPlayer()->GetPos();
			INT X = (INT)(vec.x / 50.0f);
			INT Z = (INT)(vec.z / 50.0f);
			tstringstream stream;
			stream << _T("gm goto 0 ") << X << _T(" ") << Z;
			m_pCommander->Excute(stream.str().c_str());
		}
	}

	return FALSE;
}



//-----------------------------------------------------------------------------
// init
//-----------------------------------------------------------------------------
BOOL GmConsoleFrame::ReloadUI()
{
	XmlElement element;

	tstring strPath = g_strLocalPath + _T("\\ui\\gm_console.xml");
	m_pGUI->LoadXml(&element, "VFS_System", strPath.c_str());

	// 创建
	m_pWnd = m_pGUI->CreateWnd(_T("\\desktop"), &element);

	// 注册事件处理函数
	m_pGUI->RegisterEventHandler(m_pWnd->GetFullName().c_str(), m_Trunk.sfp1(&GmConsoleFrame::EventHandler));

	m_pWndCaption = (GUIPatch*)m_pWnd->GetChild(_T("panel"));
	m_pWndSubmit = m_pWndCaption->GetChild(_T("submit"));
	m_pWndCommand = (GUIEditBox*)m_pWndCaption->GetChild(_T("frame\\command"));

	m_pWndEditKick = (GUIEditBox*)m_pWndCaption->GetChild(_T("frame0\\frame\\command"));
	m_pWndEditMoveRole = (GUIEditBox*)m_pWndCaption->GetChild(_T("frame0\\frame0\\command"));
	m_pWndEditMoveToRole = (GUIEditBox*)m_pWndCaption->GetChild(_T("frame0\\frame00\\command"));
	m_pWndEditKill = (GUIEditBox*)m_pWndCaption->GetChild(_T("frame0\\frame000\\command"));

	m_pWndKick = m_pWndCaption->GetChild(_T("frame0\\kick"));
	m_pWndMoveRole = m_pWndCaption->GetChild(_T("frame0\\moverole"));
	m_pWndMoveToRole = m_pWndCaption->GetChild(_T("frame0\\movetorole"));
	m_pWndKill = m_pWndCaption->GetChild(_T("frame0\\kill"));
	m_pWndClearBag = m_pWndCaption->GetChild(_T("frame0\\clearbag"));
	m_pWndReDrop = m_pWndCaption->GetChild(_T("frame0\\redrop"));
	m_pWndInit = m_pWndCaption->GetChild(_T("frame0\\init"));


//	m_pWndText = (GUIStatic*)m_pWnd->GetChild(_T("text"));
//	m_pWndText->Align();

	m_pWnd->SetInvisible(TRUE);

	return TRUE;
}


//-----------------------------------------------------------------------------
// update
//-----------------------------------------------------------------------------
VOID GmConsoleFrame::Update()
{
	GameFrame::Update();

}


//-----------------------------------------------------------------------------
// 控制台开关
//-----------------------------------------------------------------------------
DWORD GmConsoleFrame::Switch(DWORD dwID, BOOL bDown)
{
	if( bDown )
	{
		if(P_VALID(m_pWnd)) 
			m_pWnd->SetInvisible(!m_pWnd->IsInvisible()); 

		if( !m_pWnd->IsInvisible() )
			m_pGUI->SetActive(m_pWndCommand);
	}
	return 0;
}


//-----------------------------------------------------------------------------
// OnRolePickEvent
//-----------------------------------------------------------------------------
VOID GmConsoleFrame::OnRolePickEvent(tagRolePickEvent* pEvent)
{
	if( GT_INVALID != pEvent->dwRoleID )
	{
		m_dwCurShowRoleID = pEvent->dwRoleID;
		Role* pRole = RoleMgr::Inst()->FindRole( m_dwCurShowRoleID );
		if( !P_VALID( pRole ) || ( !pRole->IsAttributeInited() ) )
			return;

		tstring strName = pRole->GetRoleName();

		m_pWndEditKick->SetText( strName.c_str() );
		m_pWndEditMoveRole->SetText( strName.c_str() );
		m_pWndEditMoveToRole->SetText( strName.c_str() );
		m_pWndEditKill->SetText( strName.c_str() );
	}

}


//-----------------------------------------------------------------------------
// OnRoleGetNameEvent
//-----------------------------------------------------------------------------
VOID GmConsoleFrame::OnRoleGetNameEvent(tagRoleGetNameEvent* pEvent)
{
	if( GT_INVALID != pEvent->dwRoleID && m_dwCurShowRoleID == pEvent->dwRoleID )
	{
		m_pWndEditKick->SetText( pEvent->strRoleName.c_str() );
		m_pWndEditMoveRole->SetText( pEvent->strRoleName.c_str() );
		m_pWndEditMoveToRole->SetText( pEvent->strRoleName.c_str() );
		m_pWndEditKill->SetText( pEvent->strRoleName.c_str() );
	}
}