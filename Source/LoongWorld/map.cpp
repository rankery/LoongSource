//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: map.h
// author: aslan
// actor:
// data: 2008-6-27
// last:
// brief: 地图类，管理一个确定的地图实例，副本为一个独立的地图实例
//-----------------------------------------------------------------------------
#include "StdAfx.h"

#include "../WorldDefine/action.h"
#include "../WorldDefine/remote_role.h"
#include "../WorldDefine/msg_map.h"
#include "../WorldDefine/msg_loot.h"
#include "../WorldDefine/script_data.h"

#include "../ServerDefine/sspawnpoint_define.h"
#include "../ServerDefine/NPCTeam_define.h"

#include "player_session.h"
#include "world_session.h"
#include "login_session.h"
#include "map_creator.h"
#include "unit.h"
#include "role.h"
#include "creature.h"
#include "pet.h"
#include "role_mgr.h"
#include "map.h"
#include "att_res.h"
#include "shop.h"
#include "team.h"
#include "group_mgr.h"
#include "move_data.h"
#include "script_mgr.h"
#include "gm_policy.h"
#include "NPCTeam_mgr.h"
#include "NPCTeam.h"
#include "guild_CofC.h"
#include "TimeChecker.h"

//------------------------------------------------------------------------------
// 掉落物品的大小
//------------------------------------------------------------------------------
static Vector3 vGroundItemSize(50.0f, 50.0f, 50.0f);


//------------------------------------------------------------------------------
// construct
//------------------------------------------------------------------------------
Map::Map() : m_dwID(GT_INVALID), m_dwInstanceID(GT_INVALID), m_pInfo(NULL),
m_pVisTile(NULL), m_pNav(NULL), ScriptData<ESD_Map>()
{
}

//------------------------------------------------------------------------------
// destruct
//------------------------------------------------------------------------------
Map::~Map()
{
	Destroy();
}

//-------------------------------------------------------------------------------
// 初始化地图
//-------------------------------------------------------------------------------
BOOL Map::Init(const tagMapInfo* pInfo)
{
	ASSERT( P_VALID(pInfo) );
	ASSERT( EMT_Normal == pInfo->eType );

	m_pUtil = "Util";

	m_pInfo = pInfo;
	m_dwID = m_pInfo->dwID;

	m_mapSession.Clear();
	m_mapRole.Clear();
	m_mapShop.Clear();
	m_mapCofC.Clear();
	m_mapGroundItem.Clear();

	m_listSceneEffect.Clear();

	// 载入导航图数据
	IFS* pVfs = Kernel::Inst()->DefaultFS();
	m_pNav = new NavMap;
	if( !P_VALID(m_pNav) )
	{
		return FALSE;
	}

	m_pTeamMgr = new NPCTeamMgr(this);
	if(!P_VALID(m_pTeamMgr))
		return FALSE;

	m_pNav->LoadFromFile(pInfo->szMapFileName, pVfs, pVfs, NavMap::ELO_Collider | NavMap::ELO_NPCNavMap | NavMap::ELO_NPCNavGraph | NavMap::ELO_NPCNavMapDoors);
	//NavResMgr::Inst()->Report();
	Vector3 zoneSize(FLOAT_MAX, FLOAT_MAX, FLOAT_MAX);
	m_pNav->SetViewZoneSize(zoneSize);
	Vector3 zoneCenter(0, 0, 0);
	m_pNav->InvalidViewZone(zoneCenter);

	// 根据mapinfo来初始化地图的怪物，可视列表等信息
	if( FALSE == InitLogicalInfo() )
	{
		SAFE_DEL(m_pNav);
		return FALSE;
	}

	// 地图初始化脚本
	OnInit();

	return TRUE;
}

//---------------------------------------------------------------------------------
// 当初始化完成时
//---------------------------------------------------------------------------------
VOID Map::OnInit()
{
	if( P_VALID(m_pScript) )
		m_pScript->OnInit(this);
}

//---------------------------------------------------------------------------------
// 销毁
//---------------------------------------------------------------------------------
VOID Map::Destroy()
{
	// 删除地图里的怪物
	Creature* pCreature = NULL;

	TMap<DWORD, Creature*>::TMapIterator itCreature = m_mapCreature.Begin();
	while( m_mapCreature.PeekNext(itCreature, pCreature) )
	{
		Creature::Delete(pCreature);
	}
	m_mapCreature.Clear();

	TMap<DWORD, Creature*>::TMapIterator itResCreature = m_mapRespawnCreature.Begin();
	while( m_mapRespawnCreature.PeekNext(itResCreature, pCreature) )
	{
		Creature::Delete(pCreature);
	}
	m_mapRespawnCreature.Clear();


	// 删除地面物品集合
	m_mapGroundItem.ResetIterator();
	tagGroundItem* pGroundItem = NULL;
	while( m_mapGroundItem.PeekNext(pGroundItem) )
	{
		pGroundItem->DestroyItem();
		SAFE_DEL(pGroundItem);
	}

	// 删除商店
	Shop* pShop = NULL;
	TMap<DWORD, Shop*>::TMapIterator itShop = m_mapShop.Begin();
	while( m_mapShop.PeekNext(itShop, pShop) )
	{
		Shop::Delete(pShop);
	}
	m_mapShop.Clear();

	// 删除商会
	GuildCofC* pGuildCofC = NULL;
	CofCMap::TMapIterator itCofC = m_mapCofC.Begin();
	while (m_mapCofC.PeekNext(itCofC, pGuildCofC))
	{
		GuildCofC::Delete(pGuildCofC);
	}
	m_mapCofC.Clear();

	// 关闭九宫格和导航图
	SAFE_DEL_ARRAY(m_pVisTile);
	SAFE_DEL(m_pNav);

	// 删除怪物小队管理器
	SAFE_DEL(m_pTeamMgr);

	// 清空脚本
	m_pScript = NULL;
}

//---------------------------------------------------------------------------------
// 生成地图的逻辑属性，如可视链表，导航点，触发器，NPC怪物等等
//---------------------------------------------------------------------------------
BOOL Map::InitLogicalInfo()
{
	// 生成VISTILE
	m_nVisTileArrayWidth = (m_pInfo->nWidth + m_pInfo->nVisDist - 1) / m_pInfo->nVisDist + 2;
	m_nVisTileArrayHeight = (m_pInfo->nHeight + m_pInfo->nVisDist - 1) / m_pInfo->nVisDist + 2;

	m_pVisTile = new tagVisTile[m_nVisTileArrayWidth * m_nVisTileArrayHeight];
	if( !P_VALID(m_pVisTile) )
	{
		return FALSE;
	}

	// 生成地图脚本
	m_pScript = g_ScriptMgr.GetMapScript(m_dwID);

	// 加载地图怪物
	if( FALSE == InitAllMapCreature() )
	{
		SAFE_DEL_ARRAY(m_pVisTile);
		return FALSE;
	}

	return TRUE;
}

//-----------------------------------------------------------------------------------
// 初始化所以地图中的怪物，加载时为单线程
//-----------------------------------------------------------------------------------
BOOL Map::InitAllMapCreature()
{
	// 初始化怪物ID生成器
	m_CreatureIDGen.Init(m_pInfo);

	return InitAllFixedCreature() && InitAllSpawnPointCreature() && InitAllFixedDoor();
}

//-----------------------------------------------------------------------------------
// 初始化所有的地图内摆放的怪物
//-----------------------------------------------------------------------------------
BOOL Map::InitAllFixedCreature()
{
	// 一个一个的创建怪物
	tagMapCreatureInfo* pCreatureInfo = NULL;
	const tagCreatureProto* pProto = NULL;
	m_pInfo->mapCreatureInfo.ResetIterator();
	while( m_pInfo->mapCreatureInfo.PeekNext(pCreatureInfo) )
	{
		pProto = g_attRes.GetCreatureProto(pCreatureInfo->dwTypeID);
		if( !P_VALID(pProto) )
		{
			IMSG(_T("Detect a unknown creature in map, dwObjID=%u\r\n"), pCreatureInfo->dwTypeID);
			continue;
		}

		// 取出一个ID
		DWORD dwID = m_CreatureIDGen.GetNewCreatureID();
		ASSERT( IS_CREATURE(dwID) );

		// 生成出生坐标和出生朝向
		Vector3 vFaceTo;
		vFaceTo.x = cosf(pCreatureInfo->fYaw * 3.1415927f / 180.0f);
		vFaceTo.z = sinf(pCreatureInfo->fYaw * 3.1415927f / 180.0f);
		vFaceTo.y = 0.0f;

		// 生成怪物
		Creature* pCreature = Creature::Create(dwID, m_dwID, pProto, pCreatureInfo->vPos, vFaceTo, 
			ECST_Load, GT_INVALID, pCreatureInfo->bCollide, pCreatureInfo->patrolList);

		// 加入到地图中
		AddCreatureOnLoad(pCreature);

		// 如果是巢穴，则加载巢穴怪物
		if( pCreature->IsNest() )
		{
			InitNestCreature(pCreature);
		}

		// 如果是怪物小队，则加载小队怪物
		if( pCreature->IsTeam() )
		{
			InitTeamCreature(pCreature);
		}
	}

	return TRUE;
}

//-----------------------------------------------------------------------------------
// 初始化普通地图刷怪点的怪物，加载时为单线程
//-----------------------------------------------------------------------------------
BOOL Map::InitAllSpawnPointCreature()
{
	// 对每一个刷怪点	
	tagMapSpawnPointInfo* pMapSpawnInfo = NULL;
	m_pInfo->mapSpawnPoint.ResetIterator();
	while(m_pInfo->mapSpawnPoint.PeekNext(pMapSpawnInfo))
	{
		ASSERT_P_VALID(pMapSpawnInfo);
		if (!P_VALID(pMapSpawnInfo)) continue;

		// 随机选择一个怪
		const tagSSpawnPointProto* pSSpawnProto = g_attRes.GetSSpawnPointProto(pMapSpawnInfo->dwSpawnPointID);
		ASSERT_P_VALID(pSSpawnProto);
		if (!P_VALID(pSSpawnProto)) continue;

		INT nCandiNum	= 0;
		while (GT_VALID(pSSpawnProto->dwTypeIDs[nCandiNum]))
			nCandiNum++;
		if (0 == nCandiNum) continue;

		INT nIndex = IUTIL->Rand() % nCandiNum;
		const tagCreatureProto* pCreatureProto = g_attRes.GetCreatureProto(pSSpawnProto->dwTypeIDs[nIndex]);
		ASSERT_P_VALID(pCreatureProto);
		if (!P_VALID(pCreatureProto)) continue;

		// 获取id
		DWORD dwCreatureID = m_CreatureIDGen.GetNewCreatureID();

		// 生成出生坐标和出生朝向
		FLOAT fYaw = FLOAT(IUTIL->Rand() % 360);
		Vector3 vFaceTo;
		vFaceTo.x = cosf(fYaw * 3.1415927f / 180.0f);
		vFaceTo.z = sinf(fYaw * 3.1415927f / 180.0f);
		vFaceTo.y = 0.0f;

		// 创建怪
		Creature* pCreature = Creature::Create(	dwCreatureID, GetMapID(), pCreatureProto, pMapSpawnInfo->vPos, vFaceTo, 
			ECST_SpawnPoint, pMapSpawnInfo->dwSpawnPointID, pMapSpawnInfo->bCollide, NULL);
		ASSERT_P_VALID(pCreature);
		if (!P_VALID(pCreature)) continue;

		// 放入地图
		AddCreatureOnLoad(pCreature);
	}

	return TRUE;
}

//-----------------------------------------------------------------------------------
// 初始化地图中所有门，加载时为单线程
//-----------------------------------------------------------------------------------
BOOL Map::InitAllFixedDoor()
{
	tagMapDoor* pMapDoor = NULL;
	const tagCreatureProto* pProto = NULL;
	m_pInfo->mapDoor.ResetIterator();
	while( m_pInfo->mapDoor.PeekNext(pMapDoor) )
	{
		pProto = g_attRes.GetCreatureProto(pMapDoor->dwTypeID);
		if( !P_VALID(pProto) )
		{
			IMSG(_T("Detect a unknown creature in map, dwObjID=%u\r\n"), pMapDoor->dwObjID);
			continue;
		}

		// 取出一个ID
		DWORD dwID = m_DoorIDGen.GetValidDoorID();
		ASSERT( IS_DOOR(dwID) );

		// 生成出生坐标和出生朝向
		Vector3 vFaceTo;
		vFaceTo.x = cosf(pMapDoor->fYaw * 3.1415927f / 180.0f);
		vFaceTo.z = sinf(pMapDoor->fYaw * 3.1415927f / 180.0f);
		vFaceTo.y = 0.0f;

		// 生成怪物
		Creature* pCreature = Creature::Create(dwID, m_dwID, pProto, pMapDoor->vPos, vFaceTo, 
			ECST_Load, GT_INVALID, TRUE, NULL, GT_INVALID, pMapDoor->dwObjID);

		if (!P_VALID(pCreature))
			continue;

		if (!pCreature->IsDoor())
		{
			Creature::Delete(pCreature);
			continue;
		}

		pCreature->SetDoorStatus(pMapDoor->bInitState, FALSE);

		// 加入到地图中
		AddCreatureOnLoad(pCreature);

		// 加入映射表
		m_mapDoorObj2Gen.Add(pMapDoor->dwObjID, pCreature->GetID());
	}

	return TRUE;
}

