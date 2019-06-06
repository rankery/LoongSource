//--------------------------------------------------------------------------------
//!\file script_mgr.cpp
//!\author Aslan
//!
//!\date 2008-09-01
//! last 2008-09-01
//!
//!\brief Lua脚本管理器，主要负责各种脚本的组织与生成
//!
//!	Copyright (c) 2004 CTCRST Entertainment All rights reserved.
//--------------------------------------------------------------------------------
#include "stdafx.h"

#include "script_mgr.h"
#include "lua_functions.h"
#include "world.h"
#include "unit.h"
#include "role.h"
#include "creature.h"
#include "creature_ai.h"
#include "map.h"
#include "activity_mgr.h"
#include "TimeChecker.h"

ScriptMgr g_ScriptMgr;

//------------------------------------------------------------------------------------
// 初始化
//------------------------------------------------------------------------------------
BOOL ScriptMgr::Init()
{
	m_pUtil = "Util";

	RegisterScriptEventFunc();

	m_pMasterState = luaL_newstate();
	luaL_openlibs(m_pMasterState);

	m_mapThreadState.Clear();

	m_dwScriptMutexIDGen = 0;
	m_mapScriptMutex.Clear();

	m_mapQusetScript.Clear();
	m_mapCreatureScript.Clear();
	m_mapMapScript.Clear();
	m_mapItemScript.Clear();
	m_mapSkillScript.Clear();
	m_mapActScript.Clear();
	m_pRoleScript = NULL;
	m_pWorldScript = NULL;
	m_dwMaxPcallTime = 0;

	RegisterCoreFunctions();		// 注册C接口函数
	LoadScripts();					// 加载脚本

	return TRUE;
}

//-------------------------------------------------------------------------------------
// 注册异步调用函数
//-------------------------------------------------------------------------------------
VOID ScriptMgr::RegisterScriptEventFunc()
{
	RegisterEventFunc(EVT_Script_Reload,		&ScriptMgr::OnReload);
}

//-------------------------------------------------------------------------------------
// 异步事件——脚本重新加载
//-------------------------------------------------------------------------------------
VOID ScriptMgr::OnReload(DWORD dwSenderID, LPVOID pEventMessage)
{
	Reload();
}

//-------------------------------------------------------------------------------------
// 脚本管理器销毁
//-------------------------------------------------------------------------------------
VOID ScriptMgr::Destroy()
{
	DestroyScripts();	// 删除所有脚本
	DestroyAllStates();	// 删除所有State相关内容
}

//--------------------------------------------------------------------------------------
// 更新
//--------------------------------------------------------------------------------------
VOID ScriptMgr::Update()
{
	CHECK_TIME();
	EventMgr<ScriptMgr>::Update();
}

//--------------------------------------------------------------------------------------------------
// 重新加载脚本
//--------------------------------------------------------------------------------------------------
VOID ScriptMgr::Reload()
{
	UnloadScripts();	// 卸载所有脚本
	DestroyAllStates();	// 删除所有State相关内容

	// 重新生成主State
	m_pMasterState = luaL_newstate();
	luaL_openlibs(m_pMasterState);

	RegisterCoreFunctions();		// 注册C接口函数
	LoadScripts();					// 加载脚本
}

//--------------------------------------------------------------------------------------------------
// 注册供Lua使用的函数
//--------------------------------------------------------------------------------------------------
VOID ScriptMgr::RegisterCoreFunctions()
{
	LuaOpenCommLibs(m_pMasterState);
}


//----------------------------------------------------------------------------------------------------
// Lua引擎加载脚本
//-----------------------------------------------------------------------------------------------------
VOID ScriptMgr::LoadScripts()
{
	vector<tstring> luaFiles;

	// 得到脚本的路径
	TObjRef<VarContainer> pVar = "VarContainer";
	tstring strPath = pVar->GetString(_T("path"), _T("script"));

	// 将所有Lua文件存储进来
	LoadScriptsFromDir(strPath.c_str(), luaFiles, TRUE);

	IMSG(_T("Loading %d Scripts...\r\n"), luaFiles.size());

	TCHAR szFileName[MAX_PATH];

	// 依次加载各个Lua文件，并编译执行
	for(vector<tstring>::iterator itr = luaFiles.begin(); itr != luaFiles.end(); ++itr)
	{
		_sntprintf(szFileName, MAX_PATH, _T("%s"), itr->c_str());
		IMSG(_T("Loading Script %s...\r\n"), itr->c_str());

		// 载入并编译文件
		if( luaL_loadfile(m_pMasterState, m_pUtil->UnicodeToAnsi(szFileName)) != 0 )
		{
			IMSG(_T("Script %s, load failed!!!\r\n"), itr->c_str());
			const CHAR* szMsg = lua_tostring(m_pMasterState, -1);
			if( szMsg != NULL )
			{
				IMSG(_T("LuaEngine, reason: %s\r\n"), m_pUtil->AnsiToUnicode(szMsg));
			}

			lua_pop(m_pMasterState, 1);

			continue;
		}

		// 执行Lua文件
		if( lua_pcall(m_pMasterState, 0, LUA_MULTRET, 0) != 0 )
		{
			IMSG(_T("LuaEngine, could not run %s!!!!"), itr->c_str());
			const CHAR* szMsg = lua_tostring(m_pMasterState, -1);
			if( szMsg != NULL )
			{
				IMSG(_T("LuaEngine, reason: %s"), m_pUtil->AnsiToUnicode(szMsg));
			}

			lua_pop(m_pMasterState, 1);

			continue;
		}
	}

	// 清空堆栈
	lua_settop(m_pMasterState, 0);

}

