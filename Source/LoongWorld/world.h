//-----------------------------------------------------------------------------
//!\file World.h
//!\author Aslan
//!
//!\date 2008-06-10
//! last 2008-06-10
//!
//!\brief 区域服务器主框架类
//!
//!	Copyright (c) 2004 TENGWU Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include "mutex.h"

//-----------------------------------------------------------------------------
// 区域服务器主框架类
//-----------------------------------------------------------------------------
class World
{

public:
	World();
	~World();

	BOOL Init(HINSTANCE hInst);
	VOID Mainloop();
	VOID Destroy();

	BOOL	IsOverload() const			{ return m_bOverloadAlert; }
	DWORD	GetMaxTickTime() const		{ return m_dwMaxTickTime; }
	DWORD	GetIdleTime() const			{ return m_dwIdleTime; }
	DWORD	GetTotalRunMinute() const	{ return m_dwTotalRunMinute; }
	BYTE	GetSaveOneTicks() const		{ return m_bySaveOneNeedTicks; }
	DWORD	GetGoldenCode() const		{ return m_dwGoldenCode; }
	LPCTSTR	GetWorldName() const		{ return m_strWorldName.c_str(); }
	INT		GetCpuNum()	const			{ return m_nCpuNum; }
	DWORD	GetTotalMemory() const		{ return m_dwTotalPhys; }
	DWORD	GetAvailMemory() const		{ return m_dwAvailPhys; }
	INT		GetQuotaNonPagedPoolUsage()	{ return m_nQuotaNonPagedPoolUsage; }

	VOID	ShutDown()					{ InterlockedExchange((LONG*)&m_bShutingdown, TRUE); }
	BOOL	IsShutingdown() const		{ return m_bShutingdown; }

	VOID	SwitchGameGuarder(BOOL bOn);
	BOOL	IsWell() const;
	VOID	UpdateMemoryUsage();	

	DWORD	GetWorldTick()				{ return m_dwWorldTick; }
	VOID	SetWorldTickInc()			{ InterlockedExchangeAdd((LPLONG)&m_dwWorldTick, 1); }

	DWORD	GetMaxMsgSize()				{ return m_dwMaxMsgSize; }
	VOID	SetMaxMsgSize(DWORD dwMsgSize)	{ InterlockedExchange((LONG*)&m_dwMaxMsgSize, dwMsgSize); }

	tagDWORDTime GetWorldTime()			{ return m_CurrentTime; }
	VOID	UpdateWorldTime()			{ m_CurrentTime = GetCurrentDWORDTime(); }

	VOID	LockNetWork()				{ m_MutexNetwork.Acquire(); }
	VOID	UnlockNetWork()				{ m_MutexNetwork.Release(); }

public:
	FLOAT	GetLaxCheatDistanceSQ()		{ return m_fLaxCheatDistanceSQ; }
	FLOAT	GetStrictCheatDistanceSQ()	{ return m_fStrictCheatDistanceSQ; }

	TObjRef<Util>&		GetUtil()			{ return m_pUtil; }
	TObjRef<FastCode>&	GetFastCode()		{ return m_pFastCode; }
	TObjRef<Log>&		GetLog()			{ return m_pLog; }

public:
	// crc32方法封装 -- 先转换为小写再计算
	DWORD	LowerCrc32(LPCTSTR str);

private:
	// 保存所有游戏世界其他数据
	VOID	SaveWorldInfoToDB();
	VOID	WaitDBTerminate();
private:
	TSFPTrunk<World>		m_Trunk;

	TObjRef<Thread>			m_pThread;
	TObjRef<GDIRender>		m_pRender;
	TObjRef<WindowWrap>		m_pWindowWrap;
	TObjRef<GUISystem>		m_pGUI;
	TObjRef<Util>			m_pUtil;
	TObjRef<FastCode>		m_pFastCode;
	TObjRef<Console>		m_pConsole;
	TObjRef<Log>			m_pLog;

	IGUIRenderNative*		m_pGUIRender;

	volatile BOOL			m_bShutingdown;						// 服务器是否正在关闭
	volatile BOOL			m_bTerminateUpdate;					// 服务器线程停止位
	volatile BOOL			m_bOverloadAlert;					// 服务器超载警报
	volatile DWORD			m_dwIdleTime;						// 上次服务器主循环空闲时间
	volatile DWORD			m_dwMaxTickTime;					// 最大的Tick时间
	volatile BYTE			m_bySaveOneNeedTicks;				// 多少Tick保存一个玩家

	volatile DWORD			m_dwMaxMsgSize;

	DWORD					m_dwViewWidth;
	DWORD					m_dwViewHeight;

	INT						m_nCpuNum;
	DWORD					m_dwTotalPhys;
	DWORD					m_dwAvailPhys;
	INT						m_nQuotaNonPagedPoolUsage;

	tstring					m_strWorldName;						// 游戏世界名称
	DWORD					m_dwGoldenCode;						// 区域服务器的golden_code

	DWORD					m_dwLastMinute;
	DWORD					m_dwLastHour;
	DWORD					m_dwTotalRunMinute;
	volatile DWORD			m_dwWorldTick;						// 游戏世界心跳
	tagDWORDTime			m_CurrentTime;						// 当前系统时间
	DWORD					m_dwLastSaveTick;					// 上一次保存玩家记录心跳

	FLOAT					m_fLaxCheatDistanceSQ;				// 弱检测情况下的距离平方
	FLOAT					m_fStrictCheatDistanceSQ;			// 强检测情况下的距离平方

	Mutex					m_MutexNetwork;						// 网络层锁，用于与底层的logincallback和logoutcallback进行互斥

private:
	// Update线程
	DWORD ThreadUpdate(LPVOID);
	// 记录服务器主线调用堆栈信息
//	DWORD RecordStack(LPCSTR);
	// 修改多少个tick保存一个玩家数据
	DWORD ChangeSaveTicks(LPCSTR szTicks);
	// 每分钟执行一次
	VOID OnMinute();
	// 每小时执行一次
	VOID OnHour();
	// 每整点执行一次
	VOID OnClock(BYTE byHour);
	// 保存玩家信息
	VOID SaveRolePerTick();
};

extern World g_world;

#define	IUTIL		(g_world.GetUtil())
#define IFASTCODE	(g_world.GetFastCode())
#define ILOG		(g_world.GetLog())