//-----------------------------------------------------------------------------
//!\file vChatServer.cpp
//!\author Lyp
//!
//!\date 2004-08-11
//! last 2007-10-11
//!
//!\brief vengine测试程序
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "vChatServer.h"
#include "resource.h"

#define NET_CFG_FILE _T("config\\vChatCfg.xml") 
//-----------------------------------------------------------------------------
// init
//-----------------------------------------------------------------------------
BOOL ChatServer::Init(HINSTANCE hInst)
{
	m_render.Init(m_pRender);
	m_pWindow->Init(&m_render, 675, 445, TRUE);
	m_pWindow->CreateWnd(_T("vChat Server"), hInst);

	if( !m_pRender->Init(m_pWindow->GetWidth(), m_pWindow->GetHeight()) )
		return FALSE;

	m_pGUI->Init(m_pWindow->GetWidth(), m_pWindow->GetHeight(), &m_render);	
	m_pRender->SetShowFPS(FALSE);

	m_pConsole->Init(NULL);
	m_dwIDHolder = 0;
	InitializeCriticalSectionAndSpinCount(&m_LockPlayerData, 4000);

	InitSet();

	tagXServerConfig config;
	config.nPort = m_nPort;
	config.bReusePort = true;
	config.dwMaxRecvSize = 1024*1024;
	config.fnLogIn = (XLOGINCALLBACK)m_Trunk.sfp2(&ChatServer::LoginCallBack);
	config.fnLogOut = (XLOGOUTCALLBACK)m_Trunk.sfp1(&ChatServer::LogoutCallBack);
	m_pServer->Init(config);

	m_hIconInfo = ::LoadIcon(NULL, IDI_INFORMATION);
	m_pTray->Create(hInst, m_pWindow->GetHWnd(), WM_VENGINE_USER, _T("vServer"), 
		m_hIconInfo, IDR_MENU1);

	Minimise();
	return TRUE;
}


//-----------------------------------------------------------------------------
// destroy
//-----------------------------------------------------------------------------
VOID ChatServer::Destroy()
{
	m_pServer->Destroy();
	DeleteCriticalSection(&m_LockPlayerData);

	
	m_pWindow->Destroy();
	m_pGUI->Destroy();
}


//-----------------------------------------------------------------------------
// Mainloop
//-----------------------------------------------------------------------------
VOID ChatServer::Mainloop()
{
	DWORD dwMsg, dwParam1, dwParam2;
	INT nPool = m_pServer->GetPoolNum();
	INT nHeartBeat = 0;

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

		INT nMalloc = 0, nGC = 0;
		for(INT n=0; n<nPool; n++)
		{
			nMalloc += m_pServer->GetPool(n)->GetMalloc();
			nGC += m_pServer->GetPool(n)->GetGC();
		}

		m_pConsole->Watch(_T("Client"), m_mapClient.Size());
		m_pConsole->Watch(_T("TotalMalloc"), nMalloc);
		m_pConsole->Watch(_T("TotalGC"), nGC);
		m_pConsole->Watch(_T("SendSize"), (DWORD)m_pServer->GetSendSize());
		m_pConsole->Watch(_T("RealSendSize"), (DWORD)m_pServer->GetRealSendSize());
		m_pConsole->Watch(_T("ComRatio"), (DWORD)(((FLOAT)m_pServer->GetSendSize()
			- m_pServer->GetRealSendSize())*100.0f/m_pServer->GetSendSize()));
		m_pConsole->Watch(_T("FreeClient"), (DWORD)m_pServer->GetFreeClientNum());

		++nHeartBeat;
		EnterCriticalSection(&m_LockPlayerData);
		if( nHeartBeat > 1000 )
		{
			RecvAll(TRUE);
			nHeartBeat = 0;
		}
		else
			RecvAll(FALSE);
		
		m_pServer->ActiveSend();
		LeaveCriticalSection(&m_LockPlayerData);

		m_pGUI->Update();
		m_pRender->Clear(0, 0, 0);
		m_pGUI->Render();
		m_pRender->Render();

		Sleep( m_pWindow->IsWindowActive() ? 50 : 200 );
	}
}


//-----------------------------------------------------------------------------
// 最大化
//-----------------------------------------------------------------------------
VOID ChatServer::Maximise()
{
	m_pTray->MaximiseFromTray(m_pWindow->GetHWnd());
	m_pTray->SetMenuDefaultItem(ID_POPUP_MINIMISE, FALSE);
	m_pTray->EnableMenuItem(ID_POPUP_MAXIMISE, FALSE);
	m_pTray->EnableMenuItem(ID_POPUP_MINIMISE, TRUE);
}


//-----------------------------------------------------------------------------
// 最小化
//-----------------------------------------------------------------------------
VOID ChatServer::Minimise()
{
	m_pTray->MinimiseToTray(m_pWindow->GetHWnd());
	m_pTray->SetMenuDefaultItem(ID_POPUP_MAXIMISE, FALSE);
	m_pTray->EnableMenuItem(ID_POPUP_MAXIMISE, TRUE);
	m_pTray->EnableMenuItem(ID_POPUP_MINIMISE, FALSE);
}

