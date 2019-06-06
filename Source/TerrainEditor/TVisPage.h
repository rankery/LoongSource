#pragma once
#include "afxwin.h"
#include "ColorButton.h"


// TVisPage 对话框

class TVisPage : public CPropertyPage
{
	DECLARE_DYNAMIC(TVisPage)

public:
	TVisPage();
	virtual ~TVisPage();

// 对话框数据
	enum { IDD = IDD_PROPPAGE_VIS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CColorButton m_diffuse;
	CColorButton m_ambient;
	CColorButton m_specular;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedDiffuse();
	afx_msg void OnBnClickedAmbient();
	afx_msg void OnBnClickedSpecular();
	UINT m_terrainPow;
	afx_msg void OnEnChangeTerrainpow();
};
