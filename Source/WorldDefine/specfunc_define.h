#pragma once


#pragma pack(push, 1)

//------------------------------------------------------------------------------
// 物品特殊功能类型定义
//------------------------------------------------------------------------------

/** \物品特殊功能类型(该处用<val1, val2>表示有2个参数,<val>表示1个,没有注明表示没有)
*/
enum EItemSpecFunc	
{
	EISF_Null										= 0,	// 无

	EISF_AcceptQuest								= 1,	// 接取任务
	// nSpecFuncVal1: 任务编号
	// nSpecFuncVal2: 保留
	EISF_IdetifyEquip								= 2,	// 鉴定装备
	// nSpecFuncVal1: 鉴定级别
	// nSpecFuncVal2: 保留
	EISF_HoleGem									= 3,    // 镶嵌
	// nSpecFuncVal1: 保留
	// nSpecFuncVal2: 保留
	EISF_Brand										= 4,	// 烙印
	// nSpecFuncVal1: 保留
	// nSpecFuncVal2: 保留
	EISF_LongHunShi									= 5,    // 龙附
	// nSpecFuncVal1: 保留
	// nSpecFuncVal2: 保留
	EISF_RemoveAttPt								= 6,	// 属性洗点
	// nSpecFuncVal1: 保留
	// nSpecFuncVal2: 保留
	EISF_RemoveTalentPt								= 7,	// 天资洗点
	// nSpecFuncVal1: 保留
	// nSpecFuncVal2: 保留
	EISF_LearnSkill									= 8,	// 学习技能
	// nSpecFuncVal1: 技能ID
	// nSpecFuncVal2: 保留
	EISF_Grind										= 9,	// 磨石
	// nSpecFuncVal1: 恢复暂新度值
	// nSpecFuncVal2: 保留
	EISF_ComposeAdvance								= 10,	// 合成几率提升符
	// nSpecFuncVal1: 可提升类型	
	// nSpecFuncVal2: 提升百分比
	EISF_OpenSafeGuard								= 11,	// 强开PK保护
	// nSpecFuncVal1: 保留
	// nSpecFuncVal2: 保留
	EISF_RevieveItem								= 12,	// 复活道具
	// nSpecFuncVal1: 保留
	// nSpecFuncVal2: 保留
	EISF_FriendGift									= 13,	// 好友礼物
	// nSpecFuncVal1: 保留
	// nSpecFuncVal2: 保留
	EISF_Chisel										= 15,	// 开凿
	// nSpecFuncVal1: 保留
	// nSpecFuncVal2: 保留
	EISF_ProtectSign								= 16,	// 保底符
	// nSpecFuncVal1: 保留
	// nSpecFuncVal2: 保留
	EISF_Chest										= 17,	// 宝箱
	// nSpecFuncVal1: 宝箱开出的物品(宝箱表)
	// nSpecFuncVal2: 钥匙的TypeID
	EISF_ChestKey									= 18,	// 宝箱钥匙
	// nSpecFuncVal1: 保留
	// nSpecFuncVal2: 保留

  //EISF_Chest*                                     = 19,   // 宝箱保留
	// nSpecFuncVal1: 保留
    // nSpecFuncVal2: 保留
	
