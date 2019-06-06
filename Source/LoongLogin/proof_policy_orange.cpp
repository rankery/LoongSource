#include "StdAfx.h"
#include "proof_policy_orange.h"
#include "login_server.h"
#include "../ServerDefine/base_define.h"

//-------------------------------------------------------------------------
// 初始化
//-------------------------------------------------------------------------
BOOL ProofPolicyOrangeDB::Init()
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
BOOL ProofPolicyOrangeDB::QueryAccount(DWORD dwAccountID, string& strAccoutName,string& strIP)
{
	MyStream* pStream = m_DB.GetStream();
	if( !P_VALID(pStream) ) return FALSE;

	TObjRef<StreamTransport> pTrans;
	pTrans = "StreamTransport";

	pStream->SetSelect("account", "name,ip");
	pStream->SetWhere();
	pStream->FillString("id=")<<dwAccountID;

	QueryResult* pResult = m_DB.Query(pStream);

	// 释放流
	m_DB.ReturnStream(pStream);

	// 查询结果为空
	if( !P_VALID(pResult)|| pResult->GetRowCount() <= 0 ) return FALSE;

	// 记录密码
	strAccoutName = (*pResult)[0].GetString();
	// 记录ip
	strIP =	(*pResult)[1].GetString();

	return TRUE;

}
//-------------------------------------------------------------------------
// 查询
//-------------------------------------------------------------------------
BOOL ProofPolicyOrangeDB::QueryAccount(LPCSTR szAccountName, string& strPsd, tagProofResult* pProofResult)
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
	if( !P_VALID(pResult)|| pResult->GetRowCount() <= 0 ) return FALSE;

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
BOOL ProofPolicyOrangeDB::InsertAccout(LPCSTR szAccountName, LPCSTR szPsd, LPCSTR szMiBao, BOOL bGuard)
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
BOOL ProofPolicyOrangeDB::UpdateAccout(LPCSTR szAccountName, LPCSTR szPsd, LPCSTR szMiBao, BOOL bGuard)
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

/*
/	orange验证策略
/
*/
//-------------------------------------------------------------------------
// 初始化
//-------------------------------------------------------------------------
BOOL ProofPolicyOrange::Init(PROOFCALLBACK fn)
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
	

	// 初始化网络

	//登入
	CreateObj("StreamTransPPOrangeLogin", "StreamTransport");
	m_pTransLogin = "StreamTransPPOrangeLogin";
	if( !P_VALID(m_pTransLogin) ) return FALSE;

	//登出
	CreateObj("StreamTransPPOrangeLogout", "StreamTransport");
	m_pTransLogout = "StreamTransPPOrangeLogout";
	if( !P_VALID(m_pTransLogout) ) return FALSE;


	m_pTransLogin->Init(TRUE,ESTM_Stream); //
	// 启动连接线程
	if(!m_pThread->CreateThread(_T("PPOrangeConnectLoginServer"), 
		(vEngine::THREADPROC)m_Trunk.sfp0(&ProofPolicyOrange::ThreadConnectLoginServer), 
		NULL))
	{
		return FALSE;
	}

	while(!m_pThread->IsThreadActive(_T("PPOrangeConnectLoginServer")))
	{
		continue;
	}

	// 启动更新线程
	if(!m_pThread->CreateThread(_T("PPOrangeLoginThreadUpdate"),
		(vEngine::THREADPROC)m_Trunk.sfp0(&ProofPolicyOrange::ThreadLoginServerUpdate),
		NULL))
	{
		return FALSE;
	}

	while(!m_pThread->IsThreadActive(_T("PPOrangeLoginThreadUpdate")))
	{
		continue;
	}


	m_pTransLogout->Init(TRUE,ESTM_Stream); //
	// 启动连接线程
	if(!m_pThread->CreateThread(_T("PPOrangeConnectLogoutServer"), 
		(vEngine::THREADPROC)m_Trunk.sfp0(&ProofPolicyOrange::ThreadConnectLogoutServer), 
		NULL))
	{
		return FALSE;
	}

	while(!m_pThread->IsThreadActive(_T("PPOrangeConnectLogoutServer")))
	{
		continue;
	}

	// 启动更新线程
	if(!m_pThread->CreateThread(_T("PPOrangeLogoutThreadUpdate"),
		(vEngine::THREADPROC)m_Trunk.sfp0(&ProofPolicyOrange::ThreadLogoutServerUpdate),
		NULL))
	{
		return FALSE;
	}

	while(!m_pThread->IsThreadActive(_T("PPOrangeLogoutThreadUpdate")))
	{
		continue;
	}
	
	return TRUE;
}


