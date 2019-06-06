//-----------------------------------------------------------------------------
//!\file gui_frame.cpp
//!\author Lyp
//!
//!\date 2004-12-08
//! last 2008-03-31
//!
//!\brief 界面系统 frame 控件
//-----------------------------------------------------------------------------
#include "..\stdafx.h"
#include "gui_static.h"
#include "gui_frame.h"


namespace vEngine {
//-----------------------------------------------------------------------------
// constructor / destructor
//-----------------------------------------------------------------------------
GUIFrame::GUIFrame()
{
	// 注册所有可编辑属性
	m_pVarMgr->Register(_T("TopColor"), &m_dwTopColor);
	m_pVarMgr->Register(_T("BottomColor"), &m_dwBottomColor);

	m_dwTopColor = 0xff000000;
	m_dwBottomColor = 0xFFAFAFAF;

}

//-----------------------------------------------------------------------------
// 渲染
//-----------------------------------------------------------------------------
VOID GUIFrame::Render()
{
	if( !IsRefresh() )
		return GUIWnd::Render();

	/*
	// 渲染底图
	m_pCanvas->rc.SetLTWH(m_ptView, m_ptSize);
	m_pRender->Draw(m_pCanvas, m_pPic, m_dwPicColor);
	*/

	// 渲染文字
	tstring str = m_strText;
	tstring sub(_T("\\n"));
	while(tstring::npos != str.find(sub))
		str.replace(str.find(sub), sub.size(), _T("\n"));

	m_pCanvas->rc = m_rcText + m_ptView;
	m_pRender->Text(m_pCanvas, str, m_pFont, m_dwTextColor, m_dwShadeColor, m_eTextAlign, m_bTextWriteAlpha);


	// 画线
	m_pCanvas->rc.SetLTWH(m_ptView, m_ptSize);
	m_pCanvas->rc.bottom = m_pCanvas->rc.top;
	m_pRender->Line(m_pCanvas, m_dwTopColor);

	m_pCanvas->rc.SetLTWH(m_ptView, m_ptSize);
	m_pCanvas->rc.right = m_pCanvas->rc.left;
	m_pRender->Line(m_pCanvas, m_dwTopColor);

	m_pCanvas->rc.SetLTWH(m_ptView, m_ptSize);
	m_pCanvas->rc.right--;
	m_pCanvas->rc.left = m_pCanvas->rc.right ;
	m_pRender->Line(m_pCanvas, m_dwBottomColor);

	m_pCanvas->rc.SetLTWH(m_ptView, m_ptSize);
	m_pCanvas->rc.bottom--;
	m_pCanvas->rc.top = m_pCanvas->rc.bottom;
	m_pRender->Line(m_pCanvas, m_dwBottomColor);

	// 调用基类渲染
	GUIWnd::Render();
}

}	// namespace vEngine {