//-----------------------------------------------------------------------------------
// 初始化巢穴怪物
//-----------------------------------------------------------------------------------
VOID Map::InitNestCreature(Creature* pCreature)
{
	ASSERT( P_VALID(pCreature) && pCreature->IsNest() );

	const tagCreatureProto* pProto = pCreature->GetProto();
	ASSERT( P_VALID(pProto) && P_VALID(pProto->pNest) );

	const tagNestProto* pNest = pProto->pNest;
	const Vector3& vSpawnCenter = pCreature->GetCurPos();

	Vector3 vPos;
	Vector3 vFaceTo;

	for(INT n = 0; n < pNest->nCreatureNum; n++)
	{
		DWORD dwCreatureTypeID = pNest->dwSpawnID[n];
		INT nNum = pNest->nSpawnMax[n];

		const tagCreatureProto* pSpawnedCreatureProto = g_attRes.GetCreatureProto(dwCreatureTypeID);

		ASSERT( P_VALID(pSpawnedCreatureProto) && nNum > 0 );

		for(INT m = 0; m < nNum; m++)
		{
			INT nNum = 0;
			// 找到一个可行走的随机点
			while(TRUE)
			{
				vPos.x = FLOAT(IUTIL->Rand() % (pNest->nSpawnRadius * 2) - pNest->nSpawnRadius) + vSpawnCenter.x;
				vPos.z = FLOAT(IUTIL->Rand() % (pNest->nSpawnRadius * 2) - pNest->nSpawnRadius) + vSpawnCenter.z;

				if(pCreature->NeedCollide())
				{
					vPos.y = pCreature->GetCurPosTop().y;
					break;
				}
				else
				{
					if( IfCanGo(vPos.x, vPos.z) )
					{
						vPos.y = GetGroundHeight(vPos.x, vPos.z);
						break;
					}
				}

				++nNum;
				if(nNum > 100)
				{
					break;
				}
			}
			if(nNum > 20)
				continue;

			// 随机一个朝向
			FLOAT fYaw = FLOAT(IUTIL->Rand() % 360);
			vFaceTo.x = cosf(fYaw * 3.1415927f / 180.0f);
			vFaceTo.z = sinf(fYaw * 3.1415927f / 180.0f);
			vFaceTo.y = 0.0f;

			// 取出一个ID
			DWORD dwID = m_CreatureIDGen.GetNewCreatureID();
			ASSERT( IS_CREATURE(dwID) );

			// 生成怪物
			Creature* pSpawnedCreature = Creature::Create(dwID, m_dwID, pSpawnedCreatureProto, 
				vPos, vFaceTo, ECST_Nest, pCreature->GetID(), pCreature->NeedCollide(), NULL);

			// 加入到地图中
			AddCreatureOnLoad(pSpawnedCreature);
		}
	}
}


//-----------------------------------------------------------------------------------
// 初始化小队怪物
//-----------------------------------------------------------------------------------
VOID Map::InitTeamCreature(Creature* pCreature)
{
	ASSERT( P_VALID(pCreature) && pCreature->IsTeam() );

	const tagCreatureProto* pProto = pCreature->GetProto();
	ASSERT( P_VALID(pProto) && P_VALID(pProto->pNest) );

	const tagNestProto* pNest = pProto->pNest;
	const Vector3& vSpawnCenter = pCreature->GetCurPos();

	Vector3 vPos;
	Vector3 vFaceTo = pCreature->GetFaceTo();
	INT		nIndex = 0;
	INT		nNum = 0;
	INT		nNumIndex = 0;

	// 得到怪物小队的队形
	const tagNPCTeamOrder* pTeamOrder = m_pTeamMgr->GetNPCTeamOrder(pNest->eOrderType);
	ASSERT(P_VALID(pTeamOrder));

	// 创建怪物小队
	NPCTeam* pTeam = m_pTeamMgr->CreateTeam(pCreature);
	if(!P_VALID(pTeam))		return;

	// 设置队长小队ＩＤ
	pCreature->SetTeamID(pTeam->GetID());
	std::vector<POINT>::const_iterator it = pTeamOrder->NPCOrder.begin();
	while (it != pTeamOrder->NPCOrder.end())
	{

		POINT point = *it;
		CVector2D vector((FLOAT)point.x, (FLOAT)point.y);
		DWORD dwCreatureTypeID = pNest->dwSpawnID[nIndex];
		nNum = pNest->nSpawnMax[nIndex];

		const tagCreatureProto* pTeamMemProto = g_attRes.GetCreatureProto(dwCreatureTypeID);
		if(!P_VALID(pTeamMemProto))
			return;

		// 取出一个ID
		DWORD dwID = m_CreatureIDGen.GetNewCreatureID();
		ASSERT( IS_CREATURE(dwID) );

		// 计算怪物位置
		vPos = m_pTeamMgr->CalTeamMemPos(pCreature, point, vFaceTo, pNest);

		// 生成怪物
		Creature* pSpawnedCreature = Creature::Create(dwID, m_dwID, pTeamMemProto , 
			vPos, vFaceTo, ECST_Team, pCreature->GetID(), pCreature->NeedCollide(), NULL, pTeam->GetID());

		// 加入到地图中
		AddCreatureOnLoad(pSpawnedCreature);

		// 加入到怪物小队
		pTeam->AddNPCTeamMem(pSpawnedCreature);

		++nNumIndex;
		if(nNumIndex >= nNum)
		{
			++nIndex;
			nNumIndex = 0;
		}

		++it;
	}
}

//-----------------------------------------------------------------------------------
// 更新，被管理该地图的MapMgr的线程函数调用
//-----------------------------------------------------------------------------------
VOID Map::Update()
{
	CHECK_TIME_MAP(m_pInfo->szMapFileName);

	UpdateSession();
	UpdateAllObjects();
	UpdateAllShops();
	UpdateAllCofC();
}

//-------------------------------------------------------------------------------------
// 更新地图管理的所有玩家的消息
//-------------------------------------------------------------------------------------
VOID Map::UpdateSession()
{
	CHECK_TIME();
	PlayerSession* pSession = NULL;
	m_mapSession.ResetIterator();

	while( m_mapSession.PeekNext(pSession) )
	{
		pSession->Update();
	}
}

//--------------------------------------------------------------------------------------
// 更新地图管理的所有游戏对象的状态
//--------------------------------------------------------------------------------------
VOID Map::UpdateAllObjects()
{
	CHECK_TIME();
	// 更新所有地图里的玩家
	RoleMap::TMapIterator itRole = m_mapRole.Begin();
	Role* pRole = NULL;

	while( m_mapRole.PeekNext(itRole, pRole) )
	{
		pRole->Update();
	}

	// 更新地图里的所有怪物
	CreatureMap::TMapIterator itCreature = m_mapCreature.Begin();
	Creature* pCreature = NULL;

	while( m_mapCreature.PeekNext(itCreature, pCreature) )
	{
		pCreature->Update();
	}

	// 更新地图里面所有待刷新的怪物
	CreatureMap::TMapIterator itDeadCreature = m_mapRespawnCreature.Begin();
	Creature* pDeadCreature = NULL;

	while( m_mapRespawnCreature.PeekNext(itDeadCreature, pDeadCreature) )
	{
		ECreatureReviveResult eRet = pDeadCreature->TryRevive();

		if( ECRR_Success == eRet )
		{
			// 重生成功
			m_mapRespawnCreature.Erase(pDeadCreature->GetID());
			AddCreature(pDeadCreature);
		}
		else if( ECRR_Failed == eRet )
		{
			// 重生失败
		}
		else if( ECRR_NeedDestroy == eRet )
		{
			// 动态生成的，需要删除了
			m_mapRespawnCreature.Erase(pDeadCreature->GetID());
			Creature::Delete(pDeadCreature);
		}
		else if( ECRR_NeedReplace == eRet )
		{
			// 刷怪点生成的，需要替换
			SSpawnPtReplaceCreature(pDeadCreature);
		}
		else
		{
			ASSERT(0);
		}
	}

	// 更新地面物品
	TMap<INT64, tagGroundItem*>::TMapIterator it = m_mapGroundItem.Begin();
	tagGroundItem* pGroundItem = NULL;

	while( m_mapGroundItem.PeekNext(it, pGroundItem) )
	{
		switch(pGroundItem->Update())
		{
			// 移除地物
		case EGIU_Remove:
			{
				// 移除地物
				RemoveGroundItem(pGroundItem);

				// 销毁物品
				pGroundItem->DestroyItem();

				// 释放内存
				SAFE_DEL(pGroundItem);
			}
			break;
			// 同步地物状态
		case EGIU_Sync:
			{
				// 计算物品所在的可视地砖格子
				INT nVisIndex = WorldPosToVisIndex(pGroundItem->vPos);

				// 得到九宫格
				tagVisTile* pVisTile[ED_End] = {0};
				GetVisTile(nVisIndex, pVisTile);

				// 同步给加入到客户端的玩家和生物
				SyncGroundItemState(pGroundItem, pVisTile);
			}
			break;
		case EGIU_Null:
			break;
		default:
			ASSERT(0);
			break;
		}
	}
}

//---------------------------------------------------------------------------------------
// 更新该地图内所有商店
//---------------------------------------------------------------------------------------
VOID Map::UpdateAllShops()
{
	CHECK_TIME();
	Shop *pShop = NULL;
	m_mapShop.ResetIterator();

	while( m_mapShop.PeekNext(pShop) )
	{
		pShop->Update();
	}
}

//---------------------------------------------------------------------------------------
// 更新该地图内所有商会
//---------------------------------------------------------------------------------------
VOID Map::UpdateAllCofC()
{
	CHECK_TIME();
	GuildCofC *pCofC = NULL;
	m_mapCofC.ResetIterator();

	while( m_mapCofC.PeekNext(pCofC) )
	{
		pCofC->Update();
	}
}

//---------------------------------------------------------------------------------------
// 正式加入一个玩家，这只能由管理该地图的MapMgr调用
//---------------------------------------------------------------------------------------
VOID Map::AddRole(Role* pRole)
{
	ASSERT( P_VALID(pRole) );

	// 加入到玩家列表中
	m_mapRole.Add(pRole->GetID(), pRole);

	// 将该玩家的session加入到session列表中
	PlayerSession* pSession = pRole->GetSession();
	if( P_VALID(pSession) )
	{
		m_mapSession.Add(pSession->GetSessionID(), pSession);
	}

	// 设置玩家的地图
	pRole->SetMap(this);

	// 修正坐标
	FixPos(pRole->GetCurPos());

	// 修正高度
	pRole->GetMoveData().DropDownStandPoint();

	// 发送进入地图信息给客户端
	SendGotoNewMapToPlayer(pRole);

	// 发送该地图中场景特效
	SendSceneEffect(pRole);

	// 计算玩家所在的可视地砖格子
	INT nVisIndex = WorldPosToVisIndex(pRole->GetCurPos());

	// 得到九宫格
	tagVisTile* pVisTile[ED_End] = {0};
	GetVisTile(nVisIndex, pVisTile);

	// 同步给加入到客户端的玩家和生物
	SyncAddedUnits(pRole, pVisTile);

	// 让玩家落到9宫格之内
	AddToVisTile(pRole, nVisIndex);

	// 计算地图区域
	pRole->CheckMapArea();

	// 进入地图的相关事项
	pRole->OnEnterMap();

	// 调用脚本
	if( P_VALID(m_pScript) )
	{
		m_pScript->OnPlayerEnter(pRole, this);
	}
}

//----------------------------------------------------------------------------------------
// 得到碰撞检测器
//----------------------------------------------------------------------------------------
NavCollider* Map::GetNavCollider()
{
	return GetNavMap()->GetCollider();
}

//----------------------------------------------------------------------------------------
// 打开/关闭指定的门
//----------------------------------------------------------------------------------------
VOID Map::OpenCloseDoor(DWORD dwObjID, BOOL bOpen)
{
	DWORD dwDoorID = m_mapDoorObj2Gen.Peek(dwObjID);
	if (!IS_DOOR(dwDoorID))	return;
	
	Creature* pDoor = m_mapCreature.Peek(dwDoorID);
	if (!P_VALID(pDoor))	return;
	
	pDoor->SetDoorStatus(bOpen);

	bool boolOpen = bOpen ? true : false;
	GetNavMap()->GetCollider()->EnableDoorCollide(dwObjID, !boolOpen);
	GetNavMap()->GetNPCNavMap()->OpenCloseDoor(dwObjID, boolOpen);
}

//----------------------------------------------------------------------------------------
// 打开/关闭所有的门
//----------------------------------------------------------------------------------------
VOID Map::OpenCloseAllDoors(BOOL bOpen)
{
	DWORD dwDoorID = GT_INVALID;
	TMap<DWORD, DWORD>::TMapIterator iter = m_mapDoorObj2Gen.Begin();
	while (m_mapDoorObj2Gen.PeekNext(iter, dwDoorID))
	{
		if (!IS_DOOR(dwDoorID))
			continue;

		Creature* pDoor = m_mapCreature.Peek(dwDoorID);
		if (!P_VALID(pDoor))
			continue;

		pDoor->SetDoorStatus(bOpen);
	}

	bool boolOpen = bOpen ? true : false;
	GetNavMap()->GetCollider()->EnableAllDoorsCollide(!boolOpen);
	GetNavMap()->GetNPCNavMap()->OpenCloseAllDoors(boolOpen);
}

//-----------------------------------------------------------------------------------------
// 地图内加入一个生物，这是在载入时添加的，所以还不需要同步
//-----------------------------------------------------------------------------------------
VOID Map::AddCreatureOnLoad(Creature* pCreature)
{
	ASSERT( P_VALID(pCreature) );

	// 加入到生物列表中
	m_mapCreature.Add(pCreature->GetID(), pCreature);

	// 设置生物的地图
	pCreature->SetMap(this);

	// 计算生物所在的可视地砖格子
	INT nVisIndex = WorldPosToVisIndex(pCreature->GetCurPos());

	// 让生物落到九宫格之内
	AddToVisTile(pCreature, nVisIndex);

	// 如果是商店职能NPC，则挂载对应的商店
	if (ECT_NPC == pCreature->GetProto()->eType)
	{
		switch (pCreature->GetProto()->eFunctionType)
		{
		case EFNPCT_Shop:
			AddShop(pCreature->GetTypeID(), pCreature->GetShopID());
			break;

		case EFNPCT_CofC:
			AddCofC(pCreature->GetTypeID(), pCreature->GetShopID());
			break;
		}
	}
}

//-----------------------------------------------------------------------------------------
// 地图中加入一个生物，这是在游戏运行时添加，所以需要同步
//-----------------------------------------------------------------------------------------
VOID Map::AddCreature(Creature* pCreature)
{
	ASSERT( P_VALID(pCreature) );

	// 加入到生物列表中
	m_mapCreature.Add(pCreature->GetID(), pCreature);

	// 设置生物的地图
	pCreature->SetMap(this);

	// 计算生物所在的可视地砖格子
	INT nVisIndex = WorldPosToVisIndex(pCreature->GetCurPos());

	// 得到九宫格
	tagVisTile* pVisTile[ED_End] = {0};
	GetVisTile(nVisIndex, pVisTile);

	// 同步给加入到客户端的玩家和生物
	SyncAddedUnits(pCreature, pVisTile);

	// 让生物落到9宫格之内
	AddToVisTile(pCreature, nVisIndex);
}

//-----------------------------------------------------------------------------------------
// 从地图中拿走一个生物，放入死亡列表
//-----------------------------------------------------------------------------------------
VOID Map::RemoveCreature(Creature* pCreature)
{
	ASSERT( P_VALID(pCreature) );

	// 从生物列表中拿走这个怪物
	m_mapCreature.Erase(pCreature->GetID());

	// 检查生物所在的vistile
	INT nVisIndex = pCreature->GetVisTileIndex();

	// 得到九宫格
	tagVisTile* pVisTile[ED_End] = {0};
	GetVisTile(nVisIndex, pVisTile);

	// 从可视地砖中拿走，但不同步客户端
	RemoveFromVisTile(pCreature);

	// 放入到复活列表中等待复活
	m_mapRespawnCreature.Add(pCreature->GetID(), pCreature);
}

