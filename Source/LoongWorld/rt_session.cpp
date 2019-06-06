//-----------------------------------------------------------------------------
//!\file to_rt_session.cpp
//!\author Lyp
//!
//!\date 2005-04-05
//! last 2005-04-05
//!
//!\brief 量子区域服务器与服务器控制服务的对话层
//!
//!	Copyright (c) 2005 TENGWU Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#include "stdafx.h"

#include "../ServerDefine/msg_rt_s.h"
#include "../ServerDefine/base_define.h"
#include "rt_session.h"
#include "world.h"
#include "world_session.h"
#include "gm_policy.h"
#include "role_mgr.h"
#include "../WorldDefine/mall_define.h"
#include "mall.h"
#include "../WorldDefine/chat.h"
#include "channel_mgr.h"
#include "../WorldDefine/chat_define.h"
//-----------------------------------------------------------------------------
//! construction
//-----------------------------------------------------------------------------
RTSession::RTSession() : m_Trunk(this)
{
	m_bInitOK					=	FALSE;
	m_bTermConnect				=	FALSE;
	m_nSendInfoTickCountDown	=	SEND_INFO_INTERVAL;
}

//-----------------------------------------------------------------------------
//! destruction
//-----------------------------------------------------------------------------
RTSession::~RTSession()
{
}

//-----------------------------------------------------------------------------
// init
//-----------------------------------------------------------------------------
BOOL RTSession::Init()
{
	m_pThread		=	"Thread";
	m_pUtil			=	"Util";

	// 初始化成员属性
	TObjRef<VarContainer> pVar = "VarContainer";
	m_strIP			=	pVar->GetString(_T("ip gm_server"));
	m_dwPort		=	pVar->GetDword(_T("port gm_server"));
	m_dwSectionID	=	pVar->GetDword(_T("section_id world"));
	m_dwWorldID		=	pVar->GetDword(_T("id world"));

	// 创建消息管理
	CreateObj("ToRtNetCmdMgr", "NetCmdMgr");
	m_pMsgCmdMgr = "ToRtNetCmdMgr";

	// 创建连接对象，并初始化
	CreateObj("ToRT", "StreamTransport");
	m_pTran = "ToRT";
	if( !P_VALID(m_pTran) )
	{
		ERR(_T("Create ToRT(StreamTransport) obj failed!\r\n"));
		return FALSE;
	}
	m_pTran->Init();

	// 注册所有网络命令
	RegisterAllRTCommand();

	// 启动连接线程
	if(!m_pThread->CreateThread(_T("ConnectRT"), 
		(vEngine::THREADPROC)m_Trunk.sfp0(&RTSession::ThreadConnectRT), NULL))
	{
		return FALSE;
	}

	while( !m_pThread->IsThreadActive(_T("ConnectRT")) )
	{
		continue;
	}
	return TRUE;
}

//-----------------------------------------------------------------------------
// destroy
//-----------------------------------------------------------------------------
VOID RTSession::Destroy()
{
	// 等待所有线程结束
	InterlockedExchange((LONG*)&m_bTermConnect, TRUE);
	m_pThread->WaitForThreadDestroy(_T("ConnectRT"), INFINITE);

	m_pTran->Destroy();
	KillObj("ToRT");

	// 注销消息管理
	m_pMsgCmdMgr->Destroy();
	KillObj("ToRtNetCmdMgr");
}



//-----------------------------------------------------------------------------
// update
//-----------------------------------------------------------------------------
VOID RTSession::Update()
{
	UpdateSession();
	SendServerInfo();
}

