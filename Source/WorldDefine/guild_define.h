#pragma once
#include "msg_common_errorcode.h"
#include "RoleDefine.h"
#include "ItemDefine.h"
#include <bitset>

#pragma pack(push, 1)

//-----------------------------------------------------------------------------
// 宏
//-----------------------------------------------------------------------------

// 可占领城镇数量=（帮派等级+3）/4
#define MGuildMaxHoldCityNum(byGuildLevel)		((byGuildLevel + 3) / 4)

// 最大成员数
#define MGuildMaxMember(byGuildLevel)			(50 + 25 * (byGuildLevel - 1))

// 资金
#define MGuildMaxFund(byGuildLevel)				(0)//??

// 资材
#define MGuildMaxMaterial(byGuildLevel)			(0)//??

// 每日维护费用(维护费用=1000×(帮派等级^3)×（2-帮派安定度/10000）)
#define MGuildDailyCost(byGuildLevel, nGuildPeace)	(1000.0f * byGuildLevel * byGuildLevel * byGuildLevel * (2.0f - nGuildPeace / 10000.0f))

// 每日安定度消耗
#define MGuildPeaceCost(byGuildLevel)			(byGuildLevel * 10)

// 当前俸禄标准
#define MGuildMemSalary()						(0)//??

// 堂 -- 帮众
#define MGuildPosTangZhongNum(byGuildLevel)		(4 + 2 * (byGuildLevel - 1))

// 基层 -- 精英帮众
#define MGuildPosJingZhongNum(byGuildLevel)		(5 + 2 * (byGuildLevel - 1))

// 基层 -- 普通帮众 = 总人数上限 - 当前帮派成员总数

// 帮派仓库当前空间
#define MGuildWareCurSpace(byGuildLevel)		((byGuildLevel + 1) / 2 * 35)

// 每日帮务发布最大次数
#define MGuildAffairTimes(byGuildLevel)			((byGuildLevel + 2) / 3)

// 帮派等级升级上限
#define MGuildSkillLevel(byAcademyLevel)		(20 + (2 * byAcademyLevel))

//-----------------------------------------------------------------------------
// 常量
//-----------------------------------------------------------------------------

// 客户端显示成员信息页一页显示的成员个数
const INT32 MAX_GUILDMEM_DISPLAY_PERPAGE	= 26;

// 帮派组织属性常量
const INT32 MAX_GUILD_LEVEL			=	10;										// 帮派最大等级	
const INT32 MAX_GUILD_HOLDCITY		=	MGuildMaxHoldCityNum(MAX_GUILD_LEVEL);	// 帮派最大辖属城市
const INT32 MAX_GUILD_WARE_SPACE	=	MGuildWareCurSpace(MAX_GUILD_LEVEL);	// 帮派仓库最大容量
const INT32 MAX_GUILD_NAME			=	20;										// 帮派名称最大字节数
const INT32 MAX_GUILD_TENET			=	400;									// 帮派宗旨最大字节数

const INT32 MAX_GUILD_FUND				=	99999999;		// 帮派资金
const INT32 MAX_GUILD_MATERIAL			=	999999;			// 帮派资材
const INT16 MAX_GUILD_PEACE				=	9999;			// 帮派安定度
const INT32 MAX_GUILD_REP				=	999999999;		// 帮派威望
const INT32 MAX_GUILD_GROUP_PURCHASE	=	100000;			// 帮派团购指数


// 帮派个人属性常量
const INT32 MAX_GUILDMEM_TOTALCONTRIB	=	999999999;	// 累计帮派贡献
const INT32 MAX_GUILDMEM_CURCONTRIB		=	999999999;	// 当前帮派贡献
const INT32 MAX_GUILDMEM_EXP			=	999999999;	// 帮派阅历
const INT32 MAX_GUILDMEM_EXPLOIT		=	999999999;	// 帮派功勋


