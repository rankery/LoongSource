#pragma once
#include "upperstate_lp.h"
#include "CombatEvent.h"
#include "ActionUseItemAttack_LP.h"
#include "ActionUseItemSpell_LP.h"

/** \class UpperStateUseItem_LP
\brief 本地玩家使用技能状态
*/
class UpperStateUseItem_LP : public UpperState_LP
{
	enum EStep
	{
		EStep_None,
		EStep_Spell, //起手
		EStep_Attack,//攻击
	};

	struct tagEventCache
	{
		enum EEventType
		{
			EET_None,
			EET_UseSkill,
			EET_UseItem,
		};
		EEventType			 eventType;
		tagUseSkillEvent	 useSkill;
		tagUseItemEvent		 useItem;
	};

public:
	UpperStateUseItem_LP(void);
	virtual ~UpperStateUseItem_LP(void);

	//--UpperState_LP
	virtual void Active(tagGameEvent* pEvent,NavMap* pNav,LocalPlayer* pRole);
	virtual void Update(NavMap* pNav,LocalPlayer* pRole);							
	virtual void OnGameEvent(tagGameEvent* pEvent);	
	virtual void OnNetCmd(tagNetCmd* pNetCmd);
	virtual void OnExit();
	virtual bool IfCanChange2UpperState(FSM_LP::EUpperState state,tagGameEvent* pEvent);
	virtual bool IfCanChange2LowerState(FSM_LP::ELowerState state,tagGameEvent* pEvent);
	virtual void OnChangeLowerState(FSM_LP::ELowerState newState,tagGameEvent* pEvent);
	virtual bool IfCanPlayLowerAni();
	virtual bool IfLowerCanSetYaw();

	DWORD	GetUsingItemTypeID(){ return m_itemTypeID; }
private:
	ActionUseItemAttack_LP	m_actAttack;
	ActionUseItemSpell_LP	m_actSpell;

	EStep					m_step;

	DWORD					m_targetID;
	INT64					m_itemID;
	DWORD					m_itemTypeID;
	DWORD					m_dwSerial;
	float					m_attackDist;	//攻击距离
	bool					m_bAttackItem;	//是否伤害技能
	bool					m_bSpellItem;	//是否起手技能
	bool					m_bMoveable;	//是否可以移动攻击
	bool					m_bMoveSpell;	//是否可以移动起手
	bool					m_bAniBlend;	//是否上下身分开

	tagEventCache			m_eventCache;
};	
