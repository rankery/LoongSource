//----------------------------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: map_creator.h
// author: Aslan
// actor:
// data: 2008-6-27
// last:
// brief: 地图生成器，统筹管理所有的MapMgr，维护所有MapMgr的生存期，并提供地图静态属性，全局对象
//----------------------------------------------------------------------------------------------
#pragma once

#include "map.h"
#include "mutex.h"

struct tagMapInfo;
class Map;
class Team;
class MapInstance;
class MapMgr;
class MapRestrict;

//---------------------------------------------------------------------------------
// 切换地图的结构
//---------------------------------------------------------------------------------
struct tagChangeMapInfo
{
	DWORD		dwRoleID;			// 角色ID
	DWORD		dwDestMapID;		// 目标地图ID
	Vector3		vPos;				// 目标地图坐标
	DWORD		dwMisc;				// 如果针对于随机副本，则代表副本难度

	tagChangeMapInfo(DWORD)
	{
		dwRoleID = GT_INVALID;
	}
	tagChangeMapInfo()
	{
		dwRoleID = GT_INVALID;
	}
};

//-----------------------------------------------------------------------------------
// 副本删除结构
//-----------------------------------------------------------------------------------
struct tagInstanceDestroyInfo
{
	DWORD		dwMapID;			// 地图ID
	DWORD		dwInstanceID;		// 副本ID

	tagInstanceDestroyInfo(DWORD)
	{
		dwMapID = GT_INVALID;
	}
	tagInstanceDestroyInfo(DWORD _dwMapID, DWORD _dwInstanceID)
	{
		dwMapID = _dwMapID;
		dwInstanceID = _dwInstanceID;
	}
	tagInstanceDestroyInfo()
	{
		dwMapID = GT_INVALID;
	}
};

//-------------------------------------------------------------------------------------------------
// 地图管理器
//-------------------------------------------------------------------------------------------------
class MapCreator
{
public:
	typedef		TMap<DWORD, MapMgr*>		MapMgrMap;
	typedef		TMap<DWORD, NavMap*>		InstanceTemplateMap;
	typedef		TMap<DWORD, tagMapInfo*>	MapInfoMap;

public:
	MapCreator();
	~MapCreator();

	//---------------------------------------------------------------------------------------------
	// 初始化，更新及销毁
	//---------------------------------------------------------------------------------------------
	BOOL				Init();
	NavMap*				LoadInstanceTemplate(tagMapInfo* pInfo);
	VOID				Update();
	VOID				Destroy();

	//--------------------------------------------------------------------------------------------
	// 工厂相关
	//--------------------------------------------------------------------------------------------
	Map*				CreateAFMap(const tagMapInfo* pInfo);
	MapInstance*		CreateAFMapInstance(const tagMapInfo* pInfo);
	MapRestrict*		CreateAFMapRestrict(const tagMapInfo* pInfo);

	VOID				DestroyAFMap(Map* pMap);
	VOID				DestroyAFMapRestrict(MapRestrict* pMap);

	//--------------------------------------------------------------------------------------------
	// 线程管理
	//--------------------------------------------------------------------------------------------
	VOID				StartAllMapMgr();								// 本Tick开启所有地图线程运行
	VOID				WaitAllMapMgrEnd();								// 等待本Tick所有地图线程结束
	VOID				UpdateAllDelayedStuff();						// 每一Tick更新所有地图线程暂停后的操作
	VOID				StopAllMapMgr();								// 永久停止所有地图管理器线程
	Event&				GetAllMapStartEvent(INT n);						// 得到本Tick开启所有地图线程运行的事件
	VOID				OneMapMgrEnd();									// 一个地图线程本Tick执行完毕

	//--------------------------------------------------------------------------------------------
	// 各种Get
	//--------------------------------------------------------------------------------------------
	MapMgr*				GetMapMgr(DWORD dwMapID)			{ return m_mapMapMgr.Peek(dwMapID); }
	const tagMapInfo*	GetMapInfo(DWORD dwMapID)			{ return m_mapMapInfo.Peek(dwMapID); }
	DWORD				GetBornMapID()						{ return m_dwBornMapID; }
	NavMap*				GetInstanceTemplate(DWORD dwMapID)	{ return m_mapInstanceTemplate.Peek(dwMapID); }
	BOOL				IsMapExist(DWORD dwMapID)			{ return m_mapMapInfo.IsExist(dwMapID); }

	Vector3				RandGetOneBornPos();
	Map*				GetMap(DWORD dwMapID, DWORD dwInstance);
	DWORD				GetPrisonMapID()			{ return m_dwPrisonMapID; }
	const Vector3		GetPutInPrisonPoint()		{ return m_PutInPrisonPoint; }
	const Vector3		GetPutOutPrisonPoint()		{ return m_PutOutPrisonPoint; }
	const Vector3		GetReBornPoint(DWORD dwRebornMapID);
	
