//-----------------------------------------------------------------------------
//!\file to bill_session.cpp
//!\author Sunnee
//!
//!\date 2009-07-04
//! last 2009-07-04
//!
//!\brief 服务器与划拨服务器的对话层
//!
//!	Copyright (c) 2005 TENGWU Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#include "stdafx.h"

#include "../ServerDefine/msg_bill_s.h"
#include "../ServerDefine/base_define.h"
#include "../ServerDefine/role_data.h"
#include "../ServerDefine/bill_define.h"
#include "bill_session.h"
#include "world_session.h"
#include "player_session.h"
#include "role.h"
#include "../ServerDefine/log_cmdid_define.h"

//-----------------------------------------------------------------------------
//! construction
//-----------------------------------------------------------------------------
BillSession::BillSession() : m_Trunk(this)
{
	m_bInitOK					=	FALSE;
	m_bTermConnect				=	FALSE;
}

//-----------------------------------------------------------------------------
//! destruction
//-----------------------------------------------------------------------------
BillSession::~BillSession()
{
}

//-----------------------------------------------------------------------------
// init
//-----------------------------------------------------------------------------
BOOL BillSession::Init()
{
	m_pThread		=	"Thread";
	m_pUtil			=	"Util";

	// 初始化成员属性
	TObjRef<VarContainer> pVar = "VarContainer";
	m_strIP			=	pVar->GetString(_T("ip bill_server"));
	m_dwPort		=	pVar->GetDword(_T("port bill_server"));
	m_dwSectionID	=	pVar->GetDword(_T("section_id world"));
	m_dwWorldID		=	pVar->GetDword(_T("id world"));

	// 创建消息管理
	CreateObj("ToBillNetCmdMgr", "NetCmdMgr");
	m_pMsgCmdMgr = "ToBillNetCmdMgr";

	// 创建连接对象，并初始化
	CreateObj("ToBill", "StreamTransport");
	m_pTran = "ToBill";
	if( !P_VALID(m_pTran) )
	{
		ERR(_T("Create ToBill(StreamTransport) obj failed!\r\n"));
		return FALSE;
	}
	m_pTran->Init();

	// 注册所有网络命令
	RegisterAllBillCommand();

	// 启动连接线程
	if(!m_pThread->CreateThread(_T("ConnectBill"), 
		(vEngine::THREADPROC)m_Trunk.sfp0(&BillSession::ThreadConnectBill), NULL))
	{
		return FALSE;
	}

	while( !m_pThread->IsThreadActive(_T("ConnectBill")) )
	{
		continue;
	}
	return TRUE;
}

//-----------------------------------------------------------------------------
// destroy
//-----------------------------------------------------------------------------
VOID BillSession::Destroy()
{
	// 等待所有线程结束
	InterlockedExchange((LONG*)&m_bTermConnect, TRUE);
	m_pThread->WaitForThreadDestroy(_T("ConnectBill"), INFINITE);

	m_pTran->Destroy();
	KillObj("ToBill");

	// 注销消息管理
	m_pMsgCmdMgr->Destroy();
	KillObj("ToBillNetCmdMgr");
}



//-----------------------------------------------------------------------------
// update
//-----------------------------------------------------------------------------
VOID BillSession::Update()
{
	UpdateSession();
}

