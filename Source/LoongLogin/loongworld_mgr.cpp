//-----------------------------------------------------------------------------
//!\file loongworld_mgr.cpp
//!\author Aslan
//!
//!\date 2008-11-30
//! last 2008-11-30
//!
//!\brief 管理所有连接进来LoongWorld,拥有StreamServer,发送消息，处理LoongWorld发送的包
//!
//!	Copyright (c) 2004 TENGWU Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#include "stdafx.h"

#include "../ServerDefine/msg_rt_errorcode.h"
#include "../ServerDefine/login_issue.h"
#include "../ServerDefine/login_define.h"
#include "loongworld_mgr.h"
#include "loongworld.h"
#include "login_server.h"
#include "db.h"
#include "fatigue_mgr.h"
#include "proof_policy.h"

//------------------------------------------------------------------------------
// 构造函数
//------------------------------------------------------------------------------
LoongWorldMgr::LoongWorldMgr():m_Trunk(this)
{

}

//------------------------------------------------------------------------------
// 析构函数
//------------------------------------------------------------------------------
LoongWorldMgr::~LoongWorldMgr()
{
    Destroy();
}

//-------------------------------------------------------------------------------
// 初始化函数
//-------------------------------------------------------------------------------
BOOL LoongWorldMgr::Init()
{
    m_pUtil = "Util";

    // 从配置文件读入LoongWorld的配置
	TObjRef<VarContainer> pVar = "LoginServerConfig";

	INT nWorldNum = pVar->GetDword(_T("num zone_server"));

    for(INT n = 0; n < nWorldNum; ++n)
    {
		// 生成world
        LoongWorld* pWorld = new LoongWorld;

		if( FALSE == pWorld->Init(n) )
		{
			SAFE_DEL(pWorld);
			return FALSE;
		}

        m_mapLoongWorld.Add(pWorld->GetID(), pWorld);
		
		//初始化数据库数据  全部状态设成-1 songg
		BOOL bRet = sBeton.InsertWorldState(pWorld->GetWorldID());
		if(!bRet)
		{
			sBeton.UpdateWorldState(pWorld->GetWorldID(),0,-1);
		}

		//记录log
		sBeton.InsertWorldStateLog(pWorld->GetWorldID(),0,-1);
    }

    if( m_mapLoongWorld.Empty() )
        return FALSE;

    // 读取金色代码
    m_dwLoongWorldGoldenCode = pVar->GetDword(_T("zone_server golden_code"));
	m_dwLoginServerGoldenCode = pVar->GetDword(_T("login_server golden_code"));

	// 生成网络命令管理器
	CreateObj("NetCmdMgrWorld", "NetCmdMgr");
	m_pMsgCmdMgr = "NetCmdMgrWorld";
	if( !P_VALID(m_pMsgCmdMgr) ) return FALSE;

	// 生成服务器网络接口
	CreateObj("StreamServerWorld", "StreamServer");
	m_pSession = "StreamServerWorld";
	if( !P_VALID(m_pSession) ) return FALSE;

	// 注册网络命令
	RegisterLoongWorldMsg();

	// 初始化网络
	INT nPort = pVar->GetInt(_T("port"), _T("zone_session"));
	m_pSession->Init((TCPLOGINCALLBACK)m_Trunk.sfp2(&LoongWorldMgr::LoginCallBack),
		(TCPLOGOUTCALLBACK)m_Trunk.sfp1(&LoongWorldMgr::LogoutCallBack), nPort);

	return TRUE;
}

//-------------------------------------------------------------------------------
// 销毁函数
//-------------------------------------------------------------------------------
VOID LoongWorldMgr::Destroy()
{
    // 清空LoongWorld集合
    LoongWorld* pLoongWorld = NULL;


	//清空世界状态表
	sBeton.ClearWorldStateTable();

    m_mapLoongWorld.ResetIterator();
    while( m_mapLoongWorld.PeekNext(pLoongWorld) )
    {
        SAFE_DEL(pLoongWorld);
    }
	m_mapLoongWorld.Clear();

	// 注销网络
	m_pSession->Destroy();
	KillObj("StreamServerWorld");

	// 注销所有的网络命令
	UnRegisterLoongWorldMsg();
	KillObj("NetCmdMgrWorld");
}

