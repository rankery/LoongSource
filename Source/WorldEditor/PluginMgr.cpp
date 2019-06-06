#include "StdAfx.h"
#include "Resource.h"
#include ".\pluginmgr.h"
#include "EditorCameraCtrl.h"
#include "Cool3DView.h"
#include "EditorPlugin.h"
#include "Cool3DView.h"

PluginMgr::PluginMgr(void)
{
	m_pActivePlugin=NULL;
	m_pCameraCtrl=NULL;
}

PluginMgr::~PluginMgr(void)
{
}

PluginMgr* PluginMgr::Inst()
{
	static PluginMgr g_pluginMgr;
	return &g_pluginMgr;
}

void PluginMgr::LoadEditors()
{
	m_pActivePlugin=new EditorCameraCtrl;
	m_pCameraCtrl=m_pActivePlugin;
	m_pCameraCtrl->Create((DWORD)AfxGetMainWnd());
	m_pActivePlugin->AttachEngineInterface(Cool3DView::Inst()->GetCamera());
	m_editors.push_back(m_pActivePlugin);
	//--
	SetCurrentDirectory(Kernel::Inst()->GetAppPath());
	CFileFind finder;
	BOOL bWorking = finder.FindFile(_T("*.dlp"));
	while (bWorking)
	{
		bWorking = finder.FindNextFile();		
		EditorPlugin *plug=new EditorPlugin;
		m_plugins.push_back(plug);
		bool plugLoadOK=plug->LoadFromDll(finder.GetFilePath());
		ASSERT(plugLoadOK);
		plug->CreateAll(m_editors);
	}

	//--创建工具箱
	m_toolsBox.Create(IDD_ToolsBox,AfxGetMainWnd());
	m_toolsBox.m_list.ResetContent();
	for(size_t i=0;i<m_editors.size();i++)
	{
		m_toolsBox.m_list.AddString(m_editors[i]->GetUserName());
	}
	
	m_toolsBox.ShowWindow(SW_SHOW);
	m_toolsBox.UpdateWindow();
}

void PluginMgr::SetActive(UINT i)
{
	ASSERT(i < m_editors.size());
	if(m_pActivePlugin)
		m_pActivePlugin->ShowWindow(false);
	m_pActivePlugin=m_editors[i];
	m_pActivePlugin->ShowWindow(true);
}

void PluginMgr::Render()
{
	for(size_t i=0;i<m_editors.size();i++)
	{
		IPlugEditor *pPlug=m_editors[i];
		pPlug->Render(pPlug==m_pActivePlugin);
	}
}

void PluginMgr::RenderPreview()
{
	for(size_t i=0;i<m_editors.size();i++)
	{
		IPlugEditor *pPlug=m_editors[i];
		pPlug->RenderPreview();
	}
	
	if(m_pActivePlugin)
		m_pActivePlugin->RenderPreview();
}

void PluginMgr::Destory()
{
	SAFE_DELETE(m_pCameraCtrl);
	m_pActivePlugin=NULL;
	m_editors.clear();
	for(size_t i=0;i<m_plugins.size();i++)
	{
		EditorPlugin *plug=m_plugins[i];
		delete plug;	
	}
	m_plugins.clear();
}

IPlugEditor* PluginMgr::GetActive()
{
	return m_pActivePlugin;
}

IPlugEditor* PluginMgr::GetPlugByClassName(const TCHAR *szName)
{
	for(size_t i=0;i<m_editors.size();i++)
	{
		IPlugEditor *pPlug=m_editors[i];
		if(_tcsicmp(pPlug->GetClassName(),szName)==0)
			return pPlug;
	}
	return NULL;
}