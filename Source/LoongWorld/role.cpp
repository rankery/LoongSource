//-------------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: role.h
// author: Aslan
// actor:
// data: 2008-7-11
// last:
// brief: 人物数据结构
//-------------------------------------------------------------------------------
#include "StdAfx.h"

#include "player_session.h"
#include "db_session.h"

#include "../WorldDefine/chat.h"
#include "../WorldDefine/chat_define.h"
#include "../WorldDefine/RoleDefine.h"
#include "../WorldDefine/quest.h"
#include "../WorldDefine/talent_define.h"
#include "../WorldDefine/pk_define.h"
#include "../WorldDefine/msg_talent.h"
#include "../WorldDefine/msg_pk.h"
#include "../WorldDefine/msg_combat.h"
#include "../WorldDefine/msg_stall.h"
#include "../WorldDefine/SocialDef.h"
#include "../WorldDefine/msg_social.h"
#include "../WorldDefine/msg_prison.h"
#include "../WorldDefine/msg_map.h"
#include "../WorldDefine/exchange_define.h"
#include "../WorldDefine/role_att.h"
#include "../WorldDefine/QuestDef.h"
#include "../WorldDefine/msg_script.h"
#include "../WorldDefine/msg_loot.h"
#include "../WorldDefine/msg_motion.h"

#include "../ServerDefine/role_data_define.h"
#include "../ServerDefine/role_data.h"
#include "../ServerDefine/msg_quest.h"
#include "../ServerDefine/msg_skill.h"
#include "../ServerDefine/log_cmdid_define.h"
#include "../ServerDefine/login_issue.h"
#include "../WorldDefine/motion_define.h"
#include "pet_define.h"
#include "db_session.h"
#include "login_session.h"
#include "map.h"
#include "map_instance.h"
#include "world.h"
#include "map_creator.h"
#include "map_mgr.h"
#include "att_res.h"
#include "skill.h"
#include "buff.h"
#include "quest.h"
#include "quest_mgr.h"
#include "role.h"
#include "creature.h"
#include "creature_ai.h"
#include "item_creator.h"
#include "role_mgr.h"
#include "stall.h"
#include "group_mgr.h"
#include "social_mgr.h"
#include "script_mgr.h"
#include "title_mgr.h"
#include "pet_pocket.h"
#include "guild_mgr.h"
#include "pet_soul.h"
#include "guild_commodity.h"
#include "guild.h"
//#include "container_restrict.h"

// Jason 2009-12-7 神龙赐福
#include "../WorldDefine/msg_player_preventlost.h"



Role* Role::Create( DWORD dwRoleID, const tagRoleDataLoad* pData, PlayerSession* pSession )
{
	return new Role(dwRoleID, pData, pSession);
}

VOID Role::Delete( Role* &pRole )
{
	SAFE_DEL(pRole);
}

Role::SaveRole::SaveRole()
{
	BYTE *byData = new BYTE[X_ROLE_BUFF_SIZE];
	ZeroMemory(byData, X_ROLE_BUFF_SIZE);
	MMSGINIT(byData, tagNDBC_SaveRole);
	m_pSaveRole = (tagNDBC_SaveRole*)(byData) ;
	m_pSaveRole->dwSize = X_ROLE_BUFF_SIZE;
}

Role::SaveRole::~SaveRole()
{
	delete [](BYTE*)m_pSaveRole;
}

VOID Role::SaveRole::Init()
{
	m_pSaveRole->dwRoleID = GT_INVALID;
	ZeroMemory(&m_pSaveRole->RoleData, X_ROLE_BUFF_SIZE - FIELD_OFFSET(tagNDBC_SaveRole, RoleData));
}

Role::SaveRole	Role::m_SaveRole;
Mutex			Role::m_SaveRoleLock;

//-------------------------------------------------------------------------------
// constructor
//-------------------------------------------------------------------------------
Role::Role(DWORD dwRoleID, const tagRoleDataLoad* pData, PlayerSession* pSession)
		: Unit(dwRoleID, pData->RoleDataSave.dwMapID, 
				pData->RoleDataSave.fCoordinate[0], pData->RoleDataSave.fCoordinate[1], pData->RoleDataSave.fCoordinate[2],
				pData->RoleDataSave.fFace[0], pData->RoleDataSave.fFace[1], pData->RoleDataSave.fFace[2]),
		  m_ePKState(ERPKS_Peace), m_pStall(new Stall(this, &(pData->RoleDataSave))),
		  m_ItemMgr(this, pSession->GetSessionID(), dwRoleID, pData->RoleDataSave.n16BagSize, pSession->GetWareSize()),
		  m_CurMgr(this, pData->RoleDataSave.nBagGold, pData->RoleDataSave.nBagSilver, pData->RoleDataSave.nBagYuanBao,
					pSession->GetWareSilver(), pSession->GetBaiBaoYB(), pData->RoleDataSave.nExVolume),
		  m_pSession(pSession), m_LongHun(this), m_nNeedSave2DBCountDown(MIN_ROLE_SAVE2DB_INTERVAL),
		  m_Suit(this),m_dwTeamID(GT_INVALID),m_dwGroupID(GT_INVALID),m_dwTeamInvite(GT_INVALID), m_dwOwnInstanceID(GT_INVALID), m_dwOwnInstanceMapID(GT_INVALID),
		  m_dwExportMapID(GT_INVALID), m_pScript(NULL), m_pPetPocket(NULL),	m_nMotionCounter(GT_INVALID), m_dwPartnerID(GT_INVALID),
		  ScriptData<ESD_Role>(),m_nWorldTalkCounter(GT_INVALID),m_bObjectCoolOff(FALSE), m_bSpeakOff(FALSE), m_bLevelUpClosedSafeGuard(FALSE),
		  m_dwItemTransportMapID(pData->RoleDataSave.dwItemTransportMapID),m_fItemTransportX(pData->RoleDataSave.fItemTransportX),m_fItemTransportZ(pData->RoleDataSave.fItemTransportZ),
		  m_fItemTransportY(pData->RoleDataSave.fItemTransportY),m_dwLoverID(pData->RoleDataSave.dwLoverID),m_bHaveWedding(pData->RoleDataSave.bHaveWedding),
		  m_timeLastLessing(pData->RoleDataSave.timeLastLessing),m_nLessingLevel(pData->RoleDataSave.nLessingLevel),m_bFiredOnceOfLessingLoong(FALSE)
{
	Init(pData);
}

//-------------------------------------------------------------------------------
// destructor
//-------------------------------------------------------------------------------
Role::~Role()
{
	// 完成任务销毁
	tagQuestDoneSave* pDoneQuest = NULL;
	QuestDoneMap::TMapIterator it = m_mapCompleteQuests.Begin();
	while( m_mapCompleteQuests.PeekNext(it, pDoneQuest) )
	{
		SAFE_DEL(pDoneQuest);
	}
	m_mapCompleteQuests.Clear();

	SAFE_DEL(m_pStall);
	m_pScript = NULL;

	SAFE_DEL(m_pTitleMgr);

	SAFE_DEL(m_pPetPocket);

};

//--------------------------------------------------------------------------------
// 玩家的更新函数
//--------------------------------------------------------------------------------
VOID Role::Update()
{
	GetMoveData().Update();
	GetCombatHandler().Update();
	UpdateSkill();
	UpdateBuff();
	UpdatePK();
	UpdatePVP();
	UpdateHostilitySafeGuard();
	UpdateTimedIssue();
	GetItemMgr().Update();
	m_pStall->Update();
	m_CurMgr.Update();
	GetPetPocket()->Update();

	--m_nNeedSave2DBCountDown;
	UpdateTalkToWorld();
	UpdateMotionInviteState();

	// Jason 神龙赐福 2009-12-7 
	// Jason 取消主动计时，采用拉模式。
	// UpdateBlessingOfLoong();
}

//-------------------------------------------------------------------------------------------
// 将玩家加入到游戏世界中
//-------------------------------------------------------------------------------------------
BOOL Role::AddToWorld(BOOL bFirst)
{
	BOOL bRet = g_mapCreator.TakeOverRoleWhenOnline(this);

	if( bRet && P_VALID(m_pScript) )
	{
		if( bFirst )		// 第一次进入游戏世界
		{
			m_pScript->OnRoleFirstIntoWorld(this);
		}
		m_pScript->OnRoleIntoWorld(this);	// 进入游戏世界
	}

	return bRet;
}

//-----------------------------------------------------------------------
// 检查是否在对应职能NPC附近
//-----------------------------------------------------------------------
DWORD Role::CheckFuncNPC(DWORD dwNPCID, EFunctionNPCType eNPCType,
						 OUT Creature **ppNPC/* = NULL*/, OUT Map **ppMap/* = NULL*/)
{
	// 获得map
	Map *pMap = GetMap();
	if(!P_VALID(pMap))
	{
		return GT_INVALID;
	}

	// 找到NPC，并检查合法性
	Creature* pNPC = pMap->FindCreature(dwNPCID);
	if(!P_VALID(pNPC))
	{
		return E_NPC_NotFound;
	}

	if(!pNPC->IsFunctionNPC(eNPCType))
	{
		return E_NPC_NotValid;
	}

	if(!pNPC->CheckNPCTalkDistance(this))
	{
		return E_NPC_TooFar;
	}

	if(ppNPC != NULL)
	{
		*ppNPC = pNPC;
	}

	if(ppMap != NULL)
	{
		*ppMap = pMap;
	}

	return E_Success;
}

//----------------------------------------------------------------------------------------
// 初始化物品装备 -- 发给客户端
//----------------------------------------------------------------------------------------
VOID Role::SendInitStateItem()
{
	GetItemMgr().SendInitStateItem();
	GetItemMgr().SendInitStateItemCDTime();
}

//----------------------------------------------------------------------------------------
// 初始套装
//----------------------------------------------------------------------------------------
VOID Role::SendInitStateSuit()
{
	INT nSuitNum = m_Suit.GetSuitNum();
	if(0 == nSuitNum)
	{
		return;
	}

	INT nSzMsg = sizeof(tagNS_GetRoleInitState_Suit) - 1 + sizeof(tagSuitInit) * nSuitNum;

	MCREATE_MSG(pSend, nSzMsg, NS_GetRoleInitState_Suit);
	if(!P_VALID(pSend))
	{
		ASSERT(P_VALID(pSend));
		return;
	}

	m_Suit.InitSendInitState(pSend->byData);
	pSend->nSuitNum = nSuitNum;
	SendMessage(pSend, pSend->dwSize);

	MDEL_MSG(pSend);
}

//---------------------------------------------------------------------------
// 初始龙魂能力
//---------------------------------------------------------------------------
VOID Role::SendInitStateLongHun()
{
	INT8 n8Num = m_LongHun.GetActiveNum() + m_LongHun.GetPassiveNum();
	if(n8Num <=0)
	{
		return;
	}

	INT nSzMsg = sizeof(tagNS_GetRoleInitState_Longhun) + (n8Num - 1) * sizeof(INT16);
	
	MCREATE_MSG(pSend, nSzMsg, NS_GetRoleInitState_Longhun);
	if(!P_VALID(pSend))
	{
		ASSERT(P_VALID(pSend));
		return;
	}

	m_LongHun.InitSendInitState(pSend->n16EquipPos, n8Num);
	pSend->n8Num = n8Num;
	SendMessage(pSend, pSend->dwSize);

	MDEL_MSG(pSend);
}

//---------------------------------------------------------------------------
// 发送人物初始基本属性给客户端
//---------------------------------------------------------------------------
VOID Role::SendInitStateAtt()
{
	INT nBuffNum = GetBuffNum();
	DWORD dwSize = sizeof(tagNS_GetRoleInitState_Att) + ((nBuffNum > 0) ? (nBuffNum - 1)*sizeof(tagBuffMsgInfo) : 0);

	MCREATE_MSG(pSend, dwSize, NS_GetRoleInitState_Att);

	pSend->Avatar = m_Avatar;
	pSend->AvatarEquip = GetAvatarEquip();
	pSend->DisplaySet = GetDisplaySet();
	pSend->nLevel = m_nLevel;
	pSend->nCurLevelExp = m_nCurLevelExp;
	pSend->nCredit = m_nCredit;
	pSend->nIdentity = m_nIdentity;
	pSend->nVIPPoint = m_nVIPPoint;
	pSend->dwState = GetState();
	pSend->dwRoleState = m_RoleState.GetState();
	pSend->dwRoleStateEx = m_RoleStateEx.GetState();
	pSend->ePKState = m_ePKState;
	pSend->dwRebornMapID = m_dwRebornMapID;
	pSend->vRebornPoint = g_mapCreator.GetReBornPoint(m_dwRebornMapID);
	pSend->dwTransportID = m_dwItemTransportMapID;
	pSend->vTransportPoint.x = m_fItemTransportX;
	pSend->vTransportPoint.z = m_fItemTransportZ;
	pSend->dwGuildID = GetGuildID();
	pSend->u16ActTitleID = GetTitleMgr()->GetActiviteTitle();
	pSend->dwLoverID = m_dwLoverID;
	pSend->bHaveWedding = m_bHaveWedding;
	pSend->bHasLeftMsg = m_bHasLeftMsg;
	pSend->eClassType = m_eClass;
	pSend->eClassTypeEx = m_eClassEx;
	pSend->byStallLevel = this->GetStallModeLevel();
	IFASTCODE->MemCpy(pSend->nAtt, m_nAtt, sizeof(m_nAtt));
	IFASTCODE->MemCpy(pSend->nAttPointAdd, m_nAttPointAdd, sizeof(m_nAttPointAdd));

	pSend->nBuffNum = nBuffNum;

	if( nBuffNum > 0 )
	{
		GetAllBuffMsgInfo(pSend->Buff, nBuffNum);
	}

	SendMessage(pSend, pSend->dwSize);

	MDEL_MSG(pSend);
}

//----------------------------------------------------------------------------
// 发送人物初始技能给客户端
//----------------------------------------------------------------------------
VOID Role::SendInitStateSkill()
{
	INT nNum = m_mapSkill.Size();

	TMap<DWORD, Skill*>::TMapIterator it = m_mapSkill.Begin();
	Skill* pSkill = NULL;

	if( nNum <= 0 )
	{
		tagNS_GetRoleInitState_Skill send;
		IFASTCODE->MemCpy(send.Talent, m_Talent, sizeof(m_Talent));
		send.nNum = 0;

		SendMessage(&send, send.dwSize);
	}

	else
	{
		DWORD dwSize = sizeof(tagNS_GetRoleInitState_Skill) + sizeof(tagSkillMsgInfo) * (nNum-1);
		MCREATE_MSG(pSend, dwSize, NS_GetRoleInitState_Skill);

		pSend->dwSize = dwSize;
		IFASTCODE->MemCpy(pSend->Talent, m_Talent, sizeof(m_Talent));
		pSend->nNum = nNum;

		INT nIndex = 0;
		while( m_mapSkill.PeekNext(it, pSkill) )
		{
			pSkill->GenerateMsgInfo(&pSend->Skill[nIndex]);
			nIndex++;
		}

		SendMessage(pSend, pSend->dwSize);

		MDEL_MSG(pSend);
	}


}

//----------------------------------------------------------------------------
// 发送人物完成任务列表给客户端
//----------------------------------------------------------------------------
VOID Role::SendInitStateCompleteQuest()
{
	// 得到完成任务的个数
	INT nCompleteQuestNum = GetCompleteQuestCount();
	
	DWORD dwSize = sizeof(tagNS_GetRoleInitState_CompleteQuest);
	if( nCompleteQuestNum > 0 )
	{
		dwSize += (nCompleteQuestNum-1) * sizeof(tagCompleteQuestMsgInfo);
	}
	
	MCREATE_MSG(pSend, dwSize, NS_GetRoleInitState_CompleteQuest);

	pSend->nNum = nCompleteQuestNum;

	INT nIndex = 0;
	tagQuestDoneSave* pDoneQuest = NULL;
	QuestDoneMap::TMapIterator it = m_mapCompleteQuests.Begin();

	while( m_mapCompleteQuests.PeekNext(it, pDoneQuest) )
	{
		pSend->completeQuest[nIndex].u16QuestID = pDoneQuest->u16QuestID;
		pSend->completeQuest[nIndex].dwStartTime = pDoneQuest->dwStartTime;
		pSend->completeQuest[nIndex].nTimes = pDoneQuest->nTimes;
		++nIndex;

		if( nIndex >= nCompleteQuestNum ) break;
	}

	SendMessage(pSend, pSend->dwSize);

	MDEL_MSG(pSend);

}

//----------------------------------------------------------------------------
// 发送人物当前任务列表给客户端
//----------------------------------------------------------------------------
VOID Role::SendInitStateIncompleteQuest()
{
	//得到当前任务的个数
	INT nCurrentQuestNum = GetCurrentQuestCount();

	DWORD dwSize = sizeof(tagNS_GetRoleInitState_IncompleteQuest);
	if( nCurrentQuestNum > 0 )
	{
		dwSize += (nCurrentQuestNum-1) * sizeof(tagIncompleteQuestMsgInfo);
	}

	MCREATE_MSG(pSend, dwSize, NS_GetRoleInitState_IncompleteQuest);

	pSend->nNum = nCurrentQuestNum;

	INT		nIndex = 0;
	BYTE*	pTemp = (BYTE*)pSend->incompleteQuest;	

	for(INT n = 0; n < QUEST_MAX_COUNT; ++n)
	{
		if( FALSE == m_Quests[n].IsValid() ) continue;

		DWORD dwQuestSize = 0;

		m_Quests[n].GenerateMsgInfo((tagIncompleteQuestMsgInfo*)pTemp, dwSize, dwQuestSize);
		++nIndex;
		pTemp += dwQuestSize;
		

		if( nIndex >= nCurrentQuestNum ) break;
	}
	pSend->dwSize = dwSize;
	SendMessage(pSend, pSend->dwSize);

	MDEL_MSG(pSend);
}

//----------------------------------------------------------------------------
//  发送人物金钱给客户端
//----------------------------------------------------------------------------
VOID Role::SendInitStateMoney()
{
	tagNS_GetRoleInitState_Money send;

	CurrencyMgr &CurMgr		= GetCurMgr();
	send.nBagYuanBao		= CurMgr.GetBagYuanBao();
	send.n64BagSilver		= CurMgr.GetBagSilver();
	send.n64WareSilver		= CurMgr.GetWareSilver();
	send.nBaiBaoYuanBao		= CurMgr.GetBaiBaoYuanBao();
	send.nExchangeVolume	= CurMgr.GetExchangeVolume();

	for (INT nClanType = ECLT_BEGIN; nClanType < ECLT_END; ++nClanType)
	{
		send.nCurClanCon[nClanType] = CurMgr.GetClanCon(static_cast<ECLanType>(nClanType));
	}

	SendMessage(&send, send.dwSize);
}

//----------------------------------------------------------------------------
//  发送人物当前声望数据给客户端
//----------------------------------------------------------------------------
VOID Role::SendInitStateReputation()
{
	tagNS_GetRoleInitState_Reputation send;

	ClanData& clanData = GetClanData();

	for (INT32 clantype = ECLT_BEGIN; clantype < ECLT_END; ++clantype)
	{
		send.nReputation[clantype]	= clanData.RepGetVal((ECLanType)clantype);
		send.nActiveCount[clantype]	= clanData.ActCountGetVal((ECLanType)clantype);
		send.bisFame[clantype]		= clanData.IsFame((ECLanType)clantype);
	}

	SendMessage(&send, send.dwSize);
}

//----------------------------------------------------------------------------
//  发送人物所在帮派属性数据给客户端
//----------------------------------------------------------------------------
VOID Role::SendInitStateGuild()
{
	tagNS_GetRoleInitState_Guild send;

	g_guildMgr.InitGuildInitState(this, send.sGuildBase, send.sGuildMember);

	SendMessage(&send, send.dwSize);
}

//----------------------------------------------------------------------------
//  发送防沉迷数据给客户端
//----------------------------------------------------------------------------
VOID Role::SendFatigueGuardInfo( BYTE byCode )
{
	tagNS_FatigueLimit send;
	send.byLimitCode = byCode;
	SendMessage(&send, send.dwSize);
}


//----------------------------------------------------------------------------
//  发送人物好友和黑名单
//----------------------------------------------------------------------------
VOID Role::SendFriendAndEnemy()
{
	//得到好友个数
	INT nFriendNum = GetFriendCount();
	INT nIndex = 0;
	tagRoleInfo* pRoleInfo = (tagRoleInfo*)GT_INVALID;

	DWORD dwSize = sizeof(tagNS_SendFriendBlackList);
	if( nFriendNum > 0 )
	{
		dwSize += (nFriendNum-1) * sizeof(tagFriendInfo);
	}

	MCREATE_MSG(pSend, dwSize, NS_SendFriendBlackList);
	IFASTCODE->MemCpy(pSend->dwRolesID, m_dwBlackList, sizeof(DWORD) * MAX_BLACKLIST);

	pSend->nNum = nFriendNum;

	for(INT n = 0; n < MAX_FRIENDNUM; ++n)
	{
		if( GT_INVALID == m_Friend[n].dwFriendID ) continue;

		pSend->FriendInfo[nIndex].dwFriendID = m_Friend[n].dwFriendID;
		pSend->FriendInfo[nIndex].dwFriVal = m_Friend[n].dwFriVal;
		pSend->FriendInfo[nIndex].byGroup = m_Friend[n].byGroup;
		
		pRoleInfo = g_roleMgr.GetRoleInfo(m_Friend[n].dwFriendID);
		if(P_VALID(pRoleInfo))
		{
			pSend->FriendInfo[nIndex].bOnline = pRoleInfo->bOnline;
			pSend->FriendInfo[nIndex].nLevel = pRoleInfo->byLevel;
		}

		++nIndex;
	}
	SendMessage(pSend, pSend->dwSize);

	MDEL_MSG(pSend);	
}

//----------------------------------------------------------------------------------------
// 从NPC处接取任务
//----------------------------------------------------------------------------------------
INT Role::AcceptQuestFromNPC(UINT16 u16QuestID, DWORD dwNPCID)
{
	// 检测任务是否存在
	const tagQuestProto* pProto = g_questMgr.GetQuestProto(u16QuestID);
	if( !P_VALID(pProto) )	return E_QUEST_NOT_EXIST;

	Creature* pNPC = NULL;

	// 如果不是从NPC接取
	if( !P_VALID(dwNPCID) )
	{
		if( g_questMgr.IsNeedNPCAcceptQuest(u16QuestID) )
			return E_QUEST_NPC_NOT_EXIST;
	}
	// 是从NPC接取
	else
	{
		pNPC = GetMap()->FindCreature(dwNPCID);
		if( !P_VALID(pNPC) )
		{
			return E_QUEST_NPC_NOT_EXIST;
		}
		else if( FALSE == pNPC->IsInDistance(*this, 500.0f) )
		{
			return E_QUEST_NPC_TOO_FAR;
		}
		else if( FALSE == g_questMgr.IsNPCAcceptQuest(pNPC, u16QuestID) )
		{
			return E_QUEST_NOT_EXIST;
		}
	}

	// 是否符合任务接取条件
	INT nIndex = GT_INVALID;
	INT nRet = CanAcceptQuest(u16QuestID, nIndex, pNPC);

	if( E_Success == nRet )
	{
		// 符合条件，添加任务
		AddQuest(pProto, nIndex);
		m_Quests[nIndex].OnAccept(pNPC);
	}

	return nRet;
}

