//-----------------------------------------------------------------------------
//!\file player_mgr.cpp
//!\author Aslan
//!
//!\date 2008-11-30
//! last 2008-11-30
//!
//!\brief 玩家管理器
//!
//!	Copyright (c) 2004 TENGWU Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#include "stdafx.h"

#include "../WorldDefine/login.h"

#include "player_mgr.h"
#include "player.h"
#include "loongworld_mgr.h"
#include "loongworld.h"
#include "db.h"
#include "proof_policy_mgr.h"
#include "proof_policy.h"
#include "login_server.h"

//-------------------------------------------------------------------------------
// 构造函数
//-------------------------------------------------------------------------------
PlayerMgr::PlayerMgr() : m_Trunk(this), m_nPlayerNum(0), m_nPlayerLoging(0), m_nProofResultNum(0),
						m_dwClientIDGen(0), m_dwCurrVer(0), m_nPort(0), m_ePolicy(EPP_Null),m_pPolicy(NULL),m_bUseGMAccess(TRUE)
{
}

//-------------------------------------------------------------------------------
// 析构函数
//-------------------------------------------------------------------------------
PlayerMgr::~PlayerMgr()
{
	Destroy();
}

//-------------------------------------------------------------------------------
// 初始化函数
//-------------------------------------------------------------------------------
BOOL PlayerMgr::Init()
{
	// vEngine
	m_pUtil = "Util";

	// 读取配置文件
	TObjRef<VarContainer> pVar = "LoginServerConfig";

	//初始化
	if(!InitConfig())
	{
		return FALSE;
	}

	sBeton.LoadCacheAccountData(m_mapAccountData, m_mapAccountNameCrc2AccountID);

	// 读取验证方式
	m_ePolicy = (enum EProofPolicy)pVar->GetInt(_T("policy"), _T("proof"));
	if( m_ePolicy < EPP_Test || m_ePolicy >= EPP_End )
	{
		return FALSE;
	}

	// 生成验证方式
	m_pPolicy = sProofPolicyMgr.CreateProofPolicy(m_ePolicy);
	if( !P_VALID(m_pPolicy) )
	{
		return FALSE;
	}
	if( !m_pPolicy->Init((PROOFCALLBACK)m_Trunk.sfp2(&PlayerMgr::ProofCallBack)) )
	{
		return FALSE;
	}

	// 读取版本号
	LPCTSTR szVerNum = pVar->GetString(_T("version version"));
	m_dwCurrVer = m_pUtil->Crc32(szVerNum);

	// 读取端口号
	m_nPort = pVar->GetInt(_T("port player_session"));

	// 初始化网络
	m_pNetSession = "XServer";

	tagXServerConfig InitParam;
	InitParam.fnLogIn			=	(XLOGINCALLBACK)m_Trunk.sfp2(&PlayerMgr::LoginCallBack);
	InitParam.fnLogOut			=	(XLOGOUTCALLBACK)m_Trunk.sfp1(&PlayerMgr::LogoutCallBack);
	InitParam.bReusePort		=	true;
	InitParam.nPort				=	m_nPort;
	InitParam.nMaxServerLoad	=	20000;
	InitParam.nAcceptExNum		=	128;

	m_pNetSession->Init(InitParam);
	m_nPort = m_pNetSession->GetConfig()->nPort;

	// 注册网络消息
	Player::RegisterPlayerMsg();

	return TRUE;
}
//-------------------------------------------------------------------------
// 初始化配置
//-------------------------------------------------------------------------
BOOL PlayerMgr::InitConfig()
{
	TObjRef<VarContainer> pVar = "GMAccess";
	if( !P_VALID(pVar) ) return FALSE;

	std::set <tstring> setAccountName;
	
	m_bUseGMAccess = pVar->GetInt(_T("on switch"));//是否使用gm地址限制

	//读取account名
	INT iAccountNum = pVar->GetInt(_T("num account"));

	TCHAR szTemp[X_SHORT_NAME] = {_T('\0')};
	for(int iLoop = 1; iLoop<=iAccountNum; ++iLoop)
	{
		_stprintf(szTemp,_T("name%d"),iLoop);
		setAccountName.insert(pVar->GetString(szTemp,_T("account")));
	}
	CHAR cIP[X_IP_LEN]= {_T('\0')};
	CHAR cName[X_SHORT_NAME]= {_T('\0')};
	std::set <tstring>::iterator itLoop = setAccountName.begin();
	for(;itLoop != setAccountName.end();++itLoop)
	{
		INT iIPNum = pVar->GetInt(_T("num"), (*itLoop).c_str());
		for(int iLoop = 1; iLoop<=iIPNum; ++iLoop)
		{
			_stprintf(szTemp,_T("ip%d"),iLoop);

			//转成ansi
			tstring strIP = pVar->GetString(szTemp,(*itLoop).c_str());
			WideCharToMultiByte(CP_OEMCP,NULL,strIP.c_str(),-1,cIP,X_IP_LEN,NULL,FALSE);

			//转成DWORD
			DWORD dwIP = inet_addr(cIP);
			m_mapGMvsIP.insert( make_pair((*itLoop).c_str(),dwIP) );

		}		
	}
	return TRUE;
}
//-------------------------------------------------------------------------------
// 销毁函数
//-------------------------------------------------------------------------------
VOID PlayerMgr::Destroy()
{
	// 取消注册网络消息
	Player::UnRegisterPlayerMsg();

	// 注销网络
	m_pNetSession->Destroy();

    // 清空正在登录的玩家
    Player* pPlayer = NULL;
    m_mapLoginingPlayer.ResetIterator();
    while( m_mapLoginingPlayer.PeekNext(pPlayer) )
    {
        SAFE_DEL(pPlayer);
    }

	// 验证策略销毁
	m_pPolicy->Destroy();

	// 删除验证策略
	sProofPolicyMgr.DestroyProofPolicy(m_ePolicy, m_pPolicy);

	// 清除账号名缓冲
	CleanCachedAccountDatas();
}

