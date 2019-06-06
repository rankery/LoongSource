#include "StdAfx.h"
#include ".\arcball.h"

namespace Cool3D
{
	ArcBall::ArcBall(void)
	{
		m_wndWidth=800;
		m_wndHeight=600;
		m_radius=1.8f;

		m_bMousePressed=false;
		D3DXQuaternionIdentity(&m_quat);
		D3DXQuaternionIdentity(&m_lastQuat);
	}

	ArcBall::~ArcBall(void)
	{
		D3DXQuaternionIdentity(&m_quat);
		D3DXQuaternionIdentity(&m_lastQuat);
	}

	void ArcBall::ClearRotate()
	{}

	/**	把一个屏幕坐标映射到ArcBall上(指球面上)
		\remarks ArcBall是一个球心为(0,0,0),半径为m_radius的球
	*/
	void ArcBall::MapToBall(UINT x,UINT y,Vector3 *pOut)
	{
		//缩放到归一化屏幕坐标系
		float nx = (x-m_wndWidth/2)/(m_wndWidth/2);
		float ny =  (y-m_wndHeight/2)/(m_wndHeight/2);

		//scale
		nx/=m_radius;
		ny/=m_radius;

		//
		float len2=nx*nx+ny*ny;
		if(len2 > 1.0f)//outside of sphere 
		{
			float s=1.0f/sqrtf(len2);
			pOut->x=nx*s;
			pOut->y=ny*s;
			pOut->z=0.0f;
		}
		else
		{
			//x^2+y^2+z^2=r^2
			pOut->x=nx;
			pOut->y=ny;
			pOut->z=-sqrtf(1.0f-len2);
		}
	}

	void ArcBall::SetWindowSize(UINT w,UINT h)
	{
		m_wndWidth=float(w);
		m_wndHeight=float(h);
	}
	void ArcBall::OnMouseBtnDown(UINT x,UINT y)
	{
		m_bMousePressed=true;
		MapToBall(x,y,&m_savedPt);
	}

	void ArcBall::OnMouseMove(UINT x,UINT y)
	{
		if(!m_bMousePressed)
			return;
		
		Vector3 dragPt;
		MapToBall(x,y,&dragPt);

		Vector3 perp;
		D3DXVec3Cross(&perp,&m_savedPt,&dragPt);

		float len=D3DXVec3Length(&perp);

		if(len > FLOAT_EPSILON)
		{
			m_quat.x=perp.x;
			m_quat.y=perp.y;
			m_quat.z=perp.z;
			m_quat.w=D3DXVec3Dot(&m_savedPt,&dragPt);
		}
		else
		{
			D3DXQuaternionIdentity(&m_quat);
		}

		dragPt=m_savedPt;
	}

	void ArcBall::OnMouseBtnUp()
	{
		m_bMousePressed=false;
		m_lastQuat=m_lastQuat*m_quat;
		D3DXQuaternionIdentity(&m_quat);
	}

	void ArcBall::GetRotateMat(Matrix4 *pOut)
	{
		Quaternion quat;
		quat=m_lastQuat*m_quat;
		D3DXMatrixRotationQuaternion(pOut,&quat);
	}
}//namespace Cool3D