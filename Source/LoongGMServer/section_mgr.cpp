//-----------------------------------------------------------------------------
//!\file section_mgr.cpp
//!\brief 大区管理器，管理该服务器管理的所有大区
//!
//!\date 2009-04-20
//! last 2009-04-20
//!\author zhangrong
//!
//! Copyright (c) 1985-2008 CTCRST Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#include "stdafx.h"

#include "../ServerDefine/rt_define.h"
#include "../ServerDefine/msg_rt_s.h"
#include "../ServerDefine/msg_gm_tool_c.h"
#include "../ServerDefine/base_define.h"

#include "game_server.h"
#include "game_world.h"
#include "server.h"
#include "section.h"
#include "section_mgr.h"
#include "client.h"

//-----------------------------------------------------------------------------
// 全局对象
//-----------------------------------------------------------------------------
SectionMgr g_SectionMgr;

//-----------------------------------------------------------------------------
// 构造函数
//-----------------------------------------------------------------------------
SectionMgr::SectionMgr() : m_Trunk(this), m_nPort(0), 
m_nSendInfoTickCountDown(SEND_INFO_TICK_COUNT_DOWN),
m_bSendImmediately(FALSE)
{

}

//------------------------------------------------------------------------------
// 初始化
//------------------------------------------------------------------------------
BOOL SectionMgr::Init()
{
	TObjRef<VarContainer> pVar = "VarContainer";

	// 加载所有大区
	if( FALSE == LoadAllServerInfo() )
	{
		return FALSE;
	}

	// 生成网络命令管理器
	CreateObj("NetCmdMgrServer", "NetCmdMgr");
	m_pCmdMgr = "NetCmdMgrServer";
	if( !P_VALID(m_pCmdMgr) ) return FALSE;

	// 生成服务器网络接口
	CreateObj("StreamServerServer", "StreamServer");
	m_pStreamServer= "StreamServerServer";
	if( !P_VALID(m_pStreamServer) ) return FALSE;

	// 注册网络命令
	RegisterServerMsg();

	// 初始化网络
	m_nPort = pVar->GetInt(_T("port"), _T("server"));
	m_pStreamServer->Init(	(TCPLOGINCALLBACK)m_Trunk.sfp2(&SectionMgr::LoginCallBack),
							(TCPLOGOUTCALLBACK)m_Trunk.sfp1(&SectionMgr::LogoutCallBack),
							m_nPort);

	return TRUE;
}

//------------------------------------------------------------------------------
// 加载所有管理的服务器配置
//------------------------------------------------------------------------------
BOOL SectionMgr::LoadAllServerInfo()
{
	TObjRef<VarContainer> pVar = "VarContainer";

	// 读取所有可以管理的大区个数
	INT nSectionCount = pVar->GetDword(_T("num"), _T("section"));
	if( nSectionCount <= 0 ) return FALSE;
	
	// 加载各个大区
	TCHAR szTemp[X_SHORT_NAME] = {'\0'};

	for(INT n = 0; n < nSectionCount; ++n)
	{
		_stprintf(szTemp, _T("section_%d"), n);

		LPCTSTR szSectionName = pVar->GetString(_T("name"), szTemp);

		// 计算出大区的ID
		DWORD dwSectionID = pVar->GetDword(_T("id"), szTemp);

		// 生成大区
		Section* pSection = new Section(dwSectionID, szSectionName);

		// 初始化大区
		if( FALSE == pSection->Init() )
		{
			SAFE_DEL(pSection);
			return FALSE;
		}

		// 加入到列表中
		m_mapSection.Add(pSection->GetID(), pSection);
	}

	return TRUE;
}

//------------------------------------------------------------------------------------
// 销毁
//------------------------------------------------------------------------------------
VOID SectionMgr::Destroy()
{
	// 删除所有的大区
	Section* pSection = NULL;
	TMap<DWORD, Section*>::TMapIterator it = m_mapSection.Begin();

	while( m_mapSection.PeekNext(it, pSection) )
	{
		pSection->Destroy();
		SAFE_DEL(pSection);
	}
	m_mapSection.Clear();

	// 注销网络
	if(P_VALID(m_pStreamServer))	
	{
		m_pStreamServer->Destroy();
		KillObj("StreamServerServer");
	}

	// 注销所有的网络命令
	UnRegisterServerMsg();
	KillObj("NetCmdMgrServer");
	
}

