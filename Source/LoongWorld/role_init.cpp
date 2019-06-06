//-------------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: role_init.cpp
// author: 
// actor:
// data: 2008-8-11
// last:
// brief: 初始化人物数据结构
//-------------------------------------------------------------------------------
#include "StdAfx.h"

#include "../ServerDefine/base_define.h"
#include "../ServerDefine/role_data_define.h"
#include "../ServerDefine/log_cmdid_define.h"
#include "../WorldDefine/msg_item.h"
#include "../WorldDefine/msg_virgin_online.h"
#include "../WorldDefine/msg_activity.h"

#include "role.h"
#include "att_res.h"
#include "skill.h"
#include "quest.h"
#include "quest_mgr.h"
#include "map_creator.h"
#include "script_mgr.h"
#include "title_mgr.h"
#include "pet_pocket.h"
#include "vip_netbar.h"
#include "guild.h"
#include "guild_mgr.h"

//-------------------------------------------------------------------------------
// 初始化玩家
//-------------------------------------------------------------------------------
VOID Role::Init(const tagRoleDataLoad* pData)
{
	ASSERT( P_VALID(pData) );

	const tagRoleDataConst* pDataConst = &pData->RoleDataConst;
	const tagRoleDataSave* pDataSave = &pData->RoleDataSave;

	// 初始化玩家脚本
	m_pScript = g_ScriptMgr.GetRoleScript();

	// 玩家脚本数据
	IFASTCODE->MemCpy(m_ScriptData.dwData, pDataSave->Data.dwData, sizeof(DWORD)*ESD_Role);

	// 称号管理器
	m_pTitleMgr		= new TitleMgr;

	// 新建宠物带
	m_pPetPocket	= new PetPocket;
	
	// 初始化常规属性
	IFASTCODE->MemCpy(&m_Avatar, &pDataConst->Avatar, sizeof(m_Avatar));
	m_DisplaySet = pDataSave->DisplaySet;
	m_dwRebornMapID = pDataSave->dwRebornMapID;
	m_CreatTime = pDataConst->CreateTime;
	m_LoginTime = g_world.GetWorldTime();		// 重置上线时间
	m_LogoutTime = pDataSave->LogoutTime;
	m_nOnlineTime = pDataSave->nOnlineTime;
	m_nCurOnlineTime = pDataSave->nCurOnlineTime;
	m_eClass = pDataSave->eClass;
	m_eClassEx = pDataSave->eClassEx;
	
	// 初始化角色的实力值及其各子值
	m_Strength.dwValue = pData->RoleDataSave.Strength.dwValue;
	m_nStrengthNum = pData->RoleDataSave.nStrengthNum;
	m_Strength.m_dwHeroism = pData->RoleDataSave.Strength.m_dwHeroism;
	m_Strength.m_dwWit = pData->RoleDataSave.Strength.m_dwWit;
	m_Strength.m_dwErrantry = pData->RoleDataSave.Strength.m_dwErrantry;
	m_Strength.m_dwValor = pData->RoleDataSave.Strength.m_dwValor;
		
	m_bHasLeftMsg = pDataSave->bHasLeftMsg;

	// 初始化上次获取商城免费物品时间
	SetLastGetMallFreeTime(pDataSave->dwTimeGetMallFree);

	// 根据行囊是否加密设置行囊限制
	m_RoleStateEx.SetState(ERSE_BagPsdExist);
	if(GetSession() && !GetSession()->IsHaveBagPsd())
	{
		m_RoleStateEx.UnsetState(ERSE_BagPsdExist);
		m_RoleStateEx.SetState(ERSE_BagPsdPass);
	}

	// 初始化称号选项
	m_pTitleMgr->InitOpts(this, pDataSave->u16ActiveTitleID, pDataSave->sRemoteOpenSet.bTitle);

	// PK保护相关
	m_CloseSafeGuardTime = pDataSave->CloseSafeGuardTime;
	m_nDeadUnSetSafeGuardCountDown = pDataSave->nDeadUnSetSafeGuardCountDown;

	m_bNeedPrisonRevive = pDataSave->bNeedPrisonRevive;		//需要牢狱复活

	IFASTCODE->MemCpy(m_Talent, pDataSave->talent, sizeof(m_Talent));

	if(pDataSave->bSafeGuard)
	{
		SetRoleState(ERS_Safeguard, FALSE);
	}

	// 帮派
	m_dwGuildID = pDataSave->dwGuildID;
	if (GT_VALID(m_dwGuildID))
	{
		// 检查玩家是否在帮派中
		Guild* pGuild	= g_guildMgr.GetGuild(m_dwGuildID);
		if (!P_VALID(pGuild) || !P_VALID(pGuild->GetMember(GetID())))
		{
			SetGuildID(GT_INVALID);
		}
		else
		{
			// 跑商状态
			GuildCommodity* pCommodity = pGuild->GetGuildCommerce().GetCommodity(m_dwID);
			if (P_VALID(pCommodity))
			{
				SetRoleState(ERS_Commerce, FALSE);
			}
		}
	}
	
	// 读取并设置人物数据库属性
	InitAtt(pDataSave);

	// todo：读取技能列表，装备列表，buff列表，并计算各自对人物属性的影响
	/*********************************************************************************************************
	*技能列表，状态列表，称号列表，称号条件列表，物品列表，装备列表，好友列表，仇敌列表，任务列表，已完成任务列表，角色名帖
	*必须按顺序读取(该顺序和tagRoleDataSave中对应)
	*********************************************************************************************************/

	const BYTE *pDBData = &pDataSave->byData[0];	// 列表数据指针,该指针需在读取函数中移动,即pDBData的值是变化的

	// 初始化技能列表
	InitSkill(pDBData, pDataSave->nSkillNum);

	// 初始化状态列表
	InitBuff(pDBData, pDataSave->nBuffNum);

	// 初始化称号列表
	m_pTitleMgr->InitTitles(pDBData, pDataSave->nTitleNum);

	// 初始化物品装备列表
	InitItem(pDBData, pDataSave->nItemNum);

	// 初始化好友仇敌列表
	InitFriend(pDBData, pDataSave->nFriendNum);
	
	// 初始化玩家当前任务列表
	InitCurrentQuest(pDBData, pDataSave->nQuestNum);

	// 初始化玩家完成任务列表
	InitCompleteQuest(pDBData, pDataSave->nQuestDoneNum);

	// 初始化物品冷却时间
	InitItemCDTime(pDBData, pDataSave->nItemCDTimeNum);

	// 初始化好友度
	InitFriendValue(pDBData, pDataSave->nFriendshipNum);

	// 初始化黑名单
	InitBlackList(pDBData, pDataSave->nBlackNum);

	// 初始化角色名贴
	m_VCard.Init(pDBData, this);

	// 初始化氏族数据
	m_ClanData.Init(pDBData, this);

	// 初始化宠物带
	m_pPetPocket->Init(pDBData, pDataSave->nPetsNum, this);

	// 所有内容已经加载完毕，重新计算人物初始当前属性
	CalInitAtt();

	// 计算所有状态
	CalInitState();

	g_VipNetBarMgr.PlayerNotify(GetSession()->GetSessionID());

	// 初始化角色开宝箱相关状态
	InitChestState();

	// 如果大于30级，升级时不需要检测关闭pk保护
	if ( m_nLevel >= 30 )
	{
		m_bLevelUpClosedSafeGuard = TRUE;
	}

	tagNS_RoleInitOk send;
	GetSession()->SendMessage(&send, send.dwSize);

	//UpdateBlessingOfLoong();// Jason
	// Jason 12-8-2007采用拉模式
	NotifyClientIniLessingOfLoong();

	// Jason v1.3.1外部链接
	g_dbSession.SendMsgToPlayerForExtLinks  (this);
}

