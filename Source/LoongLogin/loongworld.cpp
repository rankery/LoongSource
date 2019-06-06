//-----------------------------------------------------------------------------
//!\file loongworld.h
//!\author Aslan
//!
//!\date 2008-11-30
//! last 2008-11-30
//!
//!\brief 管理某个单独的游戏世界
//!
//!	Copyright (c) 2004 TENGWU Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#include "stdafx.h"

#include "../WorldDefine/login.h"
#include "../ServerDefine/base_define.h"
#include "../ServerDefine/login_issue.h"

#include "loongworld.h"
#include "loongworld_mgr.h"
#include "player_mgr.h"
#include "player.h"
#include "db.h"
#include "login_server.h"
#include "fatigue_mgr.h"

//------------------------------------------------------------------------------
// CONSTANTS
//------------------------------------------------------------------------------
const INT	ALLOW_PLAYER_LOGIN	=	2;		// 多少秒放一个人进服务器
const INT	UPDATE_QUEUE_TIME	=	2000;	// 更新排队列表时间间隔
const FLOAT	LOGIN_LIMIT			=	0.9f;	// 当达到LoongWorld人数上限的90%,就开始排队

//-------------------------------------------------------------------------------
// 构造函数
//-------------------------------------------------------------------------------
LoongWorld::LoongWorld() : m_Data(), m_dwTime(0)
{

}

//-------------------------------------------------------------------------------
// 析构函数
//-------------------------------------------------------------------------------
LoongWorld::~LoongWorld()
{
	Destroy();
}

//-------------------------------------------------------------------------------
// 初始化函数
//-------------------------------------------------------------------------------
BOOL LoongWorld::Init(INT nIndex)
{
	if( nIndex < 0 ) return FALSE;

	m_pUtil		=	"Util";
	m_dwTime	=	timeGetTime();
	m_dwDBUpdateTime = timeGetTime();
	m_dwDBInsertTime = timeGetTime();
	// 从配置文件中得到游戏世界的具体配置
	TObjRef<VarContainer> pVar = "LoginServerConfig";

	TCHAR szTemp[X_SHORT_NAME] = {_T('\0')};
	_stprintf(szTemp, _T("zone%d"), nIndex);

	// 得到world的名字
	tstring strWorldName = pVar->GetString(_T("name"), szTemp);
	if( strWorldName.empty() ) return FALSE;

	// 设置LoongWorldData中的数据
	m_Data.bValid			=	FALSE;
	m_Data.eStatus			=	EWS_InitNotDone;
	m_Data.nCurOnlineNum	=	0;
	m_Data.nMaxOnlineNum	=	0;
	m_Data.nCurOnlineDisplay=	0;
	m_Data.dwNameCrc		=	m_pUtil->Crc32(strWorldName.c_str());
	m_Data.dwIP				=	0;
	m_Data.nPort			=	0;
	m_Data.bAutoSeal		=	pVar->GetDword(_T("auto_seal"), szTemp);
	_tcsncpy(m_Data.szName, strWorldName.c_str(), X_SHORT_NAME);
	//temp add by songg
  	m_Data.dwWorldID		=	pVar->GetDword(_T("id"), szTemp); 
    return TRUE;
}

//-------------------------------------------------------------------------------
// 更新函数
//-------------------------------------------------------------------------------
VOID LoongWorld::Update()
{
	if( IsValid() )
	{
		// 更新所有网络消息
		UpdateSession();

		// 更新所有加入到游戏世界的玩家
		UpdateInsertPlayer();

		// 更新所有排队的玩家
		UpdateQueuedPlayer();

		// 更新所有等待列表中的玩家
		UpdateWaitingPlayer();

		// 更新踢掉列表中的玩家
		UpdateKickedPlayer();
	}
	else
	{
		// 更新踢掉列表中的玩家
		UpdateKickedPlayer();
	}
}

