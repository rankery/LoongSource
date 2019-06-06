#pragma once
#include "PathFinder.h"

class LocalPlayer;
/** \class ActionMouseMove_LP
	\brief 本地玩家鼠标移动行为
*/
class ActionMouseMove_LP
{
	enum EMoveMode
	{
		EMM_DirectGo,
		EMM_FindPath,
	};

public:
	enum EResult
	{
		ER_Unknow,
		ER_Arrive,
		ER_Slide,
		ER_Drop,
		ER_Swim,
		ER_OnWater,
		ER_CannotArrive,
	};

public:
	ActionMouseMove_LP(void);
	virtual ~ActionMouseMove_LP(void);

	//--
	void Active(const Vector3& start,const Vector3& end,float validDist,NavMap* pNav,LocalPlayer* pRole,bool bCanPlayAni,bool bCanSetYaw,bool bNeedSendStopCmd=false);
	void Update(NavMap* pNav,LocalPlayer* pRole,bool bCanPlayAni,bool bCanSetYaw);
	void Stop(LocalPlayer* pRole);
	void ReplayAni(LocalPlayer* pRole);

	bool IsClosed(){ return m_bClosed;}
	EResult GetResult(){ return m_result;}

private:
	void PlaySprayEffect(NavMap* pNav,LocalPlayer* pRole);
	void SendCmd(const Vector3& start,const Vector3& end);

private:
	NavCollider_Move			m_collider;
	PathFinder					m_pathFinder;

	Vector3						m_moveEnd;
	bool						m_bClosed;
	EResult						m_result;
	float						m_validDistSq;
	bool						m_bNeedSendStopCmd;
	EMoveMode					m_moveMode;
	float						m_lastPlayeEffectTime;
};
