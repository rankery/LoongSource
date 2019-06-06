//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: marriage.h
// author: songg
// actor:
// data: 2009-09-10
// last:
// brief: 结婚系统管理
//-----------------------------------------------------------------------------
#pragma once

#include "../WorldDefine/msg_marriage.h"

//前导声明

//-----------------------------------------------------------------------------
// 玩家结婚/婚礼的管理类
//-----------------------------------------------------------------------------
class MarriageMgr
{
public:
	MarriageMgr();
	~MarriageMgr();
	
public:
	VOID Init();//初始化

public:

	VOID Update();

	BOOL IsHavingWedding()					{	return m_bHavingWedding;} //是否有婚礼在进行

	VOID BegWedding()						{	m_bHavingWedding = TRUE;	}//开始婚礼

	VOID EndWedding()						{	ClearWeddingState();	}//结束婚礼

	VOID EnableSysOfficiator()				{	m_bSysOfficiator = TRUE;	}

	VOID DisableSysOfficiator()				{	m_bSysOfficiator = FALSE;	}

public:
	//-----------------------------------------------------------------------------
	// 各种set get
	//-----------------------------------------------------------------------------
	DWORD	GetOfficiatorRoleID()				{	return m_dwOfficiatorRoleID;}
	DWORD	GetApplicantRoleID()				{	return m_dwApplicantRoleID;}
	DWORD	GetTargetRoleID()					{	return m_dwTargetRoleID;}

	VOID	SetOfficiatorRoleID(DWORD OfficiatorRoleID) { m_dwOfficiatorRoleID = OfficiatorRoleID;}
	VOID	SetTargetRoleID(DWORD TargetRoleID)			{ m_dwTargetRoleID = TargetRoleID;}
	VOID	SetApplicantRoleID(DWORD ApplicantRoleID)	{ m_dwApplicantRoleID = ApplicantRoleID;}

private:
	//清空结婚状态信息
	VOID ClearWeddingState();

private:
	//-----------------------------------------------------------------------------
	// 婚礼相关
	//-----------------------------------------------------------------------------
	BOOL m_bHavingWedding; //是否有婚礼在进行

	EWeddingStep m_eWeddingStep; //婚礼步骤

	TCHAR m_szMaleRolePledge[X_LONG_STRING];	//男方誓言

	TCHAR m_szFemaleRolePledge[X_LONG_STRING];	//女方誓言

	DWORD m_dwOfficiatorRoleID; //主婚人

	DWORD m_dwTargetRoleID; //被申请人

	DWORD m_dwApplicantRoleID; //申请人

	BOOL m_bSysOfficiator; //是否由系统主婚


private:
	//-----------------------------------------------------------------------------
	// 脚本配置相关
	//-----------------------------------------------------------------------------
	//举行婚礼需要的金钱
	DWORD m_dwWeddingDecreaseSilver; 
};
extern MarriageMgr g_MarriageMgr;
