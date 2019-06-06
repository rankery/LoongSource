//-----------------------------------------------------------------------------
//!\file	client_mgr.h
//!\brief	客户端管理器
//!
//!\date	2009-04-20
//! last	2009-04-20
//!\author	zhangrong
//!
//! Copyright (c) 1985-2008 CTCRST Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include "client_db.h"

class ClientDB;
class Client;

//===========================================有时限消息管理类===============================================//
class TimeLimitMsgMgr
{
public:
	TimeLimitMsgMgr();
	~TimeLimitMsgMgr();

	VOID Update();
	VOID AddMsg2LMTDL(DWORD, DWORD, BOOL, tagNetCmd*, DWORD);
	//VOID SetCountForAutoNotice(INT nCount){ m_nCountForAutoNotice = nCount; }
	VOID ClearMsgList();
	DWORD CancelTimeLimitedMsg(DWORD dwMsgCrc);

private:
	// 时限消息统计结构
	struct tagMsgTimeDeadLine
	{
		DWORD					dwMsgCrc;			// 消息名的Crc值
		DWORD					dwDeadLine;			// 处理时间，到了该时间点将该消息发送
		DWORD					dwClientID;			// 记录是哪个Rtclient发送的该请求
		BOOL					bType;				// true: 定时发送的消息; false: 每天按点发送的消息
		
		tagNetCmd*				pMsg;	
	};

	// 相关list
	TList<tagMsgTimeDeadLine*>	m_LstMsgTimeDeadLine;
	//TList<tagNetCmd*>			m_LstMsgNeedDel;	// 时限已到，需要删除的消息链表，以消息名Crc为主键

	// 辅助
	DWORD						m_dwUpdateTicks;
	INT32						m_nTickCountDown;
	//INT						m_nCountForAutoNotice;

	// 定时更新(单位：秒)
	VOID UpdateTimeLeftList();

	// 时限消息的发送
	DWORD SendTimeLimitMsg(DWORD, tagNetCmd*, DWORD);
	DWORD SendDoubleMsg(tagNetCmd*, DWORD);
	DWORD SendRightNotice(tagNetCmd*, DWORD);
	DWORD SendAutoNotice(tagNetCmd*, DWORD);
	DWORD SendAutoChatNotice(tagNetCmd*, DWORD);
};



//===========================================客户端管理器===============================================//
class ClientMgr
{
public:
	//-------------------------------------------------------------------------
	// CONSTRUCT
	//-------------------------------------------------------------------------
	ClientMgr();
 	~ClientMgr(){};

	//-------------------------------------------------------------------------
	// 初始化，更新及销毁
	//-------------------------------------------------------------------------
	BOOL	Init();
	VOID	Update();
	VOID	Destroy();

	//-------------------------------------------------------------------------
	// 消息相关
	//-------------------------------------------------------------------------
	VOID	SendMsg(DWORD dwClientID, LPVOID pMsg, DWORD dwSize)	{ m_pStreamServer->Send(dwClientID, pMsg, dwSize); }
	VOID	SendAllClientMsg(LPBYTE pMsg, DWORD dwSize);

private:
	//-------------------------------------------------------------------------
	// 网络层回调函数
	//-------------------------------------------------------------------------
	DWORD	LoginCallBack(LPVOID, LPVOID);
	DWORD	LogoutCallBack(LPVOID);

	//-------------------------------------------------------------------------
	// 消息注册
	//-------------------------------------------------------------------------
	VOID	RegisterClientMsg();
	VOID	UnRegisterClientMsg();

	//-------------------------------------------------------------------------
	// 权限管理
	//-------------------------------------------------------------------------
	BOOL	InitPrivilegeInfo();
	BOOL	HaveEnoughPrivilege(const tagNetCmd* pCmd, const Client* pClient);
	VOID	SendPrivilegeInfo(Client*);

	DWORD	GetClientIP(DWORD dwClientID);

	//-------------------------------------------------------------------------
	// 记录LOG
	//-------------------------------------------------------------------------
	VOID	LogGMAction(Client* pClient, EActionType eActionType, LPCTSTR szDetails);

