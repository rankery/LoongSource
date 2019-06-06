//-------------------------------------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: pet_skill.h
// author: xlguo
// actor:
// data: 2009-07-13
// last:
// brief: 宠物技能
//-------------------------------------------------------------------------------------------------------
#pragma once

struct	tagDBPetSkill;
struct	tagPetSkillProto;
struct	tagPetSkillMsgInfo;
struct	tagPetSkillCmdParam;

class PetSoul;
class Role;

// 技能收获时间
const INT	COUNT_DOWN_GAIN		= 5 * 60 * TICK_PER_SECOND;

// 技能使用时间
const INT	COUNT_DOWN_USING	= 2 * 60 * 60 * TICK_PER_SECOND;



//----------------------------------------------------------------------------------------------------
// 宠物技能
//----------------------------------------------------------------------------------------------------
class PetSkill
{
protected:
	PetSkill(DWORD dwSkillTypeID, PetSoul* pSoul, INT nPara1 = GT_INVALID, INT nPara2 = GT_INVALID);
	virtual	~PetSkill(){}	

public:
	const tagPetSkillProto* GetProto() const { return m_pProto; }

	//----------------------------------------------------------------------------------------------------
	// 创建与删除
	//----------------------------------------------------------------------------------------------------
	static PetSkill*	CreatePetSkill(DWORD dwSkillTypeID, PetSoul* pSoul, INT nPara1 = GT_INVALID, INT nPara2 = GT_INVALID);
	static PetSkill*	CreateDBPetSkill(tagDBPetSkill* pDBPetSkill, PetSoul* pSoul);
	static VOID			DeletePetSkill(PetSkill* p2Delete)	{	SAFE_DEL(p2Delete);	}
	BOOL				SaveToDB( tagDBPetSkill* pData);

	//----------------------------------------------------------------------------------------------------
	// 填写技能信息
	//----------------------------------------------------------------------------------------------------
	VOID				FillClientInfo(tagPetSkillMsgInfo* pInfo);
	DWORD				GetSkillTypeID() const;
	BYTE				GetCastCondition() const;

	//----------------------------------------------------------------------------------------------------
	// 更新
	//----------------------------------------------------------------------------------------------------
	virtual VOID		Update();

	//----------------------------------------------------------------------------------------------------
	// 冷却
	//----------------------------------------------------------------------------------------------------
	BOOL				IsCoolDowning()		{		return GT_VALID(m_nCoolDownTick);				}
	VOID				SetCoolDowning(INT nCoolDownTick = 0);
	VOID				CoolDowning();

	BOOL				IsWorkCounting()	{		return GT_VALID(m_nWorkCountTick);				}
	VOID				SetWorkCounting(INT nTickAdd = 0);
	BOOL				WorkCounting();

	VOID				BreakWork();

	//----------------------------------------------------------------------------------------------------
	// 其它
	//----------------------------------------------------------------------------------------------------
	BOOL				CanSetWorking( BOOL bWorking );
	BOOL				SetWorking(BOOL bWorking);
	VOID				SendPetSkillMsg(PVOID pData, DWORD dwSize, DWORD dwErrCode);	


protected:
	PetSoul*			GetSoul()	const			{		return m_pSoul;				}
	Role*				GetMaster() const;

	VOID				SendTimerMsg(DWORD dwPtt, INT nValue);

	INT					m_nCoolDownTick;
	INT					m_nWorkCountTick;

private:
	const tagPetSkillProto* m_pProto;
	PetSoul*			m_pSoul;
};

//----------------------------------------------------------------------------------------------------
// 被动接口
//----------------------------------------------------------------------------------------------------
class PassiveSkill : public PetSkill
{
public:
	PassiveSkill(DWORD dwSkillTypeID, PetSoul* pSoul, INT nPara1 = GT_INVALID, INT nPara2 = GT_INVALID)
		:PetSkill(dwSkillTypeID, pSoul, nPara1, nPara2){}
public:
	virtual BOOL		Active() = 0;
	virtual BOOL		DeActive() = 0;
};

