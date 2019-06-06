//-----------------------------------------------------------------------------
//!\file vServerTest.cpp
//!\author Lyp
//!
//!\date 2004-08-11
//! last 2007-10-11
//!
//!\brief vengine测试程序
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "vServerTest.h"

CONST INT SERVER_TEST_PORT = 7000;
//-----------------------------------------------------------------------------
// init
//-----------------------------------------------------------------------------
BOOL CServerTest::Init(HINSTANCE hInst)
{
	m_render.Init(m_pRender.operator->());	// 与vEngine中默认启动的render引擎连接
	//m_pWindow->Init(&m_render);
	m_pWindow->Init(&m_render,750,550,TRUE);
	m_pWindow->CreateWnd(_T("vServerTest"), hInst);

	if( !m_pRender->Init(m_pWindow->GetWidth(), m_pWindow->GetHeight()) )
		return FALSE;

	m_pGUI->Init(m_pWindow->GetWidth(), m_pWindow->GetHeight(), &m_render);	
	m_pRender->SetShowFPS(FALSE);

	m_pConsole->Init(NULL);
	m_dwIDHolder = 0;
	InitializeCriticalSectionAndSpinCount(&m_LockPlayerData, 4000);

	/*
	tagServerInitParam init_param;
	init_param.nPort = SERVER_TEST_PORT;
	init_param.SetLarge();
	init_param.nPerCpuWorkerThread = 2;
	init_param.nUnitPerWorkerThread = 50000;
	init_param.nAuthWaitTime = 5000;
	init_param.bReusePort = true;
	
	m_pServer->SetLogCallBack((LOGINCALLBACK)m_Trunk.sfp2(&CServerTest::LoginCallBack), 
		(LOGOUTCALLBACK)m_Trunk.sfp1(&CServerTest::LogoutCallBack) );
	m_pServer->Init(&init_param);*/

	/*
	m_pServer->SetLogCallBack((TCPLOGINCALLBACK)m_Trunk.sfp2(&CServerTest::LoginCallBack), 
		(TCPLOGOUTCALLBACK)m_Trunk.sfp1(&CServerTest::LogoutCallBack) );
	m_pServer->Init(SERVER_TEST_PORT);*/


	tagXServerConfig config;
	config.nPort = SERVER_TEST_PORT;
	config.bReusePort = true;
	config.dwPoolSize = 4*1024*1024;
	config.fnLogIn = (XLOGINCALLBACK)m_Trunk.sfp2(&CServerTest::LoginCallBack);
	config.fnLogOut = (XLOGOUTCALLBACK)m_Trunk.sfp1(&CServerTest::LogoutCallBack);
	m_pServer->Init(config);

	m_dwMinDelay = 9999999;
	m_dwMaxDelay = 0;
	m_dwAveDelay = 0;
	m_dwPackageNum = 0;


	return TRUE;
}


//-----------------------------------------------------------------------------
// destroy
//-----------------------------------------------------------------------------
VOID CServerTest::Destroy()
{
	m_pServer->Destroy();
//	m_pServer->SetLogCallBack(NULL, NULL);	 // 将回掉函数清空
	DeleteCriticalSection(&m_LockPlayerData);

	
	m_pWindow->Destroy();
	m_pGUI->Destroy();
}


//-----------------------------------------------------------------------------
// Mainloop
//-----------------------------------------------------------------------------
VOID CServerTest::Mainloop()
{
	DWORD dwMsg, dwParam1, dwParam2;
	INT nPool = m_pServer->GetPoolNum();

	while( FALSE == m_pWindow->MsgLoop() )
	{
		DWORD dwTime = timeGetTime();
		while( m_pWindow->PeekWindowsMsg(dwMsg, dwParam1, dwParam2) )
		{
			m_pGUI->OnWindowsMsg(dwMsg, dwParam1, dwParam2);
			if( dwMsg == WM_QUIT )
				return;
		}

		INT nMalloc = 0, nGC = 0;
		for(INT n=0; n<nPool; n++)
		{
			nMalloc += m_pServer->GetPool(n)->GetMalloc();
			nGC += m_pServer->GetPool(n)->GetGC();
		}

		m_pConsole->Watch(_T("Client"), m_mapClient.Size());
		m_pConsole->Watch(_T("TotalMalloc"), nMalloc);
		m_pConsole->Watch(_T("TotalGC"), nGC);
		m_pConsole->Watch(_T("ComRatio"), (DWORD)(((FLOAT)m_pServer->GetSendSize() - m_pServer->GetRealSendSize())*100.0f/m_pServer->GetSendSize()));
		m_pConsole->Watch(_T("FreeClient"), (DWORD)m_pServer->GetFreeClientNum());
		m_pConsole->Watch(_T("Delay"), m_dwAveDelay);
		
 		// IMSG(_T("min:%lu max:%lu ave:%lu %lu[%lu]\r\n"), m_dwMinDelay, m_dwMaxDelay, m_dwAveDelay, m_dwBigDelayNum, m_dwPackageNum);
		m_dwMinDelay = 9999999;
		m_dwMaxDelay = 0;

		EnterCriticalSection(&m_LockPlayerData);

		RecvAll();
		SendToAll();
		m_pServer->ActiveSend();

		LeaveCriticalSection(&m_LockPlayerData);

		m_pGUI->Update();
		m_pRender->Clear(0, 0, 0);
		m_pGUI->Render();
		m_pRender->Render();

		DWORD dwTimeUse = timeGetTime() - dwTime;
		if( dwTimeUse < 200 )
			Sleep(200-dwTimeUse);
	}
}




