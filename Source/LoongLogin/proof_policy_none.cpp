//-----------------------------------------------------------------------------
//!\file proof_policy_none.cpp
//!\author Aslan
//!
//!\date 2008-11-30
//! last 2008-11-30
//!
//!\brief 内部测试用类
//!
//!	Copyright (c) 2004 TENGWU Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#include "stdafx.h"

#include "proof_policy_none.h"
#include "player_mgr.h"

//-----------------------------------------------------------------------------
// DB初始化
//-----------------------------------------------------------------------------
BOOL ProofPocilyNoneDB::Init()
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

//-----------------------------------------------------------------------------
// DB查询玩家
//-----------------------------------------------------------------------------
BOOL ProofPocilyNoneDB::QueryAccount(LPCSTR szAccountName,LPCSTR szPsd, tagProofResult* pProofResult)
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

	pStream->SetSelect("account", "id,forbid_mask,privilege,guard,login_status,worldname_crc,mibao,ip,time");
	pStream->SetWhere();
	pStream->FillString("name='").FillString(szAccountName, pCon).FillString("'");
	pStream->FillString(" AND psd='").FillString(szPsd, pCon).FillString("'"); //modify by songg

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

	// 密保
	CHAR szMibao[MIBAO_LEN] = {'\0'};
	(*pResult)[6].GetBlob(szMibao, MIBAO_LEN);
	if( '\0' == szMibao[0] )
	{
		pProofResult->bNeedMibao		=	false;
	}
	else
	{
		pProofResult->bNeedMibao		=	true;
	}

	//上次登录的时间和ip
	memcpy(szIP,(*pResult)[7].GetString(),(*pResult)[7].GetLen());
	memcpy(szDateTime,(*pResult)[8].GetString(),(*pResult)[8].GetLen());

	pProofResult->dwPreLoginIP = pTrans->StringIP2IP(szIP);
	DataTime2DwordTime(pProofResult->dwPreLoginTime,szDateTime,(*pResult)[8].GetLen());

	// 归还结果集
	m_DB.FreeQueryResult(pResult);

	return TRUE;
}

//-----------------------------------------------------------------------------
// 初始化
//-----------------------------------------------------------------------------
BOOL ProofPolicyNone::Init(PROOFCALLBACK fn)
{
	m_pThread = "Thread";

	SetProofCallback(fn);		// 设置验证回调函数

	// 初始化数据库
	if( FALSE == m_ProofDB.Init() )
	{
		return FALSE;
	}

	// 初始化线程
	m_bTerminateUpdate = FALSE;

	if( FALSE == m_pThread->CreateThread( _T("thread_update_proof_none"), 
		(THREADPROC)m_Trunk.sfp1(&ProofPolicyNone::ThreadUpdate), NULL) )
		return FALSE;

	return TRUE;
}

//------------------------------------------------------------------------------
// 销毁
//------------------------------------------------------------------------------
VOID ProofPolicyNone::Destroy()
{
	// 结束线程
	InterlockedExchange((LPLONG)&m_bTerminateUpdate, TRUE);
	m_pThread->WaitForThreadDestroy( _T("thread_update_proof_none"), INFINITE);

	// 删除所有list中的数据
	tagPlayerProofData* pData = m_listProofData.PopFront();

	while( P_VALID(pData) )
	{
		SAFE_DEL(pData);

		pData = m_listProofData.PopFront();
	}
}

//------------------------------------------------------------------------------
// 更新线程
//------------------------------------------------------------------------------
VOID ProofPolicyNone::ThreadUpdate()
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

			// 从列表中取出验证数据
			tagProofResult result;

			tagPlayerProofData* pData = m_listProofData.PopFront();

			while( P_VALID(pData) )
			{
				result.dwClientID = pData->dwClientID;

				BOOL bRet = m_ProofDB.QueryAccount(pData->strAccoutName.c_str(),pData->strPsd.c_str(), &result);

				// 重置返回码
				INT nRet = E_Success;
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

				// 调用回调函数
				(*m_fnCallback)(nRet, &result);

				pData = m_listProofData.PopFront();		// 取出下一个
			}

			WaitForSingleObject(m_listProofData.GetEvent(), 500);
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

//---------------------------------------------------------------------------------
// 验证接口
//---------------------------------------------------------------------------------
VOID ProofPolicyNone::Proof(DWORD dwClientID, LPCSTR szAccoutName, LPCSTR szPsd, LPCSTR szGUID, DWORD dwIP)
{
	if( !P_VALID(dwClientID) || !P_VALID(szAccoutName) ) return;

	// 生成一个新的验证信息
	tagPlayerProofData* pData = new tagPlayerProofData(dwClientID, szAccoutName,szPsd);

	m_listProofData.PushBack(pData);
}

//-------------------------------------------------------------------------
// 验证服务器连接状态
//-------------------------------------------------------------------------
INT ProofPolicyNone::GetProofServerStatus()
{
	return EWS_Well;
}