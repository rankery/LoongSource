//-----------------------------------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: map_mgr.cpp
// author: aslan
// actor:
// data: 2008-6-27
// last:
// brief: 地图管理器
// remark: 每个地图管理器生成一条线程，对该地图中的所有人物及其他对象进行更新操作，同时完成地图间同步
//------------------------------------------------------------------------------------------------------
#include "StdAfx.h"
#include "map_mgr.h"
#include "map.h"
#include "map_creator.h"
#include "map_restrict.h"
#include "role.h"

VOLATILE BOOL MapMgr::m_bTerminate = FALSE;
//-------------------------------------------------------------------------------------------------------
// constructor
//-------------------------------------------------------------------------------------------------------
MapMgr::MapMgr() : m_pInfo(NULL), m_pSingleMap(NULL), m_pRestrict(NULL), m_nInstanceNum(0)
{
	m_mapInstance.Clear();
}

//-------------------------------------------------------------------------------------------------------
// destructor
//-------------------------------------------------------------------------------------------------------
MapMgr::~MapMgr()
{
	Destroy();
}

//-------------------------------------------------------------------------------------------------------
// 初始化地图信息，副本的初始化不在这里进行
//-------------------------------------------------------------------------------------------------------
BOOL MapMgr::Init(tagMapInfo* pInfo)
{
	ASSERT( P_VALID(pInfo) );

	m_pInfo = pInfo;

	// 生成地图策略
	m_pRestrict = g_mapCreator.CreateAFMapRestrict(m_pInfo);
	m_pRestrict->Init(this);

	// 根据地图信息是不是副本决定是否生成NavMap
	if( EMT_Normal == pInfo->eType )
	{
		m_pSingleMap = g_mapCreator.CreateAFMap(pInfo);
		if( !P_VALID(m_pSingleMap) )
			return FALSE;

		if( FALSE == m_pSingleMap->Init(pInfo) )
		{
			g_mapCreator.DestroyAFMap(m_pSingleMap);
			return FALSE;
		}
	}

	// 生成Update线程
	m_strThreadName = _T("Thread_");
	m_strThreadName += m_pInfo->szMapFileName;
	m_pThread->CreateThread(m_strThreadName.c_str(), (vEngine::THREADPROC)m_Trunk.sfp0(&MapMgr::ThreadUpdate), NULL);

	while( FALSE == m_pThread->IsThreadActive(m_strThreadName.c_str()) )
		continue;

	return TRUE;
}

//-------------------------------------------------------------------------------------------------------
// 线程更新函数
//-------------------------------------------------------------------------------------------------------
DWORD MapMgr::ThreadUpdate()
{
#ifdef _DEBUG
	_set_se_translator(vEngine::SEH_Translation);

	try
	{
#endif
		INT nWhichEvent = 0;

		while(TRUE)
		{
			g_mapCreator.GetAllMapStartEvent(nWhichEvent).Wait();

			if( m_bTerminate )
				break;

			// 普通地图
			if( EMT_Normal == m_pInfo->eType )
			{
				// 调用地图的更新
				m_pSingleMap->Update();
			}

			// 副本地图
			else
			{
				// 更新每一个副本
				m_mapInstance.ResetIterator();
				MapInstance* pInstance = NULL;
				while( m_mapInstance.PeekNext(pInstance) )
				{
					pInstance->Update();

					// 副本可以删除，删除副本
					if( pInstance->CanDestroy() )
					{
						g_mapCreator.InstanceDestroyed(m_pInfo->dwID, pInstance->GetInstanceID());
					}
				}
			}

			// 通知mapcreator已经执行完毕
			g_mapCreator.OneMapMgrEnd();

			nWhichEvent = ( (nWhichEvent == 0) ? 1 : 0 );
		}
#ifdef _DEBUG
	}
	catch(vEngine::Exception)
	{
#ifdef _DEBUG
		__asm int 3;
#endif

		// 设置ShutDown
		g_world.ShutDown();

		Sleep(3000);

		// 通知mapcreator已经执行完毕
		g_mapCreator.OneMapMgrEnd();
	}
#endif
	_endthreadex(0);

	return 0;
}