//-----------------------------------------------------------------------------
// 接收消息
//-----------------------------------------------------------------------------
VOID RTSession::UpdateSession()
{
	if( NULL == m_pTran )
		return;

	if( !m_pTran->IsConnected() && !m_pThread->IsThreadActive(_T("ConnectRT")) )
	{
		InterlockedExchange((LONG*)&m_bTermConnect, TRUE);
		m_pTran->Disconnect();

		m_pThread->WaitForThreadDestroy(_T("ConnectRT"), INFINITE);

		// 重新启动登陆服务器连接线程
		InterlockedExchange((LONG*)&m_bTermConnect, FALSE);
		m_pThread->CreateThread(_T("ConnectRT"), (vEngine::THREADPROC)m_Trunk.sfp0(&RTSession::ThreadConnectRT), NULL);

		while(FALSE == m_pThread->IsThreadActive(_T("ConnectRT")))
		{
			continue;
		}

		return;
	}

	while(TRUE)
	{
		DWORD dwSize = 0;
		LPBYTE pRecv = m_pTran->Recv(dwSize);
		if( !P_VALID(pRecv) )
			break;

		// 处理消息
		m_pMsgCmdMgr->HandleCmd((tagNetCmd*)pRecv, dwSize, GT_INVALID);

		// 回收资源
		m_pTran->FreeRecved(pRecv);

		
	}	
}

//-----------------------------------------------------------------------------
// 发送服务器情况
//-----------------------------------------------------------------------------
VOID RTSession::SendServerInfo()
{
	if( !m_pTran->IsConnected() || !m_bInitOK ) return;

	if( --m_nSendInfoTickCountDown > 0 )
		return;

	m_nSendInfoTickCountDown = SEND_INFO_INTERVAL;

	tagNSC_WorldInfo send;
	
	if( g_world.IsWell() )
	{
		send.eStatus = EWS_Well;
	}
	else
	{
		send.eStatus = EWS_SystemError;
	}
	send.nMaxOnlineNum = g_worldSession.GetPlayerNumLimit();
	send.nOnlineNum = g_worldSession.GetPlayerNumCurrent();
	send.nOnlineNumDisplay = g_worldSession.GetPlayerNumDisplay();
	m_pTran->Send(&send, send.dwSize);
}

//-----------------------------------------------------------------------------
// 连接线程(连接监控服务器)
//-----------------------------------------------------------------------------
DWORD RTSession::ThreadConnectRT()
{
#ifdef _DEBUG
	EXCEPTION_PROTECT_START;
#endif

	while( FALSE == m_bTermConnect )
	{
		if( !m_pTran->IsConnected() )
		{
			if( !m_pTran->IsTryingToConnect() )
			{
				m_pTran->TryToConnect(m_pUtil->UnicodeToUnicode8(m_strIP.c_str()), m_dwPort);
			}

			Sleep(100);
			continue;	// 重新检测连接
		}

		IMSG(_T("Contected to RT Server at %s: %d\r\n"), m_strIP.c_str(), m_dwPort);

		
		tagNSC_LoginService send;
		send.dwSectionID	=	m_dwSectionID;
		send.dwWorldID		=	m_dwWorldID;
		send.eType			=	EST_World;
		m_pTran->Send(&send, send.dwSize);

		break;
	}

#ifdef _DEBUG
	EXCEPTION_PROTECT_END;
#endif
	return 0;
}


//----------------------------------------------------------------------------------------------
// 注册所有的网络命令
//----------------------------------------------------------------------------------------------
VOID RTSession::RegisterAllRTCommand()
{
	m_pMsgCmdMgr->Register("NSS_LoginService",		(NETMSGPROC)m_Trunk.sfp2(&RTSession::HandleServerLogin),	_T("RT Server Certification Reply"));
	m_pMsgCmdMgr->Register("NSS_Close",				(NETMSGPROC)m_Trunk.sfp2(&RTSession::HandleCloseServer),	_T("Close Server"));
	m_pMsgCmdMgr->Register("NSS_Double",			(NETMSGPROC)m_Trunk.sfp2(&RTSession::HandleDouble),			_T("Double Rate"));
	m_pMsgCmdMgr->Register("NSS_AutoNotice",		(NETMSGPROC)m_Trunk.sfp2(&RTSession::HandleAutoNotice),		_T("AutoNotice"));
	m_pMsgCmdMgr->Register("NSS_RightNotice",		(NETMSGPROC)m_Trunk.sfp2(&RTSession::HandleRightNotice),	_T("RightNotice"));
	m_pMsgCmdMgr->Register("NSS_MaxNum",			(NETMSGPROC)m_Trunk.sfp2(&RTSession::HandleSetMaxNum),		_T("Set Max User Online"));
	m_pMsgCmdMgr->Register("NSS_UpdateMall",		(NETMSGPROC)m_Trunk.sfp2(&RTSession::HandleUpdateMall),		_T("Update Mall"));
	m_pMsgCmdMgr->Register("NSS_AutoChatNotice",	(NETMSGPROC)m_Trunk.sfp2(&RTSession::HandleAutoChatNotice),	_T("Chat Notice"));
	m_pMsgCmdMgr->Register("NSS_CancelDouble",		(NETMSGPROC)m_Trunk.sfp2(&RTSession::HandleCancelDouble),	_T("Cancel Double"));
	m_pMsgCmdMgr->Register("NSS_ClearBaibao",		(NETMSGPROC)m_Trunk.sfp2(&RTSession::HandleClearBaibao),	_T("Clear Baibao"));
	m_pMsgCmdMgr->Register("NSS_SetFackNumer",		(NETMSGPROC)m_Trunk.sfp2(&RTSession::HandleSetFackNum),		_T("Set online number display"));
}

