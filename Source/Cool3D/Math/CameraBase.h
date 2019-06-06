
#pragma once
#include "..\Cool3D.h"
#include "math.h"
#include "Ray.h"

namespace Cool3D
{
	/**	\class CameraBase
		\brief A basic Camera class
		\par 一个基本的摄像机类, 提供基本的运算功能, 外部可以派生来增加功能
	*/
	class Cool3D_API CameraBase
	{
	public:
		CameraBase(void);
		virtual ~CameraBase(void);

		void SetLookAt(const Vector3& from,const Vector3& lookat);
		void SetLookAt(const Vector3& from,const Vector3& lookat,const Vector3& up);
		void SetLookAt(const Vector3& from,const Vector3& lookat,int face);
		void SetPerspective(float clientw,float clienth,float fov_rad,float znear,float zfar);
		void SetOrtho(float clientw,float clienth,float znear,float zfar);
		/**	调整view和projection矩阵,使得最后的图像正好充满render target*/
		void AdjustAdaptive(const AABBox& box);

		Matrix4*	GetView() 		{ return &m_matView; }
		Matrix4*	GetPrj()		{ return &m_matPrj; }
		void GetPrjZBias(float w,Matrix4& out)
		{
			out=m_matPrj;
			out._33*=(1+w);
		}
		void GetPrjZBias(float delta,const Vector3& refPos,Matrix4& out);

		const Vector3&	GetPos() const		{	return m_vFrom; }
		const Vector3& GetLookat() const	{	return m_vLookat; }
		const Vector3& GetUp() const	{	return m_vUp;}
		Vector3 GetDirection() const
		{	
			Vector3 ret=m_vLookat-m_vFrom;
			D3DXVec3Normalize(&ret,&ret);
			return ret;
		}
		float GetZNear() const	{ return m_fZNear; }
		float GetZFar() const	{ return m_fZFar; }
		float GetFOV() const	{ return m_fFov;}

		//--
		POINT	WorldToScreen(const Vector3& wpt) const;
		Vector3	WorldToScreenF(const Vector3& wpt) const;
		POINT	ScreenFToScreen(const Vector3& spt) const;
		Vector3 ScreenToWorld(UINT screenX,UINT screenY) const;
		void GetMouseRay(UINT cursorX,UINT cursorY,Ray& out) const;

		Matrix4* GetPrj(float zFar,Matrix4& out);
		Matrix4* GetPrj(float zNear,float zFar,Matrix4& out);

		void SetZFar(float zFar);
		void SetZNear(float zNear);

	protected:
		void GetViewSizeOnNearPlane(float& halfW,float& halfH) const;
		Matrix4& MatrixCubeMapView(int face,Matrix4& matView);
	protected:
		Matrix4	m_matView;
		Matrix4	m_matInvView;

		Matrix4	m_matPrj;
		Matrix4 m_matInvPrj;

		//--
		float m_clientW;
		float m_clientH;

		float	m_fFov;
		float	m_fZNear;
		float	m_fZFar;

		//--
		Vector3 m_vFrom;
		Vector3 m_vLookat;
		Vector3 m_vUp;
	};
}//endof namespace Cool3D
