//-----------------------------------------------------------------------------
//!\file vuieditor.h
//!\author Lyp
//!
//!\date 2004-08-11
//! last 2008-01-23
//!
//!\brief vengine gui editor
//-----------------------------------------------------------------------------
#pragma once


//-----------------------------------------------------------------------------
// test
//-----------------------------------------------------------------------------
class CVUIEditor
{
public:
	BOOL Init(HINSTANCE hInst);
	VOID Mainloop();
	VOID Destroy();

private:
	D3DRender*			m_pRender;
	Util*				m_pUtil;
	WindowWrap*			m_pWindow;
	GUISystem*			m_pGUI;
	Console*			m_pConsole;
	FastCode*			m_pFastCode;

	IGUIRenderNative			m_render;
	
	TObjRef<GUIEditor>			m_pEditor;
};

