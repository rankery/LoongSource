//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: msg_guild.h
// author: 
// actor:
// data: 2009-04-13
// last:
// brief: 帮派相关
//-----------------------------------------------------------------------------
#pragma once

#include "guild_define.h"

#pragma pack(push, 1)
//-----------------------------------------------------------------------------
// 常量
//-----------------------------------------------------------------------------
CONST INT32 MAX_LOAD_GUILDMEMBER_NUM_ONCE			= 1000;		// 一次性最多返回帮派成员个数

//-----------------------------------------------------------------------------
// 服务器连接后，读取相关内容
//-----------------------------------------------------------------------------
CMD_START(NDBC_LoadAllGuild)
CMD_END

CMD_START(NDBS_LoadAllGuild)	// 每次返回一个帮派信息
	tagGuildLoad	sGuildLoad;
CMD_END

CMD_START(NDBC_LoadAllGuildMember)
CMD_END

CMD_START(NDBS_LoadAllGuildMember)
	INT				nAllGuildNum;
	tagGuildMemLoad	sGuildMemberLoad[1];
CMD_END

CMD_START(NDBS_GuildMemberInitOK)
CMD_END

CMD_START(NDBC_GuildInitOK)
CMD_END

CMD_START(NDBS_GuildInitOK)
CMD_END

//-----------------------------------------------------------------------------
// 关闭游戏服务器时，保存的帮派属性
//-----------------------------------------------------------------------------
CMD_START(NDBC_SaveAllGuild)
	INT				nGuildNum;
	tagGuildBase	pGuildBase[1];
CMD_END

//-----------------------------------------------------------------------------
// 属性变更时保存的帮派属性
//-----------------------------------------------------------------------------
CMD_START(NDBC_SaveGuildAtt)
	tagGuildBase	pGuildBase;
CMD_END

//-----------------------------------------------------------------------------
// 帮派属性
//-----------------------------------------------------------------------------
CMD_START(NDBC_CreateGuild)		// 创建帮派
	tagCreateGuildInfo	sCreateGuildInfo;
CMD_END

CMD_START(NDBS_CreateGuild)		// 创建帮派 -- 数据库创建失败才会返回//??还未反馈
	DWORD		dwErrorCode;	
	DWORD		dwGuildID;
CMD_END

CMD_START(NDBC_DismissGuild)
	DWORD		dwGuildID;
CMD_END

CMD_START(NDBC_ChangeGuildTenet)	// 修改帮派宗旨
	DWORD		dwGuildID;
	TCHAR		szNewTenet[1];
CMD_END


//-----------------------------------------------------------------------------
// 帮派成员
//-----------------------------------------------------------------------------
CMD_START(NDBC_JoinGuild)			// 加入帮派
	tagCreateGuildMemInfo	sGuildMemInfo;
CMD_END

CMD_START(NDBC_LeaveGuild)			// 离开帮派或被踢
	DWORD		dwRoleID;
CMD_END

CMD_START(NDBC_ChangeGuildPos)		// 帮派职位改变
	DWORD		dwRoleID;
	INT8		n8NewPos;
CMD_END

CMD_START(NDBC_ChangeContrib)		// 帮派贡献改变
	DWORD		dwRoleID;
	INT32		nContribution;		// 变化后的量
	INT32		nTotalContribution;
CMD_END

CMD_START(NDBC_ChangeExploit)
	DWORD		dwRoleID;
	INT32		nExploit;
CMD_END

//CMD_START(NDBC_ChangeGuildExploit)	// 帮派功勋改变
//	DWORD		dwRoleID;
//	INT32		nNewExploit;
//CMD_END
//
//CMD_START(NDBC_ChangeGuildSalary)	// 帮派工资改变
//	DWORD		dwRoleID;
//	INT32		nNewSalary;
//CMD_END

//-----------------------------------------------------------------------------
// 帮派仓库
//-----------------------------------------------------------------------------
CMD_START(NDBC_LoadGuildWareItems)
	DWORD			dwGuildID;
CMD_END

