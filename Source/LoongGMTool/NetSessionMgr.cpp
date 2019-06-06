#include "stdafx.h"
#include "NetSession.h"
#include "NetSessionMgr.h"

NetSessionMgr::NetSessionMgr( void )
{
	m_eState		= ELS_NULL;
	m_ePrivilege	= EP_Null;

}

NetSessionMgr::~NetSessionMgr( void )
{
	this->Destroy();
}

bool NetSessionMgr::Init()
{
	m_pSession = new NetSession;
	if( P_VALID(m_pSession) )
		m_pSession->Init();
	return true;
}

void NetSessionMgr::Destroy()
{
	//结束Connect线程
	::InterlockedExchange((LONG*)&thread_update_terminate_, TRUE);
	DWORD result = WaitForSingleObject(thread_update_handle_, INFINITE);
	if (thread_update_handle_ != NULL)
	{
		CloseHandle(thread_update_handle_);
	}

	if( P_VALID(m_pSession) )
	{
		SAFE_DEL(m_pSession);
	}

	m_mapPrivilege.clear();
}

void NetSessionMgr::CreateThread()
{
	//启动update线程
	thread_update_handle_ = (HANDLE)_beginthreadex(NULL, 0, (THREADPROC)&NetSessionMgr::ThreadNetMsgUpdate, this, 0, NULL);
	if(thread_update_handle_ == NULL)
	{
		CloseHandle(thread_update_handle_);
		IMSG(_T("Create update thread error!"));
			thread_update_handle_ = 0;
	}
}

UINT NetSessionMgr::ThreadNetMsgUpdate( LPVOID param )
{
	NetSessionMgr *pNetSessionMgr = (NetSessionMgr*)param;
	if( !P_VALID(pNetSessionMgr) )
	{
		return 0;
	}
	
	NetSession* netSession = pNetSessionMgr->GetNetSessionPtr();
	if( !P_VALID(netSession) )
	{
		return 0;
	}

	// 不停收网络消息
	for(;;)
	{
		if( pNetSessionMgr->thread_update_terminate_ == TRUE )
			break;

		if( !P_VALID(netSession) )
			break;

		netSession->RecvMessage();
		Sleep(100);	
	}
	return 0;
}

BOOL NetSessionMgr::IsConnected()
{
	if( P_VALID(m_pSession) )
		return m_pSession->IsConnected();
	return FALSE;
}

BOOL NetSessionMgr::IsTryingToConnect()
{
	if( P_VALID(m_pSession) )
		return m_pSession->IsTryingToConnect();
	return FALSE;
}

VOID NetSessionMgr::TryToConnect( LPCSTR szIP, INT nPort/*=0*/ )
{
	if( P_VALID(m_pSession) )
		m_pSession->TryToConnect( szIP, nPort );
}

VOID NetSessionMgr::StopTryingConnect()
{
	if( P_VALID(m_pSession) )
		m_pSession->StopTryingConnect();
}

void NetSessionMgr::SendMsg( LPVOID msg, DWORD size )
{
	if( P_VALID(m_pSession) )
		m_pSession->SendMsg( msg, size );
}

void NetSessionMgr::InsertPrivilegeMap( std::pair<DWORD, DWORD> pairPri )
{
	m_mapPrivilege.insert(pairPri);
}

bool NetSessionMgr::VerifyGMPrivilege( DWORD dwMsgCrc )
{
	std::map<DWORD, DWORD>::iterator it = m_mapPrivilege.find(dwMsgCrc);
	if( it != m_mapPrivilege.end() )
	{
		if( it->second & ( 1 << m_ePrivilege ) )
			return true;
		else 
			return false;
	}

	return false;
}