// 帮派职位人数上限
const INT32 MAX_GUILDPOS_BANGZHU		=	1;	// 帮主
const INT32 MAX_GUILDPOS_FUBANGZHU		=	3;	// 副帮主
const INT32 MAX_GUILDPOS_HONGHUFA		=	1;	// 红衣护法
const INT32 MAX_GUILDPOS_ZIHUFA			=	1;	// 紫衫护法
const INT32 MAX_GUILDPOS_TANGZHU		=	1;	// 堂主
const INT32 MAX_GUILDPOS_XIANGZHU		=	1;	// 香主


const INT MAX_UPGRADE_NEED_ITEM_TYPE	=	4;  // 帮派设施升级所需物品种类
const INT MIN_GUILD_UPGRADE_ROLE_LEVEL	=	20; // 帮派设施升级角色等级下限

// 帮派技能最大等级
const INT MAX_GUILD_SKILL_LEVEL			=	40;

// 跑商相关常量
const INT MAX_COMMODITY_CAPACITY		=	10;		// 跑商任务容器容量
const INT MAX_COFC_GOODS_NUM			=	5;		// 商会出售商品最大数量
const INT MAX_COMMODITY_GOOD_NUM		=	255;	// 商货最大堆叠个数
const INT MAX_COMMERCE_RANK_INFO		=	10;		// 跑商排行榜列表长度
const INT MAX_COMMEND_PLAYER			=	3;		// 发放跑商嘉奖人数

//-----------------------------------------------------------------------------
// 枚举
//-----------------------------------------------------------------------------

// 错误码
enum
{
	E_Guild_TangZhu_Full				= 1,	// 指定职位上成员已到上限
	E_Guild_XiangZhu_Full				= 2,	// 指定职位上成员已到上限
	E_Guild_TangMember_Full				= 3,	// 指定职位上成员已到上限
	E_Guild_Member_Full					= 4,	// 帮派成员已到上限
	E_Guild_Pos_Full					= 5,	// 指定职位上成员已到上限

	E_Guild_Power_NotEnough				= 10,	// 权限不够
	E_Guild_CanNot_Leave				= 11,	// 不可脱离帮派
	E_Guild_CanNot_Demiss				= 12,	// 不能够辞职

	E_Guild_Join_AlreadyIn				= 20,	// 已加入帮派
	E_Guild_Join_LevelLimit				= 21,	// 等级不够
	E_Guild_Join_BeInvited				= 22,	// 已处于被邀请状态
	E_Guild_Join_NotBeInvited			= 23,	// 已未处于被邀请状态
	E_Guild_Join_BeRefused				= 24,	// 邀请被拒绝

	E_Guild_Create_LevelLimit			= 25,	// 等级不够
	E_Guild_Create_NameExist			= 26,	// 重名
	E_Guild_Create_Success				= 27,	// 创建成功
	E_Guild_Create_AlreadyIn			= 28,	// 已有帮派
	
	E_GuildSet_Tenet_Empty				= 30,	// 设置宗旨为空

	E_Guild_Appoint_SamePos				= 35,	// 任职前后职位相同
	E_Guild_Appoint_SexLimit			= 36,	// 性别要求不符

	E_Guild_NotExist					= 40,	// 指定帮派不存在
	E_Guild_MemberHasMore				= 41,	// 还有其他成员(成员个数>1)
	E_Guild_MemberNotIn					= 42,	// 没有找到指定成员
	E_Guild_Level_Limit					= 43,	// 帮派等级限制
	E_Guild_State_Limit					= 44,	// 帮派状态限制
	E_Guild_Fund_NotEnough				= 45,	// 帮派资金不足
	E_Guild_Material_NotEnough			= 46,	// 帮派资材不足
	E_Guild_HoldCity_Limit				= 47,	// 帮派占领城市限制

	E_GuildWare_NoChange				= 50,	// 帮派仓库无变化,不需要更新
	E_GuildWare_Power_NotEnough			= 51,	// 帮派仓库操作权限不够

	E_GuildUpgrade_Level_Limit			= 60,	// 设施达到等级上限
	E_GuildUpgrade_Item_NotEnough		= 61,	// 上缴的物品不符合升级需要
	E_GuildUpgrade_Role_Level           = 62,   // 角色等级不够，无法升级帮派

	E_GuildAffair_Times_Limit			= 70,	// 帮务发布次数限制