//-------------------------------------------------------------------------
// 销毁
//-------------------------------------------------------------------------
VOID ProofPolicyOrange::Destroy()
{

	// 结束线程
	InterlockedExchange((LPLONG)&m_bTerminateLoginUpdate, TRUE);
	m_pThread->WaitForThreadDestroy( _T("PPOrangeLoginThreadUpdate"), INFINITE);

	InterlockedExchange((LPLONG)&m_bTerminateLoginConnect, TRUE);
	m_pThread->WaitForThreadDestroy( _T("PPOrangeConnectLoginServer"), INFINITE);

	//登出
	InterlockedExchange((LPLONG)&m_bTerminateLogoutUpdate, TRUE);
	m_pThread->WaitForThreadDestroy( _T("PPOrangeLogoutThreadUpdate"), INFINITE);

	InterlockedExchange((LPLONG)&m_bTerminateLogoutConnect, TRUE);
	m_pThread->WaitForThreadDestroy( _T("PPOrangeConnectLogoutServer"), INFINITE);


	// 销毁网络
	m_pTransLogin->Destroy();
	KillObj("StreamTransPPOrangeLogin");

	m_pTransLogout->Destroy();
	KillObj("StreamTransPPOrangeLogout");

	// 删除所有list中的数据
	tagPlayerProofData* pData = m_listProofData.PopFront();

	while( P_VALID(pData) )
	{
		SAFE_DEL(pData);

		pData = m_listProofData.PopFront();
	}

}
//-------------------------------------------------------------------------
// 连接对方login服务器线程
//-------------------------------------------------------------------------
VOID ProofPolicyOrange::ThreadConnectLoginServer()
{
#ifdef _DEBUG
	EXCEPTION_PROTECT_START;
#endif

	while( FALSE == m_bTerminateLoginConnect )
	{
		if( !m_pTransLogin->IsConnected() )
		{
			if( !m_pTransLogin->IsTryingToConnect() )
			{
				m_pTransLogin->TryToConnect(m_szLoginIP, m_nLoginPort);
			}

			Sleep(100);
			continue;	// 重新检测连接
		}

		IMSG(_T("Contected to center login server at %s: %d\r\n"), m_pUtil->Unicode8ToUnicode(m_szLoginIP), m_nLoginPort);

		break;
	}

#ifdef _DEBUG
	EXCEPTION_PROTECT_END;
#endif
	_endthreadex(0);
}

//-------------------------------------------------------------------------
// 连接对方logout服务器线程
//-------------------------------------------------------------------------
VOID ProofPolicyOrange::ThreadConnectLogoutServer()
{
#ifdef _DEBUG
	EXCEPTION_PROTECT_START;
#endif

	while( FALSE == m_bTerminateLogoutConnect )
	{
		if( !m_pTransLogout->IsConnected() )
		{
			if( !m_pTransLogout->IsTryingToConnect() )
			{
				m_pTransLogout->TryToConnect(m_szLogoutIP, m_nLogoutPort);
			}

			Sleep(100);
			continue;	// 重新检测连接
		}

		IMSG(_T("Contected to center logout server at %s: %d\r\n"), m_pUtil->Unicode8ToUnicode(m_szLogoutIP), m_nLogoutPort);

		break;
	}

#ifdef _DEBUG
	EXCEPTION_PROTECT_END;
#endif
	_endthreadex(0);
}