//----------------------------------------------------------------------------------------------
// 初始化数据库中中的人物属性
//----------------------------------------------------------------------------------------------
VOID Role::InitAtt(const tagRoleDataSave* pDataSave)
{
	// 各个一级属性的投点
	IFASTCODE->MemCpy(m_nAttPointAdd, pDataSave->nAttPointAdd, sizeof(m_nAttPointAdd));

	// 所有属性附上默认值
	for(INT n = 0; n < ERA_End; n++)
	{
		m_nBaseAtt[n] = g_attRes.GetAttDefRole(n);
	}

	// 设置一些保存在数据库中的属性（一些不随基础值改变的属性，还缺少一个士气）
	m_nAtt[ERA_HP]			=	pDataSave->nHP;
	m_nAtt[ERA_MP]			=	pDataSave->nMP;
	m_nAtt[ERA_Rage]		=	pDataSave->nRage;
	m_nAtt[ERA_Vitality]	=	pDataSave->nVitality;
	m_nAtt[ERA_Endurance]	=	pDataSave->nEndurance;
	m_nAtt[ERA_Knowledge]	=	pDataSave->nKnowledge;
	m_nAtt[ERA_Injury]		=	pDataSave->nInjury;
	m_nAtt[ERA_Morale]		=	pDataSave->nMorale;
	m_nAtt[ERA_Morality]	=	pDataSave->nMorality;
	m_nAtt[ERA_Culture]		=	pDataSave->nCulture;
	m_nAtt[ERA_AttPoint]	=	pDataSave->nAttPoint;
	m_nAtt[ERA_TalentPoint]	=	pDataSave->nTalentPoint;
	m_nAtt[ERA_Hostility]	=	pDataSave->nHostility;

	m_nLevel		= pDataSave->nLevel;					// 等级
	m_nCurLevelExp	= pDataSave->nCurExp;					// 当前升级经验

	m_eClass	= pDataSave->eClass;						// 职业
	m_eClassEx	= pDataSave->eClassEx;						// 职业扩展
	m_nCredit	= pDataSave->nCredit;						// 信用度
	m_nIdentity = pDataSave->nIdentity;						// 身份
	m_nVIPPoint = pDataSave->nVIPPoint;						// 会员积分
	m_nTreasureSum = pDataSave->nTreasureSum;				//开启宝箱计数

	// 通过人物等级计算对应的一级属性
	const tagLevelUpEffect* pLevelUp = g_attRes.GetLevelUpEffect(pDataSave->nLevel);

	SetBaseAttValue(ERA_Physique, pLevelUp->n16RoleAtt[ERA_Physique]);
	SetBaseAttValue(ERA_Strength, pLevelUp->n16RoleAtt[ERA_Strength]);
	SetBaseAttValue(ERA_Pneuma, pLevelUp->n16RoleAtt[ERA_Pneuma]);
	SetBaseAttValue(ERA_InnerForce, pLevelUp->n16RoleAtt[ERA_InnerForce]);
	SetBaseAttValue(ERA_Technique, pLevelUp->n16RoleAtt[ERA_Technique]);
	SetBaseAttValue(ERA_Agility, pLevelUp->n16RoleAtt[ERA_Agility]);

	// 设置某些二级属性
	SetBaseAttValue(ERA_MaxHP,			pLevelUp->n16HP);
	SetBaseAttValue(ERA_MaxMP,			pLevelUp->n16MP);
	SetBaseAttValue(ERA_MaxVitality,	pLevelUp->n16Vitality);

	// 根据玩家属性的投放值来增加一级属性
	ModBaseAttValue(ERA_Physique, m_nAttPointAdd[ERA_Physique]);
	ModBaseAttValue(ERA_Strength, m_nAttPointAdd[ERA_Strength]);
	ModBaseAttValue(ERA_Pneuma, m_nAttPointAdd[ERA_Pneuma]);
	ModBaseAttValue(ERA_InnerForce, m_nAttPointAdd[ERA_InnerForce]);
	ModBaseAttValue(ERA_Technique, m_nAttPointAdd[ERA_Technique]);
	ModBaseAttValue(ERA_Agility, m_nAttPointAdd[ERA_Agility]);
}