//-------------------------------------------------------------------------------
// 更新连接
//-------------------------------------------------------------------------------
VOID LoongWorld::UpdateSession()
{
	LPBYTE	pRecv		=	NULL;
	DWORD	dwSize		=	0;
	INT		nUnRecv		=	0;
	DWORD	dwTime		=	0;

	// 接收消息
	pRecv = sLoongWorldMgr.RecvMsg(m_Data.dwNameCrc, dwSize, nUnRecv);

	while( P_VALID(pRecv) )
	{
		// 处理消息
		sLoongWorldMgr.HandleCmd((tagNetCmd*)pRecv, dwSize, (DWORD)this);

		// 归还消息
		sLoongWorldMgr.ReturnMsg(m_Data.dwNameCrc, pRecv);

		// 收取下一条消息
		pRecv = sLoongWorldMgr.RecvMsg(m_Data.dwNameCrc, dwSize, nUnRecv);
	}
}

//-------------------------------------------------------------------------------
// 更新插入的玩家列表
//-------------------------------------------------------------------------------
VOID LoongWorld::UpdateInsertPlayer()
{
	if( !m_Data.bValid ) return;
	if( EWS_Well != m_Data.eStatus ) return;

	INT nBeginWaitNum = m_Data.nMaxOnlineNum * LOGIN_LIMIT;			// 开始排队的界定值
	INT nTemp = m_Data.nCurOnlineNum + m_mapWaitting.Size();		// 界定值

	Player* pPlayer	= m_listInsertPlayer.PopFront();
	while( P_VALID(pPlayer) )
	{
		// 查看排队列表中是否为空
		if( m_listQueue.Empty() )
		{
			// 如果当前在线人数超过了排队的基线，则开始排队
			if( nTemp >= nBeginWaitNum )
			{
				AddIntoQueue(pPlayer);
			}
			// 不需要排队
			else
			{
				AddIntoWaitingMap(pPlayer);
				++nTemp;		// 累加一个数字
			}
		}
		// 排队列表不为空，直接添加到排队列表
		else
		{
			AddIntoQueue(pPlayer);
		}

		// 再获取一个玩家
		pPlayer = m_listInsertPlayer.PopFront();
	}
}

//-------------------------------------------------------------------------------
// 更新排队列表
//-------------------------------------------------------------------------------
VOID LoongWorld::UpdateQueuedPlayer()
{
	if( !m_Data.bValid ) return;
	if( EWS_Well != m_Data.eStatus ) return;

	if( m_listQueue.Empty() ) return;

	// 看看是否到了更新列表的时间
	BOOL bUpdateQueue = FALSE;
	if( timeGetTime() - m_dwTime > UPDATE_QUEUE_TIME )
	{
		m_dwTime = timeGetTime();
		bUpdateQueue = TRUE;
	}

	// 设置是否已经缓慢放人的标志
	BOOL bSlowEnter = FALSE;

	INT nBeginWaitNum = m_Data.nMaxOnlineNum * LOGIN_LIMIT;							// 开始排队的界定值
	INT nNoWaitNum = nBeginWaitNum - m_Data.nCurOnlineNum - m_mapWaitting.Size();	// 可以直接进入游戏世界的数量

	// 如果不能进入，则直接设置bSlowEnter为TRUE
	INT nHavePlayer = m_Data.nMaxOnlineNum - m_Data.nCurOnlineNum - m_mapWaitting.Size();
	if( nHavePlayer <= 0 ) bSlowEnter = TRUE;

	// 轮询每个玩家
	m_listQueue.ResetIterator();
	Player* pPlayer = NULL;

	INT nIndexInQueue = 0;		// 在队伍中的索引

	while( m_listQueue.PeekNext(pPlayer) )
	{
		if( pPlayer->IsConnLost() )		// 玩家掉线了
		{
			m_listQueue.Erase(pPlayer);
			sPlayerMgr.PlayerLogout(pPlayer);
			continue;
		}

		// 如果到了更新的时候
		if( bUpdateQueue )
		{
			if( nNoWaitNum > 0 )			// 查看是否还可以直接进
			{
				m_listQueue.Erase(pPlayer);

				tagNLS_Queue send;
				send.dwPosition = 0;
				send.dwTime = 0;
				pPlayer->SendMessage(&send, send.dwSize);

				AddIntoWaitingMap(pPlayer);
				--nNoWaitNum;
			}
			else if( !bSlowEnter )			// 每次放一个人
			{
				m_listQueue.Erase(pPlayer);

				tagNLS_Queue send;
				send.dwPosition = 0;
				send.dwTime = 0;

				pPlayer->SendMessage(&send, send.dwSize);

				AddIntoWaitingMap(pPlayer);
				bSlowEnter = TRUE;
			}
			else							// 这些人继续排队了
			{
				tagNLS_Queue send;
				send.dwPosition = ++nIndexInQueue;
				send.dwTime = nIndexInQueue * ALLOW_PLAYER_LOGIN;
				pPlayer->SendMessage(&send, send.dwSize);
			}
		}
	}
}