//-----------------------------------------------------------------------------------------
// 从地图中移除宠物
//-----------------------------------------------------------------------------------------
VOID Map::RemovePet(Pet* pPet)
{
	if (!P_VALID(pPet))
	{
		ASSERT(0);
		return;
	}

	// 从生物列表中拿走这个怪物
	m_mapCreature.Erase(pPet->GetID());

	pPet->SetMap(NULL);

	// 从可视地砖中拿走，但不同步客户端
	RemoveFromVisTile(pPet);

	// 同步给客户端
	SyncRemoveUnitToBigVisTile(pPet);
}


//-----------------------------------------------------------------------------------------
// 加入到地图中的某个VisTile
//-----------------------------------------------------------------------------------------
VOID Map::AddToVisTile(Unit* pUnit, INT nVisIndex)
{
	ASSERT( P_VALID(pUnit) );
	ASSERT( nVisIndex != GT_INVALID );

	pUnit->SetVisTileIndex(nVisIndex);
	if( pUnit->IsRole() )
		m_pVisTile[nVisIndex].mapRole.Add(pUnit->GetID(), static_cast<Role*>(pUnit));
	else
		m_pVisTile[nVisIndex].mapCreature.Add(pUnit->GetID(), static_cast<Creature*>(pUnit));

	if( pUnit->IsInStateInvisible() )
		m_pVisTile[nVisIndex].mapInvisibleUnit.Add(pUnit->GetID(), pUnit);
}

//-----------------------------------------------------------------------------------------
// 从某个可视地砖中删除一个玩家
//-----------------------------------------------------------------------------------------
VOID Map::RemoveFromVisTile(Unit* pUnit)
{
	ASSERT( P_VALID(pUnit) );

	INT nVisIndex = pUnit->GetVisTileIndex();
	ASSERT( GT_VALID(nVisIndex) );

	if( pUnit->IsRole() )
		m_pVisTile[nVisIndex].mapRole.Erase(pUnit->GetID());
	else
		m_pVisTile[nVisIndex].mapCreature.Erase(pUnit->GetID());

	if( pUnit->IsInStateInvisible() )
		m_pVisTile[nVisIndex].mapInvisibleUnit.Erase(pUnit->GetID());
	
	pUnit->SetVisTileIndex(GT_INVALID);
}

//-----------------------------------------------------------------------------------------
// 世界单位转为可视地砖的索引
//-----------------------------------------------------------------------------------------
INT Map::WorldPosToVisIndex(const Vector3& pos)
{
	// 坐标非法
	if( !IsPosValid(pos) ) return GT_INVALID;

	INT nIndexX = INT(pos.x / TILE_SCALE / m_pInfo->nVisDist) + 1;
	INT nIndexZ = INT(pos.z / TILE_SCALE / m_pInfo->nVisDist) + 1;

	ASSERT( nIndexX > 0 && nIndexX < m_nVisTileArrayWidth - 1 );
	ASSERT( nIndexZ > 0 && nIndexZ < m_nVisTileArrayHeight - 1 );

	return nIndexZ * m_nVisTileArrayWidth + nIndexX;
}

//------------------------------------------------------------------------------------------
// 得到周围地砖
//------------------------------------------------------------------------------------------
VOID Map::GetVisTile(INT nVisIndex, tagVisTile* visTile[ED_End])
{
	// todo: 这个地方还要改一下，判断不是很严谨
	if( nVisIndex == GT_INVALID )
		return;

	// 视野的地砖索引
	INT m[ED_End] = {0};
	m[ED_Center]		= nVisIndex;
	m[ED_Left]			= nVisIndex - 1;
	m[ED_Right]			= nVisIndex + 1;
	m[ED_Top]			= nVisIndex - m_nVisTileArrayWidth;
	m[ED_Bottom]		= nVisIndex + m_nVisTileArrayWidth;
	m[ED_LeftTop]		= m[ED_Top] - 1;
	m[ED_RightTop]		= m[ED_Top] + 1;
	m[ED_LeftBottom]	= m[ED_Bottom] - 1;
	m[ED_RightBottom]	= m[ED_Bottom] + 1;

	for(INT n = 0 ; n < ED_End; n++)
	{
		if(m[n]>=0 && m[n] < m_nVisTileArrayWidth * m_nVisTileArrayHeight)
			visTile[n] = &m_pVisTile[m[n]];
	}
}

//------------------------------------------------------------------------------------------
// 根据某个坐标点以及范围确定有几个visTile在该矩形范围内
//------------------------------------------------------------------------------------------
VOID Map::GetVisTile(Vector3& pos, FLOAT fRange, tagVisTile* visTile[ED_End])
{
	BOOL b[ED_End] = {0};

	INT m[ED_End] = {0};
	m[ED_Center]		= GT_INVALID;
	m[ED_Left]			= GT_INVALID;
	m[ED_Right]			= GT_INVALID;
	m[ED_Top]			= GT_INVALID;
	m[ED_Bottom]		= GT_INVALID;
	m[ED_LeftTop]		= GT_INVALID;
	m[ED_RightTop]		= GT_INVALID;
	m[ED_LeftBottom]	= GT_INVALID;
	m[ED_RightBottom]	= GT_INVALID;

	// 取到中心点的visIndex
	INT nIndexX = INT(pos.x / TILE_SCALE / m_pInfo->nVisDist) + 1;
	INT nIndexZ = INT(pos.z / TILE_SCALE / m_pInfo->nVisDist) + 1;

	ASSERT( nIndexX > 0 && nIndexX < m_nVisTileArrayWidth - 1 );
	ASSERT( nIndexZ > 0 && nIndexZ < m_nVisTileArrayHeight - 1 );

	// 取到边界的索引
	FLOAT fTempX = pos.x - fRange;
	INT nSrcIndexX = INT(fTempX / TILE_SCALE / m_pInfo->nVisDist) + 1;

	fTempX = pos.x + fRange;
	INT nDestIndexX = INT(fTempX / TILE_SCALE / m_pInfo->nVisDist) + 1;

	FLOAT fTempZ = pos.z - fRange;
	INT nSrcIndexZ = INT(fTempZ / TILE_SCALE / m_pInfo->nVisDist) + 1;

	fTempZ = pos.z + fRange;
	INT nDestIndexZ = INT(fTempZ / TILE_SCALE / m_pInfo->nVisDist) + 1;


	// 设置是否包含
	b[ED_Center] = TRUE;
	if( nSrcIndexX < nIndexX )
	{
		b[ED_Left] = TRUE;	
	}
	if( nDestIndexX > nIndexX )
	{
		b[ED_Right] = TRUE;
	}
	if( nSrcIndexZ < nIndexZ )
	{
		b[ED_Top] = TRUE;
	}
	if( nDestIndexZ > nIndexZ )
	{
		b[ED_Bottom] = TRUE;
	}

	b[ED_LeftTop]		=	b[ED_Left] && b[ED_Top];
	b[ED_LeftBottom]	=	b[ED_Left] && b[ED_Bottom];
	b[ED_RightTop]		=	b[ED_Right] && b[ED_Top];
	b[ED_RightBottom]	=	b[ED_Right] && b[ED_Bottom];


	m[ED_Center] = nIndexZ * m_nVisTileArrayWidth + nIndexX;

	if( b[ED_Left] )		m[ED_Left] = m[ED_Center] - 1;
	if( b[ED_Right] )		m[ED_Right] = m[ED_Center] + 1;
	if( b[ED_Top] )			m[ED_Top] = m[ED_Center] - m_nVisTileArrayWidth;
	if( b[ED_Bottom] )		m[ED_Bottom] = m[ED_Center] + m_nVisTileArrayWidth;
	if( b[ED_LeftTop] )		m[ED_LeftTop] = m[ED_Top] - 1;
	if( b[ED_RightTop] )	m[ED_RightTop] = m[ED_Top] + 1;
	if( b[ED_LeftBottom] )	m[ED_LeftBottom] = m[ED_Bottom] - 1;
	if( b[ED_RightBottom] )	m[ED_RightBottom] = m[ED_Bottom] + 1;

	for(INT n = 0 ; n < ED_End; n++)
	{
		if( m[n] != GT_INVALID )
			visTile[n] = &m_pVisTile[m[n]];
	}

}

//------------------------------------------------------------------------------------------
// 得到玩家改变位置后离开视野的地砖
//-------------------------------------------------------------------------------------------
VOID Map::GetVisTileDec(INT nOldVisIndex, INT nNewVisIndex, tagVisTile* visTile[ED_End])
{
	if( nOldVisIndex == GT_INVALID || nNewVisIndex == GT_INVALID )
		return;

	if( nNewVisIndex == nOldVisIndex )
		return;

	INT nOldVisIndexX = nOldVisIndex % m_nVisTileArrayWidth;
	INT nOldVisIndexZ = nOldVisIndex / m_nVisTileArrayWidth;
	INT nNewVisIndexX = nNewVisIndex % m_nVisTileArrayWidth;
	INT nNewVisIndexZ = nNewVisIndex / m_nVisTileArrayWidth;

	INT m[ED_End] = {0};
	m[ED_Center]		= GT_INVALID;
	m[ED_Left]			= GT_INVALID;
	m[ED_Right]			= GT_INVALID;
	m[ED_Top]			= GT_INVALID;
	m[ED_Bottom]		= GT_INVALID;
	m[ED_LeftTop]		= GT_INVALID;
	m[ED_RightTop]		= GT_INVALID;
	m[ED_LeftBottom]	= GT_INVALID;
	m[ED_RightBottom]	= GT_INVALID;

	if( abs(nOldVisIndexX - nNewVisIndexX) >= 3
		|| abs(nOldVisIndexZ - nNewVisIndexZ) >= 3 )
	{
		m[ED_Center]		= nOldVisIndex;
		m[ED_Left]			= nOldVisIndex - 1;
		m[ED_Right]			= nOldVisIndex + 1;
		m[ED_Top]			= nOldVisIndex - m_nVisTileArrayWidth;
		m[ED_Bottom]		= nOldVisIndex + m_nVisTileArrayWidth;
		m[ED_LeftTop]		= m[ED_Top] - 1;
		m[ED_RightTop]		= m[ED_Top] + 1;
		m[ED_LeftBottom]	= m[ED_Bottom] - 1;
		m[ED_RightBottom]	= m[ED_Bottom] + 1;

		for(INT n = 0; n < ED_End; n++)
		{
			visTile[n] = &m_pVisTile[m[n]];
		}
		return;
	}

	// X左边移动
	if( nNewVisIndexX < nOldVisIndexX )
	{
		m[ED_Right]			= nOldVisIndex + 1;
		m[ED_RightTop]		= m[ED_Right] - m_nVisTileArrayWidth;
		m[ED_RightBottom]	= m[ED_Right] + m_nVisTileArrayWidth;

		if( nNewVisIndexX - nOldVisIndexX == -2 )
		{
			m[ED_Center]	= nOldVisIndex;
			m[ED_Top]		= m[ED_Center] - m_nVisTileArrayWidth;
			m[ED_Bottom]	= m[ED_Bottom] + m_nVisTileArrayWidth;
		}
	}
	// X右边移动
	else if( nNewVisIndexX > nOldVisIndexX )
	{
		m[ED_Left]			= nOldVisIndex - 1;
		m[ED_LeftTop]		= m[ED_Left] - m_nVisTileArrayWidth;
		m[ED_LeftBottom]	= m[ED_Left] + m_nVisTileArrayWidth;

		if( nNewVisIndexX - nOldVisIndexX == 2 )
		{
			m[ED_Center]	= nOldVisIndex;
			m[ED_Top]		= m[ED_Center] - m_nVisTileArrayWidth;
			m[ED_Bottom]	= m[ED_Bottom] + m_nVisTileArrayWidth;
		}
	}
	// X没有移动
	else
	{

	}

	// Z上部移动
	if( nNewVisIndexZ < nOldVisIndexZ )
	{
		m[ED_Bottom]		= nOldVisIndex + m_nVisTileArrayWidth;
		m[ED_LeftBottom]	= m[ED_Bottom] - 1;
		m[ED_RightBottom]	= m[ED_Bottom] + 1;

		if( nNewVisIndexZ - nOldVisIndexZ == -2 )
		{
			m[ED_Center]	= nOldVisIndex;
			m[ED_Left]		= m[ED_Center] - 1;
			m[ED_Bottom]	= m[ED_Center] + 1;
		}
	}
	// Z下部移动
	else if( nNewVisIndexZ > nOldVisIndexZ )
	{
		m[ED_Top]			= nOldVisIndex - m_nVisTileArrayWidth;
		m[ED_LeftTop]		= m[ED_Top] - 1;
		m[ED_RightTop]		= m[ED_Top] + 1;

		if( nNewVisIndexZ - nOldVisIndexZ == 2 )
		{
			m[ED_Center]	= nOldVisIndex;
			m[ED_Left]		= m[ED_Center] - 1;
			m[ED_Bottom]	= m[ED_Center] + 1;
		}
	}
	// Z没有移动
	else
	{

	}

	// 统计坐标点
	for(INT n = 0; n < ED_End; n++)
	{
		if( m[n] != GT_INVALID )
			visTile[n] = &m_pVisTile[m[n]];
	}
}