//-------------------------------------------------------------------------------
// 更新函数
//-------------------------------------------------------------------------------
VOID PlayerMgr::Update()
{
	// 更新玩家消息
    UpdateSession();

	// 更新验证结果
	UpdateProofResult();

	// 得到统计信息
	InterlockedExchange((LPLONG)&m_nPlayerNum,		m_mapAllPlayer.Size());
	InterlockedExchange((LPLONG)&m_nPlayerLoging,	m_mapLoginingPlayer.Size());
	InterlockedExchange((LPLONG)&m_nProofResultNum,	m_listProofResult.Size());
}

//-------------------------------------------------------------------------------
// 更新玩家消息
//-------------------------------------------------------------------------------
VOID PlayerMgr::UpdateSession()
{
	Player* pPlayer = NULL;
	m_mapLoginingPlayer.ResetIterator();
	while( m_mapLoginingPlayer.PeekNext(pPlayer) )
	{
		INT nRet = pPlayer->HandleMessage();

		if( CON_LOST == nRet )	// 掉线
		{
			m_mapLoginingPlayer.Erase(pPlayer->GetClientID());
			PlayerLogout(pPlayer);
		}
	}
}

//-------------------------------------------------------------------------------
// 更新验证返回
//-------------------------------------------------------------------------------
VOID PlayerMgr::UpdateProofResult()
{
	// 每个Tick最多处理20个人
	INT nNum = 20;

	// 取出验证结果
	tagProofResultFull* pResult = m_listProofResult.PopFront();

	while( P_VALID(pResult) )
	{
		// 找到玩家
		Player* pPlayer = m_mapLoginingPlayer.Peek(pResult->dwClientID);
		if( !P_VALID(pPlayer) )
		{
			SAFE_DEL(pResult);
			pResult = m_listProofResult.PopFront();
			continue;
		}

		if( E_Success != pResult->nRet )
		{
			// 如果返回帐号已经登陆
			if( E_ProofResult_Account_In_Use == pResult->nRet )
			{
				// 如果数据库是在线状态，则发往world服务器踢人
				if( EPLS_Online == pResult->eLoginStatus )
				{
					LoongWorld* pWorld = sLoongWorldMgr.GetLoongWorld(pResult->dwWorldNameCrc);
					if( P_VALID(pWorld) )
					{
						tagNLW_KickPlayer send;
						send.dwAccountID = pResult->dwAccountID;
						pWorld->SendKickPlayerMsg(send);
					}
					else
					{
						sBeton.FixPlayerLoginStatus(pResult->dwAccountID, EPLS_OffLine);
						pResult->nRet = E_Success;	// 注意这个地方
					}
				}
				// 如果这时候帐号处在登陆中
				else if( EPLS_Loging == pResult->eLoginStatus )
				{
					// 在本地查找是否该账号在login里面，如果有，不管了
					if( IsAccountExist(pResult->dwAccountID) )
					{

					}
					// 如果没有，则可能是因为有些情况导致不同步，那么就直接清空数据库，并设置成功
					else
					{
						sBeton.FixPlayerLoginStatus(pResult->dwAccountID, EPLS_OffLine);
						pResult->nRet = E_Success;	// 注意这个地方
					}
				}
			}
			//如果是GM工具封停 往world_forbid表里搜索一下
			else if( pResult->nRet == E_ProofResult_Forbid_GMTool )
			{
				BOOL bForbid = sBeton.IfWorldForbid(pResult->dwAccountID,pResult->dwWorldNameCrc);

				// 这表示该账号可能是别的游戏世界被封停了
				if( bForbid == FALSE )
				{
					pResult->nRet = E_Success;	// 注意这个地方
				}
			}
			// 其它情况
			else
			{

			}
		}
		// 验证成功
		else
		{
			// 如果是权限为gm到m_mapGMvsIP表里找
			if( m_bUseGMAccess && (pResult->byPrivilege != 0) )
			{
				BOOL bFind = FALSE;
				typedef std::multimap<tstring, DWORD>::iterator itFinder;

				itFinder beg = m_mapGMvsIP.lower_bound(pPlayer->GetAccoutNameT()),
					end = m_mapGMvsIP.upper_bound(pPlayer->GetAccoutNameT());
				while( beg != end )
				{
					if( beg->second == pPlayer->GetPlayerData().dwIP )
					{
						bFind = TRUE;
						break;
					}
					++beg;
				}
				if( bFind == FALSE )
				{
					// 目前对应成这个
					pResult->nRet = E_ProofResult_Account_No_Match;	// 注意这个地方

					TObjRef<StreamTransport> pTrans;
					pTrans = "StreamTransport";
					char szIP[X_IP_LEN];
					strcpy(szIP,pTrans->IP2String(pPlayer->GetPlayerData().dwIP));

					TCHAR tszIP[X_IP_LEN];
#ifdef UNICODE
					MultiByteToWideChar(CP_ACP,NULL,szIP,-1,tszIP,X_SHORT_NAME);
#else
					strcpy(tszIP,szIP);
#endif
					ILOG->Write(_T("GM:%s非法登录服务器!!	IP:%s\r\n"),pPlayer->GetAccoutNameT(),tszIP);
				}	
			}
		}

		// 如果对应客户端的ip被封了
		if( TRUE == sBeton.IfIPForbid(pPlayer->GetPlayerData().dwIP) )
		{
			//目前对应成这个
			pResult->nRet = E_ProofResult_Account_No_Match;	// 注意这个地方
		}

		// 如果验证成功，此时再验证一下游戏世界
		if( E_Success == pResult->nRet )
		{
			// 找到玩家所要请求进入的游戏世界
			LoongWorld* pWorld = sLoongWorldMgr.GetLoongWorld(pPlayer->GetLoginWorldNameCrc());
			if( !P_VALID(pWorld) || EWS_Well != pWorld->GetStatus() )
			{
				pResult->nRet = E_SelectWorld_Server_Maintenance;
			}
		}


		// 最后验证完毕
		if( E_Success == pResult->nRet )
		{
			// 设置玩家验证完毕
			pPlayer->ProofReturn(pResult);

			//设置玩家上次登录ip和时间
			pPlayer->SetPreLoginIP(pResult->dwPreLoginIP);
			pPlayer->SetPreLoginTime(pResult->dwPreLoginTime);

			// 如果该玩家还需要密保
			if( pPlayer->IsNeedMibao() )
			{
				tagNLS_MiBao send;
				pPlayer->GenerateMibao(send.szMiBao);	// 生成密保
				pPlayer->SendMessage(&send, send.dwSize);
			}
			// 不需要密保，则加入到游戏世界列表中等待
			else
			{
				RemovePlayerFromLogining(pPlayer->GetClientID());

				// 加入帐号
				AddAccount(pPlayer->GetAccountID(), pPlayer->GetClientID());

				// 将玩家加入到LoongWorld的玩家列表中
				sLoongWorldMgr.AddToWorld(pPlayer, pPlayer->GetLoginWorldNameCrc());
			}
		}
		// 如果最终没有验证成功
		else
		{
			tagNLS_ProofResult send;
			send.dwErrorCode = pResult->nRet;

			pPlayer->SendMessage(&send, send.dwSize);
		}

		SAFE_DEL(pResult);

		// 减一个人，如果减到0就退出
		if( --nNum <= 0 ) break;

		pResult = m_listProofResult.PopFront();
	}
}

