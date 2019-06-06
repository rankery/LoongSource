#pragma once

#include "msg_common_errorcode.h"

#define MAX_NOTICE_TITLE	8
#define MAX_NOTICE_MSG_LEN	1024


#pragma pack(push, 1)

//在世界频道 发言 展示物品和装备 扣银数
#define WORLD_CHANNEL_DEC_SLIVER 500

//chat错误代码
enum
{
	E_RoleChat_ForbidChat				= 1,				//禁止发言
	E_RoleChat_ServerNotReady			= 2,				//服务器没有准备好
	E_RoleChat_TooLength				= 3,				//消息长度过长(超过100个字)

	E_RoleChat_CastTalk_NoIMItem		= 10,				//传音,没有IM道具

	E_RoleChat_World_NoItem				= 20,				//世界,没有道具
	E_RoleChat_World_Frequently			= 21,				//世界,发言过快
	E_RoleChat_World_NoMoney			= 22,				//世界,金钱不够
	E_RoleChat_World_RoleNo10			= 23,				//世界,人物没满10级

	E_RoleChat_Pvt_RemoteRoleLeave		= 30,				//私聊,对方不在线
	E_RoleChat_Pvt_NoRoleName			= 31,				//私聊,没有目标玩家 
	E_RoleChat_Pvt_NoSelf				= 32,				//私聊,自己不能与自己私聊

	E_RoleChat_Guild_NoJoin				= 40,				//门派,没有门派

	E_RoleChat_Troop_NoJoin				= 50,				//队伍,没有队伍

	E_RoleChat_Current_Frequently		= 60,				//当前,发言过快
	E_RoleChat_Quest_Bag_Full			= 70,				//任务物品背包已满
	E_RoleChat_Not_GM					= 80,				//发言人不是GM

};

const DWORD MAX_CHAT_LEN = 200;						//客户端输入框最大输入
CMD_START(NC_RoleChat)		// 对某人说话
	DWORD	dwDestRoleID;	// 对谁说话
	BYTE	byChannel;		// 发言频道
	BYTE	byAutoReply;	// 自动回复 0:不是 1:是
	TCHAR	szMsg[1];		// 话的内容   
CMD_END;

CMD_START(NS_RoleChat)		// 某人对你说话
	DWORD	dwErrorCode;	
	DWORD	dwSrcRoleID;	// 是谁说的话GT_INVALID代表系统发送
	DWORD	dwDestRoleID;	// 是对谁说
	BYTE	byChannel;		// 发言频道
	BYTE	byAutoReply;	// 自动回复 0:不是 1:是
	TCHAR	szMsg[1];		// 话的内容
CMD_END;

//--------------------------------------------------------------------------------
//以下为挖宝过程中,服务发送的广播消息
//--------------------------------------------------------------------------------

//获取留言
CMD_START(NC_LoadLeftMsg)
CMD_END


//福神下凡广播 服务器广播：“XX挖宝幸运的遇到福神下凡”
CMD_START(NS_ChatBillike)
	DWORD			dwRoleID;		//玩家ID
CMD_END

//挖坏上古封印广播 服务器广播：“XX挖坏了上古封印，上古的妖怪在XXX祸害人间，请各路英雄速去斩妖附魔！”
CMD_START(NS_ChatBadOldSeal)
	DWORD			dwRoleID;		//玩家ID
	DWORD			dwMapCrc;		//地图名的CRC值
CMD_END

//财神下凡广播 服务器广播：“XX挖宝幸运的遇到财神下凡”
CMD_START(NS_Plutus)
	DWORD			dwRoleID;		//玩家ID
CMD_END

//唤醒上古之灵 服务器广播“XXX挖宝惊动了上古神灵，他们来到人间考验人类的能力”
CMD_START(NS_AwakenGods)
	DWORD			dwRoleID;		//玩家ID
CMD_END

//神之历练 若将BOSS击杀，则全服公告：“XXX（挖宝任务的玩家）挑战###（BOSS名称）成功！获得了XXX”
CMD_START(NS_GodsOfPractice)
	DWORD			dwRoleID;		//玩家 ID
	DWORD			dwNpcTypeID;	//NPC type id
	DWORD			dwItemTypeID;	//物品 type id
CMD_END

//开放怪物攻城活动 系统公告提示：“###惹闹万物，大量的怪物决定5分钟后对##城进行报复，请大家协助防御”
CMD_START(NS_MonsterAttackCity)
	DWORD			dwRoleID;		//玩家ID
	DWORD			dwMapCrc;		//地图名的CRC值
CMD_END

// 广播宝箱开出的的物品
CMD_START(NS_RoleGetChestItem)
	DWORD			dwRoleID;
	DWORD			dwItemTypeID;
CMD_END

// 自动循环公告
CMD_START(NS_AutoNotice)
	INT				nType;							//颜色
	INT				nCirInterval;					//循环间隔
	TCHAR			szContent[1];					//内容
CMD_END

// 右下角公告
CMD_START(NS_RightNotice)
	INT				nTitleLen;						// 长度
	INT				nLinkLen;						// 长度
	INT				nContentLen;					// 长度
	TCHAR			szContent[1];
CMD_END

#pragma pack(pop)