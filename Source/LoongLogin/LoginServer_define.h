//-----------------------------------------------------------------------------
//!\file LoginServer_define.h
//!
//!\date 2008-06-04
//!
//!\brief LoginServer 结构和常量的定义
//!
//-----------------------------------------------------------------------------
#pragma once

#include "../ServerDefine/login_define.h"

//-----------------------------------------------------------------------------
// CONSTANT
//-----------------------------------------------------------------------------
#define GUARD_TIME		18000		// 以秒为单位，5小时，18000秒
#define MIBAO_LEN		240			// 密保卡程度

const INT CON_LOST		= -10000;	// 连接断开标志

//------------------------------------------------------------------------------
// LoongWorld当前信息
//------------------------------------------------------------------------------
struct tagLoongWorldData 
{
	BOOL			bValid;						// 当前是否连接
	TCHAR			szName[X_SHORT_NAME];	    // World名称
	DWORD			dwNameCrc;					// 名字CRC值
	EWorldStatus	eStatus;					// 当前状态
	INT				nPort;						// 端口
	DWORD			dwIP;						// IP
	INT				nMaxOnlineNum;				// 最大在线人数
	INT				nCurOnlineNum;				// 当前在线人数
	INT				nCurOnlineDisplay;			// 显示的在线人数
	BOOL			bAutoSeal;					// 是否自动
	DWORD			dwWorldID;					// worldid

	tagLoongWorldData()
	{
		ZeroMemory(this, sizeof(*this));
	}
};

//----------------------------------------------------------------
// 登录玩家基本信息 sizeof = 396
//----------------------------------------------------------------
struct tagPlayerData 
{
	DWORD				dwClientID;							// 客户端ID
	DWORD				dwCDIndex;							// 网络ID
	DWORD				dwAccountID;						// 帐号ID
	CHAR				szAccountName[X_SHORT_NAME];		// 帐号名称
	DWORD				dwMibaoCrc;							// 根据
//	BOOL				bDisabled;							// 是否封停
	DWORD				dwFrobidMask;						// 封停掩码 每1位对应一种封停类型
	BOOL				bGuard;								// 防沉迷用户
	INT					nAccOLSec;							// 累计登录时间
	DWORD				dwIP;								// 客户端IP
	DWORD				dwWorldNameCrc;						// 游戏世界名称CRC值
	BYTE				byPrivilege;						// 权限
	BYTE				byReserved[3];						// 对齐标志

	tagPlayerData()		{ ZeroMemory(this, sizeof(*this)); }
	VOID Reset()		{ ZeroMemory(this, sizeof(*this)); }
};

//----------------------------------------------------------------
// 登录玩家基本信息 sizeof = 396
//----------------------------------------------------------------
struct tagAccountData
{
	CHAR				szAccountName[X_SHORT_NAME];
	DWORD				dwIp;
	BOOL				bGuard;
};