//------------------------------------------------------------------------------------------
// 得到玩家改变位置后进入视野的地砖
//-------------------------------------------------------------------------------------------
VOID Map::GetVisTileAdd(INT nOldVisIndex, INT nNewVisIndex, tagVisTile* visTile[ED_End])
{
	if( nOldVisIndex == GT_INVALID || nNewVisIndex == GT_INVALID )
		return;

	if( nNewVisIndex == nOldVisIndex )
		return;

	INT nOldVisIndexX = nOldVisIndex % m_nVisTileArrayWidth;
	INT nOldVisIndexZ = nOldVisIndex / m_nVisTileArrayWidth;
	INT nNewVisIndexX = nNewVisIndex % m_nVisTileArrayWidth;
	INT nNewVisIndexZ = nNewVisIndex / m_nVisTileArrayWidth;

	INT m[ED_End] = {0};
	m[ED_Center]		= GT_INVALID;
	m[ED_Left]			= GT_INVALID;
	m[ED_Right]			= GT_INVALID;
	m[ED_Top]			= GT_INVALID;
	m[ED_Bottom]		= GT_INVALID;
	m[ED_LeftTop]		= GT_INVALID;
	m[ED_RightTop]		= GT_INVALID;
	m[ED_LeftBottom]	= GT_INVALID;
	m[ED_RightBottom]	= GT_INVALID;

	if( abs(nOldVisIndexX - nNewVisIndexX) >= 3
		|| abs(nOldVisIndexZ - nNewVisIndexZ) >= 3 )
	{
		m[ED_Center]		= nNewVisIndex;
		m[ED_Left]			= nNewVisIndex - 1;
		m[ED_Right]			= nNewVisIndex + 1;
		m[ED_Top]			= nNewVisIndex - m_nVisTileArrayWidth;
		m[ED_Bottom]		= nNewVisIndex + m_nVisTileArrayWidth;
		m[ED_LeftTop]		= m[ED_Top] - 1;
		m[ED_RightTop]		= m[ED_Top] + 1;
		m[ED_LeftBottom]	= m[ED_Bottom] - 1;
		m[ED_RightBottom]	= m[ED_Bottom] + 1;

		for(INT n = 0; n < ED_End; n++)
		{
			visTile[n] = &m_pVisTile[m[n]];
		}
		return;
	}

	// X左边移动
	if( nNewVisIndexX < nOldVisIndexX )
	{
		m[ED_Left]			= nNewVisIndex - 1;
		m[ED_LeftTop]		= m[ED_Left] - m_nVisTileArrayWidth;
		m[ED_LeftBottom]	= m[ED_Left] + m_nVisTileArrayWidth;

		if( nNewVisIndexX - nOldVisIndexX == -2 )
		{
			m[ED_Center]	= nNewVisIndex;
			m[ED_Top]		= m[ED_Center] - m_nVisTileArrayWidth;
			m[ED_Bottom]	= m[ED_Bottom] + m_nVisTileArrayWidth;
		}
	}
	// X右边移动
	else if( nNewVisIndexX > nOldVisIndexX )
	{
		m[ED_Right]			= nNewVisIndex + 1;
		m[ED_RightTop]		= m[ED_Right] - m_nVisTileArrayWidth;
		m[ED_RightBottom]	= m[ED_Right] + m_nVisTileArrayWidth;

		if( nNewVisIndexX - nOldVisIndexX == 2 )
		{
			m[ED_Center]	= nNewVisIndex;
			m[ED_Top]		= m[ED_Center] - m_nVisTileArrayWidth;
			m[ED_Bottom]	= m[ED_Bottom] + m_nVisTileArrayWidth;
		}
	}
	// X没有移动
	else
	{

	}

	// Z上部移动
	if( nNewVisIndexZ < nOldVisIndexZ )
	{
		m[ED_Top]		= nNewVisIndex - m_nVisTileArrayWidth;
		m[ED_LeftTop]	= m[ED_Top] - 1;
		m[ED_RightTop]	= m[ED_Top] + 1;

		if( nNewVisIndexZ - nOldVisIndexZ == -2 )
		{
			m[ED_Center]	= nNewVisIndex;
			m[ED_Left]		= m[ED_Center] - 1;
			m[ED_Bottom]	= m[ED_Center] + 1;
		}
	}
	// Z下部移动
	else if( nNewVisIndexZ > nOldVisIndexZ )
	{
		m[ED_Bottom]		= nNewVisIndex + m_nVisTileArrayWidth;
		m[ED_LeftBottom]	= m[ED_Bottom] - 1;
		m[ED_RightBottom]	= m[ED_Bottom] + 1;

		if( nNewVisIndexZ - nOldVisIndexZ == 2 )
		{
			m[ED_Center]	= nNewVisIndex;
			m[ED_Left]		= m[ED_Center] - 1;
			m[ED_Bottom]	= m[ED_Center] + 1;
		}
	}
	// Z没有移动
	else
	{

	}

	// 统计坐标点
	for(INT n = 0; n < ED_End; n++)
	{
		if( m[n] != GT_INVALID )
			visTile[n] = &m_pVisTile[m[n]];
	}
}

//---------------------------------------------------------------------------------------------
// 得到玩家改变位置后进入视野的地砖和离开视野的地砖
//---------------------------------------------------------------------------------------------
VOID Map::GetVisTileAddAndDec(INT nOldVisIndex, INT nNewVisIndex, tagVisTile* visTileAdd[ED_End], tagVisTile* visTileDec[ED_End])
{
	if( nOldVisIndex == GT_INVALID || nNewVisIndex == GT_INVALID )
		return;

	if( nNewVisIndex == nOldVisIndex )
		return;

	INT nOldVisIndexX = nOldVisIndex % m_nVisTileArrayWidth;
	INT nOldVisIndexZ = nOldVisIndex / m_nVisTileArrayWidth;
	INT nNewVisIndexX = nNewVisIndex % m_nVisTileArrayWidth;
	INT nNewVisIndexZ = nNewVisIndex / m_nVisTileArrayWidth;

	// 进入视野的地砖索引
	INT m[ED_End] = {0};
	m[ED_Center]		= GT_INVALID;
	m[ED_Left]			= GT_INVALID;
	m[ED_Right]			= GT_INVALID;
	m[ED_Top]			= GT_INVALID;
	m[ED_Bottom]		= GT_INVALID;
	m[ED_LeftTop]		= GT_INVALID;
	m[ED_RightTop]		= GT_INVALID;
	m[ED_LeftBottom]	= GT_INVALID;
	m[ED_RightBottom]	= GT_INVALID;

	// 离开视野的地砖索引
	INT r[ED_End] = {0};
	r[ED_Center]		= GT_INVALID;
	r[ED_Left]			= GT_INVALID;
	r[ED_Right]			= GT_INVALID;
	r[ED_Top]			= GT_INVALID;
	r[ED_Bottom]		= GT_INVALID;
	r[ED_LeftTop]		= GT_INVALID;
	r[ED_RightTop]		= GT_INVALID;
	r[ED_LeftBottom]	= GT_INVALID;
	r[ED_RightBottom]	= GT_INVALID;


	if( abs(nOldVisIndexX - nNewVisIndexX) >= 3
		|| abs(nOldVisIndexZ - nNewVisIndexZ) >= 3 )
	{
		// 进入视野的可视地砖
		m[ED_Center]		= nNewVisIndex;
		m[ED_Left]			= nNewVisIndex - 1;
		m[ED_Right]			= nNewVisIndex + 1;
		m[ED_Top]			= nNewVisIndex - m_nVisTileArrayWidth;
		m[ED_Bottom]		= nNewVisIndex + m_nVisTileArrayWidth;
		m[ED_LeftTop]		= m[ED_Top] - 1;
		m[ED_RightTop]		= m[ED_Top] + 1;
		m[ED_LeftBottom]	= m[ED_Bottom] - 1;
		m[ED_RightBottom]	= m[ED_Bottom] + 1;

		// 离开视野的可视地砖
		r[ED_Center]		= nOldVisIndex;
		r[ED_Left]			= nOldVisIndex - 1;
		r[ED_Right]			= nOldVisIndex + 1;
		r[ED_Top]			= nOldVisIndex - m_nVisTileArrayWidth;
		r[ED_Bottom]		= nOldVisIndex + m_nVisTileArrayWidth;
		r[ED_LeftTop]		= r[ED_Top] - 1;
		r[ED_RightTop]		= r[ED_Top] + 1;
		r[ED_LeftBottom]	= r[ED_Bottom] - 1;
		r[ED_RightBottom]	= r[ED_Bottom] + 1;

		for(INT n = 0; n < ED_End; n++)
		{
			visTileAdd[n] = &m_pVisTile[m[n]];
			visTileDec[n] = &m_pVisTile[r[n]];
		}
		return;
	}

	// X左边移动
	if( nNewVisIndexX < nOldVisIndexX )
	{
		// 进入视野的可视地砖
		m[ED_Left]			= nNewVisIndex - 1;
		m[ED_LeftTop]		= m[ED_Left] - m_nVisTileArrayWidth;
		m[ED_LeftBottom]	= m[ED_Left] + m_nVisTileArrayWidth;

		// 离开视野的可视地砖
		r[ED_Right]			= nOldVisIndex + 1;
		r[ED_RightTop]		= r[ED_Right] - m_nVisTileArrayWidth;
		r[ED_RightBottom]	= r[ED_Right] + m_nVisTileArrayWidth;

		if( nNewVisIndexX - nOldVisIndexX == -2 )
		{
			// 进入视野的可视地砖
			m[ED_Center]	= nNewVisIndex;
			m[ED_Top]		= m[ED_Center] - m_nVisTileArrayWidth;
			m[ED_Bottom]	= m[ED_Bottom] + m_nVisTileArrayWidth;

			// 离开视野的可视地砖
			r[ED_Center]	= nOldVisIndex;
			r[ED_Top]		= r[ED_Center] - m_nVisTileArrayWidth;
			r[ED_Bottom]	= r[ED_Bottom] + m_nVisTileArrayWidth;
		}
	}
	// X右边移动
	else if( nNewVisIndexX > nOldVisIndexX )
	{
		// 进入视野的可视地砖
		m[ED_Right]			= nNewVisIndex + 1;
		m[ED_RightTop]		= m[ED_Right] - m_nVisTileArrayWidth;
		m[ED_RightBottom]	= m[ED_Right] + m_nVisTileArrayWidth;

		// 离开视野的可视地砖
		r[ED_Left]			= nOldVisIndex - 1;
		r[ED_LeftTop]		= r[ED_Left] - m_nVisTileArrayWidth;
		r[ED_LeftBottom]	= r[ED_Left] + m_nVisTileArrayWidth;

		if( nNewVisIndexX - nOldVisIndexX == 2 )
		{
			// 进入视野的可视地砖
			m[ED_Center]	= nNewVisIndex;
			m[ED_Top]		= m[ED_Center] - m_nVisTileArrayWidth;
			m[ED_Bottom]	= m[ED_Bottom] + m_nVisTileArrayWidth;

			// 离开视野的可视地砖
			r[ED_Center]	= nOldVisIndex;
			r[ED_Top]		= r[ED_Center] - m_nVisTileArrayWidth;
			r[ED_Bottom]	= r[ED_Bottom] + m_nVisTileArrayWidth;
		}
	}
	// X没有移动
	else
	{

	}

	// Z上部移动
	if( nNewVisIndexZ < nOldVisIndexZ )
	{
		// 进入视野的可视地砖
		m[ED_Top]			= nNewVisIndex - m_nVisTileArrayWidth;
		m[ED_LeftTop]		= m[ED_Top] - 1;
		m[ED_RightTop]		= m[ED_Top] + 1;

		// 离开视野的可视地砖
		r[ED_Bottom]		= nOldVisIndex + m_nVisTileArrayWidth;
		r[ED_LeftBottom]	= r[ED_Bottom] - 1;
		r[ED_RightBottom]	= r[ED_Bottom] + 1;


		if( nNewVisIndexZ - nOldVisIndexZ == -2 )
		{
			// 进入视野的可视地砖
			m[ED_Center]	= nNewVisIndex;
			m[ED_Left]		= m[ED_Center] - 1;
			m[ED_Bottom]	= m[ED_Center] + 1;

			// 离开视野的可视地砖
			r[ED_Center]	= nOldVisIndex;
			r[ED_Left]		= r[ED_Center] - 1;
			r[ED_Bottom]	= r[ED_Center] + 1;

		}
	}
	// Z下部移动
	else if( nNewVisIndexZ > nOldVisIndexZ )
	{
		// 进入视野的可视地砖
		m[ED_Bottom]		= nNewVisIndex + m_nVisTileArrayWidth;
		m[ED_LeftBottom]	= m[ED_Bottom] - 1;
		m[ED_RightBottom]	= m[ED_Bottom] + 1;

		// 离开视野的可视地砖
		r[ED_Top]			= nOldVisIndex - m_nVisTileArrayWidth;
		r[ED_LeftTop]		= r[ED_Top] - 1;
		r[ED_RightTop]		= r[ED_Top] + 1;


		if( nNewVisIndexZ - nOldVisIndexZ == 2 )
		{
			// 进入视野的可视地砖
			m[ED_Center]	= nNewVisIndex;
			m[ED_Left]		= m[ED_Center] - 1;
			m[ED_Bottom]	= m[ED_Center] + 1;

			// 离开视野的可视地砖
			r[ED_Center]	= nOldVisIndex;
			r[ED_Left]		= r[ED_Center] - 1;
			r[ED_Bottom]	= r[ED_Center] + 1;
		}
	}
	// Z没有移动
	else
	{

	}

	// 统计坐标点
	for(INT n = 0; n < ED_End; n++)
	{
		if( m[n] != GT_INVALID )
			visTileAdd[n] = &m_pVisTile[m[n]];

		if( r[n] != GT_INVALID )
			visTileDec[n] = &m_pVisTile[r[n]];
	}
}

//------------------------------------------------------------------------------------------
// 发送九宫格范围内的消息
//------------------------------------------------------------------------------------------
VOID Map::SendBigVisTileMsg(Unit *pSelf, const LPVOID pMsg, const DWORD dwSize)
{
	if( !P_VALID(pSelf) ) return;
	if( !P_VALID(pMsg) ) return;

	tagVisTile* pVisTile[ED_End] = {0};
	GetVisTile(pSelf->GetVisTileIndex(), pVisTile);

	SendBigVisTileMsg(pVisTile, pMsg, dwSize);
}

//------------------------------------------------------------------------------------------
// 发送九宫格范围内的消息
//------------------------------------------------------------------------------------------
VOID Map::SendBigVisTileMsg(tagVisTile *pVisTile[ED_End], const LPVOID pMsg, const DWORD dwSize)
{
	for(INT n = 0; n < ED_End; n++)
	{
		if( NULL == pVisTile[n] )
			continue;
		
		// 找到每个地砖的人
		TMap<DWORD, Role*>& mapRole = pVisTile[n]->mapRole;
		mapRole.ResetIterator();
		Role* pRole = NULL;

		while( mapRole.PeekNext(pRole) )
		{
			if( P_VALID(pRole) )
			{
				pRole->SendMessage(pMsg, dwSize);
			}
		}
	}
}

//------------------------------------------------------------------------------------------
// 发送消息给本地图内的玩家
//------------------------------------------------------------------------------------------
VOID Map::SendMapMessage(const LPVOID pMsg, const DWORD dwSize)
{
	if(!P_VALID(pMsg))	return;
	Role *pRole = NULL;

	TMap<DWORD, Role*>::TMapIterator it = m_mapRole.Begin();
	while (m_mapRole.PeekNext(it, pRole))
	{
		if( P_VALID(pRole) && P_VALID(pRole->GetSession()) )
		{
			pRole->GetSession()->SendMessage(pMsg, dwSize);
		}
	}
}

