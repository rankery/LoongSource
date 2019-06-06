//-----------------------------------------------------------------------------
//!\file proof_policy_own.cpp
//!\author Aslan
//!
//!\date 2008-11-30
//! last 2008-11-30
//!
//!\brief 自己公司的验证策略——中央激活
//!
//!	Copyright (c) 2004 TENGWU Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#pragma once
#include "stdafx.h"

#include "../ServerDefine/base_define.h"
#include "../ServerDefine/msg_rt_errorcode.h"
#include "proof_policy_own_define.h"
#include "proof_policy.h"
#include "proof_policy_own.h"
#include "login_server.h"
#include "db.h"
//-------------------------------------------------------------------------
// 初始化
//-------------------------------------------------------------------------
BOOL ProofPolicyOwnDB::Init()
{
	TObjRef<VarContainer> pVar = "LoginServerConfig";
	if( !P_VALID(pVar) ) return FALSE;

	BOOL bRet = m_DB.Init(pVar->GetString(_T("ip database")), 
		pVar->GetString(_T("user database")),
		pVar->GetString(_T("psd database")), 
		pVar->GetString(_T("name database")),
		(INT)pVar->GetDword(_T("port database")) );

	return bRet;
}

//-------------------------------------------------------------------------
// 查询
//-------------------------------------------------------------------------
BOOL ProofPolicyOwnDB::QueryAccount(LPCSTR szAccountName, string& strPsd, tagProofResult* pProofResult)
{
	if( !P_VALID(szAccountName) || !P_VALID(pProofResult) ) return FALSE;

	MyStream* pStream = m_DB.GetStream();
	if( !P_VALID(pStream) ) return FALSE;

	TObjRef<StreamTransport> pTrans;
	pTrans = "StreamTransport";

	char szIP[X_IP_LEN] = "";
	char szDateTime[X_DATATIME_LEN + 1]= "";
	// 申请连接
	Connection* pCon = m_DB.GetFreeConnection();

	pStream->SetSelect("account", "id,forbid_mask,privilege,guard,login_status,worldname_crc,psd,mibao,ip,time");
	pStream->SetWhere();
	pStream->FillString("name='").FillString(szAccountName, pCon).FillString("'");

	// 释放连接
	m_DB.ReturnConnection(pCon);

	QueryResult* pResult = m_DB.Query(pStream);

	// 释放流
	m_DB.ReturnStream(pStream);

	// 查询结果为空
	if( !P_VALID(pResult) || pResult->GetRowCount() <= 0) return FALSE;

	// 设置结果
	pProofResult->dwAccountID		=	(*pResult)[0].GetDword();
	pProofResult->dwFrobidMask		=	(*pResult)[1].GetDword();
	pProofResult->byPrivilege		=	(*pResult)[2].GetByte();
	pProofResult->bGuard			=	(*pResult)[3].GetBool();
	pProofResult->nGuardAccuTime	=	0;
	pProofResult->eLoginStatus		=	(EPlayerLoginStatus)(*pResult)[4].GetInt();
	pProofResult->dwWorldNameCrc	=	(*pResult)[5].GetDword();

	// 记录密码
	strPsd = (*pResult)[6].GetString();

	// 记录密保
	CHAR szMibao[MIBAO_LEN] = {'\0'};
	BOOL bRet = (*pResult)[7].GetBlob(szMibao, MIBAO_LEN);
	if( '\0' == szMibao[0] )
	{
		pProofResult->bNeedMibao = FALSE;
	}
	else
	{
		pProofResult->bNeedMibao = TRUE;
	}

	//上次登录的时间和ip
	memcpy(szIP,(*pResult)[8].GetString(),(*pResult)[8].GetLen());
	memcpy(szDateTime,(*pResult)[9].GetString(),(*pResult)[9].GetLen());

	pProofResult->dwPreLoginIP = pTrans->StringIP2IP(szIP);
	DataTime2DwordTime(pProofResult->dwPreLoginTime,szDateTime,(*pResult)[9].GetLen());

	// 归还结果集
	m_DB.FreeQueryResult(pResult);

	return TRUE;
}

