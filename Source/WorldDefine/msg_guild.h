//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: msg_guild.h
// author: 
// actor:
// data: 2009-04-03
// last:
// brief: 客户端和服务器端间消息 -- 帮派相关
//-----------------------------------------------------------------------------
#pragma once

#include "msg_common_errorcode.h"
#include "guild_define.h"

#pragma pack(push, 1)
//----------------------------------------------------------------------------
// 错误码枚举
//----------------------------------------------------------------------------
enum
{
	
};

//----------------------------------------------------------------------------
// 创建帮派
//----------------------------------------------------------------------------
CMD_START(NC_GuildCreate)
	DWORD		dwNPCID;
	TCHAR		szGuildName[1];
CMD_END

CMD_START(NS_GuildCreateBroad)	// 全服
	DWORD		dwRoleID;		// 创建者id
	DWORD		dwRoleNameID;	// 创建者名字id
	DWORD		dwGuildID;
	TCHAR		szName[1];		// 角色名+帮派名
CMD_END

//----------------------------------------------------------------------------
// 解散帮派
//----------------------------------------------------------------------------
CMD_START(NC_GuildDismiss)
	DWORD		dwNPCID;
CMD_END

CMD_START(NS_GuildDismissBroad)	// 全服
	DWORD		dwRoleID;
	TCHAR		szGuildName[1];
CMD_END

//----------------------------------------------------------------------------
// 邀请加入
//----------------------------------------------------------------------------
CMD_START(NC_GuildJoinReq)		// 玩家请求目标加入帮派
	DWORD		dwDstRoleID;
CMD_END

CMD_START(NS_GuildJoinReq)		// 服务器将请求发送给目标玩家
	DWORD		dwSrcRoleID;
	DWORD		dwGuildID;
CMD_END

CMD_START(NC_GuildJoinReqRes)	// 目标玩家反馈该请求
	DWORD		dwErrorCode;
	DWORD		dwDstRoleID;
	DWORD		dwDstGuildID;
CMD_END

CMD_START(NS_GuildJoinReqRes)	// 服务器转发给目标玩家反馈
	DWORD		dwErrorCode;
	DWORD		dwInviterID;
	DWORD		dwInviteeID;
CMD_END

CMD_START(NS_GuildJoinBroad)	// 帮派内
	DWORD		dwRoleID;
	DWORD		dwRoleNameID;
	TCHAR		szRoleName[1];
CMD_END

//------------------------------------------------------------------------------
// 玩家离开帮派
//------------------------------------------------------------------------------
CMD_START(NC_GuildLeave)
CMD_END

CMD_START(NS_GuildLeaveBroad)
	DWORD		dwSrcRoleID;
CMD_END

//------------------------------------------------------------------------------
// 开革玩家出帮
//------------------------------------------------------------------------------
CMD_START(NC_GuildKick)
	DWORD		dwRoleID;
CMD_END

CMD_START(NS_GuildKickBroad)
	DWORD		dwDstRoleID;
CMD_END

//------------------------------------------------------------------------------
// 移交帮主
//------------------------------------------------------------------------------
CMD_START(NC_GuildTurnover)
	DWORD		dwRoleID;
CMD_END

CMD_START(NS_GuildTurnoverBroad)
	DWORD		dwSrcRoleID;
	DWORD		dwDstRoleID;
CMD_END

//------------------------------------------------------------------------------
// 辞去职务
//------------------------------------------------------------------------------
CMD_START(NC_GuildDemiss)
CMD_END

CMD_START(NS_GuildDemissBroad)
	DWORD		dwRoleID;
	INT8		n8OldGuildPos;
CMD_END

//------------------------------------------------------------------------------
// 任命玩家职位
//------------------------------------------------------------------------------
CMD_START(NC_GuildAppoint)
	DWORD			dwRoleID;
	EGuildMemberPos	ePos;
CMD_END

CMD_START(NS_GuildAppointBroad)
	DWORD			dwSrcRoleID;
	DWORD			dwDstRoleID;
	EGuildMemberPos	ePos;
