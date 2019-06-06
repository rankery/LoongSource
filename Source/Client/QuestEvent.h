/********************************************************************
	created:	2008/09/14
	created:	14:9:2008   14:20
	filename: 	d:\loong_client\Client\QuestEvent.h
	file path:	d:\loong_client\Client
	file base:	QuestEvent
	file ext:	h
	author:		leili
	
	purpose:	任务系统游戏事件定义
*********************************************************************/
#pragma once

#pragma pack(push, 1)

struct tagStartNPCTalkEvent : public tagGameEvent
{
	DWORD	globalID;	// NPC全局ID
	DWORD	typeID;		// NPC属性ID

	tagStartNPCTalkEvent(LPCTSTR szEventName, GameFrame* pSenderFrame)
		: tagGameEvent(szEventName,pSenderFrame)
		, globalID(GT_INVALID)
		, typeID(GT_INVALID)
	{ } 
};

struct tagUseQuestEvent : public tagGameEvent
{
	DWORD	itemID;		// 物品TypeID
	UINT16	questID;	// 任务编号

	tagUseQuestEvent(LPCTSTR szEventName, GameFrame* pSenderFrame)
		: tagGameEvent(szEventName,pSenderFrame)
		, itemID(GT_INVALID)
		, questID(GT_INVALID)
	{ }
};

#pragma pack(pop)