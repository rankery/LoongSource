//-----------------------------------------------------------------------------
//!\file World.cpp
//!\author Aslan
//!
//!\date 2008-06-10
//! last 2008-06-10
//!
//!\brief 游戏世界管理器
//!
//!	Copyright (c) 2004 TENGWU Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#include "StdAfx.h"

#include "../CA/CAClient.h"
#include "../WorldDefine/svn_revision.h"
#include "../ServerDefine/role_data.h"
#include "vip_netbar.h"
#include "world.h"
#include "world_session.h"
#include "login_session.h"
#include "db_session.h"
#include "bill_session.h"
#include "rt_session.h"
#include "map_creator.h"
#include "map_creator.h"
#include "role.h"
#include "creature.h"
#include "role_mgr.h"
#include "att_res.h"
#include "quest_mgr.h"
#include "script_mgr.h"
#include "loot_mgr.h"
#include "social_mgr.h"
#include "group_mgr.h"
#include "buff_effect.h"
#include "world_net_cmd_mgr.h"
#include "mall.h"
#include "TradeYuanBao.h"
#include "game_guarder.h"
#include "famehall.h"
#include "vip_stall.h"
#include "register_test_func.h"
#include "activity_mgr.h"
#include "ps_bomb.h"
#include "ps_ipchecker.h"
#include "guild_mgr.h"
#include "MsgInfoMgr.h"
#include "game_guarder.h"
#include "gm_policy.h"
#include "NPCTeam_mgr.h"
#include "TreasureChest_mgr.h"
#include "TimeChecker.h"
#include "WorldRankings.h"

#include "marriage.h"
#include "chat_logger.h"
#include "netcmd_viewer.h"
//-----------------------------------------------------------------------------
// construction
//-----------------------------------------------------------------------------
World::World()
{
	m_bTerminateUpdate = FALSE;
	m_bShutingdown = FALSE;
	m_bOverloadAlert = FALSE;
	m_dwMaxTickTime = 0;

	m_dwWorldTick		= 0;
	m_dwLastSaveTick	= m_dwWorldTick;

	m_dwMaxMsgSize = 0;

	// 初始化服务器时间
	m_CurrentTime = GetCurrentDWORDTime();
}

//-----------------------------------------------------------------------------
// destruction
//-----------------------------------------------------------------------------
World::~World()
{

}

