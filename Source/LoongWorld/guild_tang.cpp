//-------------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: guild_tang.cpp
// author: Sxg
// actor:
// data: 2009-4-7
// last:
// brief: 堂
//-------------------------------------------------------------------------------
#include "StdAfx.h"

#include "guild_tang.h"
//-------------------------------------------------------------------------------
// 构造&析构
//-------------------------------------------------------------------------------
Tang::Tang()
{
	// 属性
	for(INT i=0; i<MAX_GUILDPOS_TANGZHU; ++i)
	{
		m_dwTangZhuID[i] = GT_INVALID;
	}

	for(INT i=0; i<MAX_GUILDPOS_XIANGZHU; ++i)
	{
		m_dwXiangZhuID[i] = GT_INVALID;
	}

	// 辅助属性
	m_n8CurNumTangZhu		= 0;
	m_n8CurNumXiangZhu		= 0;
	m_n8MaxNumTangZhong		= 0;
}

Tang::~Tang()
{
}

//-------------------------------------------------------------------------------
// 初始化
//-------------------------------------------------------------------------------
VOID Tang::Init(BYTE byGuildLevel)
{
	m_n8MaxNumTangZhong = MGuildPosTangZhongNum(byGuildLevel);
}

//-------------------------------------------------------------------------------
// 添加成员
//-------------------------------------------------------------------------------
DWORD Tang::AddMember(DWORD dwRoleID, ETangMemberPos eTangPos)
{
	switch(eTangPos)
	{
	case ETMP_TangZhu:		// 堂主
		return AddTangZhu(dwRoleID);
		break;
	case ETMP_XiangZhu:		// 香主
		return AddXiangZhu(dwRoleID);
		break;
	case ETMP_BangZhong:	// 帮众
		return AddTangZhong(dwRoleID);
		break;
	default:				// 错误 -- 应由上层判断，排除该种错误
		ASSERT(0);
		return GT_INVALID;
		break;
	}

	return E_Success;
}

DWORD Tang::AddTangZhu(DWORD dwRoleID)
{
	// 上限
	//if(m_n8CurNumTangZhu >= MAX_GUILDPOS_TANGZHU)
	//{
	//	return E_Guild_Member_Full;
	//}
	ASSERT(!IsTangZhuFull());

	// 设置
	for(INT i=0; i<MAX_GUILDPOS_TANGZHU; ++i)
	{
		if(GT_INVALID == m_dwTangZhuID[i])
		{
			m_dwTangZhuID[i] = dwRoleID;
			
			// 人数自加
			++m_n8CurNumTangZhu;
			return E_Success;
		}
	}

	// 执行到此处说明程序有bug
	ASSERT(0);
	return GT_INVALID;
}

DWORD Tang::AddXiangZhu(DWORD dwRoleID)
{
	// 上限
	//if(m_n8CurNumXiangZhu >= MAX_GUILDPOS_XIANGZHU)
	//{
	//	return E_Guild_Member_Full;
	//}
	ASSERT(!IsXiangZhuFull());

	// 设置
	for(INT i=0; i<MAX_GUILDPOS_XIANGZHU; ++i)
	{
		if(GT_INVALID == m_dwXiangZhuID[i])
		{
			m_dwXiangZhuID[i] = dwRoleID;

			// 人数自加
			++m_n8CurNumXiangZhu;
			return E_Success;
		}
	}

	// 执行到此处说明程序有bug
	ASSERT(0);
	return GT_INVALID;
}

DWORD Tang::AddTangZhong(DWORD dwRoleID)
{
	// 上限
	//if(m_lstTangZhong.Size() >= m_n8MaxNumBangZhong)
	//{
	//	return E_Guild_Member_Full;
	//}
	ASSERT(!IsTangZhongFull());

	// 设置
	m_lstTangZhong.PushBack(dwRoleID);

	return E_Success;
}

//-------------------------------------------------------------------------------
// 删除成员
//-------------------------------------------------------------------------------
VOID Tang::RemoveMember(DWORD dwRoleID, ETangMemberPos eTangPos)
{
	switch(eTangPos)
	{
	case ETMP_TangZhu:		// 堂主
		return RemoveTangZhu(dwRoleID);
		break;
	case ETMP_XiangZhu:		// 香主
		return RemoveXiangZhu(dwRoleID);
		break;
	case ETMP_BangZhong:	// 帮众
		return RemoveTangZhong(dwRoleID);
		break;
	default:				// 错误 -- 应由上层判断，排除该种错误
		ASSERT(0);
		break;
	}
}

VOID Tang::RemoveTangZhu(DWORD dwRoleID)
{
	if(m_n8CurNumTangZhu <= 0)
	{
		ASSERT(0);
		return;
	}

	for(INT i=0; i<MAX_GUILDPOS_TANGZHU; ++i)
	{
		if(dwRoleID == m_dwTangZhuID[i])
		{
			m_dwTangZhuID[i] = GT_INVALID;

			--m_n8CurNumTangZhu;
			return;
		}
	}

	ASSERT(0);
	return;
}

VOID Tang::RemoveXiangZhu(DWORD dwRoleID)
{
	if(m_n8CurNumXiangZhu <= 0)
	{
		ASSERT(0);
		return;
	}

	for(INT i=0; i<MAX_GUILDPOS_XIANGZHU; ++i)
	{
		if(dwRoleID == m_dwXiangZhuID[i])
		{
			m_dwXiangZhuID[i] = GT_INVALID;

			--m_n8CurNumXiangZhu;
			return;
		}
	}

	ASSERT(0);
	return;
}

VOID Tang::RemoveTangZhong(DWORD dwRoleID)
{
	if(m_lstTangZhong.Size() <= 0)
	{
		ASSERT(0);
		return;
	}

	if(!m_lstTangZhong.Erase(dwRoleID))
	{
		ASSERT(0);
		return;
	}
}

//-------------------------------------------------------------------------------
// 是否可以加入
//-------------------------------------------------------------------------------
BOOL Tang::IsPosFull(ETangMemberPos eTangPos)
{
	switch(eTangPos)
	{
	case ETMP_TangZhu:		// 堂主
		return IsTangZhuFull();
		break;
	case ETMP_XiangZhu:		// 香主
		return IsXiangZhuFull();
		break;
	case ETMP_BangZhong:	// 帮众
		return IsTangZhongFull();
		break;
	default:				// 错误 -- 应由上层判断，排除该种错误
		ASSERT(0);
		return TRUE;
		break;
	}

	return TRUE;
}