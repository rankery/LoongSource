//-----------------------------------------------------------------------------
//!\file vEngineTest.h
//!\author Lyp
//!
//!\date 2004-08-11
//! last 2007-10-11
//!
//!\brief vengine≤‚ ‘≥Ã–Ú
//-----------------------------------------------------------------------------
#pragma once


//-----------------------------------------------------------------------------
// test
//-----------------------------------------------------------------------------
class CTest
{
public:
	BOOL Init(HINSTANCE hInst);
	VOID Mainloop(LPCTSTR szText);
	VOID Destroy();

	CTest():m_Trunk(this){}

private:
	TSFPTrunk<CTest>		m_Trunk;
	TObjRef<GDIRender>		m_pRender;
	TObjRef<InputDX8>		m_pInput;
	TObjRef<InputMap>		m_pInputMap;
	TObjRef<Util>			m_pUtil;
	TObjRef<WindowWrap>		m_pWindow;
	TObjRef<LuaManager>		m_pLuaMgr;
	TObjRef<GUISystem>		m_pGUI;
	TObjRef<Console>		m_pConsole;
	TObjRef<Log>			m_pLog;
	IGUIRenderNative		m_render;


	TObjRef<VarContainer>	m_pVarContainer;

	// ≤‚ ‘≥Ã–Ú
	DWORD Test1(LPCTSTR szParam1);
	DWORD Test2(LPCTSTR szParam1);
	DWORD Test3(LPCTSTR szParam1);
	DWORD Test3Thread(LPCTSTR szParam1);
};

