//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: msg_rt_errorcode.h
// author: 
// actor:
// data: 2009-03-27
// last:
// brief: 返回给远程监控工具的错误码
//-----------------------------------------------------------------------------
#pragma once


//-----------------------------------------------------------------------------
enum
{
	E_RT_Success				= 0,

	// 商城相关
	E_Mall_ProtoLoad_Failed		= 1,	// 商城物品静态属性读取失败
	E_Mall_Init_Failed			= 2,	// 商城初始化失败

	// 封停解封账号时loginserver返回给GMserver的消息
	E_Login_GMServer_Forbidded	= 10,	// 账号已经被封
	E_Login_GMServer_Freed		= 11,	// 账号已经解封
	E_Login_GMServer_NoAccount	= 12,	// 账号不存在
	
	E_RT_Section_Error			= 20,	// 大区不存在
	E_RT_Error,	
};