//-------------------------------------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: Loong.cpp
// author: Sxg
// actor:
// data: 2008-05-12
// last:
// brief: 程序框架定义实现
//-------------------------------------------------------------------------------------------------------
#include "StdAfx.h"
#include "../ServerDefine/base_define.h"
#include "../ServerDefine/role_data.h"
#include "../ServerDefine/msg_mall.h"
#include "../ServerDefine/msg_leftmsg.h"
#include "../WorldDefine/svn_revision.h"
#include "LoongDB.h"
#include "Loong.h"
#include "rt_session.h"
#include "TimeChecker.h"
//-------------------------------------------------------------------------------------------------------
//constructor
//-------------------------------------------------------------------------------------------------------
CLoong::CLoong()
{
	m_bTerminate			= FALSE;
	m_bGameServerLogout		= FALSE;
	m_bGameServerConnect	= FALSE;
	m_pDB					= new CLoongDB;
	m_dwLogTime				= 0;
	m_szLogFilePostfix[0]	= '\0';

	// 初始化字符串
	ZeroMemory(m_szWorldName, sizeof(m_szWorldName));
	//ZeroMemory(m_szServerName, sizeof(m_szServerName));

	// 初始化反馈消息(不要对m_pLoadSimRole,m_pCreateRole和m_pLoadRole使用ZeroMemory)
	m_pLoadSimRole = new tagNDBS_LoadSimRole;	// 反馈某个帐号下的基本角色信息
	m_pCreateRole = new tagNDBS_CreateRole;		// 反馈新创建的角色的基本信息
	m_pLoadBaiBao = new tagNDBS_LoadBaiBao;		// 反馈读取的百宝袋物品信息

	m_pLoadBaiBaoLog = (tagNDBS_LoadBaiBaoLog*)m_pBuffer;	// 不定长结构，与读取角色共用buffer(多线程时会有致命危险)

	// 该结构(m_pLoadRole)大小不定,故单独定义
	m_pLoadRole = (tagNDBS_LoadRole *)m_pBuffer;

	// 版本信息(在该处初始化，减少版本变化影响)
	m_Status.dwVerLoongDB		= LOONGDB_BUILD_REVISION;
	m_Status.dwVerBeton			= BETON_BUILD_REVISION;
	m_Status.dwVerVEngine		= VENGINE_BUILD_REVISION;
	m_Status.dwVerServerDefine	= SERVERDEFINE_BUILD_REVISION;
	m_Status.dwVerXML			= XMLREADER_BUILD_REVISION;

	// 初始化消息-处理函数管理器
	m_MsgCmdMgr.Init();
}

