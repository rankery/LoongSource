//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: role_marriage.cpp
// author: songg
// actor:
// data: 2009-09-10
// last:
// brief: 玩家间结婚和婚礼的处理
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "role.h"
#include "map.h"
#include "marriage.h"
#include "role_mgr.h"
#include "../WorldDefine/msg_marriage.h"

//------------------------------------------------------------------------
// 请求结婚
//------------------------------------------------------------------------
DWORD Role::ProcGetMarriageReq(OUT Role* &pTarget, DWORD dwTgtRoleID, DWORD dwNpcID)
{
	// 是否自己和自己结婚
	if(GetID() == dwTgtRoleID)
	{
		IMSG(_T("Player<id: %u> is GetMarriage with himself!!!!!\r\n"), dwTgtRoleID);
		return GT_INVALID;
	}
	// 获取地图
	Map *pMap = GetMap();
	if(!P_VALID(pMap))
	{
		IMSG(_T("Cant't find the map when tackling the GetMarriageReq message\r\n"));
		return GT_INVALID;
	}
	// 被申请者检查
	pTarget = pMap->FindRole(dwTgtRoleID);
	if(!P_VALID(pTarget))
	{
		// 被申请者不在线,或与申请者不在同一张地图中
		return E_Marriage_NotInSame_Map;
	}
	
	//调一下结婚check脚本
	DWORD dwErrorCode = g_ScriptMgr.GetRoleScript()->OnGetMarriageCheck(this, this->GetID(), dwTgtRoleID);
	if(dwErrorCode != E_Marriage_Accept)
	{
		return dwErrorCode;
	}

	//检查源玩家和目标玩家距离月老的距离是否在限制区内
	Unit *pYueLao = pMap->FindUnit(dwNpcID);
	if(!P_VALID(pYueLao))
	{
		IMSG(_T("Cant't find the the npc yuelao in this map\r\n"));
		return GT_INVALID;
	}
	//请求结婚的玩家距离月老 10个格子内
	if(!IsInDistance(*pYueLao, 10*TILE_SCALE))
	{
		return E_Marriage_OutOfRange;
	}
	//被请求结婚的玩家也要距离月老 10个格子内
	if(!pTarget->IsInDistance(*pYueLao, 10*TILE_SCALE))
	{
		return E_Marriage_OutOfRange;
	}

	return E_Marriage_Accept;
}
//------------------------------------------------------------------------
// 玩家对请求结婚的反馈
//------------------------------------------------------------------------
DWORD Role::ProcGetMarriageReqRes(OUT Role* &pApplicant, DWORD dwApplicantRoleID, DWORD dwErrorCode)
{
	// 获取地图
	Map *pMap = GetMap();
	if(!P_VALID(pMap))
	{
		IMSG(_T("Cant't find the map when tackling the GetMarriageReqRes message.\r\n"));
		return GT_INVALID;
	}

	// 结婚申请者检查
	pApplicant = pMap->FindRole(dwApplicantRoleID);
	if(!P_VALID(pApplicant))
	{
		// 申请者下线，或者更换地图
		return E_Marriage_NotInSame_Map;
	}
	//调一下结婚check脚本
	DWORD dwScriptErrCode = g_ScriptMgr.GetRoleScript()->OnGetMarriageCheck(this, pApplicant->GetID(), this->GetID());
	if(dwScriptErrCode != E_Marriage_Accept)
	{
		return dwScriptErrCode;
	}

	return dwErrorCode;
}
//------------------------------------------------------------------------
// 请求离婚
//------------------------------------------------------------------------
DWORD Role::ProcBreakMarriageReq(OUT Role* &pTarget, DWORD dwTgtRoleID)
{
	// 是否自己和自己离婚
	if(GetID() == dwTgtRoleID)
	{
		IMSG(_T("Player<id: %u> is BreakMarriage with himself!!!!!\r\n"), dwTgtRoleID);
		return GT_INVALID;
	}
	// 获取地图
	Map *pMap = GetMap();
	if(!P_VALID(pMap))
	{
		IMSG(_T("Cant't find the map when tackling the BreakMarriageReq message\r\n"));
		return GT_INVALID;
	}
	// 被申请者检查
	pTarget = pMap->FindRole(dwTgtRoleID);
	if(!P_VALID(pTarget))
	{
		// 被申请者不在线,或与申请者不在同一张地图中
		return E_Marriage_NotInSame_Map;
	}

	//调一下离婚check脚本
	DWORD dwErrCode = g_ScriptMgr.GetRoleScript()->OnBreakMarriageCheck(this, this->GetID(), dwTgtRoleID);
	if(dwErrCode != E_Marriage_Accept)
	{
		return dwErrCode;
	}
	
	return E_Marriage_Accept;
}
//------------------------------------------------------------------------
// 玩家对请求离婚的反馈
//------------------------------------------------------------------------
DWORD Role::ProcBreakMarriageReqRes(OUT Role* &pApplicant, DWORD dwApplicantRoleID, DWORD dwErrorCode)
{
	// 获取地图
	Map *pMap = GetMap();
	if(!P_VALID(pMap))
	{
		IMSG(_T("Cant't find the map when tackling the BreakMarriageReqRes message.\r\n"));
		return GT_INVALID;
	}
	//离婚申请者检查
	pApplicant = pMap->FindRole(dwApplicantRoleID);
	if(!P_VALID(pApplicant))
	{
		// 申请者下线，或者更换地图
		return E_Marriage_NotInSame_Map;
	}
	// 是否自己离婚
	if(pApplicant->GetID() == this->GetID())
	{
		IMSG(_T("Player<id: %u> is BreakMarriage with himself!!!!!\r\n"), dwApplicantRoleID);
		return GT_INVALID;
	}
	//调一下离婚check脚本
	DWORD dwScriptErrCode = g_ScriptMgr.GetRoleScript()->OnBreakMarriageCheck(this, pApplicant->GetID(), this->GetID());
	if(dwScriptErrCode != E_Marriage_Accept)
	{
		return dwScriptErrCode;
	}
	return dwErrorCode;
}
//------------------------------------------------------------------------
// 婚礼请求
//------------------------------------------------------------------------
DWORD Role::ProcCelebrateWeddingReq(OUT Role* &pTarget,OUT Role* &pOfficiatorRole,DWORD dwOfficiatorNameCrc,DWORD dwNpcID)
{
	pTarget = NULL;
	pOfficiatorRole = NULL;

	// 检查是否有婚礼在进行
	if(g_MarriageMgr.IsHavingWedding())
	{
		return E_Wedding_HavingWedding;
	}

	// 看请求者是否已婚
	if(IsRoleMarried() == FALSE)
	{
		return E_Wedding_NotMerried;
	}
	// 看请求者是否已经举行过婚礼
	if(IsRoleWedded() == TRUE)
	{
		return E_Wedding_AlreadyWedded;
	}

	// 获取地图
	Map *pMap = GetMap();
	if(!P_VALID(pMap))
	{
		IMSG(_T("Cant't find the map when tackling the CelebrateWeddingReq message.\r\n"));
		return GT_INVALID;
	}
	//找爱人
	pTarget = pMap->FindRole(GetLoverID());
	if(!P_VALID(pTarget))
	{
		// 爱人下线，或者更换地图
		return E_Wedding_NotInSame_Map;
	}

	//看主婚人姓名是不是空
	if(dwOfficiatorNameCrc == -1)
	{
		//开启系统主婚模式
		g_MarriageMgr.EnableSysOfficiator();
	}
	else
	{
		//找主婚人
		pOfficiatorRole = pMap->FindRole(g_roleMgr.GetRoleIDByNameCrc(dwOfficiatorNameCrc));
		if(!P_VALID(pOfficiatorRole))
		{
			// 主婚人下线，或者更换地图
			return E_Wedding_NotInSame_Map;
		}
	}

	//检查这3个人到npc月老的距离
	Unit *pYueLao = pMap->FindUnit(dwNpcID);
	if(!P_VALID(pYueLao))
	{
		IMSG(_T("Cant't find the the npc yuelao in this map\r\n"));
		return GT_INVALID;
	}
	//请求结婚的玩家距离月老 10个格子内
	if(!IsInDistance(*pYueLao, 10*TILE_SCALE))
	{
		return E_Marriage_OutOfRange;
	}
	//配偶
	if(!pTarget->IsInDistance(*pYueLao, 10*TILE_SCALE))
	{
		return E_Marriage_OutOfRange;
	}
	//如果主婚人有效
	if(dwOfficiatorNameCrc != -1  && P_VALID(pOfficiatorRole))
	{
		//检查主婚人距离
		if(!pOfficiatorRole->IsInDistance(*pYueLao, 10*TILE_SCALE))
		{
			return E_Marriage_OutOfRange;
		}
	}
	return E_Wedding_Success;
}
//------------------------------------------------------------------------
// 主婚人的反馈
//------------------------------------------------------------------------
DWORD Role::ProcCelebrateWeddingReqRes(OUT Role* &pApplicant, DWORD dwErrorCode)
{
	DWORD dwApplicantID = g_MarriageMgr.GetApplicantRoleID();
	// 获取地图
	Map *pMap = GetMap();
	if(!P_VALID(pMap))
	{
		IMSG(_T("Cant't find the map when tackling the CelebrateWeddingReqRes message.\r\n"));
		return GT_INVALID;
	}
	pApplicant = pMap->FindRole(dwApplicantID);
	if(!P_VALID(pApplicant))
	{
		return E_Wedding_NotInSame_Map;
	}

	if(dwErrorCode != E_Wedding_Success)
	{
		return dwErrorCode;
	}
	/*
	夫妻双方、主婚人是否在线。
	夫妻双方、主婚人是否在NPC月老10格子范围之内。
	当前是否正有婚礼正在举行
	申请人现金是否足够XXX
	*/
	//只检查在线、距离、金钱就可以了
	DWORD dwTargetID = g_MarriageMgr.GetTargetRoleID();
	Role* pTarget = pMap->FindRole(dwTargetID);
	if(!P_VALID(pApplicant))
	{
		return E_Wedding_NotInSame_Map;
	}

/*
	//检查这3个人到npc月老的距离
	Unit *pYueLao = pMap->FindUnit(g_MarriageMgr.GetYueLaoNPCID());
	if(!P_VALID(pYueLao))
	{
		IMSG(_T("Cant't find the the npc yuelao in this map\r\n"));
		return GT_INVALID;
	}
	//主婚人距离月老 10个格子内
	if(!IsInDistance(*pYueLao, 10*TILE_SCALE))
	{
		return E_Wedding_OutOfRange;
	}
	//被申请人
	if(!pTarget->IsInDistance(*pYueLao, 10*TILE_SCALE))
	{
		return E_Wedding_OutOfRange;
	}
	//申请人
	if(!pApplicant->IsInDistance(*pYueLao, 10*TILE_SCALE))
	{
		return E_Wedding_OutOfRange;
	}

	//检查申请人金钱
	if(dwErrorCode == E_Wedding_Success)
	{

		if(pApplicant->GetCurMgr().GetBagSilver() < g_MarriageMgr.GetBreakMarriageDecreaseSilver())
		{
			return E_Marriage_NotEnough_Money;		
		}
	}
*/
	return E_Wedding_Success;
}