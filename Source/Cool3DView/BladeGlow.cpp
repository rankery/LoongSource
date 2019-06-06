// BladeGlow.cpp : 实现文件
//

#include "stdafx.h"
#include "F3DView.h"
#include "BladeGlow.h"


// CBladeGlow 对话框

IMPLEMENT_DYNCREATE(CBladeGlow, CDHtmlDialog)

CBladeGlow::CBladeGlow(CWnd* pParent /*=NULL*/)
	: CDHtmlDialog(CBladeGlow::IDD, CBladeGlow::IDH, pParent)
{
}

CBladeGlow::~CBladeGlow()
{
}

void CBladeGlow::DoDataExchange(CDataExchange* pDX)
{
	CDHtmlDialog::DoDataExchange(pDX);
}

BOOL CBladeGlow::OnInitDialog()
{
	CDHtmlDialog::OnInitDialog();
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

BEGIN_MESSAGE_MAP(CBladeGlow, CDHtmlDialog)
END_MESSAGE_MAP()

BEGIN_DHTML_EVENT_MAP(CBladeGlow)
	DHTML_EVENT_ONCLICK(_T("ButtonOK"), OnButtonOK)
	DHTML_EVENT_ONCLICK(_T("ButtonCancel"), OnButtonCancel)
END_DHTML_EVENT_MAP()



// CBladeGlow 消息处理程序

HRESULT CBladeGlow::OnButtonOK(IHTMLElement* /*pElement*/)
{
	OnOK();
	return S_OK;  // 除非将焦点设置到控件，否则返回 TRUE
}

HRESULT CBladeGlow::OnButtonCancel(IHTMLElement* /*pElement*/)
{
	OnCancel();
	return S_OK;  // 除非将焦点设置到控件，否则返回 TRUE
}