//---------------------------------------------------------------------------------
// 初始化技能
//---------------------------------------------------------------------------------
VOID Role::InitSkill(const BYTE* &pData, INT32 nNum)
{
	// 首先加载普通攻击技能
	for(INT n = 0; n < X_NORMAL_ATTACK_SKILL_NUM; n++)
	{
		DWORD dwSkillID = NORMAL_ATTACK_SKILL_ID[n];
		Skill* pSkill = new Skill(dwSkillID, 1, 0, 0, 0);
		AddSkill(pSkill, FALSE);
	}

	// 加载保存的技能
	if( nNum <= 0 ) return;

	const tagSkillSave* pSkillSave = (const tagSkillSave*)pData;

	for(INT n = 0; n < nNum; n++)
	{
		Skill* pSkill = new Skill(pSkillSave->dwID, pSkillSave->nSelfLevel, pSkillSave->nLearnLevel, pSkillSave->nCoolDown, pSkillSave->nProficiency);

		// 将技能加入到列表中
		AddSkill(pSkill, FALSE);

		pSkillSave++;
	}

	// 重设指针
	pData = (const BYTE*)((tagSkillSave*)pData + nNum);
	//pData = (const BYTE*)pSkillSave;
}

//-------------------------------------------------------------------------------
// 初始化状态
//-------------------------------------------------------------------------------
VOID Role::InitBuff(const BYTE* &pData, INT32 nNum)
{
	if( nNum <= 0 ) return;

	INT nBeBuffIndex = 0;
	INT nDeBuffIndex = MAX_BENEFIT_BUFF_NUM;

/*	const tagBuffSave* pEnd = (tagBuffSave*)pData + nNum;*/

	for(INT n = 0; n < nNum; n++)
	{
		tagBuffSave* pBuffSave = (tagBuffSave*)pData;

		const tagBuffProto* pProto = g_attRes.GetBuffProto(Buff::GetTypeIDFromIDAndLevel(pBuffSave->dwBuffID, pBuffSave->n8Level));
		if( !P_VALID(pProto) ) goto next_buff;

		// 计算离线计时Buff
		if (pProto->bOfflineConsume)
		{
			// 获取当前时间和该玩家上次下线的时间差(单位：秒)
			DWORD dwOfflineTick = CalcTimeDiff(g_world.GetWorldTime(), m_LogoutTime) * TICK_PER_SECOND;
			
			// 计算Buff剩余持续时间(即使DWORD=>INT32也没问题)
			if (dwOfflineTick > MAX_BUFF_PERSIST_TICK)
			{
				goto next_buff;
			}
			else if ( pProto->nPersistTick > (INT32)dwOfflineTick + pBuffSave->nPersistTick )
			{
				pBuffSave->nPersistTick += (INT32)dwOfflineTick;
			}
			else
			{
				goto next_buff;
			}
		}

		// 如果是有益Buff
		INT nIndex = GT_INVALID;
		if( pProto->bBenifit )
		{
			nIndex = nBeBuffIndex;
			// 有益Buff的数量已满
			if( nIndex >= MAX_BENEFIT_BUFF_NUM ) continue;
			++nBeBuffIndex;
		}
		// 否则是有害Buff
		else
		{
			nIndex = nDeBuffIndex;
			// 有害Buff的数量已满
			if( nIndex >= MAX_BUFF_NUM ) continue;
			++nDeBuffIndex;
		}

		m_Buff[nIndex].Init(this, pBuffSave, nIndex);
		m_mapBuff.Add(m_Buff[nIndex].GetID(), &m_Buff[nIndex]);

next_buff:
		// 累加指针
		pData += sizeof(tagBuffSave) - 1 + pBuffSave->n8ModifierNum * sizeof(DWORD);
	}

/*	pData = (const BYTE*)pEnd;*/
}