//-------------------------------------------------------------------------------------------------------
// destructor
//-------------------------------------------------------------------------------------------------------
CLoong::~CLoong()
{
	// 关闭所有应用层开启的线程
	InterlockedExchange((long*)(&m_bTerminate), TRUE);

	g_rtSession.Destroy();
	m_pThread->WaitForAllThreadDestroy();

	m_MsgCmdMgr.Destroy();

	// 回收内存
	SAFE_DEL(m_pCreateRole);
	SAFE_DEL(m_pLoadSimRole);
	SAFE_DEL(m_pLoadBaiBao);

	SAFE_DEL(m_pDB);

	// 销毁窗口及GUI等系统
	m_pWindow->Destroy();
	m_pConsole->Destroy();
	m_pGUI->Destroy();
	m_pRender->Destroy();
}
//-------------------------------------------------------------------------------------------------------
// 注册控制台命令
//-------------------------------------------------------------------------------------------------------
VOID CLoong::RegisterTestCmd(TObjRef<Console> pConsole)
{
	pConsole->Register(_T("dbcheck"), m_Trunk.sfp0(&CLoong::TestDBReadFunction), _T("check the db read funcation"), 0);

}
//-------------------------------------------------------------------------------------------------------
// 初始化操作
//-------------------------------------------------------------------------------------------------------
BOOL CLoong::Init(HINSTANCE hInst)
{
	// 初始化变量
	InitConfig();

	// 创建界面
	if(!CreateUI(hInst))
	{
		return FALSE;
	}

	//注册控制台命令
	RegisterTestCmd(m_pConsole);
	
	// ???屏蔽控制台窗口关闭按钮
	//m_pConsole->GetWnidow()->GetCloseButton()->SetEnable(false);


	//// 键盘响应
	//m_pInput->Init();
	//m_pInputMap->Register(0, _T("Hotkey_Console"), 
	//	(INPUTMAPEVENTHANDLE)m_pConsole->GetTrunk().sfp2(&vEngine::Console::Switch), DIK_F5, TRUE, TRUE);
	
	//注册命令处理模块
	RegisterDBCmd();

	//设置登入登出回调函数
	m_pServer->Init((TCPLOGINCALLBACK)m_Trunk.sfp2(&CLoong::LoginCallBack),
					(TCPLOGOUTCALLBACK)m_Trunk.sfp1(&CLoong::LogoutCallBack),
					m_nPort);

	// 远程监控初始化
	g_rtSession.Init(this);

	// 启动处理线程
	if(!m_pThread->CreateThread(_T("rec_msg"), (THREADPROC)m_Trunk.sfp1(&CLoong::ThreadReceiveMsg), NULL)
		|| !m_pThread->CreateThread(_T("proc_msg"), (THREADPROC)m_Trunk.sfp1(&CLoong::ThreadProcessMsg), NULL)
		|| !m_pThread->CreateThread(_T("broadcast"), (THREADPROC)m_Trunk.sfp1(&CLoong::ThreadBroadcast), NULL))
	{
		return FALSE;
	}

	return TRUE;
}
//-------------------------------------------------------------------------------------------------------
//分担一些基本的初始化工作
//-------------------------------------------------------------------------------------------------------
BOOL CLoong::InitConfig()
{
	TCHAR tszPath[MAX_PATH];
	ZeroMemory(tszPath, sizeof(tszPath));
	if (!m_pUtil->GetIniPath(tszPath, _T("server_config/db/world")))
	{
		return FALSE;
	}

	// 定义一个容器
	CreateObj("LoongVar", "VarContainer");
	TObjRef<VarContainer> pVar = "LoongVar";

	// 读入配置文件,并导入容器
	pVar->Load("VirtualFileSys", tszPath);

	m_nPort				= pVar->GetDword(_T("db_server port"));
	m_dwDBGoldenCode	= pVar->GetDword(_T("db_server golden_code"));
	m_dwGameGoldenCode	= pVar->GetDword(_T("game_server golden_code"));
	
	// 清空容器
	pVar->Clear();

	return TRUE;
}

//-------------------------------------------------------------------------------------------------------
// 广播(心跳等)
//-------------------------------------------------------------------------------------------------------
DWORD CLoong::ThreadBroadcast(LPVOID)
{
	tagHeartbeat Heartbeat;
	
	while(true)
	{
		// 关闭服务器
		if(m_bTerminate)
		{
			return 0;
		}

		m_Status.dwHeartbeat++;

		// 远程监控更新
		g_rtSession.Update();

		//// 向游戏服务器发送心跳/*m_pServer->IsClientExsit(ID_GAMESERVER)*/
		//if(m_bGameServerConnect)
		//{
		//	Heartbeat.dwGoldenCode		= m_dwDBGoldenCode;			// 服务器金色代码
		//	Heartbeat.dwHeartbeat		= m_Status.dwHeartbeat;		// 服务器心跳次数

		//	m_Status.dwUnSendMsgNum = SendMsg( &Heartbeat, sizeof(Heartbeat));
		//}

		// 每秒心跳一次
		Sleep(1000);
	}

	return 0;
}