//----------------------------------------------------------------------------------------
// 从触发器处获得一个任务
//----------------------------------------------------------------------------------------
INT Role::AcceptQuestFromMapTrigger(UINT16 u16QuestID, DWORD dwMapTriggerID)
{
	// 检测任务是否存在
	const tagQuestProto* pProto = g_questMgr.GetQuestProto(u16QuestID);
	if( !P_VALID(pProto) )	return E_QUEST_NOT_EXIST;

	// 找到是否在触发器里面
	if( FALSE == GetMap()->IsInTrigger(this, dwMapTriggerID) )
		return E_CanTakeQuest_FAILED_WRONG_TRIGGER;

	// 获得触发器任务序列号
	DWORD dwQuestSerial = GetMap()->GetTriggerQuestSerial(dwMapTriggerID);
	if( !P_VALID(dwQuestSerial) )
		return E_CanTakeQuest_FAILED_WRONG_TRIGGER;

	// 得到任务是否需要这个触发器
	INT n = 0;
	for(; n < QUEST_TRIGGERS_COUNT; ++n)
	{
		if( 0 == pProto->accept_req_trriger[n] ) continue;

		if( dwQuestSerial == pProto->accept_req_trriger[n] )
			break;
	}
	if( n >= QUEST_TRIGGERS_COUNT )
	{
		return E_CanTakeQuest_FAILED_WRONG_TRIGGER;
	}


	// 是否符合任务接取条件
	INT nIndex = GT_INVALID;
	INT nRet = CanAcceptQuest(u16QuestID, nIndex);

	if( E_Success == nRet )
	{
		// 符合条件，添加任务
		AddQuest(pProto, nIndex);
		m_Quests[n].OnAccept(NULL);
	}

	return nRet;
}

//----------------------------------------------------------------------------------------
// 某个任务是否可以接取
//----------------------------------------------------------------------------------------
INT Role::CanAcceptQuest(UINT16 u16QuestID, INT& nIndex, Creature* pNPC)
{
// 	// 防沉迷
// 	if (GetEarnRate() < 10000)
// 	{
// 		return E_QUEST_FATIGUEGUARD;
// 	}

//	g_questMgr.IsNeedNPCAcceptQuest(u16QuestID);

	// 检测任务是否存在
	const tagQuestProto* pProto = g_questMgr.GetQuestProto(u16QuestID);
	if( !P_VALID(pProto) )	return E_QUEST_NOT_EXIST;
	
	// 检测职业是否符合
	if ( E_Success != VocationCheck( pProto->class_type) )
		return E_CanTakeQuest_Class;

	//INT nTmpClass =  1 << ( m_eClass - 1 );
	////INT nTmpClassEx = 1 << ( m_eClassEx + 8 );
	//INT nTmpClassEx = 0;

	//if ( (INT)m_eClassEx != (INT)EHV_Base )
	//{
	//	nTmpClassEx = 1 << ( m_eClassEx + 8 );
	//}

	//DWORD dwTmpLimitStr = nTmpClass + nTmpClassEx;
	//if( !( pProto->class_type & dwTmpLimitStr ) )
	//	return E_CanTakeQuest_Class;

	// 检测是否已经接过该任务
	if( IsHaveQuest(u16QuestID) ) return E_CanTakeQuest_ALREADY_TAKE;

	// 检测是否做过该任务
	if( IsHaveDoneQuest(u16QuestID) )
	{
		// 如果任务是不可重复的，则返回已经做过
		if( !pProto->repeatable ) return E_CanTakeQuest_ALREADY_DONE;

		// 如果是可重复的，则检查是不是有次数限制的
		if( 0 != pProto->accept_times )
		{
			INT nTimes = GetQuestCompleteTimes(u16QuestID);
			if( nTimes >= pProto->accept_times )
				return E_CanTakeQuest_FAILED_WRONG_TIMES;
		}
	}

	// 检测前续任务
	BYTE byPreQuestOK = 0x01;
	if( pProto->prev_quest_id != 0 && FALSE == IsHaveDoneQuest(pProto->prev_quest_id) )
	{
		byPreQuestOK = 0x00;
	}
	BYTE byPreQuest2OK = 0x01;
	if( pProto->prev_quest2_id != 0 && FALSE == IsHaveDoneQuest(pProto->prev_quest2_id) )
	{
		byPreQuest2OK = 0x00;
	}
	if( pProto->prev_quest_relation == 0 && (byPreQuestOK&byPreQuest2OK) == 0 )//前序任务必须都完成
	{
		return E_CanTakeQuest_FAILED_PREV;
	}
	else if( pProto->prev_quest_relation == 1 && (byPreQuestOK|byPreQuest2OK) == 0 )//完成一个前序任务即可
	{
		return E_CanTakeQuest_FAILED_PREV;
	}

	// 检测周期性任务的时间
	if(pProto->period)
	{
		DWORD dwStartTime = GetQuestCompleteAcceptTime(u16QuestID);
		DWORD dwStartWeekDay = WhichWeekday(dwStartTime);
		DWORD dwCurrentTime = GetCurrentDWORDTime();
		DWORD dwCurrentWeekDay = WhichWeekday(dwCurrentTime);

		// 每天任务
		if(GT_INVALID != dwStartTime && EQuestPeriodic_DAY == pProto->period_type)
		{
			if(dwStartWeekDay == dwCurrentWeekDay)
				return E_CanTakeQuest_Tomorrow;
		}

		// 每周任务
		if (GT_INVALID != dwStartTime && EQuestPeriodic_WEEK == pProto->period_type)
		{
			if(dwCurrentWeekDay != (EWeek)pProto->week)
				return E_CanTakeQuest_Week;
		}
	}

	// 检测等级限制
	if( 0 != pProto->accept_req_min_level )
	{
		if( GetLevel() < pProto->accept_req_min_level )
			return E_CanTakeQuest_FAILED_LOW_LEVEL;
	}
	if( 0 != pProto->accept_req_max_level )
	{
		if( GetLevel() > pProto->accept_req_max_level )
			return E_CanTakeQuest_FAILED_LOW_LEVEL;
	}

	// 检测性别限制
	if( 0 != pProto->sex )
	{
		BYTE bySex = GetSex();
		if( 0 == bySex && 1 == pProto->sex )
		{
			return E_CanTakeQuest_FAILED_WRONG_SEX;
		}
		if( 1== bySex && 2 == pProto->sex )
		{
			return E_CanTakeQuest_FAILED_WRONG_SEX;
		}
	}

	//// 检测职业限制
	//if( ECT_Null != pProto->class_type )
	//{
	//	if( GetClass() != pProto->class_type )
	//	{
	//		return E_CanTakeQuest_FAILED_WRONG_CLASS;
	//	}
	//}

	// todo：声望
	

	// 检测技能限制
	for(INT n = 0; n < QUEST_SKILLS_COUNT; n++)
	{
		if( !P_VALID(pProto->accept_req_skill[n]) ) continue;

		Skill* pSkill = GetSkill(pProto->accept_req_skill[n]);
		if( !P_VALID(pSkill) || pSkill->GetLevel() < pProto->accept_req_skill_val[n] )
		{
			return E_CanTakeQuest_FAILED_MISSING_SKILLS;
		}
	}

	// 检测属性限制
	for(INT n = 0; n < QUEST_ATTS_COUNT; n++)
	{
		if( ERA_Null >= pProto->accept_req_att[n] ||
			ERA_End <= pProto->accept_req_att[n] )
			continue;
		
		if( GetAttValue(pProto->accept_req_att[n]) < pProto->accept_req_att_val[n] )
			return E_CanTakeQuest_FAILED_MISSING_Att;
	}

	// 检测物品限制
	for(INT n = 0; n < QUEST_ITEMS_COUNT; n++)
	{
		if( !P_VALID(pProto->accept_req_item[n]) ) continue;

		INT nTotal = GetItemMgr().GetBagSameItemCount(pProto->accept_req_item[n]) + GetItemMgr().GetQuestBagSameItemCount(pProto->accept_req_item[n]);
		if( nTotal < pProto->accept_req_item_num[n] )
		{
			return E_CanTakeQuest_FAILED_MISSING_ITEMS;
		}
	}

	// todo：检测是否已经结婚，是否有情侣

	// 检测背包能否放下接受任务后给的物品
	if( (0 != pProto->src_item) && pProto->src_item_num > 0)
	{
		if( GetItemMgr().GetBagFreeSize() < pProto->src_item_num ) 
			return E_CanTakeQuest_FAILED_BAG_FULL;
	}

	// 脚本判断
	const QuestScript* pScript = g_ScriptMgr.GetQuestScript(u16QuestID);
	if( P_VALID(pScript) && FALSE == pScript->CheckAccept(u16QuestID, this, pNPC) )
	{
		return GT_INVALID;
	}

	// 检测任务是否已满
	for(nIndex = 0; nIndex < QUEST_MAX_COUNT; ++nIndex)
	{
		if( FALSE == m_Quests[nIndex].IsValid() )
		{
			break;
		}
	}
	if( nIndex >= QUEST_MAX_COUNT )
	{
		return E_CanTakeQuest_FAILED_QUEST_NUM_FULL;
	}

	// 检测通过
	return E_Success;
}

//----------------------------------------------------------------------------------------
// 增加任务
//----------------------------------------------------------------------------------------
BOOL Role::AddQuest(const tagQuestProto* pProto, INT nIndex)
{
	if( !P_VALID(pProto) ) return FALSE;
	if( nIndex < 0 || nIndex >= QUEST_MAX_COUNT ) return FALSE;
	if( m_Quests[nIndex].IsValid() ) return FALSE;

	// 添加任务
	m_mapCurrentQuests.Add(pProto->id, &m_Quests[nIndex]);
	m_Quests[nIndex].Init(pProto, this, nIndex);

	// 发送给客户端增加一个任务
	tagNS_AddQuest send1;
	send1.u16QuestID = m_Quests[nIndex].GetID();
	m_Quests[nIndex].GetInitQuestItem(send1.n16ItemCount);
	m_Quests[nIndex].GetInitDynamicTarget(&send1.DynamicTarget, send1.dwSize);
	SendMessage(&send1, send1.dwSize);

	// 发送给数据库保存
	tagNDBC_AcceptQuest send;
	send.dwRoleID = GetID();
	send.acceptQuest.u16QuestID = m_Quests[nIndex].GetID();
	send.acceptQuest.dwAcceptTime = m_Quests[nIndex].GetAcceptTime();
	g_dbSession.Send(&send, send.dwSize);

	// 如果接任务时需要删除某些任务物品，则在这里删除
	if( pProto->del_req_item )
	{
		for(INT n = 0; n < QUEST_ITEMS_COUNT; n++)
		{
			if( !P_VALID(pProto->accept_req_item[n]) ) break;

			GetItemMgr().RemoveFromRole(pProto->accept_req_item[n], (INT32)pProto->accept_req_item_num[n], (DWORD)ELCID_Quest_Accept);
		}
	}

	// 如果接任务时会给一个任务物品，则在这里添加
	if( P_VALID(pProto->src_item) )
	{
		GetItemMgr().Add2Role(EICM_AccQuest, pProto->id, pProto->src_item, pProto->src_item_num, EIQ_Null, ELCID_Quest_Accept);
	}

	return TRUE;
}

//----------------------------------------------------------------------------------------
// 完成任务
//----------------------------------------------------------------------------------------
INT Role::CompleteQuest(UINT16 u16QuestID, DWORD dwNPCID, INT nChoiceItemIndex, UINT16& u16NexitQuestID)
{
	const tagQuestProto* pProto = g_questMgr.GetQuestProto(u16QuestID);
	if( !P_VALID(pProto) ) return E_QUEST_NOT_EXIST;

	// 检测任务是否存在
	Quest* pQuest = m_mapCurrentQuests.Peek(u16QuestID);
	if( !P_VALID(pQuest) ) return E_QUEST_NOT_EXIST;

	// 检测任务是否已经完成
	INT nRet = CanCompleteQuest(pQuest, dwNPCID, nChoiceItemIndex);

	// 如果可以交任务，则给任务奖励，并删除任务
	if( E_Success == nRet )
	{
		pQuest->SetCompleteMark(TRUE);

		// 调用脚本的完成时函数
		Creature* pNPC = GetMap()->FindCreature(dwNPCID);
		pQuest->OnComplete(pNPC);

		// 给任务奖励
		RewardQuest(pQuest, nChoiceItemIndex);
		
		// tbc:发送称号消息
		GetTitleMgr()->SigEvent(ETE_QUEST_COMPLETE, pQuest->GetID(), GT_INVALID);

		// 扣除金钱
		if(pProto->complete_del_money)
		{
			INT32 n32Money = pProto->complete_req_money;
			GetCurMgr().DecBagSilver(n32Money, ELCID_Quest_Complete);
		}

		// 删除任务
		RemoveQuest(pQuest->GetID(), TRUE);

		// 检测是否有后续自动接取任务
		if(pProto->auto_add_quest)
			u16NexitQuestID = pProto->next_quest_id;
	}

	return nRet;
}

//----------------------------------------------------------------------------------------
// 是否可以交任务
//----------------------------------------------------------------------------------------
INT Role::CanCompleteQuest(Quest* pQuest, DWORD dwNPCID, INT nChoiceItemIndex)
{
// 	// 防沉迷
// 	if (GetEarnRate() < 10000)
// 	{
// 		return E_QUEST_FATIGUEGUARD;
// 	}

	if( !P_VALID(pQuest) ) return GT_INVALID;

	Creature* pNPC = NULL;

	// 如果dwNPCID不为GT_INVALID，则检查该NPC是否合法
	if( P_VALID(dwNPCID) )
	{
		pNPC = GetMap()->FindCreature(dwNPCID);
		if( !P_VALID(pNPC) )
		{
			return E_QUEST_NPC_NOT_EXIST;
		}
		else if( FALSE == pNPC->IsInDistance(*this, 500.0f) )
		{
			return E_QUEST_NPC_TOO_FAR;
		}
		else if( FALSE == g_questMgr.IsNPCCompleteQuest(pNPC, pQuest->GetID()) )
		{
			return E_QUEST_NOT_EXIST;
		}
	}
	// 如果dwNPCID为GT_INVALID，则检测该任务是否不通过NPC就可以提交
	else
	{
		if( g_questMgr.IsNeedNPCCompleteQuest(pQuest->GetID()) )
		{
			return E_QUEST_NPC_NOT_EXIST;
		}
	}

	// 如果任务没有处于完成状态
	if( FALSE == pQuest->IsCanComplete(nChoiceItemIndex, pNPC) )
	{
		// todo: 等把返回码再整一下
		return GT_INVALID;
	}

	return E_Success;
}


//----------------------------------------------------------------------------------------
// 玩家主动删除任务
//----------------------------------------------------------------------------------------
INT Role::DeleteQuest(UINT16 u16QuestID)
{
	Quest* pQuest = GetQuest(u16QuestID);
	if( !P_VALID(pQuest) ) return E_QUEST_NOT_EXIST;

	// 调用脚本的任务删除接口
	pQuest->OnCancel();

	// 找到该任务，则删除该任务
	RemoveQuest(u16QuestID, FALSE);

	return E_Success;
}

//----------------------------------------------------------------------------------------
// 删除任务
//----------------------------------------------------------------------------------------
VOID Role::RemoveQuest(UINT16 u16QuestID, BOOL bComplete)
{
	Quest* pQuest = GetQuest(u16QuestID);
	if( !P_VALID(pQuest) ) return;

	const tagQuestProto* pProto = pQuest->GetProto();
	DWORD dwStartTime = pQuest->GetAcceptTime();
	tagQuestDynamicTarget*	pQuestDynamicTarget = pQuest->GetDynamicTarget();

	// 首先将任务拿走
	m_mapCurrentQuests.Erase(u16QuestID);
	pQuest->Destroy();

	// 如果是完成任务
	if( bComplete )
	{
		// 如果任务需要删除收集的物品，则删除物品
		if( P_VALID(pProto) && pProto->delete_item )
		{
			for(INT i = 0; i < QUEST_ITEMS_COUNT; i++)
			{
				if( !P_VALID(pProto->complete_req_item[i]) )
					break;

				GetItemMgr().RemoveFromRole(pProto->complete_req_item[i], (INT32)pProto->complete_req_item_num[i], (DWORD)ELCID_Quest_Complete);
			}
		}

		// 删除随机任务收集的物品
		if( P_VALID(pQuestDynamicTarget) && EQTT_Collect == pQuestDynamicTarget->eTargetType)
		{
			for(INT n = 0; n < DYNAMIC_TARGET_COUNT; n++)
			{
				if( !P_VALID(pQuestDynamicTarget->dwTargetID[n]) )
					break;

				GetItemMgr().RemoveFromRole(pQuestDynamicTarget->dwTargetID[n], (INT32)pQuestDynamicTarget->nTargetNum[n], (DWORD)ELCID_Quest_Complete);
			}
		}

		// 删除该任务所对应的所有任务物品
		GetItemMgr().RemoveFromRole(u16QuestID, (DWORD)ELCID_Quest_Complete);

		// 将该任务放入到完成任务列表中
		tagQuestDoneSave* pDoneQuest = m_mapCompleteQuests.Peek(u16QuestID);
		if( !P_VALID(pDoneQuest) )
		{
			pDoneQuest = new tagQuestDoneSave;
			pDoneQuest->u16QuestID = u16QuestID;
			pDoneQuest->dwStartTime = dwStartTime;
			pDoneQuest->nTimes = 0;
			m_mapCompleteQuests.Add(u16QuestID, pDoneQuest);
		}
		pDoneQuest->nTimes += 1;
		pDoneQuest->dwStartTime = dwStartTime;

		// 发送给数据库保存
		tagNDBC_CompleteQuest send;
		send.dwRoleID = GetID();
		send.QuestDone.u16QuestID = pDoneQuest->u16QuestID;
		send.QuestDone.dwStartTime = pDoneQuest->dwStartTime;
		send.QuestDone.nTimes = pDoneQuest->nTimes;
		g_dbSession.Send(&send, send.dwSize);
	}
	// 否则是删除任务
	else
	{
		// 删除该任务所对应的所有任务物品
		GetItemMgr().RemoveFromRole(u16QuestID, (DWORD)ELCID_Quest_Discard);

		// 发送给数据库删除
		tagNDBC_DiscardQuest send;
		send.dwRoleID = GetID();
		send.u16QuestID = u16QuestID;
		g_dbSession.Send(&send, send.dwSize);
	}

	// 删除任务接取时给的物品
	if( P_VALID(pProto->src_item) && pProto->src_item_num > 0 )
	{
		GetItemMgr().RemoveFromRole(pProto->src_item, pProto->src_item_num, (DWORD)ELCID_Quest_Complete);
	}
}


//----------------------------------------------------------------------------------------
// 更新任务NPC状态
//----------------------------------------------------------------------------------------
VOID Role::UpdateQuestNPCTalk(UINT16 u16QuestID, DWORD dwNPCID)
{
	// 是否有这个任务
	Quest* pQuest = GetQuest(u16QuestID);
	if( !P_VALID(pQuest) ) return;

	// 得到怪物是否存在
	Creature* pCreature = GetMap()->FindCreature(dwNPCID);
	if( !P_VALID(pCreature) ) return;

	// 尝试更新任务
	pQuest->OnEvent(EQE_Talk, pCreature->GetID(), pCreature->GetTypeID());
}

//----------------------------------------------------------------------------------------
// 更新任务Trigger状态
//----------------------------------------------------------------------------------------
VOID Role::UpdateQuestTrigger(UINT16 u16QuestID, DWORD dwTriggerID)
{
	// 是否有这个任务
	Quest* pQuest = GetQuest(u16QuestID);
	if( !P_VALID(pQuest) ) return;

	// 首先查看人物是不是在该触发器内
	if( FALSE == GetMap()->IsInTrigger(this, dwTriggerID) )
		return;

	// 得到该触发器的任务序列号
	DWORD dwTriggerSerial = GetMap()->GetTriggerQuestSerial(dwTriggerID);
	if( !P_VALID(dwTriggerSerial) ) return;

	// 尝试更新任务
	pQuest->OnEvent(EQE_Trigger, dwTriggerSerial);
}

//----------------------------------------------------------------------------
// 触发任务事件
//----------------------------------------------------------------------------
VOID Role::OnQuestEvent(EQuestEvent eQuestType, DWORD dwEventMisc1/* =0 */, DWORD dwEventMisc2/* =0 */,DWORD dwEventMisc3/*=0*/)
{
	for(INT n = 0; n < QUEST_MAX_COUNT; ++n)
	{
		if( FALSE == m_Quests[n].IsValid() ) continue;

		m_Quests[n].OnEvent(eQuestType, dwEventMisc1, dwEventMisc2, dwEventMisc3);
	}
}

//-------------------------------------------------------------------------------------
// 服务器可控对话框缺省事件
//-------------------------------------------------------------------------------------
VOID Role::OnDlgDefaultEvent(EMsgUnitType eDlgTarget, DWORD dwTargetID, EDlgOption eDlgOption)
{
	switch (eDlgTarget)
	{
	case EMUT_DlgTarget_Quest:
		{
			Quest* pQuest = GetQuest((UINT16)dwTargetID); 
			if(!P_VALID(pQuest))	return;

			pQuest->OnEvent(EQE_DlgDefault, eDlgOption);
		}
		break;
	case EMUT_DlgTarget_Item:
		{

		}
		break;
	case EMUT_DlgTarget_Creature:
		{

		}
		break;
	default:
		break;
	}
}

//----------------------------------------------------------------------------------------
// 任务奖励
//----------------------------------------------------------------------------------------
VOID Role::RewardQuest(Quest* pQuest, INT32 nChoiceItemIndex)
{
	// 防沉迷
	if (GetEarnRate() < 10000)
	{
		return ;
	}

	const tagQuestProto* pProto = pQuest->GetProto();
	if( !P_VALID(pProto) ) return;

	// 奖励物品
	for(INT n = 0; n < QUEST_REW_ITEM; ++n)
	{
		if( !P_VALID(pProto->rew_item[n]) ) break;

		if( pProto->rew_item_num[n] <= 0 ) continue;

		GetItemMgr().Add2Role(EICM_Quest, pQuest->GetID(), pProto->rew_item[n], (INT16)pProto->rew_item_num[n], (EItemQuality)pProto->rew_item_quality[n], ELCID_Quest_Complete);
	}

	// 可选奖励物品
	if( nChoiceItemIndex >= 0 && nChoiceItemIndex < QUEST_REW_ITEM )
	{
		if( P_VALID(pProto->rew_choice_item[nChoiceItemIndex]) &&
			pProto->rew_choice_item_num[nChoiceItemIndex] > 0 )
		{
			GetItemMgr().Add2Role(EICM_Quest, pQuest->GetID(), pProto->rew_choice_item[nChoiceItemIndex], 
								(INT16)pProto->rew_choice_item_num[nChoiceItemIndex], 
								(EItemQuality)pProto->rew_choice_quality[nChoiceItemIndex], ELCID_Quest_Complete);
		}
	}

	// 经验
	if( pProto->rew_xp != 0 )
	{
		ExpChange(pProto->rew_xp);
	}

	// 金钱
	if( pProto->rew_money > 0 )
	{
		GetCurMgr().IncBagSilver(pProto->rew_money, (DWORD)ELCID_Quest_Complete);
	}

	// 声望
	for (INT i = 0; JDG_VALID(ERT, pProto->rew_rep[i]) && i < QUEST_REPUTATIONS_COUNT; ++i)
	{
		GetClanData().RepModVal(MTRANS_ERT2ECLT(pProto->rew_rep[i]), pProto->rew_rep_val[i]);
	}

	// 贡献
	for (INT i = 0; JDG_VALID(ERCT, pProto->rew_contribution[i]) && i < QUEST_CONTRIBUTION_COUNT; ++i)
	{
		GetCurMgr().IncClanCon(pProto->rew_contribution_val[i], GT_INVALID, MTRANS_ECCT2ECLT(pProto->rew_contribution[i]));
	}

	// 属性
	for(INT n = 0; n < QUEST_ATTS_COUNT; ++n)
	{
		if( ERA_Null >= pProto->rew_att[n] ||
			ERA_End <= pProto->rew_att[n] )
			break;

		ModAttValue(pProto->rew_att[n], pProto->rew_att_val[n]);
	}

	// todo：Buff
}

