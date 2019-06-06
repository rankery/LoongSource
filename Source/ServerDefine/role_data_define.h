#pragma once

#include "../WorldDefine/base_define.h"
#include "../WorldDefine/RoleDefine.h"
#include "../WorldDefine/time.h"
#include "../WorldDefine/talent_define.h"
#include "../WorldDefine/currency_define.h"
#include "../ServerDefine/clandata_define.h"
#include "../ServerDefine/strength_define.h"
#pragma pack(push, 1)

struct tagDWORDTime;

//-------------------------------------------------------------------------------------
// 常量
//-------------------------------------------------------------------------------------
const INT MIN_ROLE_SAVE2DB_INTERVAL			= 60 * TICK_PER_SECOND;
const INT ROLE_TIME_STAT_INTERVAL			= 30 * 60 * TICK_PER_SECOND;

// Jason 2009-12-7 神龙赐福最大级别
// 不做限定,从属性表去最大数
static INT ROLE_MAX_LESSLEVEL_OF_LOONG		= 20 ;

//-------------------------------------------------------------------------------------
// 人物创建结构
//-------------------------------------------------------------------------------------
struct tagCreateRoleInfo
{
	tagAvatarAtt	Avatar;						// size = 16
	TCHAR			szRoleName[X_SHORT_NAME];

	tagDWORDTime	CreateTime;					// 创建时间
	DWORD			dwMapID;
	FLOAT			fX;
	FLOAT			fY;
	FLOAT			fZ;
	FLOAT			fFaceX;
	FLOAT			fFaceY;
	FLOAT			fFaceZ;
	BYTE			byLevel;
	BYTE			byReserved[3];

};

//---------------------------------------------------------------------------------------
// 实时保存数据库枚举
//---------------------------------------------------------------------------------------
enum ERealTimeSaveAtt
{
	ERTSA_Null			= -1,
	ERTSA_Exp			= 0,
	ERTSA_Level			= 1,
};

//---------------------------------------------------------------------------------------
// 离线玩家数据结构
//---------------------------------------------------------------------------------------
struct tagRoleInfo
{
	DWORD			dwRoleID;					// ID
	DWORD			dwRoleNameCrc;				// 名称CRC
	DWORD			dwAccountID;				// 账户ID
	TCHAR			szRoleName[X_SHORT_NAME];	// 名字
	DWORD			dwMapID;					// 所在地图
	tagDWORDTime	dwTimeLastLogout;			// 最后上线时间
	BYTE			byLevel;					// 等级
	BYTE			bySex;						// 性别
	bool			bOnline;					// 玩家是否在线
	BYTE			byClass;					// 职业
	DWORD			dwLoverID;					// 爱人
};

//---------------------------------------------------------------------------------------
// 安全码
//---------------------------------------------------------------------------------------
struct tagSafeCode
{
	DWORD	dwSafeCodeCrc;		// 安全码Crc32，GT_INVALID表示没有设置过
	DWORD	dwTimeReset;		// 重置时间，GT_INVALID表示没有重置
};


// Jason 神龙赐福配置文件数据结构
struct tagConfigLessingOfLoong
{
	INT nID;			// 级别0-7，也即8次
	INT nExpireTime;	// 到期时间
	INT nExperience;	// 经验值
	DWORD	dwRewardItemTypeID;	// 奖励物品typeid
	INT	nQlty;
	INT	nItemCount;		// 奖励物品数量
	tagConfigLessingOfLoong()
		:nID(0),nExpireTime(0),nExperience(0),dwRewardItemTypeID(0),nItemCount(0),nQlty(0)
	{
	}
	tagConfigLessingOfLoong(const tagConfigLessingOfLoong & t)
		:nID(t.nID),nExpireTime(t.nExpireTime),nExperience(t.nExperience),dwRewardItemTypeID(t.dwRewardItemTypeID),nItemCount(t.nItemCount),
		nQlty(t.nQlty)
	{
	}
	tagConfigLessingOfLoong & operator = (const tagConfigLessingOfLoong & t)
	{
		memcpy(this,&t,sizeof(t));
	}
};


//---------------------------------------------------------------------------------------
// accountrel表内相关属性
//---------------------------------------------------------------------------------------
struct tagAccountCommon
{
	tagSafeCode	sSafeCode;
	DWORD		dwBagPsdCrc;
	INT			nBaiBaoYuanBao;
	INT64		n64WareSilver;
	INT16		n16WareSize;

	VOID Init()
	{
		sSafeCode.dwSafeCodeCrc = GT_INVALID;
		sSafeCode.dwTimeReset	= GT_INVALID;

		dwBagPsdCrc				= GT_INVALID;
		n64WareSilver			= 0;
		nBaiBaoYuanBao			= 0;
		n16WareSize				= 0;
	}
};

//---------------------------------------------------------------------------------------
// 游戏服务器与数据库服务器存取的玩家数据结构
//---------------------------------------------------------------------------------------
struct tagRoleDataConst
{
	DWORD			dwRoleNameCrc;				// 名称CRC
	TCHAR			szRoleName[X_SHORT_NAME];	// 名称
	tagAvatarAtt	Avatar;						// 外观
	tagDWORDTime	CreateTime;					// 创建时间
};