//-------------------------------------------------------------------------
// 插入玩家记录
//-------------------------------------------------------------------------
BOOL ProofPolicyOwnDB::InsertAccout(LPCSTR szAccountName, LPCSTR szPsd, LPCSTR szMiBao, BOOL bGuard)
{
	if( !P_VALID(szAccountName) || !P_VALID(szPsd) ) return FALSE;

	MyStream* pStream = m_DB.GetStream();
	if( !P_VALID(pStream) ) return FALSE;

	// 申请连接
	Connection* pCon = m_DB.GetFreeConnection();

	pStream->SetInsert("account");
	pStream->FillString("name='").FillString(szAccountName, pCon).FillString("',");
	pStream->FillString("psd='").FillString(szPsd, pCon).FillString("',");
	pStream->FillString("mibao='").FillBlob(szMiBao, MIBAO_LEN, pCon).FillString("',");
	pStream->FillString("guard=") << bGuard;

	// 释放连接
	m_DB.ReturnConnection(pCon);

	// 查询
	BOOL bRet = m_DB.Execute(pStream);

	// 释放流
	m_DB.ReturnStream(pStream);

	return bRet;
}

//----------------------------------------------------------------------------
// 更新玩家记录
//----------------------------------------------------------------------------
BOOL ProofPolicyOwnDB::UpdateAccout(LPCSTR szAccountName, LPCSTR szPsd, LPCSTR szMiBao, BOOL bGuard)
{
	if( !P_VALID(szAccountName) || !P_VALID(szPsd) ) return FALSE;

	MyStream* pStream = m_DB.GetStream();
	if( !P_VALID(pStream) ) return FALSE;

	// 申请连接
	Connection* pCon = m_DB.GetFreeConnection();

	pStream->SetUpdate("account");
	pStream->FillString("psd='").FillString(szPsd, pCon).FillString("',");
	pStream->FillString("mibao='").FillBlob(szMiBao, MIBAO_LEN, pCon).FillString("',");
	pStream->FillString("guard=") << bGuard;
	pStream->SetWhere();
	pStream->FillString("name='").FillString(szAccountName, pCon).FillString("'");

	// 释放连接
	m_DB.ReturnConnection(pCon);

	// 查询
	BOOL bRet = m_DB.Execute(pStream);

	// 释放流
	m_DB.ReturnStream(pStream);

	return bRet;
}

//-------------------------------------------------------------------------
// 更新密码
//-------------------------------------------------------------------------
BOOL ProofPolicyOwnDB::UpdatePsd(LPCSTR szAccountName, LPCSTR szNewPsd)
{
	if( !P_VALID(szAccountName) || !P_VALID(szNewPsd) ) return FALSE;

	MyStream* pStream = m_DB.GetStream();
	if( !P_VALID(pStream) ) return FALSE;

	// 申请连接
	Connection* pCon = m_DB.GetFreeConnection();

	pStream->SetUpdate("account");
	pStream->FillString("psd='").FillString(szNewPsd, pCon).FillString("'");
	pStream->SetWhere();
	pStream->FillString("name='").FillString(szAccountName, pCon).FillString("'");

	// 释放连接
	m_DB.ReturnConnection(pCon);

	// 查询
	BOOL bRet = m_DB.Execute(pStream);

	// 释放流
	m_DB.ReturnStream(pStream);

	return bRet;
}

//-------------------------------------------------------------------------
// 更新密保
//-------------------------------------------------------------------------
BOOL ProofPolicyOwnDB::UpdateMibao(LPCSTR szAccountName, LPCSTR szMibao)
{
	if( !P_VALID(szAccountName) ) return FALSE;

	MyStream* pStream = m_DB.GetStream();
	if( !P_VALID(pStream) ) return FALSE;

	// 申请连接
	Connection* pCon = m_DB.GetFreeConnection();

	pStream->SetUpdate("account");
	pStream->FillString("mibao='").FillBlob(szMibao, MIBAO_LEN, pCon).FillString("'");
	pStream->SetWhere();
	pStream->FillString("name='").FillString(szAccountName, pCon).FillString("'");

	// 释放连接
	m_DB.ReturnConnection(pCon);

	// 查询
	BOOL bRet = m_DB.Execute(pStream);

	// 释放流
	m_DB.ReturnStream(pStream);

	return bRet;
}