//----------------------------------------------------------------------------------------
// 地图触发器
//----------------------------------------------------------------------------------------
BOOL Role::MapTrigger(DWORD dwTriggerID, DWORD dwMisc)
{
	Map* pMap = GetMap();

	if( !P_VALID(pMap) ) return FALSE;

	const tagMapInfo* pInfo = pMap->GetMapInfo();

	// 看是否存在该Trigger
	tagMapTriggerInfo* pTrigger = pInfo->mapTrigger.Peek(dwTriggerID);
	if( !P_VALID(pTrigger) ) return FALSE;

	// 检查玩家当前位置是不是在trigger范围之内
	if( !pMap->IsInTrigger(this, dwTriggerID) ) return FALSE;

	// 查看pTrigger的类型

	switch(pTrigger->eType)
	{
		// 切换地图
	case EMT_GotoNewMap:
		{
			// 得到目标地图的导航点
			const tagMapInfo* pMapInfo = g_mapCreator.GetMapInfo(pTrigger->dwMapID);
			if( !P_VALID(pMapInfo) ) return FALSE;

			const tagMapWayPointInfoList* pList = pMapInfo->mapWayPointList.Peek(pTrigger->dwWayPointID);
			if( !P_VALID(pList) ) return FALSE;

			// 从目标导航点列表中任取一个导航点
			tagWayPointInfo wayPoint;
			pList->list.RandPeek(wayPoint);

			// todo: 将来随机一个点，目前选取中心点

			// 移动到新地图
			if( FALSE == GotoNewMap(pTrigger->dwMapID, wayPoint.vPos.x, wayPoint.vPos.y, wayPoint.vPos.z, dwMisc))
				return FALSE;
		}
		break;

		// 任务触发器
	case EMT_Script:
		{
			// 得到该trigger对应的任务序列号
			if( !P_VALID(pTrigger->dwQuestSerial) ) return FALSE;
			OnQuestEvent(EQE_Trigger, pTrigger->dwQuestSerial);
		}
		break;

		// 什么都没有，则调用脚本
	case EMT_Null:
		{
			pMap->OnEnterTrigger(this, pTrigger);
		}
		break;

	default:
		break;
	}

	return TRUE;
}

//--------------------------------------------------------------------------------------
// 通知队友进入副本
//--------------------------------------------------------------------------------------
DWORD Role::InstanceNotify(BOOL bNotify)
{
	if(bNotify == FALSE)	return GT_INVALID;

	// 找到玩家小队
	const Team* pTeam = g_groupMgr.GetTeamPtr(GetTeamID());
	if( !P_VALID(pTeam) ) return GT_INVALID;

	// 检查队伍是否创建了副本
	Map* pMap = g_mapCreator.GetMap(pTeam->GetOwnInstanceMapID(), pTeam->GetOwnInstanceID());
	if( !P_VALID(pMap) || EMT_Instance != pMap->GetMapType() ) return GT_INVALID;

	MapInstanceNormal* pInstance = static_cast<MapInstanceNormal*>(pMap);
	if( !P_VALID(pInstance) ) return GT_INVALID;

	const tagInstance* pInstanceProto = pInstance->GetInstanceProto();
	if( !P_VALID(pInstanceProto) ) return GT_INVALID;	

	// 副本是否允许通知队友
	if( !pInstanceProto->bNoticeTeamate ) return GT_INVALID;

	// 通知队友
	tagNS_InstanceAgree	 send;
	send.dwInviterID	=	m_dwID;
	send.dwMapID		=	pInstance->GetMapID();
	g_groupMgr.SendTeamInstanceNotify(this, GetTeamID(), &send, send.dwSize);

	return E_Success;
}

//--------------------------------------------------------------------------------------
// 玩家是否同意其它玩家进入副本的邀请
//--------------------------------------------------------------------------------------
DWORD Role::InstanceAgree(BOOL bAgree)
{
	if(bAgree == FALSE)		return GT_INVALID;

	if (IsInRoleStateAny(ERS_Commerce | ERS_PrisonArea))	return GT_INVALID;
			
	// 找到玩家小队
	const Team* pTeam = g_groupMgr.GetTeamPtr(GetTeamID());
	if( !P_VALID(pTeam) ) return GT_INVALID;

	// 检查队伍是否创建了副本
	Map* pMap = g_mapCreator.GetMap(pTeam->GetOwnInstanceMapID(), pTeam->GetOwnInstanceID());
	if( !P_VALID(pMap) || EMT_Instance != pMap->GetMapType() ) return GT_INVALID;

	MapInstanceNormal* pInstance = static_cast<MapInstanceNormal*>(pMap);
	if( !P_VALID(pInstance) ) return GT_INVALID;

	const tagInstance* pInstanceProto = pInstance->GetInstanceProto();
	if( !P_VALID(pInstanceProto) ) return GT_INVALID;

	GotoNewMap(pInstance->GetMapID(), pInstanceProto->vEnterPos.x, pInstanceProto->vEnterPos.y, pInstanceProto->vEnterPos.z, GT_INVALID, FALSE);

	return E_Success;
}

//---------------------------------------------------------------------------------
// 玩家请求离开副本
//---------------------------------------------------------------------------------
DWORD Role::LeaveInstance()
{
	const tagMapInfo* pInfo = GetMap()->GetMapInfo();
	if( !P_VALID(pInfo) ) return GT_INVALID;

	if( EMT_Normal == pInfo->eType )
		return GT_INVALID;

	MapMgr* pMapMgr = g_mapCreator.GetMapMgr(pInfo->dwID);
	if( !P_VALID(pMapMgr) ) return GT_INVALID;

	pMapMgr->RoleInstanceOut(this);

	return E_Success;
}

//--------------------------------------------------------------------------------------
// 得到所属的副本ID（如果有队伍获取队伍的，如果没有队伍获取自己的）
//--------------------------------------------------------------------------------------
DWORD Role::GetOwnInstanceID() const
{
	DWORD dwTeamID = GetTeamID();
	
	// 玩家有小队
	if( dwTeamID != GT_INVALID )
	{
		const Team *pTeam = g_groupMgr.GetTeamPtr(dwTeamID);
		if( P_VALID(pTeam) )
		{
			return pTeam->GetOwnInstanceID();
		}
		else
		{
			return GT_INVALID;
		}
	}
	else // 玩家没有小队
	{
		return m_dwOwnInstanceID;
	}
}

//--------------------------------------------------------------------------------------
// 得到所属的副本地图ID（如果有队伍获取队伍的，没有队伍获取自己的）
//--------------------------------------------------------------------------------------
DWORD Role::GetOwnInstanceMapID() const
{
	DWORD dwTeamID = GetTeamID();

	// 玩家有小队
	if( dwTeamID != GT_INVALID )
	{
		const Team *pTeam = g_groupMgr.GetTeamPtr(dwTeamID);
		if( P_VALID(pTeam) )
		{
			return pTeam->GetOwnInstanceMapID();
		}
		else
		{
			return GT_INVALID;
		}
	}
	else // 玩家没有小队
	{
		return m_dwOwnInstanceMapID;
	}
}

//--------------------------------------------------------------------------------------
// 去一个新地图
//--------------------------------------------------------------------------------------
BOOL Role::GotoNewMap(DWORD dwDestMapID, FLOAT fX, FLOAT fY, FLOAT fZ, DWORD dwMisc, BOOL bSameInstace)
{
	if( fX < 0.0f || fZ < 0.0f || fY < 0.0f )
	{
		return FALSE;
	}

	// 检查一下地图的大小
	const tagMapInfo* pInfo = g_mapCreator.GetMapInfo(dwDestMapID);
	if( !P_VALID(pInfo) ) return FALSE;

	// 检查一下坐标
	if( fX < 0.0f || fX >= FLOAT(pInfo->nWidth * TILE_SCALE) ||
		fZ < 0.0f || fZ >= FLOAT(pInfo->nHeight * TILE_SCALE) )
		return FALSE;

	// 副骑乘下马
	PetSoul* pMountSoul = GetPetPocket()->GetMountPetSoul();
	if (P_VALID(pMountSoul))
	{
		Role* pPassenger = pMountSoul->GetPassenger();
		if (P_VALID(pPassenger))
		{
			pPassenger->StopMount();
		}
	}

	// 首先检查是不是本地图
	if( GetMapID() == dwDestMapID && bSameInstace )
	{

		// 同地图内传送
		Vector3 vNewPos(fX, fY, fZ);
		m_MoveData.ForceTeleport(vNewPos);

		// 得到新的九宫格
		GetMap()->SendGotoNewMapToPlayer(this);

		return TRUE;
	}
	else
	{
		// 是新地图， 首先找到MapMgr
		MapMgr* pMapMgr = g_mapCreator.GetMapMgr(dwDestMapID);
		if( !P_VALID(pMapMgr) ) return FALSE;

		// 没有找到MapMgr，地图ID非法
		Map* pMap = GetMap();
		if( !P_VALID(pMap) ) return FALSE;

		// 尝试切换地图
		Vector3 vDestPos;
		vDestPos.x = fX;
		vDestPos.y = fY;
		vDestPos.z = fZ;

		g_mapCreator.RoleChangeMap(m_dwID, dwDestMapID, vDestPos, dwMisc);

		return TRUE;
	}
}

//----------------------------------------------------------------------------------------------------
// 回城
//----------------------------------------------------------------------------------------------------
VOID Role::ReturnCity()
{
	if( !P_VALID(m_dwRebornMapID) ) return;

	const tagMapInfo* pInfo = g_mapCreator.GetMapInfo(m_dwRebornMapID);
	if( !P_VALID(pInfo) ) return;

	GotoNewMap(m_dwRebornMapID, pInfo->vRebornPos.x, pInfo->vRebornPos.y, pInfo->vRebornPos.z);
}

//------------------------------------------------------------------------------
// 改名
//------------------------------------------------------------------------------
VOID Role::ReName(LPCTSTR szNewName)
{
	ZeroMemory( g_roleMgr.GetRoleInfo(this->GetID())->szRoleName, X_SHORT_NAME );
	_tcscpy_s( g_roleMgr.GetRoleInfo(this->GetID())->szRoleName, X_SHORT_NAME, szNewName);

	SaveNewName2DB();
}

VOID Role::SaveNewName2DB()
{
	tagNDBC_SaveRenameInfo send;

	send.dwAccountID = (g_roleMgr.GetRoleInfo(GetID()))->dwAccountID;
	send.dwRoleID = (g_roleMgr.GetRoleInfo(GetID()))->dwRoleID;
	_tcscpy_s(send.szName, g_roleMgr.GetRoleInfo(this->GetID())->szRoleName);

	g_dbSession.Send(&send, send.dwSize);
}

//-------------------------------------------------------------------------------------
// 学习一个技能
//-------------------------------------------------------------------------------------
INT Role::LearnSkill(DWORD dwSkillID, DWORD dwNPCID, INT64 n64ItemID)
{
	// 检查该技能是否已经存在
	Skill* pSkill = GetSkill(dwSkillID);
	if( P_VALID(pSkill) ) return E_LearnSkill_Existed;

	// 首先找到该技能对应的静态属性（默认一级）
	DWORD dwSkillTypeID = Skill::CreateTypeID(dwSkillID, 1);
	const tagSkillProto* pProto = g_attRes.GetSkillProto(dwSkillTypeID);
	if( !P_VALID(pProto) ) return E_LearnSkill_ProtoNotFound;

	// 检测人物等级
	if( m_nLevel < pProto->nNeedRoleLevel )
		return E_LearnSkill_NeedMoreLevel;

	// 检测职业限制
	if( E_Success != VocationCheck( pProto->dwVocationLimit ))
		return E_LearnSkill_ClassLimit;

	// 检测前置技能
	if( GT_INVALID != pProto->dwPreLevelSkillID )
	{
		DWORD dwPreSkillID = Skill::GetIDFromTypeID(pProto->dwPreLevelSkillID);
		INT nPreSkillLevel = Skill::GetLevelFromTypeID(pProto->dwPreLevelSkillID);
		Skill* pPreSkill = GetSkill(dwPreSkillID);
		if( !P_VALID(pPreSkill) || pPreSkill->GetLevel() < nPreSkillLevel )
			return E_LearnSkill_NoPreSkill;

	}

	INT nTalentIndex = GT_INVALID;			// 如果是天资技能且该系天资已投，则记录该系天资的位置
	INT nBlankTalentIndex = GT_INVALID;		// 如果是天资技能但该系天资未投，则记录该系天资可插入的位置

	// 如果是天资技能，则检测该天资在人物身上是否已经存在
	if( ETT_Null != pProto->eTalentType )
	{
		nTalentIndex = FindTalentIndex(pProto->eTalentType);

		if( GT_INVALID != nTalentIndex )
		{
			// 检测人物该系天资所投的天资点是不是满足技能可学条件
			if( GetTalentPoint(nTalentIndex) < pProto->nNeedTalentPoint )
				return E_LearnSkill_NeedMoreTalentPoint;
		}
		else
		{
			// 人物身上没有该系天资，则判断是否还可以再学习新的天资
			nBlankTalentIndex = FindBlankTalentIndex(pProto->eTalentType);
			if( GT_INVALID == nBlankTalentIndex )
				return E_LearnSkill_ExceedTalentMaxNum;
		}
	}
	
	// NPC方式学习
	if( GT_INVALID != dwNPCID )
	{
		Creature* pNPC = GetMap()->FindCreature(dwNPCID);
		if( !P_VALID(pNPC) ) return E_LearnSkill_NPCNotEixst;

		if( FALSE == pNPC->CheckNPCTalkDistance(this) )
			return E_LearnSkill_NPCTooFar;

		if( FALSE == pNPC->IsFunctionNPC(EFNPCT_Teacher) )
			return E_LevelUpSkill_NPCCanNotTeach;

		// todo: 还要检测NPC可不可以教授该种技能
	}
	// 技能书学习
	else if( GT_INVALID != n64ItemID )
	{
		tagItem *pItem = GetItemMgr().GetBagItem(n64ItemID);
		if( !P_VALID(pItem) )
			return E_Item_NotFound;

		// 使用的等级判断
		if( pItem->IsLevelLimit(GetLevel()) )
			return E_Item_LevelLimit;
		
		// 检测特殊效果类型
		if( pItem->pProtoType->eSpecFunc != EISF_LearnSkill 
			|| pItem->pProtoType->nSpecFuncVal1 != dwSkillID)
			return E_Item_Used_NotValid;
	}
	// 帮派技能
	else if (pProto->nType2 == ESSTE_Guild)
	{
		// 上层已经通过验证
	}
	// 投点学习
	else
	{
		// 检测该技能是否有天资类型
		if( ETT_Null == pProto->eTalentType )
			return E_LearnSkill_NoTalentType;

		// 检测人物目前身上是否还有天资点
		if( GetAttValue(ERA_TalentPoint) <= 0 )
			return E_LearnSkill_NoTalentPoint;

		// 检测技能必须是固定升级方式
		if( ESLUT_Fixed != pProto->eLevelUpType )
			return GT_INVALID;
	}

	// 如果是生产技能，则判断目前身上的生产技能数量是否超过了最大数量
	if( ESSTE_Produce == pProto->nType2 )
	{
		if( GetProduceSkillNum() >= MAX_ROLE_PRODUCE_NUM )
			return E_LearnSkill_ProduceSkillFull;
	}

	// 一切判断通过，则给人物添加技能
	INT nSelfLevel = 0;
	INT nLearnLevel = 0;

	// 如果技能不是天资技能，则设置nSelfLevel
	if( ETT_Null == pProto->eTalentType )
	{
		nSelfLevel = 1;
	}
	// 如果是天资技能
	else
	{
		// 如果是通过NPC或者技能书学到的，则设置nSelfLevel
		if( GT_INVALID != dwNPCID || GT_INVALID != n64ItemID )
		{
			nSelfLevel = 1;

			// 设置某系天资
			if( nTalentIndex != GT_INVALID )
			{
				;
			}
			else if( nBlankTalentIndex != GT_INVALID )
			{
				AddTalent(nBlankTalentIndex, pProto->eTalentType, 0);
				SaveTalentPoint2DB(nBlankTalentIndex);
			}
			else
			{
				ASSERT(0);
			}
		}
		// 否则，则设置nLearnLevel，并设置相应的天资点
		else
		{
			nLearnLevel = 1;

			// 玩家扣除一个天资点
			ModAttValue(ERA_TalentPoint, -1);

			// 累加某些天资的天资点
			if( nTalentIndex != GT_INVALID )
			{
				AddTalentPoint(nTalentIndex, 1);
				SaveTalentPoint2DB(nTalentIndex);
			}
			else if( nBlankTalentIndex != GT_INVALID )
			{
				AddTalent(nBlankTalentIndex, pProto->eTalentType, 1);
				SaveTalentPoint2DB(nBlankTalentIndex);
			}
			else
			{
				ASSERT(0);
			}
		}
	}

	// 添加技能
	pSkill = new Skill(dwSkillID, nSelfLevel, nLearnLevel, 0, 0);
	AddSkill(pSkill);

	// 重新计算属性
	if( NeedRecalAtt() )
		RecalAtt();

	return E_Success;

}


//---------------------------------------------------------------------------------------
// 升级一个技能
//---------------------------------------------------------------------------------------
INT Role::LevelUpSkill(DWORD dwSkillID, DWORD dwNPCID, INT64 n64ItemID)
{
	// 检查该技能是否已经存在
	Skill* pSkill = GetSkill(dwSkillID);
	if( !P_VALID(pSkill) ) return E_LevelUpSkill_NotExist;

	// 检测技能是否已经到了最高等级
	if( pSkill->GetLevel() >= pSkill->GetMaxLevel() )
		return E_LevelUpSkill_ExceedMaxLevel;

	// 得到升级后的静态属性
	INT nLevel = pSkill->GetLevel() + 1;

	// 首先找到该技能对应的静态属性
	DWORD dwSkillTypeID = Skill::CreateTypeID(dwSkillID, nLevel);
	const tagSkillProto* pProto = g_attRes.GetSkillProto(dwSkillTypeID);
	if( !P_VALID(pProto) ) return E_LevelUpSkill_ProtoNotFound;

	// 如果是熟练度方式升级的，则不能直接做
	if( ESLUT_Exp == pProto->eLevelUpType )
		return E_LevelUpSkill_NotFixed;

	// 检测人物等级
	if( m_nLevel < pProto->nNeedRoleLevel )
		return E_LearnSkill_NeedMoreLevel;

	// todo: 检测职业限制
	//if( E_Success != VocationCheck( pProto-> ))

	// 检测前置技能
	DWORD dwPreSkillID = Skill::GetIDFromTypeID(pProto->dwPreLevelSkillID);
	INT nPreSkillLevel = Skill::GetLevelFromTypeID(pProto->dwPreLevelSkillID);
	Skill* pPreSkill = GetSkill(dwPreSkillID);
	if( !P_VALID(pPreSkill) || pPreSkill->GetLevel() < nPreSkillLevel )
		return E_LearnSkill_NoPreSkill;

	INT nTalentIndex = GT_INVALID;			// 如果是天资技能且该系天资已投，则记录该系天资的位置

	// 如果是天资技能，则检测该天资在人物身上是否已经存在
	if( ETT_Null != pProto->eTalentType )
	{
		nTalentIndex = FindTalentIndex(pProto->eTalentType);

		if( GT_INVALID != nTalentIndex )
		{
			// 检测人物该系天资所投的天资点是不是满足技能可学条件
			if( GetTalentPoint(nTalentIndex) < pProto->nNeedTalentPoint )
				return E_LearnSkill_NeedMoreTalentPoint;
		}
		else
		{
			ASSERT(0);
			return GT_INVALID;
		}
	}

	// NPC方式升级
	if( GT_INVALID != dwNPCID )
	{
		// NPC不能升级天资类型
		if( ETT_Null != pProto->eTalentType )
			return GT_INVALID;

		Creature* pNPC = GetMap()->FindCreature(dwNPCID);
		if( !P_VALID(pNPC) ) return E_LevelUpSkill_NPCNotEixst;

		if( FALSE == pNPC->CheckNPCTalkDistance(this) )
			return E_LevelUpSkill_NPCTooFar;

		if( FALSE == pNPC->IsFunctionNPC(EFNPCT_Teacher) )
			return E_LevelUpSkill_NPCCanNotTeach;

		// todo: 还要检测NPC可不可以教授该种技能
	}
	// 技能书学习
	else if( GT_INVALID != n64ItemID )
	{
		// 技能书不能升级天资类型
		if( ETT_Null != pProto->eTalentType )
			return GT_INVALID;

		// todo: 从物品管理器获得物品，并检测特殊效果类型

		// todo: 检测该技能书身上带的技能ID是不是参数中的技能ID
	}
	// 帮派技能
	else if (pProto->nType2 == ESSTE_Guild)
	{
		// 上层已经通过验证
	}
	// 投点学习
	else
	{
		// 检测该技能是否有天资类型
		if( ETT_Null == pProto->eTalentType )
			return E_LevelUpSkill_NoTalentType;

		// 检测人物目前身上是否还有天资点
		if( GetAttValue(ERA_TalentPoint) <= 0 )
			return E_LevelUpSkill_NoTalentPoint;

		// 检测技能必须是固定升级方式
		if( ESLUT_Fixed != pProto->eLevelUpType )
			return GT_INVALID;
	}

	// 一切判断通过，则给人物技能升级
	BOOL bLearn = FALSE;

	// 如果是通过NPC或者技能书升级的，则是自身等级提升
	if( GT_INVALID != dwNPCID || GT_INVALID != n64ItemID )
	{
		bLearn = 1;
	}
	// 帮派技能升级
	else if (pProto->nType2 == ESSTE_Guild)
	{
		bLearn = FALSE;
	}
	// 如果是投点升级的
	else
	{
		bLearn = TRUE;

		// 玩家扣除一个天资点
		ModAttValue(ERA_TalentPoint, -1);

		// 累加某系天资的天资点
		if( nTalentIndex != GT_INVALID )
		{
			AddTalentPoint(nTalentIndex, 1);
			SaveTalentPoint2DB(nTalentIndex);
		}
		else
		{
			ASSERT(0);
		}
	}

	// 升级
	if( bLearn )
		ChangeSkillLevel(pSkill, ESLC_Learn);
	else
		ChangeSkillLevel(pSkill, ESLC_Self);


	// 重新计算属性
	if( NeedRecalAtt() )
		RecalAtt();

	return E_Success;

}

//---------------------------------------------------------------------------------
// 遗忘技能
//---------------------------------------------------------------------------------
INT Role::ForgetSkill(DWORD dwSkillID, DWORD dwNPCID)
{
	// 检测玩家身上是否有该技能
	Skill* pSkill = GetSkill(dwSkillID);
	if( !P_VALID(pSkill) ) return E_ForgetSkill_NotExist;

	// 检测技能合法性
	if( ETT_Null != pSkill->GetTalentType() )
		return E_ForgetSkill_CanNotBeForgotten;

	// 检查NPC合法性
	Creature* pNPC = GetMap()->FindCreature(dwNPCID);
	if( !P_VALID(pNPC) ||
		FALSE == pNPC->IsFunctionNPC(EFNPCT_Teacher) ||
		FALSE == pNPC->CheckNPCTalkDistance(this))
		return E_ForgetSkill_NPCNotValid;

	// todo：检测NPC是否拥有该技能

	// 检查通过，删除技能
	RemoveSkill(dwSkillID);

	// 重新计算属性
	if( NeedRecalAtt() )
		RecalAtt();

	return E_Success;
}

