//-----------------------------------------------------------------------------
//!\file lua_manager.cpp
//!\author Lyp
//!
//!\date 2008-01-07
//! last 2008-01-07
//!
//!\brief Lua脚本管理
//-----------------------------------------------------------------------------
#include "..\stdafx.h"
#include "lua_script.h"
#include "lua_manager.h"
#include "lua\lua.hpp"

#include "..\console\console_gui.h"
#include "..\util\util.h"
#include "..\vfs\vfs.h"

#include "..\gui\gui_static.h"
#include "..\gui\gui_editbox.h"
#include "..\gui\gui_editboxex.h"
#include "..\gui\gui_listbox.h"
#include "..\gui\gui_event_define.h"

namespace vEngine {
VOID ShowStack(lua_State *L, lua_Debug *debug_ar) 
{
	lua_Debug ar;
	const char *name;
	tstringstream ss;

	TObjRef<Util> pUtil;
	TObjRef<LuaManager> pMgr;

	GUIListBox* pListStack = pMgr->GetStackWnd();
	pListStack->Clear();

	for(INT nLevel=0, nRow=0; ;nLevel++)
	{
		if( lua_getstack(L, nLevel, &ar) == 0 )
			return; /* failure: no such level in the stack */

		ss.str(_T(""));
		ss << nLevel;
		pListStack->SetText(nRow, 0, ss.str().c_str());

		lua_getinfo(L, "Slnf", &ar); /* retrieves function */
		tstring strSource = pUtil->Unicode8ToUnicode(ar.source);
		tstring strName = ar.name ? pUtil->Unicode8ToUnicode(ar.name) : pUtil->Unicode8ToUnicode(ar.what);
		pListStack->SetText(nRow, 1, strName.c_str());	// name

		ss.str(_T(""));
		ss << strSource << _T(":") << ar.currentline;
		pListStack->SetText(nRow++, 2, ss.str().c_str());	// source

		int i = 1;
		while ((name = lua_getlocal(L, &ar, i++)) != NULL) 
		{
			if( strcmp(name,"(*temporary)") == 0 )	// 内部变量不显示
			{
				lua_pop(L, 1);
				continue;
			}

			ss.str(_T(""));
			ss << nLevel;
			pListStack->SetText(nRow, 0, ss.str().c_str());

			ss.str(_T(""));
			ss << pUtil->Unicode8ToUnicode(name) << _T("[");
			ss << pUtil->Unicode8ToUnicode(luaL_typename(L, -1)) << _T("]");
			pListStack->SetText(nRow, 1, ss.str().c_str());
			
			tstring strValue;
			LPCSTR pValue = lua_tostring(L, -1);
			if( pValue )
				strValue = pUtil->Unicode8ToUnicode(pValue);
			pListStack->SetText(nRow++, 2, strValue.c_str());

			lua_pop(L, 1); /* remove variable value */
		}
	}
}



VOID ShowWatch(lua_State *L) 
{
	tstringstream ss;

	TObjRef<Util> pUtil;
	TObjRef<LuaManager> pMgr;

	GUIListBox* pListWatch = pMgr->GetWatchWnd();
	INT nLine = pListWatch->GetTotalRowNum();

	for(INT n=0; n<nLine; n++)
	{
		tstring strName = pListWatch->GetText(n, 0);
		lua_getfield(L, LUA_GLOBALSINDEX, pUtil->UnicodeToUnicode8(strName.c_str()));
		
		tstring strValue; 
		if( lua_isnil(L, -1) )
			strValue = _T("nil");
		else
			strValue = pUtil->Unicode8ToUnicode(lua_tostring(L, -1));

		pListWatch->SetText(n, 1, strValue.c_str());
		//lua_pushstring(L, pUtil->UnicodeToUnicode8(strName.c_str()));
		//INT nReturn = lua_getmetatable(L, 0);
	}

}



static void ScriptHook(lua_State *L, lua_Debug *ar)
{
	TObjRef<Util> pUtil;
	TObjRef<LuaManager> pMgr;

	INT nResult = lua_getinfo(L, "Sln", ar);
	LPCSTR pName = ar->name;
	if( !P_VALID(ar->name) )
		pName = "Main";

	// 得到当前hook住的脚本对象
	lua_pushlightuserdata(L, L);
	lua_gettable(L, LUA_GLOBALSINDEX);
	LuaScript* pScript = (LuaScript*)lua_touserdata(L, -1);

	// 当前正在debug的脚本对象，不一定等于当前hook停住的对象
	LuaScript* pDebugScript = GetScriptObject(L);

	ELuaDebugState eState = pMgr->GetDebugState();
	switch( eState )
	{
	case ELDS_StepInto:
		if( pScript->GetActiveLine() != ar->currentline-1 )
			goto __break_down;
		break;
	case ELDS_StepOver:
		if( pScript == pDebugScript && pScript->GetActiveLine() != ar->currentline-1 )
			goto __break_down;
		break;
	case ELDS_Normal:	// 检查断点
		if( pScript->IsBreakPoint(ar->currentline-1) )
			goto __break_down;
		break;
	}
	return;	// LUA继续运行


__break_down:	// LUA暂停运行

	// 得到堆栈信息
	ShowStack(L, ar);

	// 得到watch信息
	ShowWatch(L);

	// 控制权交还C++
	pMgr->SetEditScript(pScript);
	pScript->SetActiveLine(ar->currentline-1);
	pDebugScript->SetState(ELSS_Break);
	pMgr->SetDebugScript(pDebugScript);
	lua_yield(L, 1);
	return;
}


//-----------------------------------------------------------------------------
// CONSTRUCTOR
//-----------------------------------------------------------------------------
LuaManager::LuaManager():m_Trunk(this)
{
	m_pEditScript = NULL;
	m_pDebugScript = NULL;

	m_masterState = lua_open();
	if( m_masterState )
	{
		// open any auxiliary libaraies we may need
		luaopen_base(m_masterState); 
		//luaopen_string(m_masterState);
		//luaopen_table(m_masterState);
		//luaopen_math(m_masterState); 
		//luaopen_debug(m_masterState); 
		
		LuaOpenScriptLib(m_masterState);	// including our own
	}

}


//-----------------------------------------------------------------------------
// DESTRUCTOR
//-----------------------------------------------------------------------------
LuaManager::~LuaManager()
{

	LuaScript*  pScrip = NULL;
	m_mapScript.ResetIterator();
	while( m_mapScript.PeekNext(pScrip) )
		SAFE_DEL(pScrip);

	m_mapScript.Clear();
	lua_close(m_masterState);
}



//-----------------------------------------------------------------------------
// init
//-----------------------------------------------------------------------------
BOOL LuaManager::Init(LPCSTR szVFS, BOOL bDebug)
{
	m_bDebug = bDebug;
	if( m_bDebug )
	{
		// hook every line
		lua_sethook(m_masterState, ScriptHook, LUA_MASKLINE, 0);

		m_eDebugState = ELDS_Normal;

		// 注册事件处理函数
		m_pGUI->RegisterEventHandler(_T("\\desktop\\luaeditor"), m_Trunk.sfp1(&vEngine::LuaManager::EventHandler));

		tstring strPath = g_strLocalPath + _T("\\ui\\lua_debug.xml");
		m_pGUI->CreateFromFile(szVFS, strPath.c_str());

		m_pWndCaption = (GUIStatic*)m_pGUI->GetWnd(_T("\\desktop\\luaeditor\\caption"));
		m_pWndDebug = (GUIEditBoxEx*)m_pGUI->GetWnd(_T("\\desktop\\luaeditor\\debug"));
		m_pWndFileList = (GUIListBox*)m_pGUI->GetWnd(_T("\\desktop\\luaeditor\\files"));
		m_pWndCode = (GUIEditBoxEx*)m_pGUI->GetWnd(_T("\\desktop\\luaeditor\\code"));
		m_pWndState = (GUIStatic*)m_pGUI->GetWnd(_T("\\desktop\\luaeditor\\state"));
		m_pWndCodeState = (GUIStatic*)m_pGUI->GetWnd(_T("\\desktop\\luaeditor\\codestate"));
		m_pWndStack = (GUIListBox*)m_pGUI->GetWnd(_T("\\desktop\\luaeditor\\stack"));
		m_pWndWatch = (GUIListBox*)m_pGUI->GetWnd(_T("\\desktop\\luaeditor\\watch"));

		m_pGUI->Record(&m_XmlElement, m_pWndCode);
		m_pWndFileList->SetColNum(1, 200);
		m_pWndWatch->SetColNum(2,100);
		m_pWndStack->SetColNum(3, 20);
		m_pWndStack->SetColWidth(1, 100);

		m_pWndWatch->SetText(0,0,_T("new"));
		m_pWndWatch->SetText(0,1,_T("null"));
		m_pWndWatch->SetItemEditable(0,0,TRUE);
		m_pWndWatch->SetItemEditable(0,1,TRUE);
	}

	return TRUE;
}


//-----------------------------------------------------------------------------
// Creates a script object.
//-----------------------------------------------------------------------------
LuaScript* LuaManager::CreateScript(LPCSTR szVFS, LPCTSTR szFileName)
{
	if( !P_VALID(szFileName) || szFileName[0] == 0 )
		return NULL;

	DWORD dwID = m_pUtil->Crc32(szFileName);
	if( m_mapScript.IsExist(dwID) )
	{
		this->DebugMsg(_T("same name script already exist:%s\n"), szFileName);
		return (LuaScript*)GT_INVALID;	// 重名脚本已存在
	}

	GUIEditBoxEx* pWndCode = NULL;
	if( m_bDebug )
	{
		m_XmlElement.SetAttribute("Name", m_pUtil->UnicodeToUnicode8(szFileName));
		pWndCode = (GUIEditBoxEx*)m_pGUI->CreateWnd(_T("\\desktop\\luaeditor"), &m_XmlElement);
	}

	// create a script object
	LuaScript* pScrip = new LuaScript(this, pWndCode);
	m_mapScript.Add(dwID, pScrip);

	pScrip->LoadFile(szVFS, szFileName);
	if( m_bDebug )
	{
		m_pWndFileList->SetText(m_pWndFileList->GetTotalRowNum(), 0, szFileName);
		m_pWndFileList->SetCurrentTextRow(m_pWndFileList->GetTotalRowNum()-1);
		m_pWndFileList->SetCurSelectedRow(m_pWndFileList->GetTotalRowNum()-1);
		SetEditScript(pScrip);
	}
	return pScrip;
}


//-----------------------------------------------------------------------------
// Unload a script
//-----------------------------------------------------------------------------
BOOL LuaManager::DestroyScript(LPCTSTR szFileName)
{
	DWORD dwID = m_pUtil->Crc32(szFileName);
	LuaScript* pDelScript = m_mapScript.Peek(dwID);

	if( !P_VALID(pDelScript) )
		return FALSE;	// 不存在

	m_mapScript.Erase(dwID);

	if( m_bDebug )
	{
		// 重新添加一遍文件元素
		m_pGUI->AddToDestroyList(pDelScript->GetCodeWnd());
		
		m_pWndFileList->Clear();
		LuaScript* pScript = NULL;
		m_mapScript.ResetIterator();
		while( m_mapScript.PeekNext(pScript) )
			m_pWndFileList->SetText(m_pWndFileList->GetTotalRowNum(), 0, pScript->GetName().c_str());

		if( pDelScript == m_pEditScript )
			SetEditScript(pScript);
		if( pDelScript == m_pDebugScript )
			SetDebugScript(pScript);

	}

	SAFE_DEL(pDelScript);
	return TRUE;
}


//-----------------------------------------------------------------------------
// handle event
//-----------------------------------------------------------------------------
BOOL LuaManager::EventHandler(tagGUIEvent* pEvent)
{
	GUIWnd* pWnd = m_pGUI->GetWnd(pEvent->dwWndID);
	if( !P_VALID(pWnd) )
		return FALSE;

	switch( pEvent->eEvent )
	{
	case EGUIE_ItemClick:	// 项目被点击:
		if( pWnd == m_pWndFileList )
		{
			INT nRow = m_pWndFileList->GetCurSelectedRow();
			if( nRow >= m_pWndFileList->GetTotalRowNum() )
				return FALSE;
			DWORD dwID = m_pUtil->Crc32(m_pWndFileList->GetText(nRow, 0));
			LuaScript* pScript = m_mapScript.Peek(dwID);
			if( P_VALID(pScript) )
				SetEditScript(pScript);
		}
		break;
	case EGUIE_KeyDown:
		switch(pEvent->msg.dwParam1)
		{
			case VK_F5:	// 重新编译并继续运行
				this->SetDebugState(ELDS_Normal);
				m_pEditScript->SetActiveLine(GT_INVALID);
				m_pEditScript->Rebuild();
				m_pEditScript->Resume(0);
				break;
			case VK_F6:	// 继续运行
				this->SetDebugState(ELDS_Normal);
				m_pEditScript->SetActiveLine(GT_INVALID);
				m_pEditScript->Resume(0);
				break;
			case VK_F9:
				if( pWnd == m_pEditScript->GetCodeWnd() )
					m_pEditScript->SetBreakPoint(m_pEditScript->GetCodeWnd()->GetCursorLine());
				break;
			case VK_F7:
				if( pWnd == m_pEditScript->GetCodeWnd() && m_pDebugScript->GetState() == ELSS_Break )
				{
					this->SetDebugState(ELDS_StepOver);
					m_pDebugScript->Resume(0);
				}
				break;
			case VK_F8:
				if( pWnd == m_pEditScript->GetCodeWnd() && m_pDebugScript->GetState() == ELSS_Break )
				{
					this->SetDebugState(ELDS_StepInto);
					m_pDebugScript->Resume(0);
				}
				break;
		}
		break;

	case EGUIE_CursorMove:
		{
			tstringstream ss;
			ss << _T("Total Line:") << pEvent->dwParam2 << _T("  Current Line:") << pEvent->dwParam1+1;
			m_pWndCodeState->SetText(ss.str().c_str());
		}
		break;
	case EGUIE_Scroll:
		{
			tstringstream ss;
			ss << _T("Total Line:") << pEvent->dwParam2 << _T("  Current Line:") << pEvent->dwParam1+1;
			m_pWndCodeState->SetText(ss.str().c_str());

		}
		break;

	case EGUIE_EditBoxEnter:
		if( pWnd == m_pGUI->GetWnd(_T("\\desktop\\luaeditor\\command")))
		{
			GUIEditBox* pEdit = (GUIEditBox*)pWnd;
			tstring strCommand = pEdit->GetText();
			std::string strRealCommand = m_pUtil->UnicodeToUnicode8(strCommand.c_str());
			m_pEditScript->RunString(strRealCommand.c_str());
		}
		break;

	case EGUIE_UserChangeListContext:	// 用户改变List内容
		if( pWnd == m_pWndWatch )
		{
			INT n = m_pWndWatch->GetTotalRowNum()-1;
			tstring strText = m_pWndWatch->GetText(n, 0);
			if( !(strText == _T("new")) )
			{
				m_pWndWatch->SetText(n+1,0,_T("new"));
				m_pWndWatch->SetText(n+1,1,_T("null"));
				m_pWndWatch->SetItemEditable(n+1,0,TRUE);
				m_pWndWatch->SetItemEditable(n+1,1,TRUE);
			}
		}
		break;
	}

	return FALSE;
}


//-----------------------------------------------------------------------------
// Update
//-----------------------------------------------------------------------------
VOID LuaManager::Update(float fElapsedSec)
{
	LuaScript* pScrip = NULL;
	m_mapScript.ResetIterator();
	while( m_mapScript.PeekNext(pScrip) )
		pScrip->Update(fElapsedSec);
}


//-----------------------------------------------------------------------------
// output debug msg
//-----------------------------------------------------------------------------
VOID LuaManager::DebugMsg(LPCTSTR lpFormat, ...)
{
	TCHAR szTextBuf[1024];
	ZeroMemory(szTextBuf, sizeof(szTextBuf));
	va_list argptr = NULL;
	va_start(argptr, lpFormat);
	_vstprintf_s(szTextBuf, sizeof(szTextBuf)/sizeof(TCHAR), lpFormat, argptr);
	va_end(argptr);

	if( m_bDebug )
	{
		m_pWndDebug->AddText(szTextBuf);
		m_pWndDebug->SetCurLine(0xffffff);	// 卷到最后行
	}
	else
	{	
		IMSG(szTextBuf);
		IMSG(_T("\r\n"));
	}
}


//-----------------------------------------------------------------------------
// 设置当前编辑的脚本
//-----------------------------------------------------------------------------
BOOL LuaManager::SetEditScript(LuaScript* pScript)
{
	m_pEditScript = pScript;
	pScript->GetCodeWnd()->FlipToTop();
	m_pGUI->SetActive(pScript->GetCodeWnd());

	INT nLines = pScript->GetCodeWnd()->GetLineNum();
	INT nCurLines = pScript->GetCodeWnd()->GetCursorLine();

	tstringstream ss;
	ss << _T("Total Line:") << nLines << _T("  Current Line:") << nCurLines+1;
	m_pWndCodeState->SetText(ss.str().c_str());

	if( !P_VALID(m_pDebugScript) )
		m_pDebugScript = pScript;

	tstring strCaption = _T("Lua editor - ");
	strCaption.append(pScript->GetName());
	m_pWndCaption->SetText(strCaption.c_str());
	return TRUE;
}


//-----------------------------------------------------------------------------
// 设置当前正在调试的脚本
//-----------------------------------------------------------------------------
BOOL LuaManager::SetDebugScript(LuaScript* pScript)
{
	m_pDebugScript = pScript;
	return TRUE;
}


//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
VOID LuaManager::SetDebugState(ELuaDebugState eState)
{
	m_eDebugState = eState;
}


//-----------------------------------------------------------------------------
// 脚本状态改变
//-----------------------------------------------------------------------------
VOID LuaManager::OnScriptStateChange(LuaScript* pScript)
{
	if( !m_bDebug )
		return;

	tstring strText = pScript->GetName();
	strText.append(_T(":"));

	switch(pScript->GetState())
	{
	case ELSS_WaitTime:
	case ELSS_Running:
		strText.append(_T("Running"));
		break;
	case ELSS_Break:
		strText.append(_T("Break"));
		break;
	case ELSS_Done:
		strText.append(_T("Done"));
		break;
	}
	
	m_pWndState->SetText(strText.c_str());
}


} // namespace vEngine{