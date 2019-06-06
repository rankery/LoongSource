#pragma once
#include "PathFinder.h"

class LocalPlayer;
/** \class ActionMouseSwim_LP
	\brief 本地玩家鼠标游泳行为
*/
class ActionMouseSwim_LP
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
		ER_CannotArrive,
	};

public:
	ActionMouseSwim_LP(void);
	virtual ~ActionMouseSwim_LP(void);

	//--
	void Active(const Vector3& start,const Vector3& end,float validDist,NavMap* pNav,LocalPlayer* pRole,bool bNeedSendStopCmd=false);
	void Update(NavMap* pNav,LocalPlayer* pRole);
	void Stop(LocalPlayer* pRole);

	bool IsClosed(){ return m_bClosed;}
	EResult GetResult(){ return m_result;}

	void StopSound();

private:
	void PlaySprayEffect(NavMap* pNav,LocalPlayer* pRole);
	void SendCmd(const Vector3& start,const Vector3& end);

	void PlaySound(LocalPlayer* pRole);
	void UpdateSound(const Vector3& rolePos);

private:
	NavCollider_Swim			m_collider;
	PathFinder					m_pathFinder;

	Vector3						m_moveEnd;
	bool						m_bClosed;
	EResult						m_result;
	float						m_validDistSq;
	bool						m_bNeedSendStopCmd;
	EMoveMode					m_moveMode;
	float						m_lastPlayeEffectTime;
	DWORD						m_dwSoundID;
};
