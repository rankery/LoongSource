#pragma once
#include "..\Cool3D\public\iplugeditor.h"
#include "EditorCamera.h"
#include "resource.h"
#include "CameraCtrlDlg.h"

class EditorCameraCtrl : public IPlugEditor
{
public:
	EditorCameraCtrl(void);
	virtual ~EditorCameraCtrl(void);

	//--IPlugEditor
	virtual void AttachEngineInterface(void *pInterface,void *pExInterface=NULL);
	virtual void SetCamera(CameraBase *pCamera) {}
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
	EditorCamera* GetCamera()const{return m_pCamera;}
private:
	EditorCamera	*m_pCamera;
	BOOL			m_bDrag;	//鼠标左键拖动
	BOOL			m_bRightDrag;//鼠标右键拖动
	CPoint			m_DragPt;
	CCameraCtrlDlg	m_dlg;
	FLOAT			m_fMoveRate;			//摄像机移动速度
	FLOAT			m_accelerPerDist;
};
