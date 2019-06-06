//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: guild_upgrade.h
// author: Sunnee
// actor:
// data: 2009-07-03
// last:
// brief: 帮派升级和帮派设施升级
//-----------------------------------------------------------------------------
#pragma once

#include "../WorldDefine/guild_define.h"

#pragma pack(push, 1)

//-----------------------------------------------------------------------------
// 帮派设施结构
//-----------------------------------------------------------------------------
struct tagGuildFacilities : public tagGuildFacilitiesInfo
{
	INT16	nStep;
	INT32	nBaseExploit;

	tagGuildFacilities()
	{
		ZeroMemory(this, sizeof(*this));
	}
};

struct tagFacilitiesLoad;
class Guild;

class GuildUpgrade
{
public:
	GuildUpgrade();
	~GuildUpgrade();

	VOID	Init(Guild* pGuild, BOOL bRequest = FALSE);
	BOOL	IsInitOK()	{ return m_bInit; }

	// 重新创建缺失设施
	VOID	CreateFacilities();

	// 载入设施信息
	BOOL	LoadFacilitiesInfo(const tagFacilitiesLoad* pInfo, INT nNum);

	// 获取设施信息接口
	DWORD	GetGuildFacilitiesInfo(tagGuildFacilitiesInfo* pInfo, EFacilitiesType eType = EFT_End);

	// 上缴物品接口
	DWORD	HandInItems(Role* pRole, EFacilitiesType eType);

	// 保存到数据库
	VOID	SaveUpgradeInfo2DB(EFacilitiesType eType);

	// 清除数据库中所有设施信息
	VOID	RemoveAllFacilitiesInfo();
	
	// 调整帮派设施的等级
	VOID	ChangeFacilitiesLevel(EFacilitiesType eType, BYTE byValue);

	// 上限获取
	INT32	GetMaxFund()		{ return 2000000*m_Facilities[EFT_Guild].byLevel + 2000000*m_Facilities[EFT_Fund].byLevel; }
	INT32	GetMaxMaterial()	{ return 10000*m_Facilities[EFT_Guild].byLevel + 20000*m_Facilities[EFT_Material].byLevel; }
	BYTE	GetMaxSkillLevel()	{ return 20 + (2 * m_Facilities[EFT_Academy].byLevel); }

private:
	BOOL					m_bInit;
	Guild*					m_pGuild;

	tagGuildFacilities		m_Facilities[EFT_End];
};

//-----------------------------------------------------------------------------
#pragma pack(pop)