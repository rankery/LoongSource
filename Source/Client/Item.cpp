#include "stdafx.h"
#include "Item.h"
#include "ItemMgr.h"
#include "ItemEvent.h"
#include "ToolTipCreator.h"
#include "ItemProtoData.h"
#include "..\WorldDefine\shop_define.h"

#define MIsLock(item)		false
#define MIsBind(item)		(((tagItem*)item)->byBind == EBS_Bind)
#define MIsUnvend(item)		!((tagItemProto*)item)->bCanSell
#define MIsCanUse(item)		GT_VALID(((tagItemProto*)item)->dwBuffID0)

Item::Item() :
    m_dwItemState(0),
    m_bLocked(false),
    m_dwlastItemStus(0)
{
}

Item::~Item()
{

}

/** \构造函数
	\param 物品基本结构
*/
Item::Item(tagItem Data)
{
	m_dwItemState = 0;
	m_dwlastItemStus = 0;
	m_bLocked = false;
	//!拷贝数据
	memcpy(&m_data, &Data, sizeof(tagItem));
	
	const tagItemProto* pProto = ItemProtoData::Inst()->FindItemProto(m_data.dwTypeID);
	if(!P_VALID(pProto))
		return;

	//!存储物品模板
	m_pProto = pProto;
	
	//!物品状态赋值
	m_dwItemState = 0x0;
	//!任务物品不能移动
	if(EICT_Quest == m_data.eConType)
		SetMovable(FALSE);

	//!只有装备才可以穿
	if(!MIsEquipment(m_data.dwTypeID))
		SetWearable(FALSE);
	
	//!没有挂载使用效果则不可被使用
	if(!MIsCanUse(m_pProto))
		SetUseable(FALSE);

	//!锁定的物品不能丢弃
	if(MIsLock(&m_data))
		SetDroppable(FALSE);	
	
	//!注明无法出售的物品或者锁定的物品无法出售
	if(MIsUnvend(m_pProto) || MIsLock(&m_data))
		SetVendible(FALSE);

	//!已绑定或已锁定的物品无法交易
	if(MIsBind(&m_data) || MIsLock(&m_data))
		SetTradeable(FALSE);

	//!堆叠数为1的物品为不可堆叠物品
	if(1 == m_pProto->n16MaxLapNum)
		SetStackable(FALSE);

}

/** \拷贝构造函数
*/
Item::Item( const Item& Data)
{
	Item* pData = const_cast<Item*>(&Data);
	memcpy(&m_data, pData->GetData(), sizeof(tagItem));

	const tagItemProto* pProto = ItemProtoData::Inst()->FindItemProto(m_data.dwTypeID);
	if(!P_VALID(pProto))
		return;

	//!存储物品模板
	m_pProto = pProto;

	//!物品状态赋值
	m_dwItemState = 0x0;
	//!任务物品不能移动
	if(EICT_Quest == m_data.eConType)
		SetMovable(FALSE);

	//!只有装备才可以穿
	if(!MIsEquipment(m_data.dwTypeID))
		SetWearable(FALSE);

	//!没有挂载使用效果则不可被使用
	if(!MIsCanUse(m_pProto))
		SetUseable(FALSE);

	//!锁定的物品不能丢弃
	if(MIsLock(&m_data))
		SetDroppable(FALSE);	

	//!注明无法出售的物品或者锁定的物品无法出售
	if(MIsUnvend(m_pProto) || MIsLock(&m_data))
		SetVendible(FALSE);

	//!已绑定或已锁定的物品无法交易
	if(MIsBind(&m_data) || MIsLock(&m_data))
		SetTradeable(FALSE);

	//!堆叠数为1的物品为不可堆叠物品
	if(1 == m_pProto->n16MaxLapNum)
		SetStackable(FALSE);
}

