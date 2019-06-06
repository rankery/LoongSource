//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: map.h
// author: Aslan
// actor:
// data: 2008-6-27
// last:
// brief: 地图类，管理一个确定的地图实例，副本为一个独立的地图实例
//-----------------------------------------------------------------------------
#pragma once

#include "../WorldDefine/time.h"
#include "../WorldDefine/msg_map.h"
#include "../WorldDefine/msg_combat.h"
#include "../WorldDefine/ItemDefine.h"

#include "script_mgr.h"

class PlayerSession;
class Unit;
class Role;
class Creature;
class Pet;
class Map;
class Shop;
class MapScript;
class NPCTeamMgr;
class GuildCofC;

struct tagItem;
struct tagEquip;

//-------------------------------------------------------------------------------
// 默认副本最大数量
//-------------------------------------------------------------------------------
const DWORD DEFAULT_MAX_INSTANCE_NUM	=	500;


//-------------------------------------------------------------------------------
// 用于取站立高度的最大高度和最小高度
//-------------------------------------------------------------------------------
const FLOAT GET_HEIGHT_MAX_Y = 10000000.0f;
const FLOAT GET_HEIGHT_MIN_Y = -10000000.0f;

const INT CHECK_GROUND_ITEM_TICK_INTERVAL1M = 1 * 60 * TICK_PER_SECOND; // 1分钟
const INT CHECK_GROUND_ITEM_TICK_INTERVAL3M = 3 * 60 * TICK_PER_SECOND; // 3分钟

//--------------------------------------------------------------------------------
// 副本级别，用于控制不同级别副本的生成数量
//--------------------------------------------------------------------------------
enum EMapInstanceRank
{
	EMIR_NoLimit	=	0,				// 无限制
	EMIR_512		=	1,				// 512x512及其以下的副本
	EMIR_1024		=	2,				// 1024x1024的副本
	EMIR_2048		=	3,				// 2048x2048的副本
	EMIR_3072		=	4,				// 3072x3072及以上的副本

	EMIR_End		=	5,
};

const INT INSTANCE_COEF[EMIR_End] = {0, 1, 3, 6, 9};	// 每个副本占用比重的权值

//--------------------------------------------------------------------------------
// 服务器端地图导航点结构
//--------------------------------------------------------------------------------
struct tagWayPointInfo
{
	Vector3		vPos;
	Vector3		vRange;

public:
	tagWayPointInfo(DWORD)
	{
		vPos.x = vPos.y = vPos.z = -1.0;
	}
	tagWayPointInfo()
	{

	}
};

struct tagMapWayPointInfoList
{
	DWORD							dwID;					// 导航点ID
	mutable TList<tagWayPointInfo>	list;					// 导航点列表

	tagMapWayPointInfoList() { dwID = GT_INVALID; }
};

//--------------------------------------------------------------------------------
// 服务器端地图触发器结构
//--------------------------------------------------------------------------------
struct tagMapTriggerInfo
{
	DWORD 				dwObjID;					// 地图属性ID
	EMapTrigger			eType;						// 类型
	HRGN				hRgn;						// 多边形区域句柄
	AABBox				box;						// 包围盒
	FLOAT				fHeight;					// 高度
	DWORD				dwMapID;					// 切换地图触发器的目标地图ID
	DWORD				dwWayPointID;				// 切换地图触发器的目标地图导航点ID
	DWORD				dwParam;					// 参数
	DWORD				dwQuestSerial;				// 针对于任务的序号

	tagMapTriggerInfo()
	{
		dwObjID = GT_INVALID;
		eType = EMT_Null;
		fHeight = 0.0f;
		dwMapID = GT_INVALID;
		dwWayPointID = GT_INVALID;
		dwParam = GT_INVALID;
		dwQuestSerial = GT_INVALID;
	}
	~tagMapTriggerInfo()
	{
		DeleteObject(hRgn);
	}
};

//--------------------------------------------------------------------------------
// 服务器端地图区域结构
//--------------------------------------------------------------------------------
struct tagMapAreaInfo
{
	DWORD 				dwObjID;			// 地图属性
	EMapArea			eType;				// 类型
	HRGN				hRgn;				// 多边形区域句柄
	AABBox				box;				// 包围盒
	FLOAT				fHeight;			// 高度

	tagMapAreaInfo()
	{
		dwObjID = GT_INVALID;
		eType = EMA_Null;
		fHeight = 0.0f;
	}
	~tagMapAreaInfo()
	{
		DeleteObject(hRgn);
	}
};

//--------------------------------------------------------------------------------
// 怪物及NPC的生成结构
//--------------------------------------------------------------------------------
struct tagMapCreatureInfo
{
	DWORD 					dwObjID;			// 地图属性ID
	DWORD 					dwTypeID;			// 属性ID
	Vector3					vPos;				// 初始坐标
	FLOAT 					fYaw;				// 初始朝向
	tagMapWayPointInfoList*	patrolList;			// 巡逻的导航点列表
	BOOL					bCollide;			// 是否使用碰撞

	tagMapCreatureInfo() { ZeroMemory(this, sizeof(*this)); }
};

//--------------------------------------------------------------------------------
// 服务器端刷怪点结构
//--------------------------------------------------------------------------------
struct tagMapSpawnPointInfo
{
	DWORD				dwObjID;				// 地图属性id
	DWORD				dwSpawnPointID;			// 刷怪点id（小ID）
	INT					nLevelInc;				// 随机副本刷挂时的等级增加量
	BOOL				bCollide;				// 是否使用碰撞
	Vector3				vPos;					// 坐标

	tagMapSpawnPointInfo() { ZeroMemory(this, sizeof(*this)); }
};


