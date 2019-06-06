//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: guild_guild.cpp
// author: Sunnee
// actor:
// data: 2009-07-03
// last:
// brief: 帮派技能
//-----------------------------------------------------------------------------

#include "StdAfx.h"

#include "guild.h"
#include "guild_skill.h"
#include "att_res.h"
#include "role.h"

#include "../ServerDefine/log_cmdid_define.h"
#include "../ServerDefine/msg_guild.h"

//-----------------------------------------------------------------------------
// 构造与析构
//-----------------------------------------------------------------------------
GuildSkill::GuildSkill()
{
	m_bInit			= FALSE;
	m_pGuild		= NULL;
	m_dwCurSkill	= GT_INVALID;
	m_mapGuildSkill.Clear();
}

GuildSkill::~GuildSkill()
{
	Destroy();
}

//-----------------------------------------------------------------------------
// 初始化和销毁
//-----------------------------------------------------------------------------
VOID GuildSkill::Init( Guild* pGuild, BOOL bRequest /*= FALSE*/ )
{
	ASSERT(P_VALID(pGuild));
	if (!P_VALID(pGuild))
	{
		return;
	}

	m_bInit			= FALSE;
	m_pGuild		= pGuild;
	m_dwCurSkill	= GT_INVALID;

	m_mapGuildSkill.Clear();

	// 发送数据库请求
	if (bRequest)
	{
		tagNDBC_LoadGuildSkillInfo send;
		send.dwGuildID	= m_pGuild->GetGuildAtt().dwID;
		g_dbSession.Send(&send, send.dwSize);
	}
}

VOID GuildSkill::Destroy()
{
	m_bInit			= FALSE;
	m_pGuild		= NULL;
	m_dwCurSkill	= GT_INVALID;
	
	tagGuildSkill* pSkillInfo = NULL;
	MapGuildSkill::TMapIterator iter = m_mapGuildSkill.Begin();
	while (m_mapGuildSkill.PeekNext(iter, pSkillInfo))
	{
		SAFE_DEL(pSkillInfo);
	}
	m_mapGuildSkill.Clear();
}

//-----------------------------------------------------------------------------
// 载入帮派技能信息
//-----------------------------------------------------------------------------
BOOL GuildSkill::LoadGuildSkillIInfo( tagGuildSkillInfo* pSkillInfo, INT nSkillNum )
{
	if (!P_VALID(pSkillInfo) || nSkillNum <= 0)
	{
		return FALSE;
	}

	// 上层保证pSkillInfo的大小
	for (int n=0; n<nSkillNum; n++)
	{
		// 读取帮派技能静态属性
		tagGuildSkill* pGuildSkill	= new tagGuildSkill;
		if (!g_attRes.GetGuildSkillInfo(pSkillInfo[n].dwSkillID, pSkillInfo[n].nLevel, *pGuildSkill))
		{
			// 满级判断
			if (pSkillInfo[n].nLevel > m_pGuild->GetGuildFacilities().GetMaxSkillLevel())
			{
				m_mapGuildSkill.Add(pSkillInfo[n].dwSkillID, pGuildSkill);
			}
			else
			{
				SAFE_DEL(pGuildSkill);
			}
			continue;
		}
		pGuildSkill->n16Progress	= pSkillInfo[n].n16Progress;
		m_mapGuildSkill.Add(pSkillInfo[n].dwSkillID, pGuildSkill);

		// 设置当前研究技能
		if (pSkillInfo[n].bResearching)
		{
			if (!GT_VALID(m_dwCurSkill))
			{
				m_dwCurSkill = pSkillInfo[n].dwSkillID;
			}
			else if (pSkillInfo[n].dwSkillID != m_dwCurSkill)
			{
				// 数据库中有错误,纠正之
				ChangeResearchSkill(pSkillInfo[n].dwSkillID);
				m_dwCurSkill = pSkillInfo[n].dwSkillID;
			}
		}
	}

	// 初始化完成
	m_bInit = TRUE;

	return TRUE;
}

