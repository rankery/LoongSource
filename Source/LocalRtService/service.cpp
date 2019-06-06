//-----------------------------------------------------------------------------
// File: service
// Author: Zhang Rong
// Date: Apr 2009
// Last: May 2009
//
// Copyright (c) 1985-2007 CTCRST Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include <Windows.h>
#include "service.h"
#include "ServiceFrame.h"

CService::CService():m_Trunk(this),m_dwIDHolder(0)
{

}

CService::~CService()
{
	
}

//-----------------------------------------------------------------------------
// Actual code of the service that does the work.
//-----------------------------------------------------------------------------
BOOL CService::Init()
{	
	vEngine::InitNetwork();
	vEngine::InitEngine();

	m_pLog = "Log";
	m_pUtil = "Util";
	m_pMsgCmdMgr = "NetCmdMgr";
	m_pStreamServer = "StreamServer";

	// 读取Service配置文件
	tstring ConfigPath;
	if (!GetWindowsServiceInstallPath(_T(SZSERVICENAME), ConfigPath))
	{
		return FALSE;
	}
	 
	ConfigPath = ConfigPath + _T(RTSERVICE_CONFIG_INI);
	
	m_pVar = "VarContainer";
	m_pVar->Load("VirtualFileSys", ConfigPath.c_str());

	m_nPort				= m_pVar->GetInt(_T("port service"));
	m_szPath			= m_pVar->GetString(_T("path server"));
	m_szLoginName		= m_pVar->GetString(_T("login_name server"));
	m_szDBName			= m_pVar->GetString(_T("db_name server"));
	m_szWorldName		= m_pVar->GetString(_T("world_name server"));

	m_bServiceStop = FALSE;

	InitializeCriticalSection(&m_Lock);
	// 生成log
	m_pLog->Create();

	//设置StreamServer登陆/登出回调函数
	//m_pStreamServer->SetLogCallBack( (TCPLOGINCALLBACK)m_Trunk.sfp2(&CService::LoginCallBack),
	//								 (TCPLOGOUTCALLBACK)m_Trunk.sfp1(&CService::LogoutCallBack));

	//注册有效消息，并为每一个消息指定对应的处理函数。。。。。。。。
	m_pMsgCmdMgr->Register( "NLSS_Close",			(NETMSGPROC)m_Trunk.sfp2(&CService::HandleCloseWorld),	_T("NLSS_Close") );
	m_pMsgCmdMgr->Register( "NLSS_Start",			(NETMSGPROC)m_Trunk.sfp2(&CService::HandleStartWorld),	_T("NLSS_Start") );
	m_pMsgCmdMgr->Register( "NLSS_LoginService",	(NETMSGPROC)m_Trunk.sfp2(&CService::HandleServerLogin), _T("NLSS_LoginService") );
	m_pMsgCmdMgr->Register( "NLSS_SystemCmd",		(NETMSGPROC)m_Trunk.sfp2(&CService::HandleSystemCmd),	_T("NLSS_SystemCmd") );

	m_pStreamServer->Init(	(TCPLOGINCALLBACK)m_Trunk.sfp2(&CService::LoginCallBack),
							(TCPLOGOUTCALLBACK)m_Trunk.sfp1(&CService::LogoutCallBack), 
							m_nPort);
	
	return TRUE;
}

VOID CService::Destroy()
{
	m_pStreamServer->Destroy();
	m_pLog->Close();
	vEngine::DestroyEngine();
	vEngine::DestroyNetwork();
}

