#include "stdafx.h"
#include ".\cameraeuler.h"

namespace Cool3D
{
	CameraEuler::CameraEuler(void):m_vForward(0,0,1),m_vSide(1,0,0)
	{
		m_fYaw=0.0f;
		m_fPitch=0.0f;
		m_fRoll=0.0f;

		m_fDist=100.0f;
	}

	CameraEuler::~CameraEuler(void)
	{
	}

	void CameraEuler::Set(const Vector3& lookat,float yaw,float pitch,float roll,float dist)
	{
		m_vLookat=lookat;
		m_fYaw=yaw;
		m_fPitch=pitch;
		m_fRoll=roll;
		m_fDist=dist;

		UpdateViewMatrix();
	}

	void CameraEuler::Move(const Vector3& offset)
	{
		m_vLookat+=offset;
	}

	void CameraEuler::SetLookAtPos(const Vector3& lookat)
	{
		m_vLookat=lookat;
	}

	void CameraEuler::UpdateViewMatrix()
	{
		float cosY, cosP, cosR;
		float sinY, sinP, sinR;

		// Only Want to Calc these once
		cosY = cosf(m_fYaw);
		cosP = cosf(m_fPitch);
		cosR = cosf(m_fRoll);
		sinY = sinf(m_fYaw);
		sinP = sinf(m_fPitch);
		sinR = sinf(m_fRoll);

		// Fwd Vector
		m_vForward.x = sinY * cosP;
		m_vForward.y = sinP;
		m_vForward.z = cosP * cosY;

		m_vFrom=m_vLookat-(m_vForward*m_fDist);

		// Up Vector
		m_vUp.x = -cosY * sinR - sinY * sinP * cosR;
		m_vUp.y = cosP * cosR;
		m_vUp.z = -sinY * sinR - sinP * cosR * cosY;

		//side
		D3DXVec3Cross(&m_vSide,&m_vForward,&m_vUp);

		//matrix
		CameraBase::SetLookAt(m_vFrom,m_vLookat,m_vUp);
	}

}//endof namespace Cool3D