//-----------------------------------------------------------------------------
// 获取所有帮派技能信息
//-----------------------------------------------------------------------------
DWORD GuildSkill::GetGuildSkillInfo( DWORD& dwSkillID, INT16& n16Progress, INT& nSkillNum, DWORD* pLevelInfo )
{
	if (!m_bInit)
	{
		return GT_INVALID;
	}

	// 上层保证pLevelInfo大小
	if (!P_VALID(pLevelInfo))
	{
		return GT_INVALID;
	}
	tagGuildSkill* pGuildSkill	= NULL;
	nSkillNum					= 0;
	dwSkillID					= GT_INVALID;
	n16Progress					= GT_INVALID;

	// 取得当前研究技能
	pGuildSkill = m_mapGuildSkill.Peek(m_dwCurSkill);
	if (P_VALID(pGuildSkill))
	{
		dwSkillID		= m_dwCurSkill * 100 + pGuildSkill->nLevel;
		n16Progress		= pGuildSkill->n16Progress;
	}
	
	MapGuildSkill::TMapIterator iter = m_mapGuildSkill.Begin();
	while (m_mapGuildSkill.PeekNext(iter, pGuildSkill))
	{
		if (!P_VALID(pGuildSkill))
		{
			continue;
		}

		pLevelInfo[nSkillNum++] = pGuildSkill->dwSkillID * 100 + pGuildSkill->nLevel;
	}

	return E_Success;
}

//-----------------------------------------------------------------------------
// 整点升级
//-----------------------------------------------------------------------------
DWORD GuildSkill::SkillUpgradeOnClock()
{
	if (!m_bInit)
	{
		return GT_INVALID;
	}

	// 取得当先研究技能
	if (!GT_VALID(m_dwCurSkill))
	{
		return E_GuildSkill_NotResearch;
	}

	tagGuildSkill* pGuildSkill = m_mapGuildSkill.Peek(m_dwCurSkill);
	if (!P_VALID(pGuildSkill))
	{
		return E_GuildSkill_NotExist;
	}

	// 技能达到等级上限
	if (pGuildSkill->nLevel > m_pGuild->GetGuildFacilities().GetMaxSkillLevel())
	{
		return E_GuildSkill_Level_Limit;
	}

	// 帮派状态判定
	if (m_pGuild->IsInGuildStateAny(EGDSS_Shortage | EGDSS_Distress | EGDSS_Chaos))
	{
		return E_Guild_State_Limit;
	}

	FLOAT fModify = 1.0f;
	if (m_pGuild->IsInGuildState(EGDSS_Refulgence))
	{
		fModify = 1.5f;
	}
	else if (m_pGuild->IsInGuildState(EGDSS_Primacy))
	{
		fModify = 2.0f;
	}

	// 帮派资金资材限制
	const tagGuild& sAtt = m_pGuild->GetGuildAtt();
	if (sAtt.nFund < pGuildSkill->nResearchFund)
	{
		return E_Guild_Fund_NotEnough;
	}
	if (sAtt.nMaterial < pGuildSkill->nResearchMaterial)
	{
		return E_Guild_Material_NotEnough;
	}

	// 扣除帮派资源
	m_pGuild->DecGuildFund(GT_INVALID, pGuildSkill->nResearchFund, ELCLD_Guild_SkillResearch);
	m_pGuild->DecGuildMaterial(GT_INVALID, pGuildSkill->nResearchMaterial, ELCLD_Guild_SkillResearch);

	// 计算提升量(研究进度提升量=帮派等级×10×[1+（当前安定度-5000）/10000])
	INT16 n16Upgrade = INT16(sAtt.byLevel * 10.0f * (1.0f + (sAtt.n16Peace - 5000.0f) / 10000.0f) * fModify);

	pGuildSkill->n16Progress += n16Upgrade;
	if (pGuildSkill->n16Progress >= pGuildSkill->n16Fulfill)
	{
		// 取得等级上限
		INT nMaxLevel = m_pGuild->GetGuildFacilities().GetMaxSkillLevel();

		pGuildSkill->nLevel++;
		if (pGuildSkill->nLevel > nMaxLevel)
		{
			// 达到技能上限
			m_dwCurSkill				= GT_INVALID;
			pGuildSkill->n16Progress	= 0;
			ChangeResearchSkill(GT_INVALID);

			// 通知客户端
			tagNS_SetResearchSkill send;
			send.dwSkillID		= GT_INVALID;
			send.n16Progress	= 0;
			m_pGuild->SendGuildMsg(&send, send.dwSize);
		}
		else
		{
			// 读取下一级帮派技能静态属性
			g_attRes.GetGuildSkillInfo(pGuildSkill->dwSkillID, pGuildSkill->nLevel, *pGuildSkill);

			pGuildSkill->n16Progress -= pGuildSkill->n16Fulfill;
		}

		// 帮派广播
		tagNS_GuildSkillLevelUp send_broad;
		send_broad.dwSkillID = pGuildSkill->dwSkillID * 100 + pGuildSkill->nLevel;
		m_pGuild->SendGuildMsg(&send_broad, send_broad.dwSize);
	}

	// 保存到数据库
	SaveSkillInfo2DB(pGuildSkill->dwSkillID);

	return E_Success;
}