	E_GuildSkill_NotResearch			= 80,	// 当前没有技能研究
	E_GuildSkill_Wrong_Item				= 81,	// 上缴物品非法
	E_GuildSkill_Level_Limit			= 82,	// 技能达到等级上限
	E_GuildSkill_NotExist				= 83,	// 技能不存在

	E_GuildMember_Contribution_Limit	= 90,	// 成员贡献限制
	E_GuildMember_Level_Limit			= 91,	// 成员等级限制

	E_GuildCommerce_Status_Error		= 100,	// 跑商状态错误
	E_GuildCommerce_Tael_NotEnough		= 101,	// 商银不足
	E_GuildCommerce_Commend_Error		= 102,	// 嘉奖已经开启或关闭

	E_GuildCommodity_NotEnough_Space	= 103,	// 跑商容器空间不足
	E_GuildCommodity_NotEnough_Tael		= 104,	// 商银不足
	E_GuildCommodity_ItemMaxHold		= 105,	// 商货达到可拥有上限

	E_CofC_NotExist						= 110,	// 商会非法
	E_CofC_ItemNotFind					= 111,	// 没有找到指定商货
	E_CofC_ItemCannotSell				= 112,	// 指定商货不可出售
	E_CofC_ItemNotEnough				= 113,	// 商货个数不足
	E_CofC_HoldCity_Limit				= 114,	// 没有占领指定城市
};

// 状态
enum EGuildSpecState
{
	EGDSS_NULL				= 0x0000,
	EGDSS_Shortage			= 0x0001,			// 资金短缺
	EGDSS_Distress			= 0x0002,			// 贫困
	EGDSS_Chaos				= 0x0004,			// 混乱
	EGDSS_Warfare			= 0x0008,			// 征战
	EGDSS_Primacy			= 0x0010,			// 王者
	EGDSS_Refulgence		= 0x0020,			// 辉煌

	EGDSS_End				= 0xffff
};

// 职位
enum EGuildMemberPos
{
	EGMP_Null			= -1,
	EGMP_Start			= 0,

	EGMP_BangZhong		= 0,	//普通帮众
	EGMP_JingYing		= 1,	//精英帮众
	
	EGMP_BangZhu		= 2,	//帮主
	EGMP_FuBangZhu		= 3,	//副帮主
	EGMP_HongHuFa		= 4,	//红裳护法
	EGMP_ZiHuFa			= 5,	//紫衫护法

	EGMP_Tang_Start		= 6,
	EGMP_QingLongTang	= 6,	//青龙堂堂主
	EGMP_QingLongXiang	= 7,	//青龙堂香主
	EGMP_QingLongZhong	= 8,	//青龙堂帮众
	
	EGMP_ZhuQueTang		= 9,	//朱雀堂堂主
	EGMP_ZhuQueXiang	= 10,	//朱雀堂香主
	EGMP_ZhuQueZhong	= 11,	//朱雀堂帮众

	EGMP_BaiHuTang		= 12,	//白虎堂堂主
	EGMP_BaiHuXiang		= 13,	//白虎堂香主
	EGMP_BaiHuZhong		= 14,	//白虎堂帮众

	EGMP_XuanWuTang		= 15,	//玄武堂堂主
	EGMP_XuanWuXiang	= 16,	//玄武堂香主
	EGMP_XuanWuZhong	= 17,	//玄武堂帮众
	EGMP_Tang_End		= 17,

	EGMP_End			= EGMP_XuanWuZhong,
};

const INT32 X_GUILD_POS_NUM		= EGMP_End - EGMP_Start + 1;

// 堂
enum EGuildTangType
{
	EGTT_Null			= -1,
	EGTT_Start			= 0,

	EGTT_QingLong		= 0,	// 青龙堂
	EGTT_ZhuQue			= 1,	// 朱雀堂
	EGTT_BaiHu			= 2,	// 白虎堂
	EGTT_XuanWu			= 3,	// 玄武堂

	EGTT_End			= 3,
};

const INT32 X_GUILD_TANG_NUM	= EGTT_End - EGTT_Start + 1;