//-------------------------------------------------------------------------------------------------------
// 销毁
//-------------------------------------------------------------------------------------------------------
VOID MapMgr::Destroy()
{
	// 先删除地图
	if( EMT_Normal == m_pInfo->eType )
	{
		g_mapCreator.DestroyAFMap(m_pSingleMap);
	}
	else
	{
		m_mapInstance.ResetIterator();
		MapInstance* pInstance = NULL;

		while( m_mapInstance.PeekNext(pInstance) )
		{
			g_mapCreator.DestroyAFMap(pInstance);
		}
		m_mapInstance.Clear();
	}

	// 再删除策略
	g_mapCreator.DestroyAFMapRestrict(m_pRestrict);
}

//--------------------------------------------------------------------------------------------------------
// 传出副本
//--------------------------------------------------------------------------------------------------------
VOID MapMgr::RoleInstanceOut(Role *pRole)
{
	Vector3 vDestPos = pRole->GetCurPos();
	DWORD	dwDestMapID = GT_INVALID;

	// 得到出口地图和坐标
	if( FALSE == m_pRestrict->GetExportMapAndCoord(pRole, dwDestMapID, vDestPos) )
	{
		ILOG->Write(_T("Try to out of the instance failed, mapid=%d\n"), m_pInfo->dwID);
		return;
	}

	pRole->GotoNewMap(dwDestMapID, vDestPos.x, vDestPos.y, vDestPos.z);
}

//--------------------------------------------------------------------------------------------------------
// 创建副本
//--------------------------------------------------------------------------------------------------------
MapInstance* MapMgr::CreateInstance(Role* pCreator, DWORD dwMisc)
{
	if( !g_mapCreator.CanCreateInstance(m_pInfo, this) ) return NULL;

	MapInstance* pInstance = g_mapCreator.CreateAFMapInstance(m_pInfo);
	if( !P_VALID(pInstance) ) return NULL;

	g_mapCreator.AddInstance(m_pInfo, this);

	DWORD dwInstanceID = g_mapCreator.CreateNewInstanceID();

	if( FALSE == pInstance->Init(m_pInfo, dwInstanceID, pCreator, dwMisc) )
	{
		DestroyInstance(pInstance);
		return NULL;
	}

	m_mapInstance.Add(pInstance->GetInstanceID(), pInstance);

	return pInstance;
}

//--------------------------------------------------------------------------------------------------------
// 脚本创建副本
//--------------------------------------------------------------------------------------------------------
BOOL MapMgr::CreateScriptInstance(DWORD dwInstanceID)
{
	if( !g_mapCreator.CanCreateInstance(m_pInfo, this) ) return FALSE;

	if( EMT_ScriptCreate != m_pInfo->eType ) return FALSE;

	MapInstance* pInstance = g_mapCreator.CreateAFMapInstance(m_pInfo);
	if( !P_VALID(pInstance) ) return FALSE;

	g_mapCreator.AddInstance(m_pInfo, this);

	if( FALSE == pInstance->Init(m_pInfo, dwInstanceID, NULL, GT_INVALID) )
	{
		DestroyInstance(pInstance);
		return FALSE;
	}

	m_mapInstance.Add(pInstance->GetInstanceID(), pInstance);

	return TRUE;
}

//---------------------------------------------------------------------------------------------------------
// 删除副本
//---------------------------------------------------------------------------------------------------------
VOID MapMgr::DestroyInstance(MapInstance* pInstance)
{
	if( !P_VALID(pInstance) ) return;

	pInstance->OnDestroy();

	m_mapInstance.Erase(pInstance->GetInstanceID());
	g_mapCreator.RemoveInstance(m_pInfo, this);
	g_mapCreator.DestroyAFMap(pInstance);
}

//---------------------------------------------------------------------------------------------------------
// 是否能够进入
//---------------------------------------------------------------------------------------------------------
Map* MapMgr::CanEnter(Role* pRole, DWORD dwMisc/* =GT_INVALID */)
{
	return m_pRestrict->CanEnter(pRole, dwMisc);
}

//---------------------------------------------------------------------------------------------------------
// 上线后是否能够进入，否则返回出口地图及坐标
//---------------------------------------------------------------------------------------------------------
Map* MapMgr::CanTakeOverWhenOnline(Role* pRole, DWORD& dwOutMapID, Vector3& vOut)
{
	return m_pRestrict->CanTakeOverWhenOnline(pRole, dwOutMapID, vOut);
}

