//-----------------------------------------------------------------------------
//!\file proof_policy_own_define.h
//!\author Aslan
//!
//!\date 2009-06-07
//! last 2008-06-07
//!
//!\brief 中央激活公共接口定义
//!
//!	Copyright (c) 2004 TENGWU Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#pragma pack(push, 1)

#define X_STRING_LEN	50			// 字符串长度

//-----------------------------------------------------------------------------
// 初始登陆ID
//-----------------------------------------------------------------------------
CMD_START(NC_LOGIN)
	DWORD	dwLoginID;				// login ID
CMD_END

CMD_START(NS_LOGIN)
	BYTE	byResult;				// 返回结果
CMD_END

//------------------------------------------------------------------------------
// 心跳消息
//------------------------------------------------------------------------------
CMD_START(NC_HEARTBEAT)
	DWORD	dwLoginID;					// login ID
CMD_END

//------------------------------------------------------------------------------
// 玩家登入
//------------------------------------------------------------------------------
CMD_START(NC_USERLOGIN)
	CHAR	szGUID[X_STRING_LEN];			// GUID
	DWORD	dwLoginID;						// login ID
	CHAR	szAccountName[X_STRING_LEN];	// 帐号名
	CHAR	szPsd[X_STRING_LEN];			// 密码
	DWORD	dwClientID;						// 序列号
CMD_END

CMD_START(NS_USERLOGIN)
	CHAR	szGUID[X_STRING_LEN];			// GUID
	CHAR	szAccountName[X_STRING_LEN];	// 用户名
	CHAR	szPsd[X_STRING_LEN];			// 新的密码
	BYTE	byResult;						// 验证结果
	BYTE	byState;						// 0 用户状态正常   1用户账号被封停
	BYTE	byBlocktype;					// 1、会员中心封停4、手机锁账号游戏封停 8、密保卡账号封停
	BYTE	byGuard;						// 防沉迷
	DWORD	dwClientID;						// 序列号
	CHAR	szMibao[MIBAO_LEN];				// 密保
CMD_END

//-------------------------------------------------------------------------------
// 用户更改密码
//-------------------------------------------------------------------------------
CMD_START(NS_USERUPDATEPWD)
	CHAR	szGUID[X_STRING_LEN];			// GUID
	DWORD	dwLoginID;						// login ID
	CHAR	szAccountName[X_STRING_LEN];	// 用户名
	CHAR	szPsd[X_STRING_LEN];			// 新密码
CMD_END

CMD_START(NC_USERUPDATEPWD)
	CHAR	szGUID[X_STRING_LEN];			// GUID
	CHAR	szAccountName[X_STRING_LEN];	// 用户名
	DWORD	dwLoginID;						// login ID
	BYTE	byResult;						// 结果
CMD_END

//---------------------------------------------------------------------------------
// 用户绑定密保
//---------------------------------------------------------------------------------
CMD_START(NS_USERBINDMIBAO)
	CHAR	szGUID[X_STRING_LEN];			// GUID
	DWORD	dwLoginID;						// login ID
	CHAR	szAccountName[X_STRING_LEN];	// 用户名
	CHAR	szMibao[MIBAO_LEN];				// 密保
CMD_END

CMD_START(NC_USERBINDMIBAO)
	CHAR	szGUID[X_STRING_LEN];			// GUID
	CHAR	szAccountName[X_STRING_LEN];	// 用户名
	DWORD	dwLoginID;						// login ID
	BYTE	byResult;						// 结果
CMD_END

//---------------------------------------------------------------------------------
// 用户取消密保
//---------------------------------------------------------------------------------
CMD_START(NS_USERUNBINDMIBAO)
	CHAR	szGUID[X_STRING_LEN];			// GUID
	DWORD	dwLoginID;						// login ID
	CHAR	szAccountName[X_STRING_LEN];	// 用户名
	CHAR	szMibao[MIBAO_LEN];				// 密保
CMD_END

CMD_START(NC_USERUNBINDMIBAO)
	CHAR	szGUID[X_STRING_LEN];			// GUID
	CHAR	szAccountName[X_STRING_LEN];	// 用户名
	DWORD	dwLoginID;						// login ID
	BYTE	byResult;						// 结果
CMD_END


//---------------------------------------------------------------------------------
// 账号封停
//---------------------------------------------------------------------------------
CMD_START(NS_BLOCKACCOUNT)
	CHAR	szGUID[X_STRING_LEN];			// GUID
	DWORD	dwLoginID;						// login ID
	CHAR	szAccountName[X_STRING_LEN];	// 用户名
	BYTE    byBlockType;					//1、会员中心封停4、手机锁账号游戏封停 8、密保卡账号封停
CMD_END

CMD_START(NC_BLOCKACCOUNT)
	CHAR	szGUID[X_STRING_LEN];			// GUID
	CHAR	szAccountName[X_STRING_LEN];	// 用户名
	DWORD	dwLoginID;						// login ID
	BYTE	byResult;						// 结果  0 正确  1失败 
CMD_END


//---------------------------------------------------------------------------------
// 账号解封
//---------------------------------------------------------------------------------
CMD_START(NS_UNBLOCKACCOUNT)
	CHAR	szGUID[X_STRING_LEN];			// GUID
	DWORD	dwLoginID;						// login ID
	CHAR	szAccountName[X_STRING_LEN];	// 用户名
	BYTE    byUnBlockType;					//1、会员中心解封  4、手机锁账号游戏解封  8、密保卡账号解封
CMD_END

CMD_START(NC_UNBLOCKACCOUNT)
	CHAR	szGUID[X_STRING_LEN];			// GUID
	CHAR	szAccountName[X_STRING_LEN];	// 用户名
	DWORD	dwLoginID;						// login ID
	BYTE	byResult;						// 结果
CMD_END

//---------------------------------------------------------------------------------
// 账号设置防沉迷
//---------------------------------------------------------------------------------
CMD_START(NS_USERUPDATECHENMI)
	CHAR	szGUID[X_STRING_LEN];			// GUID
	DWORD	dwLoginID;						// login ID
	CHAR	szAccountName[X_STRING_LEN];	// 用户名
	BYTE    byChenMi;						//0非防沉迷 1防沉迷
CMD_END

CMD_START(NC_USERUPDATECHENMI)
	CHAR	szGUID[X_STRING_LEN];			// GUID
	CHAR	szAccountName[X_STRING_LEN];	// 用户名
	DWORD	dwLoginID;						// login ID
	BYTE	byResult;						// 结果
CMD_END



#pragma pack(pop)