// 堂内职位
enum ETangMemberPos
{
	ETMP_Null			= -1,

	ETMP_TangZhu		= 0,	// 堂主
	ETMP_XiangZhu		= 1,	// 香主
	ETMP_BangZhong		= 2,	// 帮众
};

//-----------------------------------------------------------------------------
// 枚举合法性及相互间转化方法
//-----------------------------------------------------------------------------
static BOOL IsInTang(EGuildMemberPos eGuildPos)
{
	return (eGuildPos >= EGMP_Tang_Start && eGuildPos <= EGMP_Tang_End);
}

static BOOL IsGuildPosValid(EGuildMemberPos ePos)
{
	return (ePos >= EGMP_Start && ePos <= EGMP_End);
}

static VOID GetTangType(EGuildMemberPos eGuildPos, EGuildTangType &eTang, ETangMemberPos &eTangPos)
{
	if(!IsInTang(eGuildPos))
	{
		eTang		= EGTT_Null;
		eTangPos	= ETMP_Null;
		return;
	}

	switch(eGuildPos)
	{
	case EGMP_QingLongTang:
		eTang		= EGTT_QingLong;
		eTangPos	= ETMP_TangZhu;
		break;
	case EGMP_QingLongXiang:
		eTang		= EGTT_QingLong;
		eTangPos	= ETMP_XiangZhu;
		break;
	case EGMP_QingLongZhong:
		eTang		= EGTT_QingLong;
		eTangPos	= ETMP_BangZhong;
		break;

	case EGMP_ZhuQueTang:
		eTang		= EGTT_ZhuQue;
		eTangPos	= ETMP_TangZhu;			
		break;
	case EGMP_ZhuQueXiang:
		eTang		= EGTT_ZhuQue;
		eTangPos	= ETMP_XiangZhu;
		break;
	case EGMP_ZhuQueZhong:
		eTang		= EGTT_ZhuQue;
		eTangPos	= ETMP_BangZhong;
		break;

	case EGMP_BaiHuTang:
		eTang		= EGTT_BaiHu;
		eTangPos	= ETMP_TangZhu;
		break;
	case EGMP_BaiHuXiang:
		eTang		= EGTT_BaiHu;
		eTangPos	= ETMP_XiangZhu;
		break;
	case EGMP_BaiHuZhong:
		eTang		= EGTT_BaiHu;
		eTangPos	= ETMP_BangZhong;
		break;

	case EGMP_XuanWuTang:
		eTang		= EGTT_XuanWu;
		eTangPos	= ETMP_TangZhu;
		break;
	case EGMP_XuanWuXiang:
		eTang		= EGTT_XuanWu;
		eTangPos	= ETMP_XiangZhu;
		break;
	case EGMP_XuanWuZhong:
		eTang		= EGTT_XuanWu;
		eTangPos	= ETMP_BangZhong;
		break;
	}
}

//enum EGuildMemberAtt
//{
//	EGMA_Start				= 0,
//
//	EGMA_TotalContrib		= 0,	//累计帮派贡献
//	EGMA_Contribution		= 1,	//当前帮派贡献
//	EGMA_Exploit			= 2,	//当前帮派功勋
//	EGMA_Salary				= 3,	//当前俸禄标准
//
//	EGMA_End				= 3,
//};
//
//const INT32 X_GUILD_MEMBER_ATT_NUM		= EGMA_End - EGMA_Start + 1;

//-----------------------------------------------------------------------------
// 结构
//-----------------------------------------------------------------------------
struct tagGuildBase
{
	DWORD			dwID;							//帮派ID -- 该值用帮派名称CRC32值
	DWORD			dwFounderNameID;				//帮派创始人名称ID
	DWORD			dwLeaderRoleID;					//帮派帮主角色ID
	DWORD			dwSpecState;					//帮派当前特殊状态
	BYTE			byLevel;						//帮派当前等级
	BYTE			byHoldCity[MAX_GUILD_HOLDCITY];	//帮派当前辖属城市编号，如果最大帮派等级改变，需要重新考虑内存对齐问题