struct tagRoleDataSave
{
	tagDWORDTime	LoginTime;					// 登入时间
	tagDWORDTime	LogoutTime;					// 登出时间
	INT				nOnlineTime;				// 总在线时常
	INT				nCurOnlineTime;				// 当次在线时常

	// Jason 2009-12-7 神龙赐福
	DWORD	timeLastLessing;			// 上次神龙赐福时间，注意，该值与总在线时间单位一致
	INT				nLessingLevel;				// 赐福级别，最高ROLE_MAX_LESSLEVEL_OF_LOONG级

	tagAvatarEquip	AvatarEquip;				// 装备外观
	tagDisplaySet	DisplaySet;					// 外观显示设置

	DWORD			dwMapID;					// 所在的地图的ID
	DWORD			dwRebornMapID;				// 复活地图ID
	FLOAT			fCoordinate[3];				// 位置（x,y,z）
	FLOAT			fFace[3];					// 朝向（Vectro3）

	EClassType		eClass;						// 职业
	EClassTypeEx	eClassEx;					// 扩展职业

	INT				nLevel;						// 玩家等级
	INT				nCurExp;					// 玩家当前级别的升级经验

	INT				nHP;						// 体力
	INT				nMP;						// 真气
	INT				nRage;						// 怒气
	INT				nEndurance;					// 持久力
	INT				nVitality;					// 活力
	INT				nInjury;					// 内伤
	INT				nKnowledge;					// 阅历
	INT				nMorale;					// 士气
	INT				nMorality;					// 道德
	INT				nCulture;					// 武学修为
	INT				nHostility;					// 戾气
	INT				nCredit;					// 信用度
	INT				nIdentity;					// 身份
	INT				nVIPPoint;					// 会员积分

	INT				nAttPoint;					// 玩家可用的属性点
	INT				nTalentPoint;				// 玩家可用的天资点

	tagStrength		Strength;					// 实力值
	INT				nStrengthNum;				// 实力名次

	INT				nAttPointAdd[X_ERA_ATTA_NUM];// 玩家投放到各个一级属性中的值
	tagRoleTalent	talent[X_MAX_TALENT_PER_ROLE];// 玩家的天资

	BOOL			bSafeGuard;					// 是否被PK保护
	tagDWORDTime	CloseSafeGuardTime;			// 上次关闭PK保护的时间
	INT				nDeadUnSetSafeGuardCountDown;	//关闭玩家戾气为0时回城复活所加上的pk保护倒计tick
	DWORD           dwItemTransportMapID;           //当前记录传送符绑定地图ID
	FLOAT			fItemTransportX;				//当前记录传送符绑定的x坐标
	FLOAT			fItemTransportZ;				//当前记录传送符绑定的z坐标
	FLOAT			fItemTransportY;				//当前记录传送符绑定的y坐标

	INT16			n16BagSize;					// 背包大小
	INT				nBagGold;					// 背包中金子数
	INT				nBagSilver;					// 背包中银子数
	INT				nBagYuanBao;				// 背包中元宝数
	INT				nExVolume;					// 角色赠点数

	//INT16			n16WareSize;				// 账号仓库大小
	//INT				nWareGold;					// 账号仓库中金子数
	//INT				nWareSilver;				// 账号仓库中蝇子数
	//INT				nWareYuanBao;				// 账号仓库中元宝数

	DWORD			dwGuildID;					// 帮派id
	
	INT				nTotalTax;					// 缴税总额

	tagRemoteOpenSet sRemoteOpenSet;			// 对远端玩家公开信息设置
	UINT16			u16ActiveTitleID;			// 当前称号

	DWORD			dwTimeGetMallFree;			// 上次从商城领取免费物品时间

	INT	 			nSkillNum;					// 技能个数			-- tagSkillSave
	INT				nBuffNum;					// 状态个数			-- tagBuffSave

	INT				nTitleNum;					// 称号个数			-- tagTitleSave

	INT				nItemNum;					// 物品装备个数		-- tagItem,tagEquip
	INT				nFriendNum;					// 好友个数			-- 读: tagFriendSave, 存: 实时,即单独发消息
	INT				nEnemyNum;					// 仇敌个数			-- 读: DWORD dwEnemyID, 存: 实时,即单独发消息
	INT				nQuestNum;					// 任务列表个数		-- tagQuestSave
	INT				nQuestDoneNum;				// 已完成的任务个数 -- 读：tagQuestDoneSave，存：实时
	INT				nItemCDTimeNum;				// 物品冷却时间		-- tagCDTime
	INT				nFriendshipNum;				// 友好度			-- tagFriendshipSave
	INT				nBlackNum;					// 黑名单			-- DWORD
	BOOL			bClanDataChg;				// 氏族数据是否改变
	INT				nPetsNum;					// 宠物数目
	tagScriptData<ESD_Role>	Data;				// 玩家脚本数据
	INT				nTreasureSum;				// 玩家开启宝箱数

