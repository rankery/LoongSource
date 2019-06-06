#pragma once
#include "lowerstate_rp.h"

/**	\class LowerStateStand_RP
	\brief 远程玩家站立状态
*/
class LowerStateStand_RP : public LowerState_RP
{
	enum EStep
	{
		EStep_Ready,//备战
		EStep_Stand,//站立
		EStep_Ease,//悠闲
		EStep_Buff,//播放Buff动作
	};
public:
	LowerStateStand_RP(void);
	virtual ~LowerStateStand_RP(void);

	//--LowerState_RP
	virtual void Active(tagNetCmd* pNetCmd,NavMap* pNav,Player* pRole);		
	virtual void Update(NavMap* pNav,Player* pRole);							
	virtual void OnNetCmd(tagNetCmd* pNetCmd);
	virtual void OnGameEvent(tagGameEvent* pEvent);
	virtual void OnExit();
	virtual void ReplayAni(Player* pRole);
	virtual bool IfCanChange2UpperState(FSM_RP::EUpperState state);

private:
	void PlayReadyAni(Player* pRole,bool bCheckIfCanPlay=true);
	void PlayStandAni(Player* pRole,bool bCheckIfCanPlay=true);
	void PlayEaseAni(Player* pRole,bool bCheckIfCanPlay=true);
	bool PlayBuffAni(Player* pRole,bool bCheckIfCanPlay=true);

private:
	DWORD m_sprayEffectID;//水花特效ID
	DWORD m_lastPlayEaseTime;//上次播放悠闲动作时间
	EStep m_step;
};
