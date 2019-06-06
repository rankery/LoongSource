//-------------------------------------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: pet_tracker.h
// author: xlguo
// actor:
// data: 2009-07-13
// last:
// brief: 宠物跟踪器
//-------------------------------------------------------------------------------------------------------
#pragma once


class Unit;
class Role;
class Creature;
class Pet;
class MoveData;
class PetSoul;

//----------------------------------------------------------------------------------------------------
// 宠物跟踪器
//----------------------------------------------------------------------------------------------------
class PetTracker
{
	//----------------------------------------------------------------------------------------------------
	// 一些常量
	//----------------------------------------------------------------------------------------------------
	static const INT		MAX_NEED_MOVE_LEN	= 50 * 3;
	static const INT		MIN_NEED_MOVE_LEN	= 50 * 2;
	static const INT		NEED_TRANS_LEN		= 50 * 40;
	static const INT		MAX_TRY_TIME		= 100;
	static const INT		MAX_TRANS_TICKS		= TICK_PER_SECOND * 5;

public:
	//----------------------------------------------------------------------------------------------------
	// 构造与析构
	//----------------------------------------------------------------------------------------------------
	PetTracker(PetSoul* pSoul, Pet* pPet);
	~PetTracker(){}

	//----------------------------------------------------------------------------------------------------
	// 一些get
	//----------------------------------------------------------------------------------------------------
	Role*		GetMaster()	const;
	Pet*		GetPet() const			{	return m_pPet;		}
	Unit*		GetTarget() const		{	return GetUnit(m_dwTargetID);		}

	//----------------------------------------------------------------------------------------------------
	// 一些Pet用到的方法
	//----------------------------------------------------------------------------------------------------
	VOID		SetTarget(DWORD dwTargetID);
	BOOL		NeedPutdown(const Vector3 &vMasterPos, const Vector3 &vPet);
	BOOL		PutDownBack(Pet* pPet, const Vector3 &vMasterPos, const Vector3 &vMasterFace);
	
	//----------------------------------------------------------------------------------------------------
	// 更新（控制宠物跟随）
	//----------------------------------------------------------------------------------------------------
	VOID		Update();

private:
	//----------------------------------------------------------------------------------------------------
	// 一些用到的方法
	//----------------------------------------------------------------------------------------------------
	Vector3		FindTargetDest( Unit* pTarget );
	BOOL		NeedMove(const Vector3 &vMasterPos, const Vector3 &vPet);
	Vector3		GetNearPos(const Vector3 &vMasterPos, const Vector3 &vMasterFace, FLOAT fMaxRange, FLOAT fMinRange);
	BOOL		TryGoto(Pet* pPet, const Vector3 &vDstPos);
	VOID		UpdatePetSpeed(Unit* pTarget);
	Unit*		GetUnit(DWORD dwID) const;

private:
	DWORD		m_dwTargetID;	// 目标id

	Pet*		m_pPet;			// 宠物实体
	PetSoul*	m_pSoul;		
	INT			m_nPutBackTicks;// 瞬移计时
};

