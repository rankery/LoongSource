#pragma once
#include "..\WorldBase.h"

namespace WorldBase
{
	/**	\class ObjectSpace
		\brief 物体空间定义
	*/
	class ObjectSpace
	{
	public:
		ObjectSpace(void)
			: m_pos(0.0f,0.0f,0.0f)
			, m_scale(1.0f,1.0f,1.0f)
		{	m_yaw=m_pitch=m_roll=0; }

		~ObjectSpace(void)
		{	}

		void SetPos(const Vector3& pos)
		{
			m_pos=pos;
			UpdateMatrix();
		}
		void SetScale(const Vector3& scale)
		{
			m_scale=scale;
			UpdateMatrix();
		}
		void OffsetPos(const Vector3& offset)
		{
			m_pos+=offset;
			UpdateMatrix();
		}
		void SetYaw(float rad)
		{
			m_yaw=rad;
			UpdateMatrix();
		}

		void SetRoll(float roll)
		{
			m_roll=roll;
			UpdateMatrix();
		}

		void SetPitch(float pitch)
		{
			m_pitch=pitch;
			UpdateMatrix();
		}

		const Vector3& GetScale() const		{	return m_scale;		}
		const Vector3& GetPos() const		{	return m_pos;		}
		float GetYaw() const				{	return m_yaw;		}
		float GetPitch() const				{	return m_pitch;		}
		const Matrix4* GetMatrix() const	{	return m_trans.GetMatrix(); }
		
	protected:
		void UpdateMatrix()
		{
			m_trans.LoadIdentity();
			m_trans.Translate(m_pos);
			m_trans.Rotate(m_yaw,m_pitch,m_roll);
			m_trans.Scale(m_scale.x,m_scale.y,m_scale.z);
		}
		Vector3		m_scale;
		Vector3		m_pos;
		float		m_yaw,
					m_pitch,
					m_roll;
		Transform	m_trans;
		
	};
}//namespace GFC