//-------------------------------------------------------------------------------------------------------
// 获取游戏服务器消息
//-------------------------------------------------------------------------------------------------------
DWORD CLoong::ThreadReceiveMsg(LPVOID)
{
#ifdef _DEBUG
	EXCEPTION_PROTECT_START;
#endif
	
	LPBYTE	pRecv		= NULL;
	DWORD	dwMsgSize	= GT_INVALID;
	INT		nUnrecv		= GT_INVALID;
	
	while(true)
	{	
		Sleep(SLEEPTIME);
		
		// 判断线程是否结束
		if(m_bTerminate)
		{
			break;
		}

		// 判断是否有连接，如果没有，则无限等待
		if(!m_bGameServerConnect)
		{
			continue;
		}

		// 检查是否需要创建新的log表
		if(GetCurrentDWORDTime().day != m_dwLogTime.day)
		{
			if(m_MsgQue.GetMsgNum() == 0)
			{
				// 确保修改表后缀时，不会有函数访问该字段//??
				Sleep(SLEEPTIME);
				CreateCurDataLogTable();
			}
		}
		
		// 读取游戏服务器消息队列,并保存到待处理消息队列
		while(true)
		{
			pRecv = m_pServer->Recv(ID_GAMESERVER, dwMsgSize, nUnrecv);
			m_Status.dwUnRecvMsgNum = nUnrecv;
			if(!P_VALID(pRecv))
			{
				break;
			}

			if(dwMsgSize != ((tagNetCmd*)pRecv)->dwSize)
			{
				ASSERT(0);
				IMSG(_T("Invalid net command size[<cmd>%u <size>%d]\r\n"), 
					((tagNetCmd*)pRecv)->dwID, ((tagNetCmd*)pRecv)->dwSize);
				continue;
			}

			// 当m_MsgQue的构造函数第一个参数为true时，该函数自动激活线程
			m_MsgQue.AddMsg(pRecv);

			// 统计接收消息个数
			++m_Status.dwRecvMsgNum;

			// 回收资源 -- 重复利用该内存，避免没必要的内存拷贝
			//m_pServer->FreeRecved(pRecv);
		}
	}	

#ifdef _DEBUG
	EXCEPTION_PROTECT_END;
#endif
	_endthreadex(0);
	return 0;
}

//-------------------------------------------------------------------------------------------------------
// 处理服务器消息
//-------------------------------------------------------------------------------------------------------
DWORD CLoong::ThreadProcessMsg(LPVOID)
{
#ifdef _DEBUG
	EXCEPTION_PROTECT_START;
#endif

	LPBYTE	pMsg		= NULL;
	DWORD	dwMsgSize	= GT_INVALID;
	//定义一个time检查
	CHECK_TIME_WITH_INFO(NULL,500);

	while(true)
	{
		// 判断线程是否结束
		if(m_bTerminate)
		{
			break;
		}
		
		::WaitForSingleObject(m_MsgQue.GetEvent(), SLEEPTIME);

		while(true)
		{
			pMsg = m_MsgQue.GetMsg();
			if(!P_VALID(pMsg))
			{
				break;
			}

			std::string strCmpName = m_MsgCmdMgr.GetHadleCmdString((tagNetCmd*)pMsg);
			CHECK_TIME_START(strCmpName.c_str());

			// 处理消息
			m_MsgCmdMgr.HandleCmd((tagNetCmd*)pMsg, ((tagNetCmd*)pMsg)->dwSize, GT_INVALID);

			CHECK_TIME_COMMIT();

			++m_Status.dwProcMsgNum;

			// 回收资源
			m_pServer->FreeRecved(ID_GAMESERVER, pMsg);
		}

		// 检查游戏服务器是否退出
		if( m_bGameServerLogout )
		{
			m_bGameServerLogout = FALSE;
			
			TermWorld();
			
			// 重置连接状态,保证其他服务器能够连接
			InterlockedExchange((LONG *)&m_bGameServerConnect, FALSE);
		}
	}

#ifdef _DEBUG
	EXCEPTION_PROTECT_END;
#endif
	_endthreadex(0);
	return 0;
}

//-------------------------------------------------------------------------------------------------------
// 游戏世界连接登入的通知
//-------------------------------------------------------------------------------------------------------
DWORD CLoong::LoginCallBack(LPBYTE pByte, DWORD dwSize)
{
	CResGuard::CGuard rg(m_Lock);

	// 当前已有一个游戏世界连接或连接验证错误
	if(m_bGameServerConnect != FALSE
		|| *(DWORD*)pByte != m_pUtil->Crc32("N_Certification"))
	{
		IMSG(_T("A new game world apply for connecting，or connection message is wrong!\r\n"));
		return GT_INVALID;
	}

	MTRANS_POINTER(pNc, pByte, tagN_Certification);
	if(pNc->dwGoldenCode != m_dwGameGoldenCode)
	{
		IMSG(_T("Illegal connection, golden code is wrong!"));
		return GT_INVALID;
	}

	if(InitWorld(pNc->szWorldName) != E_Success)
	{
		return GT_INVALID;
	}

	// 保证某个时刻只有一个连接
	InterlockedExchange((LONG *)&m_bGameServerConnect, TRUE);
	m_bGameServerLogout	= FALSE;

	IMSG(_T("LoongWorld Server of %s login!\n"), pNc->szWorldName);
	
	return ID_GAMESERVER; // 每个游戏世界仅有一个游戏服务器
}