/** \是否是物品
*/
BOOL Item::IsItem()
{
	BOOL bRet = P_VALID(m_data.dwTypeID) && P_VALID(m_pProto) && 
		m_data.dwTypeID == m_pProto->dwTypeID && P_VALID(m_pProto->pDisplayInfo); 
	return bRet;
}


//---------------------------------------------------
// 状态判断
//--------------------------------------------------- 
VOID Item::SetOperable(BOOL bOperable)
{
	if (bOperable)//!设置是否可操作
	{
		m_dwItemState &= ~EIS_Inoperable; 
	}
	else
	{
		m_dwItemState |= EIS_Inoperable;
	}
}

BOOL Item::IsOperable()
{
	BOOL bRet = !(m_dwItemState & EIS_Inoperable);
	return bRet;
}

VOID Item::SetMovable(BOOL bMovable)
{
	if (bMovable)//!设置是否可移动
	{
		m_dwItemState &= ~EIS_Unmovable;
	} 
	else
	{
		m_dwItemState |= EIS_Unmovable;
	}
}

BOOL Item::IsMovable()
{
	//!判断是否可移动先要判断是否可操作
	BOOL bRet = !(m_dwItemState & EIS_Unmovable);
	return IsOperable() && bRet;
}

VOID Item::SetWearable(BOOL bWearable)
{
	if (bWearable)//!设置是否可装备
	{
		m_dwItemState &= ~EIS_Unwearable;
	} 
	else
	{
		m_dwItemState |= EIS_Unwearable;
	}
}

BOOL Item::IsWearable()
{
	//!判断是否可装备先要判断是否可操作和可移动
	BOOL bRet = !(m_dwItemState & EIS_Unwearable);
	return IsOperable() && IsMovable() && bRet;
}

VOID Item::SetUseable(BOOL bUseable)
{
	if (bUseable)//!设置是否可使用
	{
		m_dwItemState &= ~EIS_Unuseable;
	} 
	else
	{
		m_dwItemState |= EIS_Unuseable;
	}
}

BOOL Item::IsUseable()
{
	//!判断是否可使用先要判断是否可操作
	BOOL bRet = !(m_dwItemState & EIS_Unuseable);
	return IsOperable() && bRet;
}

VOID Item::SetDroppable(BOOL bDroppable)
{
	if (bDroppable)//!设置是否可丢弃
	{
		m_dwItemState &= ~EIS_Undroppable;
	} 
	else
	{
		m_dwItemState |= EIS_Undroppable;
	}
}

BOOL Item::IsDroppable()
{
	//!判断是否可丢弃先要判断是否可操作和可移动
	BOOL bRet = !(m_dwItemState & EIS_Undroppable);
	return IsOperable() && IsMovable() && bRet;
}

VOID Item::SetVendible(BOOL bVendible)
{
	if (bVendible)//!设置是否可出售
	{
		m_dwItemState &= ~EIS_Unvendible;
	} 
	else
	{
		m_dwItemState |= EIS_Unvendible;
	}
}

BOOL Item::IsVendible()
{
	//!判断是否可丢弃先要判断是否可操作和可移动
	BOOL bRet = !(m_dwItemState & EIS_Unvendible);
	return IsOperable() && IsMovable() && bRet;
}

VOID Item::SetTradeable(BOOL bTradeable)
{
	if (bTradeable)//!设置是否可交易
	{
		m_dwItemState &= ~EIS_Untradeable;
	} 
	else
	{
		m_dwItemState |= EIS_Untradeable;
	}
}

BOOL Item::IsTradeable()
{
	//!判断是否可丢弃先要判断是否可操作和可移动
	BOOL bRet = !(m_dwItemState & EIS_Untradeable);
	return IsOperable() && IsMovable() && bRet;
}

VOID Item::SetStackable(BOOL bStackable)
{
	if (bStackable)//!设置是否可堆叠
	{
		m_dwItemState &= ~EIS_Unstackable;
	}
	else
	{
		m_dwItemState |= EIS_Unstackable;
	}
}

