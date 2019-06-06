#include "stdafx.h"
#include "InstanceQuitFrame.h"
#include "InstanceMgr.h"

InstanceQuitFrame::InstanceQuitFrame()
	: m_Trunk(this)
	, m_pWnd(NULL)
	, m_pWndBack(NULL)
	, m_pWndText(NULL)
	, m_pButtonQuit(NULL)
	, m_pButtonCancel(NULL)
	, m_dwStartTime(0)
{

}

InstanceQuitFrame::~InstanceQuitFrame()
{

}

BOOL InstanceQuitFrame::Init(GameFrameMgr *pMgr, GameFrame *pFather, DWORD dwParam)
{
	if (!GameFrame::Init(pMgr, pFather, dwParam))
		return FALSE;
	m_dwStartTime = timeGetTime();
	//m_pGUI->SetModal( m_pWnd );
	return TRUE;

}

BOOL InstanceQuitFrame::Destroy()
{
	GameFrame::Destroy();

	m_pWndText = NULL;
	if( P_VALID(m_pWnd) )
		m_pGUI->DestroyWnd(m_pWnd);

	m_pGUI->SetModal( NULL );

	return TRUE;
}

BOOL InstanceQuitFrame::ReloadUI()
{
	XmlElement element;
	tstring strPath = g_strLocalPath + _T("\\ui\\msgbox.xml");
	m_pGUI->LoadXml(&element, "VFS_System", strPath.c_str() );
	m_pWnd = m_pGUI->CreateWnd(_T("\\desktop"), &element);

	m_pGUI->RegisterEventHandler(m_pWnd->GetFullName().c_str(), m_Trunk.sfp1(&InstanceQuitFrame::EventHandler));
	m_pWndBack = (GUIPatch*)m_pWnd->GetChild(_T("back"));
	m_pWndText = (GUIStaticEx*)m_pWndBack->GetChild(_T("textback\\text"));
	m_pButtonQuit = (GUIButton*)m_pWndBack->GetChild(_T("ok"));
	m_pButtonCancel	= (GUIButton*)m_pWndBack->GetChild(_T("cancel"));
	m_pGUI->SetActive( m_pWnd );

	return TRUE;

}

VOID InstanceQuitFrame::Update()
{
	GameFrame::Update();
	DWORD dwRunTime = timeGetTime() - m_dwStartTime;
	if(dwRunTime > QUIT_INSTANCE_TIME)
	{
		InstanceMgr::Inst()->Send_RequestLeaveInstance();
		m_pFrameMgr->AddToDestroyList(this);
		return;
	}

	TCHAR szBuff[128];
	_stprintf( szBuff, g_StrTable[_T("Instance_Quit_Time")], (QUIT_INSTANCE_TIME - dwRunTime)/1000 + 1 );

	m_pWndText->SetText(szBuff);
}

BOOL InstanceQuitFrame::EscCancel()
{
	m_pFrameMgr->AddToDestroyList(this);
	return TRUE;
}

VOID InstanceQuitFrame::SetText(LPCTSTR szText)
{
	 if(P_VALID(m_pWndText))
		 m_pWndText->SetText(szText);
}

BOOL InstanceQuitFrame::EventHandler(tagGUIEvent *pEvent)
{
	GUIWnd* pWnd = m_pGUI->GetWnd(pEvent->dwWndID);
	if( !P_VALID(pWnd) )
		return FALSE;
	switch( pEvent->eEvent )
	{
	case EGUIE_Click:
		{
			if (pWnd == m_pButtonQuit)
			{
				InstanceMgr::Inst()->Send_RequestLeaveInstance();
				m_pFrameMgr->AddToDestroyList(this);
			}
			if (pWnd == m_pButtonCancel)
			{
				m_pFrameMgr->AddToDestroyList(this);
			}
		} break;
	case EGUIE_KeyDown:
		switch( pEvent->msg.dwParam1 )
		{
		case VK_RETURN:
			InstanceMgr::Inst()->Send_RequestLeaveInstance();
			m_pFrameMgr->AddToDestroyList(this);
			break;
		} break;
	}
	return FALSE;
}