	DWORD			dwLastStallTime;			// 摊位最近更新时间
	INT32			nStallDailyExp;				// 摊位当日经验
	INT32			nStallCurExp;				// 摊位当前等级经验
	BYTE			byStallLevel;				// 摊位等级
	DWORD			dwLoverID;					// 爱人的roleid 没结婚就是4294967295
	BOOL			bHaveWedding;				// 是否举行过婚礼
	BOOL			bHasLeftMsg;				// 有留言
	
	BOOL			bSpeakOff;					// 禁言状态
	BOOL			bNeedPrisonRevive;			// 需要在牢狱复活


	/*********************************************************************************************************
	*技能列表，状态列表，称号列表，称号条件列表，物品列表，装备列表，好友列表，仇敌列表，任务列表，已完成任务列表，
	*物品冷却时间表, 友好度表，角色名帖(自定义)
	*必须按顺序拷贝
	*********************************************************************************************************/

	BYTE			byData[1];
};

struct tagRoleDataLoad
{
	DWORD				dwSize;					// 玩家信息大小
	tagRoleDataConst	RoleDataConst;
	tagRoleDataSave		RoleDataSave;
};

struct tagRecordDel // 删除的相关信息(两次保存间)
{
	INT	 	nSkillNum;		// 技能个数(dwSkillID):  所有原因产生的技能遗失,如果先遗忘后再学,此处也会记录
	INT		nBuffNum;		// 状态个数(dwBuffID):    从玩家身上消失过的所有状态
//	INT		nTalentNum;		// 天资列表(ETalentType): 洗点产生的
//	INT		nItemNum;		// 物品个数(n64Serial):   需摧毁的，即需从数据库中删除的 -- 实时
//	INT		nEquipNum;		// 装备个数(n64Serial):   需摧毁的，即需从数据库中删除的 -- 实时
	INT		nQuestNum;		// 任务个数(dwQuestID):   包括删除的和完成的

	/*********************************************************************************************************
	*技能列表，状态列表，物品列表，装备列表，任务列表
	*必须按顺序拷贝
	*********************************************************************************************************/
	BYTE	byData[1];		// 记录类型
};

//---------------------------------------------------------------------------------------------
// 人物技能
//---------------------------------------------------------------------------------------------
struct tagSkillSave
{
	DWORD	dwID;				// 技能ID
	INT		nSelfLevel;			// 自身等级
	INT		nLearnLevel;		// 投点等级
	INT		nCoolDown;		// 技能还需要的冷却时间
	INT     nProficiency;		// 技能熟练度
};

//---------------------------------------------------------------------------------------------
// 人物buff的基本信息
//---------------------------------------------------------------------------------------------
struct tagBuffSave
{
	DWORD		dwBuffID;					// ID
	INT32		nPersistTick;				// 当前持续时间

	INT8		n8Level;					// 等级
	INT8		n8CurLapTimes;				// 当前叠加次数
	INT8		n8ModifierNum;				// 影响它的技能个数
	INT8		n8Dummy[1];

	DWORD		dwSrcUnitID;				// 源Unit的ID
	DWORD		dwSrcSkillID;				// 由哪个技能产生（大ID）
	INT64		n64Serial;					// 由哪个物品或装备产生
	DWORD		dwItemTypeID;				// 物品或装备的属性ID

	BYTE		byData[1];
};

//---------------------------------------------------------------------------------------
// 好友
//---------------------------------------------------------------------------------------
struct tagFriendSave
{
	DWORD		dwFriendID;		// 好友id
	INT32		nGroupID;		// 分组
};

struct tagFriendshipSave
{
	DWORD		dwFriendID;		// 好友id
	INT32		nFriVal;		// 友好度
};

//-----------------------------------------------------------------------------
// 保存到数据库的条件数据
//-----------------------------------------------------------------------------
struct tagTitleSave
{
	UINT16			u16TitleID;			// 称号ID
	DWORD			dwStateMark;		// 若为0，则表示已获取，否则表示para2
};

//---------------------------------------------------------------------------------------
// 玩家各个级别所对应的基本属性及升到该级得到天资和升到下一级所需的经验增量 -- 从xml中读入
//---------------------------------------------------------------------------------------
struct tagLevelUpEffect
{
	INT		nExpLevelUp;		// 升到下一级所需的经验增量
	INT16	n16RoleAtt[6];		// 筋骨、劲力、元气、内力、技力、身法
	INT16	n16RoleAttAvail;	// 得到的可投属性点
	INT16	n16TalentAvail;		// 得到的可投天资点
	INT16	n16HP;				// 当前级别血量
	INT16	n16MP;				// 当前级别真气量
	INT16	n16Vitality;		// 当前级别活力
	INT16	n16Dummy;

	tagLevelUpEffect() { ZeroMemory(this, sizeof(*this)); nExpLevelUp = GT_INVALID; }
};


//---------------------------------------------------------------------------------------
// 玩家脚本数据类型定义
//---------------------------------------------------------------------------------------
enum ERoleScriptData
{
	ERSD_Injury			=	0,				// 0用作内伤治疗状态
	ERSD_END			=	100,
};
//-------------------------------------------------------------------------------------------
#pragma pack(pop)