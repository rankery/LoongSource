#pragma once
#include "lowerstate_lp.h"

/**	\class LowerStateStand_LP
	\brief 本地玩家空闲状态
*/
class LowerStateStand_LP : public LowerState_LP
{
	enum EStep
	{
		EStep_Ready,//备战
		EStep_Stand,//站立
		EStep_Ease,//悠闲
		EStep_Buff,//播放Buff动作
	};

public:
	LowerStateStand_LP(void);
	virtual ~LowerStateStand_LP(void);

	//--State_LP
	virtual void Active(tagGameEvent* pEvent,NavMap* pNav,LocalPlayer* pRole,tagPreState* pPreState=NULL);	
	virtual void Update(NavMap* pNav,LocalPlayer* pRole);							
	virtual void OnGameEvent(tagGameEvent* pEvent);	
	virtual void OnNetCmd(tagNetCmd* pNetCmd);
	virtual void OnExit();
	virtual void ReplayAni(LocalPlayer* pRole);
	virtual bool IfCanChange2UpperState(FSM_LP::EUpperState state,tagGameEvent* pEvent);
	virtual bool IfCanChange2LowerState(FSM_LP::ELowerState state,tagGameEvent* pEvent);

	//钓鱼时不想要悠闲动作,所以每次钓鱼之前都将这个时间赋值为当前时间
	//这样钓鱼的时候，总是不能播放悠闲动作,注意此方法调用一次只能保证忽略一次悠闲动作
	//如果想一直不播放悠闲动作就要不断的调用此方法，建议如果要达到一直不播放悠闲动作的
	//效果不要使用此方法
	void ResetLastEaseTime();

private:
	void PlayReadyAni(LocalPlayer* pRole,bool bCheckIfCanPlay=true);
	void PlayStandAni(LocalPlayer* pRole,bool bCheckIfCanPlay=true);
	void PlayEaseAni(LocalPlayer* pRole,bool bCheckIfCanPlay=true);
	bool PlayBuffAni(LocalPlayer* pRole,bool bCheckIfCanPlay=true);

private:
	DWORD						m_sprayEffectID;//水花特效ID
	DWORD						m_lastPlayEaseTime;//上次播放悠闲动作时间
	EStep						m_step;
};
