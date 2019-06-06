//-----------------------------------------------------------------------------
//!\file bill_server.h
//!\author Aslan
//!
//!\date 2009-06-09
//! last 2009-06-09
//!
//!\brief 直充划拨服务器
//!
//!	Copyright (c) 2004 TENGWU Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

//-----------------------------------------------------------------------------
// 直冲划拨服务器
//-----------------------------------------------------------------------------
class BillServer
{
public:
	//-------------------------------------------------------------------------
	// CONSTRUCT
	//-------------------------------------------------------------------------
	BillServer() : m_Trunk(this), m_pGUIRender(NULL), m_dwSectionID(GT_INVALID),
					m_dwViewWidth(0), m_dwViewHeight(0), m_nCPUNum(0), m_dwTotalPhys(0), m_dwAvailPhys(0),
					m_nQuotaNonPagedPoolUsage(0), m_nTick(0), m_bIsInitOK(FALSE), m_bIsShutingDown(FALSE) {}
	~BillServer() {}

	//-------------------------------------------------------------------------
	// 初始化，主循环和销毁
	//-------------------------------------------------------------------------
	BOOL		Init(HINSTANCE hInst);
	VOID		MainLoop();
	VOID		Destroy();

	//-------------------------------------------------------------------------
	// 开启和关闭
	//-------------------------------------------------------------------------
	BOOL		IsInitOK()					{ return m_bIsInitOK; }
	BOOL		IsShutingDown()				{ return m_bIsShutingDown; }
	VOID		InitOK()					{ InterlockedExchange((LPLONG)&m_bIsInitOK, TRUE); }
	VOID		ShutDown()					{ InterlockedExchange((LPLONG)&m_bIsShutingDown, TRUE); }

	//-------------------------------------------------------------------------
	// 各种Get
	//-------------------------------------------------------------------------
	DWORD		GetSectionID()				{ return m_dwSectionID; }
	DWORD		GetTick()					{ return m_nTick++; }
	INT			GetCPUNum()					{ return m_nCPUNum; }
	DWORD		GetTotalPhyMem()			{ return m_dwTotalPhys; }
	DWORD		GetAvailPhyMem()			{ return m_dwAvailPhys; }
	INT			GetNonPagedPoolMem()		{ return m_nQuotaNonPagedPoolUsage; }

	//-------------------------------------------------------------------------
	// 辅助
	//-------------------------------------------------------------------------
	TObjRef<Util>	GetUtil()				{ return m_pUtil; }
	TObjRef<Log>	GetLog()				{ return m_pLog; }

private:
	//-------------------------------------------------------------------------
	// 更新线程
	//-------------------------------------------------------------------------
	VOID		ThreadUpdate();

	//-------------------------------------------------------------------------
	// 更新内存占用
	//-------------------------------------------------------------------------
	VOID		UpdateMemoryUsage();

	//-------------------------------------------------------------------------
	// 显示运行时信息
	//-------------------------------------------------------------------------
	VOID		DisplayRunTimeInfo();

private:
	
	TSFPTrunk<BillServer>	m_Trunk;

	TObjRef<GDIRender>		m_pRender;
	TObjRef<WindowWrap>		m_pWindowWrap;
	TObjRef<GUISystem>		m_pGUI;
	TObjRef<Util>			m_pUtil;
	TObjRef<Console>		m_pConsole;
	TObjRef<Log>			m_pLog;

	IGUIRenderNative*		m_pGUIRender;

	//--------------------------------------------------------------------------
	// 窗口大小
	//--------------------------------------------------------------------------
	DWORD					m_dwViewWidth;
	DWORD					m_dwViewHeight;

	//--------------------------------------------------------------------------
	// 状态
	//--------------------------------------------------------------------------
	volatile BOOL			m_bIsInitOK;						// 初始化成功
	volatile BOOL			m_bIsShutingDown;					// 正在关闭

	//--------------------------------------------------------------------------
	// 大区ID
	//--------------------------------------------------------------------------
	DWORD					m_dwSectionID;						// 大区ID

	//--------------------------------------------------------------------------
	// 统计信息
	//--------------------------------------------------------------------------
	INT						m_nCPUNum;							// CPU个数
	DWORD					m_dwTotalPhys;						// 总共的物理内存
	DWORD					m_dwAvailPhys;						// 总共的物理内存
	INT						m_nQuotaNonPagedPoolUsage;			// 非页面缓冲内存
	volatile DWORD			m_nTick;							// 心跳数
};

extern BillServer g_Server;					// 全局对象

#define IUTIL	(g_Server.GetUtil())
#define ILOG	(g_Server.GetLog())