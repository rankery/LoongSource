#include "StdAfx.h"
#include "proof_policy_xunLei.h"

#include "../ServerDefine/base_define.h"

#include "proof_policy.h"
#include "login_server.h"

#include "proof_policy_dll_api.h" //验证策略动态库api

#include "proof_policy_xunlei_define.h"

//-------------------------------------------------------------------------
// 初始化
//-------------------------------------------------------------------------
BOOL ProofPolicyXunLeiDB::Init()
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
BOOL ProofPolicyXunLeiDB::QueryAccount(LPCSTR szAccountName, string& strPsd, tagProofResult* pProofResult)
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
BOOL ProofPolicyXunLeiDB::InsertAccout(LPCSTR szAccountName, LPCSTR szPsd, LPCSTR szMiBao, BOOL bGuard)
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
BOOL ProofPolicyXunLeiDB::UpdateAccout(LPCSTR szAccountName, LPCSTR szPsd, LPCSTR szMiBao, BOOL bGuard)
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
/	迅雷验证策略
/
*/

//-------------------------------------------------------------------------
// 初始化
//-------------------------------------------------------------------------
BOOL ProofPolicyXunLei::Init(PROOFCALLBACK fn)
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
//songg
	m_hXunLeiDll = LoadLibrary(_T("Login.dll"));
	if( m_hXunLeiDll == NULL )//if the handle is NULL, it is failed.
	{
		ASSERT(0);
		return FALSE;
	}
	else
	{
		InitProofXunleiInterface(m_hXunLeiDll);
	}

	// 初始化网络
	CreateObj("StreamTransportPPXunLei", "StreamTransport");
	m_pTransport = "StreamTransportPPXunLei";
	if( !P_VALID(m_pTransport) ) return FALSE;

	m_pTransport->Init(); //
	m_pTransport->SetPackageSizeEndian(TRUE); //设置收包时的包头size的字节顺序
	// 启动连接线程
	if(!m_pThread->CreateThread(_T("ProofPolicyXunLeiConnectServer"), 
		(vEngine::THREADPROC)m_Trunk.sfp0(&ProofPolicyXunLei::ThreadConnectServer), 
		NULL))
	{
		return FALSE;
	}

	while(!m_pThread->IsThreadActive(_T("ProofPolicyXunLeiConnectServer")))
	{
		continue;
	}

	// 启动更新线程
	if(!m_pThread->CreateThread(_T("ProofPolicyXunLeiThreadUpdate"),
		(vEngine::THREADPROC)m_Trunk.sfp0(&ProofPolicyXunLei::ThreadUpdate),
		NULL))
	{
		return FALSE;
	}

	while(!m_pThread->IsThreadActive(_T("ProofPolicyXunLeiThreadUpdate")))
	{
		continue;
	}

	return TRUE;
}
//-------------------------------------------------------------------------
// 销毁
//-------------------------------------------------------------------------
VOID ProofPolicyXunLei::Destroy()
{

	// 结束线程
	InterlockedExchange((LPLONG)&m_bTerminateUpdate, TRUE);
	m_pThread->WaitForThreadDestroy( _T("ProofPolicyXunLeiThreadUpdate"), INFINITE);

	InterlockedExchange((LPLONG)&m_bTerminateConnect, TRUE);
	m_pThread->WaitForThreadDestroy( _T("ProofPolicyXunLeiConnectServer"), INFINITE);

	// 销毁网络
	m_pTransport->Destroy();
	KillObj("StreamTransportPPXunLei");

	// 删除所有list中的数据
	tagPlayerProofData* pData = m_listProofData.PopFront();

	while( P_VALID(pData) )
	{
		SAFE_DEL(pData);

		pData = m_listProofData.PopFront();
	}


	FreeLibrary(m_hXunLeiDll);
}
//-------------------------------------------------------------------------
// 连接线程
//-------------------------------------------------------------------------
VOID ProofPolicyXunLei::ThreadConnectServer()
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

		break;
	}

#ifdef _DEBUG
	EXCEPTION_PROTECT_END;