//--------------------------------------------------------------------------------
// 地图静态属性
//--------------------------------------------------------------------------------
struct tagMapInfo
{
	typedef TMap<DWORD, tagMapWayPointInfoList*>	MapWayPointListMap;		// 场景导航点
	typedef TMap<DWORD, tagMapTriggerInfo*>			MapTriggerMap;			// 场景触发器
	typedef TMap<DWORD,	tagMapAreaInfo*>			MapAreaMap;				// 场景区域
	typedef TMap<DWORD, tagMapCreatureInfo*>		MapCreatureInfoMap;		// 场景生物
	typedef TMap<DWORD, tagMapSpawnPointInfo*>		MapSpawnPointMap;		// 场景刷怪点
	typedef TMap<DWORD, tagMapTriggerEffect*>		MapTriggerEffectMap;	// 场景特效
	typedef TMap<DWORD, tagMapDoor*>				MapDoorMap;				// 门

	DWORD					dwID;							// 地图ID
	TCHAR					szMapFileName[X_SHORT_NAME];	// 地图文件名称

	EMapType				eType;							// 地图类型
	ENormalMapType			eNoarmalMapType;				// 普通地图的类型
	EMapInstanceRank		eInstanceRank;					// 副本级别
	DWORD					dwMaxNumLimit;					// 副本最大数量限制
	
	INT						nWidth;							// 长度
	INT						nHeight;						// 宽度
	INT						nVisDist;						// 可视距离
	DWORD					dwWeatherID;					// 对应的天气属性ID
	Vector3					vRebornPos;						// 复活点
	DWORD					dwPopMapID;						// 从场景中传送出的地图ID
	Vector3					vPopPos;						// 从场景中传送出的地图坐标

	mutable MapWayPointListMap		mapWayPointList;		// 场景导航点
	mutable MapTriggerMap			mapTrigger;				// 场景触发器
	mutable MapAreaMap				mapSafeArea;			// 安全区
	mutable MapAreaMap				mapPVPArea;				// PVP区
	mutable MapAreaMap				mapStallArea;			// 摆摊区
	mutable MapAreaMap				mapPrisonArea;			// 牢狱区
	mutable MapAreaMap				mapScriptArea;			// 脚本区域
	mutable MapCreatureInfoMap		mapCreatureInfo;		// 场景触发器
	mutable MapSpawnPointMap		mapSpawnPoint;			// 创景刷怪点
	mutable MapTriggerEffectMap		mapTriggerEffect;		// 场景特效
	mutable	MapDoorMap				mapDoor;				// 门
};

//------------------------------------------------------------------------
// 地图内 地物ID 生成器
//------------------------------------------------------------------------
class GroundItemIDGen
{
public:
	GroundItemIDGen():m_n64LastValid(1){}
	INT64 GenValidID()
	{
		return m_n64LastValid++;
	}
private:
	INT64		m_n64LastValid;
};

enum EGroundItemUpdate
{
	EGIU_Null		= -1,
	EGIU_Remove		= 0,	// 移除
	EGIU_Sync		= 1,	// 同步
};

//------------------------------------------------------------------------
// 掉到地面上的物品结构
//------------------------------------------------------------------------
struct tagGroundItem
{
	INT64					n64ID;				// 地物ID
	DWORD					dwTypeID;			// 类型ID
	INT						nNum;				// 数量或金钱

	tagItem*				pItem;				// 物品指针
	DWORD					dwPutDownUnitID;	// 掉落物品的UnitID
	DWORD					dwOwnerID;			// 归属角色
	DWORD					dwTeamID;			// 归属队伍
	DWORD					dwGroupID;			// 归属团队ID
	INT						nTickCountDown;		// 掉落物品的tick倒计时
	Vector3					vPos;				// 物品坐标


	tagGroundItem(	INT64 n64ID_, DWORD dwTypeID_, INT nNum_, tagItem* pItem_, 
					Vector3 vPos_, DWORD dwOwnerID_, DWORD dwTeamID_, DWORD dwGroupID_, DWORD dwPutDownUnitID_)
	{
		n64ID			=	n64ID_;
		dwTypeID		=	dwTypeID_;
		nNum			=	nNum_;

		pItem			=	pItem_;
		vPos			=	vPos_;
		dwOwnerID		=	dwOwnerID_;
		dwTeamID		=	dwTeamID_;
		dwGroupID		=	dwGroupID_;
		dwPutDownUnitID	=	dwPutDownUnitID_;
		nTickCountDown	=	0;
	}


	// 地物更新，返回TRUE时删除地物
	BOOL Update()
	{
		EGroundItemUpdate eRtv = EGIU_Null;
		// 更新时钟
		++nTickCountDown;

		// 1分钟任意拾取
		if (CHECK_GROUND_ITEM_TICK_INTERVAL1M == nTickCountDown)
		{
			if (GT_VALID(dwOwnerID) || GT_VALID(dwTeamID))
				eRtv = EGIU_Null;
			else
				eRtv = EGIU_Sync;

			dwOwnerID	= GT_INVALID;
			dwTeamID	= GT_INVALID;
		}
		// 3分钟消失
		else if (nTickCountDown >= CHECK_GROUND_ITEM_TICK_INTERVAL3M)
		{
			eRtv = EGIU_Remove;
		}
		return eRtv;
	}

	// 清理
	VOID DestroyItem()
	{
		if (P_VALID(pItem))
		{
			::Destroy(pItem);
		}
	}

	// 是否有效
	BOOL IsValid()
	{
		if (dwTypeID == TYPE_ID_MONEY && !P_VALID(pItem))
		{
			return TRUE;
		}
		else if (dwTypeID != TYPE_ID_MONEY && P_VALID(pItem))
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}

};