//--------------------------------------------------------------------------------------------
// 同步可视地砖内的隐身单位给玩家
//--------------------------------------------------------------------------------------------
VOID Map::SyncBigVisTileInvisUnit2Role(Role* pRole, tagVisTile *pVisTile[ED_End])
{
	// 1.是否有探隐能力
	if( !pRole->HasDectiveSkill() )
		return;
	
	BYTE			byMsg[1024] = {0};
	TList<DWORD>	listRemove;
	
	// 无行凶
	//if ( !pRole->IsInRoleState(ERS_PK))

		for(INT n = 0; n < ED_End; n++)
		{
			if( NULL == pVisTile[n] )
				continue;

			// 找到每个地砖的单位
			TMap<DWORD, Unit*>& mapUnit = pVisTile[n]->mapInvisibleUnit;
			mapUnit.ResetIterator();
			Unit* pUnit = NULL;

			while( mapUnit.PeekNext(pUnit) )
			{
				if( !P_VALID(pUnit) )
					continue;

				// 2.是否完全可见
				if( (pRole->CanSeeTargetEntirely(pUnit)) )
					continue;

					// 3.在可视列表中
					if( pRole->IsInVisList(pUnit->GetID()) )
					{
						// 在可视范围内
						if( pRole->IsInVisDist(pUnit) )
							continue;

						// 不在可视范围内
						listRemove.PushBack(pUnit->GetID());

						pRole->RemoveFromVisList(pUnit->GetID());

						continue;
					}

					// 4.不在A的可视列表中
					// 4.1不在A可视范围内
					if( !pRole->IsInVisDist(pUnit) )
						continue;

					// 4.2 在A可视范围内
					DWORD dwSize = CalMovementMsg(pUnit, byMsg, 1024);
					if(dwSize == 0)
						continue;

					pRole->SendMessage(byMsg, dwSize);

					pRole->Add2VisList(pUnit->GetID());
			}
		}


	// 如果list不为空，且self是玩家的话，则发送给该玩家要删除的远程玩家
	INT nListSize = listRemove.Size();
	if( nListSize <= 0 )
		return;

	DWORD dwSize = sizeof(tagNS_RemoveRemote) + (nListSize-1)*sizeof(DWORD);
	MCREATE_MSG(pSend, dwSize, NS_RemoveRemote);

	for(INT n = 0; n < nListSize; n++)
	{
		pSend->dwRoleID[n] = listRemove.PopFront();
	}

	pRole->SendMessage(pSend, pSend->dwSize);

	MDEL_MSG(pMsg);
}

//--------------------------------------------------------------------------------------------
// 将可视地砖内所有可视单位及掉落的物品同步给玩家
//--------------------------------------------------------------------------------------------
VOID Map::SyncBigVisTileVisUnitAndGroundItem2Role(Role* pRole, tagVisTile *pVisTile[ED_End])
{
	ASSERT( P_VALID(pRole) );

	BYTE byMsg[1024] = {0};
	DWORD dwSize = 0;	

	tagNS_SyncGroundItem send;

	// 遍历九宫格列表
	for(INT n = 0; n < ED_End; n++)
	{
		if( !P_VALID(pVisTile[n]) )
			continue;

		// 同步非隐身状态下远端玩家
		TMap<DWORD, Role*>& mapRole = pVisTile[n]->mapRole;
		mapRole.ResetIterator();
		Role* pRemoteRole = NULL;

		while( mapRole.PeekNext(pRemoteRole) )
		{
			if( pRemoteRole->IsInStateInvisible() )
				continue;

			dwSize = CalMovementMsg(pRemoteRole, byMsg, 1024);
			if( dwSize == 0 )
				continue;
			pRole->SendMessage(byMsg, dwSize);
		}

		// 同步非隐身状态下怪
		TMap<DWORD, Creature*>& mapCreature = pVisTile[n]->mapCreature;
		mapCreature.ResetIterator();
		Creature* pCreature = NULL;

		while( mapCreature.PeekNext(pCreature) )
		{
			if( pCreature->IsInStateInvisible() )
				continue;

			dwSize = CalMovementMsg(pCreature, byMsg, 1024);
			if( dwSize == 0 )
				continue;
			pRole->SendMessage(byMsg, dwSize);
		}

		//同步地面物品到客户端
		TMap<INT64, tagGroundItem*>& mapGroundItem = pVisTile[n]->mapGroundItem;
		mapGroundItem.ResetIterator();
		tagGroundItem* pGroundItem = NULL;

		while( mapGroundItem.PeekNext(pGroundItem) )
		{
			send.n64Serial	= pGroundItem->n64ID;
			send.dwTypeID	= pGroundItem->dwTypeID;
			send.nNum		= pGroundItem->nNum;

			send.dwPutDownUnitID = pGroundItem->dwPutDownUnitID;
			send.dwOwnerID = pGroundItem->dwOwnerID;
			send.dwTeamID  = pGroundItem->dwTeamID;
			send.dwGroupID = pGroundItem->dwGroupID;
			send.vPos = pGroundItem->vPos;
			send.bKilled = FALSE;
			pRole->SendMessage(&send, send.dwSize);
		}
	}
}

//--------------------------------------------------------------------------------------------
// 将隐身单位同步给可视地砖内的可以看到他的玩家
//--------------------------------------------------------------------------------------------
VOID Map::SyncInvisUnit2BigVisTileRole(Unit* pUnit, tagVisTile *pVisTile[ED_End], 
										const LPVOID pMsg, const DWORD dwSize)
{
	ASSERT(pUnit->IsInStateInvisible());
	
	tagNS_RemoveRemote sendRemove;
	sendRemove.dwRoleID[0] = pUnit->GetID();
	
	// 不再PK状态
	Role *pRoleSrc  =static_cast<Role *>(pUnit);
	if( !pRoleSrc->IsInRoleState(ERS_PK) && !pRoleSrc->IsInRoleState(ERS_PKEX) )
	{
		for(INT n = 0; n < ED_End; n++)
		{
			if( NULL == pVisTile[n] )
				continue;

			// 找到每个地砖的人
			TMap<DWORD, Role*>& mapRole = pVisTile[n]->mapRole;
			mapRole.ResetIterator();
			Role* pRole = NULL;

			while( mapRole.PeekNext(pRole) )
			{
				if( !P_VALID(pRole) )
					continue;

				// 1.是否完全可见
				if( pRole->CanSeeTargetEntirely(pUnit) )
				{
					pRole->SendMessage(pMsg, dwSize);
					continue;
				}

				// 2.B是否有探隐能力
				if( !pRole->HasDectiveSkill() )
					continue;

				// 3.在B的可视列表中
				if( pRole->IsInVisList(pUnit->GetID()) )
				{
					// 不在B可视范围内
					if( !pRole->IsInVisDist(pUnit) )
					{
						pRole->SendMessage(&sendRemove, sendRemove.dwSize);
						pRole->RemoveFromVisList(pUnit->GetID());
					}
					else
					{
						pRole->SendMessage(pMsg, dwSize);
					}

					continue;
				}

				// 4.不在B的可视列表中
				// 4.1不在B可视范围内
				if( !pRole->IsInVisDist(pUnit) )
					continue;

				// 4.2 在B可视范围内
				pRole->SendMessage(pMsg, dwSize);

				pRole->Add2VisList(pUnit->GetID());
			}
		}
	}
	else
	{
		for(INT n = 0; n < ED_End; n++)
		{
			if( NULL == pVisTile[n] )
				continue;

			// 找到每个地砖的人
			TMap<DWORD, Role*>& mapRole = pVisTile[n]->mapRole;
			mapRole.ResetIterator();
			Role* pRole = NULL;

			while( mapRole.PeekNext(pRole) )
			{
				if( !P_VALID(pRole) )
					continue;

				// 1.是否完全可见
				if( pRole->CanSeeTargetEntirely(pUnit) )
				{
					pRole->SendMessage(pMsg, dwSize);
					continue;
				}

				// 2.B是否有探隐能力
				if( !pRole->HasDectiveSkill() )
					continue;

				// 3.在B的可视列表中
				if( pRole->IsInVisList(pUnit->GetID()) )
				{
					// 不在B可视范围内
					if( !pRole->IsInVisDistForOpenMacre(pUnit) )
					{
						pRole->SendMessage(&sendRemove, sendRemove.dwSize);
						pRole->RemoveFromVisList(pUnit->GetID());
					}
					else
					{
						pRole->SendMessage(pMsg, dwSize);
					}

					continue;
				}

				// 4.不在B的可视列表中
				// 4.1不在B可视范围内
				if( !pRole->IsInVisDistForOpenMacre(pUnit) )
					continue;

				// 4.2 在B可视范围内
				pRole->SendMessage(pMsg, dwSize);

				pRole->Add2VisList(pUnit->GetID());
			}
		}
	}
		
	
}

//--------------------------------------------------------------------------------------------
// 同步可视地砖内的移动
//--------------------------------------------------------------------------------------------
VOID Map::SyncMovementToBigVisTile(Unit* pSelf)
{
	if( !P_VALID(pSelf) ) return;

	BYTE	byMsg[1024] = {0};
	DWORD	dwSize = CalMovementMsg(pSelf, byMsg, 1024);

	if( dwSize == 0 ) return;

	tagVisTile* pVisTile[ED_End] = {0};

	// 获得周围九宫格的可视地砖
	GetVisTile(pSelf->GetVisTileIndex(), pVisTile);
	
	DWORD dwCurState = pSelf->GetState();

	// 向可视地砖的人发送同步命令
	if( !pSelf->IsInStateInvisible() )	// A可见
	{
		SyncVisUnit2BigVisTileRole(pSelf, pVisTile, byMsg, dwSize);
	}
	else	// A不可见
	{
		SyncInvisUnit2BigVisTileRole(pSelf, pVisTile, byMsg, dwSize);
	}

	// 将可视地砖内隐身单位同步给当前玩家
	if( pSelf->IsRole() )
		SyncBigVisTileInvisUnit2Role(static_cast<Role*>(pSelf), pVisTile);
}

//-------------------------------------------------------------------------------------------------------------
// 改变地图可视地砖时同步
//-------------------------------------------------------------------------------------------------------------
VOID Map::SyncChangeVisTile(Unit* pUnit, INT nOldVisIndex, INT nNewVisIndex)
{
	// 先把玩家从当前的可视链表中删除
	RemoveFromVisTile(pUnit);

	// 取出因为切换地砖而删除和添加的地砖
	tagVisTile* pVisTileAdd[ED_End] = {0};
	tagVisTile* pVisTileDec[ED_End] = {0};

	GetVisTileAddAndDec(nOldVisIndex, nNewVisIndex, pVisTileAdd, pVisTileDec);

	// 首先针对删除的玩家和生物进行移除
	SyncRemovedUnits(pUnit, pVisTileDec);

	// 再同步加入到视野中的玩家和生物
	SyncAddedUnits(pUnit, pVisTileAdd);

	// 再把其加入到新的可视地砖中
	AddToVisTile(pUnit, nNewVisIndex);
}

//-------------------------------------------------------------------------------------------------------------
// 通知各个远程玩家删除该对象，并且如果Self是玩家的话，将这些远程玩家从自己本地客户端删除
//-------------------------------------------------------------------------------------------------------------
VOID Map::SyncRemovedUnits(Unit* pSelf, tagVisTile* pVisTileDec[ED_End])
{
	ASSERT( P_VALID(pSelf) );

	BOOL bSelfInvisible = pSelf->IsInStateInvisible();

	tagNS_RemoveRemote send;
	send.dwRoleID[0] = pSelf->GetID();

	// 同步给九宫格内其他玩家
	if( pSelf->IsInStateInvisible() )
	{
		SyncInvisUnit2BigVisTileRole(pSelf, pVisTileDec, &send, send.dwSize);
	}
	else
	{
		SyncVisUnit2BigVisTileRole(pSelf, pVisTileDec, &send, send.dwSize);
	}

	// 将九宫格内信息同步给该玩家
	if( pSelf->IsRole() )
	{
		Role *pRole = static_cast<Role*>(pSelf);

		SyncRemoveBigVisTileUnitAndGroundItem2Role(pRole, pVisTileDec);
	}
}

//-------------------------------------------------------------------------------------------------------------
// 角色异常九宫格后，将其内信息同步给角色
//-------------------------------------------------------------------------------------------------------------
VOID Map::SyncRemoveBigVisTileUnitAndGroundItem2Role(Role* pRole, tagVisTile *pVisTileDec[ED_End])
{
	TList<DWORD> listRemovedUnit;
	TList<INT64> listGroundItem;
	
	// 遍历九宫格列表
	for(INT n = 0;  n < ED_End; n++)
	{
		if( !P_VALID(pVisTileDec[n]) )
			continue;

		// 该可视地砖要被删除，则遍历该可视地砖中的人物列表
		TMap<DWORD, Role*>& mapRole = pVisTileDec[n]->mapRole;
		mapRole.ResetIterator();
		Role* pRemoteRole = NULL;

		while( mapRole.PeekNext(pRemoteRole) )
		{
			// 将要删除的玩家加入到列表中
			if( !pRemoteRole->IsInStateInvisible() )
				listRemovedUnit.PushBack(pRemoteRole->GetID());
			else if( pRole->IsInVisList(pRemoteRole->GetID()) )
			{
				listRemovedUnit.PushBack(pRemoteRole->GetID());
				pRole->RemoveFromVisList(pRemoteRole->GetID());
			}
		}

		// 遍历该可视地砖中的非玩家列表
		TMap<DWORD, Creature*>& mapCreature = pVisTileDec[n]->mapCreature;
		mapCreature.ResetIterator();
		Creature* pCreature = NULL;

		while( mapCreature.PeekNext(pCreature) )
		{
			// 将要删除的非玩家加入到列表中
			if( !pCreature->IsInStateInvisible() )
				listRemovedUnit.PushBack(pCreature->GetID());
			else if( pRole->IsInVisList(pCreature->GetID()) )
			{
				listRemovedUnit.PushBack(pCreature->GetID());
				pRole->RemoveFromVisList(pCreature->GetID());
			}
		}

		//遍历该可视地砖中的GroundItem列表
		TMap<INT64, tagGroundItem*>& mapGroundItem = pVisTileDec[n]->mapGroundItem;
		mapGroundItem.ResetIterator();
		tagGroundItem* pGroundItem = NULL;
		while (mapGroundItem.PeekNext(pGroundItem))
		{
			//将要同步的GroundItem加入列表
			if( pGroundItem->IsValid())
			{
				listGroundItem.PushBack(pGroundItem->n64ID);
			}
		}
	}

	// 如果list不为空，且self是玩家的话，则发送给该玩家要删除的远程玩家
	INT nListSize = listRemovedUnit.Size();

	if( nListSize > 0 )
	{
		DWORD dwSize = sizeof(tagNS_RemoveRemote) + (nListSize-1)*sizeof(DWORD);
		MCREATE_MSG(pSend, dwSize, NS_RemoveRemote);

		for(INT n = 0; n < nListSize; n++)
		{
			pSend->dwRoleID[n] = listRemovedUnit.PopFront();
		}

		pRole->GetSession()->SendMessage(pSend, pSend->dwSize);

		MDEL_MSG(pSend);
	}

	//如果list不为空,且self是玩家的话，则一次将GroundItem发送给该玩家要删除的GroundItem
	INT nSize = listGroundItem.Size();
	if (nSize > 0)
	{
		DWORD dwSize = sizeof(tagNS_RoleGroundItemDisappear) + (nSize - 1)*sizeof(INT64);
		MCREATE_MSG(pSend, dwSize, NS_RoleGroundItemDisappear);

		for (INT n = 0; n < nSize; n++)
		{
			pSend->n64Serial[n] = listGroundItem.PopFront();
		}

		pRole->GetSession()->SendMessage(pSend, pSend->dwSize);

		MDEL_MSG(pSend);
	}
}

