//-------------------------------------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: loong_test.cpp
// author: Sxg
// actor:
// data: 2008-06-04
// last:
// brief: 处理从游戏服务器接收到的消息 -- 测试相关
//-------------------------------------------------------------------------------------------------------
#include "StdAfx.h"

#include "Loong.h"
#include "LoongDB.h"
#include "../ServerDefine/msg_item.h"
#include "../ServerDefine/role_data.h"
//-------------------------------------------------------------------------------------------------------
// 压测专用
//-------------------------------------------------------------------------------------------------------
DWORD CLoong::TestLoadRole(DWORD pDBMsg, DWORD dwDummy)
{
	MGET_MSG(p, pDBMsg, NDBC_TestLoadRole);

	tagRoleDataLoad tmp;
	LPVOID pData = &tmp;
	if(!m_pDB->TestLoadRole(pData, p->dwAccountID, p->dwRoleID))
	{
		return GT_INVALID;
	}

	return 0;
}


DWORD CLoong::TestLoadRoleItem(DWORD pDBMsg, DWORD dwDummy)
{
	MGET_MSG(p, pDBMsg, NDBC_TestLoadRoleItem);

	static BYTE byTmpBuff[1024 * 1024];
	LPVOID byBuff = (LPVOID)byTmpBuff;
	INT32 nNum;

	if(!m_pDB->TestLoadRoleItem(byBuff, nNum, p->dwAccountID, p->dwRoleID))
	{
		return GT_INVALID;
	}

	return 0;
}

//-------------------------------------------------------------------------------------------------------
// 向物品表中保存物品 -- 压测专用
//-------------------------------------------------------------------------------------------------------
DWORD CLoong::TestSaveItem(DWORD pDBMsg, DWORD dwDummy)
{
	MGET_MSG(p, pDBMsg, NDBC_TestSaveItem);

	if(!m_pDB->SaveItem(p->item))
	{
		return GT_INVALID;
	}

	return 0;
}

//-------------------------------------------------------------------------------------------------------
// 向装备表和物品表中保存装备 -- 压测专用
//-------------------------------------------------------------------------------------------------------
DWORD CLoong::TestSaveEquip(DWORD pDBMsg, DWORD dwDummy)
{
	MGET_MSG(p, pDBMsg, NDBC_TestSaveEquip);

	if(!m_pDB->SaveEquip(p->equip))
	{
		return GT_INVALID;
	}

	return 0;
}

//-------------------------------------------------------------------------------------------------------
// 数据库读取测试
//-------------------------------------------------------------------------------------------------------
DWORD CLoong::TestDBReadFunction()
{
	
	INT32 nResult= m_pDB->TestDBReadFunction();
	if(nResult == E_Success)
	{
		IMSG(_T("db read funcation is well!\r\n"));
	}
	else
	{
		IMSG(_T("db read funcation is bad!\r\n"));
	}
	
	return 0;
}