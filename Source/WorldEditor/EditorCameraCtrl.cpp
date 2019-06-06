#include "StdAfx.h"
#include ".\editorcameractrl.h"
#include "Cool3DView.h"


static const float G_MOVERATEMIN=5.0f;
static const float G_MOVERATEMAX=40.0f;
static const float G_CAMERANEARDIST=100.0f;
static const float G_CAMERAFARDIST=2000.0f*50.0f;

EditorCameraCtrl::EditorCameraCtrl(void)
{
	m_pCamera=NULL;
	m_bDrag=FALSE;
	m_bRightDrag=FALSE;
	m_fMoveRate = 5.0f;
	m_accelerPerDist=(G_MOVERATEMAX-G_MOVERATEMIN)/(G_CAMERAFARDIST-G_CAMERANEARDIST);
}

EditorCameraCtrl::~EditorCameraCtrl(void)
{
}

void EditorCameraCtrl::AttachEngineInterface(void *pInterface,void *pExInterface)
{
	m_pCamera=(EditorCamera*)pInterface;
	m_dlg.m_pCamera = m_pCamera;
}

const TCHAR* EditorCameraCtrl::GetUserName()
{
	return _T("编辑器镜头控制");
}
const TCHAR* EditorCameraCtrl::GetClassName()
{
	return _T("EditorCameraCtrl");
}

void EditorCameraCtrl::Create(DWORD pParentWnd)
{
	m_dlg.Create(IDD_CameraCtrl);
}

void EditorCameraCtrl::Destroy()
{
	m_pCamera=NULL;
}
void EditorCameraCtrl::ShowWindow(bool bShow)
{
	if(!::IsWindow(m_dlg.GetSafeHwnd()))
		return;
	if(bShow)
		m_dlg.ShowWindow(SW_SHOW);
	else
		m_dlg.ShowWindow(SW_HIDE);
}

void EditorCameraCtrl::OnLButtonDown(UINT nFlags, UINT x,UINT y)
{
	m_bDrag=TRUE;
	m_DragPt.x=x;
	m_DragPt.y=y;
}

void EditorCameraCtrl::OnLButtonUp(UINT nFlags, UINT x,UINT y)
{
	m_bDrag=FALSE;
}

void EditorCameraCtrl::OnMouseMove(UINT nFlags, UINT x,UINT y)
{
	if(m_pCamera==NULL)
		return;
	
	CPoint pt(x,y);
	float fx=float(pt.x-m_DragPt.x);
	float fy=float(pt.y-m_DragPt.y);
	m_DragPt=pt;

	if(m_bDrag)
	{
		m_pCamera->SetYaw(m_pCamera->GetYaw()+_DegToRad(-fx*0.15f));
		m_pCamera->SetPitch(m_pCamera->GetPitch()+_DegToRad(-fy*0.15f));	
	}
	if(m_bRightDrag)
	{
		Vector3 offset=m_pCamera->GetForward()*fy*m_fMoveRate;
		offset+=m_pCamera->GetSide()*fx*m_fMoveRate;
		offset.y=0;
		m_pCamera->Move(offset);
	}
}

void EditorCameraCtrl::OnMouseWheel(UINT nFlags, short zDelta, UINT x,UINT y)
{
	float dist=m_pCamera->GetDist();
	float fSlider = m_dlg.GetSlider();
	float delta=(zDelta/fSlider*dist);

	float newDist=dist+delta;
	m_pCamera->SetDist(newDist);

	//--根据镜头的远近计算鼠标的移动速率
	m_fMoveRate=(newDist-G_CAMERANEARDIST)*m_accelerPerDist+G_MOVERATEMIN;
	if(m_fMoveRate<G_MOVERATEMIN)
		m_fMoveRate=G_MOVERATEMIN;
	else if(m_fMoveRate>G_MOVERATEMAX)
		m_fMoveRate=G_MOVERATEMAX;
}

void EditorCameraCtrl::OnRButtonDown(UINT nFlags, UINT x,UINT y)
{
	m_bRightDrag=TRUE;
	m_DragPt.x=x;
	m_DragPt.y=y;
}
void EditorCameraCtrl::OnRButtonUp(UINT nFlags, UINT x,UINT y)
{
	m_bRightDrag=FALSE;
}