//-------------------------------------------------------------------------
// 初始化
//-------------------------------------------------------------------------
BOOL ProofPolicyOwn::Init(PROOFCALLBACK fn)
{
	m_pUtil		=	"Util";
	m_pThread	=	"Thread";

	m_fnCallback = fn;

	// 读取配置文件
	if( !InitConfig() )
	{
		return FALSE;
	}

	// 初始化数据库
	if( !m_ProofDB.Init() )
	{
		return FALSE;
	}

	// 生成网络命令管理器
	CreateObj("NetCmdMgrProofPolicyOwn", "NetCmdMgr");
	m_pCmdMgr = "NetCmdMgrProofPolicyOwn";
	if( !P_VALID(m_pCmdMgr) ) return FALSE;

	// 初始化网络
	CreateObj("StreamTransportProofPolicyOwn", "StreamTransport");
	m_pTransport = "StreamTransportProofPolicyOwn";
	if( !P_VALID(m_pTransport) ) return FALSE;
	m_pTransport->Init();

	// 注册网络命令
	RegisterProofMsg();

	// 启动连接线程
	if(!m_pThread->CreateThread(_T("ProofPolicyOwnConnectServer"), 
		(vEngine::THREADPROC)m_Trunk.sfp0(&ProofPolicyOwn::ThreadConnectServer), 
		NULL))
	{
		return FALSE;
	}

	while(!m_pThread->IsThreadActive(_T("ProofPolicyOwnConnectServer")))
	{
		continue;
	}

	// 启动更新线程
	if(!m_pThread->CreateThread(_T("ProofPolicyOwnThreadUpdate"),
		(vEngine::THREADPROC)m_Trunk.sfp0(&ProofPolicyOwn::ThreadUpdate),
		NULL))
	{
		return FALSE;
	}

	while(!m_pThread->IsThreadActive(_T("ProofPolicyOwnThreadUpdate")))
	{
		continue;
	}

	return TRUE;
}

//-------------------------------------------------------------------------
// 初始化配置
//-------------------------------------------------------------------------
BOOL ProofPolicyOwn::InitConfig()
{
	// 加载配置文件
	CreateObj("ProofPolicyOwn", "VarContainer");
	TObjRef<VarContainer> pVar = "ProofPolicyOwn";

	if( !P_VALID(pVar) ) return FALSE;

	pVar->Load("VirtualFileSys", _T("server_config/login/center_proof.ini"));

	// 读取IP和端口
	TCHAR szIP[X_IP_LEN];
	_tcsncpy(szIP, pVar->GetString(_T("ip server")), MCalTCharArrayNum(szIP) - 1);
	memcpy(m_szIP, m_pUtil->UnicodeToUnicode8(szIP), sizeof(m_szIP) - 1);

	m_nPort = pVar->GetInt(_T("port server"));

	// 删除配置文件对象
	KillObj("ProofPolicyOwn");
	
	return TRUE;
}

//-------------------------------------------------------------------------
// 销毁
//-------------------------------------------------------------------------
VOID ProofPolicyOwn::Destroy()
{

	// 结束线程
	InterlockedExchange((LPLONG)&m_bTerminateUpdate, TRUE);
	m_pThread->WaitForThreadDestroy( _T("ProofPolicyOwnThreadUpdate"), INFINITE);

	InterlockedExchange((LPLONG)&m_bTerminateConnect, TRUE);
	m_pThread->WaitForThreadDestroy( _T("ProofPolicyOwnConnectServer"), INFINITE);

	// 销毁网络
	m_pTransport->Destroy();
	KillObj("StreamTransportProofPolicyOwn");

	// 销毁网络消息管理器
	UnregisterProofMsg();
	KillObj("NetCmdMgrProofPolicyOwn");

	// 删除所有list中的数据
	tagPlayerProofData* pData = m_listProofData.PopFront();

	while( P_VALID(pData) )
	{
		SAFE_DEL(pData);

		pData = m_listProofData.PopFront();
	}
}

