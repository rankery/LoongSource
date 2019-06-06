//-----------------------------------------------------------------------------
//!\file thread.cpp
//!\author Lyp
//!
//!\date 2004-08-09
//! last 2004-08-09
//!
//!\brief 线程管理工具
//-----------------------------------------------------------------------------
#include "..\stdafx.h"
#include "thread.h"


namespace vEngine {
//-----------------------------------------------------------------------------
//! construction
//-----------------------------------------------------------------------------
Thread::Thread()
{
	m_mapHandle.Clear();
}

//-----------------------------------------------------------------------------
//! destruction
//-----------------------------------------------------------------------------
Thread::~Thread()
{
	ASSERT(m_mapHandle.Size() == 0);
}

//-----------------------------------------------------------------------------
// create thread
//-----------------------------------------------------------------------------
BOOL Thread::CreateThread(LPCTSTR szName, THREADPROC pFn, LPVOID Param)
{
	DWORD dwID = m_pUtil->Crc32(szName);
	if( m_mapHandle.IsExist(dwID) )
	{
		ERR(_T("thread with same name %s is already exist"), szName);
		return FALSE;
	}

	HANDLE hHandle = (HANDLE)_beginthreadex
		(NULL, 0, pFn, Param, 0, NULL);
	
	if( 0 == hHandle )	// MSDN: _beginthreadex returns 0 on an error
	{
//		ERR(_T("create thread %s failed"), szName );
		return FALSE;
	}

	m_mapHandle.Add(dwID, hHandle);
	return TRUE;
}


//-----------------------------------------------------------------------------
// destroy thread
//-----------------------------------------------------------------------------
BOOL Thread::WaitForThreadDestroy(LPCTSTR szName, DWORD dwTimeOut)
{
	DWORD dwID = m_pUtil->Crc32(szName);
	if( FALSE == m_mapHandle.IsExist(dwID) )
	{
		ERR(_T("thread %s is NOT exist"), szName);
		return FALSE;
	}

	HANDLE hHandle = m_mapHandle.Peek(dwID);
	DWORD dwResult = 0;
	if( hHandle )
	{
		dwResult = ::WaitForSingleObject( hHandle, dwTimeOut );
		switch( dwResult )
		{
		case WAIT_OBJECT_0:
			SAFE_CLOSE_HANDLE( hHandle );
			m_mapHandle.Erase( dwID );
			return TRUE;
			break;

		case WAIT_TIMEOUT:
			ERR(_T("wait thread %s to destroy time out"), szName);
			break;

		default:
			ERR(_T("error in wait for thread %s to destroy"), szName);
			break;
		}
	}

	return FALSE;
}


//-----------------------------------------------------------------------------
// destroy all thread 永远等待
//-----------------------------------------------------------------------------
VOID Thread::WaitForAllThreadDestroy()
{
	std::list<HANDLE> listHandle;
	INT nThreadNum = m_mapHandle.ExportAllValue(listHandle);
	
	for(INT n=0; n<nThreadNum; n++)
	{
		HANDLE hHandle = listHandle.front();
		listHandle.pop_front();
		if( hHandle )
		{
			::WaitForSingleObject( hHandle, INFINITE );
		}
	}

	m_mapHandle.Clear();
}


//-----------------------------------------------------------------------------
// 检查县程是否运行中
//-----------------------------------------------------------------------------
BOOL Thread::IsThreadActive(LPCTSTR szName)
{
	DWORD dwID = m_pUtil->Crc32(szName);
	if( FALSE == m_mapHandle.IsExist(dwID) )
		return FALSE;

	HANDLE hHandle = m_mapHandle.Peek(dwID);
	DWORD dwResult = 0;
	if( hHandle )
	{
		dwResult = ::WaitForSingleObject( hHandle, 0 );
		switch( dwResult )
		{
		case WAIT_OBJECT_0:
			return FALSE;
			break;

		case WAIT_TIMEOUT:
			return TRUE;
			break;

		default:
			return FALSE;
			break;
		}
	}

	return FALSE;
}


//-----------------------------------------------------------------------------
// 得到指定线程的handle
//-----------------------------------------------------------------------------
HANDLE Thread::GetThreadHandle(LPCTSTR szName)
{ 
	return m_mapHandle.Peek(m_pUtil->Crc32(szName)); 
}

} // namespace vEngine {