//-----------------------------------------------------------------------------------
// 洗点
//-----------------------------------------------------------------------------------
INT Role::ClearTalent(INT64 n64ItemID, BOOL bCheckItem)
{
	if (bCheckItem)
	{
		// 判断物品本身是不是洗点道具
		tagItem* pItem = GetItemMgr().GetBagItem(n64ItemID);
		if( !P_VALID(pItem) || (pItem->pProtoType->eSpecFunc != EISF_RemoveTalentPt && pItem->pProtoType->eSpecFunc != EISF_ChangeVocation))
			return E_ClearAttPoint_ItemNotValid;

		// 使用的等级判断
		if( pItem->IsLevelLimit(GetLevel()) )
			return E_Item_LevelLimit;

		// 判断通过，删除物品
		GetItemMgr().ItemUsedFromBag(n64ItemID, (INT16)1, (DWORD)ELCID_Clear_Talent);
	}
	
	// 检测是否有天资
	for(INT n = 0; n < X_MAX_TALENT_PER_ROLE; ++n)
	{
		if( ETT_Null == m_Talent[n].eType )
			continue;

		// 删除所有该系的天资技能（对map这样操作，可能会有问题）
		TMap<DWORD, Skill*>::TMapIterator it = m_mapSkill.Begin();
		Skill* pSkill = NULL;
		while( m_mapSkill.PeekNext(it, pSkill) )
		{
			if( pSkill->GetTalentType() == m_Talent[n].eType )
				RemoveSkill(pSkill->GetID());
		}

		// 将点数返回给玩家
		ModAttValue(ERA_TalentPoint, GetTalentPoint(n));
		// 清空该系天资
		RemoveTalent(n);
		// 保存天资到数据库
		SaveTalentPoint2DB(n);
	}

	// 重新计算人物属性
	if( NeedRecalAtt() )
		RecalAtt();

	return E_Success;

}

//----------------------------------------------------------------------------------
// 开启PK保护
//----------------------------------------------------------------------------------
INT Role::OpenSafeGuard(INT64 n64ItemID)
{
	// 查看是否已经设置过了
	if( IsInRoleState(ERS_Safeguard) ) return E_SafeGuard_Already;

	if( P_VALID(GetMap()))
	{
		if( !GetMap()->CanSetSafeguard() )	return E_SafeGuard_PKStateInvalid;
	}

	// 检查一下PK状态及行凶状态
	if( IsInRoleState(ERS_PK) || IsInRoleState(ERS_PKEX) || m_ePKState != ERPKS_Peace || IsInRoleState(ERS_Arena))
		return E_SafeGuard_PKStateInvalid;

	// 检查一下时间
	//tagDWORDTime curTime = g_world.GetWorldTime();
	//if( CalcTimeDiff(curTime, m_CloseSafeGuardTime) < OPEN_SAFE_GUARD_TIME )
	//	return E_SafeGuard_TimeLimit;

	// 检查一下等级
	BOOL bUseItem = FALSE;
	if( GetLevel() >= SAFE_GUARD_FREE_LEVEL )
	{
		// 检查物品是否合法
		const tagItem* pItem = GetItemMgr().GetBagItem(n64ItemID);
		if( !P_VALID(pItem) || pItem->pProtoType->eSpecFunc != EISF_OpenSafeGuard )
		{
			return E_SafeGuard_ItemLimit;
		}
		else
		{
			bUseItem = TRUE;
		}

		// 使用的等级判断
		if( pItem->IsLevelLimit(GetLevel()) )
		{
			return E_Item_LevelLimit;
		}
	}

	// 判断通过，设置玩家为PK保护状态
	//SetRoleState(ERS_Safeguard);

	// 如果是使用了物品打开，则要从背包里删除物品
	if( bUseItem )
	{
		GetItemMgr().ItemUsedFromBag(n64ItemID, (INT16)1, (DWORD)ELCID_Open_SafeGuard);
		SetRoleState(ERS_Safeguard);
		return E_Success;
	}

	return E_SafeGuard_OpenLimit;
}

//----------------------------------------------------------------------------------
// 关闭PK保护
//----------------------------------------------------------------------------------
INT Role::CloseSafeGuard()
{
	// 检查是不是已经是该状态
	if( !IsInRoleState(ERS_Safeguard) ) return E_SafeGuard_Already;

	// 首先检测等级限制
	if( GetLevel() < SAFE_GUARD_FORCE_LEVEL )
		return E_SafeGuard_LevelLimit;

	// 判断通过，撤销玩家的PK保护状态
	UnsetRoleState(ERS_Safeguard);

	// 设置玩家关闭PK保护的时间
	m_CloseSafeGuardTime = g_world.GetWorldTime();

	return E_Success;
}

//----------------------------------------------------------------------------------
// 开启行凶状态
//----------------------------------------------------------------------------------
INT Role::SetPK(BOOL bOnlyPK)
{
	if (bOnlyPK)
	{
		if (IsInRoleState(ERS_PKEX))
			return E_PKEX_Already;
	}
	else
	{
		if (IsInRoleState(ERS_PK))
			return E_PK_Already;
	}

	// 检查玩家是不是开了PK保护
	if( IsInRoleState(ERS_Safeguard) ) return E_PK_SafeGuard;

	// 判断通过，设置玩家为行凶状态（两种行凶状态互斥）
	if (bOnlyPK)
	{
		SetRoleState(ERS_PKEX);
		UnsetRoleState(ERS_PK);
	}
	else
	{
		SetRoleState(ERS_PK);
		UnsetRoleState(ERS_PKEX);
	}

	// 设置撤销行凶状态倒计时
	m_nUnSetPKTickCountDown = UNSET_PK_TICK;
	
	OpenMacroToDealWithLuckState();

	return E_Success;
}

//----------------------------------------------------------------------------------
// 重置行凶状态
//----------------------------------------------------------------------------------
VOID Role::ResetPK()
{
	if( !IsInRoleState(ERS_PK) && !IsInRoleState(ERS_PKEX) ) return;
	m_nUnSetPKTickCountDown = UNSET_PK_TICK;

	tagNS_PK send;
	send.dwErrorCode = E_Success;
	send.bReset = TRUE;
	SendMessage(&send, send.dwSize);
	
	CloseMacroToDealWithLuckState();
}

//----------------------------------------------------------------------------------
// 更新行凶状态
//----------------------------------------------------------------------------------
VOID Role::UpdatePK()
{
	if( !IsInRoleState(ERS_PK) && !IsInRoleState(ERS_PKEX) ) return;

	if(--m_nUnSetPKTickCountDown <= 0)
	{
		UnsetRoleState(ERS_PK);
		UnsetRoleState(ERS_PKEX);
	}
}

//----------------------------------------------------------------------------------
// 开启PVP状态
//----------------------------------------------------------------------------------
VOID Role::SetPVP()
{
	if( !IsInRoleState(ERS_PVP) )
	{
		SetRoleState(ERS_PVP);
	}
	
	m_nUnSetPVPTickCountDown = UNSET_PVP_TICK;
}

//----------------------------------------------------------------------------------
// 更新PVP状态
//-----------------------------------------------------------------------------------
VOID Role::UpdatePVP()
{
	if( !IsInRoleState(ERS_PVP) ) return;

	if(--m_nUnSetPVPTickCountDown <= 0)
	{
		UnsetRoleState(ERS_PVP);
	}
}

//----------------------------------------------------------------------------------
// 计算PK状态
//----------------------------------------------------------------------------------
VOID Role::CalPKState(BOOL bSendMsg)
{
	ERolePKState ePKState = m_ePKState;
	INT n = 0;
	for(; n < ERPKS_End; n++)
	{
		if( m_nAtt[ERA_Morality] > PK_STATE_MORALITY[n] )
		{
			ePKState = (ERolePKState)n;
			break;
		}
	}
	ASSERT( n != ERPKS_End );

	// PK状态改变
	if( ePKState != m_ePKState )
	{
		m_ePKState = ePKState;

		if( bSendMsg )
		{
			tagNS_RolePKStateChange send;
			send.dwRoleID = GetID();
			send.eState = ePKState;
			if( P_VALID(GetMap()) )
			{
				GetMap()->SendBigVisTileMsg(this, &send, send.dwSize);
			}
		}
	}
}

//----------------------------------------------------------------------------------
// 设置戾气为0时回城复活开启pk保护
//----------------------------------------------------------------------------------
VOID Role::SetHostilitySafeGuard()
{
	// 戾气为0回城复活的话给与10分钟的pk保护
	if ( GetAttValue(ERA_Hostility) == 0 )
	{
		m_nDeadUnSetSafeGuardCountDown = REVIVE_UNSETSAFEGUARD_TICK;
		SetRoleState(ERS_Safeguard);
	}
}

//----------------------------------------------------------------------------------
// 更新戾气为0时回城复活开启pk保护
//----------------------------------------------------------------------------------
VOID Role::UpdateHostilitySafeGuard()
{
	// 玩家等级低于30级时不考虑
	if ( m_nLevel < 30 )	return;

	if( !IsInRoleState(ERS_Safeguard) ) return;

	if (m_nDeadUnSetSafeGuardCountDown < 0)
	{
		return;
	}
	else if (m_nDeadUnSetSafeGuardCountDown > 0)
	{
		--m_nDeadUnSetSafeGuardCountDown;
	}
	else
	{
		UnsetRoleState(ERS_Safeguard);
		m_nDeadUnSetSafeGuardCountDown = -1;
	}
}

//----------------------------------------------------------------------------------
// 经验改变
//----------------------------------------------------------------------------------
VOID Role::ExpChange(INT nValue, BOOL bKill, BOOL bForce)
{
	if( 0 == nValue ) return;

	// 减少经验
	if( nValue < 0 )
	{
		INT nTemp = m_nCurLevelExp;
		m_nCurLevelExp += nValue;
		if( m_nCurLevelExp < 0 ) m_nCurLevelExp = 0;

		tagNS_RoleExpChange send;
		send.nExp = m_nCurLevelExp;
		send.nChange = m_nCurLevelExp - nTemp;
		send.bKill = bKill;
		SendMessage(&send, send.dwSize);

		if (!bKill)
		{
			SaveExp2DB();
		}

		return;
	}

	// 如果是增加经验
	const tagLevelUpEffect* pEffect = g_attRes.GetLevelUpEffect(m_nLevel);
	INT nLevelUpExpRemain = pEffect->nExpLevelUp - m_nCurLevelExp;
	ASSERT( nLevelUpExpRemain >= 0 );

	// 不会升级
	if( nValue < nLevelUpExpRemain )
	{
		// 检测是否到了10%的倍数，如果到了，则给玩家回满血回满蓝
		INT nPhaseExp = pEffect->nExpLevelUp / 10;
		INT nOldPhase = m_nCurLevelExp / nPhaseExp;
		INT nNewPhase = (m_nCurLevelExp + nValue) / nPhaseExp;
		if( nOldPhase != nNewPhase )
		{
			SetAttValue(ERA_HP, GetAttValue(ERA_MaxHP));
			SetAttValue(ERA_MP, GetAttValue(ERA_MaxMP));
		}

		// 累加上新的经验
		m_nCurLevelExp += nValue;

		tagNS_RoleExpChange send;
		send.nExp = m_nCurLevelExp;
		send.nChange = nValue;
		send.bKill = bKill;
		SendMessage(&send, send.dwSize);
	}

	// 需要升级
	else
	{
		// 根据是否强制升级来决定技能的等级上限
		INT nMaxLevel = ROLE_LEVEL_LIMIT;
		if( bForce )
		{
			nMaxLevel = MAX_ROLE_LEVEL;
		}

		m_nCurLevelExp += nLevelUpExpRemain;

		// 已经到达上限
		if( m_nLevel == nMaxLevel )
		{
			if( nLevelUpExpRemain != 0 )
			{
				tagNS_RoleExpChange send;
				send.nExp = m_nCurLevelExp;
				send.nChange = nLevelUpExpRemain;
				send.bKill = bKill;
				SendMessage(&send, send.dwSize);
			}
		}

		// 可以升级
		else
		{
			INT nTemExpValue = nValue;
			INT nExpRealAdd = nLevelUpExpRemain;
			nValue -= nLevelUpExpRemain;
			INT nNextLevel = m_nLevel + 1;
			INT nAttPointAdd = 0;
			INT nTalentPointAdd = 0;

			// 看人物能升几级
			for(; nNextLevel <= nMaxLevel; nNextLevel++)
			{
				pEffect = g_attRes.GetLevelUpEffect(nNextLevel);

				// 还可以再继续升级
				if( pEffect->nExpLevelUp <= nValue )
				{
					nValue -= pEffect->nExpLevelUp;
					nExpRealAdd = pEffect->nExpLevelUp;
					nAttPointAdd += pEffect->n16RoleAttAvail;
					nTalentPointAdd += pEffect->n16TalentAvail;
				}
				// 不可以再升级了
				else
				{
					nAttPointAdd += pEffect->n16RoleAttAvail;
					nTalentPointAdd += pEffect->n16TalentAvail;
					break;
				}
			}

			// 给人物改变等级
			if( nNextLevel > nMaxLevel )
			{
				nNextLevel = nMaxLevel;
				// 经验修正一下
				if( nValue > pEffect->nExpLevelUp )
				{
					nValue = pEffect->nExpLevelUp;
				}
			}

			LevelChange(nNextLevel, bKill, bForce);

			m_nCurLevelExp = nValue;
			nExpRealAdd += nValue;

			// 给玩家增加属性点和天资点
			ModAttValue(ERA_AttPoint, nAttPointAdd);
			ModAttValue(ERA_TalentPoint, nTalentPointAdd);

			// 发送经验改变
			tagNS_RoleExpChange send;
			send.nExp = m_nCurLevelExp;
			send.nChange = nTemExpValue;//nExpRealAdd;
			send.bKill = bKill;
			SendMessage(&send, send.dwSize);

			SaveAttPoint2DB();
			SaveTalentPoint2DB(0);
			//m_SaveRoleLock.Acquire();
			//SaveToDB();
			//m_SaveRoleLock.Release();
		}
	}
	if (!bKill)
	{
		SaveExp2DB();
	}
}

//----------------------------------------------------------------------------
// 改变等级
//----------------------------------------------------------------------------
VOID Role::LevelChange(INT nValue, BOOL bKill, BOOL bForce)
{
	// 根据是否强制升级来决定最高等级
	INT nMaxLevel = ROLE_LEVEL_LIMIT;
	if( bForce ) nMaxLevel = MAX_ROLE_LEVEL;

	if( nValue < 0 || nValue > nMaxLevel )
		return;

	if( m_nLevel == nValue ) return;

	m_nLevel = nValue;
	m_nCurLevelExp = 0;

	// 发送等级改变
	tagNS_RoleLevelChange send;
	send.dwRoleID = GetID();
	send.nLevel = m_nLevel;
	send.bKill = send.bKill;

	SaveLevel2DB();

	if( P_VALID(GetMap()) )
	{
		GetMap()->SendBigVisTileMsg(this, &send, send.dwSize);
	}

	// 玩家在小队中,发送等级改变事件
	if(GetTeamID() != GT_INVALID)
		g_groupMgr.AddEvent(GetID(), EVT_ChangeLevel, send.dwSize, &send);

	// 同步tagRoleInfo
	g_socialMgr.AddEvent(GetID(), EVT_SynRoleLevel, send.dwSize, &send);

	// 称号事件触发
	GetTitleMgr()->SigEvent(ETE_ROLE_LEVEL, GT_INVALID, m_nLevel);

	// 重置人物的以及属性
	const tagLevelUpEffect* pEffect = g_attRes.GetLevelUpEffect(m_nLevel);
	SetBaseAttValue(ERA_Physique,	pEffect->n16RoleAtt[0] + m_nAttPointAdd[0]);
	SetBaseAttValue(ERA_Strength,	pEffect->n16RoleAtt[1] + m_nAttPointAdd[1]);
	SetBaseAttValue(ERA_Pneuma,		pEffect->n16RoleAtt[2] + m_nAttPointAdd[2]);
	SetBaseAttValue(ERA_InnerForce,	pEffect->n16RoleAtt[3] + m_nAttPointAdd[3]);
	SetBaseAttValue(ERA_Technique,	pEffect->n16RoleAtt[4] + m_nAttPointAdd[4]);
	SetBaseAttValue(ERA_Agility,	pEffect->n16RoleAtt[5] + m_nAttPointAdd[5]);

	// 设置某些基础二级属性
	SetBaseAttValue(ERA_MaxHP,		pEffect->n16HP);
	SetBaseAttValue(ERA_MaxMP,		pEffect->n16MP);
	SetBaseAttValue(ERA_MaxVitality,pEffect->n16Vitality);

	RecalAtt();

	// 回满血，回满蓝
	SetAttValue(ERA_HP, GetAttValue(ERA_MaxHP));
	SetAttValue(ERA_MP, GetAttValue(ERA_MaxMP));

	// 如果到了30级，关闭pk保护
	INT nRet = E_Success;
	if (m_nLevel >= 30)
	{
		if( !m_bLevelUpClosedSafeGuard )
		{
			nRet = CloseSafeGuard();

			// 发送消息给客户端
			tagNS_SafeGuard send;
			send.bOpen = FALSE;
			send.dwErrorCode = nRet;
			SendMessage(&send, send.dwSize);

			m_bLevelUpClosedSafeGuard = TRUE;
		}
		
	}

	if (bForce)
	{
		PlayerSession* pSession = GetSession();
		if (P_VALID(pSession) && !pSession->IsPrivilegeEnough(1))
		{
			tagNWL_ForbidAccount send;
			send.dwAccountID = pSession->GetSessionID();
			send.dwForbidCode = EPLM_Level;
			g_loginSession.Send(&send, send.dwSize);
		}
	}

	// 调用升级的脚本函数
	if( P_VALID(m_pScript))
	{
		m_pScript->OnRoleLevelChange(this);
	}
}

//-------------------------------------------------------------------------------
// 投点
//-------------------------------------------------------------------------------
INT Role::BidAttPoint(const INT nAttPointAdd[X_ERA_ATTA_NUM])
{
	// 检测点数是否合法
	INT nSum = 0;
	for(INT n = 0; n < X_ERA_ATTA_NUM; n++)
	{
		if( nAttPointAdd[n] < 0 )
			return E_BidAttPoint_PointInvalid;

		nSum += nAttPointAdd[n];
	}

	// 如果总点数为0，则直接返回
	if( nSum <= 0 ) return E_BidAttPoint_PointInvalid;

	// 检测玩家当前身上的可投放属性点够不够
	if( GetAttValue(ERA_AttPoint) < nSum )
		return E_BidAttPoint_NoEnoughAttPoint;

	// 先扣除属性点
	ModAttValue(ERA_AttPoint, -nSum);

	// 再依次对应每个属性进行加成
	for(INT n = 0; n < X_ERA_ATTA_NUM; n++)
	{
		if( 0 == nAttPointAdd[n] ) continue;

		m_nAttPointAdd[n] += nAttPointAdd[n];
		ModBaseAttValue(MTransIndex2ERAATTA(n), nAttPointAdd[n]);
	}

	// 重算人物属性
	RecalAtt();

	// 发送每个属性投放量当前值给客户端
	tagNS_RoleAttPointAddChange send;
	IFASTCODE->MemCpy(send.nAttPointAdd, m_nAttPointAdd, sizeof(m_nAttPointAdd));
	SendMessage(&send, send.dwSize);

	SaveAttPoint2DB();

	return E_Success;
}

//----------------------------------------------------------------------------
// 洗属性点
//----------------------------------------------------------------------------
INT Role::ClearAttPoint(INT64 n64ItemID, BOOL bItemCheck)
{
	// 判断物品本身是不是洗点道具
	tagItem* pItem = GetItemMgr().GetBagItem(n64ItemID);
	if (bItemCheck)
	{
		if( !P_VALID(pItem) || ( pItem->pProtoType->eSpecFunc != EISF_RemoveAttPt ) )
			return E_ClearAttPoint_ItemNotValid;

		// 使用的等级判断
		if( pItem->IsLevelLimit(GetLevel()) )
			return E_Item_LevelLimit;

		// 判断通过，删除物品
		GetItemMgr().ItemUsedFromBag(n64ItemID, (INT16)1, (DWORD)ELCID_Clear_Att);
	}

	// 清掉玩家所有一级属性投放
	INT nSum = 0;

	for(INT n = 0; n < X_ERA_ATTA_NUM; n++)
	{
		if( 0 == m_nAttPointAdd[n] ) continue;

		ModBaseAttValue(MTransIndex2ERAATTA(n), -m_nAttPointAdd[n]);
		nSum += m_nAttPointAdd[n];
		m_nAttPointAdd[n] = 0;
	}

	// 重算人物属性
	RecalAtt();

	// 更新当前可投放属性点
	ModAttValue(ERA_AttPoint, nSum);

	// 发送每个属性投放量当前值给客户端
	tagNS_RoleAttPointAddChange send;
	IFASTCODE->MemCpy(send.nAttPointAdd, m_nAttPointAdd, sizeof(m_nAttPointAdd));
	SendMessage(&send, send.dwSize);

	// 属性点存数据库
	SaveAttPoint2DB();

	return E_Success;
}

//-------------------------------------------------------------------------------
// 被攻击
//-------------------------------------------------------------------------------
VOID Role::OnBeAttacked(Unit* pSrc, Skill* pSkill, BOOL bHited, BOOL bBlock, BOOL bCrited)
{
	ASSERT( P_VALID(pSkill) && P_VALID(pSrc) );

	// 判断技能的敌我目标类型
	if( !pSkill->IsHostile() && !pSkill->IsFriendly() ) return;

	DWORD dwFriendEnemyFlag = pSrc->FriendEnemy(this);

	// 如果该技能可以对敌方使用，并且双方包含敌方关系
	if( pSkill->IsHostile() && this != pSrc && (ETFE_Hostile & dwFriendEnemyFlag) )
	{
		// 如果攻击方也是玩家，则设置双方的PVP状态和行凶状态
		if( pSrc->IsRole() )
		{
			Role* pSrcRole = static_cast<Role*>(pSrc);

			// 重置双方的PVP状态
			SetPVP();
			pSrcRole->SetPVP();

			// 被攻击者重置行凶状态
			ResetPK();

			// 如果攻击者在行凶状态，则重置行凶时间
			if( pSrcRole->IsInRoleState(ERS_PK) || pSrcRole->IsInRoleState(ERS_PKEX) )
			{
				pSrcRole->ResetPK();	
			}

			// 如果玩家处在非行凶状态，则向客户端发送警告
			if ( !IsInRoleState(ERS_PK) && !IsInRoleState(ERS_PKEX) )
			{
				tagNS_RoleBeAttacked send;
				SendMessage(&send, send.dwSize);
			}
		}
		else if( pSrc->IsCreature() )
		{
			// 怪物攻击玩家，增加一点仇恨
			Creature* pSrcCreature = static_cast<Creature*>(pSrc);
			if( P_VALID(pSrcCreature->GetAI()) )
				pSrcCreature->GetAI()->AddEnmity(this, 1);
		}

		// 检测身上的被动技能和装备的Buff触发
		if( bHited )
		{
			OnPassiveSkillBuffTrigger(pSrc, ETEE_Hited, 0, 0);
			OnEquipmentBuffTrigger(NULL, ETEE_Hited);
			OnBuffTrigger(pSrc, ETEE_Hited, 0, 0);
			if( bBlock )
			{
				OnPassiveSkillBuffTrigger(pSrc, ETEE_Block, 0, 0);
				OnEquipmentBuffTrigger(NULL, ETEE_Block);
				OnBuffTrigger(pSrc, ETEE_Block, 0, 0);
			}
			if( bCrited )
			{
				OnPassiveSkillBuffTrigger(pSrc, ETEE_Crited, 0, 0);
				OnEquipmentBuffTrigger(NULL, ETEE_Crited);
				OnBuffTrigger(pSrc, ETEE_Crited, 0, 0);
			}

			// 检测当前技能的打断
			GetCombatHandler().InterruptPrepare(CombatHandler::EIT_Skill, ESSTE_Default == pSkill->GetTypeEx());

			// 检测某些被攻击打断的buff
			OnInterruptBuffEvent(EBIF_BeAttacked);
		}
		else
		{
			OnPassiveSkillBuffTrigger(pSrc, ETEE_Dodge, 0, 0);
			OnEquipmentBuffTrigger(NULL, ETEE_Dodge);
			OnBuffTrigger(pSrc, ETEE_Dodge, 0, 0);
		}
		OnPassiveSkillBuffTrigger(pSrc, ETEE_BeAttacked, 0, 0);
		OnEquipmentBuffTrigger(NULL, ETEE_BeAttacked);
		OnBuffTrigger(pSrc, ETEE_BeAttacked, 0, 0);
	}

	// 如果该技能可以对友方使用，并且双方包含友方关系
	if( pSkill->IsFriendly() && (ETFE_Friendly & dwFriendEnemyFlag) )
	{
		// 仇恨值
		INT	nValue = pSkill->GetEnmity() + pSrc->GetAttValue(ERA_Enmity_Degree);

		// 玩家身上被动仇恨所属的怪物全部增加仇恨
		DWORD dwCreatureID = GT_INVALID;
		Creature* pCreature = (Creature*)GT_INVALID;
		m_mapEnmityCreature.ResetIterator();
		while( m_mapEnmityCreature.PeekNext(dwCreatureID) )
		{
			pCreature = GetMap()->FindCreature(dwCreatureID);
			if( !P_VALID(pCreature) ) continue;

			if( P_VALID(pCreature->GetAI()) )
				pCreature->GetAI()->AddEnmity(pSrc, nValue);
		}
	}
}

