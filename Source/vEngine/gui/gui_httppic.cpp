//-----------------------------------------------------------------------------
//!\file gui_httppic.cpp
//!\author Lyp
//!
//!\date 2004-12-08
//! last 2008-04-24
//!
//!\brief 界面系统http picture控件
//-----------------------------------------------------------------------------
#include "..\stdafx.h"
#include "gui_static.h"
#include "gui_httppic.h"

#include "gui_event_define.h"
#include "..\network\HttpDownload.h"

namespace vEngine {
//-----------------------------------------------------------------------------
// constructor / destructor
//-----------------------------------------------------------------------------
GUIHttpPic::GUIHttpPic()
{
	m_pHttp = new HttpDownload;
	m_pPic = NULL;
}


GUIHttpPic::~GUIHttpPic()
{
	SAFE_DEL(m_pHttp);
}


//-----------------------------------------------------------------------------
// init
//-----------------------------------------------------------------------------
BOOL GUIHttpPic::Init(GUISystem* pSystem, GUIWnd* pFather, XmlElement* pXmlElement)
{
	GUIWnd::Init(pSystem, pFather, pXmlElement);

	if( m_rcText.IsEmpty() )
		m_rcText = m_ptSize;

	// 创建字体
	m_pFont = m_pRender->CreateFont(m_strFont, m_nFontWidth, m_nFontHeight,
		m_nFontWeight, !m_bNoAntiAliase);
	if( !P_VALID(m_pFont) )	// 尝试使用父控件字体
		m_pFont = m_pRender->CloneFont(m_pFather->GetFont());

	// 读入图片
	m_pHttp->Download(m_strPic.c_str(), FALSE);
	return TRUE;
}


//-----------------------------------------------------------------------------
// destroy
//-----------------------------------------------------------------------------
VOID GUIHttpPic::Destroy()
{
	if( m_pPic )
		m_pRender->DestroyImage(m_pPic);	// 删除图片
	m_pRender->DestroyFont(m_pFont);	// 删除字体

	m_pHttp->Terminate();

	GUIWnd::Destroy();
}


//-----------------------------------------------------------------------------
// render
//-----------------------------------------------------------------------------
VOID GUIHttpPic::Render()
{
	if( !IsRefresh() )
		return GUIWnd::Render();

	// 渲染底图
	if( m_pPic )
	{
		m_pCanvas->rc.SetLTWH(m_ptView, m_ptSize);
		m_pRender->Draw(m_pCanvas, m_pPic, m_dwPicColor, m_eAlphaWrite);
	}

	// 渲染文字
	tstring str = m_strText;
	tstring sub(_T("\\n"));
	while(tstring::npos != str.find(sub))
		str.replace(str.find(sub), sub.size(), _T("\n"));

	m_pCanvas->rc = m_rcText + m_ptView;
	m_pRender->Text(m_pCanvas, str, m_pFont, m_dwTextColor, m_dwShadeColor,
		m_eTextAlign, m_bTextWriteAlpha);
	
	// 调用基类渲染
	GUIWnd::Render();
}


//-----------------------------------------------------------------------------
// update
//-----------------------------------------------------------------------------
VOID GUIHttpPic::Update()
{
	GUIStatic::Update();

	if( !m_pPic )
	{
		if( TRUE == m_pHttp->GetResult() )
		{
			tstring strDestFile = m_pHttp->GetDestFileName();
			m_pPic = m_pRender->CreateImage(strDestFile, m_rcPic);
			this->SetRefresh(TRUE);
		}
	}
}




}	// namespace vEngine {
