//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: item_mgr.h
// author: Sxg
// actor:
// data: 2008-06-16
// last:
// brief: 物品/装备管理器 -- 负责物品管理,与客户端同步,及记录物品log
//-----------------------------------------------------------------------------
#pragma once

#include "container.h"
#include "../WorldDefine/ItemDefine.h"
#include "../WorldDefine/msg_item.h"
#include "../ServerDefine/base_define.h"
#include "world.h"

struct	tagRoleData;
struct	tagCDInfo;
//-----------------------------------------------------------------------------

class ItemMgr
{
	typedef TMap<DWORD, tagCDInfo*> MapCDTime;
	typedef TMap<DWORD, INT> MapMaxHold;

public:
	ItemMgr(Role* pRole, DWORD dwAcctID, DWORD dwRoleID, INT16 n16BagSize, INT16 n16WareSize);
	~ItemMgr();

public:
	VOID SaveItem2DB(OUT LPVOID pData, OUT LPVOID &pOutPointer, OUT INT32 &nNum);
	VOID Update();

	BOOL IsItemCDTime(DWORD dwTypeID);
	VOID CheckCDTimeMap(DWORD dwTypeID);
	VOID Add2CDTimeMap(DWORD dwTypeID, DWORD dwCDTime = GT_INVALID);
	VOID SaveCDTime2DB(OUT LPVOID pData, OUT LPVOID &pOutPointer, OUT INT32 &nNum);
	VOID GetSameCDItemList(OUT TMap<DWORD, tagCDInfo*> &mapSameCD, IN DWORD dwTypeID);

	VOID ProcEquipNewness();

	static BOOL CalSpaceUsed(DWORD dwTypeID, INT32 nNum, 
		OUT INT16 &n16UseBagSpace, OUT INT16 &n16UseQuestSpace, OUT INT16 &n16MaxLap);

	//-----------------------------------------------------------------------------
	// 数组中是否有重复ID
	//-----------------------------------------------------------------------------
	BOOL IsRepeatID(INT64 n64ItemID[], INT32 nArraySz);

public:
	BOOL CanExchange(const tagItem& item) const;
	BOOL CanSell(const tagItem& item) const;

public:
	DWORD IdentifyEquip(INT64 n64SerialReel, INT64 n64SerialEquip, DWORD dwCmdID);

public:
	//-----------------------------------------------------------------------------
	// 角色初始化物品&装备
	//-----------------------------------------------------------------------------
	VOID	SendInitStateItem();
	VOID	SendInitStateItemCDTime();

	DWORD	Put2Container(tagItem *pItem);

	VOID	UpdateEquipSpec(const tagEquip &equip);

public:
	//-----------------------------------------------------------------------------
	// 从容器中获得相关信息
	//-----------------------------------------------------------------------------
	INT16	GetBagFreeSize();
	INT16	GetBagCurSize();
	INT16	GetBagOneFreeSpace();
	INT32	GetBagSameItemCount(DWORD dwTypeID);
	INT32	GetBagSameItemList(OUT TList<tagItem*> &list, IN DWORD dwTypeID, IN INT32 nNum = INT_MAX);

	INT16	GetQuestItemBagFreeSize();
	INT32	GetQuestBagSameItemCount(DWORD dwTypeID);

	INT16	GetBaiBaoFreeSize();
	INT16	GetWareCurSize();

	BOOL	IsBagOneSpaceFree(INT16 n16Index);
	BOOL	GetBagRandom(INT64 &n64Serial);
	BOOL	GetQuestItemBagRandom(INT64 &n64Serial);

	tagItem*	GetBagItem(INT64 n64Serial);
	tagItem*	GetBagItem(INT16 n16Index);
	tagEquip*	GetEquipBarEquip(INT64 n64Serial);
	tagEquip*	GetEquipBarEquip(INT16 n16Index);
	tagItem*	GetDisplayItem(EItemConType eConType, INT64 n64Serial);

public:	
	//-----------------------------------------------------------------------------
	// 玩家换装
	//-----------------------------------------------------------------------------
	DWORD Equip(INT64 n64SerialSrc, EEquipPos ePosDst);
	DWORD Unequip(INT64 n64SerialSrc, INT16 n16IndexDst);
	DWORD SwapWeapon();
	DWORD MoveRing(INT64 n64SerialSrc, INT16 n16PosDst);

public:
	//-----------------------------------------------------------------------------
	// 玩家获得&失去物品&装备 -- 普通物品放入背包，任务物品放入任务栏
	//-----------------------------------------------------------------------------
	BOOL Add2Role(EItemCreateMode eCreateMode, DWORD dwCreateID, 
				DWORD dwTypeID, INT32 nNum, EItemQuality eQlty, DWORD dwCmdID);

