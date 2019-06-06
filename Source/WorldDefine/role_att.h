//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: role_att.h
// author: 
// actor:
// data: 2008-07-21
// last:
// brief: 人物属性获取、变化等消息
//-----------------------------------------------------------------------------
#pragma once

#include "msg_common_errorcode.h"
#include "RoleDefine.h"
#include "talent_define.h"
#include "skill_define.h"
#include "buff_define.h"
#include "QuestDef.h"
#include "suit_define.h"
#include "SocialDef.h"
#include "pet_define.h"
#include "guild_define.h"
#pragma pack(push, 1)

enum
{
	E_BidAttPoint_PointInvalid			=	1,		// 投点不合法
	E_BidAttPoint_NoEnoughAttPoint		=	2,		// 没有足够的属性点

	E_ClearAttPoint_ItemNotValid		=	10,		// 使用的物品不合法
};

enum ERoleInitType
{
	ERIT_Null = -1,
	
	ERIT_Att				=	0,			// 人物基本属性
	ERIT_Skill				=	1,			// 人物技能
	ERIT_Item				=	2,			// 所有物品
	ERIT_ClientConfig		=	3,			// 客户端配置
	ERIT_FrindAndEnemy		=	4,			// 和所有好友的同步率,以及有哪些仇敌，黑名单
	ERIT_CompleteQuest		=	5,			// 已经完成的任务
	ERIT_IncompleteQuest	=	6,			// 没有完成的任务
	ERIT_Money				=	7,			// 金钱
	ERIT_Reputation			=	8,			// 声望
	ERIT_Guild				=	9,			// 帮派
	ERIT_End,
};

enum EClassErrorCode
{
    ECEC_Success            =   E_Success,

    ECEC_NotUpToLevel       =   1,          // 玩家等级不够
    ECEC_IsNotBaseClass     =   2,          // 只有少侠才能进行职业专精
    ECEC_LackOfNewclassItem =   3,          // 缺少职业专精凭证道具
    ECEC_LackOfReclassItem  =   4,          // 缺少更换专精职业im道具
	ECEC_IsNotBaseClassEx	=	5,			// 已就职了英雄职业
	ECEC_ClassNotMatch		=	6,			// 英雄职业与专精职业不匹配
	ECEC_ClassNotSet		=	7,			// 未就职专精职业，不可进行职业转换
	ECEC_NPCError			=	8,			// NPC无效

    ECEC_End,
};

//-----------------------------------------------------------------------------
// 状态改变
//-----------------------------------------------------------------------------
CMD_START(NS_SetState)
	DWORD		dwRoleID;
	EState		eState;
CMD_END

CMD_START(NS_UnSetState)
	DWORD		dwRoleID;
	EState		eState;
CMD_END

//-----------------------------------------------------------------------------
// 玩家状态改变
//-----------------------------------------------------------------------------
CMD_START(NS_SetRoleState)
	DWORD		dwRoleID;						// 角色ID
	ERoleState	eState;							// 状态标志
CMD_END

CMD_START(NS_UnSetRoleState)
	DWORD		dwRoleID;						// 角色ID
	ERoleState	eState;							// 状态标志
CMD_END

CMD_START(NS_SetRoleStateEx)
	ERoleStateEx	eState;							// 状态标志
CMD_END

CMD_START(NS_UnSetRoleStateEx)
	ERoleStateEx	eState;							// 状态标志
CMD_END

//-----------------------------------------------------------------------------
// 玩家显示设置改变
//-----------------------------------------------------------------------------
CMD_START(NC_Fashion)		// 设置时装模式
	bool			bFashion;	// 是否使用使装模式
CMD_END

CMD_START(NS_AvatarEquip)	// 当前外观
	DWORD			dwRoleID;
	tagAvatarEquip	sAvatarEquip;
	bool			bFashion;	// 是否使用使装模式
CMD_END

CMD_START(NC_RoleSetDisplay)	// 设置部位显示属性
	tagDisplaySet	sDisplaySet;
CMD_END

CMD_START(NS_RoleSetDisplay)
	DWORD			dwRoleID;
	tagDisplaySet	sDisplaySet;
CMD_END

