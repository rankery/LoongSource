#pragma once
#pragma pack(push, 1)

enum ESpecItemUseError
{
	E_ItemLock_HaveLocked = 1,		//物品已经被锁定
	E_ItemLock_NoLocked,			//物品尚未被锁定
	E_ItemLock_TargetError,			//目标不合法
	E_ItemLock_HaveNotItem,			//没有锁定或者解锁 道具
	E_ItemLock_OtherError,			//其它错误
};

CMD_START(NC_LockItem)
	INT64			n64Item;		// 道具的64位ID
	INT64			n64Target;		// 要绑定物品的64位ID
CMD_END

CMD_START(NS_LockItem)
	DWORD			dwErrorCode;	// 锁定结果
	BYTE			byType;			// 0：锁定操作的返回，1：解锁操作的返回
	DWORD			dwUnlockTime;	// 解锁操作时，返回解锁时间
	INT64			n64Serial;		// 目标物品ID
CMD_END

#pragma pack(pop)