//-------------------------------------------------------------------------------------------------------------
// 通知各个远程玩家删除该对象，并且如果Self是玩家的话，将这些远程玩家从自己本地客户端删除
//-------------------------------------------------------------------------------------------------------------
VOID Map::SyncRemoveUnitToBigVisTile(Unit* pSelf)
{
	INT nVisIndex = WorldPosToVisIndex(pSelf->GetMoveData().m_vPos);

	tagVisTile* pVisTile[ED_End] = {0};
	GetVisTile(nVisIndex, pVisTile);

	SyncRemovedUnits(pSelf, pVisTile);
}

//-------------------------------------------------------------------------------------------------------
// 同步加入到地砖中的玩家（动作）
//-------------------------------------------------------------------------------------------------------
VOID Map::SyncAddedUnits(Unit* pSelf, tagVisTile* pVisTileAdd[ED_End])
{
	ASSERT( P_VALID(pSelf) );

	BOOL bSelfInvisible = pSelf->IsInStateInvisible();

	BYTE byMsg[1024] = {0};
	DWORD dwSize = CalMovementMsg(pSelf, byMsg, 1024);
	if(dwSize == 0)
		return;

	// 将A同步给格子内玩家
	if( !pSelf->IsInStateInvisible() )	// A可见
	{
		SyncVisUnit2BigVisTileRole(pSelf, pVisTileAdd, byMsg, dwSize);
	}
	else	// A不可见
	{
		SyncInvisUnit2BigVisTileRole(pSelf, pVisTileAdd, byMsg, dwSize);
	}

	// 若A为玩家，则将格子内信息同步给他
	if( pSelf->IsRole() )
	{
		Role *pRole = static_cast<Role*>(pSelf);

		// 格子内非隐身单位及掉落
		SyncBigVisTileVisUnitAndGroundItem2Role(pRole, pVisTileAdd);

		// 格子内隐身单位
		SyncBigVisTileInvisUnit2Role(pRole, pVisTileAdd);
	}
}