//-----------------------------------------------------------------------------
// 上缴物品升级
//-----------------------------------------------------------------------------
DWORD GuildSkill::SkillUpgradeByHandIn( Role* pRole, INT64 n64Serial, DWORD& dwSkillID, INT16& n16Progress )
{
	if (!m_bInit)
	{
		return GT_INVALID;
	}

	// 玩家权限判断
	tagGuildMember* pMember = m_pGuild->GetMember(pRole->GetID());
	if (!P_VALID(pMember))
	{
		return E_Guild_MemberNotIn;
	}
	if (!m_pGuild->GetGuildPower(pMember->eGuildPos).bAdvSkill)
	{
		return E_Guild_Power_NotEnough;
	}

	// 取得当先研究技能
	if (!GT_VALID(m_dwCurSkill))
	{
		return E_GuildSkill_NotResearch;
	}

	tagGuildSkill* pGuildSkill = m_mapGuildSkill.Peek(m_dwCurSkill);
	if (!P_VALID(pGuildSkill))
	{
		return E_GuildSkill_NotExist;
	}

	// 技能达到等级上限(不大可能)
	if (pGuildSkill->nLevel > m_pGuild->GetGuildFacilities().GetMaxSkillLevel())
	{
		return E_GuildSkill_Level_Limit;
	}

	// 上缴物品判定
	tagItem* pItem = pRole->GetItemMgr().GetBagItem(n64Serial);
	if (!P_VALID(pItem) || pItem->pProtoType->eSpecFunc != EISF_GuildSkillBook)
	{
		return E_GuildSkill_Wrong_Item;
	}

	// 帮派状态判定
	if (m_pGuild->IsInGuildStateAny(EGDSS_Shortage | EGDSS_Distress | EGDSS_Chaos))
	{
		return E_Guild_State_Limit;
	}

	FLOAT fModify = 1.0f;
	if (m_pGuild->IsInGuildState(EGDSS_Refulgence))
	{
		fModify = 1.5f;
	}
	else if (m_pGuild->IsInGuildState(EGDSS_Primacy))
	{
		fModify = 2.0f;
	}

	// 帮派资金资材限制
	const tagGuild& sAtt = m_pGuild->GetGuildAtt();
	if (sAtt.nFund < pGuildSkill->nResearchFund)
	{
		return E_Guild_Fund_NotEnough;
	}
	if (sAtt.nMaterial < pGuildSkill->nResearchMaterial)
	{
		return E_Guild_Material_NotEnough;
	}

	// 扣除帮派资源
	m_pGuild->DecGuildFund(pRole->GetID(), pGuildSkill->nResearchFund, ELCLD_Guild_SkillResearch);
	m_pGuild->DecGuildMaterial(pRole->GetID(), pGuildSkill->nResearchMaterial, ELCLD_Guild_SkillResearch);

	// 计算提升量
	INT16 n16Upgrade = INT16(pItem->pProtoType->nSpecFuncVal1 * fModify);

	pGuildSkill->n16Progress += n16Upgrade;
	if (pGuildSkill->n16Progress >= pGuildSkill->n16Fulfill)
	{
		// 取得等级上限
		INT nMaxLevel = m_pGuild->GetGuildFacilities().GetMaxSkillLevel();

		pGuildSkill->nLevel++;
		if (pGuildSkill->nLevel > nMaxLevel)
		{
			// 达到技能上限
			m_dwCurSkill				= GT_INVALID;
			pGuildSkill->n16Progress	= 0;
			ChangeResearchSkill(GT_INVALID);

			// 通知客户端
			tagNS_SetResearchSkill send;
			send.dwSkillID		= GT_INVALID;
			send.n16Progress	= 0;
			m_pGuild->SendGuildMsg(&send, send.dwSize);
		}
		else
		{
			// 读取下一级帮派技能静态属性
			g_attRes.GetGuildSkillInfo(pGuildSkill->dwSkillID, pGuildSkill->nLevel, *pGuildSkill);

			pGuildSkill->n16Progress -= pGuildSkill->n16Fulfill;
		}

		// 帮派广播
		tagNS_GuildSkillLevelUp send_broad;
		send_broad.dwSkillID = pGuildSkill->dwSkillID * 100 + pGuildSkill->nLevel;
		m_pGuild->SendGuildMsg(&send_broad, send_broad.dwSize);
	}

	// 保存到数据库
	SaveSkillInfo2DB(pGuildSkill->dwSkillID);

	// 增加玩家贡献值
	m_pGuild->ChangeMemberContribution(pRole->GetID(), pItem->pProtoType->nSpecFuncVal1, TRUE);

	// 扣除玩家物品
	pRole->GetItemMgr().RemoveFromRole(pItem->dwTypeID, 1, ELCLD_Guild_SkillResearch);

	// 设置传出参数
	dwSkillID	= pGuildSkill->dwSkillID * 100 + pGuildSkill->nLevel;
	n16Progress	= pGuildSkill->n16Progress;

	return E_Success;
}

