#pragma once
#include "state_npc.h"

/**	\class StateDead_NPC
	\brief NPC死亡状态
*/
class StateDead_NPC : public State_NPC
{
	enum EStep
	{
        EStep_Drop,     //掉落
		EStep_FallDown,	//倒下
		EStep_Lie,		//躺着
		EStep_Fadeout,	//淡出
		EStep_Closed,	//结束
	};
public:
	StateDead_NPC(void);
	virtual ~StateDead_NPC(void);

	//--State_NPC
	virtual void Active(tagNetCmd* pNetCmd,NavMap* pNav,NPC* pRole);		
	virtual void Update(NavMap* pNav,NPC* pRole);							
	virtual void OnNetCmd(tagNetCmd* pNetCmd);
	virtual void OnGameEvent(tagGameEvent* pEvent){}

private:
    NavCollider_Jump    m_collider;      // 当npc在半空死亡时先进行掉落
    float               m_lastCDTime;
	EStep	m_step;
	float	m_startLieTime;
	float	m_startFadeTime;
};
