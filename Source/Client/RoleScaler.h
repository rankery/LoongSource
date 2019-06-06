#pragma once

class Role;
/** \class RoleScaler
	\brief 角色缩放控制器
*/
class RoleScaler
{
public:
	RoleScaler(void);
	~RoleScaler(void);

	/** 绑定角色
	*/
	void BindRole(Role* pRole)	{ m_pRole=pRole;}
	/** 开始缩放
	*/
	void Start(const Vector3& destScale);
	/** 缩放处理
	*/
	void Update();
	/** 返回当前是否正在缩放中
	*/
	bool IsScaling() const;
	/** 返回最终缩放
	*/
	const Vector3& GetDestScale() const;

private:
	Role* m_pRole;
	Vector3 m_srcScale;
	Vector3 m_dstScale;
	float m_startTime;
	float m_totalTime;
	bool  m_bStarted;
};
