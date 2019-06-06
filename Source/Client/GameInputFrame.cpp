//-----------------------------------------------------------------------------
//!\file GameInputFrame.cpp
//!\author Lyp
//!
//!\date 2008-10-09
//! last 2008-10-09
//!
//!\brief game input Frame
//!
//!	Copyright (c) 1985-2008 CTCRST Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "GameInputFrame.h"


//-----------------------------------------------------------------------------
// init
//-----------------------------------------------------------------------------
BOOL GameInputFrame::Init( GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam )
{
	if( !GameFrame::Init(pMgr, pFather, dwParam) )
		return FALSE;

	m_pMgr->RegisterEventHandle(_T("CloseGameInput"), (FRAMEEVENTPROC)m_Trunk.sfp1(&GameInputFrame::OnEvent));


	return TRUE;
}

BOOL GameInputFrame::Destroy()
{
	m_pMgr->UnRegisterEventHandler(_T("CloseGameInput"), (FRAMEEVENTPROC)m_Trunk.sfp1(&GameInputFrame::OnEvent));
	GameFrame::Destroy();

	return TRUE;
}


BOOL GameInputFrame::ReloadUI()
{
	tagPoint pt(0,0);
	m_pInputMap->LoadGUI(NULL, pt);

	return TRUE;
}

VOID GameInputFrame::OnEvent( tagGameEvent* pEvent )
{
	m_pMgr->AddToDestroyList(this);
}


BOOL GameInputFrame::EscCancel()
{
	m_pInputMap->UnloadGUI();
	return TRUE;
}