	INT32			nFund;							//帮派当前资金
	INT32			nMaterial;						//帮派当前资材
	INT32			nReputation;					//帮派当前威望
	INT32			nGroupPurchase;					//帮派团购指数
	INT16			n16Peace;						//帮派当前安宁度
	INT16			n16MemberNum;					//帮派当前成员数量	
	INT16			n16Rank;						//帮派当前排名
	BYTE			byMinJoinLevel;					//最低加入等级
	BYTE			byAffairRemainTimes;			//帮派事务剩余发布次数
	BOOL			bCommendation;					//帮派跑商嘉奖状态
	INT32			nDailyCost;						//帮派每日维护费用

	tagGuildBase()	{ ZeroMemory(this, sizeof(*this)); };
};

// 帮派成员
struct tagGuildMember
{
	DWORD			dwRoleID;						//帮派成员角色ID
	EGuildMemberPos	eGuildPos;						//角色帮派职位

	INT32			nTotalContribution;				//累计帮派贡献
	INT32			nContribution;					//当前帮派贡献
	INT32			nExploit;						//当前帮派功勋
	INT32			nSalary;						//当前俸禄标准

	BOOL			bUseGuildWare;					//使用帮派仓库权限

	tagGuildMember() {}
	
	tagGuildMember(DWORD dwRoleID, EGuildMemberPos ePos,
					INT32 nTotalContrib = 0, INT32 nContrib = 0, 
					INT32 nExploit = 0, INT32 nSalary = 0, BOOL bUseGuildWare = FALSE)
	{
		ASSERT(::IsGuildPosValid(ePos));

		this->dwRoleID				= dwRoleID;
		this->eGuildPos				= ePos;

		this->nTotalContribution	= nTotalContrib;
		this->nContribution			= nContrib;
		this->nExploit				= nExploit;
		this->nSalary				= nSalary;

		this->bUseGuildWare			= bUseGuildWare;
	}

	tagGuildMember(const tagGuildMember& sGuildMember)
	{
		ASSERT(::IsGuildPosValid(sGuildMember.eGuildPos));

		this->dwRoleID				= sGuildMember.dwRoleID;
		this->eGuildPos				= sGuildMember.eGuildPos;

		this->nTotalContribution	= sGuildMember.nTotalContribution;
		this->nContribution			= sGuildMember.nContribution;
		this->nExploit				= sGuildMember.nExploit;
		this->nSalary				= sGuildMember.nSalary;

		this->bUseGuildWare			= sGuildMember.bUseGuildWare;
	}
};

// 角色一般信息
struct tagRoleCommon
{
	INT16			n16RoleLevel;					//当前玩家等级
	BYTE			byRoleSex;						//当前玩家性别
	bool			bOnline;						//当前是否在线
	EClassType		eRoleClass;						//当前玩家职业
	DWORD			dwOfflineTime;					//当前离线时间
	INT32			nKnowledge;						//当前玩家帮派阅历
};


// 帮派成员信息
struct tagGuildMemInfo
{
	DWORD		dwRoleID;			// 成员名称
	INT8		n8GuildPos;			// 成员职位
	BYTE		byLevel;			// 成员等级
	BYTE		bySex;				// 成员性别
	BYTE		byClass;			// 成员职业
	INT32		nCurContrib;		// 成员当前帮派贡献
	DWORD		dwTimeLastLogout;	// 成员最后下线时间
	bool		bOnline;			// 是否在线
};

// 帮派成员信息扩展
struct tagGuildMemInfoEx
{
	// 个性化头像：同名帖界面中的头像 -- 发名帖相关消息
	INT32		nTotalContrib;		// 累计贡献：角色总帮派贡献
	INT32		nExploit;			// 帮派功勋
	INT32		nKnowledge;			// 帮派阅历: 不在线角色为-1
};


struct tagGuildMemberClient
{
	BOOL				bGetInfo;		// 是否以获取扩展信息
	tagGuildMemInfoEx	sInfoEx;		// 扩展信息
	tagGuildMemInfo		sInfo;			// 普通信息

