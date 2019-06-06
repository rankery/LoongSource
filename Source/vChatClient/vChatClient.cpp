//-----------------------------------------------------------------------------
//!\file vChatClient.cpp
//!\author Lyp
//!
//!\date 2004-08-11
//! last 2007-10-11
//!
//!\brief vengine测试程序
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "vChatClient.h"
#include "resource.h"

#define NET_CFG_FILE _T("config\\vChatCfg.xml") 
//-----------------------------------------------------------------------------
// init
//-----------------------------------------------------------------------------
BOOL ChatClient::Init(HINSTANCE hInst)
{
	m_bTerminateThread = FALSE;

	m_render.Init(m_pRender);
	m_pWindow->Init(&m_render, 675, 445, TRUE);
	m_pWindow->CreateWnd(_T("vEngine Chat Client"), hInst, 0, 0, 0);

	if( !m_pRender->Init(m_pWindow->GetWidth(), m_pWindow->GetHeight()) )
		return FALSE;

	m_pGUI->Init(m_pWindow->GetWidth(), m_pWindow->GetHeight(), &m_render);	
	m_pRender->SetShowFPS(FALSE);

	// 注册几个控制台命令
	m_pConsole->Init(NULL, 2000, TRUE, FALSE);
	m_pConsole->GetWindow()->SetMovable(FALSE);
	m_pGUI->DestroyWnd(m_pConsole->GetWindow()->GetChild(_T("panel\\close")));
	m_pConsole->GetCommand()->SetUnhandle(m_Trunk.sfp1(&ChatClient::Sender));

	m_hIconQuest = ::LoadIcon(NULL, IDI_WARNING);
	m_hIconInfo = ::LoadIcon(NULL, IDI_INFORMATION);

	m_pTray->Create(hInst, m_pWindow->GetHWnd(), WM_VENGINE_USER, _T("vChat"), 
		m_hIconInfo, IDR_MENU1);

	Minimise();

	InitClient();
	return TRUE;
}


//-----------------------------------------------------------------------------
// destroy
//-----------------------------------------------------------------------------
VOID ChatClient::Destroy()
{
	InterlockedExchange((LONG*)&m_bTerminateThread, TRUE);
	m_pThread->WaitForAllThreadDestroy();

	m_pWindow->Destroy();
	m_pGUI->Destroy();
}


//-----------------------------------------------------------------------------
// Mainloop
//-----------------------------------------------------------------------------
VOID ChatClient::Mainloop()
{
	DWORD dwMsg, dwParam1, dwParam2;

	while( FALSE == m_pWindow->MsgLoop() )
	{
		while( m_pWindow->PeekWindowsMsg(dwMsg, dwParam1, dwParam2) )
		{
			if( dwMsg == WM_VENGINE_USER )
				m_pTray->OnTrayNotification(dwParam1, dwParam2);

			if( dwMsg == WM_QUIT || (dwMsg == WM_SYSCOMMAND && dwParam1 == SC_MINIMIZE) )
				Minimise();

			if( dwMsg == WM_COMMAND )	// 菜单操作
			{
				switch(LOWORD(dwParam1))
				{
				case ID_POPUP_MAXIMISE:
					Maximise();
					break;
				case ID_POPUP_MINIMISE:
					Minimise();
					break;
				case ID_POPUP_EXIT:	// 真正退出
					return;
				}
			}

			m_pGUI->OnWindowsMsg(dwMsg, dwParam1, dwParam2);
		}

		m_pConsole->Watch(_T("CONNECTION"), (DWORD)m_pClient->IsConnected());
		if( m_pWindow->IsWindowActive() && m_pTray->GetIcon() == m_hIconQuest )
			m_pTray->SetIcon(m_hIconInfo);
		
		m_pGUI->Update();
		m_pRender->Clear(0, 0, 0);
		m_pGUI->Render();
		m_pRender->Render();
		Sleep( m_pWindow->IsWindowActive() ? 10 : 100 );
	}
}


//-----------------------------------------------------------------------------
// 未知控制台命令处理函数
//-----------------------------------------------------------------------------
DWORD ChatClient::Sender(LPCTSTR szText)
{
	DWORD dwSize = sizeof(tagMsg) + lstrlen(szText) * sizeof(TCHAR);
	LPBYTE pBuffer = new BYTE[dwSize];
	ZeroMemory(pBuffer, dwSize);

	tagMsg* pSend = (tagMsg*)pBuffer;
	pSend->dwSize = dwSize;
	lstrcpy(pSend->szText, szText);
	pSend->dwCode = m_pUtil->Rand();
	Fileter(pSend, dwSize);
	m_pClient->Send(pBuffer, dwSize);

	SAFE_DEL_ARRAY(pBuffer);
	return 0;
}


