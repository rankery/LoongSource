//-----------------------------------------------------------------------------
//!\file msg_adventure.h
//!
//!\date 2009-02-17
//!
//!\brief 客户端与服务器之间关于奇遇的消息
//-----------------------------------------------------------------------------

#pragma once
#pragma pack(push, 1)

// 奇遇模式
enum EAdventureType
{
	EAT_Instance,			// 奇遇副本
	EAT_GodLargess,			// 天神赐福
	EAT_DestinyBox			// 命运宝箱
};

// 天神赐福内容
enum EGodLargessContent
{
	EGLC_Item,				// 装备物品 
	EGLC_Task,				// 奇遇任务
	EGLC_Money,				// 金钱若干
	EGLC_Buf				// 增益状态
};


// 命运宝箱内容
enum EDestinyBoxContent
{
	EDBC_MonsterSiege,		// 怪物攻城
	EDBC_AwakenLich,		// 放出妖王
	EDBC_Strong,			// 铜墙铁壁
	EDBC_FrontAndRear,		// 腹背受敌
	EDBC_Money,				// 金银满仓
	EDBC_HelpWorld,			// 广济天下
	EDBC_HarmWorld,			// 天道之劫
};

//------------------------------------------------------------------------------
// 角色触发奇遇
//------------------------------------------------------------------------------
CMD_START(NS_ChooseAdventure)
	EAdventureType	eType;
CMD_END

//------------------------------------------------------------------------------
// 玩家是否同意接受奇遇
//------------------------------------------------------------------------------
CMD_START(NC_AdventureAgree)
	EAdventureType eType;
	bool	bAgree;
CMD_END

//------------------------------------------------------------------------------
// 通知所有队友玩家是否进入奇遇副本
//------------------------------------------------------------------------------
CMD_START(NS_AdventureNotify)
	DWORD	dwInviterID;
CMD_END

//------------------------------------------------------------------------------
// 同意进入奇遇副本的通知
//------------------------------------------------------------------------------
CMD_START(NC_AdventureNotifyAgree)
	bool bAgree;
CMD_END

//------------------------------------------------------------------------------
// 命运宝箱，公告全服广播
//------------------------------------------------------------------------------

CMD_START(NS_DestinyBoxBroadcast)
	TCHAR	PlayerName[X_SHORT_NAME];
	EDestinyBoxContent	eType;
	DWORD	dwID;				//需要提示ID的代表ID，不需要提示的为GT_INVALID
CMD_END

#pragma pack(pop)