CMD_END

//------------------------------------------------------------------------------
// 修改宗旨
//------------------------------------------------------------------------------
CMD_START(NC_GuildTenetChange)
	TCHAR			szTenet[1];
CMD_END

CMD_START(NS_GuildTenetChangeBroad)
	DWORD			dwRoleID;
	TCHAR			szTenet[1];
CMD_END

//------------------------------------------------------------------------------
// 操作失败反馈
//------------------------------------------------------------------------------
CMD_START(NS_GuildProcFailed)
	DWORD			dwErrorCode;
CMD_END

//----------------------------------------------------------------------------
// 同步帮派信息
//----------------------------------------------------------------------------
CMD_START(NC_SyncGuildInfo)
CMD_END

CMD_START(NS_SyncGuildInfo)
	tagGuildBase	sGuildInfo;
CMD_END

//----------------------------------------------------------------------------
// 获取帮派所有成员
//----------------------------------------------------------------------------
CMD_START(NC_GuildGetAllMembers)
CMD_END

CMD_START(NS_GuildGetAllMembers)
	DWORD		dwErrorCode;
	INT16		n16Num;
	BYTE		byData[1];		// tagGuildMemInfo
CMD_END

//----------------------------------------------------------------------------
// 获取帮派指定成员扩展信息
//----------------------------------------------------------------------------
CMD_START(NC_GuildGetMemberEx)
	DWORD		dwRoleID;
CMD_END

CMD_START(NS_GuildGetMemberEx)
	DWORD				dwRoleID;
	tagGuildMemInfoEx	sGuildMemInfoEx;
CMD_END

//----------------------------------------------------------------------------
// 刷新帮派指定成员扩展信息
//----------------------------------------------------------------------------
CMD_START(NC_GuildRefMember)
	DWORD		dwRoleID;
CMD_END

CMD_START(NS_GuildRefMember)
	tagGuildMemInfo		sGuildMemInfo;
	tagGuildMemInfoEx	sGuildMemInfoEx;
CMD_END

//------------------------------------------------------------------------------
// 帮派成员贡献变更
//------------------------------------------------------------------------------
CMD_START(NS_GuildContribution)
	DWORD		dwRoleID;
	INT32		nContribution;
	INT32		nTotalContribution;
CMD_END

//------------------------------------------------------------------------------
// 帮派成员功勋变更
//------------------------------------------------------------------------------
CMD_START(NS_GuildExploit)
	DWORD		dwRoleID;
	INT32		nExploit;
CMD_END

//------------------------------------------------------------------------------
// 获取帮派名称
//------------------------------------------------------------------------------
CMD_START(NC_GetGuildName)
	DWORD		dwGuildID;
CMD_END

CMD_START(NS_GetGuildName)
	DWORD		dwGuildID;
	TCHAR		szGuildName[1];
CMD_END

CMD_START(NC_GetSomeGuildName)
INT		nUserData;
INT		nNum;		// id的数量
DWORD	dwAllID[1];	// 变长数组,存放dwNum个id
CMD_END

// 玩家ID和名字对应的结构
struct tagGuildIDName
{
	DWORD dwID;
	TCHAR szName[X_SHORT_NAME];
};

CMD_START(NS_GetSomeGuildName)
INT				nUserData;
INT				nNum;				// 名字数量
tagGuildIDName	IdName[1];
CMD_END

//-----------------------------------------------------------------------------
// 获取帮派遵旨（只用获取自己帮派的）
//-----------------------------------------------------------------------------
CMD_START(NC_GetGuildTenet)
CMD_END

CMD_START(NS_GetGuildTenet)
	TCHAR		szGuildTenet[1];
CMD_END

//-----------------------------------------------------------------------------
// 获取帮派仓库物品信息
//-----------------------------------------------------------------------------
CMD_START(NC_GetGuildWare)
	DWORD		dwNPCID;
	DWORD		dwLastUpdateTime;
