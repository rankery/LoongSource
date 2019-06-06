#pragma once

#include "RoleDefine.h"
#include "container_define.h"
#include "../WorldDefine/skill_define.h"
#include "../WorldDefine/time.h"
#include "../WorldDefine/reputation.h"
#include "specfunc_define.h"

#pragma pack(push, 1)

class ItemScript;

//-----------------------------------------------------------------------------
// 常量定义
//-----------------------------------------------------------------------------
const INT MAX_ITEM_TYPE_NUM				= 100000;	// 静态表中，物品装备等最多条数

const INT MAX_ROLEATT_BASE_EFFECT		= 6;	// 装备原型中可填写的影响属性个数

const INT MAX_POSY_TIMES				= 20;	// 一件装备可以最多的铭文次数
const INT MAX_ROLEATT_POSY_EFFECT		= 4;	// 一件装备中铭文最多影响的属性个数

const INT MAX_ENGRAVE_TIMES				= 20;	// 一件装备可以最多的镌刻次数
const INT MAX_ROLEATT_ENGRAVE_EFFECT	= 4;	// 一件装备中镌刻最多影响的属性个数

const INT MAX_ROLEATT_ENHANCE_EFFECT	= 3;	// 强化装备的材料影响的属性个数
const INT MAX_EQUIPHOLE_NUM				= 5;	// 一件装备可以最多的镶嵌孔数数

const INT MAX_BRAND_LEVEL				= 10;	// 装备的烙印等级上限

const INT MAX_LONGHUN_LEVEL				= 10;	// 龙魂等级上限


const INT ABRASION2USETIMES				= 25;	// 武器使用次数与崭新度进率.1点崭新度 = 使用50次

const INT MAX_ITEM_DCTIME				= 7 * 24 * 60 * 60;	// 物品最长冷却时间(单位：秒)

const INT MAX_EQUIP_ATTACH_NUM          = 3;    // 装备挂载变形模型最大数量

const INT MAX_PEREQUIP_SUIT_NUM			= 3;	// 一件装备最多可属于3套套装

const INT MAX_EQUIP_BUFF_NUM			= 5;	// 装备上带有buff最大个数
//-----------------------------------------------------------------------------
// 枚举定义
//-----------------------------------------------------------------------------
enum EItemType
{
	EIT_Null		= 0,
	EIT_Expend		= 1,	// 消耗品
	EIT_Unexpend	= 2,	// 非消耗品
	EIT_Spec		= 3,	// 特殊物品
	EIT_Pet			= 4,	// 宠物
	EIT_Bag			= 5,	// 物品包
	EIT_Mall		= 6,	// 商城道具

	EIT_Weapon		= 7,	// 武器
	EIT_ClothArmor	= 8,	// 布甲
	EIT_Decoration	= 9,	// 饰品
	EIT_Fashion		= 10,	// 时装
	EIT_Armor		= 11,	// 铠甲
};

enum EItemTypeEx
{
	EITE_Null		= 0,
	EITE_Resume		= 1,	// 恢复类
	EITE_Advance	= 2,	// 强化类
	EITE_Other		= 3,	// 其他
	EITE_Material	= 4,	// 原材料
	EITE_Capture	= 5,	// 战利品
	EITE_Quest		= 6,	// 任务物品
	EITE_Book		= 7,	// 秘籍
	EITE_Pet		= 8,	// 宠物
	EITE_Ride		= 9,	// 坐骑
	EITE_Expend		= 10,	// 消耗
	EITE_Time		= 11,	// 时限

	EITE_Sword		= 12,	// 剑
	EITE_Blade		= 13,	// 刀
	EITE_Wand		= 14,	// 长兵
	EITE_Bow		= 15,	// 弓 
	EITE_Faqi		= 16,	// 法器
	EITE_Qin		= 17,	// 琴
};

enum EItemTypeReserved
{
	EITR_Null		= 0,
	EITR_Yaopin		= 1,	// 药品
	EITR_Shipin		= 2,	// 食品
	EITR_Xiandan	= 3,	// 仙丹
	EITR_Longhun	= 4,	// 龙魂
	EITR_Yinji		= 5,	// 印记
	EITR_Baoshi		= 6,	// 宝石
	EITR_Qizhen		= 7,	// 阵旗
	EITR_Ranliao	= 8,	// 染料
	EITR_Siliao		= 9,	// 饲料
	EITR_Fuzhou		= 10,	// 符咒
	EITR_Kuangshi	= 11,	// 矿石
	EITR_Caoyao		= 12,	// 草药
	EITR_ShiCai		= 13,	// 食材
	EITR_Buliao		= 14,	// 布料
	EITR_Mucai		= 15,	// 木材
	EITR_Longyuan	= 16,	// 龙魂原料
	EITR_Yinyuan	= 17,	// 印记原料

};

