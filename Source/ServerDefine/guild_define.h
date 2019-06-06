#pragma once

#include "../WorldDefine/guild_define.h"

#pragma pack(push, 1)
//-----------------------------------------------------------------------------
// 常量
//-----------------------------------------------------------------------------
const INT32 MAX_GUILD_INVITE_DELAY_TIME	= TICK_PER_SECOND * 60;	// 被邀请者响应时间
const INT32 MAX_GUILD_BUFF_SIZE			= 100 * 1000 * 1000;	// 帮派最大临时缓冲
const INT32 MAX_GUILDMEM_NUM_PER_MSG	= 10 * 1000;			// 每条消息中帮派成员的最多个数
const INT32 TEMP_GUILD_BUFF_SIZE		= 100 * 1000;			// 帮派临时缓冲

const INT32 MAX_GUILD_NAME_LEN			= 24;
const INT32 MAX_GUILD_TENET_LEN			= 400;
const INT32 MAX_GUILD_SYMBOL_LEN		= 200;

//-----------------------------------------------------------------------------
// 结构
//-----------------------------------------------------------------------------
// 帮派属性
struct tagGuild: public tagGuildBase
{
	tstring				strName;						//帮派名称
	tstring				strTenet;						//帮派宗旨
	tstring				strSymbol;						//帮派标志

	tagGuild(): tagGuildBase()	{}

	VOID Init(const tstring &strGuildName, DWORD dwGuildID, DWORD dwLeaderID, 
		DWORD dwCreatorNameID, BYTE byGuildLevel, INT16 n16GuildPeace, 
		INT32 nGuildFund, INT32 nGuildMaterial, INT32 nGuildRep, INT32 nGroupPurchase)
	{
		this->strName				= strGuildName;
		this->dwID					= dwGuildID;	
		this->dwLeaderRoleID		= dwLeaderID;
		this->dwFounderNameID		= dwCreatorNameID;
		this->byLevel				= byGuildLevel;
		this->n16Peace				= n16GuildPeace;
		this->nDailyCost			= (INT32)MGuildDailyCost(this->byLevel, this->n16Peace);
		this->nFund					= nGuildFund;
		this->nMaterial				= nGuildMaterial;
		this->nReputation			= nGuildRep;
		this->nGroupPurchase		= nGroupPurchase;
		this->byAffairRemainTimes	= MGuildAffairTimes(this->byLevel);
		this->bCommendation			= FALSE;
	}
};

// 创建帮派信息
struct tagCreateGuildInfo
{
	DWORD		dwGuildID;
	DWORD		dwCreateRoleNameID;

	INT32		nGuildRep;
	INT32		nGuildFund;
	INT32		nGuildMaterial;
	INT32		nGroupPurchase;

	INT16		n16GuildPeace;
	BYTE		byGuildLevel;
	BYTE		byAffairRemainTimes;
	TCHAR		szName[1];
};

// 新加入成员信息
struct tagCreateGuildMemInfo
{
	DWORD		dwRoleID;
	DWORD		dwGuildID;
	INT8		n8GuildPos;
};

// 邀请帖
struct tagInvite
{
	DWORD		dwInviterID;	// 邀请者id
	INT32		nInviteCountDown;

	tagInvite(DWORD dwInviterID)
	{
		this->dwInviterID		= dwInviterID;
		this->nInviteCountDown	= MAX_GUILD_INVITE_DELAY_TIME;
	}
};

//-----------------------------------------------------------------------------
// 初始连接后，读入的结构
//-----------------------------------------------------------------------------
struct tagGuildLoad: public tagGuildBase
{
	TCHAR			szName[MAX_GUILD_NAME_LEN];			// 帮派名称
	TCHAR			szTenet[MAX_GUILD_TENET_LEN];		// 帮派宗旨
	TCHAR			szSymbolURL[MAX_GUILD_SYMBOL_LEN];	// 帮派标志
};

struct tagGuildMemLoad
{
	DWORD			dwGuildID;
	tagGuildMember	sGuildMember;
};

