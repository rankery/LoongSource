//-------------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: guild_pos.cpp
// author: Sxg
// actor:
// data: 2009-4-20
// last:
// brief: 帮派职位管理
//-------------------------------------------------------------------------------
#include "StdAfx.h"

#include "guild_pos.h"
#include "role_mgr.h"
#include "../ServerDefine/role_data_define.h"
//-------------------------------------------------------------------------------
// 构造&析构
//-------------------------------------------------------------------------------
GuildPos::GuildPos()
{
	m_n8CurNumBangZhu	 = 0;
	m_n8CurNumFuBangZhu	 = 0;
	m_n8CurNumHongHuFa	 = 0;
	m_n8CurNumZiHuFa	 = 0;
	m_n8CurNumJingYing	 = 0;

	m_n8MaxNumJingYing	 = 0;

	m_n16CurNumMember	 = 0;
	m_n16MaxNumMember	 = 0;
}

GuildPos::~GuildPos()
{
}

//-------------------------------------------------------------------------------
// 初始化
//-------------------------------------------------------------------------------
VOID GuildPos::Init(BYTE byGuildLevel)
{
	for(INT i=0; i<X_GUILD_TANG_NUM; ++i)
	{
		m_Tang[i].Init(byGuildLevel);
	}

	m_n8MaxNumJingYing	= MGuildPosJingZhongNum(byGuildLevel);
	m_n16MaxNumMember	= MGuildMaxMember(byGuildLevel);
}

//-------------------------------------------------------------------------------
// 指定职位是满员
//-------------------------------------------------------------------------------
BOOL GuildPos::IsPosFull(EGuildMemberPos eGuildPos)
{
	ASSERT(!IsMemberFull());
	ASSERT(::IsGuildPosValid(eGuildPos));
	
	// 帮内直属职位判断
	switch(eGuildPos)
	{
	case EGMP_BangZhong:
		return FALSE;
	case EGMP_JingYing:
		return IsJingYingFull();
	case EGMP_FuBangZhu:
		return IsFuBangZhuFull();
	case EGMP_HongHuFa:
		return IsHongHuFaFull();
	case EGMP_ZiHuFa:
		return IsZiHuFaFull();
	case EGMP_BangZhu:
		return IsBangZhuFull();
	}

	// 各堂职位判断
	if(::IsInTang(eGuildPos))
	{
		EGuildTangType eTang;
		ETangMemberPos eTangPos;
		::GetTangType(eGuildPos, eTang, eTangPos);

		return m_Tang[eTang].IsPosFull(eTangPos);
	}

	// 没有进入上面流程返回，说明职位有问题
	return TRUE;
}

//-------------------------------------------------------------------------------
// 添加成员
//-------------------------------------------------------------------------------
DWORD GuildPos::AddMember(DWORD dwRoleID, EGuildMemberPos eGuildPos)
{
	ASSERT(!IsPosFull(eGuildPos));

	DWORD dwErrorCode = GT_INVALID;
	BOOL  bRunDefault = FALSE;

	// 帮内直属职位判断
	switch(eGuildPos)
	{
	case EGMP_BangZhong:
		dwErrorCode = AddPuTong(dwRoleID);
		break;
	case EGMP_JingYing:
		dwErrorCode = AddJingYing(dwRoleID);
		break;
	case EGMP_FuBangZhu:
		dwErrorCode = AddFuBangZhu(dwRoleID);
		break;
	case EGMP_HongHuFa:
		dwErrorCode = AddHongHuFa(dwRoleID);
		break;
	case EGMP_ZiHuFa:
		dwErrorCode = AddZiHuFa(dwRoleID);
		break;
	case EGMP_BangZhu:
		dwErrorCode = AddBangZhu(dwRoleID);
		break;
	default:
		bRunDefault = TRUE;
	}

	// 各堂职位判断
	if(::IsInTang(eGuildPos))
	{
		EGuildTangType eTang;
		ETangMemberPos eTangPos;
		::GetTangType(eGuildPos, eTang, eTangPos);

		dwErrorCode = m_Tang[eTang].AddMember(dwRoleID, eTangPos);
	}
	else if(bRunDefault)
	{
		// 执行到此处，调用该函数前的逻辑有问题
		ASSERT(0);
	}

	if(E_Success == dwErrorCode)
	{
		++m_n16CurNumMember;
	}

	return dwErrorCode;
}

DWORD GuildPos::AddBangZhu(DWORD dwRoleID)
{
	ASSERT(!IsBangZhuFull());
	++m_n8CurNumBangZhu;
	return E_Success;
}

