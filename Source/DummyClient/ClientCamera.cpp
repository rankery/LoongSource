#include "StdAfx.h"
#include ".\clientcamera.h"
#include "ClientApp.h"
#include "BuiltMapView.h"
#include ".\Win32App\GInput.h"

ClientCamera::ClientCamera(void):m_targetLookAt(0,0,0)
{
	m_bFly=false;
}

ClientCamera::~ClientCamera(void)
{
}

void ClientCamera::SetDefault()
{
	Set(Vector3(500,500,500),0,_DegToRad(45),0,700);
	SetPerspective((float)Device()->GetClientWidth(),(float)Device()->GetClientHeight()
		,_DegToRad(45),10,BuiltMapView::ViewDistBN);
}

float angle=25;
float n=200.0;

void ClientCamera::Update(const Vector3& nextLookAt)
{
	float deltatime=Kernel::Inst()->GetDeltaTime();
	if(!m_bFly)
	{
		m_targetLookAt=nextLookAt;

		const float springConst=4.6f;  // Hooke's Constant
		const float springLen=20.0f;
		Vector3 disp=m_vLookat-m_targetLookAt;
		float displen=Vec3Len(&disp);		

		if(displen > 0.05f)
		{
			Vector3 olddir;
			// Calculate Spring Force
			float force=springConst * (springLen - displen) ;
			// Apply Spring Force
			disp /= displen;
			olddir=disp;
			disp *= (force * deltatime);
			m_vLookat+=disp;

			Vector3 newdir=m_vLookat-m_targetLookAt;
			Vec3Normalize(newdir);
			if(D3DXVec3Dot(&olddir,&newdir)<=0.0f)
				m_vLookat=m_targetLookAt;
		}
	}

	if(!g_con.IsActive())
	{
		if (GInput::Inst()->KeyPressed(DIK_M))
		{
			angle++;
			n--;
			SetPerspective((float)Device()->GetClientWidth(),(float)Device()->GetClientHeight()
				,_DegToRad(angle),n,BuiltMapView::ViewDistBN);
		}
		else if (GInput::Inst()->KeyPressed(DIK_N))
		{
			angle--;
			n++;
			SetPerspective((float)Device()->GetClientWidth(),(float)Device()->GetClientHeight()
				,_DegToRad(angle),n,BuiltMapView::ViewDistBN);
		}
		else if (GInput::Inst()->KeyPressed(DIK_B))
		{
			SetDefault();
		}

		if(m_bFly)
		{
			float speed=3200*deltatime;
			Vector3 offset(0,0,0);
			if(GInput::Inst()->KeyPressed(DIK_W))//向前
			{
				offset+=GetForward()*speed;
			}
			else if(GInput::Inst()->KeyPressed(DIK_S))
			{
				offset+=GetForward()*-speed;
			}

			if(GInput::Inst()->KeyPressed(DIK_A))//向左
			{
				offset+=GetSide()*speed;
			}
			else if(GInput::Inst()->KeyPressed(DIK_D))
			{
				offset+=GetSide()*-speed;
			}

			Move(offset);
			m_vLookat=GetLookat();
		}
	}

	//--鼠标滚轮
	if(GInput::Inst()->GetRelativeZ()!=0 && !m_bFly)
	{
		float delta= -GInput::Inst()->GetRelativeZ();
		float newDist=GetDist()+delta;

		if(newDist>18180)
			newDist=18180;

		if(newDist<100)
			newDist=100;

		SetDist(newDist);
	}


	// 按下鼠标右键瞬间
	if( GInput::Inst()->MouseDownFirst(1) )
	{
		::SetCapture(g_hWnd);
		::GetCursorPos(&m_ptLast);
		::ShowCursor(FALSE);
	}

	// 松开鼠标右键瞬间
	if( GInput::Inst()->MouseUp(1) )
	{
		::ReleaseCapture();
		::ShowCursor(TRUE);
	}
	
	//--按下鼠标右键并拖动
	if(GInput::Inst()->MouseDown(1) && GInput::Inst()->IsMouseMove())
	{
		float yaw = GetYaw() - (-GInput::Inst()->GetRelativeX()*1.0f) * 0.6f * 0.006545f;
		float pitch = GetPitch() + (-GInput::Inst()->GetRelativeY()*1.0f) * 1.0f * 0.006545f;

		if( pitch < -FLOAT_PI*0.45f )
			pitch = -FLOAT_PI*0.45f;

		if( pitch>_DegToRad(80) )
			pitch=_DegToRad(80);

		SetYaw(yaw);
		SetPitch(pitch);
		::SetCursorPos(m_ptLast.x, m_ptLast.y);
	}

	UpdateViewMatrix();
}