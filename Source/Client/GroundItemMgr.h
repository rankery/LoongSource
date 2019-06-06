//-----------------------------------------------------------------------------
//!\file GroundItemMgr.h
//!\author Lyp
//!
//!\date 2008-10-27
//! last 2008-10-27
//!
//!\brief 地面物品处理
//!
//!	Copyright (c) 2008 CTCRST Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

struct tagGroundItem
{
	INT64			n64Serial;		// 物品64位ID
	DWORD			dwRoleID;		// 谁掉落的
	DWORD			dwTypeID;		// 物品typeID 
	INT				nNum;			// 数目
	DWORD			dwOwnerID;		// 归属角色
	DWORD			dwGroupID;		// 归属团队ID
	DWORD			dwTeamID;		// 归属队伍ID
	DWORD           dwEffectID;     // 物品的特效ID（本地玩家具有拾取权，会播放一个特效）
	
	BOOL			bChecked;		// 是否点击过拾取并还没收到消息
	ObjectSpace		space;			// 物品3D位置（当前位置）
	EntityNode*		pNode;

	tagGroundItem() { n64Serial = 0; dwTypeID = GT_INVALID; nNum = 0; pNode = 0; dwEffectID = GT_INVALID; bChecked = FALSE; }
};

struct tagParabolaItem //物品抛动管理
{
	INT64			n64Serail;
	DWORD			dwStartTime;	// 掉落开始时间
	FLOAT			fTotalTime;		// 掉落结束时间 
	FLOAT			fXZSpeed;		// XZ方向上的速度
	Vector3			vAxis;			// 转动轴
	Vector3			vStartPos;		// 开始抛出的位置
	Vector3			vEndPos;		// 最终的位置
};

class Item;
struct tagNS_SyncGroundItem;
struct tagNS_RoleGroundItemDisappear;
struct tagNS_PutDownItem;
struct tagNS_RolePickUpItem;
//-----------------------------------------------------------------------------
// GroundItemMgr
//-----------------------------------------------------------------------------
class GroundItemMgr
{
public:

	VOID Init();
	VOID Destroy();

	VOID Update();

	VOID ClearAll();

	tagGroundItem* PickItem(const Ray& rayMouse);
	VOID PutDownItem(Item* pItem, FLOAT fX, FLOAT fY, FLOAT fZ);
	BOOL PickUpItem(INT64 n64Serial, DWORD dwNum);

	tagGroundItem* FindItem(INT64 n64Serial);
	TMap<INT64, tagGroundItem*>& GetItemMap() { return m_mapGroundItem; }

	// 战斗系统调用此函数，以便处理消息缓存
	DWORD RoleSyncGroundItem(tagNS_SyncGroundItem* pRecv, DWORD dwParam);

	// 获取圆周之内的最近的本地玩家可拾取的物品，没有返回NULL
	tagGroundItem* GetNearestCanPickUpItem(const Vector3& vCenter, const float fRange);

    // 按类型获取圆周之内的最近的本地玩家可携拾取的物品，没有返回NULL
    tagGroundItem* GetNearestCanPickUpItem(const Vector3& vCenter, const float fRange,
        bool bMoney, bool bEquip,
        bool bMaterial, bool bTrophy, bool bMedicine, bool bOthers);

	// 丢弃动画设置
	VOID SetPutDownAction(tagGroundItem* pItem);
	
	// 本地玩家是否具有拾取权限
	bool CanPickUp(const tagGroundItem* pItem);

	// 显示错误信息
	VOID ShowErrMsg(DWORD dwErrCode);

	GroundItemMgr();
	~GroundItemMgr();
	static GroundItemMgr* Inst();

private:
	TSFPTrunk<GroundItemMgr>		m_Trunk;
	TObjRef<Util>					m_pUtil;
	TObjRef<NetCmdMgr>				m_pCmdMgr;
	TObjRef<NetSession>				m_pSession;


	TMap<INT64, tagGroundItem*>		m_mapGroundItem;
	TList<tagParabolaItem*>			m_listParabolaItem;


	DWORD RoleGroundItemDisappear(tagNS_RoleGroundItemDisappear* pRecv, DWORD dwParam);
	DWORD RolePickUpItem(tagNS_RolePickUpItem* pRecv, DWORD dwParam);


	BOOL ShowItem(tagGroundItem* pItem);
	VOID HideItem(tagGroundItem* pItem);
	// play drop sound
	VOID PlayDropSound(DWORD dwTypeID);
};