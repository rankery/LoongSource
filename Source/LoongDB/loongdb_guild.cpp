//------------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: loongdb_guild.cpp
// author: Sxg
// actor:
// data: 2008-04-14
// last:
// brief: 数据库操作应用层实现 -- 保存帮派数据相关
//------------------------------------------------------------------------------
#include "StdAfx.h"

#include "LoongDB.h"
#include "../ServerDefine/guild_define.h"
#include "../ServerDefine/msg_guild.h"
#include "../WorldDefine/container_define.h"

/***************************** 帮派属性表相关 *********************************/

//------------------------------------------------------------------------------
// 读取帮派
//------------------------------------------------------------------------------
BOOL CLoongDB::LoadAllGuild(LPVOID pBuff, INT32& nGuildNum)
{
	return Load(pBuff, &nGuildNum, GT_INVALID, GT_INVALID, 
				&CLoongDB::FormatLoadAllGuild, &CLoongDB::ProcResLoadAllGuild);
}

VOID CLoongDB::FormatLoadAllGuild(Beton::MyStream *pStream, DWORD dwAccountID, DWORD dwRoleID)
{
	pStream->SetSelect("guild", 
		"ID,"			"Name,"			"FounderNameID,"	"SpecState,"		"Level,"
		"HoldCity0,"	"HoldCity1,"	"HoldCity2,"		"Fund,"				"Material,"
		"Rep,"			"DailyCost,"	"Peace,"			"Rank,"				"Tenet,"
		"Symbol,"		"GroupPurchase," "LeaderID,"		"RemainSpreadTimes,"	"Commendation");
	pStream->SetWhere();
	pStream->FillString("ID in (select distinct GuildID from guild_member)");
}

VOID CLoongDB::ProcResLoadAllGuild(OUT LPVOID &pData, OUT INT32 *pNum, Beton::QueryResult *pRes)
{
	MTRANS_POINTER(p, pData, tagGuildLoad);

	*pNum = pRes->GetRowCount();

	for(INT32 i=0; i<*pNum; ++i)
	{
		p->dwID				= (*pRes)[0].GetDword();
		p->dwFounderNameID	= (*pRes)[2].GetDword();
		p->dwSpecState		= (*pRes)[3].GetDword();
		p->byLevel			= (*pRes)[4].GetByte();
		p->byHoldCity[0]	= (*pRes)[5].GetByte();
		p->byHoldCity[1]	= (*pRes)[6].GetByte();
		p->byHoldCity[2]	= (*pRes)[7].GetByte();
		p->nFund			= (*pRes)[8].GetInt();
		p->nMaterial		= (*pRes)[9].GetInt();
		p->nReputation		= (*pRes)[10].GetInt();
		p->nDailyCost		= (*pRes)[11].GetInt();
		p->n16Peace			= (INT16)(*pRes)[12].GetInt();
		p->n16Rank			= (INT16)(*pRes)[13].GetInt();
		p->nGroupPurchase	= (*pRes)[16].GetInt();
		p->dwLeaderRoleID	= (*pRes)[17].GetDword();
		p->byAffairRemainTimes = (BYTE)(*pRes)[18].GetDword();
		p->bCommendation	= (*pRes)[19].GetBool();

		(*pRes)[1].GetBlob(p->szName, sizeof(p->szName));
		(*pRes)[14].GetBlob(p->szTenet, sizeof(p->szTenet));
		(*pRes)[15].GetBlob(p->szSymbolURL, sizeof(p->szSymbolURL));

		pRes->NextRow();
		++p;
	}
}

//------------------------------------------------------------------------------
// 创建帮派
//------------------------------------------------------------------------------
BOOL CLoongDB::InsertGuild(tagCreateGuildInfo* pCreateGuildInfo)
{
	return Update(GT_INVALID, pCreateGuildInfo, 1, 0, &CLoongDB::FormatInsertGuild);
}