	tagGuildMemberClient()
	{
		ZeroMemory(this, sizeof(tagGuildMemberClient));
	}
	tagGuildMemberClient(const tagGuildMemInfo& sMemInfo)
	{
		memcpy(&sInfo, &sMemInfo, sizeof(tagGuildMemInfo));
		ZeroMemory(&sInfoEx, sizeof(tagGuildMemInfoEx));
		bGetInfo = FALSE;
	}
	VOID Modify(const tagGuildMemInfoEx& sMemInfoEx)
	{
		memcpy(&sInfoEx, &sMemInfoEx, sizeof(tagGuildMemInfoEx));
		bGetInfo = TRUE;
	}
	VOID Modify(const tagGuildMemInfo& sMemInfo, const tagGuildMemInfoEx& sMemInfoEx)
	{
		memcpy(&sInfo, &sMemInfo, sizeof(tagGuildMemInfo));
		memcpy(&sInfoEx, &sMemInfoEx, sizeof(tagGuildMemInfoEx));
		bGetInfo = TRUE;
	}
};

//-----------------------------------------------------------------------------
// 结构 -- 操作权限，构造函数初始化为0，默认生成NullObject(服务器端见guild_mgr.h)
//-----------------------------------------------------------------------------
// 帮派职能
struct tagGuildPower
{
	DWORD	bDismissGuild		: 1,		// 解散帮派
			bInviteJoin			: 1,		// 邀请成员加入
			bTurnoverLeader		: 1,		// 移交帮主
			bLeaveGuild			: 1,		// 离开帮派
			bDemissPostion		: 1,		// 辞去帮中职务
			bModifyTenet		: 1,		// 修改宗旨
			bModifySgin			: 1,		// 修改标志
			bSetWareRights		: 1,		// 设置帮派仓库操作权限
			bUpgrade			: 1,		// 帮派设施升级
			bCommerce			: 1,		// 帮派跑商
			bSetCommend			: 1,		// 设置跑商嘉奖状态
			bAffair				: 1,		// 帮派事务发布
			bSetSkill			: 1,		// 设置帮派升级技能
			bAdvSkill			: 1;		// 帮派技能额外升级

	tagGuildPower()
	{
		ZeroMemory(this, sizeof(*this));
	}
};

// 开革权限表
struct tagGuildKick
{
	std::bitset<X_GUILD_POS_NUM>	BitSetGuildKick;

	tagGuildKick()
	{
		BitSetGuildKick.reset();
	}
};

// 任职权限表
struct tagGuildAppoint
{
	std::bitset<X_GUILD_POS_NUM>	BitSetGuildAppoint;

	tagGuildAppoint()
	{
		BitSetGuildAppoint.reset();
	}
};

//-----------------------------------------------------------------------------
// 结构 -- 帮派配置
//-----------------------------------------------------------------------------
struct tagGuildCfg
{
	// 创建&加入最低等级
	INT32		nCreateRoleMinLevel;
	INT32		nJoinRoleMinLevel;

	// 创建所需金钱
	INT32		nGoldCreateNeeded;

	// 创建后属性
	BYTE		byGuildLevelBegin;
	BYTE		byDummy;
	INT16		n16GuildPeaceBegin;
	INT32		nGuildRepBegin;
	INT32		nGuildFundBegin;
	INT32		nGuildMaterialBegin;
	INT32		nGuildGroupPurchaseBegin;
};


// 帮派仓库更新类型
enum EGuildWareUpdate
{
	EGWU_Null			= -1,

	EGWU_Change			= 0,	// 物品变更
	EGWU_Delete			= 1,	// 物品消失

	EGWU_End
};

//-----------------------------------------------------------------------------
// 结构 -- 帮派仓库更新单位
//-----------------------------------------------------------------------------
struct tagGuildWareUpdate
{
	EGuildWareUpdate	eType;
	INT16				nIndex;
	BYTE				byData[1];

	tagGuildWareUpdate()
	{
		ZeroMemory(this, sizeof(*this));
		eType = EGWU_Null;
	}