//-----------------------------------------------------------------------------
// 玩家个人设置改变
//-----------------------------------------------------------------------------
CMD_START(NC_SetPersonalSet)				// 修改本地玩家的个人设置
	tagDWPlayerPersonalSet	PersonalSet;
CMD_END;

CMD_START(NS_SetPersonalSet)				// 返回设置
	tagDWPlayerPersonalSet	PersonalSet;
CMD_END;

//-----------------------------------------------------------------------------
// 获取本地玩家初始结构
//-----------------------------------------------------------------------------
CMD_START(NC_GetRoleInitState)
	ERoleInitType eType;					// 客户端需要得到数据类型
CMD_END

//-----------------------------------------------------------------------------
// 返回人物初始属性
//-----------------------------------------------------------------------------
CMD_START(NS_GetRoleInitState_Att)
	tagAvatarAtt	Avatar;							// 外观
	tagAvatarEquip	AvatarEquip;					// 装备外观
	tagDisplaySet	DisplaySet;						// 外观显示设置
	INT				nAtt[ERA_End];					// 当前人物属性
	INT				nAttPointAdd[X_ERA_ATTA_NUM];	// 玩家投放到各个一级属性中的值
	INT				nLevel;							// 等级
	INT				nCurLevelExp;					// 当前升级经验
	INT				nCredit;						// 信用度
	INT				nIdentity;						// 身份
	INT				nVIPPoint;						// 会员积分
	DWORD			dwState;						// 状态标志位
	DWORD			dwRoleState;					// 玩家独有状态标志位 -- 需广播给周围玩家
	DWORD			dwRoleStateEx;					// 玩家独有状态标志位 -- 只须自己知道
	ERolePKState	ePKState;						// PK状态
	DWORD			dwRebornMapID;					// 复活地图ID
	Vector3			vRebornPoint;					// 复活点
	DWORD           dwTransportID;                  // 当前记录传送符绑定地图ID
	Vector3         vTransportPoint;                // 当前记录传送符绑定地图坐标
	DWORD			dwGuildID;						// 帮派id
	UINT16			u16ActTitleID;					// 当前激活称号ID
	DWORD			dwLoverID;						// 爱人对应的roleid 没结婚则为GT_INVALID
	BOOL			bHaveWedding;					// 是否举行过婚礼
	INT				nBuffNum;						// 状态数量
	BOOL			bHasLeftMsg;					// 有留言
    EClassType      eClassType;                     // 专精职业
    EClassTypeEx    eClassTypeEx;                   // 英雄职业(未开放)
	BYTE			byStallLevel;					// 摊位等级
	tagBuffMsgInfo	Buff[1];						// 状态列表
CMD_END

//-----------------------------------------------------------------------------
// 返回人物技能列表
//-----------------------------------------------------------------------------
CMD_START(NS_GetRoleInitState_Skill)
	tagRoleTalent		Talent[X_MAX_TALENT_PER_ROLE];	// 天资
	INT					nNum;							// 技能个数
	tagSkillMsgInfo		Skill[1];						// 技能列表
CMD_END

//-----------------------------------------------------------------------------
// 已经完成的任务 不定长
//-----------------------------------------------------------------------------
CMD_START(NS_GetRoleInitState_CompleteQuest)
	INT					nNum;						// 任务个数
	tagCompleteQuestMsgInfo completeQuest[1];
CMD_END

//-----------------------------------------------------------------------------
// 当前任务
//-----------------------------------------------------------------------------
CMD_START(NS_GetRoleInitState_IncompleteQuest)
	INT					nNum;						// 任务个数
	tagIncompleteQuestMsgInfo incompleteQuest[1];
CMD_END

//-----------------------------------------------------------------------------
// 所有物品装备
//-----------------------------------------------------------------------------
CMD_START(NS_GetRoleInitState_Item)
	INT16	n16SzBag;					// 背包大小
	INT16	n16SzRoleWare;				// 仓库大小
	INT		nNum;						// 物品装备个数
	BYTE	byData[1];					// 技能列表
CMD_END

//-----------------------------------------------------------------------------
// 套装
//-----------------------------------------------------------------------------
CMD_START(NS_GetRoleInitState_Suit)
	INT		nSuitNum;
	BYTE	byData[1];					// tagSuitInit
CMD_END