//-------------------------------------------------------------------------------
// 更新函数
//-------------------------------------------------------------------------------
VOID LoongWorldMgr::Update()
{
	// 更新每一个world
	LoongWorld* pLoongWorld = NULL;

	m_mapLoongWorld.ResetIterator();
	while( m_mapLoongWorld.PeekNext(pLoongWorld) )
	{
		pLoongWorld->Update();
		//更新世界状态
		UpdateWorldState(pLoongWorld);
		
	}
}
//-------------------------------------------------------------------------------
// 更新世界状态
//-------------------------------------------------------------------------------
VOID LoongWorldMgr::UpdateWorldState(LoongWorld* pLoongWorld)
{
	//隔5秒更新一下数据库 songg
	DWORD dwCurTime = timeGetTime();
	if( (dwCurTime - pLoongWorld->GetDBUpdateTime()) > sBeton.GetWorldStateUpdateTime() )
	{
		pLoongWorld->SetDBUpdateTime(dwCurTime);

		if(pLoongWorld->GetStatus() == EWS_Well && pLoongWorld->IsValid()) //登陆且没登出
		{
			SHORT  sState = ((DOUBLE)pLoongWorld->GetCurrOnlineDisplay()/pLoongWorld->GetMaxOnlineNum())*100; //百分比

			//刷新世界状态的DB
			sBeton.UpdateWorldState(pLoongWorld->GetWorldID(),pLoongWorld->GetCurrOnlineDisplay(),sState);
		}
		else
		{
			//刷新世界状态的DB
			sBeton.UpdateWorldState(pLoongWorld->GetWorldID(),0,-1);
		}

	}
	if( (dwCurTime - pLoongWorld->GetDBInsertTime()) > sBeton.GetWorldStateInsertTime() )
	{
		pLoongWorld->SetDBInsertTime(dwCurTime);

		if(pLoongWorld->GetStatus() == EWS_Well && pLoongWorld->IsValid()) //登陆且没登出
		{
			SHORT  sState = ((DOUBLE)pLoongWorld->GetCurrOnlineDisplay()/pLoongWorld->GetMaxOnlineNum())*100; //百分比

			//世界状态的log
			sBeton.InsertWorldStateLog(pLoongWorld->GetWorldID(),pLoongWorld->GetCurrOnlineDisplay(),sState);
		}
		else
		{
			//世界状态的log
			sBeton.InsertWorldStateLog(pLoongWorld->GetWorldID(),0,-1);
		}

	}
}
//-------------------------------------------------------------------------------
// 注册网络消息
//-------------------------------------------------------------------------------
VOID LoongWorldMgr::RegisterLoongWorldMsg()
{
	m_pMsgCmdMgr->Register("NWL_Certification",		(NETMSGPROC)m_Trunk.sfp2(&LoongWorldMgr::HandleCertification),		_T("NWL_Certification") );
	m_pMsgCmdMgr->Register("NWL_WorldStatus",		(NETMSGPROC)m_Trunk.sfp2(&LoongWorldMgr::HandleZoneServerStatus),	_T("NWL_WorldStatus") );
	m_pMsgCmdMgr->Register("NWL_PlayerWillLogin",	(NETMSGPROC)m_Trunk.sfp2(&LoongWorldMgr::HandlePlayerWillLogin),	_T("NWL_PlayerWillLogin") );
	m_pMsgCmdMgr->Register("NWL_PlayerLogin",		(NETMSGPROC)m_Trunk.sfp2(&LoongWorldMgr::HandlePlayerLogin),		_T("NWL_PlayerLogin") );
	m_pMsgCmdMgr->Register("NWL_PlayerLogout",		(NETMSGPROC)m_Trunk.sfp2(&LoongWorldMgr::HandlePlayerLogout),		_T("NWL_PlayerLogout") );
	m_pMsgCmdMgr->Register("NWL_KickLog",			(NETMSGPROC)m_Trunk.sfp2(&LoongWorldMgr::HandleKickLog),			_T("NWL_KickLog") );
	m_pMsgCmdMgr->Register("NWL_WorldColsed",		(NETMSGPROC)m_Trunk.sfp2(&LoongWorldMgr::HandleWorldClosed),		_T("NWL_WorldColsed") );
	m_pMsgCmdMgr->Register("NWL_ForbidAccount",		(NETMSGPROC)m_Trunk.sfp2(&LoongWorldMgr::HandleForbidAccount),		_T("NWL_ForbidAccount") );
}