//-------------------------------------------------------------------------------
// 死亡
//-------------------------------------------------------------------------------
VOID Role::OnDead(Unit* pSrc, Skill* pSkill/*=NULL*/, const tagBuffProto* pBuff/*=NULL*/, DWORD dwSerial, DWORD dwMisc)
{
	// 如果本身已经死亡，则直接返回
	if( IsInState(ES_Dead) ) return;

	// 死亡时触发
	OnPassiveSkillBuffTrigger(pSrc, ETEE_Die);
	OnEquipmentBuffTrigger(pSrc, ETEE_Die);
	OnBuffTrigger(pSrc, ETEE_Die);

	// 设置为死亡状态
	SetState(ES_Dead);

	// 停止移动
	m_MoveData.StopMoveForce();

	// 停止当前正在释放的技能
	m_CombatHandler.End();

	// 移除身上所有死亡后消失的buff
	OnInterruptBuffEvent(EBIF_Die);

	// 清空仇恨列表
	ClearEnmityCreature();

	// 脚本判断是否进行装备掉落惩罚
	BOOL bEquipLootPenalty = IsDeadPenalty(EDPM_EquipLoot);

	// 是否有经验惩罚
	BOOL bExpPenalty = IsDeadPenalty(EDPM_Exp);

	// 是否有内伤惩罚
	BOOL bInjuryPenalty = IsDeadPenalty(EDPM_Injury);

	// 是否有戾气惩罚
	BOOL bHostilityPenalty = IsDeadPenalty(EDPM_Hostility);

	// 是否有跑商商银惩罚
	BOOL bCommercePenalty = IsDeadPenalty(EDPM_Commerce);

	BOOL bEquip = FALSE;
	BOOL bExp = FALSE;
	BOOL bInjury = FALSE;
	BOOL bHostility = FALSE;
	BOOL bCommerce = FALSE;

	// 是否能免除死亡惩罚
	CanCancelPenalty(pSrc, bEquip, bExp, bInjury, bHostility, bCommerce);
	
	if (bEquip)
		bEquipLootPenalty = FALSE;		// 免除装备掉落惩罚

	if (bExp)
		bExpPenalty = FALSE;			// 免除经验惩罚

	if (bInjury)
		bInjuryPenalty = FALSE;			// 免除内伤惩罚

	if (bHostility)
		bHostilityPenalty = FALSE;		// 免除戾气惩罚

	if (bCommerce)
		bCommercePenalty = FALSE;		// 免除跑商商银惩罚


	// 死亡惩罚
	if (bEquipLootPenalty)
		DeadPenalty(pSrc, EDPM_EquipLoot);

	if (bExpPenalty)
		DeadPenalty(pSrc, EDPM_Exp);

	if (bInjuryPenalty)
		DeadPenalty(pSrc, EDPM_Injury);

	if (bHostilityPenalty)
		DeadPenalty(pSrc, EDPM_Hostility);

	if (bCommercePenalty)
		DeadPenalty(pSrc, EDPM_Commerce);

	// 击杀者
	if( P_VALID(pSrc) )
	{
		pSrc->OnKill(this);
	}

	// 调用地图事件
	if( P_VALID(GetMap()) )
	{
		GetMap()->OnRoleDie(this, pSrc);
	}

	// 取消行凶
	UnsetRoleState(ERS_PK);
	UnsetRoleState(ERS_PKEX);

	// 发送角色死亡消息
	tagNS_RoleDead send;
	send.dwRoleID = GetID();
	send.dwSrcRoleID = (P_VALID(pSrc) ? pSrc->GetID() : GT_INVALID);
	send.eSuggestRevive = ERRT_None;
	if( P_VALID(pSkill) )
	{
		send.eCause = ERDC_Skill;
		send.dwMisc = pSkill->GetTypeID();
	}
	else if( P_VALID(pBuff) )
	{
		send.eCause = ERDC_Buff;
		send.dwMisc = pBuff->dwID;
	}
	else
	{
		send.eCause = ERDC_Other;
		send.dwMisc = GT_INVALID;
	}

	if (P_VALID(pSrc) && pSrc->IsRole() && ERPKS_Wanted == GetPKState())
	{
		// 需要牢狱复活
		m_bNeedPrisonRevive = TRUE;
		// 牢狱复活
		send.eSuggestRevive = ERRT_Prison;

		tstringstream tszMsg;
		TCHAR tszRoleName[X_SHORT_NAME] = {0};
		g_roleMgr.GetRoleNameByID(send.dwSrcRoleID, tszRoleName);
		tszMsg << tszRoleName << _T("为民除害，将");
		g_roleMgr.GetRoleNameByID(send.dwRoleID, tszRoleName);
		tszMsg << tszRoleName << _T("送入大牢");

		DWORD dwLen = sizeof(tagNS_AutoNotice) - sizeof(TCHAR) + 2*(tszMsg.str().length() + 1);

		MCREATE_MSG(pSendMsg, dwLen, NS_AutoNotice);

		pSendMsg->nCirInterval = 60;
		pSendMsg->nType = 0;
		_tcscpy(pSendMsg->szContent, tszMsg.str().c_str());

		g_roleMgr.SendWorldMsg(pSendMsg, dwLen);

		MDEL_MSG(pSendMsg);

	}
	send.dwMisc2 = dwMisc;
	send.dwSerial = dwSerial;

	if( P_VALID(GetMap()) )
	{
		GetMap()->SendBigVisTileMsg(this, &send, send.dwSize);
	}
}

//---------------------------------------------------------------------------------
// 击杀目标
//---------------------------------------------------------------------------------
VOID Role::OnKill(Unit* pTarget)
{
	ASSERT( P_VALID(pTarget) );

	// 击杀的是玩家
	if( pTarget->IsRole() )
	{
		// 触发被动技能和装备
		OnPassiveSkillBuffTrigger(pTarget, ETEE_Kill);
		OnEquipmentBuffTrigger(pTarget, ETEE_Kill);
		OnBuffTrigger(pTarget, ETEE_Kill);

		Role* pRole = static_cast<Role*>(pTarget);
		ERolePKState ePKState = pRole->GetPKState();

		// 是否有道德值惩罚
		if (IsDeadPenalty(EDPM_Morality))
		{
			INT nMoralityDec = 0;

			// 击杀任何在pvp擂台（或城战），或者戾气状态的玩家，不改变道德值
			if(!pRole->IsInRoleState(ERS_Arena) && !pRole->IsInRoleState(ERS_Hostility) )
			{
				// 击杀非行凶状态的玩家，改变道德值-10, -5, -1
				if (!pRole->IsInRoleState(ERS_PK) && !pRole->IsInRoleState(ERS_PKEX))
				{
					if( ERPKS_Peace == ePKState )
						nMoralityDec =  -10;
					else if( ERPKS_Warnning == ePKState )
						nMoralityDec = -5;
					else if( ERPKS_Wicked == ePKState )
						nMoralityDec = -1;
				}
				
				// 击杀行凶状态的玩家，改变道德值-3, -1, 0
				else
				{
					if( ERPKS_Peace == ePKState )
						nMoralityDec =  -3;
					else if( ERPKS_Warnning == ePKState )
						nMoralityDec = -1;
					else if( ERPKS_Wicked == ePKState )
						nMoralityDec = 0;
				}
			}

			if( nMoralityDec != 0 )
				ModAttValue(ERA_Morality, nMoralityDec);
		}
		
		// 是否有戾气值惩罚
		if (IsDeadPenalty(EDPM_Hostility))
		{
			// 击杀任何不在pvp擂台（或城战），既非行凶状态也非戾气状态的玩家，改变戾气
			if( !pRole->IsInRoleState(ERS_PK) && !pRole->IsInRoleState(ERS_PKEX) && !pRole->IsInRoleState(ERS_Arena) && 
				pRole->GetAttValue(ERA_Hostility)==0 )
			{
				INT nAdd = 0;

				if (pRole->GetPKState() == ERPKS_Peace)
				{
					nAdd = 3;		// 如果击杀百名玩家，戾气增加3
				}
				else
				{
					nAdd = 1;		// 如果击杀黄名，橙名或紫名玩家，戾气增加1
				}

				// 修改戾气值前先将玩家身上的戾气值对应buff去掉，再添加一个新的buff
				INT nSrcHostility = GetAttValue(ERA_Hostility);
				if (nSrcHostility > 0)
				{
					RemoveBuff(Buff::GetIDFromTypeID( HOSTILITY_DEADPENALTY_BUFF_ID[nSrcHostility - 1]), TRUE );
				}

				// 改变击杀者戾气值
				SetAttValue( ERA_Hostility, min( GetAttValue(ERA_Hostility) + nAdd, MAX_HOSTILITY ) );

				// 添加惩罚buff
				nSrcHostility = GetAttValue(ERA_Hostility);
				if (nSrcHostility > 0)
				{
					const tagBuffProto* pProto = g_attRes.GetBuffProto( HOSTILITY_DEADPENALTY_BUFF_ID[nSrcHostility - 1] );
					if( P_VALID(pProto) )
					{
						TryAddBuff(this, pProto, NULL, NULL, NULL);
					}
				}
				
				
				if ( !this->IsInRoleState(ERS_Hostility) )
				{
					// 玩家成为戾气状态
					SetRoleState(ERS_Hostility);
				}
			}

			// 被击杀玩家先去除对应戾气值的buff
			INT nTargetHostility = pRole->GetAttValue(ERA_Hostility);
			if (nTargetHostility > 0)
			{
				pRole->RemoveBuff(Buff::GetIDFromTypeID( HOSTILITY_DEADPENALTY_BUFF_ID[nTargetHostility - 1]), TRUE );
			}

			// 被击杀玩家，戾气值减1
			pRole->SetAttValue( ERA_Hostility, max( 0, pRole->GetAttValue(ERA_Hostility) - 1 ) );

			// 添加惩罚buff
			nTargetHostility = pRole->GetAttValue(ERA_Hostility);
			if (nTargetHostility > 0)
			{
				const tagBuffProto* pProto = g_attRes.GetBuffProto( HOSTILITY_DEADPENALTY_BUFF_ID[nTargetHostility - 1] );
				if( P_VALID(pProto) )
				{
					pRole->TryAddBuff(pRole, pProto, NULL, NULL, NULL);
				}
			}
			

			// 如果被击杀玩家戾气值为0并且处于戾气状态，则取消被击杀玩家戾气状态
			if ( pRole->GetAttValue(ERA_Hostility) == 0 && pRole->IsInRoleState(ERS_Hostility) )
			{
				pRole->UnsetRoleState(ERS_Hostility);
			}
		}

		// 广播击杀通缉玩家信息
		if (ERPKS_Wanted == ePKState)
		{
			tagNS_SendToPrison send;
			send.dwCaptorRoleID = GetID();
			send.dwCatchedRoleID = pRole->GetID();
			
			g_roleMgr.SendWorldMsg(&send, send.dwSize);
		}

		// 称号事件触发
		GetTitleMgr()->SigEvent(ETE_KILL_ROLE, GT_INVALID, GT_INVALID);
	}

	// 如果击杀的是怪物
	else if( pTarget->IsCreature() )
	{
		Creature* pCreature = dynamic_cast<Creature*>(pTarget);
		if (!P_VALID(pCreature))
			return;

		// 触发被动技能和装备
		if( !pCreature->IsGameObject() )
		{
			OnPassiveSkillBuffTrigger(pTarget, ETEE_Kill);
			OnEquipmentBuffTrigger(pTarget, ETEE_Kill);
			OnBuffTrigger(pTarget, ETEE_Kill);
		}

		// 称号相关
		if (pCreature->IsNPC())
		{
			GetTitleMgr()->SigEvent(ETE_KILL_NPC, pCreature->GetTypeID(), GT_INVALID);
		}
		else if (pCreature->IsMonster())
		{
			GetTitleMgr()->SigEvent(ETE_KILL_MONSTER, pCreature->GetTypeID(), GT_INVALID);
		}
		else if (pCreature->IsBoss())
		{
			GetTitleMgr()->SigEvent(ETE_KILL_BOSS, pCreature->GetTypeID(), GT_INVALID);
		}
	}
}

//---------------------------------------------------------------------------------
// 是否进行死亡惩罚
//---------------------------------------------------------------------------------
BOOL Role::IsDeadPenalty(EDeadPenaltyMode eMode)
{
	if(P_VALID(m_pScript))
		return m_pScript->IsDeadPenalty(this, eMode);

	return TRUE;
}

//---------------------------------------------------------------------------------
// 死亡惩罚
//---------------------------------------------------------------------------------
VOID Role::DeadPenalty(Unit* pSrc, EDeadPenaltyMode eMode)
{
	// 如果玩家是级别小于死亡等级惩罚的等级下限，则不受任何死亡惩罚
	if( GetLevel() < DEAD_PENALTY_LEVEL ) return;

	// 如果人物在PVP区死亡，则不会受到任何死亡惩罚
	if( IsInRoleState(ERS_PVPArea) ) return;

	switch(eMode)
	{
	case EDPM_EquipLoot:
		{
			// 装备掉落惩罚，PK保护玩家不受惩罚，被非玩家击杀也不受装备掉落的惩罚
			if( !IsInRoleState(ERS_Safeguard) && P_VALID(pSrc) && pSrc->IsRole() )
			{
				INT nLootProp = LOOT_RATE_PER_PK_STATE[m_ePKState];
				if( nLootProp > 0 &&  IUTIL->Rand() % 10000 < nLootProp )
				{
					// 80%的几率掉落1件装备或道具
					// 15%的几率掉落2件装备或道具
					// 4%的几率掉落3件装备或道具
					// 0.8%的几率掉落4件装备或道具
					// 0.2%的几率掉落5件装备或道具
					INT nNum = 0;
					INT nProp = IUTIL->Rand() % 10000;

					if( nProp < 8000 )			nNum = 1;
					else if( nProp < 9500 )		nNum = 2;
					else if( nProp < 9900 )		nNum = 3;
					else if( nProp < 9980 )		nNum = 4;
					else						nNum = 5;

					// 根据个数从背包或者装备栏中掉落物品
					TList<tagItem*> listLootItems;
					TList<tagEquip*> listLootEquips;

					// 随即确定删除的装备和物品数量
					INT nEquip = IUTIL->Rand() % (nNum + 1);
					INT nItem = nNum - nEquip;

					// 掉落装备
					GetItemMgr().LootFromEquipBar(nEquip, listLootEquips, ELCID_Dead_Penalty);

					// 生成地面装备
					tagEquip* pEquip = NULL;
					TList<tagEquip*>::TListIterator iterEquip = listLootEquips.Begin();
					while (listLootEquips.PeekNext(iterEquip, pEquip))
					{
						if (!P_VALID(pEquip))
						{
							continue;
						}

						if( GetMap() )
						{
							tagGroundItem* pGroundItem = new tagGroundItem(GetMap()->GenGroundItemID(), pEquip->dwTypeID, 
								pEquip->n16Num, pEquip, GetCurPos(), GT_INVALID, 
								GT_INVALID, 0, GetID());

							ASSERT(pGroundItem->IsValid());

							GetMap()->AddGroundItem(pGroundItem);
						}
						else
						{
							Destroy(pEquip);
						}
					}

					// 掉落物品
					GetItemMgr().LootFromBag(nItem, listLootItems, ELCID_Dead_Penalty);

					// 生成地面物品
					tagItem* pItem = NULL;
					TList<tagItem*>::TListIterator iterItem = listLootItems.Begin();
					while (listLootItems.PeekNext(iterItem, pItem))
					{
						if (!P_VALID(pItem))
						{
							continue;
						}

						if( GetMap() )
						{
							tagGroundItem* pGroundItem = new tagGroundItem(GetMap()->GenGroundItemID(), pItem->dwTypeID, 
								pItem->n16Num, pItem, GetCurPos(), GT_INVALID, 
								GT_INVALID, 0, GetID());

							ASSERT(pGroundItem->IsValid());

							GetMap()->AddGroundItem(pGroundItem);
						}
						else
						{
							Destroy(pItem);
						}
					}
				}
			}
		}
		break;

	case EDPM_Exp:
		{
			// 经验惩罚（只有行凶状态的玩家才会损失经验）
			if( IsInRoleState(ERS_PK) || IsInRoleState(ERS_PKEX) )
			{
				// 只有被玩家击杀的情况下才会损失经验
				if( P_VALID(pSrc) && pSrc->IsRole() )
				{
					// 损失的经验：[150/(角色等级^2)] %×升级经验
					const tagLevelUpEffect* pEffect = g_attRes.GetLevelUpEffect(GetLevel());
					ASSERT( P_VALID(pEffect) );
					INT level = GetLevel();
					INT nExp = FLOAT(pEffect->nExpLevelUp / level) / level * 150 ;
					ExpChange(-nExp);
				}
			}
		}

		break;

	case EDPM_Injury:
		{
			// 内伤惩罚
			// 29%几率不受内伤
			// 30%几率内伤+1
			// 20%几率内伤+2
			// 15%几率内伤+3
			// 5%几率内伤+5
			// 1%几率内伤+10
			INT nInjury = 0;
			INT nProp = IUTIL->Rand() % 100;

			if( nProp < 29 )			nInjury = 0;
			else if( nProp < 59 )		nInjury = 1;
			else if( nProp < 79 )		nInjury = 2;
			else if( nProp < 94 )		nInjury = 3;
			else if( nProp < 99 )		nInjury = 5;
			else						nInjury = 10;

			if( nInjury != 0 )
				ModAttValue(ERA_Injury, nInjury);

		}
		break;

	case EDPM_Hostility:
		{
			// 戾气状态下死亡，得到属性惩罚
			/*if (IsInRoleState(ERS_Hostility))
			{
				const tagBuffProto* pProto = g_attRes.GetBuffProto(HOSTILITY_DEADPENALTY_BUFF_ID);
				if( P_VALID(pProto) )
				{
					TryAddBuff(this, pProto, NULL, NULL, NULL);
				}
			}*/
		}
		break;

	case EDPM_Commerce:
		{
			// 跑商商银惩罚
			Guild* pGuild = g_guildMgr.GetGuild(GetGuildID());
			if (!P_VALID(pGuild))				return;
			if (!IsInRoleState(ERS_Commerce))	return;
			GuildCommodity* pCommodity = pGuild->GetGuildCommerce().GetCommodity(GetID());
			if (!P_VALID(pCommodity))			return;
			pCommodity->DeadPenalty();
		}
		break;

	default:
		break;
	}

	
	
}

//---------------------------------------------------------------------------------
// 是否能免除死亡惩罚
//---------------------------------------------------------------------------------
VOID Role::CanCancelPenalty(Unit* pSrc, BOOL& bEquip, BOOL& bExp, BOOL& bInjury, BOOL& bHostility, BOOL& bCommercePenalty)
{
	if( !P_VALID(pSrc) ) return;
	if (pSrc->IsRole())
	{
		if(P_VALID(m_pScript))
		{
			m_pScript->CanCancelPenalty(this, pSrc, bEquip, bExp, bInjury, bHostility, bCommercePenalty);
		}
	}
}

//---------------------------------------------------------------------------------
// 目标的类型标志
//---------------------------------------------------------------------------------
DWORD Role::TargetTypeFlag(Unit* pTarget)
{
	if( !P_VALID(pTarget) ) return 0;

	if( pTarget->IsRole() )
		return TargetTypeFlag(static_cast<Role*>(pTarget));
	else if( pTarget->IsCreature() )
		return TargetTypeFlag(static_cast<Creature*>(pTarget));
	else
		return 0;
}

DWORD Role::TargetTypeFlag(Role* pTarget)
{
	if( !P_VALID(pTarget) ) return 0;

	// 如果目标就是自己，则返回自身
	if( this == pTarget ) return ETF_Self;

	DWORD dwFlag = 0;

	// todo：检测玩家跟自己是不是某些社会关系，如果是，则设置上相关的某些字段

	// 小队队友
	if( IsTeamMate(pTarget) )
	{
		dwFlag |= ETF_Teammate;
	}

	// 当然属于某些社会关系的人也属于玩家
	dwFlag |= ETF_Player;

	return dwFlag;
}

DWORD Role::TargetTypeFlag(Creature* pTarget)
{
	if( !P_VALID(pTarget) ) return 0;

	DWORD dwFlag = 0;

	// 怪物
	if( pTarget->IsMonster() )
	{
		// 普通怪物
		if( pTarget->IsNormalMonster() )
		{
			if( !pTarget->IsBoss() )
				dwFlag |= ETF_NormalMonster;
			else
				dwFlag |= ETF_Boss;
		}

		// 巢穴
		else if( pTarget->IsNest())
		{
			dwFlag |= ETF_Nest;
		}
	}

	// NPC
	else if( pTarget->IsNPC() )
	{
		dwFlag |= ETF_NPC;
	}

	// 宠物
	else if( pTarget->IsPet() )
	{
		dwFlag |= ETF_Pet;
	}

	// 资源
	else if( pTarget->IsRes() )
	{
		if ( pTarget->IsManRes() )
			dwFlag |= ETF_ManRes;
		else if ( pTarget->IsNatuRes() )
			dwFlag |= ETF_NatuRes;
		else
			ASSERT(0);
	}

	// 可调查地物
	else if( pTarget->IsInves() )
	{
		dwFlag |= ETF_InvesGameObject;
	}
	// 门
	else if (pTarget->IsDoor())
	{
		dwFlag |= ETF_Door;
	}

	return dwFlag;
}

//---------------------------------------------------------------------------------
// 与目标的敌友中立判断
//---------------------------------------------------------------------------------
DWORD Role::FriendEnemy(Unit* pTarget)
{
	if( !P_VALID(pTarget) ) return 0;
	if( !P_VALID(GetMap()) || GetMap() != pTarget->GetMap() ) return 0;

	// 自身的话，则均满足
	if( this == pTarget )
	{
		return ETFE_Friendly | ETFE_Hostile | ETFE_Independent;
	}

	// 取所在地图的敌我关系判定
	BOOL bIgnore = FALSE;
	DWORD dwMapFlag = GetMap()->GetFriendEnemyFlag(this, pTarget, bIgnore);

	// 如果还需要判断单体自身的，则判断自身
	if( !bIgnore )
	{
		DWORD dwSelfFlag = 0;
		if( pTarget->IsRole() )
			dwSelfFlag = FriendEnemy(static_cast<Role*>(pTarget));
		else if( pTarget->IsCreature() )
			dwSelfFlag = FriendEnemy(static_cast<Creature*>(pTarget));
		else
			dwSelfFlag = 0;

		return (dwMapFlag | dwSelfFlag);
	}
	else
	{
		return dwMapFlag;
	}
}