BOOL Item::IsStackable(INT16 &n16Quantity)
{
	BOOL bRet = !(m_dwItemState & EIS_Unstackable);
	n16Quantity = GetItemQuantity();
	return bRet;
}

tstring Item::GetItemTips()
{
	return ToolTipCreator::Inst()->GetItemTips(this);
}

void Item::Lock()
{
    // if has been locked, return, to avoid write last status
    if (m_bLocked)
        return;

    // save current item status
    m_dwlastItemStus = m_dwItemState;

    // set all status disable
    m_dwItemState = -1;

    m_bLocked = true;
}

void Item::UnLock()
{
    // if is unlocked, return
    if (!m_bLocked)
        return;

    // load last status
    m_dwItemState = m_dwlastItemStus;

    m_bLocked = false;
}
//----------------------------------------------------------
// 装备
//----------------------------------------------------------
Equipment::Equipment()
{

}

Equipment::~Equipment()
{

}

Equipment::Equipment(tagEquip Data)
{
	m_dwItemState = 0;
	m_dwlastItemStus = 0;
	m_bLocked = false;
	//!拷贝数据
	memcpy(&m_data, &Data, sizeof(tagItem));
	INT pos = sizeof(tagItem);
	memcpy(&m_equipex, (BYTE*)(&Data)+pos, sizeof(tagEquipSpec));

	const tagEquipProto* pProto = ItemProtoData::Inst()->FindEquipProto(m_data.dwTypeID);
	if(!P_VALID(pProto))
		return;
	
	//!存储物品模板
	m_pProto = pProto; 

	//!读取装备属性加成
	LoadEquipAttribute();

	//!装备状态赋值
	m_dwItemState = 0x0;
	//!没有挂载使用效果则不可被使用
	if(!MIsCanUse(m_pProto))
		SetUseable(FALSE);

	//!锁定的物品不能丢弃
	if(MIsLock(&m_data))
		SetDroppable(FALSE);	

	//!注明无法出售的物品或者锁定的物品无法出售
	if(MIsUnvend(m_pProto) || MIsLock(&m_data))
		SetVendible(FALSE);

	//!已绑定或已锁定的物品无法交易
	if(MIsBind(&m_data) || MIsLock(&m_data))
		SetTradeable(FALSE);

	//装备物品不可堆叠
	SetStackable(FALSE);
}

Equipment::Equipment( const Equipment& Data)
{
	Equipment* pData = const_cast<Equipment*>(&Data);
	memcpy(&m_data, pData->GetData(), sizeof(tagItem));
	memcpy(&m_equipex, pData->GetEquip(), sizeof(tagEquipSpec));

	const tagEquipProto* pProto = ItemProtoData::Inst()->FindEquipProto(m_data.dwTypeID);
	if(!P_VALID(pProto))
		return;

	//!存储物品模板
	m_pProto = pProto;

	//!读取装备属性加成
	LoadEquipAttribute();

	//!装备状态赋值
	m_dwItemState = 0x0;
	//!没有挂载使用效果则不可被使用
	if(!MIsCanUse(m_pProto))
		SetUseable(FALSE);

	//!锁定的物品不能丢弃
	if(MIsLock(&m_data))
		SetDroppable(FALSE);	

	//!注明无法出售的物品或者锁定的物品无法出售
	if(MIsUnvend(m_pProto) || MIsLock(&m_data))
		SetVendible(FALSE);

	//!已绑定或已锁定的物品无法交易
	if(MIsBind(&m_data) || MIsLock(&m_data))
		SetTradeable(FALSE);

	//!装备不可堆叠
	SetStackable(FALSE);
}


BOOL Equipment::IsItem()
{
	BOOL bRet = Item::IsItem();
	return bRet;
}

void Equipment::UpdateEquipment( tagEquipSpec Data )
{
	//!拷贝数据
	memcpy(&m_equipex, (BYTE*)(&Data), sizeof(tagEquipSpec));
	
	//!重新读取装备属性加成
	LoadEquipAttribute();
}

