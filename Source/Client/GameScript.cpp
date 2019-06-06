/********************************************************************
	created:	2008/04/29
	created:	29:4:2008   14:10
	filename: 	d:\loong_client\Client\GameScript.cpp
	file path:	d:\loong_client\Client
	file base:	GameScript
	file ext:	cpp
	author:		leili
	
	purpose:	脚本对象实现
*********************************************************************/
#include "StdAfx.h"
#include "GameScript.h"

//-----------------------------------------------------------------------------
// 构造函数
//-----------------------------------------------------------------------------
GameScript::GameScript( void )
: m_eState(EGSS_NotLoad)
, m_threadState(0)
{
}

//-----------------------------------------------------------------------------
// 解析函数
//-----------------------------------------------------------------------------
GameScript::~GameScript( void )
{
	Clear();
}

//-----------------------------------------------------------------------------
// 更新
//-----------------------------------------------------------------------------
VOID GameScript::Update( float fElapsedSec )
{
	m_fWaitTime -= fElapsedSec;
	switch( m_eState )
	{
	case EGSS_WaitTime:
		if( m_fWaitTime <= 0.0f )
			Resume(0.0f);
		break;
	default:
		break;
	}
}

//-----------------------------------------------------------------------------
// 加载文件并且运行
//-----------------------------------------------------------------------------
BOOL GameScript::LoadFile( LPCSTR szVFS, LPCTSTR szFileName )
{
	TObjRef<VirtualFileSys> pVFS;
	if( P_VALID(szVFS) )
		pVFS = szVFS;

	//--读取文件
	DWORD dwSize = pVFS->GetSize(szFileName);
	if( !P_VALID(dwSize) || dwSize < 3 )	// 文件长度必须大于等于3个字节
	{
		//ERR(_T("Script file not find : %s\r\n"), szFileName);
		return FALSE;
	}

	CHAR* szScript = (CHAR*)malloc(dwSize+2);
	if( !szScript )
		return FALSE;

	pVFS->LoadFile(szScript, szFileName);
	szScript[dwSize] = szScript[dwSize+1] = 0;	// 加上UNICODE 0结束符

	INT nOffset = 0;	// LUA脚本实际起始偏移量
	if( (szScript[0] == -17) && (szScript[1] == -69) && (szScript[2] == -65) )	// UTF-8 BOM
		nOffset = 3;

	std::string strRealCode;
	strRealCode.assign(szScript+nOffset);

	// 读取并编译
	this->CreateThread(szFileName);	// 重新创建线程虚拟机
	INT nResult = luaL_loadbuffer(m_threadState, strRealCode.c_str(), strRealCode.size(), 
		m_pUtil->UnicodeToUnicode8(m_strName.c_str()));
	if( nResult )
	{
		FormatError();
		OutputError(_T("Script error"));
		return FALSE;
	}

	this->SetState(EGSS_Loaded);
	this->Resume(0.0f);

	//--释放临时内存
	free(szScript);
	return TRUE;
}

//-----------------------------------------------------------------------------
// ResumeScript
// Runs or resumes a LUA script.  Executes until a yield is encountered or
// the script terminates.
// The parameter to this function is pushed on the stack as the return value
// of the previous yield.
//-----------------------------------------------------------------------------
VOID GameScript::Resume( float fParam )
{
	// param is treated as a return value from the function that yielded
	lua_pushnumber(m_threadState, fParam);

	INT nStatus = lua_resume(m_threadState, 1);
	if( nStatus != 0 && nStatus != LUA_YIELD )
	{
		FormatError();
		OutputError(_T("Runtime Error"));
	}

	if( m_eState != EGSS_WaitTime && m_eState != EGSS_Break)
		this->SetState(EGSS_Done);
}

//-----------------------------------------------------------------------------
// 执行一个字符串
//-----------------------------------------------------------------------------
BOOL GameScript::RunString( LPCSTR szCommandString )
{
	INT nStatus = luaL_loadbuffer(m_threadState, szCommandString, strlen(szCommandString), "Immediate");
	if( nStatus )
	{
		FormatError();
		OutputError(_T("Immediate script error"));
		return FALSE;
	}
	this->SetState(EGSS_Running);
	Resume(0.0f);

	return TRUE;
}

