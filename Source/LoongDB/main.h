//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: main.h
// author: Sxg
// actor:
// data: 2008-05-12
// last:
// brief: 控制台程序入口点
//-----------------------------------------------------------------------------
#pragma once

class CLoong;

//-----------------------------------------------------------------------------
class CMain
{
public:
	BOOL Init(HINSTANCE hInst);
	VOID Mainloop();
	VOID Destroy();

private:
	TSFPTrunk<CMain>		m_Trunk;
	TObjRef<GDIRender>		m_pRender;
	TObjRef<Util>			m_pUtil;
	TObjRef<WindowWrap>		m_pWindow;
	TObjRef<GUISystem>		m_pGUI;
	IGUIRenderNative		m_Render;

	TObjRef<Console>		m_pConsole;

	CLoong*					m_pLoong;

	// 刷新UI信息
	BOOL RefreshUI();
};