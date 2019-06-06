#include "stdafx.h"
//#include "time.h"
#include "GMServer.h"
#include "Mutex.h"
#include "../../ServerDefine/msg_show_sum.h"
#include "../../ServerDefine/msg_rt_c.h"

GMServer::GMServer(INT nPort, LPCTSTR szIP)
{
	m_bActive = false;
	m_bConnected = false;
	m_nSum = 0;
	_tcscpy_s(m_szIP, X_SHORT_STRING, szIP);
	m_nPort = nPort;
}

BOOL GMServer::Init()
{
	// 创建消息管理
	m_pCmdMgr = "NetCmdMgr";

	// 创建连接对象，并初始化
	m_pStreamTransport = "StreamTransport";
	if( !P_VALID(m_pStreamTransport) )
	{
		_tprintf(_T("- Fail to Init GMServer (%s) NetWork.\r\n"), m_szIP);
		return FALSE;
	}

	m_pStreamTransport->Init();

	// 注册所有网络命令
	RegisterAllNetCommand();

	return TRUE;
}

BOOL GMServer::Connect()
{
	DWORD dwNow = GetCurrentDWORDTime();
	DWORD dwEndTime = dwNow + 1;

	while( !m_pStreamTransport->IsConnected() )
	{
		if( !m_pStreamTransport->IsTryingToConnect() )
		{
			m_pStreamTransport->TryToConnect(m_pUtil->UnicodeToUnicode8(m_szIP), m_nPort);
		}

		dwNow = GetCurrentDWORDTime(); 
		if( dwEndTime <= dwNow )
		{
			_tprintf(_T("- Fail to Connect to GMServer at %s: %d.\r\n"), m_szIP, m_nPort);
			m_bConnected = FALSE;
			return FALSE;
		}

		Sleep(100);
		continue;	// 重新检测连接
	}

	_tprintf(_T("- Contected to GMServer at %s: %d.\r\n"), m_szIP, m_nPort);
	
	m_bConnected = TRUE;

	tagNSSC_Challenge send;
	m_pStreamTransport->Send(&send, send.dwSize);

	return TRUE;
}

void GMServer::Update()
{
	// 更新连接
	if( NULL == m_pStreamTransport )
		return;

	if( !m_pStreamTransport->IsConnected() )
	{
		InterlockedExchange((LONG*)&m_bTermConnect, TRUE);
		m_pStreamTransport->Disconnect();

		// 重新连接
		InterlockedExchange((LONG*)&m_bTermConnect, FALSE);
		Connect();
	}

	
	// 处理消息
	DWORD dwSize = 0;
	LPBYTE pRecv = m_pStreamTransport->Recv(dwSize);
	
	if( !P_VALID(pRecv) )
		return;

	m_pCmdMgr->HandleCmd((tagNetCmd*)pRecv, dwSize, GT_INVALID);

	// 回收资源
	m_pStreamTransport->FreeRecved(pRecv);
	
}

void GMServer::PrintInfo(FLOAT fRate, BOOL bNewest)
{
	m_lock.Acquire();

	TMap<INT, Section*>::TMapIterator it = m_mapSection.Begin();
	Section* pSection = NULL;

	while(m_mapSection.PeekNext(it, pSection))
	{
		pSection->PrintInfo(fRate);
	}

	m_lock.Release();
}

void GMServer::RegisterAllNetCommand()
{
	m_pCmdMgr->Register("NSSS_GetDynInfo", (NETMSGPROC)m_Trunk.sfp2(&GMServer::HandleGMServerDynInfo), _T("NSSS_GetDynInfo"));

}

void GMServer::UnRegisterAllNetCommand()
{
	if(P_VALID(m_pCmdMgr))
	{
		m_pCmdMgr->Destroy();
		KillObj("NetCmdMgr");
	}	
}

DWORD GMServer::HandleGMServerDynInfo(tagNetCmd* pMsg, DWORD)
{
	tagNSSS_GetDynInfo* pRecv = (tagNSSS_GetDynInfo*) pMsg;
	
	INT nSectionCount = pRecv->nSectionCount;
	
	// 先清空改GMServer管理下的mapSection

	m_lock.Acquire();

	Section* pSection = NULL;
	TMap<INT, Section*>::TMapIterator it = m_mapSection.Begin();

	while( m_mapSection.PeekNext(it, pSection) )
	{
		GameWorld* pWorld = NULL;
		TMap<INT, GameWorld*>::TMapIterator it = pSection->GetWorldMap().Begin();
		{
			SAFE_DEL(pWorld);
		}
		pSection->GetWorldMap().Clear();

		SAFE_DEL(pSection);
	}

	m_mapSection.Clear();

	
	// 更新 mapSection 信息
	INT n = 0;
	INT nSum = 0;
	while ( n < nSectionCount )
	{
		Section* pSection = new Section;
		//ZeroMemory(pSection, sizeof(Section));

		pSection->SetSectionID( pRecv->SectionInfo[n].dwSectionID );
		pSection->SetSectionName( (LPCTSTR)pRecv->SectionInfo[n].szSectionName );

		INT m = 0;
		INT nWorldCount = pRecv->SectionInfo[n].nNum;
		while( m < nWorldCount )
		{
			GameWorld* pWorld = new GameWorld;
			//ZeroMemory(pWorld, sizeof(GameWorld));

			pWorld->SetWorldID(pRecv->SectionInfo[n].World[m].dwWorldID);
			pWorld->SetWorldName(pRecv->SectionInfo[n].World[m].szWorldName);
			pWorld->SetOnline(pRecv->SectionInfo[n].World[m].nOnline);

			if( pRecv->SectionInfo[n].World[m].nOnline >= 0 )
			{
				nSum += pRecv->SectionInfo[n].World[m].nOnline;
				pWorld->SetStatus(TRUE);
			}
			else
			{
				pWorld->SetStatus(FALSE);
			}

			pSection->AddWorld(m, pWorld);
			m++;
		}

		m_mapSection.Add(n, pSection);
		n++;
	}

	m_bActive = TRUE;
	m_nSum = nSum;
	
	m_lock.Release();

	return TRUE;
}

void GMServer::SendMsg(LPVOID pMsg, DWORD dwSize)
{
	if( !m_pStreamTransport->IsConnected() || !m_bConnected )
		return;

	BOOL bSuccess = m_pStreamTransport->Send(pMsg, dwSize);
}