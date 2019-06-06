//-----------------------------------------------------------------------------
//!\file world_mgr.cpp
//!\author Aslan
//!
//!\date 2009-06-11
//! last 2009-06-11
//!
//!\brief 游戏世界管理器
//!
//!	Copyright (c) 2004 TENGWU Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#include "stdafx.h"

#include "../ServerDefine/msg_bill_s.h"
#include "../ServerDefine/bill_define.h"

#include "world.h"
#include "world_mgr.h"
#include "bill_server.h"

//------------------------------------------------------------------------------
// 全局变量定义
//------------------------------------------------------------------------------
WorldMgr g_WorldMgr;

//------------------------------------------------------------------------------
// 初始化
//------------------------------------------------------------------------------
BOOL WorldMgr::Init()
{
	m_pUtil = "Util";

	// 加载所有world
	TObjRef<VarContainer> pVar = "BillConfig";

	// 生成网络命令管理器
	CreateObj("NetCmdMgrWorld", "NetCmdMgr");
	m_pCmdMgr = "NetCmdMgrWorld";
	if( !P_VALID(m_pCmdMgr) ) return FALSE;

	// 生成服务器网络接口
	CreateObj("StreamServerWorld", "StreamServer");
	m_pTrans = "StreamServerWorld";
	if( !P_VALID(m_pTrans) ) return FALSE;

	// 注册网络命令
	RegisterWorldCmd();

	// 加载所有world
	INT nWorldNum = pVar->GetInt(_T("num world"));
	for(INT n = 0; n < nWorldNum; ++n)
	{
		World* pWorld = new World;
		if( !pWorld->Init(n) ) return FALSE;

		m_mapWorld.Add(pWorld->GetID(), pWorld);
	}

	// 初始化网络
	m_nPort = pVar->GetInt(_T("port"), _T("server"));
	m_pTrans->Init((TCPLOGINCALLBACK)m_Trunk.sfp2(&WorldMgr::LoginCallBack),
					(TCPLOGOUTCALLBACK)m_Trunk.sfp1(&WorldMgr::LogoutCallBack),
					m_nPort);

	return TRUE;
}

//------------------------------------------------------------------------------
// 销毁
//------------------------------------------------------------------------------
VOID WorldMgr::Destroy()
{
	// 删除所有的游戏世界
	World* pWorld = NULL;
	m_mapWorld.ResetIterator();

	while( m_mapWorld.PeekNext(pWorld) )
	{
		SAFE_DEL(pWorld);
	}
	m_mapWorld.Clear();

	// 删除网络
	if( P_VALID(m_pTrans) )	
	{
		m_pTrans->Destroy();
		KillObj("StreamServerWorld");
	}

	// 注销所有的网络命令
	UnregisterWorldCmd();
	KillObj("NetCmdMgrWorld");
}

//------------------------------------------------------------------------------
// 更新
//------------------------------------------------------------------------------
VOID WorldMgr::Update()
{

}

//-------------------------------------------------------------------------------
// 注册游戏世界的网络消息
//-------------------------------------------------------------------------------
VOID WorldMgr::RegisterWorldCmd()
{
	m_pCmdMgr->Register("NWB_Login", (NETMSGPROC)m_Trunk.sfp2(&WorldMgr::HandleWorldLogin), _T("world login"));
}

//-------------------------------------------------------------------------------
// 注销游戏世界的网络消息
//-------------------------------------------------------------------------------
VOID WorldMgr::UnregisterWorldCmd()
{
	m_pCmdMgr->Destroy();
}

//--------------------------------------------------------------------------------
// 登陆请求
//--------------------------------------------------------------------------------
DWORD WorldMgr::LoginCallBack(LPVOID p1, LPVOID p2)
{
	static DWORD dwFirstMsgCrc = m_pUtil->Crc32("NWB_Login");

	// 查看消息
	tagNWB_Login* pRecv = (tagNWB_Login*)p1;
	if( pRecv->dwID != dwFirstMsgCrc ) return GT_INVALID;

	// 如果现在没有初始化完毕或者正在关闭，则返回
	if( !g_Server.IsInitOK() || g_Server.IsShutingDown() ) return GT_INVALID;

	// 找到对应的world
	World* pWorld = GetWorld(pRecv->dwWorldID);
	if( !P_VALID(pWorld) ) return GT_INVALID;

	// 查看world是不是已经连接上了
	if( pWorld->IsValid() ) return GT_INVALID;

	// world启动
	pWorld->LoongWorldLogin();
	return pWorld->GetID();
}

//---------------------------------------------------------------------------------
// 登出请求
//---------------------------------------------------------------------------------
DWORD WorldMgr::LogoutCallBack(LPVOID p)
{
	DWORD dwWorldID = (DWORD)p;

	// 找到world
	World* pWorld = GetWorld(dwWorldID);

	if( P_VALID(pWorld) )
	{
		pWorld->LoongWorldLogout();
	}

	return 0;
}

//---------------------------------------------------------------------------------
// 消息处理——登入
//---------------------------------------------------------------------------------
DWORD WorldMgr::HandleWorldLogin(tagNetCmd* pCmd, World* pWorld)
{
	tagNBW_Login send;
	send.dwErrorCode = 0;

	SendMsg(pWorld->GetID(), &send, send.dwSize);

	return 0;
}

//---------------------------------------------------------------------------------
// 得到运行时信息
//---------------------------------------------------------------------------------
VOID WorldMgr::DisPlayerRunTimeInfo(TObjRef<Console>& pConsole)
{
	World* pWorld = NULL;
	TMap<DWORD, World*>::TMapIterator it = m_mapWorld.Begin();

	tstring strWorldConn;
	tstring strWorldDBConn;
	tstring strWorldYBSuccTimes;
	tstring strWorldItemSuccTimes;

	while( m_mapWorld.PeekNext(it, pWorld) )
	{
		TCHAR szTemp[X_SHORT_NAME] = {_T('\0')};
		_stprintf(szTemp, _T("%u_"), pWorld->GetID());

		// world信息
		strWorldConn.clear();
		strWorldDBConn.clear();
		strWorldYBSuccTimes.clear();
		strWorldItemSuccTimes.clear();

		strWorldConn			+=	szTemp;
		strWorldConn			+=	_T("wc");
		strWorldDBConn			+=	szTemp;
		strWorldDBConn			+=	_T("dbc");
		strWorldYBSuccTimes		+=	szTemp;
		strWorldYBSuccTimes		+=	_T("ybst");
		strWorldItemSuccTimes	+=	szTemp;
		strWorldItemSuccTimes	+=	_T("itemst");

		pConsole->Watch(strWorldConn.c_str(),			pWorld->IsValid());
		pConsole->Watch(strWorldDBConn.c_str(),			!pWorld->IsDBLost());
		pConsole->Watch(strWorldYBSuccTimes.c_str(),	pWorld->GetBillYuanbaoSuccessTimes());
		pConsole->Watch(strWorldItemSuccTimes.c_str(),	pWorld->GetBillItemSuccessTimes());
	}
}