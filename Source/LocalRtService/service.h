///-----------------------------------------------------------------------------
/// File: service
/// Desc: 本地运维服务
/// Author: Zhang Rong
/// Date: Apr 2009
///
/// Copyright (c) 1985-2007 CTCRST Entertainment All rights reserved.
///-----------------------------------------------------------------------------
#pragma once

//------------------------------------------------------------------------------
// 远端的运维服务
//------------------------------------------------------------------------------
class RtServer
{
public:

	DWORD   m_dwID;
	BYTE	m_byType;
};

//------------------------------------------------------------------------------
// 本地运维服务的声明
//------------------------------------------------------------------------------
class CService
{
public:
	CService();
	~CService();

	// 服务的开启和关闭函数
	VOID	ServiceStart(DWORD dwArgc, LPTSTR *lpszArgv);
	VOID	ServiceStop();

	// 服务的初始化和销毁函数
	BOOL	Init();
	VOID	Destroy();

	// 登入登出回调函数
	DWORD	LoginCallBack(LPBYTE pMsg, DWORD dwSize);
	VOID	LogoutCallBack(DWORD dwID);

	// 消息处理函数
	VOID	HandleCloseWorld(tagNetCmd* pMsg,LPVOID pCli);
	VOID	HandleStartWorld(tagNetCmd* pMsg,LPVOID pCli);
	VOID	HandleServerLogin(tagNetCmd* pMsg,LPVOID pCli);
	VOID	HandleSystemCmd(tagNetCmd* pMsg,LPVOID pCli);

private:
	// 判断进程是否已经运行
	DWORD   GetProcessIdFromName(LPCTSTR pName);
	// 取得配置文件的绝对路径
	BOOL	GetWindowsServiceInstallPath(LPCTSTR ServiceName, tstring& szPath);

	TSFPTrunk<CService>				m_Trunk;
	
	// vEngine的相关工具
	TObjRef<VarContainer>			m_pVar;
	TObjRef<Log>					m_pLog;
	TObjRef<Util>					m_pUtil;
	TObjRef<NetCmdMgr>				m_pMsgCmdMgr;

	// 可配置信息
	INT								m_nPort;
	tstring							m_szPath;
	tstring							m_szLoginName;
	tstring							m_szDBName;
	tstring							m_szWorldName;

	// 公共变量
	VOLATILE BOOL					m_bServiceStop;
	VOLATILE DWORD					m_dwIDHolder;
	CRITICAL_SECTION				m_Lock;

	// 网络相关
	TObjRef<StreamServer>			m_pStreamServer;
	
	// 记录所有连接到本服务的远端运维服务
	TMap<DWORD, RtServer*>			m_mapRtServer;
	
};