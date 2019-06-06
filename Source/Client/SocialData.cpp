/********************************************************************
	created:	2008/12/11
	created:	11:12:2008   14:07
	filename: 	d:\loong_client\Client\SocialData.cpp
	file path:	d:\loong_client\Client
	file base:	SocialData
	file ext:	cpp
	author:		leili
	
	purpose:	存放从服务器发送来的好友信息、黑名单信息，同时处理查询
*********************************************************************/

#include "stdafx.h"
#include "SocialData.h"

namespace
{
	class FriendSort
	{
	public:
		bool operator()(const tagFriendInfo& friend1,const tagFriendInfo& friend2)
		{
			if( friend1.bOnline == FALSE && friend2.bOnline == TRUE )
				return false;

			if( friend1.bOnline == TRUE && friend2.bOnline == FALSE )
				return true;

			return friend1.dwFriVal > friend2.dwFriVal;
		}
	};

	class BlackListSort
	{
	public:
		bool operator()(const DWORD& black1,const DWORD& black2)
		{
			return black1 < black2;
		}
	};
}

SocialData::SocialData( void )
{

}

SocialData::~SocialData( void )
{
	m_friendsArray1.clear();
	m_friendsArray2.clear();
	m_friendsArray3.clear();
	m_friendsArray4.clear();
	m_blackArray.clear();
}

//-----------------------------------------------------------------------------
// 对某个好友列表排序
//-----------------------------------------------------------------------------
VOID SocialData::SortFriendList( EListType listType )
{
	FriendSort sortObj;
	switch( listType )
	{
	case ELT_Friend1:
		sort(m_friendsArray1.begin(),m_friendsArray1.end(),sortObj);
		break;

	case ELT_Friend2:
		sort(m_friendsArray2.begin(),m_friendsArray2.end(),sortObj);
		break;

	case ELT_Friend3:
		sort(m_friendsArray3.begin(),m_friendsArray3.end(),sortObj);
		break;

	case ELT_Friend4:
		sort(m_friendsArray4.begin(),m_friendsArray4.end(),sortObj);
		break;
	}
}

//-----------------------------------------------------------------------------
// 对黑名单排序
//-----------------------------------------------------------------------------
VOID SocialData::SortBlackList( void )
{
	BlackListSort sortObj;
	sort(m_blackArray.begin(),m_blackArray.end(),sortObj);
}

//-----------------------------------------------------------------------------
// 返回某个好友列表
//-----------------------------------------------------------------------------
BOOL SocialData::GetFriendList( EListType listType, const FriendsArray** friends ) const
{
	switch( listType )
	{
	case ELT_Friend1:
		*friends = &m_friendsArray1;
		break;

	case ELT_Friend2:
		*friends = &m_friendsArray2;
		break;

	case ELT_Friend3:
		*friends = &m_friendsArray3;
		break;

	case ELT_Friend4:
		*friends = &m_friendsArray4;
		break;

	default:
		return FALSE;
	}

	return TRUE;
}

//-----------------------------------------------------------------------------
// 通过索引返回某个好友
//-----------------------------------------------------------------------------
const tagFriendInfo* SocialData::GetFriendPtrByIndex( EListType listType, int index ) const
{
	switch( listType )
	{
	case ELT_Friend1:
		{
			if( index < (int)m_friendsArray1.size()  )
				return &m_friendsArray1[index];
			else
				return NULL;
		}
		break;

	case ELT_Friend2:
		{
			if( index < (int)m_friendsArray2.size() )
				return &m_friendsArray2[index];
			else
				return NULL;
		}
		break;

	case ELT_Friend3:
		{
			if( index < (int)m_friendsArray3.size() )
				return &m_friendsArray3[index];
			else
				return NULL;
		}
		break;

	case ELT_Friend4:
		{
			if( index < (int)m_friendsArray4.size() )
				return &m_friendsArray4[index];
			else
				return NULL;
		}
		break;
	}

	return NULL;
}

