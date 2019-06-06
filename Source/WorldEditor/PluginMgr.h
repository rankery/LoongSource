#pragma once

#include "Resource.h"
#include "EditorPlugin.h"
#include "ToolsBoxDlg.h"
class PluginMgr
{
public:
	PluginMgr(void);
	virtual ~PluginMgr(void);

	static PluginMgr* Inst();
	void LoadEditors();
	void Destory();

	void Render();
	void RenderPreview();
	IPlugEditor* GetActive();
	IPlugEditor* GetCameraCtrl()	{	return m_pCameraCtrl;}
	void SetActive(UINT i);

	IPlugEditor* GetPlugByClassName(const TCHAR *szName);

private:
	CToolsBoxDlg			m_toolsBox;
	vector<IPlugEditor*>	m_editors;
	vector<EditorPlugin*>	m_plugins;
	IPlugEditor*			m_pActivePlugin;
	IPlugEditor*			m_pCameraCtrl;
	
};


