//-----------------------------------------------------------------------------
//!\file vuieditor.cpp
//!\author Lyp
//!
//!\date 2004-08-11
//! last 2008-01-23
//!
//!\brief vengine gui editor
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "vuieditor.h"

#include "resource.h"
//-----------------------------------------------------------------------------
// init
//-----------------------------------------------------------------------------
BOOL CVUIEditor::Init(HINSTANCE hInst)
{
	// 读取菜单资源
	HMENU hMenu = ::LoadMenu(hInst, MAKEINTRESOURCE(IDC_UIEDITOR_MENU));

	m_render.Init(m_pRender);	// 与vEngine中默认启动的render引擎连接
	m_pWindow->Init(&m_render);
	m_pWindow->CreateWnd(_T("gui_test"), hInst, 0, hMenu);
	m_pRender->Init(m_pWindow->GetWidth(), m_pWindow->GetHeight());
	m_pRender->SetShowFPS(FALSE);

	m_pGUI->Init(m_pWindow->GetWidth(), m_pWindow->GetHeight(), &m_render);	
	m_pEditor->Init(m_pGUI);


	m_pGUI->LoadFont(_T("kaiti.ttf"));
	m_pGUI->LoadFont(_T("lishu.ttf"));
	m_pGUI->LoadFont(_T("hanyi.ttf"));
	m_pGUI->LoadFont(_T("MSYH.TTF"));
	m_pGUI->LoadFont(_T("msyhb.ttf"));

	//m_pConsole->Init(NULL);
	SetDefaultDir();
	return TRUE;
}


//-----------------------------------------------------------------------------
// destroy
//-----------------------------------------------------------------------------
VOID CVUIEditor::Destroy()
{
	m_pWindow->Destroy();
	m_pEditor->Destroy();
	m_pGUI->Destroy();
}


//-----------------------------------------------------------------------------
// Mainloop
//-----------------------------------------------------------------------------
VOID CVUIEditor::Mainloop()
{
	DWORD dwMsg, dwParam1, dwParam2;
	static DWORD dwTimeKeeper = timeGetTime();

	while( FALSE == m_pWindow->MsgLoop() )
	{
		if( FALSE == m_pWindow->IsWindowActive() )
		{
			Sleep(30);
			continue;
		}
		DWORD dwDeltaTime = timeGetTime() - dwTimeKeeper;
		dwTimeKeeper = timeGetTime();

		// update
		while( m_pWindow->PeekWindowsMsg(dwMsg, dwParam1, dwParam2) )
		{
			if( !m_pGUI->OnWindowsMsg(dwMsg, dwParam1, dwParam2) )	// 先给界面处理
				m_pEditor->OnWindowsMsg(dwMsg, dwParam1, dwParam2);	// 再给界面编辑器处理
		}
		m_pGUI->Update();

		// render
		m_pRender->Clear(0, 0, 0);
		m_pGUI->Render();
		m_pEditor->Render();
		m_pRender->Render();
	}
}