enum EItemQuality
{
	EIQ_Start		= -1,

	EIQ_Null		= -1,	// 未鉴定
	EIQ_White		= 0,
	EIQ_Yellow		= 1,
	EIQ_Green		= 2,
	EIQ_Blue		= 3,
	EIQ_Orange		= 4,
	// Jason 2009-11-30 增加紫色品级
	EIQ_Purple		= 5,

	EIQ_End			= 6,
};

const INT32 X_EQUIP_QUALITY_NUM = EIQ_End - EIQ_Start - 1;

enum EBindMode	// 绑定类型
{
	EBM_Null		= 0,	// 无

	EBM_Gain		= 1,	// 获得绑定
};

enum EBindStatus	// 绑定状态
{
	EBS_Unknown		= 0,	// 未知的，如商店中的商品
	EBS_Unbind		= 1,	// 未绑定
	EBS_Bind		= 2,	// 绑定
};

//enum ETimeMode		// 计时方式
//{
//	ETM_Null		= 0,
//	TEM_Create		= 1, // 生成计时
//	TEM_Online		= 2, // 线上计时
//	TEM_Battle		= 3, // 战斗计时
//	TEM_Quest		= 4, // 任务触发计时
//	TEM_Used		= 5, // 使用后计时
//	TEM_Get			= 6, // 获得计时
//};

enum ESexLimit		// 物品可使用的角色性别,要与AvatarAtt中性别对应
{
	ESL_Null		= -1,	// 不限制，即男女均可用
	ESL_Woman		= 0,	// 女限制，即女可用
	ESL_Man			= 1,	// 男限制，即男可用
};

enum EClassLimit
{
	ECL_Sword		= 0x1,	// 剑客
	ECL_JianXian	= 0x2,	// 剑仙
	ECL_XieWu		= 0x4,	// 邪舞
					
	ECL_Blade		= 0x1,	// 刀客
	ECL_BaDao		= 0x2,	// 霸刀
	ECL_GangRen		= 0x4,	// 罡刃
					
	ECL_Wand		= 0x1,	// 杖客
	ECL_ZongZhang	= 0x2,	// 宗杖
	ECL_DiMo		= 0x4,	// 地墨
					
	ECL_Bow			= 0x1,	// 灵羽
	ECL_TianJian	= 0x2,	// 天箭
	ECL_MoLing		= 0x4,	// 魔翎
					
	ECL_Valor		= 0x1,	// 气师
	ECL_XuXing		= 0x2,	// 虚行
	ECL_GuiQi		= 0x4,	// 鬼气
					
	ECL_Supple		= 0x1,	// 玄师
	ECL_XuangWang	= 0x2,	// 玄王
	ECL_MiZong		= 0x4,	// 密宗

	ECL_Apothecary	= 0x1,	// 药师
	ECL_XianYi		= 0x2,	// 仙医
	ECL_DuYin		= 0x4,	// 毒隐

	ECL_Musician	= 0x1,	// 乐师
	ECL_YueSheng	= 0x2,	// 乐圣
	ECL_HuanYin		= 0x4,	// 幻音

	ECL_Stabber		= 0x1,	// 刺客
	ECL_YingWu		= 0x2,	// 影武
	ECL_GuiWu		= 0x4,	// 鬼武
};

enum EWuXing	// 五行属性
{
	EWX_Start		= -1,
	EWX_Null		= -1,

	EWX_Metal		= 0,
	EWX_Wood		= 1,
	EWX_Water		= 2,
	EWX_Fire		= 3,
	EWX_Soil		= 4,

	EWX_End			= 5,
};

const INT32 X_WUXING_TYPE_NUM = EWX_End - EWX_Start - 1;


enum EStuffType		// 材料类型
{
	EST_Null		= 0,	//无
	EST_Stone		= 1,	//石
	EST_Jade		= 2,	//玉
	EST_Iron		= 3,	//铁
	EST_Silver		= 4,	//银
	EST_Gold		= 5,	//金
	EST_ChunXinCao	= 6,	//春辛草
	EST_XiaKuCao	= 7,	//夏枯草
	EST_QiuBaiCao	= 8,	//秋白草
	EST_DongKuiCao	= 9,	//冬葵草
	EST_DongYangCao = 10,	//东阳草
	EST_Flax		= 11,	//麻布
	EST_Leather		= 12,	//皮革
	EST_Cotton		= 13,	//棉布
	EST_SilkOrgandy	= 14,	//素纱
	EST_Silk		= 15,	//丝绸
	EST_Chop		= 16,	//肉块
	EST_Dew			= 17,	//露水
	EST_Grain		= 18,	//谷物
	EST_Bone		= 19,	//骨头
	EST_Potherb		= 20,	//野菜
	EST_IcePith		= 21,	// 冰精
	EST_FirePith	= 22,	// 火精
	EST_WoodPith	= 23,	// 木精
};