//-------------------------------------------------------------------------------------------------------
// 游戏世界连接登出的通知
//-------------------------------------------------------------------------------------------------------
DWORD CLoong::LogoutCallBack(DWORD dwDummy)
{
	CResGuard::CGuard rg(m_Lock);

	// 收完所有消息
	LPBYTE pRecv = NULL;
	DWORD dwMsgSize = 0;
	INT nUnrecv = 0;
	while(true)
	{
		LPBYTE pRecv = m_pServer->Recv(ID_GAMESERVER, dwMsgSize, nUnrecv);
		m_Status.dwUnRecvMsgNum = nUnrecv;
		if(!P_VALID(pRecv))
		{
			break;
		}

		if(dwMsgSize != ((tagNetCmd*)pRecv)->dwSize)
		{
			ASSERT(0);
			IMSG(_T("Invalid net command size[<cmd>%u <size>%d]\r\n"), 
				((tagNetCmd*)pRecv)->dwID, ((tagNetCmd*)pRecv)->dwSize);
			continue;
		}

		m_MsgQue.AddMsg(pRecv);

		// 统计接收消息个数
		++m_Status.dwRecvMsgNum;
	}

	// 设置world服务器登出
	m_bGameServerLogout	= TRUE;

	IMSG(_T("LoongWorld Server of %s logout!\n"), m_szWorldName);

	return 0;
}

//-------------------------------------------------------------------------------------------------------
// 初始化游戏世界
//-------------------------------------------------------------------------------------------------------
DWORD CLoong::InitWorld(LPCTSTR szWorldName)
{
	TCHAR szIniFile[MAX_PATH];

	// 获取配置文件名称
	_tcscpy(szIniFile, _T("server_config/db/"));
	_tcscat(szIniFile, szWorldName);

	TCHAR tszPath[MAX_PATH];
	ZeroMemory(tszPath, sizeof(tszPath));
	if (!m_pUtil->GetIniPath(tszPath, szIniFile))
	{
		return FALSE;
	}

	// 初始化数据库引擎
	if(!m_pDB->Init(tszPath))
	{
		IMSG(_T("Database for %s connected failed!\n"), m_szWorldName);

		// 向监控发送消息 //??

		return GT_INVALID;
	}

	// 创建log表
	CreateCurDataLogTable();

	// 记录游戏世界名称和运行游戏世界的机器名
	_tcsncpy(m_szWorldName, szWorldName, MCalTCharArrayNum(m_szWorldName) - 1);
	//_tcsncpy(m_szServerName, szServerName, MCalTCharArrayNum(m_szServerName) - 1);

	// 初始化统计数据
	m_Status.Init();

	IMSG(_T("Database for %s connected!\n"), m_szWorldName);

	return E_Success;
}

//-------------------------------------------------------------------------------------------------------
// 关闭游戏世界
//-------------------------------------------------------------------------------------------------------
VOID CLoong::TermWorld()
{
	// 收完所有消息，并处理
	LPVOID pMsg = NULL;
	DWORD dwMsgSize = 0;

	while(true)
	{
		LPBYTE pMsg = m_MsgQue.GetMsg();
		if(!P_VALID(pMsg)) break;

		// 处理消息
		m_MsgCmdMgr.HandleCmd((tagNetCmd*)pMsg, ((tagNetCmd*)pMsg)->dwSize, GT_INVALID);

		++m_Status.dwProcMsgNum;

		// 回收资源
		m_pServer->FreeRecved(ID_GAMESERVER, pMsg);
	}

	// 删除DB
	m_pDB->Destroy();
	m_dwLogTime = 0;
	m_szLogFilePostfix[0]	= '\0';

	// 清空游戏世界名和启动游戏世界的机器名
	ZeroMemory(m_szWorldName, sizeof(m_szWorldName));
	//ZeroMemory(m_szServerName, sizeof(m_szServerName));
}


