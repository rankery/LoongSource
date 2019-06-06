#include "StdAfx.h"
#include ".\WorldEditorMgr.h"
#include "Cool3DView.h"

WorldEditorMgr::WorldEditorMgr(void)
{
	m_pTerrainEditor=NULL;
	m_pNavMapEditor=NULL;
	m_pMapEditor=NULL;
}

WorldEditorMgr::~WorldEditorMgr(void)
{
}

WorldEditorMgr* WorldEditorMgr::Inst()
{
	static WorldEditorMgr g_WorldEditorMgr;
	return &g_WorldEditorMgr;
}

void WorldEditorMgr::Update(float deltaTime)
{

}

void WorldEditorMgr::Render()
{
	SceneGraph* pSG=Cool3DView::Inst()->GetSceneGraph();
	if(m_pTerrainEditor)
		m_pTerrainEditor->Draw(pSG);
}

void WorldEditorMgr::Create()
{
	m_pTerrainEditor=new TerrainEditor;
	m_pNavMapEditor=new NavMapEditor;
	m_pMapEditor=new GameMapEditor;

	SceneGraph* pSG=Cool3DView::Inst()->GetSceneGraph();
	ASSERT(pSG->IS_STATIC_CLASS(NullSceneGraph));
	m_pMapEditor->Create(m_pTerrainEditor,(NullSceneGraph*)pSG,m_pNavMapEditor->GetSceneGraph());
	m_pMapEditor->LoadAttDataBase();
}

void WorldEditorMgr::Destroy()
{
	SAFE_DELETE(m_pTerrainEditor);
	SAFE_DELETE(m_pMapEditor);
	SAFE_DELETE(m_pNavMapEditor);
}