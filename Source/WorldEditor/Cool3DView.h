#pragma once
#include "..\Cool3D\Cool3DEngine.h"
using namespace Cool3D;
#include "EditorCamera.h"

class Cool3DView
{
public:
	Cool3DView(void);
	~Cool3DView(void);

	static Cool3DView* Inst();
	void InitEngine(HWND hDrawWnd);
	void Destroy();

	void Render();
	void SetBkColor(DWORD color);
	EditorCamera* GetCamera()	{ return &m_camera;}
	SceneGraph* GetSceneGraph()	{ return m_pSG; }

private:
	void RenderSky(void);

private:
	bool			m_bCreated;
	EditorCamera	m_camera;
	SceneGraph		*m_pSG;

};