CMD_END

CMD_START(NS_GetGuildWare)
	DWORD		dwErrorCode;
	DWORD		dwLastUpdateTime;
	INT16		nSzGuildWare;
	INT			nItemNum;
	BYTE		byData[1];
CMD_END

//-----------------------------------------------------------------------------
// 获取操作帮派仓库权限列表
//-----------------------------------------------------------------------------
CMD_START(NC_GetGuildWarePriList)
	DWORD		dwNPCID;
CMD_END

CMD_START(NS_GetGuildWarePriList)
	INT32			nNum;				// 列表中人数
	tagGuildWarePri	sGuildWarePri[1];	// 权限信息
CMD_END

//-----------------------------------------------------------------------------
// 变更操作帮派仓库权限
//-----------------------------------------------------------------------------
CMD_START(NC_GuildWarePri)
	DWORD		dwNPCID;
	DWORD		dwRoleID;
	BOOL		bCanUse;
CMD_END

CMD_START(NS_GuildWarePri)	//Board??
	DWORD		dwRoleID;
	BOOL		bCanUse;
CMD_END

//-----------------------------------------------------------------------------
// 帮派状态改变
//-----------------------------------------------------------------------------
CMD_START(NS_GuildStateSet)
	EGuildSpecState	eState;
CMD_END

CMD_START(NS_GuildStateUnset)
	EGuildSpecState	eState;
CMD_END

//-----------------------------------------------------------------------------
// 获取帮派设施升级信息
//-----------------------------------------------------------------------------
CMD_START(NC_GetGuildUpgradeInfo)	//??这里是否需要按时间更新机制
	DWORD					dwNPCID;
CMD_END

CMD_START(NS_GetGuildUpgradeInfo)	// 获取所有帮派设施信息
	tagGuildFacilitiesInfo	sFacilitiesInfo[MAX_GUILD_FACILITIES_TYPE];
CMD_END

CMD_START(NS_UpdateFacilitiesInfo)	// 更新某一个帮派设施信息
	tagGuildFacilitiesInfo	sFacilitiesInfo;
CMD_END

//-----------------------------------------------------------------------------
// 上缴帮派设施升级所需物品
//-----------------------------------------------------------------------------
CMD_START(NC_HandInItems)
	DWORD					dwNPCID;
	EFacilitiesType			eType;
CMD_END

//-----------------------------------------------------------------------------
// 广播设施升级消息
//-----------------------------------------------------------------------------
CMD_START(NS_GuildUpgrade)
	EFacilitiesType			eType;
	BYTE					byNewLevel;
CMD_END

//-----------------------------------------------------------------------------
// 帮派事务发布消息
//-----------------------------------------------------------------------------
CMD_START(NC_SpreadGuildAffair)
	DWORD					dwBuffID;
CMD_END

CMD_START(NS_SpreadGuildAffair)
	DWORD					dwRole;			// 发布者
	DWORD					dwBuffID;
	BYTE					byRemainTimes;	// 每日剩余发布次数
CMD_END

//-----------------------------------------------------------------------------
// 帮派技能消息
//-----------------------------------------------------------------------------
CMD_START(NC_GetGuildSkillInfo)
CMD_END

CMD_START(NS_GetGuildSkillInfo)
	DWORD			dwCurSkillID;		// 当前研究技能
	INT16			n16Progress;		// 当前研究进度
	INT				nSkillNum;			// 帮派技能数量
	DWORD			dwSkillInfo[1];		// 帮派技能等级
CMD_END

CMD_START(NC_GuildSkillUpgrade)
	INT64			n64ItemSerial;		// 上缴典籍64位ID
CMD_END

CMD_START(NS_GuildSkillUpgrade)			// 当前研究技能是否存在通过NS_SetResearchSkill发送
	DWORD			dwSkillID;			// 升级技能ID
	INT16			n16Progress;		// 升级进度
CMD_END

CMD_START(NS_GuildSkillLevelUp)
	DWORD			dwSkillID;
