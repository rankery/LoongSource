//-------------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: world_net_cmd_mgr.h
// author: Sxg
// actor:
// data: 2009-2-2
// last:
// brief: 消息管理类 -- 需要地图线程上层处理的消息管理
//-------------------------------------------------------------------------------
#pragma once

#include "mutex.h"
#include "player_net_cmd_mgr.h"
#include "world_session.h"
//-------------------------------------------------------------------------------
// 网络消息结构扩展
//-------------------------------------------------------------------------------
struct tagMsgEx
{
	DWORD			dwID;		// session id
	LPBYTE			pMsg;
	DWORD			dwSize;

	tagMsgEx(DWORD dwSessionID, LPBYTE _pMsg, DWORD _dwSize)
	{
		dwID	=	dwSessionID;
		pMsg	=	_pMsg;
		dwSize	=	_dwSize;
	}

	~tagMsgEx()
	{
		if( P_VALID(pMsg) )
		{
			g_worldSession.ReturnMsg(pMsg);
		}
	}
};


//-------------------------------------------------------------------------------
// 地图上层消息处理类
//-------------------------------------------------------------------------------
class WorldNetCmdMgr
{
public:
	WorldNetCmdMgr();
	~WorldNetCmdMgr();

	BOOL Init() { return TRUE; }
	VOID Update();

public:
	VOID Add(DWORD dwSessionID, LPBYTE pMsg, DWORD dwSize);

public:
	//VOID RegisterAll();
	VOID RegisterRecvProc(LPCSTR szCmd, NETMSGHANDLER fp, LPCTSTR szDesc, DWORD dwSize);
	VOID ResetMsgCounter(LPCSTR szCmd);
	VOID ResetMsgCounter(DWORD dwMsgID);

	UINT32 GetRunTimesPerTick(DWORD dwMsgID);

private:
	TList<tagMsgEx*>	m_lstMsgEx;
	Mutex				m_mutex;

	PlayerNetCmdMgr		m_WorldNetMgr;

	// 消息每Tick统计
	TMap<DWORD, UINT32>	m_mapMsgCount;
};

extern WorldNetCmdMgr g_worldNetCmdMgr;