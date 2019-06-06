#pragma once
#include "..\WorldDefine\move_define.h"

class LocalPlayer;
/** \class ActionKeyboardWaterWalk_LP
	\brief 本地玩家键盘水上行走行为
*/
class ActionKeyboardWaterWalk_LP
{
public:
	enum EResult
	{
		ER_Unknow,
		ER_Ground,
		ER_OnWater,
		ER_Swim,
	};
public:
	ActionKeyboardWaterWalk_LP(void);
	virtual ~ActionKeyboardWaterWalk_LP(void);

	//--
	void Active(const Vector3& start,EWalkDir dir,NavMap* pNav,LocalPlayer* pRole);
	void Update(NavMap* pNav,LocalPlayer* pRole);
	void Stop(NavMap* pNav,LocalPlayer* pRole);
	bool IsClosed()				{ return m_bClosed;		}
	EResult GetResult()			{ return m_result;		}
	bool IsBlocking()			{ return m_bBlocking;	}

	EWalkDir GetMoveDir()		{ return m_moveDir;		}
	const Vector3& GetMoveVec() { return m_moveVec;		}
private:
	Vector3 CalcMoveEnd(EWalkDir dir,float cameraYaw,const Vector3& start);
	void SendCmd(const Vector3& srcPos,const Vector3& dstPos);
	void PlaySprayEffect(NavMap* pNav,LocalPlayer* pRole);
private:
	NavCollider_Swim	m_collider;
	EWalkDir			m_moveDir;
	Vector3				m_moveVec;
	bool				m_bBlocking;			//是否被挡住
	bool				m_bClosed;
	EResult				m_result;
	int                 m_nFoot;
};