enum EItemCreateMode	// 物品&装备生产方式
{
	EICM_Null			= 0,	// 没有记录
	EICM_GM				= 1,	// GM命令生成
	EICM_Loot			= 2,	// 怪物掉落
	EICM_Quest			= 3,	// 任务奖励
	EICM_Product		= 4,	// 玩家生产
	EICM_ShopNPC		= 5,	// 商店NPC
	EICM_AccQuest		= 6,	// 接任务
	EICM_Mall			= 7,	// 商城
	EICM_Gift			= 8,	// 礼品包
	EICM_ActTreasure	= 9,	// 激活氏族珍宝
	EICM_GMTool			= 10,	// GM工具生成
	EICM_PetSkill		= 11,	// 宠物技能
	EICM_VipNetBar		= 12,	// 金牌网吧
	EICM_ReGet			= 13,	// 道具恢复
	EICM_Activity		= 14,	// 活动奖励
	EICM_LessingLoong	= 15,	// 神龙赐福
	EICM_GoldStone		= 16,	// 点金石
	EICM_GemRemoval		= 17,	// 宝石拆除
	EICM_ServerMerge	= 18,	// 合服生产

};

enum EEnhanceEquipPos	// 可强化的装备类型
{
	EEEP_Hand		= 0x00000001,	// 武器
	EEEP_Head		= 0x00000002,	// 头部
	EEEP_Face		= 0x00000004,	// 面部
	EEEP_Body		= 0x00000008,	// 上身
	EEEP_Legs		= 0x00000010,	// 下身
	EEEP_Back		= 0x00000020,	// 背部
	EEEP_Wrist		= 0x00000040,	// 腕部
	EEEP_Feet		= 0x00000080,	// 足部
	EEEP_Finger		= 0x00000100,	// 戒指
	EEEP_Waist		= 0x00000200,	// 腰饰
	EEEP_Neck		= 0x00000400,	// 项链
};

enum EEquipMdlType     // 装备的模型类型
{
	EEMT_NULL	= 0,
	EEMT_FSKIN  = 1,   // 蒙皮
	EEMT_FAK    = 2,   // 关键帧
	EEMT_FSM    = 3,   // 静态模型
};

enum EEquipAttachPos    // 装备可变形部分的挂载位置
{
	EEAP_NULL   = 0,
	EEAP_Hat    = 1,	// 头部
	EEAP_Back   = 2,	// 背部
	EEAP_RSld   = 3,	// 右肩
	EEAP_LSld   = 4,	// 左肩
	EEAP_Wep6   = 5,	// 右手腕
	EEAP_Num    = 6,
};

enum EEquipSpecAtt		// 装备B类属性
{
	EESA_Null				= -1,
	EESA_Start				= 0,

	EESA_LevelLim_Simple	= 0,	// 简易		该装备等级限制-5，最低可减少至0
	EESA_LevelLim_Fine		= 1,	// 精简		该装备等级限制-10，最低可减少至0
	EESA_LevelLim_None		= 2,	// 无级别	该装备无等级限制

	EESA_AttALim_Simple		= 3,	// 轻便		该装备属性限制减少10%，取整
	EESA_AttALim_Comfort	= 4,	// 舒适		该装备属性限制减少25%，取整
	EESA_AttALim_Light		= 5,	// 轻盈		该装备属性限制减少50%，取整

	EESA_Potential_YinFeng	= 6,	// 隐凤		该装备的初始潜力值+200
	EESA_Potential_YinHuang	= 7,	// 隐凰		该装备的初始潜力值+400
	EESA_Potential_FeiFeng	= 8,	// 飞凤		该装备的初始潜力值+800
	EESA_Potential_MingHuang= 9,	// 鸣凰		该装备的初始潜力值+1200
	EESA_Potential_WoLong	= 10,	// 卧龙		装备的初始潜力值提高5%
	EESA_Potential_CangLong	= 11,	// 藏龙		装备的初始潜力值提高10%
	EESA_Potential_FuLong	= 12,	// 伏龙		装备的初始潜力值提高20%
	EESA_Potential_ShengLong= 13,	// 升龙		装备的初始潜力值提高30%

	EESA_EquipSpec_RelEnd	= 13,	// 

