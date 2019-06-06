//------------------------------------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: map_restrict.h
// author: aslan
// actor:
// data: 2009-03-26
// last:
// brief: 地图限制类，主要用于地图的进入判断及出口判断
// remark: 嵌入到地图管理器中，用于某一类地图的总体判断，针对于副本实例的判断，则由各个副本的地图派生类实现
//------------------------------------------------------------------------------------------------------

#include "StdAfx.h"
#include "map_restrict.h"
#include "map.h"
#include "map_mgr.h"
#include "map_creator.h"
#include "role.h"
#include "script_mgr.h"

//-------------------------------------------------------------------------------------------------------
// CONSTS
//-------------------------------------------------------------------------------------------------------
//const INT MAX_STABLE_INSTANCE_NUM = 10;			// 稳定副本开的最大个数

//-------------------------------------------------------------------------------------------------------
// 抽象类
//-------------------------------------------------------------------------------------------------------
MapRestrict::MapRestrict() : m_pMgr(NULL)
{

}

MapRestrict::~MapRestrict()
{
	//m_pScript = NULL;
}

VOID MapRestrict::Init(MapMgr* pMapMgr)
{
	ASSERT(P_VALID(pMapMgr));

	m_pMgr	= pMapMgr;
	m_pInfo	= pMapMgr->GetMapInfo();
}

//---------------------------------------------------------------------------------------------------------
// 普通地图
//---------------------------------------------------------------------------------------------------------
VOID MapRestrictNormal::Init(MapMgr* pMapMgr)
{
	MapRestrict::Init(pMapMgr);
}

Map* MapRestrictNormal::CanEnter(Role* pRole, DWORD dwMisc)
{
	ASSERT( P_VALID(pRole) );

	return m_pMgr->GetSingleMap();
}

Map* MapRestrictNormal::CanTakeOverWhenOnline(Role* pRole, DWORD& dwOutMapID, Vector3& vOut)
{
	ASSERT( P_VALID(pRole) );

	return m_pMgr->GetSingleMap();
}

BOOL MapRestrictNormal::GetExportMapAndCoord(Role* pRole, DWORD& dwOutMapID, Vector3& vOut)
{
	return FALSE;
}

//----------------------------------------------------------------------------------------------------------
// 副本地图
//----------------------------------------------------------------------------------------------------------
VOID MapRestrictInstance::Init(MapMgr* pMapMgr)
{
	MapRestrict::Init(pMapMgr);
}

//----------------------------------------------------------------------------------------------------------
// 某个副本是否能够进入
//----------------------------------------------------------------------------------------------------------
Map* MapRestrictInstance::CanEnter(Role* pRole, DWORD dwMisc)
{
	const tagMapInfo* pInfo = m_pMgr->GetMapInfo();
	if( !P_VALID(pInfo) || EMT_Instance != pInfo->eType ) return NULL;

	MapInstance* pInstance = NULL;

	// 首先判断副本静态属性
	INT nRet = CanEnterByInstanceInfo(pRole);

	if( E_Success == nRet )
	{
		// 再找到副本实例
		DWORD dwInstanceID = pRole->GetOwnInstanceID();

		if( P_VALID(dwInstanceID) )
		{
			pInstance =  m_pMgr->GetInstance(dwInstanceID);
			if( !P_VALID(pInstance) )
			{
				// 副本实例不存在
				nRet = E_Instance_Not_Exit;
			}
			else
			{
				// 副本实例存在，检测实例本身是否可以进入
				nRet = pInstance->CanEnter(pRole);
			}
		}
		else
		{
			// 玩家或队伍都没有副本，则创建一个
			pInstance = m_pMgr->CreateInstance(pRole, dwMisc);
			if( !P_VALID(pInstance) )
			{
				nRet = E_Instance_Full;
			}
		}
	}

	if( E_Success != nRet )		// 如果副本不允许进入，则发送给玩家消息
	{
		tagNS_EnterInstance send;
		send.dwTimeLimit = GT_INVALID;
		send.dwErrorCode = nRet;

		pRole->SendMessage(&send, send.dwSize);
		return NULL;
	}

	return pInstance;		// 将副本返回
}