	DWORD RemoveFromRole(DWORD dwTypeID, INT32 nNum, DWORD dwCmdID); // nNum == -1 时表示全部删除

	//-----------------------------------------------------------------------------
	// 删除任务相关物品 -- 需查背包，任务物品栏
	//-----------------------------------------------------------------------------
	VOID RemoveFromRole(UINT16 u16QuestID, DWORD dwCmdID);

	//-----------------------------------------------------------------------------
	// 玩家获得物品&装备
	//-----------------------------------------------------------------------------
	DWORD Add2Bag(tagItem *&pItem, DWORD dwCmdID, BOOL bInsert2DB = FALSE, BOOL bCheckAdd = TRUE);
	DWORD Add2BagByIndex(tagItem *&pItem, DWORD dwCmdID, INT16 n16Index);
	DWORD Add2BagByIndexAndInsertDB(tagItem *&pItem, DWORD dwCmdID, INT16 n16Index);
	DWORD Add2QuestBag(tagItem *&pItem, DWORD dwCmdID);
	DWORD Add2RoleWare(tagItem *&pItem, DWORD dwCmdID, BOOL bInsert2DB = FALSE, BOOL bCheckAdd = TRUE);

	//-----------------------------------------------------------------------------
	// 百宝袋相关
	//-----------------------------------------------------------------------------
	DWORD Add2BaiBao(tagItem *&pItem, DWORD dwCmdID, BOOL bReadFromDB = FALSE, DWORD dwRoleIDRel = GT_INVALID);
	DWORD ClearBaibao();
	static VOID InsertBaiBao2DB(tagItem *pItem, DWORD dwRoleID, DWORD dwCmdID);
	static VOID InsertBaiBao2DBEx(tagItem *pItem, DWORD dwAccountId, DWORD dwCmdID);

	static DWORD ProcBaiBaoRecord(DWORD dwTypeID, DWORD dwDstRoleID, DWORD dwSrcRoleID, 
		INT16 n16Type = EBBRT_System, DWORD dwTime = GT_INVALID, LPCTSTR szLeaveWords = _T(""));

public:
	//-----------------------------------------------------------------------------
	// 删除物品封装细化
	//-----------------------------------------------------------------------------
	// 从背包中取出，待放入其他背包 -- 内存没有释放
	DWORD TakeOutFromBag(INT64 n64Serial, DWORD dwCmdID, BOOL bDelFromDB);
	
	// 消耗物品个数 -- n16Num取默认值时表示全部删除，并从db中清除，且释放内存
	DWORD DelFromBag(INT64 n64Serial, DWORD dwCmdID, INT16 n16Num = GT_INVALID, BOOL bCheckDel = FALSE);

	// 从指定列表中删除指定个数的物品
	DWORD DelBagSameItem(TList<tagItem*> &list, INT32 nNum, DWORD dwCmdID);

	// 生产、使用等消耗个数或次数 -- 可堆叠物品消耗个数，不可堆叠物品消息次数
	DWORD ItemUsedFromBag(INT64 n64Serial, INT16 n16Num, DWORD dwCmdID);

	// 从背包中取出，并丢弃 -- 从db中清除，绑定物品内存释放，锁定物品不可丢弃
	DWORD DiscardFromBag(INT64 n64Serial, DWORD dwCmdID, OUT tagItem *&pOut);

	// 从背包中掉落 -- 从db中清除，绑定或锁定物品内存释放
	DWORD LootFromBag(INT64 n64Serial, DWORD dwCmdID, OUT tagItem *&pOut);

