/********************************************************************
	created:	2008/12/10
	created:	10:12:2008   11:58
	filename: 	d:\loong_client\Client\SocialData.h
	file path:	d:\loong_client\Client
	file base:	SocialData
	file ext:	h
	author:		leili
	
	purpose:	存放从服务器发送来的好友信息、黑名单信息，同时处理查询
*********************************************************************/

#pragma once

#include "..\WorldDefine\SocialDef.h"

class SocialData
{
	friend class SocialMgr;

public:
	typedef std::vector<tagFriendInfo> FriendsArray;
	typedef std::vector<DWORD> BlackArray;

	enum EListType
	{
		ELT_None		= -1,
		ELT_BlackList	= 0,
		ELT_Friend1,
		ELT_Friend2,
		ELT_Friend3,
		ELT_Friend4
	};

public:
	SocialData(void);
	~SocialData(void);

	VOID	SortFriendList( EListType listType );
	VOID	SortBlackList(void);

	BOOL	GetFriendList( EListType listType, const FriendsArray** friends ) const;
	const BlackArray& GetBlackList(void) const { return m_blackArray; }
	const tagFriendInfo* GetFriendPtrByIndex( EListType listType, int index ) const;
	const tagFriendInfo* GetFriendPtrByID( EListType listType, DWORD roleID ) const;
	tagFriendInfo*	GetFriend( DWORD roleID );
	DWORD	GetBlackListID( int index ) const;
	BOOL	HasFriend( DWORD roleID )	const;
	BOOL	HasBlack( DWORD roleID )	const;
	INT		GetFriendNum(void)	const;
	INT		GetBlackNum(void)	const;

	VOID	AddToFriendList( EListType listType, const tagFriendInfo& friendInfo );
	VOID	AddToBlackList( DWORD roleID );

	VOID	DeleteFromFriendList( EListType listType, DWORD roleID );
	VOID	DeleteFromBlackList( DWORD roleID );

	EListType	FriendLogIn( DWORD roleID );
	EListType	FriendLogOut( DWORD roleID );

private:
	FriendsArray	m_friendsArray1;
	FriendsArray	m_friendsArray2;
	FriendsArray	m_friendsArray3;
	FriendsArray	m_friendsArray4;
	BlackArray		m_blackArray;
};