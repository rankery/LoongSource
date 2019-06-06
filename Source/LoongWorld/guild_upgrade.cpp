//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: guild_upgrade.cpp
// author: Sunnee
// actor:
// data: 2009-07-03
// last:
// brief: 帮派升级和帮派设施升级
//-----------------------------------------------------------------------------

#include "StdAfx.h"

#include "../ServerDefine/msg_guild.h"
#include "../ServerDefine/guild_define.h"
#include "../ServerDefine/log_cmdid_define.h"

#include "../WorldDefine/msg_guild.h"
#include "../WorldDefine/guild_define.h"

#include "att_res.h"
#include "world.h"
#include "role.h"
#include "guild.h"
#include "guild_upgrade.h"

//-----------------------------------------------------------------------------
// 构造和析构
//-----------------------------------------------------------------------------
GuildUpgrade::GuildUpgrade()
{
}

GuildUpgrade::~GuildUpgrade()
{
}

//-----------------------------------------------------------------------------
// 初始化
//-----------------------------------------------------------------------------
VOID GuildUpgrade::Init(Guild* pGuild, BOOL bRequest /*= FALSE*/)
{
	if (!P_VALID(pGuild))
	{
		ASSERT(pGuild);
		return;
	}

	m_bInit		= FALSE;
	m_pGuild	= pGuild;

	if (bRequest)
	{
		tagNDBC_LoadFacilitiesInfo send;
		send.dwGuildID = m_pGuild->GetGuildAtt().dwID;
		g_dbSession.Send(&send, send.dwSize);
	}
}

//-----------------------------------------------------------------------------
// 获取设施升级信息
//-----------------------------------------------------------------------------
DWORD GuildUpgrade::GetGuildFacilitiesInfo( tagGuildFacilitiesInfo* pInfo, EFacilitiesType eType /*= EFT_End*/ )
{
	// 上层须保证pInfo结构的大小
	if (!P_VALID(pInfo))
		return GT_INVALID;

	if (!m_bInit)
		return GT_INVALID;

	if (eType != EFT_End)
	{
		IFASTCODE->MemCpy(pInfo, &m_Facilities[eType], sizeof(tagGuildFacilitiesInfo));
	}
	else
	{
		for (int n=EFT_Guild; n<EFT_End; n++)
		{
			IFASTCODE->MemCpy(&pInfo[n], &m_Facilities[n], sizeof(tagGuildFacilitiesInfo));
		}
	}

	return 0;
}