	// 从背包中掉落 -- 清除可掉落物品并传出掉落物品信息
	DWORD LootFromBag(INT nLootNum, TList<tagItem*>& listItems, DWORD dwCmdID);

	// 从装备栏中掉落 -- 从db中清除，绑定或锁定物品内存释放
	DWORD LootFromEquipBar(INT64 n64Serial, DWORD dwCmdID, OUT tagItem *&pOut);

	// 从装备栏中掉落 -- 清除可掉落物品并传出掉落物品信息
	DWORD LootFromEquipBar(INT nLootNum, TList<tagEquip*>& listItems, DWORD dwCmdID);

public:
	//-----------------------------------------------------------------------------
	// 背包中删除或添加多个物品(玩家间交易,邮件,任务)
	//-----------------------------------------------------------------------------
	VOID Add2Bag(tagItem* pItem[], INT32 nSize, DWORD dwCmdID, DWORD dwRoleIDRel);
	VOID RemoveFromBag(INT64 n64Serial[], INT32 nSize, DWORD dwCmdID, DWORD dwRoleIDRel);
	BOOL CheckExistInBag(OUT tagItem* pItem[], INT64 n64Serial[], INT16 n16Num[], INT32 nSize);

	//-----------------------------------------------------------------------------
	// 移动物品
	//-----------------------------------------------------------------------------
	DWORD Move(EItemConType eConType, INT64 n64Serial, INT16 n16Num, INT16 n16PosDst, DWORD dwCmdID);
	DWORD Move2Other(EItemConType eConTypeSrc, INT64 n64Serial1, 
					EItemConType eConTypeDst, INT16 n16PosDst, DWORD dwCmdID);

	//-----------------------------------------------------------------------------
	// 从装备栏上直接删除一件装备
	//-----------------------------------------------------------------------------
	tagItem* RemoveFromEquipBar(INT64 n64Serial, DWORD dwCmdID, BOOL bDelMem = FALSE);

public:
	//-----------------------------------------------------------------------------
	// 背包&角色仓库扩容
	//-----------------------------------------------------------------------------
	DWORD ExtendBag(INT64 n64serial);
	DWORD ExtendRoleWare(INT64 n64Serial);

	//-----------------------------------------------------------------------------
	// 背包&角色仓库整理
	//-----------------------------------------------------------------------------
	DWORD ReorderBag(IN LPVOID pData, OUT LPVOID pOutData, const INT16 n16Num);
	DWORD ReorderRoleWare(IN LPVOID pData, OUT LPVOID pOutData, const INT16 n16Num);

	DWORD ReorderBagEx(IN LPVOID pData, OUT LPVOID pOutData, OUT INT16 &n16OutNum, const INT16 n16Num);
	DWORD ReorderRoleWareEx(IN LPVOID pData, OUT LPVOID pOutData, OUT INT16 &n16OutNum, const INT16 n16Num);

public:
	//-----------------------------------------------------------------------------
	// 可拥有数量限制物品管理
	//-----------------------------------------------------------------------------
	BOOL IsMaxHoldLimitItem( DWORD dwTypeID );
	BOOL CanAddMaxHoldItem( DWORD dwTypeID, INT nNum );
	BOOL CanAddMaxHoldItem( const tagItem& item );
	DWORD AddMaxHoldItem( DWORD dwTypeID, INT nNum );
	DWORD AddMaxHoldItem( const tagItem& item );
	VOID RemoveMaxHoldItem( DWORD dwTypeID, INT nNum );

private:
	template<class T> VOID Save2DB(IN Container<T> &con, OUT LPVOID pData, 
								OUT LPVOID &pOutPointer, OUT INT32 &nNum);

	template<class T> VOID GetAllItem(IN Container<T> &con, const INT16 n16ReadNum, 
								OUT LPVOID pData, OUT INT32 &nSize);

	VOID FormatCDTime(OUT LPVOID pData, OUT LPVOID &pOutPointer, OUT INT32 &nNum);
	VOID UpdateCDTime();

	VOID UpdateContainer(ItemContainer& sItemCon);
	VOID UpdateContainer(EquipContainer& sEquipCon);

public:
	ItemContainer*	GetContainer(EItemConType eConType);