//-----------------------------------------------------------------------------------------------------------
// 根据副本静态属性判断是否能够进入
//-----------------------------------------------------------------------------------------------------------
INT MapRestrictInstance::CanEnterByInstanceInfo(Role* pRole)
{
	const tagInstance* pProto = g_attRes.GetInstanceProto(m_pInfo->dwID);
	if( !P_VALID(pProto) )
	{
		IMSG(_T("Can't find Instance Proto %u\r\n"), m_pInfo->dwID);
		return GT_INVALID;
	}

	// 玩家或其所在队伍是不是已经创建了其它
	DWORD dwOwnInstanceMapID = pRole->GetOwnInstanceMapID();
	if( P_VALID(dwOwnInstanceMapID) && dwOwnInstanceMapID != m_pInfo->dwID )
		return E_Instance_Already;

	// 等级限制
	if( pProto->nLevelDownLimit > pRole->GetLevel() )
		return E_Instance_Level_Down_Limit;

	if( pProto->nLevelUpLimit < pRole->GetLevel() )
		return E_Instance_Level_Up_Limit;

	// 人数下限
	if( pProto->nNumDownLimit > 1 )
	{
		const Team* pTeam = g_groupMgr.GetTeamPtr(pRole->GetTeamID());
		if( !P_VALID(pTeam) || pTeam->GetMemNum() < pProto->nNumDownLimit ) 
			return E_Instance_Role_Lack;
	}

	return E_Success;
}

//------------------------------------------------------------------------------------------------------------
// 玩家上线后的接管处理
//------------------------------------------------------------------------------------------------------------
Map* MapRestrictInstance::CanTakeOverWhenOnline(Role* pRole, DWORD& dwOutMapID, Vector3& vOut)
{
	// 玩家在副本中下线，再上线的话不会再停留在副本内，而要将玩家传出
	GetExportMapAndCoord(pRole, dwOutMapID, vOut);

	return NULL;
}

//------------------------------------------------------------------------------------------------------------
// 得到出口地图和坐标
//------------------------------------------------------------------------------------------------------------
BOOL MapRestrictInstance::GetExportMapAndCoord(Role* pRole, DWORD& dwOutMapID, Vector3& vOut)
{
	const tagInstance* pProto = g_attRes.GetInstanceProto(m_pInfo->dwID);
	if( !P_VALID(pProto) )
	{
		IMSG(_T("Can't find Instance Proto %u\r\n"), m_pInfo->dwID);
		return FALSE;
	}

	switch( pProto->eExportMode )
	{
	case EEM_Born:			// 出生点
		{
			dwOutMapID = g_mapCreator.GetBornMapID();
			vOut = g_mapCreator.RandGetOneBornPos();

			return TRUE;
		}
		break;

	case EEM_Reborn:		// 复活点
	case EEM_Current:		// 当前点
		{
			dwOutMapID = pRole->GetRebornMapID();
			const tagMapInfo* pInfo = g_mapCreator.GetMapInfo(dwOutMapID);
			if( !P_VALID(pInfo) ) return FALSE;
			vOut = pInfo->vRebornPos;

			return TRUE;
		}
		break;

	case EEM_Appoint:		// 指定
		{
			dwOutMapID = pProto->dwExportMapID;
			vOut = pProto->vExportPos;

			return TRUE;
		}
		break;

	default:
		break;
	}

	return FALSE;
}

//------------------------------------------------------------------------------------------------------------------
// 初始化
//------------------------------------------------------------------------------------------------------------------
VOID MapRestrictStable::Init(MapMgr* pMapMgr)
{
	MapRestrict::Init(pMapMgr);

	// 生成稳定副本
	INT nNum = g_mapCreator.GetStableInstanceNum();

	for(INT n = 0; n < nNum; ++n)
	{
		pMapMgr->CreateInstance(NULL, GT_INVALID);
	}
}

//-------------------------------------------------------------------------------------------------------------------
// 是否可以进入
//-------------------------------------------------------------------------------------------------------------------
Map* MapRestrictStable::CanEnter(Role* pRole, DWORD dwMisc)
{
	const tagMapInfo* pInfo = m_pMgr->GetMapInfo();
	if( !P_VALID(pInfo) || EMT_System != pInfo->eType ) return NULL;

	// 返回一个合适的map给上层
	MapInstance* pInstance = GetOnePerfectMap();

	return pInstance;
}