	INT					GetStableInstanceNum()		{ return m_nMaxStableInstanceNum; }

	//---------------------------------------------------------------------------------------------
	// 角色管理函数
	//---------------------------------------------------------------------------------------------
	VOID				RoleLogOut(DWORD dwRoleID)			{ m_listLogOutRole.PushBack(dwRoleID); }
	VOID				RoleReturnCharacter(DWORD dwRoleID)	{ m_listReturnCharacterRole.PushBack(dwRoleID); }
	VOID				RoleChangeMap(DWORD dwRoleID, DWORD dwDestMapID, Vector3& vPos, DWORD dwMisc);

	BOOL				TakeOverRoleWhenOnline(Role* pRole);

	//---------------------------------------------------------------------------------------------
	// 副本管理
	//---------------------------------------------------------------------------------------------
	DWORD				CreateNewInstanceID()	{ InterlockedExchangeAdd((LPLONG)&m_dwInstanceIDGen, 1); return m_dwInstanceIDGen; }
	VOID				InstanceDestroyed(DWORD dwMapID, DWORD dwInstanceID);

	VOID				SetInstanceDelete(DWORD dwMapID, DWORD dwInstanceID);

	BOOL				CanCreateInstance(const tagMapInfo* pInfo, MapMgr* pMapMgr);
	VOID				AddInstance(const tagMapInfo* pInfo, MapMgr* pMapMgr);
	VOID				RemoveInstance(const tagMapInfo* pInfo, MapMgr* pMapMgr);
	INT					GetInstanceNum(INT nIndex)		{ return m_nInstanceNumCur[nIndex]; }
	INT					GetInstanceCoefNum()			{ return m_nInstanceCoefNumCur; }
	INT					GetInstanceCoefNumLimit()		{ return m_nInstanceCoefNumLimit; }

private:
	//---------------------------------------------------------------------------------------------
	// 初始化
	//---------------------------------------------------------------------------------------------
	BOOL				LoadMapInfo(LPCTSTR szFileMapName, TObjRef<VarContainer>& pLogic);				// 载入地图的基本信息，载入成功时，会加入到m_mapMapInfo
	BOOL				LoadMapWayPoint(tagMapHeader* pHeader, DWORD handle, tagMapInfo* pInfo);		// 载入地图中的所有导航点
	BOOL				LoadMapTrigger(tagMapHeader* pHeader, DWORD handle, tagMapInfo* pInfo);			// 载入地图中的所有触发器
	BOOL				LoadMapArea(tagMapHeader* pHeader, DWORD handle, tagMapInfo* pInfo);			// 载入地图中的所有区域
	BOOL				LoadMapCreature(tagMapHeader* pHeader, DWORD handle, tagMapInfo* pInfo);		// 载入地图中的所有场景生物
	BOOL				LoadMapSpawnPoint(tagMapHeader* pHeader, DWORD handle, tagMapInfo* pInfo);		// 载入地图中的所有刷怪点
	BOOL				LoadMapTriggerEffect(tagMapHeader* pHeader, DWORD handle, tagMapInfo* pInfo);	// 载入地图中的所有特效
	BOOL				LoadMapDoor(tagMapHeader* pHeader, DWORD handle, tagMapInfo* pInfo);			// 载入地图中的所有门

	//----------------------------------------------------------------------------------------------
	// 工厂相关
	//----------------------------------------------------------------------------------------------
	VOID				RegisterAFMap();
	VOID				RegisterAFMapRestrict();
	VOID				UnRegisterAll();

	//----------------------------------------------------------------------------------------------
	// 线程管理
	//----------------------------------------------------------------------------------------------
	MapMgr*				CreateMapMgr(tagMapInfo* pInfo);

	//----------------------------------------------------------------------------------------------
	// 各种清理函数
	//----------------------------------------------------------------------------------------------
	VOID				LogoutAllRemovedRolePerTick();
	VOID				DealAllChangeMapRole();
	VOID				DealAllReturnCharacterRole();
	VOID				DealAllDestroiedInstance();

private:
	TObjRef<Util>						m_pUtil;
	TObjRef<Thread>						m_pThread;

	TAutoFactory<MapRestrict>			m_AFRestrict;					// 地图策略类工厂（地图类型——策略派生类）
	TAutoFactory<Map>					m_AFMap;						// 地图类工厂（地图类型——地图派生类）

	MapMgrMap							m_mapMapMgr;					// 所有的MapMgr列表（这个是否要加锁？）
	InstanceTemplateMap					m_mapInstanceTemplate;			// 预先生成所有副本的NavMap，生成副本时直接拷贝
	MapInfoMap							m_mapMapInfo;					// 所有地图的逻辑属性信息
	volatile DWORD						m_dwInstanceIDGen;				// 副本ID生成器