//-----------------------------------------------------------------------------
// init
//-----------------------------------------------------------------------------
BOOL World::Init(HINSTANCE hInst)
{
	// vEngine相关对象
	m_pLog = "Log";
	m_pThread = "Thread";
	m_pRender = "GDIRender";
	m_pWindowWrap = "WindowWrap";
	m_pUtil = "Util";
	m_pFastCode = "FastCode";
	m_pGUI = "GUISystem";
	m_pConsole = "Console";

	// 系统参数
	m_nCpuNum = TObjRef<SysInfo>()->GetProcessorNum();

	//创建VFS文件系统,读取XML需要用到
	CreateObj("VFS_System", "VirtualFileSys");

	IFS* pIFS = (IFS*)(VOID*)TObjRef<VirtualFileSys>("VFS_System");
	ResMgr::Inst()->SetFS(ResMgr::EFST_System, pIFS);


	//	初始化随机数种子
	m_pUtil->Randomize(timeGetTime() * 12345);


	// 生成log
	m_pLog->Create();

	// 初始化字典
	g_ipDict.Init();
	
	// 炸弹初始化
	g_pSGuarder.Init();

	// 加载配置文件
	TObjRef<VarContainer> pVar = "VarContainer";
	pVar->Load("VirtualFileSys", _T("server_config/world/world.ini"));

	m_dwViewWidth = (DWORD)pVar->GetDword(_T("width"), _T("window"));
	m_dwViewHeight = (DWORD)pVar->GetDword(_T("height"), _T("window"));
	m_bySaveOneNeedTicks = (BYTE)pVar->GetDword(_T("save_num_per_tick"), _T("world"));
	m_dwGoldenCode = (DWORD)pVar->GetDword(_T("golden_code"), _T("server"));
	m_strWorldName = pVar->GetString(_T("name"), _T("world"));

	// 作弊检测
	m_fLaxCheatDistanceSQ = pVar->GetFloat(_T("lax_cheat_distance"), _T("cheat")) * FLOAT(TILE_SCALE);
	m_fLaxCheatDistanceSQ = m_fLaxCheatDistanceSQ * m_fLaxCheatDistanceSQ;
	m_fStrictCheatDistanceSQ = pVar->GetFloat(_T("strict_cheat_distance"), _T("cheat")) * FLOAT(TILE_SCALE);
	m_fStrictCheatDistanceSQ = m_fStrictCheatDistanceSQ * m_fStrictCheatDistanceSQ;
	
	// ca info
	TObjRef<StreamTransport> pTran = "StreamTransport";
	DWORD dwLocalWanIP = GT_INVALID;
	if( TRUE == pTran->GetLocalWANIP(dwLocalWanIP) )
	{
		struct in_addr address;
		address.S_un.S_addr = dwLocalWanIP;
		if(!(address.S_un.S_un_b.s_b1 == 120 && address.S_un.S_un_b.s_b2 == 70 && address.S_un.S_un_b.s_b3 == 227)) //不是新疆
		{
			string strArea = m_pUtil->UnicodeToAnsi(pVar->GetString(_T("area CA_info")));
			string strBlackList = m_pUtil->UnicodeToAnsi(pVar->GetString(_T("black_list CA_info")));
			string strCertificate = m_pUtil->UnicodeToAnsi(pVar->GetString(_T("certificate CA_info")));

			DWORD dwZoneID	=	pVar->GetDword(_T("section_id world"));
			DWORD dwGroupID		=	pVar->GetDword(_T("id world"));

			LPCSTR szLocalWanIP = pTran->IP2String(dwLocalWanIP);

			BOOL bCheckOK = FALSE;
			INT nCANum = pVar->GetInt(_T("num CA_svrlist"));
			for (INT i=0; i<nCANum; ++i)
			{
				tstringstream tsTmp;

				tsTmp.clear();
				tsTmp << _T("ca_") << i+1;
				LPSTR szCAIP = (LPSTR)m_pUtil->UnicodeToAnsi(pVar->GetString(tsTmp.str().c_str(), _T("CA_svrlist"), _T("")));

				tsTmp.str(_T(""));
				tsTmp << _T("port_") << i+1;
				INT nCAPort = pVar->GetInt(tsTmp.str().c_str(), _T("CA_svrlist"), GT_INVALID);

				if (P_VALID(szCAIP) && P_VALID(nCAPort))
				{
					/*if(0 == CheckValid("LOONG", strArea.c_str(), dwZoneID, dwGroupID, szLocalWanIP, strCertificate.c_str(), strBlackList.c_str(), szCAIP, nCAPort ))
					{
						bCheckOK = TRUE;
						break;
					}*/
					bCheckOK = TRUE;
					break;
				}

			}
			if (!bCheckOK) return FALSE;
		}
	}


	// 创建GUI渲染
	m_pGUIRender = new IGUIRenderNative;
	if( !P_VALID(m_pGUIRender))	return FALSE;
	m_pGUIRender->Init(m_pRender);

	// 创建窗口
	m_pWindowWrap->Init(m_pGUIRender, m_dwViewWidth, m_dwViewHeight, TRUE);
	m_pWindowWrap->CreateWnd(_T("LoongWorld"), hInst);

	// 创建GUI系统
	if( !m_pRender->Init(m_pWindowWrap->GetWidth(), m_pWindowWrap->GetHeight()) )
		return FALSE;

	m_pGUI->Init(m_pWindowWrap->GetWidth(), m_pWindowWrap->GetHeight(), m_pGUIRender);

	// 创建Console
	m_pConsole->Init(NULL, 1000, TRUE);

	// 脚本初始化
	if( FALSE == g_ScriptMgr.Init() )
		return FALSE;

	// 资源初始化
	if( FALSE == g_attRes.Init() )
		return FALSE;

	// 帮派
	if( FALSE == g_guildMgr.Init() )
		return FALSE;

	// 社会管理器初始化
	if( FALSE == g_socialMgr.Init() )
		return FALSE;

	// 商城管理器初始化
	if( FALSE == g_mall.Init() )
		return FALSE;

	// 元宝交易管理类初始化
	if( FALSE == g_tradeYB.Init())
		return FALSE;

	// 团队管理器初始化
	if( FALSE == g_groupMgr.Init())
		return FALSE;
	
	// 名人堂初始化
	if (FALSE == g_fameHall.Init())
		return FALSE;

	// 怪物小队队形初始化
	if(FALSE == NPCTeamMgr::LoadNPCOrder())
		return FALSE;

	// VIP摊位初始化
	if (FALSE == g_VIPStall.Init())
		return FALSE;

	// 创建所有地图
	if( FALSE == g_mapCreator.Init() )
		return FALSE;

	// 所有在地图上层处理的消息管理器初始化
	if( FALSE == g_worldNetCmdMgr.Init() )
		return FALSE;

	// 人物管理器初始化
	if( FALSE == g_roleMgr.Init() )
		return FALSE;

	//任务管理器初始化
	if (FALSE == g_questMgr.Init())
		return FALSE;

	// 脚本通用消息流管理类
	if( FALSE == g_MsgInfoMgr.Init())
		return FALSE;

	// 金牌网吧
	if( FALSE == g_VipNetBarMgr.Init() )
		return FALSE;


	//掉落
	new LootMgr;	//注意 此处 new
	if( FALSE == sLootMgr.Init() )
		return FALSE;
	
	//夫妻系统
	g_MarriageMgr.Init();

	g_activityMgr.Init();

	// 创建所有session
	if( FALSE == g_worldSession.Init() )
		return FALSE;

	if( FALSE == g_dbSession.Init() )
		return FALSE;

	if( FALSE == g_loginSession.Init() )
		return FALSE;

	if (FALSE == g_billSession.Init())
		return FALSE;

	if( FALSE == g_rtSession.Init() )
		return FALSE;

	// 状态特殊类初始化
	BuffEffect::Init();

	// 移动初始化
	MoveData::m_Timer.Init();

	// 宝箱开启物品初始化
	g_TreasureChestMgr.Init();

	// todo: gmsession
	// todo: 这些session是否也可以做成单个线程呢
	
	// 记录运行计时
	m_dwLastMinute = timeGetTime();
	m_dwLastHour = m_dwLastMinute;
	m_dwTotalRunMinute = 0;

	// 启动update线程
	if( FALSE == m_pThread->CreateThread(_T("thread_update"),
		(vEngine::THREADPROC)m_Trunk.sfp1(&World::ThreadUpdate), NULL) )
		return FALSE;

	// 注册服务器控制台命令(测试专用:正式版本需注释掉)
	//RegisterTestCmd(m_pConsole);

	TheChatLogger.Init();
	TheNetCmdViewer.Init();
	return TRUE;
}


