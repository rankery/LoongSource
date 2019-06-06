// ChildView.cpp : CChildView 类的实现
//

#include "stdafx.h"
#include "WorldEditor.h"
#include "ChildView.h"
#include ".\childview.h"
#include "PluginMgr.h"
#include "Cool3DView.h"
#include "WorldEditorMgr.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CChildView

CChildView::CChildView()
{
	m_bDisRole = false;
}

CChildView::~CChildView()
{
}


BEGIN_MESSAGE_MAP(CChildView, CWnd)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_DESTROY()
	ON_COMMAND(ID_32774, On32774)
	ON_COMMAND(ID_32775, On32775)
	ON_COMMAND(ID_32776, On32776)
	ON_COMMAND(ID_DISROLE, OnID_DIsRole)
	ON_UPDATE_COMMAND_UI(ID_DISROLE, OnUpdateDisRole)
END_MESSAGE_MAP()



// CChildView 消息处理程序

BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(NULL, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW+1), NULL);

	//--
	RECT rc;
	rc.left=0;
	rc.right=800;
	rc.top=0;
	rc.bottom=600;
	AdjustWindowRectEx(&rc,cs.style,FALSE,cs.dwExStyle);
	cs.x=rc.left;
	cs.y=rc.top;
	cs.cx=rc.right-rc.left;
	cs.cy=rc.bottom-rc.top;

	return TRUE;
}

void CChildView::OnPaint() 
{
	CPaintDC dc(this); // 用于绘制的设备上下文
	
	// TODO: 在此处添加消息处理程序代码
	
	// 不要为绘制消息而调用 CWnd::OnPaint()
	Cool3DView::Inst()->Render();
}


int CChildView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	CRect rc;
	GetClientRect(rc);

	return 0;
}

BOOL CChildView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	return TRUE;
	//return CWnd::OnEraseBkgnd(pDC);
}

void CChildView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if(nFlags&MK_CONTROL)
		PluginMgr::Inst()->GetCameraCtrl()->OnLButtonDown(nFlags,point.x,point.y);
	else
		PluginMgr::Inst()->GetActive()->OnLButtonDown(nFlags,point.x,point.y);
	SetCapture();
	

	CWnd::OnLButtonDown(nFlags, point);
}

void CChildView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if(nFlags&MK_CONTROL)
		PluginMgr::Inst()->GetCameraCtrl()->OnLButtonUp(nFlags,point.x,point.y);
	else
		PluginMgr::Inst()->GetActive()->OnLButtonUp(nFlags,point.x,point.y);
	ReleaseCapture();

	CWnd::OnLButtonUp(nFlags, point);
}

void CChildView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if(nFlags&MK_CONTROL)
		PluginMgr::Inst()->GetCameraCtrl()->OnMouseMove(nFlags,point.x,point.y);
	else
		PluginMgr::Inst()->GetActive()->OnMouseMove(nFlags,point.x,point.y);

	CWnd::OnMouseMove(nFlags, point);
}

BOOL CChildView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if(nFlags&MK_CONTROL)
		PluginMgr::Inst()->GetCameraCtrl()->OnMouseWheel(nFlags,zDelta,pt.x,pt.y);
	else
		PluginMgr::Inst()->GetActive()->OnMouseWheel(nFlags,zDelta,pt.x,pt.y);

	return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}

void CChildView::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if(nFlags&MK_CONTROL)
		PluginMgr::Inst()->GetCameraCtrl()->OnRButtonDown(nFlags,point.x,point.y);
	else
		PluginMgr::Inst()->GetActive()->OnRButtonDown(nFlags,point.x,point.y);
	SetCapture();

	CWnd::OnRButtonDown(nFlags, point);
}

void CChildView::OnRButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if(nFlags&MK_CONTROL)
		PluginMgr::Inst()->GetCameraCtrl()->OnRButtonUp(nFlags,point.x,point.y);
	else
		PluginMgr::Inst()->GetActive()->OnRButtonUp(nFlags,point.x,point.y);
	ReleaseCapture();

	CWnd::OnRButtonUp(nFlags, point);
}

void CChildView::OnDestroy()
{
	CWnd::OnDestroy();
	// TODO: 在此处添加消息处理程序代码
	WorldEditorMgr::Inst()->Destroy();
	PluginMgr::Inst()->Destory();
	Cool3DView::Inst()->Destroy();
}


void CChildView::On32774()
{
	//--设置环境光
	GameMapEditor* pMap =  WorldEditorMgr::Inst()->GetMap();
	SceneGraph*  pSG = WorldEditorMgr::Inst()->GetMap()->GetSceneGraph();
	ASSERT(pSG != NULL);

	tagLight DirectLight = pSG->GetSunLight();
	CColorDialog ColorDlg(RGB(DirectLight.ambient.R*255,DirectLight.ambient.G*255,DirectLight.ambient.B*255));

	if(IDOK == ColorDlg.DoModal())
	{
		DWORD Color = ColorDlg.GetColor();
	
		Color4f colorf(Color);
		DirectLight.ambient = colorf;

		pSG->SetSunLight(DirectLight);
	}
}

void CChildView::On32775()
{
	//--设置折射光
	SceneGraph*  pSG = WorldEditorMgr::Inst()->GetMap()->GetSceneGraph();
	ASSERT(pSG != NULL);

	tagLight DirectLight = pSG->GetSunLight();
	CColorDialog ColorDlg(RGB(DirectLight.diffuse.R* 255,DirectLight.diffuse.G* 255,DirectLight.diffuse.B* 255));

	if(IDOK == ColorDlg.DoModal())
	{
		DWORD Color = ColorDlg.GetColor();
	
		Color4f colorf(Color);
		DirectLight.diffuse = colorf;

		pSG->SetSunLight(DirectLight);
	}
}

void CChildView::On32776()
{
	//--设置雾
	SceneGraph*  pSG = WorldEditorMgr::Inst()->GetMap()->GetSceneGraph();
	ASSERT(pSG != NULL);

	CColorDialog ColorDlg(pSG->GetSceneFog().color);
	if(IDOK == ColorDlg.DoModal())
	{
		DWORD Color = ColorDlg.GetColor();
		tagDistFog Fog= pSG->GetSceneFog();
		Fog.color.R = GetRValue(Color);
		Fog.color.G = GetGValue(Color);
		Fog.color.B = GetBValue(Color);
		pSG->SetSceneFog(Fog);
	}
}
void CChildView::OnID_DIsRole()
{
	TerrainEditor* pTE = WorldEditorMgr::Inst()->GetMap()->GetHeightMap();
	ASSERT(pTE);
	m_bDisRole = !m_bDisRole;
	pTE->SetGroundRole(m_bDisRole);		
}

void CChildView::OnUpdateDisRole(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_bDisRole);
}
