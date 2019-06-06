//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: msg_rt_c.h
// author: 
// actor:
// data: 2009-04-27
// last:
// brief: GM服务端与GM工具之间的消息定义
//-----------------------------------------------------------------------------
#pragma once

#include "../WorldDefine/base_define.h"
#include "gm_tool_define.h"
#include "rt_define.h"

#pragma pack(push, 1)
//-----------------------------------------------------------------------
//GM创建
//-----------------------------------------------------------------------
CMD_START(NGMTC_GMCreate)
	TCHAR			szName[X_SHORT_NAME];			//帐号
	DWORD			dwPswCrc;						//密码CRC
	EPrivilege		ePrivilege;						//权限
CMD_END

CMD_START(NGMTS_GMCreate)
	DWORD			dwErrorCode;
CMD_END

//-----------------------------------------------------------------------
//GM修改
//-----------------------------------------------------------------------
CMD_START(NGMTC_GMModify)
	TCHAR			szName[X_SHORT_NAME];			//帐号
	EPrivilege		ePrivilege;						//权限
CMD_END

CMD_START(NGMTS_GMModify)
	DWORD			dwErrorCode;
CMD_END

//-----------------------------------------------------------------------
//GM删除
//-----------------------------------------------------------------------
CMD_START(NGMTC_GMDelete)
	TCHAR			szName[X_SHORT_NAME];			//帐号
CMD_END

CMD_START(NGMTS_GMDelete)
	DWORD			dwErrorCode;
CMD_END

//-----------------------------------------------------------------------
//						登录查看msg_rt_c.h 
//-----------------------------------------------------------------------

//-----------------------------------------------------------------------
//角色恢复
//-----------------------------------------------------------------------
CMD_START(NGMTC_RoleResume)
	INT				nOldID;							//角色原来的ID
	TCHAR			szNewName[X_SHORT_NAME];		//角色新起的名字
	DWORD			dwSectionID;					//大区ID
	DWORD			dwWorldID;						//游戏世界ID
CMD_END

CMD_START(NGMTS_RoleResume)
	DWORD			dwErrorCode;
CMD_END

//-----------------------------------------------------------------------
//角色属性修改
//-----------------------------------------------------------------------
CMD_START(NGMTC_ProModify)
	TCHAR			szName[X_SHORT_NAME];			//角色名字
	INT				nLel;							//等级
	INT				nSkill;							//天资点
	INT				nYuanbao;						//元宝
	INT64			n64Exp;							//经验
	INT64			n64Money;						//钱	
	DWORD			dwSectionID;					//大区ID
	DWORD			dwWorldID;						//游戏世界ID
CMD_END

CMD_START(NGMTS_ProModify)
	DWORD			dwErrorCode;
CMD_END

//-----------------------------------------------------------------------
//物品生成
//-----------------------------------------------------------------------
CMD_START(NGMTC_ItemCreate)
	TCHAR			szName[X_SHORT_NAME];			//角色
	DWORD			dwItemTypeID;					//物品TypeID
	INT				nItemNum;						//数量
	BYTE			byQuality;						//品质
	DWORD			dwSectionID;					//大区ID
	DWORD			dwWorldID;						//游戏世界ID
CMD_END

CMD_START(NGMTS_ItemCreate)
	DWORD			dwErrorCode;
CMD_END

//-----------------------------------------------------------------------
//物品删除
//-----------------------------------------------------------------------
CMD_START(NGMTC_ItemDelete)
	TCHAR			szName[X_SHORT_NAME];			//角色
	//INT64			n64ItemID;						//物品全局ID
	DWORD			dwSectionID;					//大区ID
	DWORD			dwWorldID;						//游戏世界ID
	TCHAR			sz64IDString[1];				//要删除物品的64位ID组成的串		
CMD_END

CMD_START(NGMTS_ItemDelete)
	DWORD			dwErrorCode;
CMD_END

//-----------------------------------------------------------------------
//物品恢复
//-----------------------------------------------------------------------
CMD_START(NGMTC_ItemResume)
	TCHAR			szTargetRole[X_SHORT_NAME];		//目标角色名
	DWORD			dwSectionID;					//大区ID
	DWORD			dwWorldID;						//游戏世界ID
	TCHAR			sz64IDString[1];				//物品全局ID
CMD_END

CMD_START(NGMTS_ItemResume)
	DWORD			dwErrorCode;
CMD_END

//-----------------------------------------------------------------------
// 账号封停
//-----------------------------------------------------------------------
CMD_START(NGMTC_AccountForbid)
	TCHAR			szSectionName[X_SHORT_NAME];		//大区
	TCHAR			szWorldName[X_SHORT_NAME];			//游戏世界
	INT				nAction;							//0:解封 1：封
	TCHAR           szAccountStr[1];					//账号串,各账号之间逗号分隔
CMD_END

CMD_START(NGMTS_AccountForbid)
	//BOOL			bLast;
	TCHAR           szAccount[X_SHORT_NAME];
	DWORD			dwErrorCode;
CMD_END

