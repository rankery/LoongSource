#pragma once
#include "afxwin.h"

//--地表类型定义
enum SURFACE_TYPE
{
	SURFACETYPE_None		=0,			//无
	SURFACETYPE_Snow,					//雪地
	SURFACETYPE_Block,					//石块
	SURFACETYPE_Earth,					//土
	SURFACETYPE_Grass,					//草地
	SURFACETYPE_Sand,					//沙地
	SURFACETYPE_Ice,					//冰面
	SURFACETYPE_Metal,					//金属
	SURFACETYPE_WOOD,					//木头
	SURFACETYPE_NUM,
};

// CLayerAttDlg 对话框

class CLayerAttDlg : public CDialog
{
	DECLARE_DYNAMIC(CLayerAttDlg)

public:
	CLayerAttDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CLayerAttDlg();

// 对话框数据
	enum { IDD = IDD_LayerAtt };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

	afx_msg void OnBnClickedFiledlg();
	afx_msg void OnBnClickedOk();

public:
	bool	m_bMod;//是否是修改层属性
	CString m_szName;
	CString m_szTex;
	CString m_szUVSetIndex;
	int		m_typeIndex;
	virtual BOOL OnInitDialog();
	CComboBox m_typeList;
};