VOID CLoongDB::FormatInsertGuild(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pData)
{
	MTRANS_POINTER(p, pData, tagCreateGuildInfo);

	// 获取连接
	Beton::Connection* pCon = m_pDBLoong->GetFreeConnection();

	pStream->SetInsert("guild");
	pStream->FillString("ID=") << p->dwGuildID;
	pStream->FillString(",Name='").FillBlob(p->szName, (_tcsclen(p->szName) + 1) * sizeof(TCHAR), pCon);
	pStream->FillString("'");
	pStream->FillString(",FounderNameID=") << p->dwCreateRoleNameID;
	pStream->FillString(",Level=") << p->byGuildLevel;
	pStream->FillString(",Fund=") << p->nGuildFund;
	pStream->FillString(",Material=") << p->nGuildMaterial;
	pStream->FillString(",Rep=") << p->nGuildRep;
	pStream->FillString(",Peace=") << p->n16GuildPeace;
	pStream->FillString(",CreateTime=now()");
	pStream->FillString(",GroupPurchase=") << p->nGroupPurchase;
	pStream->FillString(",LeaderID=") << p->dwCreateRoleNameID;
	pStream->FillString(",RemainSpreadTimes=") << p->byAffairRemainTimes;

	// 释放连接
	m_pDBLoong->ReturnConnection(pCon);
}

//------------------------------------------------------------------------------
// 解散帮派
//------------------------------------------------------------------------------
BOOL CLoongDB::DeleteGuild(DWORD dwGuildID)
{
	return Update(GT_INVALID, &dwGuildID, 1, 0, &CLoongDB::FormatDeleteGuild);
}

VOID CLoongDB::FormatDeleteGuild(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pData)
{
	pStream->SetDelete("guild");
	pStream->SetWhere();
	pStream->FillString("ID=") << *((DWORD*)pData);
}

//------------------------------------------------------------------------------
// 修改帮派宗旨
//------------------------------------------------------------------------------
BOOL CLoongDB::UpdateGuildTenet(DWORD dwGuildID, LPTSTR szNewTenet)
{
	return Update(dwGuildID, szNewTenet, 1, 0, &CLoongDB::FormatUpdateGuildTenet);
}

VOID CLoongDB::FormatUpdateGuildTenet(Beton::MyStream *pStream, DWORD dwGuildID, LPVOID pData)
{
	MTRANS_POINTER(p, pData, TCHAR);
	
	// 获取连接
	Beton::Connection* pCon = m_pDBLoong->GetFreeConnection();
	
	pStream->SetUpdate("guild");
	pStream->FillString("Tenet='").FillBlob(p, (_tcsclen(p) + 1) * sizeof(TCHAR), pCon);
	pStream->FillString("'");

	pStream->SetWhere();
	pStream->FillString("ID=") << dwGuildID;

	// 释放连接
	m_pDBLoong->ReturnConnection(pCon);
}

//------------------------------------------------------------------------------
// 保存所有帮派属性
//------------------------------------------------------------------------------
BOOL CLoongDB::UpdateAllGuild(tagGuildBase *pGuildBase, INT32 nGuildNum)
{
	return	Update(GT_INVALID, pGuildBase, nGuildNum, sizeof(tagGuildBase), &CLoongDB::FormatUpdateAllGuild);
}

VOID CLoongDB::FormatUpdateAllGuild(Beton::MyStream *pStream, DWORD dwGuildID, LPVOID pData)
{
	MTRANS_POINTER(p, pData, tagGuildBase);

	pStream->SetUpdate("guild");

	pStream->FillString("SpecState=") << p->dwSpecState;
	pStream->FillString(",Level=") << p->byLevel;
	pStream->FillString(",HoldCity0=") << p->byHoldCity[0];
	pStream->FillString(",HoldCity1=") << p->byHoldCity[1];
	pStream->FillString(",HoldCity2=") << p->byHoldCity[2];
	pStream->FillString(",Fund=") << p->nFund;
	pStream->FillString(",Material=") << p->nMaterial;
	pStream->FillString(",Rep=") << p->nReputation;
	pStream->FillString(",DailyCost=") << p->nDailyCost;
	pStream->FillString(",Peace=") << p->n16Peace;
	pStream->FillString(",Rank=") << p->n16Rank;
	pStream->FillString(",GroupPurchase=") << p->nGroupPurchase;
	pStream->FillString(",RemainSpreadTimes=") << p->byAffairRemainTimes;
	pStream->FillString(",LeaderID=") << p->dwLeaderRoleID;

	pStream->SetWhere();
	pStream->FillString("ID=") << p->dwID;
}

//------------------------------------------------------------------------------
// 保存指定帮派属性
//------------------------------------------------------------------------------
BOOL CLoongDB::UpdateGuildAtt(tagGuildBase *pGuildBase)
{
	return	Update(GT_INVALID, pGuildBase, 1, sizeof(tagGuildBase), &CLoongDB::FormatUpdateAllGuild);
}

