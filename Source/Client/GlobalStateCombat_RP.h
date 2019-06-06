#pragma once
#include "globalstate_rp.h"

/** \class GlobalStateCombat_RP
	\brief 远程玩家战斗状态
*/
class GlobalStateCombat_RP :
	public GlobalState_RP
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
	GlobalStateCombat_RP(void);
	virtual ~GlobalStateCombat_RP(void);

	//--GlobalState_RP
	virtual void Active(tagNetCmd* pNetCmd,NavMap* pNav,Player* pRole,DWORD dwParam=0);		
	virtual void Update(NavMap* pNav,Player* pRole);							
	virtual void OnGameEvent(tagGameEvent* pEvent);	
	virtual void OnNetCmd(tagNetCmd* pNetCmd);

private:
	EStep m_step;
	TObjRef<Util> m_pUtil;
	tstring	m_szEnterTrackName;
	tstring	m_szLeaveTrackName;
};
