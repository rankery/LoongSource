#pragma once
#include "..\Cool3D\Cool3DEngine.h"
using namespace Cool3D;
#include "..\Cool3D\public\iplugeditor.h"

#include "..\WorldBase\gameMapEditor\gamemapeditor.h"
using namespace WorldBase;

#include".\MapObjDlg.h"

#define DEFAULT_WAYPOINT_TITLE_SIZE 1
#define QUAD_OFFSET 100

class CPlugMapEditor :public IPlugEditor
{
public:
	CPlugMapEditor(void);
	~CPlugMapEditor(void);

	//-- IPlugEditor
	virtual void AttachEngineInterface(void *pInterface,void *pExInterface=NULL);
	virtual void SetCamera(CameraBase *pCamera);

	virtual void Update(bool bActive);
	virtual void Render(bool bActive);
	virtual void RenderPreview();

	virtual const TCHAR* GetUserName();
	virtual const TCHAR* GetClassName();

	virtual void Create(DWORD pParentWnd);
	virtual void Destroy();
	virtual void ShowWindow(bool bShow);
	virtual HWND GetRenderWnd() {	return NULL; }
	virtual bool Build(const TCHAR* szMapName,const TCHAR* szSaveBasePath,DWORD dwItems,tagBuildOptions& options,IUICallBack* pUI);
	void AddMazeToSG(NullSceneGraph *pSG,const TCHAR* szMazeFile);

	virtual void OnLButtonDown(UINT nFlags, UINT x,UINT y);
	virtual void OnLButtonUp(UINT nFlags, UINT x,UINT y);
	virtual void OnMouseMove(UINT nFlags, UINT x,UINT y);
	virtual void OnMouseWheel(UINT nFlags, short zDelta, UINT x,UINT y);
	virtual void OnRButtonDown(UINT nFlags, UINT x,UINT y);
	virtual void OnRButtonUp(UINT nFlags, UINT x,UINT y);

	MapObjEdit* GetLock() const { return m_pLockSelected;}
	void SetLock(MapObjEdit* pEdit) { m_pLockSelected = pEdit;}
	GameMapEditor* GetMap() const { return m_pEngine;}
	CameraBase*	 GetCamera()  { return m_pCamera;}
    
	//建立导航点
	bool BuildWayPoint(UINT x,UINT y);
	//拾取导航点
	bool PickWayPoint(UINT x,UINT y);
	//移动拾取的导航点
	bool MovePickedWayPoint(UINT x,UINT y);

	//右键建立触发器
	bool BuildTriggerRButton(UINT x, UINT y);
	//拾取触发器
	bool PickTrigger(UINT x,UINT y);
	//拾取触发器的点
	bool PickTriggerPoint(UINT x, UINT y);
	//移动拾取的触发器
	bool MovePickedTrigger(UINT x,UINT y);
	//移动拾取的触发器的点
	bool MovePickedTriggerPoint(UINT x, UINT y);

	//移动拾取的采集对象
	bool MovePickedGather(UINT x,UINT y);
	//建立音效
	bool BuildSound(UINT x,UINT y);
	//拾取音效
	bool PickSound(UINT x,UINT y);
	//移动拾取音效
	bool MovePickedSound(UINT x,UINT y);

	//设置拾取显示标识
	void SetPickFlag();
	
	//建立点光源
	bool BuildPointLight(UINT x, UINT y);
	//拾取点光源
	bool PickPointLight(UINT x, UINT y);
	//移动拾取的点光源
	bool MovePickedPointLight(UINT x, UINT y);

	//建立寻路导航点
	bool BuildPathPoint(UINT x, UINT y);
	//拾取寻路导航点
	bool PickPathPoint(UINT x, UINT y);
	//移动拾取的寻路导航点
	bool MovePickedPathPoint(UINT x, UINT y);

	//右键建立地图区域
	bool BuildMapRectRButton(UINT x, UINT y);
	//拾取地图区域
	bool PickMapRect(UINT x, UINT y);
	//拾取地图区域的点
	bool PickMapAreaPoint(UINT x, UINT y);
	//移动地图区域
	bool MovePickMapRect(UINT x, UINT y);
	//移动拾取的触发器的点
	bool MovePickedMapAreaPoint(UINT x, UINT y);

	//建立刷怪点
	bool BuildSpawnPoint(UINT x, UINT y);
	//拾取刷怪点
	bool PickSpawnPoint(UINT x, UINT y);
	//移动刷怪点
	bool MovePickedSpawnPoint(UINT x, UINT y);

	//--NPC格子导航图生成起点
	bool BuildNPCNavStartPoint(UINT x, UINT y);
	bool PickNPCNavStartPoint(UINT x, UINT y);
	bool MovePickedNPCNavStartPoint(UINT x, UINT y);

	//--玩家格子导航图生成起点
	bool BuildPlayerNavStartPoint(UINT x, UINT y);
	bool PickPlayerNavStartPoint(UINT x, UINT y);
	bool MovePickedPlayerNavStartPoint(UINT x, UINT y);


	//绘制静态物体方块
	void DrawStatiObjQuad();

	//通过静态物体方块拾取静态物体
	bool PickStaticObjByQuad(UINT x, UINT y);
	//通过迷宫墙壁方块拾取迷宫墙壁
	//bool PickMazeWallByQuad(UINT x, UINT y);

	//鼠标左击筛选
	bool MouseFilterSelected(MapObjEdit* pSelected);

	CMapObjDlg* GetCMapObjDlg(){return &m_mapObjDlg;}

private:
	bool _Build(const TCHAR* szMapName,const TCHAR* szSaveBasePath,DWORD dwItems,tagBuildOptions& options,IUICallBack* pUI);
	bool BuildMiniMap( const AABBox& box, const DWORD nRTSize, TCHAR* szFileName, HugeQuadTreeSG* pSG );
private:
	GameMapEditor		*m_pEngine;
	CMapObjDlg			m_mapObjDlg;
	bool				m_bDraged;
	CameraBase			*m_pCamera;		
	MapObjEdit*        m_pLockSelected;

	bool                m_bBuildWayPoint;
	bool                m_bBuildTrigger;
	bool				m_bBuildMapRect;

	bool                m_bDragedSoundBox;
	bool                m_bDragedWayPoint;
	bool                m_bDragedTrigger;
	bool				m_bMoveTriggerPoint;
	bool				m_bMoveMapAreaPoint;
	bool                m_bPickObject;
	bool				m_bDragedPointlight;
	bool				m_bDragedMapRect;
	bool				m_bDragedPathPoint;
	bool				m_bDragedSpawnPoint;
	bool				m_bDragedDynamicBlock;
	bool				m_bDragedNPCNav;
	bool				m_bDragedPlayerNav;

	bool                m_bBuildChopField;
	bool                m_bPickChopField;
	bool                m_bDragedChopField;

	bool				m_bDragedGather;
	bool				m_bFirstMove;
	bool				m_bIsEqualType;

	tagMapCarrier		*m_MapCarrier;
	vector<Vector3>	m_vecCarrierPoint;
	INT					m_nCarrierPoint;		//当前载具移动到第几个点
};

extern CPlugMapEditor* g_mapEditor;
extern void BeginMFCRes();
extern void EndMFCRes();