//-------------------------------------------------------------------------
// 注册网络消息
//-------------------------------------------------------------------------
VOID ProofPolicyOwn::RegisterProofMsg()
{
	m_pCmdMgr->Register("NS_LOGIN",				(NETMSGPROC)m_Trunk.sfp2(&ProofPolicyOwn::HandleLogin),				_T("login return") );
	m_pCmdMgr->Register("NS_USERLOGIN",			(NETMSGPROC)m_Trunk.sfp2(&ProofPolicyOwn::HandleUserLogin),			_T("accout login return") );
	m_pCmdMgr->Register("NS_USERUPDATEPWD",		(NETMSGPROC)m_Trunk.sfp2(&ProofPolicyOwn::HandleUserUpdatePwd),		_T("accout update password") );
	m_pCmdMgr->Register("NS_USERBINDMIBAO",		(NETMSGPROC)m_Trunk.sfp2(&ProofPolicyOwn::HandleUserBindMibao),		_T("accout bind mibao") );
	m_pCmdMgr->Register("NS_USERUNBINDMIBAO",	(NETMSGPROC)m_Trunk.sfp2(&ProofPolicyOwn::HandleUserUnbindMibao),	_T("accout unbind mibao") );

	m_pCmdMgr->Register("NS_BLOCKACCOUNT",		(NETMSGPROC)m_Trunk.sfp2(&ProofPolicyOwn::HandleBlockAccount),	_T("block accout ") );
	m_pCmdMgr->Register("NS_UNBLOCKACCOUNT",	(NETMSGPROC)m_Trunk.sfp2(&ProofPolicyOwn::HandleUnBlockAccount),	_T("unblock accout ") );
	m_pCmdMgr->Register("NS_USERUPDATECHENMI",	(NETMSGPROC)m_Trunk.sfp2(&ProofPolicyOwn::HandleSetChenMi),			_T("set chenmi") );

}

//-------------------------------------------------------------------------
// 清空网络消息
//-------------------------------------------------------------------------
VOID ProofPolicyOwn::UnregisterProofMsg()
{
	m_pCmdMgr->Destroy();
}

//-------------------------------------------------------------------------
// 更新线程
//-------------------------------------------------------------------------
VOID ProofPolicyOwn::ThreadUpdate()
{
	DWORD dwTime = 0;

#ifdef DEBUG
	_set_se_translator(vEngine::SEH_Translation); 

	try
	{
#endif
		while( !m_bTerminateUpdate )
		{
			// 得到当前时间
			dwTime = timeGetTime();

			// 更新验证列表
			UpdateProofList();

			// 更新消息
			UpdateSession();

			// 等待
			WaitForSingleObject(m_listProofData.GetEvent(), 50);
		}
#ifdef DEBUG
	}
	catch(vEngine::Exception)
	{
		//如果是调试，则抛出异常
		if( TObjRef<Util>()->IsDebuggerPresent() )
		{
			throw;
		}
		else
		{
			exit(1);
		}
	}
#endif

	_endthreadex(0);
}

