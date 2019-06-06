#pragma once

#include ".\BaseData.h"
#include "..\WorldDefine\ItemDefine.h"
#include "..\WorldDefine\suit_define.h"
#include "..\WorldDefine\pet_equip_define.h"
#include "..\WorldDefine\longhun_define.h"

//! \struct tagTreasureProto
//! \brief 氏族珍宝列表,与itemid关联，有可能是物品也有可能是装备
//! \author hyu
struct tagTreasureProto
{
    DWORD		dwTreasureID;	//!< 氏族珍宝id,与物品id无关
    DWORD		dwItemID;		//!< 物品id,与itemid关联
    INT			nConNeed;		//!< 所需消耗氏族贡献值
    ECLanType	eCtype;			//!< 所属氏族类型
    DWORD		dwShopID;		//!< 商店ID

    tagTreasureProto() {ZeroMemory(this, sizeof(*this));}
};
//! \struct tagEquipQltyEffect
//! \brief 装备品级影响表
//! \author hqzhang
struct tagEquipQltyEffect
{
	// 基础属性影响系数
	FLOAT fWeaponFactor;
	FLOAT fArmorFactor;

	// 一级属性
	INT32 nAttAFactor;
	FLOAT fAttAFactor;
	INT32 nAttANumEffect;

	// 潜力值
	FLOAT fPotFactor;

	// 镶嵌孔数量 -- 记录出现的几率
	INT32 nHoleNumPct[MAX_EQUIPHOLE_NUM + 1];

	// 龙魂能力 -- 表里
	INT32 nLonghunPct[X_LONGHUN_TYPE_NUM];

	// 特殊属性
	INT32 nSpecAttPct;
};

/** \class	ItemProtoData
	\brief	物品、装备原始属性结构存储
*/
class ItemProtoData
	: public BaseData
{
public:
	ItemProtoData(void);
	~ItemProtoData(void);

	static ItemProtoData* Inst();

	/** 载入属性文件
	*/
	virtual void LoadFromFile();
	/** 获取物品基础结构
	*/
	const tagItemProto* FindItemProto(DWORD dwID)
	{
		if(MIsEquipment(dwID))
			return FindInMap(m_mapEquipProto, dwID);
	
		return FindInMap(m_mapItemProto, dwID);
	}
	/** 获取装备基础结构
	*/
	const tagEquipProto* FindEquipProto(DWORD dwID)
	{
		return FindInMap(m_mapEquipProto, dwID);
	}
	/** 获取套装基础结构
	*/
	const tagSuitProtoClient* FindSuitProto(DWORD dwID)
	{
		return FindInMap(m_mapSuitProto, dwID);
	}
	/** 获取物品的显示结构
	*/
	const tagItemDisplayInfo* FindItemDisplay(DWORD dwID)
	{
		const tagItemProto* pProto = FindItemProto(dwID);
		if(P_VALID(pProto))
		{
			return pProto->pDisplayInfo;
		}
		return NULL;
	}
	//! 获得珍宝
	const tagTreasureProto* FindTreasureByTrID(DWORD dwID)
	{
		return FindInMap(m_mapTreasureProto, dwID);
	}

    //! 通过itemid和氏族类型获得第一个珍宝原型
	const tagTreasureProto* FindTreasureByItemID(DWORD dwID, ECLanType eType)
	{
        multimap<DWORD, tagTreasureProto>::_Paircc range;
        multimap<DWORD, tagTreasureProto>::const_iterator it;

        range = m_mapTreasureProtoByItemID.equal_range(dwID);
        for (it = range.first; it != range.second; ++it)
        {
            if (it->second.eCtype == eType)
            {
                return &it->second;
            }
        }

        return NULL;
	}

	const map<DWORD, tagTreasureProto>* GetTreasureProto()
	{
		return &m_mapTreasureProto;
	}

    //! 通过物品原型id获得宠物装备原型
    const tagPetEquipProto* FindPetEquipByItemID(DWORD dwID);
    //! 通过宠物装备原型id获得宠物装备
    const tagPetEquipProto* FindPetEquipByEquipID(DWORD dwID);

	const tagEquipQltyEffect* FindEquipQltyEffect(INT nQlty)
	{
		return FindInMap(m_mapEquipQlty, nQlty);
	}

private:
	map<DWORD, tagItemProto>		    m_mapItemProto;		// 物品原始结构
	map<DWORD, tagEquipProto>		    m_mapEquipProto;	// 装备原始结构
	map<DWORD, tagSuitProtoClient>	    m_mapSuitProto;		// 套装原始结构
	map<DWORD, tagTreasureProto>	    m_mapTreasureProto;	// 氏族珍宝原始数据
	multimap<DWORD, tagTreasureProto>	m_mapTreasureProtoByItemID;	//通过itemid查找氏族珍宝
    map<DWORD, tagPetEquipProto>        m_mapPetEquipProtoByID;     //通过宠物装备id查找装备
	map<INT, tagEquipQltyEffect>		m_mapEquipQlty;		// 装备品级影响表
};