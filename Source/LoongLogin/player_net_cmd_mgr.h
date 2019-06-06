//-----------------------------------------------------------------------------
//!\file player_net_cmd_mgr.h
//!\author Aslan
//!
//!\date 2008-06-13
//! last 2008-06-13
//!
//!\brief 客户端命令管理器，主要解决NetCmdMgr无法针对多对象消息处理的缺陷
//!
//!	Copyright (c) 2004 CTCRST Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

//-----------------------------------------------------------------------------
// 客户端命令管理器
//-----------------------------------------------------------------------------

class Player;

typedef DWORD (Player::*NETMSGHANDLER)(tagNetCmd*);

class PlayerNetCmdMgr
{
public:
	PlayerNetCmdMgr();
	~PlayerNetCmdMgr();

	VOID Destroy();

	BOOL Register(LPCSTR szCmd, NETMSGHANDLER fp, LPCTSTR szDesc);
	VOID UnRegisterAll();

	NETMSGHANDLER GetHandler(tagNetCmd* pMsg, UINT32 nMsgSize);

	BOOL HandleCmd(tagNetCmd* pMsg, DWORD nMsgSize, Player* pSession);
protected:
	typedef struct tagPlayerCmd
	{
		std::string				strCmd;		// 命令名
		tstring					strDesc;	// 描述
		NETMSGHANDLER			handler;	// 函数指针
		UINT32					nTimes;		// 收到此命令的次数
	} tagPlayerCmd;

	TObjRef<Util>				m_pUtil;

	TMap<DWORD, tagPlayerCmd*>	m_mapProc;
};

