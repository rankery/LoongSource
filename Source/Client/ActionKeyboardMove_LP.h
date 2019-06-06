#pragma once
#include "..\WorldDefine\move_define.h"

class LocalPlayer;
/** \class ActionKeyboardMove_LP
	\brief 本地玩家键盘移动行为
*/
class ActionKeyboardMove_LP
{
public:
	enum EResult
	{
		ER_Unknow,
		ER_Stand,
		ER_Drop,
		ER_Slide,
		ER_Swim,
		ER_OnWater,
	};
public:
	ActionKeyboardMove_LP(void);
	virtual ~ActionKeyboardMove_LP(void);

	//--
	void Active(const Vector3& start,EWalkDir dir,NavMap* pNav,LocalPlayer* pRole,bool bCanPlayAni,bool bCanSetYaw);
	void Update(NavMap* pNav,LocalPlayer* pRole,bool bCanPlayAni,bool bCanSetYaw);
	void Stop(NavMap* pNav,LocalPlayer* pRole);
	bool IsClosed(){ return m_bClosed;}
	EResult GetResult(){ return m_result;}

	/** 重新播放动作
	*/
	void ReplayAni(LocalPlayer* pRole);

	EWalkDir GetMoveDir()		{ return m_moveDir;}
	const Vector3& GetMoveVec() { return m_moveVec;}
	bool IsBlocking()			{ return m_bBlocking;}

private:
	Vector3 CalcMoveEnd(EWalkDir dir,float cameraYaw,const Vector3& start);
	void SendCmd(const Vector3& srcPos,const Vector3& dstPos);
	void PlaySprayEffect(NavMap* pNav,LocalPlayer* pRole);

private:
	NavCollider_Move			m_collider;
	EWalkDir					m_moveDir;
	Vector3						m_moveVec;
	float						m_lastPlayeEffectTime;//上次播放水花特效时间
	bool						m_bBlocking;		//是否处于被挡住移动方式
	bool						m_bClosed;
	EResult						m_result;
};