DWORD Role::FriendEnemy(Role* pTarget)
{
	DWORD dwFlag = 0;

	// 先通过源方与目标方之间的关系确定敌我关系
	if( IsFriendlySocial(pTarget) )
	{
		if( !pTarget->IsInRoleState(ERS_PK) && !this->IsInRoleState(ERS_PK) )
		{
			// 双方符合某些社会关系，而且双方都不处在行凶状态，是友方
			dwFlag |= ETFE_Friendly;
		}
		else if( pTarget->IsInRoleState(ERS_PK) && this->IsInRoleState(ERS_PK) )
		{
			// 双方符合某些社会关系，而且双方都处在行凶状态，是友方
			dwFlag |= ETFE_Friendly;
		}
		else
		{
			// 双方符合某些社会关系，而且只有有一方处在行凶状态，是友方
			dwFlag |= ETFE_Friendly;
		}
	}
	else
	{
		// 如果自己设置成为行凶状态，则为敌方
		if( IsInRoleState(ERS_PK) )
		{
			dwFlag |= ETFE_Hostile;
		}

		// 如果自己为行凶状态（只对行凶玩家），并且对方也为行凶状态，则为敌方
		else if (IsInRoleState(ERS_PKEX) && (pTarget->IsInRoleState(ERS_PK) || pTarget->IsInRoleState(ERS_PKEX)) )
		{
			dwFlag |= ETFE_Hostile;
		}

		else
		{
			// 否则为中立
			dwFlag |= ETFE_Independent;
		}

		// 如果自己处在非行凶状态，则不为敌方
		if (!IsInRoleState(ERS_PK) && !IsInRoleState(ERS_PKEX))
		{
			if ( dwFlag & ETFE_Hostile )
			{
				dwFlag ^= ETFE_Hostile;
			}
		}
		
	}

	// 再考虑一下PK保护相关的内容
	if( pTarget->IsInRoleState(ERS_Safeguard) )
	{
		// 对开了PK保护的玩家不能进行PK
		if( dwFlag & ETFE_Hostile )
		{
			dwFlag ^= ETFE_Hostile;
		}
	}
	if( IsInRoleState(ERS_Safeguard) )
	{
		// 自己开了PK保护，不能对他人进行PK
		if( dwFlag & ETFE_Hostile )
		{
			dwFlag ^= ETFE_Hostile;
		}

		// 不能对当前正在PVP状态下的友方玩家进行有益招式
		if( (dwFlag & ETFE_Friendly) && pTarget->IsInRoleState(ERS_PVP) )
		{
			dwFlag ^= ETFE_Friendly;
		}
	}

	// 安全区
	//if( IsInRoleState(ERS_SafeArea) || pTarget->IsInRoleState(ERS_SafeArea) )
	//{
	//	if( dwFlag & ETFE_Hostile )
	//	{
	//		dwFlag ^= ETFE_Hostile;
	//	}
	//}

	// PVP区域
	if( IsInRoleState(ERS_PVPArea) && pTarget->IsInRoleState(ERS_PVPArea) )
	{
		dwFlag |= ETFE_Hostile;
	}

	return dwFlag;
}

DWORD Role::FriendEnemy(Creature* pCreature)
{
	DWORD dwFlag = 0;

	// 怪物
	if( pCreature->IsMonster() )
	{
		dwFlag |= ETFE_Hostile;
	}

	// NPC
	else if( pCreature->IsNPC() )
	{
		if( IsInRoleState(ERS_PK) || IsInRoleState(ERS_PKEX) )
		{
			dwFlag |= ETFE_Hostile;
		}
		else
		{
			dwFlag |= ETFE_Friendly;
		}
	}

	// 地物
	else if( pCreature->IsGameObject() )
	{
		if( pCreature->IsRes() 
			|| pCreature->IsInves())
		{
			dwFlag |= ETFE_Independent;
		}
		else if (pCreature->IsDoor())
		{
			dwFlag |= ETFE_Hostile;
		}
		else
		{
			// todo:剩下的一些地物目前先不考虑，属于无法攻击型的
		}
	}

	// 宠物
	else if( pCreature->IsPet() )
	{

	}

	return dwFlag;
}

//------------------------------------------------------------------------------
// 注册触发器技能关联表
//------------------------------------------------------------------------------
VOID Role::RegisterTriggerSkillSet(ETriggerEventType eEvent, DWORD dwSkillID)
{
	EPassiveSkillAndEquipTrigger eTriggerType = TriggerTypeToPassiveSkillAndEquipTriggerType(eEvent);
	if( EPSAET_Null == eTriggerType ) return;

	std::set<DWORD>& skillSet = m_setPassiveSkillTrigger[eTriggerType];

	skillSet.insert(dwSkillID);
}

//----------------------------------------------------------------------------
// 反注册触发器技能关联表
//----------------------------------------------------------------------------
VOID Role::UnRegisterTriggerSkillSet(ETriggerEventType eEvent, DWORD dwSkillID)
{
	EPassiveSkillAndEquipTrigger eTriggerType = TriggerTypeToPassiveSkillAndEquipTriggerType(eEvent);
	if( EPSAET_Null == eTriggerType ) return;

	std::set<DWORD>& skillSet = m_setPassiveSkillTrigger[eTriggerType];

	skillSet.erase(dwSkillID);
}

//----------------------------------------------------------------------------------
// 装备相关buff预处理，并获取tigger类型
//----------------------------------------------------------------------------------
EPassiveSkillAndEquipTrigger Role::PreRegisterTriggerEquipSet(DWORD dwTriggerID, DWORD dwBuffID, BOOL bEquip)
{
	const tagTriggerProto* pProto = g_attRes.GetTriggerProto(dwTriggerID);
	if( !P_VALID(pProto) )
	{
		MAttResCaution(_T("trigger proto"), _T("TriggerID"), dwTriggerID);
		return EPSAET_Null;
	}

	// 是否是装备类trigger
	if( ETEE_Equip == pProto->eEventType )
	{
		ProcEquipBuffTrigger(dwBuffID, bEquip);
		return EPSAET_Null;
	}

	return TriggerTypeToPassiveSkillAndEquipTriggerType(pProto->eEventType);
}

//----------------------------------------------------------------------------
// 被动技能触发器触发
//----------------------------------------------------------------------------
BOOL Role::OnPassiveSkillBuffTrigger(Unit* pTarget, ETriggerEventType eEvent, 
		DWORD dwEventMisc1/* =GT_INVALID */, DWORD dwEventMisc2/* =GT_INVALID */)
{
	// 首先根据事件类型判断出被动技能对应的触发器类型
	EPassiveSkillAndEquipTrigger eTriggerType = TriggerTypeToPassiveSkillAndEquipTriggerType(eEvent);
	if( EPSAET_Null == eTriggerType ) return FALSE;

	// 找到是否有被动技能对该trigger类型进行了注册
	if( m_setPassiveSkillTrigger[eTriggerType].empty() ) return FALSE;

	// 一个技能一个技能的轮询
	std::set<DWORD>& skillSet = m_setPassiveSkillTrigger[eTriggerType];
	for(std::set<DWORD>::iterator it = skillSet.begin(); it != skillSet.end(); it++)
	{
		// 找到技能
		DWORD dwSkillID = *it;
		Skill* pSkill = GetSkill(dwSkillID);
		if( !P_VALID(pSkill) ) continue;
		if( pSkill->GetCoolDownCountDown() > 0 )	continue;

		// todo: 这个地方还要判断被动技能的限制，combathandler里面再加一个判断被动技能能否使用的判断

		BOOL bSelfUseble = FALSE;			// 自身能否作用
		BOOL bTargetUseble = FALSE;			// 目标能否作用

		if( E_Success == m_CombatHandler.CheckTargetLogicLimit(pSkill, pTarget) )
		{
			bTargetUseble = TRUE;
		}
		if( E_Success == m_CombatHandler.CheckSkillLimit(pSkill) )
		{
			bSelfUseble = TRUE;
		}

		// 自身和目标都不能作用，则直接返回
		if( FALSE == bSelfUseble && bTargetUseble == FALSE )
			continue;

		// 检查技能是用skillbuff还是用属性表中的buffid
		for(INT n = 0; n < MAX_BUFF_PER_SKILL; n++)
		{
			DWORD dwBuffTypeID = pSkill->GetBuffTypeID(n);
			DWORD dwTriggerID = pSkill->GetTriggerID(n);
			if( !P_VALID(dwBuffTypeID) || !P_VALID(dwTriggerID) ) continue;

			const tagBuffProto* pBuffProto = g_attRes.GetBuffProto(dwBuffTypeID);
			const tagTriggerProto* pTriggerProto = g_attRes.GetTriggerProto(dwTriggerID);
			if( !P_VALID(pBuffProto) || !P_VALID(pTriggerProto) ) continue;

			// 查看trigger类型是否相同，主要是为了优化
			if( ETEE_Null != pTriggerProto->eEventType && eEvent != pTriggerProto->eEventType )
				continue;

			// 如果目标可以作用，则作用于目标
			if( bTargetUseble )
				pTarget->TryAddBuff(this, pBuffProto, pTriggerProto, pSkill, NULL, TRUE, dwEventMisc1, dwEventMisc2);

			// 如果自身可以作用，则作用于自身
			if( bSelfUseble )
				TryAddBuff(this, pBuffProto, pTriggerProto, pSkill, NULL, TRUE, dwEventMisc1, dwEventMisc2);

			StartSkillCoolDown(pSkill);
		}
	}

	return TRUE;
}

//------------------------------------------------------------------------------
// 使用物品状态触发
//------------------------------------------------------------------------------
BOOL Role::OnActiveItemBuffTrigger(tagItem* pItem, ETriggerEventType eEvent, 
					DWORD dwEventMisc1/*=GT_INVALID*/, DWORD dwEventMisc2/*=GT_INVALID*/)
{
	// 物品是否存在
	if( !P_VALID(pItem)) return FALSE;

	for(INT n = 0; n < MAX_BUFF_PER_ITEM; ++n)
	{
		DWORD dwBuffTypeID = pItem->GetBuffID(n);
		DWORD dwTriggerID = pItem->GetTriggerID(n);
		if( !P_VALID(dwBuffTypeID) || !P_VALID(dwTriggerID) ) continue;

		const tagBuffProto* pBuffProto = g_attRes.GetBuffProto(dwBuffTypeID);
		const tagTriggerProto* pTriggerProto = g_attRes.GetTriggerProto(dwTriggerID);
		if( !P_VALID(pBuffProto) || !P_VALID(pTriggerProto) ) continue;

		// 查看trigger类型是否相同，主要是为了优化
		if( ETEE_Null != pTriggerProto->eEventType && eEvent != pTriggerProto->eEventType )
			continue;

		// 给自己是否满足
		TryAddBuff(this, pBuffProto, pTriggerProto, NULL, pItem);
	}

	return TRUE;
}

//----------------------------------------------------------------------------------
// 装备相关被动trigger触发处理
//----------------------------------------------------------------------------------
VOID Role::OnEquipmentBuffTriggerCommon(ETriggerEventType eEvent, EPassiveSkillAndEquipTrigger eTriggerType)
{
	// 装备上普通buff处理 -- 轮循装备栏
	BitSetEquipPos& equipSet = m_bitsetEquipTrigger[eTriggerType];
	for(INT16 i=0; i<equipSet.size(); ++i)
	{
		if(!equipSet.test(i))
		{
			continue;
		}

		// 找到装备
		tagEquip *pEquip = GetItemMgr().GetEquipBarEquip(i);
		if(!P_VALID(pEquip))
		{
			// 执行到该处，说明装备buff，trigger处理有问题
			ASSERT(0);
			IMSG(_T("\n\n\nCaution:\n"));
			IMSG(_T("\tTrigger&Buff of equip process maybe have problem! Please tell programmer to check!"));
			continue;
		}

		// 找到装备的buff，尝试添加
		for(INT32 i=0; i<MAX_EQUIP_BUFF_NUM; ++i)
		{
			DWORD	dwBuffTypeID	=	GT_INVALID;
			DWORD	dwTriggerID		=	GT_INVALID;
			pEquip->GetTriggerIDBuffID(dwTriggerID, dwBuffTypeID, i);
			if(GT_INVALID == dwTriggerID || GT_INVALID == dwBuffTypeID)
			{
				continue;
			}

			const tagTriggerProto* pTriggerProto = g_attRes.GetTriggerProto(dwTriggerID);
			if(!P_VALID(pTriggerProto))
			{
				MAttResCaution(_T("equip trigger or buff"), _T("equip typeid"), pEquip->dwTypeID);
				continue;
			}

			// 查看trigger类型是否相同，主要是为了优化
			if(ETEE_Null == pTriggerProto->eEventType || eEvent != pTriggerProto->eEventType)
			{
				continue;
			}

			const tagBuffProto*	pBuffProto = g_attRes.GetBuffProto(dwBuffTypeID);
			if(!P_VALID(pBuffProto))
			{
				MAttResCaution(_T("equip buff"), _T("equip typeid"), pEquip->dwTypeID);
				continue;
			}

			// 给自己是否满足
			TryAddBuff(this, pBuffProto, pTriggerProto, NULL, pEquip);
		}
	}
}

//----------------------------------------------------------------------------------
// 装备相关被动trigger触发处理
//----------------------------------------------------------------------------------
VOID Role::OnEquipmentBuffTriggerLongHun(ETriggerEventType eEvent, EPassiveSkillAndEquipTrigger eTriggerType)
{
	BitSetEquipPos& bitsetLongHun = m_bitsetLongHunTrigger[eTriggerType];
	for(INT16 i=0; i<bitsetLongHun.size(); ++i)
	{
		if(!bitsetLongHun.test(i))
		{
			continue;
		}

		// 找到龙魂能力ID
		DWORD dwLongHunID = m_LongHun.GetLongHunID(i);
		if(GT_INVALID == dwLongHunID)
		{
			ASSERT(0);
			IMSG(_T("\n\n\tLongHun reg/unreg maybe have problems! Please check!!!\n\n"));
		}

#ifdef	_DEBUG

		// 找到装备
		tagEquip *pEquip = GetItemMgr().GetEquipBarEquip(i);
		if(!P_VALID(pEquip))
		{
			// 执行到该处，说明装备buff，trigger处理有问题
			ASSERT(0);
			IMSG(_T("\n\n\nCaution:\n"));
			IMSG(_T("\tTrigger&Buff of Longhun process maybe have problem! Please tell programmer to check!"));
		}
		else
		{
			DWORD	dwLongHunIDCheck	= GT_INVALID;
			BOOL	bOuter				= TRUE;

			pEquip->GetLongHunID(dwLongHunIDCheck, bOuter);
			if(dwLongHunIDCheck != dwLongHunID)
			{
				// 龙魂能力管理器中记录与实际装备情况不符
				ASSERT(0);
			}
		}

#endif

		// 获取龙魂能力原型
		const tagLongHunProto *pLongHunProto = g_attRes.GetLongHunProto(dwLongHunID);
		if(!P_VALID(pLongHunProto))
		{
			ASSERT(0);
			MAttResCaution(_T("LongHun"), _T("dwLongHunID"), dwLongHunID);
			continue;
		}

		if(GT_INVALID == pLongHunProto->dwTriggerID
			|| (GT_INVALID == pLongHunProto->dwBuffID1 && GT_INVALID == pLongHunProto->dwBuffID2))
		{
			ASSERT(0);
			MAttResCaution(_T("LongHun"), _T("dwLongHunID"), dwLongHunID);
			continue;
		}

		// trigger proto
		const tagTriggerProto* pTriggerProto	= g_attRes.GetTriggerProto(pLongHunProto->dwTriggerID);
		if(!P_VALID(pTriggerProto))
		{
			MAttResCaution(_T("longhun trigger"), _T("LongHunID"), dwLongHunID);
			continue;
		}

		// 查看trigger类型是否相同，检查用
		if(ETEE_Null == pTriggerProto->eEventType || eEvent != pTriggerProto->eEventType)
		{
			ASSERT(0);
			IMSG(_T("\n\n\tLongHun reg/unreg maybe have problems! Please check!!!\n\n"));
			continue;
		}

		// buff proto
		if(pLongHunProto->dwBuffID1 != GT_INVALID)
		{
			const tagBuffProto*	pBuffProto	= g_attRes.GetBuffProto(pLongHunProto->dwBuffID1);
			if(!P_VALID(pBuffProto))
			{
				ASSERT(0);
				MAttResCaution(_T("LongHun Buff"), _T("LongHunID"), dwLongHunID);
			}
			else
			{
				// 给自己是否满足
				TryAddBuff(this, pBuffProto, pTriggerProto, NULL, NULL);
			}
		}

		if(pLongHunProto->dwBuffID2 != GT_INVALID)
		{
			const tagBuffProto*	pBuffProto	= g_attRes.GetBuffProto(pLongHunProto->dwBuffID2);
			if(!P_VALID(pBuffProto))
			{
				ASSERT(0);
				MAttResCaution(_T("LongHun Buff"), _T("LongHunID"), dwLongHunID);
			}
			else
			{
				// 给自己是否满足
				TryAddBuff(this, pBuffProto, pTriggerProto, NULL, NULL);
			}
		}
	}
}

//----------------------------------------------------------------------------------
// 装备相关被动trigger触发处理
//----------------------------------------------------------------------------------
VOID Role::OnEquipmentBuffTriggerSuit(ETriggerEventType eEvent, EPassiveSkillAndEquipTrigger eTriggerType)
{
	SetSuitTrigger& suitSet = m_setSuitTrigger[eTriggerType];

	for(SetSuitTrigger::iterator it = suitSet.begin(); it != suitSet.end(); ++it)
	{
		// 获取套装件数，遍历已激活buff
		INT32 nSuitEquipNum = m_Suit.GetSuitEquipNum(*it);
		ASSERT(nSuitEquipNum >= MIN_SUIT_EQUIP_NUM);

		// 套装原型
		const tagSuitProto *pSuitProto = g_attRes.GetSuitProto(*it);
		if(!P_VALID(pSuitProto))
		{
			ASSERT(0);
			MAttResCaution(_T("suit"), _T("SuitID"), *it);
			continue;
		}

		if(nSuitEquipNum < pSuitProto->n8ActiveNum[0])
		{
			ASSERT(0);
			IMSG(_T("\n\n\tSuit trigger reg/unreg maybe have problems! Please check!!!\n\n"));
			continue;
		}

		for(INT32 i=0; i<MAX_SUIT_ATT_NUM; ++i)
		{
			if(nSuitEquipNum < pSuitProto->n8ActiveNum[i])
			{
				break;
			}

			if(GT_INVALID == pSuitProto->dwTriggerID[i] || GT_INVALID == pSuitProto->dwBuffID[i])
			{
				break;
			}

			const tagTriggerProto* pTriggerProto = g_attRes.GetTriggerProto(pSuitProto->dwTriggerID[i]);
			if(!P_VALID(pTriggerProto))
			{
				MAttResCaution(_T("suit trigger"), _T("suitid"), *it);
				continue;
			}

			// 查看trigger类型是否相同，主要是为了优化
			if(ETEE_Null == pTriggerProto->eEventType || eEvent != pTriggerProto->eEventType)
			{
				continue;
			}

			const tagBuffProto*	pBuffProto = g_attRes.GetBuffProto(pSuitProto->dwBuffID[i]);
			if(!P_VALID(pBuffProto))
			{
				MAttResCaution(_T("suit buff"), _T("suitid"), *it);
				continue;
			}

			// 给自己是否满足
			TryAddBuff(this, pBuffProto, pTriggerProto, NULL, NULL);
		}
	}
}

//----------------------------------------------------------------------------
// 装备状态触发
//----------------------------------------------------------------------------
BOOL Role::OnEquipmentBuffTrigger(Unit* pTarget, ETriggerEventType eEvent, 
								  DWORD dwEventMisc1/* =GT_INVALID */, DWORD dwEventMisc2/* =GT_INVALID */)
{
	// 首先根据事件类型判断出被动技能对应的触发器类型
	EPassiveSkillAndEquipTrigger eTriggerType = TriggerTypeToPassiveSkillAndEquipTriggerType(eEvent);
	if( EPSAET_Null == eTriggerType ) return FALSE;

	// 装备上普通buff处理
	if( m_bitsetEquipTrigger[eTriggerType].any() )
	{
		OnEquipmentBuffTriggerCommon(eEvent, eTriggerType);
	}

	// 龙魂能力
	if(m_bitsetLongHunTrigger[eTriggerType].any())
	{
		OnEquipmentBuffTriggerLongHun(eEvent, eTriggerType);
	}

	// 套装
	if(m_setSuitTrigger[eTriggerType].size() > 0)
	{
		OnEquipmentBuffTriggerSuit(eEvent, eTriggerType);
	}

	return TRUE;
}

//-----------------------------------------------------------------------------
// 检查地图区域
//-----------------------------------------------------------------------------
VOID Role::CheckMapArea()
{
	DWORD dwRet = 0;
	if( GetMap() )
		dwRet = GetMap()->CheckArea(this);

	// 安全区
	if( dwRet & ERS_SafeArea )
	{
		// 进入安全区域
		if( !IsInRoleState(ERS_SafeArea) ) 
		{
			// 非行凶状态，且戾气值为0的玩家进入安全区置为保护模式
			if (!IsInRoleState(ERS_PK) && !IsInRoleState(ERS_PKEX) && 0 == GetAttValue(ERA_Hostility) && !IsInRoleState(ERS_Safeguard))
			{
				SetRoleState(ERS_Safeguard);
			}

			SetRoleState(ERS_SafeArea);
		}
	}
	else
	{
		// 离开安全区域
		if( IsInRoleState(ERS_SafeArea) ) 
		{
			// 30级以上并且未处于十分钟pk死亡保护期的玩家离开安全区域时置为自由模式
			if (m_nLevel >= SAFE_GUARD_FORCE_LEVEL && m_nDeadUnSetSafeGuardCountDown <= 0 && IsInRoleState(ERS_Safeguard))
			{
				UnsetRoleState(ERS_Safeguard);
			}

			UnsetRoleState(ERS_SafeArea);
		}
	}

	// PVP区
	if( dwRet & ERS_PVPArea )
	{
		if( !IsInRoleState(ERS_PVPArea) ) SetRoleState(ERS_PVPArea);
	}
	else
	{
		if( IsInRoleState(ERS_PVPArea) ) UnsetRoleState(ERS_PVPArea);
	}

	// 摆摊区
	if( dwRet & ERS_StallArea )
	{
		if( !IsInRoleState(ERS_StallArea) ) SetRoleState(ERS_StallArea);
	}
	else
	{
		if( IsInRoleState(ERS_StallArea) ) UnsetRoleState(ERS_StallArea);
	}

	// 监狱区
	if( dwRet & ERS_PrisonArea )
	{
		if( !IsInRoleState(ERS_PrisonArea) ) SetRoleState(ERS_PrisonArea);
	}
	else
	{
		if( IsInRoleState(ERS_PrisonArea) ) UnsetRoleState(ERS_PrisonArea);
	}


}