DWORD GuildPos::AddFuBangZhu(DWORD dwRoleID)
{
	ASSERT(!IsFuBangZhuFull());
	++m_n8CurNumFuBangZhu;
	return E_Success;
}

DWORD GuildPos::AddHongHuFa(DWORD dwRoleID)
{
	ASSERT(!IsHongHuFaFull());
	++m_n8CurNumHongHuFa;
	return E_Success;
}

DWORD GuildPos::AddZiHuFa(DWORD dwRoleID)
{
	ASSERT(!IsZiHuFaFull());
	++m_n8CurNumZiHuFa;
	return E_Success;
}

DWORD GuildPos::AddJingYing(DWORD dwRoleID)
{
	ASSERT(!IsJingYingFull());
	++m_n8CurNumJingYing;
	return E_Success;
}

DWORD GuildPos::AddPuTong(DWORD dwRoleID)
{
	ASSERT(!IsMemberFull());
	return E_Success;
}

//-------------------------------------------------------------------------------
// 删除
//-------------------------------------------------------------------------------
VOID GuildPos::RemoveMember(DWORD dwRoleID, EGuildMemberPos eGuildPos)
{
	ASSERT(m_n16CurNumMember >= 1);

	BOOL  bRunDefault = FALSE;
	
	// 帮内直属职位判断
	switch(eGuildPos)
	{
	case EGMP_BangZhong:
		RemovePuTong(dwRoleID);
		break;
	case EGMP_JingYing:
		RemoveJingYing(dwRoleID);
		break;
	case EGMP_FuBangZhu:
		RemoveFuBangZhu(dwRoleID);
		break;
	case EGMP_HongHuFa:
		RemoveHongHuFa(dwRoleID);
		break;
	case EGMP_ZiHuFa:
		RemoveZiHuFa(dwRoleID);
		break;
	case EGMP_BangZhu:
		RemoveBangZhu(dwRoleID);
		break;
	default:
		bRunDefault = TRUE;
	}

	// 各堂职位判断
	if(::IsInTang(eGuildPos))
	{
		EGuildTangType eTang;
		ETangMemberPos eTangPos;
		::GetTangType(eGuildPos, eTang, eTangPos);
		m_Tang[eTang].RemoveMember(dwRoleID, eTangPos);
	}
	else if(bRunDefault)
	{
		// 执行到此处，调用该函数前的逻辑有问题
		ASSERT(0);
		return;
	}

	--m_n16CurNumMember;
}

VOID GuildPos::RemoveBangZhu(DWORD dwRoleID)
{
	--m_n8CurNumBangZhu;
	ASSERT(m_n8CurNumBangZhu >= 0);
}

VOID GuildPos::RemoveFuBangZhu(DWORD dwRoleID)
{
	--m_n8CurNumFuBangZhu;
	ASSERT(m_n8CurNumFuBangZhu >= 0);
}

VOID GuildPos::RemoveHongHuFa(DWORD dwRoleID)
{
	--m_n8CurNumHongHuFa;
	ASSERT(m_n8CurNumHongHuFa >= 0);
}

VOID GuildPos::RemoveZiHuFa(DWORD dwRoleID)
{
	--m_n8CurNumZiHuFa;
	ASSERT(m_n8CurNumZiHuFa >= 0);
}

VOID GuildPos::RemoveJingYing(DWORD dwRoleID)
{
	--m_n8CurNumJingYing;
	ASSERT(m_n8CurNumJingYing >= 0);
}

VOID GuildPos::RemovePuTong(DWORD dwRoleID)
{
	ASSERT(m_n16CurNumMember >= 1);
}

//-------------------------------------------------------------------------------
// 职位特殊要求
//-------------------------------------------------------------------------------
DWORD GuildPos::SpecGuildPosLimit(DWORD dwRoleID, EGuildMemberPos eGuildPos)
{
	// 红衣护法、紫衫护法只有女性角色可以担当
	if(EGMP_HongHuFa == eGuildPos || EGMP_ZiHuFa == eGuildPos)
	{
		tagRoleInfo *pRoleInfo = g_roleMgr.GetRoleInfo(dwRoleID);
		if(!P_VALID(pRoleInfo))
		{
			// 执行到此，说明删除角色的逻辑上有问题
			ASSERT(0);
			return E_Role_Not_ExistIn_World;
		}

		if(pRoleInfo->bySex != (BYTE)ES_Woman)
		{
			return E_Guild_Appoint_SexLimit;
		}
	}

	return E_Success;
}