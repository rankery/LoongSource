//-----------------------------------------------------------------------------
//!\file Loong_famehall.cpp
//!\author xlguo
//!
//!\date 2009-05-08
//! last 
//!
//!\brief 宠物数据库处理
//!
//!	Copyright (c) 2004 TENGWU Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "Loong.h"
#include "LoongDB.h"
#include "../ServerDefine/pet_define.h"
#include "../ServerDefine/msg_pet.h"

//-------------------------------------------------------------------------------------------------------
// 创建宠物
//-------------------------------------------------------------------------------------------------------
DWORD CLoong::CreatePet(DWORD pDBMsg, DWORD dwDummy)
{
	MGET_MSG(pRecv, pDBMsg, NDBC_CreatePetSoul);

	DWORD dwPetID = GT_INVALID;

	// 创建宠物
	BOOL bRtv = m_pDB->InsertPetSoul(&pRecv->createData, dwPetID);

	LPVOID pBuffer = g_MemPoolSafe.Alloc(1024*10);
	ZeroMemory(pBuffer, 1024*10);
	tagNDBS_CreatePetSoul* pSend = (tagNDBS_CreatePetSoul*)pBuffer;
	pSend->dwID = m_pUtil->Crc32("NDBS_CreatePetSoul");
	pSend->dwErrCode = E_FAIL;
	INT nNum = 0;

	if (bRtv)
	{
		ASSERT(IS_PET(dwPetID));
		// 加载宠物
		PVOID pLoad = (PVOID)&pSend->petDataLoad;
		if (m_pDB->LoadAPet(pLoad, dwPetID))
		{
			pSend->dwErrCode = E_Success;
		}
		
	}

	// 重新计算大小
	pSend->dwSize = sizeof(tagNDBS_CreatePetSoul) - sizeof(tagDBPet) + pSend->petDataLoad.GetSize();
	pSend->n64Serial = pRecv->n64Serial;

	// 反馈给游戏服务器
	SendMsg( pSend, pSend->dwSize);

	g_MemPoolSafe.Free(pBuffer);
	return E_Success;
}

//-------------------------------------------------------------------------------------------------------
// 获取宠物信息
//-------------------------------------------------------------------------------------------------------
DWORD CLoong::GetPetInfo(DWORD pDBMsg, DWORD dwDummy)
{
	tagNDBS_GetPetInfo send;
	INT nPetNum = 0;
	m_pDB->GetPetCountAndMaxPetID(nPetNum, send.dwMaxPetID);
	SendMsg(&send, send.dwSize);
	return E_Success;
}

BOOL CLoongDB::InsertPetSoul( const tagDBPetSoulCreate* pCreateData, DWORD& dwPetID)
{
	BOOL bRtv = Update(GT_INVALID, (LPVOID)pCreateData, 1, sizeof(tagDBPetSoulCreate), (&CLoongDB::FormatInsertPetSoul));
	if (bRtv)
	{
		dwPetID = m_dwMaxPetID + 1;
		++m_dwMaxPetID;
		++m_nPetNum;
	}
	return bRtv;
}

VOID CLoongDB::FormatInsertPetSoul( Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pData )
{
	MTRANS_POINTER(p, pData, tagDBPetSoulCreate);

	Beton::Connection* pCon = m_pDBLoong->GetFreeConnection();

	pStream->SetInsert("pet_data");
	pStream->FillString("pet_id=") << m_dwMaxPetID + 1;
	pStream->FillString(",pet_name='").FillString(p->tszName, pCon).FillString("'");
	pStream->FillString(",master_id=") << p->dwMasterID;
	pStream->FillString(",type_id=") << p->dwProtoID;
	pStream->FillString(",quality=") << p->nQuality;
	pStream->FillString(",aptitude=") << p->nAptitude;
	pStream->FillString(",potential=") << p->nPotential;
	pStream->FillString(",cur_spirit=") << p->nSpirte;

	m_pDBLoong->ReturnConnection(pCon);
}

BOOL CLoongDB::DeletePetSoul(DWORD dwPetID)
{
	return Update(dwPetID, NULL, 1, 0, &CLoongDB::FormatDeletePetSoul);
}