	EESA_Guard_Posy			= 14,	// 铭纹守护	装备在进行铭纹强化时不会失败
	EESA_Guard_Brand		= 15,	// 烙印守护	装备在进行烙印强化时不会失败
	EESA_Guard_Engrave		= 16,	// 镌刻守护	装备在进行镌刻强化时不会失败
	EESA_Guard_LongFu		= 17,	// 龙附守护	装备在进行龙附强化时不会失败
	EESA_Guard_Quench		= 18,	// 淬火守护	装备在进行淬火强化时不会失败
	EESA_Guard_Cut			= 19,	// 开凿守护	装备在进行开凿强化时不会失败
	EESA_Guard_AllProduce	= 20,	// 绝世		装备在进入任何强化时不会失败

	EESA_Guard_Talisman		= 21,	// 避邪		进行装备强化时成功率增加[增加5%]
	EESA_Guard_Fortune		= 22,	// 幸运		进行装备强化时完美率增加[增加5%]

	EESA_Guard_DeadPunish	= 23,	// 死亡守护	该装备不会在死亡时掉落

	EESA_End				= 23,
};

const INT X_EQUIP_SPECATT_NUM	= EESA_End - EESA_Start + 1;

enum ECorlor		// 颜色贴图枚举
{
	EC_Null		= -1,

	EC_Start	= 0,
	EC_Val0		= 0,
	EC_Val1		= 1,
	EC_Val2		= 2,
	EC_Val3		= 3,
	EC_Val4		= 4,
	EC_Val5		= 5,
	EC_Val6		= 6,
	EC_Val7		= 7,
	EC_Val8		= 8,
	EC_Val9		= 9,
	EC_Val10	= 10,
	EC_Val11	= 11,
	EC_End		= 11,
};

const INT X_COLOR_NUM	= EC_End - EC_Start + 1;

//-----------------------------------------------------------------------------
// 结构定义
//-----------------------------------------------------------------------------
struct tagRoleAttEffect
{
	ERoleAttribute	eRoleAtt;
	INT32			nValue;

	tagRoleAttEffect() { ZeroMemory(this, sizeof(*this)); eRoleAtt = ERA_Null; }
};

struct tagEquipQltyPct
{
	DWORD				dwTypeID;
	INT32				nEquipQltyPct[X_EQUIP_QUALITY_NUM];	// 品级生成几率
};

struct tagCDTime
{
	DWORD		dwTypeID;
	DWORD		dwTime;		// 冷却时间(单位：毫秒)
};

/** struct	tagItemDisplayInfo
brief	物品显示结构
remarks	主要用于存储物品的名称、类型、描述、模型等等	
*/
struct tagItemDisplayInfo
{
	TCHAR				szType[X_SHORT_NAME];				// 显示类型
	TCHAR				szName[X_SHORT_NAME];				// 物品名称
	TCHAR				szDesc[X_HUGE_NAME];				// 物品描述
	TCHAR				szDescEx[X_LONG_NAME];				// 物品特性描述
	TCHAR				szMaleIcon[X_SHORT_NAME];			// 物品图标男版
	TCHAR				szFemaleIcon[X_SHORT_NAME];			// 物品图标女版
	TCHAR				szActionMod[X_SHORT_NAME];			// 物品使用动作编号
	TCHAR				szEffectMod[X_SHORT_NAME];			// 物品使用特效编号
	TCHAR				szSourceEffect[X_SHORT_NAME];		// 物品使用发起者特效编号
	TCHAR				szDestEffect[X_SHORT_NAME];			// 物品使用目标特效编号
	TCHAR				szPrepareMod[X_SHORT_NAME];			// 物品使用起手动作编号
	TCHAR				szMaleRightMod[X_SHORT_NAME];		// 男模型编号1
	TCHAR				szFemaleRightMod[X_SHORT_NAME];		// 女模型编号1
	TCHAR				szMaleLeftMod[X_SHORT_NAME];		// 男模型编号2
	TCHAR				szFemaleLeftMod[X_SHORT_NAME];		// 女模型编号2
	TCHAR				szDropMod[X_SHORT_NAME];			// 掉落模型编号
	TCHAR				szPertainMod[X_SHORT_NAME];			// 附属模型编号
	TCHAR				szBladeFlareSFX[X_SHORT_NAME];		// 刀光特效编号
	TCHAR				szBladeGlowSFX[X_SHORT_NAME];		// 光晕特效编号
	TCHAR				szAttachSFX[X_SHORT_NAME];			// 龙魂挂载特效编号
	EWeaponPos			ePertainModPos;						// 附属模型挂载位置
	EEquipMdlType       eModelType;							// 模型类型
	EEquipAttachPos     eAttachModelPos[MAX_EQUIP_ATTACH_NUM];  // 挂载变形模型的位置
	BOOL				bActionBlend;						// 是否上下身分开
	
