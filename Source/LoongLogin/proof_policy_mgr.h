//-----------------------------------------------------------------------------
//!\file proof_policy_mgr.h
//!\author Aslan
//!
//!\date 2008-11-30
//! last 2008-11-30
//!
//!\brief 验证策略管理器
//!
//!	Copyright (c) 2004 TENGWU Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include "Singleton.h"

//-----------------------------------------------------------------------------
// 验证策略管理器
//-----------------------------------------------------------------------------
class ProofPolicyMgr : public Singleton<ProofPolicyMgr>
{
public:
	//-------------------------------------------------------------------------
	// CONSTRUCT
	//-------------------------------------------------------------------------
	ProofPolicyMgr() {}
	~ProofPolicyMgr();

public:
	//-------------------------------------------------------------------------
	// 初始化和销毁
	//-------------------------------------------------------------------------
	BOOL			Init();
	VOID			Destroy();

	//-------------------------------------------------------------------------
	// 生成和销毁对象
	//-------------------------------------------------------------------------
	ProofPolicy*	CreateProofPolicy(EProofPolicy ePolicy);
	VOID			DestroyProofPolicy(EProofPolicy ePolicy, ProofPolicy* pPolicy);

private:
	//-------------------------------------------------------------------------
	// 注册工厂类
	//-------------------------------------------------------------------------
	VOID			RegisterAFPolicy();
	VOID			UnregisterAFPolicy();

private:
	TAutoFactory<ProofPolicy>			m_AFPolicy;		// 验证策略工厂
};

#define sProofPolicyMgr ProofPolicyMgr::getSingleton()