//-------------------------------------------------------------------------------
// 更新等待的玩家
//-------------------------------------------------------------------------------
VOID LoongWorld::UpdateWaitingPlayer()
{
	if( m_mapWaitting.Empty() ) return;

	Player* pPlayer = NULL;
	m_mapWaitting.ResetIterator();

	while( m_mapWaitting.PeekNext(pPlayer) )
	{
		if( pPlayer->IsConnLost() )				// 掉线了
		{
			m_mapWaitting.Erase(pPlayer->GetAccountID());
			sPlayerMgr.PlayerLogout(pPlayer);
		}
		else if( pPlayer->IsSelectWorldOk() )		// 已经验证成功了
		{
			if( CalcTimeDiff(GetCurrentDWORDTime(), pPlayer->GetKickTime()) > 5 )	// 验证成功的玩家如果5秒钟不断开连接，就踢掉
			{
				m_mapWaitting.Erase(pPlayer->GetAccountID());
				KickPlayer(pPlayer);
			}
		}
	}
}

//-------------------------------------------------------------------------------
// 更新踢掉的玩家列表
//-------------------------------------------------------------------------------
VOID LoongWorld::UpdateKickedPlayer()
{
	if( m_listKickPlayer.Empty() ) return;

	Player* pPlayer = NULL;
	m_listKickPlayer.ResetIterator();

	while( m_listKickPlayer.PeekNext(pPlayer) )
	{
		if( pPlayer->IsConnLost() )		// 被踢掉了
		{
			m_listKickPlayer.Erase(pPlayer);
			sPlayerMgr.PlayerLogout(pPlayer);
		}
	}
}

//-------------------------------------------------------------------------------
// 释放资源
//-------------------------------------------------------------------------------
VOID LoongWorld::Destroy()
{
	// 去掉logtime
}

//-------------------------------------------------------------------------------
// 加入到排队列表中
//-------------------------------------------------------------------------------
VOID LoongWorld::AddIntoQueue(Player* pPlayer)
{
	if( !P_VALID(pPlayer) ) return;

	m_listQueue.PushBack(pPlayer);

	// 给客户端发送消息
	tagNLS_ProofResult send;
	send.bGuard			=	pPlayer->IsGuard();
	send.dwErrorCode	=	E_ProofResult_Queue;

	pPlayer->SendMessage(&send, send.dwSize);
}

//-------------------------------------------------------------------------------
// 加入到等待列表中
//-------------------------------------------------------------------------------
VOID LoongWorld::AddIntoWaitingMap(Player* pPlayer)
{
	if( !P_VALID(pPlayer) ) return;

	m_mapWaitting.Add(pPlayer->GetAccountID(), pPlayer);

	// 给world发送消息
	tagNLW_PlayerWillLogin send;
	send.dwVerifyCode	=	pPlayer->GetVerifyCode();
	send.dwAccountID	=	pPlayer->GetAccountID();
	send.byPrivilege	=	pPlayer->GetPrivilege();
	send.bGuard			=	pPlayer->GetPlayerData().bGuard;
	send.dwAccOLSec		=	pPlayer->GetPlayerData().nAccOLSec;
	//add for prevent thief
	send.dwPreLoginIP		=	pPlayer->GetPreLoginIP();
	send.dwPreLoginTime		=	pPlayer->GetPreLoginTime();

	strncpy_s(send.szAccount, pPlayer->GetAccoutName(), X_SHORT_NAME);

	sLoongWorldMgr.SendMsg(m_Data.dwNameCrc, &send, send.dwSize);
}