//------------------------------------------------------------------------
// 反注册网络消息
//------------------------------------------------------------------------
VOID LoongWorldMgr::UnRegisterLoongWorldMsg()
{
	m_pMsgCmdMgr->Destroy();
}

//------------------------------------------------------------------------
// 登陆回调
//------------------------------------------------------------------------
DWORD LoongWorldMgr::LoginCallBack(LPBYTE pByte, DWORD dwSize)
{
	tagNWL_Certification* pRecv = (tagNWL_Certification*)pByte;

	// 验证LoongWorld,比较配置文件金色代码
	if( pRecv->dwGoldenCode != m_dwLoongWorldGoldenCode )
		return GT_INVALID;

	// 锁住更新锁
	g_login.LockUpdate();

	// 得到world的id
	DWORD dwNameCrc = m_pUtil->Crc32(pRecv->szWorldName);

	// 找到World
	LoongWorld* pWorld = GetLoongWorld(dwNameCrc);
	if( !P_VALID(pWorld) )
	{
		IMSG(_T("one invalid loongworld login, name=%s\r\n"), pRecv->szWorldName);

		// 释放锁
		g_login.UnlockUpdate();

		return GT_INVALID;
	}

	// 尝试初始化
	if( !pWorld->WorldLogin(pRecv->dwIP, pRecv->dwPort, pRecv->dwOLAccountID, pRecv->nOLAccountNum) )
	{
		IMSG(_T("one loongworld login which is already online, name=%s\r\n"), pRecv->szWorldName);

		// 释放锁
		g_login.UnlockUpdate();

		return GT_INVALID;
	}

	// 初始化成功
	IMSG(_T("Hello LoongWorld——%s\r\n"),pRecv->szWorldName);

	// 释放锁
	g_login.UnlockUpdate();

	// 返回ID
	return dwNameCrc;
}

//----------------------------------------------------------------------------
// 登出回调
//----------------------------------------------------------------------------
DWORD LoongWorldMgr::LogoutCallBack(DWORD dwParam)
{
	DWORD dwWorldID = dwParam;

	// 锁住更新锁
	g_login.LockUpdate();

	// 查找world
	LoongWorld* pWorld = GetLoongWorld(dwWorldID);
	if( !P_VALID(pWorld) )
	{
		// 释放锁
		g_login.UnlockUpdate();

		return 0;
	}

	// 尝试登出
	pWorld->WorldLogout();
	IMSG(_T("Bye LoongWorld——%s\r\n"), pWorld->GetName());

	// 释放锁
	g_login.UnlockUpdate();

	return 0;
}

//------------------------------------------------------------------------------
// 添加到游戏世界
//------------------------------------------------------------------------------
VOID LoongWorldMgr::AddToWorld(Player* pPlayer, DWORD dwWorldNameCrc)
{
	if( !P_VALID(pPlayer) ) return;

	LoongWorld* pWorld = GetLoongWorld(dwWorldNameCrc);
	if( !P_VALID(pWorld) ) return;

	pWorld->AddPlayer(pPlayer);

	// 写入数据库
	sBeton.PlayerLogin(pPlayer->GetAccountID(), dwWorldNameCrc);
}

