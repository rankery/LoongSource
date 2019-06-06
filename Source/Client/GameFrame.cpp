//-----------------------------------------------------------------------------
//!\file GameFrame.cpp
//!\author Lyp
//!
//!\date 2008-02-17
//! last 2008-04-15
//!
//!\brief game frame
//!
//!	Copyright (c) 1985-2008 CTCRST Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "GameFrame.h"


#include "GameFrameMgr.h"
//-----------------------------------------------------------------------------
//! construction
//-----------------------------------------------------------------------------
GameFrame::GameFrame()
{
}


//-----------------------------------------------------------------------------
//! destruction
//-----------------------------------------------------------------------------
GameFrame::~GameFrame()
{
}


//-----------------------------------------------------------------------------
// init
//-----------------------------------------------------------------------------
BOOL GameFrame::Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam)
{
	m_pMgr = pMgr;
	m_pFather = pFather;
	m_dwID = m_pUtil->Crc32(m_strName.c_str());

	return ReloadUI();
}


//-----------------------------------------------------------------------------
// destroy
//-----------------------------------------------------------------------------
BOOL GameFrame::Destroy()
{
	// 删除所有子窗口
	GameFrame* pFrame = NULL;
	m_listChild.ResetIterator();
	while(m_listChild.PeekNext(pFrame))
		m_pMgr->DestroyFrame(pFrame);

	// 通知父窗口删除自己
	if( m_pFather )
		m_pFather->DelChild(this);

	return TRUE;
}


//-----------------------------------------------------------------------------
// update
//-----------------------------------------------------------------------------
VOID GameFrame::Update()
{
	GameFrame* pFrame = NULL;
	m_listChild.ResetIterator();
	while(m_listChild.PeekNext(pFrame))
	{
		DWORD dwTime = timeGetTime();
		pFrame->Update();
		dwTime = timeGetTime() - dwTime;
		if( dwTime > 0 )
		{
			tstring strTemp = m_strName + _T("-update");
			m_pConsole->Watch(strTemp.c_str(), dwTime);
		}
	}
}



//-----------------------------------------------------------------------------
// render
//-----------------------------------------------------------------------------
VOID GameFrame::Render3D()
{
	GameFrame* pFrame = NULL;
	m_listChild.ResetIterator();
	while(m_listChild.PeekNext(pFrame))
	{
		DWORD dwTime = timeGetTime();
		pFrame->Render3D();
		dwTime = timeGetTime() - dwTime;
		if( dwTime > 0 )
		{
			tstring strTemp = m_strName + _T("-render3D");
			m_pConsole->Watch(strTemp.c_str(), dwTime);
		}
		
	}
}



//-----------------------------------------------------------------------------
// 重新读入UI界面
//-----------------------------------------------------------------------------
BOOL GameFrame::ReloadUI()
{

	return TRUE;
}