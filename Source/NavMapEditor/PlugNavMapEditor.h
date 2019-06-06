#pragma once
#include "..\Cool3D\Cool3DEngine.h"
#include "..\Cool3D\EditorSupport\TerrainEditor.h"
using namespace Cool3D;

#include "..\Cool3D\Public\IPlugEditor.h"
#include "Resource.h"
#include "..\WorldBase\NavMapEditor\NavMapEditor.h"
using namespace WorldBase;

#include "MainDlg.h"

class PlugTerrainEditor;

class PlugNavMapEditor : public IPlugEditor
{
public:
	PlugNavMapEditor(void);
	virtual ~PlugNavMapEditor(void);

	//--IPlugEditor
	virtual void AttachEngineInterface(void *pInterface,void *pExInterface=NULL);
	virtual void SetCamera(CameraBase *pCamera) { m_pCamera=pCamera;}

	virtual void Update(bool bActive);
	virtual void Render(bool bActive);

	virtual const TCHAR* GetUserName();
	virtual const TCHAR* GetClassName();
	virtual void Create(DWORD pParentWnd);
	virtual void Destroy();
	virtual void ShowWindow(bool bShow);

	virtual bool Build(const TCHAR* szMapName,const TCHAR* szSaveBasePath,DWORD dwItems,tagBuildOptions& options,IUICallBack* pUI);

	virtual void OnLButtonDown(UINT nFlags, UINT x,UINT y);
	virtual void OnLButtonUp(UINT nFlags, UINT x,UINT y);
	virtual void OnMouseMove(UINT nFlags, UINT x,UINT y);
	virtual void OnMouseWheel(UINT nFlags, short zDelta, UINT x,UINT y);
	virtual void OnRButtonDown(UINT nFlags, UINT x,UINT y);
	virtual void OnRButtonUp(UINT nFlags, UINT x,UINT y);

	//--
	NavMapEditor*	GetEngine()	{ return m_pNavMapEngine; }
	CameraBase*		GetCamera()	{ return m_pCamera; }
	bool			Save();
	bool			Load();
	void			ClearCangoMap();
	void			ClearPlayerRoadMap();
	bool			LoadNPCNavMap();
	bool			LoadNPCNavGraph();
	bool			LoadPlayerNavMap();

	void			DisplayNPCCangoMap();
	void			DisplayPlayerCangoMap();
	void			DisplayManuelCangoMap();
	void			DisplayRoadMap();

private:
	bool _Build(const TCHAR* szMapName,const TCHAR* szSaveBasePath,DWORD dwItems,tagBuildOptions& options,IUICallBack* pUI);
	//void CreateBrush(tagTEditBrush& brush);
	//void DoCurrentTools(const tagTEditBrush& brush);
	void DrawCursor();
	void DrawWayPoints();

	//绘制比刷提示框框
	/**	在地面上画出一个圆圈
	\param,中心点的世界坐标,圆圈的半径(单位:格子)
	*/
	void DrawGroundCircle(float centreX,float centreZ,UINT r);
	void DrawGroundQuad(int left,int right,int top,int bottom,DWORD color);
	void DrawGroundLine(int sx,int sy,int length,float angle=0.0f,DWORD color=0xFFFFFF00);
	void DrawGroundLine(int sx,int sy,int dx,int dy,DWORD color=0xFFFFFF00);
	void DoEditCango(int inv);
	void DoEditPlayerNav(int inv);
	DWORD CalMapColor(int x,int y);
	DWORD CalNPCNavColor(int x, int y);
	DWORD CalPlayerNavColor(int x, int y);
	DWORD CalPlayerRoadMapColor(int x, int y);

private:
	CMainDlg		m_mainDlg;
	CameraBase		*m_pCamera;
	NavMapEditor	*m_pNavMapEngine;
	TerrainEditor	*m_pTerrainEngine;
	bool			m_bRender;
	Vector3			m_cursorPos;
	int				m_drag;
	POINT			m_lastDoPt;

	BitMap			*m_pManualCangoMap;		//手工编辑的可行走区域图
	BitMap			*m_pPlayerRoadMap;		//手工编辑的玩家寻路道路区域

	Heightmap		*m_pNPCNavHMap;			//npc格子导航图生成起点的heightmap
	BitMap			*m_pNPCNavBMap;			//npc格子导航图生成起点的bitmap

	Heightmap		*m_pPlayerNavHMap;		//玩家格子导航图生成起点的heightmap
	BitMap			*m_pPlayerNavBMap;		//玩家格子导航图生成起点的bitmap

	NPCNavGraph		m_npcNavGraph;			//碰撞型NPC路点导航图
};

extern int g_drawSize;
extern PlugNavMapEditor	*g_navEditor;
void BeginMFCRes();
void EndMFCRes();