#pragma once

#pragma pack(push, 1)

enum EShortcutUseSkillState 
{
	ESUSS_NULL = 0,
	ESUSS_Cache,				//缓存技能
	ESUSS_CacheUsed,			//缓存技能已经使用
};

struct tagShortcutCacheEvent : public tagGameEvent
{
	DWORD					dwSkillID;		//技能ID
	EShortcutUseSkillState	eType;			//状态
	tagShortcutCacheEvent(LPCTSTR szEventName, GameFrame* pSenderFrame) : 
	tagGameEvent(szEventName, pSenderFrame), dwSkillID( GT_INVALID ){}
};

#pragma pack(pop)