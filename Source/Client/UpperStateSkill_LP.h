#pragma once
#include "upperstate_lp.h"
#include "CombatEvent.h"
#include "ActionSkillAttack_LP.h"
#include "ActionSkillSpell_LP.h"

/** \class UpperStateSkill_LP
	\brief 本地玩家使用技能状态
*/
class UpperStateSkill_LP : public UpperState_LP
{
	enum EStep
	{
		EStep_None,
		EStep_Pursue,				// 追踪目标
		EStep_Spell,				// 起手
		EStep_Attack,				// 攻击
	};

	struct tagEventCache
	{
		enum EEventType
		{
			EET_None,
			EET_UseSkill,
			EET_UseItem,
			EET_MouseMove,
			EET_KeyboardMove,
		};
		EEventType			 eventType;
		tagUseSkillEvent	 useSkill;
		tagUseItemEvent		 useItem;
		tagMouseMoveEvent	 mouseMove;
		tagKeyboardMoveEvent keyboardMove;
	};

public:
	UpperStateSkill_LP(void);
	virtual ~UpperStateSkill_LP(void);

	//--State_LP
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

private:
	bool IsInAttackRange(LocalPlayer* pRole,Role* pTarget);
	void SendStopPursueEvent();

private:
	ActionSkillAttack_LP	m_actAttack;
	ActionSkillSpell_LP		m_actSpell;

	EStep					m_step;

	DWORD					m_targetID;
	DWORD					m_skillID;
	DWORD					m_dwSerial;
	float					m_attackDist;			//攻击距离
	bool					m_bAttackSkill;			//是否伤害技能
	bool					m_bSpellSkill;			//是否起手技能
	bool					m_bMoveable;			//是否可以移动攻击
	bool					m_bMoveSpell;			//是否可以移动起手
	bool					m_bAniBlend;			//是否上下身分开
	bool					m_bAutoNormalAttack;	//是自动接普通攻击

	DWORD					m_startSpellTime;
	DWORD					m_spellRealTime;        //真实的起手时间，actSpell中的起手时间是缩短后的（为了预播攻击动作）
	DWORD                   m_dwAttackAdvanceTime;  //攻击动作提前的时间

	bool                    m_bWaitForWeaponSwap;	// 等待武器切换（当使用副手武器技能的时候）
	tagEventCache			m_eventCache;
	DWORD                   m_dwAttackEndTime;      // 攻击结束时间
	DWORD                   m_dwAttackEndDelayTime; // 有时由于网络延迟等问题动作完成后服务器技能还没有退出，所以加个延时
};	