//-----------------------------------------------------------------------------
// 帮派设施升级需求结构
//-----------------------------------------------------------------------------
struct tagItemNeedInfo 
{
	DWORD	dwItemTypeID;
	INT32	nItemNeedNum;

	BOOL IsValid()
	{
		return GT_VALID(dwItemTypeID) && GT_VALID(nItemNeedNum);
	}
};

struct tagGuildUpgradeNeed
{
	DWORD					dwKey;
	INT16					n16FullFill;
	INT16					n16Step;
	INT32					nFund;
	INT32					nMaterial;
	INT32					nBaseExploit;
	TList<tagItemNeedInfo>	listItemInfo;

	tagGuildUpgradeNeed()
	{
		n16FullFill		= 0;
		n16Step			= 0;
		nFund			= 0;
		nMaterial		= 0;
		nBaseExploit	= 0;
		listItemInfo.Clear();
	}
};

//-----------------------------------------------------------------------------
// 帮派设施传输用结构
//-----------------------------------------------------------------------------
struct tagFacilitiesLoad
{
	EFacilitiesType	eType;
	DWORD			dwGuildID;
	DWORD			dwItemTypeID[MAX_UPGRADE_NEED_ITEM_TYPE];
	INT32			nItemNeed[MAX_UPGRADE_NEED_ITEM_TYPE];
	INT16			n16Progress;
	BYTE			byLevel;

	tagFacilitiesLoad()
	{
		ZeroMemory(this, sizeof(*this));
	}

	tagFacilitiesLoad& operator= (tagGuildFacilitiesInfo& sInfo)
	{
		eType		= sInfo.eType;
		byLevel		= sInfo.byLevel;
		n16Progress	= sInfo.nProgress;

		for (int n=0; n<MAX_UPGRADE_NEED_ITEM_TYPE; n++)
		{
			dwItemTypeID[n]	= sInfo.dwItemID[n];
			nItemNeed[n]	= sInfo.nNeedNum[n];
		}

		return *this;
	}
};

//-----------------------------------------------------------------------------
// 帮派事务信息
//-----------------------------------------------------------------------------
struct tagGuildAffairInfo 
{
	DWORD		dwBuffID;
	INT32		nFund;
	INT32		nMaterial;
	BYTE		byGuildLevel;
	BYTE		byHoldCity;
};

//-----------------------------------------------------------------------------
// 帮派技能信息
//-----------------------------------------------------------------------------
struct tagGuildSkillInfo
{
	DWORD		dwSkillID;
	INT			nLevel;
	BOOL		bResearching;	// 只在载入时使用
	INT16		n16Progress;
};

//-----------------------------------------------------------------------------
// 帮派跑商信息
//-----------------------------------------------------------------------------
struct tagRedoundInfo
{
	INT32	nExp;			// 经验奖励
	INT32	nContribution;	// 帮派贡献奖励
	INT32	nExploit;		// 帮派阅历奖励(未实现)
};

struct tagCommerceInfo		// 跑商任务静态信息
{
	DWORD			dwID;	// 等级范围
	tagTaelInfo		sTaelInfo;
	tagRedoundInfo	sRedoundInfo;
};

struct tagGuildCommerceInfo 
{
	DWORD				dwRoleID;
	INT					nLevel;
	INT32				nTael;
	tagCommerceGoodInfo	sGoodInfo[MAX_COMMODITY_CAPACITY];
};

//-----------------------------------------------------------------------------
// 跑商用静态信息
//-----------------------------------------------------------------------------
struct tagCommodityProto 
{
	DWORD				dwGoodID;
	INT32				nLowPrice;
	INT32				nHighPrice;
	BYTE				byRefreshNum;
	BYTE				byHolderID;
};

struct tagCofCProto 
{
	INT64				n64Key;
	FLOAT				fProfit;
};

struct tagCofCSPProto
{
	DWORD				dwCofCID;
	BYTE				byHolderID;
};

//-----------------------------------------------------------------------------
#pragma pack(pop)