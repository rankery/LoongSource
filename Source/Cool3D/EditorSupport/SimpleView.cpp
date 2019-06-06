#include "StdAfx.h"
#include ".\SimpleView.h"
#include "..\SceneGraph\NullSceneGraph.h"
#include "..\SceneGraph\EntityNode.h"
#include "..\Device\IDevice.h"
#include "..\Kernel.h"

namespace Cool3D
{
	SimpleView::SimpleView(void):m_bkColor(255,120,120,120)
	{
		m_pEntity=NULL;
		//--
		m_pSG=new NullSceneGraph;
		tagDistFog fog;
		fog.density=0;
		m_pSG->SetSceneFog(fog);//¹Ø±Õfog
		m_camera.Set(Vector3(0,0,0),0,0,0,1000);
		m_camera.SetPerspective((float)Device()->GetClientWidth(),(float)Device()->GetClientHeight(),3.14f/4,10,20000);
		m_pSG->BindCamera(&m_camera);
		m_lastRender=0;

		m_DragPt.x=m_DragPt.y=0;
		m_bDrag=m_bRightDrag=false;

	}

	SimpleView::~SimpleView(void)
	{
		SAFE_DELETE(m_pSG);
	}


	void SimpleView::LoadMdl(const TCHAR* szMdlName)
	{
		if(m_pEntity)
		{
			m_pSG->DetachDynamicNode(m_pEntity);
			delete m_pEntity;
			m_pEntity=NULL;
		}

		m_pEntity=new EntityNode;
		m_pEntity->LoadRes(szMdlName);
		m_pSG->AttachDynamicNode(m_pEntity);

		m_lastRender=GetTickCount();
	}

	void SimpleView::Begin()
	{
		Device()->Clear(true,true,true,m_bkColor,1.0f,0);
		Device()->Begin();
	}

	void SimpleView::RenderView(bool bAdaptive)
	{
		if(m_pEntity)
		{
			DWORD delta=GetTickCount()-m_lastRender;
			m_lastRender=GetTickCount();
			//m_pEntity->Update(delta/1000.0f);
			if(bAdaptive)
			{
				const AABBox& box=m_pEntity->GetAABBox();
				m_camera.SetLookAtPos((box.max+box.min)*0.5f);
				m_camera.AdjustAdaptive(box);
			}
		}
		if(m_pSG)
		{
			m_camera.UpdateViewMatrix();
			m_pSG->Render(Kernel::Inst()->GetDeltaTime());
		}
	}

	void SimpleView::End(HWND hWnd)
	{
		Device()->End();
		Device()->Present(hWnd);
	}

	void SimpleView::Render(HWND hWnd,bool bAdaptive)
	{
		if(!::IsWindow(hWnd))
			return;
		Begin();
		RenderView(bAdaptive);
		End(hWnd);
	}

	void SimpleView::OnLButtonDown(UINT nFlags, POINT point)
	{
		m_bDrag=TRUE;
		m_DragPt=point;
	}

	void SimpleView::OnLButtonUp(UINT nFlags, POINT point)
	{
		m_bDrag=FALSE;
	}

	void SimpleView::OnMouseMove(UINT nFlags, POINT point)
	{
		POINT pt=point;
		float fx=float(pt.x-m_DragPt.x);
		float fy=float(pt.y-m_DragPt.y);
		m_DragPt=pt;

		if(m_bDrag)
		{
			m_camera.SetYaw(m_camera.GetYaw()+_DegToRad(-fx*0.15f));
			m_camera.SetPitch(m_camera.GetPitch()+_DegToRad(-fy*0.15f));	
		}
		if(m_bRightDrag)
		{
			Vector3 offset=m_camera.GetForward()*fy*2;
			offset+=m_camera.GetSide()*fx*2;
			offset.y=0;
			m_camera.Move(offset);
		}
	}

	void SimpleView::OnRButtonDown(UINT nFlags, POINT point)
	{
		m_bRightDrag=TRUE;
		m_DragPt=point;
	}

	void SimpleView::OnRButtonUp(UINT nFlags, POINT point)
	{
		m_bRightDrag=FALSE;
	}

	void SimpleView::OnMouseWheel(UINT nFlags, short zDelta, POINT pt)
	{
		float dist=m_camera.GetDist();
		float delta=(zDelta/500.0f*dist);
		m_camera.SetDist(dist+delta);
	}
}//namespace Cool3D