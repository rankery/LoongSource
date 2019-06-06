//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: create_db_record.h
// author: Sxg
// actor:
// data: 2008-12-1
// last:
// brief: 数据记录生成相关
//-----------------------------------------------------------------------------
#include "stdafx.h"

#include "att_res.h"
#include "item_creator.h"

#include "../ServerDefine/msg_item.h"
#include "../ServerDefine/role_data.h"

//-----------------------------------------------------------------------------
// 创建角色 -- 生成规则应同账号名称(参数：Test 1 100 -- 角色名：Test1,Test2,...Test100)
//-----------------------------------------------------------------------------
DWORD WINAPI CreateRole(LPCTSTR szNamePrefix, LPCTSTR strNamePostfix, LPCTSTR szNum)
{
	INT32 nPostfix = _ttoi(strNamePostfix);
	INT32 nNum = _ttoi(szNum);

	if(nPostfix < 0 || nNum < 0)
	{
		return GT_INVALID;
	}

	for(INT32 i=nPostfix; i<nPostfix+nNum; ++i)
	{
		tagNDBC_CreateRole send;
		send.dwAccountID = i;

		// 生成角色名
		_stprintf_s(send.RoleInfo.szRoleName, _T("%s%d"), szNamePrefix, i);

		send.RoleInfo.byLevel = 1;
		send.RoleInfo.dwMapID = g_mapCreator.GetBornMapID();

		send.RoleInfo.CreateTime = GetCurrentDWORDTime();

		// 临时填写，到后期会从该地图的导航点中寻找到一个随机坐标点
		send.RoleInfo.fX = 0.0f;
		send.RoleInfo.fY = 0.0f;
		send.RoleInfo.fZ = 0.0f;
		send.RoleInfo.fFaceX = 0.0f;
		send.RoleInfo.fFaceY = 0.0f;
		send.RoleInfo.fFaceZ = 0.0f;

		g_dbSession.Send(&send, send.dwSize);
	}

	return 0;
}

//-----------------------------------------------------------------------------
// 创建非装备类物品(参数:100 1 -- 结果: 给roleid为1的角色生成100个非装备类物品)
//-----------------------------------------------------------------------------
DWORD WINAPI CreateItem(LPCTSTR strItemNum, LPCTSTR strRoleIDStart)
{
	INT32 nNum = _ttoi(strItemNum);
	DWORD dwRoleID = (DWORD)_ttoi(strRoleIDStart);
	DWORD dwTypeID = GT_INVALID;

	while(nNum > 0)
	{
		for(INT32 i=0; (i<20 && nNum>0); ++i)
		{
			// 随机获取TypeID
			dwTypeID = 1000002;
			
			tagItem *pItem = ItemCreator::Create(EICM_Null, GT_INVALID, dwTypeID);
			if(!P_VALID(pItem))
			{
				ASSERT(P_VALID(pItem));
				continue;
			}

			pItem->SetOwner(dwRoleID, dwRoleID);
			
			if(i<10)
			{
				pItem->SetPos(EICT_Bag, i);
			}
			else
			{
				pItem->SetPos(EICT_RoleWare, i-10);
			}
			
			tagNDBC_NewItem send;
			IFASTCODE->MemCpy(&send.item, pItem, SIZE_ITEM);

			g_dbSession.Send(&send, send.dwSize);

			delete pItem;

			--nNum;
		}
		
		dwRoleID += 10;
	}

	return 0;
}

//-----------------------------------------------------------------------------
// 创建装备(参数:100 1 -- 结果: 给roleid为1的角色生成100个装备)
//-----------------------------------------------------------------------------
DWORD WINAPI CreateEquip(LPCTSTR strEquipNum, LPCTSTR strRoleIDStart)
{
	INT32 nNum = _ttoi(strEquipNum);
	DWORD dwRoleID = (DWORD)_ttoi(strRoleIDStart);
	DWORD dwTypeID = GT_INVALID;

	while(nNum > 0)
	{
		for(INT32 i=0; i<20; ++i)
		{
			// 随机获取TypeID
			dwTypeID = 8010010;
			
			tagEquip *pEquip = (tagEquip*)ItemCreator::Create(EICM_Null, GT_INVALID, dwTypeID);
			if(!P_VALID(pEquip))
			{
				ASSERT(P_VALID(pEquip));
				continue;
			}

			ItemCreator::IdentifyEquip(pEquip);

			pEquip->SetOwner(dwRoleID, dwRoleID);

			if(i<10)
			{
				pEquip->SetPos(EICT_Bag, i+10);
			}
			else
			{
				pEquip->SetPos(EICT_RoleWare, i);
			}

			tagNDBC_NewEquip send;
			IFASTCODE->MemCpy(&send.equip, pEquip, SIZE_EQUIP);

			g_dbSession.Send(&send, send.dwSize);

			delete pEquip;

			--nNum;
		}

		dwRoleID += 10;
	}

	return 0;
}

std::vector<DWORD> g_LogRoleIDs;
DWORD WINAPI LogSelectRole(LPCTSTR roldid)
{
	DWORD roleID = (DWORD)atoi(TObjRef<Util>()->UnicodeToUnicode8(roldid));
	if(roleID > 0)
	{
		for(std::vector<DWORD>::const_iterator itor=g_LogRoleIDs.begin(); itor != g_LogRoleIDs.end(); itor++)
		{
			if(*itor == roleID)
				return 0;
		}
		g_LogRoleIDs.push_back(roleID);
	}
	return 0;
}

DWORD WINAPI NLogSelectRole(LPCTSTR roldid)
{
	DWORD roleID = (DWORD)atoi(TObjRef<Util>()->UnicodeToUnicode8(roldid));
	if(roleID > 0)
	{
		for(std::vector<DWORD>::iterator itor=g_LogRoleIDs.begin(); itor != g_LogRoleIDs.end(); itor++)
		{
			if(*itor == roleID)
			{
				g_LogRoleIDs.erase(itor);
				break;
			}
		}
	}
	return 0;
}