#pragma once

#pragma pack(push, 1)

//打开合成界面的方式
enum EStartCompose
{
	EStartCompose_NULL = 0,
	EStartCompose_Skill,
	EStartCompose_NPC,	
	EStartCompose_Item,
};

//
typedef union
{
	DWORD			npcID;				// NPC全局id 
	DWORD			skillID;			// 技能id
	INT64			itemID;				// 物品64id
}ComposeID;

struct tagStartComposeUIEvent 
	: public tagGameEvent
{
	EStartCompose	eStartCompose;	
	ComposeID		uComposeID;

	tagStartComposeUIEvent(LPCTSTR szEventName, GameFrame* pSenderFrame)
		: tagGameEvent(szEventName,pSenderFrame)
	{ 
	} 
};

#pragma pack(pop)