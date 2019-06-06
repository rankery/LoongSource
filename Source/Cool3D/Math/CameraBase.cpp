#include "stdafx.h"
#include ".\camerabase.h"

namespace Cool3D
{
	CameraBase::CameraBase(void)
	{
		D3DXMatrixIdentity(&m_matView);
		D3DXMatrixIdentity(&m_matInvView);
		D3DXMatrixIdentity(&m_matPrj);
		D3DXMatrixIdentity(&m_matInvPrj);

		m_clientW=m_clientH=400;
		m_fFov=30.0f;
		m_fZNear=10.0f;
		m_fZFar=1000.0f;
	}

	CameraBase::~CameraBase(void)
	{
	}
	
	void CameraBase::SetLookAt(const Vector3& from,const Vector3& lookat,const Vector3& up)
	{
		m_vFrom=from;
		m_vLookat=lookat;
		m_vUp=up;
		D3DXMatrixLookAtLH(&m_matView,&m_vFrom,&m_vLookat,&m_vUp);
		D3DXMatrixInverse(&m_matInvView,NULL,&m_matView);
	}

	void CameraBase::SetLookAt(const Vector3& from,const Vector3& lookat)
	{
		Vector3 up;
		Vector3 dir=lookat-from;
		Vector3 right;
		if(fabsf(dir.x)<FLOAT_EPSILON
			&& fabsf(dir.z)<FLOAT_EPSILON
			&& dir.y<0.0f)
		{
			up=MathConst::AxisZ;
		}
		else
		{
			D3DXVec3Cross(&right,&MathConst::AxisY,&dir);
			D3DXVec3Cross(&up,&dir,&right);
		}

		SetLookAt(from,lookat,up);
	}

	void CameraBase::SetLookAt(const Vector3& from,const Vector3& lookat,int face)
	{
		SetLookAt(from,lookat);

		Matrix4 matFaceView;
		m_matView*=MatrixCubeMapView(face,matFaceView);

		D3DXMatrixInverse(&m_matInvView,NULL,&m_matView);
	}

	void CameraBase::SetPerspective(float clientw,float clienth,float fov_rad,float znear,float zfar)
	{
		float aspect=clientw/clienth;
		D3DXMatrixPerspectiveFovLH(&m_matPrj,fov_rad,aspect,znear,zfar);
		D3DXMatrixInverse(&m_matInvPrj,NULL,&m_matPrj);

		m_clientW=clientw;
		m_clientH=clienth;
		m_fFov=fov_rad;
		m_fZNear=znear;
		m_fZFar=zfar;
	}

	void CameraBase::SetOrtho(float clientw,float clienth,float znear,float zfar)
	{
		D3DXMatrixOrthoLH(&m_matPrj,clientw,clienth,znear,zfar);
		D3DXMatrixInverse(&m_matInvPrj,NULL,&m_matPrj);

		m_clientW=clientw;
		m_clientH=clienth;
		m_fFov=0.0f;
		m_fZNear=znear;
		m_fZFar=zfar;
	}

	void CameraBase::AdjustAdaptive(const AABBox& box)
	{
		Vector3 center=(box.max+box.min)*0.5f;
		SetLookAt(m_vFrom,center,m_vUp);

		Vector3 corners[8];
		box.GetCorners(corners);

		int i;
		for(i=0;i<8;i++)
		{
			corners[i]=corners[i]*m_matView;
		}

		//--change xsacle,yscale
		//See Game Programming Gems,5.8 Real-time shadow on complex Objects,Adaptive Projection
		if(m_fFov>0)//透视投影
		{
			float rx=-FLOAT_MAX,ry=-FLOAT_MAX;
			float r;
			for(i=0;i<8;i++)
			{
				r=corners[i].x/corners[i].z;
				if(r>rx)
					rx=r;
				r=corners[i].y/corners[i].z;
				if(r>ry)
					ry=r;
			}

			m_matPrj._11=1.0f/rx;
			m_matPrj._22=1.0f/ry;
		}
		else//平行投影
		{
			float maxX=-FLOAT_MAX,maxY=-FLOAT_MAX;
			float r=m_fZNear/(m_fZNear-m_fZFar);
			float t;
			//找出经过Project矩阵计算出来的max x,y
			for(i=0;i<8;i++)
			{
				t=corners[i].x*(2.0f/m_clientW)/(r*corners[i].z+1.0f);
				if(t>maxX)
					maxX=t;
				t=corners[i].y*(2.0f/m_clientH)/(r*corners[i].z+1.0f);
				if(t>maxY)
					maxY=t;
			}

			m_matPrj._11=2.0f/m_clientW/maxX;
			m_matPrj._22=2.0f/m_clientH/maxY;
		}
		
		D3DXMatrixInverse(&m_matInvPrj,NULL,&m_matPrj);
	}

	Vector3 CameraBase::WorldToScreenF(const Vector3& wpt) const
	{
		Vector4 trans(wpt.x,wpt.y,wpt.z,1.0f);
		D3DXVec4Transform(&trans,&trans,&m_matView);
		D3DXVec4Transform(&trans,&trans,&m_matPrj);
		trans/=trans.w;
		return Vector3(trans.x,trans.y,trans.z);
	}