//-----------------------------------------------------------------------------
// 向所有客户端发送信息
//-----------------------------------------------------------------------------
VOID CServerTest::SendToAll()
{
	CHAR mess[512];
	tagMsg* pMsg = (tagMsg*)mess;

	DWORD dwTime = timeGetTime();
	tagClient* pClient = NULL;
	m_mapClient.ResetIterator();
	INT nIndex = 0;
	while( m_mapClient.PeekNext(pClient) )
	{
		if(m_pServer->GetSendCast(pClient->dwHandle)>50)
		{
			m_pServer->Kick(pClient->dwHandle);
			continue;
		}

		if( (nIndex % 4) == 0 )
			pMsg->dwIndex = 1;
		else
			pMsg->dwIndex = 0;

		pMsg->dwTimeServerSend = dwTime;
		//m_pServer->Send(pClient->dwID, pMsg, 100);
		m_pServer->Send(pClient->dwHandle, pMsg, 100);

		for(INT n=0;n<100; n++)
		{
			pMsg->dwIndex = 0;
			//m_pServer->Send(pClient->dwID, pMsg, 20);
			m_pServer->Send(pClient->dwHandle, pMsg, 100);
		}
		if( nIndex == 0 )
			m_pConsole->Watch(_T("Client0"), m_pServer->GetSendCastSize(pClient->dwHandle));
		nIndex++;
	}

}



//-----------------------------------------------------------------------------
// 接受所有客户端发来的信息
//-----------------------------------------------------------------------------
VOID CServerTest::RecvAll()
{
	tagClient* pClient = NULL;
	m_mapClient.ResetIterator();
	m_dwAveDelay = 0;
	m_dwPackageNum = 0;
	m_dwBigDelayNum = 0;
	while( m_mapClient.PeekNext(pClient) )
	{
		LPBYTE pRecv = NULL;
		INT nMsg = 0;
		DWORD dwSize = 0;
		while( pRecv = m_pServer->Recv(pClient->dwHandle, dwSize, nMsg) )
		{
			tagMsg* pMsg = (tagMsg*)pRecv;
			if( pMsg->dwIndex != 0 )
			{
				DWORD dwDelay = timeGetTime()-pMsg->dwTimeServerSend;
				if( dwDelay < m_dwMinDelay )
					m_dwMinDelay = dwDelay;
				if( dwDelay > m_dwMaxDelay )
					m_dwMaxDelay = dwDelay;

				m_dwAveDelay += dwDelay;
				m_dwPackageNum++;
				if( dwDelay > 500 )
					m_dwBigDelayNum++;
			}
			m_pServer->FreeRecved(pRecv);
		}


		/*tagUnitData* pUnit = NULL;
		while( pUnit = m_pServer->RecvUnit(pClient->dwID) )
		{
			tagMsg* pMsg = (tagMsg*)pUnit->wbuf.buf;
			if( pMsg->dwIndex != 0 )
			{
				DWORD dwDelay = timeGetTime()-pMsg->dwTimeServerSend;
				if( dwDelay < m_dwMinDelay )
					m_dwMinDelay = dwDelay;
				if( dwDelay > m_dwMaxDelay )
					m_dwMaxDelay = dwDelay;

				m_dwAveDelay += dwDelay;
				m_dwPackageNum++;
				if( dwDelay > 500 )
					m_dwBigDelayNum++;
			}

			m_pServer->ReturnUnit(pUnit);
		}*/

		/*
		INT nUnrecv = 0;
		DWORD dwSize = 0;
		LPBYTE pRecv = m_pServer->Recv(pClient->dwID, dwSize, nUnrecv);
		while( pRecv )
		{
			tagMsg* pMsg = (tagMsg*)pRecv;
			if( pMsg->dwIndex != 0 )
			{
				DWORD dwDelay = timeGetTime()-pMsg->dwTimeServerSend;
				if( dwDelay < m_dwMinDelay )
					m_dwMinDelay = dwDelay;
				if( dwDelay > m_dwMaxDelay )
					m_dwMaxDelay = dwDelay;

				m_dwAveDelay += dwDelay;
			}

			m_pServer->FreeRecved(pRecv);
			pRecv = m_pServer->Recv(pClient->dwID, dwSize, nUnrecv);
		}
		*/
	}

	if( m_dwPackageNum )
		m_dwAveDelay /= m_dwPackageNum;
}






//-----------------------------------------------------------------------------
// 客户端登陆本服务器回调函数
//-----------------------------------------------------------------------------
DWORD CServerTest::LoginCallBack(LPVOID p, LPVOID p2)
{
	EnterCriticalSection(&m_LockPlayerData);
	DWORD dwID = m_dwIDHolder++;
	
	tagXLoginParam* pParam = (tagXLoginParam*)p2;

	tagClient* pNew = new tagClient;
	pNew->dwID = dwID;
	pNew->dwHandle = pParam->dwHandle;

	m_mapClient.Add(dwID, pNew);

	LeaveCriticalSection(&m_LockPlayerData);
	return dwID;
}



//-----------------------------------------------------------------------------
// 客户端登出本服务器回调函数
//-----------------------------------------------------------------------------
DWORD CServerTest::LogoutCallBack(LPVOID p)
{
	EnterCriticalSection(&m_LockPlayerData);

	// 删除客户端
	DWORD dwID = (DWORD)p;
	tagClient* pClient = m_mapClient.Peek(dwID);
	if( P_VALID(pClient) )
	{
		delete(pClient);
		m_mapClient.Erase(dwID);
	}

	LeaveCriticalSection(&m_LockPlayerData);
	return 0;
}

