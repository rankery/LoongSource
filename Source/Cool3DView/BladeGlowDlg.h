#pragma once
#include "afxwin.h"
#include "Cool3DModels.h"


// CBladeGlowDlg 对话框

class CBladeGlowDlg : public CDHtmlDialog
{
	DECLARE_DYNCREATE(CBladeGlowDlg)

public:
	CBladeGlowDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CBladeGlowDlg();
// 重写
	HRESULT OnButtonOK(IHTMLElement *pElement);
	HRESULT OnButtonCancel(IHTMLElement *pElement);

// 对话框数据
	enum { IDD = IDD_BladeGlow, IDH = IDR_HTML_BLADEGLOWDLG };

	bool Init();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	afx_msg void OnClose( );

	bool GetTagNodes(vector<tagTagNode>& tagNodes);
	bool GetTagNodeFrame(TagNodeFrame& tagNodes);
	bool FindTagNode(DWORD name,tagTagNode& tagNode);
	bool UpdateTagNode(DWORD name,float x, float y, float z, float radius);
	bool SaveTagNodes();

	void AttachBladeGlow();
	void DetachBladeGlow();

	DECLARE_MESSAGE_MAP()
	DECLARE_DHTML_EVENT_MAP()
public:
	afx_msg void OnBnClickedRefresh();
	afx_msg void OnBnClickedSave();
	CListBox m_lstTagNodes;
	afx_msg void OnLbnSelchangeList();
	float m_fX;
	float m_fY;
	float m_fZ;
	float m_fRadius;
};