//-------------------------------------------------------------------------------
// 登录回调函数
//-------------------------------------------------------------------------------
DWORD PlayerMgr::LoginCallBack(LPVOID p1, LPVOID p2)
{
    //查看第一条消息是否正确
    static DWORD dwMsgCrc = m_pUtil->Crc32("NLC_SafeProof");

	tagUnit* pUnit = (tagUnit*)p1;
	tagXLoginParam* pParam = (tagXLoginParam*)p2;

	tagNLC_SafeProof* pCmd = (tagNLC_SafeProof*)pUnit->pBuffer;

	// 检查消息的Crc是否相同
	if( pCmd->dwID != dwMsgCrc )
	{
		return GT_INVALID;
	}

	//加入验证key
	DWORD dwProofKey = 0;
	ProofKey(dwProofKey,pCmd);
	if(dwProofKey != pCmd->dwProofKey)
	{
		return GT_INVALID; //验证key不对
	}

	// 锁住Update
	g_login.LockUpdate();

	// 生成一个新的客户端ID
	DWORD dwClientID = GenerateClientID();
    
	// 生成玩家
	Player* pPlayer = new Player(dwClientID, pParam->dwHandle, pParam->dwAddress, pCmd->dwWorldNameCrc);
	if( !P_VALID(pPlayer) )
	{
		g_login.UnlockUpdate();
		return GT_INVALID;
	}

	// 将玩家加入到列表中
	AddPlayerToAll(pPlayer);
	// 将玩家加入到登陆玩家列表中
	AddPlayerToLogining(pPlayer);

	// 释放Update
	g_login.UnlockUpdate();

    return dwClientID;
}