/***************************** 帮派成员属性表相关 *********************************/

//------------------------------------------------------------------------------
// 读入帮派成员
//------------------------------------------------------------------------------
BOOL CLoongDB::LoadAllGuildMember(LPVOID pBuff, INT32& nGuildMemNum)
{
	return Load(pBuff, &nGuildMemNum, GT_INVALID, GT_INVALID, 
				&CLoongDB::FormatLoadAllGuildMember, &CLoongDB::ProcResLoadAllGuildMember);
}

VOID CLoongDB::FormatLoadAllGuildMember(Beton::MyStream *pStream, DWORD dwAccountID, DWORD dwRoleID)
{
	pStream->SetSelect("guild_member", 
				"RoleID,"	"GuildID,"		"GuildPos,"		"TotalContrib,"		"CurContrib,"
				"Exploit,"	"Salary,"		"CanUseGuildWare");
}

VOID CLoongDB::ProcResLoadAllGuildMember(OUT LPVOID &pData, OUT INT32 *pNum, Beton::QueryResult *pRes)
{
	MTRANS_POINTER(p, pData, tagGuildMemLoad);

	*pNum = pRes->GetRowCount();

	for(INT32 i=0; i<*pNum; ++i)
	{
		p->sGuildMember.dwRoleID			= (*pRes)[0].GetDword();
		p->dwGuildID						= (*pRes)[1].GetDword();
		p->sGuildMember.eGuildPos			= (EGuildMemberPos)(*pRes)[2].GetInt();
		p->sGuildMember.nTotalContribution	= (*pRes)[3].GetInt();
		p->sGuildMember.nContribution		= (*pRes)[4].GetInt();
		p->sGuildMember.nExploit			= (*pRes)[5].GetInt();
		p->sGuildMember.nSalary				= (*pRes)[6].GetInt();
		p->sGuildMember.bUseGuildWare		= (*pRes)[7].GetBool();

		pRes->NextRow();
		++p;
	}
}

//------------------------------------------------------------------------------
// 加入帮派成员表
//------------------------------------------------------------------------------
BOOL CLoongDB::InsertGuildMember(const tagCreateGuildMemInfo& sCreateGuildMemInfo)
{
	return Update(GT_INVALID, const_cast<tagCreateGuildMemInfo*>(&sCreateGuildMemInfo), 
					1, 0, &CLoongDB::FormatInsertGuildMember);
}

VOID CLoongDB::FormatInsertGuildMember(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pData)
{
	MTRANS_POINTER(p, pData, tagCreateGuildMemInfo);
	
	pStream->SetInsert("guild_member");
	pStream->FillString("RoleID=") << p->dwRoleID;
	pStream->FillString(",GuildID=") << p->dwGuildID;
	pStream->FillString(",GuildPos=") << p->n8GuildPos;
	pStream->FillString(",JoinTime=now()");
}

//------------------------------------------------------------------------------
// 从帮派成员表中删除
//------------------------------------------------------------------------------
BOOL CLoongDB::DeleteGuildMember(DWORD dwRoleID)
{
	return Update(dwRoleID, NULL, 1, 0, &CLoongDB::FormatDeleteGuildMember);
}

VOID CLoongDB::FormatDeleteGuildMember(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pData)
{
	pStream->SetDelete("guild_member");

	pStream->SetWhere();
	pStream->FillString("RoleID=") << dwRoleID;
}

//------------------------------------------------------------------------------
// 从帮派成员表中删除
//------------------------------------------------------------------------------
BOOL CLoongDB::UpdateGuildMemberPos(DWORD dwRoleID, INT8 n8NewPos)
{
	return Update(dwRoleID, &n8NewPos, 1, 0, &CLoongDB::FormatUpdateGuildMemberPos);
}

VOID CLoongDB::FormatUpdateGuildMemberPos(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pData)
{
	pStream->SetUpdate("guild_member");
	pStream->FillString("GuildPos=") << *(INT8*)pData;

	pStream->SetWhere();
	pStream->FillString("RoleID=") << dwRoleID;
}

//------------------------------------------------------------------------------
// 帮派仓库访问权限变更
//------------------------------------------------------------------------------
BOOL CLoongDB::GuildWarePrivilege(DWORD dwRoleID, BOOL bEnable)
{
	return Update(dwRoleID, &bEnable, 1, 0, &CLoongDB::FormatGuildWarePrivilege);
}

