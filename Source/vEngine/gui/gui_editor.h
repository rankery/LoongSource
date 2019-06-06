//-----------------------------------------------------------------------------
//!\file gui_editor.h
//!\author Lyp
//!
//!\date 2004-12-08
//! last 2007-12-31
//!
//!\brief 界面系统 编辑器
//-----------------------------------------------------------------------------
#pragma once

namespace vEngine {
//-----------------------------------------------------------------------------
// editor
//-----------------------------------------------------------------------------
class VENGINE_API GUIEditor
{
public:
	BOOL Init(GUISystem* pSystem, BOOL bInGame=FALSE);
	VOID Destroy();
	VOID Render();

	// 外部通过此函数向XUI Editor系统传窗口消息, 返回TRUE代表已经处理
	BOOL OnWindowsMsg(DWORD dwMsg, DWORD dwParam1, DWORD dwParam2);
	
	GUIEditor();
	~GUIEditor();

	// 供InputMap调用的显示开关
	DWORD Switch(DWORD dwID, BOOL bDown);
	TSFPTrunk<GUIEditor>& GetTrunk() { return m_Trunk; }

private:
	TSFPTrunk<GUIEditor>		m_Trunk;
	TObjRef<Util>				m_pUtil;
	TObjRef<WindowWrap>			m_pWindow;
	TObjRef<DiscIOMgr>			m_pDiscIO;

	BOOL						m_bInGame;	// 是否处于游戏中
	HWND						m_hWnd;

	GUISystem*					m_pSystem;
	DWORD						m_dwActiveWndID;
	GUIWnd*						m_pActiveWnd;
	GUIWnd*						m_pEditor;

	INT							m_nActiveX;
	INT							m_nActiveY;
	TCHAR						m_szFileName[MAX_PATH];
	XmlElement					m_CopyElement;	// 用于Ctrl+C
	XmlElement					m_DragElement;	// 用于Drag
	tagPoint					m_ptLastMouseClickView;	// 最后一次鼠标点击的位置

	tagRect						m_rcActiveOut;
	tagRect						m_rcActiveIn;
	BOOL						m_bInResize;
	BOOL						m_bResize;
	BOOL						m_bChanges;		// 是否需要存盘


	struct tagGUIEditorAction
	{
		XmlElement		OriginEle;	// 原有控件的属性
		tstring	strFather;	// 原有控件的父控件全名
		tstring	strNew;		// 新增控件的全名
	};
	std::list<tagGUIEditorAction*>	m_listUndoAction;
	std::list<tagGUIEditorAction*>	m_listRedoAction;

	// 初始化的时候创建各种控件 
	VOID CreateControls();
	// 接受并处理按钮消息
	BOOL EventHandler(tagGUIEvent* pEvent);

	// 将FatherWnd以及其下所有控件名字加入Tree控件
	VOID AddControlsToTree(GUITree* pTree, LPCTSTR szName, GUIWnd* pWnd);

	// 重构指定控件及其子控件
	VOID Rebuild(GUIWnd* pWnd, LPCTSTR szKey, LPCTSTR szValue);

	// 记录指定控件及其子控件
	BOOL Record(XmlElement* pXmlElement, GUIWnd* pWnd);

	// 设置当前激活控件
	VOID SetActiveWnd(GUIWnd* pWnd);

	// 刷新控件树,同时设置当前激活控件
	VOID RefreshControlsTree(GUIWnd* pActiveWnd);

	// 保存到磁盘
	BOOL Save(LPCTSTR szFileName);
	// 从磁盘读取
	BOOL Load(LPCTSTR szFileName, bool bClear);
	// 清除所有控件
	BOOL Clear();

	// 记录动作行为
	VOID RecordAction(XmlElement* pOriginEle, LPCTSTR szFather, LPCTSTR szNew);
	
	VOID Undo();	
	VOID Redo();
	VOID Copy();
	VOID Paste();
	VOID Delete();

	VOID Open();
	VOID Add();
	VOID SaveAs();
};

}