#pragma once
#include "..\WorldDefine\move_define.h"

class LocalPlayer;
/** \class ActionKeyboardSwim_LP
	\brief 本地玩家键盘游泳行为
*/
class ActionKeyboardSwim_LP
{
public:
	enum EResult
	{
		ER_Unknow,
		ER_Swim,
		ER_Ground,
	};
public:
	ActionKeyboardSwim_LP(void);
	virtual ~ActionKeyboardSwim_LP(void);

	//--
	void Active(const Vector3& start,EWalkDir dir,NavMap* pNav,LocalPlayer* pRole,bool bPlaySound);
	void Update(NavMap* pNav,LocalPlayer* pRole);
	void Stop(NavMap* pNav,LocalPlayer* pRole);
	bool IsClosed()				{ return m_bClosed;		}
	EResult GetResult()			{ return m_result;		}
	bool IsBlocking()			{ return m_bBlocking;	}

	EWalkDir GetMoveDir()		{ return m_moveDir;		}
	const Vector3& GetMoveVec() { return m_moveVec;		}

	void StopSound();
	void ReplayAni(LocalPlayer* pRole);

private:
	Vector3 CalcMoveEnd(EWalkDir dir,float cameraYaw,const Vector3& start);
	void SendCmd(const Vector3& srcPos,const Vector3& dstPos);
	void PlaySprayEffect(NavMap* pNav,LocalPlayer* pRole);
	void PlaySound(LocalPlayer* pRole);
	void UpdateSound(const Vector3& rolePos);

private:
	NavCollider_Swim	m_collider;
	EWalkDir			m_moveDir;
	Vector3				m_moveVec;
	float				m_lastPlayeEffectTime;	//上次播放水花特效时间
	DWORD				m_dwSoundID;			//音效ID
	bool				m_bBlocking;			//是否被挡住
	bool				m_bClosed;
	EResult				m_result;
};