//-----------------------------------------------------------------------------
// Actual code of the service that does the work.
//-----------------------------------------------------------------------------
VOID CService::ServiceStart (DWORD dwArgc, LPTSTR *lpszArgv)
{
	EXCEPTION_PROTECT;
	// Service initialization
	// report the status to the service control manager.
	if( !ReportStatusToSCMgr(SERVICE_START_PENDING, NO_ERROR, 3000) )
		Destroy();

	this->Init();

	// report the status to the service control manager.
	if( !ReportStatusToSCMgr(SERVICE_RUNNING, NO_ERROR, 0) )
	{
		Destroy();
		DeleteCriticalSection(&m_Lock);
	}

	// End of initialization
	// Service is now running, perform work until shutdown
	DWORD dwLastReportTime = timeGetTime();
	
	while (m_bServiceStop == FALSE)
	{		
		RtServer* pRtServer = 0;
		m_mapRtServer.ResetIterator();

		while (m_mapRtServer.PeekNext(pRtServer))
		{
			LPBYTE pRecv = NULL;
			DWORD dwMsgSize = 0;
			INT nUnRecved = 0;

			pRecv = m_pStreamServer->Recv(pRtServer->m_dwID, dwMsgSize, nUnRecved);

			if (P_VALID(pRecv))
			{		
				m_pMsgCmdMgr->HandleCmd((tagNetCmd*)pRecv, dwMsgSize, (DWORD)pRtServer);			
				m_pStreamServer->FreeRecved(pRtServer->m_dwID, pRecv);			
			}
			else		
				continue;
		}

		Sleep(500);
	}
	
	//DeleteCriticalSection(&m_LockPlayerData);
	Destroy();
	DeleteCriticalSection(&m_Lock);
}



//-----------------------------------------------------------------------------
// Stops the service
// If a ServiceStop procedure is going to take longer than 3 seconds to execute,
// it should spawn a thread to execute the stop code, and return.  Otherwise, the
// ServiceControlManager will believe that the service has stopped responding.
//-----------------------------------------------------------------------------
VOID CService::ServiceStop()
{
	m_bServiceStop = TRUE;
}


//-----------------------------------------------------------------------------
// 登陆
//-----------------------------------------------------------------------------
DWORD CService::LoginCallBack(LPBYTE pMsg, DWORD dwSize)
{
	_tprintf(_T("enter LoginCallBack..........\n"));
	tagNLSS_LoginService * pCmd = (tagNLSS_LoginService *)pMsg;

	EnterCriticalSection(&m_Lock);

	DWORD dwID = ++m_dwIDHolder;
	if( dwID == 0 )
		dwID = m_dwIDHolder = 1;

	RtServer *pRtServer = new RtServer;// LogoutCallBack 中释放
	pRtServer->m_dwID	= dwID;
	pRtServer->m_byType	= pCmd->eType;

	m_mapRtServer.Add(dwID, pRtServer);
	
	LeaveCriticalSection(&m_Lock);

	return pRtServer->m_dwID;
}