//----------------------------------------------------------------------------
// 计算两个玩家是否在同一个vistile里
//----------------------------------------------------------------------------
BOOL Map::InSameBigVisTile(Unit* pSelf, Unit* pRemote)
{
	if( !P_VALID(pSelf) || !P_VALID(pRemote) )
	{
		return FALSE;
	}

	INT nSelfVisIndex = pSelf->GetVisTileIndex();
	INT nRemoteVisIndex = pRemote->GetVisTileIndex();

	if( GT_INVALID == nSelfVisIndex || GT_INVALID == nRemoteVisIndex )
	{
		return FALSE;
	}

	INT nMinus = abs(nSelfVisIndex - nRemoteVisIndex);

	if( ( nMinus >= m_nVisTileArrayWidth - 1 && nMinus <= m_nVisTileArrayWidth + 1 )
		|| 0 == nMinus
		|| 1 == nMinus
		)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

//----------------------------------------------------------------------------
// 通过人物的当前动作计算发送给客户端的同步消息
//----------------------------------------------------------------------------
DWORD Map::CalMovementMsg(Unit* pSelf, LPBYTE pMsg, DWORD dwSize)
{
	ASSERT( P_VALID(pSelf) && P_VALID(pMsg) );

	// 根据人物当前移动状态发送同步命令
	EMoveState eCurMove = pSelf->GetMoveState();
	DWORD dwRealSize = 0;

	switch (eCurMove)
	{
		// 站立
	case EMS_Stand:
		{
			tagNS_SyncStand* pSend = (tagNS_SyncStand*)pMsg;
			pSend->dwID = m_pUtil->Crc32("NS_SyncStand");
			pSend->dwSize = sizeof(tagNS_SyncStand);
			pSend->dwRoleID = pSelf->GetID();
			pSend->curPos = pSelf->GetCurPos();
			pSend->faceTo = pSelf->GetFaceTo();
			pSend->bSwim = false;

			dwRealSize = pSend->dwSize;
		}
		break;

		// 行走
	case EMS_Walk:
		{
			ASSERT( pSelf->IsRole() );

			tagNS_SyncWalk* pSend = (tagNS_SyncWalk*)pMsg;
			pSend->dwID = m_pUtil->Crc32("NS_SyncWalk");
			pSend->dwSize = sizeof(tagNS_SyncWalk);
			pSend->dwRoleID = pSelf->GetID();
			pSend->srcPos = pSelf->GetStartPos();
			pSend->dstPos = pSelf->GetDestPos();
			pSend->curTime = pSelf->GetMovePassTime();
			pSend->fXZSpeed = pSelf->GetXZSpeed();
			pSend->bCollide = true;
			pSend->bSwim = false;

			dwRealSize = pSend->dwSize;
		}
		break;

		// 跳跃
	case EMS_Jump:
		{
			ASSERT( pSelf->IsRole() );

			tagNS_SyncJump* pSend = (tagNS_SyncJump*)pMsg;
			pSend->dwID = m_pUtil->Crc32("NS_SyncJump");
			pSend->dwSize = sizeof(tagNS_SyncJump);
			pSend->dwRoleID = pSelf->GetID();
			pSend->srcPos = pSelf->GetStartPos();
			pSend->dir = pSelf->GetMoveDir();
			pSend->curTime = pSelf->GetMovePassTime();
			pSend->fXZSpeed = pSelf->GetXZSpeed();
			pSend->fYSpeed = pSelf->GetYSpeed();

			dwRealSize = pSend->dwSize;
		}
		break;

		// 掉落
	case EMS_Drop:
		{
			ASSERT( pSelf->IsRole() );

			tagNS_SyncDrop* pSend = (tagNS_SyncDrop*)pMsg;
			pSend->dwID = m_pUtil->Crc32("NS_SyncDrop");
			pSend->dwSize = sizeof(tagNS_SyncDrop);
			pSend->dwRoleID = pSelf->GetID();
			pSend->srcPos = pSelf->GetStartPos();
			pSend->dir = pSelf->GetMoveDir();
			pSend->curTime = pSelf->GetMovePassTime();
			pSend->fXZSpeed = pSelf->GetDropXZSpeed();

			dwRealSize = pSend->dwSize;
		}
		break;

		// 垂直掉落
	case EMS_VDrop:
		{
			ASSERT( pSelf->IsRole() );

			tagNS_SyncVDrop* pSend = (tagNS_SyncVDrop*)pMsg;
			pSend->dwID = m_pUtil->Crc32("NS_SyncVDrop");
			pSend->dwSize = sizeof(tagNS_SyncVDrop);
			pSend->dwRoleID = pSelf->GetID();
			pSend->srcPos = pSelf->GetStartPos();
			pSend->curTime = pSelf->GetMovePassTime();

			dwRealSize = pSend->dwSize;
		}
		break;

		// 滑落
	case EMS_Slide:
		{
			ASSERT( pSelf->IsRole() );

			tagNS_SyncSlide* pSend = (tagNS_SyncSlide*)pMsg;
			pSend->dwID = m_pUtil->Crc32("NS_SyncSlide");
			pSend->dwSize = sizeof(tagNS_SyncSlide);
			pSend->dwRoleID = pSelf->GetID();
			pSend->srcPos = pSelf->GetStartPos();
			pSend->curTime = pSelf->GetMovePassTime();
			pSend->fXZSpeed = pSelf->GetSlideSpeed();

			dwRealSize = pSend->dwSize;
		}
		break;

		// 游泳
	case EMS_Swim:
		{
			ASSERT( pSelf->IsRole() );
			Role* pRole = static_cast<Role*>(pSelf);

			tagNS_SyncWalk* pSend = (tagNS_SyncWalk*)pMsg;
			pSend->dwID = m_pUtil->Crc32("NS_SyncWalk");
			pSend->dwSize = sizeof(tagNS_SyncWalk);
			pSend->dwRoleID = pSelf->GetID();
			pSend->srcPos = pSelf->GetStartPos();
			pSend->dstPos = pSelf->GetDestPos();
			pSend->curTime = pSelf->GetMovePassTime();
			pSend->fXZSpeed = ( pRole->IsInRoleState(ERS_WaterWalk) ? pSelf->GetXZSpeed() : pSelf->GetSwimXZSpeed() );
			pSend->bCollide = true;
			pSend->bSwim = true;

			dwRealSize = pSend->dwSize;
		}
		break;

		// 漂浮
	case EMS_SwimStand:
		{
			ASSERT( pSelf->IsRole() );

			tagNS_SyncStand* pSend = (tagNS_SyncStand*)pMsg;
			pSend->dwID = m_pUtil->Crc32("NS_SyncStand");
			pSend->dwSize = sizeof(tagNS_SyncStand);
			pSend->dwRoleID = pSelf->GetID();
			pSend->curPos = pSelf->GetCurPos();
			pSend->faceTo = pSelf->GetFaceTo();
			pSend->bSwim = true;

			dwRealSize = pSend->dwSize;
		}
		break;

		// 怪物巡逻
	case EMS_CreaturePatrol:
		{
			ASSERT( pSelf->IsCreature() );

			tagNS_SyncWalk* pSend = (tagNS_SyncWalk*)pMsg;
			pSend->dwID = m_pUtil->Crc32("NS_SyncWalk");
			pSend->dwSize = sizeof(tagNS_SyncWalk);
			pSend->dwRoleID = pSelf->GetID();
			pSend->srcPos = pSelf->GetStartPos();
			pSend->dstPos = pSelf->GetDestPos();
			pSend->curTime = pSelf->GetMovePassTime();
			pSend->fXZSpeed = pSelf->GetXZSpeed() * 0.4f;	// 巡逻速度
			pSend->bCollide = static_cast<Creature*>(pSelf)->NeedCollide();

			dwRealSize = pSend->dwSize;
		}
		break;

		// 怪物行走
	case EMS_CreatureWalk:
		{
			ASSERT( pSelf->IsCreature() );

			tagNS_SyncWalk* pSend = (tagNS_SyncWalk*)pMsg;
			pSend->dwID = m_pUtil->Crc32("NS_SyncWalk");
			pSend->dwSize = sizeof(tagNS_SyncWalk);
			pSend->dwRoleID = pSelf->GetID();
			pSend->srcPos = pSelf->GetStartPos();
			pSend->dstPos = pSelf->GetDestPos();
			pSend->curTime = pSelf->GetMovePassTime();
			pSend->fXZSpeed = pSelf->GetXZSpeed();
			pSend->bCollide = static_cast<Creature*>(pSelf)->NeedCollide();

			dwRealSize = pSend->dwSize;
		}
		break;

		// 怪物逃跑
	case EMS_CreatureFlee:
		{
			ASSERT( pSelf->IsCreature() );

			tagNS_SyncWalk* pSend = (tagNS_SyncWalk*)pMsg;
			pSend->dwID = m_pUtil->Crc32("NS_SyncWalk");
			pSend->dwSize = sizeof(tagNS_SyncWalk);
			pSend->dwRoleID = pSelf->GetID();
			pSend->srcPos = pSelf->GetStartPos();
			pSend->dstPos = pSelf->GetDestPos();
			pSend->curTime = pSelf->GetMovePassTime();
			pSend->fXZSpeed = pSelf->GetXZSpeed() * 0.7;
			pSend->bCollide = static_cast<Creature*>(pSelf)->NeedCollide();

			dwRealSize = pSend->dwSize;
		}

	default:
		break;
	}

	if( dwRealSize > dwSize )
		dwRealSize = 0;

	return dwRealSize;
}

//------------------------------------------------------------------------------
// 检测玩家所在位置对应的区域
//------------------------------------------------------------------------------
DWORD Map::CheckArea(Role* pRole)
{
	if( !P_VALID(pRole) ) return 0;
	if( !P_VALID(m_pInfo) ) return 0;

	DWORD dwRet = 0;

	// 得到玩家的包裹盒和格子坐标
	AABBox roleBox = pRole->GetAABBox();
	INT nTileX = INT(pRole->GetCurPos().x / (FLOAT)TILE_SCALE);
	INT nTileZ = INT(pRole->GetCurPos().z / (FLOAT)TILE_SCALE);

	tagMapAreaInfo* pArea = NULL;

	// 首先检测安全区
	pArea = IsInArea(m_pInfo->mapSafeArea, roleBox, nTileX, nTileZ);
	if( P_VALID(pArea) ) dwRet |= ERS_SafeArea;

	// 再检测PVP区
	pArea = IsInArea(m_pInfo->mapPVPArea, roleBox, nTileX, nTileZ);
	if( P_VALID(pArea) ) dwRet |= ERS_PVPArea;

	// 再检测摆摊区
	pArea = IsInArea(m_pInfo->mapStallArea, roleBox, nTileX, nTileZ);
	if( P_VALID(pArea) ) dwRet |= ERS_StallArea;

	// 再检牢狱区
	pArea = IsInArea(m_pInfo->mapPrisonArea, roleBox, nTileX, nTileZ);
	if( P_VALID(pArea) ) dwRet |= ERS_PrisonArea;

	// 检测脚本区
	pArea = IsInArea(m_pInfo->mapScriptArea, roleBox, nTileX, nTileZ);
	if( P_VALID(pArea) ) OnEnterArea(pRole, pArea);

	// 再检测
	return dwRet;
}

//------------------------------------------------------------------------------
// 检测玩家是不是在某个触发器范围内
//------------------------------------------------------------------------------
BOOL Map::IsInTrigger(Role* pRole, DWORD dwMapTriggerID)
{
	if( !P_VALID(pRole) ) return FALSE;
	if( !P_VALID(m_pInfo) ) return FALSE;

	tagMapTriggerInfo* pTrigger = m_pInfo->mapTrigger.Peek(dwMapTriggerID);
	if( !P_VALID(pTrigger) ) return FALSE;

	// 得到玩家的包裹盒和格子坐标
	AABBox roleBox = pRole->GetAABBox();
	INT nTileX = INT(pRole->GetCurPos().x / (FLOAT)TILE_SCALE);
	INT nTileZ = INT(pRole->GetCurPos().z / (FLOAT)TILE_SCALE);

	if( !pTrigger->box.Intersects(roleBox) ) return FALSE;

	if( !PtInRegion(pTrigger->hRgn, nTileX, nTileZ) ) return FALSE;

	return TRUE;
}

//------------------------------------------------------------------------------
// 得到某个trigger对应的任务序列号
//------------------------------------------------------------------------------
DWORD Map::GetTriggerQuestSerial(DWORD dwMapTriggerID)
{
	if( !P_VALID(m_pInfo) ) return GT_INVALID;

	tagMapTriggerInfo* pTrigger = m_pInfo->mapTrigger.Peek(dwMapTriggerID);
	if( !P_VALID(pTrigger) ) return GT_INVALID;

	if( pTrigger->eType != EMT_Script ) return GT_INVALID;

	return pTrigger->dwQuestSerial;
}

//------------------------------------------------------------------------------
// 检测某个包裹盒和格子坐标是否在某个区域列表中的一个区域里
//------------------------------------------------------------------------------
tagMapAreaInfo* Map::IsInArea(tagMapInfo::MapAreaMap& mapArea, AABBox& roleBox, INT nTileX, INT nTileZ)
{
	tagMapInfo::MapAreaMap::TMapIterator it = mapArea.Begin();
	tagMapAreaInfo* pArea = NULL;

	while( mapArea.PeekNext(it, pArea) )
	{
		// 跟包裹盒都不相交
		if( !pArea->box.Intersects(roleBox) ) continue;

		// 包裹盒相交，则检测格子坐标在不在区域里面
		if( PtInRegion(pArea->hRgn, nTileX, nTileZ) )
			return pArea;
	}

	return NULL;
}

//------------------------------------------------------------------------------
// 将玩家添加到登出列表中，由上层来控制玩家的实际登出和保存
//------------------------------------------------------------------------------
VOID Map::RoleLogOut(DWORD dwID)
{
	Role* pRole = m_mapRole.Peek(dwID);
	if( P_VALID(pRole) )
	{
		// 离开地图
		RoleLeaveMap(pRole->GetID(), FALSE);
	}
}

//-------------------------------------------------------------------------------
// 玩家离开地图，只可能在主线程里面调用
//-------------------------------------------------------------------------------
VOID Map::RoleLeaveMap( DWORD dwID, BOOL bChgMap )
{
	Role* pRole = m_mapRole.Peek(dwID);
	if( P_VALID(pRole) )
	{
		//处理离开地图前的相关事项
		pRole->OnLeaveMap(bChgMap);

		// 调用脚本
		if( P_VALID(m_pScript) )
		{
			m_pScript->OnPlayerLeave(pRole, this);
		}

		// 同步周围玩家删除自己
		SyncRoleLeaveMap(pRole);

		// 将玩家从自己的列表中删除
		m_mapRole.Erase(dwID);
		pRole->SetMap(NULL);

		// 如果玩家session有效，则现在就从列表中删除
		if( pRole->GetSession() )
		{
			m_mapSession.Erase(pRole->GetSession()->GetSessionID());
		}
	}
}

//-------------------------------------------------------------------------------
// 在地图中根据某个ID查找玩家或者生物（慎用，一般要在该地图线程之内使用）
//-------------------------------------------------------------------------------
Unit* Map::FindUnit(DWORD dwID)
{
	if( IS_PLAYER(dwID) )
		return m_mapRole.Peek(dwID);
	else if( IS_CREATURE(dwID) )
		return m_mapCreature.Peek(dwID);
	else if( IS_PET(dwID) )
		return m_mapCreature.Peek(dwID);
	else
		return NULL;
}

//-------------------------------------------------------------------------------
// 创建并初始化NPC上所挂商店
//-------------------------------------------------------------------------------
BOOL Map::AddShop(DWORD dwNPCID, DWORD dwShopID)
{
	if(m_mapShop.IsExist(dwNPCID))
	{
		return TRUE;
	}

	Shop* pShop = Shop::Create(dwNPCID, dwShopID);
	if(!P_VALID(pShop))
	{
		ASSERT(P_VALID(pShop));
		IMSG(_T("Create shop<npc_typid:%u, shop_id:%u> failed!\n"), dwNPCID, dwShopID);
		return FALSE;
	}

	m_mapShop.Add(dwNPCID, pShop);

	return TRUE;
}

//-------------------------------------------------------------------------------
// 创建并初始化NPC上所挂商会
//-------------------------------------------------------------------------------
BOOL Map::AddCofC(DWORD dwNPCID, DWORD dwCofCID)
{
	if(m_mapCofC.IsExist(dwNPCID))
	{
		return TRUE;
	}

	GuildCofC* pCofC = GuildCofC::Create(dwNPCID, dwCofCID);
	if(!P_VALID(pCofC))
	{
		ASSERT(P_VALID(pCofC));
		IMSG(_T("Create CofC<npc_typid:%u, CofC_id:%u> failed!\n"), dwNPCID, dwCofCID);
		return FALSE;
	}

	m_mapCofC.Add(dwNPCID, pCofC);

	return TRUE;
}

//-------------------------------------------------------------------------------
// 玩家离开地图同步
//-------------------------------------------------------------------------------
VOID Map::SyncRoleLeaveMap(Role* pSelf)
{
	if( !P_VALID(pSelf) ) return;

	INT nVisTileIndex = pSelf->GetVisTileIndex();

	// 首先从VisTile中移除
	RemoveFromVisTile(pSelf);

	// 得到九宫格的有玩家
	tagVisTile* pVisTile[ED_End] = {0};
	GetVisTile(nVisTileIndex, pVisTile);

	// 发送移除同步命令
	SyncRemovedUnits(pSelf, pVisTile);
}

//----------------------------------------------------------------------------------
// 发送进入新地图给客户端
//----------------------------------------------------------------------------------
VOID Map::SendGotoNewMapToPlayer(Role* pSelf)
{
	if( !P_VALID(pSelf) || !P_VALID(pSelf->GetSession()) )
		return;

	tagNS_GotoNewMap send;
	send.dwMapID = pSelf->GetMapID();
	send.pos = pSelf->GetCurPos();
	send.faceTo = pSelf->GetFaceTo();

	pSelf->GetSession()->SendMessage(&send, send.dwSize);
}

//----------------------------------------------------------------------------------
// 怪物掉落物品是否到地上 --- 地面物品系统 
//----------------------------------------------------------------------------------
BOOL Map::CanPutDown(Creature* pCreature, INT nIndex, Vector3& vPos)
{
	if( !P_VALID(pCreature) )
		return FALSE;

	// 根据怪物算物品坐标
	const Vector3& vCreaturePos = pCreature->GetCurPos();

	// 得到格子序号
	INT nIndexX = INT(vCreaturePos.x / TILE_SCALE);
	INT nIndexZ = INT(vCreaturePos.z / TILE_SCALE);

	// 找出一个可行走区域
	if( FALSE == GetCanGoPosFromIndex(nIndex, nIndexX, nIndexZ, 2 * (nIndex / 8 + 1), vPos) )
	{
		return FALSE;
	}

	return TRUE;
}

//----------------------------------------------------------------------------------
// 怪物掉落物品到地上 --- 地面物品系统 
//----------------------------------------------------------------------------------
VOID Map::PutDownItem(Creature* pCreature, tagItem* pLootItem, DWORD dwOwnerID, DWORD dwTeamID, Vector3& vPos)
{
	if( !P_VALID(pCreature) || !P_VALID(pLootItem) )
		return;

	// 取怪物头顶高度
	vPos.y = pCreature->GetCurPosTop().y;

	// 生成掉落物品
	tagGroundItem* pGroundItem = new tagGroundItem(GenGroundItemID(), pLootItem->dwTypeID, 
														pLootItem->n16Num, pLootItem, vPos,
														dwOwnerID, dwTeamID, 0, pCreature->GetID());
	ASSERT(pGroundItem->IsValid());

	// 在地面上添加物品
	AddGroundItem(pGroundItem);
}

//----------------------------------------------------------------------------------
// 怪物掉落金钱到地上 --- 地面物品系统 
//----------------------------------------------------------------------------------
VOID Map::PutDownMoney(Creature* pCreature, INT nMoney, Role* pRole, Vector3 vPos)
{
	if( !P_VALID(pCreature) || !P_VALID(pRole) )
		return;

	DWORD dwTeamID = pRole->GetTeamID();
	DWORD dwOwnerID = GT_VALID(dwTeamID) ? GT_INVALID : pRole->GetID();

	// 取怪物头顶高度
	vPos.y = pCreature->GetCurPosTop().y;

	// 生成地面物品
	tagGroundItem* pGroundItem = new tagGroundItem(GenGroundItemID(), TYPE_ID_MONEY, nMoney, NULL,
													vPos, dwOwnerID, dwTeamID, 0, pCreature->GetID());
	ASSERT(pGroundItem->IsValid());

	// 往地面上增加物品
	AddGroundItem(pGroundItem);
}

BOOL Map::GetCanGoPosFromIndex(INT32 nIndex, INT32 nIndexX, INT32 nIndexZ, INT32 n, Vector3& vPos)
{
	INT index = nIndex % 8;
	switch (index)
	{
	case 0:
		nIndexX -= n;
		nIndexZ -= n;
		break;
	case 1:
		nIndexZ -= n;
		break;
	case 2:
		nIndexX += n;
		nIndexZ -= n;
		break;
	case 3:
		nIndexX += n;
		break;
	case 4:
		nIndexX += n;
		nIndexZ += n;
		break;
	case 5:
		nIndexZ += n;
		break;
	case 6:
		nIndexX -= n;
		nIndexZ += n;
		break;
	case 7:
		nIndexX -= n;
		break;
	}

	vPos.x = (FLOAT)(nIndexX * TILE_SCALE);
	vPos.z = (FLOAT)(nIndexZ * TILE_SCALE);

	if( nIndexX < 0 || nIndexX > m_pInfo->nWidth - 1 ||
		nIndexZ < 0 || nIndexZ > m_pInfo->nHeight - 1 )
	{
		return FALSE;
	}

	return TRUE;
}

//-----------------------------------------------------------------------------
// 往地面上增加物品 --- 地面物品系统
//-----------------------------------------------------------------------------
VOID Map::AddGroundItem(tagGroundItem* pGroundItem)
{
	if( !P_VALID(pGroundItem) ) return;
	if( !pGroundItem->IsValid() ) return;

	// 修正一下坐标
	AABBox box;
	FLOAT fHalfX = vGroundItemSize.x / 2.0f;
	FLOAT fHalfZ = vGroundItemSize.z / 2.0f;
	box.min.x = pGroundItem->vPos.x - fHalfX;
	box.min.z = pGroundItem->vPos.z - fHalfZ;
	box.min.y = pGroundItem->vPos.y;
	box.max.x = pGroundItem->vPos.x + fHalfX;
	box.max.z = pGroundItem->vPos.z + fHalfZ;
	box.max.y = pGroundItem->vPos.y + vGroundItemSize.y;

	pGroundItem->vPos.y = GetGroundAndModelHeight(box);

	// 如果在水面的话，则掉落到水面
	FLOAT fY = 0.0f;
	if( GetNavCollider()->IfWillOnWater(pGroundItem->vPos, fY) )
	{
		pGroundItem->vPos.y = fY;
	}

	// 加入到m_mapGroundItem中
	m_mapGroundItem.Add(pGroundItem->n64ID, pGroundItem);

	// 计算物品所在的可视地砖格子
	INT nVisIndex = WorldPosToVisIndex(pGroundItem->vPos);

	// 得到九宫格
	tagVisTile* pVisTile[ED_End] = {0};
	GetVisTile(nVisIndex, pVisTile);

	// 同步给加入到客户端的玩家和生物
	SyncAddGroundItem(pGroundItem, pVisTile);

	// 让生物落到9宫格之内
	AddGroundItemToVisTile(pGroundItem, nVisIndex);

}
//-----------------------------------------------------------------------------
// 从地面上移除物品 --- 地面物品系统
//-----------------------------------------------------------------------------
VOID Map::RemoveGroundItem(tagGroundItem *pGroundItem)
{
	if(!P_VALID(pGroundItem))
		return;

	if (!pGroundItem->IsValid())
		return;

	//从m_mapGroundItem 中移除
	m_mapGroundItem.Erase(pGroundItem->n64ID);
	//计算物品所在的可视地砖格子
	INT nVisIndex = WorldPosToVisIndex(pGroundItem->vPos);

	// 得到九宫格
	tagVisTile* pVisTile[ED_End] = {0};
	GetVisTile(nVisIndex, pVisTile);

	// 同步给加入到客户端的玩家和生物
	SyncRemoveGroundItem(pGroundItem, pVisTile);

	// 让生物落到9宫格之内
	RemoveGroundItemFromVisTile(pGroundItem, nVisIndex);

}

//-----------------------------------------------------------------------------
// 同步加入到地砖中的物品 --- 地面物品系统
//-----------------------------------------------------------------------------
VOID Map::SyncAddGroundItem(tagGroundItem *pGroundItem, tagVisTile *pVisTileAdd[ED_End])
{
	tagNS_SyncGroundItem send;

	send.n64Serial	= pGroundItem->n64ID;
	send.dwTypeID	= pGroundItem->dwTypeID;
	send.nNum		= pGroundItem->nNum;

	send.dwPutDownUnitID = pGroundItem->dwPutDownUnitID;
	send.dwOwnerID = pGroundItem->dwOwnerID;
	send.dwTeamID = pGroundItem->dwTeamID;
	send.dwGroupID = pGroundItem->dwGroupID;
	send.vPos = pGroundItem->vPos;
	send.bKilled = TRUE;
	
	// 向可视地砖的人发送同步命令
	SendBigVisTileMsg(pVisTileAdd, &send, send.dwSize);
}

//-----------------------------------------------------------------------------
// 同步加入到地砖中的物品 --- 地面物品系统
//-----------------------------------------------------------------------------
VOID Map::SyncGroundItemState(tagGroundItem *pGroundItem, tagVisTile *pVisTileAdd[ED_End])
{
	tagNS_SyncGroundItem send;

	send.n64Serial	= pGroundItem->n64ID;
	send.dwTypeID	= pGroundItem->dwTypeID;
	send.nNum		= pGroundItem->nNum;

	send.dwPutDownUnitID = pGroundItem->dwPutDownUnitID;
	send.dwOwnerID = pGroundItem->dwOwnerID;
	send.dwTeamID = pGroundItem->dwTeamID;
	send.dwGroupID = pGroundItem->dwGroupID;
	send.vPos = pGroundItem->vPos;
	send.bKilled = FALSE;

	// 向可视地砖的人发送同步命令
	SendBigVisTileMsg(pVisTileAdd, &send, send.dwSize);
}


//-----------------------------------------------------------------------------
// 同步移除地砖中的物品 --- 地面物品系统
//-----------------------------------------------------------------------------
VOID Map::SyncRemoveGroundItem(tagGroundItem *pGroundItem, tagVisTile *pVisTileAdd[ED_End])
{
	if(!pGroundItem->IsValid())
		return;

	//发送物品消失消息
	tagNS_RoleGroundItemDisappear disappear;
	disappear.n64Serial[0] = pGroundItem->n64ID;

	// 向可视地砖的人发送同步命令
	SendBigVisTileMsg(pVisTileAdd, &disappear, disappear.dwSize);
}

//-----------------------------------------------------------------------------
// 发送地图场景特效信息	---	玩家进入场景调用
//-----------------------------------------------------------------------------
VOID Map::SendSceneEffect(Role* pSelf)
{
	if (!P_VALID(pSelf))	return;

	// 向该客户端发送所有已经激活的场景特效
	m_Mutex.Acquire();
	DWORD dwObjID = GT_INVALID;
	TList<DWORD>::TListIterator iter = m_listSceneEffect.Begin();
	while (m_listSceneEffect.PeekNext(iter, dwObjID))
	{
		if (!GT_VALID(dwObjID))
			continue;

		tagNS_PlaySceneEffect send;
		send.eType		= ESET_ByObjID;
		send.dwObjID	= dwObjID;
		pSelf->SendMessage(&send, send.dwSize);
	}
	m_Mutex.Release();
}

//-----------------------------------------------------------------------------
// 开启场景特效	--- 脚本调用
//-----------------------------------------------------------------------------
VOID Map::PlaySceneEffect(ESceneEffectType eType, DWORD dwObjID, Vector3 vPos, DWORD dwEffectID)
{
	// 检查场景特效属性
	if (!P_VALID(m_pInfo))	return;
	if (!P_VALID(m_pInfo->mapTriggerEffect.Peek(dwObjID)))
		return;

	// 设置特效激活
	m_Mutex.Acquire();
	if (!m_listSceneEffect.IsExist(dwObjID))
		m_listSceneEffect.PushBack(dwObjID);
	m_Mutex.Release();

	// 通知地图中玩家开启场景特效(允许客户端激活多次)
	tagNS_PlaySceneEffect send;
	send.eType		= eType;
	send.dwObjID	= dwObjID;
	send.vPos		= vPos;
	send.dwEffectID	= dwEffectID;
	SendMapMessage(&send, send.dwSize);
}

//-----------------------------------------------------------------------------
// 关闭场景特效	--- 脚本调用
//-----------------------------------------------------------------------------
VOID Map::StopSceneEffect(DWORD dwObjID)
{
	// 检查是否已经激活
	m_Mutex.Acquire();
	if (!m_listSceneEffect.IsExist(dwObjID))
	{
		m_Mutex.Release();
		return;
	}
	else
	{
		m_listSceneEffect.Erase(dwObjID);
	}
	m_Mutex.Release();

	// 通知地图中所有玩家
	tagNS_StopSceneEffect send;
	send.dwObjID	= dwObjID;
	SendMapMessage(&send, send.dwSize);
}

//-----------------------------------------------------------------------------------------
// 加入物品到地图中的某个VisTile --- 地面物品系统
//-----------------------------------------------------------------------------------------
VOID Map::AddGroundItemToVisTile(tagGroundItem* pGroundItem, INT nVisIndex)
{
	m_pVisTile[nVisIndex].mapGroundItem.Add(pGroundItem->n64ID, pGroundItem);
}

//-----------------------------------------------------------------------------------------
// 移除地图中某个VisTile的物品 --- 地面物品系统
//-----------------------------------------------------------------------------------------
VOID Map::RemoveGroundItemFromVisTile(tagGroundItem* pGroundItem, INT nVisIndex)
{
	m_pVisTile[nVisIndex].mapGroundItem.Erase(pGroundItem->n64ID);
}

//-----------------------------------------------------------------------------------------
// 刷新点刷怪
//-----------------------------------------------------------------------------------------
VOID Map::SSpawnPtReplaceCreature( Creature* pDeadCreature )
{
	ASSERT_P_VALID(pDeadCreature);

	// 需要用到的数据
	DWORD	dwReuseID		= pDeadCreature->GetID();
	DWORD	dwSSpawnPtID	= pDeadCreature->GetSpawnPtID();
	Vector3	vPos			= pDeadCreature->GetBornPos();
	Vector3 vFase			= pDeadCreature->GetBornFace();
	BOOL	bCollide		= pDeadCreature->NeedCollide();

	// 找个新的
	const tagSSpawnPointProto* pSSpawnProto = g_attRes.GetSSpawnPointProto(dwSSpawnPtID);
	ASSERT_P_VALID( pSSpawnProto );
	if (!P_VALID(pSSpawnProto))
		return ;

	INT nCandiNum	= 0;
	while (GT_VALID(pSSpawnProto->dwTypeIDs[nCandiNum]))
		nCandiNum++;
	INT nIndex = IUTIL->Rand() % nCandiNum;
	DWORD dwNewTypeID = pSSpawnProto->dwTypeIDs[nIndex];
	ASSERT( GT_VALID(dwNewTypeID) );
	if (!P_VALID(dwNewTypeID))
		return ;
	const tagCreatureProto* pCreatureProto = g_attRes.GetCreatureProto(dwNewTypeID);


	// 删除旧的
	m_mapRespawnCreature.Erase(pDeadCreature->GetID());
	Creature::Delete(pDeadCreature);

	// 生成新的
	pDeadCreature = Creature::Create(dwReuseID, GetMapID(), pCreatureProto, vPos, vFase, ECST_SpawnPoint, dwSSpawnPtID, bCollide, NULL);

	// 添加
	AddCreature(pDeadCreature);
}

//-----------------------------------------------------------------------------------------
// 动态创建怪物
//-----------------------------------------------------------------------------------------
Creature* Map::CreateCreature(DWORD dwTypeID, Vector3& vPos, Vector3& vFaceTo, DWORD dwSpawnerID, BOOL bCollide, CHAR* pPatrolListName)
{
	Creature *pCreature = (Creature*)GT_INVALID;

	const tagCreatureProto* pProto = g_attRes.GetCreatureProto(dwTypeID);
	if( !P_VALID(pProto) )
	{
		IMSG(_T("Detect a unknown creature in map, dwTypeID=%u\r\n"), dwTypeID);
		return pCreature;
	}

	if (MoveData::IsPosInvalid(vPos) || MoveData::IsFaceInvalid(vFaceTo))
	{
		return pCreature;
	}

	if( !IsPosValid(vPos) )
	{
		IMSG(_T("creature is out of map, dwTypeID=%u\r\n"), dwTypeID);
		return pCreature;
	}

	tagMapWayPointInfoList* patrolList = NULL;
	if(P_VALID(pPatrolListName))
		patrolList = m_pInfo->mapWayPointList.Peek(m_pUtil->Crc32(pPatrolListName));

	DWORD dwID = m_CreatureIDGen.GetNewCreatureID();

	pCreature = Creature::Create(dwID, m_dwID, pProto, vPos, vFaceTo, ECST_Dynamic, dwSpawnerID, bCollide, patrolList);

	if(P_VALID(pCreature))
		AddCreature(pCreature);

	return pCreature;
}

//-----------------------------------------------------------------------------------------
// 删除怪物
//-----------------------------------------------------------------------------------------
VOID Map::DeleteCreature(DWORD dwID)
{
	Creature* pCreature = FindCreature(dwID);
	if (!P_VALID(pCreature))	return;

	// 从生物列表中拿走这个怪物
	m_mapCreature.Erase(dwID);

	// 从可视地砖中拿走，但不同步客户端
	RemoveFromVisTile(pCreature);

	// 同步给客户端
	SyncRemoveUnitToBigVisTile(pCreature);

	Creature::Delete(pCreature);
}

//------------------------------------------------------------------------------------------
// 潜行
//------------------------------------------------------------------------------------------
VOID Map::Lurk(Unit *pUnit)
{
	ASSERT(P_VALID(pUnit));

	INT nVisIndex = pUnit->GetVisTileIndex();
	ASSERT(GT_VALID(nVisIndex));

	// 加入格子隐身单位列表
	m_pVisTile[nVisIndex].mapInvisibleUnit.Add(pUnit->GetID(), pUnit);

	// 同步给可视列表内玩家
	tagNS_RemoveRemote sendRemove;
	sendRemove.dwRoleID[0] = pUnit->GetID();

	tagVisTile* pVisTile[ED_End] = {0};
	GetVisTile(nVisIndex, pVisTile);
	
	for(INT n = 0; n < ED_End; n++)
	{
		if( NULL == pVisTile[n] )
			continue;

		// 找到每个地砖的人
		TMap<DWORD, Role*>& mapRole = pVisTile[n]->mapRole;
		mapRole.ResetIterator();
		Role* pRole = NULL;

		while( mapRole.PeekNext(pRole) )
		{
			if( !P_VALID(pRole) )
				continue;

			// 1.是否完全可见
			if( pRole->CanSeeTargetEntirely(pUnit) )
			{
				continue;
			}

			ASSERT(!pRole->IsInVisList(pUnit->GetID()));

			// 2.B是否有探隐能力，且是否在可视范围内
			if( !pRole->HasDectiveSkill() || !pRole->IsInVisDist(pUnit) )
			{
				pRole->SendMessage(&sendRemove, sendRemove.dwSize);
				continue;
			}

			// 3 在B可视范围内
			pRole->Add2VisList(pUnit->GetID());
		}
	}
}

//------------------------------------------------------------------------------------------
// 潜行状态消失 -- 显形
//------------------------------------------------------------------------------------------
VOID Map::UnLurk(Unit *pUnit)
{
	ASSERT(P_VALID(pUnit));

	INT nVisIndex = pUnit->GetVisTileIndex();
	ASSERT(GT_VALID(nVisIndex));

	// 从格子隐身单位列表中去除
	m_pVisTile[nVisIndex].mapInvisibleUnit.Erase(pUnit->GetID());

	// 同步给可视列表内玩家
	BYTE	byMsg[1024] = {0};
	DWORD	dwSize = CalMovementMsg(pUnit, byMsg, 1024);

	tagVisTile* pVisTile[ED_End] = {0};
	GetVisTile(nVisIndex, pVisTile);

	for(INT n = 0; n < ED_End; n++)
	{
		if( NULL == pVisTile[n] )
			continue;

		// 找到每个地砖的人
		TMap<DWORD, Role*>& mapRole = pVisTile[n]->mapRole;
		mapRole.ResetIterator();
		Role* pRole = NULL;

		while( mapRole.PeekNext(pRole) )
		{
			if( !P_VALID(pRole) )
				continue;

			// 1.是否完全可见
			if( pRole->CanSeeTargetEntirely(pUnit) )
			{
				continue;
			}

			// 2.B是否有探隐能力，且是否在可视范围内
			if( !pRole->HasDectiveSkill() || !pRole->IsInVisDist(pUnit) )
			{
				pRole->SendMessage(byMsg, dwSize);
				continue;
			}

			// 3 在B可视范围内
			pRole->RemoveFromVisList(pUnit->GetID());
		}
	}
}

//------------------------------------------------------------------------------------------
// 潜行单位移动过程中同步给可视范围内玩家
//------------------------------------------------------------------------------------------
VOID Map::UpdateLurk2BigVisTileRole(Unit *pUnit)
{
	ASSERT(pUnit->IsInStateInvisible());

	INT nVisIndex = pUnit->GetVisTileIndex();
	ASSERT(GT_VALID(nVisIndex));

	tagVisTile* pVisTile[ED_End] = {0};
	GetVisTile(nVisIndex, pVisTile);

	tagNS_RemoveRemote sendRemove;
	sendRemove.dwRoleID[0] = pUnit->GetID();

	BYTE	byMsg[1024] = {0};
	DWORD	dwSize = CalMovementMsg(pUnit, byMsg, 1024);

	for(INT n = 0; n < ED_End; n++)
	{
		if( NULL == pVisTile[n] )
			continue;

		// 找到每个地砖的人
		TMap<DWORD, Role*>& mapRole = pVisTile[n]->mapRole;
		mapRole.ResetIterator();
		Role* pRole = NULL;

		while( mapRole.PeekNext(pRole) )
		{
			if( !P_VALID(pRole) )
				continue;

			// 1.是否完全可见
			if( pRole->CanSeeTargetEntirely(pUnit) )
			{
				continue;
			}

			// 2.B是否有探隐能力
			if( !pRole->HasDectiveSkill() )
				continue;

			// 3.在B的可视列表中
			if( pRole->IsInVisList(pUnit->GetID()) )
			{
				// 不在B可视范围内
				if( !pRole->IsInVisDist(pUnit) )
				{
					pRole->SendMessage(&sendRemove, sendRemove.dwSize);
					pRole->RemoveFromVisList(pUnit->GetID());
				}

				continue;
			}

			// 4.不在B的可视列表中
			// 4.1不在B可视范围内
			if( !pRole->IsInVisDist(pUnit) )
				continue;

			// 4.2 在B可视范围内
			pRole->SendMessage(byMsg, dwSize);
			pRole->Add2VisList(pUnit->GetID());
		}
	}
}

//---------------------------------------------------------------------------------------------------------------------
// 将周围潜行单位同步给该玩家
//---------------------------------------------------------------------------------------------------------------------
VOID Map::UpdateBigVisTileLurkUnit2Role(Role* pRole)
{
	INT nVisIndex = pRole->GetVisTileIndex();
	ASSERT(GT_VALID(nVisIndex));

	tagVisTile* pVisTile[ED_End] = {0};
	GetVisTile(nVisIndex, pVisTile);

	SyncBigVisTileInvisUnit2Role(pRole, pVisTile);
}

FLOAT Map::GetExpRate()
{
	return g_GMPolicy.GetExpRate();
}

FLOAT Map::GetLootRate()
{
	return g_GMPolicy.GetLootRate(GetMapType() == EMT_Normal);
}

Pet* Map::FindPet( DWORD dwID ) /* ?般在地图线程之内使用 */
{
	Creature* pCreature = m_mapCreature.Peek(dwID); 
	if (P_VALID(pCreature))
	{
		return dynamic_cast<Pet*>(pCreature);
	}
	else
	{
		return NULL;
	}
}
