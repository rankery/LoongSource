//-------------------------------------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: event_mgr.cpp
// author: wjiang
// actor:
// data: 2008-12-02
// last:
// brief: 事件管理类
//-------------------------------------------------------------------------------------------------------

#pragma once

#include "mutex.h"

//-------------------------------------------------------------------------------------------------------
// 事件类型
//-------------------------------------------------------------------------------------------------------
enum  EEventType
{
	EVT_NULL				=	-1,

	//---------------------------------------------------------
	// 好友相关
	//---------------------------------------------------------
	EVT_MakeFriend			=	0,
	EVT_CancelFriend		=	1,
	EVT_FriendGrp			=	2,
	EVT_InsertBlkList		=	3,
	EVT_DeleteBlkList		=	4,
	EVT_SendGift			=	5,
	EVT_SendGiftRpy			=	6,

	//---------------------------------------------------------
	// 队伍相关
	//---------------------------------------------------------
	EVT_JoinTeam			=	7,
	EVT_JoinTeamRepley		=	8,
	EVT_KickMember			=	9,
	EVT_LeaveTeam			=	10,
	EVT_SetPickMol			=	11,
	EVT_ChangeLeader		=	12,
	EVT_ChangeLevel			=	13,
	EVT_ChangeRein			=	14,

	EVT_SynRoleLevel		=   16,

	EVT_AddAllRoleBuff		=	17,

	EVT_GuildWareMoveTo		=	18,	
	EVT_GuildWareMove2Other	=	19,

	//----------------------------------------------------------	
	// VIP摊位相关	
	//----------------------------------------------------------	
	EVT_SetStallTitle		=	20,		// 设置摊位标题	
	EVT_SetStallStatus		=	21,		// 设置摊位状态	
	EVT_ChangeStallGoods	=	22,		// 变更摊位商品	

	//----------------------------------------------------------	
	// 脚本相关	
	//----------------------------------------------------------	
	EVT_Script_Reload		=	101,
	EVT_CreateTeam			=	102,
	EVT_AddMember			=	103,

	//申请入队相关协议
	// Jason 2009-11-25
	EVT_ApplyToJoinTeam		= 104,
	EVT_ApplyReply			= 105,

	EVT_End,
};

//--------------------------------------------------------------------------------------------------------
// 游戏内事件
//--------------------------------------------------------------------------------------------------------
template<class T>
class EventMgr;

class EventObj
{
	template<class T> friend class EventMgr;

private:
	EventObj(DWORD dwSender, VOID* pMsg, EEventType eType, DWORD dwSize)
	{
		// 初始化
		m_dwSender = dwSender;
		m_dwSize = dwSize;
		m_eType = eType;

		if( 0 == dwSize )
		{
			m_pMsg = NULL;
		}
		else
		{
			m_pMsg = new BYTE[dwSize];
			memcpy(m_pMsg, pMsg, dwSize);
		}
	}

	~EventObj() { SAFE_DEL_ARRAY(m_pMsg); }

	DWORD		GetSender() { return m_dwSender; }
	VOID*		GetEvent() { return m_pMsg; }
	DWORD		GetEventSize() { return m_dwSize; }
	EEventType	GetEventType() { return m_eType; }

private:
	DWORD				m_dwSender;
	VOID*				m_pMsg;
	EEventType			m_eType;
	DWORD				m_dwSize;
};

//-------------------------------------------------------------------------------------------------------
// 该类的成员函数只能在主线程调用（AddEvent除外）
//-------------------------------------------------------------------------------------------------------
template<class T>
class EventMgr
{
public:
	typedef VOID (T::*EVENTMESSAGEFUNC)(DWORD dwSender, VOID* pMsg);
	typedef std::map<EEventType, EVENTMESSAGEFUNC>	EventFuncMap;

public:
	BOOL Init();
	~EventMgr();

	VOID Update();		
	VOID AddEvent(DWORD dwSender, EEventType eType, DWORD dwSize, VOID* pMsg);

protected:
	static VOID RegisterEventFunc(EEventType EventType, EVENTMESSAGEFUNC EventMessageFunc);

private:
	TList<EventObj*>				m_listEventObj;	
	Mutex							m_Lock;
	static EventFuncMap				m_mapEventFunc;	// 事件触发器类型及对应的处理函数
};

template<class T>
typename EventMgr<T>::EventFuncMap EventMgr<T>::m_mapEventFunc; 

//-------------------------------------------------------------------------------------------------------
// 注册事件处理函数
//-------------------------------------------------------------------------------------------------------
template<class T>
VOID EventMgr<T>::RegisterEventFunc(EEventType eEventType, EVENTMESSAGEFUNC EventMessageFunc)
{
	m_mapEventFunc.insert(make_pair(eEventType,	EventMessageFunc));
}

//-------------------------------------------------------------------------------------------------------
// 事件管理类初始化
//-------------------------------------------------------------------------------------------------------
template<class T>
BOOL EventMgr<T>::Init()
{
	return TRUE;
}

//-------------------------------------------------------------------------------------------------------
// 析构函数
//-------------------------------------------------------------------------------------------------------
template<class T>
EventMgr<T>::~EventMgr()
{
	EventObj* pEvent = m_listEventObj.PopFront();
	while( P_VALID(pEvent) )
	{
		SAFE_DEL(pEvent);
		pEvent = m_listEventObj.PopFront();
	}
}


//-------------------------------------------------------------------------------------------------------
// 管理类事件处理
//-------------------------------------------------------------------------------------------------------
template<class T>
VOID EventMgr<T>::Update()
{
	EventObj* pOjb = m_listEventObj.PopFront();
	while( P_VALID(pOjb) )
	{
		EventFuncMap::iterator it = m_mapEventFunc.find(pOjb->GetEventType());
		if( it != m_mapEventFunc.end() )
		{
			EVENTMESSAGEFUNC handler = it->second;
			(((T*)this)->*handler)(pOjb->GetSender(), pOjb->GetEvent());
		}
		SAFE_DEL(pOjb);

		pOjb = m_listEventObj.PopFront();
	}
}

//-------------------------------------------------------------------------------------------------------
// 加入事件
//-------------------------------------------------------------------------------------------------------
template<class T>
VOID EventMgr<T>::AddEvent(DWORD dwSender, EEventType eType, DWORD dwSize, VOID* pMsg)
{
	ASSERT( eType > EVT_NULL && eType < EVT_End );

	EventObj* pObj = new EventObj(dwSender, pMsg, eType, dwSize);

	m_Lock.Acquire();
	m_listEventObj.PushBack(pObj);
	m_Lock.Release();
}
