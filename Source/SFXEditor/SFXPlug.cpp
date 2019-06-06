#include "StdAfx.h"
#include ".\sfxplug.h"


SFXPlug* g_editor=NULL;

SFXPlug::SFXPlug(void)
{
	m_pWorldEditorSG=NULL;
	m_pCamera=NULL;
	m_pTrn=NULL;
}

SFXPlug::~SFXPlug(void)
{
	m_pWorldEditorSG=NULL;
	m_pCamera=NULL;
	m_pTrn=NULL;
}

void SFXPlug::AttachEngineInterface(void *pInterface,void *pExInterface)
{
	if(m_pWorldEditorSG==NULL)
		m_pWorldEditorSG=(NullSceneGraph*)pInterface;
	else
		m_pTrn=(TerrainEditor*)pInterface;
}

void SFXPlug::SetCamera(CameraBase *pCamera)
{
	m_pCamera=pCamera;
}

void SFXPlug::Render( bool bActive )
{
	if( bActive )
		m_mainDlg.OnRender();
}

const TCHAR* SFXPlug::GetUserName()
{
	return _T("ÌØÐ§±à¼­Æ÷");
}

const TCHAR* SFXPlug::GetClassName()
{
	return _T("SFXPlug");
}

void SFXPlug::Create(DWORD pParentWnd)
{
	BeginMFCRes();

	if(m_mainDlg.Create(IDD_MainDlg,NULL))
	{
		m_mainDlg.ShowWindow(SW_SHOW);
		m_mainDlg.UpdateWindow();
	}

	EndMFCRes();
}

void SFXPlug::Destroy()
{

}

void SFXPlug::ShowWindow(bool bShow)
{
	if(bShow)
	{
		m_mainDlg.ShowWindow(SW_SHOW);
		m_mainDlg.BringWindowToTop();
	}
	else
	{
		m_mainDlg.ShowWindow(SW_HIDE);
	}
}

void SFXPlug::OnLButtonDown(UINT nFlags, UINT x,UINT y)
{
	if(m_mainDlg.IsDlgButtonChecked(IDC_Pick))
	{
		Vector3 pos;
		if(m_pTrn->MousePick(x,y,m_pCamera,pos))
		{	
			m_mainDlg.SetEffectPos(pos);
		}
	}
}

void SFXPlug::OnLButtonUp(UINT nFlags, UINT x,UINT y)
{}

void SFXPlug::OnMouseMove(UINT nFlags, UINT x,UINT y)
{}

void SFXPlug::OnMouseWheel(UINT nFlags, short zDelta, UINT x,UINT y)
{}

void SFXPlug::OnRButtonDown(UINT nFlags, UINT x,UINT y)
{}

void SFXPlug::OnRButtonUp(UINT nFlags, UINT x,UINT y)
{}

extern "C"
{
	__declspec(dllexport) bool _cdecl Plug_Init()
	{
		return true;
	}
	__declspec(dllexport) void _cdecl Plug_Destroy()
	{}

	__declspec(dllexport) int _cdecl Plug_GetNumClass()
	{
		return 1;
	}

	__declspec(dllexport) const TCHAR* _cdecl Plug_GetClassName(int i)
	{
		return _T("SFXEditor");
	}
	__declspec(dllexport) void* _cdecl Plug_NewObject(const TCHAR* szClassName,DWORD param1,const TCHAR* param2)
	{
		g_editor=new SFXPlug;
		return g_editor;
	}
	__declspec(dllexport) void _cdecl Plug_DelObject(const TCHAR* szClassName,void *pObj,const TCHAR* param2)
	{
		SFXPlug* pPlug=(SFXPlug*)pObj;
		delete pPlug;
		ASSERT(pPlug==g_editor);
		g_editor=NULL;
	}
}

static HINSTANCE g_hLastInst=NULL;
void BeginMFCRes()
{
	g_hLastInst=AfxGetResourceHandle();

	HINSTANCE hMyInst=GetModuleHandle(_T("SFXEditor.dlp"));
	AfxSetResourceHandle(hMyInst);
}
void EndMFCRes()
{
	ASSERT(g_hLastInst!=NULL);
	AfxSetResourceHandle(g_hLastInst);
	g_hLastInst=NULL;
}