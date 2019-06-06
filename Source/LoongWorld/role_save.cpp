//-------------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: role_save.cpp
// author: 
// actor:
// data: 2008-11-19
// last:
// brief: 人物数据结构保存
//-------------------------------------------------------------------------------
#include "StdAfx.h"

#include "../ServerDefine/role_data_define.h"
#include "../ServerDefine/role_data.h"

#include "../WorldDefine/currency_define.h"

#include "role.h"
#include "db_session.h"
#include "buff.h"
#include "title_mgr.h"
#include "pet_pocket.h"

//-------------------------------------------------------------------------------
// 保存玩家到DB
//-------------------------------------------------------------------------------
VOID Role::SaveToDB()
{
	if( _isnan(m_MoveData.m_vPos.x) || _isnan(m_MoveData.m_vPos.y) || _isnan(m_MoveData.m_vPos.z) )
	{
		ILOG->Write(_T("Error1!!!! Role's position Invalid, Role ID is %u, movestate is %d, nextstate is %d, startpos is <%f, %f, %f> endpos is <%f, %f, %f>\r\n\r\n"), 
			m_dwID, m_MoveData.m_eCurMove, m_MoveData.m_eNextPreMove, m_MoveData.m_vPosStart.x, m_MoveData.m_vPosStart.y, m_MoveData.m_vPosStart.z, 
			m_MoveData.m_vDest.x, m_MoveData.m_vDest.y, m_MoveData.m_vDest.z);
	}

	if(m_LogoutTime < m_LoginTime)
	{
		m_nOnlineTime += CalcTimeDiff(g_world.GetWorldTime(), m_LoginTime);
	}
	else
	{
		m_nOnlineTime += CalcTimeDiff(g_world.GetWorldTime(), m_LogoutTime);
	}

	m_LogoutTime = g_world.GetWorldTime();
	m_nCurOnlineTime = CalcTimeDiff(m_LogoutTime, m_LoginTime);
	//m_nOnlineTime += m_nCurOnlineTime;

	m_SaveRole.Init();
	tagNDBC_SaveRole *pSend = m_SaveRole;

	pSend->dwRoleID = m_dwID;

	// Jason 2009-12-7 神龙赐福
	pSend->RoleData.timeLastLessing = m_nLessingLevel > 0 ? m_timeLastLessing : 0;
	pSend->RoleData.nLessingLevel = m_nLessingLevel;

	pSend->RoleData.LoginTime = m_LoginTime;
	pSend->RoleData.LogoutTime = m_LogoutTime;
	pSend->RoleData.nOnlineTime = m_nOnlineTime;
	pSend->RoleData.nCurOnlineTime = m_nCurOnlineTime;
	pSend->RoleData.dwTimeGetMallFree = m_LastGetMallFreeTime;

	pSend->RoleData.DisplaySet = m_DisplaySet;
	pSend->RoleData.AvatarEquip = GetAvatarEquip();

	pSend->RoleData.dwMapID = m_dwMapID;
	pSend->RoleData.dwRebornMapID = m_dwRebornMapID;
	pSend->RoleData.fCoordinate[0] = m_MoveData.m_vPos.x;
	pSend->RoleData.fCoordinate[1] = m_MoveData.m_vPos.y;
	pSend->RoleData.fCoordinate[2] = m_MoveData.m_vPos.z;
	pSend->RoleData.fFace[0] = m_MoveData.m_vFace.x;
	pSend->RoleData.fFace[1] = m_MoveData.m_vFace.y;
	pSend->RoleData.fFace[2] = m_MoveData.m_vFace.z;

	pSend->RoleData.eClass = m_eClass;
	pSend->RoleData.eClassEx = m_eClassEx;
	pSend->RoleData.nLevel = m_nLevel;
	pSend->RoleData.nCurExp = m_nCurLevelExp;
	pSend->RoleData.nHP = GetAttValue(ERA_HP);
	pSend->RoleData.nMP = GetAttValue(ERA_MP);
	pSend->RoleData.nAttPoint = GetAttValue(ERA_AttPoint);
	pSend->RoleData.nTalentPoint = GetAttValue(ERA_TalentPoint);
	pSend->RoleData.nRage = GetAttValue(ERA_Rage);
	pSend->RoleData.nEndurance = GetAttValue(ERA_Endurance);

	IFASTCODE->MemCpy(pSend->RoleData.nAttPointAdd, m_nAttPointAdd, sizeof(m_nAttPointAdd));

	pSend->RoleData.nVitality = GetAttValue(ERA_Vitality);
	pSend->RoleData.nInjury = GetAttValue(ERA_Injury);
	pSend->RoleData.nKnowledge = GetAttValue(ERA_Knowledge);
	pSend->RoleData.nMorale = GetAttValue(ERA_Morale);
	pSend->RoleData.nMorality = GetAttValue(ERA_Morality);
	pSend->RoleData.nCulture = GetAttValue(ERA_Culture);
	pSend->RoleData.nHostility = GetAttValue(ERA_Hostility);
	pSend->RoleData.nCredit = m_nCredit;
	pSend->RoleData.nIdentity = m_nIdentity;
	pSend->RoleData.nVIPPoint = m_nVIPPoint;

	pSend->RoleData.Strength.dwValue = m_Strength.dwValue;
	pSend->RoleData.nStrengthNum = m_nStrengthNum;
	pSend->RoleData.Strength.m_dwErrantry = m_Strength.m_dwErrantry;
	pSend->RoleData.Strength.m_dwHeroism = m_Strength.m_dwHeroism;
	pSend->RoleData.Strength.m_dwValor = m_Strength.m_dwValor;
	pSend->RoleData.Strength.m_dwWit = m_Strength.m_dwWit;

	pSend->RoleData.n16BagSize = GetItemMgr().GetBagCurSize();
	pSend->RoleData.nBagGold = MSilver2DBGold(GetCurMgr().GetBagSilver());
	pSend->RoleData.nBagSilver = MSilver2DBSilver(GetCurMgr().GetBagSilver());
	pSend->RoleData.nBagYuanBao = GetCurMgr().GetBagYuanBao();
	pSend->RoleData.nExVolume = GetCurMgr().GetExchangeVolume();

	pSend->RoleData.dwGuildID = m_dwGuildID;

	pSend->RoleData.bSafeGuard = (GetRoleState() & ERS_Safeguard);
	pSend->RoleData.CloseSafeGuardTime = m_CloseSafeGuardTime;
	pSend->RoleData.nDeadUnSetSafeGuardCountDown = m_nDeadUnSetSafeGuardCountDown;
	pSend->RoleData.bNeedPrisonRevive = m_bNeedPrisonRevive;
	IFASTCODE->MemCpy(pSend->RoleData.talent, m_Talent, sizeof(pSend->RoleData.talent));

	// 对远端玩家公开选项设置
	pSend->RoleData.sRemoteOpenSet.bTitle	= GetTitleMgr()->Visibility();

	// 称号
	pSend->RoleData.u16ActiveTitleID		= GetTitleMgr()->GetActiviteTitle();

	pSend->RoleData.dwLoverID				= m_dwLoverID;
	pSend->RoleData.bHaveWedding			= m_bHaveWedding;

	// 玩家脚本数据
	IFASTCODE->MemCpy(pSend->RoleData.Data.dwData, m_ScriptData.dwData, sizeof(DWORD)*ESD_Role);

	// 玩家摊位信息
	if (P_VALID(m_pStall))
	{
		m_pStall->Save2DB(&(pSend->RoleData));
	}
	
	// 玩家禁言状态
	pSend->RoleData.bSpeakOff = m_bSpeakOff;

	/*********************************************************************************************************
	*技能列表，状态列表，称号列表，物品列表，装备列表，好友列表，仇敌列表，任务列表，已完成任务列表，
	*物品冷却时间表
	*必须按顺序
	*********************************************************************************************************/
	LPVOID pData = pSend->RoleData.byData;

	// 1.技能列表
	pSend->RoleData.nSkillNum		= 0;
	SaveSkill2DB(pData, pData, pSend->RoleData.nSkillNum);

	// 2.状态列表
	pSend->RoleData.nBuffNum		= 0;
	SaveBuff2DB(pData, pData, pSend->RoleData.nBuffNum);

	// 3.称号条件列表
	pSend->RoleData.nTitleNum		= 0;
	GetTitleMgr()->SaveTitlesToDB(pData, pData, pSend->RoleData.nTitleNum);

	// 4.物品列表 5.装备列表
	pSend->RoleData.nItemNum		= 0;
	GetItemMgr().SaveItem2DB(pData, pData, pSend->RoleData.nItemNum);

	// 6.好友列表 -- 存: 实时,即单独发消息
	pSend->RoleData.nFriendNum		= 0;

	// 7.仇敌列表 -- 存: 实时,即单独发消息
	pSend->RoleData.nEnemyNum		= 0;

	// 8.任务列表
	pSend->RoleData.nQuestNum		= 0;
	SaveQuest2DB(pData, pData, pSend->RoleData.nQuestNum);

	// 9.已完成任务列表 -- 存: 实时,即单独发消息
	pSend->RoleData.nQuestDoneNum	= 0;

	// 10.物品冷却时间
	pSend->RoleData.nItemCDTimeNum	= 0;
	GetItemMgr().SaveCDTime2DB(pData, pData, pSend->RoleData.nItemCDTimeNum);

	// 11.氏族数据
	m_ClanData.Save2DB(pData, pData, pSend->RoleData.bClanDataChg);

	// 12.宠物
	GetPetPocket()->SaveToDB(pData, pData, pSend->RoleData.nPetsNum);

	// 重新计算消息大小
	pSend->dwSize = (DWORD)((BYTE*)pData - (BYTE*)pSend);

	g_dbSession.Send(pSend, pSend->dwSize);

	// 重置保存倒计时
	ResetNeedSave2DBCD();

	if( _isnan(m_MoveData.m_vPos.x) || _isnan(m_MoveData.m_vPos.y) || _isnan(m_MoveData.m_vPos.z) )
	{
		ILOG->Write(_T("Error2!!!! Role's position Invalid, Role ID is %u, movestate is %d, nextstate is %d, startpos is <%f, %f, %f> endpos is <%f, %f, %f>\r\n\r\n"), 
			m_dwID, m_MoveData.m_eCurMove, m_MoveData.m_eNextPreMove, m_MoveData.m_vPosStart.x, m_MoveData.m_vPosStart.y, m_MoveData.m_vPosStart.z, 
			m_MoveData.m_vDest.x, m_MoveData.m_vDest.y, m_MoveData.m_vDest.z);
	}
}

