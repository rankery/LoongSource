#pragma once

#include "..\WorldDefine\ItemDefine.h"

#pragma pack(push, 1)

class ItemMgr;

enum EItemState// 物品当前状态
{
	EIS_Inoperable	=	0x1,	// 不可操作的
	EIS_Unmovable	=	0x2,	// 不可移动的
	EIS_Unwearable	=	0x4,	// 不可装备的
	EIS_Unuseable	=	0x8,	// 不可使用的
	EIS_Undroppable	=	0x10,	// 不可丢弃的
	EIS_Unvendible	=	0x20,	// 不可出售的
	EIS_Untradeable	=	0x40,	// 不可交易的
	EIS_Unstackable	=	0x80,	// 不可堆叠的
};

/** class Item
	brief 游戏中单堆物品类
*/
class Item
{
public:
	Item();
	virtual ~Item();
	/** \构造函数
	*/
	Item(tagItem Data);
	
	/** \拷贝构造函数
	*/
	Item(const Item&);
	
	/** \物品数据是否有效
	*/
	virtual BOOL IsItem();
	//---------------------------------------------------
	// 获取
	//--------------------------------------------------- 
	/** \获取基本数据
	*/
	const tagItem* GetData(){return &m_data;}
	/** \获取原始数据
	*/
	const tagItemProto* GetProto(){return m_pProto;}
	/** \获取显示数据
	*/
	const tagItemDisplayInfo* GetDisplay(){return m_pProto->pDisplayInfo;}
	/** \获取物品所在容器类型
	*/
	EItemConType GetConType(){return m_data.eConType;}
	/** \获取位置
	*/
	INT16 GetPos(){return m_data.n16Index;}
	/** \获取物品ID
	*/
	INT64 GetItemId(){return m_data.n64Serial;}
	/** \获取种类ID
	*/
	DWORD GetItemTypeID(){return m_data.dwTypeID;}
	/** \获取物品第二属性
	*/
	EItemTypeEx GetItemTypeEx(){return m_pProto->eTypeEx;}
	/**	\获取物品数量
	*/
	INT	GetItemQuantity(){return m_data.n16Num;}
	/** \获取物品Tips
	*/
	tstring GetItemTips();
	/** \获取物品名称
	*/
	tstring GetItemName(){return m_pProto->pDisplayInfo->szName;}
	/** \获取物品显示类型
	*/
	tstring GetItemDisplayType(){return m_pProto->pDisplayInfo->szType;}
	/** \获取物品材料类型
	*/
	EStuffType GetItemStuffType(){return m_pProto->eStuffType;}
	/** \获取物品特殊使用类型
	*/
	EItemSpecFunc GetItemSpecFunc(){return m_pProto->eSpecFunc;}
	/** \获取物品冷却时间
	*/
	DWORD GetItemCDTime(){return m_pProto->dwCooldownTime;}
	/** \获取物品绑定状态
	*/
	bool IsItemBinded(){return m_data.byBind == 2 ? true : false;}
	/** \获取物品是否锁定
	*/
	bool IsItemLocked(){return m_data.bLock;}
	/** \获取物品当前使用次数
	*/
	INT GetItemUseTimes(){return m_data.nUseTimes;}
	/** \获取物品制造者ID
	*/
	DWORD GetItemCreatorID(){return m_data.dwCreatorID;}
	/** \获取物品品级
	*/
	virtual BYTE GetItemQuality(){return m_pProto->byQuality;}
	/** \获取物品价格
	*/
	virtual INT GetItemPrice(){return m_pProto->nBasePrice*m_data.n16Num;}
	/** \获取物品等级
	*/
	BYTE GetItemLevel(){return m_pProto->byLevel;}
    /** \获取物品绑定角色ID
    */
    DWORD GetItemBindingRole() { return m_data.dwOwnerID; }
	//---------------------------------------------------
	// 赋值
	//--------------------------------------------------- 
	/** \设置物品ID
	*/
	VOID SetItemId(INT64 n64Id){m_data.n64Serial = n64Id;}
	/** \设置物品容器
	*/
	VOID SetConType(EItemConType eType){m_data.eConType = eType;}
	/** \设置位置
	*/
	VOID SetPos(INT16 n16Pos){m_data.n16Index = n16Pos;}

