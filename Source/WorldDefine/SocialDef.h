#pragma once

#pragma pack(push, 1)

const INT MAX_FRIENDNUM					= 100;		// 好友最大个数
const INT MAX_BLACKLIST					= 20;		// 黑名单最倒个数
const INT MAX_FRIENDVAL					= 9999;		// 好友度最大值
const INT MAX_ENEMYNUM					= 20;		// 仇敌最大个数
const INT GIFT_STORE_TIME				= 60 * TICK_PER_SECOND;	// 礼物在管道里保存时间

struct tagFriend
{
	DWORD	dwFriendID;								// 好友ID
	DWORD	dwFriVal;								// 好友度
	BYTE	byGroup;								// 分组
	tagFriend():dwFriendID(GT_INVALID),dwFriVal(0),byGroup(1){}
};

struct tagFriendInfo
{
	DWORD	dwFriendID;								// 好友ID
	DWORD	dwFriVal;								// 好友度
	BYTE	byGroup;								// 分组
	BOOL    bOnline;								// 好友是否在线
	INT		nLevel;									// 角色等级
};

struct tagSendGift				
{
	DWORD	dwSrcRoleID;							// 发送者
	DWORD	dwDestRoleID;							// 接收者
	DWORD	dwTypeID;								// 礼物TypeID
	INT 	nStoreTick;								// 发送时间
	tagSendGift(DWORD dwSrcRoleID, DWORD dwDestRoleID, DWORD dwTypeID):
		dwSrcRoleID(dwSrcRoleID),dwDestRoleID(dwDestRoleID),dwTypeID(dwTypeID)
	{
		nStoreTick = GIFT_STORE_TIME;
	}
};

struct tagFriUpdate
{
	DWORD   dwRoleID;								// 好友ID
	BOOL	bOnline;								// 是否在线
	INT		nLevel;									// 等级
};

#pragma pack(pop)