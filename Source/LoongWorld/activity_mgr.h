//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: activity.h
// author: wjiang
// actor:
// data: 2009-03-27
// last:
// brief: 活动管理器
//-----------------------------------------------------------------------------
#pragma once

#include "../ServerDefine/activity_define.h"

//----------------------------------------------------------------------------------------------------
// 固定活动类
//----------------------------------------------------------------------------------------------------

class ActScript;

class CActivity : public ScriptData<ESD_Activity>
{
public:
	CActivity();
	~CActivity();

	VOID	Init(const tagActInfo *pActInfo);
	VOID	InitScriptData(tagActivityData* pActivityData);
	BOOL	CanStart(tagDWORDTime cur_time);
	BOOL	IsStartBroad(tagDWORDTime cur_time);
	BOOL	IsEndBroad(tagDWORDTime cur_time);
	BOOL	IsEnd(tagDWORDTime cur_time);
	BOOL	IsStart() { return m_bIsStart; }
	VOID	Start();
	VOID	End();

	DWORD		GetActID() { return m_pActInfo->dwID; }
	const ActScript* GetScript() { return m_pScript; }
	
	VOID	Update();
	VOID	OnMinute(tagDWORDTime cur_time);

public:
	VOID	AddEventTime(DWORD dwTime);
	VOID	SaveActivityData2DB();

private:
	VOID    BroadActivityState(EActBroadMode eState);

private:
	const tagActInfo*				m_pActInfo;
	BOOL							m_bIsStart;				// 活动是否开始
	std::vector<tagDWORDTime>		m_vecStartBroadTime;	// 活动开始前的广播时间
	std::vector<tagDWORDTime>		m_vecEndBroadTime;		// 活动结束前的广播时间
	std::set<DWORD>					m_setOnTimeTick;		// 活动中时间触发事件的触发时间列表
	DWORD							m_dwTick;				// 活动进行的时间

	const ActScript*				m_pScript;
};


//----------------------------------------------------------------------------------------------------
// 固定活动管理类
//----------------------------------------------------------------------------------------------------
class CActivityMgr
{
public:
	CActivityMgr(){}
	~CActivityMgr();

	BOOL   Init();
	VOID   Update();
	VOID   OnMinute(DWORD cur_time);
	VOID   Destroy();

	CActivity*	GetActivity(DWORD dwID)	{ return m_mapActivity.Peek(dwID); }
	BOOL		HaveActivitStart();
	VOID		SaveAllActivityData2DB();

private:
	BOOL   LoadActivityData();

private:
	TObjRef<VarContainer>		m_pVar;

	TMap<DWORD,	tagActInfo*>	m_mapActInfo;
	TMap<DWORD, CActivity*>		m_mapActivity;
};

extern CActivityMgr	g_activityMgr;

