	tagItemDisplayInfo(){ZeroMemory(this, sizeof(*this));}
};

struct tagItemProto
{
	DWORD				dwTypeID;		// 基本物品种类编号(同种物品相同)

	EItemType			eType;			// 基本类性
	EItemTypeEx			eTypeEx;		// 扩展类型
	EItemTypeReserved	eTypeReserved;	// 保留类型

	EStuffType			eStuffType;		// 材料类型，合成用

 	DWORD				dwQuestID;		// 任务编号
	INT32				nBasePrice;		// 基础价格

	INT32				nMaxUseTimes;	// 最多使用次数
	INT16				n16MaxLapNum;	// 最大叠放数量
	INT16				n16MaxHoldNum;	// 最大拥有数量

	DWORD				dwTimeLimit;	// 有时限物品(秒)

	INT16				n16Enmity;		// 仇恨

	BYTE				byMinUseLevel;	// 等级限制
	BYTE				byMaxUseLevel;	// 等级上限
	ESexLimit			eSexLimit;		// 性别限制
	DWORD				dwVocationLimit;// 职业限制

	EReputationType		eClanRepute;	// 氏族声望类型值
	INT32				nClanReputeVal;	// 氏族声望值
	EReputationType		eOtherClanRepute;	// 其他声望类型值
	INT32				nOtherClanReputeVal;// 其他声望值

	EItemSpecFunc		eSpecFunc;		// 物品特殊功用类型
	INT32				nSpecFuncVal1;
	INT32				nSpecFuncVal2;

	ESkillOPType		eOPType;							// 作用类型
	FLOAT				fOPDist;							// 作用距离（世界单位）
	FLOAT				fOPRadius;							// 作用范围（世界单位）
	INT32				nPrepareTime;						// 起手时间（毫秒）
	DWORD				dwCooldownTime;						// 技能冷却时间(单位：ms)
	INT32				nInterruptSkillOrdRate;				// 普通技能打断几率[0,10000]
	DWORD				dwSpecBuffLimitID;					// 自身BUFF限制ID
	DWORD				dwStateLimit;						// 自身状态标志位限制
	DWORD				dwTargetLimit;						// 对象限制
	BOOL				bMoveable;							// 是否可以移动使用			
	bool				bInterruptMove;						// 是否移动打断

	BYTE				byLevel;			// 物品等级
	BYTE				byQuality;			// 物品品级

	BYTE				byBindType;			// 绑定类型

	DWORD				dwTriggerID0;		// 触发条件
	DWORD				dwBuffID0;			// Buff
	
	DWORD				dwTriggerID1;		// 触发器ID1
	DWORD				dwBuffID1;			// 状态ID1

	DWORD				dwTransTriggerID;	// 变形触发类型ID
	DWORD				dwTransID;			// 变形后ID

	bool				bNeedBroadcast;		// 是否广播(0，不广播；1，广播)
	bool				bCanSell;			// 是否不可出售(0，可出售；1，不可出售)

	bool				bNeedLog;			// 是否需要做log(该属性有运营通过数据表item_needlog编辑)
	BYTE				byLogMinQlty;		// 记录log物品的最低品级

	bool				bDeadLoot;			// 死亡掉落

	tagItemDisplayInfo*	pDisplayInfo;		// 物品显示结构，客户端用
	
	tagItemProto(){ZeroMemory(this,sizeof(*this));}
	tagItemProto(DWORD dwID){ZeroMemory(this,sizeof(*this)); dwTypeID = dwID;}
};

struct tagEquipProto: public tagItemProto
{
	DWORD				dwSuitID[MAX_PEREQUIP_SUIT_NUM];		// 套装编号
	BYTE				bySuitMinQlty[MAX_PEREQUIP_SUIT_NUM];	// 最低品级要求
	bool				bCanDye;								// 时装是否可染色

	INT16				n16AttALimit[X_ERA_ATTA_NUM];			// 属性限制
	
	DWORD				dwVocationLimitWear;					// 穿戴职业限制

	EEquipPos			eEquipPos;		// 装备位置
	EWeaponPos			eWeaponPos;		// 武器位置

	INT16				n16WuHun;		// 内功伤害计算用
	INT16				n16MinDmg;		// 武器最小伤害
	INT16				n16MaxDmg;		// 武器最大伤害
	INT16				n16Armor;		// 防具护甲

