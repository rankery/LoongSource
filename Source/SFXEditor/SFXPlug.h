#pragma once
#include "..\Cool3D\Cool3DEngine.h"
#include "..\Cool3D\SpecialEffect\EffectAll.h"
using namespace Cool3D;
#include "..\Cool3D\public\iplugeditor.h"
#include "resource.h"
#include "MainDlg.h"

class SFXPlug :	public IPlugEditor
{
public:
	SFXPlug(void);
	virtual ~SFXPlug(void);

	//--IPlugEditor 纯虚函数
	virtual void AttachEngineInterface(void *pInterface,void *pExInterface=NULL);
	virtual void SetCamera(CameraBase *pCamera);
	virtual void Render(bool bActive);
	virtual const TCHAR* GetUserName();
	virtual const TCHAR* GetClassName();
	virtual void Create(DWORD pParentWnd);
	virtual void Destroy();
	virtual void ShowWindow(bool bShow);
	virtual void OnLButtonDown(UINT nFlags, UINT x,UINT y);
	virtual void OnLButtonUp(UINT nFlags, UINT x,UINT y);
	virtual void OnMouseMove(UINT nFlags, UINT x,UINT y);
	virtual void OnMouseWheel(UINT nFlags, short zDelta, UINT x,UINT y);
	virtual void OnRButtonDown(UINT nFlags, UINT x,UINT y);
	virtual void OnRButtonUp(UINT nFlags, UINT x,UINT y);

	SceneGraph	*GetSceneGraph()	{	return m_pWorldEditorSG;}
protected:
	CMainDlg			m_mainDlg;
	
	NullSceneGraph		*m_pWorldEditorSG;	//世界编辑器的SceneGraph
	CameraBase			*m_pCamera;
	TerrainEditor		*m_pTrn;//for pick
};

extern SFXPlug* g_editor;
void BeginMFCRes();
void EndMFCRes();