//------------------------------------------------------------------------------
// 任务设置复活点
//------------------------------------------------------------------------------
DWORD Role::QuestSetRebornMap(UINT16 u16QuestID, DWORD &dwBornMapID, Vector3 &vBornPos)
{
	// 获得地图
	Map *pMap = GetMap();
	if(!P_VALID(pMap))
	{
		ASSERT(P_VALID(pMap));
		return GT_INVALID;
	}

	if(pMap->GetMapInfo()->eType != EMT_Normal)
	{
		ASSERT(pMap->GetMapInfo()->eType == EMT_Normal);
		return E_BindRebornMap_MapInvalid;
	}

	if(GetRebornMapID() == pMap->GetMapID())
	{
		return E_BindRebornMap_Already;
	}

	// 判断是否有任务
	if (IsHaveQuest(u16QuestID) || IsHaveDoneQuest(u16QuestID))
	{
		// 设置
		m_dwRebornMapID = pMap->GetMapID();
		dwBornMapID = m_dwRebornMapID;
		vBornPos = g_mapCreator.GetReBornPoint(m_dwRebornMapID);
		return E_Success;
	}

	return E_BindRebornMap_MapInvalid;
}

//------------------------------------------------------------------------------
// 复活
//------------------------------------------------------------------------------
DWORD Role::Revive(ERoleReviveType eType, INT64 n64ItemID, BOOL bNeedItem)
{
	if( !IsInState(ES_Dead) )
		return E_Revive_NotDead;	// 已经不是死亡状态了

	if (m_bNeedPrisonRevive)		// 是否需要牢狱复活
	{
		eType = ERRT_Prison;
		m_bNeedPrisonRevive = FALSE;
	}

	DWORD dwErrorCode = E_Success;
	
	// 判断复活类型
	switch(eType)
	{
	case ERRT_ReturnCity:
		dwErrorCode = CityRevive();
		break;
	case ERRT_Locus:
		dwErrorCode = LocusRevive(n64ItemID, bNeedItem);
		break;
	case ERRT_Accept:
		dwErrorCode = AcceptRevive();
		break;
	case ERRT_Prison:
		dwErrorCode = PrisonRevive();
		break;
	default:
		// 不会执行到这里(上层已做类型判断)
		ASSERT(0);
		dwErrorCode = E_Revive_Unknown;
	}

	GetMap()->OnRevive(this, eType, m_Revive.nHP, m_Revive.nMP, m_Revive.fX, m_Revive.fY, m_Revive.fZ, m_Revive.dwMapID);

	// 复活
	if(E_Success == dwErrorCode)
	{
		SetAttValue(ERA_HP, m_Revive.nHP);
		SetAttValue(ERA_MP, m_Revive.nMP);
		UnsetState(ES_Dead);
	}

	// 清空复活相关记录信息
	m_Revive.nHP = 0;
	m_Revive.nMP = 0;

	return dwErrorCode;
}

DWORD Role::CityRevive()
{
	const tagMapInfo *pMapInfo = g_mapCreator.GetMapInfo(m_dwRebornMapID);
	if(!P_VALID(pMapInfo))
	{
		ASSERT(P_VALID(pMapInfo));
		return E_Revive_MapNotFound;
	}
	
	// 设置复活点及血量和真气量
	m_Revive.nHP = GetAttValue(ERA_MaxHP);
	m_Revive.nMP = GetAttValue(ERA_MaxMP);
	m_Revive.fX = pMapInfo->vRebornPos.x;
	m_Revive.fY = pMapInfo->vRebornPos.y;
	m_Revive.fZ = pMapInfo->vRebornPos.z;
	m_Revive.dwMapID = m_dwRebornMapID;

	SetHostilitySafeGuard();

	return E_Success;
}

DWORD Role::LocusRevive(INT64 n64ItemID, BOOL bNeedItem)
{
	// 获得物品
	if (bNeedItem)
	{
		tagItem *pItem = GetItemMgr().GetBagItem(n64ItemID);
		if(!P_VALID(pItem) || pItem->pProtoType->eSpecFunc != EISF_RevieveItem)
		{
			return E_Revive_ItemLimit;
		}
	
		// 删除物品
		DWORD dwErrorCode = GetItemMgr().ItemUsedFromBag(n64ItemID, (INT16)1, (DWORD)ELCLD_Revive_Locus);
		if(dwErrorCode != E_Success)
		{
			return dwErrorCode;
		}

		// 设置复活点及血量和真气量
		m_Revive.nHP = GetAttValue(ERA_MaxHP) / 4;
		m_Revive.nMP = GetAttValue(ERA_MaxMP) / 4;
	}
	else
	{
		// 设置复活点及血量和真气量
		m_Revive.nHP = GetAttValue(ERA_MaxHP);
		m_Revive.nMP = GetAttValue(ERA_MaxMP);
	}

	m_Revive.fX = m_MoveData.m_vPos.x;
	m_Revive.fY = m_MoveData.m_vPos.y;
	m_Revive.fZ = m_MoveData.m_vPos.z;
	m_Revive.dwMapID = GetMapID();

	return E_Success;
}

DWORD Role::AcceptRevive()
{
	// 检查HP
	if(m_Revive.nHP <= 0)
	{
		return GT_INVALID;
	}
	
	return E_Success;
}

DWORD Role::PrisonRevive()
{
	DWORD	dwPrisonMapID	= g_mapCreator.GetPrisonMapID();
	Vector3	PutInPoint		= g_mapCreator.GetPutInPrisonPoint();

	const tagMapInfo *pMapInfo = g_mapCreator.GetMapInfo(dwPrisonMapID);
	if(!P_VALID(pMapInfo))
	{
		ASSERT(P_VALID(pMapInfo));
		return E_Revive_MapNotFound;
	}

	// 设置复活点及血量和真气量
	m_Revive.nHP = GetAttValue(ERA_MaxHP);
	m_Revive.nMP = GetAttValue(ERA_MaxMP);
	m_Revive.fX = PutInPoint.x;
	m_Revive.fY = PutInPoint.y;
	m_Revive.fZ = PutInPoint.z;
	m_Revive.dwMapID = dwPrisonMapID;

	return E_Success;
}

//-----------------------------------------------------------------------------------------
// 被复活
//-----------------------------------------------------------------------------------------
VOID Role::BeRevived(INT nHP, INT nMP, Unit* pSrc)
{
	if( nHP <= 0 ) return;

	m_Revive.nHP = nHP;
	m_Revive.nMP = nMP;

	if( P_VALID(pSrc) && pSrc->GetMapID() == GetMapID() )
	{
		m_Revive.dwMapID = GetMapID();
		m_Revive.fX = pSrc->GetCurPos().x;
		m_Revive.fY = pSrc->GetCurPos().y;
		m_Revive.fZ = pSrc->GetCurPos().z;
	}
	else
	{
		m_Revive.dwMapID = GetMapID();
		m_Revive.fX = GetCurPos().x;
		m_Revive.fY = GetCurPos().y;
		m_Revive.fZ = GetCurPos().z;
	}


	// 发送给客户端
	tagNS_RoleReviveNotify send;
	SendMessage(&send, send.dwSize);
}

DWORD Role::SendCloseStall()
{
	if(!IsInRoleStateAny(ERS_Stall | ERS_StallSet))
	{
		return E_Success;
	}
	
	tagNS_StallClose send;
	send.dwErrorCode	= CloseStall();
	send.dwStallRoleID	= GetID();
	if(P_VALID(GetMap()))
	{
		GetMap()->SendBigVisTileMsg(this, &send, send.dwSize);
	}

	return send.dwErrorCode;
}

DWORD Role::UpdateFriOnline()
{
	INT				nFriendNum = GetFriendCount();
	DWORD			dwFriendID = GT_INVALID;
	tagRoleInfo*	pRoleInfo = (tagRoleInfo*)GT_INVALID;
	tagFriend*		pFriend = (tagFriend*)GT_INVALID;
	INT				nIndex = 0;
	DWORD			dwSize = sizeof(tagNS_UpdateFriState);
		
	if( nFriendNum > 0 )
	{
		dwSize += (nFriendNum-1) * sizeof(tagFriUpdate);
	}
	
	MCREATE_MSG(pSend, dwSize, NS_UpdateFriState);
		
	m_mapFriend.ResetIterator();
	while(m_mapFriend.PeekNext(dwFriendID, pFriend))
	{
		pRoleInfo = g_roleMgr.GetRoleInfo(dwFriendID);
		
		if(P_VALID(pRoleInfo))
		{
			pSend->FriUpdate[nIndex].dwRoleID = dwFriendID;
			pSend->FriUpdate[nIndex].bOnline = pRoleInfo->bOnline;
			pSend->FriUpdate[nIndex].nLevel = pRoleInfo->byLevel;
			++nIndex;
		}
		else
		{	
			tagNS_RoleCancelFriend send;
			send.dwDestRoleID = dwFriendID;
			send.byGroup = pFriend->byGroup;
			send.dwErrorCode = E_Success;
			SendMessage(&send, send.dwSize);

			pFriend->dwFriendID = GT_INVALID;
			pFriend->dwFriVal = 0;
			pFriend->byGroup = 1;
			m_mapFriend.Erase(dwFriendID);
		}
	}

	pSend->nNum = nIndex;
	if( nIndex > 0) 
	{
		pSend->dwSize = (nIndex - 1) * sizeof(tagFriUpdate) + sizeof(tagNS_UpdateFriState);
	}
	else
		pSend->dwSize = sizeof(tagNS_UpdateFriState);

	SendMessage(pSend, pSend->dwSize);

	MDEL_MSG(pSend);

	return 0;
}

//--------------------------------------------------------------------------------
// 属性改变引起的其它内容改变
//--------------------------------------------------------------------------------
VOID Role::OnAttChange(INT nIndex)
{
	switch(nIndex)
	{
		// 血量
	case ERA_HP:
		m_nAtt[ERA_HP] = min(m_nAtt[ERA_HP], m_nAtt[ERA_MaxHP]);
		break;

		// 真气
	case ERA_MP:
		m_nAtt[ERA_MP] = min(m_nAtt[ERA_MP], m_nAtt[ERA_MaxMP]);
		break;

		// 活力
	case ERA_Vitality:
		m_nAtt[ERA_Vitality] = min(m_nAtt[ERA_Vitality], m_nAtt[ERA_MaxVitality]);
		break;

	case ERA_Endurance:
		m_nAtt[ERA_Endurance] = min(m_nAtt[ERA_Endurance], m_nAtt[ERA_MaxEndurance]);
		break;

		// 道德
	case ERA_Morality:
		{
			CalPKState();
		}
		break;

	default:
		break;
	}

	// 称号条件触发
	GetTitleMgr()->SigEvent(ETE_ROLE_ATT, nIndex, m_nAtt[nIndex]);
}

VOID Role::OnAttChange(bool bRecalFlag[ERA_End])
{
	// XZ方向速度
	if( bRecalFlag[ERA_Speed_XZ] )
	{
		m_fXZSpeed = X_DEF_XZ_SPEED * (FLOAT(m_nAtt[ERA_Speed_XZ]) / 10000.0f);
		if( E_Success == m_MoveData.StopMove() )
		{
			// 发送给自己速度改变
			tagNS_MoveSpeedChange send;
			send.dwRoleID = m_dwID;
			send.curPos = m_MoveData.m_vPos;
			send.faceTo = m_MoveData.m_vFace;
			SendMessage(&send, send.dwSize);
		}
	}

	// Y方向速度
	if( bRecalFlag[ERA_Speed_Y] )
	{
		m_fYSpeed = X_DEF_Y_SPEED * (FLOAT(m_nAtt[ERA_Speed_Y]) / 10000.0f);
	}

	// 游泳速度
	if( bRecalFlag[ERA_Speed_Swim] )
	{
		m_fSwimXZSpeed = X_DEF_XZ_SPEED * (FLOAT(m_nAtt[ERA_Speed_Swim]) / 10000.0f);
		if( E_Success == m_MoveData.StopMove() )
		{
			// 发送给自己速度改变
			tagNS_MoveSpeedChange send;
			send.dwRoleID = m_dwID;
			send.curPos = m_MoveData.m_vPos;
			send.faceTo = m_MoveData.m_vFace;
			SendMessage(&send, send.dwSize);
		}
	}

	// 骑乘速度
	if( bRecalFlag[ERA_Speed_Mount] )
	{
		m_fMountXZSpeed = X_DEF_XZ_SPEED * (FLOAT(m_nAtt[ERA_Speed_Mount]) / 10000.0f);
		if( E_Success == m_MoveData.StopMove() )
		{
			// 发送给自己速度改变
			tagNS_MoveSpeedChange send;
			send.dwRoleID = m_dwID;
			send.curPos = m_MoveData.m_vPos;
			send.faceTo = m_MoveData.m_vFace;
			SendMessage(&send, send.dwSize);
		}
	}

	// 体型
	if( bRecalFlag[ERA_Shape] )
	{
		FLOAT fCosf = FLOAT(m_nAtt[ERA_Shape]) / 10000.0f;
		m_Size.x = X_DEF_ROLE_SIZE_X * fCosf;
		m_Size.y = X_DEF_ROLE_SIZE_Y * fCosf;
		m_Size.z = X_DEF_ROLE_SIZE_Z * fCosf;
	}

	// 队长统御力改变
	if(GetTeamID() != GT_INVALID && bRecalFlag[ERA_Rein])
	{
		const Team* pTeam = g_groupMgr.GetTeamPtr(GetTeamID());
		if( P_VALID(pTeam) && pTeam->IsLeader(GetID()) )
		{
			tagLeaderRein	LeaderRein;
			LeaderRein.nLeaderRein = GetAttValue(ERA_Rein);
			g_groupMgr.AddEvent(GetID(), EVT_ChangeRein, sizeof(tagLeaderRein), &LeaderRein);
		}
	}
}

VOID Role::OnLeaveMap( BOOL bChgMap /*= TRUE*/ )
{
	// 切换地图消失的buff
	if (bChgMap)
	{
		// buff 打断
		OnInterruptBuffEvent(EBIF_ChangeMap);

		// 起手打断
		GetCombatHandler().InterruptPrepare(CombatHandler::EIT_Move, FALSE);

		// 释放打断
		GetCombatHandler().InterruptOperate(CombatHandler::EIT_Move, EMS_Walk);
	}

	// 清空仇恨列表
	ClearEnmityCreature();

	// 收起摆摊
	SendCloseStall();

	// 打断骑乘
	BreakMount();
	// 收起召唤宠物
	GetPetPocket()->CalledPetEnterPocket();

}

VOID Role::OnEnterMap()
{
	m_pScript->OnRoleEnterMap(this);

	// 释放召唤宠物
	GetPetPocket()->CalledPetLeavePocket();
	ContinueMount();

}

//--------------------------------------------------------------------------------
// 设置帮派属性
//--------------------------------------------------------------------------------
VOID Role::SetGuildID(DWORD dwGuildID)
{
	m_dwGuildID = dwGuildID;

	tagNDBC_ChangeRoleGuild send;
	send.dwRoleID	= GetID();
	send.dwGuildID	= dwGuildID;
	g_dbSession.Send(&send, send.dwSize);
}