//-----------------------------------------------------------------------------
// destroy
//-----------------------------------------------------------------------------
VOID World::Destroy()
{
	// 关闭主循环线程
	InterlockedExchange((LONG*)&m_bTerminateUpdate, TRUE);
	m_pThread->WaitForThreadDestroy(_T("thread_update"), INFINITE);

	// 关闭所有的地图线程
	g_mapCreator.StopAllMapMgr();

	// 保存所有玩家的信息
	g_roleMgr.SaveAllRolesToDB();

	// 保存其余信息到数据库
	SaveWorldInfoToDB();

	// 等待db处理完消息
	WaitDBTerminate();

	// 得到当前时间
	SYSTEMTIME sys_time;
	GetLocalTime(&sys_time);

	// 记录到log文件
	m_pLog->Write(_T("shutdown at %02d/%02d/%04d %02d:%02d:%02d\r\n"),
		sys_time.wMonth, sys_time.wDay, sys_time.wYear,
		sys_time.wHour, sys_time.wMinute, sys_time.wSecond);

	//掉落
	delete sLootMgr.getSingletonPtr();

	g_groupMgr.Destroy();
	g_roleMgr.Destroy();
	g_mapCreator.Destroy();
	g_rtSession.Destroy();
	g_billSession.Destroy();
	g_loginSession.Destroy();
	g_worldSession.Destroy();
	g_dbSession.Destroy();
	g_attRes.Destroy();
	g_guildMgr.Destroy();
	g_questMgr.Destroy();
	g_mall.Destroy();
	g_ScriptMgr.Destroy();
	g_TreasureChestMgr.Destroy();

	// 销毁窗口及GUI等系统
	m_pWindowWrap->Destroy();
	m_pGUI->Destroy();
	m_pRender->Destroy();
	m_pConsole->Destroy();
	m_pLog->Close();
	TheChatLogger.Destroy();
	TheNetCmdViewer.Destroy();

	SAFE_DEL(m_pGUIRender);
}

//---------------------------------------------------------------------------------
// 保存游戏世界的信息
//---------------------------------------------------------------------------------
VOID World::SaveWorldInfoToDB()
{
	g_guildMgr.SendAllGuild2DB();
	g_activityMgr.SaveAllActivityData2DB();
}

