#pragma once
#include "state_NPC.h"
#include "StateSkill_NPC.h"
#include "ActionSkillAttack_NPC.h"
#include "ActionSkillSpell_NPC.h"

class NPC;
/** \class StateSkill_NPC
	\brief NPC使用技能状态
*/
class StateSkill_NPC :
	public State_NPC
{
	enum EStep
	{
		EStep_Spell,
		EStep_Attack,
	};
public:
	StateSkill_NPC(void);
	virtual ~StateSkill_NPC(void);

	//--State_NPC
	virtual void Active(tagNetCmd* pNetCmd,NavMap* pNav,NPC* pRole);	
	virtual void OnExit();
	virtual void Update(NavMap* pNav,NPC* pRole);							
	virtual void OnNetCmd(tagNetCmd* pNetCmd);
	virtual void OnGameEvent(tagGameEvent* pEvent);

private:
	ActionSkillSpell_NPC	m_actSpell;
	ActionSkillAttack_NPC	m_actAttack;
	EStep					m_step;
	DWORD					m_targetID;
	DWORD					m_skillID;
	DWORD					m_dwSerial;
};	
