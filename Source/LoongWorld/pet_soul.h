//-------------------------------------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: pet_soul.h
// author: xlguo
// actor:
// data: 2009-07-13
// last:
// brief: 宠物灵魂
//-------------------------------------------------------------------------------------------------------
#pragma once

#include "pet_att.h"
#include "pet_equip.h"

class Pet;
class Role;
class PetTracker;
class PetSkill;

struct tagItem;
struct tagDBPet;
struct tagPetSkillCmdParam;

//----------------------------------------------------------------------------------------------------
// 宠物灵魂
// 说明：
//		技能id		dwPetSkillID;
//		技能lvl		dwPetSkillLvl;
//		技能typeid	dwPetSkillTypeID;
//		dwPetSkillTypeID = dwPetSkillID * 100 + dwPetSkillLvl;
//----------------------------------------------------------------------------------------------------
class PetSoul
{
	friend class PetAtt;
	typedef TMap<DWORD, PetSkill*>		PetSkillMap;	

public:
	//----------------------------------------------------------------------------------------------------
	// 创建与删除
	//----------------------------------------------------------------------------------------------------
	static VOID		CreateDBSoul( DWORD dwTypeID,LPCTSTR tszName,Role* pMaster, INT nQualiry, INT64 n64EggSerial );
	static PetSoul* CreateSoulByDBData(const BYTE* &pData, BOOL bCreate);
	static VOID		DeleteSoul(PetSoul* pSoul, BOOL bFromDB = FALSE);

	//----------------------------------------------------------------------------------------------------
	// Log
	//----------------------------------------------------------------------------------------------------
	static VOID		LogPet(DWORD dwMasterID, DWORD dwPetID, DWORD dwCmdID);

	//----------------------------------------------------------------------------------------------------
	// 更新
	//----------------------------------------------------------------------------------------------------
	VOID			Update();

	//----------------------------------------------------------------------------------------------------
	// 躯体与地图
	//----------------------------------------------------------------------------------------------------
	DWORD			BodyEnterMap();

	BOOL			IsPetInMap();
	DWORD			BodyLeaveMap();

	//----------------------------------------------------------------------------------------------------
	// 获取
	//----------------------------------------------------------------------------------------------------
	Role*			GetMaster()		const	{return m_pMaster;	}
	Pet*			GetBody()		const	{return m_pBody;	}
	VOID			SetBody(Pet* pBody)		{m_pBody = pBody;	}
	DWORD			GetID()			const;
	DWORD			GetProtoID()	const;
	PetAtt&			GetPetAtt()				{ return m_PetAtt;	}
	const tagPetProto* GetProto();

	//----------------------------------------------------------------------------------------------------
	// 锁定
	//----------------------------------------------------------------------------------------------------
	BOOL			IsLocked()		const	{ return m_PetAtt.IsLocked();	}
	VOID			SetLocked(BOOL bSet);

	VOID			SendSaveAtt2DB();
	VOID			FillClientPetAtt(tagPetInitAttr* pInitAtt);
	BOOL			SaveToDB(IN LPVOID pData, OUT LPVOID &pOutPointer);

	//----------------------------------------------------------------------------------------------------
	// 整合与分离
	//----------------------------------------------------------------------------------------------------
	BOOL			IntegrateInPet(Pet* pPet);
	VOID			DetachFromPet();
	BOOL			IntegrateInRole(Role* pRole);
	VOID			DetachFromRole();

	//----------------------------------------------------------------------------------------------------
	//	宠物状态相关
	//----------------------------------------------------------------------------------------------------
	BOOL			IsInState(EPetState eState);

	//----------------------------------------------------------------------------------------------------
	// 驾驭相关
	//----------------------------------------------------------------------------------------------------
	BOOL			IsMounted();
	BOOL			CanSetMount(BOOL bSet);
	BOOL			SetMount(BOOL bSet);

	//----------------------------------------------------------------------------------------------------
	// 召唤相关
	//----------------------------------------------------------------------------------------------------
	BOOL			IsCalled();
	BOOL			CanSetCalled(BOOL bSet);
	BOOL			SetCalled(BOOL bSet, BOOL bSync = TRUE);

	//----------------------------------------------------------------------------------------------------
	// 工作相关
	//----------------------------------------------------------------------------------------------------
	BOOL			IsWorking();
	BOOL			CanSetWroking( BOOL bSet, DWORD dwPetSkillTypeID );
	BOOL			SetWorking(BOOL bSet, DWORD dwPetSkillTypeID);
	BOOL			GetWorkingSkillTypeID()const {return m_dwWorkingSkillTypeID;}

