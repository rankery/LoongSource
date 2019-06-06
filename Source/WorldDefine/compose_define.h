#pragma once

#include "ItemDefine.h"

#pragma pack(push, 1)

#define MAX_CONSOLIDATE_POS_QUANTITY 11
#define MAX_CONSOLIDATE_STUFF_QUANTITY 6
#define MAX_PRODUCE_STUFF_QUANTITY 6
#define MAX_CONSOLIDATE_ROLEATT 3
#define MAX_OUTPUT_QUANTITY	6

//生成类型
enum EProduceType
{
	EPCT_NULL	= 0,
	EPCT_Artisan,			//工匠1
	EPCT_DanTraining,		//练丹2
	EPCT_Smith,				//锻造3
	EPCT_Casting,			//铸甲4
	EPCT_Dressmaker,		//裁缝5
	EPCT_Aechnics,			//工艺6
	EPCT_Smilt,				//冶练7
	EPCT_PointUp,			//点化8
	EPCT_GodArtisan,		//天匠9	add xtian 2009-4-13
	EPCT_Pet,				//灵兽10 add xtian 2009-8-21
	EPCT_EquipMaster,		// 装备匠师
};
//合成类型(生成类型的子类)
enum EComposeType
{
	ECOMT_NULL = 0,
	ECOMT_StuffRefine = 10,			//(食材,布)精炼	10
	ECOMT_StuffProcess,				//(食材,布)加工	11
	ECOMT_StuffDecompose1,			//降分			13(add xtian 2009-10-13)
	ECOMT_RefineDan = 20,			//炼丹			20
	ECOMT_Cooking,					//烹饪			21
	ECOMT_WeaponMake = 30,			//武器制造		30
	ECOMT_MillstoneMake,			//消耗品			31
	ECIMT_WeaponAlbum,				// 武器图谱		32
	ECOMT_LoricaeMake = 40,			//铠甲制造		40
	ECOMT_RollMake,					//鉴定卷制造		41
	ECOMT_ArmorAlbum,				// 铠甲图谱		42
	ECOMT_ClothMake = 50,			//布甲制造		50
	ECOMT_FashionMake,				//时装制造		51
	ECOMT_ClothArmorAlbum,			// 布甲图谱		52
	ECOMT_DecorateMake = 60,		//饰品制造		60
	ECOMT_ChiselMake,				//凿具制造		61(add xtian 12-3)
	ECOMT_GemProcess,				//宝石加工		62(add xtian 2009-2-12)
	ECOMT_DecorationAlbum,			// 饰品图谱		63
	ECOMT_Refine = 70,				//(矿石,草药)精练 70
	ECOMT_Process,					//(矿石,草药)加工 71
	ECOMT_StuffDecompose2,			//降分2			72(add xtian 2009-10-13)
	ECOMT_EquipdDecompose = 80,		//装备分解		80
	ECOMT_PointUp,					//点化			81
	ECOMT_Rebuild = 90,				//印记改造		90	add xtian 2009-4-13
	ECOMT_LoongSoulCom,				//龙魂合成		91
	ECOMT_LoongSoulReb,				//龙魂改造		92
	ECOMT_GemCompose,				//宝石合成		93
	ECOMT_GemReBuild,				//宝石改造		94
	ECOMT_GemCarve,					//宝石雕琢		95
	ECOMT_PetMake = 100,			//灵魄合成		100 add xtian 2009-8-21
	ECOMT_WeaponProduce = 110,		// 武器制造		110
	ECOMT_ArmorProdeuce = 111,		// 盔甲制造		111
	ECOMT_ClothArmorProduce = 112,	// 布甲制造		112
	ECOMT_DecorationProduce = 113,	// 饰品制造		113

};	

//每项生产包含的合成子类数量
#define ArtisanSubTypeQuan		ECOMT_StuffDecompose1 - ECOMT_StuffRefine + 1
#define DanTrainingSubTypeQuan	ECOMT_Cooking -  ECOMT_RefineDan + 1
#define SmithSubTypeQuan		ECIMT_WeaponAlbum - ECOMT_WeaponMake + 1
#define CastingSubTypeQuan		ECOMT_ArmorAlbum - ECOMT_LoricaeMake + 1
#define DressmakerSubTypeQuan	ECOMT_ClothArmorAlbum - ECOMT_ClothMake + 1
#define AechnicsSubTypeQuan		ECOMT_DecorationAlbum - ECOMT_DecorateMake + 1
#define SmiltSubTypeQuan		ECOMT_StuffDecompose2 - ECOMT_Refine + 1
#define PointUpSubTypeQuan		ECOMT_PointUp - ECOMT_EquipdDecompose + 1
#define GodArtisanSubTypeQuan	ECOMT_GemCarve - ECOMT_Rebuild + 1
#define PetSubTypeQuan			ECOMT_PetMake - ECOMT_PetMake + 1
#define EquipSubTypeQuan		ECOMT_DecorationProduce - ECOMT_WeaponProduce + 1