//-----------------------------------------------------------------------------
// 学习技能
//-----------------------------------------------------------------------------
DWORD GuildSkill::LearnGuildSkill( Role* pRole, DWORD dwSkillID, INT& nLevel, BOOL& bLearnError )
{
	if (!m_bInit)
	{
		return GT_INVALID;
	}

	// 判断玩家
	if (!P_VALID(pRole) || !GT_VALID(dwSkillID))
	{
		return GT_INVALID;
	}

	bLearnError	= FALSE;

	tagGuildMember* pMember = m_pGuild->GetMember(pRole->GetID());
	if (!P_VALID(pMember))
	{
		return E_Guild_MemberNotIn;
	}

	// 判断指定技能
	tagGuildSkill* pGuildSkill = m_mapGuildSkill.Peek(dwSkillID);
	if (!P_VALID(pGuildSkill))
	{
		return E_GuildSkill_NotExist;
	}

	// 判断已学技能
	BOOL bLearn = TRUE;
	Skill* pSkill = pRole->GetSkill(dwSkillID);
	if (P_VALID(pSkill))
	{
		bLearn = FALSE;		// 技能升级
		if (pGuildSkill->nLevel <= pSkill->GetLevel()+1)
		{
			return E_GuildSkill_Level_Limit;
		}
	}

	// 判断帮派状态
	if (m_pGuild->IsInGuildStateAny(EGDSS_Shortage | EGDSS_Distress | EGDSS_Warfare))
	{
		return E_Guild_State_Limit;
	}

	// 取得帮派技能属性
	const tagGuildSkill* pSkillProto = NULL;
	if (P_VALID(pSkill))
	{
		pSkillProto = g_attRes.GetGuildSkillProto(pSkill->GetTypeID() + 1);
	}
	else
	{
		pSkillProto = g_attRes.GetGuildSkillProto(Skill::CreateTypeID(dwSkillID, 1));
	}
	
	if (!P_VALID(pSkillProto))
	{
		return E_GuildSkill_Level_Limit;
	}

	// 检查消耗内容
	if (pRole->GetCurMgr().GetBagSilver() < pSkillProto->nLearnSilver)
	{
		return E_BagSilver_NotEnough;
	}
	if (pMember->nContribution < pSkillProto->nLearnContribution)
	{
		return E_GuildMember_Contribution_Limit;
	}
	if(m_pGuild->GetGuildAtt().nFund < pSkillProto->nLearnFund)
	{
		return E_Guild_Fund_NotEnough;
	}
	if (m_pGuild->GetGuildAtt().nMaterial < pSkillProto->nLearnMaterial)
	{
		return E_Guild_Material_NotEnough;
	}

	// 学习或者升级技能
	bLearnError			= TRUE;
	DWORD dwErrorCode	= GT_INVALID;
	if (bLearn)
	{
		dwErrorCode = pRole->LearnSkill(dwSkillID);
	}
	else
	{
		dwErrorCode = pRole->LevelUpSkill(dwSkillID);
	}

	if (E_Success == dwErrorCode)
	{
		// 扣除消耗
		pRole->GetCurMgr().DecBagSilver(pSkillProto->nLearnSilver, ELCLD_Guild_SkillLearn);
		m_pGuild->ChangeMemberContribution(pRole->GetID(), pSkillProto->nLearnContribution, FALSE);
		m_pGuild->DecGuildFund(pRole->GetID(), pSkillProto->nLearnFund, ELCLD_Guild_SkillLearn);
		m_pGuild->DecGuildMaterial(pRole->GetID(), pSkillProto->nLearnMaterial, ELCLD_Guild_SkillLearn);
	}

	// 设置传出参数
	pSkill = pRole->GetSkill(dwSkillID);
	if (P_VALID(pSkill))
	{
		nLevel	= pSkill->GetLevel();
	}

	return dwErrorCode;
}