VOID CLoongDB::FormatDeletePetSoul(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pData)
{
	pStream->SetDelete("pet_data");
	pStream->SetWhere();
	pStream->FillString("pet_id=") << dwRoleID;
}

BOOL CLoongDB::DelPetEquip(DWORD dwPetID)
{
	return DelRoleItem(dwPetID);
}
//-------------------------------------------------------------------------------------------------------
// 加载宠物
//-------------------------------------------------------------------------------------------------------
BOOL CLoongDB::LoadPets(OUT PVOID& pData, DWORD dwMasterID, INT32& nNum)
{
	// 读取角色所有宠物id
	DWORD dwPets[MAX_PETSOUL_NUM];
	memset(dwPets, GT_INVALID, sizeof(dwPets));
	if (!LoadRolePetIDs(dwPets, dwMasterID, nNum) || 0 == nNum)
	{
		return TRUE;
	}
	
	// 分别加载宠物
	for (INT i=0; i<nNum; ++i)
	{
		MTRANS_POINTER(pPetLoad, pData, tagDBPet);
		if (!LoadAPet(pData, dwPets[i]))
		{
			nNum = i;
			break;
		}
	}

	return TRUE;
}

//-------------------------------------------------------------------------------------------------------
// 加载宠物属性
//-------------------------------------------------------------------------------------------------------
BOOL CLoongDB::LoadPetSoul(LPVOID pData, DWORD dwPetID)
{
	INT nNum = 0;
	BOOL bRtv = Load(pData, &nNum, dwPetID, GT_INVALID, &CLoongDB::FormatLoadPetSoul, &CLoongDB::ProcResLoadPetSoul);
	ASSERT(nNum);
	return bRtv;
}

VOID CLoongDB::FormatLoadPetSoul( Beton::MyStream *pStream, DWORD dwPetID, DWORD dwMasterID )
{
	//								0		1			2			3
	pStream->SetSelect("pet_data", "pet_id,	pet_name,	master_id,	type_id,"
	//								4			5			6			7
									"quality,	aptitude,	potential,	cur_spirit,"
	//								8			9		10		11
									"cur_exp,	step,	grade,	talent_count,	"
	//								12				13			14
									"wuxing_energy,	pet_state,	pet_lock");
	pStream->SetWhere();

	ASSERT(!GT_VALID(dwMasterID));
	
	pStream->FillString("pet_id=") << dwPetID;
}

VOID CLoongDB::ProcResLoadPetSoul( OUT LPVOID &pData, OUT INT32 *pNum, Beton::QueryResult *pRes )
{
	MTRANS_POINTER(pPetAtt, pData, tagDBPetAtt);

	*pNum = pRes->GetRowCount();

	ASSERT(1 == *pNum);
		
	pPetAtt->dwPetID	= (*pRes)[0].GetDword();
	(*pRes)[1].GetTCHAR(pPetAtt->tszName, X_SHORT_NAME);
	
	pPetAtt->dwMasterID	= (*pRes)[2].GetDword();				// 主人ID
	pPetAtt->dwProtoID	= (*pRes)[3].GetDword();				// 原型ID
	pPetAtt->nQuality	= (*pRes)[4].GetInt();
	pPetAtt->nAptitude	= (*pRes)[5].GetInt();
	pPetAtt->nPotential	= (*pRes)[6].GetInt();
	pPetAtt->nSpirit	= (*pRes)[7].GetInt();
	pPetAtt->nExpCur	= (*pRes)[8].GetInt();
	pPetAtt->nStep		= (*pRes)[9].GetInt();
	pPetAtt->nGrade		= (*pRes)[10].GetInt();
	pPetAtt->nTalentCount= (*pRes)[11].GetInt();
	pPetAtt->nWuXingEnergy= (*pRes)[12].GetInt();
	pPetAtt->byPetState	= (*pRes)[13].GetInt();
	pPetAtt->bLocked	= (*pRes)[14].GetBool();
}

