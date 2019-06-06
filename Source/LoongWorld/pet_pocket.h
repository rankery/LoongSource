//-------------------------------------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: pet_pocket.h
// author: xlguo
// actor:
// data: 2009-07-13
// last:
// brief: 宠物带
//-------------------------------------------------------------------------------------------------------
#pragma once

class PetSoul;
class Pet;
class Role;
struct tagDBPet;

//----------------------------------------------------------------------------------------------------
// 宠物带
//----------------------------------------------------------------------------------------------------
class PetPocket
{
	typedef	TMap<DWORD, PetSoul*>	SoulTMap;
	typedef	TMap<DWORD, Pet*>		PetTMap;
	typedef TList<PetSoul*>			SoulTList;

public:
	//----------------------------------------------------------------------------------------------------
	// 构造和析构
	//----------------------------------------------------------------------------------------------------
	~PetPocket(){	Destroy();	}

	//----------------------------------------------------------------------------------------------------
	// 公共接口
	//----------------------------------------------------------------------------------------------------
	BOOL		Init(const BYTE* &pData, INT nNum, Role* pRole);
	BOOL		SaveToDB(IN LPVOID pData, OUT LPVOID &pOutPointer, OUT INT32 &nNum);
	VOID		Destroy();
	VOID		Update();

public:
	//----------------------------------------------------------------------------------------------------
	// 拿走放入
	//----------------------------------------------------------------------------------------------------
	PetSoul*	GetAway(DWORD dwPetID, BOOL bSync = FALSE);
	DWORD		PutIn(PetSoul* pSoul, BOOL bSaveDB, BOOL bSend = TRUE);
	DWORD		CanPutIn(PetSoul* pSoul);

	//----------------------------------------------------------------------------------------------------
	// 获得
	//----------------------------------------------------------------------------------------------------
	VOID		GetAllPetID(DWORD* dwPetIDbuf, INT& num);
	PetSoul*	GetPetSoul(DWORD dwPetID);
	PetSoul*	GetCalledPetSoul();
	PetSoul*	GetPreparingPetSoul();
	PetSoul*	GetMountPetSoul();

	//----------------------------------------------------------------------------------------------------
	// 生成宠物
	//----------------------------------------------------------------------------------------------------
	DWORD		HatchEgg(INT64 n64ItemID, LPCTSTR tszName);

	//----------------------------------------------------------------------------------------------------
	// 锁定解锁
	//----------------------------------------------------------------------------------------------------
	DWORD		LockPet(DWORD dwPetID, INT64 n64ItemID);
	DWORD		UnLockPet(DWORD dwPetID, INT64 n64ItemID);

	//----------------------------------------------------------------------------------------------------
	// 增强宠物
	//----------------------------------------------------------------------------------------------------
	DWORD		CalledSoulEnhance(INT64 n64ItemID);
	DWORD		CalledSoulUpStep(INT64 n64ItemID, DWORD &dwSkillID, INT &nDstStep);

	//----------------------------------------------------------------------------------------------------
	// 召唤休息
	//----------------------------------------------------------------------------------------------------
	DWORD		CallPet(DWORD dwPetID);
	DWORD		RestPet(DWORD dwPetID);
	DWORD		RestAPet();

	//----------------------------------------------------------------------------------------------------
	// 预备驾驭
	//----------------------------------------------------------------------------------------------------
	DWORD		PreparePet(DWORD dwPetID);
	DWORD		UnPreparePet(DWORD dwPetID);

	//----------------------------------------------------------------------------------------------------
	// 驾驭
	//----------------------------------------------------------------------------------------------------
	DWORD		CanRidePet();
	DWORD		RidePet();
	DWORD		UnRidePet();

	VOID		SetKeepMount(BOOL bSet)	{	m_bKeepMount = bSet;	}
	BOOL		GetKeepMount()			{	return m_bKeepMount;	}

	//----------------------------------------------------------------------------------------------------
	// 双人骑乘副驾驶
	//----------------------------------------------------------------------------------------------------
	BOOL		IsRideAfter();
	DWORD		GetHostPetID()	{return m_dwOthersPetID;}
	DWORD		CanSetRideAfter(Role* pHost, BOOL bSet);
	DWORD		SetRideAfter(Role* pHost, BOOL bSet);

	DWORD		RideAfter(Role* pMaster);// 被命令处理器调用
	DWORD		StopRideAfter(Role* pMaster);

	//----------------------------------------------------------------------------------------------------
	// 双人骑乘主驾驶
	//----------------------------------------------------------------------------------------------------
	DWORD		CanAddPassenger(Role* pPassenger);
	DWORD		CanRemovePassenger(Role* pPassenger);
	DWORD		AddPassenger(Role* pPassenger);
	DWORD		RemovePassenger(Role* pPassenger);

	//----------------------------------------------------------------------------------------------------
	// 召唤的宠物喂食
	//----------------------------------------------------------------------------------------------------
	DWORD		CalledPetFeed(INT64 n64ItemID);

public:
	//----------------------------------------------------------------------------------------------------
	// 召唤宠物进出宠物带
	//----------------------------------------------------------------------------------------------------
	VOID		CalledPetEnterPocket();
	VOID		CalledPetLeavePocket();

	//----------------------------------------------------------------------------------------------------
	// 宠物交易相关
	//----------------------------------------------------------------------------------------------------
	BOOL		CheckExistInPocket(DWORD *dwPetIDs, INT nNum );
	DWORD		CanExchange(DWORD dwPetID);
	BOOL		GetFreeSize();
	VOID		TakeFromPocket(PetSoul* *pSouls, INT nSizeSouls, DWORD* dwPetIDs, INT nNumPetID);
	VOID		PutInPocket(PetSoul* *pSouls, INT nSizeSouls);

private:
	Role*			m_pMaster;			// 主人
	SoulTMap		m_mapAllSoul;		// 所有PetSoul
	INT				m_nCalledPets;		// 已经召唤的宠物数量
	DWORD			m_dwPetIDForUpStep;	// 升阶的宠物id
	BOOL			m_bKeepMount;		// 打断之后是否继续骑乘
	DWORD			m_dwOthersPetID;	// 双人骑乘中，骑着的其他玩家的宠物

public:
	//----------------------------------------------------------------------------------------------------
	// 测试
	//----------------------------------------------------------------------------------------------------
	BOOL		DynamicTest(INT nTestNo, INT nArg1, INT nArg2);

};


