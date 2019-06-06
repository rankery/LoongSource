//-----------------------------------------------------------------------------
//!\file gui_static.cpp
//!\author Lyp
//!
//!\date 2004-12-08
//! last 2004-12-10
//!
//!\brief 界面系统static控件
//-----------------------------------------------------------------------------
#include "..\stdafx.h"
#include "gui_static.h"
#include "gui_event_define.h"


namespace vEngine {
//-----------------------------------------------------------------------------
// constructor / destructor
//-----------------------------------------------------------------------------
GUIStatic::GUIStatic()
{
	// 注册所有可编辑属性
	m_pVarMgr->Register(_T("Text"), &m_strText);
	m_pVarMgr->Register(_T("Font"), &m_strFont);
	m_pVarMgr->Register(_T("FontWidth"), &m_nFontWidth);
	m_pVarMgr->Register(_T("FontHeight"), &m_nFontHeight);
	m_pVarMgr->Register(_T("FontWeight"), &m_nFontWeight);
	m_pVarMgr->Register(_T("TextRect"), &m_rcText);
	m_pVarMgr->Register(_T("TextColor"), &m_dwTextColor);
	m_pVarMgr->Register(_T("ShadeColor"), &m_dwShadeColor);
	m_pVarMgr->Register(_T("NoAntiAliase"), &m_bNoAntiAliase, _T("no yes"));
	m_pVarMgr->Register(_T("TextWriteAlpha"), &m_bTextWriteAlpha, _T("no yes"));
	m_pVarMgr->Register(_T("TextAlign"), (INT*)&m_eTextAlign, _T("LeftTop CenterTop RightTop ")
		_T("LeftCenter Center RightCenter LeftBottom CenterBottom RightBottom"));

	m_pVarMgr->Register(_T("PicFileName"), &m_strPic);
	m_pVarMgr->Register(_T("PicRect"), &m_rcPic);
	m_pVarMgr->Register(_T("PicColor"), &m_dwPicColor);
	m_pVarMgr->Register(_T("AlphaWrite"), (INT*)&m_eAlphaWrite, _T("NoWrite Write Add"));

	m_pVarMgr->Register(_T("Tips"), &m_strTips);

	// 设置初始值
	m_dwPicColor = 0xffffffff;
	m_dwTextColor = 0xfffff2a4;
	m_dwShadeColor = 0xFF290404;
	m_eAlphaWrite = EGUIAW_NoWrite;
	m_bPointInWnd = FALSE;

	m_strFont = _T("小三楷体");
}

//-----------------------------------------------------------------------------
// init
//-----------------------------------------------------------------------------
BOOL GUIStatic::Init(GUISystem* pSystem, GUIWnd* pFather, XmlElement* pXmlElement)
{
	GUIWnd::Init(pSystem, pFather, pXmlElement);

	if( m_rcText.IsEmpty() )
		m_rcText = m_ptSize;

	// 创建字体
	m_pFont = m_pRender->CreateFont(m_strFont, m_nFontWidth, m_nFontHeight, m_nFontWeight, !m_bNoAntiAliase);
	if( !P_VALID(m_pFont) )	// 尝试使用父控件字体
		m_pFont = m_pRender->CloneFont(m_pFather->GetFont());

	// 读入图片
	m_pPic = m_pRender->CreateImage(m_strPic, m_rcPic);
	return TRUE;
}


//-----------------------------------------------------------------------------
// destroy
//-----------------------------------------------------------------------------
VOID GUIStatic::Destroy()
{
	m_pRender->DestroyImage(m_pPic);	// 删除图片
	m_pRender->DestroyFont(m_pFont);	// 删除字体

	GUIWnd::Destroy();
}


//-----------------------------------------------------------------------------
// render
//-----------------------------------------------------------------------------
VOID GUIStatic::Render()
{
	if( !IsRefresh() )
		return GUIWnd::Render();

	// 渲染底图
	m_pCanvas->rc.SetLTWH(m_ptView, m_ptSize);
	m_pRender->Draw(m_pCanvas, m_pPic, m_dwPicColor, m_eAlphaWrite);

	// 渲染文字
	tstring str = m_strText;
	tstring sub(_T("\\n"));
	while(tstring::npos != str.find(sub))
		str.replace(str.find(sub), sub.size(), _T("\n"));

	m_pCanvas->rc = m_rcText + m_ptView;
	m_pRender->Text(m_pCanvas, str, m_pFont, m_dwTextColor, m_dwShadeColor, m_eTextAlign, m_bTextWriteAlpha);
	
	// 调用基类渲染
	GUIWnd::Render();
}


//-----------------------------------------------------------------------------
// 移动控件,并可改变控件大小
//-----------------------------------------------------------------------------
BOOL GUIStatic::Resize(tagPoint& ptSize)
{
	tagPoint ptOriginSize = m_ptSize;
	BOOL bResult = GUIWnd::Resize(ptSize);

	// 大小改变后，同时修改文字区域
	if( !(ptOriginSize == m_ptSize) )
	{
		m_rcText.right += (m_ptSize.x - ptOriginSize.x);
		m_rcText.bottom += (m_ptSize.y - ptOriginSize.y);
	}
	return bResult;
}


//-----------------------------------------------------------------------------
// 消息处理
//-----------------------------------------------------------------------------
BOOL GUIStatic::OnInputMessage( tagGUIInputMsg* pMsg )
{
	switch(pMsg->dwType)
	{
	case WM_MOUSEMOVE:	// 处理窗口拖动和改变大小
		{
			GUIWnd* pPointWnd = m_pSystem->GetMousePoint();
			if(  pPointWnd == this )
			{
				if(FALSE == m_bPointInWnd)
				{
					tagGUIEvent event(this, EGUIE_MoveIntoCtrl, pMsg);
					SendEvent(&event);
					m_bPointInWnd = TRUE;
				}
			}
			else 
			{
				if( TRUE == m_bPointInWnd )
				{
					if( !P_VALID(pPointWnd) || !m_pSystem->IsChildOf(this, pPointWnd) )
					{
						tagGUIEvent event(this, EGUIE_MoveOutOfCtrl, pMsg);
						SendEvent(&event);
						m_bPointInWnd = FALSE;
					}
				}
			}
		}
		break;
	}

	return GUIWnd::OnInputMessage(pMsg);
}


}	// namespace vEngine {