//--------------------------------------------------------------------------------------
// 更新
//--------------------------------------------------------------------------------------
VOID SectionMgr::Update()
{
	UpdateSession();
	SendAllServerDynamicInfo();
	
}

//---------------------------------------------------------------------------------------
// 更新所有网络消息
//---------------------------------------------------------------------------------------
VOID SectionMgr::UpdateSession()
{
	// 更新所有的大区
	Section* pSection = NULL;
	TMap<DWORD, Section*>::TMapIterator it = m_mapSection.Begin();

	while( m_mapSection.PeekNext(it, pSection) )
	{
		pSection->Update();
	}
}

//----------------------------------------------------------------------------------------
// 注册服务器的网络命令
//----------------------------------------------------------------------------------------
VOID SectionMgr::RegisterServerMsg()
{
	m_pCmdMgr->Register("NSC_LoginService",	(NETMSGPROC)m_Trunk.sfp2(&SectionMgr::HandleWorldLogin),		_T("NSC_LoginService"));
	m_pCmdMgr->Register("NSC_WorldInfo",	(NETMSGPROC)m_Trunk.sfp2(&SectionMgr::HandleGetWorldInfo),		_T("NSC_WorldInfo"));
	m_pCmdMgr->Register("NLSC_LoginService",(NETMSGPROC)m_Trunk.sfp2(&SectionMgr::HandleRTServiceLogin),	_T("NLSC_LoginService"));

	m_pCmdMgr->Register("NSC_Double",		(NETMSGPROC)m_Trunk.sfp2(&SectionMgr::HandleRetDouble),			_T("NSC_Double"));
	m_pCmdMgr->Register("NSC_AutoNotice",	(NETMSGPROC)m_Trunk.sfp2(&SectionMgr::HandleRetAutoNotice),		_T("NSC_AutoNotice"));
	m_pCmdMgr->Register("NSC_RightNotice",	(NETMSGPROC)m_Trunk.sfp2(&SectionMgr::HandleRetRightNotice),	_T("NSC_RightNotice"));
	m_pCmdMgr->Register("NSC_ForbidAccount",(NETMSGPROC)m_Trunk.sfp2(&SectionMgr::HandleRetAccountForbid),	_T("NSC_ForbidAccount"));
	m_pCmdMgr->Register("NSC_ForbidIP",		(NETMSGPROC)m_Trunk.sfp2(&SectionMgr::HandleRetIPBlacklist),	_T("NSC_ForbidIP"));
	m_pCmdMgr->Register("NSC_CancelDouble",	(NETMSGPROC)m_Trunk.sfp2(&SectionMgr::HandleRetCancelDouble),	_T("NSC_CancelDouble"));
	//m_pCmdMgr->Register("NSC_CancelRightNotice",	(NETMSGPROC)m_Trunk.sfp2(&SectionMgr::HandleRetCancelRightNotice),	_T("NSC_CancelRightNotice"));
}

//-----------------------------------------------------------------------------------------
// 注销服务器的网络命令
//-----------------------------------------------------------------------------------------
VOID SectionMgr::UnRegisterServerMsg()
{
	if(P_VALID(m_pCmdMgr))
	{
		m_pCmdMgr->Destroy();
		KillObj("NetCmdMgrServer");
	}	
}

//------------------------------------------------------------------------------------------
// 服务器登录回调
//------------------------------------------------------------------------------------------
DWORD SectionMgr::LoginCallBack(LPVOID p1, LPVOID p2)
{
	static DWORD dwFirstMsgCrc = IUTIL->Crc32("NSC_LoginService");

	// 查看消息
	tagNSC_LoginService* pRecv = (tagNSC_LoginService*)p1;
	if( pRecv->dwID != dwFirstMsgCrc ) return GT_INVALID;

	g_Server.Lock();

	// 找到对应的server
	GameServer* pServer = GetServer(pRecv->dwSectionID, pRecv->dwWorldID, pRecv->eType);
	if( !P_VALID(pServer) )
	{
		g_Server.UnLock();
		return GT_INVALID;
	}

	// 查看server是不是已经连接上了
	if( pServer->IsValid() )
	{
		g_Server.UnLock();
		return GT_INVALID;
	}

	// 查看能不能开启成功
	if( !pServer->ServerStart() )
	{
		g_Server.UnLock();
		return GT_INVALID;
	}

	g_Server.UnLock();
	return pServer->GetID();
}

