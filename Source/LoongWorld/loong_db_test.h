//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: loong_db_test.h
// author: Sxg
// actor:
// data: 2008-12-5
// last:
// brief: 发送消息到LoongDB服务器，测试数据库处理效率
//-----------------------------------------------------------------------------
#include "stdafx.h"

#include "../ServerDefine/role_data.h"

//-----------------------------------------------------------------------------
// 读取角色(参数: 1, 100, 10 -- 结果: 从角色id为1和100之间随机从roledata表中读取10条记录)
//-----------------------------------------------------------------------------
DWORD WINAPI TestLoadRole(LPCTSTR strNamePostfix, LPCTSTR szNum, LPCTSTR szTestNum)
{
	INT nPostfix = _ttoi(strNamePostfix);
	INT nNum = _ttoi(szNum);
	INT nTestNum = _ttoi(szTestNum);

	if(nPostfix < 0 || nNum <= 0 || nTestNum <= 0)
	{
		return GT_INVALID;
	}
	
	tagNDBC_TestLoadRole send;

	while(nTestNum > 0)
	{
		send.dwAccountID = nPostfix + IUTIL->Rand()%nNum;
		send.dwRoleID = send.dwAccountID;

		g_dbSession.Send(&send, send.dwSize);

		--nTestNum;
	}
	
	return 0;
}

//-----------------------------------------------------------------------------
// 更新角色(参数: 1, 100, 10 -- 结果: 从角色id为1和100之间随机更新roledata表中10条记录)
//-----------------------------------------------------------------------------
DWORD WINAPI TestUpdateRole(LPCTSTR strNamePostfix, LPCTSTR szNum, LPCTSTR szTestNum)
{
	INT nPostfix = _ttoi(strNamePostfix);
	INT nNum = _ttoi(szNum);
	INT nTestNum = _ttoi(szTestNum);

	if(nPostfix < 0 || nNum <= 0 || nTestNum <= 0)
	{
		return GT_INVALID;
	}
	
	tagNDBC_SaveRole *pSend = new tagNDBC_SaveRole;

	pSend->RoleData.LoginTime = 0;
	pSend->RoleData.LogoutTime = 0;
	pSend->RoleData.nOnlineTime = 0;
	pSend->RoleData.nCurOnlineTime = 0;

	ZeroMemory(&pSend->RoleData.AvatarEquip, SIZE_AVATAR_EQUIP);
	pSend->RoleData.DisplaySet = 0;
	pSend->RoleData.dwMapID = 3618849499;
	pSend->RoleData.dwRebornMapID = 3618849499;
	pSend->RoleData.fCoordinate[0] = 100.0f;
	pSend->RoleData.fCoordinate[1] = 100.0f;
	pSend->RoleData.fCoordinate[2] = 100.0f;
	pSend->RoleData.fFace[0] = 100.0f;
	pSend->RoleData.fFace[1] = 100.0f;
	pSend->RoleData.fFace[2] = 100.0f;

	pSend->RoleData.eClass = EV_Base;
	pSend->RoleData.eClassEx = EHV_Base;
	pSend->RoleData.nLevel = 1;
	pSend->RoleData.nCurExp = 0;
	pSend->RoleData.nHP = 100;
	pSend->RoleData.nMP = 100;
	pSend->RoleData.nAttPoint = 0;
	pSend->RoleData.nTalentPoint = 0;
	pSend->RoleData.nRage = 0;
	pSend->RoleData.nEndurance = 20;

	ZeroMemory(pSend->RoleData.nAttPointAdd, sizeof(pSend->RoleData.nAttPointAdd));

	pSend->RoleData.nVitality = 0;
	pSend->RoleData.nInjury = 0;
	pSend->RoleData.nKnowledge = 0;
	pSend->RoleData.nMorality = 0;
	pSend->RoleData.nCulture = 0;
	pSend->RoleData.nCredit = 0;
	pSend->RoleData.nIdentity = 0;
	pSend->RoleData.nVIPPoint = 0;

	pSend->RoleData.n16BagSize = 20;
	pSend->RoleData.nBagGold = 999;
	pSend->RoleData.nBagSilver = 9999;
	pSend->RoleData.nBagYuanBao = 100;

	pSend->RoleData.bSafeGuard = ERS_Safeguard;
	pSend->RoleData.CloseSafeGuardTime = 0;
	ZeroMemory(pSend->RoleData.talent, sizeof(pSend->RoleData.talent));

	/*********************************************************************************************************
	*技能列表，状态列表，称号列表，物品列表，装备列表，好友列表，仇敌列表，任务列表，已完成任务列表，
	*物品冷却时间表
	*必须按顺序
	*********************************************************************************************************/
	LPVOID pData = pSend->RoleData.byData;

	// 1.技能列表
	pSend->RoleData.nSkillNum		= 0;

	// 2.状态列表
	pSend->RoleData.nBuffNum		= 0;

	// 3.称号列表 -- 存: 实时,即单独发消息
	pSend->RoleData.nTitleNum		= 0;

	// 4.物品列表 5.装备列表
	pSend->RoleData.nItemNum		= 0;

	// 6.好友列表 -- 存: 实时,即单独发消息
	pSend->RoleData.nFriendNum		= 0;

	// 7.仇敌列表 -- 存: 实时,即单独发消息
	pSend->RoleData.nEnemyNum		= 0;

	// 8.任务列表
	pSend->RoleData.nQuestNum		= 0;

	// 9.已完成任务列表 -- 存: 实时,即单独发消息
	pSend->RoleData.nQuestDoneNum	= 0;

	// 10.物品冷却时间
	pSend->RoleData.nItemCDTimeNum	= 0;

	// 重新计算消息大小
	pSend->dwSize = (DWORD)((BYTE*)pData - (BYTE*)pSend);

	while(nTestNum > 0)
	{
		pSend->dwRoleID = nPostfix + IUTIL->Rand()%nNum;

		g_dbSession.Send(pSend, pSend->dwSize);

		--nTestNum;
	}

	delete pSend;

	return 0;
}

