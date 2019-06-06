//-----------------------------------------------------------------------------
//!\file world_session.h
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

#include "StdAfx.h"
#include "Mutex.h"

//-----------------------------------------------------------------------------
// 服务器所能承受的最大在线人数（强制限制）
//-----------------------------------------------------------------------------
const INT MAX_PLAYER_NUM = 5000;

class PlayerSession;

class WorldSession
{
public:
	WorldSession();
	~WorldSession();

	//-------------------------------------------------------------------------
	// 初始化，更新及销毁
	//-------------------------------------------------------------------------
	BOOL			Init();
	VOID			Update();
	VOID			Destroy();

	BOOL			LoadGameConfig();

	//-------------------------------------------------------------------------
	// 各种Get
	//-------------------------------------------------------------------------
	DWORD			GetPort()				{ return m_nPort; }

	//-------------------------------------------------------------------------
	// 客户端连接相关
	//-------------------------------------------------------------------------
	PlayerSession*	FindSession(DWORD dwID);
	PlayerSession*	FindGlobalSession(DWORD dwID);
	BOOL			IsSessionExist(DWORD dwID);

	VOID			AddSession(PlayerSession* pSession);
	VOID			RemoveSession(UINT32 dwID);
	VOID			Kick(DWORD dwInternalIndex);

	VOID			AddGlobalSession(PlayerSession* pSession);
	VOID			RemoveGlobalSession(DWORD dwSessionID);

	VOID			UpdateSession();
	VOID			DoHouseKeeping();

	INT				GetAllOLAccountID(DWORD* pAccountIDs);

	//---------------------------------------------------------------------------
	// 消息相关
	//---------------------------------------------------------------------------
	__forceinline LPBYTE RecvMsg(DWORD& dwSize, INT& nMsgNum, DWORD dwInternalIndex)
	{
		return m_pNetSession->Recv(dwInternalIndex, dwSize, nMsgNum);
	}

	__forceinline VOID ReturnMsg(LPBYTE pMsg)
	{
		m_pNetSession->FreeRecved(pMsg);
	}

	__forceinline VOID SendMsg(DWORD dwInternalIndex, LPBYTE pMsg, DWORD dwSize)
	{
		InterlockedExchangeAdd((LPLONG)&m_nMsgSendThisTick, 1);
		m_pNetSession->Send(dwInternalIndex, pMsg, dwSize);
	}

	__forceinline INT GetSendCast(DWORD dwInternalIndex)
	{
		return m_pNetSession->GetSendCast(dwInternalIndex);
	}

	__forceinline INT GetSendCastSize(DWORD dwInternalIndex)
	{
		return m_pNetSession->GetSendCastSize(dwInternalIndex);
	}

	//---------------------------------------------------------------------------
	// 统计信息
	//---------------------------------------------------------------------------
	INT	GetPlayerNumLimit()				{ return m_nPlayerNumLimit; }
	INT GetPlayerNumCurrent()			{ return m_nPlayerNumCurrent; }
	INT GetPlayerNumDisplay()			{ INT display = (int)(m_nPlayerNumCurrent * m_lfPlayerNumFactor); return display>m_nPlayerNumLimit?m_nPlayerNumLimit:display;}
	INT GetPlayerNumPeek()				{ return m_nPlayerNumPeek; }
	INT GetPlayerLoginTimes()			{ return m_nPlayerIn; }
	INT GetPlayerLogoutTimes()			{ return m_nPlayerOut; }
	INT GetMsgSendCast()				{ return m_nSendCast; }
	INT GetMsgSendThisTick()			{ return m_nMsgSendThisTick; }
	INT GetMsgProceedThisTick()			{ return m_nMsgProceedThisTick; }
	INT GetMsgRecvWait()				{ return m_nMsgRecvWait; }

	INT64 GetTotalSendSize()			{ return m_pNetSession->GetSendSize(); }
	INT64 GetTotalRealSendSize()		{ return m_pNetSession->GetRealSendSize(); }
	INT GetSendComRatio();

	VOID SetMsgSendCast(INT nNum)			{ InterlockedExchange((LPLONG)&m_nSendCast, nNum); }
	VOID SetUnitSendThisTick(INT nNum)		{ InterlockedExchange((LPLONG)&m_nMsgSendThisTick, nNum); }
	VOID SetUnitProceedThisTick(INT nNum)	{ InterlockedExchange((LPLONG)&m_nMsgProceedThisTick, nNum); }
	VOID SetUnitRecvWait(INT nNum)			{ InterlockedExchange((LPLONG)&m_nMsgRecvWait, nNum); }