//-------------------------------------------------------------------------------------------
// 服务器登出回调函数
//-------------------------------------------------------------------------------------------
DWORD SectionMgr::LogoutCallBack(LPVOID p1)
{
	DWORD dwID = (DWORD)p1;

	g_Server.Lock();

	// 找到server
	GameServer* pServer = FindServer(dwID);
	if( !P_VALID(pServer) )
	{
		g_Server.UnLock();
		return 0;
	}

	// 是否已经不在线
	if( !pServer->IsValid() )
	{
		g_Server.UnLock();
		return 0;
	}

	pServer->ServerShutDown();
	g_Server.UnLock();

	return 0;
}

//-------------------------------------------------------------------------------------------
// 生成管理的服务器的所有的静态信息给某个客户端
//-------------------------------------------------------------------------------------------
VOID SectionMgr::SendAllServerStaticInfoToClient(Client* pClient)
{
	static BYTE byBuff[1024*1024] = {0};		// 预先生成一个足够大的缓冲
	static DWORD dwMsgID = IUTIL->Crc32("NRTS_AllServerStatInfo");

	if( !P_VALID(pClient) ) return;

	tagNRTS_AllServerStatInfo* pSend = (tagNRTS_AllServerStatInfo*)byBuff;
	DWORD dwSize = sizeof(tagNRTS_AllServerStatInfo)-1;		// 消息总大小

	// 得到所有的大区数量
	pSend->nSectionNum = GetSectionNum();
	if( pSend->nSectionNum <= 0 ) return;

	// 让每个大区填充自己的静态信息
	LPBYTE pBuff = byBuff + dwSize;

	Section* pSection = NULL;
	TMap<DWORD, Section*>::TMapIterator it = m_mapSection.Begin();

	while( m_mapSection.PeekNext(it, pSection) )
	{
		pSection->GenerateSectionStaticInfo(pBuff, dwSize);
	}

	// 填写ID和size
	pSend->dwID = dwMsgID;
	pSend->dwSize = dwSize;

	// 发送消息
	pClient->SendMsg(byBuff, dwSize);
}

//----------------------------------------------------------------------------------------
// 发送所有服务器状态
//----------------------------------------------------------------------------------------
VOID SectionMgr::SendAllServerDynamicInfo()
{
	static BYTE byBuff[1024*1024] = {0};
	static DWORD dwMsgID = IUTIL->Crc32("NRTS_AllServerDynaInfo");

	// 更新倒计时
	if( --m_nSendInfoTickCountDown > 0 && !m_bSendImmediately)
	{
		return;
	}

	// 倒计时到了，进行发送
	m_bSendImmediately = FALSE;
	tagNRTS_AllServerDynaInfo* pSend = (tagNRTS_AllServerDynaInfo*)byBuff;
	DWORD dwSize = sizeof(tagNRTS_AllServerDynaInfo)-1;		// 消息总大小

	// 得到所有的大区数量
	pSend->nSectionNum = GetSectionNum();
	if( pSend->nSectionNum <= 0 ) return;

	// 让每个大区填充自己的静态信息
	LPBYTE pBuff = byBuff + dwSize;

	Section* pSection = NULL;
	TMap<DWORD, Section*>::TMapIterator it = m_mapSection.Begin();

	while( m_mapSection.PeekNext(it, pSection) )
	{
		pSection->GenerateSectionDynamicInfo(pBuff, dwSize);
	}

	// 填写ID和size
	pSend->dwID = dwMsgID;
	pSend->dwSize = dwSize;

	// 发送消息
	g_ClientMgr.SendAllClientMsg(byBuff, dwSize);
	_tprintf(_T("send msg to rtclient...server dynamic status.\n"));

	// 重置倒计时
	m_nSendInfoTickCountDown = SEND_INFO_TICK_COUNT_DOWN;
}