//-------------------------------------------------------------------------------
// 初始化玩家物品(装备)
//-------------------------------------------------------------------------------
VOID Role::InitItem(const BYTE* &pData, INT32 nNum)
{
	INT32 nItemSize		= sizeof(tagItem);
	INT32 nEquipSize	= sizeof(tagEquip);
	
	// 清除外观信息
	ZeroMemory(&m_AvatarEquipEquip, SIZE_AVATAR_EQUIP);
	ZeroMemory(&m_AvatarEquipFashion, SIZE_AVATAR_EQUIP);
	
	DWORD dwErrorCode;
	TList<tagItem *> listItem;
	const tagItem	*pTmpItem	= NULL;
	tagItem			*pNewItem	= NULL;

	pTmpItem = (const tagItem *)pData;
	for(INT32 i=0; i<nNum; ++i)
	{
		if(!MIsEquipment(pTmpItem->dwTypeID))
		{
			pNewItem = new tagItem;
			IFASTCODE->MemCpy(pNewItem, pTmpItem, nItemSize);
			pNewItem->pProtoType = g_attRes.GetItemProto(pTmpItem->dwTypeID);

			pTmpItem = (const tagItem*)((BYTE*)pTmpItem + nItemSize);
		}
		else
		{
			pNewItem = new tagEquip;
			IFASTCODE->MemCpy(pNewItem, pTmpItem, nEquipSize);
			pNewItem->pProtoType = g_attRes.GetEquipProto(pTmpItem->dwTypeID);

			pTmpItem = (const tagItem*)((BYTE*)pTmpItem + nEquipSize);
		}

		if(!P_VALID(pNewItem->pProtoType))
		{
			ASSERT(P_VALID(pNewItem->pProtoType));
			MAttResCaution(_T("item/equip"), _T("typeid"), pNewItem->dwTypeID);
			IMSG(_T("The item(SerialNum: %lld) hasn't found proto type!\n"), pNewItem->n64Serial);
			Destroy(pNewItem);
			continue;
		}

		pNewItem->eStatus = EUDBS_Null;
		pNewItem->pScript = g_ScriptMgr.GetItemScript( pNewItem->dwTypeID);
		
		dwErrorCode = Put2Container(pNewItem);
		if(dwErrorCode != E_Success)
		{
			if(dwErrorCode != E_Item_Place_NotFree)
			{
				ASSERT(0);
				Destroy(pNewItem);
				continue;
			}

			listItem.PushBack(pNewItem);
		}
	}

	// 检查是否有因位置重复导致的不能添加到容器中
	while(listItem.Size() != 0)
	{
		pNewItem = listItem.PopFront();

		// 如果是背包或仓库中的物品，放入空位
		switch(pNewItem->eConType)
		{
		case EICT_Bag:
			IMSG(_T("Ignor bag overlap for role<roleid:%u> item<serial: %lld> Container<id:%u> Position<index:%u>\n"), m_dwID, pNewItem->n64Serial, pNewItem->eConType, pNewItem->n16Index);
			ILOG->Write(_T("Ignor bag overlap for role<roleid:%u> item<serial: %lld> Container<id:%u> Position<index:%u>\n"), m_dwID, pNewItem->n64Serial, pNewItem->eConType, pNewItem->n16Index);
			Destroy(pNewItem);
			break;
		case EICT_Equip:
			dwErrorCode = GetItemMgr().Add2Bag(pNewItem, (DWORD)ELCID_Bag_PosOverlap, FALSE, FALSE);
			if(E_Con_NotEnoughSpace == dwErrorCode)
			{
				IMSG(_T("Because container<eType: %d> is full, item<serial: %lld\n> load failed!\n"), 
					EICT_Bag, pNewItem->n64Serial);
				IMSG(_T("Make some places and login again can solve this problem!\n"));				
			}
			if(dwErrorCode != E_Success)
				Destroy(pNewItem);
			break;
		case EICT_RoleWare:
			dwErrorCode = GetItemMgr().Add2RoleWare(pNewItem, (DWORD)ELCID_RoleWare_PosOverlap, FALSE, FALSE);
			if(E_Con_NotEnoughSpace == dwErrorCode)
			{
				IMSG(_T("Because container<eType: %d> is full, item<serial: %lld> load failed!\n"), 
					EICT_RoleWare, pNewItem->n64Serial);
				IMSG(_T("Make some places and login again can solve this problem!\n"));
			}
			if(dwErrorCode != E_Success)
				Destroy(pNewItem);
			break;
		default:
			ASSERT(0);
			Destroy(pNewItem);
			continue;
		}
		
		ASSERT(E_Success == dwErrorCode);
	}

	// 重设指针
	pData = (const BYTE*)pTmpItem;
}