//-------------------------------------------------------------------------------------------------------
// 删除宠物
//-------------------------------------------------------------------------------------------------------
DWORD CLoong::DeletePet(DWORD pDBMsg, DWORD dwDummy)
{
	MTRANS_POINTER(pRecv, pDBMsg, tagNDBC_DelPetSoul);
	for (INT i=0; i<pRecv->nPetSoulNum; ++i)
	{
		DWORD dwPetID = pRecv->dwPetID[i];
		if (IS_PET(dwPetID))
		{
			// tbd:删除宠物技能
			m_pDB->DelPetSkill(GT_INVALID, dwPetID);
			// 删除宠物装备
			m_pDB->DelPetEquip(dwPetID);
			// 删除宠物属性
			m_pDB->DeletePetSoul(dwPetID);
		}
	}
	return E_Success;
}

DWORD CLoong::UpdatePet(DWORD pDBMsg, DWORD dwDummy)
{
	MTRANS_POINTER(pRecv, pDBMsg, tagNDBC_UpdatePetSoul);

	return m_pDB->UpdatePetAtt(&pRecv->petAtt, pRecv->petAtt.dwPetID);
}

//-------------------------------------------------------------------------------------------------------
// 读取玩家所有宠物ID
//-------------------------------------------------------------------------------------------------------
BOOL CLoongDB::LoadRolePetIDs( DWORD dwPetIDs[], DWORD dwMasterID, INT32& nNum )
{
	PVOID pData = (PVOID)dwPetIDs;
	return Load(pData, &nNum,GT_INVALID, dwMasterID, &CLoongDB::FormatLoadPetIDs, &CLoongDB::ProcResLoadPetIDs);
}

VOID CLoongDB::FormatLoadPetIDs( Beton::MyStream *pStream, DWORD dwPetID, DWORD dwMasterID )
{
	pStream->SetSelect("pet_data", "pet_id");
	pStream->SetWhere();
	pStream->FillString("master_id=") << dwMasterID;
	pStream->FillString(" order by pet_id desc limit ") << MAX_PETSOUL_NUM;
}

VOID CLoongDB::ProcResLoadPetIDs( OUT LPVOID &pData, OUT INT32 *pNum, Beton::QueryResult *pRes )
{
	MTRANS_POINTER(pPetIDs, pData, DWORD);

	*pNum = pRes->GetRowCount();

	for(INT32 i=0; i<*pNum; ++i)
	{
		pPetIDs[i] = (*pRes)[0].GetDword();
		pRes->NextRow();
	}

	pData = (LPVOID)(pPetIDs + *pNum);
}

//-------------------------------------------------------------------------------------------------------
// 读取宠物技能
//-------------------------------------------------------------------------------------------------------
BOOL CLoongDB::LoadPetSkill(OUT LPVOID& pData, DWORD dwPetID, INT32& nNum)
{
	return Load(pData, &nNum, GT_INVALID, dwPetID, &CLoongDB::FormatLoadPetSkill, &CLoongDB::ProcResLoadPetSkill);
}

VOID CLoongDB::FormatLoadPetSkill( Beton::MyStream *pStream, DWORD dwMasterID, DWORD dwPetID )
{
	pStream->SetSelect("pet_skill", "petskill_typeid,para1,para2");
	pStream->SetWhere();
	pStream->FillString("petid=")<<dwPetID;
}

VOID CLoongDB::ProcResLoadPetSkill( OUT LPVOID &pData, OUT INT32 *pNum, Beton::QueryResult *pRes )
{
	MTRANS_POINTER(pPetSkills, pData, tagDBPetSkill);
	*pNum = pRes->GetRowCount();

	for (INT i=0; i<*pNum; ++i)
	{
		pPetSkills[i].dwTypeID		= (*pRes)[0].GetDword();
		pPetSkills[i].nPara1		= (*pRes)[1].GetDword();
		pPetSkills[i].nPara2		= (*pRes)[2].GetDword();
		pRes->NextRow();
	}
	pData = &pPetSkills[*pNum];
}