//-------------------------------------------------------------------------------------------
// 开启某个大区的Server
//-------------------------------------------------------------------------------------------
BOOL SectionMgr::StartServer(EServerType eType, DWORD dwSectionID, DWORD dwWorldID)
{
	Section* pSection = GetSection(dwSectionID);
	if( !P_VALID(pSection) ) return FALSE;

	BOOL bSuccess = FALSE;
	switch (eType)
	{
	case EST_Login:
		bSuccess = pSection->StartLoginServer();
		break;

	case EST_DB:
	case EST_World:
		bSuccess = pSection->StartWorld(dwWorldID, eType);
		break;
	}

	return bSuccess;
}

//-------------------------------------------------------------------------------------------
// 关闭指定大区的Server
//-------------------------------------------------------------------------------------------
BOOL SectionMgr::CloseServer(EServerType eType, DWORD dwSectionID, DWORD dwWorldID, BOOL bForce)
{
	Section* pSection = GetSection(dwSectionID);
	if( !P_VALID(pSection) ) return FALSE;

	BOOL bSuccess = FALSE;
	switch (eType)
	{
	case EST_Login:
		bSuccess = pSection->CloseLoginServer(bForce);
		break;

	case EST_DB:
	case EST_World:
		bSuccess = pSection->CloseWorld(dwWorldID, eType, bForce);
		break;
	}

	return bSuccess;
}

//-------------------------------------------------------------------------------------------
// 根据大区ID，游戏世界ID和类型得到一个gameserver
//-------------------------------------------------------------------------------------------
GameServer* SectionMgr::GetServer(DWORD dwSectionID, DWORD dwWorldID, EServerType eType)
{
	Section* pSection = GetSection(dwSectionID);
	if( !P_VALID(pSection) ) return FALSE;

	if( EST_Login == eType )
	{
		return pSection->GetLoginServer();
	}
	else
	{
		GameWorld* pWorld = pSection->GetWorld(dwWorldID);
		if( !P_VALID(pWorld) ) return NULL;

		if( EST_World == eType )
		{
			return pWorld->GetWorld();
		}
		else if( EST_DB == eType )
		{
			return pWorld->GetDB();
		}
		else
		{
			return NULL;
		}
	}
}

//-------------------------------------------------------------------------------------------------
// 根据SectionID和WorldID得到指定的ServerDB
//-------------------------------------------------------------------------------------------------
ServerDB* SectionMgr::GetServerDB( DWORD dwSectionID, DWORD dwWorldID )
{
	Section* pSection = g_SectionMgr.GetSection(dwSectionID);
	if (!P_VALID(pSection))	
		return NULL;

	GameWorld* pGameWorld = pSection->GetWorld(dwWorldID);
	if (!P_VALID(pGameWorld))
		return NULL;

	ServerDB* pSeverDB = pGameWorld->GetServerDB();

	return pSeverDB;
}

//-------------------------------------------------------------------------------------------
// 找到一个ID指定的server
//-------------------------------------------------------------------------------------------
GameServer* SectionMgr::FindServer(DWORD dwID)
{
	// 首先根据ID找到对应的大区ID
	DWORD dwSectionID = m_mapIDSectionID.Peek(dwID);

	if( !P_VALID(dwSectionID) ) return NULL;

	// 找到对应的大区
	Section* pSection = m_mapSection.Peek(dwSectionID);
	if( !P_VALID(pSection) ) return NULL;

	// 在大区里面找到服务器
	return pSection->FindServer(dwID);
}

//---------------------------------------------------------------------------------------------
// 根据制定的ID得到指定的loginServer
//---------------------------------------------------------------------------------------------
GameServerLogin* SectionMgr::FindLoginServer(DWORD dwID)
{
	// 首先根据ID找到对应的大区ID
	DWORD dwSectionID = m_mapIDSectionID.Peek(dwID);

	if( !P_VALID(dwSectionID) ) return NULL;

	// 找到对应的大区
	Section* pSection = m_mapSection.Peek(dwSectionID);
	if( !P_VALID(pSection) ) return NULL;

	// 在大区里面找到服务器
	return pSection->FindLoginServer(dwID);
}