//-------------------------------------------------------------------------------
// 将从数据库中读入的物品(装备),放入到相应的容器中
//-------------------------------------------------------------------------------
DWORD Role::Put2Container(tagItem *pItem)
{
	DWORD dwErrorCode = GetItemMgr().Put2Container(pItem);

	if(EICT_Equip == pItem->eConType && E_Success == dwErrorCode)
	{
		MTRANS_POINTER(p, pItem, tagEquip);
		ProcEquipEffectAtt(p, TRUE, p->n16Index);
		ResetOneEquipDisplay(p, p->n16Index);
		m_Suit.Add(p, p->n16Index, FALSE);
		m_LongHun.Add(p, p->n16Index, FALSE);
	}

	return dwErrorCode;
}

//-------------------------------------------------------------------------------
// 将从数据库中读入玩家物品冷却时间
//-------------------------------------------------------------------------------
VOID Role::InitItemCDTime(const BYTE* &pData, INT32 nNum)
{
	if(nNum <= 0)
	{
		return;
	}

	// 获取当前时间和该玩家上次下线的时间差(单位：秒)
	DWORD dwInterval = CalcTimeDiff(g_world.GetWorldTime(), m_LogoutTime);
	if(dwInterval > (DWORD)MAX_ITEM_DCTIME)
	{
		// 重设指针
		pData = pData + nNum * sizeof(tagCDTime);
		return;
	}

	// 转换成毫秒
	dwInterval *= 1000;
	
	ItemMgr &itemMgr = GetItemMgr();
	const tagCDTime *p = (const tagCDTime*)pData;

	for(INT32 i=0; i<nNum; ++i)
	{
		if(p->dwTime > dwInterval)
		{
			itemMgr.Add2CDTimeMap(p->dwTypeID, p->dwTime - dwInterval);
		}

		++p;
	}

	// 重设指针
	pData = (const BYTE*)((tagCDTime *)pData + nNum);
	//pData = (const BYTE*)p;
}