//-----------------------------------------------------------------------------
// 接收消息
//-----------------------------------------------------------------------------
VOID BillSession::UpdateSession()
{
	if( NULL == m_pTran )
		return;

	if( !m_pTran->IsConnected() && !m_pThread->IsThreadActive(_T("ConnectBill")) )
	{
		InterlockedExchange((LONG*)&m_bTermConnect, TRUE);
		m_pTran->Disconnect();

		m_pThread->WaitForThreadDestroy(_T("ConnectBill"), INFINITE);

		// 重新启动登陆服务器连接线程
		InterlockedExchange((LONG*)&m_bTermConnect, FALSE);
		m_pThread->CreateThread(_T("ConnectBill"), (vEngine::THREADPROC)m_Trunk.sfp0(&BillSession::ThreadConnectBill), NULL);

		while(FALSE == m_pThread->IsThreadActive(_T("ConnectBill")))
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
// 连接线程(连接监控服务器)
//-----------------------------------------------------------------------------
DWORD BillSession::ThreadConnectBill()
{
#ifdef _DEBUG
	EXCEPTION_PROTECT_START;
#endif

	while( FALSE == m_bTermConnect )
	{
		if( !m_pTran->IsConnected() )
		{
			if( !m_pTran->IsTryingToConnect() )
			{
				m_pTran->TryToConnect(m_pUtil->UnicodeToUnicode8(m_strIP.c_str()), m_dwPort);
			}

			Sleep(100);
			continue;	// 重新检测连接
		}

		IMSG(_T("Contected to Bill Server at %s: %d\r\n"), m_strIP.c_str(), m_dwPort);


		tagNWB_Login send;
		send.dwWorldID		=	m_dwWorldID;
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
VOID BillSession::RegisterAllBillCommand()
{
	m_pMsgCmdMgr->Register("NBW_Login",			(NETMSGPROC)m_Trunk.sfp2(&BillSession::HandleServerLogin),	_T("Bill Server Certification Reply"));
	m_pMsgCmdMgr->Register("NBW_SafePickBaiBao",(NETMSGPROC)m_Trunk.sfp2(&BillSession::HandlePickBaiBao),	_T("Notice DB & Client to Reflesh BaiBao"));
}

//----------------------------------------------------------------------------------------------
// 服务器认证网络消息
//----------------------------------------------------------------------------------------------
DWORD BillSession::HandleServerLogin(tagNetCmd* pMsg, DWORD)
{
	tagNBW_Login* pRecv = (tagNBW_Login*)pMsg;

	if (pRecv->dwErrorCode == E_Success)
	{
		InterlockedExchange((LPLONG)&m_bInitOK, TRUE);
	}
	else
	{
		m_pTran->Disconnect();
		InterlockedExchange((LPLONG)&m_bInitOK, FALSE);
	}
	return 0;
}
//----------------------------------------------------------------------------------------------
// 通知客户端百宝袋元宝发生变化
//----------------------------------------------------------------------------------------------
DWORD BillSession::NoticeTheBaiBaoYuanbaoChange(tagNetCmd* pMsg)
{
	tagNBW_SafePickBaiBao* pRecv = (tagNBW_SafePickBaiBao*)pMsg;

	// 找到指定账号的在线玩家
	PlayerSession* pPlayerSession = g_worldSession.FindSession(pRecv->dwAccountID);

	// 账号不在线
	if (!P_VALID(pPlayerSession))
		return GT_INVALID;

	// 检查玩家是否游戏中
	Role* pRole = pPlayerSession->GetRole();

	// 重新设置玩家百宝元宝的数量(这里只会增加玩家元宝数量)		
	if(pRecv->dwIncYuanbao <= 0)
		return GT_INVALID;

	if (P_VALID(pRole))
	{	
		//得到当前元宝数
		pRole->GetCurMgr().IncBaiBaoYuanBao(pRecv->dwIncYuanbao, ELCID_BaiBao_Bill_YuanBao, FALSE);
	}
	// 是否在选人界面
	else if (P_VALID(g_worldSession.FindGlobalSession(pRecv->dwAccountID)))
	{
		pPlayerSession->SetBaiBaoYB(pRecv->dwIncYuanbao + pRecv->nCurYuanBao);
	}

	return E_Success;
}
//----------------------------------------------------------------------------------------------
// 刷新百宝袋内容
//----------------------------------------------------------------------------------------------
DWORD BillSession::HandlePickBaiBao( tagNetCmd* pMsg, DWORD )
{
	tagNBW_SafePickBaiBao* pRecv = (tagNBW_SafePickBaiBao*)pMsg;

	// 找到指定账号的在线玩家
	PlayerSession* pPlayerSession = g_worldSession.FindSession(pRecv->dwAccountID);
	
	// 账号不在线
	if (!P_VALID(pPlayerSession))
		return GT_INVALID;

	switch (pRecv->eBillType)
	{
	case EBBT_Yuanbao:
		{
			/* 由于出现时序错误删除
			// 更新百宝袋中元宝的数量
			tagNDBC_LoadBaiBaoYuanBao SendYuanBao;
			SendYuanBao.dwAccountID = pRecv->dwAccountID;
			g_dbSession.Send(&SendYuanBao, SendYuanBao.dwSize);
			*/

			//改变元宝数量 并且通知客户端
			NoticeTheBaiBaoYuanbaoChange(pRecv);
		}
		break;

	case EBBT_Item:
		{
			// 在游戏中
			Role* pRole = pPlayerSession->GetRole();
			if (P_VALID(pRole))
			{
				// 如果百宝袋中有空位，则申请检查是否还有未放入的物品
				if(pRole->GetItemMgr().GetBaiBaoFreeSize() > 0)
				{
					tagNDBC_LoadBaiBao send;
					send.n64Serial = pRecv->n64Serial;
					send.dwAccountID = pRecv->dwAccountID;
					send.nFreeSpaceSize = pRole->GetItemMgr().GetBaiBaoFreeSize();

					g_dbSession.Send(&send, send.dwSize);
				}
			}
		}
		break;
	}

	return 0;
}

BillSession g_billSession;