	/** \设置物品数量
	*/
	VOID SetItemQuantity(INT16 n16Num){m_data.n16Num = n16Num;}
	/** \设置使用次数
	*/
	VOID SetItemUseTimes(INT nTimes){m_data.nUseTimes = nTimes;}
	/** \设置绑定
	*/
	VOID SetItemBind(){m_data.byBind = 2;}
	
	//---------------------------------------------------
	// 状态判断
	//--------------------------------------------------- 
	/** \设置是否可操作
	*/
	VOID SetOperable(BOOL bOperable);
	/** \获取是否可操作
	*/
	BOOL IsOperable();
	
	/** \设置是否可移动
	*/
	VOID SetMovable(BOOL bMovable);
	/** \获取是否可移动
	*/
	BOOL IsMovable();

	/**\ 设置是否可装备
	*/
	VOID SetWearable(BOOL bWearable);
	/**\ 获取是否可装备
	*/
	BOOL IsWearable();

	/**\ 设置是否可使用
	*/
	VOID SetUseable(BOOL bUseable);
	/**\ 获取是否可使用
	*/
	BOOL IsUseable();

	/**\ 设置是否可丢弃
	*/
	VOID SetDroppable(BOOL bDroppable);
	/**\ 获取是否可丢弃
	*/
	BOOL IsDroppable();

	/**\ 设置是否可售出
	*/
	VOID SetVendible(BOOL bVendible);
	/**\ 获取是否可售出
	*/
	BOOL IsVendible();

	/**\ 设置是否可交易
	*/
	VOID SetTradeable(BOOL bTradeable);
	/**\ 获取是否可交易
	*/
	BOOL IsTradeable();

	/**\ 设置是否可堆叠
	*/
	VOID SetStackable(BOOL bStackable);
	/**\ 获取是否可堆叠
	*/
	BOOL IsStackable(INT16 &n16Quantity);

    /**\ lock the item
         first, save the current item status, then set all item attribute disabled
    */
    void Lock();
    /**\ unlock the item
         first, pop the last item status, then set all item attribute enabled
    */
    void UnLock();

    bool IsLocked() {return m_bLocked;}

protected:
	DWORD						m_dwItemState;		// 物品状态，记录各种状态
	const tagItemProto*			m_pProto;			// 物品原始结构
	tagItem						m_data;				// 物品基本结构
    bool                        m_bLocked;          // is locked
    DWORD                       m_dwlastItemStus;   // last item status before locked
};



/** \class	Equipment
	\brief	游戏中的单件装备类
*/
class Equipment : public Item
{
public:
	Equipment();
	virtual ~Equipment();

	Equipment(tagEquip Data);

	Equipment(const Equipment&);

	/** \物品数据是否有效
	*/
	virtual BOOL IsItem();
	
	/** \装备属性变化
	*/
	void UpdateEquipment(tagEquipSpec Data);

