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
#include "StdAfx.h"
#include "fatigue_guarder.h"
#include "player_session.h"
//-----------------------------------------------------------------------------
// 收益率列表
//-----------------------------------------------------------------------------
FatigueGuarder::tagStateRangeRate 
FatigueGuarder::m_arrStateRangeRate[EFS_NUM]	= {	{0,		180,	10000},			// 0h-3h
													{180,	300,	5000},			// 3h-5h
													{300, GT_INVALID, 0},			// 5h-UNLIMIT
													};

//-----------------------------------------------------------------------------
// 更新
//-----------------------------------------------------------------------------
VOID FatigueGuarder::SetAccOLTimeMin( DWORD dwState, DWORD dwAccOLTimeMin )
{
	if (!m_bGuard)	return;

	m_dwAccOLMin = dwAccOLTimeMin;

	m_dwState = dwState;
	ASSERT( EFS_VALID(m_dwState) );

	NotifyClient();
}

//-----------------------------------------------------------------------------
// 通知客户端
//-----------------------------------------------------------------------------
void FatigueGuarder::NotifyClient() const
{
	// 通知客户端
	tagNS_GetFatigueInfo send;
	send.byState = (BYTE)m_dwState;
	send.dwCurOLSec = m_dwAccOLMin * 60;
	m_pPlayerSession->SendMessage(&send, send.dwSize);
}