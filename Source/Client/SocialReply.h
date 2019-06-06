/********************************************************************
	created:	2009/02/01
	created:	1:2:2009   15:27
	filename: 	d:\loong_client\Client\SocialReply.h
	file path:	d:\loong_client\Client
	file base:	SocialReply
	file ext:	h
	author:		leili
	
	purpose:	自动回复声明
*********************************************************************/
#pragma once

class SocialReply
{
public:
	SocialReply(void);
	virtual ~SocialReply(void);

	BOOL	Init( LPCTSTR szFile );
	VOID	Destroy(void);

	BOOL	SetReply( LPCTSTR szReply );
	LPCTSTR GetReply(void) const;

private:
	TObjRef<DiscIOMgr>	m_pDiscIOMgr;
	DWORD				m_dwFileHandle;
	tstring				m_strReply;
};