//-----------------------------------------------------------------------------
// FormatError
// Grabs the last error message and stuffs it in lastErrorString.
//-----------------------------------------------------------------------------
VOID GameScript::FormatError()
{
	const char* msg = lua_tostring(m_threadState, -1);
	if( !msg )
		msg = "(error with no message)";
	lua_pop(m_threadState, 1);
	strcpy(m_lastErrorString, msg);
}

//-----------------------------------------------------------------------------
// OUTPUT ERROR
//-----------------------------------------------------------------------------
VOID GameScript::OutputError( LPCTSTR strType )
{
	IMSG(_T("%s:%s\r\n"), strType, m_pUtil->Unicode8ToUnicode(m_lastErrorString));
}

//-----------------------------------------------------------------------------
// 创建线程虚拟机
//-----------------------------------------------------------------------------
VOID GameScript::CreateThread( tstring strName )
{
	Clear();

	m_strName = strName;

	// create a thread/state for this object
	m_threadState = lua_newthread(m_pManager->m_masterState);
	// 把线程记录到lua注册表,1防止lua_newthread堆栈占用 2防止被垃圾收集掉
	lua_setfield(m_pManager->m_masterState, LUA_REGISTRYINDEX, m_pUtil->UnicodeToUnicode8(m_strName.c_str()));

	// save a pointer to the thread manager object in the global table
	// using the new thread's vm pointer as a key
	lua_pushlightuserdata(m_pManager->m_masterState, m_threadState);
	lua_pushlightuserdata(m_pManager->m_masterState, this );
	lua_settable(m_pManager->m_masterState, LUA_GLOBALSINDEX );
}

//-----------------------------------------------------------------------------
// 清空原来的环境
//-----------------------------------------------------------------------------
VOID GameScript::Clear()
{
	if( m_threadState != NULL )
	{
		lua_pushlightuserdata(m_pManager->m_masterState, m_threadState);
		lua_pushnil(m_pManager->m_masterState);
		lua_settable(m_pManager->m_masterState, LUA_GLOBALSINDEX );
	}

	if( !m_strName.empty() )
	{
		lua_pushnil(m_pManager->m_masterState);
		lua_setfield(m_pManager->m_masterState, LUA_REGISTRYINDEX, m_pUtil->UnicodeToUnicode8(m_strName.c_str()));
	}
}

LPCSTR GameScript::GetObjString(void* obj, void* proto)
{
	lua_getglobal(m_threadState, "GetObjString");
	lua_pushlightuserdata(m_threadState, obj);
	lua_pushlightuserdata(m_threadState, proto);

	INT nStatus = lua_pcall(m_threadState, 2, 1, 0);
	if( nStatus )
	{
		FormatError();
		OutputError(_T("Immediate script error"));
		return "";
	}

	nStatus = lua_isstring(m_threadState, -1);
	if( 0 == nStatus )
	{
		FormatError();
		OutputError(_T("Immediate script return error"));
		return "";
	}

	LPCSTR szTmp = lua_tostring(m_threadState, -1);
	lua_pop(m_threadState, 1);

	this->SetState(EGSS_Running);
	Resume(0.0f);
	return szTmp;
}

LPCSTR GameScript::GetObjString( void* proto, DOUBLE quality, DOUBLE nameid )
{
	lua_getglobal(m_threadState, "GetObjString");
	lua_pushlightuserdata(m_threadState, proto);
	lua_pushnumber(m_threadState, quality);
	lua_pushnumber(m_threadState, nameid);

	INT nStatus = lua_pcall(m_threadState, 3, 1, 0);
	if( nStatus )
	{
		FormatError();
		OutputError(_T("Immediate script error"));
		return "";
	}

	nStatus = lua_isstring(m_threadState, -1);
	if( 0 == nStatus )
	{
		FormatError();
		OutputError(_T("Immediate script return error"));
		return "";
	}

	LPCSTR szTmp = lua_tostring(m_threadState, -1);
	lua_pop(m_threadState, 1);

	this->SetState(EGSS_Running);
	Resume(0.0f);
	return szTmp;
}