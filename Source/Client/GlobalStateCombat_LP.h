#pragma once
#include "globalstate_lp.h"

class CombatSys;
/** \class GlobalStateCombat_LP
	\brief 本地玩家战斗状态
*/
class GlobalStateCombat_LP :
	public GlobalState_LP
{
	enum EStep
	{
		EStep_HandUpWeapon,   //拔武器
		EStep_OpenEquip,      //展开装备（变形）
		EStep_Fighting,		  //战斗状态
		EStep_CloseEquip,     //关闭装备（变形）
		EStep_HandDownWeapon, //收武器
	};
public:
	GlobalStateCombat_LP(void);
	virtual ~GlobalStateCombat_LP(void);

	//--GlobalState_LP
	virtual void Active(tagGameEvent* pEvent,NavMap* pNav,LocalPlayer* pRole);		
	virtual void Update(NavMap* pNav,LocalPlayer* pRole);							
	virtual void OnGameEvent(tagGameEvent* pEvent);	
	virtual void OnNetCmd(tagNetCmd* pNetCmd);

private:
	bool					m_bStartTimer;
	DWORD					m_dwStartTime;
	EStep					m_step;
	tstring					m_szEnterTrackName;
	tstring					m_szLeaveTrackName;

	CombatSys*				m_pCombatSys;
};