VOID Equipment::LoadEquipAttribute()
{
	m_mapEquipAtt.Clear();
	m_mapEquipAttPct.Clear();
	
	const tagEquipProto* pProto = (const tagEquipProto*)m_pProto;

	INT i;
	//--手动填写的属性
	for(i=0; i<MAX_ROLEATT_BASE_EFFECT; ++i)
	{
		AddEquipAttribute(pProto->BaseEffect[i].eRoleAtt, pProto->BaseEffect[i].nValue);
	}
	//--随机产生的一级属性
	for(i=0; i<X_ERA_ATTA_NUM; ++i)
	{
		AddEquipAttribute((ERoleAttribute)(ERA_AttA_Start + i), m_equipex.nRoleAttEffect[i]);
	}
	//--镌刻增加的属性
	for(i=0; i<MAX_ROLEATT_ENGRAVE_EFFECT; ++i)
	{
		AddEquipAttribute((ERoleAttribute)(ERA_EngraveAtt_Start + i), m_equipex.nEngraveAtt[i]);
	}
	//--铭文增加的属性
	for(i=0; i<MAX_ROLEATT_POSY_EFFECT; ++i)
	{
		AddEquipAttribute(m_equipex.PosyEffect[i].eRoleAtt, m_equipex.PosyEffect[i].nValue);
	}
	//--烙印增加的属性
	for(i=0; i<X_ERA_DERATE_NUM; ++i)
	{
		AddEquipAttribute((ERoleAttribute)(ERA_Derate_Start + i), m_equipex.nBrandAtt[i]);
	}
	//--时装增加的属性
	AddEquipAttribute(ERA_Appearance, m_equipex.n16Appearance);
	AddEquipAttribute(ERA_Rein,		  m_equipex.byRein);
	AddEquipAttribute(ERA_Savvy,	  m_equipex.bySavvy);
	AddEquipAttribute(ERA_Fortune,	  m_equipex.byFortune);
}

VOID Equipment::AddEquipAttribute( ERoleAttribute eAtt, INT nInc )
{
	//--增加量为零则不做处理
	if(0 == nInc)
		return;

	if(MIsValuePct(nInc))
	{
		INT nValue = m_mapEquipAttPct.Peek(eAtt);
		if(m_mapEquipAttPct.IsExist(eAtt))
		{
			m_mapEquipAttPct.ChangeValue(eAtt, nValue + MValuePctTrans(nInc));
		}
		else 
		{
			m_mapEquipAttPct.Add(eAtt, MValuePctTrans(nInc));
		}
	}
	else
	{
		INT nValue = m_mapEquipAtt.Peek(eAtt);
		if(m_mapEquipAtt.IsExist(eAtt))
		{
			m_mapEquipAtt.ChangeValue(eAtt, nValue + nInc);
		}
		else 
		{
			m_mapEquipAtt.Add(eAtt, nInc);
		}
	}
}

INT Equipment::GetItemPrice()
{
	FLOAT fFactor;
	MCalPriceFactor(fFactor, m_equipex.byQuality);
	INT nPrice = (INT)(m_pProto->nBasePrice * fFactor);
	if(nPrice < 0)
	{
		ASSERT(0);
		nPrice = 0;
	}
	return nPrice;
}

INT16 Equipment::GetEquipAttLimit( ERoleAttribute eType )
{
	if(eType >= ERA_AttA_Start && eType <= ERA_AttA_End)
	{
		const tagEquipProto* pProto = (const tagEquipProto*)m_pProto;
		INT16 n16Ret  = pProto->n16AttALimit[eType];
		INT16 n16Mod  = m_equipex.n16AttALimMod;
		FLOAT fModPct = m_equipex.n16AttALimModPct / 10000.0f;
		n16Ret = (INT16)((n16Ret + n16Mod) * (1 + fModPct));
		return n16Ret;
	}
	return 0;
}