	ItemContainer&	GetBag()			{ return m_Bag; }
	ItemContainer&	GetQuestItemBag()	{ return m_QuestItemBag; }
	ItemContainer&	GetBaiBaoBag()		{ return m_BaiBaoBag; }
	ItemContainer&	GetRoleWare()		{ return m_RoleWare; }
	EquipContainer& GetEquipBar()		{ return m_EquipBar; }

	//Jason 2009-11-25
	//修改装备潜力值，不影响潜力消耗值
	BOOL AddEquipPotVal(INT64 n64Serial,INT val);

private:
	//-----------------------------------------------------------------------------
	// 物品 -- bDelFromDB		是否从游戏数据库中删除,
	//		   bCheckRemove		是否判断该物品可从容器中删除
	//		   bDelMem			是否销毁内存
	//		   bIncUseTimes		是否消耗的是使用次数
	//-----------------------------------------------------------------------------
	DWORD AddItem(ItemContainer& container, tagItem *&pItem, DWORD dwCmdID, 
					BOOL bInsert2DB = FALSE, BOOL bCheckAdd = TRUE, DWORD dwRoleIDRel = GT_INVALID, BOOL bChangeOwner = TRUE);

	DWORD RemoveItem(ItemContainer& container, INT64 n64Serial, DWORD dwCmdID, 
					BOOL bDelFromDB = FALSE, BOOL bDelMem = FALSE, 
					BOOL bCheckRemove = TRUE, DWORD dwRoleIDRel = GT_INVALID);

	DWORD RemoveItem(ItemContainer& container, INT64 n64Serial, INT16 n16Num, DWORD dwCmdID, 
					BOOL bCheckRemove = TRUE, BOOL bDelete = FALSE);

	//-----------------------------------------------------------------------------
	// 从游戏中删除指定类别物品,且不做物品是否可从容器中删除的检查
	//-----------------------------------------------------------------------------
	DWORD RemoveItems(ItemContainer& container, DWORD dwTypeID, DWORD dwCmdID);
	DWORD RemoveItems(ItemContainer& container, DWORD dwTypeID, INT32 nNum, DWORD dwCmdID);

	//-----------------------------------------------------------------------------
	// 从游戏中删除和指定任务相关的物品
	//-----------------------------------------------------------------------------
	VOID RemoveItems(ItemContainer& container, UINT16 u16QuestID, DWORD dwCmdID);
	VOID RemoveItems(EquipContainer& container, UINT16 u16QuestID, DWORD dwCmdID);
	
	//-----------------------------------------------------------------------------
	// 背包中删除或添加多个物品(玩家间交易,邮件,任务)
	//-----------------------------------------------------------------------------
	VOID AddItems(ItemContainer& container, tagItem* pItem[], INT32 nSize, DWORD dwCmdID, DWORD dwRoleIDRel);
	VOID RemoveItems(ItemContainer& container, INT64 n64Serial[], INT32 nSize, DWORD dwCmdID, DWORD dwRoleIDRel);

	BOOL CheckItemsExist(OUT tagItem* pItem[], ItemContainer& container, 
						INT64 n64Serial[], INT16 n16Num[], INT32 nSize);

	BOOL IsQuestItem(DWORD dwTypeID);

private:
	//-----------------------------------------------------------------------------
	// 向客户端发送消息
	//-----------------------------------------------------------------------------
	VOID SendMessage(LPVOID pMsg, DWORD dwSize);
	
	//-----------------------------------------------------------------------------
	// 向容器中添加&删除物品，发送到客户端消息封装
	//-----------------------------------------------------------------------------
	VOID SendAddItem2Client(EItemConType eConType, INT16 n16Index, INT64 n64Serial, INT16 n16Num, BOOL bOverlap);
	VOID SendDelItem2Client(EItemConType eConType, INT16 n16Index, INT64 n64Serial, INT16 n16Num);
	VOID SendAddNew2Client(const tagItem *pItem);
	VOID SendItemBind(INT64 n64Seria, DWORD dwContTypel);

	VOID InsertItem2DB(tagItem &item);
	VOID DeleteItemFromDB(INT64 n64Serial, INT32 dwTypeID);