	INT32				nMaxPotVal;		// 装备最大潜力值
	INT32				nPotVal;		// 装备潜力值
	// Jason 2009-12-1 装备潜力值提升次数
	INT32				nPotIncTimes;	// 潜力值提升剩余次数，注意这个值是递减的。

	tagRoleAttEffect	BaseEffect[MAX_ROLEATT_BASE_EFFECT];// 加工前，影响的属性(和品级无关)
	BYTE				byLongfuAtt[X_WUXING_TYPE_NUM];		// 龙魂属性,即五行

	//BYTE				byClassLimit[X_ClASS_TYPE_NUM];		// 职业限制(指定职业可用)//??

	DWORD				dwTriggerID2;		// 触发器ID1
	DWORD				dwBuffID2;			// 状态ID1
	
	DWORD				dwColor;			// 颜色

	INT16				n16Newness;			// 崭新度 -- 武器用

	bool				bIdentifyProduct;	// 生产鉴定状态
	bool				bIdentifyLoot;		// 掉落鉴定状态

	tagEquipProto(){ZeroMemory(this,sizeof(*this));}
	tagEquipProto(DWORD dwID){ZeroMemory(this, sizeof(*this)); dwTypeID = dwID;}
};

#define MEquipMaxUseTimes(pEquipProto)	((pEquipProto)->n16Newness * ABRASION2USETIMES)

struct tagGemProto		// 强化物品原型
{
	DWORD				dwTypeID;		// 物品原型编号

	INT16				n16PotValNeed;	// 需要消耗的装备潜力值
	INT16				n16SuccessPct;	// 成功率

	EWuXing				eWuXing;		// 龙魂，即五行属性(-1表示该物品不是龙魂)
	tagRoleAttEffect	BaseEffect[MAX_ROLEATT_ENHANCE_EFFECT];	// 装备强化影响

	INT32				nEnhanceFlg;	// 按位记录 -- EEnhanceEquipPos

	tagGemProto() { ZeroMemory(this, sizeof(*this)); }
};

struct tagItem
{
	INT64			n64Serial;		// 物品唯一序列号(不同堆叠物品不同)
	DWORD			dwTypeID;		// 基本物品种类编号(同种物品相同)

	INT16			n16Num;			// 物品个数
	bool			bLock	: 1;	// 锁定标志
	bool			bUsing	: 1;	// 是否在对该物品进行其他操作
	BYTE			byBind;			// 绑定状态
	INT32			nUseTimes;		// 物品上技能使用次数

	DWORD			dwAccountID;	// 玩家账号ID, 百宝袋和角色仓库用(同一帐号下角色共享)
	DWORD			dwOwnerID;		// 拥有者, GT_INVALID表示没有拥有者

	EItemConType	eConType;		// 物品所在容器类型
	INT16			n16Index;		// 物品在容器中的位置
	INT16			n16Dummy;		// 内存对齐用

	EItemCreateMode	eCreateMode;	// 物品&装备生成方式
	DWORD			dwCreateID;		// 外部生成该物品的ID，如：GMID，QuestID等, 界面上不显示
	
	DWORD			dwCreatorID;	// 物品制作者，GT_INVALID表示系统生产, 界面上显示
	tagDWORDTime	dwCreateTime;	// 制作物品的时间

	DWORD			dwNameID;		// 珍宝名字ID

	tagDWORDTime	dw1stGainTime;	// 第一个玩家获得时间

	EUpdateDBStatus	eStatus;		// 记录物品是否需要进行数据库操作

	union
	{
		const tagItemProto	*pProtoType;	// 指向物品基本属性
		const tagEquipProto	*pEquipProto;	// 执行装备基本属性
	};

	const ItemScript		*pScript;		// 脚本指针
	
	tagItem() { ZeroMemory(this, sizeof(*this)); }
	VOID SetOwner(DWORD dwNewOwnerID, DWORD dwNewAccountID) { dwOwnerID = dwNewOwnerID; dwAccountID = dwNewAccountID; }
	VOID SetPos(EItemConType eNewCon, INT16 n16NewIndex) { eConType = eNewCon; n16Index = n16NewIndex; }
	VOID SetIndex(INT16 n16NewIndex) { n16Index = n16NewIndex; }
	VOID SetUsing(bool bUsingState) { bUsing = bUsingState; }
	VOID SetUpdate(EUpdateDBStatus eNewUpdateStatus) { eStatus = eNewUpdateStatus; }
	INT64 GetKey() const { return n64Serial; }
	INT16 GetNum() const { return n16Num; }
	BOOL CanOverlap() const { return (P_VALID(pProtoType) ? pProtoType->n16MaxLapNum > 1 : FALSE); }
	VOID IncUseTimes(INT32 nUse) { nUseTimes += nUse; SetUpdate(EUDBS_Update); }

