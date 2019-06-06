//-----------------------------------------------------------------------------
//!\file to_rt_session.cpp
//!\author Lyp
//!
//!\date 2005-04-05
//! last 2005-04-05
//!
//!\brief 量子区域服务器与服务器控制服务的对话层
//!
//!	Copyright (c) 2005 TENGWU Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#include "stdafx.h"

#include "../ServerDefine/msg_rt_s.h"
#include "../ServerDefine/msg_rt_errorcode.h"
#include "rt_session.h"
#include "login_server.h"
#include "loongworld_mgr.h"
#include "loongworld.h"
#include "db.h"
#include "proof_policy.h"

//-----------------------------------------------------------------------------
//! construction
//-----------------------------------------------------------------------------
RTSession::RTSession() : m_Trunk(this)
{
	m_bInitOK					=	FALSE;
	m_bTermConnect				=	FALSE;
	m_nSendInfoTickCountDown	=	SEND_INFO_INTERVAL;
}

//-----------------------------------------------------------------------------
//! destruction
//-----------------------------------------------------------------------------
RTSession::~RTSession()
{
	Destroy();
}

//-----------------------------------------------------------------------------
// init
//-----------------------------------------------------------------------------
BOOL RTSession::Init()
{
	m_pThread		=	"Thread";
	m_pUtil			=	"Util";

	// 初始化成员属性
	TObjRef<VarContainer> pVar = "LoginServerConfig";
	m_strIP				=	pVar->GetString(_T("ip gm_server"));
	m_dwPort			=	pVar->GetDword(_T("port gm_server"));
	m_dwSectionID		=	pVar->GetDword(_T("id section"));
	m_nZoneNum			=	pVar->GetInt(_T("num zone_server"));

	m_dwWorldID			=	new DWORD[m_nZoneNum];
	m_dwWorldNameCRC	=	new DWORD[m_nZoneNum];
	m_strWorldName		=   new tstring[m_nZoneNum];
	for (int n=0; n<m_nZoneNum; n++)
	{
		TCHAR szZone[MAX_PATH] = {_T('\0')};
		_stprintf(szZone, _T("zone%d"), n);

		m_dwWorldID[n]		=	pVar->GetDword(_T("id"), szZone);
		tstring tmpName		=	pVar->GetString(_T("name"), szZone);
		m_dwWorldNameCRC[n]	=	m_pUtil->Crc32(tmpName.c_str());

		m_strWorldName[n] = tmpName;
	}

	// 创建消息管理
	CreateObj("ToRtNetCmdMgr", "NetCmdMgr");
	m_pMsgCmdMgr = "ToRtNetCmdMgr";

	// 创建连接对象，并初始化
	CreateObj("ToRT", "StreamTransport");
	m_pTran = "ToRT";
	if( !P_VALID(m_pTran) )
	{
		ERR(_T("Create ToRT(StreamTransport) obj failed!\r\n"));
		return FALSE;
	}
	m_pTran->Init();

	// 注册所有网络命令
	RegisterAllRTCommand();

	// 启动连接线程
	if(!m_pThread->CreateThread(_T("ConnectRT"), 
		(vEngine::THREADPROC)m_Trunk.sfp0(&RTSession::ThreadConnectRT), NULL))
	{
		return FALSE;
	}

	while( !m_pThread->IsThreadActive(_T("ConnectRT")) )
	{
		continue;
	}
	return TRUE;
}

//-----------------------------------------------------------------------------
// destroy
//-----------------------------------------------------------------------------
VOID RTSession::Destroy()
{
	// 等待所有线程结束
	InterlockedExchange((LONG*)&m_bTermConnect, TRUE);
	m_pThread->WaitForThreadDestroy(_T("ConnectRT"), INFINITE);

	m_pTran->Destroy();
	KillObj("ToRT");

	// 注销消息管理
	m_pMsgCmdMgr->Destroy();
	KillObj("ToRtNetCmdMgr");

	SAFE_DEL_ARRAY(m_dwWorldID);
	SAFE_DEL_ARRAY(m_dwWorldNameCRC);
}



//-----------------------------------------------------------------------------
// update
//-----------------------------------------------------------------------------
VOID RTSession::Update()
{
	UpdateSession();
	SendServerInfo();
}