//-------------------------------------------------------------------------------------------------------
// 创建UI界面
//-------------------------------------------------------------------------------------------------------
BOOL CLoong::CreateUI(HINSTANCE hInst)
{
	// 初始化显示
	m_Render.Init(m_pRender.operator->());	// 与vEngine中默认启动的render引擎连接
	m_pWindow->Init(&m_Render,800,600,TRUE);
	m_pWindow->CreateWnd(_T("LoongDB"), hInst);

	if(!m_pRender->Init(800, 600))
	{
		return FALSE;
	}

	m_pGUI->Init(700, 500, &m_Render);	
	m_pRender->SetShowFPS(FALSE);

	return m_pConsole->Init(NULL);
}

//-------------------------------------------------------------------------------------------------------
// 更新UI界面
//-------------------------------------------------------------------------------------------------------
VOID CLoong::RefreshUI()
{
	m_pGUI->Update();
	m_pRender->Clear(0, 0, 0);
	m_pGUI->Render();
	m_pRender->Render();

	const CLoong::tagStatus *pStatus = GetStatus();

	// 服务器系统参数
	m_pConsole->Watch(_T("cpu_num:"), pStatus->nCpuNum);
	m_pConsole->Watch(_T("mem_total(M):"), pStatus->dwTotalPhys/1024/1024);
	m_pConsole->Watch(_T("mem_avail(M):"), pStatus->dwAvailPhys/1024/1024);
	m_pConsole->Watch(_T("non_paged_pool_usage:"), pStatus->nQuotaNonPagedPoolUsage);

	m_pConsole->Watch(_T("-- Version --"), 0);

	// 版本信息
	m_pConsole->Watch(_T("LoongDB:"), m_Status.dwVerLoongDB);
	//m_pConsole->Watch(_T("Beton:"), m_Status.dwVerBeton);
	//m_pConsole->Watch(_T("vEngine:"), m_Status.dwVerVEngine);
	//m_pConsole->Watch(_T("ServerDef:"), m_Status.dwVerServerDefine);
	//m_pConsole->Watch(_T("XML:"), m_Status.dwVerXML);

	m_pConsole->Watch(_T("-- Status --"), 0);

	// 心跳次数
	m_pConsole->Watch(_T("connect_status:"), m_bGameServerConnect);
	m_pConsole->Watch(_T("heart_beat:"), pStatus->dwHeartbeat);

	// 消息处理
	m_pConsole->Watch(_T("-- Total --"), 0);
	m_pConsole->Watch(_T("RecvNum:"), pStatus->dwRecvMsgNum);
	m_pConsole->Watch(_T("ProcMsgNum:"), pStatus->dwProcMsgNum);
	m_pConsole->Watch(_T("SendNum:"), pStatus->dwSendMsgNum);
	m_pConsole->Watch(_T("RecvSz:"), pStatus->dwRecvMsgSz);
	m_pConsole->Watch(_T("SendSz:"), pStatus->dwSendMsgSz);

	m_pConsole->Watch(_T("ReadNum:"), pStatus->dwReadNum);
	m_pConsole->Watch(_T("WriteNum:"), pStatus->dwWriteNum);

	m_pConsole->Watch(_T("-- Tick(ms) --"), pStatus->dwInterval);
	m_pConsole->Watch(_T("TickRecvNum:"), pStatus->dwTickRecvNum);
	m_pConsole->Watch(_T("TickSendNum:"), pStatus->dwTickSendNum);
	m_pConsole->Watch(_T("TickRecvSz:"), pStatus->dwTickRecvSz);
	m_pConsole->Watch(_T("TickSendSz:"), pStatus->dwTickSendSz);

	m_pConsole->Watch(_T("TickProcNum:"), pStatus->dwTickProcNum);
	m_pConsole->Watch(_T("TickReadNum:"), pStatus->dwTickReadNum);
	m_pConsole->Watch(_T("TickWriteNum:"), pStatus->dwTickWriteNum);

	m_pConsole->Watch(_T("-- Caution: --"), 0);
	m_pConsole->Watch(_T("UnSendMsgNum:"), pStatus->dwUnSendMsgNum);
	m_pConsole->Watch(_T("UnRecvMsgNum: --"), pStatus->dwUnRecvMsgNum);
	m_pConsole->Watch(_T("MaxSendedMsgSize: --"), pStatus->dwMaxSendedMsgSize);
	m_pConsole->Watch(_T("MaxSendedMsgID: --"), pStatus->dwMaxSendedMsgID);
}