//-----------------------------------------------------------------------------
// 最大化
//-----------------------------------------------------------------------------
VOID ChatClient::Maximise()
{
	m_pTray->MaximiseFromTray(m_pWindow->GetHWnd());
	m_pTray->SetMenuDefaultItem(ID_POPUP_MINIMISE, FALSE);
	m_pTray->EnableMenuItem(ID_POPUP_MAXIMISE, FALSE);
	m_pTray->EnableMenuItem(ID_POPUP_MINIMISE, TRUE);
}


//-----------------------------------------------------------------------------
// 最小化
//-----------------------------------------------------------------------------
VOID ChatClient::Minimise()
{
	m_pTray->MinimiseToTray(m_pWindow->GetHWnd());
	m_pTray->SetMenuDefaultItem(ID_POPUP_MAXIMISE, FALSE);
	m_pTray->EnableMenuItem(ID_POPUP_MAXIMISE, TRUE);
	m_pTray->EnableMenuItem(ID_POPUP_MINIMISE, FALSE);
}


//-----------------------------------------------------------------------------
// 创建客户端
//-----------------------------------------------------------------------------
VOID ChatClient::InitClient()
{
	m_pConsole->Watch(_T("CONNECTION"), 0);
	m_pClient->Init(FALSE);
	TObjRef<VarContainer> pBucket = CreateObj("Config", "VarContainer");

	if( pBucket->Load(NULL, NET_CFG_FILE, "name") )
	{
		strcpy(m_szDestIP, m_pUtil->UnicodeToAnsi(pBucket->GetString(_T("server"))));
		m_nPort = pBucket->GetInt(_T("port"));
	}
	

	// 启动线程
	m_pThread->CreateThread(_T("thread_client"),
		(THREADPROC)m_Trunk.sfp1(&ChatClient::ThreadXClient), 0);

	KillObj("Config");
}


//-----------------------------------------------------------------------------
// 接收线程
//-----------------------------------------------------------------------------
DWORD ChatClient::ThreadXClient(INT nIndex)
{
	DWORD dwMyIP = GT_INVALID;
	m_pClient->GetLocalLANIP(dwMyIP);

	tagMsg msg, msgHeart;	// 验证包\心跳包
	msg.dwState = 1;
	msgHeart.dwState = 3;	// 心跳

_thread_connect:
	m_pClient->TryToConnect(m_szDestIP, 7050);
	while( m_pClient->IsTryingToConnect() )
	{
		Sleep(rand()%1000 + 200);
		if( m_bTerminateThread )
			goto _thread_exit;
	}

	if( !m_pClient->IsConnected() )
		goto _thread_connect;

	m_pClient->Send(&msg, sizeof(msg));	// 发送验证包

	// 消息接收循环
	DWORD dwRecvSize = 0;
	LPBYTE pRecv = NULL;
	INT nHeartCount = 0;
	while( !m_bTerminateThread && m_pClient->IsConnected() )
	{
		pRecv = m_pClient->Recv(dwRecvSize);
		if( !P_VALID(pRecv) )
		{
			Sleep(100);
			++nHeartCount;
			if( nHeartCount > 100 )
			{
				m_pClient->Send(&msgHeart, sizeof(msgHeart));
				nHeartCount = 0;
			}
			continue;
		}

		// 从大包中解出小包依次处理
		LPBYTE pRecvPointer = pRecv;
		DWORD dwMsgSize = 0;
		while( pRecvPointer < pRecv + dwRecvSize )
		{
			tagMsg* pMsg = (tagMsg*)pRecvPointer;
			dwMsgSize = pMsg->dwSize;
			switch(pMsg->dwState)
			{
			case 1:
				m_pConsole->Watch(pMsg->szName, 1);
				break;
			case 2:
				m_pConsole->Watch(pMsg->szName, 0);
				break;
			case 0:
				this->Fileter(pMsg, dwMsgSize);
				IMSG(_T("%s：%s\r\n"), pMsg->szName, pMsg->szText);
				if( !m_pWindow->IsWindowActive() )
					m_pTray->SetIcon(m_hIconQuest);
				break;
			default:
				break;
			}

			pRecvPointer += dwMsgSize;
		}

		m_pClient->FreeRecved((LPBYTE)pRecv);
	}

	//m_pTray->ShowBalloon(_T("Connection Lost"), _T("vChat"), NIIF_INFO);
	m_pClient->StopTryingConnect();
	if( !m_bTerminateThread )
		goto _thread_connect;

_thread_exit:
	_endthreadex(0);
	return 0;
}


//-----------------------------------------------------------------------------
// 数据处理函数
//-----------------------------------------------------------------------------
VOID ChatClient::Fileter(tagMsg* pMsg, DWORD dwSize)
{
	m_pUtil->Randomize(pMsg->dwCode);
	DWORD dwRealSize = (dwSize - sizeof(tagMsg))/2;
	for(DWORD n=0; n<dwRealSize; n++)
	{
		pMsg->szText[n] = pMsg->szText[n] ^ m_pUtil->Rand();
	}
}

