/********************************************************************
	created:	2008/04/29
	created:	29:4:2008   11:48
	filename: 	d:\loong_client\Client\GameScriptMgr.cpp
	file path:	d:\loong_client\Client
	file base:	GameScriptMgr
	file ext:	cpp
	author:		leili
	
	purpose:	游戏脚本对象管理器实现
*********************************************************************/
#include "StdAfx.h"
#include "GameScriptMgr.h"

//-----------------------------------------------------------------------------
// CONSTRUCTOR
//-----------------------------------------------------------------------------
extern VOID LuaOpenCommonLib(lua_State*  l);
extern VOID LuaOpenToolTipLib(lua_State* l);
GameScriptMgr::GameScriptMgr(void)
: m_pData(NULL)
, m_dataSize(0)
, m_parsePos(0)
, m_pParseScript(0)
{
	m_masterState = lua_open();
	if( m_masterState )
	{
		// open any auxiliary libraries we may need
		luaopen_base(m_masterState);
		luaopen_string(m_masterState);
		luaopen_math(m_masterState);
		luaopen_table(m_masterState);
		//luaopen_debug(m_masterState);

		LuaOpenCommonLib(m_masterState);
		LuaOpenToolTipLib(m_masterState);
	}
}

//-----------------------------------------------------------------------------
// DESTRUCTOR
//-----------------------------------------------------------------------------
GameScriptMgr::~GameScriptMgr(void)
{
	GameScript*  pScrip = NULL;
	m_mapScript.ResetIterator();
	while( m_mapScript.PeekNext(pScrip) )
		SAFE_DEL(pScrip);

	m_mapScript.Clear();
	lua_close(m_masterState);

	SAFE_DEL_ARRAY( m_pData );

	IDRelevantMsgMap::iterator iter = m_mapIDRelevantMsg.begin();
	IDRelevantMsgMap::iterator end = m_mapIDRelevantMsg.end();
	for (; iter != end; ++iter)
		SAFE_DELETE(iter->second);
	m_mapIDRelevantMsg.clear();

	iter = m_mapQuestDesc.begin();
	end = m_mapQuestDesc.end();
	for (; iter != end; ++iter)
		SAFE_DELETE(iter->second);
	m_mapQuestDesc.clear();
}

//-----------------------------------------------------------------------------
// init
//-----------------------------------------------------------------------------
BOOL GameScriptMgr::Init( void )
{
	// 加载基础定义lua
	TCHAR szTmp[255];
	_stprintf(szTmp, _T("%s\\script\\BaseDefine.lua"), vEngine::g_strLocalPath.c_str());
	this->CreateScript( "VFS_System", szTmp );

	// 加载解析lua
	_stprintf(szTmp, _T("%s\\script\\MsgParse.lua"), vEngine::g_strLocalPath.c_str());
	m_pParseScript = this->CreateScript( "VFS_System", szTmp );

	LoadIDRelevantMsg();
	LoadQuestDesc();

	return TRUE;
}

//-----------------------------------------------------------------------------
// 更新
//-----------------------------------------------------------------------------
VOID GameScriptMgr::Update( float fElapsedSec )
{
	GameScript* pScrip = NULL;
	m_mapScript.ResetIterator();
	while( m_mapScript.PeekNext(pScrip) )
		pScrip->Update(fElapsedSec);
}

//-----------------------------------------------------------------------------
// 创建一个脚本对象
//-----------------------------------------------------------------------------
GameScript* GameScriptMgr::CreateScript( LPCSTR szVFS, LPCTSTR szFileName, BOOL reCreated )
{
	if( !P_VALID(szFileName) || szFileName[0] == 0 )
		return NULL;

	DWORD dwID = m_pUtil->Crc32(szFileName);
	if( m_mapScript.IsExist(dwID) )
	{
		GameScript *pScript = (GameScript*)m_mapScript.Peek(dwID);	// 返回已存在的脚本

		if( reCreated )
		{
			SAFE_DEL(pScript);
			m_mapScript.Erase(dwID);
		}
		else
			return pScript;
	}

	// create a script object
	GameScript* pScript = new GameScript();

	if( pScript->LoadFile(szVFS, szFileName) )
	{
		m_mapScript.Add(dwID, pScript);
		return pScript;
	}

	SAFE_DEL(pScript);
	return NULL;
}

