//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: gm_policy.h
// author: xlguo
// actor:
// data: 2009-06-08
// last:
// brief: GM策略类
//-----------------------------------------------------------------------------
#pragma once

#include "time_limit_mgr.h"
#include "world.h"
#include "gm_policy_part.h"


//-----------------------------------------------------------------------------
// GM双倍策略
//-----------------------------------------------------------------------------
class GMDoublePolicy
{
public:
	//-----------------------------------------------------------------------------
	// 构造与析构
	//-----------------------------------------------------------------------------
	GMDoublePolicy();
	virtual ~GMDoublePolicy(){}

public:
	//-----------------------------------------------------------------------------
	// 获取倍率（地图线程调用）
	//-----------------------------------------------------------------------------
	virtual FLOAT	GetExpRate() const 							
	{	
		return m_bDouble[EDoubleType_Exp] ? m_fCurDouble[EDoubleType_Exp] : 1.0f;
	}
	virtual FLOAT	GetLootRate(BOOL bNormalMap) const	
	{
		return m_bDouble[EDoubleType_Item] ? (bNormalMap ? m_fCurDouble[EDoubleType_Item] : 1.0f) : 1.0f;	
	}

public:
	//-----------------------------------------------------------------------------
	// 设置双倍（主线程）
	//-----------------------------------------------------------------------------
	virtual VOID	SetRate(EDoubleType eDoubleType, DWORD dwRate, DWORD dwStart, DWORD dwLast );				

	//-----------------------------------------------------------------------------
	// 双倍开关
	//-----------------------------------------------------------------------------
	virtual VOID	DoubleSwitch(EDoubleType eType, BOOL bFlag) { m_bDouble[eType] = bFlag; }		

	//-----------------------------------------------------------------------------
	// 更新（主线程）
	//-----------------------------------------------------------------------------
	VOID	Update();

private:
	FLOAT	m_fCurDouble[NUM_EDT];		// 双倍倍率 0-5
	BOOL	m_bDouble[NUM_EDT];			// 双倍开启
	DWORD	m_dwEndTime[NUM_EDT];		// 双倍倍率的截止时间
	
	//TimeLimitMgr<EDoubleAct> m_TLMgr;	// 时限管理器
};

//-----------------------------------------------------------------------------
// GM策略
//-----------------------------------------------------------------------------
class GMPolicy
{
public:
	//-----------------------------------------------------------------------------
	// 构造与析构
	//-----------------------------------------------------------------------------
	GMPolicy()
	{
		m_pDoublePolicy = new GMDoublePolicy;
		//m_pNoticePolicy	= new GMNoticePolicy;
	}
	virtual ~GMPolicy()
	{
		SAFE_DEL(m_pDoublePolicy);
		//SAFE_DEL(m_pNoticePolicy);
	}

public:
	//-----------------------------------------------------------------------------
	// 获取倍率（地图线程调用）
	//-----------------------------------------------------------------------------
	FLOAT	GetExpRate() const 							
	{	
		return m_pDoublePolicy->GetExpRate();
	}
	FLOAT	GetLootRate(BOOL bNormalMap = TRUE) const	
	{
		return m_pDoublePolicy->GetLootRate(bNormalMap);
	}

	//-----------------------------------------------------------------------------
	// 设置双倍（主线程）
	//-----------------------------------------------------------------------------
	VOID	SetRate(EDoubleType eDoubleType, DWORD dwRate, DWORD dwStart, DWORD dwLast )
	{
		m_pDoublePolicy->SetRate(eDoubleType, dwRate, dwStart, dwLast);
	}

	//-----------------------------------------------------------------------------
	// 双倍开关
	//-----------------------------------------------------------------------------
	VOID	DoubleSwitch(EDoubleType eType, BOOL bFlag) { m_pDoublePolicy->DoubleSwitch(eType, bFlag); }

public:
	//-----------------------------------------------------------------------------
	// 更新（主线程）
	//-----------------------------------------------------------------------------
	VOID Update()
	{
		m_pDoublePolicy->Update();
		//m_pNoticePolicy->Update();
	}

private:
	GMDoublePolicy*		m_pDoublePolicy;
	//GMNoticePolicy*		m_pNoticePolicy;
};

extern GMPolicy g_GMPolicy;