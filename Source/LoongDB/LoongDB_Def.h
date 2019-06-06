//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: main.h
// author: Sxg
// actor:
// data: 2008-05-12
// last:
// brief: 应用层相关定义
//-----------------------------------------------------------------------------
#pragma once




struct tagLoongDBUI
{
//	char		szServerName[X_SHORT_NAME];				// 机器名称

	INT			nClientNum;								// 客户端数量

	DWORD		dwSendPack;								// 发出的包数量
	DWORD		dwRecvPack;								// 收到的包数量
	DWORD		dwResendPack;							// 重发的包数量
	DWORD		dwSendBytes;							// 发出的字节数
	DWORD		dwRecvBytes;							// 收到的字节数

//	tagUIWorld	SimpleWorld[CACHE_MAX_WORLD];			// 游戏世界信息

};