//-------------------------------------------------------------------------------
// 初始化好友列表
//-------------------------------------------------------------------------------
VOID Role::InitFriend(const BYTE* &pData, INT32 nNum)
{
	for(INT i = 0; i < MAX_FRIENDNUM; ++i)
	{
		m_Friend[i].dwFriendID = GT_INVALID;
		m_Friend[i].dwFriVal = 0;
		m_Friend[i].byGroup = 1;
	}

	// 黑名单初时化
	memset(m_dwBlackList, 0XFF, sizeof(m_dwBlackList));
	if(nNum <= 0)
		return;

	const tagFriendSave	*pFriend = NULL;
	DWORD dwSizeFriend = sizeof(tagFriendSave);
	pFriend = (const tagFriendSave*)pData;

	for(INT m = 0; m < nNum; ++m)
	{
		SetFriend(m, pFriend->dwFriendID, 0, pFriend->nGroupID);
		pFriend = (const tagFriendSave*)((BYTE*)pFriend + dwSizeFriend);
	}

	// 重设指针
	pData = (const BYTE*)((tagFriendSave*)pData + nNum);
	//pData = (const BYTE*)pFriend;
}

//-------------------------------------------------------------------------------
// 初始化好友度
//-------------------------------------------------------------------------------
VOID Role::InitFriendValue(const BYTE* &pData, INT32 nNum)
{
	const tagFriendshipSave *pFriendSave = NULL;
	tagFriend *pFriend = NULL;
	DWORD dwSizeFriend = sizeof(tagFriendshipSave);
	pFriendSave = (const tagFriendshipSave*)pData;

	for(INT i = 0; i < nNum; ++i)
	{
		pFriend = m_mapFriend.Peek(pFriendSave->dwFriendID);

		if(P_VALID(pFriend))
			pFriend->dwFriVal = pFriendSave->nFriVal;

		pFriendSave = (const tagFriendshipSave*)((BYTE*)pFriendSave + dwSizeFriend);
	}

	// 重设指针
	pData = (const BYTE*)((tagFriendshipSave*)pData + nNum);
	//pData = (const BYTE*)pFriendSave;
}

//-------------------------------------------------------------------------------
// 初始化黑名单
//-------------------------------------------------------------------------------
VOID Role::InitBlackList(const BYTE* &pData, INT32 nNum)
{
	const DWORD *pBlackList = NULL;
	DWORD dwSize = sizeof(DWORD);
	pBlackList = (const DWORD*)pData;

	for(INT i = 0; i < nNum; ++i)
	{
		SetBlackList(i, *pBlackList);
		pBlackList = (const DWORD*)((BYTE*)pBlackList + dwSize);
	}

	// 重设指针
	pData = (const BYTE*)((DWORD*)pData + nNum);
	//pData = (const BYTE*)pBlackList;
}

//-------------------------------------------------------------------------------
// 将从数据库中读入玩家当前的任务集合
//-------------------------------------------------------------------------------
VOID Role::InitCurrentQuest(const BYTE* &pData, INT32 nNum)
{
	const tagQuestSave* pQuestSave = (const tagQuestSave*)pData;
	const tagQuestSave* pEndQuestSave = pQuestSave + nNum;
	const tagQuestProto* pProto = NULL;
	INT nIndex = 0;

	for ( ; pQuestSave < pEndQuestSave; ++pQuestSave)
	{
		pProto = g_questMgr.GetQuestProto(pQuestSave->u16QuestID);
		if( !P_VALID(pProto) ) 
		{
			MAttResCaution(_T("QuestFile"), _T("questID"), pQuestSave->u16QuestID);
			continue;
		}

		m_Quests[nIndex].Init(pQuestSave, this, nIndex);
		m_mapCurrentQuests.Add(m_Quests[nIndex].GetID(), &m_Quests[nIndex]);
		++nIndex;
	}
	
	pData = (const BYTE*)((tagQuestSave*)pData + nNum);
	//pData = (const BYTE *)pEndQuestSave;
}

