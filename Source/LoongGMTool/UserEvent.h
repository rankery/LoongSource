struct tagUserEvent
{
	DWORD		dwNameCrc;
	tstring		strEventName;

	tagUserEvent(LPCTSTR szEventName)
	{
		strEventName	= szEventName;
		dwNameCrc		= Crc32(szEventName);
	}
};

//登录
struct tagLoginEvent : public tagUserEvent
{
	DWORD		dwErrorCode;

	tagLoginEvent(LPCTSTR szEventName) : tagUserEvent(szEventName)
	{
		dwErrorCode		= 0;
	}
};

struct tagServerStatInfoEvent : public tagUserEvent
{
	DWORD		dwErrorCode;
	tagServerStatInfoEvent(LPCTSTR szEventName) : tagUserEvent(szEventName)
	{
		dwErrorCode		= 0;
	}
};

//GM创建
struct tagGMCreate : tagUserEvent
{
	DWORD		dwErrorCode;
	tagGMCreate(LPCTSTR szEventName) : tagUserEvent(szEventName)
	{
		dwErrorCode		= 0;
	}
};

//GM修改
struct tagGMModify : public tagUserEvent
{
	DWORD		dwErrorCode;
	tagGMModify(LPCTSTR szEventName) : tagUserEvent(szEventName)
	{
		dwErrorCode		= 0;
	}
};

//GM删除
struct tagGMDelete : public tagUserEvent
{
	DWORD		dwErrorCode;
	tagGMDelete(LPCTSTR szEventName) : tagUserEvent(szEventName)
	{
		dwErrorCode		= 0;
	}
};

//角色恢复
struct tagRoleResume : public tagUserEvent
{
	DWORD		dwErrorCode;
	tagRoleResume(LPCTSTR szEventName) : tagUserEvent(szEventName)
	{
		dwErrorCode		= 0;
	}
};

//角色属性修改
struct tagRoleProModify : public tagUserEvent
{
	DWORD		dwErrorCode;
	tagRoleProModify(LPCTSTR szEventName) : tagUserEvent(szEventName)
	{
		dwErrorCode		= 0;
	}
};

struct tagTimeLimited : public tagUserEvent
{
	DWORD		dwType;
	DWORD		dwErrorCode;
	DWORD		dwWorldID;
	tagTimeLimited(LPCTSTR szEventName) : tagUserEvent(szEventName)
	{
		dwErrorCode		= 0;
	}
};

//双倍
//struct tagDouble : public tagUserEvent
//{
//	DWORD			dwErrorCode;
//	DWORD			dwWorldID;
//	tagDouble(LPCTSTR szEventName) : tagUserEvent(szEventName)
//	{
//		dwErrorCode		= 0;
//		dwWorldID		= 0;
//	}
//};

//右下角公告
//struct tagNotice : public tagUserEvent
//{
//	DWORD			dwErrorCode;
//	DWORD			dwWorldID;
//	tagNotice(LPCTSTR szEventName) : tagUserEvent(szEventName)
//	{
//		dwErrorCode		= 0;
//		dwWorldID		= 0;
//	}
//};

//自动循环公告
//struct tagAutoNotice : public tagUserEvent
//{
//	DWORD			dwErrorCode;
//	DWORD			dwWorldID;
//	tagAutoNotice(LPCTSTR szEventName) : tagUserEvent(szEventName)
//	{
//		dwErrorCode		= 0;
//		dwWorldID		= 0;
//	}
//};

//跑马灯
//struct tagCastRun : public tagUserEvent
//{
//	DWORD		dwErrorCode;
//	tagCastRun(LPCTSTR szEventName) : tagUserEvent(szEventName)
//	{
//		dwErrorCode		= 0;
//	}
//};	

//道具生成
struct tagItemCreate : public tagUserEvent
{
	DWORD		dwErrorCode;
	tagItemCreate(LPCTSTR szEventName) : tagUserEvent(szEventName)
	{
		dwErrorCode		= 0;
	}
};

//道具删除
struct tagItemDelete : public tagUserEvent
{
	DWORD		dwErrorCode;
	tagItemDelete(LPCTSTR szEventName) : tagUserEvent(szEventName)
	{
		dwErrorCode		= 0;
	}
};

//道具恢复
struct tagItemResume : public tagUserEvent
{
	DWORD		dwErrorCode;
	tagItemResume(LPCTSTR szEventName) : tagUserEvent(szEventName)
	{
		dwErrorCode		= 0;
	}
};


//账号封停/解封
struct tagAccountSuspend : public tagUserEvent
{
	DWORD		dwErrorCode;
	TCHAR		szAccount[32];
	tagAccountSuspend(LPCTSTR szEventName) : tagUserEvent(szEventName)
	{
		dwErrorCode		= 0;
	}
};

//加入/删除黑名单中的IP
struct tagIPBlacklist : public tagUserEvent
{
	DWORD		dwErrorCode;
	tagIPBlacklist(LPCTSTR szEventName) : tagUserEvent(szEventName)
	{
		dwErrorCode		= 0;
	}
};

//账号解封
//struct tagAccountUnsuspend : public tagUserEvent
//{
//	DWORD		dwErrorCode;
//	LPCTSTR		szAccount;
//	tagAccountUnsuspend(LPCTSTR szEventName) : tagUserEvent(szEventName)
//	{
//		szAccount = NULL;
//		dwErrorCode		= 0;
//	}
//};