//----------------------------------------------------------------------------------
// 可视地砖
//----------------------------------------------------------------------------------

// 定义中间加上八方向
enum EDirection
{
	ED_Center = 0,
	ED_Top = 1,
	ED_Bottom = 2,
	ED_Left = 3,
	ED_Right = 4,
	ED_LeftTop = 5,
	ED_LeftBottom = 6,
	ED_RightTop = 7,
	ED_RightBottom = 8,
	ED_End = 9,
};

struct tagVisTile
{
	TMap<DWORD, Role*>				mapRole;
	TMap<DWORD, Creature*>			mapCreature;
	TMap<INT64, tagGroundItem*>		mapGroundItem;
	TMap<DWORD, Unit*>				mapInvisibleUnit;
};

//---------------------------------------------------------------------------------
// 生物ID生成器
//---------------------------------------------------------------------------------
class CreatureIDGen
{
public:
	VOID Init(const tagMapInfo* pInfo)
	{
		// 考虑到巢穴刷怪以及实时刷出的怪物，在这个基础之上再增加100个ID
		INT nCreatureNum = pInfo->mapCreatureInfo.Size() + pInfo->mapSpawnPoint.Size() + 100;

		DWORD dwMinID = (DWORD)MIN_CREATURE_ID;
		for(INT n = 0; n < nCreatureNum; n++)
		{
			m_listFreeCreatureID.PushBack(dwMinID+(DWORD)n);
		}
		m_dwMaxCreatureID = dwMinID + nCreatureNum;
	}

	DWORD GetNewCreatureID()
	{
		DWORD dwCreatureID = m_listFreeCreatureID.PopFront();

		if( !P_VALID(dwCreatureID) )
		{
			// list中已经没有多余的id了，则再生成100个
			for(INT n = 1; n < 100; n++)
			{
				m_listFreeCreatureID.PushBack(m_dwMaxCreatureID+(DWORD)n);
			}
			dwCreatureID = m_dwMaxCreatureID;
			m_dwMaxCreatureID += 100;
		}

		return dwCreatureID;
	}

	DWORD ReturnCreatureID(DWORD dwCreatureID)
	{
		ASSERT( IS_CREATURE(dwCreatureID) );
		m_listFreeCreatureID.PushFront(dwCreatureID);
	}

private:
	TList<DWORD>		m_listFreeCreatureID;
	DWORD				m_dwMaxCreatureID;
};

//---------------------------------------------------------------------------------
// 门ID生成器
//---------------------------------------------------------------------------------
class DoorIDGen
{
public:
	DoorIDGen():m_dwDoorIDValid(MIN_DOOR_ID){}
	DWORD GetValidDoorID()
	{
		return m_dwDoorIDValid++;
	}

private:
	DWORD m_dwDoorIDValid;
};

//---------------------------------------------------------------------------------------
// 地图类
//---------------------------------------------------------------------------------------
class Map : public ScriptData<ESD_Map>
{
public:
	typedef	TMap<DWORD, PlayerSession*>		SessionMap;
	typedef TMap<DWORD, Role*>				RoleMap;
	typedef TMap<DWORD, Creature*>			CreatureMap;
	typedef TMap<INT64, tagGroundItem*>		GroundItemMap;
	typedef TMap<DWORD, Shop*>				ShopMap;
	typedef TMap<DWORD, GuildCofC*>			CofCMap;

public:
	Map();
	virtual ~Map();

	//-------------------------------------------------------------------------------------
	// 创建，更新和销毁
	//-------------------------------------------------------------------------------------
	BOOL				Init(const tagMapInfo* pInfo);
	VOID				Update();
	VOID				Destroy();

	//-------------------------------------------------------------------------------------
	// 各种Get
	//-------------------------------------------------------------------------------------
	DWORD				GetMapID()			{ return m_dwID; }
	DWORD				GetInstanceID()		{ return m_dwInstanceID; }
	EMapType			GetMapType()		{ return m_pInfo->eType; }
	ENormalMapType		GetNormalMapType()	{ return m_pInfo->eNoarmalMapType; }
	NavMap*				GetNavMap()			{ return m_pNav; }
	const tagMapInfo*	GetMapInfo()		{ return m_pInfo; }
	INT					GetRoleNum()		{ return m_mapRole.Size(); }
	NPCTeamMgr*			GetTeamMgr()		{ return m_pTeamMgr; }
	RoleMap&			GetRoleMap()		{ return m_mapRole; }

	//------------------------------------------------------------------------------------------
	// 双倍
	//------------------------------------------------------------------------------------------
	FLOAT				GetExpRate() ;
	FLOAT				GetLootRate();

	//--------------------------------------------------------------------------------------
	// 可视地砖相关
	//--------------------------------------------------------------------------------------
	VOID				GetVisTile(INT nVisIndex, tagVisTile* visTile[ED_End]);
	VOID				GetVisTile(Vector3& pos, FLOAT fRange, tagVisTile* visTile[ED_End]);
	VOID				GetVisTileAdd(INT nOldVisIndex, INT nNewVisIndex, tagVisTile* pVisTile[ED_End]);
	VOID				GetVisTileDec(INT nOldVisIndex, INT nNewVisIndex, tagVisTile* pVisTile[ED_End]);
	VOID				GetVisTileAddAndDec(INT nOldVisIndex, INT nNewVisIndex, tagVisTile* pVisTileAdd[ED_End], tagVisTile* pVisTileDec[ED_End]);