//-----------------------------------------------------------------------------
// 初始化IP表
//-----------------------------------------------------------------------------
VOID ChatServer::InitSet()
{
	m_pClient->Init(FALSE);
	TObjRef<VarContainer> pBucket = CreateObj("Config", "VarContainer");
	tstring strTemp;

	std::list<tstring> listName;
	if( pBucket->Load(NULL, NET_CFG_FILE, "name", &listName) )
	{
		std::list<tstring>::iterator it;
		for(it=listName.begin();it!=listName.end();it++)
		{
			strTemp = pBucket->GetString((*it).c_str());
			DWORD dwIP = m_pClient->StringIP2IP((CHAR*)m_pUtil->UnicodeToAnsi(strTemp.c_str()));
			tstring strName = (*it);
			if( strName.size() > 30 )
				strName = strName.substr(0, 30);
			m_mapIP.insert(std::make_pair(dwIP, strName));
		}
	}

	m_nPort = pBucket->GetInt(_T("port"));
	KillObj("Config");
}


//-----------------------------------------------------------------------------
// 接受所有客户端发来的信息
//-----------------------------------------------------------------------------
VOID ChatServer::RecvAll(BOOL bHeartBeat)
{
	std::list<tagClient*> listClient;
	std::list<tagClient*>::iterator it;
	m_mapClient.ExportAllValue(listClient);
	tagClient* pClient = NULL;

	for(it = listClient.begin(); it != listClient.end(); ++it)
	{
		LPBYTE pRecv = NULL;
		INT nMsg = 0;
		DWORD dwSize = 0;
		while( pRecv = m_pServer->Recv((*it)->dwHandle, dwSize, nMsg) )
		{
			tagMsg* pMsg = (tagMsg*)pRecv;
			if( pMsg->dwState < 3 )
			{
				lstrcpy(pMsg->szName, (*it)->strName.c_str());
				this->SendToAll(pMsg, dwSize);

				if( pMsg->dwState == 1 )	// 玩家上线后，把所有其他在线玩家发给他
				{
					tagMsg msg;
					msg.dwState = 1;
					m_mapClient.ResetIterator();
					while( m_mapClient.PeekNext(pClient) )
					{
						lstrcpy(msg.szName, pClient->strName.c_str());
						m_pServer->Send((*it)->dwHandle, &msg, sizeof(msg));
					}
				}
			}

			m_pServer->FreeRecved(pRecv);
		}

		if( bHeartBeat )
		{
			tagMsg msgHeart;
			msgHeart.dwState = 3;
			m_pServer->Send((*it)->dwHandle, &msgHeart, sizeof(msgHeart));
		}
	}
}


//-----------------------------------------------------------------------------
// 向所有客户端发送消息
//-----------------------------------------------------------------------------
VOID ChatServer::SendToAll(tagMsg* pMsg, DWORD dwSize)
{
	tagClient* pClient = NULL;
	m_mapClient.ResetIterator();
	while( m_mapClient.PeekNext(pClient) )
		m_pServer->Send(pClient->dwHandle, pMsg, dwSize);
}



//-----------------------------------------------------------------------------
// 客户端登陆本服务器回调函数
//-----------------------------------------------------------------------------
DWORD ChatServer::LoginCallBack(LPVOID p, LPVOID p2)
{
	tagXLoginParam* pParam = (tagXLoginParam*)p2;
	tagUnit* pRecvUnit = (tagUnit*)p;
	tagMsg* pRecv = (tagMsg*)(pRecvUnit->pBuffer);

	if( pRecv->dwState != 1 )
		return GT_INVALID;

	if( m_mapIP.find(pParam->dwAddress) == m_mapIP.end() )
		return GT_INVALID;	// 不允许未知客户端登陆

	EnterCriticalSection(&m_LockPlayerData);
	if( m_mapClient.IsExist(pParam->dwAddress) )
	{
		LeaveCriticalSection(&m_LockPlayerData);
		return GT_INVALID; // 不允许重复登录
	}
	
	tagClient* pNew = new tagClient;
	pNew->dwIP = pParam->dwAddress;
	pNew->dwHandle = pParam->dwHandle;
	pNew->strName = m_mapIP.find(pParam->dwAddress)->second;

	m_mapClient.Add(pNew->dwIP, pNew);
	LeaveCriticalSection(&m_LockPlayerData);
	return pParam->dwAddress;
}



//-----------------------------------------------------------------------------
// 客户端登出本服务器回调函数
//-----------------------------------------------------------------------------
DWORD ChatServer::LogoutCallBack(LPVOID p)
{
	EnterCriticalSection(&m_LockPlayerData);

	// 删除客户端
	DWORD dwID = (DWORD)p;
	tagClient* pClient = m_mapClient.Peek(dwID);
	if( P_VALID(pClient) )
	{
		m_mapClient.Erase(dwID);
		
		tagMsg send;
		send.dwState = 2;
		lstrcpy(send.szName, pClient->strName.c_str());
		this->SendToAll(&send, sizeof(send));

		delete(pClient);
	}

	LeaveCriticalSection(&m_LockPlayerData);
	return 0;
}