	INT									m_nInstanceNumCur[EMIR_End];	// 不同规格副本的当前创建数量
	INT									m_nInstanceCoefNumCur;			// 当前所有副本的加权总和
	INT									m_nInstanceCoefNumLimit;		// 所有副本的加权总和上限

	DWORD								m_dwBornMapID;					// 玩家出生地图的ID
	INT									m_nBornPosNum;					// 出生点个数
	Vector3*							m_pvBornPos;					// 出生点坐标数组

	Event								m_MapStartEvent[2];				// 事件——开启所有地图线程执行（两个事件，所有地图线程的Update线程交替监听这两个事件）
	INT									m_nWhichEvent;					// 当前使用的是哪个事件
	Event								m_MapEndEvent;					// 事件——所有地图Update完毕
	volatile INT						m_nNumMapMgr;					// MapMgr的数量，每个Tick重置为MapMgr的总数量，每个MapMgr运行完一次后减少该值

	TSafeList<DWORD>					m_listLogOutRole;				// 每个Tick登出的玩家
	TSafeList<tagChangeMapInfo>			m_listChangeMapRole;			// 每个Tick要切换地图的玩家
	TSafeList<DWORD>					m_listReturnCharacterRole;		// 每个Tick要回到选人界面的玩家
	
	TSafeList<tagInstanceDestroyInfo>	m_listDestroyInstance;			// 每个Tick要删除的副本

	// 牢狱信息
	DWORD								m_dwPrisonMapID;				// 牢狱地图id
	Vector3								m_PutInPrisonPoint;				// 进牢狱区域point
	Vector3								m_PutOutPrisonPoint;			// 出牢狱区域point

	// 新手村
	INT									m_nMaxStableInstanceNum;		// 新手村副本数量
};

//--------------------------------------------------------------------------------------------
// 开启所有地图线程
//--------------------------------------------------------------------------------------------
inline VOID MapCreator::StartAllMapMgr()
{
	m_nWhichEvent = ((m_nWhichEvent == 0) ? 1 : 0);
	InterlockedExchange((LPLONG)&m_nNumMapMgr, m_mapMapMgr.Size());
	m_MapStartEvent[m_nWhichEvent].Set();
}

//--------------------------------------------------------------------------------------------
// 等待所有地图线程本tick完毕
//--------------------------------------------------------------------------------------------
inline VOID	MapCreator::WaitAllMapMgrEnd()
{
	m_MapEndEvent.Wait();
	m_MapEndEvent.ReSet();
	m_MapStartEvent[m_nWhichEvent].ReSet();
}

//---------------------------------------------------------------------------------------------
// 随机得到一个出生坐标点
//---------------------------------------------------------------------------------------------
inline Vector3 MapCreator::RandGetOneBornPos()
{
	INT nRand = m_pUtil->Rand() % m_nBornPosNum;
	return m_pvBornPos[nRand];
}

//---------------------------------------------------------------------------------------------
// 得到启动地图线程运行的事件
//---------------------------------------------------------------------------------------------
inline Event& MapCreator::GetAllMapStartEvent(INT n)
{
	return m_MapStartEvent[n];
}

//---------------------------------------------------------------------------------------------
// 某个地图线程本Tick执行完毕
//---------------------------------------------------------------------------------------------
inline VOID MapCreator::OneMapMgrEnd()
{
	// 当所有地图线程执行完毕后，设置事件通知
	if( 0 == InterlockedDecrement((LPLONG)&m_nNumMapMgr ) ) 
	{
		m_MapEndEvent.Set();
	}
}

//----------------------------------------------------------------------------------------------
// 玩家切换地图
//----------------------------------------------------------------------------------------------
inline VOID MapCreator::RoleChangeMap(DWORD dwRoleID, DWORD dwDestMapID, Vector3& vPos, DWORD dwMisc)
{
	MapMgr* pMapMgr = GetMapMgr(dwDestMapID);
	if( !P_VALID(pMapMgr) ) return;

	tagChangeMapInfo info;
	info.dwRoleID		=	dwRoleID;
	info.dwDestMapID	=	dwDestMapID;
	info.vPos			=	vPos;
	info.dwMisc			=	dwMisc;

	m_listChangeMapRole.PushBack(info);
}

//------------------------------------------------------------------------------------------------
// 副本请求删除
//------------------------------------------------------------------------------------------------
inline VOID MapCreator::InstanceDestroyed(DWORD dwMapID, DWORD dwInstanceID)
{
	MapMgr* pMapMgr = GetMapMgr(dwMapID);
	if( !P_VALID(pMapMgr) ) return;

	tagInstanceDestroyInfo info(dwMapID, dwInstanceID);

	m_listDestroyInstance.PushBack(info);
}

extern MapCreator g_mapCreator;