VOID CLoongDB::FormatGuildWarePrivilege(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pData)
{
	pStream->SetUpdate("guild_member");
	pStream->FillString("CanUseGuildWare=") << *(bool*)pData;

	pStream->SetWhere();
	pStream->FillString("RoleID=") << dwRoleID;
}

//------------------------------------------------------------------------------
// 帮派仓库物品读取
//------------------------------------------------------------------------------
BOOL CLoongDB::LoadGuildWareItems(LPVOID &pBuff, DWORD dwGuildID, INT32& nItemNum)
{
	return Load(pBuff, &nItemNum, dwGuildID, GT_INVALID, 
		&CLoongDB::FormatLoadGuildWareItems, &CLoongDB::ProcResLoadItem);
}

VOID CLoongDB::FormatLoadGuildWareItems(Beton::MyStream *pStream, DWORD dwGuildID, DWORD dwRoleID)
{
	pStream->Clear();
	pStream->FillString("select ");
	InitItemLoad(pStream);
	pStream->FillString(" from item left join equip using(SerialNum)");

	pStream->SetWhere();
	pStream->FillString("AccountID=") << dwGuildID;
	pStream->FillString(" and ContainerTypeID=") << EICT_GuildWare;
}

//------------------------------------------------------------------------------
// 帮派设施升级信息读取
//------------------------------------------------------------------------------
BOOL CLoongDB::LoadGuildFacilities(LPVOID pBuff, INT32& nInfoNum, DWORD dwGuildID)
{
	return Load(pBuff, &nInfoNum, dwGuildID, GT_INVALID, &CLoongDB::FormatLoadGuildFacilities, 
		&CLoongDB::ProcResLoadGuildFacilities);
}

VOID CLoongDB::FormatLoadGuildFacilities(Beton::MyStream *pStream, DWORD dwGuildID, DWORD dwRoleID)
{
	pStream->Clear();
	pStream->SetSelect("guild_upgrade",	"guild_id,"		"type,"	"level,"	"progress,"
										"item_type_1,"	"item_neednum_1,"	
										"item_type_2,"	"item_neednum_2,"
										"item_type_3,"	"item_neednum_3,"
										"item_type_4,"	"item_neednum_4");
	pStream->SetWhere();
	pStream->FillString("guild_id=") << dwGuildID;
}

VOID CLoongDB::ProcResLoadGuildFacilities(OUT LPVOID &pData, OUT INT32 *pNum, Beton::QueryResult *pRes)
{
	ASSERT(pNum != NULL);

	*pNum = pRes->GetRowCount();

	for(INT32 i=0; i<*pNum; ++i)
	{
		MTRANS_POINTER(pInfo, pData, tagFacilitiesLoad);

		pInfo->dwGuildID	= (*pRes)[0].GetDword();
		pInfo->eType		= (EFacilitiesType)(*pRes)[1].GetInt();
		pInfo->byLevel		= (*pRes)[2].GetByte();
		pInfo->n16Progress	= (*pRes)[3].GetShort();

		for (int n=0; n<MAX_UPGRADE_NEED_ITEM_TYPE; n++)
		{
			pInfo->dwItemTypeID[n]	= (*pRes)[n*2+4].GetDword();
			pInfo->nItemNeed[n]		= (*pRes)[n*2+5].GetInt();
		}

		pData = &pInfo[1];

		pRes->NextRow();
	}
}

//------------------------------------------------------------------------------
// 插入帮派设施升级信息
//------------------------------------------------------------------------------
BOOL CLoongDB::InsertGuildFacilities(tagFacilitiesLoad* pInfo)
{
	return Update(GT_INVALID, pInfo, 1, 0, &CLoongDB::FormatInsertGuildFacilities);
}

VOID CLoongDB::FormatInsertGuildFacilities(Beton::MyStream *pStream, DWORD dwGuildID, LPVOID pData)
{
	MTRANS_POINTER(pInfo, pData, tagFacilitiesLoad);

	pStream->Clear();
	pStream->SetInsert("guild_upgrade");
	pStream->FillString("guild_id=") << pInfo->dwGuildID;	
	pStream->FillString(",type=") << pInfo->eType;
	pStream->FillString(",level=") << pInfo->byLevel;
	pStream->FillString(",progress=") << pInfo->n16Progress;
	for (int n=1; n<=MAX_UPGRADE_NEED_ITEM_TYPE; n++)
	{
		pStream->FillString(",item_type_") << n;
		pStream->FillString("=") << pInfo->dwItemTypeID[n-1];
		pStream->FillString(",item_neednum_") << n;
		pStream->FillString("=") << pInfo->nItemNeed[n-1];
	}
}