//-------------------------------------------------------------------------
// 更新验证列表
//-------------------------------------------------------------------------
VOID ProofPolicyOwn::UpdateProofList()
{
	// 从列表中取出验证数据
	tagProofResult result;
	string strPsd;

	tagPlayerProofData* pData = m_listProofData.PopFront();

	while( P_VALID(pData) )
	{
		result.dwClientID = pData->dwClientID;
		BOOL bRet = m_ProofDB.QueryAccount(pData->strAccoutName.c_str(), strPsd, &result);

		// 重置返回码
		BOOL bNeedCenterProof = FALSE;

		INT nRet = E_Success;
		if( bRet )
		{
			// 比较密码是不是一致
			if( strPsd != pData->strPsd )
			{
				nRet = E_ProofResult_Account_No_Match;
				bNeedCenterProof = TRUE;
			}
			else if( EPLS_OffLine != result.eLoginStatus )
			{
				nRet = E_ProofResult_Account_In_Use;
			}
			else if( result.dwFrobidMask != 0  )
			{
				do{
					if(result.dwFrobidMask & EPLM_MemberCenter)
					{
						nRet = E_ProofResult_Forbid_MemberCenter;						
						break;
					}
					if(result.dwFrobidMask & EPLM_GMTool)
					{
						//这种情况最后调用回调函数时需要world_forbid表里找一下
						nRet = E_ProofResult_Forbid_GMTool;						
						break;
					}
					if(result.dwFrobidMask & EPLM_CellPhone)
					{
						nRet = E_ProofResult_Forbid_CellPhone;						
						break;
					}
					if(result.dwFrobidMask & EPLM_MiBao)
					{
						nRet = E_ProofResult_Forbid_MiBao;						
						break;
					}
					if(result.dwFrobidMask & EPLM_WaiGua)
					{
						nRet = E_ProofResult_Disabled;						
						break;
					}
				}while(0);
			}
		}
		else
		{
			nRet = E_ProofResult_Account_No_Match;
			bNeedCenterProof = TRUE;
		}

		// 如果需要中心激活但现在没有连接
		if( bNeedCenterProof && !m_bConnected )
		{
			nRet = E_SelectWorld_Server_Maintenance;
			bNeedCenterProof = FALSE;
		}

		if( bNeedCenterProof )
		{
			// 发往中心激活
			tagNC_USERLOGIN send;

			send.dwLoginID	=	g_login.GetSectionID();
			send.dwClientID	=	pData->dwClientID;
			strncpy(send.szGUID, pData->strGUID.c_str(), X_STRING_LEN);
			strncpy(send.szAccountName, pData->strAccoutName.c_str(), X_STRING_LEN);
			strncpy(send.szPsd, pData->strPsd.c_str(), X_STRING_LEN);

			m_pTransport->Send(&send, send.dwSize);
		}
		else
		{
			// 调用回调函数
			(*m_fnCallback)(nRet, &result);
		}
		
		SAFE_DEL(pData);
		pData = m_listProofData.PopFront();		// 取出下一个
	}
}

//-------------------------------------------------------------------------
// 更新消息
//-------------------------------------------------------------------------
VOID ProofPolicyOwn::UpdateSession()
{
	if( NULL == m_pTransport )
		return;

	// 检测断连
	if(!m_pTransport->IsConnected() && !m_pThread->IsThreadActive(_T("ProofPolicyOwnConnectServer")))
	{
		InterlockedExchange((LONG*)&m_bTerminateConnect, TRUE);
		InterlockedExchange((LONG*)&m_bConnected, FALSE);
		m_pTransport->Disconnect();

		m_pThread->WaitForThreadDestroy(_T("ProofPolicyOwnConnectServer"), INFINITE);

		// 重新启动数据库服务器连接线程
		InterlockedExchange((LONG*)&m_bTerminateConnect, FALSE);
		m_pThread->CreateThread(_T("ProofPolicyOwnConnectServer"), (vEngine::THREADPROC)m_Trunk.sfp0(&ProofPolicyOwn::ThreadConnectServer), NULL);

		while(FALSE == m_pThread->IsThreadActive(_T("ProofPolicyOwnConnectServer")))
		{
			m_pThread->CreateThread(_T("ProofPolicyOwnConnectServer"), (vEngine::THREADPROC)m_Trunk.sfp0(&ProofPolicyOwn::ThreadConnectServer), NULL);
			continue;
		}

		return;
	}

	// 处理来自中心激活服务器的消息
	DWORD	dwSize = 0;
	LPBYTE	pRecv = m_pTransport->Recv(dwSize);

	while( P_VALID(pRecv) )
	{
		// 处理消息
		m_pCmdMgr->HandleCmd((tagNetCmd*)pRecv, dwSize, GT_INVALID);
		m_pTransport->FreeRecved(pRecv);

		// 收取下一条消息
		pRecv = m_pTransport->Recv(dwSize);
	}
}