//-------------------------------------------------------------------------------
// 保存技能
//-------------------------------------------------------------------------------
VOID Role::SaveSkill2DB(OUT LPVOID pData, OUT LPVOID &pOutPointer, OUT INT32 &nNum)
{
	INT32 i	= 0;
	Skill* pSkill = NULL;
	tagSkillSave *pSkillSave = (tagSkillSave*)pData;

	TMap<DWORD, Skill*>::TMapIterator iter = m_mapSkill.Begin();
	while(m_mapSkill.PeekNext(iter, pSkill))
	{
		// 如果是普通攻击技能，则不保存
		if( ESSTE_Default == pSkill->GetTypeEx() ) continue;

		// 如果没变过，不保存
		if (!pSkill->IsNeedSave()) continue;

		pSkill->InitSkillSave(&pSkillSave[i++]);
		pSkill->SetSaved();
	}

	nNum = i;
	pOutPointer = &pSkillSave[nNum];
}

//-------------------------------------------------------------------------------
// 保存Buff
//-------------------------------------------------------------------------------
VOID Role::SaveBuff2DB(OUT LPVOID pData, OUT LPVOID &pOutPointer, OUT INT32 &nNum)
{
	INT32		nSize = 0;
	BYTE		*pCurPointer = (BYTE*)pData;

	INT nBuffNum = 0;
	for(INT n = 0; n < MAX_BUFF_NUM; ++n)
	{
		if( !m_Buff[n].IsValid() ) continue;

		// 下线消失的buff
		if( m_Buff[n].Interrupt(EBIF_OffLine) ) continue;

		m_Buff[n].InitBuffSave((tagBuffSave*)pCurPointer, nSize);
		pCurPointer += nSize;
		++nBuffNum;
	}

	nNum = nBuffNum;

	pOutPointer = pCurPointer;
}

//---------------------------------------------------------------------------------
// 保存任务
//---------------------------------------------------------------------------------
VOID Role::SaveQuest2DB(OUT LPVOID pData, OUT LPVOID &pOutPointer, OUT INT32 &nNum)
{
	tagQuestSave* pQuestSave = (tagQuestSave*)pData;

	nNum = 0;

	for(INT n = 0; n < QUEST_MAX_COUNT; ++n)
	{
		if( FALSE == m_Quests[n].IsValid() )
			continue;

		m_Quests[n].InitQuestSave(&pQuestSave[nNum++]);
	}

	pOutPointer = &pQuestSave[nNum];
}
