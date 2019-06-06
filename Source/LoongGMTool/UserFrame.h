#pragma once
#include "sfp_trunk.h"

class UserFrameMgr;
struct tagUserEvent;
class UserFrame
{
public:
	UserFrame(void);
	~UserFrame(void);
	
	virtual DWORD OnLoginEvent(tagUserEvent* pGameEvent);
	virtual DWORD OnServerStatInfoEvent(tagUserEvent* pGameEvent);
	//virtual DWORD OnDoublePageEvent(tagUserEvent* pGameEvent);
	//virtual DWORD OnYxjPageEvent(tagUserEvent* pGameEvent);
	//virtual DWORD OnPmdPageEvent(tagUserEvent* pGameEvent);
	//virtual DWORD OnZdxhPageEvent(tagUserEvent* pGameEvent);
	virtual DWORD OnGmMgrPageEvent(tagUserEvent* pGameEvent);
	virtual DWORD OnRolePageEvent(tagUserEvent* pGameEvent);
	virtual DWORD OnTimeLimited(tagUserEvent* pGameEvent);
	virtual DWORD OnAccountSuspendEvent(tagUserEvent* pGameEvent);
	virtual DWORD OnIPBlacklistEvent(tagUserEvent* pGameEvent);
	virtual DWORD OnAutoChatEvent(tagUserEvent* pGameEvent);

protected:
	TSFPTrunk<UserFrame>		m_Trunk;
	//UserFrameMgr*				m_UserFrameMgr;
};

