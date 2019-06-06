#pragma once

#pragma pack(push, 1)

struct tagSkillProgressEvent 
	: public tagGameEvent
{
	bool		bShow;
	bool		bSkill;
	DWORD		dwTypeID;
	DWORD		dwTime;
	tagSkillProgressEvent(LPCTSTR szEventName, GameFrame* pSenderFrame)
		: tagGameEvent(szEventName,pSenderFrame)
	{} 
};

#pragma pack(pop)