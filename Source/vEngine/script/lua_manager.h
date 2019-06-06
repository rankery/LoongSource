//-----------------------------------------------------------------------------
//!\file lua_manager.h
//!\author Lyp
//!
//!\date 2008-01-07
//! last 2008-01-07
//!
//!\brief Lua脚本管理
//-----------------------------------------------------------------------------
#pragma once
namespace vEngine {

enum ELuaDebugState
{
	ELDS_Normal = 0,
	ELDS_StepOver,	// F10
	ELDS_StepInto,	// F11
};

class LuaScript;
class GUIEditBoxEx;
class GUIListBox;
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
class VENGINE_API LuaManager
{
public:
	BOOL	Init(LPCSTR szVFS, BOOL bDebug);
	VOID	Update(float fElapsedSec);

    LuaScript* CreateScript(LPCSTR szVFS, LPCTSTR szFileName);
	BOOL DestroyScript(LPCTSTR szFileName);


	LuaScript* GetScript(DWORD dwID) { return m_mapScript.Peek(dwID); }
	INT	GetNumScripts() { return m_mapScript.Size(); }


	// debug 相关
	BOOL IsDebug() { return m_bDebug; }
	VOID DebugMsg(LPCWSTR lpFormat, ...);
	BOOL EventHandler(tagGUIEvent* pEvent);
	
	BOOL SetEditScript(LuaScript* pScript);	// 设置当前正在编辑的脚本
	BOOL SetDebugScript(LuaScript* pScript);	// 设置当前正在调试的脚本

	VOID SetDebugState(ELuaDebugState eState);
	ELuaDebugState GetDebugState() { return m_eDebugState; }
	GUIListBox* GetStackWnd() { return m_pWndStack; }
	GUIListBox* GetWatchWnd() { return m_pWndWatch; }

	VOID OnScriptStateChange(LuaScript* pScript);	// 脚本状态改变

	LuaManager(void);
	~LuaManager(void);

	lua_State*		m_masterState;

private:
	TSFPTrunk<LuaManager>		m_Trunk;
	TObjRef<Util>				m_pUtil;
	TObjRef<GUISystem>			m_pGUI;
	TMap<DWORD, LuaScript*>	m_mapScript;
	LuaScript*					m_pDebugScript;	// 当前正在调试的脚本
	LuaScript*					m_pEditScript;	// 当前正在编辑的脚本

	BOOL						m_bDebug;	// 是否调试模式
	ELuaDebugState				m_eDebugState;

	GUIStatic*					m_pWndCaption;
	GUIEditBoxEx*				m_pWndDebug;
	GUIListBox*				m_pWndFileList;
	GUIEditBoxEx*				m_pWndCode;
	GUIStatic*					m_pWndState;
	GUIStatic*					m_pWndCodeState;
	GUIListBox*				m_pWndStack;
	GUIListBox*				m_pWndWatch;
	XmlElement					m_XmlElement;
	

};

} // namespace vEngine{