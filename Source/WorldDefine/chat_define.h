#pragma once

#pragma pack(push, 1)

//玩家聊天发言频道
enum ESendChatChannel
{
	ESCC_NULL = -1,
	ESCC_CastTalk = 0,		//传音
	ESCC_World,				//世界
	ESCC_Pvt,				//私聊
	ESCC_Guild,				//帮派
	ESCC_Troop,				//队伍
	ESCC_Current,			//当前
	ESCC_Leave,				//留言
	ESCC_Combat,			//战斗
	ESCC_Affiche,			//公告
	ESCC_GM,				//GM
	ESCC_System,			//系统
	ESCC_End,		
};

//世界频道时间间隔
const INT	WORLD_CHANNEL_INTERVAL	= TICK_PER_SECOND * 30;

//世界频道消息长度限制
const INT	WORLD_CHANNEL_MSG_LEN	= 200;

//世界频道消息玩家等级限制
const INT	WORLD_CHANNEL_ROLE_LVL	= 10;

//传音IM物品ID1360001
const DWORD TYPEID_IM_CASTTALK		= 1360001;

#pragma pack(pop)