	VOID SetPlayerNumLimit(INT nPlayerLimit)
	{
		if( nPlayerLimit < 0 ) return;
		if( nPlayerLimit > MAX_PLAYER_NUM ) nPlayerLimit = MAX_PLAYER_NUM;

		InterlockedExchange((LPLONG)&m_nPlayerNumLimit, nPlayerLimit);
	}
	VOID PlayerLogin()
	{
		InterlockedExchangeAdd((LPLONG)&m_nPlayerNumCurrent, 1);
		InterlockedExchangeAdd((LPLONG)&m_nPlayerIn, 1);

		// 超过了峰值
		if( GetPlayerNumDisplay() > m_nPlayerNumPeek )
		{
			InterlockedExchange((LPLONG)&m_nPlayerNumPeek, GetPlayerNumDisplay());
		}
	}
	VOID PlayerLogout()
	{
		InterlockedExchangeAdd((LPLONG)&m_nPlayerNumCurrent, -1);
		InterlockedExchangeAdd((LPLONG)&m_nPlayerOut, 1);
	}

	VOID SetPlayNumberFactor(float f)
	{
		if( f <=0.0) return;
		m_lfPlayerNumFactor = f;

		// 超过了峰值
		if( GetPlayerNumDisplay() > m_nPlayerNumPeek )
		{
			InterlockedExchange((LPLONG)&m_nPlayerNumPeek, GetPlayerNumDisplay());
		}
	}

private:
	TSFPTrunk<WorldSession>				m_Trunk;

	TObjRef<Util>						m_pUtil;
	TObjRef<Thread>						m_pThread;

	TMap<DWORD, PlayerSession*>			m_mapAllSessions;		// 所有连接到服务器的客户端连接（帐号为主键）
	Mutex								m_AllSessionMutex;		// m_mapAllSessions的锁

	TMap<DWORD, PlayerSession*>			m_mapGlobalSessions;	// 所有还没有进入到游戏中的客户端连接（帐号为主键）
	Mutex								m_GlobalSessionMutex;	// m_mapGlobalSessions的锁

	TList<PlayerSession*>				m_listInsertPool;		// 要加入到Global Session中的Session
	Mutex								m_InsertPoolMutex;		// m_listInsertPool的锁

	TObjRef<XServer>					m_pNetSession;			// 完成端口
	INT									m_nPort;				// 监听端口
	double								m_lfPlayerNumFactor;	// 显示在线人数的因子

	volatile INT						m_nPlayerIn;			// 总玩家登入数量
	volatile INT						m_nPlayerOut;			// 总玩家登出数量

	volatile INT						m_nPlayerNumLimit;		// 服务器人数限制
	volatile INT						m_nPlayerNumCurrent;	// 当前在线人数

	volatile INT						m_nPlayerNumPeek;		// 峰值人数

	volatile INT						m_nSendCast;			// 网络底层还没有收到返回确认的发送消息数量
	volatile INT						m_nMsgSendThisTick;		// 网络底层本tick发送的消息数量
	volatile INT						m_nMsgProceedThisTick;	// 本Tick处理了多少消息
	volatile INT						m_nMsgRecvWait;			// 还有多少接收消息在等待处理

private:
	DWORD LoginCallBack(LPVOID, LPVOID);						// 客户端登陆本服务器回调函数
	DWORD LogoutCallBack(LPVOID);								// 客户端登出本服务器回调函数

private:
	volatile INT						m_nTreasureSum;			// 服务器总开启宝箱数
public:
	INT GetTreasureSum()				{ return m_nTreasureSum; }
	VOID IncTreasureSum();
	VOID SetTreasureSum(INT nSum);

public:
	INT GetMaxRecvWaitUnitNum() const { return m_nMaxRecvWaitUnitNum; }
	INT GetMaxSendCastUnitNum() const { return m_nMaxSendCastUnitNum; }
	INT GetMaxSendCastSize() const { return m_nMaxSendCastSize; }
private:
	INT m_nMaxRecvWaitUnitNum;								// 客户端等待处理的包的最大数量，超过数量将被踢掉
	INT m_nMaxSendCastUnitNum;								// 发送给客户端但还没有收到响应的包数量，超过数量将被踢掉
	INT m_nMaxSendCastSize;									// 发送给客户端但还没有收到相应的字节数量，超过即被踢掉
	
};

//--------------------------------------------------------------------------------------------
// 得到压缩率
//--------------------------------------------------------------------------------------------
inline INT WorldSession::GetSendComRatio()
{
	INT64 nSendSize = m_pNetSession->GetSendSize();
	INT64 nRealSendSize = m_pNetSession->GetRealSendSize();

	if( 0 >= nSendSize ) return 0;

	FLOAT fRatio = FLOAT(nSendSize - nRealSendSize) / FLOAT(nSendSize);

	return INT(fRatio * 100.0f);
}

extern WorldSession g_worldSession;