//------------------------------------------------------------------------------
// 帮派设施升级信息更新
//------------------------------------------------------------------------------
BOOL CLoongDB::UpdateGuildFacilities(tagFacilitiesLoad* pInfo)
{
	return Update(GT_INVALID, pInfo, 1, 0, &CLoongDB::FormatUpdateGuildFacilities);
}

VOID CLoongDB::FormatUpdateGuildFacilities(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pData)
{
	MTRANS_POINTER(pInfo, pData, tagFacilitiesLoad);

	pStream->Clear();
	pStream->SetUpdate("guild_upgrade");
	pStream->FillString("level=") << pInfo->byLevel;
	pStream->FillString(",progress=") << pInfo->n16Progress;
	for (int n=1; n<=MAX_UPGRADE_NEED_ITEM_TYPE; n++)
	{
		pStream->FillString(",item_type_") << n;
		pStream->FillString("=") << pInfo->dwItemTypeID[n-1];
		pStream->FillString(",item_neednum_") << n;
		pStream->FillString("=") << pInfo->nItemNeed[n-1];
	}

	pStream->SetWhere();
	pStream->FillString("guild_id=") << pInfo->dwGuildID;
	pStream->FillString(" and type=") << pInfo->eType;
}

//------------------------------------------------------------------------------
// 删除所有帮派设施信息
//------------------------------------------------------------------------------
BOOL CLoongDB::RemoveAllFacilities(DWORD dwGuildID)
{
	return Update(dwGuildID, NULL, 1, 0, &CLoongDB::FormatRemoveAllFacilities);
}

VOID CLoongDB::FormatRemoveAllFacilities(Beton::MyStream *pStream, DWORD dwGuildID, LPVOID pData)
{
	pStream->Clear();
	pStream->SetDelete("guild_upgrade");
	pStream->SetWhere();
	pStream->FillString("guild_id=") << dwGuildID;
}

//------------------------------------------------------------------------------
// 更新帮派成员贡献
//------------------------------------------------------------------------------
BOOL CLoongDB::ChangeMemberContribution(LPVOID pData)
{
	return Update(GT_INVALID, pData, 1, 0, &CLoongDB::FormatChangeMemberContribution);
}

VOID CLoongDB::FormatChangeMemberContribution(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pData)
{
	MTRANS_POINTER(pMsg, pData, tagNDBC_ChangeContrib);

	pStream->Clear();
	pStream->SetUpdate("guild_member");
	pStream->FillString("CurContrib=") << pMsg->nContribution;
	pStream->FillString(",TotalContrib=") << pMsg->nTotalContribution;

	pStream->SetWhere();
	pStream->FillString("RoleID=") << pMsg->dwRoleID;
}

//------------------------------------------------------------------------------
// 载入帮派技能信息
//------------------------------------------------------------------------------
BOOL CLoongDB::LoadGuildSkillInfo(LPVOID pData, DWORD dwGuildID, INT& nNum)
{
	return Load(pData, &nNum, dwGuildID, NULL, &CLoongDB::FormatLoadGuildSkillInfo, &CLoongDB::ProcResLoadGuildSkillInfo);
}

VOID CLoongDB::FormatLoadGuildSkillInfo(Beton::MyStream *pStream, DWORD dwGuildID, DWORD dwRoleID)
{
	pStream->Clear();
	pStream->SetSelect("guild_skill", "skill_id," "progress," "level," "researching");
	pStream->SetWhere();
	pStream->FillString("guild_id=") << dwGuildID;
}

VOID CLoongDB::ProcResLoadGuildSkillInfo(OUT LPVOID &pData, OUT INT32 *pNum, Beton::QueryResult *pRes)
{
	ASSERT(pNum != NULL);

	*pNum = pRes->GetRowCount();
	DWORD dwResearchID = GT_INVALID;

	for(INT32 i=0; i<*pNum; ++i)
	{
		MTRANS_POINTER(pInfo, pData, tagGuildSkillInfo);

		pInfo->dwSkillID	= (*pRes)[0].GetDword();
		pInfo->n16Progress	= (INT16)((*pRes)[1].GetInt());
		pInfo->nLevel		= (*pRes)[2].GetInt();
		pInfo->bResearching	= (*pRes)[3].GetBool();

		pData = &pInfo[1];

		pRes->NextRow();
	}
}
//------------------------------------------------------------------------------
// 创建帮派技能信息
//------------------------------------------------------------------------------
BOOL CLoongDB::InsertGuildSkillInfo(DWORD dwGuildID, tagGuildSkillInfo* pSkillInfo)
{
	return Update(dwGuildID, (LPVOID)pSkillInfo, 1, 0, &CLoongDB::FormatInsertGuildSkillInfo);
}

