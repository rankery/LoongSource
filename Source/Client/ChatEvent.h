#pragma once

#pragma pack(push, 1)

struct tagPrivateChatEvent : public tagGameEvent
{
	DWORD	dwRoleID;
	tstring strRoleName;
	tagPrivateChatEvent(LPCTSTR szEventName, GameFrame* pSenderFrame) : 
	tagGameEvent(szEventName,pSenderFrame), dwRoleID( GT_INVALID )
	{}
};

struct tagSocialInfoEvent : public tagGameEvent
{
	tstring strInfo;
	tagSocialInfoEvent(LPCTSTR szEventName, GameFrame* pSenderFrame) :
	tagGameEvent(szEventName,pSenderFrame)
	{}
};

struct tagDungeonEvent : public tagGameEvent
{
	tstring strInfo;
	tagDungeonEvent(LPCTSTR szEventName, GameFrame* pSenderFrame) :
	tagGameEvent(szEventName,pSenderFrame)
	{}
};

struct tagChannelSetEvent : public tagGameEvent
{
	INT		nChannel;
	DWORD	nParam;
	tagChannelSetEvent(LPCTSTR szEventName, GameFrame* pSenderFrame) :
	tagGameEvent(szEventName,pSenderFrame)
	{}
};

#pragma pack(pop)