#pragma once
#include "..\WorldDefine\guild_define.h"
#include "GameClientDefine.h"
#include "GuildDefine.h"

class Item;

// 创建帮派最低要求
const INT CREATE_NEED_LEVEL		= 20;					// 创建帮派角色最低等级
const INT CREATE_NEED_GOLD		= 10;					// 创建帮派所需资金(单位：金)

struct tagGuildPosAppointEvent : public tagGameEvent
{
	DWORD			dwRoleID;
	EGuildMemberPos ePos;

	tagGuildPosAppointEvent(LPCTSTR szEventName, GameFrame* pSenderFrame) : tagGameEvent(szEventName,pSenderFrame)
	{dwRoleID = GT_INVALID;ePos = EGMP_End;}
};

struct tagGuildInfoReflashEvent : public tagGameEvent
{
	DWORD dwRoleID;

	tagGuildInfoReflashEvent(LPCTSTR szEventName, GameFrame* pSenderFrame) : tagGameEvent(szEventName,pSenderFrame)
	{dwRoleID = GT_INVALID;}
};

struct tagGuildNameGetEvent : public tagGameEvent
{
	DWORD	dwGuildID;
	tstring strName;

	tagGuildNameGetEvent(LPCTSTR szEventName, GameFrame* pSenderFrame) : tagGameEvent(szEventName, pSenderFrame)
	{dwGuildID = GT_INVALID; strName = _T("");}
};

struct tagInviteJoinGuildEvent : public tagGameEvent
{
	DWORD			dwRoleID;
	tagInviteJoinGuildEvent() : tagGameEvent( _T("GuildEvent_Invite"), NULL ), dwRoleID( GT_INVALID ){}
	tagInviteJoinGuildEvent(LPCTSTR szEventName, GameFrame* pSenderFrame) : tagGameEvent(szEventName,pSenderFrame), dwRoleID( GT_INVALID ){}
};

struct tagUseGuildSkillBookEvent : public tagGameEvent
{
	Item *pItem;
	tagUseGuildSkillBookEvent(LPCTSTR szEventName, GameFrame* pSenderFrame) : tagGameEvent(szEventName,pSenderFrame)
	{
		pItem = NULL;
	}
};

struct tagRefreshPickUpSilverEvent : public tagGameEvent
{
	const tagCommerceInfo *pCommerceInfo;
	tagRefreshPickUpSilverEvent(LPCTSTR szEventName, GameFrame* pSenderFrame) : tagGameEvent(szEventName,pSenderFrame)
	{
		pCommerceInfo = NULL;
	}
};

struct tagRefreshContributeSilverEvent : public tagGameEvent
{
	INT nLevel;
	const tagCommerceInfo *pCommerceInfo;
	tagRefreshContributeSilverEvent(LPCTSTR szEventName, GameFrame* pSenderFrame) : tagGameEvent(szEventName,pSenderFrame)
	{
		pCommerceInfo = NULL;
	}
};

struct tagRefreshGuildBusinessRankEvent : public tagGameEvent
{
	BOOL bCommend;
	std::vector<tagCommerceRank> vecRankInfo;
	tagRefreshGuildBusinessRankEvent(LPCTSTR szEventName, GameFrame* pSenderFrame) : tagGameEvent(szEventName,pSenderFrame)
	{
		bCommend = FALSE;
	}
};

struct tagRefreshBusinessRankCommendStateEvent : public tagGameEvent
{
	BOOL bCommend;
	tagRefreshBusinessRankCommendStateEvent(LPCTSTR szEventName, GameFrame* pSenderFrame) : tagGameEvent(szEventName,pSenderFrame)
	{
		bCommend = FALSE;
	}
};

struct tagGuildWarePriChange : public tagGameEvent
{
    DWORD       dwRoleID;
    BOOL        bCanUse;

    tagGuildWarePriChange(LPCTSTR szEventName, GameFrame* pSenderFrame)
        : tagGameEvent(szEventName, pSenderFrame){}
};
struct tagGetSomeGuildNameEvent : public tagGameEvent
{
	int nFlag;
	tagGetSomeGuildNameEvent(LPCTSTR szEventName, GameFrame *pSenderFrame) : tagGameEvent(szEventName, pSenderFrame){}
};