	// 得到某个索引对应的BuffID
	DWORD GetBuffID(INT nIndex)
	{
		switch(nIndex)
		{
		case 0:
			return pProtoType->dwBuffID0;
		case 1:
			return pProtoType->dwBuffID1;
		case 2:
			return pProtoType->dwTransID;
		default:
			return GT_INVALID;
		}
	}

	// 得到某个索引对应的TriggerID
	DWORD GetTriggerID(INT nIndex)
	{
		switch(nIndex)
		{
		case 0:
			return pProtoType->dwTriggerID0;
		case 1:
			return pProtoType->dwTriggerID1;
		case 2:
			return pProtoType->dwTransTriggerID;
		default:
			return GT_INVALID;
		}
	}

	VOID Bind()
	{
		ASSERT(P_VALID(pProtoType));
		switch(pProtoType->byBindType)
		{
		case EBM_Null:
			byBind = EBS_Unbind;
			break;
		case EBM_Gain:
			byBind = EBS_Bind;
			break;
		}
	}

	BOOL IsBind() const { return EBS_Bind == byBind; }

	BOOL IsLevelLimit(BYTE byRoleLevel) const
	{
		return pProtoType->byMinUseLevel > byRoleLevel || pProtoType->byMaxUseLevel < byRoleLevel;
	}

	BOOL IsTimeLimit() const { return pProtoType->dwTimeLimit != GT_INVALID; }

};

struct tagEquipSpec
{
	/******装备鉴定时确定******/
	INT16				n16WuHun;		// 内功伤害计算用
	INT16				n16MinDmg;		// 武器最小伤害
	INT16				n16MaxDmg;		// 武器最大伤害
	INT16				n16Armor;		// 防具护甲

	INT32				nPotVal;		// 装备潜力值(原潜力值+道具提升值)
	INT32				nPotValUsed;	// 已经消耗的潜力值
	// Jason 2009-12-1 装备潜力值提升次数
	INT32				nPotIncTimes;	// 潜力值提升剩余次数，注意这个值是递减的。
	// Jason 2009-12-2 增加紫色装备鉴定几率
	DWORD				dwPurpleQltyIdfPct;

	// 一级属性
	INT32				nRoleAttEffect[X_ERA_ATTA_NUM];

	// 表,里龙魂能力(GT_INVALID表示没有)
	DWORD				dwLongHunInnerID;
	DWORD				dwLongHunOuterID;

	// 特殊属性
	BYTE				bySpecAtt;

	// 使用等级限制(便于外部因素作用)
	BYTE				byMinUseLevel;
	BYTE				byMaxUseLevel;

	// 品级
	BYTE				byQuality;

	/******时装生成时确定******/
	INT16				n16Appearance;	// 仪容
	BYTE				byRein;			// 统御
	BYTE				bySavvy;		// 悟性
	BYTE				byFortune;		// 福缘
	INT8				n8ColorID;		// 颜色
	INT8				n8Dummy[2];

	/******装备强化时修改******/
	bool				bCanCut;		// 是否可开凿
	BYTE				byFlareVal;		// 光晕

	// 原始一级属性(升级自动加点和手动加点部分)限制
	INT16				n16AttALimMod;
	INT16				n16AttALimModPct;	/*10000为基数1*/

	// 龙附，铭文，镌刻，镶嵌，烙印(伤害减免相关属性)
	BYTE				byLongfuLevel;
	BYTE				byPosyTimes;
	BYTE				byEngraveTimes;
	BYTE				byHoleNum;
	BYTE				byBrandLevel;

	BYTE				byLongfuAtt[X_WUXING_TYPE_NUM];
	tagRoleAttEffect	PosyEffect[MAX_ROLEATT_POSY_EFFECT];
	INT32				nEngraveAtt[MAX_ROLEATT_ENGRAVE_EFFECT];
	DWORD				dwHoleGemID[MAX_EQUIPHOLE_NUM];
	INT32				nBrandAtt[X_ERA_DERATE_NUM];

	/******生产装备 -- 品级鉴定修正率******/
	INT16				n16QltyModPct;		// 一级修正率(-10000,10000)
	INT16				n16QltyModPctEx;	// 二级修正率[0,10000)

	/******生产装备 -- 潜力值影响系数******/
	INT16				n16PotValModPct;

	/******方法******/
	tagEquipSpec() { ZeroMemory(this, sizeof(*this)); }
};

struct tagEquip: public tagItem
{
	tagEquipSpec	equipSpec;

