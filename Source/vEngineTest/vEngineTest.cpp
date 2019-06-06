//-----------------------------------------------------------------------------
//!\file vEngineTest.cpp
//!\author Lyp
//!
//!\date 2004-08-11
//! last 2007-10-11
//!
//!\brief vengine测试程序
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "vEngineTest.h"

//-----------------------------------------------------------------------------
// init
//-----------------------------------------------------------------------------
BOOL CTest::Init(HINSTANCE hInst)
{
	m_pUtil->Randomize(timeGetTime());
	m_render.Init(m_pRender);	// 与vEngine中默认启动的render引擎连接
	m_pWindow->Init(&m_render);
	m_pWindow->CreateWnd(_T("gui_test"), hInst);

	if( !m_pRender->Init(m_pWindow->GetWidth(), m_pWindow->GetHeight()) )
		return FALSE;

	m_pGUI->Init(m_pWindow->GetWidth(), m_pWindow->GetHeight(), &m_render);	
	m_pInput->Init();
	m_pConsole->Init(NULL);
	

	// 注册几个控制台命令
	m_pConsole->Register(_T("t1"), m_Trunk.sfp1(&CTest::Test1), _T("测试1"), 1);
	m_pConsole->Register(_T("t2"), m_Trunk.sfp1(&CTest::Test2), _T("测试2"), 1);
	m_pConsole->Register(_T("t3"), m_Trunk.sfp1(&CTest::Test3), _T("测试3"), 1);

	return TRUE;
}


//-----------------------------------------------------------------------------
// destroy
//-----------------------------------------------------------------------------
VOID CTest::Destroy()
{
	m_pWindow->Destroy();
	m_pGUI->Destroy();
}


//-----------------------------------------------------------------------------
// Mainloop
//-----------------------------------------------------------------------------
VOID CTest::Mainloop(LPCTSTR szText)
{
// 	int* p = 0;
// 	*p = 0;

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
			m_pGUI->OnWindowsMsg(dwMsg, dwParam1, dwParam2);
			if( dwMsg == WM_QUIT )
				return;
		}

		m_pInput->Update();
		m_pInputMap->Update();
		m_pGUI->Update();
		m_pLuaMgr->Update((FLOAT)dwDeltaTime/1000);

		// render
		m_pRender->Clear(0, 0, 0);
		m_pGUI->Render();
		m_pRender->Render();
	}
}


//-----------------------------------------------------------------------------
// Test1
//-----------------------------------------------------------------------------
DWORD CTest::Test1( LPCTSTR szParam1 )
{
	DWORD dwTime = timeGetTime();
/*
	IMSG(_T("0x%X\r\n"), m_pUtil->Crc32(_T("size_y 4003001")));
	IMSG(_T("0x%X\r\n"), m_pUtil->Crc32(_T("weapon_soul 1008061")));
	*/

	std::list<tstring> list0;
	m_pVarContainer->Load(NULL, _T("data\\system\\attdata\\skill_proto.xml"), "id", &list0);

	IMSG(_T("time:%lu\r\n"), timeGetTime() - dwTime);
	return 0;
}


//-----------------------------------------------------------------------------
// Test2
//-----------------------------------------------------------------------------
DWORD CTest::Test2( LPCTSTR szParam1 )
{
	MemPool pool;
	TList<LPVOID> list;
	LPVOID p = NULL;

	DWORD dwTime = timeGetTime();
	for(INT n=0; n<1000; n++)
	{
		for(INT m=0; m<30; m++)
		{
			DWORD dw = rand()*16;
			p = pool.Alloc(dw);
			list.PushBack(p);
		}
		
		while(!list.Empty())
		{
			p = list.PopFront();
			pool.Free(p);
		}
	}
	IMSG(_T("pool alloc time:%lu\r\n"), timeGetTime() - dwTime);
	IMSG(_T("pool size:%lu\r\n"), pool.GetSize());
	
	dwTime = timeGetTime();
	for(INT n=0; n<1000; n++)
	{
		for(INT m=0; m<30; m++)
		{
			DWORD dw = rand()*16;
			p = malloc(dw);
			list.PushBack(p);
		}

		while(!list.Empty())
		{
			p = list.PopFront();
			free(p);
		}
	}
	IMSG(_T("C alloc time:%lu\r\n"), timeGetTime() - dwTime);
	return 0;
}


//-----------------------------------------------------------------------------
// Test3
//-----------------------------------------------------------------------------
DWORD CTest::Test3( LPCTSTR szParam1 )
{
	// 创建测试线程
	_beginthreadex
		(NULL, 0, (THREADPROC)m_Trunk.sfp1(&CTest::Test3Thread), NULL, 0, NULL);
	return 0;
}


//-----------------------------------------------------------------------------
// Test3
//-----------------------------------------------------------------------------
DWORD CTest::Test3Thread( LPCTSTR szParam1 )
{
	DWORD dwTime = timeGetTime();

	std::set<INT> set;
	for(INT n=0;n<10000;++n)
	{
		INT r = m_pUtil->RandomInRange(0, 3);
		IMSG(_T("time%d, num:%d\r\n"), n, r);

//		if( set.find(r) != set.end() )
//			break;

//		set.insert(r);
	}

	return 0;
}