//----------------------------------------------------------------------------------------------------
// 主动接口
// 主动技能的使用分为两个部分
// PreHandleCmdImpl对技能进行先行判断，若失败，则不继续执行，否则设置工作时间
// 等工作时间过后执行PastHandleCmd
//----------------------------------------------------------------------------------------------------
class ActiveSkill : public PetSkill
{
public:
	ActiveSkill(DWORD dwSkillTypeID, PetSoul* pSoul, INT nPara1 = GT_INVALID, INT nPara2 = GT_INVALID)
		:PetSkill(dwSkillTypeID, pSoul, nPara1, nPara2), m_RecvSkillData(0, 0){}
	~ActiveSkill()	{		SetRecvSkillData(0, NULL);	}

public:
	DWORD			PreHandleCmd(PVOID pSkillData, DWORD dwSkillDataSize, PVOID pSendData, DWORD &dwSendSize);
	virtual DWORD	PreHandleCmdImpl(INT &nWorkingAdd) = 0;
	DWORD			PastHandleCmd();
	virtual DWORD	PastHandleCmdImpl(INT &nCoolDownAdd) = 0;

protected:
	VOID			SetSendSkillData(DWORD& dwSize, PVOID pData);
	PVOID			GetSendSkillData(DWORD* &dwSize);
	VOID			SetRecvSkillData(DWORD dwSize, PVOID pData);
	PVOID			GetRecvSkillData(DWORD &dwSize);

private:
	std::pair<DWORD, PVOID>		m_RecvSkillData;
	std::pair<DWORD*, PVOID>	m_SendSkillData;
};


//----------------------------------------------------------------------------------------------------
// 自身提升接口
//----------------------------------------------------------------------------------------------------
class EnhanceSkill : public PetSkill
{
public:
	EnhanceSkill(DWORD dwSkillTypeID, PetSoul* pSoul, INT nPara1 = GT_INVALID, INT nPara2 = GT_INVALID)
		:PetSkill(dwSkillTypeID, pSoul, nPara1, nPara2){}
public:
	virtual BOOL	Open() = 0;
	virtual BOOL	Close() = 0;
};

//----------------------------------------------------------------------------------------------------
// 宠物强化技能
//----------------------------------------------------------------------------------------------------
class PetMountAddSkill:public EnhanceSkill
{
	friend class PetSkill;

	PetMountAddSkill(DWORD dwSkillTypeID, PetSoul* pSoul, INT nPara1, INT nPara2) 
		:EnhanceSkill(dwSkillTypeID, pSoul, nPara1, nPara2),m_bInUsing(FALSE){}

private:
	BOOL		Open();
	BOOL		Close();

private:
	BOOL		m_bInUsing;
};

//----------------------------------------------------------------------------------------------------
// 宠物强身技能
//----------------------------------------------------------------------------------------------------
class PetStrengthSkill:public PassiveSkill
{
	friend class PetSkill;
	
	PetStrengthSkill(DWORD dwSkillTypeID, PetSoul* pSoul, INT nPara1, INT nPara2) 
		:PassiveSkill(dwSkillTypeID, pSoul, nPara1, nPara2),m_bInUsing(FALSE){}

private:
	BOOL		Active();
	BOOL		DeActive();

private:
	BOOL		m_bInUsing;
};

//----------------------------------------------------------------------------------------------------
// 宠物喂药技能
//----------------------------------------------------------------------------------------------------
class PetMedicineFeedSkill : public ActiveSkill
{
	friend class PetSkill;
	PetMedicineFeedSkill( DWORD dwSkillTypeID, PetSoul* pSoul, INT nPara1, INT nPara2 )
		:ActiveSkill(dwSkillTypeID, pSoul, nPara1, nPara2){}

private:
	DWORD PreHandleCmdImpl(INT &nWorkingAdd);
	DWORD PastHandleCmdImpl(INT &nCoolDownAdd);
};