	//-------------------------------------------------------------------------
	// 客户端消息处理函数
	//-------------------------------------------------------------------------
	DWORD	HandleChallenge(tagNetCmd* pMsg, Client* pClient);
	DWORD	HandleShowSumChallenge(tagNetCmd* pMsg, Client* pClient);
	DWORD	HandleCloseWorld(tagNetCmd* pMsg, Client* pClient);
	DWORD	HandleStartWorld(tagNetCmd* pMsg, Client* pClient);
	DWORD	HandleSystemCmd(tagNetCmd* pMsg, Client* pClient);
	DWORD	HandleSetMaxNum(tagNetCmd* pMsg, Client* pClient);
	DWORD	HandleUpdateMall(tagNetCmd* pMsg, Client* pClient);
	
	DWORD	HandleGMCreate(tagNetCmd* pMsg, Client* pClient);
	DWORD	HandleGMModify(tagNetCmd* pMsg, Client* pClient);
	DWORD	HandleGMDelete(tagNetCmd* pMsg, Client* pClient);
	DWORD	HandleGMQuery(tagNetCmd* pMsg, Client* pClient);
	DWORD	HandleResumeRole(tagNetCmd* pMsg, Client* pClient);
	DWORD	HandleModifyRole(tagNetCmd* pMsg, Client* pClient);
	
	DWORD	HandleDouble(tagNetCmd* pMsg, Client* pClient);
	DWORD	HandleCancelDouble(tagNetCmd* pMsg, Client* pClient);
	DWORD	HandleRightNotice(tagNetCmd* pMsg, Client* pClient);
	DWORD	HandleCancelRightNotice(tagNetCmd* pMsg, Client* pClient);
	DWORD	HandleAutoNotice(tagNetCmd* pMsg, Client* pClient);
	DWORD	HandleAutoChatNotice(tagNetCmd* pMsg, Client* pClient);
	DWORD	HandleCancelAutoChatNotice(tagNetCmd* pMsg, Client* pClient);
	
	DWORD	HandleCreateItem(tagNetCmd* pMsg, Client* pClient);
	DWORD	HandleDeleteItem(tagNetCmd* pMsg, Client* pClient);
	DWORD	HandleItemResume(tagNetCmd* pMsg, Client* pClient);
	DWORD	HandleClearBaibao(tagNetCmd* pMsg, Client* pClient);
	DWORD	HandleAccountForbid(tagNetCmd* pMsg, Client* pClient);
	DWORD	HandleBlacklist(tagNetCmd* pMsg, Client* pClient);
	
	DWORD	HandleChangeIPBind(tagNetCmd* pMsg, Client* pClient);

	DWORD	HandleShowSumReq(tagNetCmd* pMsg, Client* pClient);
	DWORD	HandleModifyFackNum(tagNetCmd* pMsg, Client* pClient);
	
	//DWORD	HandleCancelTimeLimitedMsg(tagNetCmd* pMsg, Client* pClient);
private:
	TSFPTrunk<ClientMgr>		m_Trunk;
	TObjRef<NetCmdMgr>			m_pCmdMgr;
	TObjRef<StreamServer>		m_pStreamServer;		// 服务器通信底层
	INT							m_nPort;				// 监听端口

	ClientDB					m_ClientDB;				// 客户端帐号数据库

	volatile DWORD				m_dwIDGen;				// ID生成器
	TMap<DWORD, Client*>		m_mapAllClient;			// 即将登陆的客户端列表（全局唯一ID——指针）
	TMap<DWORD, Client*>		m_mapClient;			// 已经登陆的客户端列表（帐号ID——指针）
	TMap<DWORD, DWORD>			m_mapPrivilege;			// 权限表， 用于存放各消息的权限信息

	TimeLimitMsgMgr				m_tlMsgMgr;				// 限时消息管理器
};

extern ClientMgr g_ClientMgr;
extern TimeLimitMsgMgr g_TimeLimitMsgMgr;