	//--------------------------------------------------------------------------------------
	// 玩家，生物和掉落物品的添加和删除
	//--------------------------------------------------------------------------------------
	virtual VOID		AddRole(Role* pRole);
	virtual INT			CanEnter(Role *pRole)			{ return TRUE; }
	VOID				RoleLogOut(DWORD dwID);
	virtual VOID		RoleLeaveMap(DWORD dwID, BOOL bChgMap);
	VOID				AddCreatureOnLoad(Creature* pCreature);
	VOID				AddCreature(Creature* pCreature);
	VOID				RemoveCreature(Creature* pCreature);
	VOID				RemovePet(Pet* pPet);
	VOID				AddGroundItem(tagGroundItem *pGroundItem);
	VOID				RemoveGroundItem(tagGroundItem* pGroundItem);
	VOID 				SSpawnPtReplaceCreature(Creature* pDeadCreature);
	Creature*			CreateCreature(DWORD dwTypeID, Vector3& vPos, Vector3& vFaceTo, DWORD dwSpawnerID=GT_INVALID, BOOL bCollide=FALSE, CHAR* pPatrolListName=NULL);
	VOID				DeleteCreature(DWORD dwID);

	//---------------------------------------------------------------------------------------
	// 游戏内玩家和生物的查询
	//---------------------------------------------------------------------------------------
	Role*				FindRole(DWORD dwID)			{ return m_mapRole.Peek(dwID); }		// 一般在地图线程之内使用
	Creature*			FindCreature(DWORD dwID)		{ return m_mapCreature.Peek(dwID); }	// 一般在地图线程之内使用
	Pet*				FindPet(DWORD dwID);				// 一般在地图线程之内使用;	
	Unit*				FindUnit(DWORD dwID);
	tagGroundItem*		GetGroundItem(INT64 n64Serial)	{ return m_mapGroundItem.Peek(n64Serial); }
	Shop*				GetShop(DWORD dwNPCID)			{ return m_mapShop.Peek(dwNPCID); }
	GuildCofC*			GetCofC(DWORD dwNPCID)			{ return m_mapCofC.Peek(dwNPCID); }

	//--------------------------------------------------------------------------------------
	// 基于地图九宫格的各种同步方法
	//--------------------------------------------------------------------------------------
	VOID				SyncMovementToBigVisTile(Unit* pSelf);
	VOID				SyncRemoveUnitToBigVisTile(Unit* pSelf);
	VOID				SyncChangeVisTile(Unit* pSelf, INT nOldVisIndex, INT nNewVisIndex);
	VOID				SyncRoleLeaveMap(Role* pSelf);
	VOID				SendBigVisTileMsg(Unit* pSelf, const LPVOID pMsg, const DWORD dwSize);
	VOID				SendBigVisTileMsg(tagVisTile *pVisTile[ED_End], const LPVOID pMsg, const DWORD dwSize);
	VOID				SendMapMessage(const LPVOID pMsg, const DWORD dwSize);
	VOID				SyncRemovedUnits(Unit* pSelf, tagVisTile* pVisTileDec[ED_End]);
	VOID				SyncAddedUnits(Unit* pSelf, tagVisTile* pVisTileAdd[ED_End]);
	VOID				SyncAddGroundItem(tagGroundItem *pGroundItem, tagVisTile *pVisTileAdd[ED_End]);
	VOID				SyncGroundItemState(tagGroundItem *pGroundItem, tagVisTile *pVisTileAdd[ED_End]);
	VOID				SyncRemoveGroundItem(tagGroundItem *pGroundItem, tagVisTile *pVisTileAdd[ED_End]);

	//--------------------------------------------------------------------------------------
	// 地图信息的同步
	//--------------------------------------------------------------------------------------
	VOID				SendSceneEffect(Role* pSelf);
	VOID				PlaySceneEffect(ESceneEffectType eType, DWORD dwObjID, Vector3 vPos, DWORD dwEffectID);
	VOID				StopSceneEffect(DWORD dwObjID);

	//----------------------------------------------------------------------------------------
	// 地图区域相关
	//----------------------------------------------------------------------------------------
	DWORD				CheckArea(Role* pRole);							// 检测人物所在的各种区域（返回区域标志位）

	//----------------------------------------------------------------------------------------
	// 坐标检测及修正
	//----------------------------------------------------------------------------------------
	BOOL				IsPosValid(const Vector3& vVec);
	VOID				FixPos(Vector3& vVec);

	//----------------------------------------------------------------------------------------
	// 地图场景触发器相关
	//----------------------------------------------------------------------------------------
	BOOL				IsInTrigger(Role* pRole, DWORD dwMapTriggerID);	// 是否在触发器内
	DWORD				GetTriggerQuestSerial(DWORD dwMapTriggerID);	// 得到某个trigger对应的任务序列号
	
	//----------------------------------------------------------------------------------------
	// 掉落相关的一些函数
	//----------------------------------------------------------------------------------------
	BOOL				CanPutDown(Creature* pCreature, INT nIndex, Vector3& vPos);
	VOID				PutDownItem(Creature* pCreature, tagItem* pLootItem, DWORD dwOwnerID, DWORD dwTeamID, Vector3& vPos);
	VOID				PutDownMoney(Creature* pCreature, INT nMoney, Role* pRole, Vector3 vPos);
	BOOL				GetCanGoPosFromIndex(INT32 nIndex, INT32 nIndexX, INT32 nIndexZ, INT32 n, Vector3& vPos);
	INT64				GenGroundItemID()		{ return m_GroundItemIDGen.GenValidID(); }

	//----------------------------------------------------------------------------------------
	// 地图战斗系统限制相关，主要应用于副本
	//----------------------------------------------------------------------------------------
	BOOL				CanUseItem(DWORD dwTypeID);
	BOOL				CanUseSkill(DWORD dwItemID);