//--------------------------------------------------------------------------------
// 世界喊话
//--------------------------------------------------------------------------------
BOOL Role::TalkToWorld()
{
	if (m_nWorldTalkCounter < 0)
	{
		m_nWorldTalkCounter = WORLD_CHANNEL_INTERVAL;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

//--------------------------------------------------------------------------------
// 更新世界喊话计时
//--------------------------------------------------------------------------------
VOID Role::UpdateTalkToWorld()
{
	if (m_nWorldTalkCounter >= 0)
	{
		--m_nWorldTalkCounter;
	}
}

//--------------------------------------------------------------------------------
// 拾取物品
//--------------------------------------------------------------------------------
DWORD Role::PickUpItem( INT64 n64GroundID, BOOL bRole )
{
	if (bRole)
		StopMount();

	MTRANS_ELSE_RET(pMap, GetMap(), Map, E_Loot_Map_Not_Found);
	MTRANS_ELSE_RET(pGroundItem, pMap->GetGroundItem(n64GroundID), tagGroundItem, E_Loot_Item_Not_Found);
	
	DWORD dwRtv = E_Success;

	//注意把掉落物品拾取到背包里,这里tagGroundItem中的pItem指针会被销毁,所以放进背包应该是最后
	if (pGroundItem->dwTypeID == TYPE_ID_MONEY && 
		FALSE == GetCurMgr().IncBagSilver(pGroundItem->nNum, ELCLD_PickUp_Money)
		)
		return E_Loot_Add_Bag_Not_Success;
	//注意把掉落物品拾取到背包里,这里tagGroundItem中的pItem指针会被销毁,所以放进背包应该是最后
	else if (pGroundItem->dwTypeID != TYPE_ID_MONEY && 
		E_Success != (dwRtv = GetItemMgr().Add2Bag(pGroundItem->pItem, ELCLD_PickUp_Item, TRUE))
		)
		return dwRtv;
	else
	{
		// Jason 2009-12-4队伍分配方式，漏掉的。
		Team::BroadcastLootItemMsgInTeamExceptPicker(this,pGroundItem->pItem);
		//发送物品消失消息
		tagNS_RoleGroundItemDisappear disappear;
		disappear.n64Serial[0] = n64GroundID;
		pMap->SendBigVisTileMsg(this, &disappear, disappear.dwSize);

		//从Map中删除掉落物品
		//将掉落物品从地面删除
		pMap->RemoveGroundItem(pGroundItem);

		//将指针恢复为NULL
		pGroundItem->pItem=NULL;
		SAFE_DEL(pGroundItem);
	}

	return E_Success;
}

//--------------------------------------------------------------------------------
// 放下物品
//--------------------------------------------------------------------------------
DWORD Role::PutDownItem( INT64 n64Serial )
{

	MTRANS_ELSE_RET(pMap, GetMap(), Map, GT_INVALID);

	DWORD dwRet = E_Success;
	tagItem *pItem = NULL;

	;
	if( E_Success != (dwRet = GetItemMgr().DiscardFromBag(n64Serial, ELCLD_Role_Discard_Item, pItem)))
	{
		return dwRet;
	}

	// todo：这个地方修正一下高度，像个更好一点的办法
	if(P_VALID(pItem))
	{
		tagGroundItem* pGroundItem = new tagGroundItem(pMap->GenGroundItemID(), 
													pItem->dwTypeID, pItem->n16Num, pItem, 
													GetCurPos(), GT_INVALID, GT_INVALID, 0, GetID());

		ASSERT(pGroundItem->IsValid());

		pMap->AddGroundItem(pGroundItem);
	}
	return E_Success;
}

VOID Role::SetMount( BOOL bSet, INT nMountSpeed, const tagPetProto* pPetProto )
{
	if (bSet)
	{
		// 设置人物状态
		SetRoleState(ERS_Mount);

		// 设置碰撞盒
		SetSize(pPetProto->vSize);

		// 设置人物速度
		ModAttModValue(ERA_Speed_Mount, nMountSpeed);
	}
	else
	{
		// 设置人物状态
		UnsetRoleState(ERS_Mount);

		// 设置碰撞盒
		SetAttRecalFlag(ERA_Shape);

		// 设置人物速度
		ModAttModValue(ERA_Speed_Mount, -nMountSpeed);
	}

	RecalAtt();	
}

FLOAT Role::GetVNBExpRate()
{
	return GetSession()->GetVNBExpRate() / 10000.0f;
}

FLOAT Role::GetVNBLootRate()
{
	return GetSession()->GetVNBLootRate() / 10000.0f;
}

//--------------------------------------------------------------------------------
// 增加角色开启宝箱数
//--------------------------------------------------------------------------------
VOID Role::IncTreasureSum()
{
	++m_nTreasureSum;
	if (m_nTreasureSum > ROLE_CHSET_RATE)
		m_nTreasureSum = 1;

	tagNDBC_UpdateTreasureSum send;
	send.dwRoleID	= GetID();
	send.nSum	= m_nTreasureSum;
	g_dbSession.Send(&send, send.dwSize);
	
}

//--------------------------------------------------------------------------------
// 设置角色开启宝箱数
//--------------------------------------------------------------------------------
VOID Role::SetTreasureSum(INT nSum)
{
	if (nSum > ROLE_CHSET_RATE)
		nSum = 1;

	m_nTreasureSum = nSum;
	tagNDBC_UpdateTreasureSum send;
	send.dwRoleID	= GetID();
	send.nSum	= m_nTreasureSum;
	g_dbSession.Send(&send, send.dwSize);
}

VOID Role::StopMount()
{
	RemoveBuff(Buff::GetIDFromTypeID(MOUNT2_BUFF_ID), TRUE);
	RemoveBuff(Buff::GetIDFromTypeID(MOUNT_BUFF_ID), TRUE);
}

VOID Role::SetPassenger( Role* pHost )
{
	if (P_VALID(pHost))
	{
		SetBaseAttValue(ERA_Speed_Mount, pHost->GetAttValue(ERA_Speed_Mount));
	}
	else
	{
		SetBaseAttValue(ERA_Speed_Mount, 10000);
	}
	
	RecalAtt(TRUE);

	return ;
}


VOID Role::ContinueMount()
{
	PetSoul* pSoul = GetPetPocket()->GetPreparingPetSoul();
	if (P_VALID(pSoul) && GetPetPocket()->GetKeepMount() && pSoul->CanSetMount(TRUE))
	{
		TryAddBuff(this, g_attRes.GetBuffProto(MOUNT_BUFF_ID), NULL, NULL, NULL);
		GetPetPocket()->SetKeepMount(FALSE);
	}
	
}

void Role::BreakMount()
{
	if (IsInRoleStateAny(ERS_Mount | ERS_Mount2))
	{
		GetPetPocket()->SetKeepMount(TRUE);
		StopMount();
	}
}

//----------------------------------------------------------------------------------------
// 添加指定套装的所有装备到背包(GM命令调用)
//----------------------------------------------------------------------------------------
DWORD Role::AddSuit(DWORD dwSuitID, INT nQlty)
{
	TMap<DWORD, tagEquipProto*> mapEquip = g_attRes.GetEquipMap();
	mapEquip.ResetIterator();
	
	DWORD dwEquipID = 0;
	tagEquipProto* pEquipProto = NULL;

	while ( mapEquip.PeekNext(dwEquipID, pEquipProto) )
	{
		for(INT i=0; i<MAX_PEREQUIP_SUIT_NUM; ++i)
		{
			if(dwSuitID == pEquipProto->dwSuitID[i])
			{
				tagItem *pItem = ItemCreator::Create(EICM_GM, NULL, dwEquipID, 1);
				if(!P_VALID(pItem)) return GT_INVALID;
				ASSERT( MIsEquipment(pItem->dwTypeID) );

				ItemCreator::IdentifyEquip((tagEquip*)pItem, (EItemQuality)nQlty);
				GetItemMgr().Add2Bag(pItem, ELCID_GM_CreateItem, TRUE);

				//m_Suit.Add((tagEquip*)pItem, pItem->n16Index);
			}
		}
	}

	return TRUE;
}

//----------------------------------------------------------------------------------------
// 添加指定级别的武器到背包(GM命令调用) nType 0:武器 1:其他
//----------------------------------------------------------------------------------------
DWORD Role::AddEquip(INT nType, INT nLevel, INT nQlty)
{
	TMap<DWORD, tagEquipProto*> mapEquip = g_attRes.GetEquipMap();
	mapEquip.ResetIterator();

	DWORD dwEquipID = 0;
	tagEquipProto* pEquipProto = NULL;

	while ( mapEquip.PeekNext(dwEquipID, pEquipProto) )
	{
		if(0 == nType)
		{
			if (pEquipProto->eType != 7) continue;
		}
		else
		{
			if (!(pEquipProto->eType == 8 || pEquipProto->eType == 9 || pEquipProto->eType == 10 || pEquipProto->eType == 11)) continue;
		}

		if(pEquipProto->byMinUseLevel == nLevel || pEquipProto->byMinUseLevel+5 == nLevel)
		{
			tagItem *pItem = ItemCreator::Create(EICM_GM, NULL, dwEquipID, 1);
			if(!P_VALID(pItem)) return GT_INVALID;

			ASSERT( MIsEquipment(pItem->dwTypeID) );

			ItemCreator::IdentifyEquip((tagEquip*)pItem, (EItemQuality)nQlty);
			GetItemMgr().Add2Bag(pItem, ELCID_GM_CreateItem, TRUE);
		}
	}

	return TRUE;
}

MoveData::EMoveRet Role::MoveAction( PFMoveAction2P pAction, Vector3& v1, Vector3& v2 )
{
	if(GetPetPocket()->IsRideAfter())
	{
		return MoveData::EMR_Invalid;
	}

	MoveData::EMoveRet emr = (GetMoveData().*pAction)(v1, v2);
	if (MoveData::EMR_Success == emr)
	{
		PetSoul* pSoul = GetPetPocket()->GetMountPetSoul();
		if (P_VALID(pSoul))
		{
			Role* pPassenger = pSoul->GetPassenger();
			if (P_VALID(pPassenger))
			{
				MoveData::EMoveRet emrp = ((pPassenger->GetMoveData()).*pAction)(v1, v2);
				ASSERT(MoveData::EMR_Success == emrp);
			}
		}
	}

	return emr;
}

MoveData::EMoveRet Role::MoveAction( PFMoveAction1P pAction, Vector3& v )
{
	if(GetPetPocket()->IsRideAfter())
	{
		return MoveData::EMR_Invalid;
	}

	MoveData::EMoveRet emr = (GetMoveData().*pAction)(v);
	if (MoveData::EMR_Success == emr)
	{
		PetSoul* pSoul = GetPetPocket()->GetMountPetSoul();
		if (P_VALID(pSoul))
		{
			Role* pPassenger = pSoul->GetPassenger();
			if (P_VALID(pPassenger))
			{
				MoveData::EMoveRet emrp = ((pPassenger->GetMoveData()).*pAction)(v);
				ASSERT(MoveData::EMR_Success == emrp);
			}
		}
	}

	return emr;
}

DWORD Role::CanCastMotion( Unit* pDest, DWORD dwActionID )
{
	if (!P_VALID(pDest))
	{
		return E_Motion_CanotActive;
	}

	if(!GetMotionInviteStatus(pDest->GetID()))
		return E_Motion_CanotActive;

	const tagMotionProto* pMotion = g_attRes.GetMotionProto(dwActionID);
	if (!P_VALID(pMotion))
	{
		return E_Motion_NotValid;
	}

	// 系统判断目标是否为角色（而非怪物、NPC等任何非角色的可选中目标），若不符合，则提示：“无法进行交互”，若符合则进入下一步。
	if (!pDest->IsRole())
	{
		return E_Motion_CanotActive;
	}
	Role* pDestRole = static_cast<Role*>(pDest);
	// 系统判断目标是否为异性，若不符合，则提示：“无法进行交互”，若符合则进入下一步。
	if(pDestRole->GetSex() && GetSex() || !pDestRole->GetSex() && !GetSex())
	{
		return E_Motion_CanotActive;
	}
	// 系统判断目标是否处于非持械状态，若不符合，则提示：“无法进行交互”，若符合则进入下一步。
	if (pDestRole->IsInRoleStateAny(ERS_Combat | ERS_PK | ERS_PKEX | ERS_Arena) )
	{
		return E_Motion_CanotActive;
	}
	// 系统判断双方的友好度是否满足
	tagFriend* pFriend = GetFriendPtr(pDestRole->GetID());
	if (!P_VALID(pFriend) || pFriend->dwFriVal < pMotion->dwFriendVal)
	{
		return E_Motion_FriendValNotEnough;
	}

	// 若两者直线距离大于X个格子，则距离过远，交互动作失败，被取消。
	if (!IsInCombatDistance(*pDest, MAX_MOTION_DIST))
	{
		return E_Motion_DistanceNotEnough;
	}

	return E_Motion_Success;
	// 目标角色的屏幕内弹出确认提示框：“####（发起角色姓名）请求与你###（个性动作名称）？——接受/拒绝”
	// 点击“拒绝”，则发起者提示：“对方拒绝了您的请求。”
	// 若30秒内未做响应，则自动判断为拒绝。
	// 点击“接受”，则进入交互动作播放流程：
	// 发起者向接受者开始自动移动。
	// 若在自动移动过程中接受者移动，则交互动作失败，被取消。
	// 若在自动移动过程中发起者主动移动，则交互动作失败，被取消。
	// 若两者直线距离大于X个格子，则距离过远，交互动作失败，被取消。
	// 若发起者在向接受者自动移动过程中，被阻挡（如高差、不可行走区域），则交互动作失败，被取消。
	// 当上述一切判断、流程都通过后，发起者移动至接受者同一坐标同一高度位置，然后系统自动将朝向置为同一朝向，同时开始播放交互个性动作。
	// 若在交互动作播放流程过程中，接受其他角色或被其他角色接受另外一个已发起的交互动作申请，则之前的动作播放流程被打断。

}

DWORD Role::CastMotion( Unit* pDest, DWORD dwActionID )
{
	Role* pDestRole = static_cast<Role*>(pDest);
	
	tagNS_RoleStyleAction send;
	send.dwActionID = dwActionID;

	send.dwRoleID = GetID();
	send.dwDstRoleID = pDestRole->GetID();
	GetMap()->SendBigVisTileMsg(this, &send, send.dwSize);

	return E_Motion_Success;
}

BOOL Role::GetMotionInviteStatus(DWORD dwRoleID)
{
	// 是伙伴的请求，则同意
	if (dwRoleID == m_dwPartnerID)
		return TRUE;
	// 没有伙伴，并且超时
	else if (!GT_VALID(m_dwPartnerID) && !GT_VALID(m_nMotionCounter))
		return TRUE;
	// 不是伙伴，不超时
	else
		return FALSE;
}

VOID Role::SetMotionInviteStatus(BOOL bSet, DWORD dwRoleID)
{
	if (GT_VALID(m_dwPartnerID) && dwRoleID != m_dwPartnerID)
		return ;

	if (bSet)
	{
		m_nMotionCounter = TICK_PER_SECOND * 60;
		m_dwPartnerID = dwRoleID;
	}
	else
	{
		m_nMotionCounter = GT_INVALID;
		m_dwPartnerID = GT_INVALID;
	}
}

VOID Role::UpdateMotionInviteState()
{
	if (m_nMotionCounter >= 0)
	{
		--m_nMotionCounter;
	}
	// time out, reset the partner
	else if (!GT_VALID(m_nMotionCounter))
	{
		m_dwPartnerID = GT_INVALID;
	}
}

INT Role::CanGather(Creature* pRes)
{
	// 获得资源和角色
	Creature* pCreature	= pRes;	
	Role* pRole	= this;	

	if ( !P_VALID(pCreature) || !P_VALID(pRole) )
		return GT_INVALID;

	// 判断资源并获得技能
	Skill* pGatherSkill = NULL;
	if ( pCreature->IsNatuRes() )
		pGatherSkill = pRole->GetSkill(GATHER_SKILL_MINING);
	else if ( pCreature->IsManRes() )
		pGatherSkill = pRole->GetSkill(GATHER_SKILL_HARVEST);
	if ( !P_VALID(pGatherSkill) )
		return GT_INVALID;

	// 若角色背包已满				
	ItemMgr& itemMgr = pRole->GetItemMgr();	
	if (itemMgr.GetBagFreeSize() <= 0)
		return E_UseItem_NotEnoughSpace;

	// 若资源距离够近 
	if (!pRole->IsInCombatDistance(*pCreature, pGatherSkill->GetOPDist()))
		return E_UseSkill_DistLimit;	

	// 资源已被占有
	if( pCreature->IsTagged() )
		return E_UseSkill_TargetLimit;

	return E_Success;
}

VOID Role::ModMountSpeed( INT nMod )
{
	ModAttModValue(ERA_Speed_Mount, nMod);
	RecalAtt();
}

VOID Role::SaveExp2DB()
{
	tagNDBC_UpdateRoleAtt send;

	send.dwRoleID = GetID();
	send.nType = ERTSA_Exp;
	send.nValue = m_nCurLevelExp;

	g_dbSession.Send(&send, send.dwSize);
}

VOID Role::SaveAttPoint2DB()
{
	tagNDBC_UpdateRoleAttPoint send;

	send.dwRoleID = GetID();
	send.nAttPointLeft = GetAttValue(ERA_AttPoint);
	IFASTCODE->MemCpy(send.nAttPointAdd, m_nAttPointAdd, sizeof(send.nAttPointAdd));

	g_dbSession.Send(&send, send.dwSize);
}

VOID Role::SaveTalentPoint2DB(INT nIndex)
{
	tagNDBC_UpdateRoleTalentPoint send;

	send.dwRoleID = GetID();
	send.nTalentPoint = GetAttValue(ERA_TalentPoint);
	send.Talent = m_Talent[nIndex];
	send.nIndex = nIndex;
	g_dbSession.Send(&send, send.dwSize);
}

VOID Role::SaveLevel2DB()
{
	tagNDBC_UpdateRoleAtt send;

	send.dwRoleID = GetID();
	send.nType = ERTSA_Level;
	send.nValue = m_nLevel;
	g_dbSession.Send(&send, send.dwSize);
}
// 使用记录式传送符
VOID Role::UseNoteTakingItemTransport()
{
	GotoNewMap(m_dwItemTransportMapID, m_fItemTransportX, m_fItemTransportY, m_fItemTransportZ);
	
}
// 使用记录式传送符记录当前坐标
VOID Role::SetItemTransportPoint(tagNetCmd* pCmd)
{
	MGET_MSG(pRecv, pCmd, NC_SetTransportMap);

	E_SET_TRANSPORT_ERROR_CODE dwErrorCode = EST_SuccessSet;
	
	Map *pMap = GetMap();
	//如果是副本 或 练功场
	if(pMap->GetMapInfo()->eType != EMT_Normal || 
		(pMap->GetMapInfo()->eType == EMT_Normal && pMap->GetMapInfo()->eNoarmalMapType == ENMT_TrainingField))
	{
		dwErrorCode = EST_InInstance;
	}

	// 检查物品是否在背包里
	tagItem* pItem = this->GetItemMgr().GetBagItem(pRecv->n64ItemID); 
	if( !P_VALID(pItem) ) 
	{
		dwErrorCode = EST_LackNeedItem;
	}
	else
	{	
		if( this->IsInRoleState(ERS_PrisonArea) )
		{
			dwErrorCode = EST_SelfStateLimit;
		}
		else if(pItem->pProtoType->eSpecFunc != EISF_TransportTool)
		{
			dwErrorCode = EST_SpecFunError;
		}
	}

	//回复客户端
	tagNS_SetTransportMap	sendclient;
	sendclient.dwErrorCode = dwErrorCode;
	sendclient.dwTransportMapID = pMap->GetMapID();
	sendclient.vTransportPoint = GetCurPos();
	SendMessage(&sendclient, sendclient.dwSize);

	if(dwErrorCode == EST_SuccessSet)
	{
		//
		m_dwItemTransportMapID = pMap->GetMapID();
		m_fItemTransportX = sendclient.vTransportPoint.x;
		m_fItemTransportZ = sendclient.vTransportPoint.z;
		m_fItemTransportY = sendclient.vTransportPoint.y;

		tagNDBC_UpdateRoleItemTransport senddb;
		senddb.dwRoleID	= GetID();
		senddb.dwMapID	= pMap->GetMapID();
		senddb.fX		= sendclient.vTransportPoint.x;
		senddb.fZ		= sendclient.vTransportPoint.z;
		senddb.fY		= sendclient.vTransportPoint.y;
		g_dbSession.Send(&senddb, senddb.dwSize);
	}
}


// Jason
BOOL Role::IsValidOfLessingLoong(VOID)
{
	return ROLE_MAX_LESSLEVEL_OF_LOONG > m_nLessingLevel;
}

// Jason 2009-12-7 神龙赐福
VOID	Role::UpdateBlessingOfLoong(VOID)
{
	if( IsValidOfLessingLoong() ) //达到赐福最高级别，不予赐福
	{
		tagConfigLessingOfLoong * pCur = g_attRes.GetConfigInfoOfLessingLoong(m_nLessingLevel + 1); // 数据库里面默认是0
		if( !P_VALID( pCur ) )
			return;
		DWORD _now = GetCurrentOnlineTime();

		if( (DWORD)m_timeLastLessing == (DWORD)0 )
			m_timeLastLessing = _now;

		if( /*CalcTimeDiff( */_now - m_timeLastLessing /*)*/ >= pCur->nExpireTime && !m_bFiredOnceOfLessingLoong )
		{
			m_bFiredOnceOfLessingLoong = TRUE;
			//m_timeLastLessing = _now;
			NotifyClientToReceiveLessingOfLoong();
			//++m_nLessingLevel;
		}
	}
}

// Jason
VOID Role::NotifyClientToReceiveLessingOfLoong(VOID)
{
	tagNS_SetNextBenediction msg;
	tagConfigLessingOfLoong * pCur/*, * pNext*/;
	msg.isEnd			= !IsValidOfLessingLoong();


	if( !msg.isEnd )
	{
		pCur = g_attRes.GetConfigInfoOfLessingLoong(m_nLessingLevel + 1);//默认数据库里面是0

		if( !P_VALID( pCur ) )
			return;

		DWORD _now = GetCurrentOnlineTime();
		DWORD	lastLessingTime = m_timeLastLessing;

		DWORD val = pCur->nExpireTime + (DWORD)m_timeLastLessing ;
		if( val <= _now ) //到期了，或者超期
			msg.nextServerTime = 0;
		else
			msg.nextServerTime	= val - _now;/*CalcTimeDiff( tagDWORDTime(val)  , _now )*/;

		msg.nItemTypeID		= pCur->dwRewardItemTypeID;
		msg.nItemCount		= pCur->nItemCount;
		msg.n32Exprence		= pCur->nExperience;
	}
	else
	{
		msg.nItemTypeID		= GT_INVALID;
		msg.nItemCount		= 0;
		msg.n32Exprence		= 0;
	}

	SendMessage  (&msg,msg.dwSize);
}

// Jason
VOID Role::NotifyClientIniLessingOfLoong(VOID)
{
	if( IsValidOfLessingLoong() )
	{
		tagNS_LoongBenedictionInit msg;
		SendMessage( &msg, msg.dwSize );
		NotifyClientToReceiveLessingOfLoong();
	}
}
// Jason
DWORD Role::ReceiveLessingOfLoong(VOID)
{
	tagNS_GetLoongBenediction msg;
	DWORD re = 0;

	// 达到最高8次，停止该活动
	if( IsValidOfLessingLoong() )
	{
		do 
		{
			tagConfigLessingOfLoong *pCur = g_attRes.GetConfigInfoOfLessingLoong(m_nLessingLevel + 1);
			if( !P_VALID( pCur ) )
			{
				re = ELBD_No_LessingProto;
				IMSG(_T("Caution: Lessing of loong , no lessing prototype.\n"));
				return re;
				//break;
			}

			DWORD _now = GetCurrentOnlineTime();
			DWORD	lastLessingTime = m_timeLastLessing;

			// 到期判断
			DWORD val = pCur->nExpireTime + (DWORD)m_timeLastLessing ;
			if( val > _now )
			{
				re = ELBD_Time_NotArrive;
				break;
			}// 否则到期或者超期，应该处理

			if( pCur->nItemCount > 0 && P_VALID(pCur->dwRewardItemTypeID) )
			{
				//do{
				if( ( MIsEquipment(pCur->dwRewardItemTypeID) && ( GetItemMgr().GetBagFreeSize() < pCur->nItemCount ) ) || 
					( !MIsEquipment(pCur->dwRewardItemTypeID) && ( GetItemMgr  ().CanAddMaxHoldItem(pCur->dwRewardItemTypeID,pCur->nItemCount) == FALSE || GetItemMgr().GetBagFreeSize() < 1 )/*( GetItemMgr  ().CanAddMaxHoldItem(pCur->dwRewardItemTypeID,pCur->nItemCount) == FALSE  || 
					)*/
				)
				)
				{
					re = ELBD_Bag_NotEnoughSpace;

					msg.dwErrorCode = re;
					msg.nExprence	= 0;
					msg.nItemCount	= 0;
					msg.nItemTypeID	= GT_INVALID;

					SendMessage  (&msg,msg.dwSize);

					return re;
				}

					tagItem *pItem = NULL;
					pItem = ItemCreator::CreateByProduce(EICM_LessingLoong,  GetID(), 
						pCur->dwRewardItemTypeID,(EItemQuality)pCur->nQlty,0,0,pCur->nItemCount/*,GetID()*/ );

					if( !P_VALID(pItem) )
					{
						re = ELBD_Item_Create_Failed;
						IMSG(_T("Caution: Lessing of loong , create item failed.\n"));
						return re;
					}


					// 判断是否为时装，是则鉴定
					if(P_VALID(pItem) && MIsFashion(pItem))
					{
						ItemCreator::IdentifyEquip((tagEquip*)pItem);
					}


					if( P_VALID(pItem) )
					{
						re = GetItemMgr().Add2Bag(pItem, ELCID_LessingLoong, TRUE);
					}
					else
					{
						re = ELBD_Item_Create_Failed;
						IMSG(_T("Caution: Lessing of loong , bag not enough space.\n"));
						return re;
						//break;
					}
				//}while(0);
			}

			m_timeLastLessing = /*g_world.GetWorldTime()*/GetCurrentOnlineTime();
			++m_nLessingLevel;
			m_bFiredOnceOfLessingLoong = FALSE;

			if( E_Item_MaxHold == re )
			{
				re = ELBD_Bag_NotEnoughSpace;
				//break;
				IMSG(_T("Caution: Lessing of loong , bag not enough space.\n"));
			}

			// 增加经验，不会减少经验的。
			if( pCur->nExperience > 0 )
				ExpChange(pCur->nExperience);

			msg.dwErrorCode = re;
			msg.nExprence	= pCur->nExperience;
			msg.nItemCount	= pCur->nItemCount;
			msg.nItemTypeID	= pCur->dwRewardItemTypeID;

			SendMessage  (&msg,msg.dwSize);

			NotifyClientToReceiveLessingOfLoong();

			return re;
		} while (0);
	}
	else
		re = ELBD_No_Benediction;

	msg.dwErrorCode = re;
	msg.nExprence	= 0;
	msg.nItemCount	= 0;
	msg.nItemTypeID	= GT_INVALID;

	SendMessage  (&msg,msg.dwSize);
	return re;
}


DWORD Role::GetCurrentOnlineTime(VOID)
{
	DWORD re = 0;
//引用至 role_save.cpp line : 35	
	if(m_LogoutTime < m_LoginTime)
	{
		re += CalcTimeDiff(g_world.GetWorldTime(), m_LoginTime);
	}
	else
	{
		re += CalcTimeDiff(g_world.GetWorldTime(), m_LogoutTime);
	}
	if( m_nOnlineTime == GT_INVALID )
		return re;
	re += m_nOnlineTime;

	return re;
}

DWORD Role::ProcessGoldStoneMsg( INT64 GoldStoneItemID, INT64 destItemID )
{
	tagItem *pItem = GetItemMgr().GetBagItem( GoldStoneItemID ); 
	tagItem *pDestItem = GetItemMgr().GetBagItem( destItemID ); 
	if( !P_VALID( pItem ) || !P_VALID( pDestItem ) )
	{
		IMSG(_T("Caution: Gold Stone , no such item."));
		return E_GoldStone_SrcItemNotExist;
	}

	if( EISF_BeGoldStone == pItem->pProtoType->eSpecFunc )
	{
		tagPhilosophersStoneConfig * pConfig = g_attRes.GetConfigInfoOfPhilosophersStone(pItem->pProtoType->nSpecFuncVal1,pDestItem->pProtoType->dwTypeID);
		if( /*!*/P_VALID( pConfig )  )//没有配置文件，要报一下errlog
		//{
		//	IMSG(_T("Caution: Gold Stone , cannot find config file of philosopher's stone.\n"));
		//	return E_GoldStone_NoConfigFile;
		//}

		//if( pConfig->nStoneType == pItem->pProtoType->nSpecFuncVal1 )
		{
			DWORD re = 0;
			tagItem *pNewItem = NULL;
			pNewItem = ItemCreator::Create(EICM_GoldStone,  GetID(), 
				pConfig->dwDestItemTypeID,1,GetID() );


			// 判断是否为时装，是则鉴定
			if(P_VALID(pNewItem) && MIsFashion(pNewItem))
			{
				ItemCreator::IdentifyEquip((tagEquip*)pNewItem);
			}

			GetItemMgr  ().DelFromBag(GoldStoneItemID,ELCID_PhilosophersStone,1);

			if( P_VALID(pNewItem) )
			{
				GetItemMgr().DelFromBag  (destItemID,ELCID_PhilosophersStone/*,1*/);
				re = GetItemMgr().Add2Bag(pNewItem, ELCID_PhilosophersStone, TRUE);
			}
			else
			{
				re = E_GoldStone_CreateDestItemFail;
				IMSG(_T("Caution: Gold Stone , create new item false.\n"));
				return re;
				//break;
			}
			return E_GoldStone_Success;
		}
		else
		{
			IMSG(_T("Caution: Gold Stone , philosopher's stone missing match.\n"));
			return E_GoldStone_MissingMatch;
		}
	}
	// else 不是点金石

	IMSG(_T("Caution: Gold Stone , it's not philosopher's stone.\n"));

	return E_GoldStone_OtherError;
}

DWORD Role::VocationCheck( DWORD dwVocationLimit )
{
	INT nTmpClass =  1 << ( m_eClass - 1 );
	INT nTmpClassEx = 0;
	INT nTmp = 0;

	if ( (INT)m_eClassEx != (INT)EHV_Base )
	{
		nTmpClassEx = 1 << ( m_eClassEx + 8 );
	}

	nTmp = nTmpClass + nTmpClassEx;

	if ( !( nTmp & dwVocationLimit ) )
		return E_SystemError;

	return E_Success;
}

DWORD Role::CanPickUpItem( INT64 n64GroundID, BOOL bRole /*= TRUE*/ )
{
	MTRANS_ELSE_RET(pMap, GetMap(), Map, E_Loot_Map_Not_Found);
	MTRANS_ELSE_RET(pGroundItem, pMap->GetGroundItem(n64GroundID), tagGroundItem, E_Loot_Item_Not_Found);

	if(pGroundItem->dwTeamID == GT_INVALID && pGroundItem->dwOwnerID != GT_INVALID && GetID() != pGroundItem->dwOwnerID)
		return E_Loot_Belong_To_You;

	if(pGroundItem->dwTeamID != GT_INVALID && GetTeamID() != pGroundItem->dwTeamID)
		return E_Loot_Belong_To_Team;

	if(pGroundItem->dwTeamID != GT_INVALID && pGroundItem->dwOwnerID != GT_INVALID && GetID() != pGroundItem->dwOwnerID)
		return E_Loot_Belong_To_Teammate;

	if(bRole)
	{
		//检查玩家和物品的距离 拾取范围和自动拾取范围是不一样的
		if ( abs(pGroundItem->vPos.x - GetCurPos().x) > X_DEF_AUTO_PICKUP_DIST 
			|| abs(pGroundItem->vPos.z - GetCurPos().z) > X_DEF_AUTO_PICKUP_DIST )
			return E_Loot_Pick_Up_Too_Far;
	}

	//背包是否有空间
	if ( TYPE_ID_MONEY != pGroundItem->dwTypeID && GetItemMgr().GetBagFreeSize() <= 0)
		return E_Loot_BAG_NOT_ENOUGH_SPACE;

	// 最大堆叠数目
	BOOL bCan = GetItemMgr().CanAddMaxHoldItem(pGroundItem->dwTypeID, pGroundItem->nNum);

	return bCan ? E_Success : E_Item_MaxHold;
}