//-------------------------------------------------------------------------------
// LoongWorld登陆
//-------------------------------------------------------------------------------
BOOL LoongWorld::WorldLogin(DWORD dwIP, INT nPort, DWORD* pOLAccountIDs, INT nOLAccountIDNum)
{
	if( IsValid() ) return FALSE;	// 检查是否已经连接上

	// 设置登陆信息
	m_Data.bValid			=	TRUE;
	m_Data.dwIP				=	dwIP;
	m_Data.nPort			=	nPort;
	m_Data.eStatus			=	EWS_InitNotDone;
	m_Data.nCurOnlineNum	=	0;
	m_Data.nMaxOnlineNum	=	0;
	m_Data.nCurOnlineDisplay=	0;

	// tdb:worldnamecrc为本world的状态为EPLS_Online的玩家状态为EPLS_Unknown
	sBeton.FixOneWorldPlayerLoginStatus(m_Data.dwNameCrc, EPLS_Online, EPLS_Unknown);

	// tbd:更新玩家状态，指定玩家更新为为EPLS_Online
	sBeton.ResetPlayersLoginStatus(pOLAccountIDs, nOLAccountIDNum, EPLS_Online);

	// tdb:worldnamecrc为本world的状态为EPLS_Unknown的玩家状态为EPLS_Offline
	sBeton.FixOneWorldPlayerLoginStatus(m_Data.dwNameCrc, EPLS_Unknown, EPLS_OffLine);

	if (g_login.IsEnableFatigueGuard())
		sFatigueMgr.ResetWorldAccounts(m_Data.dwNameCrc, pOLAccountIDs, nOLAccountIDNum);

	return TRUE;
}

//-------------------------------------------------------------------------------
// LoongWorld登出
//-------------------------------------------------------------------------------
BOOL LoongWorld::WorldLogout()
{
	if( !IsValid() ) return FALSE;	// 检查是否一开始就没连接上

	// 设置登出信息
	m_Data.bValid			=	FALSE;
	m_Data.dwIP				=	0;
	m_Data.nPort			=	0;
	m_Data.eStatus			=	EWS_InitNotDone;
	m_Data.nCurOnlineNum	=	0;
	m_Data.nMaxOnlineNum	=	0;
	m_Data.nCurOnlineDisplay=	0;

	// 踢掉所有该游戏世界内的玩家
	Player* pPlayer = NULL;

	// 首先踢掉insertlist中的玩家
	pPlayer = m_listInsertPlayer.PopFront();
	while( P_VALID(pPlayer) )
	{
		KickPlayer(pPlayer);

		pPlayer = m_listInsertPlayer.PopFront();
	}

	// 再踢掉排队列表中的玩家
	pPlayer = m_listQueue.PopFront();
	while( P_VALID(pPlayer) )
	{
		KickPlayer(pPlayer);

		pPlayer = m_listQueue.PopFront();
	}

	// 再踢掉等待列表中的玩家
	m_mapWaitting.ResetIterator();
	while( m_mapWaitting.PeekNext(pPlayer) )
	{
		KickPlayer(pPlayer);
	}
	m_mapWaitting.Clear();

	if (g_login.IsEnableFatigueGuard())
		sFatigueMgr.ResetWorldAccounts(m_Data.dwNameCrc, NULL, 0);

	return TRUE;
}

