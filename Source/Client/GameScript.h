/********************************************************************
	created:	2008/04/29
	created:	29:4:2008   14:10
	filename: 	d:\loong_client\Client\GameScript.h
	file path:	d:\loong_client\Client
	file base:	GameScript
	file ext:	h
	author:		leili
	
	purpose:	脚本对象声明
*********************************************************************/
#pragma once

class GameScriptMgr;

enum EGameScriptState
{
	EGSS_Invalid		= -1,
	EGSS_NotLoad		= 0,
	EGSS_Loaded,
	EGSS_Running,
	EGSS_WaitTime,
	EGSS_Break,
	EGSS_Done
};

class GameScript
{
public:
	GameScript(void);
	virtual ~GameScript(void);

	VOID			Update(float fElapsedSec);
	VOID			Resume(float fParam);
	BOOL			LoadFile(LPCSTR szVFS, LPCTSTR szFileName);
	BOOL			RunString(LPCSTR szCommandString);

	lua_State*		GetVM()								{ return m_threadState; }
	const tstring&	GetName()	const					{ return m_strName; }
	VOID			SetWaitTime(FLOAT fTime)			{ m_fWaitTime = fTime; }
	VOID			SetState(EGameScriptState eState)	{ m_eState = eState; }
	EGameScriptState GetState()	const					{ return m_eState; }

	LPCSTR			GetObjString(void* obj, void* proto);
	LPCSTR			GetObjString(void* proto, DOUBLE quality, DOUBLE nameid);

private:
	VOID			CreateThread(tstring strName);
	VOID			FormatError();
	VOID			OutputError(LPCTSTR strType);
	VOID			Clear();

private:
	TObjRef<Util>			m_pUtil;
	TObjRef<GameScriptMgr>	m_pManager;

	lua_State				*m_threadState;
	char					m_lastErrorString[256];
	tstring					m_strName;

	EGameScriptState		m_eState;		// are we waiting, not loaded yet, etc.
	FLOAT					m_fWaitTime;	// time to wake up
};