//-----------------------------------------------------------------------------
// 接收消息
//-----------------------------------------------------------------------------
VOID RTSession::UpdateSession()
{
	if( NULL == m_pTran )
		return;

	if( !m_pTran->IsConnected() && !m_pThread->IsThreadActive(_T("ConnectRT")) )
	{
		InterlockedExchange((LONG*)&m_bTermConnect, TRUE);
		m_pTran->Disconnect();

		m_pThread->WaitForThreadDestroy(_T("ConnectRT"), INFINITE);
		InterlockedExchange((LONG*)&m_bInitOK, FALSE);
		// 重新启动登陆服务器连接线程
		InterlockedExchange((LONG*)&m_bTermConnect, FALSE);
		m_pThread->CreateThread(_T("ConnectRT"), (vEngine::THREADPROC)m_Trunk.sfp0(&RTSession::ThreadConnectRT), NULL);

		while(FALSE == m_pThread->IsThreadActive(_T("ConnectRT")))
		{
			continue;
		}

		return;
	}

	while(TRUE)
	{
		DWORD dwSize = 0;
		LPBYTE pRecv = m_pTran->Recv(dwSize);
		if( !P_VALID(pRecv) )
			break;

		// 处理消息
		m_pMsgCmdMgr->HandleCmd((tagNetCmd*)pRecv, dwSize, GT_INVALID);

		// 回收资源
		m_pTran->FreeRecved(pRecv);


	}	
}

//-----------------------------------------------------------------------------
// 发送服务器情况
//-----------------------------------------------------------------------------
VOID RTSession::SendServerInfo()
{
	if( !m_pTran->IsConnected() || !m_bInitOK ) return;

	if( --m_nSendInfoTickCountDown > 0 )
		return;

	m_nSendInfoTickCountDown = SEND_INFO_INTERVAL;

	tagNSC_WorldInfo send;
	send.eStatus		= EWS_Well;
	send.nOnlineNum		= 0;
	send.nMaxOnlineNum	= 0;
	
	// 取得所有配置World的信息
	for (int n=0; n<m_nZoneNum; n++)
	{
		LoongWorld* pLoongWorld = sLoongWorldMgr.GetLoongWorld(m_dwWorldNameCRC[n]);
		if (!P_VALID(pLoongWorld))
		{
			IMSG(_T("Loong World id<%d> error!\r\n"), m_dwWorldID[n]);
			break;
		}

		if (pLoongWorld->GetStatus() != EWS_Well)
		{
			send.eStatus = EWS_SystemError;
		}
		else
		{
			send.nOnlineNum += pLoongWorld->GetCurrOnlineDisplay();
			send.nMaxOnlineNum += pLoongWorld->GetMaxOnlineNum();
		}
	}

	// 验证服务器连接状态
	ProofPolicy* pPolicy = sPlayerMgr.GetProofPolicyPtr();
	if (P_VALID(pPolicy) && pPolicy->GetProofServerStatus() != EWS_Well)
	{
		send.eStatus = EWS_ProofError;
	}

	m_pTran->Send(&send, send.dwSize);
}

//-----------------------------------------------------------------------------
// 连接线程(连接监控服务器)
//-----------------------------------------------------------------------------
DWORD RTSession::ThreadConnectRT()
{
#ifdef _DEBUG
	EXCEPTION_PROTECT_START;
#endif

	while( FALSE == m_bTermConnect )
	{
		if( !m_pTran->IsConnected() )
		{		
			if( !m_pTran->IsConnected() )
			{
				m_pTran->TryToConnect(m_pUtil->UnicodeToUnicode8(m_strIP.c_str()), m_dwPort);
			}			

			Sleep(100);
			continue;	// 重新检测连接
		}

		IMSG(_T("Contected to RT Server at %s: %d\r\n"), m_strIP.c_str(), m_dwPort);


		tagNSC_LoginService send;
		send.dwSectionID	=	m_dwSectionID;
		send.dwWorldID		=	GT_INVALID;
		send.eType			=	EST_Login;
		m_pTran->Send(&send, send.dwSize);

		break;
	}

#ifdef _DEBUG
	EXCEPTION_PROTECT_END;
#endif
	return 0;
}


//----------------------------------------------------------------------------------------------
// 注册所有的网络命令
//----------------------------------------------------------------------------------------------
VOID RTSession::RegisterAllRTCommand()
{
	m_pMsgCmdMgr->Register("NSS_LoginService",		(NETMSGPROC)m_Trunk.sfp2(&RTSession::HandleServerLogin),	_T("RT Server Certification Reply"));
	m_pMsgCmdMgr->Register("NSS_Close",				(NETMSGPROC)m_Trunk.sfp2(&RTSession::HandleCloseServer),	_T("Close Server"));

	//封停功能
	m_pMsgCmdMgr->Register("NSS_ForbidAccount",		(NETMSGPROC)m_Trunk.sfp2(&RTSession::HandleForbidAccount),	_T("Forbid One Account"));
	m_pMsgCmdMgr->Register("NSS_ForbidIP",			(NETMSGPROC)m_Trunk.sfp2(&RTSession::HandleForbidIP),	_T("Forbid One IP"));


}

