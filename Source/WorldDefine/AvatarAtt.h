#pragma once

#pragma pack(push, 1)

//序章身份
enum  EPreChapStatus
{
	EPCS_OutKungSword = 0,		//外功剑术角色
	EPCS_InKungEeriness,		//阴柔内功
	EPCS_OutKungBow,			//外功弓术
	EPCS_InKungSing,			//内功音律
	EPCS_End,
};

// 性别
enum ERoleSex
{
	ERS_Woman		= 0,
	ERS_Man			= 1,
};

//在角色创建的时候指定的属性，在游戏中一般不会改变
#define DressMdIDInvalid		255		// 不能>255,否则数据库保存会出问题(tinyint)
struct tagAvatarAtt
{
	WORD			wHairMdlID;			// 发型
	WORD			wHairTexID;			// 发色
	WORD			wFaceMdlID;			// 面部五官
	WORD			wFaceDetailTexID;	// 面部细节
	WORD			wDressMdlID;		// 服装样式
	BYTE			bySex;				// 0:girl，1:boy(便于以后扩展)
	BYTE			byReserved;			// 保留，同时对齐
	tagAvatarAtt() { memset(this, 0, sizeof(*this)); }
};

//换装属性
//Avatar系统共由多少个模型组成 
enum EAvatarElement		
{
	EAE_Start	= 0,

	EAE_Head	= 0,	// 头
	EAE_Face	= 1,	// 面部
	EAE_Upper	= 2,	// 上身
	EAE_Lower	= 3,	// 下身
	EAE_Glove	= 4,	// 手
	EAE_Boot	= 5,	// 脚
	EAE_Back	= 6,	// 背部
	EAE_RWeapon	= 7,	// 主手武器
	EAE_LWeapon	= 8,	// 副手武器

	EAE_End		= 8,
};

const INT32 X_AVATAR_ELEMENT_NUM	= EAE_End - EAE_Start + 1;

// 装备挂载特效
enum EEquipEffect
{
	EEE_Null		= 0,	// 无
	EEE_Simple		= 1,	// 第一套特效
	EEE_Complex		= 2,	// 第二套特效
	EEE_All
};

// 装备外观
struct tagEquipDisplay
{
	DWORD			dwTypeID;
	BYTE			byDisplayPos;	// 模型位置
	BYTE			byFlareVal;		// 光晕值
	BYTE			byEquipEffect;	// 挂载效果
	INT8			n8ColorID;		// 颜色

	VOID Set(BYTE byDisplayPos, DWORD dwTypeID, BYTE byFlareVal, INT8 n8ColorID)
	{
		this->byDisplayPos	= byDisplayPos;
		this->byFlareVal	= byFlareVal;
		this->n8ColorID		= n8ColorID;
		this->dwTypeID		= dwTypeID;
	}

	VOID Set(BYTE byEquipEffect)
	{
		this->byEquipEffect	= byEquipEffect;
	}
};

const BYTE X_DEFAULT_FLARE_VAL	= 0;

// 装备外观
struct tagAvatarEquip
{
	tagEquipDisplay		AvatarEquip[X_AVATAR_ELEMENT_NUM];
	DWORD				dwSuitEffectID;						// 套装特效编号

	VOID Set(BYTE byDisplayPos, DWORD dwTypeID, BYTE byFlareVal, INT8 n8ColorID)
	{
		AvatarEquip[byDisplayPos].Set(byDisplayPos, dwTypeID, byFlareVal, n8ColorID);
	}

	VOID Set(BYTE byDisplayPos, BYTE byEquipEffect)
	{
		AvatarEquip[byDisplayPos].Set(byEquipEffect);
	}
};

const INT32 SIZE_AVATAR_EQUIP = sizeof(tagAvatarEquip);

// 装备显示设置
struct tagDisplaySet
{
	bool	bFashionDisplay	: 1;	// 是否显示时装
	bool	bHideHead		: 1;	// 是否隐藏头部
	bool	bHideFace		: 1;	// 是否隐藏面部
	bool	bHideBack		: 1;	// 是否隐藏背部

	tagDisplaySet(BYTE byData = 0) { memcpy(this, &byData, sizeof(*this)); }
	operator BYTE(){ return *(BYTE*)this; }

	VOID Set(bool bHead, bool bFace, bool bBack)
	{
		bHideHead = bHead;
		bHideFace = bFace;
		bHideBack = bBack;
	}
};

#pragma pack(pop)
