//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: msg_rt_c.h
// author: 
// actor:
// data: 2009-04-27
// last:
// brief: 运维服务端与客户端之间的消息定义
//-----------------------------------------------------------------------------
#pragma once
#include "../ServerDefine/login_define.h"
#include "../WorldDefine/base_define.h"
#include "../ServerDefine/rt_define.h"
#include "../ServerDefine/gm_tool_define.h"

#pragma pack(push, 1)

enum EMsgSet
{
	NRTC_LoginService,
	NRTC_Challenge,
	NRTC_Close,
	NRTC_Start,
};

//-----------------------------------------------------------------------------
// 连接初始消息，发自运维客户端（RtClient），发到运维服务器(GMserver)
//-----------------------------------------------------------------------------
CMD_START(NRTC_LoginService)				
	
CMD_END;

//-----------------------------------------------------------------------------
// 确认连接消息，发自运维服务器(GMserver)，发到运维服务器（Rtclient）
//-----------------------------------------------------------------------------
CMD_START(NRTS_LoginService)				
	DWORD			dwErrorCode;				// 0:成功
CMD_END;

//-----------------------------------------------------------------------------
// 关闭游戏世界服务器的消息，发自运维客户端（RtClient），发到运维服务器(GMserver)
//-----------------------------------------------------------------------------
CMD_START(NRTC_Close)
	DWORD			dwSectionID;				//大区ID
	EServerType		eType;						//服务器类型
	DWORD			dwWorldID;					//游戏世界ID，对于关闭LoginServer的消息无意义
	BOOL			bForce;						//FALSE:主动关闭 
												//TRUE:强制关闭					
CMD_END

//-----------------------------------------------------------------------------
// 关闭游戏世界服务器的确认消息，发自运维服务器(GMserver)，发到运维客户端（RtClient）
//-----------------------------------------------------------------------------
CMD_START(NRTS_Close)
	DWORD			dwErrorCode;
CMD_END

//-----------------------------------------------------------------------------
// 开启游戏世界服务器的消息，发自运维客户端（RtClient），发到运维服务器(GMserver)
//-----------------------------------------------------------------------------
CMD_START(NRTC_Start)
	DWORD			dwSectionID;				//大区ID
	EServerType		eType;						//服务器类型
	DWORD			dwWorldID;					//游戏世界ID，对于开启LoginServer的消息无意义
CMD_END

//-----------------------------------------------------------------------------
// 开启游戏世界服务器的确认消息，发自运维服务器(GMserver)，发到运维客户端（RtClient）
//-----------------------------------------------------------------------------
CMD_START(NRTS_Start)
	DWORD			dwErrorCode;
CMD_END

//-----------------------------------------------------------------------------
// 变更账号绑定的IP地址
//-----------------------------------------------------------------------------
CMD_START(NRTC_BindIP)
	TCHAR			szAccountName[X_SHORT_NAME];
	DWORD			dwIP;
CMD_END

CMD_START(NRTS_BindIP)
	DWORD			dwErrorCode;
CMD_END

//-----------------------------------------------------------------------------
// 服务器动态信息包消息，发自运维服务器(GMserver)，发到运维客户端（RtClient）
//-----------------------------------------------------------------------------
struct tagServerInfo
{
	BOOL			bValid;
	EWorldStatus	eStatus;
	INT				nMaxPlayerNum;
	INT				nPlayerOnline;	
	INT				nRealOnline;
};

CMD_START(NRTS_AllServerDynaInfo)
	INT				nSectionNum;				//大区个数
	BYTE			byInfo[1];					//大区ID-login状态-world数量-world[n]ID-World[n]LoongWorld状态-World[n]LoongDB状态
	
CMD_END

//-----------------------------------------------------------------------------
// 服务器静态信息包消息，发自运维服务器(GMserver)，发到运维客户端（RtClient）
//-----------------------------------------------------------------------------
CMD_START(NRTS_AllServerStatInfo)
	INT				nSectionNum;				//大区个数
	BYTE			byInfo[1];					//大区名-大区ID-login IP-world数量-world[n]名字-world[n]ID-LoongWorldIP-LoongDBIP
CMD_END

//-----------------------------------------------------------------------------
// 账号验证消息，发自运维客户端RtClient，发到运维服务器(GMserver)
//-----------------------------------------------------------------------------
CMD_START(NRTC_Challenge)
	TCHAR			szAccountName[X_SHORT_NAME];
	DWORD			dwPsdCrc;
CMD_END

//-----------------------------------------------------------------------------
// 账号验证的确认信息，发自运维服务器(GMserver)，发到运维客户端（RtClient）
//-----------------------------------------------------------------------------
CMD_START(NRTS_Challenge)
	DWORD			dwErrorCode;
	EPrivilege		ePrivilege;
CMD_END

//-----------------------------------------------------------------------------
// 权限信息，发自运维服务器(GMserver)，发到运维客户端（RtClient）
//-----------------------------------------------------------------------------
CMD_START(NRTS_PrivilegeInfo)
	INT				nNum;						// 权限对数量
	DWORD			dwInfo[1];					// 该数字的二进制形式的每一位分别代表各消息的执行权限
CMD_END

//-----------------------------------------------------------------------------
// 发送系统指令的消息，发自运维客户端RtClient，发到运维服务器(GMserver)
//-----------------------------------------------------------------------------
CMD_START(NRTC_SystemCmd)
	DWORD			dwSectionID;				//大区ID
	EServerType		eType;						//服务器类型
	DWORD			dwWorldID;					//游戏世界ID,db不接受指令
	TCHAR			szCmd[System_Cmd_Length];	//系统指令
CMD_END

CMD_START(NRTS_SystemCmd)
	DWORD			dwErrorCode;
	EPrivilege		ePrivilege;
CMD_END

//-----------------------------------------------------------------------------
// 更改最大在线人数，发自运维客户端RtClient，发到运维服务器(GMserver)
//-----------------------------------------------------------------------------
CMD_START(NRTC_MaxNum)
	DWORD			dwSectionID;				//大区ID
	EServerType		eType;						//服务器类型
	DWORD			dwWorldID;					//游戏世界ID,db不接受指令
	INT				nMaxNum;					//新设置的最大在线人数
CMD_END

CMD_START(NRTS_MaxNum)
	DWORD			dwErrorCode;
CMD_END

//-----------------------------------------------------------------------------
// 更新商城，发自运维客户端RtClient，发到运维服务器(GMserver)
//-----------------------------------------------------------------------------
CMD_START(NRTC_UpdateMall)
	DWORD			dwSectionID;				//大区ID
	EServerType		eType;						//服务器类型
	DWORD			dwWorldID;					//游戏世界ID,db不接受指令
CMD_END

CMD_START(NRTS_UpdateMall)
	DWORD			dwErrorCode;
CMD_END

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
CMD_START(NRTC_GMQuery)
	
CMD_END

CMD_START(NRTS_GMQuery)
	DWORD		dwErrorCode;
CMD_END

#pragma pack(pop)