//----------------------------------------------------------------------------------------------
// 服务器认证网络消息
//----------------------------------------------------------------------------------------------
DWORD RTSession::HandleServerLogin(tagNetCmd* pMsg, DWORD)
{
	tagNSS_LoginService* pRecv = (tagNSS_LoginService*)pMsg;

	InterlockedExchange((LPLONG)&m_bInitOK, TRUE);
	return 0;
}

//-----------------------------------------------------------------------------------------------
// 服务器关闭网络消息
//-----------------------------------------------------------------------------------------------
DWORD RTSession::HandleCloseServer(tagNetCmd* pMsg, DWORD)
{
	g_world.ShutDown();
	return 0;
}

//-----------------------------------------------------------------------------------------------
// 设置双倍
//-----------------------------------------------------------------------------------------------
DWORD RTSession::HandleDouble( tagNetCmd* pMsg, DWORD )
{
	MGET_MSG(pRecv, pMsg, NSS_Double);

	g_GMPolicy.SetRate(pRecv->eDType, pRecv->dwRatio, pRecv->dwOpenTime, pRecv->dwLastTime);

	tagNSC_Double send;
	send.dwErrorCode	= 0;
	send.dwClientID		= pRecv->dwClientID;
	m_pTran->Send(&send, send.dwSize);

	return 0;
}

//-----------------------------------------------------------------------------------------------
// 设置自动循环公告
//-----------------------------------------------------------------------------------------------
DWORD RTSession::HandleAutoNotice( tagNetCmd* pMsg, DWORD )
{
	MGET_MSG(pSend, pMsg, NSS_AutoNotice);

	DWORD dwLen = sizeof(tagNS_AutoNotice) - sizeof(TCHAR) + 2*(wcslen(pSend->szContent) + 1);

	MCREATE_MSG(pSendMsg, dwLen, NS_AutoNotice);

	pSendMsg->nCirInterval = pSend->nCirInterval;
	pSendMsg->nType = pSend->eType;
	_tcscpy(pSendMsg->szContent, pSend->szContent);
	
	g_roleMgr.SendWorldMsg(pSendMsg, dwLen);

	MDEL_MSG(pSendMsg);

	tagNSC_AutoNotice msg;
	msg.dwErrorCode	= 0;
	msg.dwClientID		= pSend->dwClientID;
	m_pTran->Send(&msg, msg.dwSize);

	return 0;
}

//-----------------------------------------------------------------------------------------------
// 设置右下角公告
//-----------------------------------------------------------------------------------------------
DWORD RTSession::HandleRightNotice(tagNetCmd* pMsg, DWORD)
{
	MGET_MSG(pSend, pMsg, NSS_RightNotice);

	DWORD dwLen = sizeof(tagNS_RightNotice) - sizeof(TCHAR) + 2*( wcslen(pSend->szTitle) + wcslen(pSend->szContent) + wcslen(pSend->szLink) +1);
	
	MCREATE_MSG(pSendMsg, dwLen, NS_RightNotice);

	pSendMsg->nTitleLen = wcslen(pSend->szTitle);
	pSendMsg->nContentLen = wcslen(pSend->szContent);
	pSendMsg->nLinkLen = wcslen(pSend->szLink);

	_tcscpy(pSendMsg->szContent, pSend->szTitle);
	_tcscat(pSendMsg->szContent, pSend->szLink);
	_tcscat(pSendMsg->szContent, pSend->szContent);

	g_roleMgr.SendWorldMsg(pSendMsg, dwLen);

	MDEL_MSG(pSendMsg);

	tagNSC_RightNotice msg;
	msg.dwErrorCode	= 0;
	msg.dwClientID		= pSend->dwClientID;
	m_pTran->Send(&msg, msg.dwSize);

	return 0;
}