	//----------------------------------------------------------------------------------------
	// 敌我关系判断
	//----------------------------------------------------------------------------------------
	DWORD				GetFriendEnemyFlag(Unit* pSrc, Unit* pTarget, BOOL& bIgnore);

	//------------------------------------------------------------------------------------------
	// 地图区域相关
	//------------------------------------------------------------------------------------------
	tagMapAreaInfo*		IsInArea(tagMapInfo::MapAreaMap& mapArea, AABBox& roleBox, INT nTileX, INT nTileZ);

	//----------------------------------------------------------------------------------------
	// 一些辅助函数
	//----------------------------------------------------------------------------------------
	VOID				SendGotoNewMapToPlayer(Role* pSelf);
	BOOL				InSameBigVisTile(Unit* pSelf, Unit* pRemote);
	INT					WorldPosToVisIndex(const Vector3& pos);
	BOOL				IfCanGo(FLOAT fX, FLOAT fZ);
	BOOL				IfCanDirectGo(FLOAT fSrcX, FLOAT fSrcY, FLOAT fDestX, FLOAT fDestZ, POINT* pNearPos=NULL);
	BOOL				IfWillSwim(Vector3& vPos, FLOAT fHeight, FLOAT& fY);
	BOOL				IfWillOnWater(Vector3& vPos, FLOAT& fY);
	BOOL				IsRayCollide(const Vector3& vPos1, const Vector3& vPos2, DWORD dwFlag = 0);
	FLOAT				GetGroundHeight(FLOAT fX, FLOAT fZ);
	FLOAT				GetGroundAndModelHeight(AABBox& box);
	NavCollider*        GetNavCollider();

	VOID				OpenCloseDoor(DWORD dwObjID, BOOL bOpen);
	VOID				OpenCloseAllDoors(BOOL bOpen);

	template<typename UnitOperation>
	VOID				ForEachUnitInBigVisTile(Unit* pUnit, UnitOperation oper);
	template<typename UnitOperation>
	VOID				ForEachUnitInMap(UnitOperation oper);

	//-----------------------------------------------------------------------------------------
	// 地图事件
	//-----------------------------------------------------------------------------------------
	VOID				OnRoleDie(Role* pRole, Unit* pKiller);
	VOID				OnRevive(Role* pRole, ERoleReviveType eType, INT &nReviveHP, INT &nReviveMP, FLOAT &fx, FLOAT &fy, FLOAT &fz, DWORD &dwRebornMapID);
	VOID				OnCreatureDie(Creature* pCreature, Unit* pKiller);
	VOID				OnCreatureDisappear(Creature* pCreature);
	VOID				OnEnterTrigger(Role* pRole, tagMapTriggerInfo* pInfo);
	VOID				OnEnterArea(Role* pRole, tagMapAreaInfo* pInfo);
	INT		            CanInviteJoinTeam();
	INT 				CanLeaveTeam();
	INT					CanChangeLeader();
	INT					CanKickMember();
	BOOL				CanSetSafeguard();

	//------------------------------------------------------------------------------------------
	// 潜行
	//------------------------------------------------------------------------------------------
	VOID				Lurk(Unit *pUnit);
	VOID				UnLurk(Unit *pUnit);
	VOID				UpdateLurk2BigVisTileRole(Unit *pUnit);
	VOID				UpdateBigVisTileLurkUnit2Role(Role* pRole);

	
	//------------------------------------------------------------------------------------------
	// 接口
	//------------------------------------------------------------------------------------------
	DWORD				CalMovementMsgEx(Unit* pSelf, LPBYTE pMsg, DWORD dwSize)
	{
		return CalMovementMsg(pSelf, pMsg, dwSize);
	}
protected:
	//-----------------------------------------------------------------------------------------
	// 初始化
	//-----------------------------------------------------------------------------------------
	BOOL				InitLogicalInfo();						// 根据地图属性信息初始化所有逻辑信息
	BOOL				InitAllMapCreature();					// 生成地图里的初始怪物
	BOOL				InitAllFixedCreature();					// 生成地图里的所有摆放怪物
	virtual BOOL		InitAllSpawnPointCreature();			// 生成所有刷新点怪物
	BOOL				InitAllFixedDoor();						// 生成地图里的所有门
	VOID				InitNestCreature(Creature* pCreature);	// 初始化巢穴怪物
	VOID				InitTeamCreature(Creature* pCreature);  // 初始化小队怪物
	BOOL				AddShop(DWORD dwNPCID, DWORD dwShopID);	// 初始化商店
	BOOL				AddCofC(DWORD dwNPCID, DWORD dwCofCID);	// 初始化商会

	//------------------------------------------------------------------------------------------
	// 地图事件
	//------------------------------------------------------------------------------------------
	VOID				OnInit();								// 初始化时

	//------------------------------------------------------------------------------------------
	// 更新
	//------------------------------------------------------------------------------------------
	VOID				UpdateSession();			// 处理该地图所管理的玩家的所有消息
	VOID				UpdateAllObjects();			// 更新该地图内所有对象
	VOID				UpdateAllShops();			// 更新该地图内所有对象
	VOID				UpdateAllCofC();			// 更新该地图内所有对象

	//------------------------------------------------------------------------------------------
	// 九宫格相关
	//------------------------------------------------------------------------------------------
	VOID				AddToVisTile(Unit* pUnit, INT nVisIndex);
	VOID				RemoveFromVisTile(Unit* pUnit);
	VOID				AddGroundItemToVisTile(tagGroundItem* pGroundItem, INT nVisIndex);
	VOID				RemoveGroundItemFromVisTile(tagGroundItem* pGroundItem, INT nVisIndex);