#endif
	_endthreadex(0);
}
//-------------------------------------------------------------------------
// 更新线程
//-------------------------------------------------------------------------
VOID ProofPolicyXunLei::ThreadUpdate()
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
BOOL ProofPolicyXunLei::InitConfig()
{
	// 加载配置文件
	CreateObj("ProofPolicyXunLei", "VarContainer");
	TObjRef<VarContainer> pVar = "ProofPolicyXunLei";

	if( !P_VALID(pVar) ) return FALSE;

	pVar->Load("VirtualFileSys", _T("server_config/login/xunlei_proof.ini"));

	// 读取IP和端口
	TCHAR szIP[X_IP_LEN];
	_tcsncpy(szIP, pVar->GetString(_T("ip server")), MCalTCharArrayNum(szIP) - 1);
	memcpy(m_szIP, m_pUtil->UnicodeToUnicode8(szIP), sizeof(m_szIP) - 1);

	m_nPort = pVar->GetInt(_T("port server"));

	// 删除配置文件对象
	KillObj("ProofPolicyXunLei");

	return TRUE;
}
//-------------------------------------------------------------------------
// 更新验证列表
//-------------------------------------------------------------------------
VOID ProofPolicyXunLei::UpdateProofList()
{
	if(!m_pTransport->IsConnected())
		return;

#ifdef _DEBUG
	EXCEPTION_PROTECT_START;
#endif


	// 从列表中取出验证数据
	tagProofResult result;

	tagPlayerProofData* pData = m_listProofData.PopFront();

	//直接发异步的认证信息
	stXunLeiRequireData stXunLeiRequire;
	stXunLeiLoginFrame  stXunLeiRequireFrame;


	//存储stXunLeiRequire加密后的数据
	CHAR *pcEncryptData = new CHAR[XL_ENCRYPT_REQUIRE_DATA];


	int iNum = 0;

	//一次处理50个
	while( P_VALID(pData) && iNum<50)
	{

		++iNum;

		//命令	序列号	账号密码加密包  112byte
		memcpy(stXunLeiRequire.szCommand,XL_CMD_LOGIN,5);
		//这里有可能出问题
		stXunLeiRequire.usSerialNum = pData->dwClientID;
		memcpy(stXunLeiRequire.byAccountData,pData->byAccountData,100);

		//调用迅雷提供的加密算法 加密后数据116字节 songg
		PROOF_XUNLEI_AesEncrypt((char *)&stXunLeiRequire,pcEncryptData,XL_DECRYPT_REQUIRE_DATA);

		//////////////////////////////////////////////////////////////////////////

		//126个byte 帧的大小
		DWORD dwFrameSize = sizeof(stXunLeiRequireFrame.szGameID)\
			+sizeof(stXunLeiRequireFrame.szProtocolVersion)+ XL_ENCRYPT_REQUIRE_DATA;
		//游戏id
		memcpy(stXunLeiRequireFrame.szGameID,XL_GAMEID,5);
		//协议版本 就是五个空格

		//实体数据
		memcpy(stXunLeiRequireFrame.szEncryptData,pcEncryptData,XL_ENCRYPT_REQUIRE_DATA);

		//////////////////////////////////////////////////////////////////////////

		//发送数据
		m_pTransport->Send(&stXunLeiRequireFrame,dwFrameSize);

		//安全删除
		SAFE_DEL(pData);

		pData = m_listProofData.PopFront();		// 取出下一个
	}

	//安全删除
	SAFE_DEL_ARRAY(pcEncryptData);



#ifdef _DEBUG
	EXCEPTION_PROTECT_END;
#endif


}