VOID CLoongDB::FormatInsertGuildSkillInfo(Beton::MyStream *pStream, DWORD dwGuildID, LPVOID pData)
{
	MTRANS_POINTER(pSkillInfo, pData, tagGuildSkillInfo);

	pStream->Clear();

	pStream->SetInsert("guild_skill");
	pStream->FillString("guild_id=") << dwGuildID;
	pStream->FillString(",skill_id=") << pSkillInfo->dwSkillID;
	pStream->FillString(",progress=") << pSkillInfo->n16Progress;
	pStream->FillString(",level=") << pSkillInfo->nLevel;
	pStream->FillString(",researching=") << pSkillInfo->bResearching;
}

//------------------------------------------------------------------------------
// 更新帮派技能信息
//------------------------------------------------------------------------------
BOOL CLoongDB::UpdateGuildSkillInfo(DWORD dwGuildID, tagGuildSkillInfo* pSkillInfo)
{
	return Update(dwGuildID, (LPVOID)pSkillInfo, 1, 0, &CLoongDB::FormatUpdateGuildSkillInfo);
}

VOID CLoongDB::FormatUpdateGuildSkillInfo(Beton::MyStream *pStream, DWORD dwGuildID, LPVOID pData)
{
	MTRANS_POINTER(pSkillInfo, pData, tagGuildSkillInfo);

	pStream->Clear();
	
	pStream->SetUpdate("guild_skill");
	pStream->FillString("progress=") << pSkillInfo->n16Progress;
	pStream->FillString(",level=") << pSkillInfo->nLevel;
	pStream->SetWhere();
	pStream->FillString("guild_id=") << dwGuildID;
	pStream->FillString(" and skill_id=") << pSkillInfo->dwSkillID;
}

//------------------------------------------------------------------------------
// 设置当前研究技能
//------------------------------------------------------------------------------
BOOL CLoongDB::ChangeResearchGuildSkill(DWORD dwGuildID, DWORD dwSkillID)
{
	BOOL bRet = Update(dwGuildID, NULL, 1, 0, &CLoongDB::FormatClearResearchGuildSkill);

	if (bRet && GT_VALID(dwSkillID))
	{
		bRet = Update(dwGuildID, (LPVOID)&dwSkillID, 1, 0, &CLoongDB::FormatChangeResearchGuildSkill);
	}

	return bRet;
}

VOID CLoongDB::FormatChangeResearchGuildSkill(Beton::MyStream *pStream, DWORD dwGuildID, LPVOID pSkillID)
{
	pStream->Clear();

	pStream->SetUpdate("guild_skill");
	pStream->FillString("researching=") << TRUE;
	pStream->SetWhere();
	pStream->FillString("guild_id=") << dwGuildID;
	pStream->FillString(" and skill_id=") << *(DWORD*)pSkillID;
}

VOID CLoongDB::FormatClearResearchGuildSkill(Beton::MyStream *pStream, DWORD dwGuildID, LPVOID pSkillID)
{
	pStream->Clear();

	pStream->SetUpdate("guild_skill");
	pStream->FillString("researching=") << FALSE;
	pStream->SetWhere();
	pStream->FillString("guild_id=") << dwGuildID;
}

//------------------------------------------------------------------------------
// 清除指定帮派的技能信息
//------------------------------------------------------------------------------
BOOL CLoongDB::RemoveGuildSkillInfo(DWORD dwGuildID)
{
	return Update(dwGuildID, NULL, 1, 0, &CLoongDB::FormatRemoveGuildSkillInfo);
}

VOID CLoongDB::FormatRemoveGuildSkillInfo(Beton::MyStream *pStream, DWORD dwGuildID, LPVOID pSkillID)
{
	pStream->Clear();

	pStream->SetDelete("guild_skill");
	pStream->SetWhere();
	pStream->FillString("guild_id=") << dwGuildID;
}