CMD_START(NDBS_LoadGuildWareItems)
	DWORD			dwGuildID;
	INT				nItemNum;
	BYTE			byData[1];
CMD_END

CMD_START(NDBC_GuildWarePri)		// 帮派仓库权限变更
	DWORD			dwRoleID;
	BOOL			bEnable;
CMD_END

CMD_START(NDBC_SaveGuildWare)
	INT				nItemNum;
	BYTE			byData[1];
CMD_END

//-----------------------------------------------------------------------------
// 帮派设施
//-----------------------------------------------------------------------------
CMD_START(NDBC_LoadFacilitiesInfo)
	DWORD				dwGuildID;
CMD_END

CMD_START(NDBS_LoadFacilitiesInfo)
	DWORD				dwGuildID;
	INT32				nInfoNum;
	tagFacilitiesLoad	sFacilitiesInfo[1];
CMD_END

CMD_START(NDBC_CreateFacilities)
	tagFacilitiesLoad	sFacilitiesInfo;
CMD_END

CMD_START(NDBC_FacilitiesUpgrade)
	tagFacilitiesLoad	sFacilitiesInfo;
CMD_END

CMD_START(NDBC_RemoveAllFacilities)
	DWORD				dwGuildID;
CMD_END

//-----------------------------------------------------------------------------
// 帮派技能
//-----------------------------------------------------------------------------
CMD_START(NDBC_LoadGuildSkillInfo)
	DWORD				dwGuildID;
CMD_END

CMD_START(NDBS_LoadGuildSkillInfo)
	DWORD				dwGuildID;
	INT					nInfoNum;
	tagGuildSkillInfo	sGuildSkillInfo[1];
CMD_END

CMD_START(NDBC_CreateGuildSkill)
	DWORD				dwGuildID;
	tagGuildSkillInfo	sSkillInfo;
CMD_END

CMD_START(NDBC_SaveGuildSkill)
	DWORD				dwGuildID;
	tagGuildSkillInfo	sSkillInfo;
CMD_END

CMD_START(NDBC_ChangeResearchSkill)
	DWORD				dwGuildID;
	DWORD				dwNewSkillID;
CMD_END

CMD_START(NDBC_RemoveGuildSkill)
	DWORD				dwGuildID;
CMD_END

//-----------------------------------------------------------------------------
// 帮派跑商
//-----------------------------------------------------------------------------
CMD_START(NDBC_LoadCommodity)
	DWORD					dwGuildID;
CMD_END

CMD_START(NDBS_LoadCommodity)
	DWORD					dwGuildID;
	INT						nCommodityNum;
	tagGuildCommerceInfo	sCommerceInfo[1];
CMD_END

CMD_START(NDBC_LoadCommerceRank)
	DWORD					dwGuildID;
CMD_END

CMD_START(NDBS_LoadCommerceRank)
	DWORD					dwGuildID;
	INT						nRankNum;
	tagCommerceRank			sRankInfo[1];
CMD_END

CMD_START(NDBC_CreateCommodity)		// 初始商货容器为空
	DWORD				dwGuildID;
	DWORD				dwRoleID;
	INT					nLevel;
	INT32				nTael;
CMD_END

CMD_START(NDBC_ChangeTael)
	DWORD				dwRoleID;
	INT32				nTael;
CMD_END

CMD_START(NDBC_SaveCommodity)
	DWORD				dwRoleID;
	tagCommerceGoodInfo	sGoodInfo[MAX_COMMODITY_CAPACITY];
CMD_END

CMD_START(NDBC_RemoveCommodity)
	DWORD				dwRoleID;
CMD_END

CMD_START(NDBC_SetCommendation)
	DWORD				dwGuildID;
	BOOL				bCommend;
CMD_END

CMD_START(NDBC_UpdateCommerceRank)
	DWORD				dwGuildID;
	tagCommerceRank		sRankInfo;
CMD_END

CMD_START(NDBC_RemoveCommerceRank)
	DWORD				dwGuildID;
	DWORD				dwRoleID;
CMD_END

//-----------------------------------------------------------------------------
#pragma pack(pop)