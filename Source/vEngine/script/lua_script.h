//-----------------------------------------------------------------------------
//!\file lua_scrip.h
//!\author Lyp
//!
//!\date 2008-01-07
//! last 2008-01-07
//!
//!\brief Lua脚本管理
//-----------------------------------------------------------------------------
#pragma once
namespace vEngine {

enum ELuaScriptState
{
	ELSS_NotLoaded,
    ELSS_WaitTime,
    ELSS_Running,
    ELSS_Break,
    ELSS_Done
};

class LuaManager;
class LuaScript;
class GUIEditBoxEx;
VOID LuaOpenScriptLib(lua_State* l);
LuaScript*	GetScriptObject(lua_State* l);
int			LuaConsolePrint(lua_State* l);
int			LuaWaitSec     (lua_State* l);
//-----------------------------------------------------------------------------
// 单独脚本对象
//-----------------------------------------------------------------------------
class VENGINE_API LuaScript
{
public:
	friend public LuaManager;

	lua_State* GetVM() { return m_threadState; }

    VOID Update(float fElapsedSec);
    BOOL LoadFile(LPCSTR szVFS, LPCTSTR szFileName);
	VOID Resume(float param);
    BOOL RunString(LPCSTR szCommandString);
	
	tstring& GetName() { return m_strName; }
	VOID SetWaitTime(FLOAT fTime) { m_fWaitTime = fTime; }
	VOID SetState(ELuaScriptState eState);
	ELuaScriptState GetState() { return m_eState; }







	VOID CallFn(INT nParam);	// 调用脚本函数，外部需要先将函数名以及

	// for debug
	GUIEditBoxEx* GetCodeWnd() { return m_pWndCode; }
	VOID Rebuild();
	VOID SetActiveLine(INT nLine);
	INT GetActiveLine() { return m_nActiveLine; }
	VOID SetBreakPoint(INT nLine);
	BOOL IsBreakPoint(INT nLine) { return m_mapBreakPoint.IsExist(nLine); }

	LuaScript(LuaManager* mgr, GUIEditBoxEx* pWndCode);
	~LuaScript();

private:
	TObjRef<Util>		m_pUtil;
	LuaManager*			m_pManager;		// LuaManager object that created this script

    lua_State*			m_threadState;
    char				m_lastErrorString[256];
	tstring				m_strName;
	
    ELuaScriptState		m_eState;		// are we waiting, not loaded yet, etc.
	FLOAT				m_fWaitTime;	// time to wake up

	GUIEditBoxEx*		m_pWndCode;
	INT					m_nActiveLine;

	TMap<INT, DWORD>	m_mapBreakPoint;	// 断点列表

 
	VOID				CreateThread(tstring strName);
    VOID				FormatError();
    VOID				OutputError(LPCTSTR strType);
};


} // namespace vEngine{