//-------------------------------------------------------------------------
// 连接线程
//-------------------------------------------------------------------------
VOID ProofPolicyOwn::ThreadConnectServer()
{
#ifdef _DEBUG
	EXCEPTION_PROTECT_START;
#endif

	while( FALSE == m_bTerminateConnect )
	{
		if( !m_pTransport->IsConnected() )
		{
			if( !m_pTransport->IsTryingToConnect() )
			{
				m_pTransport->TryToConnect(m_szIP, m_nPort);
			}

			Sleep(100);
			continue;	// 重新检测连接
		}

		IMSG(_T("Contected to center server at %s: %d\r\n"), m_pUtil->Unicode8ToUnicode(m_szIP), m_nPort);

		// 发送初始消息
		tagNC_LOGIN	send;
		send.dwLoginID = g_login.GetSectionID();
		m_pTransport->Send(&send, send.dwSize);

		break;
	}

#ifdef _DEBUG
	EXCEPTION_PROTECT_END;
#endif
	_endthreadex(0);
}

//-------------------------------------------------------------------------
// 处理登陆
//-------------------------------------------------------------------------
DWORD ProofPolicyOwn::HandleLogin(tagNetCmd* pCmd, DWORD)
{
	tagNS_LOGIN* pRecv = (tagNS_LOGIN*)pCmd;

	if( 0 != pRecv->byResult )
	{
		ASSERT(0);
	}

	InterlockedExchange((LONG*)&m_bConnected, TRUE);	// 设置连接

	return 0;
}

//-------------------------------------------------------------------------
// 处理玩家登陆
//-------------------------------------------------------------------------
DWORD ProofPolicyOwn::HandleUserLogin(tagNetCmd* pCmd, DWORD)
{
	tagNS_USERLOGIN* pRecv = (tagNS_USERLOGIN*)pCmd;

	tagProofResult result;
	result.dwClientID = pRecv->dwClientID;

	// 查看返回码
	INT nRet = E_Success;

	// 是否操作数据库
	if( 2 == pRecv->byResult )	// 帐号不存在
	{
		nRet = E_ProofResult_Account_No_Match;
	}
	else if( 1 == pRecv->byResult )	// 密码错误
	{
		nRet = E_ProofResult_Account_No_Match;
	
		// 只尝试更新数据库，而不插入，因为如果玩家是第一次登陆，此时中央激活并没有将玩家账号放入激活列表，可能会导致划拨时没有该区记录
		m_ProofDB.UpdateAccout(pRecv->szAccountName, pRecv->szPsd, pRecv->szMibao, pRecv->byGuard);
	}
	else if( 0 == pRecv->byResult )	// 正确
	{
		// 尝试插入，如果插入失败则尝试更新
		BOOL bRet = m_ProofDB.InsertAccout(pRecv->szAccountName, pRecv->szPsd, pRecv->szMibao, pRecv->byGuard);
		if( !bRet )
		{
			bRet = m_ProofDB.UpdateAccout(pRecv->szAccountName, pRecv->szPsd, pRecv->szMibao, pRecv->byGuard);
		}
	}
	else
	{
		nRet = E_ProofResult_Account_No_Match;
	}

	// 是否再需要重新查询
	if( E_ProofResult_Account_No_Match != nRet )
	{
		string strPsd;
		BOOL bRet = m_ProofDB.QueryAccount(pRecv->szAccountName, strPsd, &result);

		if( bRet )
		{
			if( EPLS_OffLine != result.eLoginStatus )
			{
				nRet = E_ProofResult_Account_In_Use;
			}
			else if( pRecv->byState == 1 )
			{
				//表示上来就被封停了~
				sBeton.ForbidAccount(result.dwAccountID,pRecv->byBlocktype);
				//重新得到result
				m_ProofDB.QueryAccount(pRecv->szAccountName, strPsd, &result);
			}
			//如果封停掩码不为零 且 当前登录状态并未改变成E_ProofResult_Account_In_Use
			if( (result.dwFrobidMask != 0)  && (nRet != E_ProofResult_Account_In_Use) )
			{
				do{
					if(result.dwFrobidMask & EPLM_MemberCenter)
					{
						nRet = E_ProofResult_Forbid_MemberCenter;						
						break;
					}
					if(result.dwFrobidMask & EPLM_GMTool)
					{
						//这种情况最后调用回调函数时需要world_forbid表里找一下
						nRet = E_ProofResult_Forbid_GMTool;						
						break;
					}
					if(result.dwFrobidMask & EPLM_CellPhone)
					{
						nRet = E_ProofResult_Forbid_CellPhone;						
						break;
					}
					if(result.dwFrobidMask & EPLM_MiBao)
					{
						nRet = E_ProofResult_Forbid_MiBao;						
						break;
					}
					if(result.dwFrobidMask & EPLM_WaiGua)
					{
						nRet = E_ProofResult_Disabled;						
						break;
					}
				}while(0);
			}
		}
		else
		{
			nRet = E_ProofResult_Account_No_Match;
		}
	}

	// 调用回调函数
	(*m_fnCallback)(nRet, &result);



	return 0;
}

