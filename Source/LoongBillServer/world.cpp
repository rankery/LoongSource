//-----------------------------------------------------------------------------
//!\file world.cpp
//!\author Aslan
//!
//!\date 2009-06-22
//! last 2009-06-22
//!
//!\brief 游戏世界
//!
//!	Copyright (c) 2004 TENGWU Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#include "stdafx.h"

#include "../ServerDefine/msg_bill_c.h"
#include "../ServerDefine/msg_bill_s.h"
#include "../ServerDefine/bill_define.h"

#include "world_db.h"
#include "world.h"
#include "world_mgr.h"
#include "bill_server.h"
#include "login_db.h"

//-----------------------------------------------------------------------------
// CONSTRUCT
//-----------------------------------------------------------------------------
World::World() : m_dwID(GT_INVALID), m_bValid(FALSE), m_bTerminate(FALSE), m_strThreadName(), m_DB(this),
				m_nBillYuanbaoSuccessTimes(0),m_nBillItemSuccessTimes(0)
{

}

World::~World()
{
	Destroy();
}

//------------------------------------------------------------------------------
// 初始化
//------------------------------------------------------------------------------
BOOL World::Init(INT nIndex)
{
	m_pThread = "Thread";

	TObjRef<VarContainer> pVar = "BillConfig";

	// 得到world的ID
	TCHAR szTemp[X_SHORT_NAME] = {_T('\0')};
	_stprintf(szTemp, _T("world_%d"), nIndex);
	m_dwID	=	pVar->GetDword(_T("id"), szTemp);

	// 初始化数据库
	if( !m_DB.Init(nIndex) )
	{
		IMSG(_T("world db init failed, id=%u\r\n"), m_dwID);
		ILOG->Write(_T("world db init failed, id=%u\r\n"), m_dwID);
	}

	// 开启线程
	tstringstream tempStream;
	tempStream << _T("thread_world_") << m_dwID;
	m_strThreadName = tempStream.str();

	m_pThread->CreateThread(m_strThreadName.c_str(), (vEngine::THREADPROC)m_Trunk.sfp0(&World::ThreadUpdate), NULL);
	while( !m_pThread->IsThreadActive(m_strThreadName.c_str()) ) continue;

	return TRUE;
}

//--------------------------------------------------------------------------------
// 销毁
//--------------------------------------------------------------------------------
VOID World::Destroy()
{
	// 关闭线程
	InterlockedExchange((LPLONG)&m_bTerminate, TRUE);
	m_pThread->WaitForThreadDestroy(m_strThreadName.c_str(), INFINITE);

	// 清空数据库
	m_DB.Destroy();
}

//---------------------------------------------------------------------------------
// 线程函数
//---------------------------------------------------------------------------------
VOID World::ThreadUpdate()
{
#ifdef DEBUG
	_set_se_translator(vEngine::SEH_Translation);

	try
	{
#endif
		while( !m_bTerminate )
		{
			// 更新数据库
			m_DB.Update();

			// 更新消息
			UpdateSession();

			Sleep(TICK_TIME * 5);
		}
#ifdef DEBUG
	}
	catch(vEngine::Exception)
	{
		if( IUTIL->IsDebuggerPresent() )
			throw;
		else
			g_Server.ShutDown();
	}
#endif

	_endthreadex(0);
}

//----------------------------------------------------------------------------------
// 更新消息
//----------------------------------------------------------------------------------
VOID World::UpdateSession()
{
	DWORD dwSize = 0;
	INT nUnRecved = 0;

	LPBYTE pMsg = g_WorldMgr.RecvMsg(GetID(), dwSize, nUnRecved);

	while( P_VALID(pMsg) )
	{
		g_WorldMgr.HandleCmd((LPVOID)pMsg, dwSize, this);

		g_WorldMgr.ReturnMsg(GetID(), pMsg);

		pMsg = g_WorldMgr.RecvMsg(GetID(), dwSize, nUnRecved);
	}
}