//-------------------------------------------------------------------------------
// 登出回调函数
//-------------------------------------------------------------------------------
DWORD PlayerMgr::LogoutCallBack(LPVOID p)
{
	// 锁住Update
	g_login.LockUpdate();

	// 得到ID
    DWORD dwClientID = (DWORD)p;

    // 在所有玩家中查找玩家
    Player* pPlayer = m_mapAllPlayer.Peek(dwClientID);
    if( P_VALID(pPlayer) )
    {
        // 设置此玩家断开连接标志
		pPlayer->SetConnLost();
    }

	// 释放Update
	g_login.UnlockUpdate();

    return 0;
}

//---------------------------------------------------------------------------------
// 玩家登出
//---------------------------------------------------------------------------------
VOID PlayerMgr::PlayerLogout(Player* pPlayer)
{
	BOOL bLogoutFromDB = TRUE;		// 是否设置数据库中该玩家为离线

	if( !pPlayer->IsProofEnd() || GT_INVALID == pPlayer->GetAccountID() )	// 还没有验证成功，数据库根本就没有操作过
	{
		bLogoutFromDB = FALSE;
	}
	else if( pPlayer->IsSelectWorldOk() )	// world已经给了回应，说明马上就会登入到world上
	{
		bLogoutFromDB = FALSE;
	}

	// 如果需要设置数据库
	if( bLogoutFromDB )
	{
		sBeton.PlayerLogout(pPlayer->GetAccountID());
	}

	// 如果帐号合法，则移除帐号
	if( GT_INVALID != pPlayer->GetAccountID() )
	{
		RemoveAccount(pPlayer->GetAccountID());
	}

	// 从列表中删除该玩家
	RemovePlayerFromAll(pPlayer->GetClientID());

	// 删除玩家
	SAFE_DEL(pPlayer);
}