CMD_END

CMD_START(NC_LearnGuildSkill)
	DWORD			dwSkillID;			// 欲学技能类型ID
CMD_END

CMD_START(NS_LearnGuildSkill)
	DWORD			dwErrorCode;		// 帮派与技能错误码枚举重叠,单独设置此消息
	DWORD			dwSkillID;			// 学习技能的ID
CMD_END

CMD_START(NC_SetResearchSkill)
	DWORD			dwSkillID;			// 设置当前研究技能,取消研究为GT_INVALID
CMD_END

CMD_START(NS_SetResearchSkill)
	DWORD			dwSkillID;			// 返回当前研究技能ID
	INT16			n16Progress;		// 升级进度
CMD_END

//-----------------------------------------------------------------------------
// 帮派跑商相关
//-----------------------------------------------------------------------------
CMD_START(NC_GetCommodityInfo)			// 获取玩家商货信息
CMD_END

CMD_START(NS_GetCommodityInfo)			// (更新也用此消息)
	INT32				nCurTael;		// 当前商银数量
	INT					nLevel;			// 接取跑商时的等级
	INT					nGoodNum;		// 商货数量
	tagCommerceGoodInfo	sGoodInfo[1];	// 商货信息
CMD_END

CMD_START(NC_GetTaelInfo)				// 获取跑商初始信息
CMD_END

CMD_START(NS_GetTaelInfo)
	INT				nLevel;				// 接取时玩家等级
CMD_END

CMD_START(NC_GetCommerceRank)			// 获取跑商排名
CMD_END

CMD_START(NS_GetCommerceRank)
	BOOL			bCommend;			// 嘉奖状态
	INT				nRankNum;			// 上榜人数
	tagCommerceRank	sRankInfo[1];		// 排名信息
CMD_END

CMD_START(NC_AcceptCommerce)			// 接收跑商
CMD_END

CMD_START(NS_AcceptCommerce)
	INT32			nBeginningTael;		// 初始商银
CMD_END

CMD_START(NC_CompleteCommerce)			// 上缴商银
CMD_END

CMD_START(NS_CompleteCommerce)			// 上缴成功(广播?)
	DWORD			dwRoleID;			// 上缴人
	INT32			nFund;				// 贡献资金
CMD_END

CMD_START(NC_AbandonCommerce)			// 放弃跑商
CMD_END

CMD_START(NS_AbandonCommerce)			// 成功放弃
CMD_END

CMD_START(NC_SwitchCommendation)		// 开关跑商嘉奖
	BOOL			bSwitchON;
CMD_END

CMD_START(NS_SwitchCommendation)		// 广播嘉奖状态
	DWORD			dwRoleID;			// 设置人
	BOOL			bSwitchON;			// 嘉奖状态
CMD_END

//-----------------------------------------------------------------------------
// 跑商商会
//-----------------------------------------------------------------------------
CMD_START(NC_GetCofCInfo)				// 获取商会商货信息
	DWORD				dwNPCID;		// 商会ID
CMD_END

CMD_START(NS_GetCofCInfo)				// (更新也用此消息)
	DWORD				dwCofCID;		// 商会ID
	BYTE				byHoldCity;		// 特产商会(0表示一般商会)
	INT					nGoodNum;		// 售卖商货数量
	tagCommerceGoodInfo	sGoodInfo[1];	// 商货信息
CMD_END

CMD_START(NC_CloseCofC)					// 关闭商会
	DWORD				dwNPCID;		// 商会NPCID
CMD_END

CMD_START(NC_BuyCofCGoods)
	DWORD			dwNPCID;			// 商会NPCID
	DWORD			dwGoodID;			// 商货ID
	BYTE			byBuyNum;			// 购买数量
CMD_END

CMD_START(NC_SellCofCGoods)
	DWORD			dwNPCID;
	DWORD			dwGoodID;
	BYTE			bySellNum;
CMD_END

//------------------------------------------------------------------------------
#pragma pack(pop)