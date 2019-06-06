#include "..\Cool3D.h"
#include "camerabase.h"
#include "math.h"

#pragma once


namespace Cool3D
{
	/**	\class CameraEuler
		\brief 使用euler角度计算摄像机信息
	*/
	class Cool3D_API CameraEuler :	public CameraBase
	{
	public:
		CameraEuler(void);
		virtual ~CameraEuler(void);

		float GetYaw()
		{ return m_fYaw; }
		void SetYaw(float fYaw)
		{
			m_fYaw=fYaw;
		}

		float GetDist()
		{ return m_fDist; }
		void SetDist(float dist)
		{ m_fDist=dist;
		}

		float GetPitch()
		{ return m_fPitch;	}
		void SetPitch(float fPitch)
		{
			m_fPitch=fPitch;
		}
		float GetRoll()	{return m_fRoll;}
		void SetRoll(float fRoll) { m_fRoll=fRoll;}

		//允许派生类重写此函数,这样客户端可以进行camera不进入物体内部等等检测
		virtual void UpdateViewMatrix();
		void Set(const Vector3& lookat,float yaw,float pitch,float roll,float dist);
		void Move(const Vector3& offset);
		void SetLookAtPos(const Vector3& lookat);

		const Vector3& GetForward()
		{ return m_vForward; }
		const Vector3& GetSide()
		{ return m_vSide; }
		const Vector3& GetUp()
		{ return m_vUp; }

	protected:
		float m_fYaw;
		float m_fPitch;
		float m_fRoll;

		float m_fDist;//from点与lookat点之间的距离,只用来已知lookat点求from点的情况

		//--camera space axis
		Vector3 m_vForward;
		Vector3 m_vSide;
		//m_vUp decl in CameraBase
	};
}//endof namespace Cool3D