//-------------------------------------------------------------------------
// 处理玩家更新密码
//-------------------------------------------------------------------------
DWORD ProofPolicyOwn::HandleUserUpdatePwd(tagNetCmd* pCmd, DWORD)
{
	tagNS_USERUPDATEPWD* pRecv = (tagNS_USERUPDATEPWD*)pCmd;

	// 尝试更新数据库
	BOOL bRet = m_ProofDB.UpdatePsd(pRecv->szAccountName, pRecv->szPsd);

	// 发送返回消息
	tagNC_USERUPDATEPWD send;
	send.dwLoginID = g_login.GetSectionID();
	send.byResult = (bRet ? 0 : 1);
	memcpy(send.szGUID, pRecv->szGUID, X_STRING_LEN);
	memcpy(send.szAccountName, pRecv->szAccountName, X_STRING_LEN);

	m_pTransport->Send(&send, send.dwSize);

	return 0;
}

//-------------------------------------------------------------------------
// 处理玩家绑定密保
//-------------------------------------------------------------------------
DWORD ProofPolicyOwn::HandleUserBindMibao(tagNetCmd* pCmd, DWORD)
{
	tagNS_USERBINDMIBAO* pRecv = (tagNS_USERBINDMIBAO*)pCmd;

	// 尝试更新数据库
	BOOL bRet = m_ProofDB.UpdateMibao(pRecv->szAccountName, pRecv->szMibao);

	// 发送返回消息
	tagNC_USERBINDMIBAO send;
	send.dwLoginID = g_login.GetSectionID();
	send.byResult = (bRet ? 0 : 1);
	memcpy(send.szGUID, pRecv->szGUID, X_STRING_LEN);
	memcpy(send.szAccountName, pRecv->szAccountName, X_STRING_LEN);

	m_pTransport->Send(&send, send.dwSize);


	return 0;
}

//-------------------------------------------------------------------------
// 玩家取消绑定密保
//-------------------------------------------------------------------------
DWORD ProofPolicyOwn::HandleUserUnbindMibao(tagNetCmd* pCmd, DWORD)
{
	tagNS_USERUNBINDMIBAO* pRecv = (tagNS_USERUNBINDMIBAO*)pCmd;

	// 尝试更新数据库
	BOOL bRet = m_ProofDB.UpdateMibao(pRecv->szAccountName, pRecv->szMibao);

	// 发送返回消息
	tagNC_USERUNBINDMIBAO send;
	send.dwLoginID = g_login.GetSectionID();
	send.byResult = (bRet ? 0 : 1);
	memcpy(send.szGUID, pRecv->szGUID, X_STRING_LEN);
	memcpy(send.szAccountName, pRecv->szAccountName, X_STRING_LEN);

	m_pTransport->Send(&send, send.dwSize);

	return 0;
}