//-----------------------------------------------------------------------
// 玩家登录结果
//-----------------------------------------------------------------------
VOID LoongWorld::PlayerWillLoginRet(DWORD dwAccountID, DWORD dwErrorCode)
{
	// 在等待列表中查找玩家
	Player* pPlayer = m_mapWaitting.Peek(dwAccountID);
	if( P_VALID(pPlayer) )
	{
		// 如果是验证成功，则设置选择游戏世界完毕
		if( E_Success == dwErrorCode )
		{
			pPlayer->SetSelectWorldOK();
		}

		// 向玩家发送返回结果
		tagNLS_ProofResult send;
		send.bGuard			=	pPlayer->IsGuard();
		send.dwIndex		=	0;
		send.dwIP			=	m_Data.dwIP;
		send.dwPort			=	m_Data.nPort;
		send.dwAccountID	=	dwAccountID;
		send.dwVerifyCode	=	pPlayer->GetVerifyCode();

		if( E_Success == dwErrorCode )
			send.dwErrorCode	=	E_Success;
		else
			send.dwErrorCode	=	E_ProofResult_Account_No_Match;

		pPlayer->SendMessage(&send, send.dwSize);

		// 设置玩家要被踢掉的时间
		pPlayer->SetKickTime(GetCurrentDWORDTime());
	}
}


