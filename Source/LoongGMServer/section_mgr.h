//-----------------------------------------------------------------------------
//!\file section_mgr.h
//!\brief 大区管理器，管理该服务器管理的所有大区
//!
//!\date 2009-04-20
//! last 2009-04-20
//!\author zhangrong
//!
//! Copyright (c) 1985-2008 CTCRST Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include "../WorldDefine/base_define.h"
#include "../ServerDefine/rt_define.h"

class GameWorld;
class GameServer;
class GameServerLogin;
class GameServerWorld;
class GameServerDB;
class Section;
class Client;
class ServerDB;

//-----------------------------------------------------------------------------
// 大区管理器
//-----------------------------------------------------------------------------
class SectionMgr
{
public:
	SectionMgr();
	~SectionMgr(){};

	//--------------------------------------------------------------------------
	// 初始化，更新及销毁
	//--------------------------------------------------------------------------
	BOOL		Init();
	VOID		Destroy();
	VOID		Update();

	//---------------------------------------------------------------------------
	// 网络相关
	//---------------------------------------------------------------------------
	LPBYTE		RecvMsg(DWORD dwID, DWORD& dwMsgSize, INT& nUnRecved)	{ return m_pStreamServer->Recv(dwID, dwMsgSize, nUnRecved); }
	VOID		ReturnMsg(DWORD dwID, LPBYTE pMsg)	{ m_pStreamServer->FreeRecved(dwID, pMsg); }
	VOID		SendMsg(DWORD dwID, LPVOID pMsg, DWORD dwSize) { m_pStreamServer->Send(dwID, pMsg, dwSize); }
	VOID		HandleCmd(LPVOID pMsg, DWORD dwSize, DWORD dwID)	{ m_pCmdMgr->HandleCmd((tagNetCmd*)pMsg, dwSize, dwID); }

	VOID		SendAllServerStaticInfoToClient(Client* pClient);


	//---------------------------------------------------------------------------
	// 网络ID映射相关
	//---------------------------------------------------------------------------
	DWORD		CreateNewClientID() { InterlockedExchangeAdd((LPLONG)&m_dwIDGen, 1); return m_dwIDGen; }
	VOID		RegisterClientID(DWORD dwID, DWORD dwSectionID)	{ m_mapIDSectionID.Add(dwID, dwSectionID); }

	//---------------------------------------------------------------------------
	// 功能相关
	//---------------------------------------------------------------------------
	BOOL		StartServer(EServerType eType, DWORD dwSectionID, DWORD dwWorldID);
	BOOL		CloseServer(EServerType eType, DWORD dwSectionID, DWORD dwWorldID, BOOL bForce=FALSE);

	//---------------------------------------------------------------------------
	// 辅助函数
	//---------------------------------------------------------------------------
	TMap<DWORD, Section*> GetSectionMap(){return m_mapSection;}
	Section*	GetSection(DWORD dwSectionID)		{ return m_mapSection.Peek(dwSectionID); }
	Section*	GetSection(LPCTSTR);
	INT			GetSectionNum()						{ return m_mapSection.Size(); }
	GameServer* GetServer(DWORD dwSectionID, DWORD dwWorldID, EServerType eType);
	ServerDB*	GetServerDB(DWORD dwSectionID, DWORD dwWorldID);
	GameWorld*	FindGameWorld(DWORD dwID);
	VOID		SendDynamicInfo()					{ InterlockedExchange((LPLONG)&m_bSendImmediately, TRUE); }

private:
	//---------------------------------------------------------------------------
	// 网络消息注册
	//---------------------------------------------------------------------------
	VOID		RegisterServerMsg();
	VOID		UnRegisterServerMsg();

	//---------------------------------------------------------------------------
	// 网络消息处理函数
	//---------------------------------------------------------------------------
	DWORD		HandleWorldLogin(tagNetCmd* pMsg, DWORD);
	//DWORD		HandleStartWorld(tagNetCmd* pMsg, DWORD);
	//DWORD		HandleCloseWorld(tagNetCmd* pMsg, DWORD);
	DWORD		HandleGetWorldInfo(tagNetCmd* pMsg, DWORD);
	DWORD		HandleRTServiceLogin(tagNetCmd* pMsg, DWORD);

	//---------------------------------------------------------------------------
	// 向客户端返回操作结果
	//---------------------------------------------------------------------------
	DWORD		HandleRetDouble(tagNetCmd* pMsg, DWORD dwID);
	DWORD		HandleRetAutoNotice(tagNetCmd* pMsg, DWORD dwID);
	DWORD		HandleRetRightNotice(tagNetCmd* pMsg, DWORD dwID);
	DWORD		HandleRetAccountForbid(tagNetCmd* pMsg, DWORD dwID);
	DWORD		HandleRetIPBlacklist(tagNetCmd* pMsg, DWORD dwID);
	DWORD		HandleRetCancelDouble(tagNetCmd* pMsg, DWORD dwID);
	//DWORD		HandleRetCancelRightNotice(tagNetCmd* pMsg, DWORD dwID);

	//---------------------------------------------------------------------------
	// 网络通信相关
	//---------------------------------------------------------------------------
	DWORD		LoginCallBack(LPVOID, LPVOID);			// 服务器连接回调函数
	DWORD		LogoutCallBack(LPVOID);					// 服务器连接断开回调函数

	//---------------------------------------------------------------------------
	// 初始化相关
	//---------------------------------------------------------------------------
	BOOL		LoadAllServerInfo();

	//---------------------------------------------------------------------------
	// 更新相关
	//---------------------------------------------------------------------------
	VOID		UpdateSession();
	VOID		SendAllServerDynamicInfo();

	//---------------------------------------------------------------------------
	// 辅助函数相关
	//---------------------------------------------------------------------------
	GameServer*			FindServer(DWORD dwID);
	GameServerLogin*	FindLoginServer(DWORD dwID);
	GameServerWorld*	FindWorldServer(DWORD dwID);
	GameServerDB*		FindDBServer(DWORD dwID);

private:
	static const INT			SEND_INFO_TICK_COUNT_DOWN = 5 * TICK_PER_SECOND;	// 发送服务器信息的倒计时间隔

private:
	TSFPTrunk<SectionMgr>		m_Trunk;
	TObjRef<NetCmdMgr>			m_pCmdMgr;					// 网络命令管理器
	TObjRef<StreamServer>		m_pStreamServer;			// 网络接口
	INT							m_nPort;					// 本地监听的端口

	TMap<DWORD, Section*>		m_mapSection;				// 所有大区

	volatile DWORD				m_dwIDGen;					// ID生成器
	TMap<DWORD, DWORD>			m_mapIDSectionID;			// 网络ID与大区ID对应表

	INT							m_nSendInfoTickCountDown;	// 发送服务器信息的倒计时 
	volatile BOOL				m_bSendImmediately;
};

extern SectionMgr g_SectionMgr;