//-------------------------------------------------------------------------
// 处理账号封停
//-------------------------------------------------------------------------
DWORD ProofPolicyOwn::HandleBlockAccount(tagNetCmd* pCmd, DWORD)
{
	tagNS_BLOCKACCOUNT* pRecv = (tagNS_BLOCKACCOUNT*)pCmd;

	// 从列表中取出验证数据
	tagProofResult result;
	string strPsd;
	BOOL bRet = m_ProofDB.QueryAccount(pRecv->szAccountName, strPsd, &result);

	if(bRet)
	{
		DWORD dwRet = sBeton.ForbidAccount(result.dwAccountID,pRecv->byBlockType);
		bRet = ((dwRet==E_RT_Success) ? 1 : 0);
	}

	tagNC_BLOCKACCOUNT send;
	send.dwLoginID = g_login.GetSectionID();
	send.byResult = (bRet ? 0 : 1);
	memcpy(send.szGUID, pRecv->szGUID, X_STRING_LEN);
	memcpy(send.szAccountName, pRecv->szAccountName, X_STRING_LEN);
	m_pTransport->Send(&send, send.dwSize);


	return 0;
}
//-------------------------------------------------------------------------
// 处理账号解封
//-------------------------------------------------------------------------
DWORD ProofPolicyOwn::HandleUnBlockAccount(tagNetCmd* pCmd, DWORD)
{
	tagNS_UNBLOCKACCOUNT* pRecv = (tagNS_UNBLOCKACCOUNT *)pCmd;

	// 从列表中取出验证数据
	tagProofResult result;
	string strPsd;
	BOOL bRet = m_ProofDB.QueryAccount(pRecv->szAccountName, strPsd, &result);

	if(bRet)
	{
		DWORD dwRet = sBeton.RemoveAccountForbid(result.dwAccountID,pRecv->byUnBlockType);
		bRet = ((dwRet==E_RT_Success) ? 1 : 0);
	}

	tagNC_UNBLOCKACCOUNT send;
	send.dwLoginID = g_login.GetSectionID();
	send.byResult = (bRet ? 0 : 1);
	memcpy(send.szGUID, pRecv->szGUID, X_STRING_LEN);
	memcpy(send.szAccountName, pRecv->szAccountName, X_STRING_LEN);
	m_pTransport->Send(&send, send.dwSize);


	return 0;
}

//-------------------------------------------------------------------------
// 设置防沉迷
//-------------------------------------------------------------------------
DWORD ProofPolicyOwn::HandleSetChenMi(tagNetCmd* pCmd, DWORD)
{
	tagNS_USERUPDATECHENMI* pRecv = (tagNS_USERUPDATECHENMI*)pCmd;

	// 从列表中取出验证数据
	tagProofResult result;
	string strPsd;
	BOOL bRet = m_ProofDB.QueryAccount(pRecv->szAccountName, strPsd, &result);

	if(bRet)
	{
		DWORD dwRet = sBeton.SetAccountChenMi(result.dwAccountID,pRecv->byChenMi);
		bRet = ((dwRet==E_RT_Success) ? 1 : 0);
	}

	tagNC_USERUPDATECHENMI send;
	send.dwLoginID = g_login.GetSectionID();
	send.byResult = (bRet ? 0 : 1);
	memcpy(send.szGUID, pRecv->szGUID, X_STRING_LEN);
	memcpy(send.szAccountName, pRecv->szAccountName, X_STRING_LEN);
	m_pTransport->Send(&send, send.dwSize);


	return 0;
}


//-------------------------------------------------------------------------
// 验证接口
//-------------------------------------------------------------------------
VOID ProofPolicyOwn::Proof(DWORD dwClientID, LPCSTR szAccoutName, LPCSTR szPsd, LPCSTR szGUILD, DWORD dwIP)
{
	if( !P_VALID(dwClientID) || !P_VALID(szAccoutName) ) return;

	// 生成一个新的验证信息
	tagPlayerProofData* pData = new tagPlayerProofData(dwClientID, szAccoutName, szPsd, szGUILD);

	m_listProofData.PushBack(pData);
}

//-------------------------------------------------------------------------
// 验证服务器连接状态
//-------------------------------------------------------------------------
INT ProofPolicyOwn::GetProofServerStatus()
{
	if (P_VALID(m_pTransport) && m_pTransport->IsConnected())
	{
		return EWS_Well;
	}

	return EWS_ProofError;
}