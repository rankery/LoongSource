#pragma once

//快捷栏按钮使用到的GUI事件
enum EGUI_ShortcutBar_Event 
{
	EGUISCB_Drop = EGUIE_User + 100,		// 按钮拖拽的释放
	EGUISCB_DropCheck,						// 释放检查，用于快捷按钮的拖放，
	EGUISCB_DropSwap,						// 快捷键的交换事件
	EGUISCB_RightMouseDown,					// 鼠标右键点击 
	EGUISCB_LeftMouseDown,					// 鼠标左键点击
	EGUISCB_UseSpecFunc,					// 特殊使用
};

//拖动类型
enum EDragType
{
	EDT_Null = 0,
	EDT_Skill,						//技能
	EDT_Item,						//物品(背包中的物品)

	EDT_ItemSplit,					//物品拆分

	EDT_ItemEquip,					//装备栏中的物品
	EDT_ItemWare,					//仓库中的物品
	EDT_ItemShop,					//商店中的物品

    EDT_PetEquip,                   //!< 宠物装备
    EDT_GuildWare,                  //!< 帮派仓库

	EDT_End,
};

//快捷按钮类型
enum EShortcutBtnType
{
	ESBT_Null = 0,
	ESBT_Skill,						//技能
	ESBT_Item,						//物品
    ESBT_PetSkill,                  //!< 宠物技能

	ESBT_End,
};


//快捷按钮数据
class BtnData
{
private:
	EShortcutBtnType 	m_eType;		//	按钮类型
	DWORD				m_dwParam1;		//	对应数据1(skill:id,	item:数目+位置, petskill:petid)
	DWORD				m_dwParam2;		//	对应数据2(skill:NULL,	item:typeid, petskill:skillid(带等级))
	INT64				m_n64Param3;		//	(skill:NULL, item: 64id, petskill:null)

public:

	BtnData() { m_eType = ESBT_Null; m_dwParam1 = m_dwParam2 = GT_INVALID; m_n64Param3 = GT_INVALID;}

	BtnData( EShortcutBtnType eType, DWORD dwParam1, DWORD dwParam2, INT64 n64Param3 ){
		Set( eType, dwParam1, dwParam2, n64Param3 );
	}
	virtual ~BtnData(){} 

	BtnData& operator=(const BtnData& data ){
		if(this==&data)
			return *this;

		m_eType			= data.m_eType;
		m_dwParam1		= data.m_dwParam1;
		m_dwParam2		= data.m_dwParam2;
		m_n64Param3		= data.m_n64Param3;
		return *this;
	}

	void Set( EShortcutBtnType eType, DWORD dwData1, DWORD dwData2, INT64 n64Data3 )
	{
		m_eType			= eType;
		m_dwParam1		= dwData1;
		m_dwParam2		= dwData2;
		m_n64Param3		= n64Data3;
	}

	const EShortcutBtnType	GetType() const		{ return m_eType;		}
	const DWORD	GetData1() const	{ return m_dwParam1;	}
	const DWORD	GetData2() const	{ return m_dwParam2;	}	
	const INT64	GetData3() const	{ return m_n64Param3;	}

};