//------------------------------------------------------------------------------
// 处理验证消息
//------------------------------------------------------------------------------
DWORD LoongWorldMgr::HandleCertification(tagNetCmd* pMsg, LoongWorld* pWorld)
{
	tagNWL_Certification* pRecv = (tagNWL_Certification*)pMsg;

	// 发送返回消息
	tagNLW_Certification send;
	send.dwGoldenCode = m_dwLoginServerGoldenCode;

	SendMsg(pWorld->GetID(), &send, send.dwSize);

	return 0;
}

//-------------------------------------------------------------------------------
// 处理服务器信息消息
//-------------------------------------------------------------------------------
DWORD LoongWorldMgr::HandleZoneServerStatus(tagNetCmd* pMsg, LoongWorld* pWorld)
{
	tagNWL_WorldStatus* pRecv = (tagNWL_WorldStatus*)pMsg;

	pWorld->UpdateStatus(pRecv->eStatus, pRecv->nCurPlayerNum, pRecv->nPlayerNumLimit, pRecv->nCurPlayerDisplay);

	return 0;
}

//--------------------------------------------------------------------------------
// 玩家登陆
//--------------------------------------------------------------------------------
DWORD LoongWorldMgr::HandlePlayerLogin(tagNetCmd* pMsg, LoongWorld* pWorld)
{
	tagNWL_PlayerLogin* pRecv = (tagNWL_PlayerLogin*)pMsg;

	// 如果登陆不成功
	if( E_Success != pRecv->dwErrorCode )
	{
		ILOG->Write(_T("Player Login Failure! AccountID:%u ErrorCode=%d\r\n"), pRecv->dwAccountID, pRecv->dwErrorCode);

		// 更新数据库
		sBeton.PlayerLogout(pRecv->dwAccountID);
		if (g_login.IsEnableFatigueGuard())
			sFatigueMgr.LogoutNotify(pRecv->dwAccountID);
	}
	// 如果登录成功
	else
	{
		// 更新数据库
		sBeton.PlayerEnterWorld(pRecv->dwAccountID, pWorld->GetID());

		// login log
		const tagAccountData* pAccountData = sPlayerMgr.GetCachedAccountData(pRecv->dwAccountID);
		if (!P_VALID(pAccountData))
		{
			ASSERT(0);
			return GT_INVALID;
		}
		if (g_login.IsEnableFatigueGuard())
			sFatigueMgr.LoginNotify(pRecv->dwAccountID, pWorld->GetID(), pAccountData->bGuard);
		//记录本次登录的ip和time 供下次使用
		sBeton.UpdateAccountLoginInfo(pRecv->dwAccountID, pRecv->dwIP);

		//写login的log
		sBeton.LogPlayerAction(pRecv->dwAccountID, pAccountData->szAccountName, pRecv->dwIP, "login");
		sPlayerMgr.CacheIpAddres(pRecv->dwAccountID, pRecv->dwIP);
	}

	return 0;
}

//-----------------------------------------------------------------------------------
// 玩家登出
//-----------------------------------------------------------------------------------
DWORD LoongWorldMgr::HandlePlayerLogout(tagNetCmd* pMsg, LoongWorld* pWorld)
{
	tagNWL_PlayerLogout* pRecv = (tagNWL_PlayerLogout*)pMsg;

	// 更新数据库
	sBeton.PlayerOutWorld(pRecv->dwAccountID, pWorld->GetID());

	// logout log
	const tagAccountData* pAccountData = sPlayerMgr.GetCachedAccountData(pRecv->dwAccountID);
	if (!P_VALID(pAccountData))
	{
		ASSERT(0);
		return GT_INVALID;
	}

	//orange的需要登出的时候发个消息给它
	if(sPlayerMgr.GetProofPolicy() == EPP_Orange)
	{
		ProofPolicy* pPolicy =	sPlayerMgr.GetProofPolicyPtr();
		if(P_VALID(pPolicy))
		{
			pPolicy->SigPlayerLogoutEvent(pRecv->dwAccountID);
		}
	}

	sBeton.LogPlayerAction(pRecv->dwAccountID, pAccountData->szAccountName, pAccountData->dwIp, "logout");
	if (g_login.IsEnableFatigueGuard())
		sFatigueMgr.LogoutNotify(pRecv->dwAccountID);

	return 0;
}