//-----------------------------------------------------------------------------------------------
// 根据制定的ID得到指定的loongworld
//-----------------------------------------------------------------------------------------------
GameServerWorld* SectionMgr::FindWorldServer(DWORD dwID)
{
	// 首先根据ID找到对应的大区ID
	DWORD dwSectionID = m_mapIDSectionID.Peek(dwID);

	if( !P_VALID(dwSectionID) ) return NULL;

	// 找到对应的大区
	Section* pSection = m_mapSection.Peek(dwSectionID);
	if( !P_VALID(pSection) ) return NULL;

	// 在大区里面找到服务器
	return pSection->FindWorldServer(dwID);
}

//-------------------------------------------------------------------------------------------------
// 根据制定的ID得到指定的loongdb
//-------------------------------------------------------------------------------------------------
GameServerDB* SectionMgr::FindDBServer(DWORD dwID)
{
	// 首先根据ID找到对应的大区ID
	DWORD dwSectionID = m_mapIDSectionID.Peek(dwID);

	if( !P_VALID(dwSectionID) ) return NULL;

	// 找到对应的大区
	Section* pSection = m_mapSection.Peek(dwSectionID);
	if( !P_VALID(pSection) ) return NULL;

	// 在大区里面找到服务器
	return pSection->FindDBServer(dwID);
}

//-------------------------------------------------------------------------------------------------
// 根据制定的ID得到指定的GameWorld
//-------------------------------------------------------------------------------------------------
GameWorld* SectionMgr::FindGameWorld(DWORD dwID)
{
	// 首先根据ID找到对应的大区ID
	DWORD dwSectionID = m_mapIDSectionID.Peek(dwID);

	if( !P_VALID(dwSectionID) ) return NULL;

	// 找到对应的大区
	Section* pSection = m_mapSection.Peek(dwSectionID);
	if( !P_VALID(pSection) ) return NULL;

	return pSection->FindGameWorld(dwID);
}

// 根据大区名字找到大区
Section* SectionMgr::GetSection(LPCTSTR szSection)
{
	TMap<DWORD, Section*>::TMapIterator it = m_mapSection.Begin();
	Section* pSection = NULL;
	while (m_mapSection.PeekNext(it, pSection))
	{
		if ( _tcscmp(pSection->GetSectionName(), szSection) == 0 )
		{
			return pSection;
		}
	}

	return NULL;
}

//---------------------------------------------------------------------------------------------------
// 处理登陆
//---------------------------------------------------------------------------------------------------
DWORD SectionMgr::HandleWorldLogin(tagNetCmd* pMsg, DWORD dwID)
{
	tagNSC_LoginService* pRecv = (tagNSC_LoginService*)pMsg;

	// 找到server
	GameServer* pServer = FindServer(dwID);
	if( !P_VALID(pServer) ) return GT_INVALID;

	//pServer->m_dwLastestTick = GetCurrentDWORDTime();
	pServer->RefreshLastTickTime();

	// 发送返回消息
	tagNSS_LoginService send;
	send.dwErrorCode = 0;

	SendMsg(dwID, &send, send.dwSize);

	return 0;
}

//----------------------------------------------------------------------------------------------------
// 处理服务器状态
//----------------------------------------------------------------------------------------------------
DWORD SectionMgr::HandleGetWorldInfo(tagNetCmd* pMsg, DWORD dwID)
{
	_tprintf(_T("msg from game server...server dynamic status.\n"));
	tagNSC_WorldInfo* pRecv = (tagNSC_WorldInfo*)pMsg;

	// 找到server
	GameServer* pServer = FindServer(dwID);
	if( !P_VALID(pServer) ) return GT_INVALID;
	
	// 更新最近心跳时间
	//pServer->m_dwLastestTick = GetCurrentDWORDTime();
	pServer->RefreshLastTickTime();

	// 更新该server的状态
	pServer->RefreshStatus(pRecv->eStatus, pRecv->nOnlineNum, pRecv->nMaxOnlineNum, pRecv->nOnlineNumDisplay);

	return 0;
}

//----------------------------------------------------------------------------------------------------
// RTService登陆确认
//----------------------------------------------------------------------------------------------------
DWORD SectionMgr::HandleRTServiceLogin(tagNetCmd* pMsg, DWORD)
{
	MGET_MSG(pRecv, pMsg, NLSC_LoginService);

	return 0;
}

