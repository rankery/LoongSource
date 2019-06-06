/********************************************************************
	created:	2009/02/01
	created:	1:2:2009   15:31
	filename: 	d:\loong_client\Client\SocialReply.cpp
	file path:	d:\loong_client\Client
	file base:	SocialReply
	file ext:	cpp
	author:		leili
	
	purpose:	自动回复定义
*********************************************************************/

#include "stdafx.h"
#include "SocialReply.h"

SocialReply::SocialReply( void )
: m_dwFileHandle(INVALID_FILE_HANDLE)
{
}

SocialReply::~SocialReply( void )
{
	Destroy();
}

BOOL SocialReply::Init( LPCTSTR szFile )
{
	m_dwFileHandle = m_pDiscIOMgr->Create(szFile, GENERIC_READ|GENERIC_WRITE, 
		FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, 
		FILE_ATTRIBUTE_NORMAL|FILE_FLAG_RANDOM_ACCESS, NULL);

	if( m_dwFileHandle != INVALID_FILE_HANDLE )
	{
		TCHAR szBuff[51]={0};
		m_pDiscIOMgr->Read( m_dwFileHandle, szBuff, sizeof(TCHAR)*51 );
		m_strReply = szBuff;
		return TRUE;
	}

	return FALSE;
}

VOID SocialReply::Destroy( void )
{
	if( m_dwFileHandle != INVALID_FILE_HANDLE )
	{
		m_pDiscIOMgr->Close( m_dwFileHandle );
		m_dwFileHandle = INVALID_FILE_HANDLE;
	}
}

BOOL SocialReply::SetReply( LPCTSTR szReply )
{
	if( INVALID_FILE_HANDLE == m_dwFileHandle )
		return FALSE;

	TCHAR szBuff[51]={0};

	_tcsncpy( szBuff, szReply, 50);

	m_pDiscIOMgr->Seek( m_dwFileHandle, 0 );
	if( m_pDiscIOMgr->Write(m_dwFileHandle, szBuff, sizeof(TCHAR)*51) )
	{
		m_strReply = szBuff;
		return TRUE;
	}

	return FALSE;
}

LPCTSTR SocialReply::GetReply( void ) const
{
	return m_strReply.c_str();
}