//----------------------------------------------------------------------------------------------
// 服务器认证网络消息
//----------------------------------------------------------------------------------------------
DWORD RTSession::HandleServerLogin(tagNetCmd* pMsg, DWORD)
{
	tagNSS_LoginService* pRecv = (tagNSS_LoginService*)pMsg;

	InterlockedExchange((LPLONG)&m_bInitOK, TRUE);
	return 0;
}

//-----------------------------------------------------------------------------------------------
// 服务器关闭网络消息
//-----------------------------------------------------------------------------------------------
DWORD RTSession::HandleCloseServer(tagNetCmd* pMsg, DWORD)
{
	g_login.Shutdown();
	return 0;
}
//-------------------------------------------------------------------------
// 处理字符串
//-------------------------------------------------------------------------
BOOL RTSession::Forbid_DeleteBlankOfString(tstring &str)
{
	// 去掉空格
	INT nFind = 0;
	
	if((nFind = str.find_first_not_of(' ')) != str.npos)
	{
		str = str.substr(nFind);
	}

	if((nFind = str.find(' ')) != str.npos)
	{
		str = str.substr(0,nFind);
	}

	// 检查用户名是否为空
	return !str.empty();
}
//-----------------------------------------------------------------------------------------------
// 封停/解封 指定IP
//-----------------------------------------------------------------------------------------------
DWORD RTSession::HandleForbidIP(tagNetCmd* pMsg, DWORD)
{
	//根据消息 封停
	tagNSS_ForbidIP* pRecv = (tagNSS_ForbidIP*)pMsg;
	DWORD dwRet = 0;


	if(pRecv->bForbid == TRUE)
	{
		dwRet = sBeton.ForbidIP(pRecv->dwIP);
	}
	else
	{
		dwRet = sBeton.RemoveIPForbid(pRecv->dwIP);
	}

	tagNSC_ForbidIP send;
	send.dwClientID = pRecv->dwClientID;
	send.dwErrorCode = dwRet;
	m_pTran->Send(&send, send.dwSize);

	return 0;
}
//-----------------------------------------------------------------------------------------------
// 得到要封停的游戏世界名的crc
//-----------------------------------------------------------------------------------------------
DWORD RTSession::Forbid_GetForbidWorldCrc(tagNetCmd* pMsg,vector <DWORD> &vecWorldCrc)
{
	DWORD	dwWorldNameCrc	= 0;
	DWORD	dwErrorCode		= E_RT_Success;
	tagNSS_ForbidAccount* pRecv = (tagNSS_ForbidAccount*)pMsg;

	//如果接受到得世界名为空 则封停大区下所有的游戏世界
	if(_tcscmp(_T(""),pRecv->szWorld)  == 0)
	{
		for (int n=0; n<m_nZoneNum; n++)
		{
			vecWorldCrc.push_back(m_dwWorldNameCRC[n]);	
		}
		return dwErrorCode;
	}
	//如果是指定游戏世界
	else
	{
		//用worldid找一下worldnamecrc
		for (int n=0; n<m_nZoneNum; n++)
		{
			if(	_tcscmp(m_strWorldName[n].c_str(),pRecv->szWorld)  == 0)
			{
				vecWorldCrc.push_back(m_dwWorldNameCRC[n]);	
				break;
			}
		}
		//表示没找到
		if(vecWorldCrc.size() == 0)
		{
			dwErrorCode = E_RT_Error;
		}
	}
	return dwErrorCode;
}
//-----------------------------------------------------------------------------------------------
// 得到要封停账号  逗号分隔
//-----------------------------------------------------------------------------------------------
VOID RTSession::Forbid_GetForbidAccountName(tstring &strMultiAccountName,vector <tstring> &vecAccountName)
{
	int iprepos = 0;
	int icurpos = 0;
	for(;;)
	{
		tstring strAccountName;

		icurpos = strMultiAccountName.find(',',iprepos);
		//最后一个名字
		if(icurpos == -1)
		{
			strAccountName = strMultiAccountName.substr(iprepos,strMultiAccountName.length() - iprepos);
			Forbid_DeleteBlankOfString(strAccountName);

			vecAccountName.push_back(strAccountName);
			break;
		}

		strAccountName = strMultiAccountName.substr(iprepos,icurpos-iprepos);
		Forbid_DeleteBlankOfString(strAccountName);

		vecAccountName.push_back(strAccountName);
		iprepos = icurpos + 1;
	}
}
//-----------------------------------------------------------------------------------------------
// 封停/解封 指定账号 的指定游戏世界
//-----------------------------------------------------------------------------------------------
DWORD RTSession::HandleForbidAccount(tagNetCmd* pMsg, DWORD)
{
	//根据消息 封停或解封
	tagNSS_ForbidAccount* pRecv = (tagNSS_ForbidAccount*)pMsg;
	DWORD	dwRet			= FALSE;
	DWORD	dwErrorCode		= E_RT_Success;
	vector <DWORD> vecWorldCrc;

	//FOR SAFE
	DWORD dwAccountByte = pRecv->dwSize - sizeof(pRecv->dwSize) - sizeof(pRecv->dwID) - sizeof(pRecv->dwClientID) - 
		sizeof(pRecv->szWorld)	- sizeof(pRecv->bForbid);
	pRecv->szAccount[dwAccountByte/sizeof(TCHAR) - 1] = _T('\0');

	//目前按逗号分隔
	tstring strMultiAccountName = pRecv->szAccount;

	if(strMultiAccountName.length() == 0)
	{
		dwErrorCode = E_RT_Error;
	}
	if(dwErrorCode == E_RT_Error)
	{
		//往回gmserver返结果
		tagNSC_ForbidAccount send;	
		send.dwClientID		=	pRecv->dwClientID;
		send.dwErrorCode	=	dwErrorCode;
		_tcscpy(send.szAccount,_T(""));
		m_pTran->Send(&send, send.dwSize);
		return 0;
	}


	//得到要封停的游戏世界名的crc
	dwErrorCode = Forbid_GetForbidWorldCrc(pRecv,vecWorldCrc);
	if(dwErrorCode == E_RT_Error)
	{
		//往回gmserver返结果
		tagNSC_ForbidAccount send;	
		send.dwClientID		=	pRecv->dwClientID;
		send.dwErrorCode	=	dwErrorCode;
		_tcscpy(send.szAccount,_T(""));
		m_pTran->Send(&send, send.dwSize);
		return 0;
	}

	//得到要封停账号  逗号分隔
	vector <tstring> vecAccountName;
	Forbid_GetForbidAccountName(strMultiAccountName,vecAccountName);

	//是否所有操作都成功
	std::map <tstring,DWORD> mpReturnCode;
	BOOL bAllSuccess = TRUE;

	for(int iWorldLoop = 0; iWorldLoop<vecWorldCrc.size(); iWorldLoop++)
	{
		for(int iAccountLoop = 0; iAccountLoop<vecAccountName.size(); iAccountLoop++)
		{
			//如果是封停指定账户
			if(pRecv->bForbid == TRUE)
			{
				DWORD dwAccountID = GT_INVALID;
				dwRet = sBeton.ForbidAccount(vecAccountName[iAccountLoop].c_str(),EPLM_GMTool,dwAccountID,vecWorldCrc[iWorldLoop]);		
				dwErrorCode = dwRet;
				if(dwErrorCode == E_RT_Success && dwAccountID != GT_INVALID)
				{
					LoongWorld* pWorld = sLoongWorldMgr.GetLoongWorld(vecWorldCrc[iWorldLoop]);
					if( P_VALID(pWorld) )
					{
						tagNLW_KickPlayer send;
						send.dwAccountID = dwAccountID;
						pWorld->SendKickPlayerMsg(send);
					}
				}
			}
			else if(pRecv->bForbid == FALSE)
			{
				dwRet = sBeton.RemoveAccountForbid(vecAccountName[iAccountLoop].c_str(),EPLM_GMTool,vecWorldCrc[iWorldLoop]);
				dwErrorCode = dwRet;
			}
			if(dwErrorCode != E_RT_Success)
			{
				bAllSuccess = FALSE;
				mpReturnCode.insert(make_pair(vecAccountName[iAccountLoop],dwErrorCode));
			}
		}
	}
	//如果所有操作都成功 只返回一个成功
	if(bAllSuccess == TRUE)
	{
		//往回gmserver返结果
		tagNSC_ForbidAccount send;	
		send.dwClientID		=	pRecv->dwClientID;
		send.dwErrorCode	=	dwErrorCode;
		_tcscpy(send.szAccount,_T(""));
		m_pTran->Send(&send, send.dwSize);
	}
	//遍历mpReturnCode 只返回失败的
	else
	{
		std::map <tstring,DWORD>::iterator itReturnCode = mpReturnCode.begin();
		for(;itReturnCode != mpReturnCode.end(); ++itReturnCode)
		{
			//往回gmserver返结果
			tagNSC_ForbidAccount send;	
			send.dwClientID		=	pRecv->dwClientID;
			send.dwErrorCode	=	itReturnCode->second;
			if(itReturnCode->first.length() >= X_SHORT_NAME)
			{
				tstring str = itReturnCode->first.substr(0,X_SHORT_NAME-1);
				_tcscpy(send.szAccount,str.c_str());
			}
			else
			{
				_tcscpy(send.szAccount,itReturnCode->first.c_str());
			}

			m_pTran->Send(&send, send.dwSize);
		}
	}

	return 0;
}
