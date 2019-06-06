#pragma once
#pragma pack(push,1)

// 装备修改人物属性最大数目
const INT MAX_PET_EQUIP_MOD_ATT		= 3;

// 装备栏位数量
const INT SIZE_PET_EQUIP_BAR		= 4;


struct tagPetEquipProto
{
	DWORD	dwTypeID;		//技能编号	装备原型的唯一ID；
	INT		nStep;			//阶	灵兽可穿戴装备需要的最低阶数；
	INT		nGrade;			//等	灵兽可穿戴装备需要的最低等数；
	INT		nType;			//类型	策划定义的装备类型，程序只需读取编号即可；
	BOOL	bTypeUnique;	//是否同类唯一	此处置1时，类型相同的装备不可同时穿戴；若穿戴装备时，目标灵兽已穿戴同类装备，则替换原有装备；
	BOOL	bUnique;		//是否唯一	此处置1时，相同ID的装备不可同时穿戴；若穿戴装备时，目标灵兽已穿戴相同装备，则替换原有装备；
	
	INT		nPetAtt[MAX_PET_EQUIP_MOD_ATT];		//属性类型#	装备加成的灵兽属性类型；
	INT		nPetAttMod[MAX_PET_EQUIP_MOD_ATT];	//属性值#	装备加成的灵兽属性值；
};

//-----------------------------------------------------------------------------
// 宠物物品传送客户端的消息结构
//-----------------------------------------------------------------------------
struct tagPetEquipMsgInfo
{
    INT64       n64ItemID;      //!< 64位itemid
    DWORD       dwItemProtoID;  //!< 原型id
	INT8		n8Pos;			//!< 装备栏中位置
};

#pragma pack(pop)