	POINT	CameraBase::WorldToScreen(const Vector3& wpt) const
	{
		Vector3 trans=WorldToScreenF(wpt);

		POINT ret;
		ret.x=LONG((trans.x+1.0f)*0.5f*m_clientW);
		ret.y=LONG(-(trans.y-1.0f)*0.5f*m_clientH);
		return ret;
	}

	POINT	CameraBase::ScreenFToScreen(const Vector3& spt) const
	{
		POINT ret;
		ret.x=LONG((spt.x+1.0f)*0.5f*m_clientW);
		ret.y=LONG(-(spt.y-1.0f)*0.5f*m_clientH);
		return ret;
	}

	void CameraBase::GetViewSizeOnNearPlane(float& halfW,float& halfH) const
	{
		float aspect=m_clientW/m_clientH;
		halfW=tanf(m_fFov*0.5f)*m_fZNear*aspect;
		halfH=halfW/aspect;
	}
	Vector3 CameraBase::ScreenToWorld(UINT screenX,UINT screenY) const
	{
		float hw,hh;
		GetViewSizeOnNearPlane(hw,hh);
		Vector3 r(screenX/(m_clientW*0.5f)-1.0f,
			1.0f-screenY/(m_clientH*0.5f),m_fZNear);
		r.x*=hw;
		r.y*=hh;
		Vector4 ret;
		D3DXVec3Transform(&ret,&r,&m_matInvView);

		return Vector3(ret.x,ret.y,ret.z);
	}
	void CameraBase::GetMouseRay(UINT cursorX,UINT cursorY,Ray& out) const
	{
		out.origin=ScreenToWorld(cursorX,cursorY);
		out.dir=out.origin-m_vFrom;	//从From点出发,指向"鼠标所指向的点"的方向
		D3DXVec3Normalize(&out.dir,&out.dir);
		out.origin+=out.dir*m_fZNear;//进行near剪裁
	}

	void CameraBase::GetPrjZBias(float delta,const Vector3& refPos,Matrix4& out)
	{
		Vector3 wPos=WorldToScreenF(refPos);

		float epsilon=2.0f*m_fZFar*m_fZNear*delta/((m_fZFar+m_fZNear)*wPos.z*(wPos.z+delta));
		out=m_matPrj;
		out._33*=(1+epsilon);
	}

	Matrix4* CameraBase::GetPrj(float zFar,Matrix4& out)
	{
		if(m_fFov>0)//透视投影
		{
			float aspect=m_clientW/m_clientH;
			D3DXMatrixPerspectiveFovLH(&out,m_fFov,aspect,m_fZNear,zFar);
		}
		else//平行投影
		{
			D3DXMatrixOrthoLH(&out,m_clientW,m_clientH,m_fZNear,zFar);
		}
		return &out;
	}

	Matrix4* CameraBase::GetPrj(float zNear,float zFar,Matrix4& out)
	{
		if(m_fFov>0)//透视投影
		{
			float aspect=m_clientW/m_clientH;
			D3DXMatrixPerspectiveFovLH(&out,m_fFov,aspect,zNear,zFar);
		}
		else//平行投影
		{
			D3DXMatrixOrthoLH(&out,m_clientW,m_clientH,zNear,zFar);
		}
		return &out;
	}

	Matrix4& CameraBase::MatrixCubeMapView(int face,Matrix4& matView)
	{
		// Standard view that will be overridden below.
		Vector3 vFrom(0.0f,0.0f,0.0f);
		Vector3 vLookat,vUp;

		switch(face)
		{
		case 0://x:
			vLookat = Vector3( 1.0f, 0.0f, 0.0f );
			vUp		= Vector3( 0.0f, 1.0f, 0.0f );
			break;
		case 1://-x:
			vLookat = Vector3(-1.0f, 0.0f, 0.0f );
			vUp		= Vector3( 0.0f, 1.0f, 0.0f );
			break;
		case 2://y:
			vLookat = Vector3( 0.0f, 1.0f, 0.0f );
			vUp		= Vector3( 0.0f, 0.0f,-1.0f );
			break;
		case 3://-y:
			vLookat = Vector3( 0.0f,-1.0f, 0.0f );
			vUp		= Vector3( 0.0f, 0.0f, 1.0f );
			break;
		case 4://z:
			vLookat = Vector3( 0.0f, 0.0f, 1.0f );
			vUp		= Vector3( 0.0f, 1.0f, 0.0f );
			break;
		case 5://-z:
			vLookat = Vector3( 0.0f, 0.0f,-1.0f );
			vUp		= Vector3( 0.0f, 1.0f, 0.0f );
			break;
		}	
		D3DXMatrixLookAtLH(&matView, &vFrom, &vLookat, &vUp);
		return matView;
	}

	void CameraBase::SetZFar(float zFar)
	{
		if(m_fFov>0)//透视投影
		{
			SetPerspective(m_clientW,m_clientH,m_fFov,m_fZNear,zFar);
		}
		else//平行投影
		{
			SetOrtho(m_clientH,m_clientW,m_fZNear,zFar);
		}
	}

	void CameraBase::SetZNear( float zNear )
	{
		if(m_fFov>0)//透视投影
		{
			SetPerspective(m_clientW,m_clientH,m_fFov,zNear,m_fZFar);
		}
		else//平行投影
		{
			SetOrtho(m_clientH,m_clientW,zNear,m_fZFar);
		}
	}
}//endof namespace Cool3D
