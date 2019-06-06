/********************************************************************
	created:	2008/12/31
	created:	31:12:2008   9:44
	filename: 	d:\loong_client\Client\SocialEvent.h
	file path:	d:\loong_client\Client
	file base:	SocialEvent
	file ext:	h
	author:		leili
	
	purpose:	好友系统游戏事件定义
*********************************************************************/
#pragma once

#pragma pack(push, 1)

// 加为好友
struct tagAddFriendEvent : public tagGameEvent
{
	DWORD	roleID;		// 玩家ID

	tagAddFriendEvent(LPCTSTR szEventName, GameFrame* pSenderFrame)
		: tagGameEvent(szEventName,pSenderFrame)
		, roleID(GT_INVALID)
	{ } 
};

// 加入黑名单
struct tagAddBlackEvent : public tagGameEvent
{
	DWORD	roleID;		// 玩家ID

	tagAddBlackEvent(LPCTSTR szEventName, GameFrame* pSenderFrame)
		: tagGameEvent(szEventName,pSenderFrame)
		, roleID(GT_INVALID)
	{ } 
};

// 使用礼物类物品
struct tagUseGiftEvent : public tagGameEvent
{
	INT64	giftGlobalID;
	DWORD	giftTypeID;

	tagUseGiftEvent(LPCTSTR szEventName, GameFrame* pSenderFrame)
		: tagGameEvent(szEventName,pSenderFrame)
		, giftGlobalID(GT_INVALID)
		, giftTypeID(GT_INVALID)
	{ } 
};

// 查看玩家信息
struct tagDispPlayerState : public tagGameEvent
{
    DWORD   dwRoleID;     //!< 玩家ID

    tagDispPlayerState(LPCTSTR szEventName, GameFrame* pSenderFrame)
        : tagGameEvent(szEventName, pSenderFrame)
        , dwRoleID(GT_INVALID) {}
};

#pragma pack(pop)