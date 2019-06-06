#include "stdafx.h"
#include "UserFrameMgr.h"

UserFrameMgr::UserFrameMgr()
{
	m_mapHandle.clear();
}

UserFrameMgr::~UserFrameMgr()
{
	Destroy();
}


//-----------------------------------------------------------------------------
// init
//-----------------------------------------------------------------------------
BOOL UserFrameMgr::Init()
{
	return TRUE;
}

//-----------------------------------------------------------------------------
// destroy
//-----------------------------------------------------------------------------
BOOL UserFrameMgr::Destroy()
{
	std::map<DWORD, tagEventHandle*>::iterator it=m_mapHandle.begin();
	for( ; it!=m_mapHandle.end(); )
	{
		SAFE_DEL(it->second);
		it = m_mapHandle.erase(it);
	}
	return TRUE;
}

//-----------------------------------------------------------------------------
// 注册Frame事件处理函数
//-----------------------------------------------------------------------------
BOOL UserFrameMgr::RegisterEventHandle(LPCTSTR szEvent, FRAMEEVENTPROC fp)
{
	DWORD dwID = Crc32(szEvent);
	std::map<DWORD, tagEventHandle*>::iterator it = m_mapHandle.find(dwID);
	if( it!= m_mapHandle.end() )
	{
		return FALSE;
	}

	tagEventHandle* pHandle = new tagEventHandle;
	pHandle->strName = szEvent;
	pHandle->fp = fp;

	m_mapHandle.insert(std::make_pair(dwID, pHandle));
	return TRUE;
}


//-----------------------------------------------------------------------------
// 注销事件处理函数
//-----------------------------------------------------------------------------
BOOL UserFrameMgr::UnRegisterEventHandler(LPCTSTR szEvent, FRAMEEVENTPROC fp)
{
	DWORD dwID = Crc32(szEvent);
	std::map<DWORD, tagEventHandle*>::iterator it = m_mapHandle.find(dwID);
	if( it==m_mapHandle.end() )
		return FALSE;

	m_mapHandle.erase(it);
	return TRUE;
}


//-----------------------------------------------------------------------------
// 调用事件处理函数
//-----------------------------------------------------------------------------
VOID UserFrameMgr::SendEvent( tagUserEvent* pEvent )
{
	std::map<DWORD, tagEventHandle*>::iterator it = m_mapHandle.find(pEvent->dwNameCrc);
	if( it==m_mapHandle.end() )
		return;

	it->second->fp(pEvent);
}