//------------------------------------------------------------------------------
// 读取跑商容器信息
//------------------------------------------------------------------------------
BOOL CLoongDB::LoadGuildCommerceInfo(LPVOID pData, DWORD dwGuildID, INT& nNum)
{
	return Load(pData, &nNum, dwGuildID, NULL, &CLoongDB::FormatLoadGuildCommerceInfo, &CLoongDB::ProcResLoadGuildCommerceInfo);
}

VOID CLoongDB::FormatLoadGuildCommerceInfo(Beton::MyStream *pStream, DWORD dwGuildID, DWORD dwRoleID)
{
	pStream->Clear();
	pStream->SetSelect("guild_commodity", "role_id," "guild_id," "role_level," "tael," "goods");
	pStream->SetWhere();
	pStream->FillString("guild_id=") << dwGuildID;
}

VOID CLoongDB::ProcResLoadGuildCommerceInfo(OUT LPVOID &pData, OUT INT32 *pNum, Beton::QueryResult *pRes)
{
	ASSERT(pNum != NULL);

	*pNum = pRes->GetRowCount();

	for(INT32 i=0; i<*pNum; ++i)
	{
		MTRANS_POINTER(pInfo, pData, tagGuildCommerceInfo);

		pInfo->dwRoleID		= (*pRes)[0].GetDword();
		pInfo->nLevel		= (*pRes)[2].GetInt();
		pInfo->nTael		= (INT32)(*pRes)[3].GetInt();
		(*pRes)[4].GetBlob(pInfo->sGoodInfo, (*pRes)[4].GetLen());
		
		pData = &pInfo[1];

		pRes->NextRow();
	}
}

//------------------------------------------------------------------------------
// 读取跑商排名信息
//------------------------------------------------------------------------------
BOOL CLoongDB::LoadCommerceRankInfo(LPVOID pData, DWORD dwGuildID, INT& nNum)
{
	return Load(pData, &nNum, dwGuildID, NULL, &CLoongDB::FormatLoadCommerceRankInfo, &CLoongDB::ProcResLoadCommerceRankInfo);
}

VOID CLoongDB::FormatLoadCommerceRankInfo(Beton::MyStream *pStream, DWORD dwGuildID, DWORD dwRoleID)
{
	pStream->Clear();
	pStream->SetSelect("commerce_rank", "role_id," "guild_id," "times," "tael");
	pStream->SetWhere();
	pStream->FillString("guild_id=") << dwGuildID;
}

VOID CLoongDB::ProcResLoadCommerceRankInfo(OUT LPVOID &pData, OUT INT32 *pNum, Beton::QueryResult *pRes)
{
	ASSERT(pNum != NULL);

	*pNum = pRes->GetRowCount();

	for(INT32 i=0; i<*pNum; ++i)
	{
		MTRANS_POINTER(pInfo, pData, tagCommerceRank);

		pInfo->dwRoleID		= (*pRes)[0].GetDword();
		pInfo->nTimes		= (*pRes)[2].GetInt();
		pInfo->nTael		= (INT32)(*pRes)[3].GetInt();

		pData = &pInfo[1];

		pRes->NextRow();
	}
}

//------------------------------------------------------------------------------
// 新建跑商容器
//------------------------------------------------------------------------------
BOOL CLoongDB::InsertCommodityInfo(LPVOID pData)
{
	return Update(GT_INVALID, pData, 1, 0, &CLoongDB::FormatInsertCommodityInfo);
}

VOID CLoongDB::FormatInsertCommodityInfo(Beton::MyStream *pStream, DWORD dwGuildID, LPVOID pData)
{
	MTRANS_POINTER(pCommodityInfo, pData, tagNDBC_CreateCommodity);

	pStream->Clear();
	pStream->SetInsert("guild_commodity");
	pStream->FillString("role_id=") << pCommodityInfo->dwRoleID;
	pStream->FillString(",guild_id=") << pCommodityInfo->dwGuildID;
	pStream->FillString(",role_level=") << pCommodityInfo->nLevel;
	pStream->FillString(",tael=") << pCommodityInfo->nTael;
}

//------------------------------------------------------------------------------
// 更新商银
//------------------------------------------------------------------------------
BOOL CLoongDB::UpdateCommodityTael(DWORD dwRoleID, INT32 nTael)
{
	return Update(dwRoleID, (LPVOID)&nTael, 1, 0, &CLoongDB::FormatUpdateCommodityTael);
}

