#pragma once

class Role;
/** \class RoleRotater
	\brief 角色旋转控制器
*/
class RoleRotater
{
public:
	enum EType
	{
		ET_None = -1,
		ET_Pitch,
		ET_Yaw,
	};
	RoleRotater(void);
	~RoleRotater(void);

	/** 绑定角色
	*/
	void BindRole(Role* pRole,EType type,bool setTime,float rotateSpeed);
	/** 开始旋转
	*/
	void Start(float destAngle);
	/** 旋转处理
	*/
	void Update();
	/** 返回当前是否正在旋转中
	*/
	bool IsRotating();
	/** 返回旋转最终弧度
	*/
	float GetDestAngle();

private:
	bool  m_bSetTime;     // true: m_param为旋转时间 false: m_param为旋转速度
	float m_param;
	EType m_type;
	Role* m_pRole;
	float m_srcAngle;
	float m_dstAngle;
	float m_startTime;
	float m_totalTime;
	bool  m_bStarted;
};