//-----------------------------------------------------------------------------
// 上缴物品
//-----------------------------------------------------------------------------
DWORD GuildUpgrade::HandInItems( Role* pRole, EFacilitiesType eType )
{
	// 上层需要保证参数的合法性
	ASSERT(P_VALID(pRole));

	if (!m_bInit)
		return GT_INVALID;

	if (eType <= EFT_Null || eType >= EFT_End)
	{
		return GT_INVALID;
	}

	// 判断玩家等级
	if (pRole->GetLevel() < 20)
	{
		return E_GuildUpgrade_Role_Level;
	}

	tagGuildMember* pMember = m_pGuild->GetMember(pRole->GetID());
	if (!P_VALID(pMember))
	{
		return E_Guild_MemberNotIn;
	}

	// 判断该玩家的地位
	if (!m_pGuild->GetGuildPower(pMember->eGuildPos).bUpgrade)
	{
		return E_GuildWare_Power_NotEnough;
	}

	// 判断设施是否达到等级上限
	if (m_Facilities[eType].byLevel >= MAX_GUILD_LEVEL)
	{
		return E_GuildUpgrade_Level_Limit;
	}

	// 判断帮派状态和资金资材情况
	if (m_pGuild->IsInGuildStateAny(EGDSS_Shortage | EGDSS_Distress | EGDSS_Chaos | EGDSS_Warfare))
	{
		return E_Guild_State_Limit;
	}
	const tagGuild& sGuildAtt = m_pGuild->GetGuildAtt();
	if (sGuildAtt.nFund < m_Facilities[eType].nNeedFund)
	{
		return E_Guild_Fund_NotEnough;
	}
	if (sGuildAtt.nMaterial < m_Facilities[eType].nMaterial)
	{
		return E_Guild_Material_NotEnough;
	}

	// 检查玩家背包内物品是否达标
	for (int n=0; n<MAX_UPGRADE_NEED_ITEM_TYPE; n++)
	{
		INT32 nItemNum = pRole->GetItemMgr().GetBagSameItemCount(m_Facilities[eType].dwItemID[n]);
		if (nItemNum < m_Facilities[eType].nNeedNum[n])
		{
			return E_GuildUpgrade_Item_NotEnough;
		}
	}

	// 扣除帮派资金和资材
	m_pGuild->DecGuildFund(pRole->GetID(), m_Facilities[eType].nNeedFund, ELCLD_Guild_Upgrade);
	m_pGuild->DecGuildMaterial(pRole->GetID(), m_Facilities[eType].nMaterial, ELCLD_Guild_Upgrade);

	// 扣除玩家物品
	for (int n=0; n<MAX_UPGRADE_NEED_ITEM_TYPE; n++)
	{
		pRole->GetItemMgr().RemoveFromRole(m_Facilities[eType].dwItemID[n], m_Facilities[eType].nNeedNum[n], ELCLD_Guild_Upgrade);
	}

	// 提升进度
	m_Facilities[eType].nProgress += m_Facilities[eType].nStep;

	// Reset设施升级信息
	m_Facilities[eType].ResetItemInfo();

	// 提升玩家贡献值
	m_pGuild->ChangeMemberContribution(pRole->GetID(), m_Facilities[eType].nBaseExploit, TRUE);

	// 判断设施是否升级
	if (m_Facilities[eType].IsLevelUp())
	{
		// 随机读取下一个级别所需物品
		m_Facilities[eType].byLevel++;
		m_Facilities[eType].nProgress	= 0;
		if (m_Facilities[eType].byLevel >= MAX_GUILD_LEVEL)
		{
			// 重置设施属性
			m_Facilities[eType].byLevel		= MAX_GUILD_LEVEL;
			m_Facilities[eType].nFulfill	= 0;
			m_Facilities[eType].nNeedFund	= 0;
			m_Facilities[eType].nMaterial	= 0;
			m_Facilities[eType].nStep		= 0;
			m_Facilities[eType].nBaseExploit= 0;
		}
		else
		{
			g_attRes.GetGuildUpgradeItemInfo(eType, m_Facilities[eType].byLevel, m_Facilities[eType]);
		}
		if (eType == EFT_Guild)
		{
			m_pGuild->ReInitbyGuildUpgrade(m_Facilities[eType].byLevel);
		}

		// 广播帮派设施升级消息
		tagNS_GuildUpgrade send;
		send.eType		= eType;
		send.byNewLevel = m_Facilities[eType].byLevel;
		m_pGuild->SendGuildMsg(&send, send.dwSize);
	}
	else
	{
		// 随机读取本级别升级物品
		g_attRes.GetGuildUpgradeItemInfo(eType, m_Facilities[eType].byLevel, m_Facilities[eType]);
	}

	// 保存到数据库
	SaveUpgradeInfo2DB(eType);

	return 0;
}

//-----------------------------------------------------------------------------
// 保存到数据库
//-----------------------------------------------------------------------------
VOID GuildUpgrade::SaveUpgradeInfo2DB( EFacilitiesType eType )
{
	// 检查类型合法性
	if (eType >= EFT_End || eType <= EFT_Null)
	{
		return;
	}

	// 生成传输数据
	tagNDBC_FacilitiesUpgrade send;
	send.sFacilitiesInfo.dwGuildID	= m_pGuild->GetGuildAtt().dwID;
	send.sFacilitiesInfo			= m_Facilities[eType];

	g_dbSession.Send(&send, send.dwSize);
}