//-------------------------------------------------------------------------------------------------------
// 得到服务器状态信息
//-------------------------------------------------------------------------------------------------------
const CLoong::tagStatus* CLoong::GetStatus()
{
	// 系统相关
	MEMORYSTATUS memStatus;
	memStatus.dwLength = sizeof(memStatus);
	GlobalMemoryStatus(&memStatus);
	m_Status.dwTotalPhys = memStatus.dwTotalPhys;
	m_Status.dwAvailPhys = memStatus.dwAvailPhys;

	PROCESS_MEMORY_COUNTERS pmc;
	GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));
	m_Status.nQuotaNonPagedPoolUsage = pmc.QuotaNonPagedPoolUsage;

	// 系统时间
	DWORD dwTick = GetTickCount();
	m_Status.dwInterval = dwTick - m_Status.dwLastTick;
	m_Status.dwLastTick = dwTick;

	// 游戏世界相关
	m_Status.dwReadNum		= m_pDB->GetTotalReadTimes();
	m_Status.dwWriteNum		= m_pDB->GetTotalWriteTimes();

	m_Status.dwSendMsgNum	= m_pServer->GetMsgSend();
	m_Status.dwRecvMsgSz	= m_pServer->GetBytesRecv();
	m_Status.dwSendMsgSz	= m_pServer->GetBytesSend();

	m_Status.dwTickRecvNum	= m_Status.dwRecvMsgNum - m_Status.dwLastRecvNum;
	m_Status.dwTickProcNum	= m_Status.dwProcMsgNum - m_Status.dwLastProcNum;
	m_Status.dwTickSendNum	= m_Status.dwSendMsgNum - m_Status.dwLastSendNum;

	m_Status.dwTickRecvSz	= m_Status.dwRecvMsgSz - m_Status.dwLastRecvSz;
	m_Status.dwTickSendSz	= m_Status.dwSendMsgSz - m_Status.dwLastSendSz;

	m_Status.dwTickReadNum	= m_Status.dwReadNum - m_Status.dwLastReadNum;
	m_Status.dwTickWriteNum	= m_Status.dwWriteNum - m_Status.dwLastWriteNum;

	m_Status.dwLastRecvNum	= m_Status.dwRecvMsgNum;
	m_Status.dwLastProcNum	= m_Status.dwProcMsgNum;
	m_Status.dwLastSendNum	= m_Status.dwSendMsgNum;

	m_Status.dwLastRecvSz	= m_Status.dwRecvMsgSz;
	m_Status.dwLastSendSz	= m_Status.dwSendMsgSz;

	m_Status.dwLastReadNum	= m_Status.dwReadNum;
	m_Status.dwLastWriteNum	= m_Status.dwWriteNum;

	return &m_Status;
}

//-------------------------------------------------------------------------------------------------------
// 更新UI界面
//-------------------------------------------------------------------------------------------------------
VOID CLoong::MainLoop()
{
	DWORD dwMsg, dwParam1, dwParam2;
	
	while(!m_pWindow->MsgLoop() && !IsTerminate())
	{	
		if(FALSE == m_pWindow->IsWindowActive())
		{
			Sleep(50);
			continue;
		}
		
		while(m_pWindow->PeekWindowsMsg(dwMsg, dwParam1, dwParam2))
		{
			m_pGUI->OnWindowsMsg(dwMsg, dwParam1, dwParam2);
			if(dwMsg == WM_QUIT)
				return;
		}
		
		// 更新界面
		RefreshUI();

		//// 快捷键响应
		//m_pInput->Update();
		//m_pInputMap->Update();

		Sleep(500);
	}
}

