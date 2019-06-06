//-----------------------------------------------------------------------------
//!\file gui_button.cpp
//!\author Lyp
//!
//!\date 2004-12-08
//! last 2004-12-10
//!
//!\brief 界面系统button控件
//-----------------------------------------------------------------------------
#include "..\stdafx.h"
#include "gui_static.h"
#include "gui_button.h"
#include "gui_event_define.h"

namespace vEngine {
//-----------------------------------------------------------------------------
// construction
//-----------------------------------------------------------------------------
GUIButton::GUIButton()
{
	m_pVarMgr->Register(_T("Disable"), &m_bDisable, _T("no yes"));
	m_pVarMgr->Register(_T("PushPicFileName"), &m_strPushPicFileName);
	m_pVarMgr->Register(_T("PushPicRect"), &m_rcPushPic);
	m_pVarMgr->Register(_T("PushPicColor"), &m_dwPushPicColor);
	m_pVarMgr->Register(_T("PushTextColor"), &m_dwPushTextColor);
	m_pVarMgr->Register(_T("PushShadeColor"), &m_dwPushShadeColor);

	m_pVarMgr->Register(_T("MovePicFileName"), &m_strMovePicFileName);
	m_pVarMgr->Register(_T("MovePicRect"), &m_rcMovePic);
	m_pVarMgr->Register(_T("MovePicColor"), &m_dwMovePicColor);
	m_pVarMgr->Register(_T("MoveTextColor"), &m_dwMoveTextColor);
	m_pVarMgr->Register(_T("MoveShadeColor"), &m_dwMoveShadeColor);

	m_pVarMgr->Register(_T("DisablePicFileName"), &m_strDisablePicFileName);
	m_pVarMgr->Register(_T("DisablePicRect"), &m_rcDisablePic);
	m_pVarMgr->Register(_T("DisablePicColor"), &m_dwDisablePicColor);
	m_pVarMgr->Register(_T("DisableTextColor"), &m_dwDisableTextColor);
	m_pVarMgr->Register(_T("DisableShadeColor"), &m_dwDisableShadeColor);
	m_pVarMgr->Register(_T("TopColor"), &m_dwTopColor);
	m_pVarMgr->Register(_T("BottomColor"), &m_dwBottomColor);

	// 设置初始值
	m_eState = EGUIBS_PopUp;
	m_bHaveMouseKeyDown = false;
	m_dwPicColor = 0xFFFFFFFF;
	m_dwPushPicColor = 0xFFFFFFFF;
	m_dwPushTextColor = 0xfffff2a4;
	m_dwPushShadeColor = 0xFF290404;
	m_dwMovePicColor = 0xFFFFFFFF;
	m_dwMoveTextColor = 0xfffff2a4;
	m_dwMoveShadeColor = 0xFF290404;
	m_dwDisablePicColor = 0xFFFFFFFF;
	m_dwDisableTextColor = 0xfffff2a4;
	m_dwDisableShadeColor = 0xFF290404;

	m_dwTopColor = 0xff000000;
	m_dwBottomColor = 0xFFAFAFAF;

}


//-----------------------------------------------------------------------------
// init
//-----------------------------------------------------------------------------
BOOL GUIButton::Init(GUISystem* pSystem, GUIWnd* pFather, XmlElement* pXmlElement)
{
	BOOL bResult = GUIStatic::Init(pSystem, pFather, pXmlElement);
	m_pPushPic = m_pRender->CreateImage(m_strPushPicFileName, m_rcPushPic);
	m_pMovePic = m_pRender->CreateImage(m_strMovePicFileName, m_rcMovePic);
	m_pDisablePic = m_pRender->CreateImage(m_strDisablePicFileName, m_rcDisablePic);
	return bResult;
}


//-----------------------------------------------------------------------------
// destroy
//-----------------------------------------------------------------------------
VOID GUIButton::Destroy()
{
	// 删除图片
	m_pRender->DestroyImage(m_pPushPic);
	m_pRender->DestroyImage(m_pMovePic);
	m_pRender->DestroyImage(m_pDisablePic);

	GUIStatic::Destroy();
}



//-----------------------------------------------------------------------------
// render
//-----------------------------------------------------------------------------
VOID GUIButton::Render()
{
	if( !IsRefresh() )
		return GUIWnd::Render();

	m_pCanvas->rc.SetLTWH(m_ptView, m_ptSize);
	if( m_bDisable )
		m_pRender->Draw(m_pCanvas, m_pDisablePic, m_dwDisablePicColor, m_eAlphaWrite);
	else
	{
		switch(m_eState)
		{
		case EGUIBS_PushDown:
			m_pRender->Draw(m_pCanvas, m_pPushPic, m_dwPushPicColor, m_eAlphaWrite);
			break;
		case EGUIBS_HighLight:
			m_pRender->Draw(m_pCanvas, m_pMovePic, m_dwMovePicColor, m_eAlphaWrite);
			break;
		default:
			m_pRender->Draw(m_pCanvas, m_pPic, m_dwPicColor, m_eAlphaWrite);
			break;
		}
	}

	if( !m_pPic )	// 如果没有图片则自己渲染
	{
		if( EGUIBS_PushDown == m_eState )
		{
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
		}
		else
		{
			// 画线
			m_pCanvas->rc.SetLTWH(m_ptView, m_ptSize);
			m_pCanvas->rc.bottom = m_pCanvas->rc.top;
			m_pRender->Line(m_pCanvas, m_dwBottomColor);

			m_pCanvas->rc.SetLTWH(m_ptView, m_ptSize);
			m_pCanvas->rc.right = m_pCanvas->rc.left;
			m_pRender->Line(m_pCanvas, m_dwBottomColor);

			m_pCanvas->rc.SetLTWH(m_ptView, m_ptSize);
			m_pCanvas->rc.right--;
			m_pCanvas->rc.left = m_pCanvas->rc.right ;
			m_pRender->Line(m_pCanvas, m_dwTopColor);

			m_pCanvas->rc.SetLTWH(m_ptView, m_ptSize);
			m_pCanvas->rc.bottom--;
			m_pCanvas->rc.top = m_pCanvas->rc.bottom;
			m_pRender->Line(m_pCanvas, m_dwTopColor);
		}
	}

	// 渲染文字
	if( !m_strText.empty() )
	{
		tstring str = m_strText;
		tstring sub(_T("\\n"));
		while(tstring::npos != str.find(sub))
			str.replace(str.find(sub), sub.size(), _T("\n"));

		m_pCanvas->rc = m_rcText + m_ptView;

		if( m_bDisable )
			m_pRender->Text(m_pCanvas, str, m_pFont, m_dwDisableTextColor, m_dwDisableShadeColor, m_eTextAlign, m_bTextWriteAlpha);
		else
		{
			switch(m_eState)
			{
			case EGUIBS_PushDown:
				m_pCanvas->rc = m_pCanvas->rc + 1;
				m_pRender->Text(m_pCanvas, str, m_pFont, m_dwPushTextColor, m_dwPushShadeColor, m_eTextAlign, m_bTextWriteAlpha);
				break;
			case EGUIBS_HighLight:
				m_pRender->Text(m_pCanvas, str, m_pFont, m_dwMoveTextColor, m_dwMoveShadeColor, m_eTextAlign, m_bTextWriteAlpha);
				break;
			default:
				m_pRender->Text(m_pCanvas, str, m_pFont, m_dwTextColor, m_dwShadeColor, m_eTextAlign, m_bTextWriteAlpha);
				break;
			}
		}
	}

	// 调用基类渲染
	GUIWnd::Render();
}



//-----------------------------------------------------------------------------
// 基本消息处理
//-----------------------------------------------------------------------------
BOOL GUIButton::OnInputMessage(tagGUIInputMsg* pMsg)
{
	if( m_bDisable )
	{
		if( pMsg->dwType == WM_LBUTTONDOWN || pMsg->dwType == WM_LBUTTONDBLCLK )
		{
			if( m_pSystem->GetMousePoint() == this ) // if( PointInWnd(pMsg->pt) )
			{
				m_pRender->PlaySound(_T("button_disable"), 0.7f);
			}
		}
		return GUIStatic::OnInputMessage(pMsg);
	}

	switch( pMsg->dwType )
	{
	case WM_LBUTTONDOWN:
	case WM_LBUTTONDBLCLK:
		if( m_pSystem->GetMousePoint() == this ) // if( PointInWnd(pMsg->pt) )
		{
			m_eState = EGUIBS_PushDown;
			m_bHaveMouseKeyDown = true;
			SetRefresh();
			m_pRender->PlaySound(_T("button_down"), 0.7f);
		}
		break;
	
	case WM_MOUSEMOVE:
		if( m_pSystem->GetMousePoint() == this )
		{ 
			if( (MK_LBUTTON & pMsg->dwParam1) && m_bHaveMouseKeyDown )
				m_eState = EGUIBS_PushDown;
			else
				m_eState = EGUIBS_HighLight;
		}
		else
		{
			m_eState = EGUIBS_PopUp;	// 鼠标按下后离开，按钮需要松开
		}
		SetRefresh();
		break;

	case WM_LBUTTONUP:
		if( m_bHaveMouseKeyDown && PointInWnd(pMsg->pt) )
		{
			tagGUIEvent event(this, EGUIE_Click, pMsg);
			SendEvent(&event);
			m_eState = EGUIBS_HighLight;
		}
		else
			m_eState = EGUIBS_PopUp;
		m_bHaveMouseKeyDown = false;
		SetRefresh();
		break;
	}

	return GUIStatic::OnInputMessage(pMsg);
}

}	// namespace vEngine {


