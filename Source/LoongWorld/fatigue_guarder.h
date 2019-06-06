//-----------------------------------------------------------------------------
//!\file fatigue_guarder.h
//!\author xlguo
//!
//!\date 2009-02-05
//! last 
//!
//!\brief 防沉迷系统
//!
//!	Copyright (c) 2004 TENGWU Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include "..\WorldDefine\time.h"
#include "..\WorldDefine\base_define.h"
#include "..\WorldDefine\msg_fatigue_guarder.h"
#include "..\ServerDefine\fatigue_define.h"

//-----------------------------------------------------------------------------
// 沉迷防卫器
//-----------------------------------------------------------------------------
class FatigueGuarder
{
	enum
	{
		NUM_POINTS_NOTIFY = 8,		// 通知客户端时间点的个数
	};

	//-----------------------------------------------------------------------------
	// 状态结构
	//-----------------------------------------------------------------------------
	struct tagStateRangeRate
	{
		DWORD	dwBegin;
		DWORD	dwEnd;
		DWORD	dwEarnRate;
	};

public:
	FatigueGuarder(PlayerSession* pPlayerSession, BOOL bGuard = TRUE, DWORD dwAccOLAcc = 0)
		:m_bGuard(bGuard), m_dwAccOLMin(dwAccOLAcc), m_dwState(0), m_pPlayerSession(pPlayerSession)
	{
		ASSERT( P_VALID(m_pPlayerSession) );
	}

	BOOL IsGuard()	const {return m_bGuard;}
	//-----------------------------------------------------------------------------
	// 更新
	//-----------------------------------------------------------------------------
	VOID SetAccOLTimeMin(DWORD dwState, DWORD dwAccOLTimeMin);

	//-----------------------------------------------------------------------------
	// 收益率
	//-----------------------------------------------------------------------------
	DWORD GetEarnRate()	const 
	{	
		if ( !m_bGuard ) return 10000;
		ASSERT( EFS_VALID(m_dwState) );
		return m_arrStateRangeRate[m_dwState].dwEarnRate; 
	}

	//-----------------------------------------------------------------------------
	// 通知客户端
	//-----------------------------------------------------------------------------
	void NotifyClient() const ;
private:
	//-----------------------------------------------------------------------------
	// 获得状态
	//-----------------------------------------------------------------------------
	static EnumFatigueState GetState(const DWORD& dwAccOLMin)
	{
		for (INT i = EFS_BEGIN; i < EFS_END; ++i)
		{
			if ( dwAccOLMin >= m_arrStateRangeRate[i].dwBegin&&
				 dwAccOLMin < m_arrStateRangeRate[i].dwEnd )
			{
				return static_cast<EnumFatigueState>(i);
			}
		}
		ASSERT( 0 );
		return static_cast<EnumFatigueState>(GT_INVALID);		
	}
	
	
private:

	BOOL						m_bGuard;									// 是否是防沉迷用户
	DWORD						m_dwState;									// 游戏状态
	DWORD						m_dwAccOLMin;								//  当前累计在线时间

	PlayerSession*				m_pPlayerSession;							// 当前 PlayerSession

	static tagStateRangeRate	m_arrStateRangeRate[EFS_NUM];				// 收益率列表
};
