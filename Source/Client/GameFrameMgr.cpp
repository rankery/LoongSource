//-----------------------------------------------------------------------------
//!\file GameFrameMgr.cpp
//!\author Lyp
//!
//!\date 2008-02-17
//! last 2008-04-15
//!
//!\brief game frame mgr
//!
//!	Copyright (c) 1985-2008 CTCRST Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "GameFrameMgr.h"

//-----------------------------------------------------------------------------
//! construction
//-----------------------------------------------------------------------------
GameFrameMgr::GameFrameMgr()
{
}


//-----------------------------------------------------------------------------
//! destruction
//-----------------------------------------------------------------------------
GameFrameMgr::~GameFrameMgr()
{
}


//-----------------------------------------------------------------------------
// init
//-----------------------------------------------------------------------------
BOOL GameFrameMgr::Init()
{
	m_pRoot = new GameFrame;
	m_pRoot->Init(this, NULL, 0);
	m_pRoot->m_strName = _T("Root");

	DWORD dwID = m_pUtil->Crc32(_T("Root"));
	m_mapChild.Add(dwID, m_pRoot);

	return TRUE;
}

//-----------------------------------------------------------------------------
// destroy
//-----------------------------------------------------------------------------
BOOL GameFrameMgr::Destroy()
{
	m_pRoot->Destroy();
	SAFE_DEL(m_pRoot);
	return TRUE;
}


//-----------------------------------------------------------------------------
// Update
//-----------------------------------------------------------------------------
VOID GameFrameMgr::Update()
{
	// 处理destroy列表
	if( m_listDestroy.Size() > 0 )
	{
		GameFrame* pFrame;
		m_listDestroy.ResetIterator();
		while( m_listDestroy.PeekNext(pFrame) )
			this->DestroyFrame(pFrame);	// 真正删除控件

		m_listDestroy.Clear();
	}

	m_pRoot->Update();
}


//-----------------------------------------------------------------------------
// Render
//-----------------------------------------------------------------------------
VOID GameFrameMgr::Render3D()
{
	m_pRoot->Render3D();
}


//-----------------------------------------------------------------------------
// CreateFrame
//-----------------------------------------------------------------------------
GameFrame* GameFrameMgr::CreateFrame(LPCTSTR szFatherName, LPCTSTR szName, LPCTSTR szClass, 
									 DWORD dwParam)
{
	DWORD dwID = m_pUtil->Crc32(szName);
	DWORD dwFatherID = m_pUtil->Crc32(szFatherName);
	DWORD dwClassID = m_pUtil->Crc32(szClass);

	GameFrame* pFather = m_mapChild.Peek(dwFatherID);
	if( !P_VALID(pFather) )
	{
		ASSERT(0);
		IMSG(_T("Create frame:%s failed\r\n"), szName);
		return NULL;
	}

	if( m_mapChild.IsExist(dwID) )
	{
		ASSERT(0);
		IMSG(_T("Create frame:%s failed\r\n"), szName);
		return NULL;
	}

	GameFrame* pFrame = (GameFrame*)m_Factory.Create(dwClassID);
	pFrame->m_strName = szName;
	pFrame->m_dwClassID = dwClassID;
	m_mapChild.Add(dwID, pFrame);
	if( !pFrame->Init(this, pFather, dwParam) )
	{
		SAFE_DEL(pFrame);
		m_mapChild.Erase(dwID);
		IMSG(_T("Create frame:%s failed\r\n"), szName);
		return NULL;
	}
	
	pFather->AddChild(pFrame);
	return pFrame;
}



//-----------------------------------------------------------------------------
// 立即删除指定Frame
//-----------------------------------------------------------------------------
VOID GameFrameMgr::DestroyFrame(GameFrame* pFrame)
{
	// 从全局表中删除
	m_mapChild.Erase(m_pUtil->Crc32(pFrame->GetName().c_str()));

	// 调用自删除函数
	pFrame->Destroy();

	// 从内存中删除
	m_Factory.Destroy(pFrame, pFrame->m_dwClassID);
}



//-----------------------------------------------------------------------------
// 下一祯删除
//-----------------------------------------------------------------------------
BOOL GameFrameMgr::AddToDestroyList(GameFrame* pFrame)
{
	DWORD dwID = m_pUtil->Crc32(pFrame->GetName().c_str());
	if( !GT_VALID(m_mapChild.Peek(dwID)) )
		return FALSE;	// 不存在指定控件

	if( m_listDestroy.IsExist(pFrame) )
		return FALSE;	// 删除列表中已经存在则不重复添加

	m_listDestroy.PushBack(pFrame);

	// 从全局表中删除
	m_mapChild.Erase(m_pUtil->Crc32(pFrame->GetName().c_str()));

	return TRUE;
}


//-----------------------------------------------------------------------------
// 注册Frame事件处理函数
//-----------------------------------------------------------------------------
BOOL GameFrameMgr::RegisterEventHandle(LPCTSTR szEvent, FRAMEEVENTPROC fp)
{
	DWORD dwID = m_pUtil->Crc32(szEvent);
	tagEventHandle* pHandle = m_mapHandle.Peek(dwID);
	if( !P_VALID(pHandle) )
	{
		pHandle = new tagEventHandle;
		pHandle->strName = szEvent;
		m_mapHandle.Add(dwID, pHandle);
	}

	ASSERT( !pHandle->listFP.IsExist(fp) );
	pHandle->listFP.PushBack(fp);
	return TRUE;
}


//-----------------------------------------------------------------------------
// 注销事件处理函数
//-----------------------------------------------------------------------------
BOOL GameFrameMgr::UnRegisterEventHandler(LPCTSTR szEvent, FRAMEEVENTPROC fp)
{
	DWORD dwID = m_pUtil->Crc32(szEvent);
	tagEventHandle* pHandle = m_mapHandle.Peek(dwID);
	if( !P_VALID(pHandle) )
		return FALSE;

	pHandle->listFP.Erase(fp);
	if( pHandle->listFP.Size() <= 0 )
	{
		m_mapHandle.Erase(dwID);
		SAFE_DEL(pHandle);
	}

	return TRUE;
}


//-----------------------------------------------------------------------------
// 调用事件处理函数
//-----------------------------------------------------------------------------
VOID GameFrameMgr::SendEvent( tagGameEvent* pEvent )
{
	tagEventHandle* pHandle = m_mapHandle.Peek(pEvent->dwNameCrc);
	if( !P_VALID(pHandle) )
		return;

	pHandle->listFP.ResetIterator();
	FRAMEEVENTPROC fp = 0;
	while(pHandle->listFP.PeekNext(fp))
		fp(pEvent);
}