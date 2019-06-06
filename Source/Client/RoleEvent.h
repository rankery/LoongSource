#pragma once
#include "..\WorldDefine\role_att.h"

/** 本地玩家属性初始化
*/
struct tagInitLPAttEvent : public tagGameEvent
{
	tagInitLPAttEvent(LPCTSTR szEventName, GameFrame* pSenderFrame) : tagGameEvent(szEventName,pSenderFrame){}
};

/** 本地玩家属性更新
*/
struct tagUpdateLPAttEvent : public tagGameEvent
{
	ERoleAttribute	eType;
	
	tagUpdateLPAttEvent(LPCTSTR szEventName, GameFrame* pSenderFrame) : tagGameEvent(szEventName,pSenderFrame)
	{eType = ERA_Null;}
};


/** 远程玩家属性更新
*/
struct tagUpdateRRAttEvent : public tagGameEvent
{
	DWORD			dwRoleID;
	ERoleAttribute	eType;

	tagUpdateRRAttEvent(LPCTSTR szEventName, GameFrame* pSenderFrame) : tagGameEvent(szEventName,pSenderFrame)
	{dwRoleID = GT_INVALID; eType = ERA_Null;}
};

/** 玩家装备外观变化
*/
struct tagUpdataAvatarEvent : public tagGameEvent
{
	DWORD           dwRoleID;
	
	tagUpdataAvatarEvent(LPCTSTR szEventName, GameFrame* pSenderFrame) : 
	tagGameEvent(szEventName,pSenderFrame), dwRoleID( GT_INVALID ) {}
};

/** 玩家显示设置变化
*/
struct tagUpdataAvatarDisplaySetEvent : public tagGameEvent
{
	DWORD           dwRoleID;

	tagUpdataAvatarDisplaySetEvent(LPCTSTR szEventName, GameFrame* pSenderFrame) : 
	tagGameEvent(szEventName,pSenderFrame), dwRoleID( GT_INVALID ) {}
};

/** 玩家武器更换
*/
struct tagWeaponChangeEvent : public tagGameEvent
{
	DWORD           dwRoleID;
	BOOL            bRightWeapon;  // 是否主手武器
	tagWeaponChangeEvent(LPCTSTR szEventName, GameFrame* pSenderFrame) : 
	tagGameEvent(szEventName,pSenderFrame), dwRoleID( GT_INVALID ), bRightWeapon( FALSE) {}
};

/** 本地玩家等级改变
*/
struct tagUpdateLPLevel : public tagGameEvent
{
	INT		nCurLevel;	// 当前等级
	INT		nInc;		// 等级变化量
	BOOL	bLevelUp;	// 是否是升级	

	tagUpdateLPLevel(LPCTSTR szEventName, GameFrame* pSenderFrame) :
	tagGameEvent(szEventName,pSenderFrame){}
};

/** 远程玩家等级改变
*/
struct tagUpdateRRLevel : public tagGameEvent
{
	DWORD	dwRoleID;
	INT		nCurLevel;
	BOOL	bLevelUp;

	tagUpdateRRLevel(LPCTSTR szEventName, GameFrame* pSenderFrame) :
	tagGameEvent(szEventName,pSenderFrame){}
};

/** 本地玩家经验变化
*/
struct tagUpdateLPLevelExp : public tagGameEvent
{
	INT nCurLevelExp;
	INT nExpInc;

	tagUpdateLPLevelExp(LPCTSTR szEventName, GameFrame* pSenderFrame) :
	tagGameEvent(szEventName,pSenderFrame){}
};

/** 角色buff列表变化
*/
struct tagRoleBuffChangeEvent : public tagGameEvent
{
	DWORD dwRoleID;

	tagRoleBuffChangeEvent(LPCTSTR szEventName, GameFrame* pSenderFrame) :
	tagGameEvent(szEventName,pSenderFrame){}
};

/** 本地玩家状态变化
*/
struct tagLPRoleStateChangeEvent : public tagGameEvent
{
	DWORD dwOldState;
	tagLPRoleStateChangeEvent(LPCTSTR szEventName, GameFrame* pSenderFrame) :
	tagGameEvent(szEventName,pSenderFrame),dwOldState(0){}
};

/** 远程玩家状态变化
*/
struct tagRPRoleStateChangeEvent : public tagGameEvent
{
	DWORD dwRoleID;
	DWORD dwOldState;
	tagRPRoleStateChangeEvent(LPCTSTR szEventName, GameFrame* pSenderFrame) :
	tagGameEvent(szEventName,pSenderFrame),dwOldState(0){}
};

/*! \struct tagFashionEvent
\brief 角色切换时装模式事件
*/
struct tagFashionEvent : public tagGameEvent
{
	bool		bDisFashion;	/*!< 是否设置时装模式 */
	tagFashionEvent(LPCTSTR szEventName, GameFrame *pSenderFrame)
		: tagGameEvent(szEventName, pSenderFrame){}
};

/** N%经验达成
*/
struct tagPctNExpAchieve : public tagGameEvent
{
	INT		nPctMultiplied;		// N%
	tagPctNExpAchieve(LPCTSTR szEventName, GameFrame *pSenderFrame)
		: tagGameEvent(szEventName, pSenderFrame){}
};

/** 进入\离开战斗
*/
struct tagLRoleCombatState : public tagGameEvent
{
	BOOL	bEnterCombat;		//true:进入战斗 false:离开战斗
	tagLRoleCombatState(LPCTSTR szEventName, GameFrame *pSenderFrame)
		: tagGameEvent(szEventName, pSenderFrame),bEnterCombat(TRUE){}
};

//! 角色获得新称号
struct tagNewTitleDisp : public tagGameEvent
{
    tstring     strShow;        // 称号名
    tagNewTitleDisp(LPCTSTR szEventName, GameFrame* pSenderFrame)
        : tagGameEvent(szEventName, pSenderFrame) {}
};

//! 角色删除称号
struct tagDelTitleDisp : public tagGameEvent
{
    tstring     strShow;        // 称号名
    tagDelTitleDisp(LPCTSTR szEventName, GameFrame* pSenderFrame)
        : tagGameEvent(szEventName, pSenderFrame) {}
};

//! 本地及远程玩家改变称号
struct tagRoleTitleChange : public tagGameEvent
{
    DWORD			dwRoleID;
    UINT16			dwTitleID;		// 若玩家选择隐藏称号，该字段为GT_INVALID

    tagRoleTitleChange(LPCTSTR szEventName, GameFrame* pSenderFrame)
        : tagGameEvent(szEventName, pSenderFrame) {}
};