//-----------------------------------------------------------------------------
// 创建帮派技能
//-----------------------------------------------------------------------------
VOID GuildSkill::CreateGuildSkill()
{
	// 读取初始帮派技能信息
	g_attRes.LoadGuildSkillInfo(m_mapGuildSkill);

	// 设置当前无研究技能
	m_dwCurSkill = GT_INVALID;

	tagGuildSkill* pSkillInfo = NULL;
	MapGuildSkill::TMapIterator iter = m_mapGuildSkill.Begin();
	while (m_mapGuildSkill.PeekNext(iter, pSkillInfo))
	{
		if (!P_VALID(pSkillInfo))
		{
			continue;
		}
		
		// 向数据库发送1级技能
		tagNDBC_CreateGuildSkill send;
		send.dwGuildID					= m_pGuild->GetGuildAtt().dwID;
		send.sSkillInfo.nLevel			= pSkillInfo->nLevel;
		send.sSkillInfo.dwSkillID		= pSkillInfo->dwSkillID;
		send.sSkillInfo.n16Progress		= pSkillInfo->n16Progress;
		send.sSkillInfo.bResearching	= FALSE;

		g_dbSession.Send(&send, send.dwSize);
	}

	// 初始化完成
	m_bInit = TRUE;
}

//-----------------------------------------------------------------------------
// 设置当前升级技能
//-----------------------------------------------------------------------------
DWORD GuildSkill::SetCurUpgradeGuildSkill( DWORD dwRoleID, DWORD dwSkillID, INT& nLevel, INT16& n16Progress )
{
	if (!m_bInit)
	{
		return GT_INVALID;
	}

	if (!GT_VALID(dwRoleID))
	{
		return GT_INVALID;
	}

	// 同样的技能不做处理
	if (dwSkillID == m_dwCurSkill)
	{
		return GT_INVALID;
	}

	// 判断玩家权限
	tagGuildMember* pMember = m_pGuild->GetMember(dwRoleID);
	if (!P_VALID(pMember))
	{
		return E_Guild_MemberNotIn;
	}
	if (!m_pGuild->GetGuildPower(pMember->eGuildPos).bSetSkill)
	{
		return E_Guild_Power_NotEnough;
	}

	// 判断技能
	if (GT_VALID(dwSkillID))
	{
		tagGuildSkill* pGuildSkill = m_mapGuildSkill.Peek(dwSkillID);
		if (!P_VALID(pGuildSkill))
		{
			return E_GuildSkill_NotExist;
		}
		if (pGuildSkill->nLevel > m_pGuild->GetGuildFacilities().GetMaxSkillLevel())
		{
			return E_GuildSkill_Level_Limit;
		}
		n16Progress	= pGuildSkill->n16Progress;
		nLevel		= pGuildSkill->nLevel;

		// 读取技能静态属性
		g_attRes.GetGuildSkillInfo(pGuildSkill->dwSkillID, pGuildSkill->nLevel, *pGuildSkill);
	}

	// 设置当前升级技能
	ChangeResearchSkill(dwSkillID);
	m_dwCurSkill	= dwSkillID;

	return E_Success;
}

//-----------------------------------------------------------------------------
// 与数据库通讯
//-----------------------------------------------------------------------------
VOID GuildSkill::SaveSkillInfo2DB(DWORD dwSkillID)
{
	if (!GT_VALID(dwSkillID))
	{
		return;
	}

	tagGuildSkill* pGuildSkill = m_mapGuildSkill.Peek(dwSkillID);
	if (!P_VALID(pGuildSkill))
	{
		return;
	}

	tagNDBC_SaveGuildSkill send;
	send.dwGuildID				= m_pGuild->GetGuildAtt().dwID;
	send.sSkillInfo.nLevel		= pGuildSkill->nLevel;
	send.sSkillInfo.dwSkillID	= pGuildSkill->dwSkillID;
	send.sSkillInfo.n16Progress	= pGuildSkill->n16Progress;

	g_dbSession.Send(&send, send.dwSize);
}

VOID GuildSkill::ChangeResearchSkill( DWORD dwNewSkillID )
{
	tagNDBC_ChangeResearchSkill send;
	send.dwGuildID		= m_pGuild->GetGuildAtt().dwID;
	send.dwNewSkillID	= dwNewSkillID;

	g_dbSession.Send(&send, send.dwSize);
}

//-----------------------------------------------------------------------------
// 清除所有帮派技能信息
//-----------------------------------------------------------------------------
VOID GuildSkill::RemoveGuildSkillInfo()
{
	if (!m_bInit)
	{
		return;
	}

	tagNDBC_RemoveGuildSkill send;
	send.dwGuildID = m_pGuild->GetGuildAtt().dwID;

	g_dbSession.Send(&send, send.dwSize);
}