//-------------------------------------------------------------------------
// 更新消息
//-------------------------------------------------------------------------
VOID ProofPolicyXunLei::UpdateSession()
{

	if( NULL == m_pTransport )
		return;

	// 检测断连
	if(!m_pTransport->IsConnected() && !m_pThread->IsThreadActive(_T("ProofPolicyXunLeiConnectServer")))
	{
		InterlockedExchange((LONG*)&m_bTerminateConnect, TRUE);
		m_pTransport->Disconnect();

		m_pThread->WaitForThreadDestroy(_T("ProofPolicyXunLeiConnectServer"), INFINITE);

		// 重新启动验证服务器连接线程
		InterlockedExchange((LONG*)&m_bTerminateConnect, FALSE);
		m_pThread->CreateThread(_T("ProofPolicyXunLeiConnectServer"), (vEngine::THREADPROC)m_Trunk.sfp0(&ProofPolicyXunLei::ThreadConnectServer), NULL);

		while(FALSE == m_pThread->IsThreadActive(_T("ProofPolicyXunLeiConnectServer")))
		{
			m_pThread->CreateThread(_T("ProofPolicyXunLeiConnectServer"), (vEngine::THREADPROC)m_Trunk.sfp0(&ProofPolicyXunLei::ThreadConnectServer), NULL);
			continue;
		}

		return;
	}

	// 处理来自xunlei服务器的消息
	DWORD	dwSize = 0;
	LPBYTE	pRecv = m_pTransport->Recv(dwSize);

	while( P_VALID(pRecv) )
	{	
		//处理xunlei验证返回信息
		HandleXunLeiLogin(pRecv);

		//xunlei验证的返回信息
		m_pTransport->FreeRecved(pRecv);

		pRecv = m_pTransport->Recv(dwSize);
		

	}

}
//-------------------------------------------------------------------------
// 处理字符串
//-------------------------------------------------------------------------
BOOL ProofPolicyXunLei::DeleteRightSideBlankOfString(string &str)
{
	// 去掉空格
	INT nFind = 0;
	if((nFind = str.find(' ')) != str.npos)
	{
		str = str.substr(0,nFind);
	}

	// 检查用户名是否为空
	return !str.empty();
}
//-------------------------------------------------------------------------
// 处理xunlei验证信息
//-------------------------------------------------------------------------
VOID ProofPolicyXunLei::HandleXunLeiLogin(LPBYTE pRecv)
{
	//直接转
	stXunLeiLoginFrame *pXunLeiRecv = (stXunLeiLoginFrame *)(pRecv);

	tagProofResult result;

	string strPsd = "***";
	// 查看返回码
	INT nRet = E_Success;	

	//验证gameid
	if(0 != memcmp(pXunLeiRecv->szGameID,XL_GAMEID,5))
	{
		ASSERT(0);

		nRet = E_ProofResult_Account_No_Match;
		// 调用回调函数
		(*m_fnCallback)(nRet, &result);
		return;
	}
	//验证协议版本 正常应该5个空格
	if(0 != memcmp(pXunLeiRecv->szProtocolVersion,"     ",5))
	{
		ASSERT(0);

		nRet = E_ProofResult_Account_No_Match;
		// 调用回调函数
		(*m_fnCallback)(nRet, &result);
		return;
	}
	//解密数据
	stXunLeiReturnData stXunleiReturn;
	//调用迅雷提供的解密算法 songg
	PROOF_XUNLEI_AesDecrypt((char *)pXunLeiRecv->szEncryptData,(char *)&stXunleiReturn);

	//判断返回码
	/*
	0000--OK
	0001--帐号密码错
	0002--激活码功能启用，并且此用户不在已激活用户之列
	0003--验证时发生异常
	0004--获取用户ID失败
	0007--用户已被加入黑名单
	*/


	//处理用户名
	string strAccountName(stXunleiReturn.szAccountName,20);
	BOOL bRet = DeleteRightSideBlankOfString(strAccountName);

	//客户id
	result.dwClientID = stXunleiReturn.usSerialNum;

	if(bRet == FALSE)
	{
		nRet = E_ProofResult_Account_No_Match;
		// 调用回调函数
		(*m_fnCallback)(nRet, &result);
		return;
	}

	if(0 != memcmp(stXunleiReturn.szReturnCode,"0000",sizeof(stXunleiReturn.szReturnCode)))
	{
		nRet = E_ProofResult_Account_No_Match;
	}
	else 
	{
/*
		do 
		{

			//根据stXunleiReturn.szAccountName找ClientID
			if(TRUE == m_mapAccountVsClientID.IsExist(strAccountName))
			{
				//客户id
				result.dwClientID = m_mapAccountVsClientID.Peek(strAccountName);
				m_mapAccountVsClientID.Erase(strAccountName);
			}
			else
			{
				nRet = E_ProofResult_Account_No_Match;
				break;
			}
*/

			//防沉迷
			BOOL bGuard = FALSE;
			if(1 == stXunleiReturn.szGuard[0] )
			{
				bGuard = TRUE;
			}
			CHAR szMibao[MIBAO_LEN] = ""; //没有密保
			//插入数据库
			BOOL bRet = m_ProofDB.InsertAccout(strAccountName.c_str(),strPsd.c_str(), szMibao, bGuard);
			if( !bRet )
			{
				bRet = m_ProofDB.UpdateAccout(strAccountName.c_str(),strPsd.c_str(), szMibao, bGuard);
			}

//		} while (0);
	}
	if( E_ProofResult_Account_No_Match != nRet )
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
	
}
//-------------------------------------------------------------------------
// 验证接口
//-------------------------------------------------------------------------
VOID ProofPolicyXunLei::Proof(DWORD dwClientID, LPCSTR szAccoutName, LPCSTR szPsd, LPCSTR szGUILD, DWORD dwIP)
{
	if( !P_VALID(dwClientID) || !P_VALID(szAccoutName) ) return;

	//处理用户名
	string strAccountName(szAccoutName,20);
	DeleteRightSideBlankOfString(strAccountName);

	// 生成一个新的验证信息
	tagPlayerProofData* pData = new tagPlayerProofData(dwClientID, strAccountName.c_str(), szPsd, szGUILD);

	m_listProofData.PushBack(pData);

/*
	if(TRUE == m_mapAccountVsClientID.IsExist(strAccountName))
	{	
		//表示该用户之前没有通过迅雷验证，迅雷不返回AccoutName无法删除 \
		  所以在此删除并重新建立对应关系。
		m_mapAccountVsClientID.Erase(strAccountName);
	}
	m_mapAccountVsClientID.Add(strAccountName,dwClientID);
*/
}

//-------------------------------------------------------------------------
// 验证服务器连接状态
//-------------------------------------------------------------------------
INT ProofPolicyXunLei::GetProofServerStatus()
{
	if (P_VALID(m_pTransport) && m_pTransport->IsConnected())
	{
		return EWS_Well;
	}

	return EWS_ProofError;
}