VOID CLoongDB::FormatUpdateCommodityTael(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID nTael)
{
	pStream->Clear();

	pStream->SetUpdate("guild_commodity");
	pStream->FillString("tael=") << *(INT32*)nTael;
	pStream->SetWhere();
	pStream->FillString("role_id=") << dwRoleID;
}

//------------------------------------------------------------------------------
// 更新商货
//------------------------------------------------------------------------------
BOOL CLoongDB::SaveCommodityInfo(LPVOID pData, DWORD dwRoleID)
{
	return Update(dwRoleID, pData, 1, 0, &CLoongDB::FormatSaveCommodityInfo);
}

VOID CLoongDB::FormatSaveCommodityInfo(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pData)
{
	// 获取连接
	Beton::Connection* pCon = m_pDBLoong->GetFreeConnection();

	pStream->Clear();
	pStream->SetUpdate("guild_commodity");
	
	pStream->FillString("goods='");
	pStream->FillBlob(pData, sizeof(tagCommerceGoodInfo)*MAX_COMMODITY_CAPACITY, pCon);
	pStream->FillString("'");

	pStream->SetWhere();
	pStream->FillString("role_id=") << dwRoleID;

	// 释放连接
	m_pDBLoong->ReturnConnection(pCon);
}

//------------------------------------------------------------------------------
// 删除商货容器信息
//------------------------------------------------------------------------------
BOOL CLoongDB::RemoveCommodityInfo(DWORD dwRoleID)
{
	return Update(dwRoleID, NULL, 1, 0, &CLoongDB::FormatRemoveCommodityInfo);
}

VOID CLoongDB::FormatRemoveCommodityInfo(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pData)
{
	pStream->SetDelete("guild_commodity");
	pStream->SetWhere();
	pStream->FillString("role_id=") << dwRoleID;
}

//------------------------------------------------------------------------------
// 设置跑商嘉奖状态
//------------------------------------------------------------------------------
BOOL CLoongDB::SetCommendation(DWORD dwGuildID, BOOL bCommend)
{
	return Update(dwGuildID, (LPVOID)&bCommend, 1, 0, &CLoongDB::FormatSetCommendation);
}

VOID CLoongDB::FormatSetCommendation(Beton::MyStream *pStream, DWORD dwGuildID, LPVOID pData)
{
	pStream->Clear();
	pStream->SetUpdate("guild");
	pStream->FillString("Commendation=") << *(BOOL*)pData;
	pStream->SetWhere();
	pStream->FillString("ID=") << dwGuildID;
}

//------------------------------------------------------------------------------
// 创建或更新跑商排名信息
//------------------------------------------------------------------------------
BOOL CLoongDB::UpdateCommerceRankInfo(DWORD dwGuildID, LPVOID pData)
{
	return Update(dwGuildID, pData, 1, 0, &CLoongDB::FormatUpdateCommerceRankInfo);
}

VOID CLoongDB::FormatUpdateCommerceRankInfo(Beton::MyStream *pStream, DWORD dwGuildID, LPVOID pData)
{
	MTRANS_POINTER(pRankInfo, pData, tagCommerceRank);

	pStream->Clear();
	pStream->SetReplace("commerce_rank");
	pStream->FillString("role_id=") << pRankInfo->dwRoleID;
	pStream->FillString(",guild_id=") << dwGuildID;
	pStream->FillString(",times=") << pRankInfo->nTimes;
	pStream->FillString(",tael=") << pRankInfo->nTael;
}

//------------------------------------------------------------------------------
// 删除跑商排名信息
//------------------------------------------------------------------------------
BOOL CLoongDB::RemoveCommerceRankInfo(DWORD dwGuildID, DWORD dwRoleID)
{
	return Update(dwGuildID, (LPVOID)&dwRoleID, 1, 0, &CLoongDB::FormatRemoveCommerceRankInfo);
}

VOID CLoongDB::FormatRemoveCommerceRankInfo(Beton::MyStream *pStream, DWORD dwGuildID, LPVOID pData)
{
	pStream->Clear();
	pStream->SetDelete("commerce_rank");
	pStream->SetWhere();
	pStream->FillString("guild_id=") << dwGuildID;

	DWORD dwRoleID = *(DWORD*)pData;
	if (GT_VALID(dwRoleID))
	{
		pStream->FillString(" and role_id=") << dwRoleID;
	}
}