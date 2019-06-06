//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: db_session.h
// author: Sxg
// actor:
// data: 2008-5-19
// last:
// brief: 游戏服务器与数据库服务器的对话层
//-----------------------------------------------------------------------------
#pragma once

#include "StdAfx.h"

#include "..\WorldDefine\base_define.h"

// Jason [v1.3.1] 外部链接
#include "external_links.h"

//-----------------------------------------------------------------------------
class DBSession
{
public:
	DBSession();
	~DBSession();

public:
	BOOL Init();
	VOID Destroy();

	VOID Update();
	BOOL IsWell() { return (m_bInitOK && m_pTran->IsConnected()); }

	VOID Send(LPVOID pMsg, DWORD dwMsgSize)	{ m_pTran->Send(pMsg, dwMsgSize); }

	LPBYTE Recv(DWORD& dwMsgSize) { return m_pTran->Recv( dwMsgSize ); }

	// 清除收到的网络消息
	VOID FreeRecved(LPVOID pMsg) { m_pTran->FreeRecved((LPBYTE)pMsg); }

	INT	GetUnsendPackageNum() { return m_pTran->GetUnsendPackageNum(); }
	INT	GetReceivedPackageNum() { return m_pTran->GetReceivedPackageNum(); }

private:
	// 读取文件, 初始化成员
	BOOL InitConfig();

	// 注册所有的网络命令
	VOID RegisterAllDBCommand();

private:
	// 连接数据库服务器
	DWORD ThreadConnectDB();

private:
	// 认证及心跳
	DWORD HandleCertification(tagNetCmd* pMsg, DWORD);
	DWORD HandleHeartBeat(tagNetCmd* pMsg, DWORD);

	// 初始化消息
	DWORD HandleLoongWorldInitOK(tagNetCmd* pMsg, DWORD);
	DWORD HandleLoadAllRoleInfo(tagNetCmd* pMsg, DWORD);
	DWORD HandleItemInfo(tagNetCmd* pMsg, DWORD);
	DWORD HandleItemNeedLog(tagNetCmd* pMsg, DWORD);

	// 人物创建、删除和读取
	DWORD HandleRoleEnum(tagNetCmd* pMsg, DWORD);
	DWORD HandleRoleCreate(tagNetCmd* pMsg, DWORD);
	DWORD HandleRoleDelete(tagNetCmd* pMsg, DWORD);
	DWORD HandleRoleResume(tagNetCmd* pMsg, DWORD) { return 0; }
	DWORD HandleRoleLoad(tagNetCmd* pMsg, DWORD);

	// 向百宝袋放入新的物品
	DWORD HandleBaiBaoLoad(tagNetCmd* pMsg, DWORD);
	// 更新百宝袋中的元宝
	DWORD HandleBaiBaoYuanBaoLoad(tagNetCmd* pMsg, DWORD);

	// 元宝交易相关
	DWORD HandleLoadAllYBAccount(tagNetCmd* pMsg, DWORD);
	DWORD HandleLoadAllYBOrder(tagNetCmd* pMsg, DWORD);
	DWORD HandleRoleGetYBOrder(tagNetCmd* pMsg, DWORD);

	// 百宝袋历史记录相关
	DWORD HandleBaiBaoLoadLog(tagNetCmd* pMsg, DWORD);

	// 名人堂
	DWORD HandleRepRankLoad(tagNetCmd* pMsg, DWORD);
	DWORD HandleGetActTreasureList(tagNetCmd* pMsg, DWORD);
	DWORD HandleRepRstTimeStamp(tagNetCmd* pMsg, DWORD);
	DWORD HandleGetFameHallEnterSnap(tagNetCmd* pMsg, DWORD);

	// 帮派相关
	DWORD HandleLoadGuild(tagNetCmd* pMsg, DWORD);
	DWORD HandleLoadGuildMember(tagNetCmd* pMsg, DWORD);
	DWORD HandleGuildMemberInitOk(tagNetCmd* pMsg, DWORD);
	DWORD HandleLoadGuildWareItems(tagNetCmd* pMsg, DWORD);
	DWORD HandleLoadGuildUpgradeInfo(tagNetCmd* pMsg, DWORD);
	DWORD HandleLoadGuildSkillInfo(tagNetCmd* pMsg, DWORD);
	DWORD HandleLoadGuildCommerceInfo(tagNetCmd* pMsg, DWORD);
	DWORD HandleLoadCommerceRankInfo(tagNetCmd* pMsg, DWORD);
	DWORD HandleGuildInitOK(tagNetCmd* pMsg, DWORD);

	// 宠物
	DWORD HandleCreatePetSoul(tagNetCmd* pMsg, DWORD);
	
	// 商城相关
	DWORD HandleLoadAllGPInfo(tagNetCmd* pMsg, DWORD);

	// 角色名贴
	DWORD HandleLoadRoleVCard(tagNetCmd* pMsg, DWORD);

	// VIP摊位
	DWORD HandleLoadVIPStallInfo(tagNetCmd* pMsg, DWORD);

	// 固定活动
	DWORD HandleLoadActivityData(tagNetCmd* pMsg, DWORD);

	// VIP网吧
	DWORD HandleLoadVNBData(tagNetCmd* pMsg, DWORD);

	// 实力排行榜相关
	DWORD HandleInitStrengthRankings(tagNetCmd* pMsg, DWORD);
	DWORD HandleRetUpdateRankings(tagNetCmd* pMsg, DWORD);

	// 读取留言
	DWORD HandleLoadLeftMsg(tagNetCmd* pMsg, DWORD);
	
private:
	TSFPTrunk<DBSession>		m_Trunk;
	TObjRef<Util>				m_pUtil;
	TObjRef<StreamTransport>	m_pTran;
	TObjRef<Thread>				m_pThread;
	TObjRef<NetCmdMgr>			m_pMsgCmdMgr;

	// 连接参数
	CHAR						m_szDBIP[X_IP_LEN];		// 数据库服务器ip
	DWORD						m_dwDBPort;				// 数据库服务器port

	// 初次连接后，需发送的数据
	DWORD						m_dwGoldenCode;					// 数据库服务器金色代码
	//TCHAR						m_szServerName[X_LONG_STRING];	// 运行游戏世界的机器名称

	volatile BOOL				m_bTermConnect;					// 记录当前连接状态
	volatile BOOL				m_bInitOK;						// 是否初始化完成

	DWORD						m_dwInitTime;					//从db初始化所有数据所耗时间 毫秒

	// Jason v[1.3.1] 外部链接
	TList<tagSExternalLink>	m_lstExterLinks;
	ExternalLinks			m_elExternalLinks;
	tagDWORDTime			m_timLastPullExtLink;

	VOID RefreshExternalLinksList(tagNetCmd * pCmd);
	DWORD HandleExternalLinksList(tagNetCmd* pMsg, DWORD);

	VOID UpdateExternalLinksList(VOID);

	VOID InitialExternalLinksList(VOID);

public:
	VOID SendMsgToPlayerForExtLinks(Role * ply);
};

extern DBSession g_dbSession;		// DB session全局对象