	INT Size()
	{
		INT nSize = sizeof(EGuildWareUpdate) + sizeof(INT16);

		switch (eType)
		{
		case EGWU_Change:
			{
				// 上层保证byData值有效
				tagItem* pItem = (tagItem*)byData;
				if (MIsEquipment(pItem->dwTypeID))
				{
					nSize += sizeof(tagEquip);
				}
				else
				{
					nSize += sizeof(tagItem);
				}
			}
			break;

		case EGWU_Delete:
			break;

		default:
			nSize = 0;
			break;
		}

		return nSize;
	}
};

//-----------------------------------------------------------------------------
// 结构 -- 帮派仓库权限列表单位
//-----------------------------------------------------------------------------
struct tagGuildWarePri 
{
	DWORD	dwRoleID;
	BOOL	bCanUse;
};

//-----------------------------------------------------------------------------
// 帮派设施类型
//-----------------------------------------------------------------------------
enum EFacilitiesType
{
	EFT_Null		= -1,

	EFT_Guild		= 0,		// 帮派
	EFT_Fund		= 1,		// 金库
	EFT_Material	= 2,		// 资财库
	EFT_Academy		= 3,		// 书院

	EFT_End
};

const INT MAX_GUILD_FACILITIES_TYPE = EFT_End - EFT_Null - 1;

//-----------------------------------------------------------------------------
// 结构 -- 帮派设施信息
//-----------------------------------------------------------------------------
struct tagGuildFacilitiesInfo
{
	EFacilitiesType	eType;
	INT16	nProgress;
	INT16	nFulfill;
	DWORD	dwItemID[MAX_UPGRADE_NEED_ITEM_TYPE];
	INT32	nNeedNum[MAX_UPGRADE_NEED_ITEM_TYPE];
	INT32	nNeedFund;
	INT32	nMaterial;
	BYTE	byLevel;

	tagGuildFacilitiesInfo()
	{
		ZeroMemory(this, sizeof(*this));
	}

	BOOL IsLevelUp()
	{
		return nProgress >= nFulfill;
	}

	VOID ResetItemInfo()
	{
		for (int n=0; n<MAX_UPGRADE_NEED_ITEM_TYPE; n++)
		{
			dwItemID[n]	= GT_INVALID;
			nNeedNum[n] = 0;
		}
	}
};

//-----------------------------------------------------------------------------
// 结构 -- 帮派技能升级信息
//-----------------------------------------------------------------------------
struct tagGuildSkill 
{
	DWORD		dwSkillID;
	INT16		n16Progress;
	INT16		n16Fulfill;
	INT32		nResearchFund;
	INT32		nResearchMaterial;

	INT32		nLearnSilver;
	INT32		nLearnContribution;
	INT32		nLearnFund;
	INT32		nLearnMaterial;

	INT			nLevel;

	tagGuildSkill()
	{
		ZeroMemory(this, sizeof(*this));
	}
	tagGuildSkill(const tagGuildSkill &rhs)
	{
		memcpy(this, &rhs, sizeof(*this));
	}
};

//-----------------------------------------------------------------------------
// 结构 -- 帮派跑商初始信息
//-----------------------------------------------------------------------------
struct tagTaelInfo 
{
	INT32	nDeposit;		// 保证金
	INT32	nBeginningTael;	// 初始商银
	INT32	nPurposeTael;	// 指标商银
	INT32	nMaxTael;		// 商银上限
};

//-----------------------------------------------------------------------------
// 结构 -- 帮派跑商排行榜
//-----------------------------------------------------------------------------
struct tagCommerceRank 
{
	DWORD		dwRoleID;
	INT			nTimes;
	INT32		nTael;
};

//-----------------------------------------------------------------------------
// 结构 -- 帮派跑商商货
//-----------------------------------------------------------------------------
struct tagCommerceGoodInfo
{
	DWORD	dwGoodID;
	INT32	nCost;
	BYTE	byGoodNum;

	tagCommerceGoodInfo()
	{
		dwGoodID	= GT_INVALID;
		nCost		= 0;
		byGoodNum	= 0;
	}

	BOOL IsValid()
	{
		if (!GT_VALID(dwGoodID) || (dwGoodID == 0) || (byGoodNum == 0))
		{
			return FALSE;
		}

		return TRUE;
	}
};

//-----------------------------------------------------------------------------
#pragma pack(pop)