//合成配方类型
enum EFormulaType
{
	EFORMT_NULL	= 0,
	EFORMT_OreRefine,		//矿石提纯	1
	EFORMT_HerbalRefine,	//草药提纯	2
	EFORMT_ClothRefine,		//布料精纺	3
	EFORMT_FoodRefine,		//食材精制	4
	EFORMT_OreProcess,		//矿石加工	5
	EFORMT_HerbalProcess,	//草药加工	6
	EFORMT_ClothProcess,	//布料加工	7
	EFORMT_FoodProcess,		//食材加工	8
	EFORMT_ThewDrug,		//体力药		9
	EFORMT_EnergyDrug,		//真气药		10
	EFORMT_RestoreDrug,		//全回药		11
	EFORMT_GodDrug,			//神药		12
	EFORMT_SecondReDrug,	//秒回药		13
	EFORMT_InterinjuryDrug,	//内伤药		14
	EFORMT_NormalDrug,		//普通药		15
	EFORMT_Wine,			//酒			16
	EFORMT_Cate,			//美食		17
	EFORMT_Glave,			//剑			18
	EFORMT_Sword,			//刀			19
	EFORMT_Halberd,			//长兵		20
	EFORMT_Bow,				//弓			21
	EFORMT_SmallOrgan,		//小型法器	22
	EFORMT_BigOrgan,		//大型法器	23
	EFORMT_Instrument,		//琴			24
	EFORMT_Millstone,		//魔石		25
	EFORMT_Armet,			//头盔		26
	EFORMT_UpperLoricae,	//上身盔甲	27
	EFORMT_LowerLoricae,	//下身盔甲	28
	EFORMT_Armguard,		//护手		29
	EFORMT_BattleShoe,		//战靴		30
	EFORMT_Roll,			//鉴定卷		31
	EFORMT_Cap,				//帽子		32
	EFORMT_Jupe,			//上衣		33	
	EFORMT_Pants,			//下衣		34
	EFORMT_Glove,			//手套		35
	EFORMT_Shoe,			//鞋子		36
	EFORMT_Mantle,			//披风		37
	EFORMT_FashionCap,		//时装帽子	38
	EFORMT_FashionJupe,		//时装上衣	39
	EFORMT_FashionPants,	//时装下衣	40
	EFORMT_FashionGloave,	//时装手套	41
	EFORMT_FashionShoe,		//时装鞋子	42
	EFORMT_FashionMantle,	//时装披风	43
	EFORMT_FingerRing,		//戒指		44
	EFORMT_Caestus,			//腰饰		45
	EFORMT_Necklace,		//项链		46
	EFORMT_Chisel,			//凿具		47
	EFORMT_YuanRockIncise,	//原石切割	48
	EFORMT_GeBlade,			//割刀		49
	EFORMT_GangRebuild,		//钢锻印改造 50 add xtian 2009-4-13
	EFORMT_YuRebuild,		//玉磨印改造
	EFORMT_YinRebuild,		//银锻印改造
	EFORMT_JinRebuild,		//金琢印改造
	EFORMT_JinLoongSoul,	//金龙魂
	EFORMT_MuLoongSoul,		//木龙魂
	EFORMT_ShuLoongSoul,	//水龙魂
	EFORMT_HuoLoongSoul,	//火龙魂
	EFORMT_TuLoongSoul,		//土龙魂
	EFORMT_JinLoongSoulReb,	//金龙魂改造
	EFORMT_MuLoongSoulReb,	//木龙魂改造
	EFORMT_ShuLoongSoulReb,	//水龙魂改造
	EFORMT_HuoLoongSoulReb,	//火龙魂改造
	EFORMT_TuLoongSoulReb,	//土龙魂改造
	EFORMT_2Gem,			//二级宝石
	EFORMT_3Gem,			//三级宝石
	EFORMT_4Gem,			//四级宝石
	EFORMT_2GemRebuild,		//二级宝石改造
	EFORMT_3GemRebuild,		//三级宝石改造
	EFORMT_4GemRebuild,		//四级宝石改造
	EFORMT_RefineDanOther,	//炼丹其他
	EFORMT_WeaponMakeOther, //武器制造其他
	EFORMT_LoricaeMakeOther,//铸甲制造其他
	EFORMT_ClothMakeOther,	//布甲制造其他
	EFORMT_DecorateMakeOther,//饰品制造其他
	EFORMT_ShangSoul,			//上品灵魄
	EFORMT_LingSoul,			//灵品灵魄
	EFORMT_XianSoul,			//仙品灵魄
	EFORMT_ShengSoul,			//圣品灵魄
	EFORMT_MakeJade,			//炼玉
	EFORMT_PetFood,				//灵兽口粮
	EFORMT_GemBlade,			//宝石雕刀
	EFORMT_GemCarve,			//宝石雕琢
	EFORMT_ClothDecompose,		//布料降分
	EFORMT_FoodDecompose,		//食材降分
	EFORMT_OreDecompose,		//矿石降分
	EFORMT_HerbalDecompose,		//草药降分
	EFORMT_Album,				// 图谱
	EFORMT_5Gem,			// 五级宝石
	EFORMT_6Gem,			// 六级宝石
	EFORMT_7Gem,			// 七级宝石
	EFORMT_5GemRebuild,		// 五级宝石改造
	EFORMT_6GemRebuild,		// 六级宝石改造
	EFORMT_7GemRebuild,		// 七级宝石改造
};