//------------------------------------------------------------------------------------
// LoongWorld通知进入游戏世界请求返回结果
//-------------------------------------------------------------------------------------
DWORD LoongWorldMgr::HandlePlayerWillLogin(tagNetCmd* pMsg, LoongWorld* pWorld)
{
	tagNWL_PlayerWillLogin* pRecv = (tagNWL_PlayerWillLogin*)pMsg;

	pWorld->PlayerWillLoginRet(pRecv->dwAccountID, pRecv->dwErrorCode);

	return 0;
}
//-----------------------------------------------------------------------
// LoongWorld即将关闭
//-----------------------------------------------------------------------
DWORD LoongWorldMgr::HandleWorldClosed(tagNetCmd* pMsg, LoongWorld* pWorld)
{
	//改变对应玩家的登录状态
	sBeton.FixOneWorldPlayerLoginStatus(pWorld->GetID(), EPLS_Null, EPLS_OffLine);

	tagNLW_WorldColsed send;
	SendMsg(pWorld->GetID(), &send, send.dwSize);

	return 0;
}
//-----------------------------------------------------------------------
// LoongWorld踢出玩家
//-----------------------------------------------------------------------
DWORD LoongWorldMgr::HandleKickLog(tagNetCmd* pMsg, LoongWorld* pWorld)
{
	tagNWL_KickLog* pRecv = (tagNWL_KickLog*)pMsg;

	const tagAccountData* pAccountData = sPlayerMgr.GetCachedAccountData(pRecv->dwAccountID);
	if (!P_VALID(pAccountData))
	{
		ASSERT(0);
		return GT_INVALID;
	}

// 	ILOG->Write(_T("Kick Name: %s  AccountID: %d  Time: %d  Seal: %d ErrCode: %d\r\n "), 
// 				pAccountData->szAccountName, pRecv->dwAccountID, pRecv->dwKickTime, pRecv->bySeal, pRecv->u16ErrCode);

	sBeton.InsertKickLog(pAccountData->szAccountName, pRecv->dwAccountID, pRecv->dwKickTime, pRecv->u16ErrCode, pRecv->bySeal);
	if (pRecv->bySeal && pWorld->IsAutoSeal())
	{
		//songg
		DWORD dwErrorCode = sBeton.ForbidAccount(pRecv->dwAccountID, EPLM_WaiGua);
		if(dwErrorCode == E_RT_Success)
		{
			if( P_VALID(pWorld) )
			{
				tagNLW_KickPlayer send;
				send.dwAccountID = pRecv->dwAccountID;
				pWorld->SendKickPlayerMsg(send);
			}
		}
	}

	return 0;
}

//-----------------------------------------------------------------------
// LoongWorld踢出玩家
//-----------------------------------------------------------------------
DWORD LoongWorldMgr::HandleForbidAccount( tagNetCmd* pMsg, LoongWorld* pWorld )
{
	tagNWL_ForbidAccount* pRecv = (tagNWL_ForbidAccount*)pMsg;

	//songg
	DWORD dwErrorCode = sBeton.ForbidAccount(pRecv->dwAccountID, pRecv->dwForbidCode);
	if(dwErrorCode == E_RT_Success)
	{
		if( P_VALID(pWorld) )
		{
			tagNLW_KickPlayer send;
			send.dwAccountID = pRecv->dwAccountID;
			pWorld->SendKickPlayerMsg(send);
		}
	}

	return E_Success;
}


