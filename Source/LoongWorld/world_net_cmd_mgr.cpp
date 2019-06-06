//-------------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: world_net_cmd_mgr.cpp
// author: Sxg
// actor:
// data: 2009-2-2
// last:
// brief: 消息管理类 -- 需要地图线程上层处理的消息管理
//-------------------------------------------------------------------------------
#include "StdAfx.h"

#include "world_net_cmd_mgr.h"
#include "role.h"
#include "role_mgr.h"
#include "TimeChecker.h"

WorldNetCmdMgr g_worldNetCmdMgr;
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// 构造&析构
//-------------------------------------------------------------------------------
WorldNetCmdMgr::WorldNetCmdMgr()
{
	m_lstMsgEx.Clear();
	m_mapMsgCount.Clear();
}

WorldNetCmdMgr::~WorldNetCmdMgr()
{
	tagMsgEx* pMsg = m_lstMsgEx.PopFront();
	while( P_VALID(pMsg) )
	{
		SAFE_DEL(pMsg);
		pMsg = m_lstMsgEx.PopFront();
	}
	m_mapMsgCount.Clear();

	m_WorldNetMgr.UnRegisterAll();
}

//-------------------------------------------------------------------------------
// 将消息添加到消息队列
//-------------------------------------------------------------------------------
VOID WorldNetCmdMgr::Add(DWORD dwSessionID, LPBYTE pMsg, DWORD dwSize)
{
	tagMsgEx *pMsgEx = new tagMsgEx(dwSessionID, pMsg, dwSize);
	
	m_mutex.Acquire();
	m_lstMsgEx.PushBack(pMsgEx);
	ResetMsgCounter(((tagNetCmd*)pMsg)->dwID);
	m_mutex.Release();
}

//-------------------------------------------------------------------------------
// 注册消息处理函数
//-------------------------------------------------------------------------------
VOID WorldNetCmdMgr::RegisterRecvProc(LPCSTR szCmd, NETMSGHANDLER fp, LPCTSTR szDesc, DWORD dwSize)
{
	m_WorldNetMgr.RegisterRecvProc(szCmd, fp, szDesc, dwSize);
	ResetMsgCounter(szCmd);
}

//-------------------------------------------------------------------------------
// 重置消息调用计数
//-------------------------------------------------------------------------------
VOID WorldNetCmdMgr::ResetMsgCounter( LPCSTR szCmd )
{
	TObjRef<Util> pUtil = "Util";
	DWORD dwID = pUtil->Crc32(szCmd);

	ResetMsgCounter(dwID);
}

VOID WorldNetCmdMgr::ResetMsgCounter( DWORD dwMsgID )
{
	UINT32 nCount = m_WorldNetMgr.GetRecvCmdRunTimes(dwMsgID);
	if (!GT_VALID(nCount))
		return;

	if( !m_mapMsgCount.ChangeValue(dwMsgID, nCount) )
	{
		m_mapMsgCount.Add(dwMsgID, 0);
		return;
	}
}

UINT32 WorldNetCmdMgr::GetRunTimesPerTick( DWORD dwMsgID )
{
	UINT32 nCurCount = m_WorldNetMgr.GetRecvCmdRunTimes(dwMsgID);
	if (!GT_VALID(nCurCount))
		return GT_INVALID;

	UINT32 nTickCount = m_mapMsgCount.Peek(dwMsgID);
	if (!GT_VALID(nTickCount))
	{
		m_mapMsgCount.Add(dwMsgID, 0);
		nTickCount = 0;
	}

	ASSERT(nCurCount >= nTickCount);

	return nCurCount - nTickCount;
}

