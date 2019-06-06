#pragma once

class Role;
/** \class UpperRotater
	\brief 上身旋转控制器
*/
class UpperRotater
{
public:
	UpperRotater(Role *pRole);
	~UpperRotater(void);

	/** 开始旋转
	*/
	void Start(float destYaw);
	/** 旋转处理
	*/
	void Update();
	/** 返回当前是否正在旋转中
	*/
	bool IsRotating();
	/** 返回旋转最终弧度
	*/
	float GetDestYaw();

private:
	void GetAniCtrl(void);

private:
	Role	*m_pRole;
	NodeAnimationCtrl *m_pAniCtrl;
	float	m_srcYaw;
	float	m_dstYaw;
	float	m_startTime;
	float	m_totalTime;
	bool	m_bStarted;
	bool	m_bInit;
};