//-----------------------------------------------------------------------
//--双倍
//-----------------------------------------------------------------------
CMD_START(NGMTC_Double)
	EDoubleType		eDType;
	DWORD			dwOpenTime;						//开放时间
	DWORD			dwLastTime;						//持续时长
	DWORD			dwRatio;						//比例
	DWORD			dwSectionID;					//大区ID,
	DWORD			dwWorldID[1];					//游戏世界ID[n]     代表该大区下所有游戏世界		
CMD_END

//-----------------------------------------------------------------------
//--右下角公告
//-----------------------------------------------------------------------
CMD_START(NGMTC_RightNotice)								
	DWORD			dwNoticeTime;					//公告时间 tagDWORDTime 0为即时
	TCHAR			szTitle[Notice_Title];			//标题
	TCHAR			szContent[Notice_Content];		//内容
	TCHAR			szLink[Notice_Link];			//链接的网站
	DWORD			dwSectionID;					//大区ID,
	DWORD			dwWorldID[1];					//游戏世界ID[n]      游戏世界ID:GT_INVALID代表该大区下所有游戏世界											
CMD_END

//-----------------------------------------------------------------------
//--自动循环公告
//-----------------------------------------------------------------------
CMD_START(NGMTC_AutoNotice)
	TCHAR			szContent[Notice_Content];		//内容
	EColorType		eType;							//颜色
	INT				nTimeInterval;					//时间间隔
	INT				nCirInterval;					//循环间隔
	INT				nCirNum;						//循环次数
	DWORD			dwSectionID;					//大区ID,
	DWORD			dwWorldID[1];					//游戏世界ID[n]      游戏世界ID:GT_INVALID代表该大区下所有游戏世界	
CMD_END

//-----------------------------------------------------------------------
//--与时间有关的返回消息
//-----------------------------------------------------------------------
CMD_START(NGMTS_TimeLimitedMsg)
	ETimeLimitedMsg	eType;	
	DWORD			dwErrorCode;					// 0-成功
	DWORD			dwWorldID;
CMD_END

//-----------------------------------------------------------------------
// 添加/删除黑名单IP
//-----------------------------------------------------------------------
CMD_START(NGMTC_Blacklist)
	TCHAR			szSectionName[X_SHORT_NAME];		// 大区
	DWORD			dwIP;								// IP
	INT				nAction;							// 0:解封 1：封
CMD_END

CMD_START(NGMTS_Blacklist)
	DWORD			dwErrorCode;
CMD_END

//-----------------------------------------------------------------------
// 聊天频道自动公告
//-----------------------------------------------------------------------
CMD_START(NGMTC_AutoChatNotice)
	TCHAR			szNotice[X_HUGE_STRING];			// 公告内容
	BYTE			byHour;								// 发送: 时
	BYTE			byMinute;							// 发送: 分
	DWORD			dwSectionID;						// 大区ID
	DWORD			dwWorldID[1];						// 游戏世界ID[n]      游戏世界ID:GT_INVALID代表该大区下所有游戏世界	// 发布时间
CMD_END

CMD_START(NGMTS_AutoChatNotice)
	DWORD			dwErrorCode;
CMD_END

//-----------------------------------------------------------------------
// 取消所有已设定聊天频道消息
//-----------------------------------------------------------------------
CMD_START(NGMTC_CancelAutoChatNotice)
CMD_END

CMD_START(NGMTS_CancelAutoChatNotice)
	DWORD			dwErrorCode;
CMD_END

//-----------------------------------------------------------------------
// 取消所有计时消息
//-----------------------------------------------------------------------
CMD_START(NGMTC_CancelTimeLimitedMsg)
CMD_END

CMD_START(NGMTS_CancelTimeLimitedMsg)
	DWORD			dwErrorCode;
CMD_END

//-----------------------------------------------------------------------
// 取消所有双倍设置
//-----------------------------------------------------------------------
CMD_START(NGMTC_CancelDoubleMsg)
	//DWORD			dwSectionID;						// 大区ID
	//DWORD			dwWorldID[1];						// 游戏世界ID[n]      游戏世界ID:GT_INVALID代表该大区下所有游戏世界	// 发布时间
CMD_END

CMD_START(NGMTS_CancelDoubleMsg)
	DWORD			dwErrorCode;
CMD_END

//-----------------------------------------------------------------------
// 取消所有已设定右下角
//-----------------------------------------------------------------------
CMD_START(NGMTC_CancelRightNotice)
CMD_END

CMD_START(NGMTS_CancelRightNotice)
	DWORD			dwErrorCode;
CMD_END

//-----------------------------------------------------------------------
// 清空指定玩家的百宝袋
//-----------------------------------------------------------------------
CMD_START(NGMTC_ClearBaibao)
	TCHAR	szOwnerName[X_SHORT_NAME];
	DWORD	dwSectionID;
	DWORD	dwWorldID;
CMD_END

CMD_START(NGMTS_ClearBaibao)
	DWORD			dwErrorCode;
CMD_END

CMD_START(NGMTS_GMSetFackNumer)
	float			fFackRate;
CMD_END

#pragma pack(pop)
