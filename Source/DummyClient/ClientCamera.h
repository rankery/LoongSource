#pragma once
#include "DummyClient.h"

class ClientCamera :
	public CameraEuler
{
public:
	ClientCamera(void);
	virtual ~ClientCamera(void);

	void SetDefault();
	void Update(const Vector3& nextLookAt);
	void EnableFlyMode(bool enable)	{	m_bFly=enable; SetDist(100); }

private:
	Vector3 m_targetLookAt;
	bool	m_bFly;
	POINT m_ptLast;
};
