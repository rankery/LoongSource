//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: guild_affair.cpp
// author: Sunnee
// actor:
// data: 2009-07-03
// last:
// brief: 帮派事务发布
//-----------------------------------------------------------------------------

#include "StdAfx.h"

#include "../ServerDefine/log_cmdid_define.h"

#include "att_res.h"
#include "guild.h"
#include "guild_affair.h"
#include "role_mgr.h"
#include "role.h"

//-----------------------------------------------------------------------------
// 构造和析构
//-----------------------------------------------------------------------------
GuildAffair::GuildAffair()
{
	m_bInit			= FALSE;
	m_byCounter		= 0;
	m_pGuild		= NULL;
}

GuildAffair::~GuildAffair()
{
}

//-----------------------------------------------------------------------------
// 初始化
//-----------------------------------------------------------------------------
VOID GuildAffair::Init( Guild* pGuild )
{
	m_pGuild	= pGuild;
	m_byCounter	= 0;
	m_pGuild->ReCalAffairRemainTimes(0);
	m_bInit		= TRUE;
}

//-----------------------------------------------------------------------------
// 发布帮务
//-----------------------------------------------------------------------------
DWORD GuildAffair::SpreadGuildAffair( Role* pRole, DWORD dwBuffID )
{
	if (!m_bInit)
	{
		return GT_INVALID;
	}

	// 判断玩家权限
	tagGuildMember* pMember = m_pGuild->GetMember(pRole->GetID());
	if (!P_VALID(pMember))
	{
		return E_Guild_MemberNotIn;
	}

	if (!m_pGuild->GetGuildPower(pMember->eGuildPos).bAffair)
	{
		return E_Guild_Power_NotEnough;
	}

	// 判断帮派状态
	if (m_pGuild->IsInGuildStateAny(EGDSS_Shortage | EGDSS_Distress | EGDSS_Chaos | EGDSS_Warfare))
	{
		return E_Guild_State_Limit;
	}

	// 发布次数限制
	if (m_byCounter >= MGuildAffairTimes(m_pGuild->GetGuildAtt().byLevel))
	{
		return E_GuildAffair_Times_Limit;
	}

	// 取得帮务属性
	const tagGuildAffairInfo* pAffairProto = g_attRes.GetGuildAffairInfo(dwBuffID);
	if (!P_VALID(pAffairProto))	return GT_INVALID;
	
	// 帮派等级限制
	if (m_pGuild->GetGuildAtt().byLevel < pAffairProto->byGuildLevel)
	{
		return E_Guild_Level_Limit;
	}

	// 帮派占领城市限制
	if (pAffairProto->byHoldCity != 0)
	{
		BOOL bHoldCity = FALSE;
		for (int n=0; n<MAX_GUILD_HOLDCITY; n++)
		{
			if (pAffairProto->byHoldCity == m_pGuild->GetGuildAtt().byHoldCity[n])
			{
				bHoldCity = TRUE;
				break;
			}
		}
		if (!bHoldCity)
		{
			return E_Guild_HoldCity_Limit;
		}
	}

	// 帮派资源限制
	if (m_pGuild->GetGuildAtt().nFund < pAffairProto->nFund)
	{
		return E_Guild_Fund_NotEnough;
	}

	if (m_pGuild->GetGuildAtt().nMaterial < pAffairProto->nMaterial)
	{
		return E_Guild_Material_NotEnough;
	}

	// 扣除帮派资源
	m_pGuild->DecGuildFund(pRole->GetID(), pAffairProto->nFund, ELCLD_Guild_SpreadAffair);
	m_pGuild->DecGuildMaterial(pRole->GetID(), pAffairProto->nMaterial, ELCLD_Guild_SpreadAffair);

	// 发布次数增加
	m_byCounter++;
	m_pGuild->ReCalAffairRemainTimes(m_byCounter);

	// 取得buff属性
	const tagBuffProto* pProto = g_attRes.GetBuffProto(dwBuffID);
	if( !P_VALID(pProto) ) return GT_INVALID;

	tagNS_SpreadGuildAffair send;
	send.dwBuffID		= dwBuffID;
	send.dwRole			= pRole->GetID();
	send.byRemainTimes	= m_pGuild->GetGuildAtt().byAffairRemainTimes;

	// 取得成员表
	MapGuildMember& mapMember = m_pGuild->GetGuildMemberMap();

	// 给所有在线成员加buff
	tagGuildMember* pGuildMember = NULL;
	MapGuildMember::TMapIterator iter = mapMember.Begin();
	while (mapMember.PeekNext(iter, pGuildMember))
	{
		// 判断玩家是否在线
		Role* pGuildRole = g_roleMgr.GetRolePtrByID(pGuildMember->dwRoleID);
		if (P_VALID(pGuildRole))
		{
			// 添加buff并帮内广播消息
			pGuildRole->TryAddBuff(pGuildRole, pProto, NULL, NULL, NULL);
			pGuildRole->SendMessage(&send, send.dwSize);
		}
	}

	return 0;
}

//-----------------------------------------------------------------------------
// 重置帮务发布次数
//-----------------------------------------------------------------------------
VOID GuildAffair::ResetAffairCounter()
{
	if (!m_bInit)
	{
		return;
	}

	m_byCounter = 0;
	m_pGuild->ReCalAffairRemainTimes(0);
}