	//----------------------------------------------------------------------------------------------------
	// 预备驾驭相关
	//----------------------------------------------------------------------------------------------------
	BOOL			IsPreparing();
	BOOL			CanSetPreparing( BOOL bSet);
	BOOL			SetPreparing(BOOL bSet);
	
	//----------------------------------------------------------------------------------------------------
	// 多人骑乘相关
	//----------------------------------------------------------------------------------------------------
	BOOL			CanAddPassenger(Role* pPassenger);
	BOOL			CanRemovePassenger(Role* pPassenger);
	DWORD			AddPassenger(Role* pPassenger);
	DWORD			RemovePassenger(Role* pPassenger);
	Role*			GetPassenger()	{	return m_pPassenger;	}

	//----------------------------------------------------------------------------------------------------
	//	宠物技能相关
	//----------------------------------------------------------------------------------------------------
	DWORD			LearnBookSkill(INT64 n64ItemID);
	DWORD			LearnNormalSkill(INT nLevel);

	DWORD			LearnSkill( DWORD dwPetSkillTypeID );
	DWORD			ForgetSkill(DWORD dwPetSkillID);
	
	DWORD			AddSkill(PetSkill* pSkill);
	DWORD			AddSkillOnLoad(PetSkill* pSkill);

	DWORD			RemoveSkill(PetSkill* pSkill);
	PetSkill*		GetSkill(DWORD dwPetSkillID);
	INT				ExportSpecSkill(INT nPetAtt, std::list<PetSkill*> &listModSkill);
	INT				GetModSkillType(INT nPetAtt);
	DWORD			HandleSkillCmd( DWORD dwPetSkillTypeID, PVOID pSkillData, DWORD dwSkillDataSize );
	DWORD			StopSkillWorking(DWORD dwPetSkillID);
	VOID			UpdateAllSkill();

	//----------------------------------------------------------------------------------------------------
	//	宠物装备相关
	//----------------------------------------------------------------------------------------------------
	DWORD			Equip(INT64 n64ItemID, INT8 n8DstPos, BOOL bSend = FALSE);
	DWORD			UnEquip(INT64 n64ItemID, INT16 n16DstPos, BOOL bSend = FALSE);
	DWORD			EquipSwapPos(INT64 n64ItemID, INT8 n8DstPos, BOOL bSend = FALSE);
	VOID			GetEquipInfo(INT64 n64ItemID, tagPetEquipMsgInfo* pMsgInfo);
	BOOL			HasEquip();

	//----------------------------------------------------------------------------------------------------
	// 经验与升级
	//----------------------------------------------------------------------------------------------------
	DWORD			PourExp(INT64 n64ItemID);
	VOID			OnLevelUp();
	DWORD			Enhance(INT nAptitudeAdd);
	DWORD			UpStep(DWORD dwPetSkillTypeID);

	//----------------------------------------------------------------------------------------------------
	// 喂食
	//----------------------------------------------------------------------------------------------------
	DWORD Feed(INT nSpritMod);

private:
	//----------------------------------------------------------------------------------------------------
	// 实现
	//----------------------------------------------------------------------------------------------------
	BOOL			SetMaster(Role* pRole)	{	m_pMaster = pRole;	return TRUE;}

	//----------------------------------------------------------------------------------------------------
	// 构造与析构
	//----------------------------------------------------------------------------------------------------
	PetSoul();
	virtual	~PetSoul();

	//----------------------------------------------------------------------------------------------------
	// 各种初始化
	//----------------------------------------------------------------------------------------------------
	BOOL			Init(const tagDBPet* &pSoulData, BOOL bCreate);
	VOID			InitPetEquip(IN PVOID pData, OUT PVOID& pDataOut, IN INT nNum);
	VOID			InitPetSkill(IN PVOID pData, OUT PVOID& pDataOut, IN INT nNum);

private:
	Role*			m_pMaster;				// 主人
	Role*			m_pPassenger;			// 客人
	Pet*			m_pBody;				// 躯体

	PetAtt			m_PetAtt;				// 宠物属性
	PetSkillMap		m_mapPetSkill;			// 宠物技能
	DWORD			m_dwWorkingSkillTypeID;	// 当前工作技能
	PetEquipBar		m_EquipBar;				// 宠物装备栏

	INT				nCount;					// 回复灵力计数
};