//VOID WorldNetCmdMgr::RegisterAll()
//{
//	// 商城
//	m_WorldNetMgr.RegisterRecvProc("NC_MallGet",			&PlayerSession::HandleRoleMallGet,			_T("mall get item"));
//	m_WorldNetMgr.RegisterRecvProc("NC_MallUpdate",			&PlayerSession::HandleRoleMallUpdate,		_T("mall update"));
//	m_WorldNetMgr.RegisterRecvProc("NC_MallBuyItem",		&PlayerSession::HandleRoleMallBuyItem,		_T("mall buy item"));
//	m_WorldNetMgr.RegisterRecvProc("NC_MallBuyPack",		&PlayerSession::HandleRoleMallBuyPack,		_T("mall buy pack"));
//	m_WorldNetMgr.RegisterRecvProc("NC_MallPresentItem",	&PlayerSession::HandleRoleMallPresentItem,	_T("mall buy item for friend"));
//	m_WorldNetMgr.RegisterRecvProc("NC_MallPresentPack",	&PlayerSession::HandleRoleMallPresentPack,	_T("mall buy pack for friend"));
//	m_WorldNetMgr.RegisterRecvProc("NC_MallFreeGetItem",	&PlayerSession::HandleRoleMallFreeGetItem,	_T("mall get free item"));
//
//	// 元宝交易
//	m_WorldNetMgr.RegisterRecvProc("NC_SaveYB2Account",		&PlayerSession::HandleRoleSaveYB2Account,	_T("save yuan bao to account"));
//	m_WorldNetMgr.RegisterRecvProc("NC_SaveSilver2Account", &PlayerSession::HandleRoleSaveSilver2Account, _T("save silver to account"));
//	m_WorldNetMgr.RegisterRecvProc("NC_DepositAccountYB",   &PlayerSession::HandleRoleDepositYBAccount, _T("deposit yuan bao from account"));
//	m_WorldNetMgr.RegisterRecvProc("NC_DepositAccountSilver",&PlayerSession::HandleRoleDepositSilver,	_T("deposit silver from account"));
//	m_WorldNetMgr.RegisterRecvProc("NC_SynYBTradeInfo",		&PlayerSession::HandleRoleGetYBTradeInfo,	_T("get yuan bao trade information"));
//	m_WorldNetMgr.RegisterRecvProc("NC_SubmitSellOrder",	&PlayerSession::HandleRoleSubmitSellOrder,  _T("submit yuan bao sell order"));
//	m_WorldNetMgr.RegisterRecvProc("NC_SubmitBuyOrder",		&PlayerSession::HandleRoleSubmitBuyOrder,	_T("submit yuan bao buy order"));
//	m_WorldNetMgr.RegisterRecvProc("NC_DeleteOrder",		&PlayerSession::HandleRoleDeleteOrder,		_T("delete yuan bao trade order"));
//	m_WorldNetMgr.RegisterRecvProc("NC_GetYuanbaoOrder",	&PlayerSession::HandleRoleGetYBOrder,		_T("get yuan bao order"));
//
//	// 名人堂
//	m_WorldNetMgr.RegisterRecvProc("NC_ActiveTreasure",		&PlayerSession::HandleActiveTreasure,		_T("active clan treasure"));
//	
//	// 帮派
//	m_WorldNetMgr.RegisterRecvProc("NC_GuildCreate",		&PlayerSession::HandleCreateGuild,			_T("create guild"));
//	m_WorldNetMgr.RegisterRecvProc("NC_GuildDismiss",		&PlayerSession::HandleDismissGuild,			_T("dismiss guild"));
//	m_WorldNetMgr.RegisterRecvProc("NC_GuildJoinReq",		&PlayerSession::HandleJoinGuildReq,			_T("join guild req"));
//	m_WorldNetMgr.RegisterRecvProc("NC_GuildJoinReqRes",	&PlayerSession::HandleJoinGuildReqRes,		_T("join guild req res"));
//	m_WorldNetMgr.RegisterRecvProc("NC_GuildLeave",			&PlayerSession::HandleLeaveGuild,			_T("leave guild"));
//	m_WorldNetMgr.RegisterRecvProc("NC_GuildKick",			&PlayerSession::HandleKickFromGuild,		_T("kick from guild"));
//	m_WorldNetMgr.RegisterRecvProc("NC_GuildTurnover",		&PlayerSession::HandleTurnoverGuild,		_T("turnover guild"));
//	m_WorldNetMgr.RegisterRecvProc("NC_GuildDemiss",		&PlayerSession::HandleDemissFromGuild,		_T("demiss from guild"));
//	m_WorldNetMgr.RegisterRecvProc("NC_GuildAppoint",		&PlayerSession::HandleAppointForGuild,		_T("appoint for guild"));
//	m_WorldNetMgr.RegisterRecvProc("NC_GuildTenetChange",	&PlayerSession::HandleChangeGuildTenet,		_T("change guild tenet"));
//	
//	// 帮派信息获取消息
//	m_WorldNetMgr.RegisterRecvProc("NC_GuildGetAllMembers",	&PlayerSession::HandleGetGuildMembers,		_T("get all guild members"));
//	m_WorldNetMgr.RegisterRecvProc("NC_GuildGetMemberEx",	&PlayerSession::HandleGetGuildMemberEx,		_T("get guild member ex"));
//	m_WorldNetMgr.RegisterRecvProc("NC_GuildRefMember",		&PlayerSession::HandleRefreshGuildMember,	_T("refresh guild member"));
//	m_WorldNetMgr.RegisterRecvProc("NC_GetGuildName",		&PlayerSession::HandleGetGuildName,			_T("get guild name"));
//	m_WorldNetMgr.RegisterRecvProc("NC_GetGuildTenet",		&PlayerSession::HandleGetGuildTenet,		_T("get guild tenet"));
//
//	
//	//m_WorldNetMgr.RegisterRecvProc("",		&PlayerSession::Handle,	_T(""));
//}

//-------------------------------------------------------------------------------
// 注册消息处理函数
//-------------------------------------------------------------------------------
VOID WorldNetCmdMgr::Update()
{
	CHECK_TIME();
	PlayerSession *pSession = NULL;

	tagMsgEx* pMsg = m_lstMsgEx.PopFront();
	while( P_VALID(pMsg) )
	{
		pSession = g_worldSession.FindSession(pMsg->dwID);
		if( P_VALID(pSession) )
		{
			MTRANS_POINTER(pCmd, pMsg->pMsg, tagNetCmd);
			m_WorldNetMgr.HandleCmd(pCmd, pMsg->dwSize, pSession);
		}

		SAFE_DEL(pMsg);
		pMsg = m_lstMsgEx.PopFront();
	}
}