//-------------------------------------------------------------------------------
// 将从数据库中读入玩家已经完成的任务集合
//-------------------------------------------------------------------------------
VOID Role::InitCompleteQuest(const BYTE* &pData, INT32 nNum)
{
	const tagQuestDoneSave* pQuestDoneSave = (const tagQuestDoneSave*)pData;

	for(INT32 i = 0; i < nNum; ++i)
	{
		tagQuestDoneSave* pDoneQuest = new tagQuestDoneSave;
		IFASTCODE->MemCpy(pDoneQuest, pQuestDoneSave, sizeof(tagQuestDoneSave));
		m_mapCompleteQuests.Add(pDoneQuest->u16QuestID, pDoneQuest);
		
		++pQuestDoneSave;
	}

	// 重设指针
	pData = (const BYTE*)((tagQuestDoneSave*)pData + nNum);
	//pData = (const BYTE*)pQuestDoneSave;
}

//-------------------------------------------------------------------------------
// 计算人物初始当前属性
//-------------------------------------------------------------------------------
VOID Role::CalInitAtt()
{
	// 先保存一些不随基础值改变的属性
	INT nHP				=	m_nAtt[ERA_HP];
	INT nMP				=	m_nAtt[ERA_MP];
	INT nVitality		=	m_nAtt[ERA_Vitality];
	INT nEndurance		=	m_nAtt[ERA_Endurance];
	INT nKnowledge		=	m_nAtt[ERA_Knowledge];
	INT nInjury			=	m_nAtt[ERA_Injury];
	INT nMorale			=	m_nAtt[ERA_Morale];
	INT nMorality		=	m_nAtt[ERA_Morality];
	INT nCulture		=	m_nAtt[ERA_Culture];
	INT nAttPoint		=	m_nAtt[ERA_AttPoint];
	INT nTalentPoint	=	m_nAtt[ERA_TalentPoint];
	INT nHostility		=	m_nAtt[ERA_Hostility];
	INT nRage			=	m_nAtt[ERA_Rage];

	for(INT n = 0; n < ERA_End; n++)
	{
		m_nAtt[n] = CalAttMod(m_nBaseAtt[n], n);
	}

	// 通过一级属性的当前值计算二级属性
	m_nAtt[ERA_MaxHP]		=	m_nAtt[ERA_Physique] * 10 + m_nBaseAtt[ERA_MaxHP];
	m_nAtt[ERA_MaxMP]		=	m_nAtt[ERA_Pneuma] * 10 + m_nBaseAtt[ERA_MaxMP];
	m_nAtt[ERA_ExAttack]	=	m_nAtt[ERA_Strength] * 5;
	m_nAtt[ERA_ExDefense]	=	m_nAtt[ERA_Physique] * 5 + m_nAtt[ERA_Strength];
	m_nAtt[ERA_InAttack]	=	m_nAtt[ERA_InnerForce] * 5;
	m_nAtt[ERA_InDefense]	=	m_nAtt[ERA_Pneuma] * 5 + m_nAtt[ERA_InnerForce];
	m_nAtt[ERA_AttackTec]	=	m_nAtt[ERA_Technique] * 6;
	m_nAtt[ERA_DefenseTec]	=	m_nAtt[ERA_Agility] * 4;
	m_nAtt[ERA_HitRate]		=	m_nAtt[ERA_Technique] * 10;
	m_nAtt[ERA_Dodge]		=	m_nAtt[ERA_Agility] * 10;
	m_nAtt[ERA_MaxEndurance]=	(m_nAtt[ERA_Technique] + m_nAtt[ERA_Agility]) / 10 + 298;

	// 计算二级属性加成
	for(INT n = ERA_AttB_Start; n < ERA_AttB_End; n++)
	{
		m_nAtt[n] = CalAttMod(m_nAtt[n], n);
	}

	// 计算某些“当前属性”与最大值之间的最小值
	if( nHP < 0 )
	{
		m_nAtt[ERA_HP] = m_nAtt[ERA_MaxHP];
	}
	else
	{
		m_nAtt[ERA_HP] = min(nHP, m_nAtt[ERA_MaxHP]);
	}

	if( nMP < 0 )
	{
		m_nAtt[ERA_MP] = m_nAtt[ERA_MaxMP];
	}
	else
	{
		m_nAtt[ERA_MP] = min(nMP, m_nAtt[ERA_MaxMP]);
	}

	if( nVitality < 0 )
	{
		m_nAtt[ERA_Vitality] = m_nAtt[ERA_MaxVitality];
	}
	else
	{
		m_nAtt[ERA_Vitality] = min(nVitality, m_nAtt[ERA_MaxVitality]);
	}

	if( nEndurance < 0 )
	{
		m_nAtt[ERA_Endurance] = m_nAtt[ERA_MaxEndurance];
	}
	else
	{
		m_nAtt[ERA_Endurance] = min(nEndurance, m_nAtt[ERA_MaxEndurance]);
	}

	// 设置上不随基础属性改变的当前属性
	m_nAtt[ERA_Knowledge]	=	nKnowledge;
	m_nAtt[ERA_Injury]		=	nInjury;
	m_nAtt[ERA_Morale]		=	nMorale;
	m_nAtt[ERA_Morality]	=	nMorality;
	m_nAtt[ERA_Culture]		=	nCulture;
	m_nAtt[ERA_AttPoint]	=	nAttPoint;
	m_nAtt[ERA_TalentPoint]	=	nTalentPoint;
	m_nAtt[ERA_Hostility]	=	nHostility;
	m_nAtt[ERA_Rage]		=	nRage;

	// 设置某些随基本属性变化的属性
	m_fMountXZSpeed *= FLOAT(m_nAtt[ERA_Speed_Mount]) / 10000.0f;
	m_fXZSpeed *= FLOAT(m_nAtt[ERA_Speed_XZ]) / 10000.0f;
	m_fYSpeed *= FLOAT(m_nAtt[ERA_Speed_Y]) / 10000.0f;
	m_fSwimXZSpeed *= FLOAT(m_nAtt[ERA_Speed_Swim]) / 10000.0f;
	m_Size *= FLOAT(m_nAtt[ERA_Shape]) / 10000.0f;
	// todo: pk状态

	// 清空重算字段
	ClearAttRecalFlag();
}