//-----------------------------------------------------------------------------
// 登出回调
//-----------------------------------------------------------------------------
VOID CService::LogoutCallBack(DWORD dwID)
{
	EnterCriticalSection(&m_Lock);

	RtServer* pToDelete = m_mapRtServer.Peek(dwID);
	
	if( P_VALID(pToDelete) )
	{
		m_mapRtServer.Erase(dwID);
		SAFE_DEL(pToDelete);
	}

	LeaveCriticalSection(&m_Lock);
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
VOID CService::HandleServerLogin(tagNetCmd* pMsg,LPVOID pCli)
{
	RtServer* pClient = (RtServer *)pCli;

	//发送确认消息
	tagNLSC_LoginService Msg;
	Msg.dwErrorCode = 0;
	m_pStreamServer->Send(pClient->m_dwID, &Msg, sizeof(Msg));
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
VOID CService::HandleSystemCmd(tagNetCmd* pMsg,LPVOID pCli)
{	
	tagNLSS_SystemCmd* pRecv = (tagNLSS_SystemCmd*) pMsg;

	TCHAR szCmd[System_Cmd_Length];
	ZeroMemory(szCmd, System_Cmd_Length);
	wcscpy(szCmd, pRecv->szCmd);

	std::system(m_pUtil->UnicodeToUnicode8(szCmd));
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
VOID CService::HandleCloseWorld(tagNetCmd* pMsg,LPVOID pCli)
{
	//杀掉游戏世界进程
	TCHAR szCmd[X_LONG_STRING];
	ZeroMemory(szCmd, X_LONG_STRING);
	DWORD dwProcessID = GT_INVALID;

	RtServer* pClient = (RtServer *)pCli;
	switch (pClient->m_byType)
	{
	case EST_Login:
		if (!GT_VALID(dwProcessID = GetProcessIdFromName(m_szLoginName.c_str())))
			return;
		//_stprintf(szCmd,_T("ntsd -c q -p %d"), dwProcessID);
		_stprintf(szCmd,_T("taskkill /im %s"), m_szLoginName.c_str());
		break;

	case EST_DB:
		if (!GT_VALID(dwProcessID = GetProcessIdFromName(m_szDBName.c_str())))
			return;
		_stprintf(szCmd,_T("taskkill /im %s"), m_szDBName.c_str());
		break;

	case EST_World:
		if (!GT_VALID(dwProcessID = GetProcessIdFromName(m_szWorldName.c_str())))
			return;
		_stprintf(szCmd,_T("taskkill /im %s"), m_szWorldName.c_str());
		break;
	}
	
	std::system(m_pUtil->UnicodeToUnicode8(szCmd));
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
VOID CService::HandleStartWorld(tagNetCmd* pMsg,LPVOID pCli)
{
	//启动游戏世界进程
	TCHAR szCmd[X_LONG_STRING];
	ZeroMemory(szCmd, X_LONG_STRING);

	RtServer* pClient = (RtServer *)pCli;
	switch (pClient->m_byType)
	{
	case EST_Login:
		if (GetProcessIdFromName(m_szLoginName.c_str()))
			return;

		_stprintf(szCmd,_T("%s\\%s"), m_szPath.c_str(), m_szLoginName.c_str());
		break;

	case EST_DB:
		if (GetProcessIdFromName(m_szDBName.c_str()))
			return;

		_stprintf(szCmd,_T("%s\\%s"), m_szPath.c_str(), m_szDBName.c_str());
		break;

	case EST_World:
		if (GetProcessIdFromName(m_szWorldName.c_str()))
			return;

		_stprintf(szCmd,_T("%s\\%s"), m_szPath.c_str(), m_szWorldName.c_str());
		break;
	}

	//std::system(m_pUtil->UnicodeToUnicode8(szCmd));
	WinExec(m_pUtil->UnicodeToUnicode8(szCmd), SW_SHOW);
}

DWORD CService::GetProcessIdFromName( LPCTSTR pName )
{
	PROCESSENTRY32 pe;   
	DWORD id = 0;	   
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);   
	pe.dwSize = sizeof(PROCESSENTRY32);

	if(!Process32First(hSnapshot,&pe))   
		return 0;

	while(1)   
	{   
		pe.dwSize = sizeof(PROCESSENTRY32);   
		if( Process32Next(hSnapshot,&pe) == FALSE )   
			break;

		if(_tcscmp(pe.szExeFile,pName) == 0)   
		{   
			id = pe.th32ProcessID;   
			break;   
		}   

	};  

	CloseHandle(hSnapshot);	   
	return id; 
}

BOOL CService::GetWindowsServiceInstallPath( LPCTSTR ServiceName, tstring& szPath )
{
	tstring key = _T("SYSTEM\\CurrentControlSet\\Services\\");
	key = key + ServiceName;

	HKEY hKey;
	DWORD nSize=MAX_PATH;
	TCHAR vValue[MAX_PATH]={0};
	LONG ret;

	ret = RegOpenKeyEx( HKEY_LOCAL_MACHINE,
		key.c_str(),
		0,
		KEY_ALL_ACCESS,
		&hKey );

	if (ret != ERROR_SUCCESS)
	{
		return FALSE;
	}

	ret=RegQueryValueEx(hKey,
		_T("ImagePath"),
		NULL,
		NULL,
		(LPBYTE)vValue,
		&nSize);

	RegCloseKey(hKey);

	if (ret!=ERROR_SUCCESS)
	{
		return FALSE;
	}

	szPath.clear();
	szPath = vValue;
	szPath = szPath.substr(0, szPath.find_last_of(_T("\\")));

	return TRUE;
}