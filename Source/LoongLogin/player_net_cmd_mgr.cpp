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

//-----------------------------------------------------------------------------
// construct
//-----------------------------------------------------------------------------
PlayerNetCmdMgr::PlayerNetCmdMgr()
{
	m_mapProc.Clear();
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
// 注册消息
//-----------------------------------------------------------------------------
BOOL PlayerNetCmdMgr::Register(LPCSTR szCmd, NETMSGHANDLER fp, LPCTSTR szDesc)
{
	DWORD dwID = m_pUtil->Crc32(szCmd);

	tagPlayerCmd* pCmd = m_mapProc.Peek(dwID);

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
		pCmd->handler = fp;
		pCmd->strCmd = szCmd;
		pCmd->strDesc = szDesc;
		m_mapProc.Add(dwID, pCmd);
	}

	return TRUE;
}

//------------------------------------------------------------------------------
// 取消注册
//------------------------------------------------------------------------------
VOID PlayerNetCmdMgr::UnRegisterAll()
{
	m_mapProc.ResetIterator();
	tagPlayerCmd* pCmd = NULL;
	while( m_mapProc.PeekNext(pCmd) )
	{
		SAFE_DEL(pCmd);
	}

	m_mapProc.Clear();
}

//------------------------------------------------------------------------------
// 得到某个消息ID对应的处理函数
//------------------------------------------------------------------------------
NETMSGHANDLER PlayerNetCmdMgr::GetHandler(tagNetCmd* pMsg, UINT32 nMsgSize)
{
	tagPlayerCmd* pCmd = m_mapProc.Peek(pMsg->dwID);
	if( !P_VALID(pCmd) )
	{
		IMSG(_T("Unknow player command recved[<cmdid>%d <size>%d]\r\n"), pMsg->dwID, nMsgSize);
		return NULL;
	}

	if( pMsg->dwSize != nMsgSize || nMsgSize > GT_MAX_PACKAGE_LEN )
	{
		IMSG(_T("Invalid net command size[<cmd>%u <size>%d]\r\n"), pMsg->dwID, pMsg->dwSize);
		return NULL;
	}

	++pCmd->nTimes;

	return pCmd->handler;
}

//------------------------------------------------------------------------------------------
// 执行消息处理函数
//------------------------------------------------------------------------------------------
BOOL PlayerNetCmdMgr::HandleCmd(tagNetCmd* pMsg, DWORD nMsgSize, Player* pSession)
{
	if( !P_VALID(pSession) ) return FALSE;

	NETMSGHANDLER fp = GetHandler(pMsg, nMsgSize);
	if( NULL == fp ) return FALSE;

	(pSession->*fp)(pMsg);

	return TRUE;
}