	//-----------------------------------------------------------------------------
	// 装备属性改变后，即时保存数据库及向客户端发送消息
	//-----------------------------------------------------------------------------
	VOID SendEquipSpec2DB(const tagEquip &equip);
	VOID SendEquipSpec2Client(const tagEquip &equip);

private:
	__forceinline VOID LogItem(const tagItem &item1, const tagItem *pItem2, 
							  INT16 n16OptNum, DWORD dwCmdID, DWORD dwRoleIDRel = GT_INVALID);
	__forceinline VOID LogItemTimes(const tagItem &item, DWORD dwCmdID);

	BOOL IsItemNeedLog(const tagItem &item) const { return item.pProtoType->bNeedLog; }


private:
	Role*				m_pRole;

	ItemContainer		m_Bag;				// 背包
	ItemContainer		m_QuestItemBag;		// 任务物品包
	ItemContainer		m_BaiBaoBag;		// 从数据库读入物品, 注意Add时的调用接口
	ItemContainer		m_RoleWare;			// 放到角色仓库中的物品,不改变所属(注意Add时的调用接口)
	EquipContainer		m_EquipBar;			// 装备栏

	MapCDTime			m_mapCDTime;		// 物品&装备冷却时间<dwTypeID, dwRemainTime>
	MapMaxHold			m_mapMaxHold;		// 限制可拥有数量物品管理
};


//-------------------------------------------------------------------------------------------------------
// 从容器中获取所有的物品数据,个数及大小
//-------------------------------------------------------------------------------------------------------
template<class T> 
VOID ItemMgr::GetAllItem(IN Container<T> &con, const INT16 n16ReadNum, OUT LPVOID pData, OUT INT32 &nSize)
{
	nSize	= 0;

	INT16	n16Num	= 0;
	T		*pTemp	= NULL;
	BYTE	*byData = (BYTE*)pData;

	for(INT16 i=0; i<con.GetCurSpaceSize(); ++i)
	{
		pTemp = con.GetItem(i);
		if(P_VALID(pTemp))
		{
			if(MIsEquipment(pTemp->dwTypeID))	// 装备
			{
				IFASTCODE->MemCpy(byData, pTemp, SIZE_EQUIP);
				((tagEquip*)byData)->equipSpec.n16QltyModPctEx = 0;	// 对客户端隐藏二级修正率
				byData += SIZE_EQUIP;
			}
			else	// 物品
			{
				IFASTCODE->MemCpy(byData, pTemp, SIZE_ITEM);
				byData += SIZE_ITEM;
			}

			++n16Num;
			if(n16ReadNum == n16Num)
			{
				break;
			}
		}
	}

	nSize = byData - (BYTE*)pData;
}

//-------------------------------------------------------------------------------------------------------
// 和LoongDB同步物品装备信息(只同步位置及使用相关信息)
//-------------------------------------------------------------------------------------------------------
template<class T> 
VOID ItemMgr::Save2DB(IN Container<T> &con, OUT LPVOID pData, OUT LPVOID &pOutPointer, OUT INT32 &nNum)
{
	nNum = 0;

	MTRANS_POINTER(pCurPointer, pData, tagItemUpdate);

	T	*pTemp	= NULL;
	for(INT16 i=0; i<con.GetCurSpaceSize(); ++i)
	{
		pTemp = con.GetItem(i);
		if(P_VALID(pTemp) && pTemp->eStatus != EUDBS_Null)
		{
			pCurPointer[nNum].n64Serial		= pTemp->n64Serial;
			pCurPointer[nNum].dwOwnerID		= pTemp->dwOwnerID;
			pCurPointer[nNum].dwAccountID	= pTemp->dwAccountID;
			pCurPointer[nNum].nUseTimes		= pTemp->nUseTimes;
			pCurPointer[nNum].n16Num		= pTemp->n16Num;
			pCurPointer[nNum].n16Index		= pTemp->n16Index;
			pCurPointer[nNum].byConType		= pTemp->eConType;
			pCurPointer[nNum].byBindType	= pTemp->byBind;

			pTemp->SetUpdate(EUDBS_Null);

			++nNum;
		}
	}

	pOutPointer = &pCurPointer[nNum];
}


