#pragma once

class Role;
class SGDynamicProjector;

class ShadowPool
{
public:
	static int OP_MAX_SHADOW;//最多几个影子
	static int OP_SHADOW_SIZE;//贴图的大小
	static float OP_MAX_DIST;
public:
	ShadowPool(void);
	virtual ~ShadowPool(void);

	void CreateData();
	bool IsCreated() const	{	return m_shadows.size()>0;}
	void Destroy();
	void AssignToRoles();
private:
	vector<SGDynamicProjector*>	m_shadows;
	bool CanAttach( Role* pRole, const Frustum& frustum );
};