//------------------------------------------------------------------------------------------------------------
// 玩家上线后的接管处理
//------------------------------------------------------------------------------------------------------------
Map* MapRestrictStable::CanTakeOverWhenOnline(Role* pRole, DWORD& dwOutMapID, Vector3& vOut)
{
	// 可以接管，找到一个人数最少的
	return GetOnePerfectMap();
}

//-------------------------------------------------------------------------------------------------------------
// 得到出口地图和坐标
//-------------------------------------------------------------------------------------------------------------
BOOL MapRestrictStable::GetExportMapAndCoord(Role* pRole, DWORD& dwOutMapID, Vector3& vOut)
{
	return FALSE;
}

//--------------------------------------------------------------------------------------------------------------------
// 找到一个最合适的副本实例
//--------------------------------------------------------------------------------------------------------------------
MapInstance* MapRestrictStable::GetOnePerfectMap()
{
	MapInstance* pInstance = NULL;	// 人数最少的副本
	INT nMinRoleNum = INT_MAX;		// 最少人数

	// 从mapmgr中找到一个人数最少的副本
	MapInstance* pTemp = NULL;
	TMap<DWORD, MapInstance*>::TMapIterator it = m_pMgr->m_mapInstance.Begin();

	while( m_pMgr->m_mapInstance.PeekNext(it, pTemp) )
	{
		INT nNum = pTemp->GetRoleNum();
		if( nNum < nMinRoleNum )
		{
			nMinRoleNum = nNum;
			pInstance = pTemp;
		}
	}

	return pInstance;
}

//------------------------------------------------------------------------------------------------------------------
// 初始化
//------------------------------------------------------------------------------------------------------------------
VOID MapRestrictScript::Init(MapMgr* pMapMgr)
{
	MapRestrict::Init(pMapMgr);

	// 脚本也在这里进行初始化
	m_pScript = g_ScriptMgr.GetMapScript(pMapMgr->GetMapInfo()->dwID);
}

//-------------------------------------------------------------------------------------------------------------------
// 是否可以进入
//-------------------------------------------------------------------------------------------------------------------
Map* MapRestrictScript::CanEnter(Role* pRole, DWORD dwMisc)
{
	const tagMapInfo* pInfo = m_pMgr->GetMapInfo();
	if( !P_VALID(pInfo) || EMT_ScriptCreate != pInfo->eType ) return NULL;

	MapInstance* pInstance = NULL;

	if ( P_VALID(m_pScript) )
	{
		if(m_pScript->CanEnter(pRole))
		{
			// 返回一个合适的map给上层
			pInstance = GetOnePerfectMap(pRole);
		}
	}

	return pInstance;
}

//------------------------------------------------------------------------------------------------------------
// 玩家上线后的接管处理
//------------------------------------------------------------------------------------------------------------
Map* MapRestrictScript::CanTakeOverWhenOnline(Role* pRole, DWORD& dwOutMapID, Vector3& vOut)
{
	// 调用脚本
	if( P_VALID(m_pScript) )
	{
		 m_pScript->CanTakeOverWhenOnline(pRole, dwOutMapID, vOut);
	}

	return NULL;
}

//-------------------------------------------------------------------------------------------------------------
// 得到出口地图和坐标
//-------------------------------------------------------------------------------------------------------------
BOOL MapRestrictScript::GetExportMapAndCoord(Role* pRole, DWORD& dwOutMapID, Vector3& vOut)
{
	if( P_VALID(m_pScript) )
	{
		BOOL  bSuccess = FALSE; 
		bSuccess =  m_pScript->GetExportMapAndCoord(pRole, dwOutMapID, vOut);

		return bSuccess;
	}

	return FALSE;
}

//--------------------------------------------------------------------------------------------------------------------
// 找到一个最合适的副本实例
//--------------------------------------------------------------------------------------------------------------------
MapInstance* MapRestrictScript::GetOnePerfectMap(Role* pRole)
{
	DWORD  dwInstanceID = GT_INVALID;
	if( P_VALID(m_pScript) )
	{
		m_pScript->GetOnePerfectMap(pRole, dwInstanceID);
	}

	return m_pMgr->GetInstance(dwInstanceID);
}