//****************************** 内联函数实现 **********************************


//-----------------------------------------------------------------------------
// 格式化保存数据库数据
//-----------------------------------------------------------------------------
inline VOID ItemMgr::SaveCDTime2DB(OUT LPVOID pData, OUT LPVOID &pOutPointer, OUT INT32 &nNum)
{
	FormatCDTime(pData, pOutPointer, nNum);
}

//-----------------------------------------------------------------------------
// 是否可以出售
//-----------------------------------------------------------------------------
inline BOOL ItemMgr::CanSell(const tagItem& item) const
{
	return (item.pProtoType->bCanSell && !item.bLock && !MIsQuestItem(item.pProtoType));
}

//-----------------------------------------------------------------------------
// 更新装备动态信息后，相关消息封装
//-----------------------------------------------------------------------------
inline VOID ItemMgr::UpdateEquipSpec(const tagEquip &equip)
{
	SendEquipSpec2DB(equip);
	SendEquipSpec2Client(equip);
}

//-----------------------------------------------------------------------------
// 背包空闲格子数
//-----------------------------------------------------------------------------
inline INT16 ItemMgr::GetBagFreeSize()
{
	return GetBag().GetFreeSpaceSize();
}

//-----------------------------------------------------------------------------
// 背包当前总格子数
//-----------------------------------------------------------------------------
inline INT16 ItemMgr::GetBagCurSize()
{
	return GetBag().GetCurSpaceSize();
}

//-----------------------------------------------------------------------------
// 获取背包中一个空闲格子下标
//-----------------------------------------------------------------------------
inline INT16 ItemMgr::GetBagOneFreeSpace()
{
	return GetBag().GetOneFreeSpace();
}

//-----------------------------------------------------------------------------
// 获取背包中相同物品的总个数
//-----------------------------------------------------------------------------
inline INT32 ItemMgr::GetBagSameItemCount(DWORD dwTypeID)
{
	return GetBag().GetSameItemCount(dwTypeID);
}

//----------------------------------------------------------------------
// 获得容器中物品类型为dwTypeID的lis, 返回实际获得个数t -- 指定nNum时，仅找到nNum个物品即可
//----------------------------------------------------------------------
inline INT32 ItemMgr::GetBagSameItemList(OUT TList<tagItem*> &list, IN DWORD dwTypeID, IN INT32 nNum)
{
	return GetBag().GetSameItemList(list, dwTypeID, nNum);
}

//-----------------------------------------------------------------------------
// 获取任务栏相同物品的总个数
//-----------------------------------------------------------------------------
inline INT32 ItemMgr::GetQuestBagSameItemCount(DWORD dwTypeID)
{
	return GetQuestItemBag().GetSameItemCount(dwTypeID);
}

//-----------------------------------------------------------------------------
// 获取物品
//-----------------------------------------------------------------------------
inline tagItem *ItemMgr::GetBagItem(INT64 n64Serial)
{
	return GetBag().GetItem(n64Serial);
}

//-----------------------------------------------------------------------------
// 获取物品
//-----------------------------------------------------------------------------
inline tagItem *ItemMgr::GetBagItem(INT16 n16Index)
{
	return GetBag().GetItem(n16Index);
}

//-----------------------------------------------------------------------------
// 背包中指定位置格子是否空闲
//-----------------------------------------------------------------------------
inline BOOL	ItemMgr::IsBagOneSpaceFree(INT16 n16Index)
{
	return GetBag().IsOnePlaceFree(n16Index);
}

//-----------------------------------------------------------------------------
// 获得装备栏上物品
//-----------------------------------------------------------------------------
inline tagEquip* ItemMgr::GetEquipBarEquip(INT64 n64Serial)
{
	return GetEquipBar().GetItem(n64Serial);
}

//-----------------------------------------------------------------------------
// 获得装备栏上物品
//-----------------------------------------------------------------------------
inline tagEquip* ItemMgr::GetEquipBarEquip(INT16 n16Index)
{
	return GetEquipBar().GetItem(n16Index);
}

