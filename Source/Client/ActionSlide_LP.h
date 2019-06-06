#pragma once

class LocalPlayer;
/** \class ActionSlide_LP
	\brief 本地玩家滑落行为
*/
class ActionSlide_LP
{
public:
	enum EResult
	{
		ER_Unknow,
		ER_Stand,
		ER_OnWater,
		ER_Swim,
	};
public:
	ActionSlide_LP(void);
	virtual ~ActionSlide_LP(void);

	//--
	void Active(const Vector3& start,NavMap* pNav,LocalPlayer* pRole,bool bCanPlayAni);
	void Update(NavMap* pNav,LocalPlayer* pRole,bool bCanPlayAni);
	bool IsClosed(){ return m_bClosed;}
	EResult GetResult(){ return m_result;}
	void ReplayAni(LocalPlayer* pRole);

private:
	NavCollider_Slide		m_collider;
	bool					m_bClosed;
	EResult					m_result;
	float					m_startSlideTime;
	bool					m_bPlayAni;
};