	EISF_PetEgg										= 20,	// 宠物蛋
	// nSpecFuncVal1: 原型ID
	// nSpecFuncVal2: 保留
	EISF_PetFood									= 21,	// 宠物食物
	// nSpecFuncVal1: 携带等级
	// nSpecFuncVal2: 精气值
	EISF_PetSkillBook								= 22,	// 宠物技能书
	// nSpecFuncVal1: 技能原型ID
	// nSpecFuncVal2: 保留
	EISF_PetLvlupStep								= 23,	// 宠物升阶
	// nSpecFuncVal1: 灵阶后将要到达的灵阶值
	// nSpecFuncVal2: 保留
    EISF_PetEquipment                               = 24,	// 宠物装备
    // nSpecFuncVal1: 装备原型ID
    // nSpecFuncVal2: 保留
    EISF_ColorProbability                           = 25,   // 指定颜色阶段几率提升
    // nSpecFuncVal1: 数字1-4，由低到高分别表示四个颜色稀有程度阶段：低，中，高，极高
    // nSpecFuncVal2: 1-10000，表示对指定阶段的几率的提升程度（10000表示提升100%）
    EISF_Dye                                        = 26,   // 染料
    // nSpecFuncVal1: 1-12，对应颜色贴图编号
    // nSpecFuncVal2: 保留
	EISF_GuildSkillBook								= 27,	// 帮派技能典籍
	// nSpecFuncVal1: 上缴时提升进度增加量
	// nSpecFuncVal2: 帮派贡献增加量
	EISF_PetExpPour									= 28,	// 宠物经验灌注
	// nSpecFuncVal1: 灌注经验值
	// nSpecFuncVal2: 保留
	EISF_PetEnhance									= 29,	// 宠物资质提升
	// nSpecFuncVal1: 灵兽的品级
	// nSpecFuncVal2: 最大资质值
	EISF_EquipLock									= 30,	// 装备锁定
	// nSpecFuncVal1: 保留
	// nSpecFuncVal2: 保留
	EISF_EquipUnLock								= 31,	// 装备解锁
	// nSpecFuncVal1: 保留
	// nSpecFuncVal2: 保留
	EISF_PetLock									= 32,	// 宠物锁定
	// nSpecFuncVal1: 保留
	// nSpecFuncVal2: 保留
	EISF_PetUnLock									= 33,	// 宠物解锁
	// nSpecFuncVal1: 保留
	// nSpecFuncVal2: 保留
	
	EISF_PortableWare								= 34,   // 随身仓库

	EISF_TransportTool                              = 35,   // 随身传送符

	EISF_Fishing									= 36,	// 钓鱼，挂机系统

	EISF_ChangeVocation								= 37,	// 转职

	EISF_EquipAddPotValue							= 38,	// 提升装备潜力道具
	// nSpecFuncVal1: 成功率
	// nSpecFuncVal2: 潜力值的增加量
	EISF_BagExtend									= 39,	// 背包扩展
	// nSpecFuncVal1: 保留
	// nSpecFuncVal2: 保留
	EISF_RoleWareExtend								= 40,	// 仓库扩展
	// nSpecFuncVal1: 保留
	// nSpecFuncVal2: 保留
	EISF_BeGoldStone								= 41,	// 点金石
	// nSpecFuncVal1: 1： 点金石	 2： 次级点金石
	// nSpecFuncVal2: 保留
	EISF_RenameItem									= 42,	// 易名贴
	// nSpecFuncVal1: 保留
	// nSpecFuncVal2: 保留
	EISF_GemRemovalSign									= 43,	// 宝石拆除符 Jason v1.3.1
	// nSpecFuncVal1: 保留
	// nSpecFuncVal2: 保留
//	EISF_		= ,	// 
};

/** \合成几率提升符 EISF_ComposeAdvance 可提升几率类别
*/
enum ESpecFuncComposeAdvance			
{
	ESFCA_AllProduce		= 0,	// 所有生产都可提升
	ESFCA_Artisan,					// 巧匠
	ESFCA_DanTraining,				// 练丹
	ESFCA_Smith,					// 锻造
	ESFCA_Casting,					// 铸甲
	ESFCA_Dressmaker,				// 裁缝
	ESFCA_Aechnics,					// 工艺
	ESFCA_Smilt,					// 冶练

	EISFC_AllConsolidate	= 10,	// 所有装备强化都可提升
	EISFC_Posy,						// 铭纹
	EISFC_Engrave,					// 镌刻
	EISFC_Quench,					// 淬火
	

	EISFC_All				= 20,	// 所有合成都可提升 
};


#pragma pack(pop)