//-----------------------------------------------------------------------------------------------
// 设置游戏最大在线人数
//-----------------------------------------------------------------------------------------------
DWORD RTSession::HandleSetMaxNum(tagNetCmd* pMsg, DWORD)
{
	MGET_MSG(pSend, pMsg, NSS_MaxNum);

	g_worldSession.SetPlayerNumLimit(pSend->nMaxNum);

	tagNSC_MaxNum send;
	send.dwClientID = pSend->dwClientID;
	send.dwErrorCode = 0;
	m_pTran->Send(&send, send.dwSize);

	return 0;
}

//-----------------------------------------------------------------------------------------------
// 更新商城
//-----------------------------------------------------------------------------------------------
DWORD RTSession::HandleUpdateMall(tagNetCmd* pMsg, DWORD)
{
	MGET_MSG(pSend, pMsg, NSS_UpdateMall);
	
	BOOL bSuccess = FALSE;
	bSuccess = g_mall.ReInit();

	tagNSC_UpdateMall send;
	send.dwClientID = pSend->dwClientID;
	send.dwErrorCode = bSuccess;
	m_pTran->Send(&send, send.dwSize);

	return 0;
}

DWORD RTSession::HandleAutoChatNotice(tagNetCmd* pMsg, DWORD)
{
	MGET_MSG(pRecv, pMsg, NSS_AutoChatNotice);

	DWORD dwSize = sizeof(tagNS_RoleChat) + X_HUGE_STRING;
	MCREATE_MSG(pSend, dwSize, NS_RoleChat);
	pSend->byChannel		= (BYTE)ESCC_Affiche;
	pSend->dwID				= TObjRef<Util>()->Crc32("NS_RoleChat");
	pSend->dwDestRoleID		= GT_INVALID;
	pSend->dwSrcRoleID		= GT_INVALID;
	pSend->dwErrorCode		= 0;
	pSend->dwSize			= dwSize;
	IFASTCODE->MemCpy(pSend->szMsg, pRecv->szNotice, X_HUGE_STRING);

	g_roleMgr.ForEachRoleInWorld(SendAction(ESCC_Affiche, pSend));

	MDEL_MSG(pSend);
	return 0;
}

DWORD RTSession::HandleCancelDouble(tagNetCmd* pMsg, DWORD)
{
	MGET_MSG(pRecv, pMsg, NSS_CancelDouble);

	g_GMPolicy.DoubleSwitch(EDoubleType_Exp, FALSE);
	g_GMPolicy.DoubleSwitch(EDoubleType_Item, FALSE);

	tagNSC_CancelDouble send;
	send.dwErrorCode	= 0;
	send.dwClientID		= pRecv->dwClientID;
	m_pTran->Send(&send, send.dwSize);

	return 0;
}

DWORD RTSession::HandleClearBaibao(tagNetCmd* pMsg, DWORD)
{
	//MGET_MSG(pRecv, pMsg, NSS_ClearBaibao);

	//DWORD dwNameCrc = m_pUtil->Crc32(pRecv->szOwnerName);
	//Role* pRole = g_roleMgr.GetRoleIDByNameCrc(dwNameCrc);
	//if(!P_VALID(pRole)) return FALSE;

	//pRole->GetItemMgr().GetBaiBaoBag()

	return E_Success;
}

DWORD RTSession::HandleSetFackNum(tagNetCmd* pMsg, DWORD)
{
	MGET_MSG(pSend, pMsg, NSS_SetFackNumer);

	g_worldSession.SetPlayNumberFactor(pSend->fFackRate);

	return E_Success;
}


RTSession g_rtSession;