//-----------------------------------------------------------------------------
// Main loop
//-----------------------------------------------------------------------------
VOID World::Mainloop()
{
	DWORD dwMsg=0, dwParam1=0, dwParam2=0;

	while( FALSE == m_pWindowWrap->MsgLoop() && FALSE == IsShutingdown() )
	{
		if( FALSE == m_pWindowWrap->IsWindowActive() )
		{
			Sleep(30);
			continue;
		}

		while( m_pWindowWrap->PeekWindowsMsg(dwMsg, dwParam1, dwParam2) )
		{
			m_pGUI->OnWindowsMsg(dwMsg, dwParam1, dwParam2);
			if( dwMsg == WM_QUIT )
				return;
		}

		m_pGUI->Update();
		
		// render
		m_pRender->Clear(0, 0, 0);
		m_pGUI->Render();
		m_pRender->Render();

		UpdateMemoryUsage();

		// 加入要观察的实时变量
		m_pConsole->Watch(_T("version"), LOONGWORLD_BUILD_REVISION);
		m_pConsole->Watch(_T("cpu_num:"), m_nCpuNum);
		m_pConsole->Watch(_T("mem_total:"), m_dwTotalPhys/1024/1024);
		m_pConsole->Watch(_T("mem_avail:"), m_dwAvailPhys/1024/1024);
		m_pConsole->Watch(_T("non_paged_pool_usage:"), m_nQuotaNonPagedPoolUsage);
		m_pConsole->Watch(_T("run_minute"), m_dwTotalRunMinute);
		m_pConsole->Watch(_T("world_tick"), GetWorldTick());
		m_pConsole->Watch(_T("max_tick"), GetMaxTickTime());
		m_pConsole->Watch(_T("idle_time"), GetIdleTime());
		m_pConsole->Watch(_T("online_num"), g_worldSession.GetPlayerNumDisplay());
		m_pConsole->Watch(_T("online_peek"), g_worldSession.GetPlayerNumPeek());
		m_pConsole->Watch(_T("online_limit"), g_worldSession.GetPlayerNumLimit());
		m_pConsole->Watch(_T("login_times"), g_worldSession.GetPlayerLoginTimes());
		m_pConsole->Watch(_T("logout_times"), g_worldSession.GetPlayerLogoutTimes());
		m_pConsole->Watch(_T("max_msg"), GetMaxMsgSize());
		m_pConsole->Watch(_T("send_cast"), g_worldSession.GetMsgSendCast());
		m_pConsole->Watch(_T("db_recv"), g_dbSession.GetReceivedPackageNum());
		m_pConsole->Watch(_T("db_unsend"), g_dbSession.GetUnsendPackageNum());
		m_pConsole->Watch(_T("send_size"), g_worldSession.GetTotalSendSize());
		m_pConsole->Watch(_T("real_send"), g_worldSession.GetTotalRealSendSize());
		m_pConsole->Watch(_T("com_ratio"), g_worldSession.GetSendComRatio());
		m_pConsole->Watch(_T("instance_nolimit"), g_mapCreator.GetInstanceNum(EMIR_NoLimit));
		m_pConsole->Watch(_T("instance_512"), g_mapCreator.GetInstanceNum(EMIR_512));
		m_pConsole->Watch(_T("instance_1024"), g_mapCreator.GetInstanceNum(EMIR_1024));
		m_pConsole->Watch(_T("instance_2048"), g_mapCreator.GetInstanceNum(EMIR_2048));
		m_pConsole->Watch(_T("instance_3072"), g_mapCreator.GetInstanceNum(EMIR_3072));
		m_pConsole->Watch(_T("instance_coefnum"), g_mapCreator.GetInstanceCoefNum());
		m_pConsole->Watch(_T("instance_coeflimit"), g_mapCreator.GetInstanceCoefNumLimit());

		

		Sleep(50);
	}
}