//-------------------------------------------------------------------------
// 更新logout线程
//-------------------------------------------------------------------------
VOID ProofPolicyOrange::ThreadLogoutServerUpdate()
{
	DWORD dwTime = 0;

#ifdef DEBUG
	_set_se_translator(vEngine::SEH_Translation); 

	try
	{
#endif
		while( !m_bTerminateLogoutUpdate )
		{
			// 得到当前时间
			dwTime = timeGetTime();

			// 更新验证列表
			UpdatePlayerLogouList();

			// 更新消息
			UpdateLogoutSession();

			// 休息
			Sleep(50);
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
// 更新login线程
//-------------------------------------------------------------------------
VOID ProofPolicyOrange::ThreadLoginServerUpdate()
{
	DWORD dwTime = 0;

#ifdef DEBUG
	_set_se_translator(vEngine::SEH_Translation); 

	try
	{
#endif
		while( !m_bTerminateLoginUpdate )
		{
			// 得到当前时间
			dwTime = timeGetTime();

			// 更新验证列表
			UpdateProofList();

			// 更新消息
			UpdateLoginSession();

			// 休息
			Sleep(50);
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
// 初始化配置
//-------------------------------------------------------------------------
BOOL ProofPolicyOrange::InitConfig()
{
	// 加载配置文件
	CreateObj("ProofPolicyOrange", "VarContainer");
	TObjRef<VarContainer> pVar = "ProofPolicyOrange";

	if( !P_VALID(pVar) ) return FALSE;

	pVar->Load("VirtualFileSys", _T("server_config/login/orange_proof.ini"));

	// 读取IP和端口
	//登录的
	TCHAR szIP[X_IP_LEN] = {'\0'};
	_tcsncpy(szIP, pVar->GetString(_T("ip login_server")), MCalTCharArrayNum(szIP) - 1);
	memcpy(m_szLoginIP, m_pUtil->UnicodeToUnicode8(szIP), sizeof(m_szLoginIP) - 1);

	m_nLoginPort = pVar->GetInt(_T("port login_server"));

	//登出的
	_tcsncpy(szIP, pVar->GetString(_T("ip logout_server")), MCalTCharArrayNum(szIP) - 1);
	memcpy(m_szLogoutIP, m_pUtil->UnicodeToUnicode8(szIP), sizeof(m_szLogoutIP) - 1);

	m_nLogoutPort = pVar->GetInt(_T("port logout_server"));

	//Server Code
	TCHAR szServiceCode[X_OG_SC_LEN] = {'\0'};
	_tcsncpy(szServiceCode, pVar->GetString(_T("servercode orange")), MCalTCharArrayNum(szServiceCode) - 1);
	memcpy(m_szServiceCode, m_pUtil->UnicodeToUnicode8(szServiceCode), sizeof(m_szServiceCode));

	//Region
	TCHAR szRegion[X_OG_REGION_LEN] = {'\0'};
	_tcsncpy(szRegion, pVar->GetString(_T("region orange")), MCalTCharArrayNum(szRegion) - 1);
	memcpy(m_szRegion, m_pUtil->UnicodeToUnicode8(szRegion), sizeof(m_szRegion));

	// 删除配置文件对象
	KillObj("ProofPolicyOrange");

	return TRUE;
}
//-------------------------------------------------------------------------
// 更新玩家登出列表
//-------------------------------------------------------------------------
VOID ProofPolicyOrange::UpdatePlayerLogouList()
{
	if(!m_pTransLogin->IsConnected())
		return;

#ifdef _DEBUG
	EXCEPTION_PROTECT_START;
#endif

	int iNum = 0;
	DWORD dwAccountID = m_listPlayerLogout.PopFront();
	while( P_VALID(dwAccountID) && iNum<50)
	{

		++iNum;

		string strAccountName;
		string strIP;
		BOOL bRet = m_ProofDB.QueryAccount(dwAccountID,strAccountName,strIP);
		if(bRet == TRUE)
		{
			//Send To: 610108'TF'Username'R'''61.62.183.73'11731016;Username'
			//拼验证数据
			stringstream strLogoutFrame;
			strLogoutFrame<<m_szServiceCode;
			strLogoutFrame<<X_OG_SEND_DIVIDE_SIGN;
			strLogoutFrame<<m_szRegion;
			strLogoutFrame<<X_OG_SEND_DIVIDE_SIGN;
			strLogoutFrame<<strAccountName;
			strLogoutFrame<<X_OG_SEND_DIVIDE_SIGN;
			strLogoutFrame<<"R";
			strLogoutFrame<<X_OG_SEND_DIVIDE_SIGN;
			strLogoutFrame<<""; //
			strLogoutFrame<<X_OG_SEND_DIVIDE_SIGN;
			strLogoutFrame<<""; //
			strLogoutFrame<<X_OG_SEND_DIVIDE_SIGN;
			strLogoutFrame<<strIP;// 登录ip
			strLogoutFrame<<X_OG_SEND_DIVIDE_SIGN;
			strLogoutFrame<<"";//
			strLogoutFrame<<X_OG_SEND_DIVIDE_SIGN;
			strLogoutFrame<<"\r\n";

			//发送数据
			m_pTransLogout->Send((void *)strLogoutFrame.str().c_str(),strLogoutFrame.str().length());
			
		}
		dwAccountID = m_listPlayerLogout.PopFront();		// 取出下一个
	}

#ifdef _DEBUG
	EXCEPTION_PROTECT_END;
#endif


}
//-------------------------------------------------------------------------
// 更新验证列表
//-------------------------------------------------------------------------
VOID ProofPolicyOrange::UpdateProofList()
{
	if(!m_pTransLogin->IsConnected())
		return;

#ifdef _DEBUG
	EXCEPTION_PROTECT_START;
#endif


	// 从列表中取出验证数据
	tagProofResult result;

	tagPlayerProofData* pData = m_listProofData.PopFront();
	int iNum = 0;

	//一次处理50个
	while( P_VALID(pData) && iNum<50)
	{

		++iNum;

		//////////////////////////////////////////////////////////////////////////
		//以下数据都走ansi

		TObjRef<StreamTransport> pTrans;
		pTrans = "StreamTransport";

		//psd解密
		string strAccoutName;
		string strEncryptPsd; //已加密的密码
		vector <string> vecOrangeData;
		string strAccountData = (CHAR *)pData->byAccountData;
		//解析数据
		Proof_GetOrangeData(strAccountData,vecOrangeData,X_CLIENT_RECEIVE_DIVIDE_SIGN);
		if(vecOrangeData.size() < 2)
			continue;
		strAccoutName = vecOrangeData[0];
		strEncryptPsd = vecOrangeData[1];

		//解密密码
		INT32 nLen = strEncryptPsd.length();
		LPSTR pDecryptPsd = new CHAR[nLen];
		ZeroMemory(pDecryptPsd, nLen);
		IUTIL->Decrypt((void *)strEncryptPsd.c_str(), pDecryptPsd, nLen, 0);

		//Send To: 610108'TF'Username'Password'1'I'0'218.168.85.252'319612568;Username'
		//拼验证数据
		stringstream strLoginFrame;
		strLoginFrame<<m_szServiceCode;
		strLoginFrame<<X_OG_SEND_DIVIDE_SIGN;
		strLoginFrame<<m_szRegion;
		strLoginFrame<<X_OG_SEND_DIVIDE_SIGN;
		strLoginFrame<<strAccoutName;
		strLoginFrame<<X_OG_SEND_DIVIDE_SIGN;
		strLoginFrame<<pDecryptPsd;
		strLoginFrame<<X_OG_SEND_DIVIDE_SIGN;
		strLoginFrame<<"1"; //1:AuthenticationFlag     1、同時執行驗證 (Authentication)
		strLoginFrame<<X_OG_SEND_DIVIDE_SIGN;
		strLoginFrame<<"I"; //I授權 (Authorization) (Default)	P查詢剩餘的點數
		strLoginFrame<<X_OG_SEND_DIVIDE_SIGN;
		strLoginFrame<<"0"; //DuplicateCheck
		strLoginFrame<<X_OG_SEND_DIVIDE_SIGN;
		strLoginFrame<<pTrans->IP2String(pData->dwIP);// 登录ip
		strLoginFrame<<X_OG_SEND_DIVIDE_SIGN;
		strLoginFrame<<pData->dwClientID;// dwClientID
		strLoginFrame<<X_OG_SEND_DIVIDE_SIGN;
		strLoginFrame<<"\r\n";

		//////////////////////////////////////////////////////////////////////////

		//发送数据
		m_pTransLogin->Send((void *)strLoginFrame.str().c_str(),strLoginFrame.str().length());

		//安全删除
		SAFE_DEL(pData);
		SAFE_DEL(pDecryptPsd);

		pData = m_listProofData.PopFront();		// 取出下一个
	}

#ifdef _DEBUG
	EXCEPTION_PROTECT_END;
#endif


}
//-------------------------------------------------------------------------
// 更新Logout消息
//-------------------------------------------------------------------------
VOID ProofPolicyOrange::UpdateLogoutSession()
{
	if( NULL == m_pTransLogout )
		return;

	// 检测断连
	if(!m_pTransLogout->IsConnected() && !m_pThread->IsThreadActive(_T("PPOrangeConnectLogoutServer")))
	{
		InterlockedExchange((LONG*)&m_bTerminateLogoutConnect, TRUE);
		m_pTransLogout->Disconnect();

		m_pThread->WaitForThreadDestroy(_T("PPOrangeConnectLogoutServer"), INFINITE);

		// 重新启动验证服务器连接线程
		InterlockedExchange((LONG*)&m_bTerminateLogoutConnect, FALSE);
		m_pThread->CreateThread(_T("PPOrangeConnectLogoutServer"), (vEngine::THREADPROC)m_Trunk.sfp0(&ProofPolicyOrange::ThreadConnectLogoutServer), NULL);

		while(FALSE == m_pThread->IsThreadActive(_T("PPOrangeConnectLogoutServer")))
		{
			m_pThread->CreateThread(_T("PPOrangeConnectLogoutServer"), (vEngine::THREADPROC)m_Trunk.sfp0(&ProofPolicyOrange::ThreadConnectLogoutServer), NULL);
			continue;
		}

		return;
	}

	// 处理来自Orange服务器的消息
	DWORD	dwSize = 0;
	LPBYTE	pRecv = m_pTransLogout->Recv(dwSize);

	while( P_VALID(pRecv) )
	{	
		//Orange验证的返回信息
		m_pTransLogout->FreeRecved(pRecv);

		pRecv = m_pTransLogout->Recv(dwSize);
	}
}

//-------------------------------------------------------------------------
// 更新Login消息
//-------------------------------------------------------------------------
VOID ProofPolicyOrange::UpdateLoginSession()
{
	if( NULL == m_pTransLogin )
		return;

	// 检测断连
	if(!m_pTransLogin->IsConnected() && !m_pThread->IsThreadActive(_T("PPOrangeConnectLoginServer")))
	{
		InterlockedExchange((LONG*)&m_bTerminateLoginConnect, TRUE);
		m_pTransLogin->Disconnect();

		m_pThread->WaitForThreadDestroy(_T("PPOrangeConnectLoginServer"), INFINITE);

		// 重新启动验证服务器连接线程
		InterlockedExchange((LONG*)&m_bTerminateLoginConnect, FALSE);
		m_pThread->CreateThread(_T("PPOrangeConnectLoginServer"), (vEngine::THREADPROC)m_Trunk.sfp0(&ProofPolicyOrange::ThreadConnectLoginServer), NULL);

		while(FALSE == m_pThread->IsThreadActive(_T("PPOrangeConnectLoginServer")))
		{
			m_pThread->CreateThread(_T("PPOrangeConnectLoginServer"), (vEngine::THREADPROC)m_Trunk.sfp0(&ProofPolicyOrange::ThreadConnectLoginServer), NULL);
			continue;
		}

		return;
	}

	// 处理来自Orange服务器的消息
	DWORD	dwSize = 0;
	LPBYTE	pRecv = m_pTransLogin->Recv(dwSize);

	while( P_VALID(pRecv) )
	{	
		//处理Orange验证返回信息
		HandleOrangeLogin(pRecv);

		//Orange验证的返回信息
		m_pTransLogin->FreeRecved(pRecv);

		pRecv = m_pTransLogin->Recv(dwSize);
	}

}

//-------------------------------------------------------------------------
// 处理Orange验证信息
//-------------------------------------------------------------------------
VOID ProofPolicyOrange::HandleOrangeLogin(LPBYTE pRecv)
{
	//Response		Type	Length	Description
	//ResultCode	String	max 2	1:成功 ,	0:失敗 , -1:系統錯誤
	//Outstring		String			參考下面說明
	//G-ConsoleCode String			对方唯一标示一个用户的code
	//UniqueID		String			開發商輸入的封包ID (此參數為option)

 	tagProofResult result;

	string strAccountName;
	//不存密码
	string strPsd = "***";
	// 查看返回码
	INT nRet = E_Success;	

	vector <string> vecOrangeData;
	//直接转
	string strOrangeRecv = (CHAR*)pRecv;
	//解析数据
	Proof_GetOrangeData(strOrangeRecv,vecOrangeData,X_OG_RECEIVE_DIVIDE_SIGN);
	//返回码数据项错误
	if (vecOrangeData.size() < 4)
	{
		nRet = E_ProofResult_Account_No_Match;
		// 调用回调函数
		(*m_fnCallback)(nRet, &result);
		return;
	}

	//客户id
	result.dwClientID = atoi(vecOrangeData[3].c_str());

	if(0 != memcmp(vecOrangeData[0].c_str(),"1",vecOrangeData[0].size()))
	{
		//帳號被鎖
		if(0 == memcmp(vecOrangeData[1].c_str(),X_FROBID_MEMBER_CENTER,vecOrangeData[1].size()))
		{
			nRet = E_ProofResult_Forbid_MemberCenter;
		}
		else
		{
			nRet = E_ProofResult_Account_No_Match;
		}
	}
	else 
	{
		do 
		{
			strAccountName = vecOrangeData[2];

			//防沉迷
			BOOL bGuard = FALSE;
			CHAR szMibao[MIBAO_LEN] = ""; //没有密保
			//插入数据库
			BOOL bRet = m_ProofDB.InsertAccout(strAccountName.c_str(),strPsd.c_str(), szMibao, bGuard);
			if( !bRet )
			{
				bRet = m_ProofDB.UpdateAccout(strAccountName.c_str(),strPsd.c_str(), szMibao, bGuard);
			}

		} while (0);
	}
	if( E_ProofResult_Account_No_Match != nRet && E_ProofResult_Forbid_MemberCenter != nRet )
	{

		BOOL bRet = m_ProofDB.QueryAccount(strAccountName.c_str(), strPsd, &result);

		if( bRet )
		{
			if( EPLS_OffLine != result.eLoginStatus )
			{
				nRet = E_ProofResult_Account_In_Use;
			}
			else if( result.dwFrobidMask != 0 )
			{
				do{
					if(result.dwFrobidMask & EPLM_GMTool)
					{
						//这种情况最后调用回调函数时需要world_forbid表里找一下
						nRet = E_ProofResult_Forbid_GMTool;						
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
}
//-----------------------------------------------------------------------------------------------
// 去掉回车换行
//-----------------------------------------------------------------------------------------------
BOOL ProofPolicyOrange::CheckString(string& str)
{
	// 以回车换行结束
	INT nFind = 0;
	while( (nFind = str.find('\r', nFind)) != str.npos )
		str = str.substr(0,nFind);

	// 
	nFind = 0;
	while( (nFind = str.find('\n', nFind)) != str.npos )
		str = str.substr(0,nFind);

	// 检查用户名是否为空
	return str.empty();
}
//-----------------------------------------------------------------------------------------------
// 得到各个数据项 单引号分隔
//-----------------------------------------------------------------------------------------------
VOID ProofPolicyOrange::Proof_GetOrangeData(const string &strFrameString,vector <string> &vecOrangeData,char cDivideSign)
{
	int iprepos = 0;
	int icurpos = 0;
	for(;;)
	{
		string strTemp;

		icurpos = strFrameString.find(cDivideSign,iprepos);
		//最后一个
		if(icurpos == -1)
		{
			strTemp = strFrameString.substr(iprepos,strFrameString.length() - iprepos);

			//检查最后一个字符串
			CheckString(strTemp);

			vecOrangeData.push_back(strTemp);
			break;
		}
		strTemp = strFrameString.substr(iprepos,icurpos-iprepos);

		vecOrangeData.push_back(strTemp);
		iprepos = icurpos + 1;
	}
}

//-------------------------------------------------------------------------
// 玩家登出事件
//-------------------------------------------------------------------------
VOID ProofPolicyOrange::SigPlayerLogoutEvent(DWORD dwAccountID)
{
	if(!P_VALID(dwAccountID)) return;
	m_listPlayerLogout.PushBack(dwAccountID);
}

//-------------------------------------------------------------------------
// 验证接口
//-------------------------------------------------------------------------
VOID ProofPolicyOrange::Proof(DWORD dwClientID, LPCSTR szAccoutName, LPCSTR szPsd, LPCSTR szGUILD, DWORD dwIP)
{
	if( !P_VALID(dwClientID) || !P_VALID(szAccoutName) ) return;

	// 生成一个新的验证信息
	tagPlayerProofData* pData = new tagPlayerProofData(dwClientID, szAccoutName, szPsd, szGUILD,dwIP);

	m_listProofData.PushBack(pData);

}

//-------------------------------------------------------------------------
// 验证服务器连接状态
//-------------------------------------------------------------------------
INT ProofPolicyOrange::GetProofServerStatus()
{
	if (P_VALID(m_pTransLogin) && m_pTransLogin->IsConnected())
	{
		return EWS_Well;
	}

	return EWS_ProofError;
}