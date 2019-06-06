//-----------------------------------------------------------------------------
//!\file	client_db.h
//!\brief	客户端数据库管理类
//!
//!\date	2009-04-20
//! last	2009-04-20
//!\author	zhangrong
//!
//! Copyright (c) 1985-2008 CTCRST Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include "../WorldDefine/base_define.h"

#define PRIVILEGE_TABLE_NAME	""
const INT nPrivilegeCount = 3;

//Log结构
enum EActionType
{
	EAT_Null,

	EAT_CreateGM = 1,			// 线下GM创建
	EAT_ModifyGM,				// 线下GM修改
	EAT_DeleteGM,				// 线下GM删除

	EAT_ResumeRole,				// 角色恢复
	EAT_ProModify,				// 属性修改

	EAT_ItemCreate,				// 道具生成
	EAT_ItemDelete,				// 道具删除
	EAT_ItemResume,				// 道具恢复

	EAT_DoubleStart,			// 开放双倍
	EAT_DoubleEnd,				// 双倍删除

	EAT_AutoNoticeStart,		// 自动循环公告
	EAT_AutoNoticeEnd,			// 自动循环公告删除

	EAT_NoticeStart,			// 右下角公告
	EAT_NoticeEnd,				// 右下角公告删除

	EAT_AutoChatStart,			// 聊天频道公告
	EAT_AutoChatEnd,			// 聊天频道公告删除

	EAT_StartServer,			// 开启服务器
	EAT_CloseServer,			// 关闭服务器
	EAT_KillServer,				// 强制关闭服务器

	EAT_IpBind,					// 绑定IP

	EAT_SendSystemCmd,			// 发送系统消息

	EAT_End
};

#define IsActionValid(x) ((x)>EAT_Null && (x)<EAT_End)

struct tagLogRecord 
{
	DWORD		dwClientID;
	DWORD		dwTime;
	EActionType	eActionType;
	TCHAR		szName[X_SHORT_NAME];
	TCHAR		szIP[X_SHORT_NAME];
	TCHAR		szDetails[X_LONG_STRING];
};

//-----------------------------------------------------------------------------
// 客户端数据库管理类
//-----------------------------------------------------------------------------
class ClientDB
{
public:
	//-------------------------------------------------------------------------
	// CONSTRUCT
	//-------------------------------------------------------------------------
	ClientDB();
	~ClientDB() {}

	//-------------------------------------------------------------------------
	// 初始化及销毁
	//-------------------------------------------------------------------------
	BOOL	Init(LPCTSTR strHost, LPCTSTR strUser, LPCTSTR strPassword, LPCTSTR strDatabase, INT nPort);
	VOID	Destroy();

	//-------------------------------------------------------------------------
	// 数据库相关处理
	//-------------------------------------------------------------------------
	BOOL	DBQueryClientData(LPCTSTR szName, DWORD dwPsdCrc, DWORD& dwClientID, INT& nPrivilege);
	BOOL	DBCheckIPBind(LPCTSTR szName, DWORD dwClientIP);
	BOOL	DBCreateGameMaster(LPCTSTR szName, DWORD dwPsdCrc, INT nPrivilege);
	BOOL	DBDeleteGameMaster(LPCTSTR szName);
	BOOL	DBChangeGMPrivilege(LPCTSTR szName, INT nNewPrivilege);
	BOOL	DBQueryPrivilegeInfo(TMap<DWORD, DWORD>& mapPrivilege);
	BOOL	DBQueryAccountPrivilege(LPCTSTR szName, INT& nPrivilege);
	BOOL	DBGMExist(LPCTSTR szName);

	BOOL	DBChangeBindIp(LPCTSTR szName, DWORD dwIP);
	BOOL	DBQueryBindIp(LPCTSTR szName, DWORD& dwIP);
	BOOL	DBReplaceBindIp(LPCTSTR szName, DWORD dwIP);

	BOOL	DBLogGMAction(DWORD dwAccountID, LPCTSTR szName, LPCTSTR szIP, EActionType eActionType, LPCTSTR szDetails);

private:
	DataBase		m_DB;			// Mysql数据库
	
};