//-----------------------------------------------------------------------------
// 获取百宝袋空闲格子数
//-----------------------------------------------------------------------------
inline INT16 ItemMgr::GetBaiBaoFreeSize()
{
	return GetBaiBaoBag().GetFreeSpaceSize();
}

//-----------------------------------------------------------------------------
// 角色仓库当前总格子数
//-----------------------------------------------------------------------------
inline INT16 ItemMgr::GetWareCurSize()
{
	return GetRoleWare().GetCurSpaceSize();
}

//-----------------------------------------------------------------------------
// 得到任务栏可用空间
//-----------------------------------------------------------------------------
inline INT16 ItemMgr::GetQuestItemBagFreeSize()
{
	return GetQuestItemBag().GetFreeSpaceSize();
}

//-----------------------------------------------------------------------------
// 从背包中随机取个一个物品的64位id
//-----------------------------------------------------------------------------
inline BOOL	ItemMgr::GetBagRandom(INT64 &n64Serial)
{
	return GetBag().GetRandom(n64Serial);
}

//-----------------------------------------------------------------------------
// 从任务栏中随机取个一个物品的64位id
//-----------------------------------------------------------------------------
inline BOOL	ItemMgr::GetQuestItemBagRandom(INT64 &n64Serial)
{
	return GetQuestItemBag().GetRandom(n64Serial);
}

//-----------------------------------------------------------------------------
// 获取展示物品数据
//-----------------------------------------------------------------------------
inline tagItem* ItemMgr::GetDisplayItem(EItemConType eConType, INT64 n64Serial) 
{ 
	//是否装备栏
	if(eConType == EICT_Equip)
	{
		//是装备栏
		return GetEquipBar().GetItem((INT16)n64Serial);
	}
	else
	{
		ItemContainer* pContainer = GetContainer(eConType); 
		if (!P_VALID(pContainer))
			return NULL;
		return pContainer->GetItem(n64Serial);
	}
}

//-----------------------------------------------------------------------------
// 穿装备
//-----------------------------------------------------------------------------
inline DWORD ItemMgr::Equip(INT64 n64SerialSrc, EEquipPos ePosDst)
{
	return GetEquipBar().Equip(GetBag(), n64SerialSrc, ePosDst);
}

//-----------------------------------------------------------------------------
// 脱装备
//-----------------------------------------------------------------------------
inline DWORD ItemMgr::Unequip(INT64 n64SerialSrc, INT16 n16IndexDst)
{
	return GetEquipBar().Unequip(n64SerialSrc, GetBag(), n16IndexDst);
}

//-----------------------------------------------------------------------------
// 主副手物品对换
//-----------------------------------------------------------------------------
inline DWORD ItemMgr::SwapWeapon()
{
	return GetEquipBar().SwapWeapon();
}

//-----------------------------------------------------------------------------
// 两个戒指互换
//-----------------------------------------------------------------------------
inline DWORD ItemMgr::MoveRing(INT64 n64SerialSrc, INT16 n16PosDst)
{
	return GetEquipBar().MoveTo(n64SerialSrc, (EEquipPos)n16PosDst);
}

//-----------------------------------------------------------------------------
// 删除任务相关物品 -- 需查背包，任务物品栏
//-----------------------------------------------------------------------------
inline VOID ItemMgr::RemoveFromRole(UINT16 u16QuestID, DWORD dwCmdID)
{
	RemoveItems(GetBag(), u16QuestID, dwCmdID);
	RemoveItems(GetQuestItemBag(), u16QuestID, dwCmdID);
	RemoveItems(GetEquipBar(), u16QuestID, dwCmdID);
}

//-----------------------------------------------------------------------------
// 玩家获得物品&装备
//-----------------------------------------------------------------------------
inline DWORD ItemMgr::Add2Bag(tagItem *&pItem, DWORD dwCmdID, BOOL bInsert2DB, BOOL bCheckAdd)
{
	return AddItem(GetBag(), pItem, dwCmdID, bInsert2DB, bCheckAdd);
}

inline DWORD ItemMgr::Add2QuestBag(tagItem *&pItem, DWORD dwCmdID)
{
	return AddItem(GetQuestItemBag(), pItem, dwCmdID, TRUE, TRUE);
}