//-----------------------------------------------------------------------------
// 读取物品&装备(参数: 1, 100, 10 -- 结果: 从角色id为1和100之间随机读取10个角色的物品&装备)
//-----------------------------------------------------------------------------
DWORD WINAPI TestLoadItem(LPCTSTR strNamePostfix, LPCTSTR szNum, LPCTSTR szTestNum)
{
	INT nPostfix = _ttoi(strNamePostfix);
	INT nNum = _ttoi(szNum);
	INT nTestNum = _ttoi(szTestNum);

	if(nPostfix < 0 || nNum <= 0 || nTestNum <= 0)
	{
		return GT_INVALID;
	}

	tagNDBC_TestLoadRoleItem send;

	while(nTestNum > 0)
	{
		send.dwAccountID = (nPostfix + IUTIL->Rand()%nNum)/10 * 10 + 1;
		send.dwRoleID = send.dwAccountID;

		g_dbSession.Send(&send, send.dwSize);

		--nTestNum;
	}

	return 0;
}

//-----------------------------------------------------------------------------
// 保存物品(参数: 1, 100, 10, 999 -- 结果:从Serial为1到100之间的物品中随机抽取一件,并用typeid为999的物品信息更新)
//-----------------------------------------------------------------------------
DWORD WINAPI TestUpdateItem(LPCTSTR szSerialStart, LPCTSTR szNum, LPCTSTR szTestNum, LPCTSTR szItemTypeID)
{
	INT64	n64SerialStart = _ttoi64(szSerialStart);
	INT		nNum = _ttoi(szNum);
	INT		nTestNum = _ttoi(szTestNum);
	DWORD	dwTypeID	= (DWORD)_ttoi(szItemTypeID);

	if(n64SerialStart <= 0 || nNum <= 0 || nTestNum <= 0)
	{
		return GT_INVALID;
	}

	if(MIsEquipment(dwTypeID))
	{
		return GT_INVALID;
	}
	
	tagItem *pItem = ItemCreator::Create(EICM_Null, GT_INVALID, dwTypeID);
	if(!P_VALID(pItem))
	{
		ASSERT(P_VALID(pItem));
		return GT_INVALID;
	}

	pItem->n16Index = IUTIL->Rand()%10;
	
	while(nTestNum > 0)
	{
		pItem->n64Serial = n64SerialStart + IUTIL->Rand()%nNum;
		pItem->dwOwnerID = IUTIL->Rand()%50000;
		pItem->dwAccountID = pItem->dwOwnerID;
		pItem->eConType	= (EItemConType)(IUTIL->Rand()%5);

		tagNDBC_TestSaveItem send;
		IFASTCODE->MemCpy(&send.item, pItem, SIZE_ITEM);

		g_dbSession.Send(&send, send.dwSize);

		--nTestNum;
	}

	delete pItem;
	
	return 0;
}

//-----------------------------------------------------------------------------
// 保存装备(参数: 1, 100, 10, 999 -- 结果:从Serial为1到100之间的物品中随机抽取一件,并用typeid为999的装备信息更新)
//-----------------------------------------------------------------------------
DWORD WINAPI TestUpdateEquip(LPCTSTR szSerialStart, LPCTSTR szNum, LPCTSTR szTestNum, LPCTSTR szEquipTypeID)
{
	INT64	n64SerialStart = _ttoi64(szSerialStart);
	INT		nNum = _ttoi(szNum);
	INT		nTestNum = _ttoi(szTestNum);
	DWORD	dwTypeID	= (DWORD)_ttoi(szEquipTypeID);

	if(n64SerialStart <= 0 || nNum <= 0 || nTestNum <= 0)
	{
		return GT_INVALID;
	}

	if(!MIsEquipment(dwTypeID))
	{
		return GT_INVALID;
	}

	tagEquip *pEquip = (tagEquip*)ItemCreator::Create(EICM_Null, GT_INVALID, dwTypeID);
	if(!P_VALID(pEquip))
	{
		ASSERT(P_VALID(pEquip));
		return GT_INVALID;
	}

	while(nTestNum > 0)
	{
		ItemCreator::IdentifyEquip(pEquip);

		pEquip->n64Serial = n64SerialStart + IUTIL->Rand()%nNum;
		pEquip->n16Index = IUTIL->Rand()%10 + 10;

		tagNDBC_TestSaveEquip send;
		IFASTCODE->MemCpy(&send.equip, pEquip, SIZE_EQUIP);

		g_dbSession.Send(&send, send.dwSize);

		--nTestNum;
	}

	delete pEquip;

	return 0;
}