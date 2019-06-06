#pragma once

#include "Singleton.h"
#include "Mutex.h"
#include "../ServerDefine/fatigue_define.h"

//-------------------------------------------------------------------------
// 防沉迷管理器
//-------------------------------------------------------------------------
class FatigueMgr : public Singleton<FatigueMgr>
{
public:
	// 初始化，读取玩家状态等等
	BOOL	Init();

	// 读取所有在线防沉迷玩家
	BOOL	LoadAllOLAccounts();

	// 通知防沉迷服务器，多线程运行
	VOID	NotifyFagitueSvr();

	// 清楚某个游戏世界的玩家帐号
	VOID	ResetWorldAccounts(DWORD dwWorldNameCrcReset, DWORD *pOLAccountIDs, INT nOLAccountIDNum);

	// 登入通知，多线程运行
	VOID	LoginNotify(DWORD dwAccount, DWORD dwWorldNameCrc, BOOL bGuard);

	// 登出通知， 多线程运行
	VOID	LogoutNotify(DWORD dwAccount);

	// 通知玩家累计在线时间，单线程中运行
	VOID	NotifyPlayer(const tagFatigueNotify* pNotify);

	// 向world发送消息，单线程运行
	VOID	SendWorldNotifys();

	// 主线程更新
	VOID	Update();

private:
	//accountid, worldnamecrc
	TMap<DWORD, DWORD>						m_mapAccountID2WorldNameCrc;
	Mutex									m_setLock;

	// worldnamecrc, list
	TMap<DWORD, TList<tagFatigueNotify*>* > m_mapNotifyLists;
	Mutex									m_mapLock;
	
	// 分钟计时器
	INT										m_nMinCounter;
	INT										m_nCircleUnit;

	BOOL									m_bEnable;
};

#define sFatigueMgr FatigueMgr::getSingleton()