	//------------------------------------------------------------------------------------------
	// 数据同步
	//------------------------------------------------------------------------------------------
	VOID				SyncBigVisTileInvisUnit2Role(Role* pRole, tagVisTile *pVisTile[ED_End]);
	VOID				SyncBigVisTileVisUnitAndGroundItem2Role(Role* pRole, tagVisTile *pVisTile[ED_End]);

	VOID				SyncInvisUnit2BigVisTileRole(Unit* pUnit, tagVisTile *pVisTile[ED_End], 
													const LPVOID pMsg, const DWORD dwSize);
	VOID				SyncVisUnit2BigVisTileRole(Unit* pUnit, tagVisTile *pVisTile[ED_End], 
													const LPVOID pMsg, const DWORD dwSize);

	VOID				SyncRemoveBigVisTileUnitAndGroundItem2Role(Role* pRole, tagVisTile *pVisTileDec[ED_End]);

	
	//------------------------------------------------------------------------------------------
	// 辅助函数
	//------------------------------------------------------------------------------------------
	DWORD				CalMovementMsg(Unit* pSelf, LPBYTE pMsg, DWORD dwSize);
protected:
	TObjRef<Util>			m_pUtil;

	DWORD					m_dwID;					// 地图ID
	DWORD					m_dwInstanceID;			// 副本ID（普通地图此ID为GT_INVALID）

	//-------------------------------------------------------------------------------------------
	// 对象容器
	//-------------------------------------------------------------------------------------------
	SessionMap				m_mapSession;			// 地图里面的session列表
	RoleMap					m_mapRole;				// 地图里面的所有玩家
	CreatureMap				m_mapCreature;			// 地图里面的所有生物
	CreatureMap				m_mapRespawnCreature;	// 死亡后等待刷新的生物
	GroundItemMap			m_mapGroundItem;		// 地面掉落物品
	ShopMap					m_mapShop;				// 地图里面的所有商店<dwNpcID, pShop>
	CofCMap					m_mapCofC;				// 帮派跑商商会

	TMap<DWORD, DWORD>		m_mapDoorObj2Gen;		// 门ObjID与生物ID映射表
	//-------------------------------------------------------------------------------------------
	// 九宫格
	//-------------------------------------------------------------------------------------------
	INT						m_nVisTileArrayWidth;	// 最大可视地砖宽度
	INT						m_nVisTileArrayHeight;	// 最大可视地砖长度
	tagVisTile*				m_pVisTile;				// 可视地砖，动态生成的数组

	//-------------------------------------------------------------------------------------------
	// 地图属性
	//-------------------------------------------------------------------------------------------
	const tagMapInfo*		m_pInfo;				// 地图属性信息（外部静态属性，不能删除）
	NavMap*					m_pNav;					// 导航图
	const MapScript*		m_pScript;				// 地图脚本

	//-------------------------------------------------------------------------------------------
	// 已触发场景特效列表
	//-------------------------------------------------------------------------------------------
	TList<DWORD>			m_listSceneEffect;		// 已触发场景特效ObjID列表
	Mutex					m_Mutex;				// 特效列表锁

	//-------------------------------------------------------------------------------------------
	// 怪物ID生成器
	//-------------------------------------------------------------------------------------------
	CreatureIDGen			m_CreatureIDGen;		// 生物ID生成器

	//-------------------------------------------------------------------------------------------
	// 地物ID生成器
	//-------------------------------------------------------------------------------------------
	GroundItemIDGen			m_GroundItemIDGen;		// 地物ID生成器

	//-------------------------------------------------------------------------------------------
	// 怪物小队管理器
	//-------------------------------------------------------------------------------------------
	NPCTeamMgr*				m_pTeamMgr;

	//-------------------------------------------------------------------------------------------
	// 门ID生成器
	//-------------------------------------------------------------------------------------------
	DoorIDGen				m_DoorIDGen;			// 门ID生成器
};

//-----------------------------------------------------------------------------------------------
// 副本基类
//-----------------------------------------------------------------------------------------------
class MapInstance : public Map
{
public:
	MapInstance() : m_bDelete(FALSE), m_bEnd(FALSE) {}
	virtual ~MapInstance() {}

	virtual BOOL	Init(const tagMapInfo* pInfo, DWORD dwInstanceID, Role* pCreator=NULL, DWORD dwMisc=GT_INVALID) = 0;
	virtual VOID	Update() = 0;
	virtual VOID	Destroy() = 0;

	virtual VOID	AddRole(Role* pRole) = 0;
	virtual	VOID	RoleLeaveMap(DWORD dwID, BOOL bChgMap) = 0;
	virtual INT		CanEnter(Role *pRole)		{ if( IsDelete() ) return E_Instance_End_Delete; else return E_Success; }
	virtual BOOL	CanDestroy()				{ return IsDelete() && m_mapRole.Empty(); }

	virtual VOID	OnDestroy() = 0;

	//---------------------------------------------------------------------------------------------
	// 副本结束和删除标志位
	//---------------------------------------------------------------------------------------------
	VOID			SetDelete()					{ if( IsDelete() ) return; InterlockedExchange((LPLONG)&m_bDelete, TRUE); OnDelete(); }
	VOID			SetEnd()					{ if( IsEnd() ) return; InterlockedExchange((LPLONG)&m_bEnd, TRUE); }
	BOOL			IsEnd()						{ return m_bEnd; }
	BOOL			IsDelete()					{ return m_bDelete; }

protected:
	virtual VOID	OnDelete() = 0;						// 副本设置删除标志时的处理，注意，并不是在副本析构时调用
	virtual BOOL	InitAllSpawnPointCreature() = 0;	// 生成所有刷新点怪物

protected:
	volatile BOOL			m_bDelete;					// 副本删除标志位
	volatile BOOL			m_bEnd;						// 副本结束标志位
};