//----------------------------------------------------------------------------------------------------
// 宠物拾取技能
//----------------------------------------------------------------------------------------------------
class PetPickUpSkill : public ActiveSkill
{
	friend class PetSkill;
	PetPickUpSkill( DWORD dwSkillTypeID, PetSoul* pSoul, INT nPara1, INT nPara2 )
		:ActiveSkill(dwSkillTypeID, pSoul, nPara1, nPara2)	{}

private:
	DWORD PreHandleCmdImpl(INT &nWorkingAdd);
	DWORD PastHandleCmdImpl(INT &nCoolDownAdd);
};

//----------------------------------------------------------------------------------------------------
// 宠物天赋技能
//----------------------------------------------------------------------------------------------------
class PetWuXingSkill : public ActiveSkill
{
	friend class PetSkill;
	PetWuXingSkill( DWORD dwSkillTypeID, PetSoul* pSoul, INT nPara1, INT nPara2 )
		:ActiveSkill(dwSkillTypeID, pSoul, nPara1, nPara2)	{}

private:
	DWORD PreHandleCmdImpl(INT &nWorkingAdd);
	DWORD PastHandleCmdImpl(INT &nCoolDownAdd);
};

//----------------------------------------------------------------------------------------------------
// 宠物状态技能
//----------------------------------------------------------------------------------------------------
class PetBuffSkill : public ActiveSkill
{
	friend class PetSkill;
	PetBuffSkill( DWORD dwSkillTypeID, PetSoul* pSoul, INT nPara1, INT nPara2 )
		:ActiveSkill(dwSkillTypeID, pSoul, nPara1, nPara2)	{}

private:
	DWORD PreHandleCmdImpl(INT &nWorkingAdd);
	DWORD PastHandleCmdImpl(INT &nCoolDownAdd);

	BOOL		bInUse;
};

//----------------------------------------------------------------------------------------------------
// 宠物采集技能
//----------------------------------------------------------------------------------------------------
class PetGatherSkill : public ActiveSkill
{
	friend class PetSkill;
	PetGatherSkill( DWORD dwSkillTypeID, PetSoul* pSoul, INT nPara1, INT nPara2 )
		:ActiveSkill(dwSkillTypeID, pSoul, nPara1, nPara2)	{}

private:
	DWORD PreHandleCmdImpl(INT &nWorkingAdd);
	DWORD PastHandleCmdImpl(INT &nCoolDownAdd);

};

//----------------------------------------------------------------------------------------------------
// 宠物小贩技能
//----------------------------------------------------------------------------------------------------
class PetSaleSkill : public ActiveSkill
{
	friend class PetSkill;
	PetSaleSkill( DWORD dwSkillTypeID, PetSoul* pSoul, INT nPara1, INT nPara2 )
		:ActiveSkill(dwSkillTypeID, pSoul, nPara1, nPara2)	{}

private:
	DWORD PreHandleCmdImpl(INT &nWorkingAdd);
	DWORD PastHandleCmdImpl(INT &nCoolDownAdd);

};

//----------------------------------------------------------------------------------------------------
// 宠物训练技能
//----------------------------------------------------------------------------------------------------
class PetExperanceSkill : public ActiveSkill
{
	friend class PetSkill;
	PetExperanceSkill( DWORD dwSkillTypeID, PetSoul* pSoul, INT nPara1, INT nPara2 )
		:ActiveSkill(dwSkillTypeID, pSoul, nPara1, nPara2)	{}

private:
	DWORD PreHandleCmdImpl(INT &nWorkingAdd);
	DWORD PastHandleCmdImpl(INT &nCoolDownAdd);

};

//----------------------------------------------------------------------------------------------------
// 宠物仓管技能
//----------------------------------------------------------------------------------------------------
class PetStoreHouseSkill : public ActiveSkill
{
	friend class PetSkill;
	PetStoreHouseSkill( DWORD dwSkillTypeID, PetSoul* pSoul, INT nPara1, INT nPara2 )
		:ActiveSkill(dwSkillTypeID, pSoul, nPara1, nPara2)	{}

private:
	DWORD PreHandleCmdImpl(INT &nWorkingAdd);
	DWORD PastHandleCmdImpl(INT &nCoolDownAdd);

};

