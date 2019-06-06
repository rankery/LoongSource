//-------------------------------------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: team.cpp
// author: wjiang
// actor:
// data: 2008-12-10
// last:
// brief: 队伍类
//-------------------------------------------------------------------------------------------------------
#include "StdAfx.h"
#include "MsgInfoMgr.h"
#include "role.h"
#include "role_mgr.h"
#include "team.h"

#include "../ServerDefine/base_define.h"

MsgInfoMgr g_MsgInfoMgr;

MsgInfoMgr::~MsgInfoMgr()
{
	MsgInfo* pMsgInfo = (MsgInfo*)GT_INVALID;
	std::list<DWORD>		listMsgInfo;
	m_mapMsgInfo.ExportAllKey(listMsgInfo);

	std::list<DWORD>::iterator it = listMsgInfo.begin();

	for (; it != listMsgInfo.end(); ++it )
	{
		pMsgInfo = m_mapMsgInfo.Peek(*it);
		SAFE_DEL(pMsgInfo);
	}
	
	/*TMap<DWORD, MsgInfo*>::TMapIterator it = m_mapMsgInfo.Begin();
	while(m_mapMsgInfo.PeekNext(it, pMsgInfo))
	{
		SAFE_DEL(pMsgInfo);
	}*/

	m_mapMsgInfo.Clear();	
}

//-------------------------------------------------------------------------------------------------------
// 创建脚本通用消息
//-------------------------------------------------------------------------------------------------------
DWORD MsgInfoMgr::BeginMsgEvent()
{
	m_Lock.Acquire();
	MsgInfo* pMsgInfo = new MsgInfo(m_dMsgInfoID);
	DWORD	dwMsgInfoID	= m_dMsgInfoID;

	m_mapMsgInfo.Add(m_dMsgInfoID, pMsgInfo);

	++m_dMsgInfoID;
	m_Lock.Release();

	return dwMsgInfoID;
}

//-------------------------------------------------------------------------------------------------------
// 向消息中添加相关事件数据
//-------------------------------------------------------------------------------------------------------
VOID MsgInfoMgr::AddMsgEvent(DWORD dwMsgInfoID, EMsgUnitType eMsgUnitType, LPVOID pData)
{
	MsgInfo *pMsgInfo = m_mapMsgInfo.Peek(dwMsgInfoID);
	if(!P_VALID(pMsgInfo))
		return;

	pMsgInfo->AddMsgUnit(eMsgUnitType, pData);
}

//-------------------------------------------------------------------------------------------------------
// 发送脚本通用消息给玩家
//-------------------------------------------------------------------------------------------------------
VOID MsgInfoMgr::DispatchRoleMsgEvent(DWORD dwMsgInfoID, Role *pRole)
{
	if(!P_VALID(pRole))
		return;

	MsgInfo *pMsgInfo = m_mapMsgInfo.Peek(dwMsgInfoID);
	if(!P_VALID(pMsgInfo))
		return;

	LPVOID pSend = pMsgInfo->CreateMsgByMsgInfo(g_MemPoolSafe);
	DWORD dwSize = pMsgInfo->GetMsgSize();

	pRole->SendMessage(pSend, dwSize);
	pMsgInfo->DeleteMsg(pSend, g_MemPoolSafe);

	RemoveMsgInfo(pMsgInfo);
}

//-------------------------------------------------------------------------------------------------------
// 给服务器所有地图内的玩家发送脚本通用消息
//-------------------------------------------------------------------------------------------------------
VOID MsgInfoMgr::DispatchWorldMsgEvent(DWORD dwMsgInfoID)
{
	MsgInfo *pMsgInfo = m_mapMsgInfo.Peek(dwMsgInfoID);
	if(!P_VALID(pMsgInfo))
		return;

	LPVOID pSend = pMsgInfo->CreateMsgByMsgInfo(g_MemPoolSafe);
	DWORD dwSize = pMsgInfo->GetMsgSize();

	g_roleMgr.SendWorldMsg(pSend, dwSize);
	pMsgInfo->DeleteMsg(pSend, g_MemPoolSafe);

	RemoveMsgInfo(pMsgInfo);
}

//-------------------------------------------------------------------------------------------------------
// 给同一地图内的玩家发送脚本通用消息
//-------------------------------------------------------------------------------------------------------
VOID MsgInfoMgr::DispatchMapMsgEvent(DWORD dwMsgInfoID, Map* pMap)
{
	if(!P_VALID(pMap))	return;

	MsgInfo *pMsgInfo = m_mapMsgInfo.Peek(dwMsgInfoID);
	if(!P_VALID(pMsgInfo))
		return;

	LPVOID pSend = pMsgInfo->CreateMsgByMsgInfo(g_MemPoolSafe);
	DWORD dwSize = pMsgInfo->GetMsgSize();

	pMap->SendMapMessage(pSend, dwSize);
	pMsgInfo->DeleteMsg(pSend, g_MemPoolSafe);

	RemoveMsgInfo(pMsgInfo);
}

//-------------------------------------------------------------------------------------------------------
// 给同一地图内的小队队友发送脚本通用消息
//-------------------------------------------------------------------------------------------------------
VOID MsgInfoMgr::DispatchTeamMapMsgEvent(DWORD dwMsgInfoID, Map *pMap, Team *pTeam)
{
	if( !P_VALID(pTeam) ) return;
	if( !P_VALID(pMap) ) return;

	MsgInfo *pMsgInfo = m_mapMsgInfo.Peek(dwMsgInfoID);
	if(!P_VALID(pMsgInfo))
		return;

	LPVOID pSend = pMsgInfo->CreateMsgByMsgInfo(g_MemPoolSafe);
	DWORD dwSize = pMsgInfo->GetMsgSize();

	pTeam->SendTeamMesInSameMap(pMap->GetMapID(), pSend, dwSize);
	pMsgInfo->DeleteMsg(pSend, g_MemPoolSafe);

	RemoveMsgInfo(pMsgInfo);
}