//----------------------------------------------------------------------------------------------------
// 向GMserver返回操作结果
//----------------------------------------------------------------------------------------------------
DWORD SectionMgr::HandleRetDouble(tagNetCmd* pMsg, DWORD dwID)
{
	MGET_MSG(pRecv, pMsg, NSC_Double);

	GameWorld* pWorld = g_SectionMgr.FindGameWorld(dwID);
	if (!P_VALID(pWorld))
	{
		return GT_INVALID;
	}

	if ( pRecv->dwErrorCode != 0 )
	{
		tagNGMTS_TimeLimitedMsg send;
		send.eType = ETLM_Double;
		send.dwErrorCode	= pRecv->dwErrorCode;
		send.dwWorldID		= pWorld->GetID();
		g_ClientMgr.SendMsg(pRecv->dwClientID, &send, send.dwSize);
	}
	return 0;
}

DWORD SectionMgr::HandleRetAutoNotice( tagNetCmd* pMsg, DWORD dwID )
{
	MGET_MSG(pRecv, pMsg, NSC_AutoNotice);

	GameWorld* pWorld = g_SectionMgr.FindGameWorld(dwID);
	if (!P_VALID(pWorld))
	{
		return GT_INVALID;
	}
	
	if ( pRecv->dwErrorCode != 0 )
	{
		tagNGMTS_TimeLimitedMsg send;
		send.eType = ETLM_AutoNotice;
		send.dwErrorCode	= pRecv->dwErrorCode;
		send.dwWorldID		= pWorld->GetID();
		g_ClientMgr.SendMsg(pRecv->dwClientID, &send, send.dwSize);
	}

	return 0;
}

DWORD SectionMgr::HandleRetRightNotice( tagNetCmd* pMsg, DWORD dwID )
{
	MGET_MSG(pRecv, pMsg, NSC_RightNotice);

	GameWorld* pWorld = g_SectionMgr.FindGameWorld(dwID);
	if (!P_VALID(pWorld))
	{
		return GT_INVALID;
	}

	if ( pRecv->dwErrorCode != 0 )
	{
		tagNGMTS_TimeLimitedMsg send;
		send.eType = ETLM_RightNotice;
		send.dwErrorCode = pRecv->dwErrorCode;
		send.dwWorldID		= pWorld->GetID();
		g_ClientMgr.SendMsg(pRecv->dwClientID, &send, send.dwSize);
	}
	
	return 0;
}

DWORD SectionMgr::HandleRetAccountForbid(tagNetCmd* pMsg, DWORD dwID)
{
	MGET_MSG(pRecv, pMsg, NSC_ForbidAccount);

	tagNGMTS_AccountForbid send;
	//send.bLast = TRUE; // 默认情况下是最后一条消息 
	_tcscpy(send.szAccount, pRecv->szAccount);
	send.dwErrorCode = pRecv->dwErrorCode;

	g_ClientMgr.SendMsg(pRecv->dwClientID, &send, send.dwSize);

	return 0;
}

DWORD SectionMgr::HandleRetIPBlacklist(tagNetCmd* pMsg, DWORD dwID)
{
	MGET_MSG(pRecv, pMsg, NSC_ForbidIP);

	tagNGMTS_Blacklist send;
	send.dwErrorCode = pRecv->dwErrorCode;

	g_ClientMgr.SendMsg(pRecv->dwClientID, &send, send.dwSize);

	return 0;
}

DWORD SectionMgr::HandleRetCancelDouble(tagNetCmd* pMsg, DWORD dwID)
{
	MGET_MSG(pRecv, pMsg, NSC_CancelDouble);

	tagNGMTS_CancelDoubleMsg send;
	send.dwErrorCode = pRecv->dwErrorCode;

	g_ClientMgr.SendMsg(pRecv->dwClientID, &send, send.dwSize);

	return 0;
}

//DWORD SectionMgr::HandleRetCancelRightNotice(tagNetCmd* pMsg, DWORD dwID)
//{
//	MGET_MSG(pRecv, pMsg, NSC_CancelRightNotice);
//
//	tagNGMTS_CancelRightNotice send;
//	send.dwErrorCode = pRecv->dwErrorCode;
//
//	g_ClientMgr.SendMsg(pRecv->dwClientID, &send, send.dwSize);
//
//	return 0;
//}