//-----------------------------------------------------------------------------
// 所有物品冷却时间
//-----------------------------------------------------------------------------
CMD_START(NS_GetRoleInitState_ItemCDTime)
	INT		nNum;						// 冷却时间物品(TypeID)个数
	BYTE	byData[1];					// tagCDTime
CMD_END

//-----------------------------------------------------------------------------
// 所有金钱
//-----------------------------------------------------------------------------
CMD_START(NS_GetRoleInitState_Money)
	INT32	nBagYuanBao;
	INT64	n64BagSilver;
	INT64	n64WareSilver;
	INT32	nBaiBaoYuanBao;
	INT32	nExchangeVolume;
	INT32	nCurClanCon[ECLT_NUM];
CMD_END

//-----------------------------------------------------------------------------
// 所有声望
//-----------------------------------------------------------------------------
CMD_START(NS_GetRoleInitState_Reputation)
	INT		nReputation[ECLT_NUM];
    BYTE    nActiveCount[ECLT_NUM];	
    BYTE    bisFame[ECLT_NUM];
CMD_END


//-----------------------------------------------------------------------------
// 龙魂能力
//-----------------------------------------------------------------------------
CMD_START(NS_GetRoleInitState_Longhun)
	INT8	n8Num;
	INT16	n16EquipPos[1];				// 龙魂能力被激活的装备位
CMD_END

//-----------------------------------------------------------------------------
// 所在帮派信息及个人信息
//-----------------------------------------------------------------------------
CMD_START(NS_GetRoleInitState_Guild)
	tagGuildBase	sGuildBase;			// 帮派基本属性
	tagGuildMember	sGuildMember;		// 个人信息
CMD_END

//------------------------------------------------------------------------------
// 获取远程玩家当前属性
//------------------------------------------------------------------------------
CMD_START(NC_GetRemoteRoleState)
	INT		nRoleNum;
	DWORD	dwRoleID[1];	// 可能是远程人物，也可能是远程怪物，一次发送最多50个
CMD_END

//-------------------------------------------------------------------------------------------
// 客户端远程玩家同步数据消息结构
//-------------------------------------------------------------------------------------------
struct tagRemoteRoleData
{
	DWORD				dwID;						// 玩家ID
	INT					nLevel;						// 等级
	FLOAT				fPos[3];					// 位置
	FLOAT				fFaceTo[3];					// 朝向
	DWORD				dwState;					// 状态标志位
	DWORD				dwRoleState;				// 角色独有状态标志位
	ERolePKState		ePKState;					// PK状态
	DWORD				dwGuildID;					// 帮派id(GT_INVALID表示没有加入帮派)
	INT8				n8GuildPos;					// 帮派职位
	UINT16				u16CurActTitleID;			// 激活称号(GT_INVALID表示没有激活称号)
	BYTE				byStallLevel;				// 摊位模型等级
	DWORD				dwMountPetID;				// 骑乘宠物id
	DWORD				dwMountPetTypeID;			// 骑乘宠物typeid
	tagDisplaySet		sDisplaySet;				// 装备外观显示设置
	tagAvatarAtt		Avatar;						// 外观
	tagAvatarEquip		AvatarEquip;				// 装备外观
	INT					nAtt[ERRA_End];				// 远程玩家基本属性
	tagRemoteOpenSet	sRemoteOpenSet;				// 对远端玩家公开信息设置
    EClassType          eClassType;                 // 专精职业
    EClassTypeEx        eClassTypeEx;               // 英雄职业
    DWORD			    dwLoverID;					// 爱人对应的roleid 没结婚则为GT_INVALID
    BOOL			    bHaveWedding;				// 是否举行过婚礼
	INT					nBuffNum;					// 状态数量
	tagBuffMsgInfo		Buff[1];					// 状态列表
};

CMD_START(NS_GetRemoteRoleState)
	tagRemoteRoleData		RoleData;		// 远程玩家结构，可能到后期也会变成不定长结构
CMD_END