//配方来源
enum EFormulaFrom
{
	EFormulaFrom_NULL = 0,
	EFormulaFrom_Skill,					//技能
	EFormulaFrom_NPC,					//NPC
	EFormulaFrom_Item,					//物品
	EFormulaFrom_SkillorNPC,			//技能及NPC
	EFormulaFrom_SkillorItem,			//技能及物品
	EFormulaFrom_NPCorItem,				//NPC及物品
};

//装备强化类型
enum EConsolidateType
{
	ECliDateT_NULL 		= 0,		//无
	ECliDateT_Posy		= 1,		//铭纹
	ECliDateT_Engrave	= 2,		//镌刻
	ECliDateT_Quench,				//淬火
};

//强化铭纹属性
enum EPosyAtt
{
	EPosyAtt_NULL		= 0,	//无	
	EPosyAtt_MaxHP		= 1,    //最大体力
	EPosyAtt_MaxMP		= 2,	//最大真气
	EPosyAtt_ExAttack	= 3,	//外功攻击
	EPosyAtt_InAttack	= 4,	//内功攻击
	EPosyAtt_ExDefense	= 5,	//外功防御
	EPosyAtt_InDefense	= 6,	//内功防御

	EPosyAtt_End,
};
//强化镌刻属性
enum EEngraveAtt
{
	EEngraveAtt_NULL		= 0,	//无
	EEngraveAtt_WeaponDmg	= 1,	//武器伤害
	EEngraveAtt_WeaponSoul  = 2,	//武魂
	EEngraveAtt_Armor		= 3,	//防具护甲(防)
	EEngraveAtt_Deration	= 4,	//防具护甲(饰)
	EEngraveAtt_End,
};

//淬火类型
enum EQuenchType
{
	EEQuenchType_Metal		= 0,	//金淬
	EEQuenchType_Wood,				//木淬
	EEQuenchType_Water,				//水淬
	EEQuenchType_Fire,				//火淬
	EEQuenchType_Soil,				//土淬
};


//可强化装备部位
struct tagConsolidatePos
{
	EEquipPos ePos;
	BYTE	  byConsolidate;
};

//强化材料
struct tagConsolidateStuff
{
	DWORD				dwStuffID;			//材料ID
	EStuffType			eStuffType;			//材料类型
	DWORD				dwStuffNum;			//材料数量
};

//生产合成材料
struct tagProduceSutff
{
	DWORD				dwStuffID;			//材料ID
	EStuffType			eStuffType;			//材料类型
	DWORD				dwStuffNum;			//材料数量	
};

//铭纹结构体
struct tagPosyProto
{

	DWORD				dwID;					//配方ID
	TCHAR				szName[X_SHORT_NAME];	//配方名称
	EPosyAtt			ePosyAtt;				//铭纹属性
	BYTE				byPosyTimes;			//铭纹次数
	INT					nSuccessRate;			//成功率
	DWORD				dwProduceTime;			//生产计时
	DWORD				dwPotValConsume;		//潜力值消耗
	DWORD				dwMoneyConsume;			//金钱消耗
	FLOAT 				fcoefficientA; 			//属性价值公式参数A
	FLOAT 				fcoefficientB; 			//属性价值公式参数B
	FLOAT 				fcoefficientC; 			//属性价值公式参数C
	tagConsolidateStuff	ConsolidateStuff[MAX_CONSOLIDATE_STUFF_QUANTITY]; //强化材料
	tagConsolidatePos   ConsolidatePos[MAX_CONSOLIDATE_POS_QUANTITY];	//可强化装备部位
};

//镌刻结构体
struct tagEngraveProto
{