BOOL CLoongDB::DelPetSkill(DWORD dwPetSkillID, DWORD dwPetID)
{
	return Update(dwPetID, &dwPetSkillID, 1, sizeof(DWORD), &CLoongDB::FormatDelPetSkill);
}

VOID CLoongDB::FormatDelPetSkill( Beton::MyStream *pStream, DWORD dwPetID, LPVOID pData )
{
	DWORD dwPetSkillID = *((DWORD*)pData);
	pStream->SetDelete("pet_skill");
	pStream->SetWhere();
	if (P_VALID(dwPetSkillID))
	{
		pStream->FillString("petskill_typeid=")<<dwPetSkillID;
		pStream->FillString(" and ");
	}
	
	pStream->FillString("petid=")<<dwPetID;
}

BOOL CLoongDB::InsertPetSkill( PVOID pData, DWORD dwPetID )
{
	return Update(dwPetID, pData, 1, sizeof(tagDBPetSkill), &CLoongDB::FormatInsertPetSkill);
}

VOID CLoongDB::FormatInsertPetSkill( Beton::MyStream *pStream, DWORD dwPetID, LPVOID pData )
{
	MTRANS_POINTER(pSkillData, pData, tagDBPetSkill);

	pStream->SetInsert("pet_skill");
	pStream->FillString("petid=") << dwPetID;
	pStream->FillString(",petskill_typeid=") << pSkillData->dwTypeID;
	pStream->FillString(",para1=") << (DWORD)pSkillData->nPara1;
	pStream->FillString(",para2=") << (DWORD)pSkillData->nPara2;

}

BOOL CLoongDB::UpdatePetSkill( PVOID &pData, const INT nNum, DWORD dwPetID )
{
	return Update(dwPetID, pData, nNum, sizeof(tagDBPetSkill), &CLoongDB::FormatUpdatePetSkill, &pData);
}

VOID CLoongDB::FormatUpdatePetSkill( Beton::MyStream *pStream, DWORD dwPetID, LPVOID pData )
{
	MTRANS_POINTER(pSkillData, pData, tagDBPetSkill);

	pStream->SetUpdate("pet_skill");
	pStream->FillString("para1=") << (DWORD)pSkillData->nPara1;
	pStream->FillString(",para2=") << (DWORD)pSkillData->nPara2;

	pStream->SetWhere();
	pStream->FillString("petid=") << dwPetID;
	pStream->FillString(" and petskill_typeid=") << pSkillData->dwTypeID;

}

//-------------------------------------------------------------------------------------------------------
// 读取宠物装备
//-------------------------------------------------------------------------------------------------------
BOOL CLoongDB::LoadPetEquip( OUT LPVOID& pData, DWORD dwPetID, INT32& nNum )
{
	return Load(pData, &nNum, GT_INVALID, dwPetID, &CLoongDB::FormatLoadPetEquip, &CLoongDB::ProcResLoadItem);
}

VOID CLoongDB::FormatLoadPetEquip( Beton::MyStream *pStream, DWORD dwAccountID, DWORD dwPetID)
{
	pStream->Clear();
	pStream->FillString("select ");
	InitItemLoad(pStream);
	pStream->FillString(" from item left join equip using(SerialNum)");

	pStream->SetWhere();
	pStream->FillString("OwnerID=") << dwPetID;
}

//-------------------------------------------------------------------------------------------------------
// 读取一只宠物
//-------------------------------------------------------------------------------------------------------
BOOL CLoongDB::LoadAPet(OUT PVOID &pData, DWORD dwPetID )
{
	MTRANS_POINTER(pPetLoad, pData, tagDBPet);

	if (!LoadPetSoul(&pPetLoad->petAtt, dwPetID))
	{
		return FALSE;
	}
	PVOID pVarLenData = pPetLoad->byData;
	LoadPetSkill(pVarLenData, dwPetID, pPetLoad->nPetSkillNum);
	LoadPetEquip(pVarLenData, dwPetID, pPetLoad->nPetEquipNum);

	pData = pVarLenData;
	return TRUE;
}