//------------------------------------------------------------------------------------------------
// 某个点是否是否是可行走点
//------------------------------------------------------------------------------------------------
inline BOOL Map::IfCanGo(FLOAT fX, FLOAT fZ)
{
	return GetNavMap()->GetNPCNavMap()->IfCanGo(fX, fZ);
}

//---------------------------------------------------------------------------------------
// 某两个点之间是否可以通过
//---------------------------------------------------------------------------------------
inline BOOL Map::IfCanDirectGo(FLOAT fSrcX, FLOAT fSrcY, FLOAT fDestX, FLOAT fDestZ, POINT* pNearPos)
{
	return GetNavMap()->GetNPCNavMap()->IfCanDirectGo(fSrcX, fSrcY, fDestX, fDestZ, pNearPos);
}

//----------------------------------------------------------------------------------------
// 得到地面高度
//----------------------------------------------------------------------------------------
inline FLOAT Map::GetGroundHeight(FLOAT fX, FLOAT fZ)
{
	return GetNavMap()->GetNPCNavMap()->SafeGetHeight(fX, fZ);
}

//----------------------------------------------------------------------------------------
// 得到某个点是否要游泳
//----------------------------------------------------------------------------------------
inline BOOL Map::IfWillSwim(Vector3& vPos, FLOAT fHeight, FLOAT& fY)
{
	return GetNavMap()->GetCollider()->IfWillSwim(vPos, fHeight, fY);
}


//----------------------------------------------------------------------------------------
// 得到某个点是否在水面
//----------------------------------------------------------------------------------------
inline BOOL Map::IfWillOnWater(Vector3& vPos, FLOAT& fY)
{
	return GetNavMap()->GetCollider()->IfWillOnWater(vPos, fY);
}

//----------------------------------------------------------------------------------------
// 检测射线检测
//----------------------------------------------------------------------------------------
inline BOOL Map::IsRayCollide(const Vector3& vPos1, const Vector3& vPos2, DWORD dwFlag/* = 0*/)
{
	Ray ray;
	ray.origin = vPos1;
	ray.dir = vPos2 - vPos1;
	ray.length = Vec3Dist(vPos1, vPos2);
	Vec3Normalize(ray.dir);

	if( GetNavMap()->GetCollider()->RayCollideBoxAndTrn(ray, dwFlag) )
	{
		return TRUE;
	}

	return FALSE;
}

//----------------------------------------------------------------------------------------
// 得到地面和模型高度
//----------------------------------------------------------------------------------------
inline FLOAT Map::GetGroundAndModelHeight(AABBox& box)
{
	// 从Y指定的高度向下碰
	AABBox boxTemp = box;
	boxTemp.max.y = box.min.y + 10.0f;
	boxTemp.min.y = GET_HEIGHT_MIN_Y;

	FLOAT fIntersectY = 0.0f;
	GetNavMap()->GetCollider()->AABBoxCollideBoxTopAndTrn(boxTemp, 0.0f, fIntersectY);
	return fIntersectY;
}

//------------------------------------------------------------------------------------------------------------------
// 玩家死亡
//------------------------------------------------------------------------------------------------------------------
inline VOID Map::OnRoleDie(Role* pRole, Unit* pKiller)
{
	if( P_VALID(m_pScript) )
	{
		m_pScript->OnRoleDie(pRole, pKiller, this);
	}
}

//------------------------------------------------------------------------------------------------------------------
// 玩家是否可以开启PK保护
//------------------------------------------------------------------------------------------------------------------
inline BOOL Map::CanSetSafeguard()
{
	if( P_VALID(m_pScript) )
	{
		return m_pScript->CanSetSafeguard(this);
	}

	return TRUE;
}

//------------------------------------------------------------------------------------------------------------------
// 玩家是否可以使用物品
//------------------------------------------------------------------------------------------------------------------
inline BOOL Map::CanUseItem(DWORD dwTypeID)
{
	if( P_VALID(m_pScript) ) 
	{
		return m_pScript->CanUseItem(this, dwTypeID);
	}

	return TRUE;
}

//------------------------------------------------------------------------------------------------------------------
// 玩家是否可以使用技能
//------------------------------------------------------------------------------------------------------------------
inline BOOL Map::CanUseSkill(DWORD dwTypeID)
{
	if( P_VALID(m_pScript) )
	{
		return m_pScript->CanUseSkill(this, dwTypeID);
	}

	return TRUE;
}

//------------------------------------------------------------------------------------------------------------------
// 玩家复活
//------------------------------------------------------------------------------------------------------------------
inline VOID Map::OnRevive(Role* pRole, ERoleReviveType eType, INT &nReviveHP, INT &nReviveMP, FLOAT &fx, FLOAT &fy, FLOAT &fz, DWORD &dwRebornMapID)
{
	if( P_VALID(m_pScript) )
	{
		m_pScript->Revive(pRole, eType, nReviveHP, nReviveMP, fx, fy, fz, dwRebornMapID);
	}
}

//-------------------------------------------------------------------------------------------------------------------
// 怪物死亡
//-------------------------------------------------------------------------------------------------------------------
inline VOID Map::OnCreatureDie(Creature* pCreature, Unit* pKiller)
{
	if( P_VALID(m_pScript) )
	{
		m_pScript->OnCreatureDie(pCreature, pKiller, this);
	}
}

//-------------------------------------------------------------------------------------------------------------------
// 怪物消失
//-------------------------------------------------------------------------------------------------------------------
inline VOID Map::OnCreatureDisappear(Creature* pCreature)
{
	if( P_VALID(m_pScript) ) 
	{
		m_pScript->OnCreatureDisappear(pCreature, this);
	}
}

