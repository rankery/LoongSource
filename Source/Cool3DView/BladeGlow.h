#pragma once


// CBladeGlow 对话框

class CBladeGlow : public CDHtmlDialog
{
	DECLARE_DYNCREATE(CBladeGlow)

public:
	CBladeGlow(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CBladeGlow();
// 重写
	HRESULT OnButtonOK(IHTMLElement *pElement);
	HRESULT OnButtonCancel(IHTMLElement *pElement);

// 对话框数据
	enum { IDD = IDD_BladeGlow, IDH = IDR_HTML_BLADEGLOW };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
	DECLARE_DHTML_EVENT_MAP()
};