//-----------------------------------------------------------------------------------
// 从目录搜索
//-----------------------------------------------------------------------------------
VOID ScriptMgr::LoadScriptsFromDir(LPCTSTR szDir, std::vector<tstring>& luaFiles, BOOL bFirst)
{
	if( bFirst )
	{
		// 如果是第一次进入该函数，则寻找全局脚本目录
		TCHAR szGlobalDir[MAX_PATH];
		_tcscpy(szGlobalDir, szDir);
		_tcscat(szGlobalDir, _T("\\"));
		_tcscat(szGlobalDir, _T("gloable"));
		LoadScriptsFromDir(szGlobalDir, luaFiles);
	}

	HANDLE hFile;
	WIN32_FIND_DATA fd;
	memset(&fd, 0, sizeof(fd));

	TCHAR szSearchName[MAX_PATH];

	_tcscpy(szSearchName, szDir);
	_tcscat(szSearchName, _T("\\*.*"));

	hFile = FindFirstFile(szSearchName, &fd);
	FindNextFile(hFile, &fd);

	while( FindNextFile(hFile, &fd) )
	{
		// 如果是目录，则递归调用
		if( fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
		{
			if( !bFirst || 0 != _tcscmp(fd.cFileName, _T("gloable")) )
			{
				_tcscpy(szSearchName, szDir);
				_tcscat(szSearchName, _T("\\"));
				_tcscat(szSearchName, fd.cFileName);
				LoadScriptsFromDir(szSearchName, luaFiles);
			}
		}
		else
		{
			TCHAR* ext = _tcsrchr(fd.cFileName, _T('.'));
			
			if(P_VALID(ext) && !_tcsicmp(ext, _T(".lua")) )
			{
				tstring fname = szDir;
				fname += _T("\\");
				fname += fd.cFileName;

				luaFiles.push_back(fname);
			}
		}
	}
	FindClose(hFile);
}

//-----------------------------------------------------------------------------------
// 删除所有脚本
//-----------------------------------------------------------------------------------
VOID ScriptMgr::DestroyScripts()
{
	// 清空任务脚本
	QuestScript* pQuestScript = NULL;
	m_mapQusetScript.ResetIterator();
	while( m_mapQusetScript.PeekNext(pQuestScript) )
	{
		SAFE_DEL(pQuestScript);
	}
	m_mapQusetScript.Clear();

	// 清空怪物脚本
	CreatureScript* pCreatureScript = NULL;
	m_mapCreatureScript.ResetIterator();
	while( m_mapCreatureScript.PeekNext(pCreatureScript) )
	{
		SAFE_DEL(pCreatureScript);
	}
	m_mapCreatureScript.Clear();

	// 清空地图脚本
	MapScript* pMapScript = NULL;
	m_mapMapScript.ResetIterator();
	while( m_mapMapScript.PeekNext(pMapScript) )
	{
		SAFE_DEL(pMapScript);
	}
	m_mapMapScript.Clear();

	// 清空活动脚本
	ActScript* pActScript = NULL;
	m_mapActScript.ResetIterator();
	while( m_mapActScript.PeekNext(pActScript) )
	{
		SAFE_DEL(pActScript);
	}
	m_mapActScript.Clear();

	// 清空物品脚本
	ItemScript* pItemScript = NULL;
	m_mapItemScript.ResetIterator();
	while( m_mapItemScript.PeekNext(pItemScript) )
	{
		SAFE_DEL(pItemScript);
	}
	m_mapItemScript.Clear();

	// 清空物品脚本
	SkillScript* pSkillScript = NULL;
	m_mapSkillScript.ResetIterator();
	while( m_mapSkillScript.PeekNext(pSkillScript) )
	{
		SAFE_DEL(pSkillScript);
	}
	m_mapSkillScript.Clear();

	// 清空玩家脚本
	if( P_VALID(m_pRoleScript) )
	{
		SAFE_DEL(m_pRoleScript);
	}

	// 清空世界脚本
	if( P_VALID(m_pWorldScript) )
	{
		SAFE_DEL(m_pWorldScript);
	}
}

//-----------------------------------------------------------------------------------
// 卸载所有脚本
//-----------------------------------------------------------------------------------
VOID ScriptMgr::UnloadScripts()
{
	// 重置任务脚本
	QuestScript* pQuestScript = NULL;
	m_mapQusetScript.ResetIterator();
	while( m_mapQusetScript.PeekNext(pQuestScript) )
	{
		pQuestScript->Destroy();
	}

	// 重置怪物脚本
	CreatureScript* pCreatureScript = NULL;
	m_mapCreatureScript.ResetIterator();
	while( m_mapCreatureScript.PeekNext(pCreatureScript) )
	{
		pCreatureScript->Destroy();
	}

	// 重置地图脚本
	MapScript* pMapScript = NULL;
	m_mapMapScript.ResetIterator();
	while( m_mapMapScript.PeekNext(pMapScript) )
	{
		pMapScript->Destroy();
	}

	// 重置活动脚本
	ActScript* pActScript = NULL;
	m_mapActScript.ResetIterator();
	while( m_mapActScript.PeekNext(pActScript) )
	{
		pActScript->Destroy();
	}

	// 重置物品脚本
	ItemScript* pItemScript = NULL;
	m_mapItemScript.ResetIterator();
	while( m_mapItemScript.PeekNext(pItemScript) )
	{
		pItemScript->Destroy();
	}

	// 重置物品脚本
	SkillScript* pSkillScript = NULL;
	m_mapSkillScript.ResetIterator();
	while( m_mapSkillScript.PeekNext(pSkillScript) )
	{
		pSkillScript->Destroy();
	}

	// 重置玩家脚本
	if( P_VALID(m_pRoleScript) )
	{
		m_pRoleScript->Destroy();
	}

	// 清空世界脚本
	if( P_VALID(m_pWorldScript) )
	{
		m_pWorldScript->Destroy();
	}
}

//-----------------------------------------------------------------------------------
// 卸载所有的State相关内容
//-----------------------------------------------------------------------------------
VOID ScriptMgr::DestroyAllStates()
{
	// 删除所有的线程状态
	std::list<DWORD> listThreadID;
	m_mapThreadState.ExportAllKey(listThreadID);

	for( std::list<DWORD>::iterator it = listThreadID.begin(); it != listThreadID.end(); ++it )
	{
		DWORD dwThreadID = *it;

		char szBuf[64] = {0};
		_ultoa_s(dwThreadID, szBuf, 10);

		lua_pushnil(m_pMasterState);
		lua_setfield(m_pMasterState, LUA_REGISTRYINDEX, szBuf);
	}
	m_mapThreadState.Clear();

	// 删除主state
	lua_close(m_pMasterState);

	// 删除所有脚本锁
	TMap<DWORD, Mutex*>::TMapIterator it = m_mapScriptMutex.Begin();
	Mutex* pMutex = NULL;

	while( m_mapScriptMutex.PeekNext(it, pMutex) )
	{
		SAFE_DEL(pMutex);
	}
	m_mapScriptMutex.Clear();
}

//-----------------------------------------------------------------------------------
// 得到一个可用的thread
//-----------------------------------------------------------------------------------
lua_State* ScriptMgr::GetThreadState()
{
	//DWORD dwThreadID = GetCurrentThreadId();

	//lua_State* pThreadState = m_mapThreadState.Peek(dwThreadID);

	//if( !P_VALID(pThreadState) )
	//{
	//	pThreadState = CreateThreadState(dwThreadID);
	//	ASSERT( P_VALID(pThreadState) );
	//}

	//return pThreadState;

	return m_pMasterState;
}

//-----------------------------------------------------------------------------------
// 生成一个新的thread
//-----------------------------------------------------------------------------------
lua_State* ScriptMgr::CreateThreadState(DWORD dwThreadID)
{
	IMSG(_T("create thread state, thread is %u\r\n"), dwThreadID);

	m_Lock.Acquire();

	lua_State* pThreadState = lua_newthread(m_pMasterState);

	// 将新的state写入注册表，防止被垃圾收集
	char szBuf[64] = {0};
	_ultoa_s(dwThreadID, szBuf, 10);
	lua_setfield(m_pMasterState, LUA_REGISTRYINDEX, szBuf);

	// 将新的state加入到map中
	m_mapThreadState.Add(dwThreadID, pThreadState);

	m_Lock.Release();

	return pThreadState;
}

//-----------------------------------------------------------------------------------
// 错误屏显
//-----------------------------------------------------------------------------------
VOID ScriptMgr::ErrMsg(lua_State* pState)
{
	IMSG(_T("Script Error: %s\r\n"), m_pUtil->AnsiToUnicode(lua_tostring(pState, -1)));
	lua_pop(pState, 1);
}

//-----------------------------------------------------------------------------------
// 通用脚本调用函数（目前还不能返回字符串）
//-----------------------------------------------------------------------------------
VOID ScriptMgr::CallScriptFunction(CHAR* szFunction, CHAR* szFormat, ...)
{
	CHECK_TIME_WITH_INFO(szFunction);

	if( !P_VALID(szFunction) ) return;

	lua_State* pThreadState = GetThreadState();
	if( !P_VALID(pThreadState) ) return;

	m_Lock.Acquire();

	DWORD dwStartTime = timeGetTime();

	lua_getglobal(pThreadState, szFunction);
	if( lua_isnil(pThreadState, -1) )
	{
		IMSG(_T("Script Error: unknown function %s\r\n"), m_pUtil->AnsiToUnicode(szFunction));
		lua_pop(pThreadState, 1);

		m_Lock.Release();
		return;
	}

	// from the pil book
	va_list vl;
	INT nArg = 0, nRes = 0;		// 参数和结果的数量

	va_start(vl, szFormat);

	// 压入参数
	BOOL bArgEnd = FALSE;
	for(nArg = 0; *szFormat && FALSE == bArgEnd; ++nArg)
	{
		luaL_checkstack(pThreadState, 1, "too many arguments");

		switch(*szFormat++)
		{
		case 'd':	// double参数
			{
				double d = va_arg(vl, double);
				lua_pushnumber(pThreadState, d);
			}
			break;

		case 'f':	// float参数
			{
				double f = va_arg(vl, double);
				lua_pushnumber(pThreadState, f);
			}
			break;

		case 'u':	// dword参数
			{
				unsigned int u = va_arg(vl, unsigned int);
				lua_pushnumber(pThreadState, u);
			}
			break;

		case 'i':	// int参数
			{
				int i = va_arg(vl, int);
				lua_pushinteger(pThreadState, i);
			}
			break;

		case 's':	// 字符串参数
			{
				char* c = (char*)va_arg(vl, unsigned int);
				lua_pushstring(pThreadState, c);
			}
			break;

		case '>':	// 参数结束
			bArgEnd = TRUE;
			goto _arg_end; 
			
		default:
			break;
		}
	}

_arg_end:

	// 调用函数
	nRes = strlen(szFormat);
	if( lua_pcall(pThreadState, nArg, nRes, 0) != 0 )
	{
		ErrMsg(pThreadState);

		m_Lock.Release();
		return;
	}

	INT nNum = lua_gettop(pThreadState);

	// 检索结果
	INT nIndex = -nRes;

	while(*szFormat)
	{
		switch(*szFormat++)
		{
		case 'd':		// double结果
			*va_arg(vl, double*) = lua_tonumber(pThreadState, nIndex);
			break;

		case 'f':		// float结果
			*va_arg(vl, float*)  = lua_tonumber(pThreadState, nIndex);
			break;

		case 'u':		// dword结果
			*va_arg(vl, unsigned int*) = lua_tonumber(pThreadState, nIndex);
			break;

		case 'i':		// int结果
			*va_arg(vl, int*)  = lua_tointeger(pThreadState, nIndex);
			break;

		case 'b':		// BOOL结果
			*va_arg(vl, BOOL*)  = lua_toboolean(pThreadState, nIndex);
			break;

		default:
			break;
		}

		++nIndex;
	}

	lua_pop(pThreadState, nRes);

	// 结束
	va_end(vl);

	DWORD dwEndTime = timeGetTime();

	if(m_dwMaxPcallTime < (dwEndTime - dwStartTime) )
	{
		m_dwMaxPcallTime = dwEndTime - dwStartTime;
		ILOG->Write(_T("Max calling time script function is <%s>, useing %d millisecond.\r\n"), m_pUtil->Unicode8ToUnicode(szFunction), m_dwMaxPcallTime);
	}

	m_Lock.Release();
}

//-----------------------------------------------------------------------------------
// 注册怪物AI脚本函数
//-----------------------------------------------------------------------------------
VOID ScriptMgr::RegisterCreatureEvent(DWORD dwID, EScriptCreatureEvent eEvent, const CHAR* szFunction)
{
	ASSERT( GT_VALID(dwID) && ( eEvent >= 0 && eEvent < ESCAE_End ) && P_VALID(szFunction) );

	// 首先查找该怪物是否已经有相应的脚本了，如果没有就生成
	CreatureScript* pScript = m_mapCreatureScript.Peek(dwID);

	if( !P_VALID(pScript) )
	{
		pScript = new CreatureScript;
		m_mapCreatureScript.Add(dwID, pScript);
	}

	// 注册
	pScript->RegisterFunc(eEvent, szFunction);
}

//-----------------------------------------------------------------------------------
// 注册玩家脚本函数
//-----------------------------------------------------------------------------------
VOID ScriptMgr::RegisterRoleEvent(EScriptRoleEvent eEvent, const CHAR* szFunction)
{
	ASSERT( (eEvent >= 0 && eEvent < ESRE_End) && P_VALID(szFunction) );

	// 首先查找该玩家是否已经有相应的脚本了，如果没有就生成
	if( !P_VALID(m_pRoleScript) )
	{
		m_pRoleScript = new RoleScript;
	}

	// 注册
	m_pRoleScript->RegisterFunc(eEvent, szFunction);
}

//-----------------------------------------------------------------------------------
// 注册地图脚本函数
//-----------------------------------------------------------------------------------
VOID ScriptMgr::RegisterMapEvent(const CHAR* szMapName, EScriptMapEvent eEvent, const CHAR* szFunction)
{
	ASSERT( P_VALID(szMapName) && (eEvent >= 0 && eEvent < ESME_End) && P_VALID(szFunction) );

	DWORD dwMapID = IUTIL->Crc32(IUTIL->Unicode8ToUnicode(szMapName));

	// 如果没有地图脚本就生成
	MapScript* pScript = m_mapMapScript.Peek(dwMapID);
	if( !P_VALID(pScript) )
	{
		pScript = new MapScript;
		m_mapMapScript.Add(dwMapID, pScript);
	}

	// 注册
	pScript->RegisterFunc(eEvent, szFunction);
}

//-----------------------------------------------------------------------------------
// 注册任务脚本函数
//-----------------------------------------------------------------------------------
VOID ScriptMgr::RegisterQuestEvent(UINT16 u16ID, EScriptQuestEvent eEvent, const CHAR* szFunction)
{
	ASSERT( GT_VALID(u16ID) && ( eEvent >= 0 && eEvent < ESQE_End ) && P_VALID(szFunction) );

	// 首先查找该任务是否已经有相应的脚本了，如果没有就生成
	QuestScript* pScript = m_mapQusetScript.Peek(u16ID);
	if( !P_VALID(pScript) )
	{
		pScript = new QuestScript;
		m_mapQusetScript.Add(u16ID, pScript);
	}

	// 注册
	pScript->RegisterFunc(eEvent, szFunction);
}

//-----------------------------------------------------------------------------------
// 注册固定活动脚本函数
//-----------------------------------------------------------------------------------
VOID ScriptMgr::RegisterActEvent(DWORD dwActID, EScriptActEvent eEvent, const CHAR* szFunction)
{
	ASSERT( GT_VALID(dwActID) && ( eEvent >= 0 && eEvent < ESAE_End ) && P_VALID(szFunction) );

	// 首先查找该活动是否已经有相应的脚本了，如果没有就生成
	ActScript* pScript = m_mapActScript.Peek(dwActID);
	if( !P_VALID(pScript) )
	{
		pScript = new ActScript;
		m_mapActScript.Add(dwActID, pScript);
	}

	// 注册
	pScript->RegisterFunc(eEvent, szFunction);
}

//-----------------------------------------------------------------------------------
// 注册游戏世界脚本函数
//-----------------------------------------------------------------------------------
VOID ScriptMgr::RegisterWorldEvent(EScriptWorldEvent eEvent, const CHAR* szFunction)
{
	ASSERT(( eEvent >= 0 && eEvent < ESWE_End ) && P_VALID(szFunction) );

	if( !P_VALID(m_pWorldScript) )
	{
		m_pWorldScript = new WorldScript;
	}

	m_pWorldScript->RegisterFunc(eEvent, szFunction);
}

//-----------------------------------------------------------------------------------
// 注册游戏世界脚本函数
//-----------------------------------------------------------------------------------
VOID ScriptMgr::RegisterItemEvent(DWORD dwTypeID, EScriptItemEvent eEvent, const CHAR* szFunction)
{
	ASSERT( GT_VALID(dwTypeID) && ( eEvent >= 0 && eEvent < ESAE_End ) && P_VALID(szFunction) );

	ItemScript* pScript = m_mapItemScript.Peek(dwTypeID);
	if( !P_VALID(pScript) )
	{
		pScript = new ItemScript;
		m_mapItemScript.Add(dwTypeID, pScript);
	}

	pScript->RegisterFunc(eEvent, szFunction);
}

//-----------------------------------------------------------------------------------
// 注册游戏世界脚本函数
//-----------------------------------------------------------------------------------
VOID ScriptMgr::RegisterSkillEvent(DWORD dwTypeID, EScriptSkillEvent eEvent, const CHAR* szFunction)
{
	ASSERT( GT_VALID(dwTypeID) && ( eEvent >= 0 && eEvent < ESAE_End ) && P_VALID(szFunction) );

	SkillScript* pScript = m_mapSkillScript.Peek(dwTypeID);
	if( !P_VALID(pScript) )
	{
		pScript = new SkillScript;
		m_mapSkillScript.Add(dwTypeID, pScript);
	}

	pScript->RegisterFunc(eEvent, szFunction);
}


//---------------------------------------------------------------------------------------
// 任务脚本对象的相关事件函数
//---------------------------------------------------------------------------------------
INT  QuestScript::CheckAccept(UINT16 u16QuestID, Role* pRole, Creature* pNPC) const
{
	if( !P_VALID(m_szFunc[ESQE_On_CheckAccept]) ) return TRUE;

	Map* pMap = pRole->GetMap();
	if( !P_VALID(pMap) ) return FALSE;

	DWORD dwMapID = pMap->GetMapID();
	DWORD dwInstanceID = pMap->GetInstanceID();
	DWORD dwNPCID = P_VALID(pNPC) ? pNPC->GetID() : GT_INVALID;

	INT nRet = 0;
	g_ScriptMgr.CallScriptFunction(m_szFunc[ESQE_On_CheckAccept], "uuiuu>i", dwMapID, dwInstanceID, u16QuestID, pRole->GetID(), dwNPCID, &nRet);

	return nRet;
}

INT  QuestScript::CheckComplete(UINT16 u16QuestID, Role* pRole, Creature* pNPC) const
{
	if( !P_VALID(m_szFunc[ESQE_On_CheckComplete]) ) return TRUE;

	Map* pMap = pRole->GetMap();
	if( !P_VALID(pMap) ) return FALSE;

	DWORD dwMapID = pMap->GetMapID();
	DWORD dwInstanceID = pMap->GetInstanceID();
	DWORD dwNPCID = P_VALID(pNPC) ? pNPC->GetID() : GT_INVALID;

	INT nRet = 0;
	g_ScriptMgr.CallScriptFunction(m_szFunc[ESQE_On_CheckComplete], "uuiuu>i", dwMapID, dwInstanceID, u16QuestID, pRole->GetID(), dwNPCID, &nRet);

	return nRet;
}

VOID QuestScript::OnInit(UINT16 u16QuestID, Role* pOwner) const
{
	if( !P_VALID(m_szFunc[ESQE_On_Init]) ) return;

	Map* pMap = pOwner->GetMap();
	if(!P_VALID(pMap))	return;

	DWORD dwMapID = pMap->GetMapID();
	DWORD dwInstanceID = pMap->GetInstanceID();

	g_ScriptMgr.CallScriptFunction(m_szFunc[ESQE_On_Init], "uuiu", dwMapID,dwInstanceID, u16QuestID, pOwner->GetID());
}

VOID QuestScript::OnAccept(UINT16 u16QuestID, Role* pOwner, Creature* pNPC) const
{
	if( !P_VALID(m_szFunc[ESQE_On_Accept]) ) return;

	Map* pMap = pOwner->GetMap();
	if( !P_VALID(pMap) ) return;

	DWORD dwMapID = pMap->GetMapID();
	DWORD dwInstanceID = pMap->GetInstanceID();
	DWORD dwNPCID = P_VALID(pNPC) ? pNPC->GetID() : GT_INVALID;

	g_ScriptMgr.CallScriptFunction(m_szFunc[ESQE_On_Accept], "uuiuu", dwMapID, dwInstanceID, u16QuestID, pOwner->GetID(), dwNPCID);
}

VOID QuestScript::OnComplete(UINT16 u16QuestID, Role* pOwner, Creature* pNPC) const
{
	if( !P_VALID(m_szFunc[ESQE_On_Complete]) ) return;

	Map* pMap = pOwner->GetMap();
	if( !P_VALID(pMap) ) return;

	DWORD dwMapID = pMap->GetMapID();
	DWORD dwInstanceID = pMap->GetInstanceID();
	DWORD dwNPCID = P_VALID(pNPC) ? pNPC->GetID() : GT_INVALID;

	g_ScriptMgr.CallScriptFunction(m_szFunc[ESQE_On_Complete], "uuiuu", dwMapID, dwInstanceID, u16QuestID, pOwner->GetID(), dwNPCID);
}

VOID QuestScript::OnCancel(UINT16 u16QuestID, Role* pOwner) const
{
	if( !P_VALID(m_szFunc[ESQE_On_Cancel]) ) return;

	Map* pMap = pOwner->GetMap();
	if( !P_VALID(pMap) ) return;

	DWORD dwMapID = pMap->GetMapID();
	DWORD dwInstanceID = pMap->GetInstanceID();

	g_ScriptMgr.CallScriptFunction(m_szFunc[ESQE_On_Cancel], "uuiu", dwMapID, dwInstanceID, u16QuestID, pOwner->GetID());
}

VOID QuestScript::OnCreatureKill(UINT16 u16QuestID, Role* pOwner, DWORD dwCreatureTypeID) const
{
	if( !P_VALID(m_szFunc[ESQE_On_Creature_Kill]) ) return;

	Map* pMap = pOwner->GetMap();
	if( !P_VALID(pMap) ) return;

	DWORD dwMapID = pMap->GetMapID();
	DWORD dwInstanceID = pMap->GetInstanceID();

	g_ScriptMgr.CallScriptFunction(m_szFunc[ESQE_On_Creature_Kill], "uuiuu", dwMapID, dwInstanceID, u16QuestID, pOwner->GetID(), dwCreatureTypeID);
}

VOID QuestScript::OnNPCTalk(UINT16 u16QuestID, Role* pOwner, DWORD dwNPCID, DWORD dwNPCTypeID) const
{
	if( !P_VALID(m_szFunc[ESQE_On_NPC_Talk]) ) return;

	Map* pMap = pOwner->GetMap();
	if( !P_VALID(pMap) ) return;

	DWORD dwMapID = pMap->GetMapID();
	DWORD dwInstanceID = pMap->GetInstanceID();

	g_ScriptMgr.CallScriptFunction(m_szFunc[ESQE_On_NPC_Talk], "uuiuuu", dwMapID, dwInstanceID, u16QuestID, pOwner->GetID(), dwNPCID, dwNPCTypeID);
}

VOID QuestScript::OnDlgDefault(UINT16 u16QuestID, Role* pOwner, DWORD dwDlgOption) const
{
	if( !P_VALID(m_szFunc[ESQE_On_Dlg_Default]) ) return;

	Map* pMap = pOwner->GetMap();
	if(!P_VALID(pMap)) return;

	DWORD dwMapID = pMap->GetMapID();
	DWORD dwInstanceID = pMap->GetInstanceID();

	g_ScriptMgr.CallScriptFunction(m_szFunc[ESQE_On_Dlg_Default], "uuiuu", dwMapID, dwInstanceID, u16QuestID, pOwner->GetID(), dwDlgOption);
}

VOID QuestScript::OnInvest(UINT16 u16QuestID, Role* pOwner, DWORD dwCreatureTypeID) const
{
	if(!P_VALID(m_szFunc[ESQE_On_Invest])) return;

	Map* pMap = pOwner->GetMap();
	if(!P_VALID(pMap))	return;

	DWORD dwMapID = pMap->GetMapID();
	DWORD dwInstanceID = pMap->GetInstanceID();

	g_ScriptMgr.CallScriptFunction(m_szFunc[ESQE_On_Invest], "uuiuu", dwMapID, dwInstanceID, u16QuestID, pOwner->GetID(), dwCreatureTypeID);
}

//-----------------------------------------------------------------------------------------
// 怪物脚本对象的相关事件函数
//-----------------------------------------------------------------------------------------
VOID CreatureScript::OnLoad(Creature* pCreature) const
{
	if( !P_VALID(m_szFunc[ESCAE_On_Load]) ) return;

	Map* pMap = pCreature->GetMap();
	if( !P_VALID(pMap) ) return;

	DWORD dwMapID = pMap->GetMapID();
	DWORD dwInstanceID = pMap->GetInstanceID();

	g_ScriptMgr.CallScriptFunction(m_szFunc[ESCAE_On_Load], "uuu", dwMapID, dwInstanceID, pCreature->GetID());
}

VOID CreatureScript::OnTalk(Creature* pCreature, Role* pRole, INT nIndex) const
{
	if( !P_VALID(m_szFunc[ESCAE_On_Talk]) ) return;

	Map* pMap = pCreature->GetMap();
	if( !P_VALID(pMap) ) return;

	DWORD dwMapID = pMap->GetMapID();
	DWORD dwInstanceID = pMap->GetInstanceID();

	g_ScriptMgr.CallScriptFunction(m_szFunc[ESCAE_On_Talk], "uuuuui", dwMapID, dwInstanceID, pCreature->GetID(), pCreature->GetTypeID(), pRole->GetID(), nIndex);
}

VOID CreatureScript::OnRespawn(Creature* pCreature) const
{
	if( !P_VALID(m_szFunc[ESCAE_On_Respawn]) ) return;

	Map* pMap = pCreature->GetMap();
	if( !P_VALID(pMap) ) return;

	DWORD dwMapID = pMap->GetMapID();
	DWORD dwInstanceID = pMap->GetInstanceID();

	g_ScriptMgr.CallScriptFunction(m_szFunc[ESCAE_On_Respawn], "uuu", dwMapID, dwInstanceID, pCreature->GetID());
}

VOID CreatureScript::OnEnterCombat(Creature* pCreature) const
{
	if( !P_VALID(m_szFunc[ESCAE_On_Enter_Combat]) ) return;

	Map* pMap = pCreature->GetMap();
	if( !P_VALID(pMap) ) return;

	DWORD dwMapID = pMap->GetMapID();
	DWORD dwInstanceID = pMap->GetInstanceID();

	g_ScriptMgr.CallScriptFunction(m_szFunc[ESCAE_On_Enter_Combat], "uuu", dwMapID, dwInstanceID, pCreature->GetID());
}

VOID CreatureScript::OnLeaveCombat(Creature* pCreature) const
{
	if( !P_VALID(m_szFunc[ESCAE_On_Leave_Combat]) ) return;

	Map* pMap = pCreature->GetMap();
	if( !P_VALID(pMap) ) return;

	DWORD dwMapID = pMap->GetMapID();
	DWORD dwInstanceID = pMap->GetInstanceID();

	g_ScriptMgr.CallScriptFunction(m_szFunc[ESCAE_On_Leave_Combat], "uuu", dwMapID, dwInstanceID, pCreature->GetID());
}


VOID CreatureScript::OnDie(Creature* pCreature, Unit* pKiller, BOOL& bRemove) const
{
	if( !P_VALID(m_szFunc[ESCAE_On_Die]) ) return;

	Map* pMap = pCreature->GetMap();
	if( !P_VALID(pMap) ) return;

	DWORD dwMapID = pMap->GetMapID();
	DWORD dwInstanceID = pMap->GetInstanceID();
	DWORD dwKillerID = P_VALID(pKiller) ? pKiller->GetID() : GT_INVALID;

	g_ScriptMgr.CallScriptFunction(m_szFunc[ESCAE_On_Die], "uuuuu>b", dwMapID, dwInstanceID, pCreature->GetID(), pCreature->GetTypeID(), dwKillerID, &bRemove);
}

VOID CreatureScript::OnInvest(Creature* pCreature, Role* pScr) const
{
	if( !P_VALID(m_szFunc[ESCAE_On_Invest])) return;

	Map* pMap = pCreature->GetMap();
	if(!P_VALID(pMap))	return;

	DWORD dwMapID = pMap->GetMapID();
	DWORD dwInstanceID = pMap->GetInstanceID();

	g_ScriptMgr.CallScriptFunction(m_szFunc[ESCAE_On_Invest], "uuuuu", dwMapID, dwInstanceID, pCreature->GetID(), pCreature->GetTypeID(), pScr->GetID());
}

VOID CreatureScript::OnUpdateAI(Creature* pCreature) const
{
	if( !P_VALID(m_szFunc[ESCAE_On_UpdateAI]) ) return;

	Map* pMap = pCreature->GetMap();
	if(!P_VALID(pMap))	return;

	AIController* pAI = pCreature->GetAI();
	if( !P_VALID(pAI) ) return;

	DWORD dwMapID = pMap->GetMapID();
	DWORD dwInstanceID = pMap->GetInstanceID();

	g_ScriptMgr.CallScriptFunction(m_szFunc[ESCAE_On_UpdateAI], "uuuu", dwMapID, dwInstanceID, pCreature->GetID(), pAI->GetCurrentStateType());
}

VOID CreatureScript::OnUpdateCurAI(Creature* pCreature) const
{
	if( !P_VALID(m_szFunc[ESCAE_On_UpdateCurAI]) ) return;

	Map* pMap = pCreature->GetMap();
	if(!P_VALID(pMap))	return;

	AIController* pAI = pCreature->GetAI();
	if( !P_VALID(pAI) ) return;

	DWORD dwMapID = pMap->GetMapID();
	DWORD dwInstanceID = pMap->GetInstanceID();

	g_ScriptMgr.CallScriptFunction(m_szFunc[ESCAE_On_UpdateCurAI], "uuuu", dwMapID, dwInstanceID, pCreature->GetID(), pAI->GetCurrentStateType());
}

VOID CreatureScript::OnEnterCurAI(Creature* pCreature) const
{
	if( !P_VALID(m_szFunc[ESCAE_On_EnterCurAI]) ) return;

	Map* pMap = pCreature->GetMap();
	if(!P_VALID(pMap))	return;

	AIController* pAI = pCreature->GetAI();
	if( !P_VALID(pAI) ) return;

	DWORD dwMapID = pMap->GetMapID();
	DWORD dwInstanceID = pMap->GetInstanceID();

	g_ScriptMgr.CallScriptFunction(m_szFunc[ESCAE_On_EnterCurAI], "uuuu", dwMapID, dwInstanceID, pCreature->GetID(), pAI->GetCurrentStateType());
}

VOID CreatureScript::OnLeaveCurAI(Creature* pCreature) const
{
	if( !P_VALID(m_szFunc[ESCAE_On_LeaveCurAI]) ) return;

	Map* pMap = pCreature->GetMap();
	if(!P_VALID(pMap))	return;

	AIController* pAI = pCreature->GetAI();
	if( !P_VALID(pAI) ) return;

	DWORD dwMapID = pMap->GetMapID();
	DWORD dwInstanceID = pMap->GetInstanceID();

	g_ScriptMgr.CallScriptFunction(m_szFunc[ESCAE_On_LeaveCurAI], "uuuu", dwMapID, dwInstanceID, pCreature->GetID(), pAI->GetCurrentStateType());
}

BOOL CreatureScript::OnEventCurAI(Creature* pCreature) const
{
	if( !P_VALID(m_szFunc[ESCAE_On_EventCurAI]) ) return FALSE;

	Map* pMap = pCreature->GetMap();
	if(!P_VALID(pMap))	return FALSE;

	AIController* pAI = pCreature->GetAI();
	if( !P_VALID(pAI) ) return FALSE;

	DWORD dwMapID = pMap->GetMapID();
	DWORD dwInstanceID = pMap->GetInstanceID();

	BOOL bRet = FALSE;
	g_ScriptMgr.CallScriptFunction(m_szFunc[ESCAE_On_EventCurAI], "uuuu>i", dwMapID, dwInstanceID, pCreature->GetID(), pAI->GetCurrentStateType(), bRet);

	return bRet;
}

VOID CreatureScript::OnDisappear( Creature* pCreature ) const
{
	if( !P_VALID(m_szFunc[ESCAE_On_Disappear]) ) return;

	Map* pMap = pCreature->GetMap();
	if(!P_VALID(pMap))	return;

	AIController* pAI = pCreature->GetAI();
	if( !P_VALID(pAI) ) return;

	DWORD dwMapID = pMap->GetMapID();
	DWORD dwInstanceID = pMap->GetInstanceID();

	g_ScriptMgr.CallScriptFunction(m_szFunc[ESCAE_On_Disappear], "uuuu", dwMapID, dwInstanceID, pCreature->GetID(), pAI->GetCurrentStateType());
}

//-------------------------------------------------------------------------------------------
// 玩家脚本
//-------------------------------------------------------------------------------------------
VOID RoleScript::OnRoleOnline(Role* pRole) const
{
	if( !P_VALID(m_szFunc[ESRE_On_Online]) ) return;

	g_ScriptMgr.CallScriptFunction(m_szFunc[ESRE_On_Online], "u", pRole->GetID());
}

VOID RoleScript::OnRoleFirstOnline(Role* pRole) const
{
	if( !P_VALID(m_szFunc[ESRE_On_FirstOnline]) ) return;

	g_ScriptMgr.CallScriptFunction(m_szFunc[ESRE_On_FirstOnline], "u", pRole->GetID());
}

VOID RoleScript::OnRoleIntoWorld(Role* pRole) const
{
	if( !P_VALID(m_szFunc[ESRE_On_IntoWorld])) return;

	Map* pMap = pRole->GetMap();
	if( !P_VALID(pMap) )	return;

	DWORD dwMapID = pMap->GetMapID();
	DWORD dwInstanceID = pMap->GetInstanceID();

	g_ScriptMgr.CallScriptFunction(m_szFunc[ESRE_On_IntoWorld], "uuu", dwMapID, dwInstanceID, pRole->GetID());
}

VOID RoleScript::OnRoleFirstIntoWorld(Role* pRole) const
{
	if( !P_VALID(m_szFunc[ESRE_On_FirstIntoWorld])) return;

	Map* pMap = pRole->GetMap();
	if( !P_VALID(pMap) )	return;

	DWORD dwMapID = pMap->GetMapID();
	DWORD dwInstanceID = pMap->GetInstanceID();

	g_ScriptMgr.CallScriptFunction(m_szFunc[ESRE_On_FirstIntoWorld], "uuu", dwMapID, dwInstanceID, pRole->GetID());
}

VOID RoleScript::OnRoleEnterMap(Role* pRole) const
{
	if( !P_VALID(m_szFunc[ESRE_On_EnterMap])) return;

	Map* pMap = pRole->GetMap();
	if( !P_VALID(pMap) )	return;

	DWORD dwMapID = pMap->GetMapID();
	DWORD dwInstanceID = pMap->GetInstanceID();

	g_ScriptMgr.CallScriptFunction(m_szFunc[ESRE_On_EnterMap], "uuu", dwMapID, dwInstanceID, pRole->GetID());
}

VOID RoleScript::OnRoleLevelChange(Role* pRole) const
{
	if( !P_VALID(m_szFunc[ESRE_On_LevelChange])) return;

	Map* pMap = pRole->GetMap();
	if( !P_VALID(pMap) )	return;

	DWORD dwMapID = pMap->GetMapID();
	DWORD dwInstanceID = pMap->GetInstanceID();

	g_ScriptMgr.CallScriptFunction(m_szFunc[ESRE_On_LevelChange], "uuu", dwMapID, dwInstanceID, pRole->GetID());
}

BOOL RoleScript::IsDeadPenalty(Role* pRole, EDeadPenaltyMode eMode) const
{
	if( !P_VALID(m_szFunc[ESRE_IsEquipLootPenalty + eMode]) ) return TRUE;
	BOOL  bRet = TRUE;

	Map* pMap = pRole->GetMap();
	if( !P_VALID(pMap))		return TRUE;

	DWORD dwMapID = pMap->GetMapID();
	DWORD dwInstanceID = pMap->GetInstanceID();

	g_ScriptMgr.CallScriptFunction(m_szFunc[ESRE_IsEquipLootPenalty + eMode], "uuu>b", dwMapID, dwInstanceID, pRole->GetID(), &bRet);

	return bRet;
}

VOID RoleScript::OnOpenChest(Role* pRole, DWORD dwChestID, DWORD dwKeyID) const
{
	if (!P_VALID(m_szFunc[ESRE_On_OpenChest]))	return;

	Map* pMap = pRole->GetMap();
	if (!P_VALID(pMap))	return;

	DWORD dwMapID = pMap->GetMapID();
	DWORD dwInstanceID = pMap->GetInstanceID();

	g_ScriptMgr.CallScriptFunction(m_szFunc[ESRE_On_OpenChest], "uuuuu", dwMapID, dwInstanceID, pRole->GetID(), dwChestID, dwKeyID);
}

VOID RoleScript::OnStopChest(Role* pRole, DWORD dwChestID, DWORD dwKeyID) const
{
	if (!P_VALID(m_szFunc[ESRE_On_StopChest]))	return;

	Map* pMap = pRole->GetMap();
	if (!P_VALID(pMap))	return;

	DWORD dwMapID = pMap->GetMapID();
	DWORD dwInstanceID = pMap->GetInstanceID();

	g_ScriptMgr.CallScriptFunction(m_szFunc[ESRE_On_StopChest], "uuuuu", dwMapID, dwInstanceID, pRole->GetID(), dwChestID, dwKeyID);
}

VOID RoleScript::OnAgainChest(Role *pRole) const
{
	if (!P_VALID(m_szFunc[ESRE_On_AgainChest]))	return;

	Map* pMap = pRole->GetMap();
	if (!P_VALID(pMap))	return;

	DWORD dwMapID = pMap->GetMapID();
	DWORD dwInstanceID = pMap->GetInstanceID();

	g_ScriptMgr.CallScriptFunction(m_szFunc[ESRE_On_AgainChest], "uuu", dwMapID, dwInstanceID, pRole->GetID());
}

VOID RoleScript::OnGetItem(Role* pRole, DWORD dwChestID, DWORD dwItemID, INT nNum) const
{
	if (!P_VALID(m_szFunc[ESRE_On_AgainChest]))	return;

	Map* pMap = pRole->GetMap();
	if (!P_VALID(pMap))	return;

	DWORD dwMapID = pMap->GetMapID();
	DWORD dwInstanceID = pMap->GetInstanceID();

	g_ScriptMgr.CallScriptFunction(m_szFunc[ESRE_On_GetItem], "uuuuui", dwMapID, dwInstanceID, pRole->GetID(), dwChestID, dwItemID, nNum);
}

VOID RoleScript::OnGetMarriage(Role* pRole, DWORD dwApplicantRoleID,DWORD dwTargetRoleID) const
{
	if (!P_VALID(m_szFunc[ESRE_On_GetMarriage]))	return;

	Map* pMap = pRole->GetMap();
	if (!P_VALID(pMap))	return;

	DWORD dwMapID = pMap->GetMapID();
	DWORD dwInstanceID = pMap->GetInstanceID();

	g_ScriptMgr.CallScriptFunction(m_szFunc[ESRE_On_GetMarriage], "uuuu", dwMapID, dwInstanceID, dwApplicantRoleID, dwTargetRoleID);
}
DWORD RoleScript::OnGetMarriageCheck(Role* pRole, DWORD dwApplicantRoleID,DWORD dwTargetRoleID) const
{
	if (!P_VALID(m_szFunc[ESRE_On_GetMarriageCheck]))	return E_SystemError;

	Map* pMap = pRole->GetMap();
	if (!P_VALID(pMap))	return E_SystemError;

	DWORD dwErrCode = E_Success;

	DWORD dwMapID = pMap->GetMapID();
	DWORD dwInstanceID = pMap->GetInstanceID();

	g_ScriptMgr.CallScriptFunction(m_szFunc[ESRE_On_GetMarriageCheck], "uuuu>u", dwMapID, dwInstanceID, dwApplicantRoleID, dwTargetRoleID,&dwErrCode);

	return dwErrCode;
}
VOID RoleScript::OnBreakMarriage(Role* pRole, DWORD dwApplicantRoleID,DWORD dwTargetRoleID) const
{
	if (!P_VALID(m_szFunc[ESRE_On_GetMarriage]))	return;

	Map* pMap = pRole->GetMap();
	if (!P_VALID(pMap))	return;

	DWORD dwMapID = pMap->GetMapID();
	DWORD dwInstanceID = pMap->GetInstanceID();

	g_ScriptMgr.CallScriptFunction(m_szFunc[ESRE_On_BreakMarriage], "uuuu", dwMapID, dwInstanceID, dwApplicantRoleID, dwTargetRoleID);
}
DWORD RoleScript::OnBreakMarriageCheck(Role* pRole, DWORD dwApplicantRoleID,DWORD dwTargetRoleID) const
{
	if (!P_VALID(m_szFunc[ESRE_On_BreakMarriageCheck]))	return E_SystemError;

	Map* pMap = pRole->GetMap();
	if (!P_VALID(pMap))	return E_SystemError;

	DWORD dwErrCode = E_Success;

	DWORD dwMapID = pMap->GetMapID();
	DWORD dwInstanceID = pMap->GetInstanceID();

	g_ScriptMgr.CallScriptFunction(m_szFunc[ESRE_On_BreakMarriageCheck], "uuuu>u", dwMapID, dwInstanceID, dwApplicantRoleID, dwTargetRoleID,&dwErrCode);

	return dwErrCode;
}

INT RoleScript::OnGetRoleFishingExp(Role* pRole) const
{
	if (!P_VALID(m_szFunc[ESRE_GetRoleFishingExp]))	return E_SystemError;

	Map* pMap = pRole->GetMap();
	if (!P_VALID(pMap))	return E_SystemError;

	DWORD dwMapID = pMap->GetMapID();
	DWORD dwInstanceID = pMap->GetInstanceID();

	INT exp = 0;

	g_ScriptMgr.CallScriptFunction(m_szFunc[ESRE_GetRoleFishingExp], "uuu>i", dwMapID, dwInstanceID, pRole->GetID(), &exp);

	return exp;
}

DWORD RoleScript::CanCancelPenalty(Role* pRole, Unit* pKiller, BOOL& bEquip, BOOL& bExp, BOOL& bInjury, BOOL& bHostility, BOOL& bCommerce) const
{
	if (!P_VALID(m_szFunc[ESRE_CanCancelPenalty]))	return E_SystemError;

	Map* pMap = pRole->GetMap();
	if (!P_VALID(pMap))	return E_SystemError;

	DWORD dwMapID = pMap->GetMapID();
	DWORD dwInstanceID = pMap->GetInstanceID();

	g_ScriptMgr.CallScriptFunction(m_szFunc[ESRE_CanCancelPenalty], "uuuu>bbbbb", dwMapID, dwInstanceID, pRole->GetID(), pKiller->GetID(), &bEquip, &bExp, &bInjury, &bHostility, &bCommerce);
	return E_Success;
}

VOID RoleScript::OnCompleteCommerce(Role* pRole,DWORD dwTaelProgress) const
{
	if (!P_VALID(m_szFunc[ESRE_On_CompleteCommerce]))	return;

	Map* pMap = pRole->GetMap();
	if (!P_VALID(pMap))	return;

	DWORD dwMapID = pMap->GetMapID();
	DWORD dwInstanceID = pMap->GetInstanceID();

	g_ScriptMgr.CallScriptFunction(m_szFunc[ESRE_On_CompleteCommerce], "uuuu", dwMapID, dwInstanceID, pRole->GetID(), dwTaelProgress);
}

//---------------------------------------------------------------------------------------------
// 地图脚本
//---------------------------------------------------------------------------------------------
VOID MapScript::OnInit(Map* pMap) const
{
	if( !P_VALID(m_szFunc[ESME_OnInit]) ) return;

	g_ScriptMgr.CallScriptFunction(m_szFunc[ESME_OnInit], "uu", pMap->GetMapID(), pMap->GetInstanceID());
}

VOID MapScript::OnTimer(Map* pMap, INT nMilliSec) const
{
	if( !P_VALID(m_szFunc[ESME_OnTimer]) ) return;

	g_ScriptMgr.CallScriptFunction(m_szFunc[ESME_OnTimer], "uui", pMap->GetMapID(), pMap->GetInstanceID(), nMilliSec);
}

VOID MapScript::CanTakeOverWhenOnline(Role* pRole, DWORD& dwOutMapID, Vector3& vOut) const
{
	if( !P_VALID(m_szFunc[ESME_CanEnterWhenOnline]) ) return;

	// 统一脚本坐标为格子坐标
	vOut.x = vOut.x / TILE_SCALE;
	vOut.z = vOut.z / TILE_SCALE;

	g_ScriptMgr.CallScriptFunction(m_szFunc[ESME_CanEnterWhenOnline], "uu>ufff", dwOutMapID,  pRole->GetID(), 
								&dwOutMapID, &vOut.x, &vOut.y, &vOut.z);
	// 统一脚本坐标为格子坐标
	vOut.x = vOut.x * TILE_SCALE;
	vOut.z = vOut.z * TILE_SCALE;
}

VOID MapScript::GetOnePerfectMap(Role* pRole, DWORD &dwInstanceID) const
{
	if( !P_VALID(m_szFunc[ESME_GetOnePerfectMap]) ) return;
	Map* pMap = pRole->GetMap();
	if( !P_VALID(pMap))   return;

	g_ScriptMgr.CallScriptFunction(m_szFunc[ESME_GetOnePerfectMap], "uuu>u", pMap->GetMapID(), pMap->GetInstanceID(), pRole->GetID(), &dwInstanceID);
}

INT MapScript::GetExportMapAndCoord(Role* pRole, DWORD& dwOutMapID, Vector3& vOut) const
{
	if( !P_VALID(m_szFunc[ESME_GetExportMapAndCoord]) ) return 0;
	Map* pMap = pRole->GetMap();
	if( !P_VALID(pMap))   return 0;

	INT nRet = 0;

	// 统一脚本坐标为格子坐标
	vOut.x = vOut.x / TILE_SCALE;
	vOut.z = vOut.z / TILE_SCALE;

	g_ScriptMgr.CallScriptFunction(m_szFunc[ESME_GetExportMapAndCoord], "uuu>iufff", pMap->GetMapID(), pMap->GetInstanceID(), pRole->GetID(), 
								&nRet, &dwOutMapID, &vOut.x, &vOut.y, &vOut.z);

	// 统一脚本坐标为格子坐标
	vOut.x = vOut.x * TILE_SCALE;
	vOut.z = vOut.z * TILE_SCALE;

	return nRet;
}

INT MapScript::CanEnter(Role* pRole) const
{
	if( !P_VALID(m_szFunc[ESME_CanEnter]) ) return 0;
	Map* pMap = pRole->GetMap();
	if( !P_VALID(pMap))  return 0;

	INT nRet = 0;

	g_ScriptMgr.CallScriptFunction(m_szFunc[ESME_CanEnter], "uuu>i", pMap->GetMapID(), pMap->GetInstanceID(), pRole->GetID(), &nRet);

	return nRet;
}

VOID MapScript::OnPlayerEnter(Role* pRole, Map* pMap) const
{
	if( !P_VALID(m_szFunc[ESME_OnPlayerEnter]) ) return;

	g_ScriptMgr.CallScriptFunction(m_szFunc[ESME_OnPlayerEnter], "uuu", pMap->GetMapID(), pMap->GetInstanceID(), pRole->GetID());
}

VOID MapScript::OnPlayerLeave(Role* pRole, Map* pMap) const
{
	if( !P_VALID(m_szFunc[ESME_OnPlayerLeave]) ) return;

	g_ScriptMgr.CallScriptFunction(m_szFunc[ESME_OnPlayerLeave], "uuu", pMap->GetMapID(), pMap->GetInstanceID(), pRole->GetID());
}

VOID MapScript::OnCreatureDie(Creature* pCreature, Unit* pKiller, Map* pMap) const
{
	if( !P_VALID(m_szFunc[ESME_OnCreatureDie]) ) return;
	if( !P_VALID(pCreature) ) return;

	DWORD dwKillerID = GT_INVALID;
	if( P_VALID(pKiller) ) dwKillerID = pKiller->GetID();

	g_ScriptMgr.CallScriptFunction(m_szFunc[ESME_OnCreatureDie], "uuuuu", pMap->GetMapID(), pMap->GetInstanceID(), pCreature->GetID(), pCreature->GetTypeID(), dwKillerID);
}

VOID MapScript::OnCreatureDisappear(Creature* pCreature, Map* pMap) const
{
	if( !P_VALID(m_szFunc[ESME_OnCreatureDisappear]) ) return;
	if( !P_VALID(pCreature) ) return;

	g_ScriptMgr.CallScriptFunction(m_szFunc[ESME_OnCreatureDisappear], "uuuu", pMap->GetMapID(), pMap->GetInstanceID(), pCreature->GetID(), pCreature->GetTypeID());
}

VOID MapScript::OnRoleDie(Role* pRole, Unit* pKiller, Map* pMap) const
{
	if( !P_VALID(m_szFunc[ESME_OnRoleDie]) ) return;
	if( !P_VALID(pRole) ) return;

	DWORD dwKillerID = GT_INVALID;
	if( P_VALID(pKiller) ) dwKillerID = pKiller->GetID();

	g_ScriptMgr.CallScriptFunction(m_szFunc[ESME_OnRoleDie], "uuuu", pMap->GetMapID(), pMap->GetInstanceID(), pRole->GetID(), dwKillerID);
}

BOOL MapScript::CanSetSafeguard(Map* pMap) const
{
	if( !P_VALID(m_szFunc[ESME_Safeguard]) ) return  TRUE;  
	BOOL bRet = TRUE;
                                                                                                                                  
	g_ScriptMgr.CallScriptFunction(m_szFunc[ESME_Safeguard],"uu>b",pMap->GetMapID(), pMap->GetInstanceID(), &bRet);

	return bRet;
}

BOOL MapScript::CanUseItem(Map* pMap, DWORD dwTypeID) const
{
	if( !P_VALID(m_szFunc[ESME_CanUseItem]) ) return  TRUE;  
	BOOL bRet = TRUE;

	g_ScriptMgr.CallScriptFunction(m_szFunc[ESME_CanUseItem],"uuu>b",pMap->GetMapID(), pMap->GetInstanceID(), dwTypeID, &bRet);

	return bRet;
}

BOOL MapScript::CanUseSkill(Map* pMap, DWORD dwTypeID) const
{
	if( !P_VALID(m_szFunc[ESME_CanUseSkill]) ) return  TRUE;
	BOOL bRet = TRUE;

	g_ScriptMgr.CallScriptFunction(m_szFunc[ESME_CanUseSkill], "uuu>b", pMap->GetMapID(), pMap->GetInstanceID(), dwTypeID, &bRet);

	return bRet;
}

VOID MapScript::Revive(Role* pRole, ERoleReviveType eType, INT &nReviveHP, INT &nReviveMP, FLOAT &fx, FLOAT &fy, FLOAT &fz, DWORD &dwRebornMapID) const 
{
	if( !P_VALID(m_szFunc[ESME_On_Revive]) )  return;
	Map* pMap = pRole->GetMap();
	if( !P_VALID(pMap))   return;

	DWORD dwMapID = pMap->GetMapID();
	DWORD dwInstanceID = pMap->GetInstanceID();

	// 统一脚本坐标为格子坐标
	fx = fx / TILE_SCALE;
	fz = fz / TILE_SCALE;

	g_ScriptMgr.CallScriptFunction(m_szFunc[ESME_On_Revive], "uuuiiifffu>iifffu", dwMapID, dwInstanceID, pRole->GetID(), eType, 
						nReviveHP, nReviveMP, fx, fy, fz, dwRebornMapID, &nReviveHP, &nReviveMP, &fx, &fy, &fz, &dwRebornMapID);


	fx = fx * TILE_SCALE;
	fz = fz * TILE_SCALE;
}

VOID MapScript::OnEnterTrigger(Role* pRole, tagMapTriggerInfo* pTrigger, Map* pMap) const
{
	if( !P_VALID(m_szFunc[ESME_OnEnterTrigger]) ) return;
	if( !P_VALID(pRole) || !P_VALID(pTrigger) ) return;

	g_ScriptMgr.CallScriptFunction(m_szFunc[ESME_OnEnterTrigger], "uuuu", pMap->GetMapID(), pMap->GetInstanceID(), pRole->GetID(), pTrigger->dwObjID);
}

VOID MapScript::OnEnterArea(Role* pRole, tagMapAreaInfo* pArea, Map* pMap) const
{
	if( !P_VALID(m_szFunc[ESME_OnEnterArea]) ) return;
	if( !P_VALID(pRole) || !P_VALID(pArea) ) return;

	g_ScriptMgr.CallScriptFunction(m_szFunc[ESME_OnEnterArea], "uuuu", pMap->GetMapID(), pMap->GetInstanceID(), pRole->GetID(), pArea->dwObjID);
}

INT  MapScript::CanInviteJoinTeam(Map* pMap) const
{
	if( !P_VALID(m_szFunc[ESME_CanInviteJoinTeam]) ) return 0;
	INT	 nRet = E_Success;

	g_ScriptMgr.CallScriptFunction(m_szFunc[ESME_CanInviteJoinTeam], "uu>i", pMap->GetMapID(), pMap->GetInstanceID(), &nRet);

	return nRet;
}

INT MapScript::CanLeaveTeam(Map* pMap) const
{
	if( !P_VALID(m_szFunc[ESME_CanLeaveTeam]) ) return 0;
	INT  nRet = E_Success;

	g_ScriptMgr.CallScriptFunction(m_szFunc[ESME_CanLeaveTeam], "uu>i", pMap->GetMapID(), pMap->GetInstanceID(), &nRet);

	return nRet;
}

INT MapScript::CanChangeLeader(Map* pMap) const
{
	if( !P_VALID(m_szFunc[ESME_CanChangeLeader]) ) return 0;
	INT  nRet = E_Success;

	g_ScriptMgr.CallScriptFunction(m_szFunc[ESME_CanChangeLeader], "uu>i", pMap->GetMapID(), pMap->GetInstanceID(), &nRet);

	return nRet;
}

INT MapScript::CanKickMember(Map* pMap) const
{
	if ( !P_VALID(m_szFunc[ESME_CanKickMember]) ) return 0;
	INT nRet = E_Success;

	g_ScriptMgr.CallScriptFunction(m_szFunc[ESME_CanKickMember], "uu>i", pMap->GetMapID(), pMap->GetInstanceID(), &nRet);

	return nRet;
}

DWORD MapScript::FriendEnemy(Map* pMap, Unit* pSrc, Unit* pTarget, BOOL& bIgnore) const
{
	if( !P_VALID(m_szFunc[ESME_FriendEnemy]) ) return 0;

	BOOL bFriend		=	FALSE;
	BOOL bHostile		=	FALSE;
	BOOL bIndependent	=	FALSE;

	g_ScriptMgr.CallScriptFunction(m_szFunc[ESME_FriendEnemy], "uuuu>bbbb", pMap->GetMapID(), pMap->GetInstanceID(), pSrc->GetID(), pTarget->GetID(),
									&bFriend, &bHostile, &bIndependent, &bIgnore);

	DWORD dwFlag = 0;
	if( bFriend )		dwFlag	|=	ETFE_Friendly;
	if( bHostile )		dwFlag	|=	ETFE_Hostile;
	if( bIndependent )	dwFlag	|=	ETFE_Independent;

	return dwFlag;
}

//---------------------------------------------------------------------------------------------
// 活动脚本
//---------------------------------------------------------------------------------------------
VOID ActScript::OnInit(DWORD dwActID) const
{
	if(!P_VALID(m_szFunc[ESAE_OnInit]))	return;

	g_ScriptMgr.CallScriptFunction(m_szFunc[ESAE_OnInit], "u", dwActID);
}

VOID ActScript::OnTimer(DWORD dwActID, INT nSec) const
{
	if( !P_VALID(m_szFunc[ESAE_OnTimer]) ) return;

	g_ScriptMgr.CallScriptFunction(m_szFunc[ESAE_OnTimer], "ui", dwActID, nSec);
}

VOID ActScript::OnTimerMin(DWORD dwActID) const
{
	if( !P_VALID(m_szFunc[ESAE_OnTimerMin]) ) return;

	g_ScriptMgr.CallScriptFunction(m_szFunc[ESAE_OnTimerMin], "u", dwActID);
}

VOID ActScript::OnActStart(DWORD dwActID) const
{
	if( !P_VALID(m_szFunc[ESAE_OnStart]) ) return;

	g_ScriptMgr.CallScriptFunction(m_szFunc[ESAE_OnStart], "u", dwActID);
}

VOID ActScript::OnActEnd(DWORD dwActID) const
{
	if( !P_VALID(m_szFunc[ESAE_OnEnd]) ) return;

	g_ScriptMgr.CallScriptFunction(m_szFunc[ESAE_OnEnd], "u", dwActID);
}

VOID ActScript::OnDefaultRequest(DWORD dwActID, Role* pRole, DWORD	dwEventType) const
{
	if( !P_VALID(m_szFunc[ESAE_OnDefaultRequest]) ) return;

	g_ScriptMgr.CallScriptFunction(m_szFunc[ESAE_OnDefaultRequest], "uuu", dwActID, pRole->GetID(), dwEventType);
}

VOID ActScript::BroadActivityState(EActBroadMode eState) const
{
	if( !P_VALID(m_szFunc[ESAE_Broad]) ) return;

	g_ScriptMgr.CallScriptFunction(m_szFunc[ESAE_Broad], "i", eState);
}

//---------------------------------------------------------------------------------------------
// 游戏世界事件脚本
//---------------------------------------------------------------------------------------------
VOID WorldScript::OnAdventure(Role *pRole) const
{
	if( !P_VALID(m_szFunc[ESWE_Adventure]) ) return;

	g_ScriptMgr.CallScriptFunction(m_szFunc[ESWE_Adventure], "u", pRole->GetID());
}

//---------------------------------------------------------------------------------------------
// 物品事件脚本
//---------------------------------------------------------------------------------------------
INT ItemScript::CanUseItem(Map* pMap, DWORD dwTypeID, DWORD dwTargetID, BOOL &bIgnore) const
{
	if( !P_VALID(m_szFunc[ESIE_CanUse]) ) return 0;
	BOOL bUseable = FALSE;
	INT	 nRet = E_Success;

	g_ScriptMgr.CallScriptFunction(m_szFunc[ESIE_CanUse], "uuuu>ii", pMap->GetMapID(), pMap->GetInstanceID(), dwTypeID, dwTargetID, &nRet, &bIgnore);

	return nRet;
}

VOID ItemScript::UseItem(Map* pMap, DWORD dwTypeID, DWORD dwTargetID) const
{
	if( !P_VALID(m_szFunc[ESIE_Use]) ) return;

	g_ScriptMgr.CallScriptFunction(m_szFunc[ESIE_Use], "uuuu", pMap->GetMapID(), pMap->GetInstanceID(), dwTypeID, dwTargetID);
}

//-------------------------------------------------------------------------------------
// 能否使用技能
//-------------------------------------------------------------------------------------
DWORD SkillScript::CanCastSkill( Map* pMap, DWORD dwSkillID, DWORD dwOwnerID, DWORD dwDstUnitID ) const
{
	if( !P_VALID(m_szFunc[ESSE_CanCast]) ) return 0;
	BOOL bRet = FALSE;

	g_ScriptMgr.CallScriptFunction(m_szFunc[ESSE_CanCast], "uuuuu>i", pMap->GetMapID(), pMap->GetInstanceID(), dwSkillID, dwOwnerID, dwDstUnitID, &bRet);

	return bRet;
}

//-------------------------------------------------------------------------------------
// 使用技能
//-------------------------------------------------------------------------------------
VOID SkillScript::CastSkill(Map* pMap, DWORD dwSkillID, DWORD dwOwnerID, BOOL &bIgnore) const
{
	if( !P_VALID(m_szFunc[ESSE_Cast]) ) return;

	g_ScriptMgr.CallScriptFunction(m_szFunc[ESSE_Cast], "uuuu>i", pMap->GetMapID(), pMap->GetInstanceID(), dwSkillID, dwOwnerID, &bIgnore);
}