//--------------------------------------------------------------------------------------------------------------------
// 玩家进入触发器
//--------------------------------------------------------------------------------------------------------------------
inline VOID Map::OnEnterTrigger(Role* pRole, tagMapTriggerInfo* pInfo)
{
	if( P_VALID(m_pScript) )
	{
		m_pScript->OnEnterTrigger(pRole, pInfo, this);
	}
}

//---------------------------------------------------------------------------------------------------------------------
// 玩家进入区域
//---------------------------------------------------------------------------------------------------------------------
inline VOID Map::OnEnterArea(Role* pRole, tagMapAreaInfo* pInfo)
{
	if( P_VALID(m_pScript) )
	{
		m_pScript->OnEnterArea(pRole, pInfo, this);
	}
}

//---------------------------------------------------------------------------------------------------------------------
// 将可见单位同步给周围玩家
//---------------------------------------------------------------------------------------------------------------------
inline VOID Map::SyncVisUnit2BigVisTileRole(Unit* pUnit, tagVisTile *pVisTile[ED_End], 
											const LPVOID pMsg, const DWORD dwSize)
{
	SendBigVisTileMsg(pVisTile, pMsg, dwSize);
}

//---------------------------------------------------------------------------------------------------------------------
// 是否允许邀请组队
//---------------------------------------------------------------------------------------------------------------------
inline INT Map::CanInviteJoinTeam()
{
	if( P_VALID(m_pScript) )
	{
		return m_pScript->CanInviteJoinTeam(this);
	}

	return 0;
}

//---------------------------------------------------------------------------------------------------------------------
// 是否允许离开队伍
//---------------------------------------------------------------------------------------------------------------------
inline INT Map::CanLeaveTeam()
{
	if( P_VALID(m_pScript) )
	{
		return m_pScript->CanLeaveTeam(this);
	}

	return 0;
}

//---------------------------------------------------------------------------------------------------------------------
// 是否允许移交队长
//---------------------------------------------------------------------------------------------------------------------
inline INT Map::CanChangeLeader()
{
	if( P_VALID(m_pScript) )
	{
		return m_pScript->CanChangeLeader(this);
	}

	return 0;
}

//---------------------------------------------------------------------------------------------------------------------
// 是否可以踢掉玩家
//---------------------------------------------------------------------------------------------------------------------
inline INT Map::CanKickMember()
{
	if( P_VALID(m_pScript) ) 
	{
		return m_pScript->CanKickMember(this);
	}

	return 0;
}

//-----------------------------------------------------------------------------------------------------------------------
// 敌我关系判断
//-----------------------------------------------------------------------------------------------------------------------
inline DWORD Map::GetFriendEnemyFlag(Unit* pSrc, Unit* pTarget, BOOL& bIgnore)
{
	if( !P_VALID(m_pScript) )
	{
		bIgnore = FALSE;
		return 0;
	}

	return m_pScript->FriendEnemy(this, pSrc, pTarget, bIgnore);
}

//-----------------------------------------------------------------------------------------------------------------------
// 坐标非法
//-----------------------------------------------------------------------------------------------------------------------
inline BOOL Map::IsPosValid(const Vector3& vVec)
{
	// 检测是否超出边界
	if( vVec.x < 0.0f || vVec.x >= FLOAT(m_pInfo->nWidth * TILE_SCALE) ||
		vVec.z < 0.0f || vVec.z >= FLOAT(m_pInfo->nHeight * TILE_SCALE) )
		return FALSE;

	return TRUE;
}

//--------------------------------------------------------------------------------------------------------------------------
// 修正坐标
//--------------------------------------------------------------------------------------------------------------------------
inline VOID Map::FixPos(Vector3& vVec)
{
	// 修正范围
	if( vVec.x < 0.0f )				vVec.x = 0.0f;
	if( vVec.z < 0.0f )				vVec.z = 0.0f;
	if( vVec.x >= FLOAT(m_pInfo->nWidth * TILE_SCALE) )		vVec.x = FLOAT(m_pInfo->nWidth * TILE_SCALE) - 1.0f;
	if( vVec.z >= FLOAT(m_pInfo->nHeight * TILE_SCALE) )	vVec.z = FLOAT(m_pInfo->nHeight * TILE_SCALE) - 1.0f;

	// 修正高度
	FLOAT fGroundHeight = GetGroundHeight(vVec.x, vVec.z);
	if( vVec.y < fGroundHeight ) vVec.y = fGroundHeight;
}

template<typename UnitOperation>
VOID Map::ForEachUnitInBigVisTile(Unit* pSelf, UnitOperation oper )
{
	if( !P_VALID(pSelf) ) return;

	tagVisTile* pVisTile[ED_End] = {0};
	GetVisTile(pSelf->GetVisTileIndex(), pVisTile);

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
				oper(pRole);				
			}
		}
	}
}

template<typename UnitOperation>
VOID Map::ForEachUnitInMap( UnitOperation oper )
{
	// 找到每个地图的人
	RoleMap::TMapIterator itrR = m_mapRole.Begin();
	Role* pRole = NULL;

	while( m_mapRole.PeekNext(itrR, pRole) )
	{
		if( P_VALID(pRole) && pred(pRole))
		{
			oper(pRole);				
		}
	}

	// 找到每个地图的怪物
	CreatureMap::TMapIterator itrC = m_mapCreature.Begin();
	Creature* pCreature = NULL;

	while( m_mapCreature.PeekNext(itrC, pCreature) )
	{
		if( P_VALID(pCreature) && pred(pCreature))
		{
			oper(pCreature);				
		}
	}
}
