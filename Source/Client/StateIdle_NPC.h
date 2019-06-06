#pragma once
#include "state_npc.h"

/**	\class StateIdle_NPC
	\brief 远程玩家下身空闲状态
*/
class StateIdle_NPC : public State_NPC
{
	enum EStep
	{
		EStep_Stand,//站立
		EStep_Ease,//悠闲
		EStep_Buff,//Buff动作
	};
public:
	StateIdle_NPC(void);
	virtual ~StateIdle_NPC(void);

	//--State_NPC
	virtual void Active(tagNetCmd* pNetCmd,NavMap* pNav,NPC* pRole);		
	virtual void Update(NavMap* pNav,NPC* pRole);							
	virtual void OnNetCmd(tagNetCmd* pNetCmd);
	virtual void OnGameEvent(tagGameEvent* pEvent);

private:
	bool PlayBuffAni(NPC* pRole);

private:
	DWORD m_lastPlayEaseTime;//上次播放悠闲动作时间
	EStep m_step;
};
