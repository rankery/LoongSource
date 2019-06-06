#pragma once
#include "upperstate_lp.h"

class LocalPlayer;
class Role;
struct tagGroundItem;
/** \class UpperStatePickup_LP
	\brief 本地玩家上身拾取状态
*/
class UpperStatePickup_LP :
	public UpperState_LP
{
public:
	UpperStatePickup_LP(void);
	virtual ~UpperStatePickup_LP(void);

	//--UpperState_LP
	virtual void Active(tagGameEvent* pEvent,NavMap* pNav,LocalPlayer* pRole);	
	virtual void Update(NavMap* pNav,LocalPlayer* pRole);							
	virtual void OnGameEvent(tagGameEvent* pEvent);	
	virtual void OnNetCmd(tagNetCmd* pNetCmd);
	virtual bool IfCanChange2UpperState(FSM_LP::EUpperState state,tagGameEvent* pEvent);
	virtual bool IfCanChange2LowerState(FSM_LP::ELowerState state,tagGameEvent* pEvent);
	virtual void OnChangeLowerState(FSM_LP::ELowerState newState,tagGameEvent* pEvent);
	virtual bool IfCanPlayLowerAni();
	virtual bool IfLowerCanSetYaw();

private:
	void SendMouseMoveEvent(const Vector3& start, const Vector3& end);

private:
	INT64					m_itemID;
	Vector3					m_itemPos;
};	