	/** \获取装备扩展结构
	*/
	const tagEquipSpec* GetEquip(){return &m_equipex;}
	/** \获取装备潜力值
	*/
	INT32 GetEquipPotVal(){return m_equipex.nPotVal;}
	/** \获取装备潜力使用值
	*/
	INT32 GetEquipUsedPotVal(){return m_equipex.nPotValUsed;}
    /** \获取装备当前潜力值上限
    */
    INT32 GetEquipCurrPotMax(){return m_equipex.nPotVal + m_equipex.nPotValUsed;}
	/*	\ 获得装备潜力值的上限
	*/
	INT32 GetEquipPotMax() { return ((const tagEquipProto*)m_pProto)->nMaxPotVal;}
	/*	\ 获得装备潜力值可提升的次数
	*/
	INT32 GetEquipPotAdvanceTime() { return m_equipex.nPotIncTimes; }
	/** \获取装备栏位
	*/
	EEquipPos GetEquipPos(){return ((const tagEquipProto*)m_pProto)->eEquipPos;}
	/** \获取装备当前镌刻锻数
	*/
	BYTE GetEquipEngraveTimes(){return m_equipex.byEngraveTimes;}
	/** \获取装备当前铭文锻数
	*/
	BYTE GetEquipPosyTimes(){return m_equipex.byPosyTimes;}
	/** \当前装备烙印级别
	*/
	BYTE GetEquipBrandLevel(){return m_equipex.byBrandLevel;}
	/** \当前装备最大镶嵌数
	*/
	BYTE GetEquipMaxHoleNum(){return m_equipex.byHoleNum;}
	/** \当前装备当前镶嵌数
	*/
	BYTE GetEquipHoleNum()
	{
		for(INT i=0; i<m_equipex.byHoleNum; ++i)
		{
			if(!P_VALID(m_equipex.dwHoleGemID[i]))
				return i;
		}
		return m_equipex.byHoleNum;
	}
	/** \获取物品品级
	*/
	virtual BYTE GetItemQuality(){return m_equipex.byQuality;} 
	/** \获取物品价格
	*/
	virtual INT GetItemPrice();
	/** \获取装备是否鉴定
	*/
	BOOL IsNotIdetified(){return m_equipex.byQuality == (BYTE)GT_INVALID;}
	/** \获取装备五行数值
	*/
	BYTE GetEquipWuxing(EWuXing eType){return m_equipex.byLongfuAtt[eType];}
	/** \获取武器最小攻击
	*/
	INT16 GetEquipMinDmg(){return m_equipex.n16MinDmg;}
	/** \获取武器最大攻击
	*/
	INT16 GetEquipMaxDmg(){return m_equipex.n16MaxDmg;}
	/** \获取武器武魂值
	*/
	INT16 GetEquipWuhun(){return m_equipex.n16WuHun;}
	/** \获取防具护甲值
	*/
	INT16 GetEquipArmor(){return m_equipex.n16Armor;}
	/** \获取武器暂新度
	*/
	INT GetEquipNewness(){return ((const tagEquipProto*)m_pProto)->n16Newness - m_data.nUseTimes/ABRASION2USETIMES;}
	/** \获取装备属性加成
	*/
	VOID GetEquipAttribute(TMap<ERoleAttribute, INT>& Map){Map = m_mapEquipAtt;}
	/** \获取装备属性百分比加成
	*/
	VOID GetEquipAttributePct(TMap<ERoleAttribute, INT>& Map){Map = m_mapEquipAttPct;}
	/** \获取装备已镶嵌宝石ID
	*/
	DWORD GetEquipHoldGemID(INT index){return m_equipex.dwHoleGemID[index];}
	/** \获取装备表龙魂能力
	*/
	DWORD GetEquipOuterLonghun(){return m_equipex.dwLongHunOuterID;}
	/** \获取装备里龙魂能力
	*/
	DWORD GetEquipInnerLonghun(){return m_equipex.dwLongHunInnerID;}
	/** \获取装备特殊属性
	*/
	BYTE GetEquipSpecAttribute(){return m_equipex.bySpecAtt;}
	/** \是否可开凿
	*/
	BOOL IsCanChisel(){return m_equipex.bCanCut;};
	/** \获取装备当前属性限制
	*/
	INT16 GetEquipAttLimit(ERoleAttribute eType);
	/** \是否可染色
	*/
	BOOL IsCanDye(){return ((const tagEquipProto*)m_pProto)->bCanDye;}

protected:
	/** \读取装备属性加成到加成列表
	*/
	VOID LoadEquipAttribute();
	/** \插入属性加成列表
	*/
	VOID AddEquipAttribute(ERoleAttribute eAtt, INT nInc);

protected:
	tagEquipSpec				m_equipex;			// 装备扩展结构
	TMap<ERoleAttribute, INT>	m_mapEquipAtt;		// 装备属性加成列表
	TMap<ERoleAttribute, INT>	m_mapEquipAttPct;	// 装备属性百分比加成列表
};

#pragma pack(pop)