//-----------------------------------------------------------------------------
// 通过ID返回某个好友
//-----------------------------------------------------------------------------
const tagFriendInfo* SocialData::GetFriendPtrByID( EListType listType, DWORD roleID ) const
{
	const FriendsArray *pFriendArray = NULL;

	switch( listType )
	{
	case ELT_Friend1:
		pFriendArray = &m_friendsArray1;
		break;
	case ELT_Friend2:
		pFriendArray = &m_friendsArray2;
		break;
	case ELT_Friend3:
		pFriendArray = &m_friendsArray3;
		break;
	case ELT_Friend4:
		pFriendArray = &m_friendsArray4;
		break;
	}

	if( P_VALID(pFriendArray) )
	{
		FriendsArray::const_iterator iter;
		for( iter=pFriendArray->begin(); iter!=pFriendArray->end(); ++iter )
		{
			const tagFriendInfo& friendInfo = *iter;
			if( roleID == friendInfo.dwFriendID )
			{
				return &friendInfo;
			}
		}
	}

	return NULL;
}

//-----------------------------------------------------------------------------
// 往某个好友列表添加好友
//-----------------------------------------------------------------------------
VOID SocialData::AddToFriendList( EListType listType, const tagFriendInfo& friendInfo )
{
	tagFriendInfo addInfo = friendInfo;

	switch( listType )
	{
	case ELT_Friend1:
		{
			addInfo.byGroup = 1;

			m_friendsArray1.push_back( addInfo );
		}
		break;

	case ELT_Friend2:
		{
			addInfo.byGroup = 2;

			m_friendsArray2.push_back( addInfo );
		}
		break;

	case ELT_Friend3:
		{
			addInfo.byGroup = 3;

			m_friendsArray3.push_back( addInfo );
		}
		break;

	case ELT_Friend4:
		{
			addInfo.byGroup = 4;

			m_friendsArray4.push_back( addInfo );
		}
		break;

	default:
		break;
	}
}

//-----------------------------------------------------------------------------
// 添加黑名单
//-----------------------------------------------------------------------------
VOID SocialData::AddToBlackList( DWORD roleID )
{
	m_blackArray.push_back( roleID );
}

//-----------------------------------------------------------------------------
// 从某个好友列表中删除某好友
//-----------------------------------------------------------------------------
VOID SocialData::DeleteFromFriendList( EListType listType, DWORD roleID )
{
	FriendsArray *pArray = NULL;
	switch( listType )
	{
	case ELT_Friend1:
		pArray = &m_friendsArray1;
		break;

	case ELT_Friend2:
		pArray = &m_friendsArray2;
		break;

	case ELT_Friend3:
		pArray = &m_friendsArray3;
		break;

	case ELT_Friend4:
		pArray = &m_friendsArray4;
		break;

	default:
		break;
	}

	if( P_VALID( pArray ) )
	{
		FriendsArray::iterator iter;
		for( iter=pArray->begin(); iter!=pArray->end(); ++iter )
		{
			tagFriendInfo& friendInfo = *iter;
			if( friendInfo.dwFriendID == roleID )
			{
				pArray->erase( iter );
				return;
			}
		}
	}
}

//-----------------------------------------------------------------------------
// 从黑名单中删除
//-----------------------------------------------------------------------------
VOID SocialData::DeleteFromBlackList( DWORD roleID )
{
	BlackArray::iterator iter;
	for( iter=m_blackArray.begin(); iter!=m_blackArray.end(); ++iter )
	{
		if( roleID == *iter )
		{
			m_blackArray.erase(iter);
			return;
		}
	}
}

//-----------------------------------------------------------------------------
// 根据索引从黑名单中获得玩家ID
//-----------------------------------------------------------------------------
DWORD SocialData::GetBlackListID( int index ) const
{
	if( index < (int)m_blackArray.size() )
	{
		return m_blackArray[index];
	}
	else
		return GT_INVALID;
}

//-----------------------------------------------------------------------------
// 是否存在某好友
//-----------------------------------------------------------------------------
BOOL SocialData::HasFriend( DWORD roleID ) const
{
	FriendsArray::const_iterator iter;

	for( iter=m_friendsArray1.begin(); iter!=m_friendsArray1.end(); ++iter )
	{
		const tagFriendInfo& info = *iter;
		if( info.dwFriendID == roleID )
			return TRUE;
	}

	for( iter=m_friendsArray2.begin(); iter!=m_friendsArray2.end(); ++iter )
	{
		const tagFriendInfo& info = *iter;
		if( info.dwFriendID == roleID )
			return TRUE;
	}

	for( iter=m_friendsArray3.begin(); iter!=m_friendsArray3.end(); ++iter )
	{
		const tagFriendInfo& info = *iter;
		if( info.dwFriendID == roleID )
			return TRUE;
	}

	for( iter=m_friendsArray4.begin(); iter!=m_friendsArray4.end(); ++iter )
	{
		const tagFriendInfo& info = *iter;
		if( info.dwFriendID == roleID )
			return TRUE;
	}

	return FALSE;
}