DWORD CLoong::LoadLeftMsg( DWORD pDBMsg, DWORD dwDummy )
{
	MTRANS_POINTER(pRecv, pDBMsg, tagNDBC_LoadLeftMsg);

	BYTE buffer[1024 * MAX_LEFTMSG_NUM] = {0};
	tagNDBS_LoadLeftMsg* pSend = reinterpret_cast<tagNDBS_LoadLeftMsg*>(buffer);
	pSend->dwID = m_pUtil->Crc32("NDBS_LoadLeftMsg");
	pSend->dwRoleID = pRecv->dwRoleID;
	DWORD dwAllLeftMsgSize = 0;
	BOOL bHasLeftMsg = m_pDB->LoadLeftMsg(pRecv->dwRoleID, pSend->byData, dwAllLeftMsgSize);
	if (bHasLeftMsg)
	{
		pSend->dwSize = sizeof(tagNDBS_LoadLeftMsg) - 1 + dwAllLeftMsgSize;
	}
	else
	{
		pSend->dwSize = sizeof(tagNDBS_LoadLeftMsg);
	}
	
	SendMsg( pSend, pSend->dwSize);

	return E_Success;
}

DWORD CLoong::InsertLeftMsg( DWORD pDBMsg, DWORD dwDummy )
{
	MTRANS_POINTER(pRecv, pDBMsg, tagNDBC_InsertLeftMsg);
	DWORD dwIndexes[MAX_LEFTMSG_NUM] = {0};
	INT nNum = 0;
	m_pDB->LoadLeftMsgIndexes(dwIndexes, pRecv->data.dwRoleID, nNum);
	if (nNum >= MAX_LEFTMSG_NUM)
	{
		m_pDB->DelLeftMsg(pRecv->data.dwRoleID, dwIndexes[0]);
	}
	m_pDB->InsertLeftMsg(pRecv->data.dwRoleID, pRecv->data.dwDateTime, &(pRecv->data));
	return E_Success;
}

DWORD CLoong::ClearLeftMsg( DWORD pDBMsg, DWORD dwDummy )
{
	MTRANS_POINTER(pRecv, pDBMsg, tagNDBC_ClearLeftMsg);
	m_pDB->ClearLeftMsg(pRecv->dwRoleID);
	return E_Success;
}

DWORD CLoong::UpdateRoleAtt( DWORD pDBMsg, DWORD dwDummy )
{
	MTRANS_POINTER(pRecv, pDBMsg, tagNDBC_UpdateRoleAtt);
	m_pDB->UpdateRoleAtt(pRecv->dwRoleID, pRecv);
	return E_Success;
}

DWORD CLoong::UpdateRoleAttPoint( DWORD pDBMsg, DWORD dwDummy )
{
	MTRANS_POINTER(pRecv, pDBMsg, tagNDBC_UpdateRoleAttPoint);
	m_pDB->UpdateRoleAttPoint(pRecv->dwRoleID, (PVOID)pDBMsg);
	return E_Success;
}

DWORD CLoong::UpdateRoleTalentPoint( DWORD pDBMsg, DWORD dwDummy )
{
	MTRANS_POINTER(pRecv, pDBMsg, tagNDBC_UpdateRoleTalentPoint);
	m_pDB->UpdateRoleTalentPoint(pRecv->dwRoleID, (PVOID)pDBMsg);
	return E_Success;
}
DWORD CLoong::UpdateRoleItemTransport( DWORD pDBMsg, DWORD dwDummy )
{
	MTRANS_POINTER(pRecv, pDBMsg, tagNDBC_UpdateRoleItemTransport);
	m_pDB->UpdateRoleItemTranport(pRecv->dwRoleID, (PVOID)pDBMsg);
	return E_Success;
}
VOID CLoong::SendMsg( PVOID pSend, DWORD dwSize )
{
	tagNetCmd* pSendCmd = (tagNetCmd*)pSend;
	if (dwSize > m_Status.dwMaxSendedMsgSize)
	{
		m_Status.dwMaxSendedMsgSize = dwSize;
		m_Status.dwMaxSendedMsgID = pSendCmd->dwID;
	}

	m_pServer->Send(ID_GAMESERVER, pSend, dwSize);
}