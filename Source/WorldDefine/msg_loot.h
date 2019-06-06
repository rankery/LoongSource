//-----------------------------------------------------------------------------
//!\file msg_loot.h
//!
//!\date 2008-10-22
//!
//!\brief 客户端与服务器之间关于掉落的消息
//-----------------------------------------------------------------------------
#pragma once

#include "msg_common_errorcode.h"
#include "loot.h"

#pragma pack(push, 1)

enum
{
	E_Loot_BAG_NOT_ENOUGH_SPACE = 1,	//背包没有足够空间
	E_Loot_Item_Not_Found		= 2,	//拾取物品没找到
	E_Loot_Role_Not_Found		= 3,	//拾取玩家没找到
	E_Loot_Map_Not_Found		= 4,	//拾取玩家所在地图没找到
	E_Loot_Add_Bag_Not_Success  = 5,	//加入背包不成功
	E_Loot_Disard_Item_Not_Success  = 6,	//加入背包不成功
	E_Loot_Pick_Up_Too_Far			= 7,	//玩家距离物品太远,无法拾取
	E_Loot_Belong_To_Team			= 8,	//只有本小队所有玩家才能拾取
	E_Loot_Belong_To_Teammate		= 9,	//只有本小队内指定的玩家才能拾取
	E_Loot_Belong_To_You			= 10,	//该物品不属于您
};

//-----------------------------------------------------------------------------
//	地面物体同步	S------>C
//-----------------------------------------------------------------------------
CMD_START(NS_SyncGroundItem)
	INT64	n64Serial;			// 物体序列号
	DWORD	dwPutDownUnitID;	// 掉落物品的UintID
	DWORD	dwOwnerID;			// 归属角色
	DWORD	dwTeamID;			// 归属小队ID
	DWORD	dwGroupID;			// 归属团队ID
	DWORD	dwTypeID;			// 类型
	INT		nNum;				// 数目
	Vector3 vPos;				// 物品掉落位置
	BOOL    bKilled;			// 怪物是否被击杀或玩家死亡
CMD_END;

//-----------------------------------------------------------------------------
//	地面物体消失---被捡起或者时间到 变长  S------>C
//-----------------------------------------------------------------------------
CMD_START(NS_RoleGroundItemDisappear)
	INT64	n64Serial[1];	// 物体序列号
CMD_END;

//-----------------------------------------------------------------------------
//	物品掉落---当怪物被击杀或玩家死亡,物品掉落到地上  S------>C
//-----------------------------------------------------------------------------
// CMD_START(NS_PutDownItem)
// 	INT64		n64Serial;	// 物体序列号
// 	DWORD		dwRoleID;	// 谁掉落的
// 	DWORD		dwOwnerID;	// 归属角色
// 	DWORD		dwGroupID;	// 归属队伍ID
// 	DWORD		dwTypeID;	// 类型
// 	INT			nNum;		// 数量
// 	Vector3		vPos;		//物品掉落位置
// CMD_END

//-----------------------------------------------------------------------------
//	人物主动掉落物品---就是销毁此物品 
//-----------------------------------------------------------------------------
CMD_START(NC_RolePutDownItem)
	INT64	n64Serial;	// 物体序列号
CMD_END;

CMD_START(NS_RolePutDownItem)
	DWORD	dwErrorCode;	//返回错误码
CMD_END

//-----------------------------------------------------------------------------
//	拾取物品
//-----------------------------------------------------------------------------
CMD_START(NC_RolePickUpItem)
	INT64 n64Serial;		//物品ID 
CMD_END

CMD_START(NS_RolePickUpItem)
	DWORD dwRoleID;			// 拾取人ID
	INT64 n64Serial;        // 物品64位ID
	DWORD dwErrorCode;		// 返回错误码
CMD_END

//-----------------------------------------------------------------------------
// 得到地面物体具体信息
//-----------------------------------------------------------------------------
// CMD_START(NC_GetGroundItemInfo)
// 	INT64 n64Serial;		//物品ID 
// CMD_END
// 
// CMD_START(NS_GetGroundItemInfo)
// 	INT64 n64Serial;        // 物品64位ID
// 	DWORD dwErrorCode;		// 返回错误码
// CMD_END

#pragma pack(pop)