//-------------------------------------------------------------------------------------------------------
// 保存宠物
//-------------------------------------------------------------------------------------------------------
BOOL CLoongDB::SavePets( DWORD dwRoleID, IN LPVOID pData, INT32 nNum, OUT LPVOID* ppOutPointer )
{
	MTRANS_POINTER(pDBPet, pData, tagDBPet);

	while(nNum--)
	{
		pDBPet = (tagDBPet*)pData;
		DWORD dwPetID = pDBPet->petAtt.dwPetID;
		SaveAPet(pData, dwPetID);
	}

	*ppOutPointer = pData;
	return TRUE;
}


BOOL CLoongDB::SaveAPet( OUT PVOID& pData, DWORD dwPetID)
{
	MTRANS_POINTER(pDBPet, pData, tagDBPet);
	
	if (!UpdatePetAtt(&pDBPet->petAtt, pDBPet->petAtt.dwPetID))
	{
		return FALSE;
	}
	
	PVOID pVarData = pDBPet->byData;
	UpdatePetSkill(pVarData, pDBPet->nPetSkillNum, dwPetID);

	pData = pVarData;

	return TRUE;
}

BOOL CLoongDB::UpdatePetAtt( const tagDBPetAtt* pPetAtt, DWORD& dwPetID)
{
	return Update(GT_INVALID, (LPVOID)pPetAtt, 1, sizeof(tagDBPetAtt), (&CLoongDB::FormatUpdatePetAtt));
}

VOID CLoongDB::FormatUpdatePetAtt( Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pData )
{
	MTRANS_POINTER(p, pData, tagDBPetAtt);

	Beton::Connection* pCon = m_pDBLoong->GetFreeConnection();

	pStream->SetReplace("pet_data");

	pStream->FillString("pet_id=") << p->dwPetID;
	pStream->FillString(",pet_name='").FillString(p->tszName, pCon).FillString("'");
	pStream->FillString(",master_id=") << p->dwMasterID;
	pStream->FillString(",type_id=") << p->dwProtoID;
	pStream->FillString(",quality=") << p->nQuality;
	pStream->FillString(",aptitude=") << p->nAptitude;
	pStream->FillString(",potential=") << p->nPotential;
	pStream->FillString(",cur_spirit=") << p->nSpirit;
	pStream->FillString(",wuxing_energy=") << p->nWuXingEnergy;
	pStream->FillString(",talent_count=") << p->nTalentCount;
	pStream->FillString(",pet_state=") << p->byPetState;
	pStream->FillString(",cur_exp=") << p->nExpCur;
	pStream->FillString(",step=") << p->nStep;
	pStream->FillString(",grade=") << p->nGrade;
	pStream->FillString(",pet_lock=") << p->bLocked;

	m_pDBLoong->ReturnConnection(pCon);
}

//-------------------------------------------------------------------------------------------------------
// 添加宠物技能
//-------------------------------------------------------------------------------------------------------
DWORD CLoong::InsertPetSkill(DWORD pDBMsg, DWORD dwDummy)
{
	MTRANS_POINTER(pRecv, pDBMsg, tagNDBC_InsertPetSkill);
	m_pDB->InsertPetSkill(&pRecv->petSkill, pRecv->dwPetID);
	return E_Success;
}

//-------------------------------------------------------------------------------------------------------
// 删除宠物技能
//-------------------------------------------------------------------------------------------------------
DWORD CLoong::DelPetSkill(DWORD pDBMsg, DWORD dwDummy)
{
	MTRANS_POINTER(pRecv, pDBMsg, tagNDBC_DelPetSkill);
	m_pDB->DelPetSkill(pRecv->dwPetSkillTypeID, pRecv->dwPetID);
	return E_Success;
}

//-------------------------------------------------------------------------------------------------------
// 添加宠物装备
//-------------------------------------------------------------------------------------------------------
// DWORD CLoong::PetEquip(DWORD pDBMsg, DWORD dwDummy)
// {
// 
// 	return E_Success;
// }
//-------------------------------------------------------------------------------------------------------
// 删除宠物装备
//-------------------------------------------------------------------------------------------------------