inline DWORD ItemMgr::Add2RoleWare(tagItem *&pItem, DWORD dwCmdID, BOOL bInsert2DB, BOOL bCheckAdd)
{
	return AddItem(GetRoleWare(), pItem, dwCmdID, bInsert2DB, bCheckAdd);
}

//-----------------------------------------------------------------------------
// 从背包中取出，待放入其他背包 -- 内存没有释放
//-----------------------------------------------------------------------------
inline DWORD ItemMgr::TakeOutFromBag(INT64 n64Serial, DWORD dwCmdID, BOOL bDelFromDB)
{
	return RemoveItem(GetBag(), n64Serial, dwCmdID, bDelFromDB, FALSE, TRUE);
}

//-----------------------------------------------------------------------------
// 消耗物品个数 -- n16Num取默认值时表示全部删除，并从db中清除，且释放内存
//-----------------------------------------------------------------------------
inline DWORD ItemMgr::DelFromBag(INT64 n64Serial, DWORD dwCmdID, INT16 n16Num, BOOL bCheckDel)
{
	if(GT_INVALID == n16Num)
	{
		return RemoveItem(GetBag(), n64Serial, dwCmdID, TRUE, TRUE, bCheckDel);
	}

	return RemoveItem(GetBag(), n64Serial, n16Num, dwCmdID, bCheckDel, TRUE);
}

//-----------------------------------------------------------------------------
// 消耗物品(使用次数或个数) -- 剩余使用次数为0时，从db中删除，并释放内存
//-----------------------------------------------------------------------------
inline DWORD ItemMgr::ItemUsedFromBag(INT64 n64Serial, INT16 n16Num, DWORD dwCmdID)
{
	return RemoveItem(GetBag(), n64Serial, n16Num, dwCmdID, TRUE, FALSE);
}

//-----------------------------------------------------------------------------
// 背包中删除或添加多个物品(玩家间交易,邮件,任务)
//-----------------------------------------------------------------------------
inline VOID ItemMgr::Add2Bag(tagItem* pItem[], INT32 nSize, DWORD dwCmdID, DWORD dwRoleIDRel)
{
	AddItems(GetBag(), pItem, nSize, dwCmdID, dwRoleIDRel);
}

inline VOID ItemMgr::RemoveFromBag(INT64 n64Serial[], INT32 nSize, DWORD dwCmdID, DWORD dwRoleIDRel)
{
	RemoveItems(GetBag(), n64Serial, nSize, dwCmdID, dwRoleIDRel);
}

inline BOOL ItemMgr::CheckExistInBag(OUT tagItem* pItem[], INT64 n64Serial[], INT16 n16Num[], INT32 nSize)
{
	return CheckItemsExist(pItem, GetBag(), n64Serial, n16Num, nSize);
}

//-----------------------------------------------------------------------------
// 背包整理
//-----------------------------------------------------------------------------
inline DWORD ItemMgr::ReorderBag(IN LPVOID pData, OUT LPVOID pOutData, const INT16 n16Num)
{
	return GetBag().Reorder(pData, pOutData, n16Num) ? E_Success : GT_INVALID;
}

inline DWORD ItemMgr::ReorderBagEx(IN LPVOID pData, OUT LPVOID pOutData, 
							OUT INT16 &n16OutNum, const INT16 n16Num)
{
	return GetBag().ReorderEx(pData, pOutData, n16OutNum, n16Num) ? E_Success : GT_INVALID;
}

//-----------------------------------------------------------------------------
// 角色仓库整理
//-----------------------------------------------------------------------------
inline DWORD ItemMgr::ReorderRoleWare(IN LPVOID pData, OUT LPVOID pOutData, const INT16 n16Num)
{
	return GetRoleWare().Reorder(pData, pOutData, n16Num) ? E_Success : GT_INVALID;
}

inline DWORD ItemMgr::ReorderRoleWareEx(IN LPVOID pData, OUT LPVOID pOutData, 
							   OUT INT16 &n16OutNum, const INT16 n16Num)
{
	return GetRoleWare().ReorderEx(pData, pOutData, n16OutNum, n16Num) ? E_Success : GT_INVALID;
}