//---------------------------------------------------------------------
// 远程生物属性结构
//---------------------------------------------------------------------
struct tagRemoteCreatureData
{
	DWORD			dwID;						// 生物ID
	DWORD			dwTypeID;					// 属性ID
	INT				nLevel;						// 等级
	DWORD			dwState;					// 状态标志位
	FLOAT			fPos[3];					// 位置
	FLOAT			fFaceTo[3];					// 朝向
	INT				nAtt[ERRA_End];				// 远程玩家基本属性
	DWORD			dwMapObjID;					// 地图对象ID	
	BOOL			bOpen;						// 门初始状态
	DWORD			dwTaggedOwner;				// 
	INT				nBuffNum;					// 状态数量
	tagBuffMsgInfo	Buff[1];					// 状态列表
};

CMD_START(NS_GetRemoteCreatureState)
	tagRemoteCreatureData	CreatureData;		// 远程生物结构
CMD_END

//---------------------------------------------------------------------
// 远程宠物属性结构
//---------------------------------------------------------------------
struct tagRemotePetData
{
    DWORD           dwRoleID;                   // 宠物所属的角色id，为-1时代表有问题
	DWORD			dwID;						// 生物ID
	DWORD			dwProtoID;					// 原型ID
	UPetState		uState;						// 形态
};

CMD_START(NS_GetRemotePetState)
	tagRemotePetData	PetData;
CMD_END

//------------------------------------------------------------------------------
// 本地玩家属性改变
//------------------------------------------------------------------------------
CMD_START(NS_RoleAttChangeSingle)
	ERoleAttribute	eType;
	INT				nValue;
CMD_END

struct tagRoleAttValue
{
	ERoleAttribute	eType;
	INT				nValue;
};

CMD_START(NS_RoleAttChangeMutiple)
	INT				nNum;
	tagRoleAttValue	value[1];
CMD_END

//-------------------------------------------------------------------------------
// 远程玩家以及远程生物属性改变
//------------------------------------------------------------------------------
CMD_START(NS_RemoteAttChangeSingle)
	DWORD			dwRoleID;
	ERemoteRoleAtt	eType;
	INT				nValue;
CMD_END

struct tagRemoteAttValue
{
	ERemoteRoleAtt	eType;
	INT				nValue;
};

CMD_START(NS_RemoteAttChangeMutiple)
	DWORD				dwRoleID;
	INT					nNum;
	tagRemoteAttValue	value[1];
CMD_END

//--------------------------------------------------------------------------------
// 玩家经验改变
//--------------------------------------------------------------------------------
CMD_START(NS_RoleExpChange)
	INT		nExp;				// 当前
	INT		nChange;			// 变化多少
	BOOL	bKill;				// 是否通过击杀怪物获得
CMD_END

//--------------------------------------------------------------------------------
// 玩家声望
//--------------------------------------------------------------------------------
CMD_START(NS_RoleReputeChange)
	BYTE	byClanType;			// 氏族
	INT		nRepute;			// 当前
	INT		nChange;			// 变化多少
CMD_END

//-----------------------------------------------------------------------------
// 名人状态改变
//-----------------------------------------------------------------------------
CMD_START(NS_BecomeFame)
    BYTE    eClanType;
CMD_END

//-----------------------------------------------------------------------------
// 远程玩家帮派信息变化
//-----------------------------------------------------------------------------
CMD_START(NS_RemoteRoleGuildInfoChange)
	DWORD	dwRoleID;
	DWORD	dwGuildID;					// 帮派id(GT_INVALID表示没有加入帮派)
	INT8	n8GuildPos;					// 帮派职位
CMD_END

//-----------------------------------------------------------------------------
// 氏族珍宝激活次数改变
//-----------------------------------------------------------------------------
CMD_START(NS_TreasureActCountChange)
    BYTE    eClanType;
    BYTE    nActCount;
CMD_END

//-----------------------------------------------------------------------------
// 氏族贡献达到上限
//-----------------------------------------------------------------------------
CMD_START(NS_ClanConUpperLimit)
	BYTE    eClanType;
CMD_END

//--------------------------------------------------------------------------------
// 玩家等级改变
//--------------------------------------------------------------------------------
CMD_START(NS_RoleLevelChange)
	DWORD	dwRoleID;
	INT		nLevel;
	BOOL	bKill;				// 是否因为击杀怪物产生变化
CMD_END

//--------------------------------------------------------------------------------
// 玩家属性投点
//--------------------------------------------------------------------------------
CMD_START(NC_RoleBidAttPoint)
	INT	nAttPointAdd[X_ERA_ATTA_NUM];