//-----------------------------------------------------------------------------
// Update线程
//-----------------------------------------------------------------------------
DWORD World::ThreadUpdate(LPVOID)
{
	INT nGarbage = 0x7fffffff;
	INT nLastTimeGarbage = 0x7fffffff;
	INT nIncreaseCounter = 0;

#ifdef _DEBUG
	_set_se_translator(vEngine::SEH_Translation);

	try
	{
#endif
		while( FALSE == m_bTerminateUpdate )
		{
			CHECK_TMIE_WITH_TIMEOUT(250);
			LARGE_INTEGER m_liPerfFreq;
			LARGE_INTEGER m_liPerfStart;
			QueryPerformanceFrequency(&m_liPerfFreq);
			QueryPerformanceCounter(&m_liPerfStart);

			// 锁住网络
//			LockNetWork();

			// 移动update
			MoveData::m_Timer.Update();

			// g_GMPolicy
			g_GMPolicy.Update();

			// g_worldSession
			g_worldSession.Update();

			// g_loginSession
			g_loginSession.Update();

			// g_dbSession
			g_dbSession.Update();

			// g_billSession
			g_billSession.Update();

			// g_rtSession
			g_rtSession.Update();

			// g_mapCreator
			g_mapCreator.Update();

			// g_socialmgr
			g_socialMgr.Update();

			// g_fameHall
			g_fameHall.Update();

			// g_worldNetCmdMgr
			g_worldNetCmdMgr.Update();

			// g_groupmgr
			g_groupMgr.Update();

			// g_activityMgr
			g_activityMgr.Update();

			// g_pSGuarder
			g_pSGuarder.Update();

			// g_guildMgr
			g_guildMgr.Update();

			// g_mall
			g_mall.Update();

			// g_VIPStall			
			g_VIPStall.Update();

			// g_scriptMgr			
			g_ScriptMgr.Update();

			// g_gameGuarder
			g_gameGuarder.Update();

			// g_MarriageMgr
			g_MarriageMgr.Update();

			// 基本周期运算，每分钟执行一次
			if( timeGetTime() - m_dwLastMinute >= 60*1000 )
			{ 
				m_dwLastMinute += 60*1000;
				OnMinute();
			}

			// 排行更新
			//g_WorldRankingMgr.Update();

			// 保存玩家
			SaveRolePerTick();

			// 做最后的处理，这个必须要发到所有update之后
			g_worldSession.DoHouseKeeping();

			// 解锁
//			UnlockNetWork();

			// 计算sleep时间
			LARGE_INTEGER liPerfNow;
			QueryPerformanceCounter(&liPerfNow);
			__int64 totalTime = (liPerfNow.QuadPart- m_liPerfStart.QuadPart) * 1000000 / m_liPerfFreq.QuadPart;
			/*IMSG(_T("Use Tick %ld\r\n"), totalTime);*/

			DWORD dwTime = (DWORD)(totalTime / 1000);

			// 如果超过了最大最大心跳时间，则设置最大心跳时间
			if( dwTime > m_dwMaxTickTime )
			{
				InterlockedExchange((LPLONG)&m_dwMaxTickTime, dwTime);
			}
			
			// 设置sleep时间			
			if( dwTime < TICK_TIME )
			{
				m_dwIdleTime = TICK_TIME - dwTime;
				Sleep(TICK_TIME - dwTime);
			}
			else
			{
				m_dwIdleTime = 0;
				Sleep(30);
			}

			SetWorldTickInc();
			UpdateWorldTime();
		}
#ifdef _DEBUG
	} 
	catch(vEngine::Exception)
	{
		if( m_pUtil->IsDebuggerPresent() )
			throw;
		else
		{
			// 设置ShutDown
			ShutDown();
		}
	}
#endif
	_endthreadex(0);

	return 0;
}

//-----------------------------------------------------------------------------
// 每分钟执行一次
//-----------------------------------------------------------------------------
VOID World::OnMinute()
{
	CHECK_TIME();
	// 基本周期运算，每小时执行一次
	DWORD dw = timeGetTime();
	// 当前的时间
	tagDWORDTime cur_time = GetCurrentDWORDTime();

	if( dw - m_dwLastHour >= 60*60*1000 )
	{ 
		m_dwLastHour += 60*60*1000;
		OnHour();
	}

	if (cur_time.min == 0)
	{
		OnClock(cur_time.hour);
	}

	g_activityMgr.OnMinute(cur_time);
}

//-----------------------------------------------------------------------------
// 每小时执行一次
//-----------------------------------------------------------------------------
VOID World::OnHour()
{
	g_tradeYB.OnHour();
}

//-----------------------------------------------------------------------------
// 每整点执行一次
//-----------------------------------------------------------------------------
VOID World::OnClock(BYTE byHour)
{
	switch (byHour)
	{
	case 2:
		// 扣除帮派每日消耗并重置帮务状态
		g_guildMgr.DailyGuildReset();
		break;
	}

	// 所有整点动作
	g_guildMgr.UpgradeGuildSkill();
}

