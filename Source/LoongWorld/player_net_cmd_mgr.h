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

class PlayerSession;

typedef DWORD (PlayerSession::*NETMSGHANDLER)(tagNetCmd*);

//-----------------------------------------------------------------------------
// 客户端命令管理器
//-----------------------------------------------------------------------------
class PlayerNetCmdMgr
{
public:
	PlayerNetCmdMgr();
	~PlayerNetCmdMgr();

	VOID Init();
	VOID Destroy();

	VOID LogAllMsg();

	BOOL RegisterRecvProc(LPCSTR szCmd, NETMSGHANDLER fp, LPCTSTR szDesc, DWORD dwSize);
	BOOL RegisterSendProc(LPCSTR szCmd);
	VOID UnRegisterAll();

	NETMSGHANDLER GetHandler(tagNetCmd* pMsg, UINT32 nMsgSize);
	VOID CountServerMsg(DWORD dwMsgID);

	BOOL HandleCmd(tagNetCmd* pMsg, DWORD nMsgSize, PlayerSession* pSession);

	// 取得消息执行次数
	UINT32 GetRecvCmdRunTimes(DWORD dwMsgID);


protected:
	typedef struct tagPlayerCmd
	{
		std::string				strCmd;			// 命令名
		tstring					strDesc;		// 描述
		DWORD					dwSize;			// 消息大小
		NETMSGHANDLER			handler;		// 函数指针
		volatile UINT32			nTimes;			// 此命令的次数
	} tagPlayerCmd;

	TObjRef<Util>				m_pUtil;
	TObjRef<Log>				m_pLog;

	TMap<DWORD, tagPlayerCmd*>	m_mapRecvProc;	// 接收消息的处理及统计
	TMap<DWORD,	tagPlayerCmd*>	m_mapSendProc;	// 发送消息的处理及统计
};