//-----------------------------------------------------------------------------
// 载入设施信息
//-----------------------------------------------------------------------------
BOOL GuildUpgrade::LoadFacilitiesInfo( const tagFacilitiesLoad* pInfo, INT nNum )
{
	if (!P_VALID(pInfo) || nNum != MAX_GUILD_FACILITIES_TYPE)
	{
		return FALSE;
	}

	for (int m=0; m<nNum; m++)
	{
		// 检查类型合法性
		EFacilitiesType eType = pInfo[m].eType;
		if (eType >= EFT_End || eType <= EFT_Null)
		{
			return FALSE;
		}

		// 载入数据库保存数据
		m_Facilities[eType].eType		= (EFacilitiesType)eType;
		m_Facilities[eType].byLevel		= pInfo[m].byLevel;
		if (eType == EFT_Guild)
		{
			m_pGuild->ReInitbyGuildUpgrade(pInfo[m].byLevel);
		}
		m_Facilities[eType].nProgress	= pInfo[m].n16Progress;
		for (int n=0; n<MAX_UPGRADE_NEED_ITEM_TYPE; n++)
		{
			m_Facilities[eType].dwItemID[n]	= pInfo[m].dwItemTypeID[n];
			m_Facilities[eType].nNeedNum[n]	= pInfo[m].nItemNeed[n];
		}

		// 载入设施基础属性
		if (!g_attRes.GetGuildUpgradeBaseInfo(eType, pInfo[m].byLevel, m_Facilities[eType]))
		{
			m_Facilities[eType].ResetItemInfo();
		}	
	}

	// 初始化完成
	m_bInit = TRUE;

	return TRUE;
}

//-----------------------------------------------------------------------------
// 创建设施
//-----------------------------------------------------------------------------
VOID GuildUpgrade::CreateFacilities()
{
	for (int n=EFT_Guild; n<EFT_End; n++)
	{
		// 创建World本地设施
		if (n == EFT_Guild)
		{
			m_Facilities[n].byLevel	= 1;
			m_pGuild->ReInitbyGuildUpgrade(1);
		}
		else
		{
			m_Facilities[n].byLevel = 0;
		}
		m_Facilities[n].nProgress	= 0;
		g_attRes.GetGuildUpgradeItemInfo((BYTE)n, m_Facilities[n].byLevel, m_Facilities[n]);

		// 通知数据库
		tagNDBC_CreateFacilities send;
		send.sFacilitiesInfo.dwGuildID	= m_pGuild->GetGuildAtt().dwID;
		send.sFacilitiesInfo			= m_Facilities[n];

		g_dbSession.Send(&send, send.dwSize);
	}

	// 初始化完成
	m_bInit = TRUE;
}

//-----------------------------------------------------------------------------
// 清除数据库所有设施信息
//-----------------------------------------------------------------------------
VOID GuildUpgrade::RemoveAllFacilitiesInfo()
{
	if (!m_bInit)
	{
		return;
	}

	tagNDBC_RemoveAllFacilities send;
	send.dwGuildID = m_pGuild->GetGuildAtt().dwID;

	g_dbSession.Send(&send, send.dwSize);
}

//-----------------------------------------------------------------------------
// 调整设施的等级
//-----------------------------------------------------------------------------
VOID GuildUpgrade::ChangeFacilitiesLevel(EFacilitiesType eType, BYTE byValue)
{
	m_Facilities[eType].byLevel = byValue;
	if (m_Facilities[eType].byLevel > MAX_GUILD_LEVEL)
		m_Facilities[eType].byLevel = MAX_GUILD_LEVEL;
	if (m_Facilities[eType].byLevel < 0)
		m_Facilities[eType].byLevel = 0;

	if (eType == EFT_Guild)
	{
		m_pGuild->ReInitbyGuildUpgrade(m_Facilities[EFT_Guild].byLevel);
	}

	// Reset设施升级信息
	m_Facilities[eType].ResetItemInfo();

	// 随机读取本级别升级物品
	g_attRes.GetGuildUpgradeItemInfo(eType, m_Facilities[eType].byLevel, m_Facilities[eType]);

	// 保存到数据库
	SaveUpgradeInfo2DB(eType);
}