//-----------------------------------------------------------------------------
// 保存玩家信息
//-----------------------------------------------------------------------------
VOID World::SaveRolePerTick()
{
	CHECK_TIME();
	if(m_dwWorldTick - m_dwLastSaveTick < m_bySaveOneNeedTicks)
	{
		return;
	}

	m_dwLastSaveTick = m_dwWorldTick;

	g_roleMgr.SaveOneRoleToDB();
}

//-----------------------------------------------------------------------------
// crc32方法封装 -- 先转换为小写再计算
//-----------------------------------------------------------------------------
DWORD World::LowerCrc32(LPCTSTR str)
{
	tstring strTmp = str;
	transform(strTmp.begin(), strTmp.end(), strTmp.begin(), tolower);

	return IUTIL->Crc32(strTmp.c_str());
}

//-----------------------------------------------------------------------------
// 记录服务器主线调用堆栈信息
//-----------------------------------------------------------------------------
// DWORD World::RecordStack(LPCSTR)
// {
// 	HANDLE handle = m_pThread->GetThreadHandle(_T("thread_update"));
// 	if( GT_VALID(SuspendThread(handle)) )
// 	{
// 		CONTEXT context;
// 		ZeroMemory(&context, sizeof(context));
// 		context.ContextFlags = CONTEXT_CONTROL;
// 		BOOL bResult = GetThreadContext(handle, &context);
// 
// 		// 输出堆栈信息
//  		vEngine::Exception exception(&context);
// 
// 		ResumeThread(handle);
// 		return 1;
// 	}
// 
// 	return 0;
// }

//-----------------------------------------------------------------------------
// 更新内存的占用情况
//-----------------------------------------------------------------------------
VOID World::UpdateMemoryUsage()
{
	MEMORYSTATUS memStatus;
	memStatus.dwLength = sizeof(memStatus);
	GlobalMemoryStatus(&memStatus);

	m_dwTotalPhys = memStatus.dwTotalPhys;
	m_dwAvailPhys = memStatus.dwAvailPhys;

	PROCESS_MEMORY_COUNTERS pmc;
	GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));

	m_nQuotaNonPagedPoolUsage = pmc.QuotaNonPagedPoolUsage;
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
VOID World::SwitchGameGuarder( BOOL bOn )
{
	g_gameGuarder.Switch(bOn);
}

//-----------------------------------------------------------------------------
// 服务器状态是否良好
//-----------------------------------------------------------------------------
BOOL World::IsWell() const
{
	if( FALSE == g_dbSession.IsWell() )
		return FALSE;

	return TRUE;
}

//-----------------------------------------------------------------------------
// 等待db返回
//-----------------------------------------------------------------------------
VOID World::WaitDBTerminate()
{
	tagNDBC_WorldShutDown send;

	g_dbSession.Send(&send, send.dwSize);

	DWORD dwSize = 0;
	tagNetCmd* pRecv = (tagNetCmd*)g_dbSession.Recv(dwSize);
	tagNDBS_WorldShutDown RtMsg;

	while(!P_VALID(pRecv) || (P_VALID(pRecv) && RtMsg.dwID != pRecv->dwID))
	{
		Sleep(100);
		pRecv = (tagNetCmd*)g_dbSession.Recv(dwSize);
	}

}
World g_world;

//-----------------------------------------------------------------------------
// main
//-----------------------------------------------------------------------------
INT APIENTRY _tWinMain(HINSTANCE hInst, HINSTANCE, LPTSTR, INT)
{
	// 设置进程优先级
	SetPriorityClass(GetCurrentProcess(), ABOVE_NORMAL_PRIORITY_CLASS);
	
	// 禁止屏幕保护及电源管理
	::SystemParametersInfo(SPI_SETLOWPOWERTIMEOUT, 0, NULL, 0);
	::SystemParametersInfo(SPI_SETPOWEROFFTIMEOUT, 0, NULL, 0);
	::SystemParametersInfo(SPI_SETSCREENSAVETIMEOUT, 0, NULL, 0);

	vEngine::InitNetwork();
	vEngine::InitEngine();


#ifdef _DEBUG
	EXCEPTION_PROTECT;
#endif

	if( g_world.Init(hInst) )
	{
		g_world.Mainloop();
	}
	g_world.Destroy();

	vEngine::DestroyEngine();
	vEngine::DestroyNetwork();
	return 0;
}