//-----------------------------------------------------------------------
// 计算初始状态
//-----------------------------------------------------------------------
VOID Role::CalInitState()
{
	// 计算PK状态
	CalPKState(FALSE);

	// 计算死亡状态
	if( GetAttValue(ERA_HP) <= 0 )
	{
		SetState(ES_Dead, FALSE);
	}
}

//------------------------------------------------------------------------
// 上线
//------------------------------------------------------------------------
VOID Role::Online(BOOL bFirst/* =FALSE */)
{
	if( bFirst )
		VirginOnline();

	// 调用上线的脚本函数
	if( P_VALID(m_pScript) )
	{
		m_pScript->OnRoleOnline(this);
	}
}

//------------------------------------------------------------------------
// 第一次上线
//------------------------------------------------------------------------
VOID Role::VirginOnline()
{
	// 设置在线时间
	m_nCurOnlineTime = 0;
	m_nOnlineTime = 0;

	// 设置出生地图和复活地图
	m_dwRebornMapID = m_dwMapID = g_mapCreator.GetBornMapID();

	// 得到一个随机的出生点
	Vector3 vBornPos = g_mapCreator.RandGetOneBornPos();

	// 随机一个朝向
	FLOAT fYaw = FLOAT(IUTIL->Rand() % 360);

	// 出生点
	m_MoveData.Reset(vBornPos.x, vBornPos.y, vBornPos.z, cosf(fYaw * 3.1415927f / 180.0f), 0.0f, sinf(fYaw * 3.1415927f / 180.0f));

	// 调用初次上限的脚本函数
	if( P_VALID(m_pScript) )
	{
		m_pScript->OnRoleFirstOnline(this);
	}
}

//------------------------------------------------------------------------
// 初始化宝箱相关数据
//------------------------------------------------------------------------
VOID Role::InitChestState()
{
	m_TreasureState.nChestSerial = 0;
	m_TreasureState.nKeySerial = 0;
	m_TreasureState.dwChestTypeID = 0;
	m_TreasureState.dwKeyTypeID = 0;
	m_TreasureState.ChestItem.dwTypeID = 0;
	m_TreasureState.ChestItem.nNum = 0;
	m_TreasureState.ChestItem.nTableID = 0;
	m_TreasureState.ChestItem.fBeginRate = 0.0;
	m_TreasureState.ChestItem.fNormalRate = 0.0;
	m_TreasureState.ChestItem.fRoleRate = 0.0;
	m_TreasureState.ChestItem.fServerRate = 0.0;
	
}