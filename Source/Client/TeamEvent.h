#pragma once

//*********************************由组队系统发出的游戏事件**********************************//

/** \添加队员
*/
struct tagAddTeammateEvent : public tagGameEvent
{
	DWORD dwRoleID;
	tagAddTeammateEvent() : tagGameEvent( _T("tagAddTeammateEvent"), NULL ), dwRoleID( GT_INVALID ){}
	tagAddTeammateEvent(LPCTSTR szEventName, GameFrame* pSenderFrame) : tagGameEvent(szEventName,pSenderFrame), dwRoleID( GT_INVALID ){}
};

/** \移除队员
*/
struct tagDelTeammateEvent : public tagGameEvent
{
	DWORD dwRoleID;
	tagDelTeammateEvent() : tagGameEvent( _T("tagDelTeammateEvent"), NULL ), dwRoleID( GT_INVALID ){}
	tagDelTeammateEvent(LPCTSTR szEventName, GameFrame* pSenderFrame) : tagGameEvent(szEventName,pSenderFrame), dwRoleID( GT_INVALID ){}
};

/** \离开队伍
*/
struct tagLeaveTeamEvent : public tagGameEvent
{
	tagLeaveTeamEvent() : tagGameEvent( _T("tagLeaveTeamEvent"), NULL ){}
	tagLeaveTeamEvent(LPCTSTR szEventName, GameFrame* pSenderFrame) : tagGameEvent(szEventName,pSenderFrame){}
};

/** \队长更换
*/
struct tagTeamLeaderChangeEvent : public tagGameEvent
{
	DWORD dwNewLeaderID;
	tagTeamLeaderChangeEvent() : tagGameEvent( _T("tagTeamLeaderChangeEvent"), NULL ), dwNewLeaderID( GT_INVALID ){}
	tagTeamLeaderChangeEvent(LPCTSTR szEventName, GameFrame* pSenderFrame) : tagGameEvent(szEventName,pSenderFrame), dwNewLeaderID( GT_INVALID ){}
};

/** \队友状态初始化
*/
struct tagTeammateStateInitEvent : public tagGameEvent
{
	DWORD				dwRoleID;
	tagTeammateStateInitEvent() : tagGameEvent( _T("tagTeammateStateInitEvent"), NULL ), dwRoleID( GT_INVALID ){}
	tagTeammateStateInitEvent(LPCTSTR szEventName, GameFrame* pSenderFrame) : tagGameEvent(szEventName,pSenderFrame), dwRoleID( GT_INVALID ){}
};


/** \队友等级改变
*/
struct tagTeammateLevelChangeEvent : public tagGameEvent
{
	DWORD			dwRoleID;
	INT             nLevel;
	tagTeammateLevelChangeEvent() : tagGameEvent( _T("tagTeammateLevelChangeEvent"), NULL ), dwRoleID( GT_INVALID ){}
	tagTeammateLevelChangeEvent(LPCTSTR szEventName, GameFrame* pSenderFrame) : tagGameEvent(szEventName,pSenderFrame), dwRoleID( GT_INVALID ){}
};

/** \队友名字更新
*/
struct tagTeammateNameUpdataEvent : public tagGameEvent
{
	DWORD dwRoleID;
	tstring strName;
	tagTeammateNameUpdataEvent() : tagGameEvent( _T("tagTeammateNameUpdataEvent"), NULL ), dwRoleID( GT_INVALID ){}
	tagTeammateNameUpdataEvent(LPCTSTR szEventName, GameFrame* pSenderFrame) : tagGameEvent(szEventName,pSenderFrame), dwRoleID( GT_INVALID ){}
};

/** \队友属性改变
*/
struct tagTeammateAttChangeEvent : public tagGameEvent
{
	DWORD			dwRoleID;
	ERemoteRoleAtt	eType;
	INT				nValue;
	tagTeammateAttChangeEvent() : tagGameEvent( _T("tagTeammateAttChangeEvent"), NULL ), dwRoleID( GT_INVALID ){}
	tagTeammateAttChangeEvent(LPCTSTR szEventName, GameFrame* pSenderFrame) : tagGameEvent(szEventName,pSenderFrame), dwRoleID( GT_INVALID ){}
};

//*********************************外部发给组队系统的游戏事件**********************************//

/** \组队邀请
*/
struct tagInviteJoinTeamEvent : public tagGameEvent
{
	DWORD			dwRoleID;
	tagInviteJoinTeamEvent() : tagGameEvent( _T("tagInviteJoinTeamEvent"), NULL ), dwRoleID( GT_INVALID ){}
	tagInviteJoinTeamEvent(LPCTSTR szEventName, GameFrame* pSenderFrame) : tagGameEvent(szEventName,pSenderFrame), dwRoleID( GT_INVALID ){}
};

/** \组队申请
*/
struct tagApplyJoinTeamEvent : public tagGameEvent
{
	DWORD			dwRoleID;
	tagApplyJoinTeamEvent() : tagGameEvent( _T("tagApplyJoinTeamEvent"), NULL ), dwRoleID( GT_INVALID ){}
	tagApplyJoinTeamEvent(LPCTSTR szEventName, GameFrame* pSenderFrame) : tagGameEvent(szEventName,pSenderFrame), dwRoleID( GT_INVALID ){}
};