	VOID SetFlareVal(BYTE byFlareVal)
	{
		equipSpec.byFlareVal = byFlareVal;
	}

	VOID ChangeFlareVal(BYTE byFlareVal)
	{
		equipSpec.byFlareVal += byFlareVal;
		equipSpec.byFlareVal = (equipSpec.byFlareVal > 100) ? 100 : equipSpec.byFlareVal;
		equipSpec.byFlareVal = (equipSpec.byFlareVal < 0 ) ? 0 : equipSpec.byFlareVal;
	}

	VOID ChangePotVal(INT nValue)
	{
		INT nOrgPotVal = equipSpec.nPotVal;
		equipSpec.nPotVal += nValue;

		if(equipSpec.nPotVal < 0)		equipSpec.nPotVal = 0;

		if(equipSpec.nPotVal > pEquipProto->nMaxPotVal)
			equipSpec.nPotVal = pEquipProto->nMaxPotVal;

		if(nValue < 0)
			equipSpec.nPotValUsed += (nOrgPotVal - equipSpec.nPotVal);
	}

	VOID IncAttackTimes()
	{		
		++nUseTimes;

		if(eStatus != EUDBS_Update)
		{
			SetUpdate(EUDBS_Update);
		}
	}

	VOID DecAttackTimes(INT nTimes)
	{
		nUseTimes = min(MEquipMaxUseTimes(pEquipProto), nUseTimes);
		nUseTimes = max(0, nUseTimes - nTimes);

		if(eStatus != EUDBS_Update)
		{
			SetUpdate(EUDBS_Update);
		}
	}

	BOOL IsNewessChange(INT &nCurAttackTimes)
	{
		nCurAttackTimes = MEquipMaxUseTimes(pEquipProto);
		if(nUseTimes > nCurAttackTimes)
		{
			nUseTimes = nCurAttackTimes;
			return FALSE;
		}

		nCurAttackTimes = nUseTimes;
		if(nUseTimes % ABRASION2USETIMES == 0)
		{
			return TRUE;
		}

		return FALSE;
	}

	VOID GetTriggerIDBuffID(OUT DWORD &dwTriggerID, OUT DWORD &dwBuffID, IN INT nIndex)
	{
		dwTriggerID = dwBuffID = GT_INVALID;

		if(!P_VALID(pEquipProto))
		{
			ASSERT(0);
			return;
		}

		switch(nIndex)
		{
		case 0:
			dwTriggerID = pEquipProto->dwTriggerID0;
			dwBuffID	= pEquipProto->dwBuffID0;
			break;
		case 1:
			dwTriggerID = pEquipProto->dwTriggerID1;
			dwBuffID	= pEquipProto->dwBuffID1;
			break;
		case 2:
			dwTriggerID = pEquipProto->dwTriggerID2;
			dwBuffID	= pEquipProto->dwBuffID2;
			break;
		default:
			break;
		}
	}

	VOID GetLongHunID(DWORD &dwLongHunID, BOOL &bOuterLongHun) const
	{
		if(equipSpec.dwLongHunOuterID != GT_INVALID)
		{
			bOuterLongHun	= TRUE;
			dwLongHunID		= equipSpec.dwLongHunOuterID;
			return;
		}

		if(equipSpec.dwLongHunInnerID != GT_INVALID)
		{
			bOuterLongHun	= FALSE;
			dwLongHunID		= equipSpec.dwLongHunInnerID;
			return;
		}
	}

	BYTE GetLongfuAtt(INT nWuXingType) const
	{
		return equipSpec.byLongfuAtt[nWuXingType] + pEquipProto->byLongfuAtt[nWuXingType];
	}
};

// 销毁物品&装备(避免在tagItem中使用虚析构函数)
static VOID Destroy(tagItem *&pItem)
{
	if(!P_VALID(pItem))
	{
		return;
	}
	
	if(MIsEquipment(pItem->dwTypeID))
	{
		delete ((tagEquip*)pItem);
	}
	else
	{
		delete pItem;
	}

	pItem = NULL;
}

// 销毁物品&装备(避免在tagItem中使用虚析构函数)
static VOID Destroy(tagEquip *&pEquip)
{
	if(!P_VALID(pEquip))
	{
		return;
	}

	SAFE_DEL(pEquip);
}

//-----------------------------------------------------------------------------
// 常量定义 -- 和结构相关
//-----------------------------------------------------------------------------
const INT SIZE_ITEM						= sizeof(tagItem);
const INT SIZE_EQUIP					= sizeof(tagEquip);
const INT SIZE_EQUIPSPEC				= sizeof(tagEquipSpec);


//-----------------------------------------------------------------------------
#pragma pack(pop)