	DWORD				dwID;					//配方ID
	TCHAR				szName[X_SHORT_NAME];	//配方名称
	EEngraveAtt			eEngraveAtt;			//镌刻属性
	BYTE				byEngraveTimes;			//镌刻次数
	INT					nSuccessRate;			//成功率
	DWORD				dwProduceTime;			//生产计时
	DWORD				dwPotValConsume;		//潜力值消耗
	DWORD				dwMoneyConsume;			//金钱消耗
	FLOAT 				fcoefficientA; 			//属性价值公式参数A
	FLOAT 				fcoefficientB; 			//属性价值公式参数B
	FLOAT 				fcoefficientC; 			//属性价值公式参数C
	tagConsolidateStuff	ConsolidateStuff[MAX_CONSOLIDATE_STUFF_QUANTITY]; //强化材料
	tagConsolidatePos   ConsolidatePos[MAX_CONSOLIDATE_POS_QUANTITY];	//可强化装备部位
};

//淬火属性
struct tagQuenchAtt
{
	EWuXing				eWuXing;
	INT					nWuXingValue;
};

//淬火结构体
struct tagQuenchProto
{
	DWORD				dwID;					//配方ID
	TCHAR				szName[X_SHORT_NAME];	//配方名称
	EQuenchType			eQuenchType;			//淬火类型
	tagQuenchAtt		srcQuenchAtt;			//原属性及属性值
	tagQuenchAtt		dstQuenchAtt;			//目标属性及属性值
	DWORD				dwPotValConsume;		//潜力值消耗
	DWORD				dwMoneyConsume;			//金钱消耗
	INT					nSuccessRate;			//成功率
	DWORD				dwProduceTime;			//生产计时
	tagConsolidateStuff	ConsolidateStuff[MAX_CONSOLIDATE_STUFF_QUANTITY]; //强化材料
};

//生产合成
struct tagProduceProto
{
	DWORD				dwID;					//配方ID
	TCHAR				szName[X_SHORT_NAME];	//配方名称
	EProduceType		eProType;				//生产类型
	EComposeType		eComType;				//合成类型
	EFormulaType		eFormType;				//配方类型
	EFormulaFrom		eFormFrom;				//配方来源
	DWORD				dwFormLvl;				//配方等级
	DWORD				dwProItemTypeID;		//合成的物品
	DWORD				dwProQuantity;			//合成的数量
	INT					nSuccessRate;			//成功率
	DWORD				dwProduceTime;			//生产计时
	DWORD				dwVitalityConsume;		//活力消耗
	DWORD				dwMasterIncre;			//熟练度增加
	DWORD				dwMoneyConsume;			//金钱消耗
	tagProduceSutff		ProduceStuff[MAX_PRODUCE_STUFF_QUANTITY];//生产材料
};


//强化道具强化的属性
struct tagConsolidateRoleAtt
{
	ERoleAttribute		eRoleAtt;
	INT					nAttVal;
};

//镶嵌,烙印,龙附(强化道具)
struct tagConsolidateItem
{
	DWORD					dwID;						//道具的ID
	DWORD					dwPotValConsume;			//潜力值消耗
	INT						nSuccessRate;				//成功率
	EWuXing					eWuXing;					//龙魂属性
	tagConsolidateRoleAtt	tagRoleAtt[MAX_CONSOLIDATE_ROLEATT];		//强化属性	
	tagConsolidatePos   ConsolidatePos[MAX_CONSOLIDATE_POS_QUANTITY];	//可强化装备部位
};

//  分解点化输出物品
struct tagOutputStuff
{
	DWORD				dwStuffTypeID;			//输出材料TypeID
	INT					nPerMinVal;				//完美最小数量
	INT					nPerMaxVal;				//完美最大数量
	INT					nSucMinVal;				//成功最小数量
	INT					nSucMaxVal;				//成功最大数量
	INT					nRate;					//几率
};

// 分解点化
struct tagDeComposeProto
{
	DWORD				dwID;					//配方ID
	TCHAR				szName[X_SHORT_NAME];	//配方名称
	EComposeType		eComType;				//合成类型
	BYTE				byLevel;				//装备等级
	BYTE				byQuality;				//装备品级
	EItemType			eType;					//装备类型
	EItemTypeEx			eTypeEx;				//武器类型
	EEquipPos			ePos;					//装备位置
	DWORD				dwFormLvl;				//配方等级
	EFormulaFrom		eFormFrom;				//配方来源
	INT					nSuccessRate;			//成功率
	DWORD				dwProduceTime;			//生产计时
	DWORD				dwVitalityConsume;		//活力消耗
	DWORD				dwMasterIncre;			//熟练度增加
	DWORD				dwMoneyConsume;			//金钱消耗
	tagOutputStuff		OutputStuff[MAX_OUTPUT_QUANTITY];			//输出材料		
};

#pragma pack(pop)
