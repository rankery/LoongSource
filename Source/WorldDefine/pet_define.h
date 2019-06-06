//-----------------------------------------------------------------------------
//! filename    pet_define.h
//! author      hyu/xlguo
//! date        2009-3-24
//! last        2009-3-25
//! brief       宠物相关定义
//! sa 
//-----------------------------------------------------------------------------
#pragma once

#pragma pack(push, 1)

//! 宠物模型形态数量
enum EPetModelType
{
    EPMT_Follow  =   0,  //!< 跟随
    EPMT_Mount,         //!< 骑乘
    //EPMT_Fly,	        //!< 飞行
    EPMT_NUM
};

//! 宠物状态
enum EPetState
{
	EPS_Called		= 0x01,
	EPS_Working		= 0x02,
	EPS_Preparing	= 0x04,
	EPS_Mounting	= 0x08,
};

enum EPetStateFlag
{
	EPSF_Mounting		= 0x0001,
	EPSF_UnMounting		= 0x0002,
	EPSF_Preparing		= 0x0004,
	EPSF_UnPreparing	= 0x0008,
	EPSF_Working		= 0x0010,
	EPSF_UnWorking		= 0x0020,
	EPSF_Called			= 0x0040,
	EPSF_UnCalled		= 0x0080,
};


//! 宠物状态
union UPetState
{
	struct
	{
		bool bisCalled      : 1;
		bool bisWorking     : 1;
		bool bisPreparing   : 1;
		bool bisRiding      : 1;
	};
	BYTE	byPetState;
};


// 宠物初始化属性
struct tagPetInitAttr
{
    DWORD           dwID;                       //!< 当宠物出生时,在游戏世界生成的唯一ID
    DWORD           dwTypeID;                   //!< 宠物类型ID,对应tagPetProto中的ID
    UINT8           nQuality;                   //!< 品质
    INT16           nAptitude;                  //!< 资质
    INT16           nSpiritMax;                 //!< 灵力最大值
    INT16           nSpirit;                    //!< 灵力
    INT16           nPotential;                 //!< 潜能
    INT16           nWuXingEnergy;              //!< 五行力(最大9999)
    UINT8           nLevel;                     //!< 虚拟等级
    INT             nExpCurrent;                //!< 当前经验
    INT             nExpLevelUp;                //!< 升级经验
    INT             nMountSpeed;                //!< 骑乘速度
    INT             nBagGrid;                   //!< 行囊格数
    INT8            byPetState;                 //!< 宠物状态(EPetState)
    INT8            nMountNum;                  //!< 当前骑乘数目
    INT8            nMountNumMax;               //!< 最大骑乘数目
	bool			bLocked;					//!< 是否锁定
    INT8            nPetEquipNum;	            //!< 宠物装备数
    INT8            nPetSkillNum;	            //!< 宠物技能数
    BYTE			byData[1];                  //!< 依次为所有装备结构tagPetEquipMsgInfo,所有技能结构tagPetSkillMsgInfo
};

// 需缓冲本地的宠物信息
struct tagPetDispInfo
{
    TCHAR           szName[X_SHORT_NAME];       //!< 宠物名
    DWORD	        dwTypeID;                   //!< 宠物原型id
};


// 通讯用宠物属性枚举
enum ECSPetAtt
{
	ECSPA_Null				= -1,
	ECSPA_Begin				= 0,

	ECSPA_nQuality          = 0,    //!< 品质
	ECSPA_nAptitude         = 1,    //!< 资质
    ECSPA_nSpiritMax        = 2,    //!< 最大灵力
	ECSPA_nSpirit           = 3,    //!< 灵力
    ECSPA_nPotential        = 4,    //!< 潜能
	ECSPA_nWuXingEnergy     = 5,    //!< 五行力
	ECSPA_nLevel            = 6,    //!< 虚拟等级
	ECSPA_nExpCurrent       = 7,    //!< 当前经验
	ECSPA_nExpLevelUp       = 8,    //!< 升级经验
	ECSPA_nMountSpeed       = 9,    //!< 骑乘速度
    ECSPA_BagGrid           = 10,    //!< 行囊格数
	ECSPA_PetState          = 11,   //!< 宠物状态(EPetState)
	ECSPA_nMountNum			= 12,	//!< 当前骑乘人数
	ECSPA_nMountNumMax		= 13,	//!< 最大骑乘人数
	ECSPA_bLocked			= 14,	//!< 是否锁定

	ECSPA_End,
};

// 宠物升级获得技能最大数目
CONST INT MAX_PET_STEP_UPGRADE_SKILL_NUM		= 6;


// 宠物升级原型
struct tagPetLvlUpProto
{
	DWORD	dwVLevel;
	INT		nStep;
	INT		nGrade;
	INT		nExpLvlUpNeed;
	INT		nMoneyRatePourExpNeed;
};

// 宠物升阶道具表
struct tagPetLvlUpItemProto
{
	DWORD	dwTypeID;
	INT		dwSkillIDs[MAX_PET_STEP_UPGRADE_SKILL_NUM];
};

const INT ECSPA_Num = ECSPA_End - ECSPA_Begin;

#define EPATOERPA(n)	(ECSPetAtt)(n)

// 阶
const INT MIN_PET_STEP	= 0;								// 宠物最小阶
const INT MAX_PET_STEP	= 9;								// 宠物最大阶
const INT NUM_PET_STEP	= MAX_PET_STEP - MIN_PET_STEP + 1;	// 宠物阶数
#define IS_PET_STEP(nlvl)	((nlvl) >= MIN_PET_STEP && (nLvl) <= MAX_PET_STEP)

// 等
const INT MIN_PET_GRADE	= 1;								// 宠物最小等
const INT MAX_PET_GRADE	= 9;								// 宠物最大等
const INT NUM_PET_GRADE	= MAX_PET_GRADE - MIN_PET_GRADE + 1;// 宠物等数
#define IS_PET_GRADE(nlvl)	((nlvl) >= MIN_PET_GRADE && (nLvl) <= MAX_PET_GRADE)

// 虚拟等级
const INT MIN_PET_VLEVEL	= 1;
const INT MAX_PET_VLEVEL	= 90;
const INT NUM_PET_VLEVEL	= MAX_PET_VLEVEL - MIN_PET_VLEVEL + 1;// 宠物虚拟等级
#define IS_PET_VLEVEL(nlvl)	((nlvl) >= MIN_PET_VLEVEL && (nLvl) <= MAX_PET_VLEVEL)

#define MMID(val, min, max)		((val) = ((val) > (max)) ? (max) : (((val) < (min)) ? (min) : val))
// 虚拟等级到阶等的转换
inline VOID TransVLevel2StepGrade( INT nVLevel, INT &nStep, INT &nGrade )
{
	MMID(nVLevel, MIN_PET_VLEVEL, MAX_PET_VLEVEL);
	nVLevel -= 1;
	nStep	= nVLevel / NUM_PET_GRADE;
	nGrade	= nVLevel % NUM_PET_GRADE + 1;
}
// 阶等到虚拟等级的转换
inline VOID TransStepGrade2VLevel( INT nStep, INT nGrade, INT &nVLevel )
{
	MMID(nStep, MIN_PET_STEP, MAX_PET_STEP);
	MMID(nGrade, MIN_PET_GRADE, MAX_PET_GRADE);
	nGrade	-= 1;
	nVLevel = nStep * NUM_PET_GRADE + nGrade;
	nVLevel += 1;
}
#undef MMID

// 最远被邀请骑乘距离
const FLOAT MAX_MOUNTINVITE_DISTANCE = 10 * 50.0f;	

#pragma pack(pop)