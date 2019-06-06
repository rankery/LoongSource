#pragma once
#include "PathFinder.h"

class LocalPlayer;
/** \class ActionMouseWaterWalk_LP
\brief 本地玩家鼠标移动行为
*/
class ActionMouseWaterWalk_LP
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
		ER_Land,
		ER_Swim,
		ER_CannotArrive,
	};

public:
	ActionMouseWaterWalk_LP(void);
	virtual ~ActionMouseWaterWalk_LP(void);

	//--
	void Active(const Vector3& start,const Vector3& end,float validDist,NavMap* pNav,LocalPlayer* pRole,bool bNeedSendStopCmd=false);
	void Update(NavMap* pNav,LocalPlayer* pRole);
	void Stop(LocalPlayer* pRole);

	bool IsClosed(){ return m_bClosed;}
	EResult GetResult(){ return m_result;}
private:
	void PlaySprayEffect(NavMap* pNav,LocalPlayer* pRole);
	void SendCmd(const Vector3& start,const Vector3& end);
private:
	NavCollider_Swim			m_collider;
	PathFinder					m_pathFinder;

	Vector3						m_moveEnd;
	bool						m_bClosed;
	EResult						m_result;
	float						m_validDistSq;
	bool						m_bNeedSendStopCmd;
	EMoveMode					m_moveMode;
	int							m_nFoot;
};