////-----------------------------------------------------------------------
//// 认证消息
////-----------------------------------------------------------------------
//VOID LoongWorld::HandleCertification(tagNetCmd* pMsg,LPVOID param)
//{
//     tagNWL_Certification* pRecv = (tagNWL_Certification*)pMsg;
//    
//     m_Data.nPort = pRecv->dwPort;
//     
//	 //得到IP
//	 /*sockaddr_in addr_in;
//	 m_pSession->GetClientSockName(&addr_in, m_dwClientID);
//	 m_pData->dwIP       = addr_in.sin_addr.S_un.S_addr;*/
//	m_pData->dwIP	= pRecv->dwIP;
//
//	 in_addr addr;
//	 addr.S_un.S_addr =  m_pData->dwIP;
//	 tstring str = m_pUtil->Unicode8ToUnicode(inet_ntoa( addr));
//	 str += _T("\n");
//	IMSG(str.c_str());
//     tagNLW_Certification send;
//     send.dwGoldenCode = sLoongWorldMgr.m_dwLoginServerGoldenCode;
//	 m_pSession->Send( m_pData->dwClientID, &send, send.dwSize );
//}
//    
////-----------------------------------------------------------------------
//// 状态消息
////-----------------------------------------------------------------------
//VOID LoongWorld::HandleZoneServerStatus(tagNetCmd* pMsg, LPVOID param)
//{
//    tagNWL_WorldStatus* pRecv = (tagNWL_WorldStatus*)pMsg;
//	
//	m_Data.eStatus			=	pRecv->eStatus;
//    m_Data.nMaxOnlineNum	=	pRecv->nPlayerNumLimit;
//	m_Data.nCurOnlineNum	=	pRecv->nCurPlayerNum;
//}
//
////-----------------------------------------------------------------------
//// 玩家登录LoongWorld
////-----------------------------------------------------------------------
//VOID LoongWorld::HandlePlayerLogin(tagNetCmd* pMsg,LPVOID param)
//{
//    tagNWL_PlayerLogin* pRecv = (tagNWL_PlayerLogin*)pMsg;
//    
//
//	// 更新累计在线时间
//	m_LockLogTime.Acquire();
//	tagLogTimeData* pLogTimeData = m_mapLogTime.Peek(pRecv->dwAccountID);
//	m_LockLogTime.Release();
//
//	ASSERT(P_VALID(pLogTimeData));
//	if (!P_VALID(pLogTimeData))
//	{
//		m_LockLogTime.Acquire();
//		m_mapLogTime.Erase(pRecv->dwAccountID);
//		SAFE_DEL(pLogTimeData);
//		m_LockLogTime.Release();
//		return;
//	}
//	pLogTimeData->SetLogin(GetCurrentDWORDTime());
//	pLogTimeData->UpdateAccULSec();
//
//    if(pRecv->dwErrorCode != E_Success)
//    {
//
//		//加入到KickList
//		//sPlayerMgr.AddKickList(pPlayer);
//		IMSG( _T("Login ErrorCode =%d  \r\n"), pRecv->dwErrorCode );
//		
//		sBeton.UpdatePlayerIntoWorld(LOGIN_STATUS_OFFLINE,pLogTimeData->dwtLogin, pLogTimeData->GetAccULSec(), pLogTimeData->GetAccOLSec(), pRecv->dwAccountID);
//		pLogTimeData->SetLogout(GetCurrentDWORDTime());
//		pLogTimeData->UpdateAccOLSec();
//		sBeton.sBeton.UpdatePlayerOutWorld(LOGIN_STATUS_OFFLINE, pLogTimeData->dwtLogout, pLogTimeData->GetCurOLSec(), pLogTimeData->GetAccOLSec(), pRecv->dwAccountID);
//		
//		ILOG->Write(_T("Player Login Failure! AccountID:%u ErrorCode=%d\r\n"), pRecv->dwAccountID, pRecv->dwErrorCode);
//
//		// 删除时间数据
//		m_LockLogTime.Acquire();
//		m_mapLogTime.Erase(pRecv->dwAccountID);
//		SAFE_DEL(pLogTimeData);
//		m_LockLogTime.Release();
//
//		return;
//    }
//    
//    //更新数据库数据
//    sBeton.UpdatePlayerIntoWorld(LOGIN_STATUS_ONLINE,pLogTimeData->dwtLogin, pLogTimeData->GetAccULSec(), pLogTimeData->GetAccOLSec(), pRecv->dwAccountID);
//
//	//查找此用户ID的姓名
//	tstring name = sBeton.QueryPlayerAccountName(pRecv->dwAccountID);
//
//	IMSG( _T("Player AccountID =%u Name = %s Login \r\n"), pRecv->dwAccountID, name.c_str());
//	ILOG->Write(_T("Player AccountID:%u Name = %s Login\r\n"), pRecv->dwAccountID, name.c_str());
//}
//
////-----------------------------------------------------------------------
//// 玩家登出LoongWorld
////-----------------------------------------------------------------------
//VOID LoongWorld::HandlePlayerLogout(tagNetCmd* pMsg,LPVOID param)
//{
//    tagNWL_PlayerLogout* pRecv = (tagNWL_PlayerLogout*)pMsg;
//
//	// 更新累计下线时间
//	m_LockLogTime.Acquire();
//	tagLogTimeData* pLogTimeData = m_mapLogTime.Peek(pRecv->dwAccountID);
//	m_LockLogTime.Release();
//
//	if (P_VALID(pLogTimeData))
//	{
//		pLogTimeData->SetLogout(GetCurrentDWORDTime());
//		pLogTimeData->UpdateAccOLSec();
//
//		//更新数据库数据
//		sBeton.UpdatePlayerOutWorld(LOGIN_STATUS_OFFLINE, pLogTimeData->dwtLogout, pLogTimeData->GetCurOLSec(), pLogTimeData->GetAccOLSec(), pRecv->dwAccountID);
//
//		// 删除时间数据
//		m_LockLogTime.Acquire();
//		m_mapLogTime.Erase(pRecv->dwAccountID);
//		SAFE_DEL(pLogTimeData);
//		m_LockLogTime.Release();
//	}
//	else
//	{
//		sBeton.UpdatePlayerOutWorld(LOGIN_STATUS_OFFLINE, GetCurrentDWORDTime(), 0, 0, pRecv->dwAccountID);
//	}
//
//	//查找此用户ID的姓名
//	tstring name = sBeton.QueryPlayerAccountName(pRecv->dwAccountID);
//
//	//记录玩家登出Log
//	sBeton.InsertLogoutLog(pRecv->dwAccountID);
//	IMSG( _T("Player AccountID =%u Name = %s Logout \r\n"), pRecv->dwAccountID, name.c_str());
//	ILOG->Write(_T("Player AccountID:%u Name = %s Logout\r\n"), pRecv->dwAccountID, name.c_str());
//}
//
////-----------------------------------------------------------------------
//// 玩家向LoongWorld发出登录请求
////-----------------------------------------------------------------------
// VOID LoongWorld::HandlePlayerWillLogin(tagNetCmd* pMsg,LPVOID param)
//{
//    tagNWL_PlayerWillLogin* pRecv = (tagNWL_PlayerWillLogin*)pMsg;
//    
//    //查找玩家
//    Player* pPlayer = m_mapWaitting.Peek(pRecv->dwAccountID);
//    if(!P_VALID(pPlayer))
//	{
//		//直接更改数据库
//		//tbc
//		sBeton.UpdatePlayerOutWorld(LOGIN_STATUS_OFFLINE, (DWORD)GetCurrentDWORDTime(), 0, 0, pRecv->dwAccountID);
//		IMSG( _T("HandlePlayerWillLogin Player Not Found \r\n") );
//		ILOG->Write(_T("HandlePlayerWillLogin Player Not Found! AccountID:%u \r\n"), pRecv->dwAccountID);
//        return;
//	}
//    //给玩家发送 NLS_ProofResult消息
//    tagNLS_ProofResult send;
//    send.bGuard = pPlayer->IsFatigueGuard();
//    send.dwErrorCode = E_Success;
//    send.dwIndex = 0;
//    send.dwIP = m_Data.dwIP;
//    send.dwPort = m_Data.nPort;
//    send.dwAccountID = pRecv->dwAccountID;
//    send.dwVerifyCode = pPlayer->GetRandomNum();
//	pPlayer->SendMsg(&send, send.dwSize);
//
//	////加入Kick列表
//	pPlayer->SetKickTime(GetCurrentDWORDTime());
//	//sPlayerMgr.AddKickList(pPlayer);
//	pPlayer->SetProofResult(TRUE);
//
//	// 添加用户登录时间
//	m_LockLogTime.Acquire();
//	tagLogTimeData* pLogTimeData = m_mapLogTime.Peek(pPlayer->GetAccountID());
//	if (!P_VALID(pLogTimeData))
//	{
//		pLogTimeData = new tagLogTimeData;
//		m_mapLogTime.Add(pPlayer->GetAccountID(), pLogTimeData);
//	}
//	m_LockLogTime.Release();
//	pLogTimeData->bGuard = pPlayer->GetPlayerData()->bGuard;
//	pLogTimeData->dwtLastLogout = pPlayer->GetPlayerData()->dwLastLogoutTime;
//	pLogTimeData->dwAccOLSec = pPlayer->GetPlayerData()->dwAccOLSec;
//	pLogTimeData->dwAccULSec = pPlayer->GetPlayerData()->dwAccULSec;
//}
//-----------------------------------------------------------------------
// LoongWorld踢出玩家
//-----------------------------------------------------------------------
// VOID LoongWorld::HandleKickLog(tagNetCmd* pMsg,LPVOID param)
// {
// 	MGET_MSG(pRecv, pMsg, NWL_KickLog);
// 
// 	tstring tsAccountName = sBeton.QueryPlayerAccountName(pRecv->dwAccountID).c_str();
// 
// #ifdef _DEBUG
// 	IMSG(_T("************GameGuarderKick*************\r\n"));
// 	IMSG(_T("*Kick Name: %s\r\n*AccountID: %d\r\n*Time: %d\r\n*Seal: %d\r\n*ErrCode: %d\r\n"), tsAccountName.c_str(), pRecv->dwAccountID, pRecv->dwKickTime, pRecv->bySeal, pRecv->u16ErrCode);
// 	IMSG(_T("************GameGuarderKick*************\r\n"));
// #endif
// 
// 	ILOG->Write(_T("Kick Name: %s  AccountID: %d  Time: %d  Seal: %d ErrCode: %d\r\n "), tsAccountName.c_str(), pRecv->dwAccountID, pRecv->dwKickTime, pRecv->bySeal, pRecv->u16ErrCode);
// 
// 	sBeton.InsertKickLog(tsAccountName.c_str(), pRecv->dwAccountID, pRecv->dwKickTime, pRecv->u16ErrCode, pRecv->bySeal);
// 	if (pRecv->bySeal && m_Data.bAutoSeal)
// 	{
// 		sBeton.UpdateSealAccount(pRecv->dwAccountID, pRecv->dwKickTime, pRecv->u16ErrCode, pRecv->bySeal);
// 	}
// 	
// }
// 
// 
// 
// 