CMD_END

CMD_START(NS_RoleBidAttPoint)
	DWORD dwErrorCode;	// 错误码
CMD_END

//---------------------------------------------------------------------------------
// 玩家属性洗点
//---------------------------------------------------------------------------------
CMD_START(NC_RoleClearAttPoint)
	INT64 n64ItemID;				// 洗点道具
CMD_END

CMD_START(NS_RoleClearAttPoint)
	DWORD dwErrorCode;				// 错误码
CMD_END

//---------------------------------------------------------------------------------
// 玩家各个一级属性投放值改变
//---------------------------------------------------------------------------------
CMD_START(NS_RoleAttPointAddChange)
	INT	nAttPointAdd[X_ERA_ATTA_NUM];
CMD_END

//---------------------------------------------------------------------------------
// 玩家好友和黑名单
//---------------------------------------------------------------------------------
CMD_START(NS_SendFriendBlackList)
	DWORD			dwRolesID[MAX_BLACKLIST];
	INT				nNum;		  // 好友数量
	tagFriendInfo	FriendInfo[1];
CMD_END;

//---------------------------------------------------------------------------------
// 开/关门
//---------------------------------------------------------------------------------
CMD_START(NS_OpenDoor)
	DWORD			dwRoleID;
CMD_END

CMD_START(NS_CloseDoor)
	DWORD			dwRoleID;
CMD_END

//---------------------------------------------------------------------------------
// 怪物所属改变
//---------------------------------------------------------------------------------
CMD_START(NS_CreatureTaggedOwnerChange)
	DWORD			dwCreatureID;
	DWORD			dwTaggedOwner;
CMD_END

//---------------------------------------------------------------------------------
// NPC对话
//---------------------------------------------------------------------------------
CMD_START(NC_NPCTalk)
	DWORD			dwNPCId;
CMD_END

//----------------------------------------------------------------------------------
// 向服务器请求对方的装备
//----------------------------------------------------------------------------------
CMD_START(NC_GetRemoteRoleEquipInfo)
	DWORD			dwRoleID;
CMD_END

CMD_START(NS_GetRemoteRoleEquipInfo)
	DWORD			dwRoleID;					
	INT				nEquipNum;					//装备个数
	BYTE			byEquip[1];					//装备
CMD_END

//----------------------------------------------------------------------------------
// 向服务器请求职业专精
//----------------------------------------------------------------------------------
CMD_START(NC_SetClass)
    DWORD           dwNpcId;    // npc id
    EClassType      eClass;
CMD_END

CMD_START(NS_SetClass)
    EClassErrorCode dwErrCode;  // E_SUCCESS when Okey
CMD_END

//----------------------------------------------------------------------------------
// 向服务器更换专精职业
//----------------------------------------------------------------------------------
CMD_START(NC_ChangeClass)
    DWORD           dwNpcId;    // npc id
    EClassType      eClass;
CMD_END

CMD_START(NS_ChangeClass)
    EClassErrorCode dwErrCode;  // E_SUCCESS when Okey
CMD_END

//-----------------------------------------------------------------------------
// 角色专精职业发生改变，给周围玩家广播
//-----------------------------------------------------------------------------
CMD_START(NS_RoleClassChangeBroadcast)
    DWORD			dwRoleID;
    EClassType	    eClass;
CMD_END;

//------------------------------------------------------------------------------
// 角色改名
//------------------------------------------------------------------------------
CMD_START(NC_LPRename)
	INT64 n64RenameItemID;		// 易名贴
	TCHAR szNewName[X_SHORT_NAME];			// 玩家新名字
CMD_END

CMD_START(NS_LPRename)
	DWORD dwRoleID;				// 角色ID
	DWORD dwErrorCode;
	TCHAR szNewName[X_SHORT_NAME];			// 玩家新名字
	TCHAR szOldName[X_SHORT_NAME];			// 玩家旧名字
CMD_END

//-----------------------------------------------------------------------------
// 玩家名字改变，给周围玩家广播
//-----------------------------------------------------------------------------
CMD_START(NS_RoleRename)
	DWORD dwRoleID;				// 玩家ID
	TCHAR szNewName[1];			// 玩家新名字
CMD_END
	
#pragma pack(pop)