//-----------------------------------------------------------------------------
// 黑名单中是否存在某玩家
//-----------------------------------------------------------------------------
BOOL SocialData::HasBlack( DWORD roleID ) const
{
	BlackArray::const_iterator iter;

	for( iter=m_blackArray.begin(); iter!=m_blackArray.end(); ++iter )
	{
		if( *iter == roleID )
			return TRUE;
	}

	return FALSE;
}

INT SocialData::GetFriendNum( void ) const
{
	return (INT)(m_friendsArray1.size() + m_friendsArray2.size() + m_friendsArray3.size() + m_friendsArray4.size());
}

INT SocialData::GetBlackNum( void ) const
{
	return (INT)( m_blackArray.size() );
}

SocialData::EListType SocialData::FriendLogIn( DWORD roleID )
{
	FriendsArray::iterator iter;

	for( iter=m_friendsArray1.begin(); iter!=m_friendsArray1.end(); ++iter )
	{
		tagFriendInfo& info = *iter;
		if( info.dwFriendID == roleID )
		{
			info.bOnline = TRUE;
			return ELT_Friend1;
		}
	}

	for( iter=m_friendsArray2.begin(); iter!=m_friendsArray2.end(); ++iter )
	{
		tagFriendInfo& info = *iter;
		if( info.dwFriendID == roleID )
		{
			info.bOnline = TRUE;
			return ELT_Friend2;
		}
	}

	for( iter=m_friendsArray3.begin(); iter!=m_friendsArray3.end(); ++iter )
	{
		tagFriendInfo& info = *iter;
		if( info.dwFriendID == roleID )
		{
			info.bOnline = TRUE;
			return ELT_Friend3;
		}
	}

	for( iter=m_friendsArray4.begin(); iter!=m_friendsArray4.end(); ++iter )
	{
		tagFriendInfo& info = *iter;
		if( info.dwFriendID == roleID )
		{
			info.bOnline = TRUE;
			return ELT_Friend4;
		}
	}

	return ELT_None;
}

SocialData::EListType SocialData::FriendLogOut( DWORD roleID )
{
	FriendsArray::iterator iter;

	for( iter=m_friendsArray1.begin(); iter!=m_friendsArray1.end(); ++iter )
	{
		tagFriendInfo& info = *iter;
		if( info.dwFriendID == roleID )
		{
			info.bOnline = FALSE;
			return ELT_Friend1;
		}
	}

	for( iter=m_friendsArray2.begin(); iter!=m_friendsArray2.end(); ++iter )
	{
		tagFriendInfo& info = *iter;
		if( info.dwFriendID == roleID )
		{
			info.bOnline = FALSE;
			return ELT_Friend2;
		}
	}

	for( iter=m_friendsArray3.begin(); iter!=m_friendsArray3.end(); ++iter )
	{
		tagFriendInfo& info = *iter;
		if( info.dwFriendID == roleID )
		{
			info.bOnline = FALSE;
			return ELT_Friend3;
		}
	}

	for( iter=m_friendsArray4.begin(); iter!=m_friendsArray4.end(); ++iter )
	{
		tagFriendInfo& info = *iter;
		if( info.dwFriendID == roleID )
		{
			info.bOnline = FALSE;
			return ELT_Friend4;
		}
	}

	return ELT_None;
}

tagFriendInfo* SocialData::GetFriend( DWORD roleID )
{
	FriendsArray::iterator iter;

	for( iter=m_friendsArray1.begin(); iter!=m_friendsArray1.end(); ++iter )
	{
		tagFriendInfo& info = *iter;
		if( info.dwFriendID == roleID )
		{
			return &info;
		}
	}

	for( iter=m_friendsArray2.begin(); iter!=m_friendsArray2.end(); ++iter )
	{
		tagFriendInfo& info = *iter;
		if( info.dwFriendID == roleID )
		{
			return &info;
		}
	}

	for( iter=m_friendsArray3.begin(); iter!=m_friendsArray3.end(); ++iter )
	{
		tagFriendInfo& info = *iter;
		if( info.dwFriendID == roleID )
		{
			return &info;
		}
	}

	for( iter=m_friendsArray4.begin(); iter!=m_friendsArray4.end(); ++iter )
	{
		tagFriendInfo& info = *iter;
		if( info.dwFriendID == roleID )
		{
			return &info;
		}
	}

	return NULL;
}