//------------------------------------------------------------------------------------
// 发送验证
//------------------------------------------------------------------------------------
VOID PlayerMgr::Proof(DWORD dwClientID, LPCSTR szAccoutName, LPCSTR szPsd, LPCSTR szGUID, DWORD dwIP)
{
	if( !P_VALID(szAccoutName) || !P_VALID(szPsd) || !P_VALID(szGUID) )
		return;

	// 发送给相应的验证策略进行验证
	m_pPolicy->Proof(dwClientID, szAccoutName, szPsd, szGUID, dwIP);
}

//--------------------------------------------------------------------------------------
// 验证回调函数
//--------------------------------------------------------------------------------------
VOID PlayerMgr::ProofCallBack(INT nRet, tagProofResult* pResult)
{
	tagProofResultFull* pResultFull = new tagProofResultFull;
	memcpy(pResultFull, pResult, sizeof(tagProofResult));
	pResultFull->nRet = nRet;

	m_listProofResult.PushBack(pResultFull);
}

VOID PlayerMgr::CacheAccountName( DWORD dwAccountID, const CHAR* szAccountName)
{
	tagAccountData* pAccountData = m_mapAccountData.Peek(dwAccountID);		
	if (!P_VALID(pAccountData))
	{
		pAccountData = new tagAccountData;
		m_mapAccountData.Add(dwAccountID, pAccountData);
	}
	memcpy(pAccountData->szAccountName, szAccountName, X_SHORT_NAME);
}

VOID PlayerMgr::CacheIpAddres( DWORD dwAccountID, DWORD dwIp)
{
	tagAccountData* pAccountData = m_mapAccountData.Peek(dwAccountID);		
	if (!P_VALID(pAccountData))
	{
		pAccountData = new tagAccountData;
		m_mapAccountData.Add(dwAccountID, pAccountData);
	}
	pAccountData->dwIp = dwIp;
}

VOID PlayerMgr::CacheGuard( DWORD dwAccountID, BOOL bGuard)
{
	tagAccountData* pAccountData = m_mapAccountData.Peek(dwAccountID);		
	if (!P_VALID(pAccountData))
	{
		pAccountData = new tagAccountData;
		m_mapAccountData.Add(dwAccountID, pAccountData);
	}
	pAccountData->bGuard = bGuard;
}

VOID PlayerMgr::EraseCachedAccountData( DWORD dwAccountID )
{
	tagAccountData* pAccountData = m_mapAccountData.Peek(dwAccountID);
	if (P_VALID(pAccountData))
	{
		SAFE_DEL(pAccountData);
		m_mapAccountData.Erase(dwAccountID);
	}
}

VOID PlayerMgr::CleanCachedAccountDatas()
{
	std::list<DWORD> listIDs;
	m_mapAccountData.ExportAllKey(listIDs);
	std::list<DWORD>::iterator itr = listIDs.begin();
	while(itr != listIDs.end())
	{
		EraseCachedAccountData(*itr);
		++itr;
	}
}

VOID PlayerMgr::MapAccountName2AccountID( LPCSTR szAccountName, DWORD dwAccountID )
{
	DWORD dwNameCrc = m_pUtil->Crc32(szAccountName);
	if (!GT_VALID(m_mapAccountNameCrc2AccountID.Peek(dwNameCrc)))
	{
		m_mapAccountNameCrc2AccountID.Add(dwNameCrc, dwAccountID);
	}
}

DWORD PlayerMgr::GetAccountIDByAccountName( LPCSTR szAccountName )
{
	DWORD dwNameCrc = m_pUtil->Crc32(szAccountName);
	return m_mapAccountNameCrc2AccountID.Peek(dwNameCrc);
}






