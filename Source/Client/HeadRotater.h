#pragma once

class Role;
/** \class HeadRotater
	\brief 上身旋转控制器
*/
class HeadRotater
{
public:
	HeadRotater(Role* pRole);
	~HeadRotater(void);

	/** 开始旋转
	*/
	void Start(float destYaw,float destPitch);
	/** 旋转处理
	*/
	void Update(void);
	/** 返回当前是否正在旋转中
	*/
	bool IsRotating(void)		const;
	/** 返回旋转最终弧度
	*/
	float GetDestYaw(void)		const	{ return m_dstYaw; }
	float GetDestPitch(void)	const	{ return m_dstPitch; }

private:
	void GetAniCtrl(void);

private:
	Role	*m_pRole;
	NodeAnimationCtrl *m_pAniCtrl;
	float	m_srcYaw;
	float	m_dstYaw;
	float	m_srcPitch;
	float	m_dstPitch;
	float	m_yawStartTime;
	float	m_yawTotalTime;
	bool	m_bYawStarted;
	bool	m_bPitchStarted;
	float	m_pitchStartTime;
	float	m_pitchTotalTime;
	bool	m_bInit;
};
