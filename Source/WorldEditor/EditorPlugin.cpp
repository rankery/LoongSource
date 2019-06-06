#include "StdAfx.h"
#include ".\editorplugin.h"
#include "WorldEditorMgr.h"
#include "Cool3DView.h"

EditorPlugin::EditorPlugin(void)
{
}

EditorPlugin::~EditorPlugin(void)
{
	DeleteAll();
}

void EditorPlugin::CreateAll(vector<IPlugEditor*>& editors)
{
	ASSERT(m_created.size() == 0);//not created;
	
	for(int i=0;i<PlugIn::GetNumClass();i++)
	{
		IPlugEditor *editor=(IPlugEditor *)PlugIn::NewObject(PlugIn::GetClassName(i),0,NULL);
		editors.push_back(editor);
		m_created.push_back(editor);
		
		if(_tcsicmp(editor->GetClassName(),_T("TerrainEditor"))==0)
			editor->AttachEngineInterface(WorldEditorMgr::Inst()->GetTerrain());
		else if(_tcsicmp(editor->GetClassName(),_T("NavMapEditor"))==0)
		{
			editor->AttachEngineInterface(WorldEditorMgr::Inst()->GetNavMap(),WorldEditorMgr::Inst()->GetTerrain());
		}
		else if( _tcsicmp(editor->GetClassName(),_T("MapObjEditor")) ==0 ||
			 _tcsicmp(editor->GetClassName(), _T("AttrEditor")) == 0)
		{
			editor->AttachEngineInterface(WorldEditorMgr::Inst()->GetMap());
		}
		else if(_tcsicmp(editor->GetClassName(),_T("SFXPlug"))==0)
		{
			SceneGraph *pSG=WorldEditorMgr::Inst()->GetMap()->GetSceneGraph();
			editor->AttachEngineInterface(pSG);
			editor->AttachEngineInterface(WorldEditorMgr::Inst()->GetTerrain());
		}
		editor->Create((DWORD)::AfxGetMainWnd());
		editor->SetCamera(Cool3DView::Inst()->GetCamera());
		editor->ShowWindow(false);
	}
}

void EditorPlugin::DeleteAll()
{
	for(size_t i=0;i<m_created.size();i++)
		PlugIn::DelObject(m_created[i]->GetClassName(),m_created[i],NULL);
	m_created.clear();
}
