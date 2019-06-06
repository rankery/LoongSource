#include "stdafx.h"
#include "UserFrame.h"
#include "UserFrameMgr.h"

UserFrame::UserFrame( void )
{
	//m_UserFrameMgr = new UserFrameMgr;
	m_Trunk = this;
}

UserFrame::~UserFrame( void )
{
	/*if( P_VALID(m_UserFrameMgr) )
		SAFE_DEL(m_UserFrameMgr);*/
}


DWORD UserFrame::OnLoginEvent( tagUserEvent* pGameEvent )
{
	return 0;
}

//DWORD UserFrame::OnYxjPageEvent( tagUserEvent* pGameEvent )
//{
//	return 0;
//}

//DWORD UserFrame::OnPmdPageEvent( tagUserEvent* pGameEvent )
//{
//	return 0;
//}

//DWORD UserFrame::OnZdxhPageEvent( tagUserEvent* pGameEvent )
//{
//	return 0;
//}

DWORD UserFrame::OnGmMgrPageEvent( tagUserEvent* pGameEvent )
{
	return 0;
}

DWORD UserFrame::OnServerStatInfoEvent( tagUserEvent* pGameEvent )
{
	return 0;
}

DWORD UserFrame::OnRolePageEvent( tagUserEvent* pGameEvent )
{
	return 0;
}

DWORD UserFrame::OnTimeLimited( tagUserEvent* pGameEvent )
{
	return 0;
}

DWORD UserFrame::OnAccountSuspendEvent(tagUserEvent* pGameEvent)
{
	return 0;
}

DWORD UserFrame::OnIPBlacklistEvent(tagUserEvent* pGameEvent)
{
	return 0;
}

DWORD UserFrame::OnAutoChatEvent(tagUserEvent* pGameEvent)
{
	return 0;
}
//DWORD UserFrame::OnDoublePageEvent( tagUserEvent* pGameEvent )
//{
//	return 0;
//}