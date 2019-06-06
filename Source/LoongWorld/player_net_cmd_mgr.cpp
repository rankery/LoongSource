//-----------------------------------------------------------------------------
//!\file player_net_cmd_mgr.h
//!\author Aslan
//!
//!\date 2008-06-13
//! last 2005-06-13
//!
//!\brief 客户端命令管理器
//!
//!	Copyright (c) 2004 CTCRST Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "player_net_cmd_mgr.h"
#include "player_session.h"

//-----------------------------------------------------------------------------
// construct
//-----------------------------------------------------------------------------
PlayerNetCmdMgr::PlayerNetCmdMgr()
{
	m_mapRecvProc.Clear();
	m_mapSendProc.Clear();
}

//-----------------------------------------------------------------------------
// destruct
//-----------------------------------------------------------------------------
PlayerNetCmdMgr::~PlayerNetCmdMgr()
{
	Destroy();
}

//-----------------------------------------------------------------------------
// destroy
//-----------------------------------------------------------------------------
VOID PlayerNetCmdMgr::Destroy()
{
}

//-----------------------------------------------------------------------------
// 打印所有的网络消息统计信息到log
//-----------------------------------------------------------------------------
VOID PlayerNetCmdMgr::LogAllMsg()
{
	tagPlayerCmd* pCmd = NULL;

	m_pLog = "Log";
	m_pUtil = "Util";

	m_pLog->Write(_T("\r\n\r\n"));

	// 首先打印所有的客户端消息
	m_pLog->Write(_T("Client Msg Statistics:\r\n"));

	m_mapRecvProc.ResetIterator();
	while( m_mapRecvProc.PeekNext(pCmd) )
	{
		m_pLog->Write(_T("%s\t\t%u\r\n"), m_pUtil->Unicode8ToUnicode(pCmd->strCmd.c_str()), pCmd->nTimes);
	}
	m_pLog->Write(_T("\r\n\r\n"));

	// 再打印服务器端的消息
	m_pLog->Write(_T("Server Msg Statistics:\r\n"));

	m_mapSendProc.ResetIterator();
	while( m_mapSendProc.PeekNext(pCmd) )
	{
		m_pLog->Write(_T("%s\t\t%u\r\n"), m_pUtil->Unicode8ToUnicode(pCmd->strCmd.c_str()), pCmd->nTimes);
	}
	m_pLog->Write(_T("\r\n\r\n"));
}

//-----------------------------------------------------------------------------
// 注册接收消息
//-----------------------------------------------------------------------------
BOOL PlayerNetCmdMgr::RegisterRecvProc(LPCSTR szCmd, NETMSGHANDLER fp, LPCTSTR szDesc, DWORD dwSize)
{
	DWORD dwID = m_pUtil->Crc32(szCmd);

	tagPlayerCmd* pCmd = m_mapRecvProc.Peek(dwID);

	if( P_VALID(pCmd) )
	{
		if( pCmd->strCmd != szCmd )
		{
			ASSERT(0);	// 两个命令拥有相同的CRC
			return FALSE;
		}
	}
	else
	{
		pCmd = new tagPlayerCmd;
		pCmd->nTimes = 0;
		pCmd->dwSize = dwSize;
		pCmd->handler = fp;
		pCmd->strCmd = szCmd;
		pCmd->strDesc = szDesc;
		m_mapRecvProc.Add(dwID, pCmd);
	}

	return TRUE;
}

//------------------------------------------------------------------------------
// 注册发送消息
//------------------------------------------------------------------------------
BOOL PlayerNetCmdMgr::RegisterSendProc(LPCSTR szCmd)
{
	DWORD dwID = m_pUtil->Crc32(szCmd);

	tagPlayerCmd* pCmd = m_mapSendProc.Peek(dwID);

	if( P_VALID(pCmd) )
	{
		if( pCmd->strCmd != szCmd )
		{
			ASSERT(0);
			return FALSE;
		}
	}
	else
	{
		pCmd = new tagPlayerCmd;
		pCmd->nTimes = 0;
		pCmd->dwSize = 0;
		pCmd->handler = NULL;
		pCmd->strCmd = szCmd;
		m_mapSendProc.Add(dwID, pCmd);
	}

	return TRUE;
}

//------------------------------------------------------------------------------
// 取消注册
//------------------------------------------------------------------------------
VOID PlayerNetCmdMgr::UnRegisterAll()
{
	tagPlayerCmd* pCmd = NULL;

	m_mapRecvProc.ResetIterator();
	while( m_mapRecvProc.PeekNext(pCmd) )
	{
		SAFE_DEL(pCmd);
	}
	m_mapRecvProc.Clear();

	m_mapSendProc.ResetIterator();
	while( m_mapSendProc.PeekNext(pCmd) )
	{
		SAFE_DEL(pCmd);
	}
	m_mapSendProc.Clear();
}

//------------------------------------------------------------------------------
// 得到某个消息ID对应的处理函数
//------------------------------------------------------------------------------
NETMSGHANDLER PlayerNetCmdMgr::GetHandler(tagNetCmd* pMsg, UINT32 nMsgSize)
{
	tagPlayerCmd* pCmd = m_mapRecvProc.Peek(pMsg->dwID);
	if( !P_VALID(pCmd) )
	{
		IMSG(_T("Unknow player command recved[<cmdid>%u <size>%d]\r\n"), pMsg->dwID, nMsgSize);
		return NULL;
	}

	if( pMsg->dwSize != nMsgSize || nMsgSize > GT_MAX_PACKAGE_LEN || nMsgSize < pCmd->dwSize )
	{
		IMSG(_T("Invalid net command size[<cmd>%u <size>%d]\r\n"), pMsg->dwID, pMsg->dwSize);
		return NULL;
	}

	InterlockedExchangeAdd((LPLONG)&pCmd->nTimes, 1);

	return pCmd->handler;
}

//------------------------------------------------------------------------------------------
// 执行消息处理函数
//------------------------------------------------------------------------------------------
BOOL PlayerNetCmdMgr::HandleCmd(tagNetCmd* pMsg, DWORD nMsgSize, PlayerSession* pSession)
{
	if( !P_VALID(pSession) ) return FALSE;

	NETMSGHANDLER fp = GetHandler(pMsg, nMsgSize);
	if( NULL == fp ) return FALSE;

	(pSession->*fp)(pMsg);

	return TRUE;
}

//-------------------------------------------------------------------------------------------
// 服务器端的发包计数
//-------------------------------------------------------------------------------------------
VOID PlayerNetCmdMgr::CountServerMsg(DWORD dwMsgID)
{
	tagPlayerCmd* pCmd = m_mapSendProc.Peek(dwMsgID);

	if( P_VALID(pCmd) )
	{
		InterlockedExchangeAdd((LPLONG)&pCmd->nTimes, 1);
	}
}

//-------------------------------------------------------------------------------------------
// 取得接收命令执行次数
//-------------------------------------------------------------------------------------------
UINT32 PlayerNetCmdMgr::GetRecvCmdRunTimes( DWORD dwMsgID )
{
	tagPlayerCmd* pCmd = m_mapRecvProc.Peek(dwMsgID);

	if (P_VALID(pCmd))
	{
		return pCmd->nTimes;
	}

	return GT_INVALID;
}
