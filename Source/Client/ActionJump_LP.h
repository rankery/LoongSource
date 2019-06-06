#pragma once

class LocalPlayer;
/** \class ActionJump_LP
	\brief 本地玩家跳跃行为
*/
class ActionJump_LP
{
	enum EStep
	{
		EStep_StartJump,
		EStep_Jumpping,
		EStep_Land,
		EStep_Closed
	};

public:
	enum EResult
	{
		ER_Unknow,
		ER_Slide,
		ER_VDrop,
		ER_Swim,
		ER_OnWater,
		ER_Land,
	};

public:
	ActionJump_LP(void);
	virtual ~ActionJump_LP(void);

	//--
	void Active(const Vector3& start,const Vector3& dir,bool bOrginJump,NavMap* pNav,LocalPlayer* pRole);
	void Update(bool bPlayLandAni,NavMap* pNav,LocalPlayer* pRole);
	bool IsClosed(){ return m_step==EStep_Closed;}
	EResult GetResult(){ return m_result;}
	bool IsLanding(){ return m_step==EStep_Land;}

private:
	NavCollider_Jump		m_collider;
	EStep					m_step;
	EResult					m_result;
	bool					m_bPlayInEffect;//是否已播放入水特效
};