//-----------------------------------------------------------------------------
// 删除一个脚本对象
//-----------------------------------------------------------------------------
BOOL GameScriptMgr::DestroyScript( LPCTSTR szFileName )
{
	DWORD dwID = m_pUtil->Crc32(szFileName);
	GameScript* pDelScript = m_mapScript.Peek(dwID);

	if( !P_VALID(pDelScript) )
		return FALSE;

	m_mapScript.Erase(dwID);

	SAFE_DEL(pDelScript);
	return TRUE;
}

//-----------------------------------------------------------------------------
// 注册函数
//-----------------------------------------------------------------------------
void GameScriptMgr::RegFunc( LPCSTR szFunName,lua_CFunction pFunc )
{
	ASSERT(m_masterState!=NULL);
	lua_pushcfunction(m_masterState,pFunc);
	lua_setglobal(m_masterState,szFunName);
}

VOID GameScriptMgr::SetData( LPVOID pData, DWORD size )
{
	SAFE_DEL_ARRAY(m_pData);

	m_dataSize = size;
	m_parsePos = 0;
	m_pData = new BYTE[size];
	memcpy( m_pData, pData, size );
}

bool GameScriptMgr::LoadIDRelevantMsg( void )
{
	list<tstring> listName;
	tstring strPath = _T("data\\system\\attdata\\id_msg.xml");
	TObjRef<VarContainer> con = CreateObj("idmsg", "VarContainer");
	if (!con->Load("VFS_System", strPath.c_str(), "id", &listName))
	{
		IMSG( _T("load file:%s failed\r\n"), strPath.c_str() );
		return false;
	}

	list<tstring>::iterator iend = listName.end();
	
	for (list<tstring>::iterator ibegin = listName.begin(); ibegin != iend; ++ibegin)
	{
		int id = con->GetInt(_T("id"), ibegin->c_str());
		tstring *pName = new tstring(con->GetString(_T("name"), ibegin->c_str()));
		m_mapIDRelevantMsg.insert(make_pair(id, pName));
	}
	KillObj("idmsg");

	return true;
}

bool GameScriptMgr::LoadQuestDesc( void )
{
	tstring strPath = g_strLocalPath + _T("\\quest\\quest_string.xml");

	list<tstring> descList;
	TObjRef<VarContainer> con = CreateObj( "QuestDesc", "VarContainer" );
	if(!con->Load("VFS_System", strPath.c_str(), "name", &descList))
	{
		IMSG(_T("Load file quest_string.xml failed\r\n"));
		return FALSE;
	}
	list<tstring>::iterator end = descList.end();
	for (list<tstring>::iterator iter = descList.begin(); iter != end; ++iter)
	{
		DWORD dwID = con->GetDword(_T("name"), iter->c_str(), 0);
		tstring *pDesc = new tstring(con->GetString(_T("value"), iter->c_str(), _T("")));
		m_mapQuestDesc.insert(std::make_pair(dwID, pDesc));
	}
	KillObj("QuestDesc");

	return TRUE;
}

LPCTSTR GameScriptMgr::GetMsg( DWORD msgID )
{
	IDRelevantMsgMap::iterator finder = m_mapIDRelevantMsg.find(msgID);
	if( finder != m_mapIDRelevantMsg.end() )
	{
		return finder->second->c_str();
	}

	return NULL;
}

LPCTSTR GameScriptMgr::GetQuestDesc( DWORD msgID )
{
	IDRelevantMsgMap::iterator finder = m_mapQuestDesc.find(msgID);
	if( finder != m_mapQuestDesc.end() )
	{
		return finder->second->c_str();
	}

	return NULL;
}