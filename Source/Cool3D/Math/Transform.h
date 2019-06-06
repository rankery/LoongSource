#pragma once
#include "..\Cool3D.h"
#include "Math.h"

namespace Cool3D
{
	/**	\class Transform
		\brief 变换工具,可以计算一系列变换
		\remarks
		例如:Transform t;
		t.RotateAxis('x',3.1415926f);
		t.Translate(Vector3(0,10,0));
		t.TransVec3(v);
		相当于将v先平移,然后再旋转.
	*/
	class Cool3D_API Transform
	{
	public:
		enum ECombineMode
		{
			ECM_ToLeft,
			ECM_ToRight
		};
	public:
		Transform(void);
		Transform(ECombineMode mode);
		~Transform(void);

		/**	将Transform置成单位矩阵,即没有任何变换		*/
		void LoadIdentity();
		/**	增加一个移动		*/
		void Translate(const Vector3& offset);
		/**	增加旋转		*/
		void Rotate(float yaw,float pitch,float roll);
		/**	增加绕某个轴旋转
			\param axis是字母x,y,z之一,rad是旋转角度,角度为正数则为逆时针方向旋转
		*/
		void RotateAxis(char axis,float rad);
		/**	增加绕任意轴旋转		*/
		void RotateAxis(const Vector3& axis,float rad);
		/** 添加四元数旋转 */
		void RotateQuat(const Quaternion& q);
		/**	增加缩放		*/
		void Scale(float x,float y,float z);

		/** 在当前的变换中加入一个变换		*/
		void CombineTrans(const Matrix4* pMatrix);
		void CombineTransRight(const Matrix4* pMatrix);
		/**	使用一个矩阵替换当前的矩阵		*/
		void LoadMatrix(const Matrix4* pMatrix);
		
		void TransVec3(Vector3& v);
		const Matrix4* GetMatrix() const	{	return &m_mat;}
	private:
		Matrix4	m_mat;
		ECombineMode m_mode;
	};
}//namespace Cool3D