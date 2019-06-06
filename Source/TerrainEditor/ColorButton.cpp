// ColorButton.cpp : 实现文件
//

#include "stdafx.h"
#include "ColorButton.h"


// CColorButton

IMPLEMENT_DYNAMIC(CColorButton, CButton)
CColorButton::CColorButton()
{
	m_color=0;
}

CColorButton::~CColorButton()
{
}


BEGIN_MESSAGE_MAP(CColorButton, CButton)
	ON_WM_PAINT()
END_MESSAGE_MAP()



void CColorButton::SetColor(COLORREF color)
{
	m_color=color;
	Invalidate();
}

// CColorButton 消息处理程序

void CColorButton::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CButton::OnPaint()
	CRect rc;
	GetClientRect(rc);
	dc.FillSolidRect(rc,m_color);
}

COLORREF CColorButton::GetColor()
{
	return m_color;
}